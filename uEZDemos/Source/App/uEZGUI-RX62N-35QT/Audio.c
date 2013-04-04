/*-------------------------------------------------------------------------*
 * File:  Audio.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Routines for handling audio.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <uEZ.h>
#include "AppDemo.h"
#include "Audio.h"
#include <uEZToneGenerator.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define OPTION_USE_GPIO_LINES_FOR_AUDIO     0 // no longer used

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static volatile TUInt32 G_dummyCounter = 0;
T_uezDevice G_tg = 0;

/*---------------------------------------------------------------------------*
 * Routine:  CalibrateAudioTiming
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a rough LPC2478 specific calibration method to determine
 *      how fast to control the lines to the audio.  Yes, this is crude.
 *---------------------------------------------------------------------------*/
void CalibrateAudioTiming(void)
{
    if (UEZToneGeneratorOpen("Speaker", &G_tg) != UEZ_ERROR_NONE)
        G_tg = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  PlayAudio
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a tone for the given length, staying here until done.
 * Inptus:
 *      TUInt32 aHz                 -- Tone in Hz
 *      TUInt32 aMS                 -- Duration of tone
 *---------------------------------------------------------------------------*/
void PlayAudio(TUInt32 aHz, TUInt32 aMS)
{

#if OPTION_USE_GPIO_LINES_FOR_AUDIO
    static HAL_GPIOPort **p_gpio0;
    TUInt32 n;
    TUInt32 start = UEZTickCounterGet();
    TUInt32 count;

    HALInterfaceFind("GPIO0", (T_halWorkspace **)&p_gpio0);
    (*p_gpio0)->SetOutputMode(p_gpio0, (1<<26));

    // Wait for the parameters to be ready
    n = (G_subTickCount*1000/2)/aHz;
    taskDISABLE_INTERRUPTS();
    G_dummyCounter = 0;
    // Wait for first tick count
    while ((T1IR&2)==0)
        {}
    while (aMS--) {
        T1IR = 2;
        while (!(T1IR & 2)) {
            count = n;
            while (count--)
                { G_dummyCounter++; }
            (*p_gpio0)->Clear(p_gpio0, (1<<26));
            count = n;
            while (count--)
                { G_dummyCounter++; }
            (*p_gpio0)->Set(p_gpio0, (1<<26));
        }
    }

    taskENABLE_INTERRUPTS();
#else
    if (!G_tg)
        CalibrateAudioTiming();
    //PWMAudio(aHz, (aMS+10)/10, 0);
    if (G_tg)
        UEZToneGeneratorPlayTone(
                G_tg,
                TONE_GENERATOR_HZ(aHz),
                aMS);
    else
        UEZTaskDelay(aMS);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  ButtonClick
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a standard button click sound
 *---------------------------------------------------------------------------*/
void ButtonClick(void)
{
    PlayAudio(250, 20);
}

/*---------------------------------------------------------------------------*
 * Routine:  BeepError
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a beep for errors.
 *---------------------------------------------------------------------------*/
void BeepError(void)
{
    PlayAudio(100, 500);
}

/*---------------------------------------------------------------------------*
 * Routine:  Beep
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a standard beep.
 *---------------------------------------------------------------------------*/
void Beep(void)
{
    // Beep at 500 Hz for 250 ms
    PlayAudio(500, 250);
}

/*-------------------------------------------------------------------------*
 * End of File:  Audio.c
 *-------------------------------------------------------------------------*/
