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

#ifndef GLCD_ILI9431_DRIVER_H
#define GLCD_ILI9431_DRIVER_H

#include <stdint.h>

    #define LCD_WIDTH  320
    #define LCD_HEIGHT 240
    
    // ILI9341 16Bit基本色
    #define BLACK_COLOR     0x0000
    #define WHITE_COLOR     0xFFFF 
    #define RED_COLOR       0xF800
    #define GREEN_COLOR     0x07E0
    #define BLUE_COLOR      0x001F
    #define YELLOW_COLOR    0xFFE0
    #define CYAN_COLOR      0x07FF
    #define MAGENTA_COLOR   0xF81F
    #define GRAY_COLOR      0x8410

    #define LIGHT_SILVER_A      0xCE59
    #define LIGHT_SILVER_B      0xD69A
    #define LIGHT_SILVER_C      0xDEDB
    #define LIGHT_SILVER_D      0xE71C
    #define LIGHT_SILVER_E      0xDEFB
    #define VERY_LIGHT_SILVER   0xEF5D
    #define NEAR_WHITE_SILVER   0xF79E

    void MSSP_SPI1_ILI9341_Initialize(void);
  
    
    void MSSP_SPI1_ILI9341_WriteCommand_Proc(unsigned char data);
    void MSSP_SPI1_ILI9341_WriteData_Proc(unsigned char data);
    
    void MSSP_SPI1_ILI9341_Set_Address_Window_Proc(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void MSSP_SPI1_ILI9341_Back_Ground_Draw_Proc(void);
    void MSSP_SPI1_ILI9341_Rectangle_Button_Draw_Proc(void);

#endif