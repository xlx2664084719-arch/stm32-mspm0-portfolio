#include "bsp_buzzer.h"

extern volatile uint32_t g_sys_ms;

static uint32_t beep_until = 0;

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio.GPIO_Pin   = GPIO_Pin_15;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpio);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);       /* 低电平触发，默认高=不响 */
}

void Buzzer_Set(uint8_t on)
{
    if (on) GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    else    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void Buzzer_Beep(uint16_t ms)
{
    beep_until = g_sys_ms + ms;
    Buzzer_Set(1);
}

void Buzzer_Task1ms(void)
{
    if (beep_until && (int32_t)(g_sys_ms - beep_until) >= 0) {
        Buzzer_Set(0);
        beep_until = 0;
    }
}
