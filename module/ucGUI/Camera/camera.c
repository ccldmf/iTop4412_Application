/*
 * @brief
 * @file     camera.c
 * @author   cc
 * @version
 * @date     Mon 15 Jan 2018 21:44:25 CST
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
#include <time.h>
#include "camera.h"
#include "../Config/LCDConf.h"
#include "../Config/GUIConf.h"
#include "../Core/GUI.h"


// Save image data buffer queue number
#define CAMRERA_BUFFER_NUM      2

int g_open_camera_fd = -1;
static int g_frame_size = 0;

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct FrameBuffer
    {
    void *frameStart;
    size_t frameLength;
    };
struct FrameBuffer *g_frame_buffer;

static int xioctl(int fd, int request, void * arg)
    {
    int r;
    do
    {
    r = ioctl(fd, request, arg);
    }while (-1 == r && EINTR == errno);
    return r;
    }

/**
 *@brief 打开设备
 *@return 0：成功  -1：失败
 */
static int openDevice(const char *devName)
    {
    struct stat st;
    if( -1 == stat( devName , &st ) )
        {
        printf( "Cannot identify '%s'\n" , devName );
        return -1;
        }

    if ( !S_ISCHR( st.st_mode ) )
        {
        printf( "%s is no device\n" , devName );
        return -1;
        }

    g_open_camera_fd = open( devName , O_RDWR | O_NONBLOCK , 0 );
    if ( -1 == g_open_camera_fd )
        {
        printf( "Cannot open '%s'\n" , devName );
        return -1;
        }
    return 0;
    }

/**
 *@brief 设备初始化
 *@return 0：成功  -1：失败
 */
static int initDevice( const int width , const int height )
    {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int min;

    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_QUERYCAP , &cap ) )
        {
        printf( "VIDIOC_QUERYCAP error:LINE:%d\n" , __LINE__ );
        return -1;
        }

    if ( !( cap.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
        {
        printf( "This device is no video capture device\n" );
        return -1;
        }

    if ( !( cap.capabilities & V4L2_CAP_STREAMING ) )
        {
        printf( "This device does not support streaming i/o\n" );
        return -1;
        }

    struct v4l2_input input;
    memset( &input , 0 , sizeof( struct v4l2_input ) );
    input.index = 0;
    int rtn = ioctl( g_open_camera_fd , VIDIOC_S_INPUT , &input );
    if ( rtn < 0 )
        {
        printf( "VIDIOC_S_INPUT error;LINE:%d\n" , __LINE__ );
        return -1;
        }

    CLEAR ( fmt );
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width  = width;
    fmt.fmt.pix.height = height;

    /*-----------------------------------------------------
    Set format
    -----------------------------------------------------*/
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;

    if ( fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV )
        {
        printf("V4L2_PIX_FMT_YUYV\n");
        }
    else if ( fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420 )
        {
        printf("V4L2_PIX_FMT_YUV420\n");
        }
    else if ( fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_NV12 )
        {
        printf("V4L2_PIX_FMT_NV12\n");
        }

    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_S_FMT , &fmt ) )
        {
        printf( "VIDIOC_S_FMT error:LINE:%d\n" , __LINE__ );
        return -1;
        }

    fmt.type = V4L2_BUF_TYPE_PRIVATE;
    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_S_FMT , &fmt ) )
        {
        printf( "VIDIOC_S_FMT error;LINE:%d\n" , __LINE__ );
        return -1;
        }
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_G_FMT , &fmt ) )
        {
        printf( "VIDIOC_G_FMT error;LINE:%d\n" , __LINE__ );
        return -1;
        }

    min = fmt.fmt.pix.width * 2;
    if ( fmt.fmt.pix.bytesperline < min )
        fmt.fmt.pix.bytesperline = min;
    min = ( unsigned int )width * height * 3 / 2;
    if ( fmt.fmt.pix.sizeimage < min )
        fmt.fmt.pix.sizeimage = min;

    g_frame_size = fmt.fmt.pix.sizeimage;

    return 0;
    }

/**
 *@brief  内存空间初始化
 *@return 0：成功  -1：失败
 */
