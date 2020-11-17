#ifndef __GPIO_DEFS_H
#define __GPIO_DEFS_H

#include "stm32f0xx.h"
#include "delay.h"
#include "mcu.h"
//#include "stm32f0xx_hal_gpio.h"
/*
#define CSB_PIN					GPIO_PIN_11                      
#define CSB_PORT				GPIOA
#define CSB_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define FCSB_PIN				GPIO_PIN_11                      
#define FCSB_PORT				GPIOA
#define FCSB_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SCL_PIN				GPIO_PIN_11                      
#define SCL_PORT				GPIOA
#define SCL_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SDA_PIN				GPIO_PIN_11                      
#define SDA_PORT				GPIOA
#define SDA_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()


#define cmt_spi3_csb_1()        HAL_GPIO_WritePin(CSB_PORT, CSB_PIN, GPIO_PIN_SET)
#define cmt_spi3_csb_0()        HAL_GPIO_WritePin(CSB_PORT, CSB_PIN, GPIO_PIN_RESET)

#define cmt_spi3_fcsb_1()       HAL_GPIO_WritePin(FCSB_PORT, FCSB_PIN, GPIO_PIN_SET)
#define cmt_spi3_fcsb_0()       HAL_GPIO_WritePin(FCSB_PORT, FCSB_PIN, GPIO_PIN_RESET)
    
#define cmt_spi3_scl_1()        HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET)
#define cmt_spi3_scl_0()        HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET)

#define cmt_spi3_sda_1()        HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET)
#define cmt_spi3_sda_0()        HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET)
#define cmt_spi3_sda_read()     HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN)
*/

/*
#define SET_GPIO_OUT(x)             GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_PP)
#define SET_GPIO_IN(x)              GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING)
#define SET_GPIO_OD(x)              GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_OD)
#define SET_GPIO_AIN(x)             GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AIN)
#define SET_GPIO_AFOUT(x)           GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AF_PP)
#define SET_GPIO_AFOD(x)            GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AF_OD)
#define SET_GPIO_H(x)               (x->BSRR = x##_PIN) //GPIO_SetBits(x, x##_PIN)
#define SET_GPIO_L(x)               (x->BRR  = x##_PIN) //GPIO_ResetBits(x, x##_PIN)
#define READ_GPIO_PIN(x)            (((x->IDR & x##_PIN)!=Bit_RESET) ?1 :0) //GPIO_ReadInputDataBit(x, x##_PIN) 
*/
#endif
