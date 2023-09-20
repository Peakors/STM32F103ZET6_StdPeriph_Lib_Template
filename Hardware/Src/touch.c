#include "touch.h"


/**************************** 触摸屏硬件管脚初始化函数 ****************************
功  能：把触摸屏对应的管脚进行初始化
参  数：无
返回值：无
硬件连接方式：
XTP2046    LCD屏     MCU
CLK       T_CLK      PB1       推挽输出
CS        T_CS       PB2       推挽输出
DIN       T_MOSI     PF9       推挽输出
DOUT      T_MISO     PF8       浮空输入
PEN       T_PEN      PF10      浮空输入
***********************************************************************************/
void Touch_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF,ENABLE);
	
	/* PB2 -- T_CS -- CS  PB1 -- CLK -- DCLK */  
	//片选线---时钟线  都是有单片机控制---->推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_2;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/* PF9 -- MOSI -- DIN */
	//推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* PF8 -- MISO -- DOUT PF10 -- T_PEN -- PEN */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/*PF10 -- T_PEN -- PEN 笔接触中断位    当有触摸动作时---》XPT2046输出低电平*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	\
}

/**************************** 触摸屏数据传输函数 ****************************
功  能：MCU发送数据到触摸屏控制器或从触摸屏控制器读取输出
参  数：需要发送的数据
返回值：读取到的数据
***************************************************************************/
u16 Touch_Get_ADC(u8 common)
{
	u8 i;
	u16 data = 0;
	            
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);                          //拉低片选
	                                                         
	/* 发送控制字节 */                                        
	for(i = 0; i < 8; i++)                                   
	{      
    //时钟线----》时钟脉冲	
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);                      //拉低时钟线，准备数据
		if(common & 0x80)            //1000 0000                                  
		{                                                      
			GPIO_SetBits(GPIOF,GPIO_Pin_9);                      //拉高MOSI数据线，输出1
		}                                                      
		else                                                   
		{                                                      
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);                      //拉低MOSI数据线，输出0
		}                                                      
		Delay_us(1);                                           
		GPIO_SetBits(GPIOB,GPIO_Pin_1);                           //时钟产生上升沿，发出数据
		Delay_us(1);                                           
		common <<= 1;                                             //传次高位                                          
	}                                                        
	                                                         
	/* 产生一个空闲时钟 */    
   //XPT2046转换需要的时间	---一个时钟脉冲
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);	                       
	Delay_us(1);	                                           
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	Delay_us(1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);

	/* 读数据 */
	for(i = 0; i < 12; i++)
	{
		data <<= 1;          
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);   //拉低时钟线，准备数据
		Delay_us(1);          
		GPIO_SetBits(GPIOB,GPIO_Pin_1);     //时钟产生上升沿，发出数据
		if( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8) )
		{
			data |= 1;
		}
		Delay_us(1);
	}
	
	/* 空闲状态 */            
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);    //拉低时钟线             
	GPIO_SetBits(GPIOB,GPIO_Pin_2);     //拉高片选              
	GPIO_ResetBits(GPIOF,GPIO_Pin_9);   //拉低数据线

	return data;
}

/**************************** 触摸屏坐标获取函数 ****************************
功  能：读取触摸屏ADC转换后的数据量
参  数：无
返回值：读取触摸屏的X、Y轴坐标值
***************************************************************************/
TOUCH_TYPE_DEF Touch_Get_XY(void)
{
	TOUCH_TYPE_DEF touch;
	u16 buff[10],temp;
	u8 i,j;
	
	/* 获取X轴坐标AD值 */
	for(i = 0; i < 10; i++)
	{
		buff[i] = Touch_Get_ADC(0xD0);           //读取X轴AD值
	}
	
	/* 数据排序 冒泡排序   懵逼定理*/
	for(i = 0; i < 10; i++)
	{
		for(j = i + 1; j < 9; j++)
		{
			if(buff[i] < buff[j])
			{
				temp = buff[i];
				buff[i] = buff[j];
				buff[j] = temp;
			}
		}
	}
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += buff[i];
	}
	/* 求平均数 */
	touch.x = temp / 8;     
	
	/* 获取Y轴坐标AD值 */
	for(i = 0; i < 10; i++)
	{
		buff[i] = Touch_Get_ADC(0x90);    //读取Y轴AD值
	}
	
	/* 数据排序 */
	for(i = 0; i < 10; i++)
	{
		for(j = i + 1; j < 9; j++)
		{
			if(buff[i] < buff[j])
			{
				temp = buff[i];
				buff[i] = buff[j];
				buff[j] = temp;
			}
		}
	}
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += buff[i];
	}
	 /* 求平均数 */
	touch.y = temp / 8;  
	
	return touch;
}


