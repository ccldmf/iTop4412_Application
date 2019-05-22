/**
 * @brief
 * @file     touchScreen.h
 * @author   cc
 * @version
 * @date     Fri 10 May 2019 08:13:18 PM PDT
 * @note
 */

#ifndef TOCUHSCREEN_H
#define TOUCHSCREEN_H

#define TOUCH_SCREEN_DEV_NAME   "/dev/input/event2"

/**
 *@brief TouchScreen初始化
 *@return 成功：0 失败：-1
 */
int TouchScreenInit( void );

/**
 *@brief TouchScreen基本信息
 *@return 无
 */
void TouchScreenInfo( void );

/**
 *@brief 获得TouchScreen坐标
 *@return 成功：0   失败：-1
 */
int TouchScreenRead( int *x , int *y );

/**
 *@brief 关闭TouchScreen
 *@return 无
 */
void TouchScreenClose( void );

#endif

