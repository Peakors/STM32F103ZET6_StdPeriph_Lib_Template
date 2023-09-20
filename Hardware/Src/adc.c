#include "adc.h"


/**************************** ADC模块初始化函数 ****************************
功  能：初始ADC_1模块
参  数：无
返回值：无
硬件连接：
			MQ2 	（可燃气体）   		---> ADC1_IN10 --->PC0
***************************************************************************/
void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* 开启GPIO口和ADC的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1,ENABLE);
	/* 设置ADC模块工作频率 72/6 = 12M 不超过14MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	/* 设置ADC通道管脚对应的GPIOA234 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                          //模拟输入引脚
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/* 设置ADC模块工作模式 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //ADC独立模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                     //单次循环模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                           //单通道
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //打开软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //数据右对齐 -- 低12位
	ADC_InitStructure.ADC_NbrOfChannel = 1;                                 //顺序转换的规则组的通道数目
	ADC_Init(ADC1,&ADC_InitStructure);                                      //根据参数初始化外设
	
	/* 使能ADC模块 */
	ADC_Cmd(ADC1,ENABLE);                                                   //开启ADC1
	
	/* ADC模块校验 */
	ADC_ResetCalibration(ADC1);																							//重置ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));															//获取ADC1重置校准寄存器的状态,设置状态则等待

	ADC_StartCalibration(ADC1);					    																//开始ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));		   														 //等待校准完成
}

/**************************** ADC转换值获取函数 ****************************
功  能：获取ADC模块转换后的数值
参  数：无
返回值：存放ADC转换值
***************************************************************************/

u16 Get_adcvalue(u8 device)
{
	u16 Buff[10] = {0};
	u16 temp = 0;
	u8 i,j;
	switch(device)
	{
		case Mq2:  ADC_RegularChannelConfig(ADC1,10, 1,ADC_SampleTime_239Cycles5); break;  //2---PA2设置指定ADC的规则组通道，一个序列，采样时间
	}

	for(i = 0; i < 10; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);               //软件启动ADC转换
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET)
		{
			/* 等待转换完成 */
		}
		Buff[i] = ADC_GetConversionValue(ADC1);             //读取ADC数据寄存器中的值
	}
	/* ADC软件滤波 */
	for(i = 0; i < 9; i++)
	{
		for(j = i + 1; j < 10; j++)
		{
			if(Buff[i] > Buff[j])
			{
				temp = Buff[i];
				Buff[i] = Buff[j];
				Buff[j] = temp;
			}
		}
	}
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += Buff[i];
	}
	return (temp / 8);
}

/*************************** ADC转换结果显示函数 **************************
函数功能：ADC最终结果函数
参数：设备名称
返回值：无
**************************************************************************/
u16 ADC_Val_Disp(u8 device_num)
{
	u16 ADC_Temp = 0;                     //ADC转换后的数字量
	/* 获取ADC转换值 */
	ADC_Temp = Get_adcvalue(device_num);
	/**/
	switch(device_num)
	{
		case Mq2:return (u16)((((5000.0 - 300) / 4096) * ADC_Temp) + 300);
	}
	return 0;	
}
