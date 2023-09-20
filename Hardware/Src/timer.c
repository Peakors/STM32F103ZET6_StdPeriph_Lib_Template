#include "timer.h"

/********************************************************************
*定 时 器 TIMER3 初 始 化
*********************************************************************/
void timer3_init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    //开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //使能定时器5时钟
    //定时器结构体配置
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 1000;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    //配置中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能定时器中断，设置中断类型为更新中断
    //中断配置
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
    //使能定时器
    TIM_Cmd(TIM3, ENABLE);//使能定时器
}

/********************************************************************
*timer中断服务函数
*********************************************************************/
void TIM3_IRQHandler(void) {
    if (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == 1)//获取定时器3是否为更新中断
    {
        //运行标志 如果在运行将不再加加处理
        if ((wifi.TimerRunFlag) && (!wifi.FinishFlag)) {
            //计时器加操作
            wifi.CurrentTimer++;
            if (wifi.CurrentTimer >= wifi.WaitTimer) {
                wifi.FinishFlag = 1;                       //标志位置1操作 表示可运行
                wifi.TimerRunFlag = 0;                                           //关闭定时器计数
            }
        }
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);  //清除中断标志位，必须手动清除
    }
}
