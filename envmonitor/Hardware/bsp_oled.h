/**
  ******************************************************************************
  * @file    bsp_oled.h
  * @brief   0.96" SSD1306 OLED 驱动（软件 I2C，PB8=SCL，PB9=SDA）
  *          与 MPU6050 共用同一条 I2C 总线（多从机总线复用）
  ******************************************************************************
  */
#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t col, uint8_t row, char ch);        /* col:0~21 row:0~7 */
void OLED_ShowString(uint8_t col, uint8_t row, const char *s);
void OLED_ShowNum(uint8_t col, uint8_t row, int32_t num, uint8_t len);
void OLED_Update(void);                                        /* 将缓冲区刷到屏幕 */

#endif
