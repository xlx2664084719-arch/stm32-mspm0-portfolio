#include "snake_game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 全局变量定义，与您的代码风格完全一致 */
Game_State g_game_state = GAME_STATE_MAIN_MENU;
Snake_Dir g_snake_dir = SNAKE_DIR_RIGHT;
Snake_Dir g_snake_next_dir = SNAKE_DIR_RIGHT;
Point g_snake_body[SNAKE_MAX_LENGTH] = {0};
uint8_t g_snake_length = 0;
Point g_food_pos = {0};
uint16_t g_game_score = 0;
uint16_t g_game_high_score = 0;
Rank_Item g_game_rank[SNAKE_RANK_MAX_NUM] = {0};
Game_Difficulty g_game_difficulty = GAME_DIFFICULTY_MEDIUM;
uint32_t g_game_tick_cnt = 0;
uint32_t storage_state;  // 存储状态变量，和数字钟一致

/* 难度对应刷新间隔（ms），与您的定时器时基完全匹配 */
/* Medium 速度是普通的 2 倍（250ms = 500ms/2），Hard 速度是普通的 4 倍（125ms = 500ms/4） */
const uint16_t g_game_speed_table[3] = {500, 250, 125};

/* 游戏初始化函数 */
void Snake_Game_Init(void)
{
    Snake_Storage_Init();   // ← 新增这行
    
    // 初始化默认参数
    g_game_high_score = 0;
    g_game_difficulty = GAME_DIFFICULTY_MEDIUM;
    memset(g_game_rank, 0, sizeof(g_game_rank));
    g_game_tick_cnt = 0;
    g_game_state = GAME_STATE_MAIN_MENU;
}

/* 游戏核心逻辑帧函数，与您的 Clock_Tick 风格一致 */
void Snake_Game_Tick(void)
{
    // 仅游戏运行状态执行逻辑
    if (g_game_state != GAME_STATE_PLAYING) {
        return;
    }
    g_game_tick_cnt++;

    // 按难度控制刷新帧率
    uint16_t tick_threshold = g_game_speed_table[g_game_difficulty];
    
    // 调整难度对应的 tick 次数（定时器 100ms 触发一次）
    // Easy: 200ms = 2 次，Medium: 150ms = 1-2 次，Hard: 100ms = 1 次
    if (g_game_difficulty == GAME_DIFFICULTY_EASY) {
        tick_threshold = 2;
    } else if (g_game_difficulty == GAME_DIFFICULTY_MEDIUM) {
        tick_threshold = 1;
    } else {
        tick_threshold = 1;
    }
    
    if (g_game_tick_cnt < tick_threshold) {
        return;
    }
    g_game_tick_cnt = 0;

    // 更新蛇方向
    g_snake_dir = g_snake_next_dir;
    
    // 切换 LED_L3 表示小蛇正在移动
    DL_GPIO_togglePins(LED_L3_PORT, LED_L3_PIN);

    // 移动蛇身体
    for (int i = g_snake_length - 1; i > 0; i--) {
        g_snake_body[i] = g_snake_body[i - 1];
    }

    // 更新蛇头位置
    switch (g_snake_dir) {
        case SNAKE_DIR_UP:
            g_snake_body[0].y -= 8;
            break;
        case SNAKE_DIR_DOWN:
            g_snake_body[0].y += 8;
            break;
        case SNAKE_DIR_LEFT:
            g_snake_body[0].x -= 8;
            break;
        case SNAKE_DIR_RIGHT:
            g_snake_body[0].x += 8;
            break;
    }

    // 撞墙检测（像素坐标）
    if (g_snake_body[0].x < 8 || g_snake_body[0].x > 112 || g_snake_body[0].y < 8 || g_snake_body[0].y > 40) {
        g_game_state = GAME_STATE_GAME_OVER;
        // Snake_Game_Sound_GameOver();  // 禁用音效，避免阻塞
        Snake_Game_Rank_Update();
        return;
    }

    // 撞身体检测
    for (int i = 1; i < g_snake_length; i++) {
        if (g_snake_body[0].x == g_snake_body[i].x && g_snake_body[0].y == g_snake_body[i].y) {
            g_game_state = GAME_STATE_GAME_OVER;
            // Snake_Game_Sound_GameOver();  // 禁用音效，避免阻塞
            Snake_Game_Rank_Update();
            return;
        }
    }

    // 吃到食物检测
    if (g_snake_body[0].x == g_food_pos.x && g_snake_body[0].y == g_food_pos.y) {
        // 加分
        g_game_score += 10;
        if (g_game_score > g_game_high_score) {
            g_game_high_score = g_game_score;
        }
        // 蛇身变长：复制最后一个蛇身的位置
        if (g_snake_length < SNAKE_MAX_LENGTH) {
            g_snake_body[g_snake_length] = g_snake_body[g_snake_length - 1];
            g_snake_length++;
        }
        // 检查是否达到胜利条件
        if (g_snake_length >= SNAKE_WIN_LENGTH) {
            g_game_state = GAME_STATE_WIN;
            Snake_Game_Rank_Update();
            return;
        }
        // 播放音效 - 简化，减少阻塞
        // Snake_Game_Sound_Eat();  // 暂时禁用，避免卡死
        // LED 闪烁
        DL_GPIO_togglePins(LED_L3_PORT, LED_L3_PIN);
        // 重新生成食物：在边界内随机位置（像素坐标，8 的倍数）
        uint8_t food_valid;
        uint8_t retry_cnt = 0;
        const uint8_t max_retry = 50;  // 增加重试次数
        do {
            food_valid = 1;
            // 使用更随机的种子
            g_food_pos.x = (rand() % 14 + 1) * 8;
            g_food_pos.y = (rand() % 5 + 1) * 8;
            // 避免食物生成在蛇身上
            for (int i = 0; i < g_snake_length; i++) {
                if (g_snake_body[i].x == g_food_pos.x && g_snake_body[i].y == g_food_pos.y) {
                    food_valid = 0;
                    break;
                }
            }
            retry_cnt++;
            // 超过最大重试次数，强制退出（避免死循环）
            if (retry_cnt >= max_retry) {
                // 使用默认位置
                g_food_pos.x = 64;
                g_food_pos.y = 24;
                food_valid = 1;
                break;
            }
        } while (!food_valid);
        // 保存参数
        Snake_Game_Save_Params(&storage_state);
    }

    // 刷新画面
    Snake_Game_Draw();
}

