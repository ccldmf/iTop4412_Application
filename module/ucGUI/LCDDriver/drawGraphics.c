/**
 * @brief
 * @file     drawGraphics.c
 * @author   cc
 * @version
 * @date     Sat 30 Mar 2019 06:30:20 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include "lcd_framebuffer.h"

/**
 *@brief LCD画点
 *@param x:横坐标 y:纵坐标 color:颜色
 *@return 成功：0 失败：-1
 */
int LcdDrawPoint(unsigned int x,unsigned int y,unsigned int color)
    {
    int theWidth,theHeight;
    int theTmp = GetLcdScreenWidthAndHeight();
    theWidth   = (theTmp >> 16) & 0xFFFF;
    theHeight  = theTmp & 0x0000FFFF;
    if(x > theWidth || y > theHeight)
        {
        printf("LcdDrawPoint param error\n");
        return -1;
        }
    unsigned char *lcd_buf = GetLcdMmapBuf();
    *((int*)lcd_buf + y*theWidth + x) = color;
    return 0;
    }

/**
 *@brief LCD画线
 *@param x1 y1：起点坐标 x2 y2:终点坐标 color:线条颜色
 */
void LcdDrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color)
    {
    unsigned int t;
    int xerr = 0,yerr = 0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)
        {
        incx = 1;
        }
    else if(delta_x == 0)
        {
        incx = 0;
        }
    else
        {
        incx = -1;
        delta_x = -delta_x;
        }
    if(delta_y > 0)
        {
        incy = 1;
        }
    else if(delta_y == 0)
        {
        incy = 0;
        }
    else
        {
        incy = -1;
        delta_y = -delta_y;
        }

    if(delta_x > delta_y)
        {
        distance = delta_x;
        }
    else
        {
        distance = delta_y;
        }

    for(t = 0;t <= distance+1;t++)
        {
        LcdDrawPoint(uRow,uCol,color);
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
            {
            xerr -= distance;
            uRow += incx;
            }
        if(yerr > distance)
            {
            yerr -= distance;
            uCol += incy;
            }
        }
    }


