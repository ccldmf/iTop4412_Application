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
    if(strstr(UartRecvBuf,ack) == NULL)         //在接收到的数据中没有找到指定的ACK字符串
        {
        printf("not come here\n");
        printf("data:%s\n",UartRecvBuf);
        return -1;
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
 *@brief Esp8266查看版本信息
 *@return 成功：版本信息 失败：null
 */
 char* Esp8266CheckVersion(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+GMR","OK");
    if(ret < 0)
        {
        printf("Esp8266CheckVersion error\n");
        return NULL;
        }
    return UartRecvBuf;
    }

/**
 *@brief Esp8266回显功能设置
 *@return 成功：0 失败：-1
 */
int Esp8266SetReturnDisplay(unsigned char flag)
    {
    int ret = -1;
    if(0 == flag)
        {
        ret = Esp8266SendCmd("ATE0","OK");
        }
    else if(1 == flag)
        {
        ret = Esp8266SendCmd("ATE1","OK");
        }
    return ret;
    }

/**
 *@brief Esp8266恢复出厂设置
 *@return 成功：0  失败：-1
 */
int Esp8266ResetFactoryData(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+RESTORE","OK");
    return ret;
    }

/**
 *@brief Esp8266设置串口参数
 *@return 成功：0  失败：-1
 */
int Esp8266SetUart(unsigned int baudrate,unsigned char databits,
                   unsigned char stopbits,unsigned char parity,unsigned char flow_con)
    {
    int ret = -1;
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
 *@brief Esp8266查询使用模式
 *@return 成功：返回模式 失败：-1
 */
enum WIFIMODE Esp8266CheckMode(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWMODE?","OK");
    if(ret == 0)
        {
        printf("Check Mode data:%s\n",UartRecvBuf);
        }
    else
        {
        ret = WIFI_ERROR;
        }
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
 *@brief Esp8266查看连接路由器
 *@return 成功：0 失败：-1
 */
int Esp8266CheckRouter(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWJAP?","OK");
    if(ret == 0)
        {
        printf("Check Router Info:%s\n",UartRecvBuf);
        }
    return ret;
    }

/**
 *@brief Esp8266查看当前可用的AP路由器
 *@return 成功：0 失败：-1
 */
int Esp8266CheckUsefulAPRouter(void)
{
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWLAP","OK");
    if(ret == 0)
        {
        printf("Check Useful Router Info:%s\n",UartRecvBuf);
        }
    return ret;
}

/**
 *@brief Esp8266退出与路由器的AP连接
 *@return 成功：0 失败：-1
 */
int Esp8266QuitRouterAPConnect(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWQAP","OK");
    return ret;
    }

/**
 *@brief Esp8266获得当前AP路由器参数
 *@return 成功：0 失败：-1
 */
int Esp8266GetAPRouterParam(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWSAP?","OK");
    if(ret == 0)
        {
        printf("Get Router Param Info:%s\n",UartRecvBuf);
        }
    return ret;
    }

/**
 *@brief Esp8266查看已经连接的设备
 *@return 成功：0 失败：-1
 */
int Esp8266CheckConnectedDevice(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWLIF","OK");
    if(ret == 0)
        {
        printf("Check connected device Info:%s\n",UartRecvBuf);
        }
    return ret;
    }

/**
 *@brief Esp8266设置DHCP
 *@param mode:模式 en:使能控制
 *@return 成功：0 失败：-1
 */
int Esp8266SetDHCP(enum WIFIMODE mode,unsigned char enable)
    {
    int ret = -1;
    return ret;
    }

/**
 *@brief Esp8266设置STA开机自动连接
 *@param en:使能控制
 *@return 成功：0 失败：-1
 */
int Esp8266SetSTAAutoConnect(unsigned char enable)
    {
    int ret = -1;
    memset(UartSendBuf,0,SENDBUFSIZE);
    sprintf(UartSendBuf,"AT+CWAUTOCONN=\"%d\"",enable);
    ret = Esp8266SendCmd(UartSendBuf,"OK");
    return ret;
    }

/**
 *@brief Esp8266设置STA的MAC地址
 *@param mac:MAC地址
 *@return 成功：0 失败：-1
 */
int Esp8266SetSTAMacAddress(char*mac)
    {
    int ret =-1;
    return ret;
    }

/**
 *@brief Esp8266获取STA的MAC地址
 *@return 成功：MAC地址 失败：NULL
 */
char* Esp8266GetSTAMacAddress(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CIPSTAMAC?","OK");
    if(ret == 0)
        {
        printf("Get STA Mac Address:%s\n",UartRecvBuf);
        }
    else
        {
        return NULL;
        }
    return UartRecvBuf;
    }

/**
 *@brief Esp8266设置AP的MAC地址
 *@param mac:MAC地址
 *@return 成功：0 失败：-1
 */
int Esp8266SetAPMacAddress(char*mac)
    {
    int ret =-1;
    return ret;
    }

/**
 *@brief Esp8266获取AP的MAC地址
 *@return 成功：MAC地址 失败：NULL
 */
char* Esp8266GetAPMacAddress(void)
{
    int ret = -1;
    ret = Esp8266SendCmd("AT+CIPAPMAC?","OK");
    if(ret == 0)
        {
        printf("Get AP Mac Address:%s\n",UartRecvBuf);
        }
    else
        {
        return NULL;
        }
    return UartRecvBuf;

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
 *@return 成功：0 失败：-1
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
