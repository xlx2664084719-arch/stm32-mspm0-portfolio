#include "Timer.h"

bool timer_running = false;
uint8_t timer_hour = 0;
uint8_t timer_min = 0;
uint8_t timer_sec = 0;

void Timer_Init(void) {
    timer_running = false;
    timer_hour = 0;
    timer_min = 0;
    timer_sec = 0;
}

void Timer_Start(uint8_t h, uint8_t m, uint8_t s) {
    timer_hour = h;
    timer_min = m;
    timer_sec = s;
    timer_running = true;
}

void Timer_Stop(void) {
    timer_running = false;
    timer_hour = 0;
    timer_min = 0;
    timer_sec = 0;
}

void Timer_Tick(void) {
    if (!timer_running) return;

    if (timer_sec == 0) {
        if (timer_min == 0) {
            if (timer_hour == 0) {
                timer_running = false;
                OLED_ShowString(0, 1, "          ");
                return;
            }
            timer_hour--;
            timer_min = 59;
            timer_sec = 59;
        } else {
            timer_min--;
            timer_sec = 59;
        }
    } else {
        timer_sec--;
    }
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", timer_hour, timer_min, timer_sec);
    OLED_ShowString(0, 1, buf);
}