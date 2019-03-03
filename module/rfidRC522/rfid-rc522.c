/**
 * @brief
 * @file     rfid-rc522.c
 * @author   cc
 * @version
 * @date     Tue 27 Feb 2018 04:37:38 AM PST
 * @mail     13568859409@163.com
 * @note
 */

#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "rfid-rc522.h"


//RC522模块文件描述符
static int g_rc522_dev_fd = -1;

/**
 *@brief RC522写数据到指定寄存器
 *@return 写入数据的长度
 */
static int RC522Write(int addr,int data)
    {
    int ret;
    unsigned char  TxBuf[2];

    //bit7:MSB=0,bit6~1:addr,bit0:RFU=0
    TxBuf[0] = ((unsigned char)addr << 1)&0x7E;
    //TxBuf[0] &= 0x7E;

    TxBuf[1] = (unsigned char)data;

    ret = write(g_rc522_dev_fd, TxBuf, 2);
    if (ret < 0)
        printf("spi:SPI Write error\n");
    usleep(10);
    return ret;
    }

/**
 *@brief RC522读取寄存器数据
 *@return 读取寄存器的数据
 */
static char RC522Read(int addr)
    {
    int ret;
    unsigned char  ReData;
    unsigned char  Address;

    Address  = (unsigned char)addr << 1;
    Address |= (1 << 7);
    Address &= ~(1 << 0);

    ret = write(g_rc522_dev_fd, &Address, 1);
    if (ret < 0)
        printf("spi:SPI Write error\n");
    usleep(100);
    ret = read(g_rc522_dev_fd, &ReData, 1);
    if (ret < 0)
        printf("spi:SPI Read error\n");

    return ReData;
    }

/**
 *@brief 设置RC522寄存器位
 *@param reg:寄存器地址 mask:置位值
 */
static void RC522SetBitMask(unsigned char reg,unsigned char mask)
    {
    char tmp = 0x0;
    tmp = RC522Read(reg) | mask;
    RC522Write(reg,tmp | mask);
    }

/**
 *@brief 清除RC522寄存器位
 *@param reg:寄存器地址 mask:清位值
 */
static void RC522ClearBitMask(unsigned char reg,unsigned char mask)
    {
    char tmp = 0x0;
    tmp = RC522Read(reg)&(~mask);
    RC522Write(reg,tmp);
    }

/**
 *@brief 通过RC522和ISO14443卡通讯
 *@param Command:RC522命令字
 *       pInData[IN]:通过RC522发送到卡片的数据
 *       InLenByte[IN]:发送数据的字节长度
 *       pOutData[OUT]:接收到的卡片返回数据
 *       *pOutLenBit[OUT]:返回数据的位长度
 *@return
 */
static char RC522ComMF522(unsigned char Command,unsigned char *pInData,unsigned char InLenByte,unsigned char *pOutData,unsigned int *pOutLenBit)
    {
    char status = MI_ERR;
    unsigned char irqEn  = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int  i;

    switch (Command)
    {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    RC522Write(ComIEnReg, irqEn|0x80);
    RC522ClearBitMask(ComIrqReg, 0x80);
    RC522Write(CommandReg, PCD_IDLE);
    RC522SetBitMask(FIFOLevelReg, 0x80);        // 清空FIFO
    for(i=0; i<InLenByte; i++)
        RC522Write(FIFODataReg, pInData[i]);    // 数据写入FIFO

    RC522Write(CommandReg, Command);            // 命令写入命令寄存器

    if(Command == PCD_TRANSCEIVE)
        RC522SetBitMask(BitFramingReg,0x80);    // 开始发送

    i = 6000; //根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
    n = RC522Read(ComIrqReg);
    i--;
    }while((i!=0)&&!(n&0x01)&&!(n&waitFor));

    RC522ClearBitMask(BitFramingReg, 0x80);

    if(i!=0)
    {
    if(!(RC522Read(ErrorReg) & 0x1B))
        {
        status = MI_OK;
        if (n&irqEn&0x01)
        status = MI_NOTAGERR;
        if(Command == PCD_TRANSCEIVE)
            {
            n = RC522Read(FIFOLevelReg);

            lastBits = RC522Read(ControlReg) & 0x07;
            if(lastBits)
                *pOutLenBit = (n-1)*8 + lastBits;
            else
                *pOutLenBit = n*8;

            if(n == 0)
                n = 1;
            if(n>MAXRLEN)
                n = MAXRLEN;

            for (i=0; i<n; i++)
                pOutData[i] = RC522Read(FIFODataReg);
            }
        }
    else
        {
        status = MI_ERR;
        }
    }

    RC522SetBitMask(ControlReg, 0x80);// stop timer now
    RC522Write(CommandReg, PCD_IDLE);

    return status;
    }

