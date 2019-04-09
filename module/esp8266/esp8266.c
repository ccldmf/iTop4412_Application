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

// 定义存放可用路由器信息空间
// Size = AP_INFO_MAX_COUNT*sizeof(struct AP_ROUTER_INFO)
struct AP_ROUTER_INFO *mAPRounterInfo = NULL;

// 定义存放版本信息空间
// Size = sizeof(struct VERSION_INFO)
struct VERSION_INFO *mVersionInfo = NULL;

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
    else
        {
        mAPRounterInfo = (struct AP_ROUTER_INFO*)malloc(sizeof(struct AP_ROUTER_INFO) +
                                                        sizeof(struct SINGLE_AP_INFO) * AP_INFO_MAX_COUNT);
        if(mAPRounterInfo == NULL)
            {
            printf("Esp8266Init malloc router info space error\n");
            return -1;
            }
        else
            {
            memset(mAPRounterInfo,0,AP_INFO_MAX_COUNT*sizeof(struct AP_ROUTER_INFO));
            }

        mVersionInfo = (struct VERSION_INFO*)malloc(sizeof(struct VERSION_INFO));
        if(mVersionInfo == NULL)
            {
            printf("Esp8266Init malloc Version info space error\n");
            free(mAPRounterInfo->singleAPInfo);
            free(mAPRounterInfo);
            return -1;
            }
        else
            {
            memset(mVersionInfo,0,sizeof(struct VERSION_INFO));
            }
        }
    return ret;
    }

/**
 *@brief Esp8266模块关闭
 *@return 成功：0 失败：-1
 */
int Esp8266Close(void)
    {
    if(mAPRounterInfo != NULL)
        {
        free(mAPRounterInfo->singleAPInfo);
        free(mAPRounterInfo);
        }
    if(mVersionInfo != NULL)
        {
        free(mVersionInfo);
        }
    return 0;
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
    char *sendPtr = GetUartSendBuf();
    char *recvPtr = GetUartRecvBuf();
    sprintf(sendPtr,"%s\r\n",cmd);
    ret = UartSendData(sendPtr,strlen(sendPtr));
    if(ret < 0)
        {
        printf("Esp8266SendCmd SendData error.-L:%d\n",__LINE__);
        return -1;
        }

    sleep(1);
    if(strstr(recvPtr,ack) == NULL)         //在接收到的数据中没有找到指定的ACK字符串
        {
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
struct VERSION_INFO* Esp8266CheckVersion(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+GMR","OK");
    if(ret < 0)
        {
        printf("Esp8266CheckVersion error\n");
        return NULL;
        }
    char *p1,*p2;
    char *recvPtr = GetUartRecvBuf();
    p1 = strstr(recvPtr,"AT version:");
    p2 = strstr(recvPtr,"(");

    if(p1 != NULL && p2 != NULL && p1 < p2)
        {
        p1 = p1 + strlen("AT version:");
        memcpy(mVersionInfo->ATVersion,p1,p2-p1);
        }
    else
        {
        return NULL;
        }

    p1 = p2;
    p1 = strstr(p1,"SDK version:");
    p2 = strstr(p1,"(");
    if(p1 != NULL && p2 != NULL && p1 < p2)
        {
        p1 = p1 + strlen("SDK version:");
        memcpy(mVersionInfo->SDKVersion,p1,p2-p1);
        }
    else
        {
        return NULL;
        }

    p1 = p2;
    p1 = strstr(p1,"compile time:");
    p2 = strstr(p1,"OK");
    if(p1 != NULL && p2 != NULL && p1 < p2)
        {
        p1 = p1 + strlen("compile time:");
        memcpy(mVersionInfo->SDKVersion,p1,p2-p1);
        }
    else
        {
        return NULL;
        }
    return mVersionInfo;
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
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CWMODE=%d",mode);
    ret = Esp8266SendCmd(theSendPtr,"OK");
    return ret;
    }

/**
 *@brief Esp8266查询使用模式
 *@return 成功：返回模式 失败：-1
 */
enum WIFIMODE Esp8266CheckMode(void)
    {
    char *ptr;
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWMODE?","OK");
    if(ret == 0)
        {
        char *theRecvPtr = GetUartRecvBuf();
        ptr = strstr(theRecvPtr,":");
        if(ptr != NULL)
            {
            ret = (*(ptr+1) - '0');
            if(ret < WIFI_STATION || ret > WIFI_AP_STATION)
                {
                ret = WIFI_ERROR;
                }
            }
        }
    else
        {
        ret = WIFI_ERROR;
        }
    return ret;
    }

/**
 *@brief Esp8266模块设置路由器
 *@param ssid:路由器名 password:密码
 *@return 成功：0 失败：-1
 */
int Esp8266SetRouter(const char *ssid,const char *password)
    {
    int ret = -1;
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CWJAP=\"%s\",\"%s\"",ssid,password);
    ret = Esp8266SendCmd(theSendPtr,"WIFI CONNECTED");
    return ret;
    }

/**
 *@brief Esp8266查看连接路由器
 *@return 成功:返回选择的AP 失败：NULL
 */
char* Esp8266CheckRouter(void)
    {
    char *ptr;
    int ret = -1;
    ret = Esp8266SendCmd("AT+CWJAP?","OK");
    if(ret == 0)
        {
        char *theRecvPtr = GetUartRecvBuf();
        printf("Check Router Info:%s\n",theRecvPtr);
        ptr = strstr(theRecvPtr,":");
        if(ptr == NULL)
            {
            return NULL;
            }
        else
            {
            ptr = ptr + 1;
            ptr = strtok(ptr,"\"");
            return ptr;
            }
        }
    return NULL;
    }

/**
 *@brief Esp8266查看当前可用的AP路由器
 *@return 成功：AP信息 失败：NULL
 */
struct AP_ROUTER_INFO* Esp8266CheckUsefulAPRouter(void)
    {
    char *ptr,*tmp,*tmpOffset;
    int ret = -1;
    int SsidLen;
    ret = Esp8266SendCmd("AT+CWLAP","OK");
    if(ret == 0)
    {
    char *theRecvPtr = GetUartRecvBuf();;
    ptr = strstr(theRecvPtr,"(");
    ret = 0;
    while(ptr != NULL)
        {
        ptr = ptr + 1;
        tmpOffset = ptr;
        mAPRounterInfo->singleAPInfo[ret].Type = *ptr - '0';
        ptr = ptr + 3;
        //printf("ptr = %s\n",ptr);

        tmp = strtok(ptr,"\"");
        //printf("tmp = %s\n",tmp);
        SsidLen = strlen(tmp);
        //printf("SsidLen = %d\n",SsidLen);

        memcpy(mAPRounterInfo->singleAPInfo[ret].Ssid,ptr,SsidLen);

        tmp = strstr(ptr,",");
        tmp = strtok(tmp,",");
        //printf("tmp:%s\n",tmp);
        mAPRounterInfo->singleAPInfo[ret].Rssi = 255 - atoi(tmp) + 1;
        mAPRounterInfo->APCountNum = ret + 1;

        ret++;
        if(ret > AP_INFO_MAX_COUNT)
            {
            break;
            }

        printf("ptr:%s\n",tmpOffset);
        ptr = strstr(tmpOffset,")");
        }
    }
    printf("ret=%d\n",ret);
// Balder_test -->
    int i = 0;
    for(i = 0;i < mAPRounterInfo->APCountNum;i++)
    {
    printf("type-%d = %d\n",i,mAPRounterInfo->singleAPInfo[i].Type);
    printf("Ssid-%d = %s\n",i,mAPRounterInfo->singleAPInfo[i].Ssid);
    printf("Rssi-%d = %d\n",i,mAPRounterInfo->singleAPInfo[i].Rssi);
    }
// <-- Balder_test
    return mAPRounterInfo;
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
 *@brief Esp8266设置DHCP
 *@param mode:模式 en:使能控制
 *@return 成功：0 失败：-1
 */
int Esp8266SetDHCP(unsigned char mode,unsigned char enable)
    {
    int ret = -1;
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CWDHCP=%d,%d",mode,enable);
    ret = Esp8266SendCmd(theSendPtr,"OK");
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
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CWAUTOCONN=%d",enable);
    ret = Esp8266SendCmd(theSendPtr,"OK");
    return ret;
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
        printf("Get AP Mac Address\n");
        }
    else
        {
        return NULL;
        }
    return NULL;  //just for test
}

/**
 *@brief Esp8266设置AP模式的IP地址
 *@return 成功：0 失败：-1
 */
int Esp8266SetAPIPAddress(char *ip)
    {
    int ret = -1;
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CIPAP=\"%s\"",ip);
    ret = Esp8266SendCmd(theSendPtr,"OK");
    return ret;
    }

