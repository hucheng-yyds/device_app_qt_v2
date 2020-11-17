/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.h
 * @brief   Generic radio handlers
 *
 * @version 1.1
 * @date    Feb 08 2017
 * @author  CMOSTEK R@D
 */
 
#ifndef __MCU_H
#define __MCU_H
#include "types.h"
//#include "radio.h"
#include "sky1311t.h"

#define VERSION_NUM	59
#define COMMUNICATE_CRC_ADD  	0//0xA59F

//#define SUPPORT_CONCENTRA
/*
#define BYTE  unsigned char
#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define DWORD unsigned long
*/
/*
typedef unsigned char  u8;
typedef unsigned char  BYTE;
typedef unsigned short  u16;
typedef unsigned long  u32;
typedef unsigned long  DWORD;
*/

#define _EOS_ '\0' 

//#include "stm32f0xx_hal_gpio_ex.h"
//#include "stm32f0xx_hal_gpio.h"
/*#define EVENT_DEVICE_ERROR	4
#define EVENT_CHARGE_FINISH	3
#define EVENT_CHARGING		2
#define EVENT_IDLE			1
#define EVENT_NB_FAIL		0
*/

#define 	T_F_5MS     0x01
#define     T_5MS   	0x02
#define     T_100MS   	0x04
#define     T_1000MS 	0x08
#define     T_2000MS 	0x10
#define     T_5000MS 	0x20
#define     T_60S 		0x40

#define EC20_IDLE								0
#define EC20_DELAY_START						1
#define EC20_AT								2
#define EC20_CARD_STATUS						3
#define EC20_CREG							4	
#define EC20_CGREG								6
#define EC20_QICSGP							7
#define EC20_QIACT							8
#define EC20_ATV1						9
#define EC20_NET_CONNECT								10
#define EC20_NET_SUCC   				11
#define EC20_QI_SEND							12
#define EC20_SEND_DATA						13
#define EC20_SEND_DATA_WAIT					14
#define EC20_SEND_START						20
#define EC20_SEND_CHECK						21
#define EC20_SENDING							22
#define EC20_SEND_WAIT						23
#define EC20_GET								24
#define EC20_USER								25
#define EC20_SECRET							26

/*
#define communicateUartDataMax 200
#define communicateUartDataMax1 200

typedef struct _CommunicateType{
    volatile BYTE ucGetFinishFlg;
    volatile BYTE CommunicateUartData[communicateUartDataMax];
    u8 CommunicateValidUartData[communicateUartDataMax1];
    volatile u8 uDataLen;
    u8 uDataLenBk;
    u8 uDataValidLen;
    u8 mac[9];
    u8 packId[6];
}CommunicateType;
*/
typedef struct _Hlw8032Type
{
  u8  uStatusReg;
  u8  uDetectionReg;
  u32 uVParaReg;
  u32 uVReg;
  u32 uIParaReg;
  u32 uIReg;
  u32 uWParaReg;
  u32 uWReg;
  u8  uDataUpdate;
  u16 uPFData;
  u8  uCheckSum;
}Hlw8032Type;
typedef struct _DeviceType
{
  u8 uDeviceInsideTempera;
  u8 uDeviceOutsideTempera;
  u8 uDeviceOutsideFire;
  u8 u1Status;
  u8 u1I;
  u8 u1IWAuto; //使用电流，功率，自动
  u16 u1RemainingTime;
  u16 u1RemainingW;
  u16 u1OffI;
  u16 u1DelayCheckI;
  u8 u2Status;
  u8 u2I;
  u8 u2IWAuto; //使用电流，功率，自动
  u8 u2RemainingTime;
  u8 u2RemainingW;
  u8 u2OffI;
  u8 u2DelayCheckI;
  u8 uDevice4GSignal;
  u8 uDeviceNetworkingSignal;
  u8 uDelayCheckITime;
  u8 temp1;
  u8 temp2;
}DeviceType;

//ParaReg

