/**
 * @brief
 * @file     tcp-test.c
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 11:20:29 PM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include "tcp.h"

int main(int argc,char **argv)
    {
    int ret;
    char data[] = "{\"M\":\"checkin\",\"ID\":\"6757\",\"K\":\"a215adb24\"}\n";
    char recvdata[100];
    TcpClose();
    ret = TcpInit(IP,PORT);
    if(ret < 0)
        {
        printf("TcpInit error\n");
        return -1;
        }
    TcpSendData(data,sizeof(data));
    ret = TcpRecvdData(recvdata,30);
    if(ret < 0)
        {
        printf("Recv data error\n");
        return -1;
        }
    else
        {
        recvdata[ret] = '\0';
        printf("recv:%s\n",recvdata);
        }
    TcpClose();
    return 0;
    }



