#ifndef __USART3__H__
#define __USART3__H__

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"



extern const  char *voice_buff[];

void Voice_Config(u32 bound);
void Send_Song_CMD(u8 * cmd,u8 cmd_len);
void Set_Song_Volume(unsigned char volume_num);
void Appoint_Song_Volume(unsigned char Song_num);
void Appoint_Song_Name(unsigned char *Song_Name);

#endif










