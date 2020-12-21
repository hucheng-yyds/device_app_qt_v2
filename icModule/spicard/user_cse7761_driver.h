/****************************** File Name:user_cse7761.h ******************************/
#ifndef  __USER_CSE7761_DRIVER_H__
#define  __USER_CSE7761_DRIVER_H__
#include "mcu.h"

#define PIN_MISO						GPIO_PIN_7
#define PIN_MOSI						GPIO_PIN_6
#define PIN_CLK							GPIO_PIN_5
#define PIN_IRQ							GPIO_PIN_4


#define SPI_DELAY_ENABLE				1
#define SPI_REGISTER_CTL				1

#if(SPI_REGISTER_CTL == 1)
#define setCLKHigh     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define setCLKLow      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define setMosiHigh    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)
#define setMosiLow 	   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)
//#define getMisoState (((*(volatile u32 *)(0xc00003bc))& 0x10000000 ) >> 28) //高为1，低为0
//#define getIrqState  (((*(volatile u32 *)(0xc00003ac))& 0x10000000 ) >> 28) //高为1，低为0
#define getMisoState   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) //高为0x10000000，低为0x00000000
#define getIrqState    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) //高为0x10000000，低为0x00000000
#define setIrqHigh    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)
#define setIrqLow 	   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
#else
#endif

extern char cse7761MisoReversalFlag;

void spiWriteByte(unsigned char dat);
void cse7761Write(unsigned char add,unsigned char *dat,unsigned int size);
void cse7761Read(unsigned char add,unsigned char *dat,unsigned int size);

#endif
/********************************** FILE END **************************************/
