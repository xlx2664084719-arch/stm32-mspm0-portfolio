/**
  ******************************************************************************
  * @file    main.c
  * @brief   EnvMonitor 环境监测终端（一期）
  *
  * 架构：时间片轮询调度（毫秒时基）
  *   [BSP 层]  Hardware/: led key buzzer oled mpu6050 w25q64 adc delay uart
  *   [App 层]  User/: main + storage_manager
  *   任务表：  1ms  蜂鸣器关断
  *             10ms 按键扫描（状态机消抖）
  *             100ms 传感器采样 + 滤波 + 报警判定 + UI 更新(200ms)
  *             1000ms 串口 CSV 输出（VOFA+ 友好）
  *             10000ms 自动写日志；长按 PA0 手动存档
  *
  * 接线：OLED+MPU6050 共 I2C(PB8/PB9)；W25Q64 走 SPI1(PA4~7)；
  *       PA1 光敏 AO / PA2 电位器；PA0 按键；PB15 蜂鸣器；PC13 LED
  ******************************************************************************
  */
#include "stm32f10x.h"
#include <stdio.h>
#include "delay.h"
#include "uart.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_buzzer.h"
#include "bsp_oled.h"
#include "bsp_mpu6050.h"
#include "bsp_w25q64.h"
#include "bsp_adc.h"
#include "storage_manager.h"

volatile uint32_t g_sys_ms = 0;      /* 系统毫秒时基：SysTick 中断维护 */

#define ALARM_LIGHT_PCT   15         /* 光照低于 15% 报警 */
#define ALARM_TILT_DEG    30         /* 倾角超过 30 度报警 */

static uint8_t  ui_page = 0;
static uint8_t  flash_ok = 0;

/* ---------- UI：三页信息 ---------- */
static void ui_refresh(const mpu_data_t *m)
{
    OLED_Clear();
    if (ui_page == 0) {                            /* 主页 */
        OLED_ShowString(0, 0, "EnvMonitor v1.0");
        OLED_ShowString(0, 2, "Light:");           /* 亮度百分比 */
        OLED_ShowNum(7, 2, ADC_GetLight(), 3);
        OLED_ShowString(10, 2, "%");
        OLED_ShowString(0, 4, "Volt:");
        OLED_ShowNum(6, 4, ADC_GetPotVoltageX100() / 100, 1);
        OLED_ShowChar(7, 4, '.');
        OLED_ShowNum(8, 4, ADC_GetPotVoltageX100() % 100, 2);
        OLED_ShowString(10, 4, "V");
        OLED_ShowString(0, 6, "Tilt:");
        OLED_ShowNum(6, 6, m->pitch_x10 / 10, 3);
        OLED_ShowChar(9, 6, '.');
        OLED_ShowNum(10, 6, m->pitch_x10 % 10, 1);
        OLED_ShowString(12, 6, "deg");
    } else if (ui_page == 1) {                     /* 传感器原始值 */
        OLED_ShowString(0, 0, "RAW DATA");
        OLED_ShowString(0, 2, "ADC1:");
        OLED_ShowNum(6, 2, ADC_GetSmoothed(0), 4);
        OLED_ShowString(0, 3, "ADC2:");
        OLED_ShowNum(6, 3, ADC_GetSmoothed(1), 4);
        OLED_ShowString(0, 5, "AX:");
        OLED_ShowNum(4, 5, m->ax, 6);
        OLED_ShowString(0, 6, "AZ:");
        OLED_ShowNum(4, 6, m->az, 6);
    } else {                                       /* 存储状态 */
        OLED_ShowString(0, 0, "STORAGE(W25Q64)");
        OLED_ShowString(0, 2, "Records:");
        OLED_ShowNum(9, 2, (int32_t)Storage_Count(), 6);
        OLED_ShowString(0, 3, "LastSeq:");
        OLED_ShowNum(9, 3, Storage_LastSeq(), 5);
        OLED_ShowString(0, 6, flash_ok ? "hold KEY=save" : "FLASH OFFLINE");
    }
    OLED_Update();
}

int main(void)
{
    uint32_t t_key = 0, t_sense = 0, t_ui = 0, t_ser = 0, t_save = 0;
    uint8_t  mpu_ok = 0;
    mpu_data_t mpu;

    /* 先初始化 DWT 延时，再建立 SysTick 1ms 心跳（二者已解耦） */
    Delay_Init();
    SysTick_Config(SystemCoreClock / 1000);

    LED_Init();
    Key_Init();
    Buzzer_Init();
    UART1_Init(115200);
    ADC_DMA_Init();
    OLED_Init();
    mpu_ok = (MPU6050_Init() == 0);
    Storage_Init();
    flash_ok = (W25Q64_GetID() == 0xEF4017);

    printf("\r\n=== EnvMonitor v1.0 | %s | %s ===\r\n",
           mpu_ok ? "MPU6050 OK" : "MPU6050 N/A",
           flash_ok ? "W25Q64 OK" : "W25Q64 N/A");
    Buzzer_Beep(80);

    while (1) {
        uint32_t now = g_sys_ms;

        /* 1ms：蜂鸣器非阻塞关断 */
        Buzzer_Task1ms();

        /* 10ms：按键 */
        if (now - t_key >= 10) {
            t_key = now;
            Key_Task10ms();
            switch (Key_GetEvent()) {
            case KEY_EVENT_SHORT:
                ui_page = (ui_page + 1) % 3;
                t_ui = 0;                      /* 立即刷新 UI */
                break;
            case KEY_EVENT_LONG:
                if (flash_ok) {
                    Storage_Save(ADC_GetLight(), ADC_GetPotVoltageX100(),
                                 mpu.pitch_x10, mpu.roll_x10);
                    printf("SAVE ok, total=%d\r\n", (int)Storage_Count());
                    Buzzer_Beep(150);
                    t_ui = 0;
                }
                break;
            default: break;
            }
        }

        /* 100ms：采样滤波 + 报警判定 */
        if (now - t_sense >= 100) {
            t_sense = now;
            ADC_FilterTick();
            if (mpu_ok) MPU6050_Read(&mpu);

            {
                /* 报警仅看光照绝对阈值；
                 * 倾角报警默认关闭——MPU 安装方向不固定易误报，数据仅显示 */
                uint8_t alarm = (ADC_GetLight() < ALARM_LIGHT_PCT);
                if (alarm) Buzzer_Set((g_sys_ms / 200) & 1);  /* 断续鸣叫 */
                else if (g_sys_ms >= 1000)  Buzzer_Set(0);    /* 避开开机提示音 */
            }
        }

        /* 200ms：UI 刷新 */
        if (now - t_ui >= 200) {
            t_ui = now;
            ui_refresh(&mpu);
        }

        /* 1000ms：串口数据流（VOFA+ / 串口助手） */
        if (now - t_ser >= 1000) {
            t_ser = now;
            LED_Toggle();                      /* 心跳灯 */
            printf("L:%d,P:%d,T:%d.%d,R:%d.%d,N:%d\r\n",
                   ADC_GetLight(),
                   ADC_GetPotVoltageX100(),
                   mpu.pitch_x10 / 10, mpu.pitch_x10 % 10,
                   mpu.roll_x10 / 10, mpu.roll_x10 % 10,
                   (int)Storage_Count());
        }

        /* 10s：自动记录一条环境快照 */
        if (flash_ok && now - t_save >= 10000) {
            t_save = now;
            Storage_Save(ADC_GetLight(), ADC_GetPotVoltageX100(),
                         mpu.pitch_x10, mpu.roll_x10);
        }
    }
}
