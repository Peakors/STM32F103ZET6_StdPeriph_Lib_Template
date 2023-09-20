#ifndef __IIC_H__
#define __IIC_H__
#include "stm32f10x.h"

#define IIC_SDA  GPIO_Pin_11   //at24c02与GY30进行共用
#define IIC_CLK  GPIO_Pin_10
#define IIC_GPIO GPIOB

#define READ_SDA() GPIO_ReadInputDataBit(IIC_GPIO,IIC_SDA)

#define SDA_H  GPIO_SetBits(IIC_GPIO,IIC_SDA)
#define CLK_H  GPIO_SetBits(IIC_GPIO,IIC_CLK)

#define SDA_L  GPIO_ResetBits(IIC_GPIO,IIC_SDA)
#define CLK_L  GPIO_ResetBits(IIC_GPIO,IIC_CLK)

void SDA_IN(void);
void SDA_OUT(void);

void IIC_Init(void);
void IIC_Stop(void);
void IIC_Start(void);
void IIC_Send_Data(u8 dat);
u8 IIC_Read_Data(u8 ack);
u8 IIC_Get_ACK(void);
void IIC_Send_ACK(void);
void IIC_Send_NoACK(void);

#endif 