/**
 *@brief RC522设备打开
 *@return 成功：0 失败：-1
 */
int OpenRc522Dev(void)
    {
    g_rc522_dev_fd = open(RC522_DEV,O_RDWR);
    if(g_rc522_dev_fd < 0)
        {
        printf("open %s error\n",RC522_DEV);
        return -1;
        }
    return 0;
    }

/**
 *@brief RC522开启天线
 */
static void Rc522AntennaOn(void)
    {
    unsigned char i;
    RC522Write(TxASKReg,0x40);
    usleep(20);

    i = RC522Read(TxControlReg);
    if(!(i&0x03))
        {
        RC522SetBitMask(TxControlReg,0x03);
        }
    i = RC522Read(TxASKReg);
    }

/**
 *@brief RC522模块初始化
 *@return 成功：0 失败：-1
 */
int Rc522Init(void)
    {
    int ret;
    char version = 0;

    //reset
    RC522Write(CommandReg,PCD_RESETPHASE);
    usleep(10);
    RC522Write(ModeReg,0x3D);
    RC522Write(TReloadRegL, 30);
    RC522Write(TReloadRegH, 0);
    RC522Write(TModeReg, 0x8D);
    RC522Write(TPrescalerReg, 0x3E);

    version = RC522Read(VersionReg);
    printf("Chip Version:0x%x\n",version);
    usleep(5000);

    Rc522AntennaOn();

    return 0;
    }

/**
 *@brief 寻卡
 *@param reg_code:寻卡方式 0x52 = 寻感应区内所有符合1443A标准的卡
 *                         0x26 = 寻未进入休眠的卡
 *       pTagType:卡片类型代码
 *@return 成功返回MI_OK
 */
char Rc522Request(unsigned char req_code,unsigned char *pTagType)
    {
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    RC522ClearBitMask(Status2Reg, 0x08);
    RC522Write(BitFramingReg, 0x07);
    RC522SetBitMask(TxControlReg, 0x03);

    ucComMF522Buf[0] = req_code;

    status = RC522ComMF522(PCD_TRANSCEIVE, ucComMF522Buf,
                            1, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10))
        {
        *pTagType     = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
        }
    else
        {
        status = MI_ERR;
        }
    return status;

    }

/**
 *@bfief 防冲撞
 *@param pSnr:卡片序列号
 *@return 成功：MI_OK
 */
