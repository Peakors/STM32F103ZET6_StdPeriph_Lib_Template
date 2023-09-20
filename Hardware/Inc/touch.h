#ifndef __TOUCH_H__
#define	__TOUCH_H__

#include <stm32f10x.h>
#include "sys_tick.h"
#include "lcd_gui.h"
#include "math.h"
#include "usart.h"      //测试坐标点时使用
#include "ili9486.h"
typedef struct
{
	/* 触摸屏坐标值 */
	u16 x;
	u16 y;
	
	/* 触摸屏比例系数、偏移量*/
	float kx;        //比例系数
	float ky;
	s16 offset_x;    //偏移量
	s16 offset_y;
}TOUCH_TYPE_DEF;


/* 函数声明 */
void Touch_Gpio_Init(void);
void Touch_Adj_Init(void);
u16 Touch_Get_ADC(u8 common);
TOUCH_TYPE_DEF Touch_Get_XY(void);
TOUCH_TYPE_DEF Touch_Lcd(void);
TOUCH_TYPE_DEF Touch_Scanf(void);

void Touch_Init(void);

#endif

