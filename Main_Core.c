//==============================================================================
// File Name            : CoreMain.c
// Creator              : 高見 謙介
// Created Date         : 2025年8月6日
// CPU                  : PIC32MX370F512H
// Compiler             : MPLAB XC32 v4.45
// Overview             : 熱式風速計(I2C)+温度計測器(I2C)=UART通信にてPC(VC#)にて受信
// Operating Frequency  : 80Mhz
// Construction Rules   : 変数の命名は型ﾌﾟﾚﾌｨｯｸｽ+ｽﾈ-ｸｹ-ｽを用いる(例:uint8_t u8_adc_result;)
//                      : 宣言はextern記憶ｸﾗｽ指定子などで実体はここになく別にある事を示す(extern int counter)
//                      : 定義は実体を作りﾒﾓﾘ確保する(int counter)　変数の定義は宣言でもあるため省略、定義時には初期化を行う事
//                      : ｲﾝｽﾀﾝｽは型(構造体)から実際に生成された具体物を示す(RTC_Time now)
//                      : 関数の命名はｷｬﾒﾙｹ-ｽ+ｽﾈ-ｸｹ-ｽを用いる(ILI9341_DrawPixel)
// 履歴         :
//    [2025/07/18] 初版作成
//    [2025/07/20] 動作ｸﾛｯｸ設計(CPUの最大動作周波数は50MHz(32MIPS)で構築)
//==============================================================================
// ﾋﾟﾝ対応表(I2C1/I2C2/SPI1/UART1関連)
//==============================================================================
// SCL1             : 44番PIN(RD10)固定
// SDA1             : 43番PIN(RD9)固定
// SCL2             : 32番PIN(RF5)固定
// SDA2             : 31番PIN(RF4)固定

// SDI1(MISI)       : 51番PIN(RD3)PPS設定
// SDO1(MISO)       : 58番PIN(RF0)PPS設定
// SDI2(MISI)       : 45番PIN(RD11)PPS設定
// SDO2(MISO)       : 1番PIN(RE5)PPS設定
// SCK1             : 35番PIN(RF6)固定
// SCK2             : 4番PIN(RG6)固定
// ILI9488_CS       : 54番PIN(RD6)ﾏｸﾛ定義 ChipSelect:TFT表示器選択)
// XPT2046_CS       : 55番PIN(RD7)ﾏｸﾛ定義 ChipSelect:ﾀｯﾁｺﾝﾄﾛ-ﾗ選択)
// SPI2_SD_CARD_CS  : 2番PIN(RE6)ﾏｸﾛ定義
// SPI1_DC          : 52番PIN(RD4)ﾏｸﾛ定義 Data/Command (LOW=ｺﾏﾝﾄﾞ/HIGH=ﾃﾞ-ﾀ)
// SPI2_DC          : 3番PIN(RE7)ﾏｸﾛ定義
// SPI1_RST         : 53番PIN(RD5)ﾏｸﾛ定義 Reset(LOW=ﾘｾｯﾄ/HIGH=ｽﾀ-ﾄ)

// U1TX(UART1送信)  : 5番PIN(RG7)PPS設定
// U1RX(UART1受信)  : 6番PIN(RG8)PPS設定

// INT1(外部割込み) ： 17番PIN(RB6)PPS設定 RTC8564NBからの1Hz割込み入力
// INT2(外部割込み) ： 43番PIN(RD9)PPS設定 XPT2048からのIRQ信号をﾀｯﾁ割込みとして入力

//==============================================================================

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// DEVICE CONFIGURATION WORD 3(DEVCFG3ﾚｼﾞｽﾀ)
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#pragma config USERID       = 0xFFFF        // 接続確認用の任意ID(書き込み時のみ使用)

#pragma config FSRSSEL      = PRIORITY_0    // 割り込み処理時に使用するｼｬﾄﾞｳﾚｼﾞｽﾀｾｯﾄを使用しない

#pragma config PMDL1WAY     = OFF           // PMDﾚｼﾞｽﾀの再設定(ﾘｺﾝﾌｨｸﾞ)を許可する

#pragma config IOL1WAY      = OFF           // PPS機能の再設定を許可する

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// DEVICE CONFIGURATION WORD 2(DEVCFG2ﾚｼﾞｽﾀ)
// ﾌﾟﾗｲﾏﾘｵｼﾚ-ﾀ(Posc)は20MHz水晶発振器を使用
// SYSCLK = (外部ｸﾛｯｸ ÷ FPLLIDIV) X FPLLMUL ÷ FPLLODIV
// 20MHz  / 4(FPLLDIV)  = 5MHz
// 10MHz  X 16(FPLLMUL) = 80MHz
// 200MHz X 1(FPLLODIV) = 80Mhz(SYSCLK)
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#pragma config FPLLIDIV     = DIV_4     // SYSCLK PLL入力分周器(4倍分周)

