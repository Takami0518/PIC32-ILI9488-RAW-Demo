//==============================================================================
// File Name            : CoreMain.c
// Creator              : 高見 謙介
// Created Date         : 2025年8月6日
// CPU                  : PIC32MX370F512H
// Compiler             : MPLAB XC32 v4.45
// Overview             : 熱式風速計(I2C)+温度計測器(I2C)=UART通信にてPC(VC#)にて受信
// Operating Frequency  : 80Mhz
// 
// ﾋﾟﾝ設定     :
//    RA0 : 温度ｾﾝｻｰ入力
//    RA1 : ｽﾃｰﾀｽLED出力
//    RC0 : UART TX(9600bps)
// 履歴         :
//    [2025/07/18] 初版作成
//    [2025/07/20] 動作ｸﾛｯｸ設計(CPUの最大動作周波数は50MHz(32MIPS)で構築)
//==============================================================================

#ifndef SYSTEM_CALCULATIONS_H
#define SYSTEM_CALCULATIONS_H

    // ADC仕様
    #define ADC_BITS    12              // 12bit ADC
    #define ADC_MAX     ((1 << ADC_BITS) - 1)
    #define VREF        3.3f            // ADC基準電圧

    // センサ特性係数（グラフの式）
    #define COEFF_A     0.5335f         // a
    #define COEFF_B     4.0383f         // b

    // 無風オフセット（必要に応じて調整）
    #define V_OFFSET    0.5335f         // 無風時電圧[V]

    float adc_to_voltage(uint16_t adc_value);
    float voltage_to_windspeed(float voltage);

#endif