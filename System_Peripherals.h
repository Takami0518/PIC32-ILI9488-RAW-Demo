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

#ifndef SYSTEM_PERIPHERALS_H
#define SYSTEM_PERIPHERALS_H

#include <xc.h>
#include <stdbool.h>

//==============================================================================
// I2CxCON用制御ﾋﾞｯﾄ定義(I2C1･I2C2共用)
//==============================================================================
// I2Cｲﾈ-ﾌﾞﾙﾋﾞｯﾄ
#define I2CxCON_ON      (1<<15)
// ｱｲﾄﾞﾙﾓ-ﾄﾞ停止ﾋﾞｯﾄ(CPUがｱｲﾄﾞﾙ状態⇒I2Cは動作継続)
#define I2CxCON_SIDL    (0<<13)
// SCLxﾘﾘ-ｽ制御ﾋﾞｯﾄ(I2Cｽﾚ-ﾌﾞとして動作する場合に使用)
#define I2CxCON_SCLREL  (0<<12)
// I2C予約ｱﾄﾞﾚｽ有効化ﾋﾞｯﾄ
#define I2CxCON_STRICT  (0<<11)
// 10ﾋﾞｯﾄのｽﾚ-ﾌﾞｱﾄﾞﾚｽ制御ﾋﾞｯﾄ
#define I2CxCON_A10M    (0<<10)
// ｽﾙ-ﾚ-ﾄ制御ﾋﾞｯﾄ
#define I2CxCON_DISSLW  (0<<9)
// SMBus入力ﾚﾍﾞﾙ制御ﾋﾞｯﾄ
#define I2CxCON_SMEN    (0<<8)
// 一般呼び出し有効ﾋﾞｯﾄ(I2Cｽﾚ-ﾌﾞとして動作する場合)
#define I2CxCON_GCEN    (0<<7)
// SCLxｸﾛｯｸｽﾄﾚｯﾁ有効化ﾋﾞｯﾄ(I2Cｽﾚ-ﾌﾞとして動作する場合)
#define I2CxCON_STREN   (0<<6)
// ACKﾃﾞ-ﾀﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合､ﾏｽﾀ-受信時に適用)
#define I2CxCON_SMEN    (0<<5)
// ACKｼ-ｹﾝｽ有効化ﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合､ﾏｽﾀ-受信時に適用可能)
#define I2CxCON_ACKEN   (0<<4) 
// 受信有効ﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合)
#define I2CxCON_RCEN    (0<<3) 
// ｽﾄｯﾌﾟｺﾝﾃﾞｨｼｮﾝ有効ﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合)
#define I2CxCON_PEN     (0<<2)
// 繰り返しｽﾀ-ﾄ条件有効ﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合)
#define I2CxCON_RSEN    (0<<1)
// ｽﾀ-ﾄ条件有効ﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合)
#define I2CxCON_SEN     (0<<0)

//I2CxCONﾚｼﾞｽﾀ用のﾋﾞｯﾄﾌｨ-ﾙﾄﾞ生成ﾏｸﾛ
#define I2Cx_CON_INIT_BITS  ( \
        I2CxCON_ON      \
      | I2CxCON_SIDL    \
      | I2CxCON_SCLREL  \
      | I2CxCON_STRICT  \
      | I2CxCON_A10M    \
      | I2CxCON_DISSLW  \
      | I2CxCON_SMEN    \
      | I2CxCON_GCEN    \
      | I2CxCON_STREN   \
      | I2CxCON_ACKEN   \
      | I2CxCON_RCEN    \
      | I2CxCON_PEN     \
      | I2CxCON_RSEN    \
      | I2CxCON_SEN     \
)

