#ifndef CLOCK_H
#define CLOCK_H
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include <math.h> 
#include "eeprom_emulation_type_a.h"
#include <string.h>
#include "Timer.h"

typedef struct {
    uint8_t hour, min, sec, day, month;
    uint16_t year;
    uint8_t alarm_hour, alarm_min;
    uint8_t alarm_on;
} Clock_t;
typedef struct {
    uint8_t hour, min;
    uint8_t day, month;
    uint16_t year;
    bool enabled; 
} Alarm_t;
extern bool is_12hour_mode;
extern Alarm_t alarms[3];
extern uint8_t current_alarm; 
extern bool alarm_set_submode; 
extern bool alarm_ringing; 
extern bool in_set_mode;
extern uint32_t eepromState;
extern bool timer_running;
extern uint8_t timer_hour;
extern uint8_t timer_min;
extern uint8_t timer_sec;
extern bool timer_set_submode;

void Clock_Init(void);
void Clock_Tick(void);
void Clock_Set(uint8_t key);
void Clock_Save(uint32_t* state);
void Clock_GetTimeStr(char *str);
void Clock_GetDateStr(char *str);
void Clock_GetAlarmTimeStr(char *str, uint8_t index) ;
void Clock_GetAlarmDateStr(char *str, uint8_t index);
void Timer_Init(void);
void Timer_Start(uint8_t h, uint8_t m, uint8_t s);
void Timer_Stop(void);
void Timer_Tick(void);
uint8_t Clock_GetWeekday(uint16_t year, uint8_t month, uint8_t day);  // 计算星期几，返回0~6 (0=周日,1=周一,...,6=周六)
void Clock_GetDateWithWeekdayStr(char *str);  // 新函数：生成带星期的日期字符串
	#endif