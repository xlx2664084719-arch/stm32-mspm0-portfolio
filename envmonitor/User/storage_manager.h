/**
  ******************************************************************************
  * @file    storage_manager.h
  * @brief   W25Q64 环形日志管理（应用层）：16 字节定长记录，页对齐无跨界
  ******************************************************************************
  */
#ifndef __STORAGE_MANAGER_H
#define __STORAGE_MANAGER_H

#include "stm32f10x.h"
#include <stdint.h>

typedef struct {
    uint16_t seq;        /* 序号（回绕） */
    uint32_t ts_ms;      /* 开机以来毫秒 */
    uint16_t light;      /* 光照百分比 */
    uint16_t pot_x100;   /* 电位器电压 x100 */
    int16_t  pitch_x10;
    int16_t  roll_x10;
    uint16_t crc;        /* 前 14 字节累加和 */
} log_rec_t;             /* 16 字节：256B 页正好放 16 条，永不跨页 */

void     Storage_Init(void);      /* 初始化 Flash 并载入配置页 */
int      Storage_Save(uint16_t light, uint16_t pot, int16_t p, int16_t r);
uint32_t Storage_Count(void);      /* 累计记录条数 */
uint16_t Storage_LastSeq(void);

#endif
