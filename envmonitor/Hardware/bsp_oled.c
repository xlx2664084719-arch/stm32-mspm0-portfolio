/**
  ******************************************************************************
  * @file    bsp_oled.c
  * @brief   SSD1306 OLED 驱动（软件 I2C + 显存缓冲 + 整帧刷新）
  *
  * 架构说明：
  *   - 软件模拟 I2C（开漏输出 + 外部上拉），PB8=SCL，PB9=SDA
  *   - 先写 128x64 显存缓冲（1KB RAM），再整帧 I2C 刷新，避免闪烁
  *   - SSD1306 从机地址 0x78；MPU6050 为 0xD0，二者共总线互不干扰
  ******************************************************************************
  */
#include "bsp_oled.h"
#include "oledfont.h"
#include "delay.h"

/* ---- 引脚定义（换接线只改这里）---- */
#define OLED_SCL_PORT   GPIOB
#define OLED_SCL_PIN    GPIO_Pin_8
#define OLED_SDA_PORT   GPIOB
#define OLED_SDA_PIN    GPIO_Pin_9

#define SCL_H()  GPIO_SetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define SCL_L()  GPIO_ResetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define SDA_H()  GPIO_SetBits(OLED_SDA_PORT, OLED_SDA_PIN)
#define SDA_L()  GPIO_ResetBits(OLED_SDA_PORT, OLED_SDA_PIN)
#define SDA_READ()  GPIO_ReadInputDataBit(OLED_SDA_PORT, OLED_SDA_PIN)

#define OLED_ADDR   0x78        /* SSD1306 写地址 */
#define I2C_DELAY   delay_us(2) /* ~250kHz，稳定优先 */

static uint8_t oled_gram[8][128];   /* 显存：8 页 x 128 列，每字节纵向 8 像素 */

/* ============ 软件I2C 底层 ============ */
static void i2c_start(void)
{
    SDA_H(); SCL_H(); I2C_DELAY;
    SDA_L(); I2C_DELAY;
    SCL_L(); I2C_DELAY;
}

static void i2c_stop(void)
{
    SDA_L(); I2C_DELAY;
    SCL_H(); I2C_DELAY;
    SDA_H(); I2C_DELAY;
}

/* 返回从机 ACK，0=应答 */
static uint8_t i2c_send_byte(uint8_t b)
{
    uint8_t i, ack;
    for (i = 0; i < 8; i++) {
        if (b & 0x80) SDA_H(); else SDA_L();
        b <<= 1;
        I2C_DELAY;
        SCL_H(); I2C_DELAY;
        SCL_L(); I2C_DELAY;
    }
    SDA_H();                        /* 释放 SDA 等待 ACK */
    I2C_DELAY;
    SCL_H(); I2C_DELAY;
    ack = SDA_READ();
    SCL_L(); I2C_DELAY;
    return ack;
}

static void oled_write_cmd(uint8_t cmd)
{
    i2c_start();
    i2c_send_byte(OLED_ADDR);
    i2c_send_byte(0x00);            /* 控制字节：命令流 */
    i2c_send_byte(cmd);
    i2c_stop();
}

/* ============ 初始化 ============ */
void OLED_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 开漏输出，依靠模块上的 4.7k 上拉电阻形成 I2C 总线 */
    gpio.GPIO_Pin  = OLED_SCL_PIN | OLED_SDA_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);
    SCL_H(); SDA_H();

    delay_ms(100);                  /* 等待 OLED 上电复位 */

    oled_write_cmd(0xAE);           /* 关显示 */
    oled_write_cmd(0xD5); oled_write_cmd(0x80);   /* 时钟分频 */
    oled_write_cmd(0xA8); oled_write_cmd(0x3F);   /* 复用率 64 */
    oled_write_cmd(0xD3); oled_write_cmd(0x00);   /* 显示偏移 0 */
    oled_write_cmd(0x40);           /* 起始行 0 */
    oled_write_cmd(0x8D); oled_write_cmd(0x14);   /* 电荷泵开启 */
    oled_write_cmd(0x20); oled_write_cmd(0x00);   /* 水平寻址模式 */
    oled_write_cmd(0xA1);           /* 段重映射 */
    oled_write_cmd(0xC8);           /* COM 扫描方向 */
    oled_write_cmd(0xDA); oled_write_cmd(0x12);   /* COM 引脚配置 */
    oled_write_cmd(0x81); oled_write_cmd(0xCF);   /* 对比度 */
    oled_write_cmd(0xD9); oled_write_cmd(0xF1);   /* 预充电 */
    oled_write_cmd(0xDB); oled_write_cmd(0x40);   /* VCOMH */
    oled_write_cmd(0xA4);           /* 正常显示（非全亮） */
    oled_write_cmd(0xA6);           /* 正常（非反色） */
    oled_write_cmd(0xAF);           /* 开显示 */

    OLED_Clear();
    OLED_Update();
}

/* ============ 显存操作 ============ */
void OLED_Clear(void)
{
    uint8_t page, col;
    for (page = 0; page < 8; page++)
        for (col = 0; col < 128; col++)
            oled_gram[page][col] = 0x00;
}

void OLED_Update(void)
{
    uint8_t page, col;
    for (page = 0; page < 8; page++) {
        oled_write_cmd(0xB0 | page);            /* 页地址 */
        oled_write_cmd(0x00); oled_write_cmd(0x10);  /* 列地址低/高 4 位 = 0 */
        i2c_start();
        i2c_send_byte(OLED_ADDR);
        i2c_send_byte(0x40);                    /* 控制字节：数据流 */
        for (col = 0; col < 128; col++)
            i2c_send_byte(oled_gram[page][col]);
        i2c_stop();
    }
}

void OLED_ShowChar(uint8_t col, uint8_t row, char ch)
{
    uint8_t i, page;
    uint8_t idx = (ch >= 32 && ch < 32 + 92) ? (uint8_t)(ch - 32) : 0;

    if (row >= 8) return;
    page = row;                                 /* 每行文本 = 1 个显示页(8像素) */
    for (i = 0; i < 6; i++) {
        uint16_t c = col * 6 + i;
        if (c < 128) oled_gram[page][c] = font6x8[idx][i];
    }
}

void OLED_ShowString(uint8_t col, uint8_t row, const char *s)
{
    while (*s) {
        OLED_ShowChar(col++, row, *s++);
        if (col > 21) break;
    }
}

void OLED_ShowNum(uint8_t col, uint8_t row, int32_t num, uint8_t len)
{
    char buf[12];
    uint8_t i = 0, neg = 0;
    uint32_t un;

    if (num < 0) { neg = 1; un = (uint32_t)(-num); } else un = (uint32_t)num;

    if (un == 0) buf[i++] = '0';
    while (un > 0 && i < 10) { buf[i++] = '0' + un % 10; un /= 10; }
    if (neg) buf[i++] = '-';
    while (i < len && i < 11) buf[i++] = ' ';   /* 前导空格对齐 */

    /* 逆序输出 */
    while (i > 0) OLED_ShowChar(col++, row, buf[--i]);
}
