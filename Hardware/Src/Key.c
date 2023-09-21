#include "key.h"

/********************************
函数名：void  KEY_GPIO_Init(void)
函数功能：KEY连接GPIO口初始化
参数：无
返回值：无
*********************************/
void KEYGpioInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;   /* 定义一个结构体类型变量 */
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE,
                           ENABLE);                                        /* 使能GPIOA、GPIOE时钟 */
    //GPIO初始化
    /* 设置PE2 、3、4工作模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;                                 /* 使用端口编号5、6 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                                             /* 浮空输入功能 */
    GPIO_Init(GPIOE, &GPIO_InitStructure);                                                             /* 初始化GPIOE组 */
    /* 设置PE2 、3、4工作模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                                                                             /* 使用端口编号5、6 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                                             /* 浮空输入功能 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                                             /* 初始化GPIOA组 */
}

/********************************
函数名：unsigned char   KEY_Scan_Function(void)
函数功能：KEY扫描函数--对比消抖、
参数：无
返回值：无
*********************************/
unsigned char KEYScanFunction(void) {
    unsigned char Key_return_value = 0;                                                                                //返回值变量
    static unsigned char KEY_1, KEY_2, KEY_3, Count = 0, Free_Flag = 0;               //记录三次值变量  次数
    //进行值传递
    //KEY_3 0  KEY_2 1  KEY_1 1
    KEY_3 = KEY_2;
    KEY_2 = KEY_1;
    //KEY_3 1  KEY_2 1  KEY_1 1
    //记录次数
    Count++;
    //判定按下
    if ((Free_Flag == 0) &&
        ((!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))) || (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))) ||
         (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3))) || ((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2))))) {
        if (!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)))
            KEY_1 = 1;
        else if (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)))
            KEY_1 = 2;
        else if (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)))
            KEY_1 = 3;
        else if ((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)))
            KEY_1 = 4;
        if (Count >= 3) {
            Count = 0;
            if ((KEY_1 == KEY_2) && (KEY_3 == KEY_1)) {
                Key_return_value = KEY_1;
                Free_Flag = 1;
            }
        }
    }
        //松手操作
    else if ((Free_Flag == 1) && ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))) &&
             ((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))) && ((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3))) &&
             (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)))) {
        Free_Flag = 0;
        Count = 0;
    }
    return Key_return_value;
}

/*
 * LEFT -> 1
 * RIGHT -> 2
 * UP -> 3
 * DOWN -> 4
 * 阻塞式Key Scanning
 */
uint8_t key_scan() {
    uint8_t value = 0;

    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==1 ){
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==1);
        Delay_ms(20);
        value =1;
    }
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0);
        Delay_ms(20);
        value =2;
    }

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);
        Delay_ms(20);
        value =3;
    }

    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0);
        Delay_ms(20);
        value =4;
    }

    return value;


}










