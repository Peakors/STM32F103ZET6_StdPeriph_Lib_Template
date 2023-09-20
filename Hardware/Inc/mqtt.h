#ifndef __MQTT__
#define __MQTT__

#include "stm32f10x.h"  
#include <stdio.h>
#include "wifi.h"
#include "sys_tick.h"
#include <string.h>

#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
typedef struct
{
	char *rxbuf;        																				   //接收数组
	u16 rxlen;																								   //接收长度
	char *txbuf;                                                   //发送数组
	u16 txlen;                                                   //发送长度
	void (*Init)(u8 *rx_buff,u16 rx_len,u8 *tx_buff,u16 tx_len);           //初始化
	u8 (*Connect)(char *ClientID,char *Username,char *Password); //CONNECT连接函数
	u8 (*SubscribeTopic)(char *topic,u8 qos,u8 whether);         //MQTT订阅/取消订阅数据打包函数
	u8 (*PublishData)(char *topic, char *message, u8 qos);       //发布信息函数
	void (*SendHeart)(void);																		 //发送ping包函数 心跳函数
	void (*Disconnect)(void);                                    //取消连接函数
}mqtt_struct;
u8 Sendheart(void);
extern mqtt_struct MY_MQTT;


//函数声明
static void Init(u8 *rx_buff,u16 rx_len,u8 *tx_buff,u16 tx_len);
static void Mqtt_SendBuf(u8 *buf,u16 len);
static u8 Connect(char *ClientID,char *Username,char *Password);
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether);
static u8 PublishData(char *topic, char *message, u8 qos);
static void SentHeart(void);
static void Disconnect(void);
char Reception_Comparison(const u8 * Comparison_Buff,u8 Comparison_Len);
void Processing_before_sending(void);



#endif





