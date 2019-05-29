/**
 * @brief
 * @file     lcd_framebuffer.h
 * @author   cc
 * @version
 * @date     Sat 10 Mar 2018 12:06:21 AM PST
 * @mail     13568859409@163.com
 * @note
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

//LCD设备名
#define LCDDEVNAME      "/dev/fb0"

/**
 *@brief LCD初始化
 *@return 成功：0 失败：-1
 */
int LcdInit(void);

/**
 *@brief LCD固定参数信息
 *@param fb_pix_screeninfo
 */
void LcdFixScreenInfo(void);

/**
 *@brief LCD可变参数信息
 *@param fb_var_screeninfo
 */
void LcdVarScreenInfo(void);

/**
 *@brief 获得LCD屏的总字节数
 *@return 成功：字节数 失败:-1
 */
int GetLcdScreenSize(void);

/**
 *@brief 获得LCD屏的宽度，高度
 *@desc 高16位为屏的宽度，低16位屏的高度
 *@return 成功：值 失败:-1
 */
int GetLcdScreenWidthAndHeight(void);

/**
 *@brief 获得LCD映射到内存的操作地址
 *@desc  该函数定义放置到GUI.h中了，
 *       用于应用层直接使用
 *@return 成功：映射地址 失败：NULL
 */
//unsigned char *GetLcdMmapBuf(void);

/**
 *@brief LCD画点
 *@param x:横坐标 y:纵坐标 color:颜色
 *@return 成功：0 失败：-1
 */
int LcdDrawPoint(unsigned int x,unsigned int y,unsigned int color);

/**
 *@brief LCD画横线
 *@param x1:横坐标 y:纵坐标
 *       x2:横坐标 color:颜色
 *@return 成功：0 失败：-1
 */
int LcdDrawHLineExt(unsigned int x1,unsigned int y,unsigned int x2 ,unsigned int color);

/**
 *@brief LCD关闭
 */
void LcdClose(void);

#endif /*FRAMEBUFFER_H*/

