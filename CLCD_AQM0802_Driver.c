#include "CLCD_AQM0802_Driver.h"
#include "System_Peripherals.h"

void MSSP_Lcd_Initialize(void)
{
    __delay_ms(40); 
    
    // ﾌｧﾝｸｼｮﾝｾｯﾄ
    MSSP_Lcd_SendCommandDataProc(0x38);
    
    // ﾌｧﾝｸｼｮﾝｾｯﾄ
    MSSP_Lcd_SendCommandDataProc(0x39);  // ｲﾝﾀ-ﾌｪ-ｽ=8bit 使用行数2 ﾀﾞﾌﾞﾙ高さﾌｫﾝﾄ=不使用 拡張ｺﾏﾝﾄﾞ=使用
    
    // 内部発振器周波数(拡張ｺﾏﾝﾄﾞ使用時のみ)
    MSSP_Lcd_SendCommandDataProc(0x14);  // F2:内部発振器周波数を調整
    
    // ｺﾝﾄﾗｽﾄ調整
    MSSP_Lcd_SendCommandDataProc(0x7C);  
    
    // 
    MSSP_Lcd_SendCommandDataProc(0x55);
    
    MSSP_Lcd_SendCommandDataProc(0x6D);
    
    __delay_ms(200);//200ms wait
    
    MSSP_Lcd_SendCommandDataProc(0x38);
    
    MSSP_Lcd_SendCommandDataProc(0x0C);
    
    MSSP_Lcd_SendCommandDataProc(0x01);
 
}

void MSSP_Lcd_SendCommandDataProc(unsigned char cmd)
{
    // I2Cｽﾀ-ﾄ
    MSSP_I2C1_Start();
    
    __delay_us(50);
    
    // ｽﾚ-ﾌﾞｱﾄﾞﾚｽ送信
    MSSP_I2C1_SendDataProc(LCD_addr);

    // ｺﾝﾄﾛ-ﾙﾊﾞｲﾄを送信
    MSSP_I2C1_SendDataProc(0x00);

    // ｺﾏﾝﾄﾞ送信
    MSSP_I2C1_SendDataProc(cmd);
    
    // I2Cｽﾄｯﾌﾟ
    MSSP_I2C1_Stop();
}

void MSSP_Lcd_SendCharacterDataProc(unsigned char dat)
{
    // I2Cｽﾀ-ﾄ
    MSSP_I2C1_Start();
    
    __delay_us(50);
            
    // ｽﾚ-ﾌﾞｱﾄﾞﾚｽ送信
    MSSP_I2C1_SendDataProc(LCD_addr);

    // ｺﾝﾄﾛ-ﾙﾊﾞｲﾄを送信
    MSSP_I2C1_SendDataProc(0x40);

    // 文字ﾃﾞ-ﾀ送信
    MSSP_I2C1_SendDataProc(dat);

    // I2Cｽﾄｯﾌﾟ
    MSSP_I2C1_Stop(); //stop condition
}

void MSSP_Lcd_String_Send(char *str)
{
    while(*str)
    {
        MSSP_Lcd_SendCharacterDataProc(*str++); 
    }
}

/* Clear Display */
void MSSP_Lcd_Clear(void)
{
    MSSP_Lcd_SendCommandDataProc(0x01);

    __delay_ms(1);

    __delay_us(80);
}

/* Return Home `*/
void MSSP_Lcd_Home(void)
{
    MSSP_Lcd_SendCommandDataProc(0x02);

    __delay_ms(1);

    __delay_us(80);
}

/* Cursor 2line top */
void MSSP_LCD_2line(void)
{
    MSSP_Lcd_SendCommandDataProc(0xC0);
}

