
#ifndef _SKY1311_INSTAN_H
#define _SKY1311_INSTAN_H

#include "types.h"
#include "sky1311t.h"

#define sky1311Disable()    SKY1311_DISABLE()
#define sky1311Enable()     SKY1311_ENABLE()




///////////////////////////////////////////////////////////////////////////////////
//常量结构体
//extern Sky1311ConstType gSky1311Const;
extern PSky1311t gpSky1311t;
//extern void McuApiSky1311ReadCallBack(CModule oM,
//		BYTE* pucUid,		      
//		BYTE* pucBuff, 
//		BYTE* pucLen, 
//		BYTE* pCardType);

extern BOOL Sky1311tHandleLoop(void);
//extern void Error_Handler(void);
extern BOOL MX_SKY1311_Init(PSky1311t pSky);
extern void sky1311IoInit();
#define  CARDREMOVED        0
#define  HASCARDIN          1
#define  READCARDOVER       2
#define  WAITREMOVE         4
#define  GOTOSLEEP			8
#define  CARD_WAIT			16
extern void cardTest(PSky1311t pSky);
extern BOOL m1ReadPro();

#endif