extern void timePro();
extern void ledPro();
extern void IsrMcu1msTimeBase();
//extern void tempPro();
extern void RFInit();
extern void RFRxMode();
extern unsigned char RFRevData( unsigned char *buf );
extern unsigned char RFSendData( unsigned char *buf, unsigned char size );
extern void hlw8032Pro();
extern void Uart1Send(u8 uData);
extern void Uart2Send(u8 uData);
extern void PrintBuff(BYTE* pucBuff, WORD ucLen);
extern void UartPutStr(BYTE *pFmt);
extern void keyPro();
extern void eventPro();
extern void McuDelayMs(u16 dwMs);
extern void chipIdGet();
extern void cmt2300Pack(u8 *uData,u8 uCmd);
extern void ec20Pro();
void checkSelfPro();
//#define Printf(x, y)			UartPrintf(x, y)
void SoftReset(void);

#define LED_RED_ON				1
#define LED_RED_ON_OFF			2
#define LED_GREEN_ON			4
#define LED_GREEN_ON_OFF		8
#define LED_RED_FLASH_TWO		16
#define LED_RED_FLASH_THR		32



#define ADD_DEFAULT_DATA		0x5a
#define ADD_DEFAULT				0
#define ADD_DELAY_CHECK_I_TIME	1
#define ADD_IP0		23
#define ADD_IP1		24
#define ADD_IP2		25
#define ADD_IP3		26
#define ADD_PORT0	27
#define ADD_PORT1	28

#define EVENT_CHARGING_DEVICE_FAULT			250
#define EVENT_CHARGING_DELAY_CHECK_I_TIME	6
#define EVENT_CHARGING_OFF_I	5
#define EVENT_CHARGING_OFF		4
#define EVENT_CHARGING_AUTO		3
#define EVENT_CHARGING_W		2
#define EVENT_CHARGING_TIME		1

#define communicateUartDataMax 250
#define communicateUartDataMax1 240
typedef struct _CommunicateType{
    volatile BYTE ucGetFinishFlg;
    volatile BYTE CommunicateUartData[communicateUartDataMax];
    BYTE CommunicateValidUartData[communicateUartDataMax1];
    volatile WORD wDataLen;
    WORD wDataLenBk;
    volatile WORD wData;
    u8 uDataValidLen;
	u8 mac[9];
}CommunicateType;

#define COMMUNICATE_COMMOD_ASK						11
#define COMMUNICATE_COMMOD_OPEN_RECORD				12
#define COMMUNICATE_COMMOD_KEY						13
#define COMMUNICATE_COMMOD_OPEN						21
#define COMMUNICATE_COMMOD_UPDATA_USER				22
#define COMMUNICATE_COMMOD_MODIFY_RELAY_TIME		23
#define COMMUNICATE_COMMOD_LOCK_STATUS				24
#define COMMUNICATE_COMMOD_LOCK_STATUS_CONTINUE		25
#define COMMUNICATE_COMMOD_UNLOCK_STATUS_CONTINUE	26
#define COMMUNICATE_COMMOD_POWER_OFF_ON				27
#define COMMUNICATE_COMMOD_UART1					31
#define COMMUNICATE_COMMOD_UART3					32
#define COMMUNICATE_COMMOD_TIME_ON_OFF				34
#define COMMUNICATE_COMMOD_UPDATA					35
#define COMMUNICATE_COMMOD_MCU_TO_WIEGAND			36
#define COMMUNICATE_COMMOD_WEIGAND_TO_MCU			37
#define COMMUNICATE_COMMOD_M1_UUID					44
#define COMMUNICATE_COMMOD_M1_READ					45
#define COMMUNICATE_COMMOD_M1_WRITE					46
#define COMMUNICATE_COMMOD_BLUE						50
#define COMMUNICATE_COMMOD_MCU_WEIGAND_OUT			51						
#define COMMUNICATE_COMMOD_MCU_WEIGAND_HIGH_LOW		52						
#define COMMUNICATE_COMMOD_MCU_WEIGAND_CHECK		53						

void CommunicateM1UUID(Tag_Type uType,u8 *uUUID);

#endif
