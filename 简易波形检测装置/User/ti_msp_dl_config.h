/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                         TIMA1
#define PWM_0_INST_IRQHandler                                   TIMA1_IRQHandler
#define PWM_0_INST_INT_IRQN                                     (TIMA1_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                              4000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_0_C0_PORT                                                 GPIOB
#define GPIO_PWM_0_C0_PIN                                         DL_GPIO_PIN_26
#define GPIO_PWM_0_C0_IOMUX                                      (IOMUX_PINCM57)
#define GPIO_PWM_0_C0_IOMUX_FUNC                     IOMUX_PINCM57_PF_TIMA1_CCP0
#define GPIO_PWM_0_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_0_C1_PORT                                                 GPIOA
#define GPIO_PWM_0_C1_PIN                                         DL_GPIO_PIN_11
#define GPIO_PWM_0_C1_IOMUX                                      (IOMUX_PINCM22)
#define GPIO_PWM_0_C1_IOMUX_FUNC                     IOMUX_PINCM22_PF_TIMA1_CCP1
#define GPIO_PWM_0_C1_IDX                                    DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMA0)
#define TIMER_0_INST_IRQHandler                                 TIMA0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                         (19999U)



/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI0
#define SPI_0_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                       DL_GPIO_PIN_2
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM15)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM15_PF_SPI0_PICO
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                       DL_GPIO_PIN_3
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM16)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM16_PF_SPI0_SCLK



/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC0
#define ADC12_0_INST_IRQHandler                                  ADC0_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA
#define GPIO_ADC12_0_C0_PORT                                               GPIOA
#define GPIO_ADC12_0_C0_PIN                                       DL_GPIO_PIN_27
#define GPIO_ADC12_0_IOMUX_C0                                    (IOMUX_PINCM60)
#define GPIO_ADC12_0_IOMUX_C0_FUNC                (IOMUX_PINCM60_PF_UNCONNECTED)



/* Defines for DMA_CH2 */
#define DMA_CH2_CHAN_ID                                                      (1)
#define DAC12_INST_DMA_TRIGGER                          (DMA_DAC0_EVT_BD_1_TRIG)
/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define ADC12_0_INST_DMA_TRIGGER                      (DMA_ADC0_EVT_GEN_BD_TRIG)


/* Port definition for Pin Group key */
#define key_PORT                                                         (GPIOB)

/* Defines for user: GPIOB.31 with pinCMx 68 on package pin 27 */
// pins affected by this interrupt request:["user"]
#define key_INT_IRQN                                            (GPIOB_INT_IRQn)
#define key_INT_IIDX                            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define key_user_IIDX                                       (DL_GPIO_IIDX_DIO31)
#define key_user_PIN                                            (DL_GPIO_PIN_31)
#define key_user_IOMUX                                           (IOMUX_PINCM68)
/* Port definition for Pin Group IMU */
#define IMU_PORT                                                         (GPIOB)

/* Defines for CS_IMU: GPIOB.12 with pinCMx 29 on package pin 36 */
#define IMU_CS_IMU_PIN                                          (DL_GPIO_PIN_12)
#define IMU_CS_IMU_IOMUX                                         (IOMUX_PINCM29)
/* Port definition for Pin Group W25Q64 */
#define W25Q64_PORT                                                      (GPIOB)

