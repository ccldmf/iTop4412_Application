/**
 * @brief
 * @file     ControlCamera.c
 * @author   cc
 * @version
 * @date     Thu 30 May 2019 06:25:52 AM PDT
 * CST
 * @note
 */
#ifndef CONCTROLCAMERA_H
#define CONCTROLCAMERA_H

 /**
 *@brief 摄像头初始化
 *@return 0：成功  -1：失败
 */
int GUI_Camera_Init( void );

/**
 *@brief 显示摄像头画面
 *@return 0：成功  -1：失败
 */
int GUI_Show_Camera_Image( void );

/**
 *@brief 关闭摄像头画面
 *@return 0：成功  -1：失败
 */
int GUI_Close_Camera_Image( void );

/**
 *@brief 拍照
 *@return 0：成功  -1：失败
 */
int GUI_Take_Photo( void );

/**
 *@brief 录视频
 *@param seconds:录视频的时间长度(s)
 *@return 0：成功  -1：失败
 */
int GUI_Record_Video( int seconds );

#endif // CONCTROLCAMERA_H

