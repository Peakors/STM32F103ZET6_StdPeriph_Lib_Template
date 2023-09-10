#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"

#include "printf_uart.h"


int main(void){
//	Key_Init();
//	LED_Init();


    Uart1Init(115200); // 初始化Uart1

    PrintfInit(USART1); // printf 重定向到Uart1

    volatile uint32_t counter = 0;

	while (1){
        counter++;
        printf("hello world \r\n");

        Delay_s(1);
	}
}
