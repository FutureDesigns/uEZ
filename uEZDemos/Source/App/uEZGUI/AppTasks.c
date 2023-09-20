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
#include <uEZProcessor.h>
#include <uEZDemoCommon.h>
#include "Audio.h"
#include <uEZToneGenerator.h>
#include <uEZAudioMixer.h>
#include "AppTasks.h"
#include <NVSettings.h>
#include <HAL/GPIO.h>
#include <Config_Build.h>
#if (UEZ_ENABLE_TCPIP_STACK == 1)
#include <uEZNetwork.h>
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <NetworkStartup.h>
#include <AppHTTPServer.h>
#endif

#define HEARTBEAT_BLINK_DELAY			 250

T_uezTask G_heartBeatTask;

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
    UEZGPIOOutput(GPIO_HEARTBEAT_LED);
    UEZGPIOSetMux(GPIO_HEARTBEAT_LED, 0);

    // initial quick blink at bootup
    for (int i = 0; i < 20; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
    }

    // Blink
    while(1) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
    }
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

#if APP_ENABLE_HEARTBEAT_LED_ON
    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 128, (void *)0, UEZ_PRIORITY_NORMAL, &G_heartBeatTask);
#endif
#if (UEZ_SPEAKER_TEST == 1)
    UEZTaskCreate(SpkrTestContinuous, "Tone", 256, (void *)0, UEZ_PRIORITY_NORMAL, 0);
#endif

#if (UEZ_ENABLE_TCPIP_STACK == 1)
    error = UEZTaskCreate(
                (T_uezTaskFunction)NetworkStartup,
                "NetStart",
                UEZ_TASK_STACK_BYTES(1024),
                (void *)0,
                UEZ_PRIORITY_NORMAL,
                0);
#endif

    return error;
}

#ifndef UEZ_SPEAKER_TEST_HZ 
#define UEZ_SPEAKER_TEST_HZ 1600 // Maximum power output for BeStar Speaker
#endif
/*---------------------------------------------------------------------------*
 * Task:  Speaker Test
 *---------------------------------------------------------------------------*
 * Description:
 *      Continuous Speaker Test at Constant Hz Tone
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 SpkrTestContinuous(T_uezTask aMyTask, void *aParams)
{
    UEZTaskDelay(7500);
    for (;;)
    {
        if (G_mmTestMode == EFalse)
        {
            UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
            
            while (G_mmTestMode == EFalse) {
                PlayAudioContinuous(UEZ_SPEAKER_TEST_HZ);
                UEZTaskDelay(1000);
            }
            
            UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
        }
        else
        {
          while (G_mmTestMode == ETrue) {
            UEZTaskDelay(10000);
          }
        }
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.c
 *-------------------------------------------------------------------------*/
