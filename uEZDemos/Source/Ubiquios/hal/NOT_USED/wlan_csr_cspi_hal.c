/*
 * ubiquiOS porting layer for STM32 Nucleo: WLAN CSR CSPI
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
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


/* WL_ENn       PB5 */
#define WLAN_ENn_PORT           GPIOB
#define WLAN_ENn_PIN            GPIO_PIN_5
#define WLAN_ENn_ENABLE         RESET
#define WLAN_ENn_DISABLE        SET

 /* WL_CSPI_CSn     PB6 */
#define WL_CSPI_CSn_PORT        GPIOB
#define WL_CSPI_CSn_PIN         GPIO_PIN_6

/* WL_CSPI_IRQn     PB10 */
#define WL_CSPI_IRQn_PORT       GPIOB
#define WL_CSPI_IRQn_PIN        GPIO_PIN_10


/** Current clock rate for CSPI communication */
static uint32_t current_clock_rate = 0;

void
EXTI15_10_IRQHandler(void)
{
    if ( __HAL_GPIO_EXTI_GET_FLAG(WL_CSPI_IRQn_PIN))
    {
        ub_wlan_hal_csr_cspi_interrupt_handler();
        __HAL_GPIO_EXTI_CLEAR_IT(WL_CSPI_IRQn_PIN);
    }
}


void
ub_wlan_hal_csr_cspi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    current_clock_rate = UB_WLAN_HAL_CSR_CSPI_CLOCK_RATE_BOOT;
    spi_hal_enable(UB_WLAN_HAL_CSR_CSPI_CLOCK_RATE_BOOT);

    /* GPIOs */
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = WLAN_ENn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(WLAN_ENn_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = WL_CSPI_CSn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(WL_CSPI_CSn_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = WL_CSPI_IRQn_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WL_CSPI_IRQn_PORT, &GPIO_InitStruct);

    ub_wlan_hal_csr_cspi_set_interrupt_enabled(false);

    /* Enable and set EXTI 15:10 Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    power_hal_init();
}


void
ub_wlan_hal_csr_cspi_set_power_on(bool on)
{
    power_hal_set_power_on(SUBSYSTEM_WLAN, on);

    if (on)
    {
        spi_hal_enable(current_clock_rate);
        HAL_GPIO_WritePin(WLAN_ENn_PORT, WLAN_ENn_PIN, WLAN_ENn_ENABLE);
    }
    else
    {
        HAL_GPIO_WritePin(WLAN_ENn_PORT, WLAN_ENn_PIN, WLAN_ENn_DISABLE);
        spi_hal_disable();
    }
}


void
ub_wlan_hal_csr_cspi_select(bool enable)
{
    if (enable)
    {
        HAL_GPIO_WritePin(WL_CSPI_CSn_PORT, WL_CSPI_CSn_PIN, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(WL_CSPI_CSn_PORT, WL_CSPI_CSn_PIN, GPIO_PIN_SET);
        ub_wlan_hal_csr_cspi_rw(0xFF);
    }
}


uint8_t
ub_wlan_hal_csr_cspi_rw(uint8_t write_data)
{
    spi_hal_enable(current_clock_rate);
    return spi_hal_rw(write_data);
}


void
ub_wlan_hal_csr_cspi_set_clock_rate(ub_wlan_hal_csr_cspi_clock_rate_t clock_rate)
{
    switch (clock_rate)
    {
    case UB_WLAN_HAL_CSR_CSPI_CLOCK_RATE_BOOT:
        /* Max rate: 12.5 MHz */
        current_clock_rate = SPI_HAL_CLOCK_12_5MHZ;
        break;

    case UB_WLAN_HAL_CSR_CSPI_CLOCK_RATE_FAST:
        /* Max rate: 50 MHz */
        current_clock_rate = SPI_HAL_CLOCK_50MHZ;
        break;

    case UB_WLAN_HAL_CSR_CSPI_CLOCK_RATE_SLOW:
        /* Max rate: 1 MHz */
        current_clock_rate = SPI_HAL_CLOCK_1MHZ;
        break;
    }

    spi_hal_set_clock_rate(current_clock_rate);
}


void
ub_wlan_hal_csr_cspi_set_interrupt_enabled(bool enable)
{
    if (enable)
        EXTI->IMR |= WL_CSPI_IRQn_PIN;
    else
        EXTI->IMR &= ~WL_CSPI_IRQn_PIN;
}


bool
ub_wlan_hal_csr_cspi_is_interrupt_asserted(void)
{
    return HAL_GPIO_ReadPin(WL_CSPI_IRQn_PORT, WL_CSPI_IRQn_PIN) == GPIO_PIN_RESET;
}
