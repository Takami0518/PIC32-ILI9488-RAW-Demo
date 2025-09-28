#ifndef TPM_XPT2046_DRIVER_H
#define TPM_XPT2046_DRIVER_H

#include <stdbool.h>

//==============================================================================
// XTP2046用の設定ｺﾏﾝﾄﾞ
//==============================================================================
// ｺﾏﾝﾄﾞﾌｫ-ﾏｯﾄ: [S | D2 D1 D0 | MODE | SER/DFR | PD1 PD0]
//  S: ｽﾀ-ﾄﾋﾞｯﾄ(常に1)
//  D2-D0: 入力ﾁｬﾝﾈﾙ選択
//  001=X座標, 101=Y座標, 011=Z1, 100=Z2
//  MODE: 0=12bit変換, 1=8bit変換
//  SER/DFR: 0=差動, 1=ｼﾘｱﾙ
//  PD1-PD0: 電源ﾀﾞｳﾝ制御 (11=常時ON, 00=省電力)

// X軸(12bit/差動) + PD=11 常時ON
#define X_AXIS_DATA_GET_COMMAND     0x93

// Y軸(12bit/差動) + PD=11 常時ON
#define Y_AXIS_DATA_GET_COMMAND     0xD3

// Z1(押圧1) + PD=11 常時ON
#define Z1_AXIS_DATA_GET_COMMAND    0xB3

// Z2(押圧2) + PD=11 常時ON
#define Z2_AXIS_DATA_GET_COMMAND    0xC3

//==============================================================================
// XTP2046用の関数群
//==============================================================================
#define MIN_HOLD_MS   5   // 最低押下時間
#define DEBOUNCE_MS   1   // チャタリング吸収

extern volatile uint8_t  u8_xpt_irq_flag;
extern volatile uint32_t u32_ms_ticks;

#define XPT_IRQ_IS_LOW()   (!PORTDbits.RD9)

typedef enum{ TP_IDLE, TP_DEB, TP_WAIT_MIN } TP_GSTATE;

static TP_GSTATE g = TP_IDLE;

static uint32_t t0 = 0;


bool XPT2046_Touch_IsValidPress(void);

#endif 