#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oled_spi_V0.2.h"
#include "eeprom_emulation_type_a.h"
#include "snake_game_storage.h"   // ← 新增这行
#include <stdbool.h>
#include <stdint.h>

/* 贪吃蛇相关尺寸/数量常量 */
#define SNAKE_MAX_LENGTH         (70U)   /* 最大蛇身长度，满足胜利条件 */
#define SNAKE_RANK_MAX_NUM       (5U)    /* 排行榜条目数量，可根据需要调整 */
#define SNAKE_GAME_SCREEN_WIDTH  (128U)  /* OLED 宽度（像素） */
#define SNAKE_GAME_SCREEN_HEIGHT (64U)   /* OLED 高度（像素） */
#define SNAKE_GAME_GRID_SIZE     (8U)    /* 单元格边长像素，128/8=16 列，64/8=8 行 */
#define SNAKE_WIN_LENGTH         (70U)   /* 胜利条件：蛇身长度达到 70（占满屏幕） */

/* 游戏状态枚举 */
typedef enum {
    GAME_STATE_MAIN_MENU = 0,    // 主菜单
    GAME_STATE_PLAYING,           // 游戏中
    GAME_STATE_PAUSE,             // 暂停
    GAME_STATE_GAME_OVER,         // 游戏结束
    GAME_STATE_WIN,               // 游戏胜利
    GAME_STATE_RANK,              // 排行榜
    GAME_STATE_SETTING            // 设置界面
} Game_State;

/* 蛇方向枚举 */
typedef enum {
    SNAKE_DIR_UP = 0,
    SNAKE_DIR_DOWN,
    SNAKE_DIR_LEFT,
    SNAKE_DIR_RIGHT
} Snake_Dir;

/* 坐标结构体 */
typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

/* 排行榜结构体 */
typedef struct {
    uint16_t score;
    uint8_t  rank;
} Rank_Item;

/* 游戏难度枚举 */
typedef enum {
    GAME_DIFFICULTY_EASY = 0,
    GAME_DIFFICULTY_MEDIUM,
    GAME_DIFFICULTY_HARD
} Game_Difficulty;

/* 全局变量声明，与您的代码风格完全一致 */
extern Game_State g_game_state;
extern Snake_Dir g_snake_dir;
extern Snake_Dir g_snake_next_dir;
extern Point g_snake_body[SNAKE_MAX_LENGTH];
extern uint8_t g_snake_length;
extern Point g_food_pos;
extern uint16_t g_game_score;
extern uint16_t g_game_high_score;
extern Rank_Item g_game_rank[SNAKE_RANK_MAX_NUM];
extern Game_Difficulty g_game_difficulty;
extern uint32_t g_game_tick_cnt;

/* 函数声明，命名规范与您原有代码完全对齐 */
void Snake_Game_Init(void);
void Snake_Game_Tick(void);
void Snake_Game_Key_Process(uint8_t key);
void Snake_Game_Draw(void);
void Snake_Game_Save_Params(uint32_t * storage_state);
void Snake_Game_Load_Params(void);
void Snake_Game_Rank_Update(void);
void Snake_Game_Rank_Reset(void);
void Snake_Game_Low_Power_Check(void);
void Snake_Game_Sound_Eat(void);
void Snake_Game_Sound_GameOver(void);
void Snake_Game_Win_Show(void);

void Snake_Storage_Init(void);
void Snake_Storage_Save(uint32_t *state);   // 注意参数和数字钟一致
void Snake_Storage_Load(void);

#endif