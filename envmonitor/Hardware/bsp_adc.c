/**
  ******************************************************************************
  * @file    bsp_adc.c
  * @brief   ADC1 规则组双通道连续扫描，DMA 循环搬运到内存缓冲；
  *          应用层读取时做滑动平均滤波
  *
  * 讲解要点：连续扫描 + 循环 DMA 让采样完全由硬件自动完成，
  * CPU 零开销读取最新值 —— 这是"DMA 减轻 CPU 负担"的最小示例
  ******************************************************************************
  */
#include "bsp_adc.h"
#include "delay.h"

static volatile uint16_t adc_raw[ADC_CH_NUM];        /* DMA 目标：ch0=PA1 ch1=PA2 */
static uint16_t adc_hist[ADC_CH_NUM][ADC_SMOOTH_N];  /* 滤波历史窗口 */
static uint8_t  hist_idx = 0;
static uint8_t  hist_filled = 0;

void ADC_DMA_Init(void)
{
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef  adc;
    DMA_InitTypeDef  dma;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);               /* ADC 时钟 = 72/6 = 12MHz（上限14M） */

    /* PA1/PA2 模拟输入 */
    gpio.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio);

    /* DMA1 通道1 -> ADC1 */
    DMA_DeInit(DMA1_Channel1);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma.DMA_MemoryBaseAddr     = (uint32_t)adc_raw;
    dma.DMA_DIR                = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize         = ADC_CH_NUM;
    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode               = DMA_Mode_Circular;  /* 循环模式：永动 */
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1：连续 + 扫描，软件触发后一直跑 */
    adc.ADC_Mode                   = ADC_Mode_Independent;
    adc.ADC_ScanConvMode           = ENABLE;
    adc.ADC_ContinuousConvMode     = ENABLE;
    adc.ADC_ExternalTrigConv       = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign              = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel           = ADC_CH_NUM;
    ADC_Init(ADC1, &adc);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5); /* PA1 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5); /* PA2 */

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);                     /* 上电校准，精度关键 */
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);         /* 启动，之后硬件永动 */
}

/* 每 100ms 调一次：更新滤波窗口 */
void ADC_FilterTick(void)
{
    uint8_t ch;
    for (ch = 0; ch < ADC_CH_NUM; ch++)
        adc_hist[ch][hist_idx] = adc_raw[ch];
    hist_idx = (hist_idx + 1) % ADC_SMOOTH_N;
    if (hist_idx == 0) hist_filled = 1;
}

uint16_t ADC_GetSmoothed(uint8_t ch)
{
    uint8_t i, n = hist_filled ? ADC_SMOOTH_N : hist_idx;
    uint32_t sum = 0;
    if (n == 0) return adc_raw[ch];
    for (i = 0; i < n; i++) sum += adc_hist[ch][i];
    return (uint16_t)(sum / n);
}

uint16_t ADC_GetRaw(uint8_t ch)
{
    return ADC_GetSmoothed(ch);
}

uint16_t ADC_GetLight(void)
{
    /* 实测该模块：光照越强 AO 电压越低，取反后即为亮度百分比（已验证） */
    uint32_t raw = ADC_GetSmoothed(0);
    uint32_t pct = raw * 100 / 4095;
    return (uint16_t)(100 - pct);
}

/* ---- 光照基线自校准 ----
 * 不同厂家光敏模块 AO 电压随光照的变化方向不同（分压拓扑决定），
 * 与其猜测极性，不如上电把当前环境光采为基线，报警看"相对偏差"，
 * 与方向无关 —— 换任何模拟光传感器都成立 */
static uint16_t light_baseline = 2048;   /* 默认中点，校准前兜底 */

void ADC_CaptureLightBaseline(void)
{
    uint8_t i;
    uint32_t sum = 0;
    for (i = 0; i < 64; i++) {
        sum += adc_raw[0];
        delay_ms(4);                     /* 256ms 采 64 点均值 */
    }
    light_baseline = (uint16_t)(sum / 64);
}

uint16_t ADC_GetLightBaseline(void)  { return light_baseline; }

/* 相对基线的偏差（绝对值），0~4095 */
uint16_t ADC_GetLightDeviation(void)
{
    uint16_t now = ADC_GetSmoothed(0);
    return (now > light_baseline) ? (now - light_baseline) : (light_baseline - now);
}

uint16_t ADC_GetPotVoltageX100(void)
{
    /* 电位器分压 0~3.3V -> x100 显示 */
    uint32_t raw = ADC_GetSmoothed(1);
    return (uint16_t)(raw * 330 / 4095);
}
