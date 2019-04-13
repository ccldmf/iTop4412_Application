/**
 * @brief
 * @file     uart.h
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 06:30:58 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

//串口相关的头文件
#include<stdio.h>      /*标准输入输出定义*/
#include<stdlib.h>     /*标准函数库定义*/
#include<unistd.h>     /*Unix 标准函数定义*/
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>      /*文件控制定义*/
#include<termios.h>    /*PPSIX 终端控制定义*/
#include<errno.h>      /*错误号定义*/
#include<string.h>


/**
 *@brief 串口初始化
 *@param devname;设备名 nSpeed:波特率 nBits:数据位
 *@param nEvent:奇偶校验位 nStop:停止位
 *@return 成功：0 失败：-1
 */
int UartInit(const char *devname,int nSpeed,int nBits,char nEvent,int nStop);

/**
 *@brief 获得串口发送数据地址
 *@return 成功：发送地址 失败：NULL
 */
char *GetUartSendBuf(void);

/**
 *@brief 获得串口接收可用数据地址
 *@return 成功：接收地址 失败：NULL
 */
char *GetUartRecvBuf(void);

/**
 *@brief 串口数据发送
 *@param data:数据 len:数据长度
 *@return 成功：0 失败：-1
 */
int UartSendData(const char *data,int len);

/**
 *@brief 串口接收数据
 *@param data:接收数据存放的空间  len:需要接收的数据长度
 *@return 返回实际接收到的数据长度
 */
int UartRecvData(char *data,int len);

/**
 *@brief 释放不再使用的接收数据空间
 *@return 无
 */
void UartRecvFree(void);

/**
 *@brief 串口关闭
 */
void UartClose(void);

