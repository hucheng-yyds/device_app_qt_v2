#ifndef __NB_H
#define	__NB_H

#include "mcu.h"
#define NOP() __NOP()

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef DWORD
#define DWORD	unsigned long
#endif

#ifndef u8
#define u8  unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u32
#define u32 unsigned long
#endif

#ifndef U8
#define U8  unsigned char
#endif

#ifndef S8
#define S8  char
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef U32
#define U32 unsigned long
#endif
/*
#ifndef BOOL
#define BOOL    bool
#endif
*/
#ifndef FALSE
#define FALSE   false
#endif
#ifndef TRUE
#define TRUE    true
#endif



//#define NB_PWRKEY	25
//#define NB_RST		28
//#define NB_PSMINT	4


#define NB_PWRKEY_PORT	GPIOB
#define NB_PWRKEY_PIN	GPIO_PIN_11
#define NB_PWRKEY_HIGH	HAL_GPIO_WritePin(NB_PWRKEY_PORT,NB_PWRKEY_PIN,GPIO_PIN_SET)
#define NB_PWRKEY_LOW	HAL_GPIO_WritePin(NB_PWRKEY_PORT,NB_PWRKEY_PIN,GPIO_PIN_RESET)

#define NB_RST_PORT		GPIOB
#define NB_RST_PIN		GPIO_PIN_10
#define NB_RST_HIGH		HAL_GPIO_WritePin(NB_RST_PORT,NB_RST_PIN,GPIO_PIN_SET)
#define NB_RST_LOW		HAL_GPIO_WritePin(NB_RST_PORT,NB_RST_PIN,GPIO_PIN_RESET)

#define NB_PSMINT_PORT	GPIOB
#define NB_PSMINT_PIN	GPIO_PIN_1
#define NB_PSMINT_HIGH	HAL_GPIO_WritePin(NB_PSMINT_PORT,NB_PSMINT_PIN,GPIO_PIN_SET)
#define NB_PSMINT_LOW	HAL_GPIO_WritePin(NB_PSMINT_PORT,NB_PSMINT_PIN,GPIO_PIN_RESET)





#define NB_IDLE								0
#define NB_DELAY_START						1
#define NB_OFF_SLEEP						2
#define NB_OFF_EDRX							3
#define NB_ON_PSM							4
#define NB_IMEI								6
#define NB_CPIN								7
#define NB_ICCID							8
#define NB_RADIO_SIGNAL						9
#define NB_NET_CHECK						10
//#define NB_PND								10
//#define NB_CHECK_IF_NBIOT   				11
//#define NB_CHECK_IP							12

#define NB_CONNECT_IOT						13
#define NB_ONE_NET_CREATE					14
#define NB_ONE_NET_ADD_OBJ					15
#define NB_ONE_NET_OPEN						16
#define NB_ONE_NET_DISCOVERRSP				17


//#define NB_SEND_START						20
#define NB_SEND_CHECK						21
#define NB_SENDING							22
#define NB_SEND_WAIT						23
#define NB_GET								24
#define NB_USER								25
#define NB_SECRET							26

#define NB_TIME_OUT							30
#define NB_CLEAR_FREQ						31



//#define 
#define NB_DISCONNECT						230
#define NB_ERROR							231
#define NB_OFF_RADIO						232
#define NB_OFF_RADIO_WAIT					233
#define NB_OFF_RADIO1						234
#define NB_OFF_RADIO1_WAIT					235
#define NB_OFF_FRE							236
#define NB_OFF_FRE_WAIT						236

#define NB_WAIT_TO_SLEEP					240
#define NB_WAIT_TO_SLEEP1					241
#define NB_WAIT_TO_SLEEP2					242
#define NB_WAIT_TO_POWER_OFF				247
#define NB_SLEEP							248
#define NB_WAKEUP							249
#define NB_WAKEUP_WAIT						250
#define NB_STATUS							251
#define NB_WAIT								252


#define NB_OPEN					0
#define NB_ADD					1
#define NB_MODIFY				2
#define NB_DEL					3
#define NB_ADD_BLE_SECRET		4
#define NB_MODIFY_BLE_SECRET	5
#define NB_DEL_BLE_SECRET		6
#define NB_ADD_NUMBER_SECRET	7
#define NB_MODIFY_NUMBER_SECRET	8
#define NB_DEL_NUMBER_SECRET	9
#define NB_ADD_NFC_SECRET		10
#define NB_MODIFY_NFC_SECRET	11
#define NB_DEL_NFC_SECRET		12
#define NB_ADD_ID_CARD_SECRET		13
#define NB_MODIFY_ID_CARD_SECRET	14
#define NB_DEL_ID_CARD_SECRET		15
#define NB_ADD_FINGER_SECRET		16
#define NB_MODIFY_FINGER_SECRET		17
#define NB_DEL_FINGER_SECRET		18



#define HEART_CMD				0x22
#define OPEN_INFO_CMD			0x24
#define WARM_INFO_CMD			0x25
#define DEFAULT_CMD				0x26
#define OPEN_CMD				0x41
#define OPER_USER_CMD			0x42
#define OPER_SECRET_CMD			0x43
#define NB_ACK					0xAA


//开锁方式
#define OPEN_FUN_FINGER_OPEN				1
#define OPEN_FUN_NUMBER_KEYBOARD_OPEN		2
#define OPEN_FUN_NFC_OPEN					3
#define OPEN_FUN_MECH_KEYS_OPEN				4
#define OPEN_FUN_BLE_OPEN					5
#define OPEN_FUN_NB_OPEN					6
#define OPEN_FUN_ID_CARD_OPEN				7

