#include "dht11.h"
//DHT11_DATA  -- PG2
/**************************************************
	作用：初始化PG2的工作模式
	参数：mode  1：初始化为输入模式 0：初始化为输出模式
***************************************************/
void dht11_data_mode(u8 mode) {
    GPIO_InitTypeDef GPIO_InitStruct;
    //1、打开PG2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    //2、配置工作模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    if (mode == 1) {
        //配置为浮空输入模式
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    } else if (mode == 0) {
        //配置为通用推挽输出模式
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    }
    GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/**********************************************
	作用：读取温湿度值
	参数：t：存放读取的温度  h：存放读取的湿度
	返回值：1：读取成功  0：读取失败
***********************************************/
u8 dht11_read_ht(u8 *t, u8 *h) {
    int i, j;
    int cnt;
    u8 buff[5] = {0};
    dht11_data_mode(0);                                                            //配置数据线为输出模式
    //发送起始信号
    GPIO_SetBits(GPIOG, GPIO_Pin_2);                                    //数据线拉高
    GPIO_ResetBits(GPIOG, GPIO_Pin_2);                        //数据线拉低
    Delay_ms(18);
    GPIO_SetBits(GPIOG, GPIO_Pin_2);                            //数据线拉高
    Delay_us(30);
    dht11_data_mode(1);                                    //配置数据线为输入模式
    //接收响应信号
    cnt = 0;
    while (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2))  //等待数据线变低
    {
        cnt++;
        Delay_us(1);
        if (cnt > 500) {
            return 0;                                    //返回错误
        }
    }
    cnt = 0;
    while (!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)) //等待数据线变高
    {
        cnt++;
        Delay_us(1);
        if (cnt > 500) {
            return 0;                                    //返回错误
        }
    }
    //接收数据
    for (i = 0; i < 5; i++)                                               //4字节+校验字节
    {
        for (j = 7; j >= 0; j--)                                           //组合字节
        {
            cnt = 0;
            while (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2))//等待数据线变低
            {
                cnt++;
                Delay_us(1);
                if (cnt > 500) {
                    return 0;                                  //返回错误
                }
            }
            cnt = 0;
            while (!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2))//等待数据线变高
            {
                cnt++;
                Delay_us(1);
                if (cnt > 500) {
                    return 0;                                  //返回错误
                }
            }
            Delay_us(40);
            if (GPIOG->IDR & (1 << 2)) {
                //读取到1
                buff[i] |= 1 << j;
            } else {
                //读取到0
                buff[i] &= ~(1 << j);
            }
        }
    }

    //校验数据buff[0]湿度整数 + buff[1]湿度小数 + buff[2]温度整数 + buff[3]温度小数 buff[4]校验
    if (buff[0] + buff[1] + buff[2] + buff[3] == buff[4]) {
        *t = buff[2];
        *h = buff[0];
        return 1;
    } else {
        return 0;
    }
}



