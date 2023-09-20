#include "usart2.h"

/********************************************************************
*Description：USART_2模块中断初始化
*Boand：串口使用的波特率
*Explain：RX ---> PA2、TX ---> PA3
*********************************************************************/
void USART_2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;     /* 定义GPIO模块结构体类型变量 */
	USART_InitTypeDef USART_InitStruct;  /* 定义USART模块结构体类型变量 */
	NVIC_InitTypeDef NVIC_InitStructure;  /* 定义NVIC中断结构体类型变量 */
	/* 设置UASART模块功能管脚 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  /* 使能GPIO端口模块时钟 */
	/* USART1_RX（PA2）浮空输入 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	/* USART1_TX（PA3）复用推挽输出 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	/* 设置USART模块工作模式 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 					/* 使能USART_3模块发送和接收*/
	USART_InitStruct.USART_BaudRate = bound;                      				/* 设置USART_3模块波特率 */
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;     					/* USART_3模块8位数据长度 */
	USART_InitStruct.USART_Parity = USART_Parity_No;               				/* USART_3模块禁止奇偶校验 */
	USART_InitStruct.USART_StopBits = USART_StopBits_1;            				/* USART_3模块1位停止位 */
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* 禁止硬件流 */
	USART_Init(USART2,&USART_InitStruct);      														/* 使用USART_InitStruct 参数初始化USART_3模块 */
	/* USART模块NVIC 配置 */
	/* NVIC中断优先级分组为第一组 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ;  					/* 抢占优先级等级为1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		    						/* 响应优先级等级为3 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;												/* 使能中断源NVIC中断 */
	NVIC_Init(&NVIC_InitStructure);	 																			/* 使用NVIC_InitStructure 参数初始化NVIC控制器 */
	/* 开启USART模块中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);    										/*接收中断*/ 
	/* 使能USART模块 */
	USART_Cmd(USART2, ENABLE);
}

/********************************************************************
* Description：串口2发送字符
* Str：需要发送的数据
*********************************************************************/
void Usart2_Send_n_Byte(char *Str,int n)
{
	int i;
	for(i = 0; i < n ;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET) 
		{
			/* 等待发送完成 */
		}
		USART_SendData(USART2,Str[i]);   /* 发送字符串中的字符 */
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET) 
		{
			/* 等待发送完成 */
		}
		
	}
}
/********************************************************************
* Description：USART_2中断服务函数
*********************************************************************/
void USART2_IRQHandler(void)
{
	//如果为接收中断
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)           //判断串口1发生的中断是否为接收中断
	{
		/*modbus数组接收数据下标进行加加操作*/
		if(wifi.CurrentSubscript < 510)
		{
				wifi.ReceiveBuff[(wifi.CurrentSubscript++)] = USART_ReceiveData(USART2);
		}
		/*如果为第一帧数据将开启定时器*/
			
		if(wifi.CurrentSubscript == 1)
		{
			wifi.TimerRunFlag =1;
		}
		//定时器归零操作
		wifi.CurrentTimer = 0 ;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}
