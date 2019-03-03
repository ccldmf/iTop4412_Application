/**
 * @brief
 * @file     bigito.c
 * @author   cc
 * @version
 * @date     Wed 27 Jun 2018 01:30:42 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bigiot.h"
#include "../tcp/tcp.h"
#define USETCP

#ifdef USETCP
#include "../tcp/tcp.h"
#else

#endif

/**
 *@brief 设备登录
 *@param id:设备ID apikey:设备apikey
 *@return 无
 */
void BigiotLogin(const char *id,const char *apikey)
    {
    char ptr1[] = "{\"M\":\"checkin\",\"ID\":\"";
    char ptr2[] = "\",\"K\":\"";
    char ptr3[] = "\"}\n";
    char *ptr = malloc(100);
    int len;
    memset(ptr,0,100);
    memcpy(ptr,ptr1,sizeof(ptr1));              //ptr1
    len = sizeof(ptr1) - 1;
    memcpy((ptr + len),id,strlen(id));          //id
    len = len + strlen(id);
    memcpy((ptr + len),ptr2,sizeof(ptr2));      //ptr2
    len = len + sizeof(ptr2) - 1;
    memcpy((ptr + len),apikey,strlen(apikey));  //apikey
    len = len + strlen(apikey);
    memcpy((ptr + len),ptr3,sizeof(ptr3));      //ptr3
    len = strlen(ptr);
    ptr[len-1] = '\n';                          //添加\n，必须要

#ifdef USETCP
    TcpSendData((const char *)ptr,len);
#else

#endif

    }

/**
 *@brief 发送数据
 *@param id:设备id data_id:数据接口ID value:值
 *@return 无
 */
void BigiotSendData(const char *id,const char *data_id,const char *value)
    {
    char ptr1[] = "{\"M\":\"update\",\"ID\":\"";
    char ptr2[] = "\",\"V\":{\"";
    char ptr3[] = "\":\"";
    char ptr4[] = "\"}}\n";
    char *ptr = malloc(100);
    int len,ret;

    memset(ptr,0,100);
    memcpy(ptr,ptr1,sizeof(ptr1));
    len = sizeof(ptr1) - 1;
    memcpy((ptr+len),id,strlen(id));
    len += strlen(id);
    memcpy((ptr+len),ptr2,sizeof(ptr2));
    len += sizeof(ptr2) - 1;
    memcpy((ptr+len),data_id,strlen(data_id));
    len += strlen(data_id);
    memcpy((ptr+len),ptr3,sizeof(ptr3));
    len += sizeof(ptr3) - 1;
    memcpy((ptr+len),value,strlen(value));
    len += strlen(value);
    memcpy((ptr+len),ptr4,sizeof(ptr4));
    len = strlen(ptr);
#ifdef USETCP
    ret = TcpSendData(ptr,len);
#else

#endif
    }

/**
 *@brief 接收数据
 *@param data:存放数据空间  len:读取数据的长度
 *@return 成功：返回实际接收到的数据长度  失败：-1
 */
int BigiotRecvData(char *data,int len)
    {
    int ret = 0;
#ifdef USETCP
    ret = TcpRecvdData(data,len);
#else

#endif
    *(data+ret) = '\0';     //在末尾添加字符串结束标志
    return ret;
    }

/**
 *@brief 查询设备或者用户是否在线
 *@param id:设备或者用户的id type:查询类型，分为设备、用户、游客
 *@return 无
 */
void BigiotIsOnLine(const char *id,enum IDTYPE type)
    {
    char ptr1[] = "{\"M\":\"isOL\",\"ID\":[\"";
    char ptr2[] = "\"]}\n";
    char *ptr = malloc(100);
    int len;
    memcpy(ptr,ptr1,sizeof(ptr1));          //ptr1
    len = sizeof(ptr1) - 1;

    if(type == DEVICE_ID)
        {
        memcpy(ptr+len,"D",sizeof("D"));
        }
    else if(type == USER_ID)
        {
        memcpy(ptr+len,"U",sizeof("U"));
        }
    else
        {
        memcpy(ptr+len,"P",sizeof("P"));
        }
    len = len + 1;
    memcpy((ptr+len),id,strlen(id));            //id
    len = len + strlen(id);
    memcpy((ptr+len),ptr2,sizeof(ptr2));        //ptr2
    printf("cmd:%s\n",ptr);
#ifdef USETCP
    TcpSendData(ptr,strlen(ptr));
#else
#endif
    }

/**
 *@brief 查询当前设备状态
 *@param 无
 *@return 无
 */
void BigiotStatus(void)
    {
    char ptr[] = "{\"M\":\"status\"}\n";
#ifdef USETCP
    TcpSendData(ptr,strlen(ptr));
#else
#endif
    }

/**
 *@brief 查询服务器时间
 *@param 无
 *@return 无
 */
void BigiotTime(void)
    {
    char ptr[] = "{\"M\":\"time\",\"F\":\"Y-m-d H:i:s\"}\n";
#ifdef USETCP
    TcpSendData(ptr,strlen(ptr));
#else

#endif
    }

/**
 *@brief 发送报警信息
 *@param message:报警信息 type：报警方式
 *@return 无
 */
void BigiotAlertMessage(const char *message,const char *type)
    {
    char ptr1[] = "{\"M\":\"alert\",\"C\":\"";
    char ptr2[] = "\",\"B\":\"";
    char ptr3[] = "\"}\n";
    char *ptr = malloc(100);
    int len;
    memset(ptr,0,100);
    memcpy(ptr,ptr1,sizeof(ptr1));                      //ptr1
    len = sizeof(ptr1) - 1;
    memcpy((ptr + len),message,strlen(message));        //message
    len = len + strlen(message);
    memcpy((ptr + len),ptr2,sizeof(ptr2));              //ptr2
    len = len + sizeof(ptr2) - 1;
    memcpy((ptr + len),type,strlen(type));              //type
    len = len + strlen(type);
    memcpy((ptr + len),ptr3,sizeof(ptr3));              //ptr3
    len = strlen(ptr);
    ptr[len-1] = '\n';                                  //添加\n，必须要
#ifdef USETCP
    TcpSendData(ptr,strlen(ptr));
#else

#endif

    }

/**
 *@brief 设备下线
 *@param id:设备id  apikey:设备apikey
 *@return 无
 */
void BigiotLogout(const char *id,const char *apikey)
    {
    char ptr1[] = "{\"M\":\"checkout\",\"ID\":\"";
    char ptr2[] = "\",\"K\":\"";
    char ptr3[] = "\"}\n";
    char *ptr = malloc(100);
    int len;
    memset(ptr,0,100);
    memcpy(ptr,ptr1,sizeof(ptr1));              //ptr1
    len = sizeof(ptr1) - 1;
    memcpy((ptr + len),id,strlen(id));          //id
    len = len + strlen(id);
    memcpy((ptr + len),ptr2,sizeof(ptr2));      //ptr2
    len = len + sizeof(ptr2) - 1;
    memcpy((ptr + len),apikey,strlen(apikey));  //apikey
    len = len + strlen(apikey);
    memcpy((ptr + len),ptr3,sizeof(ptr3));      //ptr3
    len = strlen(ptr);
    ptr[len-1] = '\n';                          //添加\n，必须要
#ifdef USETCP
    TcpSendData(ptr,strlen(ptr));
#else

#endif
    }
