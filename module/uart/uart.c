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
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include "uart.h"
#include <sys/epoll.h>
#include <sys/ioctl.h>


//串口文件描述符
static int g_uart_fd = -1;

// EPOLL
static int g_epoll_fd = -1;
struct epoll_event event;
struct epoll_event events[6];

// 设定接收到的数据是否还有用
char g_RecvDataNotUse = 0;

#define RECVBUFSIZE     2048
#define SENDBUFSIZE     2048

#define RECVBUFCOUNT    2
enum RECVBUFSTATE
    {
    RECVBUF_IDLE,
    RECVBUF_BUSY,
    RECVBUF_USE
    };

//定义串口接收数据存放空间
struct uartRecv
    {
    enum RECVBUFSTATE bufState;
    char UartRecvBuf[RECVBUFSIZE];
    }mUartRecv[RECVBUFCOUNT];

static char UartRecvTmpBuf[SENDBUFSIZE];
//定义串口发送数据存放空间
static char UartSendBuf[SENDBUFSIZE];

// 获取空闲的接收数据空间地址
static char* GetRecvIdleBuf(void);

/**
 *@brief 配置出口相关参数
 *@param devname;设备名 nSpeed:波特率 nBits:数据位
 *@param nEvent:奇偶校验位 nStop:停止位
 *@return 成功：0 失败：-1
 */
static int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
    {
    struct termios theOption,oldOpton;
    if( tcgetattr( fd,&oldOpton) !=  0)
        {
        printf("tcgetattr error\n");
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
        printf("Uart set param error\n");
        return -1;
        }
    return 0;
    }

/**
 *@brief 接收串口数据线程
 */
static void *UartRecvPthreadHandle(void *arg)
    {
    int ret;
    printf("uartRecvPthreadHandle\n");
    char *theRecvPtr;
    while(1)
        {
        ret = UartRecvData(UartRecvTmpBuf,RECVBUFSIZE);
        if(ret > 0)
            {
            if(g_RecvDataNotUse == 1)
                {
                theRecvPtr = GetRecvIdleBuf();
                if(theRecvPtr != NULL)
                    {
                    memcpy(theRecvPtr,UartRecvTmpBuf,ret);
                    }
                else
                    {
                    printf("Don't find recv buf\n");
                    }
                }
            else
                {
                printf("Don't target data\n");
                }
            }
        }
    }

static int EpollInit(void)
    {
    int ret = -1;
    g_epoll_fd = epoll_create(6);
    event.events = EPOLLET | EPOLLIN;
    event.data.fd = g_uart_fd;
    if(epoll_ctl(g_epoll_fd,EPOLL_CTL_ADD,g_uart_fd,&event) == 0)
        {
        ret = 0;
        }
    return ret;
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
    pthread_t uartRecvPthread;
    g_uart_fd = open(devname,O_RDWR|O_NOCTTY);
    if(g_uart_fd > 0)
        {
        fcntl(g_uart_fd,F_SETFL,0);
        ret = set_opt(g_uart_fd,nSpeed,nBits,nEvent,nStop);
        if(ret == 0)
            {
            ret = EpollInit();
            if(ret == 0)
                {
                for(ret = 0;ret < RECVBUFCOUNT;ret++)
                    {
                    mUartRecv[ret].bufState = RECVBUF_IDLE;
                    memset(mUartRecv[ret].UartRecvBuf,0,RECVBUFSIZE);
                    }
                ret = pthread_create(&uartRecvPthread, NULL, UartRecvPthreadHandle, NULL);
                if(ret != -1)
                    {
                    return 0;
                    }
                }
            }
        }
    return -1;
    }

/**
 *@brief 获得串口接收数据空闲的空间
 *@return 成功：接收地址 失败：NULL
 */
static char* GetRecvIdleBuf(void)
    {
    int i;
    for(i = 0;i < RECVBUFCOUNT;i++)
        {
        if(mUartRecv[i].bufState == RECVBUF_IDLE)
            {
            mUartRecv[i].bufState = RECVBUF_BUSY;
            memset(mUartRecv[i].UartRecvBuf,0,RECVBUFSIZE);
            return mUartRecv[i].UartRecvBuf;
            }
        }
    return NULL;
    }

/**
 *@brief 获得串口接收可用数据地址
 *@return 成功：接收地址 失败：NULL
 */
char *GetUartRecvBuf(void)
    {
    int i;
    for(i = 0;i < RECVBUFCOUNT;i++)
        {
        if(mUartRecv[i].bufState == RECVBUF_BUSY || mUartRecv[i].bufState == RECVBUF_USE)
            {
            mUartRecv[i].bufState = RECVBUF_USE;
            return mUartRecv[i].UartRecvBuf;
            }
        }
    return NULL;
    }

/**
 *@brief 释放不再使用的接收数据空间
 *@return 无
 */
void UartRecvFree(void)
    {
    int i;
    g_RecvDataNotUse = 0;
    for(i = 0;i < RECVBUFCOUNT;i++)
        {
        if(mUartRecv[i].bufState == RECVBUF_USE || mUartRecv[i].bufState == RECVBUF_BUSY)
            {
            mUartRecv[i].bufState = RECVBUF_IDLE;
            memset(mUartRecv[i].UartRecvBuf,0,RECVBUFSIZE);
            }
        }
    }

/**
 *@brief 获得串口发送数据地址
 *@return 成功：发送地址 失败：NULL
 */
char *GetUartSendBuf(void)
    {
    memset(UartSendBuf,0,SENDBUFSIZE);
    return UartSendBuf;
    }

/**
 *@brief 串口数据发送
 *@param data:数据 len:数据长度
 *@return 成功：0 失败：-1
 */
int UartSendData(const char *data,int len)
    {
    int ret;
    if(len > SENDBUFSIZE)
        {
        return -1;
        }
    g_RecvDataNotUse = 1;

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
    int tmplen,read_len = 0;
    int i,ret;
    int wait_count = 0;
    do
    {
    ret = epoll_wait(g_epoll_fd,events,1,100);
    if( ret == 0 )
        {
        wait_count ++;
        }
    else
        {
        for(i = 0;i < ret;i++)
            {
            if(( events[i].events & EPOLLERR ) ||
               ( events[i].events & EPOLLHUP ) ||
               ( !(events[i].events & EPOLLIN )))
               {
                printf("No data\n");
                break;
               }
            else if( events[i].events & EPOLLIN )
               {
                tmplen = read( events[i].data.fd ,data + read_len,len );
                tcdrain( g_uart_fd );
                tcflush( g_uart_fd,TCIOFLUSH );
                if( read_len < len )
                    {
                    read_len += tmplen;
                    wait_count = 0;     // 再次获取数据
                    }
                else                    // 读取数据完毕
                    {
                    break;
                    }
               }
            }
        }
    }while(wait_count < 3);
    return read_len;
    }

/**
 *@brief 串口关闭
 */
void UartClose(void)
    {
    close(g_uart_fd);
    close(g_epoll_fd);
    }

