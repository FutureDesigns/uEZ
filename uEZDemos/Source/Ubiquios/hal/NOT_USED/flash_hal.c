/*
 * Copyright 2016 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */

/* This is set in the Makefile */
#ifdef WLAN_HEADER
#include WLAN_HEADER
#endif

#include "stm32f4xx_hal.h"

#include "flash_hal.h"
#include "spi_hal.h"
#include "ubiquios.h"


#define SPI_FLASH_CSn_PORT        GPIOA
#define SPI_FLASH_CSn_PIN         GPIO_PIN_4


/** Enumeration of commands used by the flash IC */
typedef enum flash_commands_t {
    FLASH_READ = 0x03,
    FLASH_HS_READ = 0x0b,
    FLASH_ERASE = 0x60,
    FLASH_PAGE_PROGRAM = 0x02,
    FLASH_READ_STATUS = 0x05,
    FLASH_WRITE_ENABLE = 0x06,
    FLASH_WRITE_DISABLE = 0x04
} flash_commands_t;


/** RAM shadow copy of the table of contents in flash */
static flash_hal_table_entry_t flash_hal_toc[FLASH_HAL_IDX_NUM_ENTRIES];

/** Tracks whether the flash_hal_toc is valid */
static bool flash_hal_toc_valid = false;

static inline uint32_t swap32(uint32_t v)
{
    uint32_t result;
    __asm volatile ("rev %0, %1" : "=l" (result) : "l" (v) );
    return result;
}


static void
flash_hal_spi_select(bool enable)
{
    if (enable)
    {
        HAL_GPIO_WritePin(SPI_FLASH_CSn_PORT, SPI_FLASH_CSn_PIN, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(SPI_FLASH_CSn_PORT, SPI_FLASH_CSn_PIN, GPIO_PIN_SET);
        spi_hal_rw(0xFF);
    }
}


static void
flash_read(uint32_t address, uint32_t length, uint8_t *buffer)
{
    uint32_t addr_be32 = swap32(address); /* Convert to big endian */
    uint8_t command = FLASH_HS_READ;

    flash_hal_spi_select(true);

    spi_hal_rw(command);
    /* We only use the lower three octets and this is stored in BE32 */
    spi_hal_write_block(&(((uint8_t *)&addr_be32)[1]), 3);
    /* Need a dummy read at 40MHz */
    spi_hal_rw(0xff);
    spi_hal_read_block(buffer, length);

    flash_hal_spi_select(false);
}


ub_status_t
flash_hal_read(flash_hal_idx_t idx, uint32_t offset, uint32_t *length,
               uint8_t *buffer)
{
    uint32_t address;

    spi_hal_enable(SPI_HAL_CLOCK_40MHZ);

    if (!flash_hal_toc_valid)
    {
        /* Store a shadow copy of the index in RAM */
        flash_read(0, sizeof(flash_hal_toc), (uint8_t *)flash_hal_toc);

        /* Only mark as valid if we have read a valid table... which means
         * the first entry in the table must not have offset 0x0000 or 0xFFFF.
         */
        if (flash_hal_toc[0].offset != 0x0000 &&
                flash_hal_toc[0].offset != 0xFFFF)
        {
            flash_hal_toc_valid = true;
        }

        /* Failed to read TOC, return error */
        if (!flash_hal_toc_valid)
            return UB_STATUS_HARDWARE_ERROR;
    }

    address = flash_hal_toc[idx].offset + offset;

    if (offset > flash_hal_toc[idx].length)
    {
        ub_ustdio_printf("Attempting to read past end (offset=%u, length=%u)\n", offset, flash_hal_toc[idx].length);
        return UB_STATUS_INVALID_ARGUMENTS;
    }

    if ((offset + *length) > flash_hal_toc[idx].length)
    {
        *length = flash_hal_toc[idx].length - offset;
    }

    flash_read(address, *length, buffer);

    spi_hal_disable();

    return UB_STATUS_SUCCESS;
}


void
flash_hal_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    static bool initialised = false;

    if (initialised)
        return;

    __GPIOA_CLK_ENABLE(); /* SPI CS */

    GPIO_InitStruct.Pin = SPI_FLASH_CSn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(SPI_FLASH_CSn_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(SPI_FLASH_CSn_PORT, SPI_FLASH_CSn_PIN, GPIO_PIN_SET);

    initialised = true;
}

