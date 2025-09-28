// R1ﾚｽﾎﾟﾝｽﾌｫ-ﾏｯﾄ(CMD0応答時に返却される1ﾊﾞｲﾄ)
// bit7 : 常に0                 (0=応答有効)
// bit6 : Parameter error       (ﾊﾟﾗﾒ-ﾀ不正)
// bit5 : Address error         (ｱﾄﾞﾚｽ不正)
// bit4 : Erase sequence error  (ｲﾚ-ｽｼ-ｹﾝｽ不正)
// bit3 : CRC error             (CRCﾁｪｯｸ失敗)
// bit2 : Illegal command       (未定義/非対応CMD)
// bit1 : Erase reset           (ｲﾚ-ｽﾘｾｯﾄ発生)
// bit0 : In idle state         (1=Idle中, 0=Ready)

// R7ﾚｽﾎﾟﾝｽﾌｫ-ﾏｯﾄ(CMD8応答時に返却される5ﾊﾞｲﾄ(その内1ﾊﾞｲﾄはR1ﾚｽﾎﾟﾝｽ))
// Byte[0] : R1ﾚｽﾎﾟﾝｽ (ｽﾃ-ﾀｽ 1ﾊﾞｲﾄ)
// Byte[1] : 常に0x00
// Byte[2] : 常に0x00
// Byte[3] : 電圧受入ﾊﾟﾀ-ﾝ (Voltage accepted)　0x01 = 2.7〜3.6V対応 (一般的なSDHC/SDXC)　それ以外 = 非対応
// Byte[4] : ｴｺ-ﾊﾟﾀ-ﾝ (Check pattern) ﾎｽﾄが送信した下位8ﾋﾞｯﾄ (通常0xAA) 一致すれば通信正常

// R3ﾚｽﾎﾟﾝｽﾌｫ-ﾏｯﾄ(CMD58応答時に返却される5ﾊﾞｲﾄ(その内1ﾊﾞｲﾄはR1ﾚｽﾎﾟﾝｽ))
// OCR：Operation Conditions Register(4ﾊﾞｲﾄ) : 動作条件ﾚｼﾞｽﾀ
// bit31(2ﾊﾞｲﾄ目のMSB)  : SDﾓ-ﾄﾞ時BUSYﾌﾗｸﾞ / SPIﾓ-ﾄﾞでは常に0
// bit30                : CCS(Card Capacity Status) 
//                          1=SDHC/SDXC(ﾌﾞﾛｯｸｱﾄﾞﾚｽ方式)
//                          0=SDSC(ﾊﾞｲﾄｱﾄﾞﾚｽ方式)
// bit29: UHS-II対応ﾌﾗｸﾞ(将来拡張)
// bit23-15: 動作電圧範囲 (例: 2.7-3.6Vでﾋﾞｯﾄ17-23が1)
// その他 : 予約領域

#include <stdio.h>
#include "ff.h"
#include "diskio.h"
//#include "../System_Peripherals.h"
//#include "../sdcard.h"

#include "System_Peripherals.h"
#include "sdcard.h"
#include "SD_Driver.h"


static DSTATUS stat = STA_NOINIT;

static uint8_t response_array[5];

//============================
// SDｶ-ﾄﾞをﾏｳﾝﾄしてﾌｧｲﾙを開く関数
// mode: FA_READ / FA_WRITE / FA_OPEN_ALWAYS など
//============================
FRESULT SD_OpenFile(FIL *fp, const char *path, BYTE mode)
{
    FRESULT res;

    // ﾌｧｲﾙｼｽﾃﾑをﾏｳﾝﾄ
    res = f_mount(&FatFs, "1:", 1);
    if (res != FR_OK) 
    {
        printf("f_mount error: %d\n", res);
        
        return res;
    }

    // ﾌｧｲﾙｵ-ﾌﾟﾝ
    res = f_open(fp, path, mode);
    if (res != FR_OK)
    {
        printf("f_open error: %d\n", res);
    
        f_mount(0, "1:", 0); // ｱﾝﾏｳﾝﾄ
    }

    return res;
}