char Rc522Anticoll(unsigned char *pSnr)
    {
    char status;
    unsigned char i, snr_check = 0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    RC522ClearBitMask(Status2Reg, 0x08);
    RC522Write(BitFramingReg, 0x00);
    RC522ClearBitMask(CollReg, 0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = RC522ComMF522(PCD_TRANSCEIVE, ucComMF522Buf,
                            2, ucComMF522Buf, &unLen);

    if(status == MI_OK)
        {
        for (i=0; i<4; i++)
            {
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
            }
        if (snr_check != ucComMF522Buf[i])
            {
            status = MI_ERR;
            }
        }

    RC522SetBitMask(CollReg,0x80);

    return status;
    }

/**
 *@brief 用于MF522计算CRC16
 *@param pIn:计算数据  len：数据长度  pOut:计算之后的数据
 */
static void CalulateCRC(unsigned char *pIn,unsigned char len,unsigned char *pOut)
    {
    unsigned char i,n;

    RC522ClearBitMask(DivIrqReg,0x04);
    RC522Write(CommandReg,PCD_IDLE);
    RC522SetBitMask(FIFOLevelReg,0x80);

    for(i = 0;i < len;i++)
        {
        RC522Write(FIFODataReg,*(pIn + i));
        }
    RC522Write(CommandReg,PCD_CALCCRC);
    i = 0xFF;
    do
    {
    n = RC522Read(DivIrqReg);
    i--;
    }while((i != 0) && !(n&0x04));

    pOut[0] = RC522Read(CRCResultRegL);
    pOut[1] = RC522Read(CRCResultRegM);
    }

/**
 *@brief 选定卡片
 *@param pSnr:卡片序列号，4字节
 *@reutrn 成功：MI_OK
 */
char Rc522Select(unsigned char *pSnr)
    {
    char status;
    unsigned char i;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;

    for(i = 0;i < 4;i++)
        {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6] ^= *(pSnr+i);
        }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    RC522ClearBitMask(Status2Reg,0x08);

    status = RC522ComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    if((status == MI_OK) && (unLen == 0x18))
        {
        status = MI_OK;
        }
    else
        {
        status = MI_ERR;
        }
    return status;
    }

/**
 *@brief 验证卡片密码
 *@param auto_mode:密码验证模式  0x60 = 验证A密钥
 *                               0x61 = 验证B密钥
 *       addr:地址块
 *       pKey:密码
 *       pSnr:卡片序列号，4字节
 *@return 成功：MI_OK
 */
char Rc522AuthState(unsigned char auto_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
    {
    char status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auto_mode;
    ucComMF522Buf[1] = addr;

    memcpy(&ucComMF522Buf[2], pKey, 6);
    memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = RC522ComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if((status != MI_OK) || (!(RC522Read(Status2Reg)&0x08)))
        {
        status = MI_ERR;
        }
    return status;
    }

/**
 *@brief 读取卡一块的数据
 *@param addr:块地址
 *       p:读出的数据,16字节
 *@return 成功：MI_OK
 */
char Rc522ReadBlock(unsigned char addr ,unsigned char *p)
    {
    char status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522ComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if((status == MI_OK) && (unLen == 0x90))
        {
        for(i = 0;i < 16;i++)
        {
        *(p + i) = ucComMF522Buf[i];
        }
        }
    else
        {
        status = MI_ERR;
        }
    return status;
    }

/**
 *@brief 写数据到卡的一块
 *@param addr:块地址
 *       p:写入的数据,16字节
 *@return 成功：MI_OK
 */
char Rc522WriteBlock(unsigned char addr ,unsigned char *p)
    {
    char status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522ComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if((status != MI_OK) || (unLen != 4) || (ucComMF522Buf[0] & 0x0F) != 0x0A)
        {
        status = MI_ERR;
        }

    if(status == MI_OK)
        {
        for(i = 0;i < 16;i++)
            {
            ucComMF522Buf[i] = *(p + i);
            }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = RC522ComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if((status != MI_OK) || (unLen != 4) || (ucComMF522Buf[0] & 0x0F) != 0x0A)
            {
            status = MI_ERR;
            }
        }
    return status;
    }

/**
 *@brief 命令卡片进入休眠状态
 *@return 成功：MI_OK
 */
char Rc522Halt(void)
    {
    unsigned char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = RC522ComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
    }

/**
 *@brief 关闭RC522设备
 *@return 成功：0 失败：-1
 */
void Rc522CloseDev(void)
    {
    close(g_rc522_dev_fd);
    }
