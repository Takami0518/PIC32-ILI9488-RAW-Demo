#include <xc.h>
#include <stdbool.h>
#include "ADC_MCP3425_Driver.h"

// 関数名   : MSSP_I2C1_MCP3425_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
// 内部ﾌﾟﾙｱｯﾌﾟ20?50kΩではI2Cﾎﾞ-ﾚ-ﾄの高速処理が不向きなので外部ﾌﾟﾙｱｯﾌﾟ(4.7kΩ)に変更
void MSSP_I2C1_MCP3425_Initialize(void)
{
    bool ReceiveAcknowledgeData = 1;
    
    // RG3/SDAを入力ﾓ-ﾄﾞ
    TRISGbits.TRISG3 = 0x01;
    
    // ﾌﾟﾙｱｯﾌﾟ有効
    //CNPUGbits.CNPUG3 = 0x01;
    
    // RG2/SCLを入力ﾓ-ﾄﾞ
    TRISGbits.TRISG2 = 0x01;
    
    // ﾌﾟﾙｱｯﾌﾟ有効
    //CNPUGbits.CNPUG2 = 0x01;
    
    // I2Cｽﾀ-ﾄ
    MSSP_I2C1_Start();
    
    __delay_us(50);
    
    // MCP3425を書込みﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    ReceiveAcknowledgeData = MSSP_I2C1_SendDataProc( MCP3425_WRITE_ADRESS_BYTE);
    
    // MCP3425からのACKﾁｪｯｸ
    if(ReceiveAcknowledgeData == false)
    {
        // ﾘﾀ-ﾝﾎ-ﾑ
        MSSP_Lcd_Home();
        
        // ｴﾗ-表示1
        MSSP_Lcd_String_Send("FailNACK");
        
        // 2行目へｷｬﾘｯｼﾞﾘﾀ-ﾝ
        MSSP_LCD_2line();
        
        // ｴﾗ-表示2
        MSSP_Lcd_String_Send("AD1SAW");
    }
    
    // ｺﾝﾌｨｸﾞﾚ-ｼｮﾝ値を送信
    ReceiveAcknowledgeData = MSSP_I2C1_SendDataProc(MCP3425_CONFIGURATION_BYTE);
    
    // MCP3425からのACKﾁｪｯｸ
    if(ReceiveAcknowledgeData == false)
    {
        // ﾘﾀ-ﾝﾎ-ﾑ
        MSSP_Lcd_Home();
        
        // ｴﾗ-表示1
        MSSP_Lcd_String_Send("FailNACK");
        
        // 2行目へｷｬﾘｯｼﾞﾘﾀ-ﾝ
        MSSP_LCD_2line();
        
        // ｴﾗ-表示2
        MSSP_Lcd_String_Send("AD1/CFW");
    }
    
    // I2Cｽﾄｯﾌﾟ
    MSSP_I2C1_Stop();
}

// 関数名   : MSSP_I2C1_MCP3425_GetDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C1_MCP3425_GetDataProc(unsigned char *upper_data, unsigned char *lower_data)
{
    // I2Cｽﾀ-ﾄ
    MSSP_I2C1_Start();
    
    __delay_us(50);
        
    // MCP3425を読込ﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    MSSP_I2C1_SendDataProc(MCP3425_READ__ADRESS_BYTE);  
    
    // MCP3425からのACKﾁｪｯｸ
    while(MSSP_I2C1_CheckAckProc()){};
        
    //__delay_ms(1);
    
    // 上位1ﾊﾞｲﾄﾃﾞ-ﾀを受信
    MSSP_I2C1_GetDataProc(upper_data);
    
    // ﾏｽﾀ-ACKを送信
    MSSP_I2C1_SendAckProc();
    
    // 下位1ﾊﾞｲﾄﾃﾞ-ﾀを受信
    MSSP_I2C1_GetDataProc(lower_data);
    
    // ﾏｽﾀ-NACKを送信(受信終了)
    MSSP_I2C1_SendNackProc();
    
    //I2Cｽﾄｯﾌﾟ
    MSSP_I2C1_Stop();
}

