/**
  ******************************************************************************
  * @file    bsp_mpu6050.h
  * @brief   MPU6050 三轴加速度/陀螺仪驱动（I2C，与 OLED 共用 PB8/PB9 总线）
  ******************************************************************************
  */
#ifndef __BSP_MPU6050_H
#define __BSP_MPU6050_H

#include "stm32f10x.h"

typedef struct {
    int16_t ax, ay, az;     /* 原始加速度（±2g 量程，16384 LSB/g） */
    int16_t gx, gy, gz;     /* 原始角速度 */
    int16_t pitch_x10;      /* 俯仰角 x10（度），由加速度解算 */
    int16_t roll_x10;       /* 横滚角 x10（度） */
} mpu_data_t;

uint8_t MPU6050_Init(void);          /* 返回 0=成功，1=未检测到器件 */
void MPU6050_Read(mpu_data_t *d);    /* 读取一次数据并解算倾角 */

#endif
