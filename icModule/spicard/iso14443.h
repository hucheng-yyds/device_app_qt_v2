/******************************************************************************/
/*               (C) 斯凯瑞利（北京）科技有限公司(SKYRELAY)                   */
/*                                                                            */
/* 此代码是斯凯瑞利（北京）科技有限公司为支持客户编写的示例程序的一部分       */
/* 所有使用斯凯瑞利相关芯片的用户可以无偿使用该代码，但需保留本声明部分       */
/* 本代码仅为演示使用，不保证符合所有的应用要求，当有特定规范或需求时，       */
/* 使用者需自行确认代码是否符合要求，不合要求时自行做出相应的修改。           */
/* (V1.00)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** 文 件 名： iso14443.h
 **
 ** 文件简述： 与ISO14443以及M1卡有关的操作函数，变量以及常量定义
 **
 ** 版本历史:
 ** 2015-10-05 V1.00  Mark   正式版本
 **
 ******************************************************************************/
#ifndef __ISO14443_H_
#define __ISO14443_H_

#include "sky1311t.h"
#define false			 FALSE

 /**
 ******************************************************************************
 ** \简述：全局预定义
 **
 ******************************************************************************/
#define     SEL1            0x93
#define     SEL2            0x95
#define     SEL3            0x97

#define     REQA            0x26
#define     WUPA            0x52
#define     SELECT          0x90
#define     HALTA           0x50
#define     PATS            0xE0
#define     PPS             0xD0

#define	    APF_CODE	    0x05		// REQB命令帧前缀字节APf
#define	    APN_CODE	    0x05		// REQB命令帧前缀字节APn
#define	    APC_CODE	    0x1D		// ATTRIB命令帧前缀字节APC
#define	    HALTB_CODE	    0x50		// 挂起命令

#define     REQIDEL		    0x00		// IDLE
#define     REQALL		    0x08		// ALL

#define     M1_AUTH         0x60
#define     M1_AUTH_KEYA    0x60
#define     M1_AUTH_KEYB    0x61
#define     M1_READ         0x30
#define     M1_WRITE        0xA0
#define     M1_INCREMENT    0xC1
#define     M1_DECREMENT    0xC0
#define     M1_RESTORE      0xC2
#define     M1_TRANSFER     0xB0
//#define     M1_ACK          0xA0
#define     M1_ACK          0x0A



//I-block (00xxxxxx)b (not (00xxx101)b)
//R-block (10xxxxxx)b (not (1001xxxx)b)
//S-block (11xxxxxx)b (not (1110xxxx)b and not (1101xxxx)b)
/**
 ******************************************************************************
 ** \简述： 数据类型定义
 **
 ******************************************************************************/

//============= 全局变量和函数定义 ================================================================
typedef struct
{
	BYTE PUPI[4];					// 伪唯一PICC标识符
	BYTE AppDat[4];					// 应用数据
	BYTE ProtInf[3];					// 参数信息
} nAQTB_t;

 /******************************************************************************
 ** \简述： 全局变量定义
 **
 ******************************************************************************/

/**
 ******************************************************************************
 ** \简述： 函数声明
 **
 ******************************************************************************/
sta_result_t M1_3Pass(PSky1311t pSky,BYTE blockAddr, BYTE *m1Key, BYTE *uid, u32 RA);
sta_result_t M1_Authentication_Auto(PSky1311t pSky,BYTE auth_mode,BYTE *m1Key,BYTE *uid, BYTE blockAddr);
//sta_result_t M1_PwdChange(PSky1311t pSky,BYTE blockAddr, BYTE *key_a, BYTE *key_b);
//sta_result_t M1_Write(PSky1311t pSky,BYTE blockAddr, BYTE* data);
sta_result_t M1_Read(PSky1311t pSky,BYTE blockAddr, BYTE *blockBuff);
sta_result_t M1_Increment(PSky1311t pSky,BYTE blockAddr, u32 value);
sta_result_t M1_Decrement(PSky1311t pSky,BYTE blockAddr, u32 value);
sta_result_t M1_Transfer(PSky1311t pSky,BYTE blockAddr);
sta_result_t M1_Restore(PSky1311t pSky,BYTE blockAddr);

sta_result_t piccRequestA(PSky1311t pSky,BYTE *ATQA);
sta_result_t piccWakeupA(PSky1311t pSky,BYTE *ATQA);
//sta_result_t piccAntiA(BYTE rand_bit, BYTE *uid);
sta_result_t piccAntiA(PSky1311t pSky,BYTE SEL, BYTE rand_bit, BYTE *uid);
//sta_result_t piccSelectA(BYTE *uid);
sta_result_t piccSelectA(PSky1311t pSky,BYTE SEL,BYTE *uid, BYTE *SAK);
void piccHaltA(PSky1311t pSky);
sta_result_t piccDeselectA(PSky1311t pSky);

sta_result_t piccRequestB(PSky1311t pSky,BYTE ucReqCode, BYTE ucAFI, BYTE N, nAQTB_t *pATQB);
sta_result_t piccSlotMarker(PSky1311t pSky,BYTE N, nAQTB_t *pATQB);
sta_result_t piccAttrib(PSky1311t pSky,BYTE *pPUPI, BYTE ucDSI_DRI,BYTE MAX_FSDI,
				 BYTE ucCID, BYTE ucProType, BYTE *pHigherLayerINF, BYTE ucINFLen,
				 BYTE *pAATTRIB, BYTE *pRLen);
sta_result_t piccHaltB(PSky1311t pSky,BYTE *pPUPI);
sta_result_t piccDeselectB(PSky1311t pSky,BYTE *param);

sta_result_t piccATS(PSky1311t pSky,BYTE *ATS);
sta_result_t piccPPS(PSky1311t pSky,WORD high_rate, BYTE *pps);

BYTE ISO14443_3_Deal_Flow(PSky1311t pSky,BYTE *rxBuff, BYTE *rxSize);

sta_result_t ISO14443_Transcive(PSky1311t pSky,BYTE *txBuf, WORD txSize, BYTE *rxBuf, WORD *rxSize);

//

void getFSCFSD(BYTE bFSCIFSDI, BYTE bType);
#endif