#pragma config FPLLMUL      = MUL_16    // SYSCLK PLL入力逓倍器(16倍逓倍)

#pragma config FPLLODIV     = DIV_1     // SYSCLK PLL出力分周器(1倍分周)

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// DEVICE CONFIGURATION WORD 1(DEVCFG1ﾚｼﾞｽﾀ)
// ｾｶﾝﾀﾞﾘｵｼﾚ-ﾀ(Sosc)は使用しない、ﾍﾟﾘﾌｪﾗﾙﾊﾞｽｸﾛｯｸは80MHzを使用
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#pragma config FNOSC        = PRIPLL        // 発振器選択ﾋﾞｯﾄ(ﾌﾟﾗｲﾏﾘ発振器(PLL付き)(XT+HS+EC+PLL))

#pragma config FSOSCEN      = OFF           // ｾｶﾝﾀﾞﾘｵｼﾚｰﾀの無効

#pragma config IESO         = OFF           // 二段階ｸﾛｯｸ:起動時は内部⇒安定後に外部へ自動切替

#pragma config POSCMOD      = HS            // 20MHz外部水晶発振子を使用 ﾌﾟﾗｲﾏﾘ発振器の設定(HS発振ﾓｰﾄﾞ)

#pragma config OSCIOFNC     = OFF           // OSCOピンでのCLKO出力は無効

#pragma config FPBDIV       = DIV_1         // ﾍﾟﾘﾌｪﾗﾙﾊﾞｽｸﾛｯｸ分周器(PBCLK = SYSCLK / 1)

#pragma config FCKSM        = CSECMD        // ﾌｪ-ﾙｾ-ﾌｸﾛｯｸﾓﾆﾀ無効(ｸﾛｯｸ異常時にFRCへ自動切替し動作を継続)

#pragma config WDTPS        = PS1048576     // Watchdog Timer Postscaler (1:1048576)

#pragma config WINDIS       = OFF           // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)

#pragma config FWDTEN       = OFF           // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

#pragma config FWDTWINSZ    = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// DEVICE CONFIGURATION WORD 0(DEVCFG0ﾚｼﾞｽﾀ設定)
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)

#pragma config ICESEL = ICS_PGx3        // ICE/ICD Comm Channel Select (Communicate on PGEC3/PGED3)

#pragma config PWP = OFF                // Program Flash Write Protect (Disable)

#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)

#pragma config CP = OFF                 // Code Protect (Protection Disabled)


#include <xc.h>

//割込みﾍﾞｸﾀ・割込み関連ﾏｸﾛ(__ISRなど)を定義
#include <sys/attribs.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "System_Structs.h"
#include "System_Peripherals.h"
#include "System_Calculations.h"
#include "System_GraphPlot.h"
#include "System_Font.h"
#include "ADC_MCP3425_Driver.h"
#include "GLCD_ILI9488_Driver.h"
#include "RTC_RTC8564NB_Driver.h"
#include "TPM_XPT2046_Driver.h"
#include "ff.h"
#include "diskio.h"
#include"FatFs/../SD_Driver.h"


int i = 0;

uint16_t old_x = 0;

uint16_t old_y = 0;

uint16_t new_x = 0;

uint16_t new_y = 0;

uint8_t data = 0;

uint8_t spiTxBuffer[64];   // 送信用バッファ（グローバル）

uint8_t oneByte;    // DMA転送用バッファ（RAM上必須）

uint8_t dma0_done;

// ﾀｯﾁﾊﾟﾈﾙ押下の際のﾌﾗｸﾞ
volatile uint8_t u8_xpt_irq_flag = 0;

// 押下時間ｶｳﾝﾄ用
volatile uint32_t u32_ms_ticks = 0;

// ﾊﾞｯｸｸﾞﾗﾝﾄﾞ配色用 
uint8_t Color_Packet_One_Line[1440];


#define LINE_BYTES   (480 * 3)        // 1ライン=1440byte
#define LINES_PER_READ 3
#define READ_CHUNK  (LINE_BYTES * LINES_PER_READ)

uint8_t LineBuf[READ_CHUNK];


uint8_t card_type;

//BYTE buffer[512];

FATFS FatFs;   // ワークエリア(必須)

