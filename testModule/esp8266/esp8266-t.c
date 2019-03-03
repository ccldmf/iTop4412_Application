/**
 * @brief
 * @file     esp8266-t.c
 * @author   cc
 * @version
 * @date     Sun 01 Jul 2018 12:12:02 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include "esp8266.h"
#include <pthread.h>
#include "../uart/uart.h"

int main(int argc,char **argv)
    {
    int ret;
    char c;
    char data[50];
    ret = UartInit("/dev/ttySAC0",115200,8,'N',1);
    if(ret < 0)
        {
        printf("UartInit error\n");
        return -1;
        }
    ret = Esp8266Init();
    if(ret < 0)
        {
        printf("esp8266 init error\n");
        UartClose();
        return -1;
        }
    while(1)
        {
        c = getchar();
        switch(c)
            {
            case 'S':                   //测试命令
            ret = Esp8266SendCmd("AT","OK");
            if(ret == 0)
                {
                printf("Esp8266SendCmd is ok\n");
                }
            else
                {
                printf("Esp8266SendCmd error\n");
                }
                break;
            case 'D':                   //数据发送，尚未测试
            ret = Esp8266SendData("hello",NULL);
            if(ret == 0)
                {
                printf("Esp8266SendData is ok\n");
                }
            else
                {
                printf("Esp8266SendData error\n");
                }
                break;
            case 'R':                   //模块重启
            ret = Esp8266Reseat();
            if(ret < 0)
                {
                printf("reseat error\n");
                }
            else
                {
                printf("reseat ok\n");
                }
                break;
            case 'M':                   //设置wifi模式
            ret = Esp8266SetMode(1);
            if(ret < 0)
                {
                printf("set mode error\n");
                }
            else
                {
                printf("set mode ok\n");
                }
                break;
            case 'O':                   //设置路由器
            ret = Esp8266SetRouter("TP-LINK_028A","ldmf1994");
            if(ret < 0)
                {
                printf("set router error\n");
                }
            else
                {
                printf("set router ok\n");
                }
                break;
            case 'G':                   //获得IP
            ret = Esp8266GetIPAddr(data);
            if(ret < 0)
                {
                printf("get ip error\n");
                }
            else
                {
                printf("ip data:%s\n",data);
                }
                break;
            case 'C':                   //连接服务器
            ret = Esp8266ConnectServer("TCP","192.168.1.102",8080);
            if(ret < 0)
                {
                printf("connect server error\n");
                }
            else
                {
                printf("connect server ok\n");
                }
                break;
            case 'P':                   //设置开启透传模式
            ret = Esp8266SetTransMode();
            if(ret < 0)
                {
                printf("set transmode error\n");
                }
            else
                {
                printf("set transmode ok\n");
                }
                break;
            case 'N':                   //设置开始透传
            ret = Esp8266StartTransmission();
            if(ret < 0)
                {
                printf("start transmission error\n");
                }
            else
                {
                printf("start transmission ok\n");
                }
                break;
            default:
                break;
            };
        }

    return 0;
    }