//==============================================================================
// I2CxSTA用制御ﾋﾞｯﾄ定義(I2C1･I2C2共用)
//==============================================================================
// 確認ｽﾃ-ﾀｽﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合､ﾏｽﾀ-送信操作に適用可能)
#define I2CxSTA_ACKSTAT     (0<<15)   
// 送信ｽﾃ-ﾀｽﾋﾞｯﾄ(I2Cﾏｽﾀ-として動作している場合､ﾏｽﾀ-送信操作に適用可能)
#define I2CxSTA_TRSTAT      (0<<14)
// ﾏｽﾀ-ﾊﾞｽ衝突検出ﾋﾞｯﾄ
#define I2CxSTA_BCL         (0<<10)
// 一般呼び出しｽﾃ-ﾀｽﾋﾞｯﾄ
#define I2CxSTA_GCSTAT      (0<<9)
// 10ﾋﾞｯﾄのｱﾄﾞﾚｽｽﾃ-ﾀｽﾋﾞｯﾄ
#define I2CxSTA_ADD10       (0<<8)
// 書き込み衝突検出ﾋﾞｯﾄ
#define I2CxSTA_IWCOL       (0<<7)
// 受信ｵ-ﾊﾞ-ﾌﾛ-ﾌﾗｸﾞﾋﾞｯﾄ
#define I2CxSTA_I2COV       (0<<6)
// ﾃﾞ-ﾀ/ｱﾄﾞﾚｽﾋﾞｯﾄ(I2Cｽﾚ-ﾌﾞとして動作している場合)
#define I2CxSTA_D_A         (0<<5)
// I2Cｽﾄｯﾌﾟﾋﾞｯﾄ
#define I2CxSTA_P           (0<<4)
// I2Cｽﾀ-ﾄﾋﾞｯﾄ
#define I2CxSTA_S           (0<<3)
// 読み取り/書き込み情報ﾋﾞｯﾄ(I2Cｽﾚ-ﾌﾞとして動作している場合)
#define I2CxSTA_R_W         (0<<2)
// 受信ﾊﾞｯﾌｧﾌﾙｽﾃｰﾀｽﾋﾞｯﾄ
#define I2CxSTA_RBF         (0<<1)
// 送信ﾊﾞｯﾌｧﾌﾙｽﾃ-ﾀｽﾋﾞｯﾄ
#define I2CxSTA_TBF         (0<<0)

// I2CxSTAﾚｼﾞｽﾀ用のﾋﾞｯﾄﾌｨ-ﾙﾄﾞ生成ﾏｸﾛ
#define I2Cx_STA_INIT_BITS (\
        I2CxSTA_ACKSTAT \
      | I2CxSTA_TRSTAT  \
      | I2CxSTA_BCL     \
      | I2CxSTA_GCSTAT  \
      | I2CxSTA_ADD10   \
      | I2CxSTA_IWCOL   \
      | I2CxSTA_I2COV   \
      | I2CxSTA_D_A     \
      | I2CxSTA_P       \
      | I2CxSTA_S       \
      | I2CxSTA_R_W     \
      | I2CxSTA_RBF     \
      | I2CxSTA_TBF     \
)

