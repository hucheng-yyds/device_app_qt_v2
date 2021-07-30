#ifndef _Sky1311t_Driver_H
#define _Sky1311t_Driver_H
#include "types.h"
#include "sky1311t.h"
#include "GeneralModule.h"

//卡号长度
#define TAG_NUM_SIZE_MAX		16

//默认的读卡数据长度
#define TAG_READ_SIZE_DEF       2
//默认工作状态
#define SKY1311_WKM_DEFAULT     SKY1311_WKM_SLEEP//SKY1311_WKM_CONTINUE//

//Sky1311 驱动类声明
typedef struct _Sky1311Class Sky1311Class,*CSky1311;

//Sky1311工作状态
typedef enum{
	//初始化
	SKY1311_INIT,
	//空闲
	SKY1311_IDLE,
	//校准
	SKY1311_CALI,
	//卡检测
	SKY1311_DET,
	//读卡
	SKY1311_READ,
	//
	SKY1311_MAX
}Sky1311StateType;

//工作模式类型
typedef enum{
	//睡眠模式
	SKY1311_WKM_SLEEP,
	//连续工作模式
	SKY1311_WKM_CONTINUE,
	//
	SKY1311_WKM_MAX
}Sky1311WorkModeType;

//SKY1311 API接口类型
typedef struct _Sky1311ApiType{
	//外部引用对象句柄
	void* pObj;
	//读卡
        BOOL (*ApiSky1311ReadCard)(
            CSky1311 oNfc,
            BYTE *pucBuff, 
            BYTE uLen,
	    WORD wOffset);
	//校准
	void (*ApiSky1311Calibration)(CSky1311 oNfc);
	//设置自动读卡长度
	void (*ApiSky1311SetReadLen)(CSky1311 oNfc, BYTE ucLen);
	//启用/停用
	void (*ApiSky1311StartStop)(CSky1311 oNfc, BOOL bStop);
	//模式选择
	void (*ApiSky1311ModeSelect)(CSky1311 oNfc, Sky1311WorkModeType Mode);
	//卡类型选择
	void (*ApiSky1311CardSelect)(CSky1311 oNfc, BYTE ucSel);
	//读卡回调函数
	void (*ApiSky1311ReadCallBack)(
		CModule oM,
		BYTE* pucUid,
		BYTE* pucBuff, 
		BYTE* pucLen, 
                BYTE* pCardType);
}Sky1311ApiType, *PSky1311Api;

//API接口类型
typedef const struct{	
	//缓冲区大小
	WORD wBuffSize;
	//如果在外部调用，可设置为NULL
	void (*HwInit)(void);
	//Spi片选脚控制
	void (*SetNssPin)(void);
	void (*ClrNssPin)(void);
	//SKY311芯片片选唤醒脚控制
	void (*SetWakeupPin)(void);
	void (*ClrWakeupPin)(void);
	//读取卡片检测脚
	HiLowType (*GetCardDet)(void);
	//SPI发送一个字节
	BYTE (*SendReceiveByte)(BYTE ucData);
	//SPI接收数据包
	void (*SendReceiveBuffer)(const BYTE *pucCmd, BYTE ucLen, BYTE *pucRsp);
	//延时1ms
	void (*Delay)(DWORD dwMs);
}Sky1311ConstType,*PSky1311Const;

//SKY1311驱动类实现
struct _Sky1311Class{
	//基类
	ModuleClass Base;
	PSky1311Const pConst;
	//API接口
	PSky1311Api pApi;
	//中断模式使用，中断脚中断中设置为true表示有数据收到
	Sky1311WorkModeType Mode;
        
        BYTE ucReadLen;//读数据长度
        u16 wRcValue;
        u8  uRcAdcValue;
        BOOL bhasCardIn;
        BOOL bcardReadOver;
        Sky1311StateType Status;
	volatile WORD w1msCnt;
};