/* 按键处理函数，与您的 Clock_Set 风格完全一致 */
void Snake_Game_Key_Process(uint8_t key)
{
    // 按键映射：1=上 9=下 3=左 4=右 0=确认/暂停 15=返回/退出
    switch (g_game_state) {
        // 主菜单按键处理
        case GAME_STATE_MAIN_MENU:
            if (key == 0) {
                // 开始游戏
                g_game_state = GAME_STATE_PLAYING;
                // 完全重置游戏状态
                g_snake_length = 3;
                g_snake_body[0].x = 64; g_snake_body[0].y = 24;
                g_snake_body[1].x = 56; g_snake_body[1].y = 24;
                g_snake_body[2].x = 48; g_snake_body[2].y = 24;
                // 清除多余的蛇身
                for (int i = 3; i < SNAKE_MAX_LENGTH; i++) {
                    g_snake_body[i].x = 0;
                    g_snake_body[i].y = 0;
                }
                g_snake_dir = SNAKE_DIR_RIGHT;
                g_snake_next_dir = SNAKE_DIR_RIGHT;
                g_game_score = 0;
                g_game_tick_cnt = 0;
                // 生成初始食物（8 的倍数）
                // 使用多重随机种子，避免伪随机
                srand(DL_TimerA_getTimerCount(TIMER_0_INST) ^ (uint32_t)(&g_game_tick_cnt) + g_game_score + 1);
                g_food_pos.x = (rand() % 14 + 1) * 8;
                g_food_pos.y = (rand() % 5 + 1) * 8;
                // 完整清屏
                OLED_Clear();
                Snake_Game_Draw();
            } else if (key == 1) {
                // 查看排行榜
                g_game_state = GAME_STATE_RANK;
                // 完整清屏，避免花屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Rank List");
                char buf[16];
                for (int i = 0; i < SNAKE_RANK_MAX_NUM; i++) {
                    sprintf(buf, "%d. %04d", i+1, g_game_rank[i].score);
                    OLED_ShowString(0, i+2, buf);
                }
                OLED_Refresh();
            } else if (key == 2) {
                // 进入设置
                g_game_state = GAME_STATE_SETTING;
                // 完整清屏，避免花屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Game Setting");
                // 直接显示当前难度，不使用 sprintf
                if (g_game_difficulty == 0) {
                    OLED_ShowString(0, 2, "Diff: Easy");
                } else if (g_game_difficulty == 1) {
                    OLED_ShowString(0, 2, "Diff: Medium");
                } else {
                    OLED_ShowString(0, 2, "Diff: Hard");
                }
                OLED_ShowString(0, 4, "1:Change Diff");
                OLED_ShowString(0, 5, "3:Reset Rank");
                OLED_ShowString(0, 7, "15:Back");
                OLED_Refresh();
            }
            break;

        // 游戏中按键处理
        case GAME_STATE_PLAYING:
            if (key == 0) {
                // 暂停游戏
                g_game_state = GAME_STATE_PAUSE;
                OLED_ShowString(40, 3, "PAUSE");
            } else if (key == 5 && g_snake_dir != SNAKE_DIR_DOWN) {
                g_snake_next_dir = SNAKE_DIR_UP;
            } else if (key == 9 && g_snake_dir != SNAKE_DIR_UP) {
                g_snake_next_dir = SNAKE_DIR_DOWN;
            } else if (key == 8 && g_snake_dir != SNAKE_DIR_RIGHT) {
                g_snake_next_dir = SNAKE_DIR_LEFT;
            } else if (key == 10 && g_snake_dir != SNAKE_DIR_LEFT) {
                g_snake_next_dir = SNAKE_DIR_RIGHT;
            }
            break;

        // 暂停状态按键处理
        case GAME_STATE_PAUSE:
            if (key == 0) {
                // 继续游戏
                g_game_state = GAME_STATE_PLAYING;
                Snake_Game_Draw();
            } else if (key == 15) {
                // 退出到主菜单
                g_game_state = GAME_STATE_MAIN_MENU;
                // 完整清屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Snake Game");
                OLED_ShowString(0, 2, "0.Start Game");
                OLED_ShowString(0, 3, "1.Rank List");
                OLED_ShowString(0, 4, "2.Setting");
                OLED_Refresh();
            }
            break;

        // 游戏结束按键处理
        case GAME_STATE_GAME_OVER:
            if (key == 0 || key == 15) {
                // 返回主菜单
                g_game_state = GAME_STATE_MAIN_MENU;
                // 完整清屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Snake Game");
                OLED_ShowString(0, 2, "0.Start Game");
                OLED_ShowString(0, 3, "1.Rank List");
                OLED_ShowString(0, 4, "2.Setting");
                OLED_Refresh();
            }
            break;

        // 游戏胜利按键处理
        case GAME_STATE_WIN:
            if (key == 0 || key == 15) {
                // 返回主菜单
                g_game_state = GAME_STATE_MAIN_MENU;
                // 完整清屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Snake Game");
                OLED_ShowString(0, 2, "0.Start Game");
                OLED_ShowString(0, 3, "1.Rank List");
                OLED_ShowString(0, 4, "2.Setting");
                OLED_Refresh();
            }
            break;

        // 排行榜按键处理
        case GAME_STATE_RANK:
            if (key == 15) {
                // 返回主菜单
                g_game_state = GAME_STATE_MAIN_MENU;
                // 完整清屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Snake Game");
                OLED_ShowString(0, 2, "0.Start Game");
                OLED_ShowString(0, 3, "1.Rank List");
                OLED_ShowString(0, 4, "2.Setting");
                OLED_Refresh();
            }
            break;

        // 设置界面按键处理
        case GAME_STATE_SETTING:
            if (key == 1) {
                // 切换难度
                g_game_difficulty = (g_game_difficulty + 1) % 3;
                // 直接显示当前难度，不使用 sprintf
                if (g_game_difficulty == 0) {
                    OLED_ShowString(0, 2, "Diff: Easy      ");
                } else if (g_game_difficulty == 1) {
                    OLED_ShowString(0, 2, "Diff: Medium    ");
                } else {
                    OLED_ShowString(0, 2, "Diff: Hard      ");
                }
                OLED_Refresh();
            } else if (key == 3) {
                // 重置排行榜
                Snake_Game_Rank_Reset();
                OLED_ShowString(0, 5, "Rank Reset OK");
                OLED_Refresh();
            } else if (key == 15) {
                // 返回主菜单
                g_game_state = GAME_STATE_MAIN_MENU;
                // 完整清屏
                OLED_Clear();
                OLED_ShowString(0, 0, "Snake Game");
                OLED_ShowString(0, 2, "0.Start Game");
                OLED_ShowString(0, 3, "1.Rank List");
                OLED_ShowString(0, 4, "2.Setting");
                OLED_Refresh();
            }
            break;
    }
}

