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
 ** 文 件 名： userdef.h
 **
 ** 文件简述： 用户定义和配置部分，用来选择一些编译和运行条件
 **
 ** 版本历史:
 ** 2015-10-05 V1.00  Mark   正式版本
 **
 ******************************************************************************/
#ifndef  __USERDEF_H_
#define  __USERDEF_H_
/**
 ******************************************************************************
 ** \简述： 配置开关定义
 **
 ******************************************************************************/
#define RATE_TEST       0       // 是否要做速率切换测试
#define M1_TEST         1       // 是否要做M1卡测试
#define FIFO_TEST       1       // 是否要做FIFO信息测试

#define CHECKCARD       1       // 是否要询卡，1：是，0：不是
#define READERCARD      1       // 是否询卡读卡，1：是，0：不是
#define USEDCMD         1       // 是否使用串口命令，1：是，0：不是
#define SZTCARDTEST     0       // 是否测试深圳通卡，1：是， 0：不是
#define RCTEST          1
/******************************************************************************/
/* 卡类型选择 */
#define AUTO            0
#define ONLYM1          1
#define ONLYSMART       2

#define CARDTYPE        AUTO     // (decide on SAK)

#define MINADVAL	0x30    // 小于等于此值就读卡
#define MAXADVAL        0xFF
//#define CARDCHECK   0
//#define CARDOPERAT  1
//#define FREQCALI    2

#define RSSI_DELTA      6//6//8
#define RSSI_ABS        8//8//10

#endif