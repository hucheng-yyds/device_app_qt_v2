#include <fcntl.h> 
#include <sys/ioctl.h>

#include "mcu.h"
#include "sky1311Instan.h"
//#include "debugapp.h"
//#include "Beeper.h"
//#include "touch.h"
//#include "menKouJi.h"

//#include "sleep.h"

#define GPIO_CMD_MAGE 'G'
#define GPIO_PWM0_SET     _IOW(GPIO_CMD_MAGE,0,char)
#define GPIO_PWM1_SET     _IOW(GPIO_CMD_MAGE,1,char)
#define GPIO_SET          _IOW(GPIO_CMD_MAGE,2,char)
#define BELL_GET         _IOW(GPIO_CMD_MAGE,3,char)
#define DET_GET          _IOW(GPIO_CMD_MAGE,4,char)
#define LED_PWM_SET      _IOW(GPIO_CMD_MAGE,5,char)
#define LED_PWM0_SET      _IOW(GPIO_CMD_MAGE,6,char)
#define RF_GET         _IOR(GPIO_CMD_MAGE,7,char)
#define CSN0_SET      _IOW(GPIO_CMD_MAGE,8,char)

#define LED_G_SET      _IOW(GPIO_CMD_MAGE,9,char)
#define LED_B_SET      _IOW(GPIO_CMD_MAGE,10,char)
#define RELAY_SET      _IOW(GPIO_CMD_MAGE,11,char)
#define MOTOR_SET      _IOW(GPIO_CMD_MAGE,12,char)

extern int pgpiofd;
extern int spifd_1311;
extern char getcardid;

u8 guccardStatus = CARDREMOVED;

extern sta_result_t TypeA_test(PSky1311t pSky,PTagInfo pTag);
extern sta_result_t SmartTypeB_test(PSky1311t pSky,PTagInfo pTag);
extern  void cardTest(PSky1311t pSky);
extern u8 SPI_ReadByte_LSB_1311(int fd, u8 readAddress,u8 udata);
extern int SPI_WriteByte_LSB_1311(int fd, u8 writeAddress, u8 writedata);
extern int SPI_WriteBytes_LSB_1311(int fd, u8 spireg, u8 *buf, u8 length);
extern int SPI_ReadBytes_LSB_1311(int fd, u8 spireg, u8 *buf, u8 length);
extern int SPI_WriteCmd_1311(int fd, u8 writedata);

//extern PBeeper gpBeeper;

static u8 McuReadCardCallBack(void *p,PTagInfo pTag, BYTE *uBuff, BYTE uLen){
 // printf("TagType=%d\r\n",pTag->TagType);
  //PrintBuff(pTag->uCardId,pTag->uCardIdLen);
  //BeepRunCycle(gpBeeper,1,50,10);
  return 0;
}
static u8 McuSpiSendCmd(u8 ucData)
{
    SPI_WriteCmd_1311(spifd_1311,ucData);

}
static u8 McuSpiSendByte(u8 ucRegAddr,u8 ucData)
{
    SPI_WriteByte_LSB_1311(spifd_1311,ucRegAddr,ucData);

}
static u8 McuSpiSendBytes(u8 ucRegAddr,u8 *ucData,u8 len)
{
    SPI_WriteBytes_LSB_1311(spifd_1311,ucRegAddr,ucData,len);
}
static u8 McuSpiGetByte(u8 ucRegAddr,u8 ucData)
{
    return SPI_ReadByte_LSB_1311(spifd_1311,ucRegAddr,ucData);
}
static u8 McuSpiGetBytes(u8 ucRegAddr,u8 *ucData,u8 len)
{
    SPI_ReadBytes_LSB_1311(spifd_1311,ucRegAddr,ucData,len);
}


static void McuDisablePdPin(void){
}
static void McuEnablePdPin(void){
}
static void McuCsPinLow(void){
	//system("himm 0x120D5040 0x00");
	int j=0;
	ioctl(pgpiofd,CSN0_SET,&j);
}
static void McuCsPinHigh(void){
	//system("himm 0x120D5040 0xFF");
	int j=1;
	ioctl(pgpiofd,CSN0_SET,&j);
}
static HiLowType McuIrqReadPin(void){
	int j=0;
	ioctl(pgpiofd,RF_GET,&j);
	if(j){
		return ST_HIGH;
	}else{
		return ST_LOW;
	}
}

