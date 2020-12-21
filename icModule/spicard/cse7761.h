#ifndef __CSE7761_H
#define	__CSE7761_H

#include "mcu.h"

void cse7761Pro();
void cse7761_SPI_init(void);

#define CS_H 		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)
#define CS_L 		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
#define CLK_H		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define CLK_L		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define MOSI_H 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)
#define MOSI_L 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)
#define ReadMISO    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)//GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)

#endif /* __LED_H */

