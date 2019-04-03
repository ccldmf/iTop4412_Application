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
#include "../../module/uart/uart.h"
#include "../../module/esp8266/esp8266.h"

int main(int argc,char **argv)
    {
    int ret;
    char c;
    char data[50];
    char *ptr;
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
        case '1':                   //测试命令
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
            ptr = Esp8266CheckVersion();
            if(ptr == NULL)
                {
                printf("check version error\n");
                }
            else
                {
                printf("Version Info: %s\n",ptr);
                }
                break;
        case '4':                   //回显设置,即设置发送的命令是否再次显示
            ret = Esp8266SetReturnDisplay(1);
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
            ptr = Esp8266CheckRouter();
            if(ptr == NULL)
                {
                printf("check router error\n");
                }
            else
                {
                printf("check router ok,Router= %s\n",ptr);
                }
                break;
        case 'b':                   //查看可用的路由器
            ptr = Esp8266CheckUsefulAPRouter();
            if(ptr == NULL)
                {
                printf("check useful router error\n");
                }
            else
                {
                printf("check useful router ok\n");
                }
                break;
        case 'c':                   //退出连接的AP路由器
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

        case 'f':                   //设置DHCP
            ret = Esp8266SetDHCP(0,1);
            if(ret < 0)
                {
                printf("set DHCP error\n");
                }
            else
                {
                printf("set DHCP ok\n");
                }
                break;

        case 'g':                   //设置STA自动连接
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

        case 'j':                   //设置AP的MAC地址
            ret = Esp8266SetAPMacAddress(NULL);
            if(ret < 0)
                {
                printf("set ap mac address error\n");
                }
            else
                {
                printf("set ap mac address ok\n");
                }
                break;

        case 'k':                   //获得AP的MAC地址
            ptr = Esp8266GetAPMacAddress();
            if(ptr == NULL)
               {
               printf("get ap mac address error\n");
               }
            else
               {
               printf("get ap mac address ok\n");
               }
               break;
        case 'm':                   //设置AP的IP地址
            ret = Esp8266SetAPIPAddress("192.168.1.105");
            if(ret < 0)
               {
               printf("set ap ip address error\n");
               }
            else
               {
               printf("set ap ip address ok\n");
               }
               break;
       case 'n':                   //获得AP的IP地址
            ptr = Esp8266GetAPIPAddress();
            if(ptr == NULL)
               {
               printf("get ap ip address error\n");
               }
            else
               {
               printf("get ap ip address ok\n");
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

        case 'G':                   //获得IP
            ret = Esp8266GetLocalIPAddr(data);
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
            ret = Esp8266ConnectServer("TCP","192.168.1.101",8080);
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
        case 'Q':                   //Ping
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
        default:
            break;
            };
        }

    return 0;
    }
