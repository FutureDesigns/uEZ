/*-------------------------------------------------------------------------*
 * File:  Replay_TS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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
#include <HAL/GPIO.h>
#include <Device/ADCBank.h>
#include <Device/Touchscreen.h>
#include <Types/GPIO.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_Replay_Interface;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void Touchscreen_Replay_Create(
    const char *aName,
    const char *aExistingTS);

void Touchscreen_Replay_RecordStart(const char *aName, const char *aFilename);
void Touchscreen_Replay_PlayStart(const char *aName, const char *aFilename);
void Touchscreen_Replay_Stop(const char *aName);

/*-------------------------------------------------------------------------*
 * End of File:  Replay_TS.c
 *-------------------------------------------------------------------------*/
