/**
 * @brief
 * @file     lcd_framebuffer.c
 * @author   cc
 * @version
 * @date     Sat 10 Mar 2018 12:06:09 AM PST
 * @mail     13568859409@163.com
 * @note
 */

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lcd_framebuffer.h"

//LCD文件描述符
int g_lcd_fd = -1;
//LCD映射操作的内存地址
unsigned char *g_lcd_mmap_buf = NULL;
//LCD屏字节总数
int g_lcd_screen_size = 0;

//LCD屏固定参数结构体
struct fb_fix_screeninfo g_fix_info;
//LCD屏可变参数结构体
struct fb_var_screeninfo g_var_info;

/**
 *@brief LCD初始化
 *@return 成功：0 失败：-1
 */
int LcdInit(void)
    {
    int ret;
    g_lcd_fd = open(LCDDEVNAME,O_RDWR);
    if(g_lcd_fd < 0)
        {
        printf("oepn %s error\n",LCDDEVNAME);
        return -1;
        }
    ret = ioctl(g_lcd_fd,FBIOGET_FSCREENINFO,&g_fix_info);
    if(ret < 0)
        {
        printf("FBIOGET_FSCREENINFO ioctl error\n");
        return -1;
        }
    ret = ioctl(g_lcd_fd,FBIOGET_VSCREENINFO,&g_var_info);
    if(ret < 0)
        {
        printf("FBIOGET_VSCREENINFO ioctl error\n");
        return -1;
        }
    return 0;
    }

/**
 *@brief LCD固定参数信息
 *@param fb_pix_screeninfo
 */
void LcdFixScreenInfo(void)
    {
    printf("smem_start = 0x%x.\n", g_fix_info.smem_start);         //LCD显存的起始地址(物理地址)
    printf("smem_len = %d.\n",     g_fix_info.smem_len);           //LCD显存的字节大小
    printf("line_length = %d\n",   g_fix_info.line_length);        //LCD一行的长度
    }

/**
 *@brief LCD可变参数信息
 *@param fb_var_screeninfo
 */
void LcdVarScreenInfo(void)
    {
    printf("xres = %d\n",           g_var_info.xres);              //水平分辨率
    printf("yres = %d\n",           g_var_info.yres);              //垂直分辨率
    printf("xres_virtual = %d\n",   g_var_info.xres_virtual);      //虚拟水平分辨率
    printf("yres_virtual = %d\n",   g_var_info.yres_virtual);      //虚拟垂直分辨率
    printf("xoffset = %d\n",        g_var_info.xoffset);           //当前显存水平偏移量
    printf("yoffset = %d\n",        g_var_info.yoffset);           //当前显存垂直偏移量
    printf("bits_per_pixel = %d\n", g_var_info.bits_per_pixel);    //像素深度
    printf("height = %d\n",         g_var_info.height);            //LCD的物理高度mm
    printf("width = %d\n",          g_var_info.width);             //LCD的物理宽度mm
    printf("pixclock = %d\n",       g_var_info.pixclock);          //像素时钟
    printf("left_margin = %d\n",    g_var_info.left_margin);
    printf("right_margin = %d\n",   g_var_info.right_margin);
    printf("upper_margin = %d\n",   g_var_info.upper_margin);
    printf("lower_margin = %d\n",   g_var_info.lower_margin);
    printf("hsync_len = %d\n",      g_var_info.hsync_len);
    printf("vsync_len = %d\n",      g_var_info.vsync_len);
    }

/**
 *@brief 映射LCD到内存
 *@return 成功：0 失败:-1
 */
int LcdMmap(void)
    {
    g_lcd_screen_size = g_var_info.xres * g_var_info.yres * (g_var_info.bits_per_pixel/8);
    g_lcd_mmap_buf = (unsigned char*)mmap(NULL,g_lcd_screen_size,PROT_READ | PROT_WRITE,MAP_SHARED,g_lcd_fd,0);
    if(g_lcd_mmap_buf == (void*)-1)
        {
        printf("%s mmap error\n",__FUNCTION__);
        return -1;
        }
    return 0;
    }

/**
 *@brief 获得LCD屏的总字节数
 *@return 成功：字节数 失败:-1
 */
int GetLcdScreenSize(void)
    {
    return g_lcd_screen_size;
    }

/**
 *@brief 获得LCD屏的宽度，高度
 *@desc 高16位为屏的宽度，低16位屏的高度
 *@return 成功：值 失败:-1
 */
int GetLcdScreenWidthAndHeight(void)
    {
    int theValue = (g_var_info.xres_virtual << 16) | g_var_info.yres_virtual ;
    return theValue;
    }


/**
 *@brief 获得LCD映射到内存的操作地址
 *@return 成功：映射地址 失败：NULL
 */
unsigned char *GetLcdMmapBuf(void)
    {
    return g_lcd_mmap_buf;
    }

/**
 *@brief LCD关闭
 */
void LcdClose(void)
    {
    close(g_lcd_fd);
    }
