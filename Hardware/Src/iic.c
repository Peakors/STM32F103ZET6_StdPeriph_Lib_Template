#include "iic.h"
#include "sys_tick.h"

/*******************************************************************
函数功能：   SDA初始化
*******************************************************************/
void SDA_IN(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    /*SDA-----PB11*/ //浮空输入
    GPIO_InitStruct.GPIO_Pin = IIC_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_GPIO, &GPIO_InitStruct);
}

/*******************************************************************
函数功能：   SDA初始化
*******************************************************************/
void SDA_OUT(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    /*SDA-----PB11*/ //推挽输出 50MHZ
    GPIO_InitStruct.GPIO_Pin = IIC_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_GPIO, &GPIO_InitStruct);
}

/*******************************************************************
函数功能：   
*******************************************************************/
void IIC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能B时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//开启AFIO时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /*SDA-----PB11*/ //推挽输出 50MHZ
    GPIO_InitStruct.GPIO_Pin = IIC_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_GPIO, &GPIO_InitStruct);

    /*CLK-----PB10*/ //推挽输出 50MHZ
    GPIO_InitStruct.GPIO_Pin = IIC_CLK;
    GPIO_Init(IIC_GPIO, &GPIO_InitStruct);

    CLK_H;
    SDA_H;
}

/*******************************************************************
函数功能：   起始信号
*******************************************************************/
void IIC_Start(void) {
    SDA_OUT();
    SDA_H;                            //串行数据线高电平（空闲信号）
    CLK_H;                            //串行时钟线高电平（空闲信号）
    Delay_us(4);
    SDA_L;                            //串行数据线拉出下降沿
    Delay_us(4);
    CLK_L;                            //串行时钟线拉出下降沿
}

/*******************************************************************
函数功能：   结束信号
*******************************************************************/
void IIC_Stop(void) {
    SDA_OUT();                    //输出模式
    CLK_L;                            //串行时钟线低电平
    SDA_L;                            //串行数据线低电平
    CLK_H;                            //串行时钟线高电平
    Delay_us(4);
    SDA_H;                            //串行数据线高电平  上升沿
    Delay_us(4);

}

/*******************************************************************
函数功能：   发送应答信号
*******************************************************************/
void IIC_Send_ACK(void) {

    CLK_L;
    SDA_OUT();                            //输出模式
    SDA_L;                                        //串行数据线低电平
    Delay_us(2);
    CLK_H;                                        //串行时钟线线高电平    上下
    Delay_us(2);
    CLK_L;                                        //串行时钟线线低电平
}

/*******************************************************************
函数功能：   发送非应答信号
*******************************************************************/
void IIC_Send_NoACK(void) {
    CLK_L;
    SDA_OUT();
    SDA_H;                                            //串行数据线为高电平
    Delay_us(2);
    CLK_H;
    Delay_us(2);
    CLK_L;
}

/*******************************************************************
函数功能：   发送8位数据
*******************************************************************/
void IIC_Send_Data(u8 dat) {
    u8 i;
    SDA_OUT();
    CLK_L;
    for (i = 0; i < 8; i++)                    //分8次传输数据  一位一位传递  串行
    {
        if (dat & 0x80)                                //先发最高位  1000 0000
        {
            SDA_H;     //写1
        } else {
            SDA_L;     //写0
        }
        dat <<= 1;                                    //左移操作  次高位-->最高位
        Delay_us(2);
        CLK_H;
        Delay_us(2);
        CLK_L;
        Delay_us(2);
    }
}

/*******************************************************************
函数功能：   等待应答
*******************************************************************/
u8 IIC_Get_ACK(void) {
    u8 ERRTIME = 0;    //超时变量
    SDA_IN();
    SDA_L;
    Delay_us(1);
    CLK_H;
    Delay_us(1);
    while (READ_SDA()) {
        ERRTIME++;
        if (ERRTIME >= 250) {
            IIC_Stop();
            return 1;
        }
    }
//	Delay_us(5);
    CLK_L;
    return 0;
}

/*******************************************************************
函数功能：   读取8位数据
*******************************************************************/
u8 IIC_Read_Data(u8 ack) {
    unsigned char i, date = 0;
    SDA_IN();
    for (i = 0; i < 8; i++) {

        CLK_L;
        Delay_us(2);
        CLK_H;
        date <<= 1;
        if (READ_SDA()) {
            date++;
        }
        Delay_us(2);
    }
    if (ack) {
        IIC_Send_ACK();                                                                    //发送应答
    } else {
        IIC_Send_NoACK();                                                                    //发送非应答
    }
    return date;

}