/* 游戏画面绘制函数 - 优化版本 */
void Snake_Game_Draw(void)
{
    // 清除整个游戏区域（第 0-6 行）
    // 比 OLED_Clear() 更快，因为只清除需要的区域
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 16; col++) {
            OLED_ShowChar(col * 8, row, ' ');
        }
    }

    // 绘制边框
    // 上边界 y=0
    OLED_ShowString(0, 0, "----------------");
    // 下边界 y=6
    OLED_ShowString(0, 6, "----------------");
    // 左右边界 x=0 和 x=120（像素坐标），y=1-5
    for (int i = 1; i < 6; i++) {
        OLED_ShowString(0, i, "+");
        OLED_ShowString(120, i, "+");
    }

    // 绘制食物
    OLED_ShowString(g_food_pos.x, g_food_pos.y / 8, "#");

    // 绘制蛇身
    for (int i = 0; i < g_snake_length; i++) {
        OLED_ShowString(g_snake_body[i].x, g_snake_body[i].y / 8, "*");
    }

    // 绘制分数
    char buf[16];
    sprintf(buf, "S:%03d", g_game_score);
    OLED_ShowString(72, 0, buf);

    OLED_Refresh();
}

/* 游戏参数保存 */
void Snake_Game_Save_Params(uint32_t * storage_state)
{
    Snake_Storage_Save(&storage_state);   // ← 和数字钟一样使用 &storage_state
}

