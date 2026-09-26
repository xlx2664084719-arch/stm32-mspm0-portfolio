/**
  ******************************************************************************
  * @file    uart.c
  * @brief   USART1 初始化 + printf 字符输出重定向
  *
  * 对比 MSPM0 的思路：
  *   - MSPM0 由 SysConfig 生成 UART 配置；这里手动配，但套路固定：
  *     1. 开时钟（GPIOA + USART1）—— F1 必须手动开，最容易忘！
  *     2. 配引脚复用（TX 复用推挽，RX 浮空输入）
  *     3. 配 USART 参数并使能
  ******************************************************************************
  */
#include "uart.h"

void UART1_Init(uint32_t baud)
{
    GPIO_InitTypeDef  gpio;
    USART_InitTypeDef usart;

    /* 1. 开时钟：GPIOA 挂在 APB2，USART1 也挂在 APB2（F1 里 USART1 特殊在 APB2） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* 2. PA9 = TX，复用推挽输出 */
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* 3. PA10 = RX，浮空输入 */
    gpio.GPIO_Pin  = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    /* 4. 配置并使能 USART1 */
    usart.USART_BaudRate            = baud;
    usart.USART_WordLength          = USART_WordLength_8b;
    usart.USART_StopBits            = USART_StopBits_1;
    usart.USART_Parity              = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

/* printf 底层：每次输出一个字符（MicroLIB 下生效） */
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
