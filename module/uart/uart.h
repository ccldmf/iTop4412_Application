/**
 * @brief
 * @file     uart.h
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 06:30:58 AM PDT
 * @mail     13568859409@163.com
 * @note
 */

/**
 *@brief 串口初始化
 *@param devname;设备名 nSpeed:波特率 nBits:数据位
 *@param nEvent:奇偶校验位 nStop:停止位
 *@return 成功：0 失败：-1
 */
int UartInit(const char *devname,int nSpeed,int nBits,char nEvent,int nStop);

/**
 *@brief 串口数据发送
 *@param data:数据 len:数据长度
 *@return 成功：0 失败：-1
 */
int UartSendData(const char *data,int len);

/**
 *@brief 串口接收数据
 *@param data:接收数据存放的空间  len:需要接收的数据长度
 *@return 返回实际接收到的数据长度
 */
int UartRecvData(char *data,int len);

/**
 *@brief 串口关闭
 */
void UartClose(void);
