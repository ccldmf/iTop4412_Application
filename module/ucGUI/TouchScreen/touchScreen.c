/**
 * @brief
 * @file     touchScreen.c
 * @author   cc
 * @version
 * @date     Fri 10 May 2019 08:13:34 PM PDT
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include "touchScreen.h"

/*-----------------------------------------------------
Touch screen trigger return value.
-----------------------------------------------------*/
typedef struct
    {
    int x;            // X coordinate
    int y;            // Y coordinate
    }TOUCH_SCREEN_INFO;

enum TOUCH_SCREEN_STATUS
    {
    TOUCH_X_VALUE,
    TOUCH_Y_VALUE,
    TOUCH_PRESS_DOWN,
    TOUCH_PRESS_UP,
    TOUCH_SYNC,
    TOUCH_ERROR
    };

// Open touch screen fd.
static int g_touch_screen_fd = -1;

// Save touch screen opration information.
static TOUCH_SCREEN_INFO *mTouchScreenInfo;

/**
 *@brief TouchScreen初始化
 *@return 成功：0 失败：-1
 */
int TouchScreenInit( void )
    {
    g_touch_screen_fd = open ( TOUCH_SCREEN_DEV_NAME , O_RDONLY );
    if ( g_touch_screen_fd < 0 )
        {
        printf ( "TouchScreenInit error\n" );
        return -1;
        }
    else
        {
        mTouchScreenInfo = ( TOUCH_SCREEN_INFO * )malloc ( sizeof ( TOUCH_SCREEN_INFO ) );
        if ( NULL == mTouchScreenInfo )
            {
            printf ( "TouchScreen malloc error\n" );
            return -1;
            }
        else
            {
            // Init Touch Screen Info
            mTouchScreenInfo->x = 0;
            mTouchScreenInfo->y = 0;
            return 0;
            }
        }
    }

/**
 *@brief TouchScreen基本信息
 *@return 无
 */
void TouchScreenInfo( void )
    {
    struct input_absinfo theAbsInfo;
    // Get the input abs info of X
    ioctl ( g_touch_screen_fd , EVIOCGABS( ABS_X ) , &theAbsInfo );
    printf ( "X ABS Min = %d\n" , theAbsInfo.minimum );
    printf ( "X ABS Max = %d\n" , theAbsInfo.maximum );

    // Get the input abs info of Y
    ioctl ( g_touch_screen_fd , EVIOCGABS( ABS_Y ) , &theAbsInfo );
    printf ( "Y ABS Min = %d\n" , theAbsInfo.minimum );
    printf ( "Y ABS Max = %d\n" , theAbsInfo.maximum );
    }

/**
 *@brief 读取坐标值
 *@return 0：x轴  1：y轴      2:按下    3:松开    -1：失败
 */
enum TOUCH_SCREEN_STATUS ReadTouchScreen( int *value )
    {
    int ret;
    fd_set theRds;
    struct input_event theEventInfo;

    /* Timeout. */
    FD_ZERO ( &theRds );
    FD_SET ( g_touch_screen_fd , &theRds );
    ret = select ( g_touch_screen_fd + 1 , &theRds , NULL , NULL , NULL );
    if ( ret >= 0 )
        {
        if ( FD_ISSET ( g_touch_screen_fd , &theRds ) )
            {
            ret = read ( g_touch_screen_fd , &theEventInfo , sizeof ( struct input_event ) );
            if ( ret > 0 )
                {
                /*-----------------------------------------------------
                Type     Code     Value             Desc
                3        0        xxx -> X value   Touch the X
                3        1        xxx -> Y value   Touch the Y
                1        330       1               Press down
                1        330       0               Press up
                -----------------------------------------------------*/
                if ( theEventInfo.type == 0x03 )
                    {
                    if ( theEventInfo.code == 0x00 )                // X value
                        {
                        *value = theEventInfo.value;
                        return TOUCH_X_VALUE;
                        }
                    else if ( theEventInfo.code == 0x01 )           // Y value
                        {
                        *value = theEventInfo.value;
                        return TOUCH_Y_VALUE;
                        }
                    else
                        {
                        return TOUCH_SYNC;
                        }
                    }
                else if ( theEventInfo.type == 0x01 && theEventInfo.code == 0x014A )
                    {
                    if ( theEventInfo.value == 1 )                  // Press Down
                        {
                        return TOUCH_PRESS_DOWN;
                        }
                    else if ( theEventInfo.value == 0x00 )          // Press Up
                        {
                        return TOUCH_PRESS_UP;
                        }
                    }
                }
            }
        }
    return TOUCH_ERROR;
    }

/**
 *@brief 获得TouchScreen坐标
 *@return 成功：0   失败：-1
 */
int TouchScreenRead( int *x , int *y )
    {
    int ret,i;
    int value;
    int returnValue = 0;
    char readCount = 0;
    while ( readCount < 2 )
        {
        ret = ReadTouchScreen( &value );
        if ( ret == TOUCH_X_VALUE )
            {
            readCount ++ ;
            mTouchScreenInfo->x = value;
            }
        else if ( ret == TOUCH_Y_VALUE )
            {
            readCount ++ ;
            mTouchScreenInfo->y = value;
            }
        else if ( ret == TOUCH_PRESS_DOWN || ret == TOUCH_SYNC)
            {
            }
        else if ( ret == TOUCH_PRESS_UP )
            {
            returnValue = -1;
            break;
            }
        }

    *x = mTouchScreenInfo->x;
    *y = mTouchScreenInfo->y;
    return returnValue;
    }

/**
 *@brief 关闭TouchScreen
 *@return 无
 */
void TouchScreenClose( void )
    {
    close ( g_touch_screen_fd );
    free( mTouchScreenInfo );
    }



