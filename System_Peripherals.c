//==============================================================================
// File Name            : Peripheral.c
// Creator              : 高見 謙介
// Created Date         : 2025年8月6日
// CPU                  : PIC32MX370F512H
// Compiler             : MPLAB XC32 v4.45
// Overview             : CPUに関連するﾍﾟﾘﾌｪﾗﾙ・ﾓｼﾞｭ-ﾙ機能制御を集約
//                      : I2C･SPIはMSSP(MasterSynchronousSerialPort)を使用
//                      : UARTはPSS(Peripheral Pin Select)を使用してI/Oﾋﾟﾝ割り当て
// Operating Frequency  : 80MHz
// Peripheral Bus Clock : 80MHz
//==============================================================================

#include <xc.h>
#include <sys/attribs.h>    // ISRﾏｸﾛ用
#include <sys/kmem.h>       // KVA_TO_PAﾏｸﾛ用

#include "System_Peripherals.h"

// 関数名   : CPU_Initialize
// 機能     : I/Oの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : CPU起動後に開始されること
void CPU_Initialize(void)
{
    // ﾎﾟ-ﾄBは全てﾃﾞｼﾞﾀﾙﾓ-ﾄﾞ
    ANSELB  = 0x0000;
    // ﾎﾟ-ﾄCは全てﾃﾞｼﾞﾀﾙﾓ-ﾄﾞ
    ANSELC  = 0x0000;
    // ﾎﾟ-ﾄDは全てﾃﾞｼﾞﾀﾙﾓ-ﾄﾞ
    ANSELD  = 0x0000;
    // ﾎﾟ-ﾄEは全てﾃﾞｼﾞﾀﾙﾓ-ﾄﾞ
    ANSELE  = 0x0000;
    // ﾎﾟ-ﾄGは全てﾃﾞｼﾞﾀﾙﾓ-ﾄﾞ
    ANSELG  = 0x0000;
    
    // ﾎﾟ-ﾄBは全て出力ﾓ-ﾄﾞ
    TRISB   = 0x0000;
    // ﾎﾟ-ﾄCは全て出力ﾓ-ﾄﾞ
    TRISC   = 0x0000;
    // ﾎﾟ-ﾄDは全て出力ﾓ-ﾄﾞ   
    TRISD   = 0x0000;
    // ﾎﾟ-ﾄEは全て出力ﾓ-ﾄﾞ   
    TRISE   = 0x0000;
    // ﾎﾟ-ﾄFは全て出力ﾓ-ﾄﾞ   
    TRISF   = 0x0000;
    // ﾎﾟ-ﾄGは全て出力ﾓ-ﾄﾞ
    TRISG   = 0x0000;
}

// 関数名   : PPS_External_Interrupt_Setup
// 機能     : MSSP I2C1使用における有効化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void PPS_External_Interrupt_Setup(void)
{
    // PPS unlock
    SYSKEY = 0xAA996655;
    
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK = 0;
    
    // ﾋﾟﾝ方向設定(RB6を入力に設定）
    TRISBbits.TRISB6 = 1; 
    
    // RB6(17番PIN)を外部割込みﾋﾟﾝ(INT1)に設定(RTC8564NBからの1Hz割込み入力用)
    INT1R   = 0b0101;
    
    // PPS lock
    CFGCONbits.IOLOCK = 1;
    
    SYSKEY = 0x00000000;
}

void External_Interrupt_Initialize(void)
{
    // ﾏﾙﾁﾍﾞｸﾀﾓ-ﾄﾞ有効化
    INTCONbits.MVEC = 1;
    
    // 全体の割込み許可
    __builtin_enable_interrupts();
    
    // RB6(17番PIN)を外部割込みﾋﾟﾝ(INT1)に設定
    PPS_External_Interrupt_Setup();
   
    // 割込みｴｯｼﾞ設定(立ち上がりで割込み発生)
    INTCONbits.INT1EP = 0;   

    // ﾌﾗｸﾞｸﾘｱ
    IFS0bits.INT1IF = 0;    

    // 優先度設定(IPL3)
    IPC1bits.INT1IP = 3;    
    IPC1bits.INT1IS = 0;
        
    // 割込み有効化
    IEC0bits.INT1IE = 1;    
}

// 関数名   : PPS_External_Interrupt_Setup
// 機能     : MSSP I2C1使用における有効化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void PPS_External_Interrupt2_Setup(void)
{
    // PPS unlock
    SYSKEY = 0xAA996655;
    
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK = 0;
    
    // ﾋﾟﾝ方向設定(RD9を入力に設定)
    TRISDbits.TRISD9 = 1;
    
    // RD9(43番PIN)を外部割込みﾋﾟﾝ(INT2)に設定(XPT2048からのIRQﾋﾟﾝからのﾀｯﾁ割込み入力用)
    INT2R   = 0b0000;
    
    // PPS lock
    CFGCONbits.IOLOCK = 1;
    
    SYSKEY = 0x00000000;
}

