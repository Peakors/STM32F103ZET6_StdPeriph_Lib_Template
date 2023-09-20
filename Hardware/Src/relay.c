#include "relay.h"

/****************************************
函数功能:初始化继电器连接的IO
输入参数:传值方式皆为输入参数	
输出参数:传地址无const修饰的皆为输出参数	
函数返回值:无
硬件连接说明:忽略
*****************************************/
void RELAYInit(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStruct);
		
}
