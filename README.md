# 嵌入式作品集 · MSPM0G3519（Cortex-M0+）

武汉大学 · 基于 TI MSPM0G3519 的三个独立完成的嵌入式小项目。

所有项目基于 TI MSPM0 SDK 2.08.00.03 + Keil MDK 开发，BSP 分层架构（BSP 硬件封装层 / User 应用层 / Project 工程配置），外设覆盖：GPIO、TIM（定时器/PWM）、UART、ADC、DAC、SPI（OLED）、4x4 矩阵键盘、Flash EEPROM 仿真。

## 项目列表

| 项目 | 文件夹 | 简介 |
|---|---|---|
| 数字钟 | [`digital_clock`](./digital_clock) | OLED 显示 + 4x4 键盘调时 + EEPROM 断电记忆 + 闹钟音乐播放 |
| 贪吃蛇 | [`snake_game`](./snake_game) | OLED 游戏画面 + 矩阵键盘操控 + 最高分掉电保存 |
| 简易波形检测装置 | [`简易波形检测装置`](./简易波形检测装置) | ADC 采集 + DAC 回放 + OLED 波形显示（软件马拉松项目） |

## 环境与构建

- 硬件：TI EVM_MSPM0G3519 开发板、SPI OLED（SSD1306）、4x4 矩阵键盘
- 软件：Keil MDK 5.38+（AC6）、TI MSPM0 SDK 2.08.00.03（默认安装于 `C:\ti\mspm0_sdk_2_08_00_03`）
- 构建：打开各项目 `Project/*.uvprojx`，Keil 内编译。工程通过 SDK 相对路径引用 DriverLib，需保持 SDK 安装位置
- 烧录：板载 XDS110 调试器

## 通用技术点

- **BSP 分层**：所有硬件操作封装在 `BSP/`（oled_spi 驱动、Keyboard 扫描、clock、MusicPlayer、eeprom_emulation），应用逻辑在 `User/main.c`，二者只通过头文件接口交互
- **EEPROM 仿真**：利用片内 Flash 模拟 EEPROM（`eeprom_emulation_type_a`），实现数字钟时间/贪吃蛇最高分的掉电保存
- **SysConfig 生成 + 手写驱动结合**：引脚配置用 TI SysConfig 生成（`ti_msp_dl_config.c`），业务驱动手写
