 
#include "mcu.h"
//#include "cmt2300.h"

//#include "Delay.h"
#include "Crc16.h"
#include <string.h>
#include "nb.h"

#include "sky1311t.h"

unsigned char timeFlg=0;           
unsigned char Time2MsBuff=0;                
unsigned char Time50MsBuff=0;        
unsigned char Time80MsBuff=0;        
unsigned char Time110MsBuff=0;       
unsigned char Time1000MsBuff=0;      
unsigned char Time2000MsBuff=0;      
unsigned char Time60sBuff=0;     
static volatile DWORD gdw1msCounter;

static u32 CpuID;
u8 chipId[12]={0x01,0x23,0x45,0x67,0x89,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
//extern DeviceType deviceInfo;
DeviceType deviceInfo;
u8 getData[64];
u8 checkSelfFlg=0;
u8 checkSelfCnt=0;
u8 checkSelfBuff=0;
u8 checkSelfACnt=0;
u8 checkSelfBCnt=0;

//extern unsigned long electricity;

void cmt2300Pack(u8 *uData,u8 uCmd)
{
	u8 i=0;
	WORD uCrc;
	for(i=0;i<16;i++)
	{
		*(uData+14+i) = *(uData+i);
	}
	for(i=0;i<12;i++)
	{
		*(uData+i) = chipId[i];
	}
	*(uData+12) = 19;
	*(uData+13) = uCmd;
	uCrc = GetCrc16Ccitt(0,uData,30);
	*(uData+30) = (u8)(uCrc>>8);
	*(uData+31) = (u8)(uCrc);
}

void chipIdGet()
{

}

void IsrMcu1msTimeBase(){
	Time2MsBuff ++;
	if(Time2MsBuff >= 5)
	{
		Time2MsBuff = 0;
		timeFlg |= 0x01;
	}
	if(gdw1msCounter)
	{
		gdw1msCounter --;
	}
}
void McuDelayMs(u16 dwMs){
/*	gdw1msCounter= dwMs;
	while (gdw1msCounter)
	{
	}*/
	usleep(dwMs*1000);
}

u8 keyFlg=0;
u8 keyBuff=0;
u8 testFlg=0;
void keyPro()
{

}


void timePro()
{
	if(timeFlg&T_5MS)
	{
		timeFlg&=~T_5MS;
	}
	if(timeFlg&T_100MS)
	{
		timeFlg&=~T_100MS;
	}
	if(timeFlg&T_1000MS)
	{
		timeFlg&=~T_1000MS;
	}
	if(timeFlg&T_2000MS)
	{
		timeFlg&=~T_2000MS;
	}
	if(timeFlg&T_5000MS)
	{
		timeFlg&=~T_5000MS;
	}
	if(timeFlg&T_60S)
	{
		timeFlg&=~T_60S;
	}
	if((timeFlg&T_F_5MS)==0)
	{
		return;
	}
	timeFlg|=T_5MS;
	timeFlg&=~T_F_5MS;
	
	Time50MsBuff ++;
	if(Time50MsBuff >= 20)
	{
		Time50MsBuff = 0;
		timeFlg |= T_100MS;
	}

	if(timeFlg&T_100MS)
	{
		Time80MsBuff ++;
		if(Time80MsBuff >= 10)
		{
			Time80MsBuff = 0;
			timeFlg |= T_1000MS;
		}
	}	
	if(timeFlg&T_100MS)
	{
		Time1000MsBuff ++;
		if(Time1000MsBuff >= 20)
		{
			Time1000MsBuff = 0;
			timeFlg |= T_2000MS;
		}
	}	
	if(timeFlg&T_100MS)
	{
		Time2000MsBuff ++;
		if(Time2000MsBuff >= 50)
		{
			Time2000MsBuff = 0;
			timeFlg |= T_5000MS;
		}
	}	
	if(timeFlg&T_1000MS)
	{
		Time60sBuff ++;
		if(Time60sBuff >= 60)
		{
			Time60sBuff = 0;
			timeFlg |= T_60S;
		}
	}
}
/*
（1）闪红色：表示故障/不能使用；
（2）常亮红色：表示正常/空闲；
（3）闪绿色：充电中；
（4）常亮绿色：充电已经完成；【注意：（未进行充电）插头插上和拨下能否检测到？？如果不能检测到，绿灯常亮和红灯常亮会冲突】
*/

unsigned char ledFlg=LED_RED_ON_OFF;
unsigned char led1Flg=LED_RED_ON_OFF;
unsigned char ledBuff=0;
unsigned char led1Buff=0;
extern unsigned char nbErrorBuff;

void ledPro()
{

}
void Uart1Send(u8 uData)
{
}
void Uart2Send(u8 uData)
{
}


void UartPutChar(u8 uData)
{
}
void UartPutStr(BYTE *pFmt){
	BYTE ucBff; // character buffer
	while (1){
		ucBff = *pFmt; // get a character
		if (ucBff == _EOS_) // check end of string
			break;
		UartPutChar(ucBff); // put a character
		pFmt++; // next
	} // while
}

void PrintBuff(BYTE* pucBuff, WORD ucLen)
{
	WORD ucCnt, ucCntR= 0;
	if (ucLen> 0){
		printf((BYTE*)"Data[0-%d]:\r\n", ucLen-1);
		for (ucCnt=0; ucCnt< ucLen; ucCnt++){
			printf((BYTE*)"%x ", pucBuff[ucCnt]);
			if (++ucCntR> 15){
				ucCntR= 0;
				UartPutStr("\r\n");
			}
		}
		if (ucCntR){
			UartPutStr("\r\n");
		}
	}
	else{
		UartPutStr("No Data\r\n");
	}
}
/*
void UartPrintf(UartDbgType* pDbg, BYTE *pFmt, DWORD dwValue){
	BYTE ucBff, ucDisp;
	BOOL bNotZero=FALSE, bHex=FALSE;
	DWORD dwDivider=1000000000;
	while(*pFmt){
		ucBff = *pFmt;
		pFmt++;
		if(ucBff == '%'){ // check special case
			switch(*(pFmt++)){ // check next character
				case '%':
					UartPutChar(pDbg, '%');
					break;
				case 'x': // hexadecimal number
				case 'X':
					dwDivider = 0x10000000;
					bHex = TRUE;
				case 'd': // decimal number
				case 'i':
				if(dwValue){
					while(dwDivider){
						ucDisp = (BYTE)(dwValue/dwDivider);
						dwValue = dwValue%dwDivider;
						if(ucDisp)
							bNotZero=TRUE;
						if(bNotZero){
							if (ucDisp>9)
								UartPutChar(pDbg, (BYTE)(ucDisp-0x0A+ 'A'));
							else
								UartPutChar(pDbg, (BYTE)(ucDisp+ '0'));
						}
						if(bHex)
							dwDivider/= 0x10;
						else
							dwDivider /= 10;
					}
				}
				else
					UartPutChar(pDbg, '0');
				break;
			} // switch
		}
		else // general
			UartPutChar(pDbg, ucBff); // put a character
	} // while
}
*/

//#define EVENT_PUBLIC_FREQ		1
u8 eventFlg=0;
u8 eventBuff=0;
u8 event1Flg=0;
u8 event1Buff=0;



void event2ControlAutoPro()
{
}

void event2ControlWPro()
{}


void event2ControlTimePro()
{}
void event2CheckIPro()
{}

void event2Charging()
{}

void event1ControlAutoPro()
{
}


void event1ControlWPro()
{}
void event1ControlTimePro()
{}

void event1CheckIPro()
{}

void event1Charging()
{}

void eventPro()
{}
void SoftReset(void)
{
}
CommunicateType gCommunicate;
void CommunicateSend(u16 Length,u8 *Buffer)//发送字符串
{
  	BYTE uLen=Length,i=0;
	while(uLen != 0)
	{
          //McuCommunicatePutChar(*(Buffer+i++));
		  Uart1Send(*(Buffer+i++));
          uLen--;
	}
}
u8 uartDataTemp[100];
void sendPack(u8 uCommond,u8 *uData,u8 uLength)
{
	u8 i = 0;
	u16 uCrc=0;
	uartDataTemp[0] = 0xD2;
	uartDataTemp[1] = 0xCF;
	uartDataTemp[2] = uLength + 4;
	uartDataTemp[3] = VERSION_NUM;
	uartDataTemp[4] = uCommond;
	for(i=0;i<uLength;i++)
	{
		uartDataTemp[5+i] = *(uData+i);
	}
	uCrc = GetCrc16Ccitt(COMMUNICATE_CRC_ADD,uartDataTemp,5+uLength);//0xA5 9F 73 B4
	uartDataTemp[5+uLength] = (u8)(uCrc>>8);
	uartDataTemp[6+uLength] = (u8)(uCrc);	
	CommunicateSend(uLength+7,uartDataTemp);
}
void memcpyStr(u8 *uData1,u8 *uData2,u8 uLength)
{
	u8 i=0;
	for(i=0;i<uLength;i++)
	{
		*(uData1+i) = *(uData2+i);
	}
}

void CommunicateM1UUID(Tag_Type uType,u8 *uUUID)
{
	gCommunicate.CommunicateValidUartData[0] = 0;
	gCommunicate.CommunicateValidUartData[1] = uType;
	memcpyStr(&gCommunicate.CommunicateValidUartData[2],uUUID,8);
	sendPack(COMMUNICATE_COMMOD_M1_UUID,&gCommunicate.CommunicateValidUartData[0],10);
}







