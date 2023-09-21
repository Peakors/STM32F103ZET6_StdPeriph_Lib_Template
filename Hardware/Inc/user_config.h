#ifndef _USER_
#define _USER_
//头文件
#include "stm32f10x.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "relay.h"
#include "timer.h"
#include "time4.h"
#include "lcd_gui.h"
#include "touch.h"
#include "dht11.h"
#include "adc.h"
#include "usart2.h"
#include "mqtt.h"

#include "stdbool.h"

#include "gy30.h"

//宏定义
#define DeviceName 	 "Light"
#define ProductKey 	 "k08wu4HmZ9e"
#define DeviceSecret "1d0d1245b386ed27c68ccb46f77f971a"
/*宏定义--客户端ID 用户名*/
#define ClientID	DeviceName"|securemode=3,signmethod=hmacsha1|"							//用户ID
#define UserName	DeviceName"&"ProductKey                                   	//用户名
#define PassWord	"b6cb7627e3bec56161e9e66869f1e01668b006a1"                  //用户密码
#define aliyun_ip ProductKey".iot-as-mqtt.cn-shanghai.aliyuncs.com"           //阿里云IP
#define DEVICE_SUBSCRIBE "/sys/"ProductKey"/"DeviceName"/thing/service/property/set"	//订阅主题
#define DEVICE_PUBLISH "/sys/"ProductKey"/"DeviceName"/thing/event/property/post"     //发布消息


//函数声明
typedef struct {
	bool LED1_States;
	bool LED2_States;
	bool LED3_States;
	bool LED4_States;
	bool BEEP_States;
	bool RELAY1_States;
	bool RELAY2_States;
	bool AutoMode;
	float MQ2;
	float MQ135;
    int Rain_value;
    int Soil_value;
    int light_value;
	int Yl83;
	int Speed;
	int Direction;
	u8 T;
	u8 H;
	int Page;
}DevInfo;

void BootAnimation(void);
void HostComputerControl(DevInfo *devinfo);
void TouchScreenControl(DevInfo *devinfo);
void GetdevInfo( DevInfo *devinfo);
void DealWithData( DevInfo *devinfo);
void DisplayInfo(const DevInfo *devinfo);
void SetDevStates(const DevInfo *devinfo);
void ReportTask(const DevInfo *devinfo);
#endif


