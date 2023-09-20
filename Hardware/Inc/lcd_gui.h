#ifndef __LCD_GUI_H__
#define __LCD_GUI_H__

/* 需要根据使用的具体控制器型号以及主控芯片包含不同的头文件 */
#include "ili9486.h"
#include <stm32f10x.h>

/* 常用LCD_GUI函数 */
void LCD_Draw_Point(u16 xpos, u16 ypos, u16 Color);
void Draw_Big_Point(u16 x,u16 y,u16 color);
void LCD_SetDrawBlock(u32 SX,u32 SY,u32 EX,u32 EY);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 pcolor);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 pcolor);
void Draw_Circle(u16 x,u16 y,u8 r,u16 pcolor);
void TP_draw_adjust_point(u16 x,u16 y,u16 color);
void LCD_Dis_8x16Ascill(u16 Xpos, u16 Ypos, char str, u16 Pcolor, u16 Bcolor,u8 Mode);
void LCD_Dis_8x16string(u16 xpos,u16 ypos,const char *str,u16 pcolor,u16 bcolor,u8 Mode);
void Show_Font(u16 Xpos, u16 Ypos,u8 *dzk,u8 size,u16 Pcolor, u16 Bcolor,u8 Mode);
void Show_Picture(u32 x,u32 y,u32 Width, u32 High,const unsigned char * p);
void Dis_Pic(unsigned int Xpos, unsigned int Ypos,unsigned int P_xpos, unsigned int P_ypos, unsigned char *p);
void Auto_Show_Picture(u32 x,u32 y,const unsigned char * data);
u16 RGB(u8 R, u8 G, u8 B);
void LCD_Dis_Ascill_Sf(u16 Xpos, u16 Ypos, char Str, u16 Pcolor, u16 Bcolor, u16 Size,u8 Mode);
void LCD_Dis_Str_Sf(u16 Xpos, u16 Ypos, const unsigned char *Str, u16 Pcolor, u16 Bcolor, u16 Size,u8 Mode);
void LCD_Dis_hz_Sf(u16 Xpos, u16 Ypos, const unsigned char *Str, u16 Pcolor, u16 Bcolor, u16 Size,u8 Mode);
void LCD_Dis_hzc_Sf(u16 Xpos, u16 Ypos, const unsigned char *Str,u16 Pcolor, u16 Bcolor, u16 Size,u8 Mode);
void LCD_Dis_String(u16 Xpos,u16 Ypos, const unsigned char *Str, u16 Pcolor, u16 Bcolor, u16 Size,u8 Mode);
void LcdPrintf(const char *fmt,...);


extern int CurrentPosition;
#endif
