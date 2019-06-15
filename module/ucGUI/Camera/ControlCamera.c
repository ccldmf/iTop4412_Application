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
#include "../Core/GUI.h"

// Camera status
enum CameraControlState theCameraContrlState;

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
                GetPicture( ptr , CAMERA_SHOW_IMAGE );
                break;

            case CAMERA_CLOSE_IMAGE:
                GUI_Clear();
                GUI_Exec1();
                break;

            case CAMERA_TAKE_PHOTO:
                TakePhoto( NULL );
                theCameraContrlState = CAMERA_SHOW_IMAGE;
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
    theCameraContrlState = CAMERA_IDLE;
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

