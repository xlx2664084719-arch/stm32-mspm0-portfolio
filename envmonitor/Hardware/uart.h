/**
  ******************************************************************************
  * @file    uart.h
  * @brief   USART1 串口驱动 + printf 重定向
  *          TX = PA9, RX = PA10（Blue Pill 板上丝印 A9 / A10）
  *          波特率默认 115200，可直接接 USB-TTL 接 VOFA+ / 串口助手
  ******************************************************************************
  */
#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"
#include <stdio.h>

void UART1_Init(uint32_t baud);   /* 初始化 USART1 */
/* 重定向 printf 后，直接 printf("data:%d\n", x); 即可发数据 */

#endif
