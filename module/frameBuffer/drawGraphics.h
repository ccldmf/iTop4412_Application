/**
 * @brief
 * @file     drawGraphics.h
 * @author   cc
 * @version
 * @date     Sat 30 Mar 2019 06:30:10 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

/**
 *@brief LCD画点
 *@param x:横坐标 y:纵坐标 color:颜色
 *@return 成功：0 失败：-1
 */
int LcdDrawPoint(unsigned int x,unsigned int y,unsigned int color);

/**
 *@brief LCD画线
 *@param x1 y1：起点坐标 x2 y2:终点坐标 color:线条颜色
 */
void LcdDrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color);

/**
 *@brief LCD显示字符串
 *@param x1 y1：起点坐标 x2 y2:终点坐标 color:字符颜色
 */
void LcdDrawStr(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color,char *ptr);

/**
 *@brief LCD画按钮
 *@param x1 y1：起点坐标 width height:按钮的长和宽
 *       str:按钮中的字符 str-color:字符颜色 back-color:背景颜色
 */
void LcdDrawButton(unsigned int x1,unsigned int y1,unsigned int width,unsigned int height,
                   char *str,unsigned int str_color,unsigned int back_color);






