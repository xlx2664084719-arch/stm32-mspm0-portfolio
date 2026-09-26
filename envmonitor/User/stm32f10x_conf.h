/**
  ******************************************************************************
  * @file    stm32f10x_conf.h
  * @brief   标准外设库配置文件 —— 决定哪些外设头文件被包含
  *          （需要用哪个外设，就取消对应 #include 的注释；
  *           只有被包含的外设头文件对应的 assert_param 检查才会生效）
  ******************************************************************************
  */

#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* 取消注释以包含对应外设头文件（建议：用到哪个开哪个，可加快编译） */
#include "stm32f10x_adc.h"
/* #include "stm32f10x_bkp.h"   */
/* #include "stm32f10x_can.h"   */
/* #include "stm32f10x_cec.h"   */
/* #include "stm32f10x_crc.h"   */
/* #include "stm32f10x_dac.h"   */
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
/* #include "stm32f10x_flash.h" */
/* #include "stm32f10x_fsmc.h"  */
#include "stm32f10x_gpio.h"
/* #include "stm32f10x_i2c.h"   */
/* #include "stm32f10x_iwdg.h"  */
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
/* #include "stm32f10x_rtc.h"   */
/* #include "stm32f10x_sdio.h"  */
#include "stm32f10x_spi.h"
/* #include "stm32f10x_tim.h"   */
#include "stm32f10x_usart.h"
/* #include "stm32f10x_wwdg.h"  */
#include "misc.h"  /* NVIC 配置 */

/* 导出类型/常量（保持默认即可） */

/* 断言检查：调试时可打开 USE_FULL_ASSERT 宏（在 Keil 选项 Define 中添加），
 * 配合下面的 assert_failed 函数定位参数错误；发布时注释掉以减小体积 */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line);
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t*)__FILE__, __LINE__))
#else
#define assert_param(expr) ((void)0)
#endif

#endif /* __STM32F10x_CONF_H */