/* Defines for CS_W25Q64: GPIOB.13 with pinCMx 30 on package pin 37 */
#define W25Q64_CS_W25Q64_PIN                                    (DL_GPIO_PIN_13)
#define W25Q64_CS_W25Q64_IOMUX                                   (IOMUX_PINCM30)
/* Defines for L1: GPIOA.14 with pinCMx 36 on package pin 43 */
#define LED_L1_PORT                                                      (GPIOA)
#define LED_L1_PIN                                              (DL_GPIO_PIN_14)
#define LED_L1_IOMUX                                             (IOMUX_PINCM36)
/* Defines for L2: GPIOA.17 with pinCMx 39 on package pin 54 */
#define LED_L2_PORT                                                      (GPIOA)
#define LED_L2_PIN                                              (DL_GPIO_PIN_17)
#define LED_L2_IOMUX                                             (IOMUX_PINCM39)
/* Defines for L4: GPIOB.28 with pinCMx 65 on package pin 24 */
#define LED_L4_PORT                                                      (GPIOB)
#define LED_L4_PIN                                              (DL_GPIO_PIN_28)
#define LED_L4_IOMUX                                             (IOMUX_PINCM65)
/* Defines for L5: GPIOB.29 with pinCMx 66 on package pin 25 */
#define LED_L5_PORT                                                      (GPIOB)
#define LED_L5_PIN                                              (DL_GPIO_PIN_29)
#define LED_L5_IOMUX                                             (IOMUX_PINCM66)
/* Defines for L6: GPIOB.30 with pinCMx 67 on package pin 26 */
#define LED_L6_PORT                                                      (GPIOB)
#define LED_L6_PIN                                              (DL_GPIO_PIN_30)
#define LED_L6_IOMUX                                             (IOMUX_PINCM67)
/* Defines for L7: GPIOC.6 with pinCMx 84 on package pin 63 */
#define LED_L7_PORT                                                      (GPIOC)
#define LED_L7_PIN                                               (DL_GPIO_PIN_6)
#define LED_L7_IOMUX                                             (IOMUX_PINCM84)
/* Defines for L8: GPIOC.7 with pinCMx 85 on package pin 64 */
#define LED_L8_PORT                                                      (GPIOC)
#define LED_L8_PIN                                               (DL_GPIO_PIN_7)
#define LED_L8_IOMUX                                             (IOMUX_PINCM85)
/* Port definition for Pin Group keyboard */
#define keyboard_PORT                                                    (GPIOB)

