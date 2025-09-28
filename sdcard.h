#ifndef SDCARD_H
#define SDCARD_H

// SD Card Commands (SPIモード用)
#define CMD0    0   // GO_IDLE_STATE
#define CMD1    1   // SEND_OP_COND (MMC用, SDはACMD41を使用)
#define CMD8    8   // SEND_IF_COND
#define CMD9    9   // SEND_CSD
#define CMD10   10  // SEND_CID
#define CMD12   12  // STOP_TRANSMISSION
#define CMD16   16  // SET_BLOCKLEN
#define CMD17   17  // READ_SINGLE_BLOCK
#define CMD18   18  // READ_MULTIPLE_BLOCK
#define CMD24   24  // WRITE_BLOCK
#define CMD25   25  // WRITE_MULTIPLE_BLOCK
#define CMD55   55  // APP_CMD
#define CMD58   58  // READ_OCR

// Application-specific commands (ACMD = CMD55 の後に送る)
#define ACMD41  41  // SD_SEND_OP_COND

#define SDSC 0
#define SDHC 1

//#define ERROR_CMD8_0X7F_HANDLER

extern uint8_t card_type;

uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg);
DSTATUS SD_disk_initialize(void);
static DRESULT SD_ReadSingleBlock(uint8_t *buff, uint32_t sector);

#endif // SDCARD_H
