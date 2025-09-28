#ifndef CROM_GT20L16J1Y_DRIVER_H
#define CROM_GT20L16J1Y_DRIVER_H

int get_utf8_char(const char *in, uint8_t out[4]);
void GT20_ReadFont16(uint32_t addr, uint8_t *buf);
uint32_t GT20_GetFontAddress(uint8_t row, uint8_t col);


// UTF-8 → 区点マップ1件分
typedef struct
{
    uint8_t utf8[3];   // 例: {0xE3,0x81,0x81}
    
    uint8_t nbytes;    // UTF-8のバイト数(多くは3)
    
    uint8_t kuten_row; // 区点 行
    
    uint8_t kuten_col; // 区点 列
    
} U8Kuten;

extern const U8Kuten kMap[];
extern const size_t kMapCount;
#endif
