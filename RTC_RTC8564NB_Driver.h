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

#ifndef RTC_RTC8564NB_DRIVER_H
#define RTC_RTC8564NB_DRIVER_H

#include <stdint.h>

    //*****************************************************//
    // Analog to Digital Converter MCP3425制御用
    //*****************************************************//

    // ｽﾚ-ﾌﾞｱﾄﾞﾚｽ+R/Wを定義
    #define RTC8564NB_WRITE_ADDRESS_BYTE                 0xA2        // ｱﾄﾞﾚｽﾊﾞｲﾄ(RTC8564NBｱﾄﾞﾚｽ(0x51) + 書込みﾋﾞｯﾄ(0x00) = 0xA2)
    #define RTC8564NB_READ__ADDRESS_BYTE                 0xA3        // ｱﾄﾞﾚｽﾊﾞｲﾄ(RTC8564NBｱﾄﾞﾚｽ(0x51) + 読込みﾋﾞｯﾄ(0x01) = 0xA3)

    // RTC8564NBのｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ1で使用する時計･ｶﾚﾝﾀﾞ機能の停止ﾊﾟﾗﾒ-ﾀ
    #define RTC8564NB_STOP_PARAMETER_BYTE               0x20        // 機能停止
    #define RTC8564NB_NONSTOP_PARAMETER_BYTE            0x00        // 機能開始

    // RTC8564NBのｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ2で使用する割込み･ｲﾍﾞﾝﾄ機能のﾊﾟﾗﾒ-ﾀ
    #define RTC8564NB_INTERRUPT_EVENT_PARAMETER_BYTE    0x00        // 機能は使用しない

    
    void RTC8564_Get_Initialize_Data_Table_Size(uint8_t *pInitialize_Command_Size);
  
    void RTC8564_Get_Initialize_Interrupt_Command_Table_Size(uint8_t *pInitialize_Interrupt_Command_Size);
    
    void RTC8564_Get_Time_Data_Table_Size(uint8_t *pTime_Data_Table_Size);
    
    void MSSP_RTC8564NB_Initialize(uint8_t deviceAddress, uint8_t registerAddress, uint8_t commandData);
        
    void MSSP_RTC8564NB_GetTimeDataProc(uint8_t ReadRegister, uint8_t *GetDataArray);
    
    uint8_t RTC8564NB_Bcd_to_Dec_to_Str_Conversion(uint8_t TempGarbageNumber, uint8_t *TempBcdData, RTC_RTC8564NB_DataStruct *t);


#endif