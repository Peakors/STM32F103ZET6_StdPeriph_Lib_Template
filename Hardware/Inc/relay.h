#ifndef _RELAY_H_
#define _RELAY_H_

#include "stm32f10x.h"


//宏定义
#define RELAY1_ON GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define RELAY1_OFF GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define RELAY2_ON GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define RELAY2_OFF GPIO_SetBits(GPIOC, GPIO_Pin_2)

//函数声明
void RELAYInit(void);
#endif
