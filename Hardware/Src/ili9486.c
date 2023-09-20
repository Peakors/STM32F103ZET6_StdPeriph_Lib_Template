#include "ili9486.h"
#include "lcd_gui.h"

#define FSMC 1																			         //1：开启FSMC功能  0：启用8080通信
int LCD_Width,LCD_High;                                     //LCD屏尺寸大小

/***********************************************************************************************
函数名称  ：LCD_GPIO_Init
函数功能  ：把LCD液晶屏对应的管脚进行初始化
函数形参  ：无
函数返回值：无
函数描述
	硬件连接方式：
		数据线：
		DB0---PD14	DB1---PD15	DB2---PD0	DB3---PD1
		DB4---PE7	DB5---PE8	DB6---PE9	DB7---PE10
		DB8---PE11	DB9---PE12	DB10---PE13	DB11---PE14
		DB12---PE15	DB13---PD8	DB14---PD9	DB15---PD10
	控制线：
		WR---PD5	RD---PD4	CS---PG12	RS(D/C)---PG0	BL--PB0
************************************************************************************************/
void LCD_GPIO_Init(void)
{      
#if	FSMC
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC->AHBENR |= 1 << 8;     	    //使能FSMC时钟  
	/* 使能FSMC控制器功能管脚对应的GPIO端口 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  /* 使能GPIOB端口模块时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);  /* 使能GPIOD端口模块时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);  /* 使能GPIOE端口模块时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);  /* 使能GPIOG端口模块时钟 */
	/* FSMC控制器数据线 （16根数据线）*/
	/* 输出速度有要求要在20~40ns：50M 周期时间为：1/50M = 20ns */
	//PD组
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
	//PE组
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	/* FSMC控制器控制线（6-1=5根地址线）*/
	//PD组
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
	//PG组
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStruct);
	//PB组
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	/* FSMC控制器配置（LCD使用NE4，也就对应BTCR[6],[7]） */

	
	/* FSMC控制器功能模式  */
	FSMC_Bank1->BTCR[6] |= 1 << 14;	       //使用扩展模式（读写使用不同的时序）
	FSMC_Bank1->BTCR[6] |= 1 << 12;	       //存储器写使能
	FSMC_Bank1->BTCR[6] &= ~(3 << 4);      //清零
	FSMC_Bank1->BTCR[6] |= 1 << 4; 	       //存储器数据宽度为16bit 
	FSMC_Bank1->BTCR[6] &= ~(3 << 2);      //外设存储器位SRAM
	/* 读时序控制寄存器 */
	FSMC_Bank1->BTCR[7] &= ~(3 << 28);     //模式A
	FSMC_Bank1->BTCR[7] &= ~(0xFF << 8);   //数据保存时间清零
	FSMC_Bank1->BTCR[7] |= 0X16 << 8;  	   //数据保存时间(RD低电平持续时间355ns)60个HCLK = 13.8 * 15 = 260ns	
	FSMC_Bank1->BTCR[7] |= 0x09 << 0;      //地址建立时间(RD高电平持续时间90ns)15个HCLK = 13.8ns（1/72M） * 7=90ns	
    
	/* 写时序控制寄存器（LCD使用NE4，也就对应BWTR[6] */
	FSMC_Bank1E->BWTR[6] &= ~(3 << 28);    //模式A 
	FSMC_Bank1E->BWTR[6] &= ~(0xFF << 8);  //数据保存时间清零
	FSMC_Bank1E->BWTR[6] |= 4 << 8; 	     //数据保存时间(WR低电平持续时间15ns)1个HCLK = 13.8 * 2 =27.6ns
	FSMC_Bank1E->BWTR[6] &= ~(0x0F << 0);  //地址建立时间清零 	 
	FSMC_Bank1E->BWTR[6] |= 4 << 0;		     //地址建立时间(WD高电平持续时间15ns)1个HCLK =  13.8 * 2 =27.6ns
	
	/* 使能BANK1,区域4 */
	FSMC_Bank1->BTCR[6]|= 1<<0;		       //使能BANK1，块1的区域4	
	
	/* LCD背光控制线 */
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);    //关闭背光，在LCD初始化完后再打开       
#else
	GPIO_InitTypeDef GPIO_InitStructure;  							//定义GPIO端口结构体变量
	/* 使能LCD硬件管脚时钟 */
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG 
	                      | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF),ENABLE);
	/* 设置LCD硬件管脚模式 */
	/* DB0~DB15数据线：推挽输出  时钟50M */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
	                              GPIO_Pin_10 |GPIO_Pin_14 | GPIO_Pin_15;         
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                              GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	GPIO_Init(GPIOE,&GPIO_InitStructure);
		
	/* 控制线：推挽输出  时钟50M 空闲为高电平 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 ; 
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_4 | GPIO_Pin_5);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ; 
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_0);
	
	/* LCD背光 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);         //关闭背光，在LCD初始化完后再打开 	
#endif
}
/***********************************************************************************************
函数名称  ：LCD_CTRL_CMD
函数功能  ：MUC把相应的控制命令发送到显示控制器
函数形参  ：需要输入的命令
函数返回值：无
函数描述
	发送方式：
		GPIO_SetBits()    --- 发送数据“1”
		GPIO_ResetBits()  --- 发送数据“0”
***********************************************************************************************/
void LCD_CTRL_CMD(u16 common)
{
#if FSMC
TFTLCD_REG = common;
#else
	GPIO_ResetBits(GPIOG, GPIO_Pin_12);  									//CS拉低
	GPIO_ResetBits(GPIOG, GPIO_Pin_0);   									//D/C拉低
	GPIO_ResetBits(GPIOD, GPIO_Pin_5);   									//WR拉低
	/* 把命令发送到数据总线 */
	if(common & 0x01)   GPIO_SetBits(GPIOD,GPIO_Pin_14); else GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	if(common & 0x02)   GPIO_SetBits(GPIOD,GPIO_Pin_15); else GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	if(common & 0x04)   GPIO_SetBits(GPIOD,GPIO_Pin_0);  else GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	if(common & 0x08)   GPIO_SetBits(GPIOD,GPIO_Pin_1);  else GPIO_ResetBits(GPIOD, GPIO_Pin_1);
	if(common & 0x10)   GPIO_SetBits(GPIOE,GPIO_Pin_7);  else GPIO_ResetBits(GPIOE, GPIO_Pin_7);
	if(common & 0x20)   GPIO_SetBits(GPIOE,GPIO_Pin_8);  else GPIO_ResetBits(GPIOE, GPIO_Pin_8);
	if(common & 0x40)   GPIO_SetBits(GPIOE,GPIO_Pin_9);  else GPIO_ResetBits(GPIOE, GPIO_Pin_9);
	if(common & 0x80)   GPIO_SetBits(GPIOE,GPIO_Pin_10); else GPIO_ResetBits(GPIOE, GPIO_Pin_10);
	if(common & 0x0100) GPIO_SetBits(GPIOE,GPIO_Pin_11); else GPIO_ResetBits(GPIOE, GPIO_Pin_11);
	if(common & 0x0200) GPIO_SetBits(GPIOE,GPIO_Pin_12); else GPIO_ResetBits(GPIOE, GPIO_Pin_12);
	if(common & 0x0400) GPIO_SetBits(GPIOE,GPIO_Pin_13); else GPIO_ResetBits(GPIOE, GPIO_Pin_13);
	if(common & 0x0800) GPIO_SetBits(GPIOE,GPIO_Pin_14); else GPIO_ResetBits(GPIOE, GPIO_Pin_14);
	if(common & 0x1000) GPIO_SetBits(GPIOE,GPIO_Pin_15); else GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	if(common & 0x2000) GPIO_SetBits(GPIOD,GPIO_Pin_8);  else GPIO_ResetBits(GPIOD, GPIO_Pin_8);
	if(common & 0x4000) GPIO_SetBits(GPIOD,GPIO_Pin_9);  else GPIO_ResetBits(GPIOD, GPIO_Pin_9);
	if(common & 0x8000) GPIO_SetBits(GPIOD,GPIO_Pin_10); else GPIO_ResetBits(GPIOD, GPIO_Pin_10);
	GPIO_SetBits(GPIOD,GPIO_Pin_5);       								//WR拉高
	GPIO_SetBits(GPIOG,GPIO_Pin_0);       								//D/C命令数据选择线拉高
	GPIO_SetBits(GPIOG,GPIO_Pin_12);      								//拉高CS
#endif
}

