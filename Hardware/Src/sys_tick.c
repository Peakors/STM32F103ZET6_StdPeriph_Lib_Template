#include "stm32f10x.h"

/*
static __INLINE uint32_t SysTick_Config(uint32_t ticks);
//用来装载定时的时间值
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);
//用来设置滴答时钟的时钟频率

目的：用滴答时钟形成一个1us的定时器；
1us = T*(1/f)；
T:要装载的数；
f:滴答时钟运行的时钟频率；

1s = 1000ms;
1ms = 1000us;
1s = 1000000us;
1M = 1000000;
1/1M = 1us;
t= 1/72M = 1/72us
1us = 72 * t;
SystemCoreClock/1000000 = 72     ---- 1us
SystemCoreClock/100000  = 720    ---- 10us
SystemCoreClock/10000   = 7200   ---- 100us
SystemCoreClock/1000    = 72000  ---- 1ms
SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//打开滴答时钟
	
SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭滴答时钟
*/
/****************************************************
函数名：SysTickConfig
形参：无
返回值：无
函数功能：滴答时钟初始化
****************************************************/
void SysTickConfig(void)
{
	//配置为72Mhz时钟
	//t= 1/72M = 1/72us;
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	if(SysTick_Config(SystemCoreClock/1000000) == 1)//赋初值 72
	{                    
		while(1);
	}
	
	//关闭滴答时钟
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	//SysTick->CTRL &= ~0x01;// 0xfe  1111 1110
	//SysTick->CTRL |= 0x01;
	//给某些位写0，用按位与（&）
	//给某些位写1，用按位或（|）
}
/****************************************************
函数名：Delay_us
形参：time代表要延时的时间（单位us）
返回值：无
函数功能：精确us延时函数
****************************************************/
u32 TimeDelay = 0;
void Delay_us(u32 time)
{
	TimeDelay = time;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimeDelay !=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
/****************************************************
函数名：Delay_ms
形参：time代表要延时的时间（单位ms）
返回值：无
函数功能：精确ms延时函数
****************************************************/
void Delay_ms(u32 time)
{
	TimeDelay = time*1000;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimeDelay !=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
/****************************************************
函数名：SysTick_Handler
形参：无
返回值：无
函数功能：滴答时钟中断服务函数
****************************************************/

void SysTick_Handler(void)
{
	if(TimeDelay > 0)
		TimeDelay--;
}








