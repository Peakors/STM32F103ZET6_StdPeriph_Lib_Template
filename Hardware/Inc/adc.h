#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdio.h>

/* ADC通道宏定义 */
#define Mq2    1
#define MQ135_val 2
#define Rain 3
#define Soil 4


void Adc_Init(void);
u16 Get_adcvalue(u8 device);
u16 ADC_Val_Disp(u8 device_num);

#endif

