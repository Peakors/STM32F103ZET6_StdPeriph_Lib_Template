#include "wifi.h"
#include "lcd_gui.h"
struct WIFI wifi;

/********************************************************************
*wifi发送命令接收并进行处理比对
*********************************************************************/
u8 Wifi_Send_Check(const char *send_data,int usart_wait_time,int wait_timer,char *check_str)
{
	int timer=0;
	Usart2_Send_n_Byte((char *)send_data,strlen(send_data));	//发送数据
	wifi.FinishFlag=0;																				//设置完成标志位为0
	wifi.CurrentTimer=0;																			//设置当前时间为0
	wifi.WaitTimer = usart_wait_time;													//设置接收数据间间隔时间为2秒时间
	while(!wifi.FinishFlag)																		//等待数据接收完成
	{
			Delay_ms(1);
			timer++;
			if(timer > wait_timer){																//超时
					printf("超时%s\r\n",wifi.ReceiveBuff);
					Clean_Recv_Data();
					return 0;
			}
	}
	printf("检验%s\r\n",wifi.ReceiveBuff);	
	//检验是否具有字符串OK
	if(strstr(wifi.ReceiveBuff,check_str)==NULL){ 		//对比
			printf("检验%s\r\n",wifi.ReceiveBuff);		
			Clean_Recv_Data();
			return 0;
	}
	Clean_Recv_Data();
	return 1;
}
/********************************************************************
*清空函数
*********************************************************************/
void Clean_Recv_Data(void)
{
		memset(wifi.ReceiveBuff,0,sizeof(wifi.ReceiveBuff));
		wifi.CurrentSubscript=0;
}

/********************************************************************
*wifi配置连接函数
*********************************************************************/
u8 Wifi_Connect(char *IP,int Port,char * wifiname,char* passwd)
{
		char ipbuff[100]={0};
		char userinfo[100]={0};
		p1:
		//清空接收数组及下标
		Clean_Recv_Data();
		/*******************恢复出厂设置模式***********************/
		if(!Wifi_Send_Check("AT+RESTORE\r\n",1000,2000,"csum"))
		{
				printf("恢复出厂设置失败\r\n");
				//退出透传模式
				Wifi_Send_Check("+++",500,500,"+++");
				goto p1;
		}
		printf("恢复出厂设置成功\r\n");
		LcdPrintf("AT+RESTORE\r\n");
		/********************设置模式****************************/
		if(!Wifi_Send_Check("AT+CWMODE=1\r\n",1500,6000,"OK"))
		{
			printf("模式设置失败\r\n");
			return 1;
		}
		printf("模式设置成功\r\n");
		LcdPrintf("AT+RESTORE\r\n");
		/********************进行复位****************************/
//		if(!Wifi_Send_Check("AT+RST\r\n",7000,60000,"chksum"))
//		{
//				printf("复位失败\r\n");
//				return 2;
//		}
//		printf("复位成功\r\n");
		/********************wifi配置只显示ssid*******************/
//		if(!Wifi_Send_Check("AT+CWLAPOPT=1,2\r\n",1500,5000,"OK"))
//		{
//				printf("配置扫描周围热点显示失败\r\n");
//				return 3;
//		}
//		printf("配置扫描周围热点显示成功\r\n");
		/********************搜索wifi****************************/
//		if(!Wifi_Send_Check("AT+CWLAP\r\n",6000,60000,wifiname))
//		{
//				printf("需要连入的wifi未开启/未找到\r\n");
//				return 4;
//		}
//		printf("已找到要连入的wifi\r\n");
		/********************进行连接wifi*************************/
		sprintf(userinfo,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifiname,passwd);
		if(!Wifi_Send_Check(userinfo,8000,100000,"OK"))
		{
					printf("连接wifi失败 密码失败\r\n");
					return 5;
		}
		printf("连接wifi密码成功\r\n");
		LcdPrintf("AT+CWJAP=\"%s\",\"%s\"\r\n",wifiname,passwd);
		/*******************配置单路连接模式***********************/
//		if(!Wifi_Send_Check("AT+CIPMUX=0\r\n",1000,6000,"OK"))
//		{
//				printf("配置单路连接模式失败\r\n");
//				return 6;
//		}
//		printf("配置单路连接模式成功\r\n");
		/*******************配置透传模式模式***********************/
		if(!Wifi_Send_Check("AT+CIPMODE=1\r\n",1000,5000,"OK"))
		{
				printf("透传模式失败\r\n");
				return 7;
		}
		printf("透传模式模式成功\r\n");
		LcdPrintf("AT+CIPMODE=1\r\n");
		/*******************连接服务器及端口号***********************/
		sprintf(ipbuff,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",IP,Port);
		if(!Wifi_Send_Check(ipbuff,1000,10000,"OK"))
		{
				printf("连接服务器及端口号失败\r\n");
				return 8;
		}
		printf("连接服务器及端口号成功\r\n");
		LcdPrintf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",IP,Port);
		/***********************透传模式****************************/
		if(!Wifi_Send_Check("AT+CIPSEND\r\n",1000,5000,">"))
		{
				printf("透传失败\r\n");
				return 9;
		}
		printf("透传成功\r\n");
		LcdPrintf("AT+CIPSEND\r\n");
		Clean_Recv_Data();
		wifi.WaitTimer = 500;																								 //设置接收数据间间隔时间为500秒时间
		return 0;
}
