/**
  ******************************************************************************
  * @file    delay.c
  * @brief   DWT 延时实现
  *
  * 【修复记录】原实现复用 SysTick 做延时，每次 delay 都会重配 SysTick 并
  * 关闭其中断（CTRL=0），导致 main.c 的 1ms 系统心跳被杀死，所有时间片
  * 任务永不执行 —— 典型的"两个模块抢占同一硬件资源"事故。
  * 现改为 DWT->CYCCNT 纯轮询，与 SysTick 完全解耦。
  ******************************************************************************
  */
#include "delay.h"

void Delay_Init(void)
{
    /* 使能 DWT 周期计数器（调试组件，内核自带） */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);   /* 72 ticks/us */
    while ((DWT->CYCCNT - start) < ticks) {
        /* 无符号减法：计数器回绕也正确 */
    }
}

void delay_ms(uint32_t ms)
{
    while (ms--) {
        delay_us(1000);
    }
}
