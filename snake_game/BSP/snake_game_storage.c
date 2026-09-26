#include "snake_game_storage.h"
#include "eeprom_emulation_type_a.h"

static uint32_t storage_buffer[EEPROM_EMULATION_DATA_SIZE / 4];
static uint32_t storage_state;

void Snake_Storage_Init(void)
{
    storage_state = EEPROM_TypeA_init(&storage_buffer[0]);
    if (storage_state != EEPROM_EMULATION_INIT_OK) {
        OLED_ShowString(0,0,"Storage Init Err");
        __BKPT(0);
    }
    Snake_Storage_Load();
}

void Snake_Storage_Save(uint32_t *state)
{
    storage_buffer[1] = g_game_high_score;
    storage_buffer[2] = g_game_difficulty;

    for (uint8_t i = 0; i < SNAKE_RANK_MAX_NUM; i++) {
        storage_buffer[4 + i] = g_game_rank[i].score;
    }

    if (gEEPROMTypeAEraseFlag == 1) {
        EEPROM_TypeA_eraseLastSector();
        gEEPROMTypeAEraseFlag = 0;
    }

    *state = EEPROM_TypeA_writeData(storage_buffer);   // 和数字钟完全一致
}

void Snake_Storage_Load(void)
{
    EEPROM_TypeA_readData(storage_buffer);   // 和数字钟一样，不赋值

    g_game_high_score = storage_buffer[1];
    g_game_difficulty = storage_buffer[2];

    for (uint8_t i = 0; i < SNAKE_RANK_MAX_NUM; i++) {
        // 检查分数是否有效（0-9999 范围内）
        if (storage_buffer[4 + i] > 9999) {
            g_game_rank[i].score = 0;
        } else {
            g_game_rank[i].score = storage_buffer[4 + i];
        }
        g_game_rank[i].rank = i + 1;
    }
}