/***********************************************************************************************
函数名称  ：LCD_CTRL_DATA
函数功能  ：MUC把相应的数据发送到显示控制器
函数形参  ：需要发送的数据
函数返回值：无
函数描述
	发送方式：
		GPIO_SetBits()    --- 发送数据“1”
		GPIO_ResetBits()  --- 发送数据“0”
***********************************************************************************************/
void LCD_CTRL_DATA(u16 data)
{
#if FSMC
	TFTLCD_RAM = data;
#else
	GPIO_ResetBits(GPIOG, GPIO_Pin_12);  										//CS拉低
	GPIO_SetBits(GPIOG, GPIO_Pin_0);     										//D/C拉高
	GPIO_ResetBits(GPIOD, GPIO_Pin_5);   										//WR拉低
	/* 把数据发送到数据总线 */
	if(data & 0x01)   GPIO_SetBits(GPIOD,GPIO_Pin_14); else GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	if(data & 0x02)   GPIO_SetBits(GPIOD,GPIO_Pin_15); else GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	if(data & 0x04)   GPIO_SetBits(GPIOD,GPIO_Pin_0);  else GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	if(data & 0x08)   GPIO_SetBits(GPIOD,GPIO_Pin_1);  else GPIO_ResetBits(GPIOD, GPIO_Pin_1);
	if(data & 0x10)   GPIO_SetBits(GPIOE,GPIO_Pin_7);  else GPIO_ResetBits(GPIOE, GPIO_Pin_7);
	if(data & 0x20)   GPIO_SetBits(GPIOE,GPIO_Pin_8);  else GPIO_ResetBits(GPIOE, GPIO_Pin_8);
	if(data & 0x40)   GPIO_SetBits(GPIOE,GPIO_Pin_9);  else GPIO_ResetBits(GPIOE, GPIO_Pin_9);
	if(data & 0x80)   GPIO_SetBits(GPIOE,GPIO_Pin_10); else GPIO_ResetBits(GPIOE, GPIO_Pin_10);
	if(data & 0x0100) GPIO_SetBits(GPIOE,GPIO_Pin_11); else GPIO_ResetBits(GPIOE, GPIO_Pin_11);
	if(data & 0x0200) GPIO_SetBits(GPIOE,GPIO_Pin_12); else GPIO_ResetBits(GPIOE, GPIO_Pin_12);
	if(data & 0x0400) GPIO_SetBits(GPIOE,GPIO_Pin_13); else GPIO_ResetBits(GPIOE, GPIO_Pin_13);
	if(data & 0x0800) GPIO_SetBits(GPIOE,GPIO_Pin_14); else GPIO_ResetBits(GPIOE, GPIO_Pin_14);
	if(data & 0x1000) GPIO_SetBits(GPIOE,GPIO_Pin_15); else GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	if(data & 0x2000) GPIO_SetBits(GPIOD,GPIO_Pin_8);  else GPIO_ResetBits(GPIOD, GPIO_Pin_8);
	if(data & 0x4000) GPIO_SetBits(GPIOD,GPIO_Pin_9);  else GPIO_ResetBits(GPIOD, GPIO_Pin_9);
	if(data & 0x8000) GPIO_SetBits(GPIOD,GPIO_Pin_10); else GPIO_ResetBits(GPIOD, GPIO_Pin_10);
	GPIO_SetBits(GPIOD,GPIO_Pin_5);       								//WR拉高
	GPIO_SetBits(GPIOG,GPIO_Pin_12);      								//拉高CS
#endif
}