static int initMap()
    {
    int i;
    char *ptr ;
    struct v4l2_requestbuffers req;
    CLEAR (req);
    req.count = CAMRERA_BUFFER_NUM;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_REQBUFS , &req ) )
        {
        printf( "initMap:Device does not support memory mapping\n" );
        return -1;
        }

    if (req.count < 2)
        {
        printf( "Insufficient buffer memory\n" );
        return -1;
        }

    g_frame_buffer = ( struct FrameBuffer * ) calloc( req.count , sizeof( struct FrameBuffer ) );
    if ( !g_frame_buffer )
    {
    printf( "initMap:calloc memory error\n" );
    return -1;
    }

    for ( i = 0; i < req.count; ++i )
        {
        struct v4l2_buffer buf;
        CLEAR ( buf );
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if ( -1 == xioctl( g_open_camera_fd , VIDIOC_QUERYBUF , &buf ) )
            {
            printf( "VIDIOC_QUERYCAP error;LINE:%d\n" , __LINE__ );
            }
        g_frame_buffer[ i ].frameLength = buf.length;
        g_frame_buffer[ i ].frameStart = mmap( NULL , buf.length ,
                                        PROT_READ | PROT_WRITE ,
                                        MAP_SHARED , g_open_camera_fd , buf.m.offset );
        if ( MAP_FAILED == g_frame_buffer[ i ].frameStart )
            return -1;
        }
    return 0;
    }

/**
 *@brief 释放申请的空间
 *@return 无
 */
static void freeMap()
    {
    unsigned int i;
    for ( i = 0; i < CAMRERA_BUFFER_NUM; ++i )
        {
        if ( -1 == munmap( g_frame_buffer[ i ].frameStart, g_frame_buffer[ i ].frameLength ) )
            {
            printf( "munmap error;LINE:%d\n",__LINE__ );
            }
        }
    }

/**
 *@brief 摄像头初始化
 *@param devName:设备名 w:宽度  h:高度
 *@return 0：成功  -1：失败
 */
int CameraInit( const char *devName ,int w ,int h )
    {
    int ret;
    ret = openDevice( devName );
    if ( ret < 0 )
        {
        printf( "openDevice error\n" );
        return ret;
        }

    ret = initDevice( w,h );
    if ( ret < 0 )
        {
        printf( "initDevice error\n " );
        return -1;
        }

    ret = initMap();
    if ( ret < 0 )
        {
        printf( "initMap error\n" );
        return -1;
        }

    ret = StartCapturing();
    if ( ret < 0 )
        {
        printf( "StartCapturing error\n");
        return -1;
        }

    return 1;
    }

/**
 *@brief 获得图像大小
 *@return  图像大小
 */
int GetPictureSize()
    {
    return g_frame_size;
    }

/**
 *@brief 将抓取到的一帧数据设置为指定颜色
 *@return  无
 */
static void ChangeFrameColor( char *des , char *src )
    {
    char *des_ptr  = ( char *)des + ( LCD_XSIZE * 4 );
    char *src_ptr = ( short *)src;
    int i , j ;
    int theRect_X      = ( DISPLAY_VIDEO_OFFSET / 4 );
    int theRect_Y      = LCD_YSIZE;
    int theRect_Widht  = ( DISPLAY_VIDEO_OFFSET + LCD_XSIZE ) / 4;
    int theRext_Height = LCD_YSIZE;

    // Draw a rect
    GUI_SetColor(GUI_RED);
    GUI_DrawHLine( 0 , theRect_X , theRect_Widht ) ;
    GUI_DrawHLine( ( theRect_Y - 1 ) , theRect_X , theRect_Widht );

    GUI_DrawVLine( ( theRect_X - 1 ) , 0 , theRect_Y );
    GUI_DrawVLine( ( theRect_X + LCD_XSIZE / 4 + 1 ) , 0 , theRect_Y );

    for ( i = 0 ; i < ( LCD_YSIZE - 2 ) ; i++ )
        {
        for ( j = 0 ; j < LCD_XSIZE ; j++ )
            {
            *( des_ptr + i * LCD_XSIZE * 4  + j + DISPLAY_VIDEO_OFFSET ) = ( * ( src_ptr + i * CAMERA_DEVICE_WIDTH  + j ) ) ;
            }
        }
    }

