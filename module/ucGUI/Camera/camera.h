/**
 * @brief
 * @file     camera.h
 * @author   cc
 * @version
 * @date     Mon 15 Jan 2018 21:44:50 CST
 * CST
 * @note
 */

#ifndef CAMERA_H
#define CAMERA_H

// 摄像头控制状态
enum CameraControlState
    {
    CAMERA_IDLE,            // Idle
    CAMERA_SHOW_IMAGE,      // Show image
    CAMERA_CLOSE_IMAGE,     // Close image
    CAMERA_TAKE_PHOTO,      // Take photo
    CAMERA_RECORD_VIDEO     // Record video
    };

/**
 *@brief 摄像头初始化
 *@param devName:设备名 w:宽度  h:高度
 *@return 0：成功  -1：失败
 */
int CameraInit(const char *devName,int w,int h);

/**
 *@brief 获得图像大小
 *@return  图像大小
 */
int GetPictureSize();

/**
 *@brief 获得一帧图像
 *@return  0：成功     -1：失败
 */
int GetPicture( char * , enum CameraControlState status );

/**
 *@brief 拍照
 *@param name:照片名，如果为NULL,则使用系统默认值
 *@return  0：成功     -1：失败
 */
int TakePhoto( char *name );

/**
 *@brief 录制视频
 *@param name:视频名，如果为NULL,则使用系统默认值
 *@param seconds:录制视频长度
 *@return  0：成功     -1：失败
 */
int RecordVideo( char *name , int seconds );

/**
 *@brief 开始图像传输
 *@return  0：成功     -1：失败
 */
int StartCapturing();

/**
 *@brief 停止图像传输
 *@return  0：成功     -1：失败
 */
void StopCapturing();

/**
 *@brief 关闭摄像头
 *@return  无
 */
void CameratClose();

#endif // CAMERA_H

