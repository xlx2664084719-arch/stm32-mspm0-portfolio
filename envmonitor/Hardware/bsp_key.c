/**
  ******************************************************************************
  * @file    bsp_key.c
  * @brief   按键扫描：状态机式消抖（连续 3 次 10ms 采样一致才确认），
  *          长按计时用系统毫秒时基，全程非阻塞
  ******************************************************************************
  */
#include "bsp_key.h"

extern volatile uint32_t g_sys_ms;   /* SysTick 毫秒时基（main.c 维护） */

#define DEBOUNCE_TICKS   3           /* 3 x 10ms 消抖 */
#define LONG_PRESS_MS    1500

void Key_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin  = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_IPU;  /* 上拉输入，按下接 GND */
    GPIO_Init(GPIOA, &gpio);
}

static key_event_t key_event = KEY_EVENT_NONE;
static uint8_t  stable_state = 1;    /* 确认后的电平：1=松开 */
static uint8_t  cnt          = 0;
static uint32_t press_start  = 0;
static uint8_t  long_fired   = 0;

void Key_Task10ms(void)
{
    uint8_t raw = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

    if (raw != stable_state) {
        if (++cnt >= DEBOUNCE_TICKS) {           /* 连续一致才翻转状态 */
            stable_state = raw;
            cnt = 0;
            if (stable_state == 0) {             /* 确认按下：开始计时 */
                press_start = g_sys_ms;
                long_fired = 0;
            } else {                             /* 确认松开 */
                if (!long_fired)
                    key_event = KEY_EVENT_SHORT; /* 未触发过长按 -> 短按 */
            }
        }
    } else {
        cnt = 0;
        if (stable_state == 0 && !long_fired &&
            (g_sys_ms - press_start) >= LONG_PRESS_MS) {
            key_event = KEY_EVENT_LONG;
            long_fired = 1;
        }
    }
}

key_event_t Key_GetEvent(void)
{
    key_event_t e = key_event;
    key_event = KEY_EVENT_NONE;
    return e;
}
