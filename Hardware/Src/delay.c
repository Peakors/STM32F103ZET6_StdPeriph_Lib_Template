#include "stm32f10x.h"
#include "sys_tick.h"

/****************************************************
函数名：Delay_Init
形参：无
返回值：无
函数功能：精确延时的初始化
****************************************************/
void Delay_Init(void)
{
	SysTickConfig();
}

