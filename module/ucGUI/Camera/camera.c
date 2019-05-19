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
#include "camera.h"

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
 *@brief 读取一帧图像
 *@return  0：成功     -1：失败
 */
static int read_frame( char *image )
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
    memcpy( image , g_frame_buffer[ 0 ].frameStart , g_frame_size );

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
int GetPicture(char *image)
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

    ret = read_frame( image );
    return ret;
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

