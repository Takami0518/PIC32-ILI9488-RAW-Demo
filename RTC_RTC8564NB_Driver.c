#include "System_Structs.h"
#include "RTC_RTC8564NB_Driver.h"

 // 構造体初期化
 RTC_RTC8564NB_DataStruct StructRtcRtc8564nbDatas =
 {
     // 初期化ﾙ-ﾌﾟ用のｶｳﾝﾀを初期化
    .Init_Counter = 0,
    
    // RTC8564NB初期化ｺﾏﾝﾄﾞ数保持用を初期化
    .Initialize_Command_Data_Size = 0,
    
    // RTC8564NB初期化ｺﾏﾝﾄﾞ格納用を初期化
    .RTC8564_Initialize_Data_Table =
    {
        {0x00, 0x20},   // 01.ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ1：時計動作停止
        {0x01, 0x12},   // 02.ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ2：全割込み停止
        {0x02, 0x30},   // 03.秒
        {0x03, 0x22},   // 04.分
        {0x04, 0x21},   // 05.時
        {0x05, 0x17},   // 06.日
        {0x06, 0x07},   // 07.曜
        {0x07, 0x08},   // 08.月
        {0x08, 0x25},   // 09.年(下2桁)
        {0x09, 0x00},   // 10.ｱﾗ-ﾑﾚｼﾞｽﾀ:分
        {0x0A, 0x00},   // 11.ｱﾗ-ﾑﾚｼﾞｽﾀ:時
        {0x0B, 0x00},   // 12.ｱﾗ-ﾑﾚｼﾞｽﾀ:日
        {0x0C, 0x00},   // 13.ｱﾗ-ﾑﾚｼﾞｽﾀ:曜
        {0x0D, 0x00},   // 14.CLKOUT出力設定ﾚｼﾞｽﾀ
        {0x0E, 0x00},   // 15.定周期ﾀｲﾏ割り込み機能制御ﾚｼﾞｽﾀ
        {0x0F, 0x00},   // 16.定周期ﾀｲﾏ用ﾀﾞｳﾝｶｳﾝﾀ
        {0x00, 0x00}    // 17.時計動作開始
        
    },
    
    // 割込み機能初期化ﾙ-ﾌﾟ用のｶｳﾝﾀを初期化
    .Init_Int_Counter = 0,
    
    // RTC8564NB割込みｺﾏﾝﾄﾞ格納用を初期化
    .RTC8564_Initialize_Interrupt_Data_Table = 
    {
        {0x0E, 0x00},   // 割込みﾀｲﾏ-動作停止
        
        {0x01, 0x11},   // ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ2(TI/TPﾋﾞｯﾄ:定周期ﾀｲﾏ割込みｲﾍﾞﾝﾄ発生を継続動作させる)＋(TIEﾋﾞｯﾄ:割込みｲﾍﾞﾝﾄ発生時にINT端子からHighﾚﾍﾞﾙの信号を発生させる)
              
        {0x0D, 0x80},   // CLKOUTﾋﾟﾝから32.768kHzのｸﾛｯｸを出力(※CLKOEﾋﾟﾝをHighﾚﾍﾞﾙに接続しｲﾈ-ﾌﾞﾙ)⇒Timer1の基準ｸﾛｯｸに使用(CPUのT1CKﾋﾟﾝへ接続)
        
        {0x0F, 0x01},   // 定周期ﾀｲﾏ用ﾀﾞｳﾝｶｳﾝﾀ(1秒で割込みﾊﾟﾙｽ発生)
        
        {0x0E, 0x82},   // 定周期ﾀｲﾏ割り込み機能制御ﾚｼﾞｽﾀ(割込みﾀｲﾏ-開始＋ﾀｲﾏ-ｸﾛｯｸ周期：1Hz)
    },
    
    // ﾃﾞ-ﾀ読み取りﾙ-ﾌﾟ用ｶｳﾝﾀを初期化
    .Read_Counter = 0,
    
    // 時刻ﾃﾞ-ﾀ保持用ﾊﾞｯﾌｧ数を初期化
    .Time_Data_Buffer_Size = 0,
    
    // 取得した時刻ﾃﾞ-ﾀ保持用を初期化
    .RTC8564_Time_Data_Table = 
    {
        {0x02, 0x00},   // 1.秒
        {0x03, 0x00},   // 2.分
        {0x04, 0x00},   // 3.時
        {0x05, 0x00},   // 4.日
        {0x06, 0x00},   // 5.曜
        {0x07, 0x00},   // 6.月
        {0x08, 0x00}    // 7.年
    },
    
    .RTC8564_Mseconds_Counter = 0,
    
    .RTC8564_Result_Time_Data_Year      = {0,0,0},
    
    .RTC8564_Result_Time_Data_Month     = {0,0,0},
    
    .RTC8564_Result_Time_Data_Day       = {0,0,0},
    
    .RTC8564_Result_Time_Data_Hour      = {0,0,0},
    
    .RTC8564_Result_Time_Data_Minutes   = {0,0,0},
    
    .RTC8564_Result_Time_Data_Seconds   = {0,0,0},
    
    .RTC8564_Result_Time_Data_Mseconds  = {0,0,0}
        
 };
 