TOUCH_TYPE_DEF TOUCH_LCD;
/**************************** 触摸屏初始化校准函数 ****************************
功  能：计算出Kx、Bx、Ky、By的值
参  数：无
返回值：无
校准方式：四点校准
*****************************************************************************/
void Touch_Adj_Init(void)
{
	float f1,f2;
	TOUCH_TYPE_DEF touch[4];

RE_ADJUST:
	//第一个点
	TP_draw_adjust_point(20, 20, 0xf800);         //画一个十字
	while( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10) )      
	{
		/* 等待触摸屏被按下 */
	}
	Delay_ms(10);                                 //延时去抖
	touch[0]=Touch_Get_XY( );                     //读取触摸屏的XY轴值
	while( !(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)) )   
	{
		/* 等待释放触摸屏 */
	}	
	TP_draw_adjust_point(20 ,20,0x2744);           //清除第一个十字
	
	//第二个点
	TP_draw_adjust_point((LCD_Width - 20),20, 0xf800);       //画一个十字
	while( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10) )      
	{
		/* 等待触摸屏被按下 */
	}
	Delay_ms(10);                     //延时去抖
	touch[1]=Touch_Get_XY( );                //读取触摸屏的XY轴值
	while( !(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)) )  
	{
		/* 等待释放触摸屏 */
	}	
	TP_draw_adjust_point((LCD_Width - 20), 20, 0x2744);         //清除第二个十字
	
	//第三个点
	TP_draw_adjust_point(20, (LCD_High - 20), 0xf800);         //画一个十字
	while( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10) )      
	{
		/* 等待触摸屏被按下 */
	}
	Delay_ms(10);                       //延时去抖
	touch[2]=Touch_Get_XY( );                  //读取触摸屏的XY轴值
	while( !(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)) )  
	{
		/* 等待释放触摸屏 */
	}	
	TP_draw_adjust_point(20,(LCD_High - 20), 0x2744);           //清除第三个十字
	
	//第四个点
	TP_draw_adjust_point((LCD_Width- 20), (LCD_High - 20), 0xf800);     //画一个十字
	while( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10) )      
	{
		/* 等待触摸屏被按下 */
	}
	Delay_ms(10);                         //延时去抖
	touch[3]=Touch_Get_XY( );                    //读取触摸屏的XY轴值
	while( !(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)) )  
	{
		/* 等待释放触摸屏 */
	}	
	TP_draw_adjust_point((LCD_Width - 20), (LCD_High - 20), 0x2744);     //清除第四个十字
	
	//第 1 2点间的距离的平方
	f1=(touch[1].x-touch[0].x)*(touch[1].x-touch[0].x) +
	   (touch[1].y-touch[0].y)*(touch[1].y-touch[0].y);
	
	//第3 4点间的距离的平方
	f2=(touch[3].x-touch[2].x)*(touch[3].x-touch[2].x) +
	   (touch[3].y-touch[2].y)*(touch[3].y-touch[2].y);
	
	if(f1 / f2 > 1.2 || f1 / f2 < 0.8)
	{
		goto RE_ADJUST;
	}
	//有效0.8-----1.2倍
	//第 1 3点间的距离的平方
	f1=(touch[2].x-touch[0].x)*(touch[2].x-touch[0].x) +
	   (touch[2].y-touch[0].y)*(touch[2].y-touch[0].y);
	
	//第2 4点间的距离的平方
	f2=(touch[3].x-touch[1].x)*(touch[3].x-touch[1].x) +
	   (touch[3].y-touch[1].y)*(touch[3].y-touch[1].y);
	
	if(f1 / f2 > 1.2 || f1 / f2 < 0.8)
	{
		goto RE_ADJUST;
	}
	//有效0.8-----1.2倍
	//第 1 4点间的距离的平方
	f1 = (touch[3].x-touch[0].x)*(touch[3].x-touch[0].x) +
	     (touch[3].y-touch[0].y)*(touch[3].y-touch[0].y);
	
	//第2 3点间的距离的平方
	f2 = (touch[2].x-touch[1].x)*(touch[2].x-touch[1].x) +
	     (touch[2].y-touch[1].y)*(touch[2].y-touch[1].y);
	
	if(f1 / f2 > 1.2 || f1 / f2 < 0.8)
	{
		goto RE_ADJUST;
	}
	//
	//求x方向的比例系数和偏移量y=ax+b
	TOUCH_LCD.kx =(float)((LCD_Width - 20) - 20) / (touch[3].x - touch[0].x);
	TOUCH_LCD.offset_x = 20 - TOUCH_LCD.kx * touch[0].x;
	//求y方向的比例系数和偏移量
	TOUCH_LCD.ky = (float)((LCD_High - 20) - 20) / (touch[3].y - touch[0].y);
	TOUCH_LCD.offset_y = 20 - TOUCH_LCD.ky * touch[0].y;
	printf("%f %f %d %d\r\n",TOUCH_LCD.kx,TOUCH_LCD.ky,TOUCH_LCD.offset_x,TOUCH_LCD.offset_y);
	//0.293686 0.082523 -558 -8
}