void External_Interrupt2_Initialize(void)
{
    // ﾏﾙﾁﾍﾞｸﾀﾓ-ﾄﾞ有効化
    INTCONbits.MVEC = 1;
    
    // 全体の割込み許可
    __builtin_enable_interrupts();
    
    // 割込み処理用ﾋﾟﾝ定義(43番PIN(RD9)PPS設定 XPT2048からのIRQ信号をﾀｯﾁ割込みとして入力)
    PPS_External_Interrupt2_Setup();
    
    // XPT2048のIRQ信号はﾉﾝﾀｯﾁ時にHIGH/ﾀｯﾁ時にLOWなので割込みｲﾍﾞﾝﾄ発生ﾄﾘｶﾞはLOW設定
    INTCONbits.INT2EP   = 0;
    
    // 割込みﾌﾗｸﾞｸﾘｱ
    IFS0bits.INT2IF     = 0;

    IPC2bits.INT2IP = 4;     // 割込み優先度
    IPC2bits.INT2IS = 0;     // サブ優先度

    // 割込み無効
    IEC0bits.INT2IE = 0;
}


void Timer1_ExternalClk_Initialize(void)
{
    // RC14ﾋﾟﾝをT1CK(Timer1入力ｸﾛｯｸｿ-ｽ)として使用する為、入力ﾓ-ﾄﾞに設定
    TRISCbits.TRISC14 = 1;
    
    T1CON = 0;              // Timer1 初期化
    TMR1 = 0;               // カウンタクリア

    T1CONbits.TCS = 1;      // 外部クロック入力
    T1CONbits.TCKPS = 0;    // プリスケーラ 1:1 (必要に応じ変更)
    //PR1 = 328 - 1;             // 32.768 カウント ? 1 ms
    PR1 = 32 - 1;
    
    IPC1bits.T1IP = 3;      // 優先度
    IPC1bits.T1IS = 0;      // サブ優先度
    IFS0bits.T1IF = 0;      // フラグクリア
    IEC0bits.T1IE = 1;      // 割込み許可

    T1CONbits.TON = 1;      // タイマー開始
}


// 関数名   : MSSP_I2C1_ControlRegister_Initialize
// 機能     : MSSP I2C1使用における有効化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void MSSP_I2C1_ControlRegister_Initialize(void)
{   
    // I2C1CONﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
    I2C1CON = I2Cx_CON_INIT_BITS;
}

// 関数名   : MSSP_I2C1_StatasRegister_Initialize
// 機能     : MSSP I2C1ｽﾃ-ﾀｽﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_I2C1_StatasRegister_Initialize(void)
 {
     // I2C1STATﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定 
     I2C1STAT = I2Cx_STA_INIT_BITS;
 }

// 関数名   : MSSP_I2C1_Baudrate_Initialize
// 機能     : MSSP I2C1ﾎﾞ-ﾚ-ﾄ初期化設定(100kHzで動作)
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void MSSP_I2C1_Baudrate_Initialize(void)
{
    // I2C1ﾌﾟﾛﾄｺﾙのﾎﾞ-ﾚ-ﾄを設定(100kHz))
    //I2C1BRG = ((80000000 / (2 * 100000)) - 1);
    
    // I2C1ﾌﾟﾛﾄｺﾙのﾎﾞ-ﾚ-ﾄを設定(400kHz))
    I2C1BRG = ((80000000 / (2 * 400000)) - 1);
}