/* Defines for H1: GPIOB.6 with pinCMx 23 on package pin 30 */
#define keyboard_H1_PIN                                          (DL_GPIO_PIN_6)
#define keyboard_H1_IOMUX                                        (IOMUX_PINCM23)
/* Defines for H2: GPIOB.7 with pinCMx 24 on package pin 31 */
#define keyboard_H2_PIN                                          (DL_GPIO_PIN_7)
#define keyboard_H2_IOMUX                                        (IOMUX_PINCM24)
/* Defines for H3: GPIOB.8 with pinCMx 25 on package pin 32 */
#define keyboard_H3_PIN                                          (DL_GPIO_PIN_8)
#define keyboard_H3_IOMUX                                        (IOMUX_PINCM25)
/* Defines for H4: GPIOB.9 with pinCMx 26 on package pin 33 */
#define keyboard_H4_PIN                                          (DL_GPIO_PIN_9)
#define keyboard_H4_IOMUX                                        (IOMUX_PINCM26)
/* Defines for V1: GPIOB.20 with pinCMx 48 on package pin 67 */
#define keyboard_V1_PIN                                         (DL_GPIO_PIN_20)
#define keyboard_V1_IOMUX                                        (IOMUX_PINCM48)
/* Defines for V2: GPIOB.24 with pinCMx 52 on package pin 71 */
#define keyboard_V2_PIN                                         (DL_GPIO_PIN_24)
#define keyboard_V2_IOMUX                                        (IOMUX_PINCM52)
/* Defines for V3: GPIOB.25 with pinCMx 56 on package pin 75 */
#define keyboard_V3_PIN                                         (DL_GPIO_PIN_25)
#define keyboard_V3_IOMUX                                        (IOMUX_PINCM56)
/* Defines for V4: GPIOB.27 with pinCMx 58 on package pin 77 */
#define keyboard_V4_PIN                                         (DL_GPIO_PIN_27)
#define keyboard_V4_IOMUX                                        (IOMUX_PINCM58)
/* Defines for RES: GPIOB.23 with pinCMx 51 on package pin 70 */
#define OLED_RES_PORT                                                    (GPIOB)
#define OLED_RES_PIN                                            (DL_GPIO_PIN_23)
#define OLED_RES_IOMUX                                           (IOMUX_PINCM51)
/* Defines for DC: GPIOC.8 with pinCMx 86 on package pin 65 */
#define OLED_DC_PORT                                                     (GPIOC)
#define OLED_DC_PIN                                              (DL_GPIO_PIN_8)
#define OLED_DC_IOMUX                                            (IOMUX_PINCM86)
/* Defines for CS: GPIOC.9 with pinCMx 87 on package pin 66 */
#define OLED_CS_PORT                                                     (GPIOC)
#define OLED_CS_PIN                                              (DL_GPIO_PIN_9)
#define OLED_CS_IOMUX                                            (IOMUX_PINCM87)
/* Defines for sw1: GPIOA.7 with pinCMx 14 on package pin 17 */
#define switch_sw1_PORT                                                  (GPIOA)
#define switch_sw1_PIN                                           (DL_GPIO_PIN_7)
#define switch_sw1_IOMUX                                         (IOMUX_PINCM14)
/* Defines for sw2: GPIOA.8 with pinCMx 19 on package pin 22 */
#define switch_sw2_PORT                                                  (GPIOA)
#define switch_sw2_PIN                                           (DL_GPIO_PIN_8)
#define switch_sw2_IOMUX                                         (IOMUX_PINCM19)
/* Defines for sw3: GPIOA.9 with pinCMx 20 on package pin 23 */
#define switch_sw3_PORT                                                  (GPIOA)
#define switch_sw3_PIN                                           (DL_GPIO_PIN_9)
#define switch_sw3_IOMUX                                         (IOMUX_PINCM20)
/* Defines for sw4: GPIOA.12 with pinCMx 34 on package pin 41 */
#define switch_sw4_PORT                                                  (GPIOA)
#define switch_sw4_PIN                                          (DL_GPIO_PIN_12)
#define switch_sw4_IOMUX                                         (IOMUX_PINCM34)
/* Defines for sw5: GPIOA.13 with pinCMx 35 on package pin 42 */
#define switch_sw5_PORT                                                  (GPIOA)
#define switch_sw5_PIN                                          (DL_GPIO_PIN_13)
#define switch_sw5_IOMUX                                         (IOMUX_PINCM35)
/* Defines for sw6: GPIOA.16 with pinCMx 38 on package pin 45 */
#define switch_sw6_PORT                                                  (GPIOA)
#define switch_sw6_PIN                                          (DL_GPIO_PIN_16)
#define switch_sw6_IOMUX                                         (IOMUX_PINCM38)
/* Defines for sw7: GPIOA.22 with pinCMx 47 on package pin 62 */
#define switch_sw7_PORT                                                  (GPIOA)
#define switch_sw7_PIN                                          (DL_GPIO_PIN_22)
#define switch_sw7_IOMUX                                         (IOMUX_PINCM47)
/* Defines for sw8: GPIOB.22 with pinCMx 50 on package pin 69 */
#define switch_sw8_PORT                                                  (GPIOB)
#define switch_sw8_PIN                                          (DL_GPIO_PIN_22)
#define switch_sw8_IOMUX                                         (IOMUX_PINCM50)



/* Defines for DAC12 */
#define DAC12_IRQHandler                                         DAC0_IRQHandler
#define DAC12_INT_IRQN                                           (DAC0_INT_IRQn)
#define GPIO_DAC12_OUT_PORT                                                GPIOA
#define GPIO_DAC12_OUT_PIN                                        DL_GPIO_PIN_15
#define GPIO_DAC12_IOMUX_OUT                                     (IOMUX_PINCM37)
#define GPIO_DAC12_IOMUX_OUT_FUNC                   IOMUX_PINCM37_PF_UNCONNECTED


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_ADC12_0_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_DAC12_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
