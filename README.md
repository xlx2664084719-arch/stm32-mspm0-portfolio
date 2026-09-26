# 嵌入式作品集 · STM32 + MSPM0

独立完成的嵌入式开发作品集，覆盖 ST 与 TI 两家 MCU 平台。

## 项目列表

| 项目 | 平台 | 简介 |
|---|---|---|
| [EnvMonitor 环境监测终端](./envmonitor) | STM32F103C8T6 | 多传感器综合终端：BSP 分层 + 时间片调度，I2C/SPI/ADC+DMA/Flash 环形日志全栈 |
| [数字钟](./digital_clock) | TI MSPM0G3519 | OLED + 4x4 键盘调时 + EEPROM 断电记忆 + 闹钟音乐播放 |
| [贪吃蛇](./snake_game) | TI MSPM0G3519 | OLED 游戏 + 矩阵键盘操控 + 最高分掉电保存 |
| [简易波形检测装置](./简易波形检测装置) | TI MSPM0G3519 | ADC 采集 + DAC 回放 + OLED 波形显示（软件马拉松限时项目） |

## 技术栈总览

- **通信**：软件 I2C（多从机总线复用）、硬件 SPI、UART（printf 重定向）
- **模拟**：ADC 连续扫描 + DMA 循环搬运、DAC 波形重建、滑动平均滤波
- **存储**：SPI Flash 环形日志（页对齐/掉电恢复）、片内 Flash EEPROM 仿真
- **架构**：BSP 板级支持包分层、时间片轮询调度、状态机消抖、器件自检降级
- **工具链**：Keil MDK（Arm Compiler 6）、TI SysConfig、ST-Link / XDS110

## 环境

- STM32 项目：Keil MDK 5.43 + STM32F1xx DFP，打开 `envmonitor/Project/*.uvprojx` 直接编译
- MSPM0 项目：Keil MDK + [TI MSPM0 SDK 2.08.00.03](https://www.ti.com/tool/MSPM0-SDK)（默认安装于 `C:\ti\mspm0_sdk_2_08_00_03`）
