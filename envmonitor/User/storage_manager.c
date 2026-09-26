/**
  ******************************************************************************
  * @file    storage_manager.c
  * @brief   环形日志：扇区0页0 存配置（魔数/计数/写指针），扇区1起存数据
  *
  * Flash 使用纪律（面试常问）：
  *   1. 擦除最小单位是 4KB 扇区，写入前必须先擦
  *   2. 页编程一次最多 256B 且不能跨页 -> 记录定长 16B，16 条/页
  *   3. 写满 8MB 回卷覆盖最旧数据（环形）
  ******************************************************************************
  */
#include "storage_manager.h"
#include "bsp_w25q64.h"

#define CFG_MAGIC     0x454E564Du      /* 'ENVM' */
#define CFG_ADDR      0x000000         /* 配置页 */
#define DATA_ADDR     0x001000         /* 数据区起点（扇区1） */
#define FLASH_SIZE    0x800000u        /* 8MB */

static uint32_t rec_count = 0;       /* 累计条数 */
static uint32_t write_addr = DATA_ADDR;

static void load_config(void)
{
    uint8_t page[256];
    uint32_t magic;
    W25Q64_Read(CFG_ADDR, page, 16);
    magic  = (uint32_t)page[0] | ((uint32_t)page[1] << 8)
           | ((uint32_t)page[2] << 16) | ((uint32_t)page[3] << 24);
    if (magic == CFG_MAGIC) {
        rec_count = (uint32_t)page[4]  | ((uint32_t)page[5]  << 8)
                  | ((uint32_t)page[6]  << 16) | ((uint32_t)page[7]  << 24);
        write_addr = (uint32_t)page[8] | ((uint32_t)page[9]  << 8)
                   | ((uint32_t)page[10] << 16) | ((uint32_t)page[11] << 24);
        if (write_addr < DATA_ADDR || write_addr >= FLASH_SIZE)
            write_addr = DATA_ADDR;
    }
    /* 魔数不符（全新芯片）则保持默认，首次 Save 时写入 */
}

void Storage_Init(void)
{
    if (W25Q64_Init() != 0) return;      /* Flash 不在线则禁用存储功能 */
    load_config();
}

static void save_config(void)
{
    uint8_t page[16];
    page[0] = CFG_MAGIC & 0xFF;  page[1]  = (CFG_MAGIC >> 8)  & 0xFF;
    page[2] = (CFG_MAGIC >> 16) & 0xFF; page[3] = (CFG_MAGIC >> 24) & 0xFF;
    page[4] = rec_count & 0xFF;  page[5]  = (rec_count >> 8)  & 0xFF;
    page[6] = (rec_count >> 16) & 0xFF; page[7] = (rec_count >> 24) & 0xFF;
    page[8] = write_addr & 0xFF; page[9]  = (write_addr >> 8)  & 0xFF;
    page[10]= (write_addr >> 16) & 0xFF; page[11]= (write_addr >> 24) & 0xFF;

    W25Q64_SectorErase(CFG_ADDR);        /* 擦 4KB 扇区 0 */
    W25Q64_PageProgram(CFG_ADDR, page, 12);
}

int Storage_Save(uint16_t light, uint16_t pot, int16_t p, int16_t r)
{
    log_rec_t rec;
    uint8_t *p8 = (uint8_t *)&rec;
    uint8_t i, sum = 0;

    rec.seq      = (uint16_t)(rec_count & 0xFFFF);
    rec.ts_ms    = rec_count;            /* 简化：以序号代替时间戳，后续可换 RTC */
    rec.light    = light;
    rec.pot_x100 = pot;
    rec.pitch_x10= p;
    rec.roll_x10 = r;
    for (i = 0; i < 14; i++) sum += p8[i];
    rec.crc = sum;

    W25Q64_PageProgram(write_addr, p8, 16);
    write_addr += 16;
    if (write_addr >= FLASH_SIZE) write_addr = DATA_ADDR;   /* 环形回卷 */
    rec_count++;
    save_config();
    return 0;
}

uint32_t Storage_Count(void)  { return rec_count; }
uint16_t Storage_LastSeq(void){ return (uint16_t)((rec_count ? rec_count - 1 : 0) & 0xFFFF); }
