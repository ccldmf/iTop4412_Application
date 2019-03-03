/**
 * @brief
 * @file     bigiot.h
 * @author   cc
 * @version
 * @date     Wed 27 Jun 2018 01:02:14 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

#ifndef _BIGIOT_H
#define _BIGIOT_H

//ID类型
enum IDTYPE
    {
    DEVICE_ID,      //设备ID
    USER_ID,        //用户ID
    TOURISTS_ID     //游客ID
    };


/**
 *@brief 设备登录
 *@param id:设备ID apikey:设备apikey
 *@return 无
 */
void BigiotLogin(const char *id,const char *apikey);

/**
 *@brief 发送数据
 *@param id:设备id	data_id:数据接口ID value:值
 *@return 无
 */
void BigiotSendData(const char *id,const char *data_id,const char *value);

/**
 *@brief 接收数据
 *@param data:存放数据空间  len:读取数据的长度
 *@return 成功：返回实际接收到的数据长度  失败：-1
 */
int BigiotRecvData(char *,int len);

/**
 *@brief 查询设备或者用户是否在线
 *@param id:设备或者用户的id type:查询类型，分为设备、用户、游客
 *@return 无
 */
void BigiotIsOnLine(const char *id,enum IDTYPE type);

/**
 *@brief 查询当前设备状态
 *@param 无
 *@return 无
 */
void BigiotStatus(void);

/**
 *@brief 查询服务器时间
 *@param 无
 *@return 无
 */
void BigiotTime(void);

/**
 *@brief 发送报警信息
 *@param message:报警信息 type：报警方式
 *@return 无
 */
void BigiotAlertMessage(const char *,const char *);

/**
 *@brief 设备下线
 *@param id:设备id  apikey:设备apikey
 *@return 无
 */
void BigiotLogout(const char *,const char *);

#endif
