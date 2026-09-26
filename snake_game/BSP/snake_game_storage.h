#ifndef SNAKE_GAME_STORAGE_H
#define SNAKE_GAME_STORAGE_H
#include "ti_msp_dl_config.h"
#include "snake_game.h"

void Snake_Storage_Init(void);
void Snake_Storage_Save(uint32_t *state);   // 和数字钟完全一致的签名
void Snake_Storage_Load(void);

#endif
