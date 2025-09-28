#ifndef SD_DRIVER_H
#define SD_DRIVER_H

uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg);
DSTATUS MMC_disk_initialize(void);
DSTATUS MMC_disk_status(void);
void SD_SendDummyClocks(void);

void FatFs_FileReadTest(void);

extern FATFS FatFs;

#endif