#include <stdio.h>
#include <math.h>
#include "System_Calculations.h"

// ADC値から電圧へ変換
float adc_to_voltage(uint16_t adc_value)
{
    return ((float)adc_value * VREF) / (float)ADC_MAX;
}

// 電圧から風速[m/s]へ変換
float voltage_to_windspeed(float voltage)
{
    float v_corr = voltage - V_OFFSET;

    if (v_corr <= 0.0f) return 0.0f; // 無風以下は0

    return COEFF_A * powf(v_corr, COEFF_B);
}