//==============================================================================
// SPIxCON用制御ﾋﾞｯﾄ定義(SPI1･SPI2用)
//==============================================================================
// SPIﾌﾚ-ﾑﾓ-ﾄﾞを有効にするためのﾋﾞｯﾄ(ﾌﾚ-ﾑﾓ-ﾄﾞ不使用)
#define SSPxCON_FRMEN       (0<<31)
// SPIのﾌﾚ-ﾑ同期信号(FSYNC)をﾏｽﾀ-が出力するかｽﾚｰﾌﾞが入力するかの設定(ﾏｽﾀ-としてFSYNCを出力)
#define SPIxCON_FRMSYNC     (0<<30)
// ﾌﾚ-ﾑ同期信号(FSYNC)の極性を設定するﾋﾞｯﾄ(ﾌﾚ-ﾑﾓ-ﾄﾞ有効の時に使用)
#define SPIxCON_FRMPOL      (0<<29)
// SPIﾏｽﾀ-･ｽﾚ-ﾌﾞ選択信号の自動制御機能制御ﾋﾞｯﾄ(CSは手動制御)
#define SPIxCON_MSSEN       (0<<28)
// ﾌﾚ-ﾑ同期ﾊﾟﾙｽ幅ﾋﾞｯﾄ制御ﾋﾞｯﾄ(ﾌﾚ-ﾑ同期ﾊﾟﾙｽは1ｸﾛｯｸ幅)
#define SPIxCON_FRMSYPW     (0<<27)
// ﾌﾚｰﾑ同期ﾊﾟﾙｽのｶｳﾝﾄ数を設定するﾋﾞｯﾄﾌｨｰﾙﾄﾞ制御ﾋﾞｯﾄ(1文字ごとにﾌﾚ-ﾑ同期ﾊﾟﾙｽを出す)
#define SPIxCON_FRMCNT      (0<<24)
// SPIｸﾛｯｸの供給元を選択する制御ﾋﾞｯﾄ(FRMEN=1の時のみ有効)
#define SPIxCON_MCLKSEL     (0<<23)    
// SPIﾌﾚｰﾑの終わりを示すﾌﾗｸﾞﾋﾞｯﾄ
#define SPIxCON_SPIFE       (0<<17)
// EnhancedBufferﾓ-ﾄﾞの有効/無効を制御するﾋﾞｯﾄ(標準ﾊﾞｯﾌｧﾓ-ﾄﾞ(Tx/Rxﾊﾞｯﾌｧは1ﾜ-ﾄﾞのみ)
#define SPIxCON_ENHBUF      (0<<16)
// SPIﾓｼﾞｭ-ﾙ1(SPI1)の動作開始･停止を制御する制御ﾋﾞｯﾄ
#define SPIxCON_ON          (1<<15)
// SPI1ﾓｼﾞｭ-ﾙの動作停止条件を制御する制御ﾋﾞｯﾄ(CPUがｽﾘ-ﾌﾟした際SPI1は停止しない)
#define SPIxCON_SIDL        (0<<13)
// SPI1ﾓｼﾞｭ-ﾙにおけるSDO(Serial Data Out)ﾋﾟﾝの制御ﾋﾞｯﾄ(SDOﾋﾟﾝを通常通り使用(ﾃﾞ-ﾀ出力有効)
#define SPIxCON_DISSDO      (0<<12)
// SPI通信で送受信するﾃﾞ-ﾀのﾋﾞｯﾄ幅を32ﾋﾞｯﾄか16ﾋﾞｯﾄを選択する制御ﾋﾞｯﾄ
#define SPIxCON_MODE32      (0<<11)
#define SPIxCON_MODE16      (0<<10)
//SPI通信でMISOﾗｲﾝのﾃﾞ-ﾀをｸﾛｯｸのどのﾀｲﾐﾝｸﾞで読み取るかを設定
#define SPIxCON_SMP         (0<<9)
// SPIｸﾛｯｸ(SCK)設定、ｸﾛｯｸｴｯｼﾞの組み合わせ (CKPとｾｯﾄ)で決まる
#define SPIxCON_CKE         (1<<8)
// SPIのｽﾚ-ﾌﾞｾﾚｸﾄ(SS)ﾋﾟﾝをﾊｰﾄﾞｳｪｱが自動的に制御するか設定
#define SPIxCON_SSEN        (0<<7)
// SPIｸﾛｯｸ信号(SCK)のｱｲﾄﾞﾙ時の電圧ﾚﾍﾞﾙ(極性)を設定
#define SPIxCON_CKP         (0<<6)
// SPIﾓｼﾞｭ-ﾙをﾏｽﾀ-として動作させるかｽﾚ-ﾌﾞとして動作させるかを設定(ﾏｽﾀ-設定)
#define SPIxCON_MSTEN       (1<<5)
// SDIﾋﾟﾝの入力を無効にしてﾋﾟﾝを一般I/Oﾋﾟﾝとして使うか､入力を無効化を設定
#define SPIxCON_DISSDI      (0<<4)
// SPIの送信割り込みが発生するﾀｲﾐﾝｸﾞ(条件)を設定
#define SPIxCON_STXISEL     (0<<2)
// SPIの受信割り込みが発生するﾀｲﾐﾝｸﾞ(条件)を設定
#define SPIxCON_SRXISEL     (0<<0)

// Mode0    :CPOL0/CPHA0    CKP=0/CKE=1
// Mode1    :CPOL0/CPHA1    CKP=0/CKE=0
// Mode2    :CPOL1/CPHA0    CKP=1/CKE=1
// Mode3    :CPOL1/CPHA1    CKP=1/CKE=0

// SPIxCONﾚｼﾞｽﾀ用のﾋﾞｯﾄﾌｨ-ﾙﾄﾞ生成ﾏｸﾛ
#define SPIx_CON_INIT_BITS  ( \
        SSPxCON_FRMEN   \
      | SPIxCON_FRMSYNC \
      | SPIxCON_FRMPOL  \
      | SPIxCON_MSSEN   \
      | SPIxCON_FRMSYPW \
      | SPIxCON_FRMCNT  \
      | SPIxCON_MCLKSEL \
      | SPIxCON_ENHBUF  \
      | SPIxCON_ON      \
      | SPIxCON_SIDL    \
      | SPIxCON_DISSDO  \
      | SPIxCON_MODE32  \
      | SPIxCON_MODE16  \
      | SPIxCON_SMP     \
      | SPIxCON_CKE     \
      | SPIxCON_SSEN    \
      | SPIxCON_CKP     \
      | SPIxCON_MSTEN   \
      | SPIxCON_DISSDI  \
      | SPIxCON_STXISEL \
      | SPIxCON_SRXISEL \
)

//==============================================================================
// SPIxSTA用制御ﾋﾞｯﾄ定義(SPI1･SPI2共用)
//==============================================================================

