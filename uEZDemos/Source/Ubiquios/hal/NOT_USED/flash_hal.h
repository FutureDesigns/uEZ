/*
 * ubiquiOS porting layer for STM32F7 Discovery: Flash memory
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#ifndef FLASH_HAL_H__
#define FLASH_HAL_H__

#include "ubiquios.h"

/**
 * This API is used to interface with external flash memory to retrieve
 * stored configuration and firmware for Bluetooth and WLAN.
 *
 * The content stored in the flash memory must be structured such that
 * it begins with a table describing the memory contents. The table
 * is made up of a number of entries, each of type flash_hal_table_entry_t.
 * The enumeration \ref flash_hal_idx_t provides the indexes into this
 * table.
 */

/**
 * Entry in the table of flash contents.
 */
typedef struct flash_hal_table_entry_t
{
    /** Starting address of data */
    uint32_t offset;
    /** Length of data */
    uint32_t length;
} flash_hal_table_entry_t;


/**
 * Enumeration of indices into the flash table of contents.
 */
typedef enum flash_hal_idx_t
{
    /** WLAN */
    FLASH_HAL_IDX_WLAN = 0,
    /** Bluetooth */
    FLASH_HAL_IDX_BT   = 1,
    /** Total number of entries in the table */
    FLASH_HAL_IDX_NUM_ENTRIES
} flash_hal_idx_t;


/**
 * Initialise the flash HAL.
 */
extern void
flash_hal_init(void);

/**
 * Reads a block of data from flash.
 *
 * \param idx       Index into the flash table of contents used to select
 *                  where in flash to read from.
 * \param offset    Offset at which to begin reading from.
 * \param buffer    Buffer to put data into.
 * \param length    Pointer to an integer that contains the length of
 *                  data to read. This must be updated by the function
 *                  to be the amount of data actually read.
 * \return          UB_STATUS_SUCCESS on success, else an appropriate
 *                  error code.
 */
extern ub_status_t
flash_hal_read(flash_hal_idx_t idx,
               uint32_t offset, uint32_t *length, uint8_t *buffer);

#endif
