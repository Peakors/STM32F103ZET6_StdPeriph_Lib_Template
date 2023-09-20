#include "usart.h"

/********************************
函数名：void USART_GPIO_Init(void)
函数功能：串口连接GPIO口初始化
参数：无
返回值：无
*********************************/
void USART_GPIO_Init(void)
{
		//定义结构体变量
		GPIO_InitTypeDef  GPIO_InitStructure;   /* 定义一个结构体类型变量 */
		//开启时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;              /* 使用端口编号9 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        /* 输出速度50M */
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;         /* 复用推挽输出功能 */
		GPIO_Init(GPIOA,&GPIO_InitStructure);                    /* 初始化GPIOA组 */
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;             /* 使用端口编号10 */
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;   /* 浮空输入功能 */
		GPIO_Init(GPIOA,&GPIO_InitStructure);                   /* 初始化GPIOA组 */
}
/********************************
函数名：void USART_Config(u32 baud)
函数功能：USART初始化
参数：波特率  9600   115200
返回值：无
*********************************/
void USART_Config(u32 baud)
{
		USART_InitTypeDef USART_InitStruct;
		//初始化GPIO
		USART_GPIO_Init();
		//开启串口控制器时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);    /* 开启USART时钟 */
		USART_InitStruct.USART_BaudRate = baud;                  /* 波特率 */
		USART_InitStruct.USART_WordLength = USART_WordLength_8b; /* 数据位8位 */
		USART_InitStruct.USART_StopBits = USART_StopBits_1;      /*停止位1位*/
		USART_InitStruct.USART_Parity   = USART_Parity_No ;      /*无奇偶校验*/
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*硬件流失能*/
		USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;/*开启接收发送失能*/
		USART_Init(USART1,&USART_InitStruct);                     /*usart初始化*/
		NVIC_Config();																						/*配置中断矢量控制器*/
		//一个一个字符
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);            /*开启usart接收中断*/
		//空闲中断判定一帧数据结束
		USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);						  /*串口配置为空闲中断*/
		USART_Cmd(USART1,ENABLE);																	/*开启usart模块*/
}
/*************************************
函数名：void NVIC_Config(void)
函数功能：中断矢量控制器配置   串口1
参数：无
返回值：无
**************************************/
void NVIC_Config(void)
{
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;			        /*开启usart1全局中断*/
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;      /*抢占优先级1*/
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;              /*响应优先级1*/
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;                 /*中断源*/
		NVIC_Init(&NVIC_InitStruct);
}
/*************************************
函数名：void USART_Send_Data(u8 * data)
函数功能：USART发送函数
参数：数据首地址   "1111111111\r\n"
返回值：无
**************************************/
void USART_Send_Data(u8 * data)
{
		while(* data != '\0')
		{
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);     /*等待到为1---没有数据*/
			USART_SendData(USART1, * data++);                             /*发送数据并对数据地址进行偏移*/
		}
}

/********************************
函数名：u8 USART_Recv_Data(void)
函数功能：USART接收函数
参数：无
返回值：一个字符
*********************************/
u8 USART_Recv_Data(void)
{
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE )==RESET);     /*等待数据*/
		return USART_ReceiveData(USART1);                              /*发送数据并对数据地址进行偏移*/
}
/***************** printf打印函数 **********************
fputc函数是printf函数的底层输出函数
重定向--->重新定义输出方向--->printf--->屏幕
printf--->串口
********************************************************/
//int fputc(int data, FILE* file)
//{
//		USART1->DR = data;                                             //把需要发送的数据写入数据寄存器
//		while( !(USART1->SR & (1 << 7)) )
//		{
//			/* 等待发送收完成 */
//		}
//		return data;
//}
/************多串口实现printf打印通用函数****************
vsnprintf函数是printf函数的底层规格数据函数
********************************************************/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
		//剖析linux及libc源码得到该操作
		unsigned char UsartPrintfBuf[1024];
		va_list ap;
		unsigned char *pStr = UsartPrintfBuf;
		va_start(ap, fmt);
		vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);	//将数据规格格式化
		va_end(ap);
		while(*pStr != 0)
		{
			USART_SendData(USARTx, *pStr++);
			while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		}
}
/********************************
函数名：void USART1_IRQHandler(void)
函数功能：中断服务函数
参数：无
返回值：无
*********************************/
u8 uart1_count = 0;
char USART1_Rcv_Buff[128];                                                  
int uart1_finish_flage = 0;
void USART1_IRQHandler(void)
{
		int a;
		if((uart1_finish_flage == 0)&&(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET))//判断串口1发生的中断是否为接收中断
		{
			USART1_Rcv_Buff[uart1_count++] = USART_ReceiveData(USART1);                    //接收数据
		}
		if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)                            //空闲中断
		{
			a=USART1->SR;
			a=USART1->DR;                                                                 //标志位清零
			uart1_finish_flage=1;
			USART_ClearITPendingBit(USART1, USART_IT_IDLE);                              // 清除空闲中断
		}
}