/**
 *@brief 读取一帧图像
 *@return  0：成功     -1：失败
 */
static int read_frame( char *image , enum CameraControlState status )
    {
    struct v4l2_buffer buf;

    CLEAR ( buf );
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_DQBUF , &buf ) )
        {
        printf("VIDIOC_DQBUF error;LINE:%d\n",__LINE__);
        return -1;
        }
    assert( buf.index < CAMRERA_BUFFER_NUM );

    if ( CAMERA_SHOW_IMAGE == status )
        {
        ChangeFrameColor( image , g_frame_buffer[ 0 ].frameStart );
        }
    else if ( CAMERA_TAKE_PHOTO == status || CAMERA_RECORD_VIDEO == status )
        {
        memcpy( image , g_frame_buffer[ 0 ].frameStart , g_frame_size );
        }

    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_QBUF , &buf ) )
        {
        printf( "VIDIOC_QBUF error;LINE:%d\n" , __LINE__ );
        return -1;
        }
    return 0;
    }

/**
 *@brief 获得一帧图像
 *@return  0：成功     -1：失败
 */
int GetPicture( char *image , enum CameraControlState status )
    {
    int ret;
    fd_set fds;
    struct timeval tv;
    int r;
    FD_ZERO( &fds );
    FD_SET( g_open_camera_fd , &fds );
    /* Timeout. */
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    r = select( g_open_camera_fd + 1 , &fds , NULL , NULL , &tv );
    if ( -1 == r || 0 == r )
        {
        printf("getimage error;LINE:%d\n",__LINE__);
        return -1;
        }

    ret = read_frame( image , status );
    return ret;
    }

static void YUV2RGB( char *Psrc , char *Pdes )
    {
    char* pRGB = Pdes;
    char* pYUV = Psrc;
    char* pY   = pYUV;
    char* pU   = pY + CAMERA_DEVICE_WIDTH * CAMERA_DEVICE_HEIGHT ;
    char* pV   = pU + CAMERA_DEVICE_WIDTH * CAMERA_DEVICE_HEIGHT / 4;
    int bgr[ 4 ];
    int i,j,k;
    int yIdx,uIdex,vIdex;
    int idx;
    for( i = 0 ; i < CAMERA_DEVICE_HEIGHT ; i++ )
        {
        for( j = 0 ; j < CAMERA_DEVICE_WIDTH ; j++ )
            {
            yIdx  = i * CAMERA_DEVICE_WIDTH + j;
            vIdex = i * CAMERA_DEVICE_WIDTH / 4 + j;
            uIdex = vIdex;
            bgr[ 0 ] = ( int )( pY[ yIdx ] + 1.732 * ( pU[ vIdex ] - 128 ) );                                   // b
            bgr[ 1 ] = ( int )( pY[ yIdx ] - 0.698 * ( pU[ uIdex ] - 128 ) - 0.703 * ( pV[ vIdex ] - 128 ) );   // g
            bgr[ 2 ] = ( int )( pY[ yIdx ] + 0.370 * ( pV[ uIdex ] - 128 ) );                                   // r
             for( k = 0 ; k < 4 ; k++ )
                {
                idx = ( i * CAMERA_DEVICE_WIDTH + j ) * 4 + k;
                if( bgr[ k ] >= 0 && bgr[ k ] <= 255 )
                    pRGB[ idx ] = bgr[ k ];
                else
                    pRGB[ idx ] = ( bgr[ k ] < 0 ) ? 0 : 255;
                }
            }
        }
    }

/**
 *@brief 拍照
 *@param name:照片名，如果为NULL,则使用系统默认值
 *@return  0：成功     -1：失败
 */
