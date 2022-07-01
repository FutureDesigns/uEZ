/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <Device/MassStorage.h>
#include <string.h>
#include <uEZPlatform.h>

#define _INTEGER
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef unsigned char	BYTE;
typedef unsigned int	UINT;
typedef unsigned short  WCHAR;
#include "diskio.h"
//#include "mci.h"
//#include "usbhost_ms.h"
#include "ff.h"

static DEVICE_MassStorage **G_fatfsMassStorageDevices[FATFS_MAX_MASS_STORAGE_DEVICES];
//static FATFS G_fatfs[FATFS_MAX_MASS_STORAGE_DEVICES];
static FATFS *G_fatfs; // [FATFS_MAX_MASS_STORAGE_DEVICES];
static TBool G_init = EFalse;

static void IEnsureInit(void)
{
    if (!G_init) {
        G_fatfs = UEZMemAllocPermanent(sizeof(FATFS)*FATFS_MAX_MASS_STORAGE_DEVICES);
      // TESTING
//      G_fatfs = (void *)0x7FD02000; // hard coded!
        memset(G_fatfsMassStorageDevices, 0, sizeof(G_fatfsMassStorageDevices));
        G_init = ETrue;
    }
}

T_uezError FATFS_RegisterMassStorageDevice(
        TUInt32 aSlot,
        DEVICE_MassStorage **aMS)
{
    char message[10];
    IEnsureInit();
    if (aSlot >= FATFS_MAX_MASS_STORAGE_DEVICES)
        return UEZ_ERROR_OUT_OF_RANGE;

    if (G_fatfsMassStorageDevices[aSlot] != 0)
        return UEZ_ERROR_NOT_AVAILABLE;

    G_fatfsMassStorageDevices[aSlot] = aMS;
//    f_mount(aSlot, &G_fatfs[aSlot]);
    sprintf(message, "%d:", aSlot);
    f_mount(G_fatfs+aSlot, message, 1);

    return UEZ_ERROR_NONE;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize(BYTE drv)
{
    T_uezError error;
    DEVICE_MassStorage **p_ms;

    IEnsureInit();
	p_ms = G_fatfsMassStorageDevices[drv];
    if (p_ms) {
        error = (*p_ms)->Init(p_ms, 0);
        if (error == UEZ_ERROR_NONE) {
            return disk_status(drv);
        } else {
            return STA_NOINIT;
        }
    }

    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (BYTE drv)
{
    T_uezError error;
    DSTATUS dstatus;
    T_msStatus status;
	DEVICE_MassStorage **p_ms;

    IEnsureInit();
    p_ms = G_fatfsMassStorageDevices[drv];
    if (p_ms) {
        error = (*p_ms)->GetStatus(p_ms, &status);
        if (error != UEZ_ERROR_NONE)
            return STA_NOINIT;

        dstatus = 0;
        if (status & MASS_STORAGE_STATUS_NEED_INIT)
            dstatus |= STA_NOINIT;
        if (status & MASS_STORAGE_STATUS_NO_MEDIUM)
            dstatus |= STA_NODISK;
        if (status & MASS_STORAGE_STATUS_WRITE_PROTECTED)
            dstatus |= STA_PROTECT;

        return dstatus;
    }

    return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
    T_uezError error;

    DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[drv];
    if (p_ms) {
        error = (*p_ms)->Read(p_ms, sector, count, buff);
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
            return RES_NOTRDY;
        } if (error == UEZ_ERROR_NONE) {
            return RES_OK;
        } else {
            return RES_ERROR;
        }
    }

    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
    T_uezError error;

    DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[drv];
    if (p_ms) {
        error = (*p_ms)->Write(p_ms, sector, count, buff);
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
            return RES_NOTRDY;
        } if (error == UEZ_ERROR_NONE) {
            return RES_OK;
        } else {
            return RES_ERROR;
        }
    }

    return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT stat = RES_PARERR;
	if (ctrl == CTRL_SYNC) {
	    // Sync command current does nothing since all mass storage
	    // media currently does everything on block by block basis
	    return RES_OK;
	}
	return stat;
}

#if 0
BYTE VerifyActiveDisk(BYTE rRequestedDisk) {
   switch (rRequestedDisk) {
   case MMC:
      xprintf("MMC drive selected\n");
      break;

   case USB:
      xprintf("USB drive selected\n");
      break;

   default:
      rRequestedDisk = 0;
      xprintf("No or invalid drive selected\n");
      break;
   }
   return rRequestedDisk;
}
#endif