//============================
// SDｶ-ﾄﾞをｸﾛ-ｽﾞしてｱﾝﾏｳﾝﾄ
//============================
FRESULT SD_CloseFile(FIL *fp)
{
    FRESULT res;

    res = f_close(fp);
    
    if (res != FR_OK)
    {
        printf("f_close error: %d\n", res);
    }

    res = f_mount(0, "1:", 0);
    if (res != FR_OK)
    {
        printf("f_unmount error: %d\n", res);
    }

    return res;
}


// この関数ではｺﾏﾝﾄﾞﾌﾚ-ﾑ[Start+Cmd] [Arg31-24] [Arg23-16] [Arg15-8] [Arg7-0] [CRC7+End]を生成
// 送信しSDｶ-ﾄﾞからの応答を受信、状態遷移を判断して次処理へ移行する
uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg)
{
    uint8_t i;
    uint8_t packet[6];
    volatile int dummy = 0;

    memset(response_array, 0, sizeof(response_array));
    
    // CSをLOWにしてSDｶ-ﾄﾞ選択
    SPI2_SD_CARD_CS = 0;

    // ｽﾀ-ﾄﾋﾞｯﾄ(0x40) + ｺﾏﾝﾄﾞｲﾝﾃﾞｯｸｽ(下位6ﾋﾞｯﾄがｺﾏﾝﾄﾞ番号)
    packet[0] = 0x40 | cmd;
    
    // ｱ-ｷﾞｭﾒﾝﾄ(32bit)を生成(SDｶ-ﾄﾞで言う引数を生成)
    packet[1] = (arg >> 24) & 0xFF;   
    
    packet[2] = (arg >> 16) & 0xFF;
    
    packet[3] = (arg >> 8) & 0xFF;
    
    packet[4] = arg & 0xFF;
    
    // CRC(巡回冗長検査)ﾃﾞ-ﾀ通信での｢ﾋﾞｯﾄ化けがないか｣を検出する為の誤り検出ｺ-ﾄﾞ
    // CMD0(ｶ-ﾄﾞﾘｾｯﾄ)の場合は0x95
    if (cmd == CMD0)
    {
        packet[5] = 0x95;
    }
    // CMD8(ｶ-ﾄﾞﾊﾞ-ｼﾞｮﾝ確認)の場合は0x87
    else if (cmd == CMD8)
    {
        packet[5] = 0x87;
    }
    // CMD55(ｱﾌﾟﾘｺﾏﾝﾄﾞ通知)とACMD41(初期化)の場合は0x01
    else if(cmd == CMD55 || cmd == ACMD41)
    {
        packet[5] = 0x01;
    }
    // CMD58(ｶ-ﾄﾞｽﾍﾟｯｸ確認)の場合は0xFF
    else if(cmd == CMD58 || cmd == CMD17)
    {
        packet[5] = 0xFF;
    }
    
    // CMD8ｴﾗ-ﾋﾞｯﾄ0x7F対応のﾘﾄﾗｲ用ｼﾞｬﾝﾌﾟﾗﾍﾞﾙ
    CMD8_ERROR_HANDLER:
    
    // 生成したｺﾏﾝﾄﾞﾌﾚ-ﾑを送信
    for (i = 0; i < 6; i++)
    {
        MSSP_SPI2_TransferData(packet[i]);
    }

    // SDｶ-ﾄﾞからの応答処理(R1ﾚｽﾎﾟﾝｽ100ﾊﾞｲﾄ分待ち)
    for (i = 0; i < 100; i++)
    {
        response_array[0] = MSSP_SPI2_TransferData(0xFF);
        
        // 7bit目が0なら応答成立(不成立は0xFFが返される)
        if ((response_array[0] & 0x80) == 0)
        {
            // CMD0(ｶ-ﾄﾞﾘｾｯﾄ)かCMD8(ｶ-ﾄﾞﾊﾞ-ｼﾞｮﾝ確認)の場合
            if(cmd == CMD0 || cmd == CMD8 || cmd == CMD55)
            {
                    // SDｶ-ﾄﾞ初回差込みの時だけR1ﾚｽﾎﾟﾝｽが0x7Fが発生する
                    // CMD8のｴﾗ-ﾋﾞｯﾄ(R1ﾚｽﾎﾟﾝｽ：0x7F)CMD8をﾘﾄﾗｲする
                    if ((cmd == CMD8) && (response_array[0] == 0x7F))
                    {
                        // ｼﾞｬﾝﾌﾟ命令
                        goto CMD8_ERROR_HANDLER;
                    }
                // ｱｲﾄﾞﾙ状態を取得(起動完了)正常に終了した場合は0x01が返答される
                // ただしﾊﾞ-ｼﾞｮﾝ1.xのｶ-ﾄﾞの場合、CMD8は未ｻﾎﾟ-ﾄなので0x05(ILLEGAL_COMMAND)が返答される
                if (response_array[0] == 0x01 || response_array[0] == 0x05) break;
            }
            // ACMD41(初期化)かCMD58(ｶ-ﾄﾞｽﾍﾟｯｸ確認)の場合
            else if(cmd == ACMD41 || cmd == CMD58 || cmd == CMD17)
            {
                // ﾚﾃﾞｨ状態を取得(初期化完了)
                if (response_array[0] == 0x00) break;
            }
        }
    }
    
    // 各ｺﾏﾝﾄﾞ実行後のSDｶ-ﾄﾞの状態をR1ﾚｽﾎﾟﾝｽで確認(1ﾊﾞｲﾄ読み込み)    
    // R1ﾚｽﾎﾟﾝｽが0x01ならｱｲﾄﾞﾙ状態成立、ﾊﾞ-ｼﾞｮﾝ1.xの場合は0x05でｱｲﾄﾞﾙ状態成立
    if(response_array[0] == 0x01 || response_array[0] == 0x05 )
    {
        // ｱｲﾄﾞﾙ状態成立
        dummy++;
    }
    // 1bit目が0ならﾚﾃﾞｨ状態が成立
    else if (response_array[0] == 0x00)
    {
        // ﾚﾃﾞｨ状態成立
        dummy++;
    }
    else
    {
        // ｴﾗ-や無応答
        dummy++;
    }
    
    // R7ﾚｽﾎﾟﾝｽの追加処理(5ﾊﾞｲﾄ読み込み)
    if(cmd == CMD8 || cmd == CMD58)
    {
        for (i = 1; i < 5; i++)
        {
            // R7ﾚｽﾎﾟﾝｽの残りの4ﾊﾞｲﾄを読む
            response_array[i] = MSSP_SPI2_TransferData(0xFF);
        }
        if(cmd == CMD8)
        {
            // R7ﾚｽﾎﾟﾝｽのﾊﾟﾀ-ﾝﾁｪｯｸ(3ﾊﾞｲﾄ目の電圧許容でSDHC/SDXCとSDSCを認識区別する)
            if(response_array[0] == 0x01 && response_array[3] == 0x01 && response_array[4] == 0xAA)
            {
                // SDHC/SDXC候補
                dummy++;
            }
            else
            {
                // SDSC候補
                dummy++;
            }
        }
        //
        else if(cmd == CMD58)
        {
            if((response_array[1] & 0x40) == 0)
            {
                // SDSC確定
                card_type = SDSC;
            }
            else
            {
                // SDHC確定
                card_type = SDHC;
            }
        }
    }

    if (cmd != CMD17) 
    {
        MSSP_SPI2_TransferData(0xFF); // 余分8clk
    
        SPI2_SD_CARD_CS = 1;          // デセレクト
        
        MSSP_SPI2_TransferData(0xFF); // さらに8clkでDO解放を確実化
    }

    // R1ﾚｽﾎﾟﾝｽのﾊﾟﾗﾒ-ﾀを返す
    return response_array[0];
}


