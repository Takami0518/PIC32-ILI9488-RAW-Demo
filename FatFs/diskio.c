/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */
#include "../SD_Driver.h"

/* Example: Declarations of the platform and disk functions in the project */
//#include "platform.h"
//#include "storage.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_RAM 	0	/* Map FTL to physical drive 0 */
#define DEV_MMC		1	/* Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Map USB MSD to physical drive 2 */


// CMD0:SPIﾓ-ﾄﾞで動作指定し状態をｱｲﾄﾞﾙ状態へ遷移
// CMD


// DSTATUS型はFatFs独自型「ﾄﾞﾗｲﾌﾞ状態を表す型」
DSTATUS stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

// pdrvは物理ﾄﾞﾗｲﾌﾞ番号
DSTATUS disk_status (BYTE pdrv)
{
    // 指定された物理ﾄﾞﾗｲﾌﾞ番号によって処理分岐
    switch (pdrv)
    {
        // RAMﾃﾞｨｽｸは未実装なので常に未初期化扱い
        case DEV_RAM:
        
            // DSTATUS型のSTA_NOINIT(初期化未実行状態)として処理
            stat = STA_NOINIT;
        
            return stat;

        // SDｶ-ﾄﾞは実装する為、初期化処理を行う
        case DEV_MMC :
        
            // SDｶ-ﾄﾞの状態を返す
            stat = MMC_disk_status();

            return stat;

        // USBは未実装なので常に未初期化扱い
        case DEV_USB :
		
            // DSTATUS型のSTA_NOINIT(初期化未実行状態)として処理
            stat = STA_NOINIT;

            return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS stat;

	switch (pdrv)
    {
        // RAMﾃﾞｨｽｸは未実装なので常に未初期化扱い
        case DEV_RAM :
		
            // DSTATUS型のSTA_NOINIT(初期化未実行状態)として処理
            stat = STA_NOINIT;

		return stat;

        // SDｶ-ﾄﾞは実装する為、初期化処理を行う
        case DEV_MMC :
        
            // SDｶ-ﾄﾞの初期化実行
            stat = SD_disk_initialize();

		return stat;

        // RAMﾃﾞｨｽｸは未実装なので常に未初期化扱い
        case DEV_USB :

            // DSTATUS型のSTA_NOINIT(初期化未実行状態)として処理
            stat = STA_NOINIT;

		return stat;
	}
	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = SD_disk_read(buff, sector, count);

		// translate the reslut code here

		return result;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
