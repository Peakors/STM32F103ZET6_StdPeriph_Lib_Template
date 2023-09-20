#ifndef _USART_
#define _USART_
//头文件
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//宏定义

//函数声明
void USART_GPIO_Init(void);
void USART_Config(u32 baud);
void USART_Send_Data(u8 * data);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
u8 USART_Recv_Data(void);
void NVIC_Config(void);
extern u8 uart1_count;
extern char USART1_Rcv_Buff[128];
extern int uart1_finish_flage;
extern 	u8 rc;
#endif


