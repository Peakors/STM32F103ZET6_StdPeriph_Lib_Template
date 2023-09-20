#include "gy30.h"

/************************ BH1750写指令函数 ***********************
Dev_Adder：模块IIC通讯地址
Common：   需要发送的控制指令
******************************************************************/
void BH1750_Write_COM(unsigned char Dev_Adder, unsigned char Common) {
    IIC_Start();                   //起始信号
    IIC_Send_Data(Dev_Adder);      //发送从设备地址（从设备地址+读写控制位）
    if (IIC_Get_ACK())            //检测应答
    {
        goto WR_End;
    }
    IIC_Send_Data(Common);         //发送测试指令
    if (IIC_Get_ACK())            //检测应答
    {
        goto WR_End;
    }
    WR_End:
    IIC_Stop();                       //发送结束信号
}

/************************ BH1750读数据函数 ***********************
Dev_Adder：模块IIC通讯地址
******************************************************************/
u16 BH1750_Read_Data(unsigned char Dev_Adder) {
    unsigned char Buff[2] = {0};
    u16 Data = 0;
    IIC_Start();                          //起始信号
    IIC_Send_Data(Dev_Adder | 0x01);      //发送从设备地址（从设备地址+读写控制位）
    if (IIC_Get_ACK())                   //检测应答
    {
        goto WR_End;       //跳转语句---无条件跳转，一旦满足条件，直接跳转
    }
    /* 读取光照数据 */
    Buff[0] = IIC_Read_Data(1);             //读取高字节光照数据 并发送应答信号
    Buff[1] = IIC_Read_Data(0);            //读取低字节光照数据 并发送非应答信号

    WR_End:
    IIC_Stop();                             //发送结束信号
    Delay_ms(5);

    /* 合并光强数据 */
    Data = Buff[0];
    Data = (Data << 8) | Buff[1];        //合成数据，即光照数据

    return Data;                                                    //光照强度数值
}

/************************ BH1750初始化函数 ***********************
根据需要参考pdf进行参数修改
*****************************************************************/
void BH1750_Init(void) {
    BH1750_Write_COM(SlaveAddress, 0x01);      //开电源
    BH1750_Write_COM(SlaveAddress, 0x10);      //连续模式
    Delay_ms(180);                            //等到测量结束---代表一个容错率
}