/**************************** 触摸屏坐标转换成LCD坐标函数 ****************************
功  能：把触摸屏的坐标和LCD液晶屏的实际坐标关联起来
参  数：无
返回值：无
计算公式：Lcd坐标 = f(比例系数) * Toch坐标 + offset(偏移量
************************************************************************************/
TOUCH_TYPE_DEF Touch_Lcd(void)
{
	TOUCH_TYPE_DEF a1,a2;
	
	a1 = Touch_Get_XY( );                              //获取触摸屏AD转换值
		a1.x=TOUCH_LCD.kx *a1.x +TOUCH_LCD.offset_x;
		a1.y=TOUCH_LCD.ky *a1.y +TOUCH_LCD.offset_y;
	
	a2 = Touch_Get_XY( );                              //获取触摸屏AD转换值
		a2.x=TOUCH_LCD.kx *a2.x +TOUCH_LCD.offset_x; 
		a2.y=TOUCH_LCD.ky *a2.y +TOUCH_LCD.offset_y;
	
	if(abs(a1.x-a2.x)<5 &&abs(a1.y-a2.y)<5)
	{
		a1.x=(a1.x+a2.x)/2;
		a1.y=(a1.y+a2.y)/2;
	}
	else
	{
		a1.x=0xffff;
		a1.y=0xffff;
	}
	
	return a1;
}

/**************************** 触摸屏坐标扫描函数 ****************************
功  能：获取触摸屏坐标值
参  数：无
返回值：返回触摸屏的实时触摸坐标
***************************************************************************/
TOUCH_TYPE_DEF Touch_Scanf(void)
{
	static u8 Touch_Sta = 1;
	
	TOUCH_TYPE_DEF touch_key[2];
	TOUCH_TYPE_DEF touchkey = {0xffff,0xffff};
	
	if( (Touch_Sta == 1)&&(!(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10))) )            //允许是被并且触摸屏有按下
	{
		
		Delay_ms(5);                                     //延时去抖
		if( !(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)) )
		{
			
			touch_key[0] = Touch_Lcd();
			touch_key[1] = Touch_Lcd();
			
			/* 求绝对值 */
			if( (abs(touch_key[0].x -touch_key[1].x)<5) &&
			    (abs(touch_key[0].y -touch_key[1].y)<5) )       //识别到坐标有效
			{
				touchkey.x = (touch_key[0].x +touch_key[1].x)/2;
				touchkey.y = (touch_key[0].y +touch_key[1].y)/2;    
				
				Touch_Sta=0;                                     //不允许识别

			}
			else
			{
				touch_key[0].x = 0xffff;
				touch_key[0].y = 0xffff;
			}
		}
	}
	else if( GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10))
	{
		Touch_Sta = 1;                                           //允许识别
	}
	
	return touchkey;
}


/**************************** 触摸屏初始化函数 ****************************
功  能：初始化触摸屏
参  数：无
返回值：无
***************************************************************************/
void Touch_Init(void)
{
	Touch_Gpio_Init();     				//GPIO初始化                                                      //触摸屏硬件初始化
//	Touch_Adj_Init();      				//校准  
//printf("%f %f %d %d\r\n",TOUCH_LCD.kx,TOUCH_LCD.ky,TOUCH_LCD.offset_x,TOUCH_LCD.offset_y);
	//0.293686 0.082523 -558 -8	//触摸屏校准
	TOUCH_LCD.kx = 0.325733;
	TOUCH_LCD.ky = 0.084287;
	TOUCH_LCD.offset_x = -898 ;
	TOUCH_LCD.offset_y = -15;

}







