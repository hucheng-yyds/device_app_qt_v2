
#ifndef _GeneralModule_h
#define _GeneralModule_h

#include "stdlib.h"
#include "string.h"

#define GmPutStr(x)						//PutStr(x)
#define GmPrintf(x, y)						//Printf(x, y)
#define GmPrintBuff(x, y)					//PrintBuff(x, y)

//调试信息类型
//  字符串
#define GM_LOG_CMD_STRING					0
//  无符号数
#define GM_LOG_CMD_VALUE_U					1
//  有符号数
#define GM_LOG_CMD_VALUE_S					2
//  缓冲区
#define GM_LOG_CMD_BUFFER					3
//  其他
#define GM_LOG_CMD_OTHER					4

//模块状态类型
typedef enum{
	//模块刚创建的初始化阶段，未能工作
	GMST_INIT,
	//空闲状态，可进入休眠或阻塞
	GMST_IDLE,
	//正常运行状态
	GMST_RUNNING,
	//停止状态，不运行LoopHandler()和Isr1mTimebase()
	GMST_HALT,
	//故障状态
	GMST_FAIL,
	//
	GMST_MAX
}ModuStatType;

//基类声明
typedef struct _ModuleClass ModuleClass, *CModule;

//---------------------------------------------------------
//			公共函数类型
//
//	公共函数包括初始化、状态机、1ms时基函数
//	该类型包括函数的指针，执行优先级，和按此优先级下一个
//	执行模块的地址
//---------------------------------------------------------
typedef struct _ModFuncType{
	//函数指针
	void (*Func)(CModule pM);
	//执行优先级
	BYTE ucPriority;
	//下一个模块
	CModule pNext;
}ModFuncType, *PModFunc;

//---------------------------------------------------------
//			通用程序模块类型
//
//	这是所有程序模块的基类，用于派生出各种实际功能的类
//---------------------------------------------------------
struct _ModuleClass{
	//模块名称
	char* strName;
	//公共函数：初始化、状态机、1ms时基函数
	ModFuncType MfInit, MfLoop, MfTimebase;
	//状态
	ModuStatType Status;
	//析构函数
	void (*Free)(CModule* ppM);
	//输出调试信息的函数
	void (*Log)(BYTE ucCmd, BYTE* pucBuff, DWORD dwValue);
};

//---------------------------------------------------------
//			功能块类型
//
//	功能块用于组织程序模块，包含按初始化、状态机、1ms时基函数
//	的优先级顺序的第一个模块句柄，后续顺序由模块的公共函数成员
//	的pNext指针形成的链表决定，以NULL结束
//---------------------------------------------------------
typedef struct _FuncBlockType{
	//功能块名称
	char* strName;
	//按初始化、状态机、1ms时基函数优先级顺序的第一个模块句柄
	CModule pMInit, pMLoop, pMTimebase;
}FuncBlockType, *PFuncBlock;

//基类构造函数参数类型
typedef struct _BaseParaType{
	//初始化函数
	void (*Init)(CModule pM);
	//初始化函数的优先级
	BYTE ucPrioInit;
	//状态机函数
	void (*Loop)(CModule pM);
	//状态机函数的优先级
	BYTE ucPrioLoop;
	//时基函数
	void (*Timebase)(CModule pM);
	//时基函数的优先级
	BYTE ucPrioTb;
}BaseParaType, *PBasePara;

//---------------------------------------------------------------------
//			创建一个功能块的函数
//功能：
//	用于创建一个功能块
//参数：
//	strName：功能块的名称
//返回：
//	创建好的功能块的句柄，NULL表示创建失败
//---------------------------------------------------------------------
extern PFuncBlock NewFuncBlock(char* strName);

//---------------------------------------------------------------------
//			基类对象的构造函数
//功能：
//	用于创建一个基类对象并加入功能块
//参数：
//	ppB：将要加入的功能块句柄的指针
//	strName：模块名称
//	pPara：基类参数
//	Free：析构函数
//返回：
//	创建的对象句柄，NULL表示创建失败
//---------------------------------------------------------------------
extern CModule GmNewModule(PFuncBlock* ppB,
	char* strName, 
	PBasePara pPara,
	void (*Free)(CModule* ppM));

//---------------------------------------------------------------------
//		加载一个模块到功能块中的函数
//功能：
//	用于加载一个模块到功能块中
//参数：
//	ppB：功能块句柄的指针
//	ppM：需要加载的模块的指针
//返回：
//	TRUE：加载成功，FALSE：加载失败
//---------------------------------------------------------------------
extern BOOL GmLoadModule(
	PFuncBlock* ppB, 
	CModule* ppM);

//---------------------------------------------------------------------
//		从功能块中卸载一个模块的函数
//功能：
//	用于从功能块中卸载一个模块
//参数：
//	ppB：功能块句柄的指针
//	pM：需要卸载的模块的指针
//返回：
//	TRUE：卸载成功，FALSE：卸载失败
//---------------------------------------------------------------------
extern BOOL GmRemoveModule(
	PFuncBlock* ppB, 
	CModule* ppM);

//---------------------------------------------------------------------
//		从功能块中删除一个模块的函数
//功能：
//	用于从功能块中删除一个模块
//参数：
//	ppB：功能块句柄的指针
//	pM：需要删除的模块的指针
//返回：
//	TRUE：删除成功，FALSE：删除失败
//---------------------------------------------------------------------
extern BOOL GmDeleteModule(
	PFuncBlock* ppB, 
	CModule* ppM);

//---------------------------------------------------------------------
//		运行功能块的初始化函数
//功能：
//	用于按设定的优先级运行功能块里所有模块的初始化函数
//参数：
//	pB：功能块的指针
//返回：
//	无
//---------------------------------------------------------------------
extern void GmRunFuncBlockInit(PFuncBlock pB);

//---------------------------------------------------------------------
//		运行功能块的状态机函数
//功能：
//	用于按设定的优先级运行功能块里所有模块的状态机函数
//参数：
//	pB：功能块的指针
//返回：
//	无
//---------------------------------------------------------------------
extern void GmRunFuncBlockLoopHandler(PFuncBlock pB);

//---------------------------------------------------------------------
//		运行功能块的1ms时基函数
//功能：
//	用于按设定的优先级运行功能块里所有模块的1ms时基函数
//参数：
//	pB：功能块的指针
//返回：
//	无
//---------------------------------------------------------------------
extern void GmRunFuncBlockIsr1msTimebase(PFuncBlock pB);

//---------------------------------------------------------------------
//		返回程序模块状态指针的函数
//功能：
//	返回程序模块状态的指针，用于读写该状态
//参数：
//	pM：程序模块的句柄
//返回：
//	所需程序模块状态的指针
//---------------------------------------------------------------------
extern ModuStatType* GmPointerToModuleStatus(CModule pM);


#endif