/************************************************************************************************
函数名称  ：LCD_Init
函数功能  ：LCD初始化设置
函数形参  ：无
函数返回值：无
函数描述  ：
	初始化代码由液晶屏厂家提供，用户需要按实际使用方式修改部分参数
************************************************************************************************/
void LCD_Init(void)
{
	LCD_GPIO_Init();
	LCD_Width=240;
	LCD_High=320;
//************* Start Initial Sequence **********//
	LCD_CTRL_CMD(0xCF);
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_DATA (0x83);
	LCD_CTRL_DATA (0X30);
	LCD_CTRL_CMD(0xED);
	LCD_CTRL_DATA (0x64);
	LCD_CTRL_DATA (0x03);
	LCD_CTRL_DATA (0X12);
	LCD_CTRL_DATA (0X81);
	LCD_CTRL_CMD(0xE8);
	LCD_CTRL_DATA (0x85);
	LCD_CTRL_DATA (0x01);
	LCD_CTRL_DATA (0x79);
	LCD_CTRL_CMD(0xCB);
	LCD_CTRL_DATA (0x39);
	LCD_CTRL_DATA (0x2C);
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_DATA (0x34);
	LCD_CTRL_DATA (0x02);
	LCD_CTRL_CMD(0xF7);
	LCD_CTRL_DATA (0x20);

	LCD_CTRL_CMD(0xEA);
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_CMD(0xC0);																		  //Power control
	LCD_CTRL_DATA (0x1D); 																	//VRH[5:0]
	LCD_CTRL_CMD(0xC1); 																		//Power control
	LCD_CTRL_DATA (0x11); 																	//SAP[2:0];BT[3:0]
	LCD_CTRL_CMD(0xC5); 																		//VCM control
	LCD_CTRL_DATA (0x33);
	LCD_CTRL_DATA (0x34);
	LCD_CTRL_CMD(0xC7); 																		//VCM control2
	LCD_CTRL_DATA (0Xbe);
	LCD_CTRL_CMD(0x36); 																		// Memory Access Control
	LCD_CTRL_DATA (0x08);
	LCD_CTRL_CMD(0xB1);
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_DATA (0x1B);
	LCD_CTRL_CMD(0xB6); 																		// Display Function Control
	LCD_CTRL_DATA (0x0A);
	LCD_CTRL_DATA (0xA2);
	LCD_CTRL_CMD(0xF2);																		 	// 3Gamma Function Disable
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_CMD(0x26); 																		//Gamma curve selected
	LCD_CTRL_DATA (0x01);
	LCD_CTRL_CMD(0xE0); 																		//Set Gamma
	LCD_CTRL_DATA (0x0F);
	LCD_CTRL_DATA (0x23);
	LCD_CTRL_DATA (0x1F);

	LCD_CTRL_DATA (0x09);
	LCD_CTRL_DATA (0x0f);
	LCD_CTRL_DATA (0x08);
	LCD_CTRL_DATA (0x4B);
	LCD_CTRL_DATA (0Xf2);
	LCD_CTRL_DATA (0x38);
	LCD_CTRL_DATA (0x09);
	LCD_CTRL_DATA (0x13);
	LCD_CTRL_DATA (0x03);
	LCD_CTRL_DATA (0x12);
	LCD_CTRL_DATA (0x07);
	LCD_CTRL_DATA (0x04);
	LCD_CTRL_CMD(0XE1); 																		//Set Gamma
	LCD_CTRL_DATA (0x00);
	LCD_CTRL_DATA (0x1d);
	LCD_CTRL_DATA (0x20);
	LCD_CTRL_DATA (0x02);
	LCD_CTRL_DATA (0x11);
	LCD_CTRL_DATA (0x07);
	LCD_CTRL_DATA (0x34);
	LCD_CTRL_DATA (0x81);
	LCD_CTRL_DATA (0x46);
	LCD_CTRL_DATA (0x06);
	LCD_CTRL_DATA (0x0e);
	LCD_CTRL_DATA (0x0c);
	LCD_CTRL_DATA (0x32);
	LCD_CTRL_DATA (0x38);
	LCD_CTRL_DATA (0x0F);
	
	LCD_CTRL_CMD(0x11); 																				// Sleep out
	Delay_ms(120);
	LCD_CTRL_CMD(0x29); 																				// Display on
	/* 由用户按实际情况添加 */
	LCD_CTRL_CMD(0X3A);            															//设定LCD颜色位深
	LCD_CTRL_DATA(0X55);      																	//LCD颜色位深为16BPP
	LCD_CTRL_CMD(0X36);            															//设置LCD扫描方向 
	LCD_CTRL_DATA(0X08);      																	//扫描方向为从上到下，从左到右
	LCD_Clear(0,LCD_Width-1,0,LCD_High-1,RGB(255,255,255));  				//把LCD清成白屏
	GPIO_SetBits(GPIOB,GPIO_Pin_0);   													//开启背光
}



