

#ifndef __DEBUG_APP_H
#define __DEBUG_APP_H

#include "Types.h"

#if (USE_DEBUG)

#include "UartDebug.h"

extern BYTE gucDbgRxBuff[];
extern BYTE gucDbgCmdBuff[];
extern UartDbgType gUartDbg;
#ifdef USE_DEBUG_ONLY_GET
#define PutChar(x)		printf(x)	
#define PutStr(x)		printf(x)		
#define Printf(x, y)	printf(x,y)
#elif defined USE_DEBUG_TX_ENABLE
#define PutChar(x)			UartPutChar(&gUartDbg, x)
#define PutStr(x)				UartPutStr(&gUartDbg, x)
#define Printf(x, y)			UartPrintf(&gUartDbg, x, y)
#else
#define PutChar(x)			
#define PutStr(x)				
#define Printf(x, y)
#endif
#ifndef DISABLE_PRINT_SIGNED_VALUE
#define PrintfS8(x, y)			UartPrintfS8(&gUartDbg, x, y)
#define PrintfS16(x, y)		UartPrintfS16(&gUartDbg, x, y)
#define PrintfS32(x, y)		UartPrintfS32(&gUartDbg, x, y)
#else
#define PrintfS8(x, y)
#define PrintfS16(x, y)
#define PrintfS32(x, y)
#endif
#ifndef DISABLE_PRINT_BUFF
#define PrintBuff(x, y)		UartPrintBuff16(&gUartDbg, x, y)
#else
#define PrintBuff(x, y)
#endif
#define DbgInit()				UartDbgInit(&gUartDbg, gucDbgRxBuff, gucDbgCmdBuff)
#define IsrDebugRx()			IsrUartDebugRx(&gUartDbg)
#define DbgHandle()			UartDbgHandle(&gUartDbg)

#else

#define PutStr(x)
#define Printf(x, y)
#define PrintfS8(x, y)
#define PrintfS16(x, y)
#define PrintfS32(x, y)
#define PrintBuff(x, y)
#define DbgInit()
#define IsrDebugRx()
#define DbgHandle()

#endif

#endif

