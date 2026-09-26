# EnvMonitor —— 多传感器环境监测终端（STM32F103C8T6）

独立开发的裸机综合项目：BSP 分层架构 + 时间片轮询调度，覆盖 STM32 常用外设全栈。

## 功能

- **OLED 界面**（SSD1306，软件 I2C）：三页信息（环境数据 / 传感器原始值 / 存储状态），按键切页
- **姿态检测**：MPU6050 加速度解算俯仰/横滚角（与 OLED 共用同一条 I2C 总线，多从机复用）
- **模拟量采集**：光照（光敏模块）+ 电压（ADC1 双通道连续扫描 + DMA 循环搬运 + 8 点滑动平均滤波）
- **数据存储**：W25Q64 SPI Flash 环形日志——16 字节定长记录（页对齐防跨页）、配置页存魔数/计数/写指针、掉电恢复、写满回卷
- **报警**：光照低于阈值蜂鸣器断续鸣叫
- **串口**：USART1 1Hz 输出 CSV 数据流（VOFA+ 友好），printf 重定向
- **按键**：状态机消抖（连续 3 次 10ms 采样一致）+ 长按/短按事件识别

## 架构

```
App 层        main.c（时间片调度） storage_manager.c（环形日志）
服务层        SysTick 1ms 时基  DWT 延时  滑动平均滤波
BSP 层        bsp_led bsp_key bsp_buzzer bsp_oled bsp_mpu6050 bsp_w25q64 bsp_adc
驱动库        CMSIS + STM32F10x StdPeriph Library (SPL)
```

设计要点：

- **软件 I2C**：开漏输出 + 上拉实现标准时序，引脚灵活；处理了重复起始与末字节 NACK
- **SysTick/DWT 资源解耦**：SysTick 专用于 1ms 系统心跳，延时用内核 DWT 周期计数器，避免模块间抢占定时器资源
- **Flash 使用纪律**：先擦后写、页编程不跨 256B 边界（16B 记录 × 16 条/页）、环形回卷
- **器件自检降级**：MPU6050 读 WHO_AM_I、W25Q64 读 JEDEC ID，不在线时 UI 提示并继续运行

## 资源占用

ROM 17.1KB / RAM 2.1KB（芯片 64KB / 20KB），HEX 随仓库提供（`Project/Objects` 略，直接 Keil 编译）。

## 构建

- Keil MDK 5.43（Arm Compiler 6）+ Keil.STM32F1xx_DFP 器件包
- 打开 `Project/STM32F103_Template.uvprojx`，F7 编译，F8 烧录（ST-Link，SWD）

## 接线速查

| 模块 | 引脚 |
|---|---|
| OLED / MPU6050 | SCL→PB8，SDA→PB9（共总线） |
| W25Q64 | CS→PA4，SCK→PA5，MISO→PA6，MOSI→PA7 |
| 光敏 AO | PA1（ADC1_IN1） |
| 电位器 | PA2（ADC1_IN2） |
| 蜂鸣器 | PB15（低电平触发） |
| 按键 | PA0（上拉输入，接地触发） |
| 串口 | PA9=TX，115200-8-N-1 |
