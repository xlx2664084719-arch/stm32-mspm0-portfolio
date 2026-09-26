#ifndef TIMER_H
#define TIMER_H

#include "ti_msp_dl_config.h"
#include "bsp.h"
#include <math.h> 
#include "eeprom_emulation_type_a.h"
#include <string.h>
#include <stdbool.h>

extern bool timer_running; 
extern uint8_t timer_hour;
extern uint8_t timer_min; 
extern uint8_t timer_sec;

void Timer_Init(void);  
void Timer_Start(uint8_t h, uint8_t m, uint8_t s);  
void Timer_Stop(void); 
void Timer_Tick(void);  

#endif