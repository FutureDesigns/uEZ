/*
 * ubiquiOS porting layer for uEZ: System Time
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"
#include <uEZ.h>


/**
 * Return the current time in milliseconds
 */
uint32_t
ub_time_hal_time_now_milliseconds(void)
{
    return UEZTickCounterGet();
}

/**
 * This function is invoked by the ubiquiOS core to initialize the
 * time HAL. Here we simply setup our Ticker object to give a 1 kHz
 * tick, and attach \c millisecond_tick() as our handler.
 */
void
ub_time_hal_init(void)
{
    //Handled by the uEZRTOS
}
