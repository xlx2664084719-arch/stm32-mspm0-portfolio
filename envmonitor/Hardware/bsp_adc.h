/**
  ******************************************************************************
  * @file    bsp_adc.h
  * @brief   ADC1 双通道扫描 + DMA 循环采集（PA1=光照 AO，PA2=电位器）
  ******************************************************************************
  */
#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f10x.h"

#define ADC_CH_NUM  2
#define ADC_SMOOTH_N 8          /* 滑动平均窗口 */

void ADC_DMA_Init(void);
void ADC_FilterTick(void);               /* 每 100ms 调用：更新滑动平均窗口 */
uint16_t ADC_GetSmoothed(uint8_t ch);    /* 取某通道的滑动平均原始值 0~4095 */
uint16_t ADC_GetRaw(uint8_t ch);         /* 同上（语义别名） */
uint16_t ADC_GetLight(void);             /* 亮度百分比 0~100（越亮越大，已验证） */

/* 光照：基线自校准 + 相对偏差报警（与模块输出极性无关） */
void     ADC_CaptureLightBaseline(void); /* 上电采一次环境光做基线 */
uint16_t ADC_GetLightBaseline(void);
uint16_t ADC_GetLightDeviation(void);    /* 相对基线偏差绝对值 0~4095 */
#define ALARM_LIGHT_DEV  1200            /* 偏差超约 30% 满量程即报警 */

uint16_t ADC_GetPotVoltageX100(void);    /* 电位器电压 x100，如 3.30V -> 330 */

#endif
