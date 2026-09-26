#include "clock.h"
#include <string.h>
#include <stdio.h>

Clock_t clock={0};
Alarm_t alarms[3] = {0};
uint32_t eepromBuffer[EEPROM_EMULATION_DATA_SIZE/4];
uint32_t eepromState;
bool pause_tick = false;
bool in_set_mode = false;
bool is_12hour_mode = false;
bool alarm_set_submode = false;
bool timer_set_submode = false;
uint8_t current_alarm = 0; 
bool alarm_ringing = false; 
static uint8_t set_mode = 0;
void Clock_Init(void){
						eepromState = EEPROM_TypeA_init(&eepromBuffer[0]);	
		if (eepromState != EEPROM_EMULATION_INIT_OK) {
        OLED_ShowString(0,0,"INIT ERROR");
			__BKPT(0);
		}
						EEPROM_TypeA_readData(eepromBuffer);
            clock.hour = eepromBuffer[1];
            clock.min = eepromBuffer[2];
            clock.sec = eepromBuffer[3];
            clock.day = eepromBuffer[4];
            clock.month = eepromBuffer[5];
            clock.year = eepromBuffer[6];
            is_12hour_mode = eepromBuffer[7];  
            for (uint8_t i = 0; i < 3; i++) {
                uint16_t base = 8 + i * 6; 
                alarms[i].year = eepromBuffer[base];
                alarms[i].month = eepromBuffer[base+1];
                alarms[i].day = eepromBuffer[base+2];
                alarms[i].hour = eepromBuffer[base+3];
                alarms[i].min = eepromBuffer[base+4];
                alarms[i].enabled = eepromBuffer[base+5];
            }
						timer_hour   = eepromBuffer[26];
        timer_min    = eepromBuffer[27];
        timer_sec    = eepromBuffer[28];
        timer_running = (eepromBuffer[29] == 1);
            for (uint8_t i = 0; i < 3; i++) {
                alarms[i].enabled = false;
            }
            is_12hour_mode = false;
        
    
}
void Clock_Tick(void){
    clock.sec++;
    if (clock.sec >= 60) {
        clock.sec = 0;
        clock.min++;
        if (clock.min >= 60) {
            clock.min = 0;
            clock.hour++;
            if (clock.hour >= 24) {
                clock.hour = 0;
                if (!pause_tick) {
                    clock.day++;
                    static const uint8_t days_in_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                    uint8_t max_days = days_in_month[clock.month];
                    if (clock.month == 2 && ((clock.year % 4 == 0 && clock.year % 100 != 0) || (clock.year % 400 == 0))) {
                        max_days = 29;
                    }
                    if (clock.day > max_days) {
                        clock.day = 1;
                        clock.month++;
                        if (clock.month > 12) {
                            clock.month = 1;
                            clock.year++;
                        }
                    }
                }
            }
        }
    }

    for (uint8_t i = 0; i < 3; i++) {
        if (//alarms[i].enabled &&
            clock.year == alarms[i].year &&
            clock.month == alarms[i].month &&
            clock.day == alarms[i].day &&
            clock.hour == alarms[i].hour &&
            clock.min == alarms[i].min ) {
            alarm_ringing = true;
        }
    }
}
void Clock_Set(uint8_t key)
{
    pause_tick = in_set_mode;  // 设置模式下暂停日期进位

    // 按键0：循环切换模式（主设置 → 闹钟 → 定时器 → 退出）
    if (key == 0) {
        if (!in_set_mode) {
            // 第一次按0 → 进入主时间设置模式
            in_set_mode = true;
            alarm_set_submode = false;
            timer_set_submode = false;
            set_mode = 1;  // 从小时开始，避免无提示
            OLED_ShowString(0,6,"SetMode");
            OLED_ShowString(0,5,"          ");  // 清旧提示
            OLED_ShowString(0,5,"hour");   // 显示第一个项目
        }
        else if (!alarm_set_submode && !timer_set_submode) {
            // 第二次按0 → 进入闹钟设置子模式
            alarm_set_submode = true;
            timer_set_submode = false;
            current_alarm = 0;
            set_mode = 1;  // 闹钟从year开始
            OLED_ShowString(0,6,"AlarmSet");
            OLED_ShowString(0,5,"           ");

            // 立即显示当前闹钟的时分和年月日
            char time_str[9];
            Clock_GetAlarmTimeStr(time_str, current_alarm);
            OLED_ShowString(0, 0, time_str);

            char date_str[11];
            Clock_GetAlarmDateStr(date_str, current_alarm);
            OLED_ShowString(0, 2, date_str);
        }
        else if (alarm_set_submode && !timer_set_submode) {
            // 第三次按0 → 进入定时器设置子模式
            alarm_set_submode = false;
            timer_set_submode = true;
            set_mode = 1;  // 定时器从hour开始
            OLED_ShowString(0,6,"TimerSet");
            OLED_ShowString(0,5,"         ");
            OLED_ShowString(0,5,"hr");  // 显示第一个项目

            // 初始化并显示定时器初始值在第1行
            timer_hour = 0;
            timer_min = 0;
            timer_sec = 0;
            timer_running = false;
            char buf[9];
            sprintf(buf, "%02d:%02d:%02d", timer_hour, timer_min, timer_sec);
            OLED_ShowString(0, 1, buf);
        }
        else {
            // 第四次按0 → 完全退出所有设置模式
            in_set_mode = false;
            alarm_set_submode = false;
            timer_set_submode = false;
            timer_running = false;  // 退出时停止倒计时

            // 清屏防止重影
            OLED_ShowString(0,0,"                ");  // 清时间/闹钟时分行
            OLED_ShowString(0,2,"                             ");  // 清日期/闹钟日期行
            OLED_ShowString(0,5,"                ");            // 清项目提示
            OLED_ShowString(0,6,"                 ");          // 清模式提示
        }
        return;
    }

    // 非设置模式下忽略所有按键
    if (!in_set_mode) return;

    // 每次切换前清旧的项目提示（防止重影）
    OLED_ShowString(0,5,"     ");

    // 闹钟设置子模式
    if (alarm_set_submode) {
        if (key == 15) {  // 键15：切换闹钟项目或下一个闹钟
            if (set_mode < 5) {
                set_mode++;  // year → mon → day → hr → min
            } else {
                set_mode = 1;  // 循环回 year
                current_alarm = (current_alarm + 1) % 3;
            }

            // 显示当前闹钟编号 + 项目（短提示，防不全）
            char prompt[8];
            sprintf(prompt, "AL%d%s", current_alarm+1, 
                    set_mode==1 ? "year" : set_mode==2 ? "mon  " : set_mode==3 ? "day   " : set_mode==4 ? "hr   " : "min   ");
            OLED_ShowString(0,5,prompt);

            // 刷新当前闹钟显示
            char time_str[9];
            Clock_GetAlarmTimeStr(time_str, current_alarm);
            OLED_ShowString(0, 0, time_str);

            char date_str[11];
            Clock_GetAlarmDateStr(date_str, current_alarm);
            OLED_ShowString(0, 2, date_str);
        } 
        else {
            // 根据当前项目加值
            Alarm_t *a = &alarms[current_alarm];
            if (set_mode == 1) {a->year += key; if (a->year > 2100) a->year = 2000;}
            else if (set_mode == 2) {a->month = ((a->month - 1 + key) % 12) + 1;}
            else if (set_mode == 3) {a->day = ((a->day - 1 + key) % 31) + 1;}
            else if (set_mode == 4) {a->hour = (a->hour + key) % 24;}
            else if (set_mode == 5) {a->min = (a->min + key) % 60;}

            // 调整后立即刷新闹钟显示
            char time_str[9];
            Clock_GetAlarmTimeStr(time_str, current_alarm);
            OLED_ShowString(0, 0, time_str);

            char date_str[11];
            Clock_GetAlarmDateStr(date_str, current_alarm);
            OLED_ShowString(0, 2, date_str);
        }
    }
    // 定时器设置子模式
    else if (timer_set_submode) {
        static uint8_t timer_mode = 1;  // 1: hour, 2: min, 3: sec

        if (key == 15) {
            timer_mode = (timer_mode % 3) + 1;
            const char* prompts[] = {"","hr  ","min  ","sec  "};
            OLED_ShowString(0,5, prompts[timer_mode]);
        } 
        else {
            // 加值到当前项目
            if (timer_mode == 1) timer_hour = (timer_hour + key) % 24;
            else if (timer_mode == 2) timer_min  = (timer_min  + key) % 60;
            else if (timer_mode == 3) timer_sec  = (timer_sec  + key) % 60;

            // 实时显示定时器在第1行
            char buf[9];
            sprintf(buf, "%02d:%02d:%02d", timer_hour, timer_min, timer_sec);
            OLED_ShowString(0, 1, buf);
        }
    }
    // 主时间设置模式（原有完整逻辑）
    else {
        if (key == 15) {
            set_mode = (set_mode + 1) % 9;
            const char* prompts[] = {"","hour","minu","seco","day","mon","year"};
            OLED_ShowString(0,5, prompts[set_mode]);
        } 
        else if (set_mode == 1) clock.hour = (clock.hour + key) % 24;
        else if (set_mode == 2) clock.min  = (clock.min  + key) % 60;
        else if (set_mode == 3) clock.sec  = (clock.sec  + key) % 60;
        else if (set_mode == 4) clock.day  = ((clock.day  - 1 + key) % 31) + 1;
        else if (set_mode == 5) clock.month= ((clock.month - 1 + key) % 12) + 1;
        else if (set_mode == 6) clock.year += key; if (clock.year > 2100) clock.year = 2000;
    }

    Clock_Save(&eepromState);
}
void Clock_Save(uint32_t * state){
		eepromBuffer[1] = clock.hour;
    eepromBuffer[2] = clock.min;
    eepromBuffer[3] = clock.sec;
    eepromBuffer[4] = clock.day;
    eepromBuffer[5] = clock.month;
    eepromBuffer[6] = clock.year;
		eepromBuffer[7] = is_12hour_mode;
	for (uint8_t i = 0; i < 3; i++) {
        uint16_t base = 8 + i * 6;
        eepromBuffer[base] = alarms[i].year;
        eepromBuffer[base+1] = alarms[i].month;
        eepromBuffer[base+2] = alarms[i].day;
        eepromBuffer[base+3] = alarms[i].hour;
        eepromBuffer[base+4] = alarms[i].min;
        eepromBuffer[base+5] = alarms[i].enabled;
    }
	eepromBuffer[26] = timer_hour;
    eepromBuffer[27] = timer_min;
    eepromBuffer[28] = timer_sec;
    eepromBuffer[29] = timer_running ? 1 : 0;
	if(gEEPROMTypeAEraseFlag==1){
		EEPROM_TypeA_eraseLastSector();
		gEEPROMTypeAEraseFlag=0;
	}
	*state=EEPROM_TypeA_writeData(eepromBuffer);
}
void Clock_GetTimeStr(char *str) {
    if (is_12hour_mode) {
        uint8_t disp_hour = clock.hour % 12;
        if (disp_hour == 0) disp_hour = 12;  
        const char* am_pm = (clock.hour < 12) ? "AM" : "PM";
        sprintf(str, "%02d:%02d:%02d %s", disp_hour, clock.min, clock.sec, am_pm);
    } else {
        sprintf(str, "%02d:%02d:%02d", clock.hour, clock.min, clock.sec);
    }
}

