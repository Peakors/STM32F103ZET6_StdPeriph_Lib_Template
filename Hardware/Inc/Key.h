#ifndef _KEY_
#define _KEY_

//头文件
#include "stm32f10x.h"
#include "delay.h"

//宏定义

//函数声明
void KEYGpioInit(void);
unsigned char  KEYScanFunction(void);
uint8_t key_scan();

#endif