int TakePhoto( char *name )
    {
    char *thePictureName;
    char *thePictureData;
    char *theRGBData;
    FILE *thePicture;

    if ( NULL != name )
        {
        thePictureName = name;
        }
    else
        {
        time_t t;
        struct tm *theTime = NULL;
        thePictureName = ( char * )malloc( 50 );
        memset( thePictureName , 0 , 50 );
        memcpy( thePictureName , SAVE_CAMERA_PICTURE_PATH , strlen( SAVE_CAMERA_PICTURE_PATH ) );
        time( &t );
        theTime = localtime( &t );
        strftime( thePictureName + strlen( SAVE_CAMERA_PICTURE_PATH ) , 20 , "%Y_%m_%d_%H_%M_%S" , theTime );
        }
    printf( "Save Picture File Name : %s\n", thePictureName );
    thePicture     = fopen( thePictureName , "w+" );

    thePictureData = ( char * )malloc( g_frame_size );
    memset( thePictureData , 0 , g_frame_size );

    GetPicture( thePictureData , CAMERA_TAKE_PHOTO );

    theRGBData = ( char * )malloc( CAMERA_DEVICE_WIDTH * CAMERA_DEVICE_HEIGHT * 4 );
    memset( theRGBData , 0 , CAMERA_DEVICE_WIDTH * CAMERA_DEVICE_HEIGHT * 4 );

    YUV2RGB( thePictureData , theRGBData );

    fwrite( theRGBData , 1 , CAMERA_DEVICE_WIDTH * CAMERA_DEVICE_HEIGHT * 4 , thePicture );
    fclose( thePicture );
    free( thePictureData );
    free( thePictureName );
    free( theRGBData );
    return 0;
    }

/**
 *@brief 录制视频
 *@param name:视频名，如果为NULL,则使用系统默认值
 *@param seconds:录制视频长度
 *@return  0：成功     -1：失败
 */
int RecordVideo( char *name , int seconds )
    {
    char *theVideoName;
    FILE *theVideo;
    char *theVideoData;
    time_t t;
    struct tm *theTime = NULL;
    int theCurrentTime;

    if ( NULL != name )
        {
        theVideoName = name;
        }
    else
        {
        theVideoName = ( char * )malloc( 50 );
        memset( theVideoName , 0 , 50 );
        memcpy( theVideoName , SAVE_CAMERA_VIDEO_PATH , strlen( SAVE_CAMERA_VIDEO_PATH ) );
        time( &t );
        theTime = localtime( &t );
        strftime( theVideoName + strlen( SAVE_CAMERA_VIDEO_PATH ) , 20 , "%Y_%m_%d_%H_%M_%S" , theTime );
        }
    printf( "Save Video File Name : %s\n", theVideoName );
    theVideo     = fopen( theVideoName , "w+" );

    theVideoData = ( char * )malloc( g_frame_size );
    memset( theVideoData , 0 , g_frame_size );

    time( &t );
    theCurrentTime = t;

    while( (theCurrentTime + seconds ) >= t )
        {
        GetPicture( theVideoData , CAMERA_RECORD_VIDEO );
        fwrite( theVideoData , 1 , g_frame_size , theVideo );
        t = time( &t );
        }

    fclose( theVideo );
    free( theVideoData );
    free( theVideoName );
    return 0;
    }

/**
 *@brief 开始图像传输
 *@return  0：成功     -1：失败
 */
int StartCapturing()
    {
    unsigned int i;
    enum v4l2_buf_type type;
    for ( i = 0; i < CAMRERA_BUFFER_NUM; ++i )
        {
        struct v4l2_buffer buf;
        CLEAR ( buf );
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        if ( -1 == xioctl( g_open_camera_fd , VIDIOC_QBUF , &buf ) )
            {
            printf( "VIDIOC_QBUF error:LINE:%d\n" , __LINE__ );
            return -1;
            }
        }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_STREAMON , &type ) )
        {
        printf( "VIDIOC_STREAMON error:LINE:%d\n" , __LINE__ );
        return -1;
        }
    return 0;
    }

/**
 *@brief 停止图像传输
 *@return  0：成功     -1：失败
 */
void StopCapturing()
    {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ( -1 == xioctl( g_open_camera_fd , VIDIOC_STREAMOFF , &type ) )
        {
        printf( "VIDIOC_STREAMOFF error;LINE:%d\n" , __LINE__ );
        }
    }

/**
 *@brief 关闭摄像头
 *@return  无
 */
void CameratClose()
    {
    freeMap();
    close( g_open_camera_fd );
    }

