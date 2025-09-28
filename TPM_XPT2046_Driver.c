#include <xc.h>
#include "TPM_XPT2046_Driver.h"
#include "System_Peripherals.h"
#include "System_Structs.h"

// 有効押下が確定した瞬間にtrueを返す
bool XPT2046_Touch_IsValidPress(void)
{
    const uint8_t pressed_now = (PORTBbits.RB3 == 0); // Low=押下
    
    switch(g)
    {   
        // ﾀｯﾁﾊﾟﾈﾙｱｲﾄﾞﾙ状態
        case TP_IDLE:

            // ﾀｯﾁﾊﾟﾈﾙが押下された処理(External2_Handler割込み処理にてﾌﾗｸﾞｱｯﾌﾟ)
            if (u8_xpt_irq_flag == 1)
            {
                
                   
                // ﾌﾗｸﾞﾀﾞｳﾝ
                u8_xpt_irq_flag = 0; 
                
                // 押下されたﾀｲﾐﾝｸﾞを取得
                t0 = u32_ms_ticks;
                
                // 状態をﾃﾞﾊﾞｳﾝｽ処理へ遷移
                g = TP_DEB;
            }

            // ﾀｯﾁﾊﾟﾈﾙが押下されていない場合は終了
            break;

        // ﾃﾞﾊﾞｳﾝｽ処理状態
        case TP_DEB:

            // 押下された時間を測定(40msec以上なら次処理へ遷移)
            if ((u32_ms_ticks - t0) >= DEBOUNCE_MS)
            {
                // ﾀｯﾁﾊﾟﾈﾙが継続して押下中
                if (PORTBbits.RB3 == 0)
                {
                    // 
                    t0 = u32_ms_ticks;
                    
                    g = TP_WAIT_MIN;
                }
                else 
                {
                    g = TP_IDLE;
                }
            }
            break;

        case TP_WAIT_MIN:

            if (!XPT_IRQ_IS_LOW())
            {
                g = TP_IDLE; break;
            }
            if ((u32_ms_ticks - t0) >= MIN_HOLD_MS)
            {
                g = TP_IDLE;     // ここで有効タッチ確定

                // IQR入力による外部割込み2が発生した場合(ﾌﾞｻﾞ-発音)
                LATEbits.LATE2 = 1;

                __delay_ms(10);

                LATEbits.LATE2 = 0;

                return true;
            }
            break;
    }
    return false;
}

void XPT2046_Axis_Data_Get(int cnt, TPM_XPT2046_DataStruct *tpm_data)
{
    uint16_t data;
    
    //SPIﾓ-ﾄﾞ:Mode 0（CPOL=0, CPHA=0）変更
    SPI1CONbits.CKE = 1;
    
    SPI1CONbits.CKP = 0;
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_XPT2046_CS = 0;
    
    MSSP_SPI1_TransferData(X_AXIS_DATA_GET_COMMAND);
    
    // SPI通信で高位ﾊﾞｲﾄを取得し、8bit左ｼﾌﾄしてX軸ﾃﾞ-ﾀ配列に格納
    tpm_data -> u16_x_axis_datas[cnt] = MSSP_SPI1_TransferData(0x00) << 8;

    // SPI通信で低位ﾊﾞｲﾄを取得し、X軸ﾃﾞ-ﾀ配列へOR演算で追加
    tpm_data -> u16_x_axis_datas[cnt] |= MSSP_SPI1_TransferData(0x00);
    
    // XPT2046の12bit有効ﾃﾞ-ﾀにするため右ｼﾌﾄ(下位4bitを捨てる)
    tpm_data -> u16_x_axis_datas[cnt] = tpm_data -> u16_x_axis_datas[cnt] >> 4;
    
    MSSP_SPI1_TransferData(Y_AXIS_DATA_GET_COMMAND);
    
    // SPI通信で高位ﾊﾞｲﾄを取得し、8bit左ｼﾌﾄしてY軸ﾃﾞ-ﾀ配列に格納
    tpm_data -> u16_y_axis_datas[cnt] = MSSP_SPI1_TransferData(0x00) << 8;

    // SPI通信で低位ﾊﾞｲﾄを取得し、Y軸ﾃﾞ-ﾀ配列へOR演算で追加
    tpm_data -> u16_y_axis_datas[cnt] |= MSSP_SPI1_TransferData(0x00);
    
    // XPT2046の12bit有効ﾃﾞ-ﾀにするため右ｼﾌﾄ(下位4bitを捨てる)
    tpm_data -> u16_y_axis_datas[cnt] = tpm_data -> u16_y_axis_datas[cnt] >> 4;
    
    MSSP_SPI1_TransferData(Z1_AXIS_DATA_GET_COMMAND);
    
    // SPI通信で高位ﾊﾞｲﾄを取得し、8bit左ｼﾌﾄしてZ1軸ﾃﾞ-ﾀ配列に格納
    tpm_data -> u16_z1_axis_datas[cnt] = MSSP_SPI1_TransferData(0x00) << 8;

    // SPI通信で低位ﾊﾞｲﾄを取得し、Z1軸ﾃﾞ-ﾀ配列へOR演算で追加
    tpm_data -> u16_z1_axis_datas[cnt] |= MSSP_SPI1_TransferData(0x00);
    
    // XPT2046の12bit有効ﾃﾞ-ﾀにするため右ｼﾌﾄ(下位4bitを捨てる)
    tpm_data -> u16_z1_axis_datas[cnt] = tpm_data -> u16_z1_axis_datas[cnt] >> 4;
    
    MSSP_SPI1_TransferData(Z2_AXIS_DATA_GET_COMMAND);
    
    // SPI通信で高位ﾊﾞｲﾄを取得し、8bit左ｼﾌﾄしてZ2軸ﾃﾞ-ﾀ配列に格納
    tpm_data -> u16_z2_axis_datas[cnt] = MSSP_SPI1_TransferData(0x00) << 8;

    // SPI通信で低位ﾊﾞｲﾄを取得し、Z2軸ﾃﾞ-ﾀ配列へOR演算で追加
    tpm_data -> u16_z2_axis_datas[cnt] |= MSSP_SPI1_TransferData(0x00);
    
    // XPT2046の12bit有効ﾃﾞ-ﾀにするため右ｼﾌﾄ(下位4bitを捨てる)
    tpm_data -> u16_z2_axis_datas[cnt] = tpm_data -> u16_z2_axis_datas[cnt] >> 4;
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_XPT2046_CS = 1;
    
    //SPIﾓ-ﾄﾞ:Mode 3（CPOL=1, CPHA=1）変更
    SPI1CONbits.CKE = 0;
    
    SPI1CONbits.CKP = 1;
}