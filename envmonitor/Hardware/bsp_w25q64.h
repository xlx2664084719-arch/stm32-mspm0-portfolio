/**
  ******************************************************************************
  * @file    bsp_w25q64.h
  * @brief   W25Q64 SPI Flash 驱动（硬件 SPI1：PA5=SCK PA6=MISO PA7=MOSI PA4=CS）
  *          8MB 容量，用于环境数据日志的环形存储
  ******************************************************************************
  */
#ifndef __BSP_W25Q64_H
#define __BSP_W25Q64_H

#include "stm32f10x.h"

uint8_t W25Q64_Init(void);                       /* 返回 0=识别成功 */
void W25Q64_Read(uint32_t addr, uint8_t *buf, uint32_t len);
void W25Q64_PageProgram(uint32_t addr, const uint8_t *buf, uint16_t len); /* len<=256 */
void W25Q64_SectorErase(uint32_t addr);          /* 擦除 addr 所在 4KB 扇区 */
uint32_t W25Q64_GetID(void);                     /* 读 JEDEC ID（0xEF4017） */

#endif