/* 游戏参数加载 */
void Snake_Game_Load_Params(void)
{
    Snake_Storage_Load();   // ← 和数字钟一样
}

/* 排行榜更新函数 */
void Snake_Game_Rank_Update(void)
{
    // 插入新分数
    if (g_game_score == 0) return;
    uint16_t temp_scores[SNAKE_RANK_MAX_NUM + 1] = {0};
    for (int i = 0; i < SNAKE_RANK_MAX_NUM; i++) {
        temp_scores[i] = g_game_rank[i].score;
    }
    temp_scores[SNAKE_RANK_MAX_NUM] = g_game_score;

    // 排序
    for (int i = 0; i < SNAKE_RANK_MAX_NUM + 1; i++) {
        for (int j = i + 1; j < SNAKE_RANK_MAX_NUM + 1; j++) {
            if (temp_scores[i] < temp_scores[j]) {
                uint16_t temp = temp_scores[i];
                temp_scores[i] = temp_scores[j];
                temp_scores[j] = temp;
            }
        }
    }

    // 更新排行榜
    for (int i = 0; i < SNAKE_RANK_MAX_NUM; i++) {
        g_game_rank[i].score = temp_scores[i];
        g_game_rank[i].rank = i + 1;
    }

    // 保存到 Flash - 仅在分数有效时保存
    if (g_game_score > 0) {
        Snake_Game_Save_Params(&storage_state);
    }

    // 检查是否是胜利状态
    if (g_game_state == GAME_STATE_WIN) {
        Snake_Game_Win_Show();
    } else {
        // 游戏结束界面显示 - 完整清屏，避免花屏
        OLED_Clear();
        OLED_ShowString(30, 1, "GAME OVER");
        char buf[16];
        sprintf(buf, "Score: %04d", g_game_score);
        OLED_ShowString(30, 3, buf);
        OLED_ShowString(10, 6, "0/15:Back Menu");
        OLED_Refresh();
    }
}

/* 游戏胜利界面显示 */
void Snake_Game_Win_Show(void)
{
    OLED_Clear();
    OLED_ShowString(20, 0, "CONGRATULATIONS!");
    OLED_ShowString(35, 2, "YOU WIN!");
    char buf[16];
    sprintf(buf, "Score: %04d", g_game_score);
    OLED_ShowString(30, 4, buf);
    sprintf(buf, "Length: %d", g_snake_length);
    OLED_ShowString(30, 6, buf);
    OLED_ShowString(10, 7, "0/15:Back Menu");
    OLED_Refresh();
}

/* 排行榜重置函数 */
void Snake_Game_Rank_Reset(void)
{
    memset(g_game_rank, 0, sizeof(g_game_rank));
    g_game_high_score = 0;
    Snake_Game_Save_Params(&storage_state);
}

/* 低功耗检测函数 - 已禁用 */
void Snake_Game_Low_Power_Check(void)
{
    // 已完全禁用低功耗功能
}

/* 吃到食物音效，复用您原有PWM外设 */
void Snake_Game_Sound_Eat(void)
{
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 5000, DL_TIMER_CC_0_INDEX);
    delay_cycles(CPUCLK_FREQ / 1000 * 50);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_0_INDEX);
}

/* 游戏结束音效，复用您原有 PWM 外设 */
void Snake_Game_Sound_GameOver(void)
{
    for (int i = 0; i < 3; i++) {
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 3000, DL_TIMER_CC_0_INDEX);
        delay_cycles(CPUCLK_FREQ / 1000 * 100);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_0_INDEX);
        delay_cycles(CPUCLK_FREQ / 1000 * 50);
    }
}

