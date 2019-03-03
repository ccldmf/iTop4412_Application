/**
 * @brief
 * @file     esp8266.h
 * @author   cc
 * @version
 * @date     Sat 30 Jun 2018 11:17:55 PM PDT
 * @mail     13568859409@163.com
 * @note
 */

#ifndef _ESP8266_H
#define _ESP8266_H

enum WIFIMODE
    {
    WIFI_STATION = 1,
    WIFI_AP,
    WIFI_AP_STATION
    };


/**
 *@brief Esp8266模块初始化
 *@return 成功：0 失败：-1
 */
int Esp8266Init(void);

/**
 *@brief Esp8266模块发送密令
 *@param cmd:发送的密令字符串
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendCmd(const char *cmd,const char *ack);

/**
 *@brief Esp8266模块发送数据
 *@param cmd:发送的数据字符串,此时不需要添加回车
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendData(const char *data,const char *ack);

/**
 *@brief Esp8266模块重启
 *@return 成功：0 失败：-1
 */
int Esp8266Reseat(void);

/**
 *@brief Esp8266模块应用模式
 *@param mode：wifi模式参数
 *@return 成功：0 失败：-1
 */
int Esp8266SetMode(enum WIFIMODE mode);

/**
 *@brief Esp8266模块设置路由器
 *@param ssid:路由器名 password:密码
 *@return 成功：0 失败：-1
 */
int Esp8266SetRouter(const char *ssid,const char *password);

/**
 *@brief Esp8266模块IP获取
 *@param 存放IP地址
 *@return 成功：0 失败：-1
 */
int Esp8266GetIPAddr(char *ip);

/**
 *@brief Esp8266模块连接到服务器
 *@param type:通信协议类型 ip:服务器ip地址 port服务器端口
 *@return 成功：0 失败：-1
 */
int  Esp8266ConnectServer(const char *type,const char *ip,const unsigned int port);


/**
 *@brief Esp8266模块设置开启透传模式
 *@param 无
 *@return 成功：0 失败：-1
 */
int Esp8266SetTransMode(void);

/**
 *@brief Esp8266模块开始透传
 *@param 无
 *@return 成功：0	失败：-1
 */
int Esp8266StartTransmission(void);

#endif  /* _ESP8266_H */

