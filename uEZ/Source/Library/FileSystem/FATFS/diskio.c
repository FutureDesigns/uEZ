/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2020        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <Device/MassStorage.h>
#include <string.h>
#include <uEZPlatform.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
static DEVICE_MassStorage **G_fatfsMassStorageDevices[FATFS_MAX_MASS_STORAGE_DEVICES];
//static FATFS G_fatfs[FATFS_MAX_MASS_STORAGE_DEVICES];
static FATFS *G_fatfs; // [FATFS_MAX_MASS_STORAGE_DEVICES];
static TBool G_init = EFalse;

static void IEnsureInit(void)
{
  if (!G_init) {
    G_fatfs = UEZMemAllocPermanent(sizeof(FATFS)*FATFS_MAX_MASS_STORAGE_DEVICES);
    // 
    //memset(G_fatfs, 0, sizeof(FATFS)*FATFS_MAX_MASS_STORAGE_DEVICES); // We don't need to actually clear this, but leave as example.
    memset(G_fatfsMassStorageDevices, 0, sizeof(G_fatfsMassStorageDevices));
    G_init = ETrue;
  }
}

#if FF_USE_LFN == 3	/* Dynamic memory allocation */ // TODO not tested yet
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/
void* ff_memalloc (	/* Returns pointer to the allocated memory block (null if not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
	return UEZMemAlloc(msize);	/* Allocate a new memory block with POSIX API */
}

/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/
void ff_memfree (
	void* mblock	/* Pointer to the memory block to free (nothing to do if null) */
)
{
	UEZMemFree(mblock);	/* Free the memory block with POSIX API */
}
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
  T_uezError error;
  DSTATUS dstatus;
  T_msStatus status;
  DEVICE_MassStorage **p_ms;
  
  IEnsureInit();
  p_ms = G_fatfsMassStorageDevices[pdrv];
  if (p_ms) {
    error = (*p_ms)->GetStatus(p_ms, &status);
    if (error != UEZ_ERROR_NONE)
      return STA_NOINIT;
    
    if ((status & MASS_STORAGE_STATUS_NO_MEDIUM) || (status & MASS_STORAGE_STATUS_NEED_INIT)) {
      // No Medium?  Mark as not mounted and try again
      G_fatfs[pdrv].fs_type = 0;
      error = (*p_ms)->GetStatus(p_ms, &status);
    }
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
  sprintf(message, "%u:", aSlot);
  f_mount(G_fatfs+aSlot, message, 1);
  
  return UEZ_ERROR_NONE;
}

