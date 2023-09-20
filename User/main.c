#include "main.h"


int main(void) {
    //添加printf重定向
    printf_retarget(115200, USART1);
    printf("printf retarget success\r\n");


    DevInfo devinfo;
    unsigned char ConnetFlag=0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断优先级分组
    Delay_Init();       															//定时器延时初始化
    USART_Config(115200);															//串口1(调试串口及上位机串口)初始化
    BEEPGpioInit();																		//BEEP初始化
    KEYGpioInit();																		//KEY按键初始化
    LEDInit();																				//LED初始化
    RELAYInit();																			//继电器IO初始化
    LCD_Init();																				//LCD屏初始化
    Adc_Init();																				//ADC初始化
    Touch_Init();																		  //触摸屏初始化
    USART_2_Init(115200);															//串口2 115200
    timer3_init();																		//串口2辅助定时器

    timer4_init(1000);


    //    BootAnimation();      //TODO 开机动画

    printf("Init Success!\r\n");



    Wifi_Connect(aliyun_ip,1883,"xxxyiot","xxxya508");
    MY_MQTT.Init((u8*)wifi.ReceiveBuff,512,(u8*)wifi.SendBuff,512);
    ConnetFlag = MY_MQTT.Connect(ClientID,UserName,PassWord);  							//连接
    ConnetFlag ? printf("MQTT连接成功\r\n"):printf("MQTT连接失败\r\n");
    ConnetFlag=MY_MQTT.SubscribeTopic(DEVICE_SUBSCRIBE,0,1);								//主题 QOS=0,订阅
    ConnetFlag ? printf("主题订阅成功\r\n"):printf("主题订阅失败\r\n");
    memset(&devinfo,0,sizeof(DevInfo));
    devinfo.Page = 1;
    while(1)
    {
        /************************输入任务部分****************************************/
        HostComputerControl(&devinfo);								//上位机控制命令
        TouchScreenControl(&devinfo);									//触摸屏控制命令
        GetdevInfo(&devinfo);													//获取设备信息
        /************************处理任务部分****************************************/
        DealWithData(&devinfo);												//处理信息  报警等
        /************************输出任务部分****************************************/
        DisplayInfo(&devinfo);												//显示信息
        SetDevStates(&devinfo);												//设置设备状态
        ReportTask(&devinfo);													//上报数据
        /************************清空任务部分****************************************/
    }
}
