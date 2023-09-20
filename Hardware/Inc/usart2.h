#ifndef _USART2_
#define _USART2_
//头文件
#include "stm32f10x.h"
#include <stdio.h>
#include "wifi.h"
//宏定义

//函数声明
void USART_2_Init(u32 bound);
void Usart2_Send_n_Byte(char *Str,int n);
#endif


