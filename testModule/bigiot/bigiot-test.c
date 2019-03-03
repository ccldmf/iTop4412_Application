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
#include "bigiot.h"
#include "../tcp/tcp.h"

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

#define random(a,b)	((rand()%(b-a+1))+a)

void *thread2_handler(void *arg)
    {
    printf("pthread2 handler function!\n");
    char c;
    char data[10];
    while(1)
        {
        c = getchar();
        printf("c = %c\n",c);
        switch(c)
            {
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
                    sleep(6);
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
    TcpInit(IP,PORT);
    pthread_create(&thread1,NULL,(void*)thread1_handler,NULL);
    pthread_create(&thread2,NULL,(void*)thread2_handler,NULL);
    while(1)
        {
#if 0
        ret = BigiotRecvData(data,100);
        if(ret > 0)
            {
            printf("recv data:%s\n",data);
            }
#endif
        sleep(10);
        };
    return 0;
    }