/////////////////////////////////////////////////////////
Sky1311tDriverConstType gSky1311tDriverConstType={
  McuReadCardCallBack,// u8 (*ReadCallBack)(PTagInfo pTag, u8 *uBuff, u16 uLen);
   //u8 (*SpiSendByte)(u8 ucData);
  McuSpiSendCmd,//BYTE (*SpiSendCmd)(BYTE ucData);
  McuSpiSendByte,//BYTE (*SpiSendByte)(BYTE ucRegAddr,BYTE ucData);
  McuSpiSendBytes,//BYTE (*SpiSendBytes)(BYTE ucRegAddr,BYTE *ucData,BYTE len);
  McuSpiGetByte,//BYTE (*SpiGetByte)(BYTE ucRegAddr,BYTE ucData);
  McuSpiGetBytes, //BYTE (*SpiGetBytes)(BYTE ucRegAddr,BYTE *ucData,BYTE len);
  McuDisablePdPin,//void (*DisablePdPin)(void);
  McuEnablePdPin,//void (*EnablePdPin)(void);
  McuCsPinLow,//void (*CsPinLow)(void);
  McuCsPinHigh,//void (*CsPinHigh)(void);
  McuIrqReadPin,//bool (*IrqReadPin)(void);
  //1ms延时
  McuDelayMs,//void (*DelayMs)(uint32_t dwTime);
};
PSky1311tDriverConst gp1311Const=&gSky1311tDriverConstType;

Sky1311tDriverVarType gSky1311tDriverVarType={
  .wRcValue =0, 0,
  .wMoveCardTimeOut =0,
  .ucRcAdcValue =0,
  .ucNoCardCnt =0,
  .ucNoMoveCardCnt=0,
  .bhasCardIn = FALSE,
  .bcardReadOver = FALSE,
};

//WORD wRcValue,wMoveCardTimeOut;
//  BYTE ucRcAdcValue,ucNoCardCnt;
//  BOOL bhasCardIn;
//  BOOL bcardReadOver;

Sky1311tType gSky1311tType={
  &gSky1311tDriverConstType,
  &gSky1311tDriverVarType,
};

PSky1311t gpSky1311t=&gSky1311tType;
extern u8 wakeUpFlg;

void HAL_GPIO_EXTI_Callback(u16 GPIO_Pin){

}
extern u8 sleepFlg;
BOOL hasCardIn=FALSE;
/*void IRQ_1311_EXIT_IRQHandle(void){

}
*/
/*static void Error_Handler()
{
  //while(1)
  {
  }
}
*/
static BOOL sky1311t_rcval_Init(PSky1311t pSky)
{
    /* RC 频率校准，获得最接近13.56M的RC值以及对应的AD值 */
    pSky->pVar->wRcValue = sky1311RCFreqCali(pSky);
    if(pSky->pVar->wRcValue == 0)        // 没有采集到合适的AD值
    {
      return TRUE;
    }
    return FALSE;
}
/* SPI1 init function */
static void MX_SPI_SKY1311T_Init(void)
{
}
static void McuSpiCsPinInit(void){

}
////1311 Init////////////////////////////////////
void sky1311IoInit()
{
	McuSpiCsPinInit();
	MX_SPI_SKY1311T_Init();
}
BOOL MX_SKY1311_Init(PSky1311t pSky)
{
        BOOL bRet=FALSE;
        
        
        if(sky1311t_rcval_Init(pSky)){
          //PutStr("SKY1311T initialization failed..\r\n");
          bRet=TRUE;
        }else{
         printf("SKY1311T initialization succeeded..grcValue=0x%X \r\n",pSky->pVar->wRcValue);
        }
        return bRet;
}
/////////////////////////////////////////////////////////////////
/*  将与读卡芯片IRQ引脚连接的IO口设置为可外部中断 */
void IRQ_Sky1311t_Init(void)
{
    //IRQ Init
   // EXTI_ClearITPendingBit(EXTI_IT_Pin2);
   // GPIO_Init(SKY1311_IRQ_GPIO_PORT, SKY1311_IRQ_PIN, GPIO_Mode_In_FL_IT);
   // EXTI_SelectPort(EXTI_Port_B);
   // EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Rising);
}
void IRQ_Sky1311t_DeInit(void)
{
    //IRQ Init
   // EXTI_ClearITPendingBit(EXTI_IT_Pin2);
   // GPIO_Init(SKY1311_IRQ_GPIO_PORT, SKY1311_IRQ_PIN, GPIO_Mode_In_FL_No_IT);
   // EXTI_DeInit();
}
void Sky1311InSleep(void)
{
//    SKY1311_DISABLE();
//    systemHalt_GPIOConfig(); // GPIO config
   // IRQ_Sky1311t_Init();             // 开启外部中断，有卡可以唤醒MCU
//    PWR_UltraLowPowerCmd(ENABLE);
//    halt();             // 进入停机模式，等待唤醒
//    IRQ_DeInit();       // 禁止外部中断
//    wakeup_Init();
}

