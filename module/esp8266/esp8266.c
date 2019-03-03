/**
 * @brief
 * @file     esp8266.c
 * @author   cc
 * @version
 * @date     Sat 30 Jun 2018 11:18:12 PM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../uart/uart.h"
#include "esp8266.h"

#define RECVBUFSIZE     1024
#define SENDBUFSIZE     1024

//定义串口接收数据存放空间
static char UartRecvBuf[RECVBUFSIZE];
//定义串口发送数据存放空间
static char UartSendBuf[SENDBUFSIZE];

/**
 *@brief Esp8266模块初始化
 *@param 成功：0	失败：-1
 */
int Esp8266Init(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT","OK");
    if(ret < 0)
        {
        printf("Esp8266Init error\n");
        }
    return ret;
    }

/**
 *@brief Esp8266模块发送密令
 *@param cmd:发送的密令字符串
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendCmd(const char *cmd,const char *ack)
    {
    int ret;
    sprintf(UartSendBuf,"%s\r\n",cmd);
    ret = UartSendData(UartSendBuf,strlen(UartSendBuf));
    if(ret < 0)
        {
        printf("Esp8266SendCmd SendData error.-L:%d\n",__LINE__);
        return -1;
        }
    memset(UartRecvBuf,0,RECVBUFSIZE);
    ret = UartRecvData(UartRecvBuf,RECVBUFSIZE);
    if(ret < 0)
        {
        printf("Esp8266SendCmd RecvData error.-L:%d\n",__LINE__);
        return -1;
        }
    if(strstr(UartRecvBuf,ack) == NULL)				//在接收到的数据中没有找到指定的ACK字符串
        {
        printf("not come here\n");
        printf("data:%s\n",UartRecvBuf);
        return -1;
        }
    return 0;
    }

/**
 *@brief Esp8266模块发送数据
 *@param cmd:发送的数据字符串,此时不需要添加回车
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendData(const char *data,const char *ack)
    {
    int ret = -1;
    ret = UartSendData(data,strlen(data));
    if(ret < 0)
        {
        printf("Esp8266SendData SendData error.-L:%d\n",__LINE__);
        return -1;
        }
    if(ack != NULL)                                     //需要对接收的数据进行比较
        {
        memset(UartRecvBuf,0,RECVBUFSIZE);
        ret = UartRecvData(UartRecvBuf,RECVBUFSIZE);
        if(ret < 0)
            {
            printf("Esp8266SendData RecvData error.-L:%d\n",__LINE__);
            return -1;
            }
        if(strstr(UartRecvBuf,ack) == NULL)
            {
            printf("Esp8266SendData ack error.-L:%d\n",__LINE__);
            return -1;
            }
        return 0;
        }
    return 0;
    }

/**
 *@brief Esp8266模块重启
 *@return 成功：0 失败：-1
 */
int Esp8266Reseat(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+RST","OK");
    return ret;
    }

/**
 *@brief Esp8266模块应用模式
 *@param mode：wifi模式参数
 *@return 成功：0 失败：-1
 */
int Esp8266SetMode(enum WIFIMODE mode)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    sprintf(UartSendBuf,"AT+CWMODE=%d",mode);
    ret = Esp8266SendCmd(UartSendBuf,"OK");
    return ret;
    }

/**
 *@brief Esp8266模块设置路由器
 *@param ssid:路由器名	password:密码
 *@return 成功：0 失败：-1
 */
int Esp8266SetRouter(const char *ssid,const char *password)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    sprintf(UartSendBuf,"AT+CWJAP=\"%s\",\"%s\"",ssid,password);
    ret = Esp8266SendCmd(UartSendBuf,"OK");
    return ret;
    }

/**
 *@brief Esp8266模块IP获取
 *@param 存放IP地址
 *@return 成功：0 失败：-1
 */
int Esp8266GetIPAddr(char *ip)
    {
    int ret = -1;
    char *ptr,*ptr2;
    memset(UartSendBuf,0,SENDBUFSIZE);
    sprintf(UartSendBuf,"AT+CIFSR");
    ret = Esp8266SendCmd(UartSendBuf,"OK");
    if(ret == 0)
        {
        ptr = strstr(UartRecvBuf,"CIFSR:STAIP");
        if(ptr != NULL)
            {
            ptr += sizeof("CIFSR:STAIP");
            ptr2 = strstr(ptr,"+CIFSR");
            *(ptr + (ptr2-ptr)) = '\0';             //将第一行结束换行符置为\0
            memcpy(ip,ptr,(ptr2-ptr)-1);
            }
        else                                        //未找到指定字符串
            {
            ret = -1;
            }
        }
    return ret;
    }

/**
 *@brief Esp8266模块连接到服务器
 *@param type:通信协议类型 ip:服务器ip地址 port服务器端口
 *@return 成功：0	失败：-1
 */
int  Esp8266ConnectServer(const char *type,const char *ip,const unsigned int port)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    sprintf(UartSendBuf,"AT+CIPSTART=\"%s\",\"%s\",%d",type,ip,port);
    ret = Esp8266SendCmd(UartSendBuf,"OK");
    return ret;
    }

/**
 *@brief Esp8266模块设置开启透传模式
 *@param 无
 *@return 成功：0 失败：-1
 */
int Esp8266SetTransMode(void)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    ret = Esp8266SendCmd("AT+CIPMODE=1","OK");
    return ret;
    }

/**
 *@brief Esp8266模块开始透传
 *@param 无
 *@return 成功：0 失败：-1
 */
int Esp8266StartTransmission(void)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    ret = Esp8266SendCmd("AT+CIPSEND","OK");
    return ret;
    }
