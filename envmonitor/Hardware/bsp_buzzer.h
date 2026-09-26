/**
  ******************************************************************************
  * @file    bsp_buzzer.h
  * @brief   有源蜂鸣器：PB15，低电平触发
  ******************************************************************************
  */
#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H
#include "stm32f10x.h"

void Buzzer_Init(void);
void Buzzer_Set(uint8_t on);         /* 1=响 0=停 */
void Buzzer_Beep(uint16_t ms);       /* 非阻塞：置响 ms 毫秒（由 Buzzer_Task 关闭） */
void Buzzer_Task1ms(void);           /* 每 1ms 调用，用于自动关断 */

#endif
