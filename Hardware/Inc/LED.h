#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"


//宏定义
#define LED1_ON GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LED1_OFF GPIO_SetBits(GPIOB, GPIO_Pin_5)

#define LED2_ON GPIO_ResetBits(GPIOE, GPIO_Pin_5)
#define LED2_OFF GPIO_SetBits(GPIOE, GPIO_Pin_5)

#define LED3_ON GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define LED3_OFF GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define LED4_ON GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define LED4_OFF GPIO_SetBits(GPIOA, GPIO_Pin_6)
//翻转操作
#define LED1_OVER  GPIOB->ODR ^= 1<<5      	              //异或操作
#define LED2_OVER  GPIOE->ODR ^= 1<<5      	              //异或操作
#define LED3_OVER  GPIOA->ODR ^= 1<<5      	              //异或操作
#define LED4_OVER  GPIOA->ODR ^= 1<<6      	              //异或操作
//函数声明
void LEDInit(void);
#endif
