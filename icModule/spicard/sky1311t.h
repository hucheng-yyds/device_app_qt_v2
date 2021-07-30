/******************************************************************************/
/*               (C) 斯凯瑞利（北京）科技有限公司(SKYRELAY)                   */
/*                                                                            */
/* 此代码是斯凯瑞利（北京）科技有限公司为支持客户编写的示例程序的一部分       */
/* 所有使用斯凯瑞利相关芯片的用户可以无偿使用该代码，但需保留本声明部分       */
/* 本代码仅为演示使用，不保证符合所有的应用要求，当有特定规范或需求时，       */
/* 使用者需自行确认代码是否符合要求，不合要求时自行做出相应的修改。           */
/* (V1.10)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** 文 件 名： sky1311s.h
 **
 ** 文件简述： 操作sky1311s的所有函数，常量和变量
 **
 ** 版本历史:
 ** 2015-10-05 V1.00  EH   第一个正式版本
 ** 2017-3-25  V.10  Release    更新代码和函数结构，重新发布
 **
 ******************************************************************************/
#ifndef  __SKY1311S_H_
#define  __SKY1311S_H_

#include "types.h"
#include "sky1311t_reg.h"
#include "userdef.h"

//#include "debugapp.h"
#define DrvPutStr(x)       printf(x)
#define DrvPrintf(x,y)     printf(x,y)
#define DrvPrintBuff(x,y)  printf(x,y)

#define SPI_TIMEOUT       1000//5000

#define NOCHANGE    0
#define LARGEN      1
#define LESSEN      2
#define MINIMUM     3
#define MAXIMUM     4

typedef enum sta_result
{
    sta_Ok                      = 0,  ///< 无错，操作成功
    sta_Error                   = 1,  ///< 非特定错误
    sta_Timeout                 = 3,  ///< 超时错误
    sta_RxParity			    = 4,  ///< 接收奇偶校验错
    sta_RxCheckCRC			    = 5,  ///< 接收CRC校验错
    sta_FifoFull                = 6,  ///< FIFO满
    sta_FifoEmpty               = 7,  ///< FIFO空
    sta_Collision               = 8,  ///< 防冲突错误
    sta_Framing                 = 9,  ///< 数据帧错误
    sta_UIDFormat               = 10, ///< UID格式错误
    sta_M1Error                 = 11, ///< 操作M1卡错误

    sta_ErrorRequest            = 12,  ///< 执行REQA/WUPA 命令时出错
    sta_ErrorAnticollision      = 13,  ///< 执行防冲突命令ANTICOLLISION时出错
    sta_ErrorSelect             = 14,  ///< 执行SELECT 命令时出错
    sta_ErrorAts                = 15,  ///< 执行ATS命令出错
    sta_ErrorInvalidMode            = 16,  ///< Operation not allowed in current mode
    sta_ErrorUninitialized          = 17,  ///< Module (or part of it) was not initialized properly
    sta_ErrorBufferFull             = 18,  ///< Circular buffer can not be written because the buffer is full
    sta_ErrorTimeout                = 19,  ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    sta_ErrorNotReady               = 20, ///< A requested final state is not reached
    sta_OperationInProgress         = 21,  ///< Indicator for operation in progress

    sta_UnknowError				= 0x7F,
    sta_NoResponse              = 0xFF
}sta_result_t,en_result_t;


typedef enum sta_field
{
    NoObjectIn              = 0,    ///< 读卡器场内无卡或异物
    PiccIn                  = 1,    ///< 有卡在场内
    OtherObjectIn           = 2
}sta_field_t;

extern BYTE  irq_sta;


typedef enum _Tag_Type{
  UNDEFINED_TAG_TYPE=0,
  TAG_TYPE_14443A0,     //1  UUID = 4
  TAG_TYPE_14443A1,     //2  UUID = 7
  TAG_TYPE_14443A2,     //3  CPU卡
  TAG_TYPE_14443A3,     //4  Apdu手机
  TAG_TYPE_14443B,      // 5  身份证
  TAG_TYPE_14443B_ST25TB,//6  ST25TB
  TAG_TYPE_15693,       //7  15693
  TAG_TYPE_FELICA,      //8  菲林卡
  TAG_MAX
}Tag_Type;

typedef struct _TagInfoType{
  BYTE uCardId[23];
  BYTE uCardIdLen;
  BYTE uATQA,uSAK;
  Tag_Type TagType;
}TagInfoType,*PTagInfo;