/************************************************************************************************
函数名称  ：LCD_Clear
函数功能  ：LCD液晶屏清屏，设置LCD液晶显示屏的屏幕底色
函数形参  ：SX、EX：X轴的起始坐标和结束坐标；SY、EY：Y轴的起始坐标和结束坐标；Color： 屏幕颜色
函数返回值：无
函数描述  ：
	设置LCD屏坐标，每个坐标要分高8位和低8位两次发送
************************************************************************************************/
void LCD_Clear(u16 SX, u16 EX, u16 SY, u16 EY, u16 Color)
{
	u32 i;
	CurrentPosition = 0;
	/* 设置清屏坐标 */
	LCD_CTRL_CMD(0x2A);                   							//发送命令，设置X轴坐标
	LCD_CTRL_DATA((SX & 0xff00) >> 8);    							//发送起始坐标的高8位
	LCD_CTRL_DATA((SX & 0x00ff));         							//发送起始坐标的低8位
	LCD_CTRL_DATA((EX & 0xff00) >> 8);    							//发送结束坐标的高8位
	LCD_CTRL_DATA((EX & 0x00ff));         							//发送结束坐标的低8位
	
	LCD_CTRL_CMD(0x2B);                   							//发送命令，设置Y轴坐标
	LCD_CTRL_DATA((SY & 0xff00) >> 8);    							//发送起始坐标的高8位
	LCD_CTRL_DATA((SY & 0x00ff));         							//发送起始坐标的低8位
	LCD_CTRL_DATA((EY & 0xff00) >> 8);    							//发送结束坐标的高8位
	LCD_CTRL_DATA((EY & 0x00ff));         							//发送结束坐标的低8位
	
	LCD_CTRL_CMD(0x2C);                        					//发送命令，设置颜色
	for(i = 0; i < ((EX - SX + 1) * ((EY - SY + 1))); i++)
	{
		LCD_CTRL_DATA(Color);             								//输入LCD清屏颜色
	}
}


