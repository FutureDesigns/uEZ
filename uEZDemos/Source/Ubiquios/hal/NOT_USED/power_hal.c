/*
 * ubiquiOS porting layer for STM32 Nucleo: Power control
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"
#include "stm32f4xx_hal.h"

#include "power_hal.h"

/* PWR_EN       PA0 */
#define PWR_EN_PORT             GPIOA
#define PWR_EN_PIN              GPIO_PIN_0
#define PWR_EN_ENABLE           SET
#define PWR_EN_DISABLE          RESET


void
power_hal_init(void)
{
    static bool initialised = false;
    GPIO_InitTypeDef GPIO_InitStruct;

    if (initialised)
        return;

    /* GPIOs */
    __GPIOA_CLK_ENABLE();

    /* Configure PWR_EN as output */
    GPIO_InitStruct.Pin = PWR_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(PWR_EN_PORT, &GPIO_InitStruct);

    initialised = true;
}


void
power_hal_set_power_on(power_hal_subsystem_t subsystem, bool on)
{
    static unsigned subsystem_power_state = 0;

    if (on)
        subsystem_power_state |= (unsigned)subsystem;
    else
        subsystem_power_state &= ~((unsigned)subsystem);

    /* If power is enabled for any subsystem set the PWR_EN pin to enabled;
     * if power is disabled for all subsystems set the PWR_EN pin to disabled.
     */
    if (subsystem_power_state)
        HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, PWR_EN_ENABLE);
    else
        HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, PWR_EN_DISABLE);
}
