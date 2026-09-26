/*------------------------------------------------------------------------------
 * File Name Empty_OLED
 * Description: Empty Programme with OLED and Keyboard support;
 *----------------------------------------------------------------------------*/
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oledpicture_V0.2.h"
#include "oled_spi_V0.2.h"
#include <math.h>
#include "eeprom_emulation_type_a.h"
#include <string.h>
#include "clock.h"
#include "Timer.h"
#include "MusicPlayer.h"
#include "MusicScore.h"


//extern bool in_set_mode; 
void delay_ms(uint8_t ms){
    delay_cycles(ms*CPUCLK_FREQ/1000);
}

uint8_t scan_keyboard(void) {
    static uint8_t last_key = 0xFF;
    uint32_t h_arr[4] = {keyboard_H1_PIN, keyboard_H2_PIN, keyboard_H3_PIN, keyboard_H4_PIN};
    uint32_t v_arr[4] = {keyboard_V1_PIN, keyboard_V2_PIN, keyboard_V3_PIN, keyboard_V4_PIN};
    for (int i = 0; i < 4; i++) {
        DL_GPIO_clearPins(keyboard_PORT, h_arr[i]);
        delay_cycles(4800); 
        for (int j = 0; j < 4; j++) {
            if (DL_GPIO_readPins(keyboard_PORT, v_arr[j]) == 0) {  
                delay_cycles(480000);  
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
        delay_cycles(2400000); 
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


void Timer_1s_Callback(void)
{
    Clock_Tick();      // 更新主时间
    Timer_Tick();      // 更新定时器倒计时
		Clock_Save(&eepromState);
    // 如果在闹钟设置子模式，不刷新主时间（保持闹钟显示）
    if (in_set_mode && alarm_set_submode) {
        return;
    }

    // 正常刷新主时间和日期（第0行和第2行）
    char time_str[9];
    Clock_GetTimeStr(time_str);
    OLED_ShowString(0,0,time_str);

    char date_str[20];  // 空间要够 "2025-01-16 周四" ≈ 15~16字符
    Clock_GetDateWithWeekdayStr(date_str);
    OLED_ShowString(0,2,date_str);

    // 新增：只要定时器在运行，且不在任何设置模式，就持续刷新第1行倒计时
    if (timer_running && !in_set_mode && !alarm_set_submode && !timer_set_submode) {
        char buf[9];
        sprintf(buf, "%02d:%02d:%02d", timer_hour, timer_min, timer_sec);
        OLED_ShowString(0, 1, buf);
    } 
//    else if (!timer_running) {
//        // 定时器停止时清空第1行（防止残留）
//        OLED_ShowString(0, 1, "          ");
//    }
}

int main(void){
    SYSCFG_DL_init();
    Keyboard_init();
    OLED_Init();
    Clock_Init();
    OLED_Clear();
		Timer_Init();
//	  MusicPlayer_init();
    DL_Timer_startCounter(TIMER_0_INST);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    in_set_mode = false;
    while(1){
        uint8_t key = scan_keyboard();
        if (key != 0xFF) {
            Clock_Set(key);
            char key_str[3];
            sprintf(key_str, "%02d", key);
            OLED_ShowString(0, 4, key_str);
            delay_ms(200);
        }
        static bool last_user_state = true;
        bool current_user = DL_GPIO_readPins(key_PORT, key_user_PIN);
        if (last_user_state && !current_user) {
            is_12hour_mode = !is_12hour_mode;
            Clock_Save(&eepromState);
            OLED_ShowString(0, 6, is_12hour_mode ? "12H Mode" : "24H Mode");
						OLED_ShowString(0, 0, "                                 ");
            delay_ms(50);
        }
        last_user_state = current_user;
				if (in_set_mode && alarm_set_submode) {
				char time_str[9];
				Clock_GetAlarmTimeStr(time_str, current_alarm);
				OLED_ShowString(0, 0, time_str);
				char date_str[11];
				Clock_GetAlarmDateStr(date_str, current_alarm);
				OLED_ShowString(0, 2, date_str);
					}
				if (alarm_ringing) {
//              playMusic(MEGALOVANIA,200);
						DL_GPIO_togglePins(LED_L2_PORT,LED_L2_PIN);
					delay_ms(100);
        } 
        if (key == 0 && alarm_ringing) {
            alarm_ringing = false;
            DL_GPIO_setPins(LED_L2_PORT, LED_L2_PIN);
        }
        delay_ms(20);
    }
}

void TIMER_0_INST_IRQHandler(void){
    DL_Timer_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    Timer_1s_Callback();
	DL_GPIO_togglePins(LED_L1_PORT,LED_L1_PIN);
}