// 関数名   : MSSP_I2C1_Start
// 機能     : I2C1でｽﾀｰﾄｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C_Initialize関数が実行済みであること
bool MSSP_I2C1_Start(void)
{
    // ｽﾀ-ﾄ発行開始
    I2C1CONbits.SEN = 1;
    // 発行完了まで待機(SEN=0になるまで待つ)
    while(I2C1CONbits.SEN){};

    // ｽﾀ-ﾄが正しく検出されたか確認
    if (I2C1STATbits.S)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C1_Stop
// 機能     : I2C1でｽﾄｯﾌﾟｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C_Initialize関数が実行済みであること
bool MSSP_I2C1_Stop(void)
{
    // ｽﾄｯﾌﾟ発行開始
    I2C1CONbits.PEN = 0x01;
    
    // 発行完了まで待機(PEN=0になるまで待つ)
    while(I2C1CONbits.PEN){};

    // ｽﾄｯﾌﾟが正しく検出されたか確認
    if (I2C1STATbits.RBF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C1_RepeatStart
// 機能     : I2C1でﾘｽﾀ-ﾄｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C1_Initialize関数が実行済みであること
bool MSSP_I2C1_RepeatStart(void)
{    
    // ﾘｽﾀ-ﾄ発行開始
    I2C1CONbits.RSEN = 0x01;
    
    // 発行完了まで待機(RSEN=0になるまで待つ)
    while(I2C1CONbits.RSEN){};
    
    // ﾘｽﾀ-ﾄが正しく検出されたか確認
    if(I2C1STATbits.S)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C1_SendDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
bool MSSP_I2C1_SendDataProc(unsigned char dat)
{
    // ﾃﾞ-ﾀを送信
    I2C1TRN = dat;
    
    // ﾃﾞ-ﾀﾊﾞｯﾌｧが空になるのを待つ
    while(I2C1STATbits.TBF){};
    
    // 送信完了を待機(TRSTAT=1の間は送信中)
    while(I2C1STATbits.TRSTAT){};
    
    // ｽﾚ-ﾌﾞからのACK/NACKの確認
    if (!I2C1STATbits.ACKSTAT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C1_GetDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C1_GetDataProc(unsigned char *get_data)
{
    // 受信有効ﾋﾞｯﾄをｵﾝ
    I2C1CONbits.RCEN    = 0x01;
    
    // 1ﾊﾞｲﾄのﾃﾞ-ﾀ受信を終えるのを待つ
    while(!I2C1STATbits.RBF){};
        
    // 取得ﾃﾞ-ﾀはﾎﾟｲﾝﾀで返却
    *get_data = I2C1RCV;
}

// 関数名   : MSSP_I2C1_CheckAckProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
bool MSSP_I2C1_CheckAckProc(void)
{
    // ｽﾚ-ﾌﾞからのACK/NACKの確認
	if (I2C1STATbits.ACKSTAT)
    {
		return true;
	}
    else
    {
		return false;
	}
}

// 関数名   : MSSP_I2C1_SendAckProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C1_SendAckProc(void)
{
    // ACKDTにACKをセット(負論理なので0を設定)
    I2C1CONbits.ACKDT = 0;

    // ACK信号生成
    I2C1CONbits.ACKEN = 1;
    
    while(I2C1CONbits.ACKEN){};
}

// 関数名   : MSSP_I2C1_SendNackProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C1_SendNackProc(void)
{
    // ACKDTにNACKをセット(負論理なので1を設定)
	I2C1CONbits.ACKDT = 1;
    
    // NACK信号生成
	I2C1CONbits.ACKEN = 1;
    
    while(I2C1CONbits.ACKEN){};
}

// 関数名   : MSSP_I2C2_ControlRegister_Initialize
// 機能     : MSSP I2C2使用における有効化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void MSSP_I2C2_ControlRegister_Initialize(void)
{   
    // I2C2CONﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
    I2C2CON = I2Cx_CON_INIT_BITS;
 }

// 関数名   : MSSP_I2C2_StatasRegister_Initialize
// 機能     : MSSP I2C2ｽﾃ-ﾀｽﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_I2C2_StatasRegister_Initialize(void)
{
    // I2C2STATﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定 
    I2C2STAT = I2Cx_STA_INIT_BITS;
}

// 関数名   : MSSP_I2C2_Baudrate_Initialize
// 機能     : MSSP I2C2ﾎﾞ-ﾚ-ﾄ初期化設定(100kHzで動作)
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void MSSP_I2C2_Baudrate_Initialize(void)
{
    // I2C2ﾌﾟﾛﾄｺﾙのﾎﾞ-ﾚ-ﾄを設定(100kHz))
    //I2C2BRG = ((80000000 / (2 * 100000)) - 1);
    
    // I2C2ﾌﾟﾛﾄｺﾙのﾎﾞ-ﾚ-ﾄを設定(400kHz))
    I2C2BRG = ((80000000 / (2 * 400000)) - 1);
}

// 関数名   : MSSP_I2C2_Start
// 機能     : I2C2でｽﾀｰﾄｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C_Initialize関数が実行済みであること
bool MSSP_I2C2_Start(void)
{
    // ｽﾀ-ﾄ発行開始
    I2C2CONbits.SEN = 1;
    // 発行完了まで待機(SEN=0になるまで待つ)
    while(I2C2CONbits.SEN){};

    // ｽﾀ-ﾄが正しく検出されたか確認
    if (I2C2STATbits.S)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C2_Stop
// 機能     : I2C2でｽﾄｯﾌﾟｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C_Initialize関数が実行済みであること
bool MSSP_I2C2_Stop(void)
{
    // ｽﾄｯﾌﾟ発行開始
    I2C2CONbits.PEN = 0x01;
    
    // 発行完了まで待機(PEN=0になるまで待つ)
    while(I2C2CONbits.PEN){};

    // ｽﾄｯﾌﾟが正しく検出されたか確認
    if (I2C2STATbits.RBF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C2_RepeatStart
// 機能     : I2C2でﾘｽﾀ-ﾄｺﾝﾃﾞｨｼｮﾝを発行し､検出を確認する
// 引数     : なし
// 戻り値   : 正常処理完了：True　異常終了：False
// 使用条件 : MSSP_I2C2_Initialize関数が実行済みであること
bool MSSP_I2C2_RepeatStart(void)
{    
    // ﾘｽﾀ-ﾄ発行開始
    I2C2CONbits.RSEN = 0x01;
    
    // 発行完了まで待機(RSEN=0になるまで待つ)
    while(I2C2CONbits.RSEN){};
    
    // ﾘｽﾀ-ﾄが正しく検出されたか確認
    if(I2C2STATbits.S)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C2_SendDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
bool MSSP_I2C2_SendDataProc(unsigned char dat)
{
    // ﾃﾞ-ﾀを送信
    I2C2TRN = dat;

    // ﾃﾞ-ﾀﾊﾞｯﾌｧが空になるのを待つ
    while(I2C2STATbits.TBF){};

    // 送信完了を待機(TRSTAT=1の間は送信中)
    while(I2C2STATbits.TRSTAT){};
    
    // ｽﾚ-ﾌﾞからのACK/NACKの確認
    if (!I2C2STATbits.ACKSTAT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 関数名   : MSSP_I2C2_GetDataProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C2_GetDataProc(unsigned char *get_data)
{
    // 受信有効ﾋﾞｯﾄをｵﾝ
    I2C2CONbits.RCEN    = 0x01;
    
    // 1ﾊﾞｲﾄのﾃﾞ-ﾀ受信を終えるのを待つ
    while(!I2C2STATbits.RBF){};
        
    // 取得ﾃﾞ-ﾀはﾎﾟｲﾝﾀで返却
    *get_data = I2C2RCV;
}

// 関数名   : MSSP_I2C2_CheckAckProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
bool MSSP_I2C2_CheckAckProc(void)
{
    // ｽﾚ-ﾌﾞからのACK/NACKの確認
	if (I2C2STATbits.ACKSTAT)
    {
		return true;
	}
    else
    {
		return false;
	}
}

// 関数名   : MSSP_I2C2_SendAckProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C2_SendAckProc(void)
{
    // ACKDTにACKをセット(負論理なので0を設定)
    I2C2CONbits.ACKDT = 0;
    
    __delay_us(125);

    // ACK信号生成
    I2C2CONbits.ACKEN = 1;
    
    while(I2C2CONbits.ACKEN){};
}

// 関数名   : MSSP_I2C2_SendNackProc
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_I2C2_SendNackProc(void)
{
    // ACKDTにNACKをセット(負論理なので1を設定)
	I2C2CONbits.ACKDT = 1;
    
    __delay_us(125);

    // NACK信号生成
	I2C2CONbits.ACKEN = 1;
	
    while(I2C2CONbits.ACKEN){}
}
// 関数名   : PPS_Spi1IO_Set
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void PPS_Spi1IO_Set(void)
{
    // PPS unlock
    SYSKEY = 0xAA996655;
    
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK = 0;
    
    // RD3(51番PIN)をSDI1:MISI(受信)に設定
    SDI1R   = 0b0000;
    
    // RF0(58番PIN)をSDO1:MISO(送信)に設定
    RPF0R   = 0b1000;
    
    // SCK1はRF6(35番PIN)で固定で変更不可
    
    // PPS lock
    CFGCONbits.IOLOCK = 1;
    SYSKEY = 0x00000000;

    // CS(ﾁｯﾌﾟｾﾚｸﾄ)･DC(ﾃﾞ-ﾀ/ｺﾏﾝﾄﾞ)･RST(ﾘｾｯﾄ)ﾋﾟﾝを出力設定
    SPI1_ILI9488_CS_MODE    = 0x00;
    
    SPI1_XPT2046_CS_MODE    = 0x00;
    
    SPI1_DC_MODE            = 0x00;
    
    SPI1_RST_MODE           = 0x00;
    
    // CS(ﾁｯﾌﾟｾﾚｸﾄ)･DC(ﾃﾞ-ﾀ/ｺﾏﾝﾄﾞ)･RST(ﾘｾｯﾄ)ﾋﾟﾝは出力しておく
    SPI1_ILI9488_CS         = 0x01;
    
    SPI1_XPT2046_CS         = 0x01;
    
    SPI1_DC                 = 0x01;
    
    SPI1_RST                = 0x01;
}
 
// 関数名   : MSSP_SPI1_ControlRegister_Initialize
// 機能     : MSSP SPI1ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_SPI1_ControlRegister_Initialize(void)
 {
     // SPI1CONﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
     SPI1CON = SPIx_CON_INIT_BITS;
 }
 
// 関数名   : MSSP_SPI1_StatusRegister_Initialize
// 機能     : MSSP SPI1ｽﾃ-ﾀｽﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_SPI1_StatusRegister_Initialize(void)
 {
     // SPI2STATﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
     SPI1STAT = SPIx_STA_INIT_BITS;
 }
 
 
// 関数名   : MSSP_SPI1_StatusRegister_Initialize
// 機能     : DMA1初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void SPI1_DMA1_Initialize(void)
{
    // DMAｺﾝﾄﾛ-ﾗ全体を有効化
    DMACONbits.ON = 1;
    
    DCH0CON = 0;
    DCH0ECON = 0;
    DCH0INT  = 0;

    DCH0INTbits.CHBCIE = 1;    // Block Complete 割込み有効
    
    // 優先度(3 = 高め)
    DCH0CONbits.CHPRI = 3;

    // ﾄﾘｶﾞをSPI1 TX に設定（PIC32MX370F512Hでは _SPI1_TX_IRQ = 37）
    DCH0ECONbits.CHSIRQ = _SPI1_TX_IRQ;
    
    // ﾊ-ﾄﾞｳｪｱﾄﾘｶﾞ有効
    DCH0ECONbits.SIRQEN = 1;
    
    // --- 割込み設定 ---
    IFS2CLR = _IFS2_DMA0IF_MASK;  // フラグクリア
    IEC2SET = _IEC2_DMA0IE_MASK;  // 割込み許可
    IPC10bits.DMA0IP = 5;         // 優先度5
    IPC10bits.DMA0IS = 0;         // サブ優先度0    
}

// --- 1バイト送信 ---
void DMA_SPI1_Send_Byte_Proc(uint8_t data)
{
    oneByte = data;

    DCH0CONbits.CHEN = 0;                  // チャネル停止

    DCH0SSA = KVA_TO_PA(&oneByte);         // ソース = RAM上の1バイト
    DCH0SSIZ = 1;                          // ソースサイズ = 1

    DCH0DSA = KVA_TO_PA(&SPI1BUF);         // 宛先 = SPI1BUF
    DCH0DSIZ = 1;                          // 宛先サイズ = 1

    DCH0CSIZ = 1;                          // 転送単位 = 1バイト

    DCH0INTCLR = 0x00FF;                   // 割込みフラグクリア
    DCH0CONbits.CHEN = 1;                  // チャネル有効化
    DCH0ECONbits.CFORCE = 1;               // ⭐ 強制スタートで1バイト送信

    // ﾃﾞﾊﾞｯｸ用完了待ち(ﾌﾞﾛｯｷﾝｸﾞ) 
    //while (DCH0CONbits.CHEN);
     DCH0INTCLR = _DCH0INT_CHBCIF_MASK;     // フラグクリア
}

// 関数名   : MSSP_SPI1_Baudrate_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void DMA0_to_SPI1_Data_Transmission_Proc(uint8_t *data, uint16_t size)
{
    // ﾃﾞ-ﾀﾓ-ﾄﾞ発行
    SPI1_DC     = 0x01;
    
    // LCDに余裕を与える（必要なら）
    __delay_us(100);
    
    // DMAﾁｬﾝﾈﾙ0停止
    DCH0CONbits.CHEN = 0;
    
    // DMAﾁｬﾝﾈﾙ0の転送元(ｿ-ｽ)の開始ｱﾄﾞﾚｽ指定
    DCH0SSA = KVA_TO_PA(&data[0]);
    
    // DMAﾁｬﾝﾈﾙ0の転送元(ｿ-ｽ)のﾊﾞｲﾄ数を指定
    DCH0SSIZ = size;
    
    // DMAﾁｬﾝﾈﾙ0の転送先(ﾃﾞｽﾃｨﾈ-ｼｮﾝ)の開始ｱﾄﾞﾚｽ指定(SPI1のﾊﾞｯﾌｧ)
    DCH0DSA = KVA_TO_PA(&SPI1BUF);
    
    // 転送先(ﾃﾞｽﾃｨﾈ-ｼｮﾝ)に書込むﾌﾞﾛｯｸｻｲｽﾞ(ﾊﾞｲﾄ数)を指定(SPI1のﾊﾞｯﾌｧｻｲｽﾞ)
    // 「SPI1CONbits.MODE32」と「SPI1CONbits.MODE16」は「0」なのでSPI1BUFのﾊﾞｯﾌｧｻｲｽﾞは8ﾋﾞｯﾄ(1ﾊﾞｲﾄ)
    DCH0DSIZ = 1;

    // 転送先(ﾃﾞｽﾃｨﾈ-ｼｮﾝ)への1回で扱う(転送)ﾊﾞｲﾄ数(1ﾊﾞｲﾄ単位転送)
    DCH0CSIZ = 1;
    
    // ISR割込みﾌﾗｸﾞ関連をｸﾘｱ
    DCH0INTCLR = 0x00FF;
    
    // ﾁｬﾈﾙ有効化
    DCH0CONbits.CHEN = 1;

    // 強制ｽﾀ-ﾄ
    DCH0ECONbits.CFORCE = 1;

    // ﾃﾞﾊﾞｯｸ用完了待ち(ﾌﾞﾛｯｷﾝｸﾞ) 
    while (DCH0CONbits.CHEN);
    DCH0INTCLR = _DCH0INT_CHBCIF_MASK;     // フラグクリア
}
 
// 関数名   : MSSP_SPI1_Baudrate_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI1_Baudrate_Initialize(void)
{
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:10MHzで動作)
    SPI1BRG = 80000000 / (2 * (10000000)) - 1;
    
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:5MHzで動作)
    //SPI1BRG = 80000000 / (2 * (5000000)) - 1;
}

// 関数名   : MSSP_SPI1_SendRecvDataProc
// 機能     : SPI1送受信の処理
// 引数     : 送信ﾃﾞ-ﾀ：data(受信の場合はﾀﾞﾐ-ﾃﾞ-ﾀ),受信処理切り分け：Send_Or_Recv 送信処理のみSPI_SEND(0) 受信処理を含むSPI_RECV(1)　
// 戻り値   : 
// 使用条件 :
uint8_t MSSP_SPI1_TransferData(unsigned char data)
{
    // 送信処理：送信ﾊﾞｯﾌｧ空き待ち
    while (SPI1STATbits.SPITBF){};

    // ﾃﾞ-ﾀ送信
    SPI1BUF = data;
    
    // 受信完了待ち
    while (!SPI1STATbits.SPIRBF){};
    
    // 受信ﾃﾞ-ﾀを返す
    return SPI1BUF;
}

// 関数名   : SPI1_FlushRX
// 機能     : SPI1の受信ﾊﾞｯﾌｧおよびｴﾗ-ﾌﾗｸﾞをｸﾘｱする
// 引数     : なし
// 戻り値   : なし
// 使用条件 :
//    ・SPI1ﾓｼﾞｭ-ﾙが有効化されていること
//    ・DMA送信や連続送信後に呼び出し、受信ﾊﾞｯﾌｧに溜まったﾀﾞﾐ-ﾃﾞ-ﾀを捨て、
//      SPIROV(受信ｵ-ﾊﾞ-ﾌﾛ-)をｸﾘｱして次回の転送で停止しないようにする
void SPI1_FlushRX(void)
{
    // SPIBFはReceiveBufferFull(RBF)ﾌﾗｸﾞ1なら「受信ﾊﾞｯﾌｧに未読ﾃﾞ-ﾀがある状態」
    while (SPI1STATbits.SPIRBF)
    {
        // RBFに残りがあればﾊﾞｯﾌｧの読み捨て
        (void)SPI1BUF;
    }
    
    // 受信ﾊﾞｯﾌｧｵ-ﾊﾞ-ﾌﾛ-処理
    #ifdef _SPI1STAT_SPIROV_MASK

        // SPIROVは受信ｵ-ﾊﾞ-ﾌﾛ-ﾌﾗｸﾞ「受信ﾊﾞｯﾌｧを読み出さずに新しいﾃﾞ-ﾀ入るとﾌﾗｸﾞ1になる」
        // ﾌﾗｸﾞが立つと消さない限りは新しいﾃﾞ-ﾀを受け付けなくなる
    
        // SPIROV(受信ｵ-ﾊﾞ-ﾌﾛ-ﾌﾗｸﾞ)を強制的にｸﾘｱする
        if (SPI1STATbits.SPIROV) SPI1STATCLR = _SPI1STAT_SPIROV_MASK;
    #endif

    // 送信ｺﾘｼﾞｮﾝ処理
    #ifdef _SPI1STAT_WCOL_MASK

        // WCOLは送信ﾊﾞｯﾌｧﾌﾙ(SPITBF=1)の状態で追加書込みが発生した場合、書込みは無効となりﾌﾗｸﾞ1になる
        // ﾌﾗｸﾞが立っている間は、その直前の送信が無効になっている可能性がある
        // 放置すると次の送信も正しく動作しない可能性がある
    
        // WCOL(送信ｺﾘｼﾞｮﾝﾌﾗｸﾞ)を強制的にｸﾘｱする
        if (SPI1STATbits.WCOL)   SPI1STATCLR = _SPI1STAT_WCOL_MASK;
    #endif
}

// 関数名   : SPI2_FlushRX
// 機能     : SPI2の受信ﾊﾞｯﾌｧおよびｴﾗ-ﾌﾗｸﾞをｸﾘｱする
// 引数     : なし
// 戻り値   : なし
// 使用条件 :
//    ・SPI2ﾓｼﾞｭ-ﾙが有効化されていること
//    ・DMA送信や連続送信後に呼び出し、受信ﾊﾞｯﾌｧに溜まったﾀﾞﾐ-ﾃﾞ-ﾀを捨て、
//      SPIROV(受信ｵ-ﾊﾞ-ﾌﾛ-)をｸﾘｱして次回の転送で停止しないようにする
void SPI2_FlushRX(void)
{
    // SPIBFはReceiveBufferFull(RBF)ﾌﾗｸﾞ1なら「受信ﾊﾞｯﾌｧに未読ﾃﾞ-ﾀがある状態」
    while (SPI2STATbits.SPIRBF)
    {
        // RBFに残りがあればﾊﾞｯﾌｧの読み捨て
        (void)SPI2BUF;
    }
    
    // 受信ﾊﾞｯﾌｧｵ-ﾊﾞ-ﾌﾛ-処理
    #ifdef _SPI2STAT_SPIROV_MASK

        // SPIROVは受信ｵ-ﾊﾞ-ﾌﾛ-ﾌﾗｸﾞ「受信ﾊﾞｯﾌｧを読み出さずに新しいﾃﾞ-ﾀ入るとﾌﾗｸﾞ1になる」
        // ﾌﾗｸﾞが立つと消さない限りは新しいﾃﾞ-ﾀを受け付けなくなる
    
        // SPIROV(受信ｵ-ﾊﾞ-ﾌﾛ-ﾌﾗｸﾞ)を強制的にｸﾘｱする
        if (SPI2STATbits.SPIROV) SPI2STATCLR = _SPI2STAT_SPIROV_MASK;
    #endif

    // 送信ｺﾘｼﾞｮﾝ処理
    #ifdef _SPI2STAT_WCOL_MASK

        // WCOLは送信ﾊﾞｯﾌｧﾌﾙ(SPITBF=1)の状態で追加書込みが発生した場合、書込みは無効となりﾌﾗｸﾞ1になる
        // ﾌﾗｸﾞが立っている間は、その直前の送信が無効になっている可能性がある
        // 放置すると次の送信も正しく動作しない可能性がある
    
        // WCOL(送信ｺﾘｼﾞｮﾝﾌﾗｸﾞ)を強制的にｸﾘｱする
        if (SPI2STATbits.WCOL)   SPI2STATCLR = _SPI2STAT_WCOL_MASK;
    #endif
}

// 関数名   : PPS_Spi2IO_Set
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void PPS_Spi2IO_Set(void)
{
    TRISDbits.TRISD11 = 1;
    
    // PPS unlock
    SYSKEY = 0xAA996655;
    
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK = 0;
    
    // RD11(45番PIN)をSDI2:MISI(受信)に設定
    SDI2R   = 0b0011;
    
    // RE5(1番PIN)をSDO2:MISO(送信)に設定
    RPE5R   = 0b0110;
    
    // SCK2はRF6(35番PIN)で固定で変更不可
    
    // PPS lock
    CFGCONbits.IOLOCK = 1;
    SYSKEY = 0x00000000;

    // CS(ﾁｯﾌﾟｾﾚｸﾄ)･DC(ﾃﾞ-ﾀ/ｺﾏﾝﾄﾞ)･RST(ﾘｾｯﾄ)ﾋﾟﾝを出力設定
    SPI2_SD_CARD_CS_MODE    = 0x00;
    
    SPI2_DC_MODE            = 0x00;
    
    // CS(ﾁｯﾌﾟｾﾚｸﾄ)･DC(ﾃﾞ-ﾀ/ｺﾏﾝﾄﾞ)･RST(ﾘｾｯﾄ)ﾋﾟﾝは出力しておく
    SPI2_SD_CARD_CS         = 0x01;
    
    SPI2_DC                 = 0x01;
}

// 関数名   : MSSP_SPI2_ControlRegister_Initialize
// 機能     : MSSP SPI2ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_SPI2_ControlRegister_Initialize(void)
 {
     // SPI2CONﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
     SPI2CON = SPIx_CON_INIT_BITS;
 }
 
// 関数名   : MSSP_SPI2_StatusRegister_Initialize
// 機能     : MSSP SPI2ｽﾃ-ﾀｽﾚｼﾞｽﾀの初期化
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
 void MSSP_SPI2_StatusRegister_Initialize(void)
 {
     // SPI2STATﾚｼﾞｽﾀに制御用ﾋﾞｯﾄﾌｨ-ﾙﾄﾞ設定
     SPI2STAT = SPIx_STA_INIT_BITS;
 }

// 関数名   : MSSP_SPI2_Baudrate_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void MSSP_SPI2_Baudrate_Initialize(void)
{
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:100kHzで動作)
    SPI2BRG = 80000000 / (2 * (100000)) - 1;
    
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:10MHzで動作)
    //SPI2BRG = 80000000 / (2 * (10000000)) - 1;
    
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:5MHzで動作)
    //SPI2BRG = 80000000 / (2 * (5000000)) - 1;
}
 
// 関数名   : MSSP_SPI2_SendRecvDataProc
// 機能     : SPI2送受信の処理
// 引数     : 送信ﾃﾞ-ﾀ：data(受信の場合はﾀﾞﾐ-ﾃﾞ-ﾀ),受信処理切り分け：Send_Or_Recv 送信処理のみSPI_SEND(0) 受信処理を含むSPI_RECV(1)　
// 戻り値   : 
// 使用条件 :
uint8_t MSSP_SPI2_TransferData(uint8_t tx)
{
    // 送信処理：送信ﾊﾞｯﾌｧ空き待ち
    while (SPI2STATbits.SPITBF){};

    // ﾃﾞ-ﾀ送信
    SPI2BUF = tx;
    
    // 受信完了待ち
    while (!SPI2STATbits.SPIRBF){};
    
    // 受信ﾃﾞ-ﾀを返す
    return SPI2BUF;
}

 
// 関数名   : PPS_UartIO_Set
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void PPS_UartIO_Set(void)
{
    // PPS unlock
    SYSKEY = 0xAA996655;
    
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK = 0;
    
    // RG7(5番PIN)をU1TX(送信)に設定
    RPG7R   = 0b0011;
    
    // RG8(6番PIN)をU1RX(受信)に設定
    U1RXR   = 0b0001;
    
    // PPS lock
    CFGCONbits.IOLOCK = 1;
    SYSKEY = 0x00000000;
}

// 関数名   : MSSP_Uart1ModeRegister_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void Uart1ModeRegister_Initialize(void)
{
    // UARTﾓｼﾞｭ-ﾙの電源ON/OFF制御ﾋﾞｯﾄ
    U1MODEbits.ON       = 0x01;
    
    // ｱｲﾄﾞﾙﾓ-ﾄﾞ停止ﾋﾞｯﾄ(CPUがｱｲﾄﾞﾙ状態⇒UARTは動作継続)
    U1MODEbits.SIDL     = 0x00;
    
    // IrDAｴﾝｺ-ﾀﾞおよびﾃﾞｺ-ﾀﾞ有効化ﾋﾞｯﾄ
    U1MODEbits.IREN     = 0x00;
    
    // UxRTSﾋﾟﾝﾋﾞｯﾄのﾓ-ﾄﾞ選択(RTSﾋﾟﾝはﾊ-ﾄﾞｳｪｱﾌﾛ-制御ﾓ-ﾄﾞ(自動制御))
    U1MODEbits.RTSMD    = 0x00;
    
    // UARTのﾋﾟﾝ構成を選択するﾋﾞｯﾄ(通常のTX/RXのみ)
    U1MODEbits.UEN      = 0x00;
    
    // ｽﾘ-ﾌﾟ状態からの復帰方法制御ﾋﾞｯﾄ(ｽﾀ-ﾄﾋﾞｯﾄを検出しない)
    U1MODEbits.WAKE     = 0x00;
    
    // ﾙ-ﾌﾟﾊﾞｯｸﾓ-ﾄﾞ(通常動作(送信ﾋﾟﾝTXからﾃﾞ-ﾀ出力､受信ﾋﾟﾝRXでﾃﾞ-ﾀ入力))
    U1MODEbits.LPBACK   = 0x00;
    
    // 自動ﾎﾞ-ﾚ-ﾄ検出機能を制御ﾋﾞｯﾄ(無効)
    U1MODEbits.ABAUD    = 0x00;
    
    // UART受信信号の極性(反転)を設定ﾋﾞｯﾄ(通常の受信極性)
    U1MODEbits.RXINV    = 0x00;
    
    // UARTのﾎﾞ-ﾚ-ﾄ(通信速度)生成設定するﾋﾞｯﾄ(ﾊｲｽﾋﾟ-ﾄﾞ：4分周)
    U1MODEbits.BRGH     = 0x01;
    
    // UART通信のﾃﾞ-ﾀﾋﾞｯﾄ数とﾊﾟﾘﾃｨﾋﾞｯﾄの設定(8bitﾌﾚ-ﾑの偶数ﾊﾟﾘﾃｨ(EVEN))
    U1MODEbits.PDSEL    = 0x01;
    
    // UART通信におけるｽﾄｯﾌﾟﾋﾞｯﾄ数(停止ﾋﾞｯﾄ数)設定ﾋﾞｯﾄ(2ｽﾄｯﾌﾟﾋﾞｯﾄ)
    U1MODEbits.STSEL    = 0x01;
}

// 関数名   : MSSP_Uart1StatusRegister_Initialize
// 機能     : 
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void Uart1StatusRegister_Initialize(void)
{
    /* ADM_EN と ADDEN の違い

    ADDEN はｱﾄﾞﾚｽ検出機能そのものの有効/無効(機能のON/OFF)を制御するﾋﾞｯﾄ

    ADM_EN は実際にｱﾄﾞﾚｽ検出ﾓｰﾄﾞに切り替わっている状態を制御するﾋﾞｯﾄで､割り込みや状態監視に使われる*/
    
    // ｱﾄﾞﾚｽ検出ﾓ-ﾄﾞ制御ﾋﾞｯﾄ(ﾏﾙﾁﾄﾞﾛｯﾌﾟ通信や特定ﾌﾟﾛﾄｺﾙなどで受信したﾃﾞ-ﾀがｱﾄﾞﾚｽﾌﾚ-ﾑか判断する)
    U1STAbits.ADM_EN    = 0x00;
    
    // ｱﾄﾞﾚｽ検出ﾓ-ﾄﾞでｱﾄﾞﾚｽﾌﾚ-ﾑの検出ﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.ADDR      = 0x00;
    
    // 送信割り込みの発生条件を設定するﾋﾞｯﾄ(送信ﾊﾞｯﾌｧ空き(ﾊﾞｯﾌｧに少なくとも1ﾊﾞｲﾄ空き)で割り込み発生)
    U1STAbits.UTXISEL   = 0x00;
    
    // 送信ﾃﾞ-ﾀの極性反転(TXﾋﾟﾝの信号反転)を制御ﾋﾞｯﾄ
    U1STAbits.UTXINV    = 0x00;
    
    // UARTの受信機を有効･無効にする制御ﾋﾞｯﾄ(ﾃﾞ-ﾀ受信を有効)
    U1STAbits.URXEN     = 0x01;
    
    // 送信ﾗｲﾝにﾌﾞﾚｰｸｼｸﾞﾅﾙ(BREAK)を送信するための制御ﾋﾞｯﾄ
    U1STAbits.UTXBRK    = 0x00;
    
    // UART送信機を有効･無効にする制御ﾋﾞｯﾄ(ﾃﾞ-ﾀ送信を有効)
    U1STAbits.UTXEN     = 0x01;
    
    // 送信ﾊﾞｯﾌｧの満杯状態を示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.UTXBF     = 0x00;
    
    // 送信ﾚｼﾞｽﾀが空かどうかを示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.TRMT      = 0x00;
    
    // 受信割り込み発生条件を設定するﾋﾞｯﾄ(1ﾊﾞｲﾄ受信ごとに割り込み)
    U1STAbits.URXISEL   = 0x00;
    
    // ｱﾄﾞﾚｽ検出機能の有効･無効を制御ﾋﾞｯﾄ
    U1STAbits.ADDEN     = 0x00;
    
    // 受信ﾗｲﾝのｱｲﾄﾞﾙ状態を示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.RIDLE     = 0x00;
    
    // ﾊﾟﾘﾃｨｴﾗ-を示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.PERR      = 0x00;
    
    // ﾌﾚ-ﾐﾝｸﾞｴﾗ-を示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.FERR      = 0x00;
    
    // ｵ-ﾊﾞ-ﾗﾝを示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.OERR      = 0x00;
    
    // 受信ﾊﾞｯﾌｧにﾃﾞ-ﾀが存在するかを示すﾌﾗｸﾞﾋﾞｯﾄ
    U1STAbits.URXDA     = 0x00;
}

// 関数名   : MSSP_Uart1Baudrate_Initialize
// 機能     : BRGH = 0x01(ﾊｲｽﾋﾟ-ﾄﾞ)で設定
//          : U1BRG FPBCLK(ﾍﾟﾘﾌｪﾗﾙﾊﾞｽｸﾛｯｸ)÷(4×ﾎﾞ-ﾚ-ﾄ)?1
//          : 80000000÷(4×115200)?1＝172
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void Uart1Baudrate_Initialize(void)
{
    U1BRG = (80000000 / (4 * 115200)) - 1;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_//_/_
//_/ Delay関数群
//_/ PIC32MXには遅延処理関数がないため設置
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_//_/_
void __delay_us(unsigned int d)
{
	unsigned int delayCount, startTime;       

	startTime = _CP0_GET_COUNT();
    
	delayCount = d * CCLK_US;
    
	while((_CP0_GET_COUNT() - startTime) < delayCount);  
 }

void __delay_ms(unsigned int d)
{   
	unsigned int delayCount, startTime;       

	startTime = _CP0_GET_COUNT();   
    
	delayCount = d * CCLK_MS;
    
	while((_CP0_GET_COUNT() - startTime) < delayCount);   
}
