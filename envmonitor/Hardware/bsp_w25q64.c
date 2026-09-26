/**
  ******************************************************************************
  * @file    bsp_w25q64.c
  * @brief   W25Q64 驱动：SPI1 硬件外设 + 片选软件控制
  *          要点：写前必须先擦（Flash 只能 1→0）；页编程不能跨 256 字节页边界
  ******************************************************************************
  */
#include "bsp_w25q64.h"
#include "delay.h"

#define CS_H()  GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define CS_L()  GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define CMD_WRITE_EN   0x06
#define CMD_READ       0x03
#define CMD_PAGE_PROG  0x02
#define CMD_SECTOR_ER  0x20
#define CMD_JEDEC_ID   0x9F
#define CMD_READ_SR1   0x05

static void spi1_low_speed(void)
{
    /* 擦写时序余量：先降到低速，识别后可提回高速 */
    SPI_InitTypeDef spi;
    SPI_I2S_DeInit(SPI1);
    spi.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode              = SPI_Mode_Master;
    spi.SPI_DataSize          = SPI_DataSize_8b;
    spi.SPI_CPOL              = SPI_CPOL_Low;      /* 模式0：CPOL=0 CPHA=0 */
    spi.SPI_CPHA              = SPI_CPHA_1Edge;
    spi.SPI_NSS               = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  /* 9MHz */
    spi.SPI_FirstBit          = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1, &spi);
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t W25Q64_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    /* PA4 = CS 软件控制，推挽输出，空闲高 */
    gpio.GPIO_Pin   = GPIO_Pin_4;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    CS_H();

    /* PA5/6/7 = SCK/MISO/MOSI，复用推挽 + 复用上拉输入 */
    gpio.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);
    gpio.GPIO_Pin   = GPIO_Pin_6;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    spi1_low_speed();
    return (W25Q64_GetID() == 0xEF4017) ? 0 : 1;
}

static uint8_t spi_rw(uint8_t b)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, b);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

static void cs_begin(void) { CS_L(); }
static void cs_end(void)   { CS_H(); }

static void write_enable(void)
{
    cs_begin(); spi_rw(CMD_WRITE_EN); cs_end();
}

/* 忙等待：状态寄存器 BUSY 位（带 500ms 超时，防 Flash 接触不良时卡死） */
static void wait_busy(void)
{
    uint8_t sr;
    extern volatile uint32_t g_sys_ms;
    uint32_t start = g_sys_ms;
    do {
        cs_begin(); spi_rw(CMD_READ_SR1); sr = spi_rw(0x00); cs_end();
    } while ((sr & 0x01) && (g_sys_ms - start) < 500);
}

uint32_t W25Q64_GetID(void)
{
    uint32_t id;
    cs_begin();
    spi_rw(CMD_JEDEC_ID);
    id  = (uint32_t)spi_rw(0x00) << 16;
    id |= (uint32_t)spi_rw(0x00) << 8;
    id |= spi_rw(0x00);
    cs_end();
    return id;
}

void W25Q64_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    wait_busy();
    cs_begin();
    spi_rw(CMD_READ);
    spi_rw((addr >> 16) & 0xFF);
    spi_rw((addr >> 8)  & 0xFF);
    spi_rw(addr & 0xFF);
    for (i = 0; i < len; i++) buf[i] = spi_rw(0x00);
    cs_end();
}

void W25Q64_PageProgram(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    if (len > 256) len = 256;
    if ((addr & 0xFF) + len > 256)              /* 防跨页：截断到本页尾 */
        len = 256 - (addr & 0xFF);
    write_enable();
    wait_busy();
    cs_begin();
    spi_rw(CMD_PAGE_PROG);
    spi_rw((addr >> 16) & 0xFF);
    spi_rw((addr >> 8)  & 0xFF);
    spi_rw(addr & 0xFF);
    for (i = 0; i < len; i++) spi_rw(buf[i]);
    cs_end();
    wait_busy();
}

void W25Q64_SectorErase(uint32_t addr)
{
    write_enable();
    wait_busy();
    cs_begin();
    spi_rw(CMD_SECTOR_ER);
    spi_rw((addr >> 16) & 0xFF);
    spi_rw((addr >> 8)  & 0xFF);
    spi_rw(addr & 0xFF);
    cs_end();
    wait_busy();                                /* 典型 45ms，最长 400ms */
}
