/**
  ******************************************************************************
  * @file    bsp_key.h
  * @brief   按键驱动：PA0 上拉输入，非阻塞消抖 + 短按/长按事件识别
  ******************************************************************************
  */
#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"
#include <stdint.h>

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT,        /* 短按释放（<1s） */
    KEY_EVENT_LONG          /* 长按（>=1.5s，按下期间报一次） */
} key_event_t;

void         Key_Init(void);
void         Key_Task10ms(void);     /* 每 10ms 调用：扫描+消抖+事件判定 */
key_event_t  Key_GetEvent(void);     /* 取走事件（取出后清零） */

#endif