DSTATUS SD_disk_initialize(void)
{
    // CMD0     : ｶ-ﾄﾞをIdle状態にﾘｾｯﾄ
    // CMD8     : 電圧範囲とVer2.0以降判定(R7応答確認)
    // CMD55    : 次がｱﾌﾟﾘｹ-ｼｮﾝｺﾏﾝﾄﾞであることを通知
    // ACMD41   : 初期化要求･Busy解除待ち(HCS指定可)
    // CMD58    : OCR(ｵﾍﾟﾚ-ﾃｨﾝｸﾞ･ｺﾝﾃﾞｨｼｮﾝ･ﾚｼﾞｽﾀ)読込･Busy確認･容量ﾀｲﾌﾟ判定(SDHC/SDXC or SDSC)

    uint8_t response;
    
    // ｽﾃ-ﾀｽを未初期化
    //DSTATUS stat = STA_NOINIT;

    // SDｶ-ﾄﾞに80ｸﾛｯｸ送信(CSはHIGH)
    SD_SendDummyClocks();
    
    // SPI2受信ﾊﾞｯﾌｧをﾘﾌﾚｯｼｭ
    SPI2_FlushRX();

    // CMD0送信でｶ-ﾄﾞﾘｾｯﾄ(CMD0:GO_IDLE_STATE) SPIﾓ-ﾄﾞで動作指定しｱｲﾄﾞﾙ状態へ遷移
    if(SD_SendCommand(CMD0, 0) != 0x01)
    {
        return stat;
    }

    // CMD8送信でSDSC/SDHC･SDXCどちらに対応しているか確認
    if (SD_SendCommand(CMD8, 0x1AA) == 0x01)
    {
        // CMD8で応答があった場合、SDVer2.0以降対応 (SDHC/SDXCの可能性あり)
        // ACMD41で初期化完了待ち
        do{
            // 次のｺﾏﾝﾄﾞがｱﾌﾟﾘｹ-ｼｮﾝ専用(ACMD系)と通知
            SD_SendCommand(CMD55, 0);
            
            // SDｶ-ﾄﾞを初期化実行(ｱｲﾄﾞﾙ状態⇒ﾚﾃﾞｨ状態)
            response = SD_SendCommand(ACMD41, 0x40000000);
        
            // R1ﾚｽﾎﾟﾝｽ(1bit)が0x00(ﾚﾃﾞｨ状態)なら初期化終了    
        }while (response != 0x00);
    }
    else
    {
        // CMD8で応答しない又はﾀｲﾑｱｳﾄの場合、SDVer1.xの旧ｶ-ﾄﾞ(SDSCの可能性あり)
        // ACMD41で初期化完了待ち
        do{
            // 次のｺﾏﾝﾄﾞがｱﾌﾟﾘｹ-ｼｮﾝ専用(ACMD系)と通知
            SD_SendCommand(CMD55, 0);
            
            // SDｶ-ﾄﾞを初期化実行(ｱｲﾄﾞﾙ状態⇒ﾚﾃﾞｨ状態)
            response = SD_SendCommand(ACMD41,0);
            
        // R1ﾚｽﾎﾟﾝｽ(1bit)が0x00(ﾚﾃﾞｨ状態)なら初期化終了
        }while (response != 0x00);
    }

    // CMD58(ｶ-ﾄﾞｽﾍﾟｯｸ確認)でOperation Conditions Registerを実行
    SD_SendCommand(CMD58, 0);

    // 初期化成功
    if (response == 0x00)
    {
        stat &= ~STA_NOINIT;
        return stat;
        
        //return 0;   // OK
    }
    else
    {
        return STA_NOINIT;
    }
}