// 関数名   : MSSP_I2C2_MCP3425_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
// 内部ﾌﾟﾙｱｯﾌﾟ20?50kΩではI2Cﾎﾞ-ﾚ-ﾄの高速処理が不向きなので外部ﾌﾟﾙｱｯﾌﾟ(4.7kΩ)に変更
void MSSP_I2C2_MCP3425_Initialize(void)
{
    bool ReceiveAcknowledgeData;
    
    // RF4/SDA2を入力ﾓ-ﾄﾞ
    TRISFbits.TRISF4 = 0x01;
    
    // ﾌﾟﾙｱｯﾌﾟ有効
    //CNPUFbits.CNPUF4 = 0x01;
    
    // RF5/SCL2を入力ﾓ-ﾄﾞ
    TRISFbits.TRISF5 = 0x01;
    
    // ﾌﾟﾙｱｯﾌﾟ有効
    //CNPUFbits.CNPUF5 = 0x01;
    
    // I2Cｽﾀ-ﾄ
    MSSP_I2C2_Start();
    
    __delay_us(50);
    
    // MCP3425を書込みﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    ReceiveAcknowledgeData = MSSP_I2C2_SendDataProc(MCP3425_WRITE_ADRESS_BYTE);

    // MCP3425からのACKﾁｪｯｸ
    if(ReceiveAcknowledgeData == false)
    {
        // ﾘﾀ-ﾝﾎ-ﾑ
        MSSP_Lcd_Home();
        
        // ｴﾗ-表示1
        MSSP_Lcd_String_Send("FailNACK");
        
        // 2行目へｷｬﾘｯｼﾞﾘﾀ-ﾝ
        MSSP_LCD_2line();
        
        // ｴﾗ-表示2
        MSSP_Lcd_String_Send("AD2/CFW");
    }
    
    // ｺﾝﾌｨｸﾞﾚ-ｼｮﾝ値を送信
    ReceiveAcknowledgeData = MSSP_I2C2_SendDataProc(MCP3425_CONFIGURATION_BYTE);
   
    // MCP3425からのACKﾁｪｯｸ
    if(ReceiveAcknowledgeData == false)
    {
        // ﾘﾀ-ﾝﾎ-ﾑ
        MSSP_Lcd_Home();
        
        // ｴﾗ-表示1
        MSSP_Lcd_String_Send("FailNACK");
        
        // 2行目へｷｬﾘｯｼﾞﾘﾀ-ﾝ
        MSSP_LCD_2line();
        
        // ｴﾗ-表示2
        MSSP_Lcd_String_Send("AD2/CFW");
    }
    
    // I2Cｽﾄｯﾌﾟ
    MSSP_I2C2_Stop();
}

// 関数名   : MSSP_I2C2_MCP3425_GetDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C2_MCP3425_GetDataProc(unsigned char *upper_data, unsigned char *lower_data)
{
    // I2Cｽﾀ-ﾄ
    MSSP_I2C2_Start();
    
    __delay_us(50);
        
    // MCP3425を読込ﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    MSSP_I2C2_SendDataProc(MCP3425_READ__ADRESS_BYTE);  
    
    // MCP3425からのACKﾁｪｯｸ
    while(MSSP_I2C2_CheckAckProc()){};
        
    
    // 上位1ﾊﾞｲﾄﾃﾞ-ﾀを受信
    MSSP_I2C2_GetDataProc(upper_data);
    
    // ﾏｽﾀ-ACKを送信
    MSSP_I2C2_SendAckProc();
    
    // 下位1ﾊﾞｲﾄﾃﾞ-ﾀを受信
    MSSP_I2C2_GetDataProc(lower_data);
    
    // ﾏｽﾀ-NACKを送信(受信終了)
    MSSP_I2C2_SendNackProc();
    
    //I2Cｽﾄｯﾌﾟ
    MSSP_I2C2_Stop();
}