// SPI受信ﾊﾞｯﾌｧの要素数(受信ﾊﾞｯﾌｧ内にあるﾃﾞ-ﾀの数)を示すﾌｨ-ﾙﾄﾞﾋﾞｯﾄ
// EnhancedBufferﾓ-ﾄﾞ(ENHBUF = 1)が有効な場合にのみ使えるｶｳﾝﾀ
#define SPIxSTAT_RXBUFELM   (0<<28)

// 送信ﾊﾞｯﾌｧ(TX FIFO)に現在何個のﾃﾞ-ﾀが入っているかを示すﾌｨ-ﾙﾄﾞﾋﾞｯﾄ
// EnhancedBufferﾓ-ﾄﾞ(ENHBUF=1)が有効な場合にのみ使えるｶｳﾝﾀ
#define SPIxSTAT_TXBUFELM   (0<<20)

// SPI通信におけるﾌﾚ-ﾑｴﾗ-を検出したことを示すﾋﾞｯﾄ
#define SPIxSTAT_FRMERR     (0<<12)

// SPIﾓｼﾞｭ-ﾙが現在通信中(転送中)かどうかを示すｽﾃ-ﾀｽﾋﾞｯﾄ
#define SPIxSTAT_SPIBUSY    (0<<11)

// 送信ｱﾝﾀﾞ-ﾗﾝｴﾗ-を示すﾋﾞｯﾄ
// EnhancedBufferﾓ-ﾄﾞ(ENHBUF=1)かつﾌﾚ-ﾑﾓ-ﾄﾞ(FRMEN=1)使用時に発動
#define SPIxSTAT_SPITUR     (0<<8)

// ShiftRegisterEmpty(ｼﾌﾄﾚｼﾞｽﾀが空)を示すﾋﾞｯﾄ
#define SPIxSTAT_SRMT       (0<<7)

// ReceiveOverflowError(受信ｵ-ﾊﾞ-ﾌﾛｰｴﾗ-)を示すﾋﾞｯﾄ
#define SPIxSTAT_SPIROV     (0<<6)

// SPI ReceiveBufferEmpty(受信ﾊﾞｯﾌｧが空)を示すｽﾃ-ﾀｽﾋﾞｯﾄ
#define SPIxSTAT_SPIRBE     (0<<5)

// SPI TransmitBufferEmpty(送信ﾊﾞｯﾌｧ空)を示すｽﾃ-ﾀｽﾋﾞｯﾄ
#define SPIxSTAT_SPITBE     (0<<3)

// SPI TransmitBufferFull(送信ﾊﾞｯﾌｧﾌﾙ)を示すﾌﾗｸﾞﾋﾞｯﾄ
#define SPIxSTAT_SPITBF     (0<<1)

// SPI ReceiveBufferFull(受信ﾊﾞｯﾌｧﾌﾙ)を示すﾌﾗｸﾞﾋﾞｯﾄ
#define SPIxSTAT_SPIRBF     (0<<0)

// SPIxSTAﾚｼﾞｽﾀ用のﾋﾞｯﾄﾌｨ-ﾙﾄﾞ生成ﾏｸﾛ
#define SPIx_STA_INIT_BITS (\
        SPIxSTAT_RXBUFELM   \
      | SPIxSTAT_TXBUFELM   \
      | SPIxSTAT_FRMERR     \
      | SPIxSTAT_SPIBUSY    \
      | SPIxSTAT_SPITUR     \
      | SPIxSTAT_SRMT       \
      | SPIxSTAT_SPIROV     \
      | SPIxSTAT_SPIRBE     \
      | SPIxSTAT_SPITBE     \
      | SPIxSTAT_SPITBF     \
      | SPIxSTAT_SPIRBF     \
)

// ILI9488(SPI)用CS(ﾁｯﾌﾟｾﾚｸﾄ) 54番PIN
#define SPI1_ILI9488_CS_MODE        TRISDbits.TRISD6

// GT20L16J1Y(SPI)用CS(ﾁｯﾌﾟｾﾚｸﾄ) 55番PIN
#define SPI1_XPT2046_CS_MODE        TRISDbits.TRISD7

// SPI用DC(ﾃﾞ-ﾀ/ｺﾏﾝﾄﾞ) 52番PIN
#define SPI1_DC_MODE                TRISDbits.TRISD4

// SPI用RST(ﾘｾｯﾄ) 53番PIN
#define SPI1_RST_MODE               TRISDbits.TRISD5

