/**
 * @brief
 * @file     tcp.c
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 08:07:59 PM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int g_clt_sock = -1;

int TcpInit(const char *ip,const int port)
    {
    //创建套接字
    g_clt_sock = socket(AF_INET,SOCK_STREAM,0);
    if(g_clt_sock < 0)
        {
        printf("socket error\n");
        return -1;
        }
    //绑定信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    //发起连接
    socklen_t addr_len = sizeof(addr);
    int connect_fd = connect(g_clt_sock,(struct sockaddr*)&addr,addr_len);
    if(connect_fd < 0)
        {
        printf("connect error\n");
        return -1;
        }
    return 0;
    }

int TcpRecvdData(char *data,int len)
    {
    int ret = -1;
    ret = read(g_clt_sock,data,len);
    if(ret < 0)
        {
        printf("recv data error\n");
        return -1;
        }
    else
        {
        return ret;
        }
    }

int TcpSendData(const char *data,int len)
    {
    int ret;
    ret = write(g_clt_sock,data,len);
    if(ret < 0)
        {
        printf("send data error\n");
        return -1;
        }
    return 0;
    }

void TcpClose(void)
    {
    close(g_clt_sock);
    }
