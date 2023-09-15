#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"

#ifdef __GCC_UART_RETARGET

#include "printf_uart.h"

#endif


int main(void) {

#ifdef __GCC_UART_RETARGET
    Uart1Init(115200); // 初始化Uart1

    PrintfInit(USART1); // printf 重定向到Uart1
#endif
//	Key_Init();
	LED_Init();




    volatile uint32_t counter = 0;

    while (1) {
        counter++;
//        printf("hello world \r\n");

      LED1_Turn();
			LED0_Turn();

        Delay_s(1);
    }
}
