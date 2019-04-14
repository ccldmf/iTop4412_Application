/**
 * @brief
 * @file     bigiot-test.c
 * @author   cc
 * @version
 * @date     Wed 27 Jun 2018 03:09:44 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../../module/bigiot/bigiot.h"

#ifdef USETCP
#include "../../module/tcp/tcp.h"
#else
#include "../../module/esp8266/esp8266.h"
#include "../../module/uart/uart.h"
#endif

void *thread1_handler(void *arg)
    {
    printf("pthread1 handler function!\n");
    int ret = 0;
    char data[100] = {0};
    while(1)
        {
        ret = BigiotRecvData(data,100);
        if(ret > 0)
            {
            printf("recv data:%s\n",data);
            }
        }
    }

#define random(a,b) ((rand()%(b-a+1))+a)

void *thread2_handler(void *arg)
    {
    printf("pthread2 handler function!\n");
    char c;
    char data[10];
    int ret;
    char *ptr;
    while(1)
        {
        c = getchar();
        printf("c = %c\n",c);
        switch(c)
            {

#ifdef USEUART
            case '2':                   //模块重启
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

            case '7':                   //设置wifi模式
                ret = Esp8266SetMode(WIFI_STATION);
                if(ret < 0)
                    {
                    printf("set mode error\n");
                    }
                else
                    {
                    printf("set mode ok\n");
                    }
                    break;
            case '9':                   //设置连接路由器
                ret = Esp8266SetRouter("TP-LINK_028A","ccldmf1994");
                if(ret < 0)
                    {
                    printf("set router error\n");
                    }
                else
                    {
                    printf("set router ok\n");
                    }
                    break;

            case 'g':                   //获得IP
                ptr = Esp8266GetLocalIPAddr();
                if(ptr < 0)
                    {
                    printf("get ip error\n");
                    }
                else
                    {
                    printf("ip data:%s\n",ptr);
                    }
                    break;
            case 'h':                   //连接服务器
                ret = Esp8266ConnectServer("TCP","121.42.180.30",8181);

                if(ret < 0)
                    {
                    printf("connect server error\n");
                    }
                else
                    {
                    printf("connect server ok\n");
                    }
                    break;
            case 'i':                   //设置开启透传模式
                ret = Esp8266SetTransMode(1);
                if(ret < 0)
                    {
                    printf("set transmode error\n");
                    }
                else
                    {
                    printf("set transmode ok\n");
                    }
                    break;
            case 'j':                   //设置开始透传
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

            case 'k':                   //退出透传模式
                Esp8266QuitTransmission();
                printf("quit transmission test\n");
                break;

            case 'm':                   //传输数据
                ret = Esp8266SendData("hello world",NULL);
                if(ret < 0)
                    {
                    printf("send data error\n");
                    }
                else
                    {
                    printf("send data ok\n");
                    }
                    break;
#endif

            case 'S':           //查询状态
                BigiotStatus();
                break;
            case 'T':           //查询时间
                BigiotTime();
                break;
            case 'L':           //登录
                BigiotLogin((const char *)"6757",(const char *)"a215adb24");
                break;
            case 'O':           //下线
                BigiotLogout((const char *)"6757",(const char *)"a215adb24");
                break;
            case 'A':           //报警    测试未通过
                BigiotAlertMessage((const char *)"alert test",(const char *)"13568859409@163.com");
                break;
            case 'I':           //检训设备是否在线
                BigiotIsOnLine("6757",DEVICE_ID);
                break;
            case 'E':           //发送数据值终端
                while(1)
                    {
                    sprintf(data,"%d",random(20,90));
                    BigiotSendData("6757","6033",data);
                    //BigiotSendData("6757","6033","35.3");
                    sleep(1);
                    }
                break;
            default:
            break;
            }
        }
    }

int main(int argc,char **argv)
    {
    pthread_t thread1,thread2;

#ifdef USETCP
    TcpInit(IP,PORT);
    pthread_create(&thread1,NULL,(void*)thread1_handler,NULL);
#else
    UartInit("/dev/ttySAC0",115200,8,'N',1);
    Esp8266Init();
#endif

    pthread_create(&thread2,NULL,(void*)thread2_handler,NULL);
    while(1)
        {
        sleep(10);
        };
    return 0;
    }