//错误代码
#define ERROR_CODE_LOCK_RIPPED				1
#define ERROR_CODE_FORCE_OPEN				2
#define ERROR_CODE_FINGER_TRY_OPEN_FREEZE	3
#define ERROR_CODE_SECRET_TRY_OPEN_FREEZE	4
#define ERROR_CODE_NFC_TRY_OPEN_FREEZE		5
#define ERROR_CODE_KEY_TRY_OPEN_FREEZE		6
#define ERROR_CODE_LOW_BAT_WARM				7
#define ERROR_CODE_TEMP						8
#define ERROR_CODE_FORCE_OPEN_WARM			9


/*
typedef enum open_fun
{
    FINGER_OPEN             = 1,  ///< 非特定错误
    NUMBER_KEYBOARD_OPEN    = 2,  ///< 超时错误
    NFC_OPEN			    = 3,  ///< 接收奇偶校验错
    MECH_KEYS_OPEN			= 4,  ///< 接收CRC校验错
    BLE_OPEN                = 5,  ///< FIFO满
    NB_OPEN               	= 6,  ///< FIFO空
    ID_CARD_OPEN            = 7,  ///< 防冲突错误
}open_fun_t;

typedef enum fail_fun
{
    LOCK_RIPPED_FAIL        = 1,  ///< 非特定错误
    FORCE_OPEN_FAIL    		= 2,  ///< 超时错误
    FINGER_TRY_OPEN_FAIL	= 3,  ///< 接收奇偶校验错
    SECRET_TRY_OPEN_FAIL	= 4,  ///< 接收CRC校验错
    NFC_TRY_OPEN_FAIL       = 5,  ///< FIFO满
    KEY_TRY_OPEN_FAIL       = 6,  ///< FIFO空
    LOW_BAT_TRY_OPEN_FAIL   = 7,  ///< 防冲突错误
    TEMP_FAIL 				= 8,
    FORCE_OPEN_FAIL			= 9
}fail_fun_t;
*/

#define NB_SEND_USER		0x20
#define NB_SEND_SECRET		0x10
#define NB_RECORD			0x08
#define NB_CONTINUE_SEND	0x04
#define NB_DEFAULT			0x02
#define NB_HEART			0x01

//#define NB_SLEEP_ENTER	1
void nbDeviceStatus(u8 operaType,u8 motorStatus);
void NB_Init(void);
extern void nbPro(void);
extern unsigned char intToStr(unsigned char dData);
//void nbCacheHeartPack(unsigned char lockType,unsigned char motorStatus,unsigned long uAutoNum);
//extern void nbCacheHeartPack(u8 id,u8 *packId);
extern void nbCacheDefaultPack(unsigned char id,unsigned char *packId);
extern void nbCacheRecordPack(u8 id,u8 *phone,u8 uOpenFun);
extern void warmInfoCmd(unsigned char uError);
extern void heartCmd(unsigned char *uGetData);
extern unsigned char nbCacheNull(void);
extern void nbSleepPro(void);
//extern void nbSleepOffPro();
extern unsigned char nbCacheCheckAllNull(void);
extern void intTo3Str(unsigned char uStart,unsigned char *uEnd);
extern unsigned char strToInt(unsigned char dData);
unsigned char is_in(unsigned char *s,unsigned char sSize,char *c,unsigned char cSize);
void memsetStr(unsigned char *uData1,unsigned char uData2,unsigned char uLength);
void memcpyStr(unsigned char *uData1,unsigned char *uData2,unsigned char uLength);
typedef struct _nbCacheType{
    unsigned char uData[80];
	unsigned char packId[6];
    unsigned char uLength;
    unsigned char uLengthBk;
	unsigned char sendIndex;//0
	unsigned char lockType;//0
}nbCacheType;

/*enum _deviceStatus{
	idle=0,
	fault,
	charging,
	charged,
	boardHighTemp,
	fire,
	outHighTemp,
	outLine,
}deviceStatus;
*/

//status
#define DEVICE_IDLE						0//1、空闲  
#define DEVICE_FAULT					1//0、设备故障  
#define DEVICE_CHARGING					2//2、充电中 
#define DEVICE_CHARGE_FINISH			3//3、充电完成  
#define DEVICE_TEMPERA_HIGH				4//4、温度太高保护  
#define DEVICE_PROTECT					5//5、火焰保护   
#define DEVICE_ENVI_TEMPERA_PROTECTION	6//6、环境温度保护  
#define DEVICE_OFF_LINE					7//7、离线
#define DEVICE_I_HIGH					8//8、电流太高


#define CONTROL_TIME	     1//1、按时间  
#define CONTROL_W  	       2//2、按电量  
#define CONTROL_AUTO       3//3、自动充满  
#define CONTROL_OFF_DEVICE 4//4、强制关闭设备充电   
#define CONTROL_OFF_I      5//5、断电电流 
#define CONTROL_DELAY_CHECK_I_TIME 6//6、延时检测电流时间   

typedef struct _nbPortStausType{
    u8 device;
	u8 controlTimeWI;//按时间，电量，电流
	u16 remainTimeWI;//剩余时间，电量，电流
	u8 v;
	u8 i;
}deviceStatus;

#define NB_DELAY_MAX	15

#define SLEEP_WAKEUP			0
#define SLEEP					1
#define SLEEP_HEART				2

void nbCacheHeartPack();
unsigned char is_in(unsigned char *s,unsigned char sSize,char *c,unsigned char cSize);
void clearNoContinueData(void);


#define NB_ERROR_MAX	3
#define NB_CONNECT_STATUS_CONNECTED		1
#define NB_CONNECT_STATUS_DISCONNECT	0



#endif /* __LED_H */