// SDｶ-ﾄﾞ(SPI)用CS(ﾁｯﾌﾟｾﾚｸﾄ) 2番PIN
#define SPI2_SD_CARD_CS_MODE        TRISEbits.TRISE6

// SDｶ-ﾄﾞ(SPI)用CS(ﾁｯﾌﾟｾﾚｸﾄ) 3番PIN
#define SPI2_DC_MODE                TRISEbits.TRISE7 

// ILI9488用CSﾋﾟﾝ
#define SPI1_ILI9488_CS             LATDbits.LATD6

// GT20L16J1Y用CSﾋﾟﾝ
#define SPI1_XPT2046_CS             LATDbits.LATD7

// SPI1用DCﾋﾟﾝ
#define SPI1_DC                     LATDbits.LATD4

// RSTﾋﾟﾝ
#define SPI1_RST                    LATDbits.LATD5

// SDｶ-ﾄﾞ用CSﾋﾟﾝ
#define SPI2_SD_CARD_CS             LATEbits.LATE6

// SPI2用DCﾋﾟﾝ
#define SPI2_DC                     LATEbits.LATE7


    // ﾃﾞｨﾚｲ関数用
    #define CCLK            (80000000L)		// ｼｽﾃﾑｸﾛｯｸ

    #define PBCLK           (CCLK / 1)		// ﾍﾟﾘﾌｪﾗﾙﾊﾞｽｸﾛｯｸ

    #define SAMPLE_RATE     5000

    #define CCLK_US         (PBCLK / 1000 / 1000)

    #define CCLK_MS         (PBCLK / 1000)


//==============================================================================
// Main_Coreで使用する関数ﾌﾟﾛﾄﾀｲﾌﾟ宣言(関数に対する引数・戻り値を宣言)
//==============================================================================

void CPU_Initialize(void);

void PPS_External_Interrupt_Setup(void);

void External_Interrupt_Initialize(void);

void Timer1_ExternalClk_Initialize(void);

void MSSP_I2C1_ControlRegister_Initialize(void);

void MSSP_I2C1_StatasRegister_Initialize(void);

void MSSP_I2C1_Baudrate_Initialize(void);

bool MSSP_I2C1_Start(void);

bool MSSP_I2C1_Stop(void);

bool MSSP_I2C1_RepeatStart(void);

bool MSSP_I2C1_SendDataProc(unsigned char dat);

void MSSP_I2C1_GetDataProc(unsigned char *get_data);

bool MSSP_I2C1_CheckAckProc(void);

void MSSP_I2C1_SendAckProc(void);

void MSSP_I2C1_SendNackProc(void);


void MSSP_I2C2_ControlRegister_Initialize(void);

void MSSP_I2C2_StatasRegister_Initialize(void);

void MSSP_I2C2_Baudrate_Initialize(void);

bool MSSP_I2C2_Start(void);

bool MSSP_I2C2_Stop(void);

bool MSSP_I2C2_RepeatStart(void);

bool MSSP_I2C2_SendDataProc(unsigned char dat);

void MSSP_I2C2_GetDataProc(unsigned char *get_data);

bool MSSP_I2C2_CheckAckProc(void);

void MSSP_I2C2_SendAckProc(void);

void MSSP_I2C2_SendNackProc(void);

void PPS_Spi1IO_Set(void); 

void MSSP_SPI1_ControlRegister_Initialize(void);

void MSSP_SPI1_StatusRegister_Initialize(void);

void MSSP_SPI1_Baudrate_Initialize(void);

uint8_t MSSP_SPI1_TransferData(unsigned char data);
    
void SPI1_FlushRX(void);

void SPI2_FlushRX(void);

void SPI1_DMA1_Initialize(void);

void DMA_SPI1_Send_Byte_Proc(uint8_t data);

void DMA0_to_SPI1_Data_Transmission_Proc(uint8_t *data, uint16_t size);

    
void PPS_Spi2IO_Set(void);    

void MSSP_SPI2_ControlRegister_Initialize(void);

void MSSP_SPI2_StatusRegister_Initialize(void);

void MSSP_SPI2_Baudrate_Initialize(void);

uint8_t MSSP_SPI2_TransferData(uint8_t tx);

void SPI2_FlushRX(void);


void PPS_UartIO_Set(void);

void Uart1ModeRegister_Initialize(void);

void Uart1StatusRegister_Initialize(void);

void Uart1Baudrate_Initialize(void);

void __delay_us(unsigned int d);

void __delay_ms(unsigned int d);

extern uint8_t spiTxBuffer[64];   // 外部参照宣言

extern uint8_t oneByte;


extern uint8_t Color_Packet_One_Line[1440];

#endif