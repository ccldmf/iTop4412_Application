/**
 * @brief
 * @file     uart.c
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 06:31:17 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include "uart.h"


//串口文件描述符
static int g_uart_fd = -1;

/**
 *@brief 配置出口相关参数
 *@param devname;设备名 nSpeed:波特率 nBits:数据位
 *@param nEvent:奇偶校验位 nStop:停止位
 *@return 成功：0 失败：-1
 */
static int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
    {
    struct termios theOption,oldOpton;
    if  ( tcgetattr( fd,&oldOpton) !=  0)
        {
        perror("SetupSerial 1");
        return -1;
        }

    bzero( &theOption, sizeof( theOption ));
    //设置串口输入和输出波特率
    switch( nSpeed )
       {
       case 2400:
           cfsetispeed(&theOption, B2400);
           cfsetospeed(&theOption, B2400);
           break;
       case 4800:
           cfsetispeed(&theOption, B4800);
           cfsetospeed(&theOption, B4800);
           break;
       case 9600:
           cfsetispeed(&theOption, B9600);
           cfsetospeed(&theOption, B9600);
           break;
       case 115200:
           cfsetispeed(&theOption, B115200);
           cfsetospeed(&theOption, B115200);
           break;
       case 460800:
           cfsetispeed(&theOption, B460800);
           cfsetospeed(&theOption, B460800);
           break;
       case 921600:
           cfsetispeed(&theOption, B921600);
           cfsetospeed(&theOption, B921600);
           break;
       default:
           cfsetispeed(&theOption, B9600);
           cfsetospeed(&theOption, B9600);
           break;
       }

    //修改控制模式，保证程序不会占用串口
    theOption.c_cflag |= CLOCAL;
    //且能够从串口中读取数据
    theOption.c_cflag |= CREAD;

    //设置数据流控控制，此处不适用数据流控制
    theOption.c_cflag &= ~CRTSCTS;

    //屏蔽其他标志位
    theOption.c_cflag &= ~CSIZE;
    //设置数据位
    switch( nBits )
        {
        case 5:
            theOption.c_cflag |= CS5;
            break;
        case 6:
            theOption.c_cflag |= CS6;
            break;
        case 7:
            theOption.c_cflag |= CS7;
            break;
        case 8:
            theOption.c_cflag |= CS8;
            break;
        default:
            theOption.c_cflag |= CS8;
            break;
        }

    //设置校验位
    switch( nEvent )
        {
        case 'O':       //设置为奇校验
            theOption.c_cflag |= (PARODD | PARENB);
            theOption.c_iflag |= INPCK;
            break;
        case 'E':       //设置为偶校验
            theOption.c_cflag |= PARENB;
            theOption.c_cflag &= ~PARODD;
            theOption.c_iflag |= INPCK;
            break;
        case 'N':       //无奇偶校验
            theOption.c_cflag &= ~PARENB;
            theOption.c_iflag &= ~INPCK;
            break;
        }

    //设置停止位
    if( nStop == 1 )
        theOption.c_cflag &= ~CSTOPB;
    else if ( nStop == 2 )
        theOption.c_cflag |= CSTOPB;
    else
        theOption.c_cflag &= ~CSTOPB;

    //修改输出模式，原始数据输出
    theOption.c_oflag &= ~OPOST;
    theOption.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    //设置等待时间和最小接收字符
    theOption.c_cc[VTIME] = 1;      //读取一个字符等待 1*(1/10)s
    theOption.c_cc[VMIN]  = 1;      //读取字符的最少个数为1

    //如果发生数据溢出，接收数据，但是不再读取，刷新收到的数据但是不读
    tcflush(fd,TCIFLUSH);
    //激活配置
    if((tcsetattr(fd,TCSANOW,&theOption))!=0)
        {
        perror("Uart set param error");
        return -1;
        }
    return 0;
    }

/**
 *@brief 串口初始化
 *@param devname;设备名 nSpeed:波特率 nBits:数据位
 *@param nEvent:奇偶校验位 nStop:停止位
 *@return 成功：0 失败：-1
 */
int UartInit(const char *devname,int nSpeed,int nBits,char nEvent,int nStop)
    {
    int ret;
    g_uart_fd = open(devname,O_RDWR|O_NOCTTY);
    if(g_uart_fd < 0)
        {
        printf("open %s is failed\n",devname);
        return -1;
        }
    printf("fd = %d\n",g_uart_fd);

    fcntl(g_uart_fd,F_SETFL,0);

    ret = set_opt(g_uart_fd,nSpeed,nBits,nEvent,nStop);
    if(ret < 0)
        {
        printf("set uart param error\n");
        return -1;
        }
    return 0;
    }

/**
 *@brief 串口数据发送
 *@param data:数据 len:数据长度
 *@return 成功：0 失败：-1
 */
int UartSendData(const char *data,int len)
    {
    int ret;
    ret = write(g_uart_fd,data,len);
    if(ret == len)
        {
        return 0;
        }
    else
        {
        tcflush(g_uart_fd,TCOFLUSH);
        }
    return -1;
    }

/**
 *@brief 串口接收数据
 *@param data:接收数据存放的空间  len:需要接收的数据长度
 *@return 返回实际接收到的数据长度
 */
int UartRecvData(char *data,int len)
    {
    int ret;
    int fs_sel;
    fd_set fs_read;
    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(g_uart_fd,&fs_read);

    time.tv_sec = 3;
    time.tv_usec = 0;

    fs_sel = select(g_uart_fd+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)                  //有数据
        {
        ret = read(g_uart_fd,data,len);
        return ret;
        }
    else                        //错误
        {
        return -1;
        }
    }

/**
 *@brief 串口关闭
 */
void UartClose(void)
    {
    close(g_uart_fd);
    }