BOOL Sky1311HuntingCard(PSky1311t pSky,PTagInfo pTagInfo){
            //checkCardDeinit(pSky);          // 不需要检卡
            BOOL uReturn;
			//touchOff();
            if(sta_Ok == TypeA_test(pSky,pTagInfo))          // 是否Type A 卡
            {
                uReturn = TRUE;
            }
            else
            {
            	#ifndef SUPPORT_CARD_EMULATION
                if(sta_Ok== SmartTypeB_test(pSky,pTagInfo))
                {
                  uReturn = TRUE;
                }
                else        // 没有读到卡
                #endif
                {
                  #if SUPPORT_DEBUG_INF
				  PutStr("read fail\r\n");
				  #endif
				  uReturn = FALSE;
                }
            }
			//touchOn();
            return uReturn;
}

/*  将与读卡芯片IRQ引脚连接的IO口设置为可外部中断 */
void IRQ_Interrupt_Init(void)
{
}

void IRQ_Interrupt_DeInit(void)
{
}

/* 进入低功耗 active halt 状态 */
inline void system_ActiveHalt(PSky1311t pSky)
{

}


extern BYTE getData[64];
TagInfoType TagInfo;	 
u8 errorCard=0;
u8 uSecret=0;
u8 uUser=0;
extern u8 openDoorFlg;


extern u8 timeFlg;
u8 irq_sta1=0;
//boolean 
/*irq_sta1 = sky1311ReadReg(gpSky1311t,ADDR_ANA_CFG5);  // 读中断寄存器
Printf("have card irq_sta1=%x",irq_sta1);
Printf("effi AD=%x",gpSky1311t->pVar->wRcValue);
*/

//extern u8 nbDelayBuff;
//extern u16 offLedBuzzerCnt;
u8 nfcDelayCnt=0;
BOOL m1ReadPro()
{
	if(sta_Ok == TypeA_test(gpSky1311t,&TagInfo)) 		 // 是否Type A 卡
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL m1WritePro()
{
	if(sta_Ok == TypeA_test(gpSky1311t,&TagInfo)) 		 // 是否Type A 卡
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
extern unsigned char ledFlg;
BOOL Sky1311tHandleLoop(void){
	//if(timeFlg&T_100MS)
	{
		if(guccardStatus == 0)
		{
			if(nfcDelayCnt)
			{
				nfcDelayCnt --;
				return FALSE;
			}
			if(Sky1311HuntingCard(gpSky1311t,&TagInfo))
			{
			  gpSky1311t->pConst->ReadCallBack(NULL,&TagInfo,NULL,0);
			  if(!((TagInfo.uCardId[0]==0)
			  	&&(TagInfo.uCardId[1]==0)
			  	&&(TagInfo.uCardId[2]==0)
			  	&&(TagInfo.uCardId[3]==0)
			  ))
			  {
				  guccardStatus = 1;
				  if(TagInfo.uCardIdLen >=8)
				  {
					TagInfo.uCardIdLen = 8;
				  }
				 // CommunicateM1UUID(TagInfo.TagType,TagInfo.uCardId);
				  ledFlg = LED_RED_ON_OFF;
				  getcardid=1;
			  }
			}
		}	
		else
		{
			if(Sky1311HuntingCard(gpSky1311t,&TagInfo) == FALSE)
			{
				guccardStatus = 0;
			}
		}
	}
	
	return FALSE;
}
