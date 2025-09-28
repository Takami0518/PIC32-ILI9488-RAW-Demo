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

#ifndef CLCD_AQM0802_DRIVER_H
#define CLCD_AQM0802_DRIVER_H

    #define LCD_addr 0x7C 

    void MSSP_Lcd_Initialize(void);
    
    void MSSP_Lcd_SendCommandDataProc(unsigned char cmd);
    
    void MSSP_Lcd_SendCharacterDataProc(unsigned char dat);
    
    void MSSP_Lcd_String_Send(char *str);
    
    void MSSP_Lcd_Clear(void);

    void MSSP_Lcd_Home(void);
    
    void MSSP_LCD_2line(void);
    
#endif