void Clock_GetDateStr(char *str) {
    sprintf(str, "%04d-%02d-%02d", clock.year, clock.month, clock.day);
}
void Clock_GetAlarmTimeStr(char *str, uint8_t index) {
    Alarm_t *a = &alarms[index];
    sprintf(str, "%02d:%02d", a->hour, a->min);
}

void Clock_GetAlarmDateStr(char *str, uint8_t index) {
    Alarm_t *a = &alarms[index];
    sprintf(str, "%04d-%02d-%02d", a->year, a->month, a->day);
}
uint8_t Clock_GetWeekday(uint16_t year, uint8_t month, uint8_t day)
{
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    uint16_t century = year / 100;
    year %= 100;
    uint8_t weekday = (day + (13 * (month + 1) / 5) + year + year / 4 + century / 4 + 5 * century) % 7;
    return (weekday + 1) % 7;  // 调整为 0=周日, 1=周一, ..., 6=周六
}

// 生成带星期的日期字符串（放在第2行）
void Clock_GetDateWithWeekdayStr(char *str)
{
		const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
// 或用英文： {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    uint8_t w = Clock_GetWeekday(clock.year, clock.month, clock.day);
    sprintf(str, "%04d-%02d-%02d %s", clock.year, clock.month, clock.day, weekdays[w]);
}