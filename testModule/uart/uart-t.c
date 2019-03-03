/**
 * @brief
 * @file     uart-t.c
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 07:06:14 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"


#define UARTNAME    "/dev/ttySAC0"

int main(int argc,char *argv)
    {
    char *recv_data;
    int ret;
    ret = UartInit(UARTNAME,115200,8,'N',1);
    if(ret < 0)
        {
        printf("UartInit error\n");
        return -1;
        }

    recv_data = malloc(1024);
    if(recv_data == NULL)
        {
        printf("malloc memory error\n");
        return -1;
        }
    char AT_cmd[] = {'A','T','\r','\n'};
    while(1)
        {
#if 0
        ret = UartRecvData(recv_data,10);
        if(ret > 0)
            {
            UartSendData(recv_data,ret);
            }
#else
        UartSendData(AT_cmd,sizeof(AT_cmd));
        ret = UartRecvData(recv_data,100);
        if(ret > 0)
            {
            printf("recv:%s\n",recv_data);
            }
        sleep(1);
#endif
        }
    return 0;
    }