// 関数名   : RTC8564_Get_Initialize_Data_Table_Size
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void RTC8564_Get_Initialize_Data_Table_Size(uint8_t *pInitialize_Command_Size)
{
    *pInitialize_Command_Size = sizeof(StructRtcRtc8564nbDatas.RTC8564_Initialize_Data_Table) / sizeof(StructRtcRtc8564nbDatas.RTC8564_Initialize_Data_Table[0]);
}

// 関数名   : RTC8564_Get_Initialize_Interrupt_Command_Table_Size
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void RTC8564_Get_Initialize_Interrupt_Command_Table_Size(uint8_t *pInitialize_Interrupt_Command_Size)
{
    *pInitialize_Interrupt_Command_Size = sizeof(StructRtcRtc8564nbDatas.RTC8564_Initialize_Interrupt_Data_Table) / sizeof(StructRtcRtc8564nbDatas.RTC8564_Initialize_Interrupt_Data_Table[0]);
}

// 関数名   : RTC8564_Get_Time_Data_Table_Size
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void RTC8564_Get_Time_Data_Table_Size(uint8_t *pTime_Data_Table_Size)
{
    *pTime_Data_Table_Size = sizeof(StructRtcRtc8564nbDatas.RTC8564_Time_Data_Table) / sizeof(StructRtcRtc8564nbDatas.RTC8564_Time_Data_Table[0]);
}

// 関数名   : MSSP_MCP3425_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_RTC8564NB_Initialize(uint8_t deviceAddress, uint8_t registerAddress, uint8_t commandData)
{    
    // I2Cｽﾀ-ﾄ
    MSSP_I2C2_Start();
    
    // ｽﾚ-ﾌﾞｱﾄﾞﾚｽ(書込み・読み込み)を送信
    MSSP_I2C2_SendDataProc(deviceAddress);
    
    // 制御ﾚｼﾞｽﾀを指定
    MSSP_I2C2_SendDataProc(registerAddress);
    
    // 指定ﾚｼﾞｽﾀへの設定ﾃﾞ-ﾀ送信
    MSSP_I2C2_SendDataProc(commandData);
    
    // I2Cｽﾄｯﾌﾟ
    MSSP_I2C2_Stop();
}

// 関数名   : MSSP_RTC8564NB_GetDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_RTC8564NB_GetTimeDataProc(uint8_t ReadRegister, uint8_t *GetDataArray)
{
    // I2Cｽﾀ-ﾄ
    MSSP_I2C2_Start();
    
    __delay_us(50);
        
    // RTC8564NBを書込ﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    MSSP_I2C2_SendDataProc(RTC8564NB_WRITE_ADDRESS_BYTE);  
   
    // 読み込みﾚｼﾞｽﾀを指定
    MSSP_I2C2_SendDataProc(ReadRegister);
    
    // ﾘﾋﾟ-ﾄｽﾀ-ﾄ
    MSSP_I2C2_RepeatStart();
    
    // RTC8564NBを書込ﾓ-ﾄﾞでｽﾚ-ﾌﾞｱﾄﾞﾚｽを送信
    MSSP_I2C2_SendDataProc(RTC8564NB_READ__ADDRESS_BYTE); 
    
    // 1ﾊﾞｲﾄﾃﾞ-ﾀを受信
    MSSP_I2C2_GetDataProc(GetDataArray);
    
    // ﾏｽﾀ-NACKを送信(受信終了)
    MSSP_I2C2_SendNackProc();
    
    //I2Cｽﾄｯﾌﾟ
    MSSP_I2C2_Stop();
}

// 関数名   : RTC8564NB_BcdtoDecConversion
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
uint8_t RTC8564NB_Bcd_to_Dec_to_Str_Conversion(uint8_t TempGarbageNumber, uint8_t *TempBcdData, RTC_RTC8564NB_DataStruct *t)
{
    // 取得したﾃﾞ-ﾀにｺﾞﾐが混入しているのでﾏｽｸ処理
    switch(TempGarbageNumber)
    {
        // 秒の変換
        case 0x00:
            
            // 取得した秒ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x7F;
            
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Seconds[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Seconds[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Seconds[2] = '\0';
            
            break;
            
        // 分の変換
        case 0x01:
            
            // 取得した分ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x7F;
            
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Minutes[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Minutes[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Minutes[2] = '\0';
            
            break;
            
        // 時の変換
        case 0x02:
            
            // 取得した時ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x3F;
            
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Hour[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Hour[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Hour[2] = '\0';
            
            break;
        
        // 日の変換
        case 0x03:
            
            // 取得した日ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x3F;
        
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Day[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Day[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Day[2] = '\0';
            
            break;
        
        // 曜日は表示予定無い為、文字変換無し
        case 0x04:
            
            // 取得した週ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x07;
            
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            break;
            
        // 月
        case 0x05:
            
            // 取得した月ﾃﾞ-ﾀをﾏｽｸしｺﾞﾐ除去
            *TempBcdData &= 0x1F;
            
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Month[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Month[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Month[2] = '\0';
            
            break;            
            
        // 年(ﾏｽｸ処理不要)
        default:
  
            // BCD⇒10進数へ変換
            *TempBcdData = ((*TempBcdData >> 4) * 10) + (*TempBcdData & 0x0F);
            
            // 10進数⇒文字へ変換
            t->RTC8564_Result_Time_Data_Year[0] = (*TempBcdData / 10) + '0';
            
            t->RTC8564_Result_Time_Data_Year[1] = (*TempBcdData % 10) + '0';
            
            t->RTC8564_Result_Time_Data_Year[2] = '\0';
            
            break;        
    }
}