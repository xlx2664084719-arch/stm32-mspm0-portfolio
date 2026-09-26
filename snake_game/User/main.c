/*------------------------------------------------------------------------------
 * File Name: ??????????
 * Description: ????MSPM0G3519?????????��???????????????????
 *----------------------------------------------------------------------------*/
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oledpicture_V0.2.h"
#include "oled_spi_V0.2.h"
#include <math.h>
#include "eeprom_emulation_type_a.h"
#include <string.h>
#include "snake_game.h"

/* ???????????��???????? */
void delay_ms(uint8_t ms){
    delay_cycles(ms*CPUCLK_FREQ/1000);
}

/* ???????????��??????��????100%???????????????? */
uint8_t scan_keyboard(void) {
    static uint8_t last_key = 0xFF;
    uint32_t h_arr[4] = {keyboard_H1_PIN, keyboard_H2_PIN, keyboard_H3_PIN, keyboard_H4_PIN};
    uint32_t v_arr[4] = {keyboard_V1_PIN, keyboard_V2_PIN, keyboard_V3_PIN, keyboard_V4_PIN};
    for (int i = 0; i < 4; i++) {
        DL_GPIO_clearPins(keyboard_PORT, h_arr[i]);
        delay_cycles(4800); 
        for (int j = 0; j < 4; j++) {
            if (DL_GPIO_readPins(keyboard_PORT, v_arr[j]) == 0) {  
                delay_cycles(48000);  
                if (DL_GPIO_readPins(keyboard_PORT, v_arr[j]) == 0) {  
                    uint8_t current_key = i * 4 + j;  
                    if (current_key != last_key) {
                        last_key = current_key;
                    }
                    DL_GPIO_setPins(keyboard_PORT, h_arr[i]);
                    return current_key;
                }
            }
        }
        DL_GPIO_setPins(keyboard_PORT, h_arr[i]);
    }
    if (last_key != 0xFF) {
        delay_cycles(240000); 
        uint8_t still_pressed = 0;
        for (int i = 0; i < 4; i++) {
            DL_GPIO_clearPins(keyboard_PORT, h_arr[i]);
            delay_cycles(4800);
            for (int j = 0; j < 4; j++) {
                if (DL_GPIO_readPins(keyboard_PORT, v_arr[j]) == 0) {
                    still_pressed = 1;
                }
            }
            DL_GPIO_setPins(keyboard_PORT, h_arr[i]);
            if (still_pressed) break;
        }
        if (!still_pressed) {
            last_key = 0xFF;
        }
    }
    return 0xFF;
}

/* 100ms 定时回调函数 */
void Timer_Game_Callback(void)
{
    // 执行游戏逻辑
    Snake_Game_Tick();
}

/* ???????????????????��?????????????? */
int main(void){
    // 初始化外设
    SYSCFG_DL_init();
    OLED_Init();
    Snake_Game_Init();
    Snake_Storage_Load();
    
    // 初始化界面
    OLED_Clear();
    OLED_ShowString(20, 2, "Snake Game");
    OLED_ShowString(10, 4, "MSPM0G3519");
    OLED_ShowString(0, 7, "Init OK, Press 0");
    OLED_Refresh();
    delay_ms(1000);
    OLED_Clear();
    
    // 主菜单界面
    OLED_ShowString(0, 0, "Snake Game");
    OLED_ShowString(0, 2, "0.Start Game");
    OLED_ShowString(0, 3, "1.Rank List");
    OLED_ShowString(0, 4, "2.Setting");
    OLED_Refresh();
    
    // 启动游戏定时器
    DL_Timer_startCounter(TIMER_0_INST);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    
    // 启动 PWM 用于音效
    DL_TimerA_startCounter(PWM_0_INST);
    
    while(1){
        // ????????????????????
        uint8_t key = scan_keyboard();
        if (key != 0xFF) {
            char buf[16];
            sprintf(buf, "Key:%02d S:%d", key, g_game_state);
            OLED_ShowString(0, 7, buf);
            OLED_Refresh();
            Snake_Game_Key_Process(key);
            delay_ms(100);
        }
        
        // ??????????
        
        delay_ms(20);
    }
}

/* ??????��????????100%?????????��??? */
void TIMER_0_INST_IRQHandler(void){
    DL_Timer_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    Timer_Game_Callback();
}

/* GPIO 中断处理函数 */
void GPIOB_INT_IRQHandler(void){
    uint32_t interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOB, 
        key_user_PIN | keyboard_V1_PIN | keyboard_V2_PIN | keyboard_V3_PIN | keyboard_V4_PIN);
    // 清除中断标志
    if (interrupt_status != 0) {
        DL_GPIO_clearInterruptStatus(GPIOB, interrupt_status);
    }
}