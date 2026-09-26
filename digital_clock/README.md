# 数字钟（MSPM0G3519）

基于 OLED + 4x4 矩阵键盘的多功能数字钟。

## 功能

- OLED 实时显示时：分：秒
- 键盘调时/设置闹钟
- 闹钟触发时通过板载蜂鸣器播放音乐（`BSP/MusicPlayer.c` + 曲谱 `MusicScore.h`，Timer PWM 驱动）
- 时间数据写入片内 Flash（EEPROM 仿真），断电重启不丢失

## 技术要点

- TIM 定时器 1s 计时节拍 + SysConfig 生成外设初始化
- Flash 扇区擦写管理（`eeprom_emulation_type_a`）
- PWM 音调合成

## 构建

Keil 打开 `Project/digital_clock.uvprojx`（需 TI MSPM0 SDK 2.08.00.03）。
