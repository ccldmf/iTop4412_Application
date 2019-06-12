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

// 摄像头每帧图片颜色
enum FrameColor
    {
    COLOR_GRAY,         // 灰度
    COLOR_RGB,          // 真彩
    COLOR_COLOR,        // 伪彩
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
int GetPicture(char * , enum FrameColor color);

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

