/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>

#include "mflash_drv.h"

#include "uEZ.h"
#include "uEZErrors.h"
#include "uEZFlash.h"
#include <uEZPlatformAPI.h> 
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>

T_uezDevice         gFlash;
T_FlashChipInfo     gFlashChipInfo;

/* Initialize flash peripheral,
 * cannot be invoked directly, requires calling wrapper in non XIP memory */
static T_uezError mflash_drv_init_internal(void)
{
    T_uezError error = UEZ_ERROR_NONE;

    UEZPlatform_IAP_Require();
    error = UEZFlashOpen("IAP", &gFlash);
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "mflash_drv_init::UEZFlashOpen failed, error=%d\n", error);

        __BKPT(0);
    }

    error = UEZFlashGetChipInfo(gFlash, &gFlashChipInfo);
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "mflash_drv_init::UEZFlashGetChipInfo failed, error=%d\n", error);

        __BKPT(0);
    }

    return error;
}

/* API - initialize 'mflash' - calling wrapper */
int32_t mflash_drv_init(void)
{
    return mflash_drv_init_internal();
}

/* Erase single sector */
static T_uezError mflash_drv_sector_erase_internal(uint32_t sector_addr)
{
    T_uezError          error = UEZ_ERROR_NONE;
    T_FlashBlockInfo    flashBlockInfo = { 0 };

    error = UEZFlashGetBlockInfo(gFlash, (sector_addr & 0x00FFFFFF), &flashBlockInfo);

    if(flashBlockInfo.iSize != MFLASH_SECTOR_SIZE)
    {
        DEBUG_RTT_Printf(0, "mflash_drv_sector_erase_internal sector size from block info doesn't match macro!\n");

        __BKPT(0);
    }

    error = UEZFlashBlockErase(gFlash, sector_addr, MFLASH_SECTOR_SIZE);
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "mflash_drv_sector_erase_internal::UEZFlashBlockErase failed, error=%d\n", error);

        __BKPT(0);
    }

    return error;
}

/* API - Erase single sector - calling wrapper */
int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    return mflash_drv_sector_erase_internal(sector_addr);
}

/* Page program */
static int32_t mflash_drv_page_program_internal(uint32_t page_addr, const uint32_t *data)
{
    T_uezError          error = UEZ_ERROR_NONE;

    error = UEZFlashWrite(gFlash, page_addr, (TUInt8 *)data, MFLASH_PAGE_SIZE);
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "mflash_drv_sector_erase_internal::UEZFlashBlockErase failed, error=%d\n", error);

        __BKPT(0);
    }

    return error;
}

/* API - Page program - calling wrapper */
int32_t mflash_drv_page_program(uint32_t page_addr, uint32_t *data)
{
    return mflash_drv_page_program_internal(page_addr, data);
}

#if 0
/* API - Read data */
int32_t mflash_drv_read(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    void *ptr = mflash_drv_phys2log(addr, len);

    if (ptr == NULL)
    {
        return kStatus_InvalidArgument;
    }

    memcpy(buffer, ptr, len);
    return kStatus_Success;
}
#endif

/* API - Get pointer to FLASH region */
void *mflash_drv_phys2log(uint32_t addr, uint32_t len)
{
    uint32_t last_addr = (addr - MFLASH_DRIVE_START_ADDR) + (len != 0 ? len - 1 : 0);

    if (last_addr > (MFLASH_DRIVE_END_ADDR - MFLASH_DRIVE_START_ADDR))
    {
        //return NULL;
    }

    return (void *)(addr);
}

/* API - Get pointer to FLASH region */
uint32_t mflash_drv_log2phys(void *ptr, uint32_t len)
{
    uint32_t log_addr = (uint32_t)ptr;

    if (log_addr < MFLASH_DRIVE_START_ADDR || log_addr > MFLASH_DRIVE_END_ADDR)
    {
        return MFLASH_INVALID_ADDRESS;
    }

    if (log_addr + len > MFLASH_DRIVE_END_ADDR + 1)
    {
        return MFLASH_INVALID_ADDRESS;
    }

    return (log_addr);
}
