//
// Created by Peakors on 9/18/2023.
//

#ifndef STM32F103ZET6_STDPERIPH_LIB_TEMPLATE_RETARGET_H
#define STM32F103ZET6_STDPERIPH_LIB_TEMPLATE_RETARGET_H

#include "stm32f10x.h"
#include "stdio.h"

void printf_retarget(uint32_t bound, USART_TypeDef *uarTx);

#endif //STM32F103ZET6_STDPERIPH_LIB_TEMPLATE_RETARGET_H
