/*-------------------------------------------------------------------------*
 * File:  AppTasks.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Various tasks to possibly run.  Examples if nothing else.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZNetwork.h>
#include <uEZProcessor.h>
#include "AppTasks.h"
#include <NVSettings.h>
#include <HAL/GPIO.h>
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <NetworkStartup.h>
#include <AppHTTPServer.h>

#ifndef HEARTBEATLED // new default on all LPC1788/4088 uEZGUIs
#define HEARTBEAT_PORT                  "GPIO1"
#define HEARTBEATLED                     13
#endif
/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
#if (UEZ_PROCESSOR != NXP_LPC4357)
    HAL_GPIOPort **p_gpio;
    TUInt8 heartbeatLED = HEARTBEATLED;

    HALInterfaceFind(HEARTBEAT_PORT, (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetOutputMode(p_gpio, 1 << heartbeatLED);
    (*p_gpio)->SetMux(p_gpio, heartbeatLED, 0); // set to GPIO
    // Blink
    for (;;) {
        (*p_gpio)->Set(p_gpio, 1 << heartbeatLED);
        UEZTaskDelay(250);
        (*p_gpio)->Clear(p_gpio, 1 << heartbeatLED);
        UEZTaskDelay(250);
    }
#else
    TUInt32 blinkrate = 250;

    UEZGPIOOutput(GPIO_P0_11);

    // Blink
    for (;;) {
        UEZGPIOSet(GPIO_P0_11);
        UEZTaskDelay(blinkrate);
        UEZGPIOClear(GPIO_P0_11);
        UEZTaskDelay(blinkrate);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
T_uezError SetupTasks(void)
{
    T_uezError error = UEZ_ERROR_NONE;

#if (APP_ENABLE_HEARTBEAT_LED == 1)
    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
#endif

    error = UEZTaskCreate(
                (T_uezTaskFunction)NetworkStartup,
                "NetStart",
                UEZ_TASK_STACK_BYTES(1024),
                (void *)0,
                UEZ_PRIORITY_NORMAL,
                0);

    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.c
 *-------------------------------------------------------------------------*/
