#include "led.h"

/****************************************
函数功能:初始化LED连接的IO
输入参数:传值方式皆为输入参数	
输出参数:传地址无const修饰的皆为输出参数	
函数返回值:无
硬件连接说明:忽略
*****************************************/
void LEDInit(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		/******************GPIOE***************/
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOE,&GPIO_InitStruct);
		/******************GPIOB***************/
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		//全部关闭
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}
