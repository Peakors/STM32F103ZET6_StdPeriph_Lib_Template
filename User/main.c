#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"

#include "retarget.h"


int main(void) {
    //添加printf重定向
    printf_retarget(115200, USART1);

//    Key_Init();
//    LED_Init();


    volatile uint32_t counter = 0;

    while (1) {
        counter++;
        printf("hello world \r\n");

//        LED1_Turn();
//        LED0_Turn();

        Delay_s(1);
    }
}
