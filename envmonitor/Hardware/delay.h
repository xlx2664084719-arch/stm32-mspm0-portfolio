/**
  ******************************************************************************
  * @file    delay.h
  * @brief   基于 DWT 周期计数器的高精度延时（不占用 SysTick！）
  *
  * 重要设计说明：SysTick 已被 main.c 用作 1ms 系统心跳时基，
  * 延时功能改用 Cortex-M3 内核的 DWT->CYCCNT（32 位循环计数器，
  * 72MHz 下约 59.6 秒回绕一次，用无符号减法自然处理回绕）。
  ******************************************************************************
  */
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void Delay_Init(void);        /* 使能 DWT 计数器，main 最先调用一次 */
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif
