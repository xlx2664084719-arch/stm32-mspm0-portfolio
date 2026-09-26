/*------------------------------------------------------------------------------
 * File Name: Wave_Debug_Version
 * Description: ����ʶ����԰棨��ѵ�7�����ָ����ң�
 *----------------------------------------------------------------------------*/
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oled_spi_V0.2.h"
#include <stdio.h>
#include <math.h>

#define SAMPLE_COUNT   1024
#define ADC_REF        3.3f
#define ADC_MAX        4095.0f
#define SAMPLE_RATE    53545.0f

float vpp = 0, freq = 0, vdc = 0, vrms = 0, duty = 0;
char wave_type[16] = "UNKNOWN";
int cross_history[4] = {0};
uint8_t history_idx = 0;
static uint16_t adc_buffer[SAMPLE_COUNT];

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_Clear();

    OLED_ShowString(0, 0, "Advanced Version");
    delay_cycles(CPUCLK_FREQ * 1.5);
		OLED_Clear();
    DL_ADC12_startConversion(ADC12_0_INST);

    while (1) {
        uint16_t maxv = 0, minv = 0xFFFF;
        uint32_t sum = 0, sumsq = 0;
        int cross = 0;
        uint32_t high_time = 0;
        uint32_t flat_count = 0;
        uint16_t last = 0;
        uint16_t period_start = 0, period_end = 0;
        int period_count = 0;

        for (int i = 0; i < SAMPLE_COUNT; i++) {
            DL_ADC12_startConversion(ADC12_0_INST);
            delay_cycles(1800);
            uint16_t val = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
            adc_buffer[i] = val;

            if (val > maxv) maxv = val;
            if (val < minv) minv = val;
            sum += val;
            sumsq += (uint32_t)val * val;

            if (maxv > minv && (val > maxv - (maxv-minv)*0.12 || val < minv + (maxv-minv)*0.12)) flat_count++;

            last = val;
        }
        
        uint16_t vdc_raw = sum / SAMPLE_COUNT;
        high_time = 0;
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            if (adc_buffer[i] > vdc_raw) high_time++;
        }
        
        cross = 0;
        period_count = 0;
        last = adc_buffer[0];
        
        for (int i = 1; i < SAMPLE_COUNT; i++) {
            uint16_t val = adc_buffer[i];
            if (last < vdc_raw && val >= vdc_raw) {
                cross++;
                if (period_count == 0) {
                    period_start = i;
                } else if (period_count == 1) {
                    period_end = i;
                }
                period_count++;
            }
            last = val;
        }

        vpp  = (maxv - minv) * ADC_REF / ADC_MAX;
        vdc  = (sum / SAMPLE_COUNT) * ADC_REF / ADC_MAX;
        vrms = sqrtf((float)sumsq / SAMPLE_COUNT) * ADC_REF / ADC_MAX;
        duty = (float)high_time / SAMPLE_COUNT * 100.0f;
        
        cross_history[history_idx] = cross;
        history_idx = (history_idx + 1) % 4;
        int cross_avg = (cross_history[0] + cross_history[1] + cross_history[2] + cross_history[3]) / 4;
        
        freq = cross_avg * SAMPLE_RATE / SAMPLE_COUNT / 2.0f;
        
        float rms_ratio = vrms / (vpp / 2.0f);
        float flat_ratio = (float)flat_count / SAMPLE_COUNT;
        
        if (flat_ratio > 0.20f && vrms < 1.3f && duty > 30.0f && duty < 70.0f) {
            freq = freq * 1.064f;
        }

        if (vpp < 0.1f) {
            strcpy(wave_type, "NO SIGNAL");
        } else if (freq < 300.0f && vpp >= 0.3f) {
            if (flat_ratio > 0.70f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio > 0.40f) {
                strcpy(wave_type, "SINE");
            } else {
                strcpy(wave_type, "TRIANGLE");
            }
        } else if (vpp < 0.2f) {
            if (flat_ratio > 0.50f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio >= 0.27f) {
                strcpy(wave_type, "SINE");
            } else {
                strcpy(wave_type, "TRIANGLE");
            }
        } else if (vpp < 0.3f) {
            if (flat_ratio > 0.55f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio >= 0.29f) {
                strcpy(wave_type, "SINE");
            } else {
                strcpy(wave_type, "TRIANGLE");
            }
        } else if (vpp < 0.5f) {
            if (flat_ratio > 0.70f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio >= 0.40f) {
                strcpy(wave_type, "SINE");
            } else {
                strcpy(wave_type, "TRIANGLE");
            }
        } else if (vpp < 1.2f) {
            if (flat_ratio > 0.60f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio > 0.38f || (flat_ratio > 0.32f && rms_ratio > 0.65f)) {
                strcpy(wave_type, "SINE");
            } else if (flat_ratio < 0.35f || rms_ratio < 0.62f) {
                strcpy(wave_type, "TRIANGLE");
            } else {
                strcpy(wave_type, "SINE");
            }
        } else {
            if (flat_ratio > 0.20f && vrms < 1.3f) {
                strcpy(wave_type, "SQUARE");
            } else if (flat_ratio > 0.35f || (flat_ratio > 0.35f && rms_ratio > 0.65f)) {
                strcpy(wave_type, "SINE");
            } else {
                strcpy(wave_type, "TRIANGLE");
            }
        }

        char buf[32];
        sprintf(buf, "Vpp:%.3fV ", vpp);       OLED_ShowString(0, 0, buf);
        sprintf(buf, "Freq:%.1fHz ", freq);    OLED_ShowString(0, 1, buf);
        OLED_ShowString(0, 2, "                ");
        sprintf(buf, "Wave:%s ", wave_type);   OLED_ShowString(0, 2, buf);
        
        if (strcmp(wave_type, "SQUARE") == 0) {
            sprintf(buf, "Duty:%.1f%% ", duty);
            OLED_ShowString(0, 3, buf);
        } else {
            OLED_ShowString(0, 3, "            ");
        }
        
        sprintf(buf, "Vdc:%.3fV ", vdc);       OLED_ShowString(0, 4, buf);
        sprintf(buf, "Vrms:%.3fV ", vrms);     OLED_ShowString(0, 5, buf);
        sprintf(buf, "R:%.2f F:%.2f", rms_ratio, flat_ratio); OLED_ShowString(0, 7, buf);

        delay_cycles(CPUCLK_FREQ / 5);
    }
}