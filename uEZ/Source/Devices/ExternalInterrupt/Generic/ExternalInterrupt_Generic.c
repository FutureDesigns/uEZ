/*-------------------------------------------------------------------------*
 * File:  ExternalInterrupt_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the ExternalInterrupt for the PCA9555
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <HAL/HAL.h>
#include <HAL/ExternalInterrupt.h>
#include "ExternalInterrupt_Generic.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ExternalInterrupt *iDevice;
    HAL_ExternalInterrupt **iHAL;
    T_uezSemaphore iSem;
} T_ExternalInterrupt_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab(p)         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease(p)      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9555.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iHAL = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the rate of a given blink register (as close as possible).
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 *      TUInt8 aTrigger             -- Trigger type (if used)
 *      EINTCallback aCallbackFunc  -- Function to call when trigger.
 *      void *aCallbackWorkspace    -- Parameter to pass to callback function.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Set(
    void *aWorkspace,
    TUInt32 aChannel,
    T_EINTTrigger aTrigger,
    EINT_Callback aCallbackFunc,
    void *aCallbackWorkspace,
    T_EINTPriority aPriority,
    const char *aName)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = (*p->iHAL)->Set(p->iHAL, aChannel, aTrigger, aCallbackFunc,
        aCallbackWorkspace, aPriority, aName);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Unregister the external interrupt handler.
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Reset(void *aWorkspace, TUInt32 aChannel)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = (*p->iHAL)->Reset(p->iHAL, aChannel);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the flag for this external interrupt case
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Clear(void *aWorkspace, TUInt32 aChannel)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = (*p->iHAL)->Clear(p->iHAL, aChannel);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable the given external interrupt
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Disable(void *aWorkspace, TUInt32 aChannel)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = (*p->iHAL)->Disable(p->iHAL, aChannel);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the given external interrupt
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Enable(void *aWorkspace, TUInt32 aChannel)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = (*p->iHAL)->Enable(p->iHAL, aChannel);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the given external interrupt
 * Inputs:
 *      void *aWorkspace             -- Workspace to configure
 *      const char *aName            -- Name of External Interrupt HAL driver
 *                                      to link to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Configure(
    void *aWorkspace,
    const char *aHALName)
{
    T_ExternalInterrupt_Generic_Workspace *p =
        (T_ExternalInterrupt_Generic_Workspace *)aWorkspace;

    // Just link the driver
    return HALInterfaceFind(aHALName, (T_halWorkspace **)&p->iHAL);
}

/*---------------------------------------------------------------------------*
 * Creation routines:
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Create(
    const char *aName, 
    const char *aHALName)
{
    T_uezDeviceWorkspace *p;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&ExternalInterrupt_Generic_Interface, 0, &p);
    return ExternalInterrupt_Generic_Configure(p, aHALName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ExternalInterrupt ExternalInterrupt_Generic_Interface = { {
// Common device interface
    "EINT:Generic",
    0x0100,
    ExternalInterrupt_Generic_InitializeWorkspace,
    sizeof(T_ExternalInterrupt_Generic_Workspace), },

// Functions
    ExternalInterrupt_Generic_Set,
    ExternalInterrupt_Generic_Reset,
    ExternalInterrupt_Generic_Clear,
    ExternalInterrupt_Generic_Disable,
    ExternalInterrupt_Generic_Enable, };

/*-------------------------------------------------------------------------*
 * End of File:  ExternalInterrupt_Generic.c
 *-------------------------------------------------------------------------*/
