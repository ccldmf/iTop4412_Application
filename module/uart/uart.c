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
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0)
        {
        perror("SetupSerial 1");
        return -1;
        }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch( nBits )
        {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        }

    switch( nEvent )
        {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
        }

    switch( nSpeed )
        {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 921600:
            printf("B921600\n");
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        }
    if( nStop == 1 )
        newtio.c_cflag &=  ~CSTOPB;
    else if ( nStop == 2 )
        newtio.c_cflag |=  CSTOPB;
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
        {
        perror("com set error");
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
    if(ret < 0)
        {
        printf("UartSendData error\n");
        return -1;
        }
    return 0;
    }

/**
 *@brief 串口接收数据
 *@param data:接收数据存放的空间  len:需要接收的数据长度
 *@return 返回实际接收到的数据长度
 */
int UartRecvData(char *data,int len)
    {
    int read_len = 0;
    int ret;
    int fs_sel;
    unsigned char continue_read = 1;
    fd_set fs_read;
    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(g_uart_fd,&fs_read);

    time.tv_sec = 3;            //定时2秒，如果在2秒钟仍未收到数据，则认为是下一个数据
    time.tv_usec = 0;

    do
    {
    fs_sel = select(g_uart_fd+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)                  //有数据
        {
        ret = read(g_uart_fd,(data+read_len),len);
        read_len += ret;
        }
    else if(fs_sel == 0)        //超时
        {
        continue_read = 0;
        return read_len;
        }
    else                        //错误
        {
        return -1;
        }
    }while((continue_read == 1)&&(read_len < len));
    }

/**
 *@brief 串口关闭
 */
void UartClose(void)
    {
    close(g_uart_fd);
    }
