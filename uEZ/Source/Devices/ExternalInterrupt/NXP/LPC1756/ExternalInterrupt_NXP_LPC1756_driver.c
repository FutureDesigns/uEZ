/*-------------------------------------------------------------------------*
 * File:  ExternalInterrupt_NXP_LPC1756.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the ExternalInterrupt for the PCA9555
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <uEZDeviceTable.h>
#include "ExternalInterrupt_NXP_LPC1756.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NUM_EXTERNAL_INTERRUPTS         4

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt8 iTrigger;
    void *iCallbackWorkspace;
    EINT_Callback iCallbackFunc;
    T_irqPriority iPriority;
} T_eintCallback;

typedef struct {
    const DEVICE_ExternalInterrupt *iDevice;
    T_uezSemaphore iSem;
} T_ExternalInterrupt_NXP_LPC1756_Workspace;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab(p)         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease(p)      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC1756_EINT0_Callback);
static IRQ_ROUTINE(LPC1756_EINT1_Callback);
static IRQ_ROUTINE(LPC1756_EINT2_Callback);
static IRQ_ROUTINE(LPC1756_EINT3_Callback);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_eintCallback G_eintCallbacks[NUM_EXTERNAL_INTERRUPTS];
const TISRFPtr G_eintFuncs[NUM_EXTERNAL_INTERRUPTS] = {
    LPC1756_EINT0_Callback,
    LPC1756_EINT1_Callback,
    LPC1756_EINT2_Callback,
    LPC1756_EINT3_Callback,
};

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9555.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_NXP_LPC1756_InitializeWorkspace(void *aW)
{
    T_uezError error;
    TUInt8 i;

    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
            (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // None are in use yet
    for (i=0; i<NUM_EXTERNAL_INTERRUPTS; i++)
        G_eintCallbacks[i].iCallbackFunc = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_EINTx_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      The following routines are called from the IRQ and is used to
 *      call the function.  NOTE:  This is still INSIDE the ISR.
 *---------------------------------------------------------------------------*/
