#include <xc.h>
#include <sys/attribs.h>    // ISRﾏｸﾛ用
#include <sys/kmem.h>       // KVA_TO_PAﾏｸﾛ用
#include "System_Peripherals.h"
#include "GLCD_ILI9431_Driver.h"


// 関数名   : MSSP_SPI1_ILI9341_Initialize
// 機能     : ILI9341(3.2inch):X320/Y240
// 引数     : 
// 戻り値   : 
// 使用条件 : SPIﾓ-ﾄﾞ3で動作(必須条件) CKE=0 CKP=1の設定でないと動作しない(ﾃﾞ-ﾀｼ-ﾄ記載無し)(CPOL=1/CPHA=1)
// CKE=0 : ﾃﾞ-ﾀ転送はｸﾛｯｸの立ち上がり(LOW⇒HIGH)で行う。
// CKP=1 : ｸﾛｯｸｱｲﾄﾞﾙ状態はHIGH

// ｶﾗ-TFTにはVCOMと呼ばれる液晶の基準となる共通電圧(ｺﾝﾄﾗｽﾄ向上･ちらすき防止)

void MSSP_SPI1_ILI9341_Initialize(void)
{
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x00;
    
    // ﾊ-ﾄﾞｳｪｱﾘｾｯﾄ実行
    SPI1_RST = 0;
    __delay_ms(20);
    SPI1_RST = 1;
    __delay_ms(5);
        
    // 電源制御1(PowerControl1)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0xC0);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x23);

    // 電源制御2(PowerControl2)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0xC1);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x10);

    // VCOM制御1
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0xC5);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x3E);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x28);
    
    // ﾒﾓﾘｱｸｾｽｺﾝﾄﾛ-ﾙ(画面回転:横向表示)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x36);
    MSSP_SPI1_ILI9341_WriteData_Proc(0xA8);

    // ﾋﾟｸｾﾙﾌｫ-ﾏｯﾄ設定(16bit/pixel)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x3A);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x55);

    // ﾃﾞｨｽﾌﾟﾚｲON解除(ｽﾘ-ﾌﾟｱｳﾄ)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x11);
    __delay_ms(120);  // 120ms待機（スリープ解除）

    // ﾃﾞｨｽﾌﾟﾚｲﾌﾞﾗｲﾈｽ(輝度)
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x51);
    MSSP_SPI1_ILI9341_WriteData_Proc(0xFF);
    
    // ﾃﾞｨｽﾌﾟﾚｲON
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x29);
  
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x01;

}

// 関数名   : MSSP_SPI1_ILI9341_WriteCommand_Proc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_ILI9341_WriteCommand_Proc(unsigned char data)
{
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x00;
    
    // ｺﾏﾝﾄﾞﾓ-ﾄﾞ発行
    SPI1_DC     = 0x00;
    
    // ｺﾏﾝﾄﾞﾃﾞ-ﾀ送信処理
    MSSP_SPI1_SendDataProc(data);
    
    // ｺﾏﾝﾄﾞﾃﾞ-ﾀをDMA送信処理
    //DMA_SPI1_Send_Byte_Proc(data);

    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x01;
}

// 関数名   : MSSP_SPI1_ILI9341_WriteData_Proc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_ILI9341_WriteData_Proc(unsigned char data)
{
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x00;
    
    // ﾃﾞ-ﾀﾓ-ﾄﾞ発行
    SPI1_DC     = 0x01;
    
    // ｺﾏﾝﾄﾞﾃﾞ-ﾀ送信処理
    MSSP_SPI1_Send_Recv_Data_Proc(data,SPI_SEND);
    
    // ｺﾏﾝﾄﾞﾃﾞ-ﾀをDMA送信処理
    //DMA_SPI1_Send_Byte_Proc(data);   

    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x01;
}
// 関数名   : MSSP_SPI1_ILI9341_Set_Address_Window_Proc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_ILI9341_Set_Address_Window_Proc(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // CASET
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2A);
    MSSP_SPI1_ILI9341_WriteData_Proc(x0 >> 8); MSSP_SPI1_ILI9341_WriteData_Proc(x0 & 0xFF);
    MSSP_SPI1_ILI9341_WriteData_Proc(x1 >> 8); MSSP_SPI1_ILI9341_WriteData_Proc(x1 & 0xFF);

    //RASET
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2B);
    MSSP_SPI1_ILI9341_WriteData_Proc(y0 >> 8); MSSP_SPI1_ILI9341_WriteData_Proc(y0 & 0xFF);
    MSSP_SPI1_ILI9341_WriteData_Proc(y1 >> 8); MSSP_SPI1_ILI9341_WriteData_Proc(y1 & 0xFF);
    
    // RAMWR
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2C); 
}

