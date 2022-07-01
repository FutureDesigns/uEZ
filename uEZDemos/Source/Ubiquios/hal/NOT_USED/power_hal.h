/*
 * ubiquiOS porting layer for STM32 Nucleo: Power control
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#ifndef POWER_HAL_H__
#define POWER_HAL_H__

#include "ubiquios.h"


/**
 * Initialise the power control HAL.
 */
extern void
power_hal_init(void);


/**
 * Enumeration of subsystems that use the power HAL.
 */
typedef enum power_hal_subsystem_t
{
    SUBSYSTEM_WLAN           = 0x01,
    SUBSYSTEM_BT             = 0x02,
} power_hal_subsystem_t;


/**
 * Power on/off request for a given subsystem.  Note that power will
 * remain on as long as at least one subsystem requests it to be on.
 */
extern void
power_hal_set_power_on(power_hal_subsystem_t subsystem, bool on);


#endif
