#include "beep.h"
/********************************
函数名：void Beep_GPIO_Init(void)
函数功能：蜂鸣器连接GPIO口初始化
参数：无
返回值：无
*********************************/
void BEEPGpioInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                    /* 定义一个结构体类型变量 */
	//打开B组时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    /* 使能GPIOB时钟 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;              /* 使用端口编号8 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        /* 输出速度50M */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;        /* 推挽输出功能 */
	GPIO_Init(GPIOB, &GPIO_InitStructure);                   /* 初始化GPIOE组 */
}