void SD_SendDummyClocks(void)
{
    int i;
    
    uint8_t dummy;
    
    SPI2_SD_CARD_CS = 1;
    
    // CSをHIGHにして80クロック流す
    for (i = 0; i < 10; i++)
    {
        MSSP_SPI2_TransferData(0xFF);
    }
}

DSTATUS MMC_disk_status(void)
{
    // ｶ-ﾄﾞ未挿入検出が出来るならば個々でﾁｪｯｸ、出来ない場合は初期化で判断する
    // if (!SD_CardInserted()) return STA_NODISK;

    // 初期化済みﾌﾗｸﾞをﾁｪｯｸ
    if (stat & STA_NOINIT) 
    {
        return STA_NOINIT;
    }

    return 0;   // 正常
}

/*-----------------------------------------------------------------------*/
/* Stub functions for unused drives (RAM / USB)                          */
/*-----------------------------------------------------------------------*/

int RAM_disk_read (BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;   // 未実装
}

int RAM_disk_write (const BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;   // 未実装
}

int USB_disk_read (BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;   // 未実装
}

int USB_disk_write (const BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;   // 未実装
}

int MMC_disk_read (BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;  // 未実装
}

int MMC_disk_write (const BYTE* buff, LBA_t sector, UINT count)
{
    (void)buff; (void)sector; (void)count;
    return -1;  // 未実装
}