/**
 *@brief Esp8266获取AP模式的IP地址
 *@return 成功：IP地址 失败：NULL
 */
char* Esp8266GetAPIPAddress(void)
    {
    int ret = -1;
    ret = Esp8266SendCmd("AT+CIPAP?","OK");
    if(ret < 0)
        {
        return NULL;
        }
    return NULL;    //just for test
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
        char *theRecvPtr = GetUartRecvBuf();
        if(theRecvPtr == NULL)
            {
            return -1;
            }
        if(strstr(theRecvPtr,ack) == NULL)
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
int Esp8266GetLocalIPAddr(char *ip)
    {
    int ret = -1;
    char *ptr,*ptr2;
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CIFSR");
    ret = Esp8266SendCmd(theSendPtr,"OK");
    if(ret == 0)
        {
        char *theRecvPtr = GetUartRecvBuf();
        ptr = strstr(theRecvPtr,"CIFSR:STAIP");
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
    char *theSendPtr = GetUartSendBuf();
    sprintf(theSendPtr,"AT+CIPSTART=\"%s\",\"%s\",%d",type,ip,port);
    ret = Esp8266SendCmd(theSendPtr,"OK");
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
    ret = Esp8266SendCmd("AT+CIPSEND","OK");
    return ret;
    }

/**
 *@brief Esp8266模块ping功能
 *@param ip:IP地址
 *@return 成功：0 失败：-1
 */
int Esp8266PingFeture(const char *ip)
{
int ret = -1;
char *theSendPtr = GetUartSendBuf();
sprintf(theSendPtr,"AT+PING=\"%s\"",ip);
ret = Esp8266SendCmd(theSendPtr,"OK");
return ret;
}