FRESULT res;      // FatFs関数の戻り値(結果ｺ-ﾄﾞ)
FIL fil;          // ﾌｧｲﾙｵﾌﾞｼﾞｪｸﾄ構造体
UINT br;          // 実際に読み込んだﾊﾞｲﾄ数
char buf[1024];   // 読み込みﾊﾞｯﾌｧ

        
 // 風速ﾃﾞ-ﾀ格納用構造体を定義(ｸﾞﾛ-ﾊﾞﾙ変数内定義なのでﾃﾞ-ﾀｾｸﾞﾒﾝﾄ常駐)
 ADC_MCP3425_DataStruct StructAdcMcp3425DatasCH1 =
 {
    .u8_upper_binary_data       = 0,
    
    .u8_lower_binary_data       = 0,
    
    .u16_adc_binary_data        = 0,
    
    .f_voltage_data             = 0,
    
    .c_voltage_str_data         = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    
    .c_adc_binary_str_data      = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    
    .c_result_windspeed_str_data  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00}
 };

// 温度ﾃﾞ-ﾀ格納用構造体を定義(ｸﾞﾛ-ﾊﾞﾙ変数内定義なのでﾃﾞ-ﾀｾｸﾞﾒﾝﾄ常駐)
ADC_MCP3425_DataStruct StructAdcMcp3425DatasCH2 =
 {
    .u8_upper_binary_data       = 0,
    
    .u8_lower_binary_data       = 0,
    
    .u16_adc_binary_data        = 0,
    
    .f_voltage_data             = 0,
    
    .c_voltage_str_data         = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    
    .c_adc_binary_str_data      = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    
    .c_result_windspeed_str_data  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00}
 };
  
TPM_XPT2046_DataStruct SrructTpmXpt2046Datas = 
{
    // ﾀｯﾁﾊﾟﾈﾙ押下判定
    .f_valid_press = 0,
    
    // X軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
    .u16_x_axis_datas = {0x00,0x00,0x00},

    // X軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
    .u16_median_x_axis_data = 0,
    
    // Y軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
    .u16_y_axis_datas = {0x00,0x00,0x00},
 
    // Y軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
    .u16_median_y_axis_data = 0,
            
    // Z1軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
    .u16_z1_axis_datas = {0x00,0x00,0x00},

    // Z1軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
    .u16_median_z1_axis_data = 0,

    // Z2軸測定値(3回分の生ﾃﾞｰﾀを格納する配列)
    .u16_z2_axis_datas = {0x00,0x00,0x00},

    // Z2軸測定値の中央値(ﾉｲｽﾞ低減・安定化のため中央値を算出して格納)
    .u16_median_z2_axis_data = 0,
    
    // Z1軸中央値-Z2軸中央値からのﾀｯﾁﾊﾟﾈﾙ押下圧力値
    .u16_z_azis_pressure_data = 0
            
};
  

// 関数名   : ExtInt1Handler
// 機能     : RTC8564NB(2番PIN)から1Hzｿ-ｽｸﾛｯｸを基準に1msec毎にTimer1割込みを動作させる
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void __ISR(_EXTERNAL_1_VECTOR, IPL3SOFT) ExtInt1Handler(void)
{
    StructRtcRtc8564nbDatas.RTC8564_Mseconds_Counter = 0;
            
    // 割込みﾌﾗｸﾞｸﾘｱ
    IFS0bits.INT1IF = 0;

    // 割込み時の処理(Timer1開始)
    T1CONbits.TON = 1;
}

// 関数名   : External2_Handler
// 機能     : ILI9341(XPT2048)からのﾀｯﾁﾊﾟﾈﾙ信号(IRQ)から割込みを発生させる
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void __ISR(_EXTERNAL_2_VECTOR, IPL4SOFT) External2_Handler(void)
{
    // 割込みﾌﾗｸﾞｸﾘｱ
    IFS0bits.INT2IF = 0;
    
    // ﾀｯﾁﾊﾟﾈﾙ押下ﾌﾗｸﾞｱｯﾌﾟ
    u8_xpt_irq_flag = 1;
    
    // ﾃｽﾄX座標取得
    //XPT2046_Axis_Data_Get();
}

// 関数名   : Timer1Handler
// 機能     : 1msec毎に割込み発生しにmsecｶｳﾝﾀをｲﾝｸﾘﾒﾝﾄする
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void __ISR(_TIMER_1_VECTOR, IPL3SOFT) Timer1Handler(void)
{
    //Timer1割込みﾌﾗｸﾞｸﾘｱ
    IFS0bits.T1IF = 0;
    
    //ﾐﾘ秒ｶｳﾝﾄ
    StructRtcRtc8564nbDatas.RTC8564_Mseconds_Counter++;
    
    // ﾀｯﾁﾊﾟﾈﾙ押下時間ｶｳﾝﾄ
    u32_ms_ticks++;
}

