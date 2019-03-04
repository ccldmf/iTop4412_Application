/**
 * @brief
 * @file     tcp.h
 * @author   cc
 * @version
 * @date     Tue 26 Jun 2018 08:00:06 PM PDT
 * @mail     13568859409@163.com
 * @note
 */

#ifndef _TCP_H
#define _TCP_H

#define IP      "121.42.180.30"
#define PORT    8181

int TcpInit(const char *,const int);

int TcpRecvdData(char *,int len);

int TcpSendData(const char *,int len);

void TcpClose(void);

#endif


