/*
 * ubiquiOS porting layer on STM32 Nucleo: SPI
 *
 * Copyright 2016 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */


#include "stm32f4xx_hal.h"

#include "spi_hal.h"


/** Local SPI port instance. */
static SPI_HandleTypeDef hspi;


/** Tracks whether SPI has been enabled. */
static bool spi_enabled = false;


/** Tracks the current clock rate. */
static uint32_t current_clock_rate = 0;


/** SPI timeout in milliseconds. */
#define SPI_TIMEOUT (5000)


void
spi_hal_enable(uint32_t clock_rate)
{
    if (spi_enabled)
    {
        if (clock_rate != current_clock_rate)
            spi_hal_set_clock_rate(clock_rate);
        return;
    }

    /* GPIOs */
    __GPIOA_CLK_ENABLE(); /* SPI MOSI, MISO, SCK */

    /* Setup SPI */
    hspi.Instance = SPI_INTERFACE;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.Direction = SPI_DIRECTION_2LINES;
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    /* Initialise to boot max rate: 12.5 MHz
     * Divide fPCLK by 8 gives us 10.5 MHz */
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode = SPI_TIMODE_DISABLED;
    hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi.Init.CRCPolynomial = 10;

    /* Note: spi_hal_set_clock_rate() will execute HAL_SPI_Init() */
    spi_hal_set_clock_rate(clock_rate);

    spi_enabled = true;
}


void
spi_hal_disable(void)
{
    if (!spi_enabled)
        return;

    HAL_SPI_DeInit(&hspi);

    spi_enabled = false;
}


uint8_t
spi_hal_rw(uint8_t write_data)
{
    uint8_t read_data;

    HAL_SPI_TransmitReceive(&hspi, &write_data, &read_data, 1, SPI_TIMEOUT);

    return read_data;
}


void
spi_hal_set_clock_rate(uint32_t clock_rate)
{
    if (clock_rate >= SPI_HAL_CLOCK_50MHZ)
    {
       /* Divide fPCLK by 2 gives us 42 MHz */
       hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    }
    else if (clock_rate >= SPI_HAL_CLOCK_25MHZ)
    {
        /* Divide fPCLK by 4 gives us 21 MHz */
        hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    }
    else if (clock_rate >= SPI_HAL_CLOCK_12_5MHZ)
    {
        /* Divide fPCLK by 8 gives us 10.5 MHz */
        hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    }
    else
    {
       /* Divide fPCLK by 128 gives us 650 KHz */
       hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    }
    HAL_SPI_Init(&hspi);
    current_clock_rate = clock_rate;
}


void
spi_hal_write_repeated(const uint8_t data, uint16_t count)
{
    uint8_t tx_buf[64];
    uint8_t rx_buf[sizeof(tx_buf)];

    if (count < sizeof(tx_buf))
        memset(tx_buf, data, count);
    else
        memset(tx_buf, data, sizeof(tx_buf));

    while (count > 0)
    {
        uint16_t tx_cnt;
        if (count < sizeof(tx_buf))
            tx_cnt = count;
        else
            tx_cnt = sizeof(tx_buf);

        HAL_SPI_TransmitReceive(&hspi, tx_buf, rx_buf, tx_cnt, SPI_TIMEOUT);
        count -= tx_cnt;
    }
}


void
spi_hal_write_block(const uint8_t *data, uint16_t length)
{
    uint8_t rx_buf[64];

    while (length > 0)
    {
        uint16_t tx_len;
        if (length < sizeof(rx_buf))
            tx_len = length;
        else
            tx_len = sizeof(rx_buf);

        HAL_SPI_TransmitReceive(&hspi, (uint8_t *)data, rx_buf, tx_len,
                                SPI_TIMEOUT);
        length -= tx_len;
        data += tx_len;
    }
}


void
spi_hal_read_block(uint8_t *data, uint16_t length)
{
    uint8_t tx_buf[64];

    if (length < sizeof(tx_buf))
        memset(tx_buf, 0xFF, length);
    else
        memset(tx_buf, 0xFF, sizeof(tx_buf));

    while (length > 0)
    {
        uint16_t rx_len;
        if (length < sizeof(tx_buf))
            rx_len = length;
        else
            rx_len = sizeof(tx_buf);

        HAL_SPI_TransmitReceive(&hspi, tx_buf, data, rx_len, SPI_TIMEOUT);
        length -= rx_len;
        data += rx_len;
    }
}

