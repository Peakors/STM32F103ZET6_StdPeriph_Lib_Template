#include "user_config.h"
#include "pic.h"

/***************************************************************
函数名称:void BootAnimation(void)
函数功能:开机动画
输入参数:无
输出参数:无
返 回 值:无
****************************************************************/
void BootAnimation(void) {
    LCD_Dis_String(0, 0, "Peakors 202003040041", 0x0000, 0xffff, 1, 1);
    for (int i = 0; i < 5; i++) {
        Auto_Show_Picture(0, 32, gImage_11);
        Delay_ms(500);
        Auto_Show_Picture(0, 32, gImage_12);
        Delay_ms(500);
        Auto_Show_Picture(0, 32, gImage_13);
        Delay_ms(500);
    }
}


/***************************************************************
函数名称:void HostComputerControl(DevInfo *devinfo)
函数功能:上位机下发指令进行接收并处理
输入参数:无
输出参数:设备状态结构体变量信息
返 回 值:无
****************************************************************/
void HostComputerControl(DevInfo *devinfo) {
    if (uart1_finish_flage) {
        if (strcmp(USART1_Rcv_Buff, "#LED1_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->LED1_States = true;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED1_OFF#") == 0) {
            devinfo->LED1_States = false;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED2_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->LED2_States = true;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED2_OFF#") == 0) {
            devinfo->LED2_States = false;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED3_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->LED3_States = true;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED3_OFF#") == 0) {
            devinfo->LED3_States = false;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED4_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->LED4_States = true;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#LED4_OFF#") == 0) {
            devinfo->LED4_States = false;
            devinfo->AutoMode = false;
        } else if (strcmp(USART1_Rcv_Buff, "#RELAY1_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->RELAY1_States = true;
        } else if (strcmp(USART1_Rcv_Buff, "#RELAY1_OFF#") == 0) {
            devinfo->RELAY1_States = false;
        } else if (strcmp(USART1_Rcv_Buff, "#RELAY2_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->RELAY1_States = true;
        } else if (strcmp(USART1_Rcv_Buff, "#RELAY2_OFF#") == 0) {
            devinfo->RELAY1_States = false;
        } else if (strcmp(USART1_Rcv_Buff, "#BEEP_ON#") == 0)//  对比函数 对比成功为0 对比失败非0
        {
            devinfo->BEEP_States = true;
        } else if (strcmp(USART1_Rcv_Buff, "#BEEP_OFF#") == 0) {
            devinfo->BEEP_States = false;
        } else if (strncmp(USART1_Rcv_Buff, "#WaterLamp", 10) == 0) {
            int val = sscanf(USART1_Rcv_Buff, "#WaterLamp:%d|Order:%d#", &devinfo->Speed, &devinfo->Direction);
            timer4_init(devinfo->Speed * 500);
            devinfo->AutoMode = true;
        }
        //处理结束将数组进行清空
        memset(USART1_Rcv_Buff, 0, sizeof(USART1_Rcv_Buff));
        uart1_count = 0;
        uart1_finish_flage = 0;
    }
}

/***************************************************************
函数名称:void HostComputerControl(DevInfo *devinfo)
函数功能:上位机下发指令进行接收并处理
输入参数:无
输出参数:设备状态结构体变量信息
返 回 值:无
****************************************************************/
void TouchScreenControl(DevInfo *devinfo) {
    TOUCH_TYPE_DEF touchnum = Touch_Scanf();
    if (touchnum.x != 65535) {
        LCD_Clear(0, 239, 0, 319, 0xffff);
        printf("x:%d  y:%d\r\n", touchnum.x, touchnum.y);
        //12  263   85 314
        if ((devinfo->Page == 1) && (touchnum.x < 85) && (touchnum.x > 12) && (touchnum.y < 314) &&
            (touchnum.y > 263)) {
            //进入该作用域  2界面
            devinfo->Page = 2;

        }
            //150 266 213 316
        else if ((devinfo->Page == 1) && (touchnum.x < 213) && (touchnum.x > 150) && (touchnum.y < 316) &&
                 (touchnum.y > 266)) {
            //进入该作用域  3界面
            devinfo->Page = 3;
        }
            //180 260
        else if ((devinfo->Page == 2) && (touchnum.x < 239) && (touchnum.x > 180) && (touchnum.y < 319) &&
                 (touchnum.y > 260)) {
            //进入该作用域  1界面
            devinfo->Page = 1;
        } else if ((devinfo->Page == 3) && (touchnum.x < 239) && (touchnum.x > 180) && (touchnum.y < 319) &&
                   (touchnum.y > 260)) {
            //进入该作用域  1界面
            devinfo->Page = 1;
        }
    }
}

/***************************************************************
函数名称:void GetdevInfo(DevInfo *devinfo)
函数功能:获取设备信息
输入参数:设备状态结构体变量信息
输出参数:无
返 回 值:无
****************************************************************/
void GetdevInfo(DevInfo *devinfo) {
    devinfo->MQ2 = ADC_Val_Disp(Mq2);
//    Delay_ms(500);
    devinfo->MQ135 = ADC_Val_Disp(MQ135_val);
//    Delay_ms(500);
    devinfo->Rain_value = ADC_Val_Disp(Rain);
//    Delay_ms(500);
    devinfo->Soil_value = ADC_Val_Disp(Soil);
//    Delay_ms(500);
    devinfo->light_value = BH1750_Read_Data(0x46);
//    Delay_ms(500);
    dht11_read_ht(&devinfo->T, &devinfo->H);
}

/***************************************************************
函数名称:void DealWithData( DevInfo *devinfo)
函数功能:处理信息
输入参数:设备状态结构体变量信息
输出参数:无
返 回 值:无
****************************************************************/
void DealWithData(DevInfo *devinfo) {

}

/***************************************************************
函数名称:void DisplayInfo(const DevInfo *devinfo)
函数功能:LCD屏显示信息
输入参数:设备状态结构体变量信息
输出参数:无
返 回 值:无
****************************************************************/
void DisplayInfo(const DevInfo *devinfo) {
    //显示
//    switch (devinfo->Page) {
//        case 1:
//            Auto_Show_Picture(0, 0, gImage_main);
//            break;
//        case 2:
//            Auto_Show_Picture(180, 260, gImage_return);
//            LCD_Dis_String(0, 50, "xxxxxxx", 0x0008, 0xffee, 2, 1);
//            break;
//        case 3:
//            Auto_Show_Picture(180, 260, gImage_return);
//            LCD_Dis_String(0, 50, "123456789", 0x0008, 0xffee, 2, 1);
//            break;
//    }

    LCD_Dis_String(0, 50, "曹睿", 0x0008, 0xffee, 2, 1);




}

/***************************************************************
函数名称:void SetDevStates(const DevInfo *devinfo)
函数功能:设置设备状态
输入参数:无
输出参数:设备状态结构体变量信息
返 回 值:无
****************************************************************/
void SetDevStates(const DevInfo *devinfo) {
    if (!devinfo->AutoMode) {
        TIM_Cmd(TIM4, DISABLE);                                    //使能能定时器
        devinfo->LED1_States ? LED1_ON : LED1_OFF;
        devinfo->LED2_States ? LED2_ON : LED2_OFF;
        devinfo->LED3_States ? LED3_ON : LED3_OFF;
        devinfo->LED4_States ? LED4_ON : LED4_OFF;
    } else if (devinfo->AutoMode) {
        Direction = devinfo->Direction;
    }
    devinfo->BEEP_States ? BEEP_ON : BEEP_OFF;
    devinfo->RELAY1_States ? RELAY1_ON : RELAY1_OFF;
    devinfo->RELAY2_States ? RELAY2_ON : RELAY2_OFF;
}

/***************************************************************
函数名称:void  ReportTask(const DevInfo *devinfo)
函数功能:上报设备状态
输入参数:设备状态结构体变量信息
输出参数:无
返 回 值:无
****************************************************************/
void ReportTask(const DevInfo *devinfo) {
    static u32 count = 0;
    count++;
    if (count > 20) {
        count = 0;
        u8 tmpbuf[1024] = {0};
        sprintf((char *) tmpbuf,
                "{\"method\":\"thing.event.property.post\",\"id\":\"292200613\",\"params\":{\"Light\":%d,\"temperature\":%d,\"Humidity\":%d,\"MQ2_Value\":%d,\"MQ135_Value\":%d,\"Rain_Value\":%d,\"soilHumidity\":%d},\"version\":\"1.0.0\"}",
                devinfo->light_value, (int) devinfo->T, (int) devinfo->H, (int) devinfo->MQ2,(int) devinfo->MQ135, devinfo->Rain_value, devinfo->Soil_value);
//		sprintf((char *)tmpbuf,"{\"method\":\"thing.event.property.post\",\"id\":\"292200613\",\"params\":{\"LightSwitch\":1},\"version\":\"1.0.0\"}");
        MY_MQTT.PublishData(DEVICE_PUBLISH, (char *) tmpbuf, 0);

        printf("\r\n===================\r\n");

        printf("=====>#MQ2:%d\r\n=====>#T:%d\r\n=====>#H:%d\r\n", (int) devinfo->MQ2, (int) devinfo->T, (int) devinfo->H);
        printf("=====>#MQ135:%d\r\n", (int) devinfo->MQ135);
        printf("=====>#Rain:%d\r\n",devinfo->Rain_value);
        printf("=====>#Soil:%d\r\n",devinfo->Soil_value);
        printf("=====>#光照强度:%d\r\n",devinfo->light_value);
        printf("===================\r\n");



    }
}
