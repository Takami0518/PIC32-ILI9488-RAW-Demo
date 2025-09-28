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

#ifndef SYSTEM_STRUCTS_H
#define SYSTEM_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

    //*****************************************************//
    // 構造体
    //*****************************************************//

    // 風速･温度ﾃﾞ-ﾀ格納用構造体を宣言(型定義なのでﾒﾓﾘ確保しない)
    typedef struct
    {
        // MCP3425のAD値上位8ﾋﾞｯﾄ取得用
        uint8_t u8_upper_binary_data;

        // MCP3425のAD値下位8ﾋﾞｯﾄ取得用
        uint8_t u8_lower_binary_data;

        // AD値上位8ﾋﾞｯﾄ+下位8ﾋﾞｯﾄのﾏ-ｼﾞ用
        uint16_t u16_adc_binary_data;

        // 整数AD値 ⇒ 電圧ﾃﾞ-ﾀ(実数)ﾊﾞｯﾌｧ
        float f_voltage_data;

        // 電圧ﾃﾞ-ﾀ(実数) ⇒ 電圧ﾃﾞ-ﾀ文字列ﾊﾞｯﾌｧ
        char c_voltage_str_data[7];

        // 整数AD値　⇒　ADC文字列ﾊﾞｯﾌｧ
        char c_adc_binary_str_data[7];

        // 電圧ﾃﾞ-ﾀ(実数) ⇒ 風速ﾃﾞ-ﾀ文字列ﾊﾞｯﾌｧ
        char c_result_windspeed_str_data[7];

    }ADC_MCP3425_DataStruct;

    // 時刻ﾃﾞ-ﾀ格納用構造体を宣言(型定義なのでﾒﾓﾘ確保しない)
    typedef struct
    {
        // 初期化ﾙ-ﾌﾟ用のｶｳﾝﾀ
        uint8_t Init_Counter;

        // 初期化ｺﾏﾝﾄﾞ数保持用
        uint8_t Initialize_Command_Data_Size;

        // 初期化ｺﾏﾝﾄﾞ格納用(constで読み取り専用)
        const uint8_t RTC8564_Initialize_Data_Table[17][2];


        // 割込み機能初期化ﾙ-ﾌﾟ用のｶｳﾝﾀ
        uint8_t Init_Int_Counter;

        // 割込みｺﾏﾝﾄﾞ数保持用
        uint8_t Initialize_Interrupt_Command_Data_Size;

        // 割込みｺﾏﾝﾄﾞ格納用(constで読み取り専用)
        const uint8_t RTC8564_Initialize_Interrupt_Data_Table[5][2];

        // ﾃﾞ-ﾀ読み取りﾙ-ﾌﾟ用ｶｳﾝﾀ
        uint8_t Read_Counter;

        // RTC8564NB時刻ﾃﾞ-ﾀ取得用のﾊﾞｯﾌｧ要素数保持用
        uint8_t Time_Data_Buffer_Size;

        // 時刻ﾃﾞ-ﾀ格納用ﾊﾞｯﾌｧ
        uint8_t RTC8564_Time_Data_Table[7][2];
        
        // msecｶｳﾝﾄ用(volatile:ｺﾝﾊﾟｲﾗによる最適化禁止)
        volatile uint32_t RTC8564_Mseconds_Counter;  

        // 最終時刻ﾃﾞ-ﾀ格納用ﾊﾞｯﾌｧ
        char RTC8564_Result_Time_Data_Year[3];

        char RTC8564_Result_Time_Data_Month[3];

        char RTC8564_Result_Time_Data_Day[3];

        char RTC8564_Result_Time_Data_Hour[3];

        char RTC8564_Result_Time_Data_Minutes[3];

        char RTC8564_Result_Time_Data_Seconds[3];

        char RTC8564_Result_Time_Data_Mseconds[3];


    }RTC_RTC8564NB_DataStruct;

    // ﾀｯﾁﾊﾟﾈﾙ座標取得用構造体を宣言(型定義なのでﾒﾓﾘ確保しない)
    typedef struct
    {
        // ﾀｯﾁﾊﾟﾈﾙ押下判定
        bool f_valid_press;
        
        // X軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
        uint16_t u16_x_axis_datas[3];

        // X軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
        uint16_t u16_median_x_axis_data;
        
        // Y軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
        uint16_t u16_y_axis_datas[3];

        // Y軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
        uint16_t u16_median_y_axis_data;
        
        // Z1軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
        uint16_t u16_z1_axis_datas[3];

        // Z1軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
        uint16_t u16_median_z1_axis_data;
 
        // Z2軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
        uint16_t u16_z2_axis_datas[3];

        // Z2軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
        uint16_t u16_median_z2_axis_data;
        
        // Z1軸中央値-Z2軸中央値からのﾀｯﾁﾊﾟﾈﾙ押下圧力値
        uint16_t u16_z_azis_pressure_data;
 
    }TPM_XPT2046_DataStruct;
    
    // extern:この変数や配列は別のﾌｧｲﾙで定義されているので参照のみ=ｸﾞﾛ-ﾊﾞﾙ変数として定義してるよ！って意味
    // externすることで変数・構造体はｸﾞﾛ-ﾊﾞﾙ変数でしか定義出来なくなるので注意(関数内のﾛ-ｶﾙ変数でｲﾝｽﾀﾝｽすると別物の変数となりｴﾗ-になる)

    extern ADC_MCP3425_DataStruct StructAdcMcp3425DatasCH1;

    extern ADC_MCP3425_DataStruct StructAdcMcp3425DatasCH2;

    extern RTC_RTC8564NB_DataStruct StructRtcRtc8564nbDatas;

    extern TPM_XPT2046_DataStruct SrructTpmXpt2046Datas;
    
#endif 