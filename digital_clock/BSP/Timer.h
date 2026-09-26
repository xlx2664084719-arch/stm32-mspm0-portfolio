#ifndef TIMER_H
#define TIMER_H
#include <stdio.h>
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include <math.h> 
#include "eeprom_emulation_type_a.h"
#include <string.h>
#include <stdbool.h>

extern bool timer_running;        // 定时器是否在倒计时
extern uint8_t timer_hour;        // 剩余小时
extern uint8_t timer_min;         // 剩余分钟
extern uint8_t timer_sec;         // 剩余秒

void Timer_Init(void);            // 初始化（可选）
void Timer_Start(uint8_t h, uint8_t m, uint8_t s);  // 开始倒计时
void Timer_Stop(void);            // 停止并清零
void Timer_Tick(void);            // 每秒调用一次（在主回调中调用）

#endif