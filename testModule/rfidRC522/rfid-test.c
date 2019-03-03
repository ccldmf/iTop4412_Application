/**
 * @brief
 * @file     rfid-test.c
 * @author   cc
 * @version
 * @date     Wed 28 Feb 2018 05:03:34 AM PST
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <string.h>
#include "rfid-rc522.h"

int main(int argc,char **argv)
    {
    unsigned char UID[5], Temp[4];
    unsigned char WriteBuff[16],ReadBuff[16];
    unsigned char Key[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    unsigned long num = 0;
    int i;
    int ret;
    int rc522_handle;
    ret = OpenRc522Dev();				//打开设备
    if(ret != 0)
        {
        printf("Open Rc522 Dev Error\n");
        return -1;
        }

    ret = Rc522Init();                  //Rc522模块初始化
    if(ret != 0)
        {
        printf("Rc522 Init Error\n");
        return -1;
        }

    while(1)
        {
        rc522_handle = getchar();
        if(rc522_handle == 'A')             //进入找卡，直到找到一次
            {

            while(1)
                {
                if(Rc522Request(0x52,Temp) == MI_OK)    //寻卡
                    {
                    if(Temp[0]==0x04 && Temp[1]==0x00)
                        printf("MFOne-S50\n");
                    else if(Temp[0]==0x02 && Temp[1]==0x00)
                        printf("MFOne-S70\n");
                    else if(Temp[0]==0x44 && Temp[1]==0x00)
                        printf("MF-UltraLight\n");
                    else if(Temp[0]==0x08 && Temp[1]==0x00)
                        printf("MF-Pro\n");
                    else if(Temp[0]==0x44 && Temp[1]==0x03)
                        printf("MF Desire\n");
                    else
                        printf("Unknown\n");

                    if(Rc522Anticoll(UID) == MI_OK)		//防冲突
                        {
                        printf("Card Id is(%d):", num++);
                        for(i=0; i<4; i++)
                        printf("%x", UID[i]);
                        printf("\n");
                        if(Rc522Select(UID) != MI_OK)		//选卡
                            {
                            printf("Rc522Select error\n");
                            }
                        else
                            {
                            printf("Rc522Select ok\n");
                            }

                        if(Rc522AuthState(PICC_AUTHENT1A,1,Key,UID) != MI_OK)   //验证卡片密码
                            {
                            printf("Rc522AuthState error\n");
                            }
                        else
                            {
                            printf("Rc522AuthState ok\n");
                            }
                        memcpy(WriteBuff,"hellohellohello",16);

                        if(Rc522WriteBlock(1,WriteBuff) != MI_OK)               //向块中写入数据
                            {
                            printf("Rc522ReadBlock error\n");
                            }

                        if(Rc522ReadBlock(1,ReadBuff) != MI_OK)                 //读取块中数据
                            {
                            printf("Rc522ReadBlock error\n");
                            }
                        else
                            {
                            printf("Rc522ReadBlock ok\n");
                            printf("Read Str:%s\n",ReadBuff);
                            }

                        Rc522Request(0x52,Temp);//clear

                        Rc522Halt();                                            //命令卡片进入休眠模式
                        break;                                                  //跳出继续找卡
                        }
                    else
                        {
                        printf("no serial num read\n");
                        }
                    }
                else
                    {
                    printf("No Card!\n");
                    }

                usleep(300000);
                }
            }
        }
    return 0;
    }
