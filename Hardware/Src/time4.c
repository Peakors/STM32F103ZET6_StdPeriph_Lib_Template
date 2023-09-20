#include "time4.h"

/********************************************************************
*定 时 器 TIMER4 初 始 化
*********************************************************************/
void timer4_init(int TIM4_Period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	//开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);		//使能定时器5时钟
	//定时器结构体配置
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TIM4_Period;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	//配置中断
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//使能定时器中断，设置中断类型为更新中断
	//中断配置
	NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	//使能定时器
	TIM_Cmd(TIM4,ENABLE);//使能定时器
}

/********************************************************************
*timer中断服务函数
*********************************************************************/
int Direction = 0;
void TIM4_IRQHandler(void)
{
	static int num =0;
	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_Update)==1)//获取定时器3是否为更新中断
	{
		LED1_OFF;LED2_OFF;LED3_OFF;LED4_OFF;
		switch(num++%4)
		{
			case 0:
				Direction?LED1_ON:LED4_ON;
			break;
			case 1:
				Direction?LED2_ON:LED3_ON;
			break;
			case 2:
				Direction?LED3_ON:LED2_ON;
			break;
			case 3:
				Direction?LED4_ON:LED1_ON;
			break;
		}
		TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);  //清除中断标志位，必须手动清除
	}
}
