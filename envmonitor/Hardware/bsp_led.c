#include "bsp_led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio.GPIO_Pin   = GPIO_Pin_13;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);       /* 默认灭（低电平点亮） */
}

void LED_Set(uint8_t on)
{
    if (on) GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    else    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_Toggle(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13,
        (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13));
}
