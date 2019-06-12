/**
 * @brief
 * @file     ControlCamera.c
 * @author   cc
 * @version
 * @date     Thu 30 May 2019 06:25:52 AM PDT
 * CST
 * @note
 */

#include "../Config/GUIConf.h"
#if GUI_SUPPORT_CAMERA
#include <pthread.h>
#include <stdio.h>
#include "camera.h"
#include "ControlCamera.h"

// 摄像头控制状态
enum CameraControlState
    {
    CAMERA_IDLE,
    CAMERA_SHOW_IMAGE,
    CAMERA_CLOSE_IMAGE,
    CAMERA_TAKE_PHOTO,
    CAMERA_RECORD_VIDEO
    }theCameraContrlState;

/*
void fb_drawbmp(int width,int height,unsigned char* pdata)
    {
    char* pRGB = GetLcdMmapBuf();
    char* pYUV = pdata;
    char* pY   = pYUV;
    char* pU   = pY + 640*512;
    char* pV   = pU + 640*512/4;
    int bgr[4];

    int i,j,k;
    int yIdx,uIdex,vIdex;
    int idx;
    for(i = 0;i < height;i++)
        {
        for(j = 0;j < width;j++)
            {
            yIdx = i*640+j;
            vIdex = i*640/4 + j;
            uIdex = vIdex;
            bgr[0] = (int)(pY[yIdx] + 1.732 * (pU[vIdex] - 128));                                // b
            bgr[1] = (int)(pY[yIdx] - 0.698 * (pU[uIdex] - 128) - 0.703*(pV[vIdex] - 128));      // g
            bgr[2] = (int)(pY[yIdx] + 0.370 * (pV[uIdex] - 128));                                // r

            for(k = 0;k < 4;k++)
                {
                idx = (i*width + j)*4 + k;
                if(bgr[k] >= 0 && bgr[k] <= 255)
                    pRGB[idx] = bgr[k];
                else
                    pRGB[idx] = (bgr[k] < 0)?0:255;
                }
            }
        }
    }
*/

/**
 *@brief 摄像头线程，用于摄像头独立使用
 */
static void *CameraHandlePthread( void *arg )
    {
    char *ptr;
    while( 1 )
        {
        switch ( theCameraContrlState )
            {
            case CAMERA_SHOW_IMAGE:
                ptr = GetLcdMmapBuf();
                GetPicture( ptr , COLOR_GRAY);
                break;

            case CAMERA_CLOSE_IMAGE:
                break;

            case CAMERA_TAKE_PHOTO:
                break;

            case CAMERA_RECORD_VIDEO:
                break;

            case CAMERA_IDLE:
            default:
                break;

            }
        }
    }

/**
 *@brief 摄像头初始化
 *@return 0：成功  -1：失败
 */
int GUI_Camera_Init( void )
    {
    int ret;
    pthread_t theCameraPthread;

    // Create pthread for camera work
    pthread_create( &theCameraPthread , NULL , CameraHandlePthread , NULL );

    // Camera Init
    ret = CameraInit( CAMERA_DEVICE_NAME , CAMERA_DEVICE_WIDTH , CAMERA_DEVICE_HEIGHT );
    if ( ret < 0 )
        {
        printf( "GUI_Camera_Init error\n" );
        return -1;
        }

    // Set cameara state
    //theCameraContrlState = CAMERA_IDLE;
    theCameraContrlState = CAMERA_SHOW_IMAGE;
    return 0;
    }

/**
 *@brief 显示摄像头画面
 *@return 0：成功  -1：失败
 */
int GUI_Show_Camera_Image( void )
    {
    theCameraContrlState = CAMERA_SHOW_IMAGE;
    }

/**
 *@brief 关闭摄像头画面
 *@return 0：成功  -1：失败
 */
int GUI_Close_Camera_Image( void )
    {
    theCameraContrlState = CAMERA_CLOSE_IMAGE;
    }

/**
 *@brief 拍照
 *@return 0：成功  -1：失败
 */
int GUI_Take_Photo( void )
    {
    theCameraContrlState = CAMERA_TAKE_PHOTO;
    }

/**
 *@brief 录视频
 *@param seconds:录视频的时间长度(s)
 *@return 0：成功  -1：失败
 */
int GUI_Record_Video( int seconds )
    {
    theCameraContrlState = CAMERA_RECORD_VIDEO;
    }

#endif