// 関数名   : DMA0Handler
// 機能     : DMA0でSPI1へﾃﾞ-ﾀ送信が完了すれば完了ﾌﾗｸﾞを立てる
// 引数     : なし
// 戻り値   : なし
// 使用条件 : 
void __ISR(_DMA0_VECTOR, IPL5SOFT) DMA0Handler(void)
{
    IFS2CLR = _IFS2_DMA0IF_MASK;  // フラグクリア

    // 転送完了処理（例: フラグを立てる）
    dma0_done = 1;
}
    
void main(void)
{
    //==========================================================================
    // CPU初期化
    //==========================================================================
    CPU_Initialize();
    
    //==========================================================================
    // 外部割込機能初期化
    //==========================================================================
    
    // INT1外部割込み機能初期化(RTC8564NBからの1Hz入力割り込み)
    External_Interrupt_Initialize();
    
    // INT2外部割込み機能初期化(IRQ入力割込み)
    External_Interrupt2_Initialize();
    
    //==========================================================================
    // ﾀｲﾏ-割込機能初期化
    //==========================================================================
    // Timer1割込み機能初期化
    Timer1_ExternalClk_Initialize();
    
    //==========================================================================
    // I2C機能初期化
    //==========================================================================
    
    // I2C1ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ初期化
    MSSP_I2C1_ControlRegister_Initialize();
    
    // I2C1ｽﾃ-ﾀｽﾚｼﾞｽﾀ初期化
    MSSP_I2C1_StatasRegister_Initialize();
    
    // I2C1ﾎﾞ-ﾚ-ﾄ初期化設定
    MSSP_I2C1_Baudrate_Initialize();

    // I2C2ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ初期化
    MSSP_I2C2_ControlRegister_Initialize();
    
    // I2C2ｽﾃ-ﾀｽﾚｼﾞｽﾀ初期化
    MSSP_I2C2_StatasRegister_Initialize();
    
    // I2C2ﾎﾞ-ﾚ-ﾄ初期化設定
    MSSP_I2C2_Baudrate_Initialize();
    
    //==========================================================================
    // SPI機能初期化
    //==========================================================================
    
    // SPI1通信用ﾎﾟ-ﾄを設定
    PPS_Spi1IO_Set();
        
    // SPI1ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ初期化
    MSSP_SPI1_ControlRegister_Initialize();
    
    // SPI1ｽﾃ-ﾀｽﾚｼﾞｽﾀ初期化
    MSSP_SPI1_StatusRegister_Initialize();
    
    // SPI1ﾎﾞ-ﾚ-ﾄ初期化設定
    MSSP_SPI1_Baudrate_Initialize();
    
    // SPI1用DMA初期化設定
    SPI1_DMA1_Initialize();
    
    // SPI2通信用ﾎﾟ-ﾄを設定
    PPS_Spi2IO_Set();
    
    // SPI2ｺﾝﾄﾛ-ﾙﾚｼﾞｽﾀ初期化
    MSSP_SPI2_ControlRegister_Initialize();
    
    // SPI2ｽﾃ-ﾀｽﾚｼﾞｽﾀ初期化
    MSSP_SPI2_StatusRegister_Initialize();
    
    // SPI2ﾎﾞ-ﾚ-ﾄ初期化設定
    MSSP_SPI2_Baudrate_Initialize();
    
    //==========================================================================
    // UART機能初期化
    //==========================================================================    
    
    // UART通信用ﾎﾟ-ﾄを設定
    PPS_UartIO_Set();
    
    // UARTﾎﾞ-ﾚ-ﾄ初期化設定
    Uart1Baudrate_Initialize();
    
    // UARTﾓ-ﾄﾞﾚｼﾞｽﾀ初期化
    Uart1ModeRegister_Initialize();
    
    // UARTｽﾃ-ﾀｽﾚｼﾞｽﾀ初期化
    Uart1StatusRegister_Initialize();
    
    //==========================================================================
    // 各ﾃﾞﾊﾞｲｽ初期化
    //==========================================================================    

    // ﾃﾞﾊﾞｯｸﾞ用LCD初期化
    MSSP_Lcd_Initialize();
    
    // ADC(ch1) MCP3425初期化
    MSSP_I2C1_MCP3425_Initialize();
    
    // ADC(ch2) MCP3425初期化
    MSSP_I2C2_MCP3425_Initialize();
    
    // ILI9488初期化(最小限の初期化設定)
    MSSP_SPI1_ILI9488_Initialize();
    
    // SDｶ-ﾄﾞを初期化(ﾏｳﾝﾄ・ｵ-ﾌﾟﾝ処理) ※ｼｽﾃﾑ停止時は必ずｸﾛ-ｽﾞ処理と・ｱﾝﾏｳﾝﾄ処理を入れる事
    res = SD_OpenFile(&fil, "1:fill_red.raw", FA_READ);
    
    if (res != FR_OK)    // ｵ-ﾌﾟﾝ失敗時は終了
    {    
        return;
    }
    else
    {
        // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:10MHzで動作)
        SPI2BRG = 80000000 / (2 * (10000000)) - 1;
    }
    
    // ﾌｧｲﾙの読み取り位置を先頭に戻す
    f_lseek(&fil, 0);

    int j = 0;
    
     // ﾌｧｲﾙを最後まで読むﾙ-ﾌﾟ
    while (1)  
    {      
        // bufに (sizeof(buf)-1) ﾊﾞｲﾄまで読み込む
        // 実際に読んだﾊﾞｲﾄ数は br に返る
        //res = f_read(&fil, buf, sizeof(buf) - 1, &br);
        
        // SDカードから3ライン分まとめて読み込む
        res = f_read(&fil, LineBuf, READ_CHUNK, &br);
        
        // 読み込みｴﾗ-発生の場合
        if (res != FR_OK)
        {
            printf("f_read error: %d\n", res);

            // 内側のﾙ-ﾌﾟを抜ける
            break;          
        }
        if (br == 0)   // EOF (ﾌｧｲﾙ終端) に到達
        {
            // EOFなので内側のﾙ-ﾌﾟを抜ける
            break;
        }
        
        // 実際に読めたライン数を計算
        int lines = br / LINE_BYTES;

        for (int k = 0; k < lines; k++)
        {
            // kライン目のポインタを計算
            uint8_t *pLine = &LineBuf[k * LINE_BYTES];

            // DMAで描画
            MSSP_SPI1_DMA_ILI9488_DrawLineFromBuffer(0, j, 480, pLine);

            // 転送完了待ち
            while (SPI1STATbits.SPIBUSY) {}

            j++;
            
        }
        // ﾃﾞﾊﾞｯｸﾞUART出力(先頭16ﾊﾞｲﾄだけ)
        //printf("line %d (br=%d)\n", j, br);
        //dump_hex(Test_Color_Packet_One_Line, 16);  
        
        //buf[br] = '\0';     // C文字列として終端

        //printf("%s", buf);  // 読み込んだ内容を出力
    }
    
    // ﾊﾞｯｸｽｸﾘ-ﾝ色を設定(X軸始点,Y軸始点,幅指定,高さ指定,R,G,B)
    //MSSP_SPI1_DMA_ILI9488_BackGround_Color_Set(0xFC, 0x00, 0x00); // 赤
    //MSSP_SPI1_DMA_ILI9488_BackGround_Color_Set(0x00, 0xFC, 0x00); // 緑
    //MSSP_SPI1_DMA_ILI9488_BackGround_Color_Set(0x00, 0x00, 0xFC); // 青
    //MSSP_SPI1_DMA_ILI9488_BackGround_Color_Set(0xFC, 0xFC, 0xFC); // 白   
    //MSSP_SPI1_DMA_ILI9488_FillBackground(0, 0, 480, 360, 0x00, 0x00, 0x00); // 黒
        
    // ｸﾞﾗﾌﾌﾟﾛｯﾄｴﾘｱに対して塗り潰し(X軸始点,Y軸始点,幅指定,高さ指定,R,G,B)
    //MSSP_SPI1_DMA_ILI9488_FillBackground(5, 60, 355, 260 , 0x00, 0x00, 0x00);
    
    // 塗り潰した箇所に罫線を描画(引数は共用しない)
    //MSSP_SPI1_DMA_ILI9488_DrawBorder(5, 60, 355, 260, 0xFC, 0xFC, 0xFC);
    
    // ﾒﾆｭ-ﾎﾞﾀﾝを塗り潰し
    //MSSP_SPI1_DMA_ILI9488_FillBackground(5, 5, 75, 50, 0x00, 0x80, 0xFA);
    
    
    // RTC8564初期化用ｺﾏﾝﾄﾞﾃ-ﾌﾞﾙから要素数を取得
    RTC8564_Get_Initialize_Data_Table_Size(&StructRtcRtc8564nbDatas.Initialize_Command_Data_Size);
    
    // RTC8564NB初期化
    for(StructRtcRtc8564nbDatas.Init_Counter = 0; StructRtcRtc8564nbDatas.Init_Counter < StructRtcRtc8564nbDatas.Initialize_Command_Data_Size ; StructRtcRtc8564nbDatas.Init_Counter++)
    {
        MSSP_RTC8564NB_Initialize(RTC8564NB_WRITE_ADDRESS_BYTE, StructRtcRtc8564nbDatas.RTC8564_Initialize_Data_Table[StructRtcRtc8564nbDatas.Init_Counter][0], StructRtcRtc8564nbDatas.RTC8564_Initialize_Data_Table[StructRtcRtc8564nbDatas.Init_Counter][1]);
    }

    // RTC8564割込み機能初期化ｺﾏﾝﾄﾞﾃ-ﾌﾞﾙから要素数を取得
    RTC8564_Get_Initialize_Interrupt_Command_Table_Size(&StructRtcRtc8564nbDatas.Initialize_Interrupt_Command_Data_Size);

    // RTC8564NB割込み機能初期化
    for(StructRtcRtc8564nbDatas.Init_Int_Counter = 0; StructRtcRtc8564nbDatas.Init_Int_Counter < StructRtcRtc8564nbDatas.Initialize_Interrupt_Command_Data_Size ; StructRtcRtc8564nbDatas.Init_Int_Counter++)
    {
        MSSP_RTC8564NB_Initialize(RTC8564NB_WRITE_ADDRESS_BYTE, StructRtcRtc8564nbDatas.RTC8564_Initialize_Interrupt_Data_Table[StructRtcRtc8564nbDatas.Init_Int_Counter][0], StructRtcRtc8564nbDatas.RTC8564_Initialize_Interrupt_Data_Table[StructRtcRtc8564nbDatas.Init_Int_Counter][1]);
    }
    
    // 画面が高速描画する際に割込みが発生するため、描画が完了後にIRQ割込みを許可
    IEC0bits.INT2IE = 1;     // 割込み有効




    
     // 無限ﾙ-ﾌﾟでﾌｧｲﾙを繰り返し読む
    while (1)   
    {    

        
        // ﾕ-ｻﾞ-からのﾀｯﾁﾊﾟﾈﾙ入力監視(有効な押下かどうか判定)
        SrructTpmXpt2046Datas.f_valid_press= XPT2046_Touch_IsValidPress();
        
        // ﾀｯﾁﾊﾟﾈﾙ入力が有効と判定された場合の処理
        if(SrructTpmXpt2046Datas.f_valid_press == true)
        {
            // X/Y/Z座標を3回ｻﾝﾌﾟﾘﾝｸﾞしてﾉｲｽﾞ低減用ﾃﾞ-ﾀを取得
            for(int i = 0; i < 3; i++)
            {
                XPT2046_Axis_Data_Get(i, &SrructTpmXpt2046Datas);
            }
            
            // X軸: 3回ｻﾝﾌﾟﾙのうち中央値近似 (2番目と3番目の平均値)
            SrructTpmXpt2046Datas.u16_median_x_axis_data = (SrructTpmXpt2046Datas.u16_x_axis_datas[1] + SrructTpmXpt2046Datas.u16_x_axis_datas[2]) / 2;

            // Y軸: 同様に中央値近似
            SrructTpmXpt2046Datas.u16_median_y_axis_data = (SrructTpmXpt2046Datas.u16_y_axis_datas[1] + SrructTpmXpt2046Datas.u16_y_axis_datas[2]) / 2;
            
            // Z1軸: 押圧ﾚﾍﾞﾙ検出用の中央値近似
            SrructTpmXpt2046Datas.u16_median_z1_axis_data = (SrructTpmXpt2046Datas.u16_z1_axis_datas[1] + SrructTpmXpt2046Datas.u16_z1_axis_datas[2]) / 2;
            
            // Z2軸: 押圧ﾚﾍﾞﾙ検出用の中央値近似
            SrructTpmXpt2046Datas.u16_median_z2_axis_data = (SrructTpmXpt2046Datas.u16_z2_axis_datas[1] + SrructTpmXpt2046Datas.u16_z2_axis_datas[2]) / 2;
            
            // 押圧ﾃﾞ-ﾀ(Z値)を算出 (Z1-Z2の差分でﾀｯﾁ圧力を評価)
            SrructTpmXpt2046Datas.u16_z_azis_pressure_data = SrructTpmXpt2046Datas.u16_median_z1_axis_data - SrructTpmXpt2046Datas.u16_median_z2_axis_data;
            
            // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
            SPI1_XPT2046_CS = 0;
    
            // ﾀｯﾁ解放時: ADC/ﾘﾌｧﾚﾝｽ/ﾄﾞﾗｲﾊﾞをOFFにして省電力ﾓ-ﾄﾞ(PD=00) 次ﾀｯﾁ時の入力を受け付けなくなる
            MSSP_SPI1_TransferData(0x90);
            
            // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
            SPI1_XPT2046_CS = 1;
            
            __asm("nop");
        }
            
        // 時刻ﾃﾞ-ﾀを構造体内の配列へ格納する為、要素数を取得
        RTC8564_Get_Time_Data_Table_Size(&StructRtcRtc8564nbDatas.Time_Data_Buffer_Size);
        
        // RTC8564から時刻ﾃﾞ-ﾀを取得するﾙ-ﾌﾟ処理
        for(StructRtcRtc8564nbDatas.Read_Counter = 0; StructRtcRtc8564nbDatas.Read_Counter < StructRtcRtc8564nbDatas.Time_Data_Buffer_Size; StructRtcRtc8564nbDatas.Read_Counter++)
        {
            // RTC8564NBから時刻ﾃﾞ-ﾀを読み込む処理
            MSSP_RTC8564NB_GetTimeDataProc(StructRtcRtc8564nbDatas.RTC8564_Time_Data_Table[StructRtcRtc8564nbDatas.Read_Counter][0],&StructRtcRtc8564nbDatas.RTC8564_Time_Data_Table[StructRtcRtc8564nbDatas.Read_Counter][1]);
        
            // 読み込んだBCDﾃﾞ-ﾀを10進数へ変換とｺﾞﾐ除去
            RTC8564NB_Bcd_to_Dec_to_Str_Conversion(StructRtcRtc8564nbDatas.Read_Counter, &StructRtcRtc8564nbDatas.RTC8564_Time_Data_Table[StructRtcRtc8564nbDatas.Read_Counter][1], &StructRtcRtc8564nbDatas);
        }
        
        sprintf(StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Mseconds, "%03d", StructRtcRtc8564nbDatas.RTC8564_Mseconds_Counter);

        // I2C1側のMCP3425(ch1)でAD値の取得ﾌﾟﾛｾｽ
        MSSP_I2C1_MCP3425_GetDataProc(&StructAdcMcp3425DatasCH1.u8_upper_binary_data, &StructAdcMcp3425DatasCH1.u8_lower_binary_data);
       
        // 取得した上位ﾊﾞｲﾄと下位ﾊﾞｲﾄのﾏ-ｼﾞ処理
        StructAdcMcp3425DatasCH1.u16_adc_binary_data = (StructAdcMcp3425DatasCH1.u8_upper_binary_data << 8) | StructAdcMcp3425DatasCH1.u8_lower_binary_data;

        // PGAx1の場合の電圧変換処理
        StructAdcMcp3425DatasCH1.f_voltage_data = (StructAdcMcp3425DatasCH1.u16_adc_binary_data * 2.048) / 0x7FFF;

        // 整数値(ADC)を文字変換して配列へ格納
        sprintf(StructAdcMcp3425DatasCH1.c_adc_binary_str_data,"%d",StructAdcMcp3425DatasCH1.u16_adc_binary_data);
        
        // 浮動小数点数値を文字変換し配列へ格納
        sprintf(StructAdcMcp3425DatasCH1.c_voltage_str_data,"%.4f",StructAdcMcp3425DatasCH1.f_voltage_data);
        
        // 電圧値を風速値へ変換
        float windspeed1 = voltage_to_windspeed(StructAdcMcp3425DatasCH1.f_voltage_data);
        
        // 風速値(浮動小数点数値)を文字変換し配列へ格納
        sprintf(StructAdcMcp3425DatasCH1.c_result_windspeed_str_data,"%.4f",windspeed1);
        

        // 16x16ﾌｫﾝﾄ表示テスト
        //ILI9341_DrawFont16x16(00, 50, font_a, 0x0000, 0xFFFF);  // 黒文字、白背景
        
/*
        // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
        SPI1_ILI9341_CS     = 0x00;
        
        new_x = i;
        
        new_y = ((StructAdcMcp3425DatasCH1.u16_adc_binary_data * 270UL) / 32767UL);
        
        if(i == 0)
        {
            old_x = 0;
            
            old_y = ((StructAdcMcp3425DatasCH1.u16_adc_binary_data * 270UL) / 32767UL);
        }
        else
        {
            DrawLine(old_x, old_y, new_x, new_y);
                        
            old_x = new_x;
            
            old_y = new_y;
        }
        
        if(i <= 319)
        {
            i++;
        }
        else
        {
             ILI9341_ClearScreen(0xFFFF);
             
             MSSP_SPI1_ILI9341_Rectangle_Button_Draw_Proc();
             
             i=0;
        }
        
        // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
        SPI1_ILI9341_CS     = 0x01;
*/
        // I2C2側のMCP3425(ch2)でAD値の取得ﾌﾟﾛｾｽ
        MSSP_I2C2_MCP3425_GetDataProc(&StructAdcMcp3425DatasCH2.u8_upper_binary_data, &StructAdcMcp3425DatasCH2.u8_lower_binary_data);
        
        // 取得した上位ﾊﾞｲﾄと下位ﾊﾞｲﾄのﾏ-ｼﾞ処理
        StructAdcMcp3425DatasCH2.u16_adc_binary_data = (StructAdcMcp3425DatasCH2.u8_upper_binary_data << 8) | StructAdcMcp3425DatasCH2.u8_lower_binary_data;
        
        // PGAx1の場合の変換処理
        StructAdcMcp3425DatasCH2.f_voltage_data = (StructAdcMcp3425DatasCH2.u16_adc_binary_data * 2.048) / 0x7FFF;
        
       // 整数値(ADC)を文字変換して配列へ格納
        sprintf(StructAdcMcp3425DatasCH2.c_adc_binary_str_data,"%d",StructAdcMcp3425DatasCH2.u16_adc_binary_data);
        
        // 浮動小数点数値を文字変換し配列へ格納
        sprintf(StructAdcMcp3425DatasCH2.c_voltage_str_data,"%.4f",StructAdcMcp3425DatasCH2.f_voltage_data);
        
        // 電圧値を風速値へ変換
        float windspeed2 = voltage_to_windspeed(StructAdcMcp3425DatasCH2.f_voltage_data);
        
        // 風速値(浮動小数点数値)を文字変換し配列へ格納
        sprintf(StructAdcMcp3425DatasCH2.c_result_windspeed_str_data,"%.4f",windspeed2);
        
        
        // 風速値をLCD1行目へ表示
        MSSP_Lcd_String_Send(StructAdcMcp3425DatasCH1.c_result_windspeed_str_data);

        // 2行目へｷｬﾘｯｼﾞﾘﾀ-ﾝ
        MSSP_LCD_2line();
        
        // 温度値をLCD2行目へ表示
        MSSP_Lcd_String_Send(StructAdcMcp3425DatasCH2.c_voltage_str_data);
        
        __delay_ms(50);

        MSSP_Lcd_Clear();

        MSSP_Lcd_Home();
        
        
        // 送信ﾊﾞｯﾌｧが空になるまで待機
        while(U1STAbits.UTXBF){};
        
        printf("\033[2J");    // 画面全体クリア
        printf("\033[H");     // カーソルを左上に移動
        
        printf("%s年%s月%s日%s時%s分%s秒%s\n\r",
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Year,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Month,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Day,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Hour,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Minutes,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Seconds,
                StructRtcRtc8564nbDatas.RTC8564_Result_Time_Data_Mseconds);
        
        // UART通信で電圧値とADC値をまとめて送信(ADC値はｶｯｺ付き)
        printf("CH1:%sv (b%s)(%sm/sec)\n\r",StructAdcMcp3425DatasCH1.c_voltage_str_data,StructAdcMcp3425DatasCH1.c_adc_binary_str_data,StructAdcMcp3425DatasCH1.c_result_windspeed_str_data);
              
        // 送信ﾊﾞｯﾌｧが空になるまで待機
        while(U2STAbits.UTXBF){};
        
        // UART通信で電圧値を送信
        printf("CH2:%sv (b%s)(%sm/sec)\n\r",StructAdcMcp3425DatasCH2.c_voltage_str_data,StructAdcMcp3425DatasCH2.c_adc_binary_str_data,StructAdcMcp3425DatasCH2.c_result_windspeed_str_data);
        
    }       
    return;
}

//============================
// HEXダンプ関数 (16バイトごとに改行)
//============================
void dump_hex(uint8_t *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", buf[i]);
        if ((i + 1) % 16 == 0)   // 16バイトごとに改行
        {
            printf("\r\n");
        }
    }
    printf("\r\n");  // 最後に改行
}



// printf用出力関数
void _mon_putc(unsigned char c)
{
    while (U1STAbits.UTXBF);       // 送信バッファが空くまで待つ
    U1TXREG = c;                   // 1文字送信
}