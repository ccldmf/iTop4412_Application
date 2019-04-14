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
#include <pthread.h>
#include <string.h>
#include "../../module/uart/uart.h"
#include "../../module/esp8266/esp8266.h"

#define RECVBUFSIZE     1024
unsigned char g_Transmission_Flag = 0;

void *uartRecvPthreadHandle(void *arg)
    {
    char *ptr;
    printf("uartRecvPthreadHandle\n");
    while(1)
        {
        if(g_Transmission_Flag == 1)
            {
            ptr = GetUartRecvBuf();
            if(ptr != NULL)
                {
                printf("Recv Data:%s\n",ptr);
                }
            UartRecvFree();
            sleep(1);
            }
        }
    }
// 7 2 9 g h i j m
int main(int argc,char **argv)
    {
    int ret;
    char c;
    char data[50];
    char *ptr;
    struct VERSION_INFO* theVersion;
    pthread_t uartRecvPthread;
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

    if (pthread_create(&uartRecvPthread, NULL, uartRecvPthreadHandle, NULL) == -1)
        {
        printf("create error!\n");
        return 1;
        }

    while(1)
        {
        c = getchar();
        switch(c)
            {
        case '1':                   //测试命令
            ret = Esp8266SendCmd("AT","OK");
            UartRecvFree();
            if(ret == 0)
                {
                printf("Esp8266SendCmd is ok\n");
                }
            else
                {
                printf("Esp8266SendCmd error\n");
                }
                break;
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
        case '3':                   //版本信息
            theVersion = Esp8266CheckVersion();
            if(theVersion == NULL)
                {
                printf("check version error\n");
                }
            else
                {
                printf("Version Info: %s\n",theVersion->ATVersion);
                printf("SDKVers Info: %s\n",theVersion->SDKVersion);
                printf("ComTime Info: %s\n",theVersion->ComTime);
                }
                break;
        case '4':                   //回显设置,即设置发送的命令是否再次显示
            ret = Esp8266SetReturnDisplay(0);
            if(ret == -1)
                {
                printf("set return display error\n");
                }
            else
                {
                printf("set return display OK\n");
                }
                break;
        case '5':                   //恢复出厂设置
            ret = Esp8266ResetFactoryData();
            if(ret == -1)
                {
                printf("set factory data error\n");
                }
            else
                {
                printf("set factory data OK\n");
                }
                break;
        case '6':                   //设置串口参数
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
        case '8':                   //查看wifi模式
            ret = Esp8266CheckMode();
            if(ret < 0)
                {
                printf("check mode error\n");
                }
            else
                {
                printf("check mode ok,Mode = %d\n",ret);
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
        case 'a':                   //查看连接路由器
            ptr = Esp8266CheckConnectRouter();
            if(ptr == NULL)
                {
                printf("check router error\n");
                }
            else
                {
                printf("check router ok,Router= %s\n",ptr);
                }
                break;

        case 'b':                   //退出连接的路由器
            ret = Esp8266QuitRouterAPConnect();
            if(ret < 0)
                {
                printf("quit router connect error\n");
                }
            else
                {
                printf("quit router connect ok\n");
                }
                break;

        case 'c':                   //设置DHCP
            ret = Esp8266SetDHCP(1,1);
            if(ret < 0)
                {
                printf("set DHCP error\n");
                }
            else
                {
                printf("set DHCP ok\n");
                }
                break;

        case 'd':                   //设置STA自动连接
            ret = Esp8266SetSTAAutoConnect(1);
            if(ret < 0)
                {
                printf("set sta auto connect error\n");
                }
            else
                {
                printf("set sta auto connect ok\n");
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
                g_Transmission_Flag = 1;
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
        case 'n':                   //Ping
            ret = Esp8266PingFeture("192.168.1.101");
            if(ret < 0)
                {
                printf("ping error\n");
                }
            else
                {
                printf("ping ok\n");
                }
                break;
        case 'o':                           // 关闭TCP或者UDP的连接
            ret = Esp8266DisableConnect();
            if(ret == 0)
                {
                printf("Esp8266DisableConnect OK\n");
                }
            else
                {
                printf("Esp8266DisableConnect error\n");
                }
            break;
        default:
            break;
            };
        }

    return 0;
    }