// FatFs が参照するシステム時刻取得関数
DWORD get_fattime(void)
{
    return ((DWORD)(2025 - 1980) << 25)  // 年 (1980年基準)
         | ((DWORD)1 << 21)              // 月 (1月)
         | ((DWORD)1 << 16)              // 日 (1日)
         | ((DWORD)0 << 11)              // 時
         | ((DWORD)0 << 5)               // 分
         | ((DWORD)0 >> 1);              // 秒/2
}  

/*-----------------------------------------------------------------------*/
/* SDｶ-ﾄﾞからｾｸﾀ(512ﾊﾞｲﾄ)を読み出す処理                                */
/*-----------------------------------------------------------------------*/
DRESULT SD_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    // 要求されたｾｸﾀ数(count)だけ繰り返す
    for (UINT i = 0; i < count; i++) {

        // 読み出し先ﾊﾞｯﾌｧを512ﾊﾞｲﾄごとにずらし、
        // 読みたいｾｸﾀ番号を更新しながら1ﾌﾞﾛｯｸ読み出し
        if (SD_ReadSingleBlock(buff + i * 512, sector + i) != RES_OK) {

            // 1つでも失敗したら即ｴﾗ-を返す
            return RES_ERROR;
        }
    }

    // 全てのﾌﾞﾛｯｸ読み出しが成功したら正常終了を返す
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* 1ｾｸﾀ(512ﾊﾞｲﾄ)をSDｶ-ﾄﾞから読み出す処理                               */
/*-----------------------------------------------------------------------*/
static DRESULT SD_ReadSingleBlock(uint8_t *buff, uint32_t sector)
{
    // ｶ-ﾄﾞ種類によりｱﾄﾞﾚｽ解釈を変更
    // SDSC → ﾊﾞｲﾄ単位なので ×512
    // SDHC/SDXC → ﾌﾞﾛｯｸ単位そのまま
    //uint32_t addr = (card_type == SDSC) ? (sector << 9) : sector;

    uint32_t addr;
    
    if(card_type == SDSC)
    {
        addr = sector << 9;
    }
    else
    {
        addr = sector;
    }
    
    // CMD17(指定したﾌﾞﾛｯｸのﾃﾞ-ﾀを読み出す)送信 (READ_SINGLE_BLOCK)
    if (SD_SendCommand(CMD17, addr) != 0x00) 
    {
        return RES_ERROR;
    }

    // ﾃﾞ-ﾀﾄ-ｸﾝ(0xFE)待ち
    uint8_t token;
    int retry = 100000;
    do
    {
        token = MSSP_SPI2_TransferData(0xFF);
    
    } while (token == 0xFF && --retry);

    
    if (token != 0xFE)
    {
        return RES_ERROR;
    }
    
    // 512ﾊﾞｲﾄ分のﾃﾞ-ﾀを受信
    for (int j = 0; j < 512; j++)
    {
        buff[j] = MSSP_SPI2_TransferData(0xFF);
    }
   
    // CRC2ﾊﾞｲﾄを捨て読み
    MSSP_SPI2_TransferData(0xFF);
    MSSP_SPI2_TransferData(0xFF);

    SPI2_SD_CARD_CS = 1;
    
    return RES_OK;
}