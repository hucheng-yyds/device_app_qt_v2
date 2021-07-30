/****************************** File Name:user_cse7761.h ******************************/
#ifndef  __USER_CSE7761_H__
#define  __USER_CSE7761_H__
#include "mcu.h"

//#include "gpio_api.h"
//#include "user_global.h"
#include "user_cse7761_driver.h"
//#define SUPPORT_7611_PRINTF

#define cse7761Printf userPrintfDebug

void userCes7761Init(void);
unsigned long cse7761ReadUAdc();
unsigned long cse7761ReadU();
void cse7761Pro();
void cse7761AStartPro();
void cse7761BStartPro();



typedef struct
{
unsigned int v;
unsigned int ia;
unsigned char iaBuff;
unsigned char iaLessBuff;
unsigned int pa;
unsigned long long ea;
unsigned long wa;  //0.001
unsigned int ib;
unsigned char ibBuff;
unsigned char ibLessBuff;
unsigned int pb;
unsigned long long eb;
unsigned long wb;
unsigned int relay;
unsigned int usLed;
unsigned int countDownTime;
unsigned int countDownCmd;
} PowerInf_t;

#define UPDATE_ENERGY_MIN 	536871 //1<<29/1000*1??????对应0.001kwh

#endif
/********************************** FILE END **************************************/
