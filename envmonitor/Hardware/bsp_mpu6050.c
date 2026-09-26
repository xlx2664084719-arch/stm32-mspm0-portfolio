/**
  ******************************************************************************
  * @file    bsp_mpu6050.c
  * @brief   MPU6050 驱动：寄存器级 I2C 读写 + 加速度倾角解算
  *          总线与 OLED 共用（PB8/PB9），本文件直接内联一份同样的软件 I2C
  *          底层（同一总线只允许一个驱动操作时序，这里约定两者不并发调用）
  ******************************************************************************
  */
#include "bsp_mpu6050.h"
#include "delay.h"
#include <math.h>

/* 引脚与 bsp_oled.c 一致：PB8=SCL，PB9=SDA */
#define SCL_H()  GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define SCL_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define SDA_H()  GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define SDA_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define SDA_READ() GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)
#define I2C_DLY  delay_us(2)

#define MPU_ADDR      0xD0        /* AD0=GND 时从机地址 */
#define REG_WHO_AM_I  0x75
#define REG_PWR_MGMT  0x6B
#define REG_SMPLRT    0x19
#define REG_CONFIG    0x1A
#define REG_GYRO_CFG  0x1B
#define REG_ACCEL_CFG 0x1C
#define REG_ACCEL_X   0x3B

static void i2c_start(void)
{ SDA_H(); SCL_H(); I2C_DLY; SDA_L(); I2C_DLY; SCL_L(); I2C_DLY; }

static void i2c_stop(void)
{ SDA_L(); I2C_DLY; SCL_H(); I2C_DLY; SDA_H(); I2C_DLY; }

static uint8_t i2c_write(uint8_t b)
{
    uint8_t i, ack;
    for (i = 0; i < 8; i++) {
        if (b & 0x80) SDA_H(); else SDA_L();
        b <<= 1; I2C_DLY; SCL_H(); I2C_DLY; SCL_L(); I2C_DLY;
    }
    SDA_H(); I2C_DLY; SCL_H(); I2C_DLY;
    ack = SDA_READ();
    SCL_L(); I2C_DLY;
    return ack;
}

static uint8_t i2c_read(uint8_t ack_en)
{
    uint8_t i, b = 0;
    SDA_H();
    for (i = 0; i < 8; i++) {
        b <<= 1; I2C_DLY; SCL_H(); I2C_DLY;
        if (SDA_READ()) b |= 1;
        SCL_L(); I2C_DLY;
    }
    if (ack_en) SDA_L(); else SDA_H();  /* 主机应答/非应答 */
    I2C_DLY; SCL_H(); I2C_DLY; SCL_L(); I2C_DLY;
    SDA_H();
    return b;
}

/* 单字节写寄存器 */
static void mpu_write_reg(uint8_t reg, uint8_t val)
{
    i2c_start();
    i2c_write(MPU_ADDR);
    i2c_write(reg);
    i2c_write(val);
    i2c_stop();
}

/* 连续读 len 字节 */
static void mpu_read_regs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    i2c_start();
    i2c_write(MPU_ADDR);
    i2c_write(reg);
    i2c_start();                        /* 重复起始，转读 */
    i2c_write(MPU_ADDR | 0x01);
    for (i = 0; i < len; i++)
        buf[i] = i2c_read(i < len - 1); /* 最后字节 NACK */
    i2c_stop();
}

uint8_t MPU6050_Init(void)
{
    uint8_t id = 0;
    uint8_t t;

    /* 等效于 SysConfig 里使能一次器件——唤醒 + 量程配置 */
    mpu_write_reg(REG_PWR_MGMT, 0x00);  /* 退出睡眠 */
    for (t = 0; t < 5; t++) { delay_ms(2); }
    mpu_write_reg(REG_SMPLRT, 0x07);    /* 采样率 1kHz/8 = 125Hz */
    mpu_write_reg(REG_CONFIG, 0x03);    /* DLPF 44Hz，滤机械振动 */
    mpu_write_reg(REG_GYRO_CFG, 0x00);  /* ±250 dps */
    mpu_write_reg(REG_ACCEL_CFG, 0x00); /* ±2g */

    mpu_read_regs(REG_WHO_AM_I, &id, 1);
    return (id == 0x68) ? 0 : 1;
}

void MPU6050_Read(mpu_data_t *d)
{
    uint8_t buf[14];
    float ax, ay, az;

    mpu_read_regs(REG_ACCEL_X, buf, 14);
    d->ax = (int16_t)((buf[0]  << 8) | buf[1]);
    d->ay = (int16_t)((buf[2]  << 8) | buf[3]);
    d->az = (int16_t)((buf[4]  << 8) | buf[5]);
    d->gx = (int16_t)((buf[8]  << 8) | buf[9]);
    d->gy = (int16_t)((buf[10] << 8) | buf[11]);
    d->gz = (int16_t)((buf[12] << 8) | buf[13]);

    /* 加速度解算姿态角（静态重力法）：1 LSB = 1/16384 g */
    ax = d->ax / 16384.0f;
    ay = d->ay / 16384.0f;
    az = d->az / 16384.0f;
    d->pitch_x10 = (int16_t)(atan2(-ax, sqrt(ay * ay + az * az)) * 572.96f);
    d->roll_x10  = (int16_t)(atan2(ay, az) * 572.96f);
}
