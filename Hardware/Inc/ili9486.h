#ifndef __ILI9341_H__
#define __ILI9341_H__

#include <stm32f10x.h>
#include <stm32f10x_conf.h> 
#include "sys_tick.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

#define TFTLCD_REG     ( *( (u16*)0x6c000000 ) )
#define TFTLCD_RAM     ( *( (u16*)0x6c000800 ) )

void LCD_CTRL_CMD(u16 common);
void LCD_CTRL_DATA(u16 data);
void LCD_Clear(u16 SX, u16 EX, u16 SY, u16 EY, u16 Color);
void LCD_Init(void);
void LCD_CTRL_RD(u8 par_count,int * RD_Num);

extern int LCD_Width,LCD_High;
#endif

