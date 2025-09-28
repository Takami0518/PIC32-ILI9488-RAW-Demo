//==============================================================================
// File Name            : GLCD_ILI9488_Driver.h
// Creator              : 高見 謙介
// Created Date         : 2025年8月6日
// CPU                  : PIC32MX370F512H
// Compiler             : MPLAB XC32 v4.45
// Overview             : ILI9488 GLCDﾄﾞﾗｲﾊﾞ(制御/描画関数ﾌﾟﾛﾄﾀｲﾌﾟ宣言)
// Operating Frequency  : 80MHz
// History              :
//    [2025/07/18] 初版作成
//    [2025/07/20] DMA対応描画関数を追加
//==============================================================================

//==============================================================================
// ｲﾝｸﾙ-ﾄﾞｶﾞ-ﾄﾞ: 多重ｲﾝｸﾙ-ﾄﾞ防止用ﾏｸﾛ
//==============================================================================
#ifndef GLCD_ILI9488_DRIVER_H
#define GLCD_ILI9488_DRIVER_H

//==============================================================================
// ILI9488 基本制御関数ﾌﾟﾛﾄﾀｲﾌﾟ宣言
//==============================================================================

// ILI9488ﾊ-ﾄﾞｳｪｱﾘｾｯﾄ(LOW=ﾘｾｯﾄ / HIGH=起動)
void ILI9488_Reset(void);

// ILI9488へｺﾏﾝﾄﾞを送信(受信値は無視)
void MSSP_SPI1_ILI9488_SendCommand(unsigned char data);

// ILI9488へﾃﾞ-ﾀを送信(受信値は無視)
void MSSP_SPI1_ILI9488_SendData(uint8_t data);

// ILI9488へﾃﾞ-ﾀを送受信する
void MSSP_SPI1_ILI9488_TransferData(uint8_t data);

// SPI1(MSSP)を使用してILI9488を初期化
void MSSP_SPI1_ILI9488_Initialize(void);

// 描画ｳｨﾝﾄﾞｳを設定(座標x,y 幅w 高さh)
void MSSP_SPI1_ILI9488_Set_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

//==============================================================================
// ILI9488 DMA描画処理関数ﾌﾟﾛﾄﾀｲﾌﾟ宣言
//==============================================================================

// 背景を指定色で塗りつぶし
void MSSP_SPI1_DMA_ILI9488_FillBackground(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B);

// 枠線を指定色で描画
void MSSP_SPI1_DMA_ILI9488_DrawBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint8_t R, uint8_t G, uint8_t B);

#endif