T_uezError FATFS_UnregisterMassStorageDevice(TUInt32 aSlot)
{
  char message[10];
  IEnsureInit();
  if (aSlot >= FATFS_MAX_MASS_STORAGE_DEVICES)
    return UEZ_ERROR_OUT_OF_RANGE;
  
  // Is there anything registered there?
  if (G_fatfsMassStorageDevices[aSlot] == 0)
    return UEZ_ERROR_NONE;
  
  sprintf(message, "%u:", aSlot);
  f_mount(G_fatfs+aSlot, message, 0);
  // Clear the slot for the next drive
  memset(G_fatfs+aSlot, 0, sizeof(G_fatfs[aSlot]));
  
  // Ok, clear it out
  G_fatfsMassStorageDevices[aSlot] = 0;
  
  return UEZ_ERROR_NONE;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
  T_uezError error;
  DEVICE_MassStorage **p_ms;
  
  IEnsureInit();
  p_ms = G_fatfsMassStorageDevices[pdrv];
  if (p_ms) {
    error = (*p_ms)->Init(p_ms, 0);
    if (error == UEZ_ERROR_NONE) {
      return disk_status(pdrv);
    } else {
      return STA_NOINIT;
    }
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
/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
  T_uezError error;
  
  DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[pdrv];
  if (p_ms) {
    // Are we on a 32-bit boundary?
    if ((((int32_t)buff) & 3) != 0) {
      // If the pointer is NOT on a 32-bit boundary, do it the slow way
      // Determine 32-bit aligned memory pointer to scratch area
      char *p_scratch = (char *)(((int32_t)(G_fatfs[pdrv].scratch+3)) & ~3);
      
      while (count--) {
        error = (*p_ms)->Read(p_ms, sector++, 1, p_scratch); // read data into scratch buffer first
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
          return RES_NOTRDY;
        } if (error == UEZ_ERROR_NONE) {
          memcpy(buff, p_scratch, 512); // Then copy data back into standard buffer
          buff += 512;
          if (count == 0)
            return RES_OK;
        } else {
          return RES_ERROR;
        }
      }
    } else {
      // If the pointer is on a 32-bit boundary, do it the fast way
      error = (*p_ms)->Read(p_ms, sector, count, buff);
      if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
        return RES_NOTRDY;
      } if (error == UEZ_ERROR_NONE) {
        return RES_OK;
      } else {
        return RES_ERROR;
      }
    }
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
/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
  T_uezError error;
  BYTE *buffPtr = (BYTE *)buff;
  
  DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[pdrv];
  if (p_ms) {
    // Are we on a 32-bit boundary?
    if ((((int32_t)buff) & 3) != 0) {
      // If the pointer is NOT on a 32-bit boundary, do it the slow way
      // Determine 32-bit aligned memory pointer to scratch area
      char *p_scratch = (char *)(((int32_t)(G_fatfs[pdrv].scratch+3)) & ~3);
      
      while (count--) {
        memcpy(p_scratch, buffPtr, 512); // copy data to write into aligned buffer first			    
        error = (*p_ms)->Write(p_ms, sector++, 1, p_scratch);
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
          return RES_NOTRDY;
        } if (error == UEZ_ERROR_NONE) {
          buffPtr += 512;
          if (count == 0)
            return RES_OK;
        } else {
          return RES_ERROR;
        }
      }
    } else {
      // If the pointer is on a 32-bit boundary, do it the fast way
      error = (*p_ms)->Write(p_ms, sector, count, buff);
      if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
        return RES_NOTRDY;
      } if (error == UEZ_ERROR_NONE) {
        return RES_OK;
      } else {
        return RES_ERROR;
      }
    }
  }

  
  return RES_PARERR;

}
#endif /* _READONLY */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_PARERR;
/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
    T_uezError error;
    DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[pdrv];

  if (p_ms) {
        if (cmd == CTRL_SYNC) {
            switch (pdrv) {
                case DEV_RAM : // Process of the command for the RAM drive
                    // Sync command current does nothing since all mass storage
                    // media currently does everything on block by block basis
                    return RES_OK;
            case DEV_MMC : // Process of the command for the MMC/SD card
                    // Sync command current does nothing since all mass storage
                    // media currently does everything on block by block basis
                    return RES_OK;
            case DEV_USB : // Process of the command the USB drive
                    // Sync command current does nothing since all mass storage
                    // media currently does everything on block by block basis
                    return RES_OK;
            }
        }
        
        if (cmd == CTRL_TRIM) { //  Inform DEV data on BLK of sectors is no longer used (needed at FF_USE_TRIM == 1)
            switch (pdrv) {
            case DEV_RAM : // Process of the command for the RAM drive
                return res;
            case DEV_MMC : // Process of the command for the MMC/SD card
                return res;
            case DEV_USB : // Process of the command the USB drive
                return res;
            }
        }

        // Remaining commands all deal with reading size info, so call size info then return the req info.
        T_msSizeInfo aInfo;
        error = (*p_ms)->GetSizeInfo(p_ms,&aInfo); // single command for RAM/MMC/USB in uEZ
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
            return RES_NOTRDY;
        } else if (error == UEZ_ERROR_NONE) {
            if (cmd == GET_SECTOR_COUNT) { //Get media size (needed at FF_USE_MKFS == 1)
                *(DWORD*)buff = aInfo.iNumSectors; //LogBlockNbr;
                return RES_OK;
            }
            if (cmd == GET_SECTOR_SIZE) { // Get sector size (needed at FF_MAX_SS != FF_MIN_SS)
                *(WORD*)buff = aInfo.iSectorSize; //LogBlockSize;
                return RES_OK;
            }
            if (cmd == GET_BLOCK_SIZE) { // Get erase block size (needed at FF_USE_MKFS == 1)
                // Why are these different numbers on SD card?
                //*(DWORD*)buff = aInfo.iSectorSize/512; //.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
                *(DWORD*)buff = aInfo.iBlockSize;  // num sectors per block 
                return RES_OK;
            }
        }
        return res;
    }
    return RES_PARERR;
}

/********************************************************************
 *                 FDI Modifications Here
 *******************************************************************/
DRESULT disk_getInfo (
	BYTE pdrv,		/* Physical drive number (0..) */
	T_msSizeInfo *aInfo		/* info struct pointer */
)
{
    T_uezError error;
    DEVICE_MassStorage **p_ms = G_fatfsMassStorageDevices[pdrv];

    if (p_ms) {
        error = (*p_ms)->GetSizeInfo(p_ms,aInfo);
        if ((error == UEZ_ERROR_TIMEOUT) || (error == UEZ_ERROR_NAK)) {
            return RES_NOTRDY;
        } else if (error == UEZ_ERROR_NONE) {
            return RES_OK;
        }
    }
    return RES_PARERR;
}

#if FF_FS_REENTRANT	/* Mutal exclusion */

/// TODO replace this line.
//const osMutexDef_t Mutex[FF_VOLUMES];	/* Table of CMSIS-RTOS mutex */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/
int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create the sync object */
	BYTE vol,			/* Corresponding volume (logical drive number) */
	FF_SYNC_t* sobj		/* Pointer to return the created sync object */
)
{
	/* FreeRTOS */
	*sobj = xSemaphoreCreateMutex();
	return (int)(*sobj != NULL);
}

/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/
int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to an error */
	FF_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	/* FreeRTOS */
  vSemaphoreDelete(sobj);
	return 1;
}

/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/
int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	FF_SYNC_t sobj	/* Sync object to wait */
)
{
	/* FreeRTOS */
	return (int)(xSemaphoreTake(sobj, FF_FS_TIMEOUT) == pdTRUE);
}

/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/
void ff_rel_grant (
	FF_SYNC_t sobj	/* Sync object to be signaled */
)
{
	/* FreeRTOS */
	xSemaphoreGive(sobj);
}
#endif