//---------------------------------------------------------------------
//		SKY1311驱动层的构造函数
//功能：
//	用于SKY1311驱动层对象的函数
//参数：
//	ppB：将要加入的功能块句柄的指针
//	strName：SKY1311驱动层的名称
//	ucPriInit,ucPriLoop,ucPriTb：公共函数的执行优先级
//	Free：析构函数
//	pConst：常量结构体
//	pObj：外部调用模块的句柄
//	ApiSky1311ReadCallback：读卡回调函数读到卡片后调用，带出卡号和数据
//返回：
//	创建的对象句柄，NULL表示创建失败
//---------------------------------------------------------------------


//========================================================================
//				API接口函数
//========================================================================

//---------------------------------------------------------------------
//			SKY1311读卡函数
//功能：
//	用于在卡片工作时读取卡片的数据，必须在ApiSky1311ReadCallback()
//	回调函数中使用
//参数：
//	oM：SKY1311驱动层的句柄
//	pucBuff：输出读到的的卡片数据
//	ucLen：数据长度
//	wOffset：数据的起始位置
//返回：
//	RET_SUCCESS：读取成功，RET_FAILURE：读取失败
//---------------------------------------------------------------------
extern BOOL ApiSky1311ReadCard(
	CSky1311 oNfc,
	BYTE* pucBuff, 
	BYTE ucLen, 
	WORD wOffset);

//---------------------------------------------------------------------
//			SKY1311校准唤醒灵敏度的函数
//功能：
//	用于获取唤醒的阈值，该阈值在本层保存。初始化时自动执行，也可以
//	在需要的时候执行
//参数：
//	oM：SKY1311驱动层的句柄
//返回：
//	无
//---------------------------------------------------------------------
extern void ApiSky1311Calibration(CSky1311 oNfc);

//---------------------------------------------------------------------
//			SKY1311设置读卡数据长度的函数
//功能：
//	用于设置回调函数ApiSky1311ReadCallback()读卡的数据长度
//参数：
//	oM：SKY1311驱动层的句柄
//	ucLen：数据长度
//返回：
//	无
//---------------------------------------------------------------------
extern void ApiSky1311SetReadLen(CSky1311 oNfc, BYTE ucLen);

//---------------------------------------------------------------------
//			SKY1311启用和停用的函数
//功能：
//	用于设置SKY1311启用和停用，停用后读卡不起作用
//参数：
//	oM：SKY1311驱动层的句柄
//	bStop：FALSE：启用，TRUE：停用
//返回：
//	无
//---------------------------------------------------------------------
extern void ApiSky1311StartStop(CSky1311 oNfc, BOOL bStop);

//---------------------------------------------------------------------
//			SKY1311设定使用的卡类型的函数
//功能：
//	用于设定使用的卡类型
//参数：
//	oNfc：SKY1311驱动层的句柄
//	ucSel：开放的卡类型：
//		NOTHING		0x00 // 0000 0000
//		NFCTYPE1 	0x01 // 0000 0001 
//		NFCTYPE2 	0x02 // 0000 0010 
//		NFCTYPE3 	0x04 // 0000 0100 
//		NFCTYPE4A 	0x08 // 0000 1000 
//		NFCTYPE4B 	0x10 // 0001 0000 
//		NFCTYPE5 	0x20 // 0010 0000 
//		NFCTYPEMIFARE	0x40 // 0100 0000
//		NFC_PHONE 	0x80 // 1000 000
//		ALL 		0xFF // 1111 1111 
//返回：
//	无
//---------------------------------------------------------------------
extern void ApiSky1311CardSelect(CSky1311 oNfc, BYTE ucSel);

//---------------------------------------------------------------------
//			SKY1311设定工作模式的函数
//功能：
//	用于设置SKY1311的工作模式
//参数：
//	oM：SKY1311驱动层的句柄
//	Mode：	SKY1311_WKM_SLEEP：周期性唤醒读卡模式，
//		SKY1311_WKM_CONTINUE：连续模式
//返回：
//	无
//---------------------------------------------------------------------
extern void ApiSky1311ModeSelect(CSky1311 oNfc, Sky1311WorkModeType Mode);

#endif