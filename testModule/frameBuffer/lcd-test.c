/**
 * @brief
 * @file     lcd-test.c
 * @author   cc
 * @version
 * @date     Fri 09 Mar 2018 10:35:33 PM PST
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include "lcd_framebuffer.h"

int main(int argc,char **argv)
    {
    int ret,len;
    int i,j;
    unsigned char *lcd_buf;
    //初始化
    ret = LcdInit();
    if(ret < 0)
        {
        printf("LcdInit error\n");
        LcdClose();
        return -1;
        }
    //打印LCD相关信息
    LcdFixScreenInfo();
    LcdVarScreenInfo();
    //映射到内存
    ret = LcdMmap();
    if(ret < 0)
        {
        printf("LcdMmap error\n");
        LcdClose();
        return -1;
        }
    len = GetLcdScreenSize();
    if(len < 0)
        {
        printf("GetLcdScreenSize error\n");
        LcdClose();
        return -1;
        }
    printf("len = %d\n",len);
    lcd_buf = GetLcdMmapBuf();
    if(lcd_buf == NULL)
        {
        printf("GetLcdMmapAddr error\n");
        LcdClose();
        return -1;
        }
    printf("LcdMmapAddr = %p\n",lcd_buf);

    memset(lcd_buf,0,len);

    LcdDrawLine(0,0,480,272,0xFF);          //蓝色
    LcdDrawLine(0,272,480,0,0xFF00);        //绿色
    LcdDrawLine(0,100,480,100,0xFF0000);    //红色

    //关闭
    LcdClose();

    return 0;
    }
