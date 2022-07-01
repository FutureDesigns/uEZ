/*
 * ubiquiOS porting layer for STM32 Nucleo: SPI
 *
 * Copyright 2016 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#ifndef SPI_HAL_H__
#define SPI_HAL_H__

#include "ubiquios.h"


/**
 * Enumeration of commonly used SPI clock rates.
 */
typedef enum spi_hal_clock_rate_t
{
    /** SPI clock speed 1 MHz */
    SPI_HAL_CLOCK_1MHZ                  = 1000000,

    /** SPI clock speed 12.5 MHz */
    SPI_HAL_CLOCK_12_5MHZ               = 12500000,

    /** SPI clock speed 25 MHz */
    SPI_HAL_CLOCK_25MHZ                 = 25000000,

    /** SPI clock speed 40 MHz */
    SPI_HAL_CLOCK_40MHZ                 = 40000000,

    /** SPI clock speed 50 MHz */
    SPI_HAL_CLOCK_50MHZ                 = 50000000,
} spi_hal_clock_rate_t;


/*
 * SPI_CLK    D13     PA5 |
 * SPI_MISO   D12     PA6 |- SPI1
 * SPI_MOSI   D11     PA7 |
 */
#define SPI_INTERFACE        (SPI1)
#define SPI_CLK_PORT         (GPIOA)
#define SPI_CLK_PIN          (GPIO_PIN_5)
#define SPI_MISO_MOSI_PORT   (GPIOA)
#define SPI_MISO_PIN         (GPIO_PIN_6)
#define SPI_MOSI_PIN         (GPIO_PIN_7)


/**
 * Enables the SPI HAL. This will initialise the SPI interface and route
 * it to the appropriate I/O pins.
 */
extern void
spi_hal_enable(uint32_t clock_rate);


/**
 * Disable the SPI HAL. This will deinitialise the SPI interface and
 * disconnect it from the I/O pins (setting them as inputs).
 */
extern void
spi_hal_disable(void);


/**
 * Perform a read-write operation on the SPI interface.
 *
 * \param write_data The octet to write.
 *
 * \returns the octet read.
 */
extern uint8_t
spi_hal_rw(uint8_t write_data);


/**
 * Set the SPI clock rate.
 *
 * \param clock_rate The clock rate to set.
 */
extern void
spi_hal_set_clock_rate(uint32_t clock_rate);


/**
 * Write a block of data over SPI
 *
 * \param data   The data to write.
 * \param length Length of the data.
 */
extern void
spi_hal_write_block(const uint8_t *data, uint16_t length);


/**
 * Read a block of data over SPI
 *
 * \param data   Data buffer to receive the read information.
 * \param length Length of the data.
 */
extern void
spi_hal_read_block(uint8_t *data, uint16_t length);


#endif
