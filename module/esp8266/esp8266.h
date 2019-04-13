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
    WIFI_AP_STATION,
    WIFI_ERROR = -1
    };

//版本信息
struct VERSION_INFO
    {
    char ATVersion[64];     // AT Version
    char SDKVersion[64];    // SDK Version
    char ComTime[64];       // Compile time
    };

/**
 *@brief Esp8266模块初始化
 *@return 成功：0 失败：-1
 */
int Esp8266Init(void);

/**
 *@brief Esp8266模块关闭
 *@return 成功：0 失败：-1
 */
int Esp8266Close(void);

/**
 *@brief Esp8266模块发送密令
 *@param cmd:发送的密令字符串
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendCmd(const char *cmd,const char *ack);

/**
 *@brief Esp8266模块重启
 *@return 成功：0 失败：-1
 */
int Esp8266Reseat(void);

/**
 *@brief Esp8266查看版本信息
 *@return 成功：版本信息 失败：null
 */
struct VERSION_INFO* Esp8266CheckVersion(void);

/**
 *@brief Esp8266回显功能设置
 *@return 成功：0 失败：-1
 */
int Esp8266SetReturnDisplay(unsigned char);

/**
 *@brief Esp8266恢复出厂设置
 *@return 成功：0  失败：-1
 */
int Esp8266ResetFactoryData(void);

/**
 *@brief Esp8266设置串口参数
 *@return 成功：0  失败：-1
 */
int Esp8266SetUart(unsigned int,unsigned char,
                   unsigned char,unsigned char,unsigned char);

/**
 *@brief Esp8266模块应用模式
 *@param mode：wifi模式参数
 *@return 成功：0 失败：-1
 */
int Esp8266SetMode(enum WIFIMODE mode);

/**
 *@brief Esp8266查询使用模式
 *@return 成功：返回模式 失败：-1
 */
enum WIFIMODE Esp8266CheckMode(void);

/**
 *@brief Esp8266模块设置路由器
 *@param ssid:路由器名 password:密码
 *@return 成功：0 失败：-1
 */
int Esp8266SetRouter(const char *ssid,const char *password);

/**
 *@brief Esp8266查看连接路由器
 *@return 成功:返回选择的AP 失败：NULL
 */
char* Esp8266CheckConnectRouter(void);

/**
 *@brief Esp8266退出与路由器的AP连接
 *@return 成功：0 失败：-1
 */
int Esp8266QuitRouterAPConnect(void);

/**
 *@brief Esp8266设置DHCP
 *@param mode:模式 en:使能控制
 *@desc  mode = 0:AP  mode = 1:STA  mode =2:AP_STA
 *@desc  en = 0:去能 en = 1:使能
 *@return 成功：0 失败：-1
 */
int Esp8266SetDHCP(unsigned char mode,unsigned char en);

/**
 *@brief Esp8266设置STA开机自动连接
 *@param en:使能控制
 *@return 成功：0 失败：-1
 */
int Esp8266SetSTAAutoConnect(unsigned char en);

/**
 *@brief Esp8266模块发送数据
 *@param cmd:发送的数据字符串,此时不需要添加回车
 *@param ack:期待的应答结果，为空，则表示不需要等待应答
 *@return 成功：0 失败：-1
 */
int Esp8266SendData(const char *data,const char *ack);

/**
 *@brief Esp8266模块IP获取
 *@param 存放IP地址
 *@return 成功：IP地址 失败：NULL
 */
char* Esp8266GetLocalIPAddr(void);

/**
 *@brief Esp8266模块连接到服务器
 *@param type:通信协议类型 ip:服务器ip地址 port服务器端口
 *@return 成功：0 失败：-1
 */
int  Esp8266ConnectServer(const char *type,const char *ip,const unsigned int port);

/**
 *@brief Esp8266模块设置开启透传模式
 *@param mode：0：关闭透传   1：开启透传
 *@return 成功：0 失败：-1
 */
int Esp8266SetTransMode(int mode);

/**
 *@brief Esp8266模块开始透传
 *@param 无
 *@return 成功：0 失败：-1
 */
int Esp8266StartTransmission(void);

/**
 *@brief Esp8266退出透传模式
 *@param 无
 *@return 无
 */
void Esp8266QuitTransmission(void);

/**
 *@brief Esp8266断开TCP或者UDP连接
 *@param 无
 *@return 成功：0 失败：-1
 */
int Esp8266DisableConnect(void);

/**
 *@brief Esp8266模块ping功能
 *@param ip:IP地址
 *@return 成功：0 失败：-1
 */
int Esp8266PingFeture(const char *ip);

#endif  /* _ESP8266_H */

