#ifndef _WIFI_
#define _WIFI_
//头文件
#include "stm32f10x.h"
#include "usart2.h"
#include "usart.h"
#include "sys_tick.h"
#include <string.h>
//宏定义
struct WIFI{
	//运行标志
	int FinishFlag;
	//定时器运行标志
	int TimerRunFlag;
	//当前计时
	unsigned int CurrentTimer;
	//等待时间
	unsigned int WaitTimer;
	//接收数组
	char ReceiveBuff[512];
	//数组下标
	int CurrentSubscript;
	//发送数组
	char SendBuff[512];
};
extern struct WIFI wifi;

//函数声明
u8 Wifi_Send_Check(const char *send_data,int usart_wait_time,int wait_timer,char *check_str);
u8 Wifi_Connect(char *IP,int Port,char * wifiname,char* passwd);
void Clean_Recv_Data(void);
void Recv_Json(void);

#endif


