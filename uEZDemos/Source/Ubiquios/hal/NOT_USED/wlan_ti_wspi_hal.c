/*
 * ubiquiOS porting layer for STM32 Nucleo: WLAN TI WSPI
 *
 * Copyright 2016 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"

/* This is set in the Makefile */
#ifdef WLAN_HEADER
#include WLAN_HEADER
#endif


#include "stm32f4xx_hal.h"

#include "power_hal.h"
#include "spi_hal.h"
#include "flash_hal.h"


/* WL_EN       PB5 */
#define WLAN_EN_PORT            GPIOB
#define WLAN_EN_PIN             GPIO_PIN_5
#define WLAN_EN_ENABLE          SET
#define WLAN_EN_DISABLE         RESET

 /* WL_WSPI_CSn     PB6 */
#define WL_WSPI_CSn_PORT        GPIOB
#define WL_WSPI_CSn_PIN         GPIO_PIN_6
#define WL_WSPI_CSn_ENABLE      RESET
#define WL_WSPI_CSn_DISABLE     SET

/* WL_WSPI_IRQn     PB10 */
#define WL_WSPI_IRQn_PORT       GPIOB
#define WL_WSPI_IRQn_PIN        GPIO_PIN_10
#define WL_WSPI_IRQn_ASSERTED   GPIO_PIN_RESET
#define WL_WSPI_IRQn_DEASSERTED GPIO_PIN_SET


void
EXTI15_10_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_FLAG(WL_WSPI_IRQn_PIN))
    {
        ub_wlan_hal_ti_wspi_interrupt_handler();
        __HAL_GPIO_EXTI_CLEAR_IT(WL_WSPI_IRQn_PIN);
    }
}


void
ub_wlan_hal_ti_wspi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIOs */
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    /* Initially we drive WLAN_EN enable and CSn low to disable the WLAN
     * module and ensure that it is not parasitically powered. */

    /* Setup WLAN_EN */
    GPIO_InitStruct.Pin = WLAN_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(WLAN_EN_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(WLAN_EN_PORT, WLAN_EN_PIN, WLAN_EN_DISABLE);

    /* Setup CSn */
    GPIO_InitStruct.Pin = WL_WSPI_CSn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(WL_WSPI_CSn_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(WL_WSPI_CSn_PORT, WL_WSPI_CSn_PIN, 0);

    /* Setup IRQn */
    GPIO_InitStruct.Pin = WL_WSPI_IRQn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WL_WSPI_IRQn_PORT, &GPIO_InitStruct);

    /* Enable and set EXTI 15:10 Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    power_hal_init();
    flash_hal_init();
}


void
ub_wlan_hal_ti_wspi_set_power_on(bool on)
{
    if (on)
    {
        /* Turn power on */
        power_hal_set_power_on(SUBSYSTEM_WLAN, true);

        /* Wait a little bit for it to settle */
        ub_delay(UB_MILLISECONDS(1));

        HAL_GPIO_WritePin(WLAN_EN_PORT, WLAN_EN_PIN, WLAN_EN_ENABLE);
    }
    else
    {
        HAL_GPIO_WritePin(WLAN_EN_PORT, WLAN_EN_PIN, WLAN_EN_DISABLE);
        HAL_GPIO_WritePin(WL_WSPI_CSn_PORT, WL_WSPI_CSn_PIN, WL_WSPI_CSn_ENABLE);

        /* Wait a little bit and then power off */
        ub_delay(UB_MILLISECONDS(1));

        power_hal_set_power_on(SUBSYSTEM_WLAN, false);

        spi_hal_disable();
    }
}


void
ub_wlan_hal_ti_wspi_enable(void)
{
    spi_hal_enable(SPI_HAL_CLOCK_50MHZ);
    spi_hal_set_clock_rate(SPI_HAL_CLOCK_50MHZ);
}


void
ub_wlan_hal_ti_wspi_select(bool enable)
{
    if (enable)
    {
        HAL_GPIO_WritePin(WL_WSPI_CSn_PORT, WL_WSPI_CSn_PIN, WL_WSPI_CSn_ENABLE);
    }
    else
    {
        HAL_GPIO_WritePin(WL_WSPI_CSn_PORT, WL_WSPI_CSn_PIN, WL_WSPI_CSn_DISABLE);
        ub_wlan_hal_ti_wspi_rw(0xFF);
    }
}


uint8_t
ub_wlan_hal_ti_wspi_rw(uint8_t write_data)
{
    spi_hal_enable(SPI_HAL_CLOCK_50MHZ);
    return spi_hal_rw(write_data);
}


void
ub_wlan_hal_ti_wspi_set_interrupt_enabled(bool enable)
{
    if (enable)
        EXTI->IMR |= WL_WSPI_IRQn_PIN;
    else
        EXTI->IMR &= ~WL_WSPI_IRQn_PIN;
}


bool
ub_wlan_hal_ti_wspi_is_interrupt_asserted(void)
{
    return HAL_GPIO_ReadPin(WL_WSPI_IRQn_PORT, WL_WSPI_IRQn_PIN) ==
           WL_WSPI_IRQn_ASSERTED;
}


ub_status_t
ub_wlan_hal_ti_get_firmware_block(uint32_t offset, uint32_t *length,
                                  uint8_t *buffer)
{
    return flash_hal_read(FLASH_HAL_IDX_WLAN, offset, length, buffer);
}