// 関数名   : MSSP_SPI1_ILI9341_Back_Ground_Draw_Proc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_ILI9341_Back_Ground_Draw_Proc(void)
{    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x00;
    
    // 描画位置をｾｯﾄ(始点:X0/Y0 終点:X319/Y239)
    //MSSP_SPI1_ILI9341_Set_Address_Window_Proc(0, 0, 479, 319);
    
        // X範囲 0〜319
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2A);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x00);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x00);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x01);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x3F);

    // Y範囲 0〜479
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2B);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x00);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x00);
    MSSP_SPI1_ILI9341_WriteData_Proc(0x01);
    MSSP_SPI1_ILI9341_WriteData_Proc(0xDF);

    // RAM書き込み開始
    MSSP_SPI1_ILI9341_WriteCommand_Proc(0x2C);
    
    
    // ﾋﾟｸｾﾙﾃﾞ-ﾀを送信(2ﾊﾞｲﾄ毎)
    for(int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    {
        MSSP_SPI1_ILI9341_WriteData_Proc((GREEN_COLOR >> 8) & 0xFF);
        
        MSSP_SPI1_ILI9341_WriteData_Proc(GREEN_COLOR & 0xFF);
    }
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x01;
}


// 関数名   : MSSP_SPI1_ILI9341_Rectangle_Button_Draw_Proc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_ILI9341_Rectangle_Button_Draw_Proc(void)
{
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x00;
    
    // 描画位置をｾｯﾄ(始点:X0/Y0 終点:X319/Y48)
    MSSP_SPI1_ILI9341_Set_Address_Window_Proc(0, 0, 319, 48);
     
    // ｺﾏﾝﾄﾞﾎﾞﾀﾝ背景色塗り潰し
    for(int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    {
        MSSP_SPI1_ILI9341_WriteData_Proc((NEAR_WHITE_SILVER >> 8) & 0xFF);

        MSSP_SPI1_ILI9341_WriteData_Proc(NEAR_WHITE_SILVER & 0xFF);
    }
    
    /*for (uint16_t i = 0; i < 320; i++)
    {
        ILI9341_DrawPixel(0x0000 + i, 0x0030, BLACK_COLOR);
        
        ILI9341_DrawPixel(0x0000 + i, 0x0000, BLACK_COLOR);
    }
    
    for (uint16_t i = 0; i < 0x30; i++)
    {
        ILI9341_DrawPixel(0x0000, 0x0000 + i, BLACK_COLOR);
        
        ILI9341_DrawPixel(0x003F, 0x0000 + i, BLACK_COLOR);

        ILI9341_DrawPixel(0x007E, 0x0000 + i, BLACK_COLOR);
        
        ILI9341_DrawPixel(0x00BD, 0x0000 + i, BLACK_COLOR);
        
        ILI9341_DrawPixel(0x00FC, 0x0000 + i, BLACK_COLOR);
    }*/
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9341_CS     = 0x01;
    
}


void ILI9341_ClearScreen(uint16_t color)
{
    uint32_t pixels = LCD_WIDTH * LCD_HEIGHT;
    
    MSSP_SPI1_ILI9341_Set_Address_Window_Proc(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    for (uint32_t i = 0; i < pixels; i++)
    {
        MSSP_SPI1_ILI9341_WriteData_Proc(color >> 8);
        MSSP_SPI1_ILI9341_WriteData_Proc(color & 0xFF);
    }
}

void DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    
    int dx = x1 - x0;
    int dy = y1 - y0;

    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    if (steps == 0)
    {
        ILI9341_DrawPixel(x0, y0, RED_COLOR);
        
        return;
    }

    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps; i++)
    {
        ILI9341_DrawPixel((uint16_t)(x + 0.5f), (uint16_t)(y + 0.5f), RED_COLOR);
        
        x += x_inc;
        
        y += y_inc;
    }
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
   
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) return;
    
    MSSP_SPI1_ILI9341_Set_Address_Window_Proc(x, y, x, y);   // 1ピクセルの描画範囲指定

    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x00;
    
    // ﾃﾞ-ﾀﾓ-ﾄﾞ発行
    SPI1_DC     = 0x01;
    
    MSSP_SPI1_SendDataProc(color >> 8);

    MSSP_SPI1_SendDataProc(color & 0xFF);
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    //SPI1_ILI9341_CS     = 0x01;
}

void ILI9341_DrawFont16x16(int x, int y, const uint8_t *fontData, uint16_t fg, uint16_t bg)
{
    for (int row = 0; row < 16; row++) {
        // 下から上に読むように行を反転
        uint16_t line = (fontData[(15 - row) * 2] << 8) | fontData[(15 - row) * 2 + 1];

        for (int col = 0; col < 16; col++) {
            if (line & (0x8000 >> col)) {
                ILI9341_DrawPixel(x + col, y + row, fg);
            } else {
                ILI9341_DrawPixel(x + col, y + row, bg);
            }
        }
    }
}

