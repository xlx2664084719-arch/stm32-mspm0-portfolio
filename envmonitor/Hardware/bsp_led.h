/**
  ******************************************************************************
  * @file    bsp_led.h / bsp_buzzer.h 合并说明见各文件
  * @brief   LED: PC13 板载（低电平亮）；蜂鸣器: PB15（低电平触发，有源）
  ******************************************************************************
  */
#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f10x.h"

void LED_Init(void);
void LED_Set(uint8_t on);        /* 1=亮 0=灭 */
void LED_Toggle(void);

#endif
