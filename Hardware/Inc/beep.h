#ifndef _BEEP_
#define _BEEP_

//头文件
#include "stm32f10x.h"
//宏定义
#define BEEP_ON   GPIO_SetBits(GPIOB, GPIO_Pin_8)        	  //设置为高电平
#define BEEP_OFF  GPIO_ResetBits(GPIOB, GPIO_Pin_8)       	//设置为低电平
//翻转操作
#define BEEP_OVER  GPIOB->ODR ^= 1<<8      	              //异或操作
//函数声明
void BEEPGpioInit(void);


#endif

