/*-------------------------------------------------------------------------*
 * File:  Button_Generic_EINT_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the ButtonBank for the EINT
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <uEZ.h>
#include <Device/ButtonBank.h>
#include <Device/I2CBus.h>
#include "Button_Generic_EINT.h"
#include <UEZGPIO.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ButtonBank *iDevice;
    T_uezSemaphore iSem;
    TUInt32 iButtonsActive;
    TUInt8 iButtonsPressed[UEZ_BUTTON_GENERIC_EINT_MAX_NUM_BUTTONS];
} T_Button_Generic_EINT_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Button_Generic_EINT_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for EINT.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_Generic_EINT_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Button_Generic_EINT_Workspace *p = (T_Button_Generic_EINT_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    memset(p->iButtonsPressed, 0, sizeof(p->iButtonsPressed));
    p->iButtonsActive = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_Generic_EINT_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current EINT ButtonBank reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_Generic_EINT_SetActiveButtons(
    void *aWorkspace,
    TUInt32 aButtonsActive)
{
    T_Button_Generic_EINT_Workspace *p =
        (T_Button_Generic_EINT_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iButtonsActive = aButtonsActive;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_Generic_EINT_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read which buttons are pressed since the last read
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 *aButtons           -- Buttons pressed
 *---------------------------------------------------------------------------*/
T_uezError Button_Generic_EINT_Read(void *aWorkspace, TUInt32 *aButtons)
{
    T_Button_Generic_EINT_Workspace *p =
        (T_Button_Generic_EINT_Workspace *)aWorkspace;
    TUInt32 i;
    TUInt32 bit;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    RTOS_ENTER_CRITICAL();
    *aButtons = 0;

    // Reconstruct the response
    for (i = 0; i < UEZ_BUTTON_GENERIC_EINT_MAX_NUM_BUTTONS; i++) {
        bit = 1 << i;
        if ((p->iButtonsActive & bit) && (p->iButtonsPressed[i])) {
            *aButtons |= bit;
            p->iButtonsPressed[i] = 0;
        }
    }
    RTOS_EXIT_CRITICAL();
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

static TBool IGenericButtonEINTSwitchPressed(void *aWorkspace, TUInt32 aChannel)
{
    PARAM_NOT_USED(aChannel);

    // The workspace, in this case, is a pointer to the iButtonsPressed byte
    // Just set it to 1.
    *((TUInt8 *)aWorkspace) = 1;
    return ETrue;
}

void ButtonBank_Generic_EINT_Configure(
    void *aWorkspace,
    const T_Button_EINT_Source *aSourceArray,
    const TUInt32 aNumSources)
{
    T_Button_Generic_EINT_Workspace *p =
        (T_Button_Generic_EINT_Workspace *)aWorkspace;
    const T_Button_EINT_Source *p_source = aSourceArray;
    TUInt32 i;
    T_uezDevice eint;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    for (i = 0; i < aNumSources; i++, p_source++) {
        UEZEINTOpen(p_source->iEINTName, &eint, p_source->iEINTChannel,
            p_source->aTrigger, p_source->aPriority,
            IGenericButtonEINTSwitchPressed, (void *)(&p->iButtonsPressed[i]));
        UEZEINTClear(eint, p_source->iEINTChannel);
        UEZEINTEnable(eint, p_source->iEINTChannel);
    }
    UEZSemaphoreRelease(p->iSem);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ButtonBank ButtonBank_Generic_EINT_Interface = { {
// Common device interface
    "ButtonBank:Generic:EINT",
    0x0100,
    Button_Generic_EINT_InitializeWorkspace,
    sizeof(T_Button_Generic_EINT_Workspace), },

// Functions
    Button_Generic_EINT_SetActiveButtons,
    Button_Generic_EINT_Read, };

/*-------------------------------------------------------------------------*
 * End of File:  Button_Generic_EINT_driver.c
 *-------------------------------------------------------------------------*/