typedef struct _Sky1311tDriverConstType{
  //读卡后，回调函数
  BYTE (*ReadCallBack)(void *p,PTagInfo pTag, BYTE *uBuff, BYTE uLen);
  //Spi发送数据函数
  BYTE (*SpiSendCmd)(BYTE ucData);
  BYTE (*SpiSendByte)(BYTE ucRegAddr,BYTE ucData);
  BYTE (*SpiSendBytes)(BYTE ucRegAddr,BYTE *ucData,BYTE len);
  BYTE (*SpiGetByte)(BYTE ucRegAddr,BYTE ucData);
  BYTE (*SpiGetBytes)(BYTE ucRegAddr,BYTE *ucData,BYTE len);
  //sky1311 Pd脚 不使能（置低电平）
  void (*DisablePdPin)(void);
  //sky1311 Pd脚 使能（置高电平）
  void (*EnablePdPin)(void);
  //sky1311 Spi有效 cs脚 （置低电平）
  void (*CsPinLow)(void);
  //sky1311 Spi无效 cs脚 （置高电平）
  void (*CsPinHigh)(void);
  //sky1311 Irq脚读  到为高电平时有卡，
  HiLowType (*IrqReadPin)(void);
  //1ms延时
  void (*DelayMs)(u16 dwTime);
}Sky1311tDriverConstType,*PSky1311tDriverConst;

typedef struct _Sky1311tDriverVarType{
  WORD wRcValue,wMoveCardTimeOut;
  BYTE ucRcAdcValue,ucNoCardCnt,ucNoMoveCardCnt;
  BOOL bhasCardIn;
  BOOL bcardReadOver;
}Sky1311tDriverVarType,*PSky1311tDriverVar;

typedef struct _Sky1311tType{
  PSky1311tDriverConst pConst;
  PSky1311tDriverVar  pVar;
}Sky1311tType,*PSky1311t;


/**
 ******************************************************************************
 ** \简述： 函数声明
 **
 ******************************************************************************/
void SetBitMask(PSky1311t pSky,BYTE ucRegAddress, BYTE ucMask);
void ClearBitMask(PSky1311t pSky,BYTE ucRegAddr, BYTE ucMask);
void analogInit(PSky1311t pSky);
void directModeInit(PSky1311t pSky,BYTE ucMfout_sel);
void pcdAntennaOn(PSky1311t pSky,BYTE ucPcdType);
void pcdAntennaOff(PSky1311t pSky);
void irqClearAll(PSky1311t pSky);
void irqClear(PSky1311t pSky,BYTE ucIrq);
void bitRateSet(PSky1311t pSky,WORD wHigh_rate);
void typeAOperate(PSky1311t pSky);
void typeBOperate(PSky1311t pSky);
void sky1311Init(PSky1311t pSky);
void sky1311Reset(PSky1311t pSky);
void resetPicc(PSky1311t pSky);
void sky1311ReadAll(PSky1311t pSky);


sta_result_t bitCollisionTrans(PSky1311t pSky,BYTE* ucTxBuf, BYTE ucTxLen, BYTE ucLastBitNum);
void sky1311_fifo_tx(PSky1311t pSky,BYTE ucTxType, BYTE *ucTxBuff, WORD wTxSize);
sta_result_t sky1311_fifo_rx(PSky1311t pSky,BYTE ucRxType, BYTE ucRateType, BYTE *ucRxBuff, WORD *wRxSize);
sta_result_t ExchangeData(PSky1311t pSky,u8 cardType, u8 rateType,
                    u8 *pSendDataBuf, u16 ucSendLen,
                    u8 *pRcvDataBuf,  u16 *pRcvLen);

WORD sky1311RCFreqCali(PSky1311t pSky);
BYTE checkCardNear(BYTE ucMaxRCFreqVal);
BYTE checkCardNearXtal(PSky1311t pSky);
void checkCardInit(PSky1311t pSky,WORD wRc_val);
void checkCardDeinit(PSky1311t pSky);
void sky1311CardRemove(PSky1311t pSky,WORD wRc_val);
BOOL checkCardRemove(PSky1311t pSky,u16 rc_val);

extern void IsrSky1311Drv1msTimebase(PSky1311t pSky);
extern void sky1311WriteCmd(PSky1311t pSky,BYTE cmd);
extern void sky1311WriteReg(PSky1311t pSky,BYTE regAddress, BYTE data);
extern BYTE sky1311ReadReg(PSky1311t pSky,BYTE regAdd);
extern void sky1311WriteFifo(PSky1311t pSky,BYTE *udata, BYTE ucount);
extern void sky1311ReadFifo(PSky1311t pSky,BYTE *udata, BYTE ucount);

#endif