static void ILPC1756_ProcessExternalInterrupt(TUInt8 aChannel)
{
    if (G_eintCallbacks[aChannel].iCallbackFunc(
            G_eintCallbacks[aChannel].iCallbackWorkspace,
            aChannel))
        SC->EXTINT = (1<<aChannel);
}
static IRQ_ROUTINE(LPC1756_EINT0_Callback)
{
    IRQ_START();
    ILPC1756_ProcessExternalInterrupt(0);
    IRQ_END();
}
static IRQ_ROUTINE(LPC1756_EINT1_Callback)
{
    IRQ_START();
    ILPC1756_ProcessExternalInterrupt(1);
    IRQ_END();
}
static IRQ_ROUTINE(LPC1756_EINT2_Callback)
{
    IRQ_START();
    ILPC1756_ProcessExternalInterrupt(2);
    IRQ_END();
}
static IRQ_ROUTINE(LPC1756_EINT3_Callback)
{
    IRQ_START();
    ILPC1756_ProcessExternalInterrupt(3);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_Set
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
T_uezError ExternalInterrupt_NXP_LPC1756_Set(
        void *aWorkspace,
        TUInt32 aChannel,
        T_EINTTrigger aTrigger,
        EINT_Callback aCallbackFunc,
        void *aCallbackWorkspace,
        T_EINTPriority aPriority,
        const char *aName)
{
    TUInt32 irqNum;
    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
        (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aWorkspace;
    T_eintCallback *p_callback;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt already in use?
        p_callback = G_eintCallbacks+aChannel;
        if (p_callback->iCallbackFunc) {
            error = UEZ_ERROR_ALREADY_EXISTS;
            break;
        }

        // Is this interrupt already registered?
        irqNum = EINT0_IRQn+aChannel;
        if (InterruptIsRegistered(irqNum)) {
            error = UEZ_ERROR_NOT_AVAILABLE;
            break;
        }

        // Interrupt is available.  Register it
        p_callback->iPriority = (T_irqPriority) aPriority;
        p_callback->iTrigger = aTrigger;
        p_callback->iCallbackWorkspace = aCallbackWorkspace;
        p_callback->iCallbackFunc = aCallbackFunc;

        // Start disabled
        InterruptDisable(irqNum);
        InterruptRegister(
            irqNum,
            G_eintFuncs[aChannel],
            (T_irqPriority) aPriority,
            aName);

        // Setup the proper mode (edge or level)
        if ((aTrigger == EINT_TRIGGER_EDGE_FALLING) || (aTrigger == EINT_TRIGGER_EDGE_RISING)) {
            // Edge triggered
            SC->EXTMODE |= (1 << aChannel);
        } else {
            // Level triggered
            SC->EXTMODE &= ~(1 << aChannel);
        }

        // Setup the proper polarity
        if ((aTrigger == EINT_TRIGGER_LEVEL_HIGH) || (aTrigger == EINT_TRIGGER_EDGE_RISING)) {
            // high or rising is 1
            SC->EXTPOLAR |= (1 << aChannel);
        } else {
            // low or falling is 0
            SC->EXTPOLAR &= ~(1 << aChannel);
        }

        // Make sure the external interrupt has been cleared
        SC->EXTINT = (1<<aChannel);

        // Do not loop
        break;
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Unregister the external interrupt handler.
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_NXP_LPC1756_Reset(
        void *aWorkspace,
        TUInt32 aChannel)
{
    TUInt32 irqNum;
    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
        (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aWorkspace;
    T_eintCallback *p_callback;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt already in cleared?
        p_callback = G_eintCallbacks+aChannel;
        if (!p_callback->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Unregister it
        irqNum = EINT0_IRQn+aChannel;
        InterruptDisable(irqNum);
        InterruptUnregister(irqNum);
        p_callback->iPriority = (T_irqPriority) 0;
        p_callback->iTrigger = 0;
        p_callback->iCallbackWorkspace = 0;
        p_callback->iCallbackFunc = 0;

        // Do not loop
        break;
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the flag for this external interrupt case
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_NXP_LPC1756_Clear(
        void *aWorkspace,
        TUInt32 aChannel)
{
    TUInt32 irqNum;
    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
        (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aWorkspace;
    T_eintCallback *p_callback;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_callback = G_eintCallbacks+aChannel;
        if (!p_callback->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = EINT0_IRQn+aChannel;
        InterruptDisable(irqNum);

        // Do not loop
        break;
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable the given external interrupt
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_NXP_LPC1756_Disable(
        void *aWorkspace,
        TUInt32 aChannel)
{
    TUInt32 irqNum;
    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
        (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aWorkspace;
    T_eintCallback *p_callback;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_callback = G_eintCallbacks+aChannel;
        if (!p_callback->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = EINT0_IRQn+aChannel;
        InterruptDisable(irqNum);

        // Do not loop
        break;
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_NXP_LPC1756_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the given external interrupt
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 3)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_NXP_LPC1756_Enable(
        void *aWorkspace,
        TUInt32 aChannel)
{
    TUInt32 irqNum;
    T_ExternalInterrupt_NXP_LPC1756_Workspace *p =
        (T_ExternalInterrupt_NXP_LPC1756_Workspace *)aWorkspace;
    T_eintCallback *p_callback;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_callback = G_eintCallbacks+aChannel;
        if (!p_callback->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = EINT0_IRQn+aChannel;
        InterruptEnable(irqNum);

        // Do not loop
        break;
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Creation routines:
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Generic_Create(const char *aName)
{
    T_uezDeviceWorkspace *p;

    // Initialize the DAC0 device
    return UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&ExternalInterrupt_NXP_LPC1756_Interface, 0, &p);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ExternalInterrupt ExternalInterrupt_NXP_LPC1756_Interface = {
	{
	    // Common device interface
	    "EINT:NXP:LPC1756",
	    0x0100,
	    ExternalInterrupt_NXP_LPC1756_InitializeWorkspace,
	    sizeof(T_ExternalInterrupt_NXP_LPC1756_Workspace),
	},
	
    // Functions
    ExternalInterrupt_NXP_LPC1756_Set,
    ExternalInterrupt_NXP_LPC1756_Reset,
    ExternalInterrupt_NXP_LPC1756_Clear,
    ExternalInterrupt_NXP_LPC1756_Disable,
    ExternalInterrupt_NXP_LPC1756_Enable,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ExternalInterrupt_NXP_LPC1756.c
 *-------------------------------------------------------------------------*/
