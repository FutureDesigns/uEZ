/*-------------------------------------------------------------------------*
 * File:  GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1768 GPIO Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_GPIO.h>
#include <HAL/Interrupt.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 *iPINSELReg1;
    TVUInt32 *iPINSELReg2;
    TVUInt32 *iPINMODEReg1;
    TVUInt32 *iPINMODEReg2;
    TVUInt32 *iIOPIN;
    TVUInt32 *iIOSET;
    TVUInt32 *iIOCLR;
    TVUInt32 *iIODIR;

    // uEZ v2.02
    TVUInt32 *iPINMODE_ODReg;
    TVUInt32 *iIOIntEnR;
    TVUInt32 *iIOIntEnF;
    TVUInt32 *iIOIntStatR;
    TVUInt32 *iIOIntStatF;
    TVUInt32 *iIOIntClr;
} LPC1768_GPIO_PortInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const LPC1768_GPIO_PortInfo *iPortInfo;

    // uEZ v2.02
    TUInt32 iLocked;
    T_gpioInterruptHandler iInterruptCallback;
    void *iInterruptCallbackWorkspace;
} T_LPC1768_GPIO_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const LPC1768_GPIO_PortInfo GPIO_LPC1768_Port0_PortInfo = {
    (TVUInt32 *)&LPC_PINCON->PINSEL0,
    (TVUInt32 *)&LPC_PINCON->PINSEL1,
    (TVUInt32 *)&LPC_PINCON->PINMODE0,
    (TVUInt32 *)&LPC_PINCON->PINMODE1,
    (TVUInt32 *)&LPC_GPIO0->FIOPIN,
    (TVUInt32 *)&LPC_GPIO0->FIOSET,
    (TVUInt32 *)&LPC_GPIO0->FIOCLR,
    (TVUInt32 *)&LPC_GPIO0->FIODIR,
    (TVUInt32 *)&LPC_PINCON->PINMODE_OD0,
    (TVUInt32 *)&LPC_GPIOINT->IO0IntEnR,
    (TVUInt32 *)&LPC_GPIOINT->IO0IntEnF,
    (TVUInt32 *)&LPC_GPIOINT->IO0IntStatR,
    (TVUInt32 *)&LPC_GPIOINT->IO0IntStatF,
    (TVUInt32 *)&LPC_GPIOINT->IO0IntClr,
};

const LPC1768_GPIO_PortInfo GPIO_LPC1768_Port1_PortInfo = {
    (TVUInt32 *)&LPC_PINCON->PINSEL2,
    (TVUInt32 *)&LPC_PINCON->PINSEL3,
    (TVUInt32 *)&LPC_PINCON->PINMODE2,
    (TVUInt32 *)&LPC_PINCON->PINMODE3,
    (TVUInt32 *)&LPC_GPIO1->FIOPIN,
    (TVUInt32 *)&LPC_GPIO1->FIOSET,
    (TVUInt32 *)&LPC_GPIO1->FIOCLR,
    (TVUInt32 *)&LPC_GPIO1->FIODIR,
    (TVUInt32 *)&LPC_PINCON->PINMODE_OD1,
    0,
    0,
    0,
    0,
    0,
};

const LPC1768_GPIO_PortInfo GPIO_LPC1768_Port2_PortInfo = {
    (TVUInt32 *)&LPC_PINCON->PINSEL4,
    (TVUInt32 *)&LPC_PINCON->PINSEL5,
    (TVUInt32 *)&LPC_PINCON->PINMODE4,
    (TVUInt32 *)&LPC_PINCON->PINMODE5,
    (TVUInt32 *)&LPC_GPIO2->FIOPIN,
    (TVUInt32 *)&LPC_GPIO2->FIOSET,
    (TVUInt32 *)&LPC_GPIO2->FIOCLR,
    (TVUInt32 *)&LPC_GPIO2->FIODIR,
    (TVUInt32 *)&LPC_PINCON->PINMODE_OD2,
    (TVUInt32 *)&LPC_GPIOINT->IO2IntEnR,
    (TVUInt32 *)&LPC_GPIOINT->IO2IntEnF,
    (TVUInt32 *)&LPC_GPIOINT->IO2IntStatR,
    (TVUInt32 *)&LPC_GPIOINT->IO2IntStatF,
    (TVUInt32 *)&LPC_GPIOINT->IO2IntClr,
};

const LPC1768_GPIO_PortInfo GPIO_LPC1768_Port3_PortInfo = {
    (TVUInt32 *)&LPC_PINCON->PINSEL6,
    (TVUInt32 *)&LPC_PINCON->PINSEL7,
    (TVUInt32 *)&LPC_PINCON->PINMODE6,
    (TVUInt32 *)&LPC_PINCON->PINMODE7,
    (TVUInt32 *)&LPC_GPIO3->FIOPIN,
    (TVUInt32 *)&LPC_GPIO3->FIOSET,
    (TVUInt32 *)&LPC_GPIO3->FIOCLR,
    (TVUInt32 *)&LPC_GPIO3->FIODIR,
    (TVUInt32 *)&LPC_PINCON->PINMODE_OD3,
    0,
    0,
    0,
    0,
    0,
};

const LPC1768_GPIO_PortInfo GPIO_LPC1768_Port4_PortInfo = {
    (TVUInt32 *)&LPC_PINCON->PINSEL8,
    (TVUInt32 *)&LPC_PINCON->PINSEL9,
    (TVUInt32 *)&LPC_PINCON->PINMODE8,
    (TVUInt32 *)&LPC_PINCON->PINMODE9,
    (TVUInt32 *)&LPC_GPIO4->FIOPIN,
    (TVUInt32 *)&LPC_GPIO4->FIOSET,
    (TVUInt32 *)&LPC_GPIO4->FIOCLR,
    (TVUInt32 *)&LPC_GPIO4->FIODIR,
    (TVUInt32 *)&LPC_PINCON->PINMODE_OD4,
    0,
    0,
    0,
    0,
    0,
};

static T_LPC1768_GPIO_Workspace *G_LPC1768Port0Workspace = 0;
static T_LPC1768_GPIO_Workspace *G_LPC1768Port2Workspace = 0;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void LPC1768_GPIO_ProcessIRQ(T_LPC1768_GPIO_Workspace *p);

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Port0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1768_Port0_PortInfo;
    p->iLocked = 0;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC1768Port0Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Port1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPIO Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Port1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1768_Port1_PortInfo;
    p->iLocked = 0;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Port2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPIO Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Port2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1768_Port2_PortInfo;
    p->iLocked = 0;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC1768Port2Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Port3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPIO Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Port3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1768_Port3_PortInfo;
    p->iLocked = 0;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Port4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPIO Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Port4_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1768_Port4_PortInfo;
    p->iLocked = 0;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 *      DEPRECATED
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Activate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are active as GPIO pins.
 *      Once active, the pins are set to GPIO inputs or outputs.
 *      DEPRECATED
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 *      DEPRECATED
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iIODIR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iIODIR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOSET) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOCLR) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a group of GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 *      TUInt32 *aPinsRead          -- Pointer to pins result.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_Read(
            void *aWorkspace,
            TUInt32 aPortPins,
            TUInt32 *aPinsRead)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Get the bits
    *aPinsRead = ((*p_info->iIOPIN) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_SetPull(
            void *aWorkspace,
            TUInt8 aPortPinIndex,
            T_gpioPull aPull)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Control the pull up/down/none register for this pin
    if (aPortPinIndex < 16) {
        aPortPinIndex <<= 1;
        *p_info->iPINMODEReg1 =
            (*p_info->iPINMODEReg1 &
                (~(3<<aPortPinIndex))) |
                (aPull << aPortPinIndex);
    } else {
        aPortPinIndex -= 16;
        aPortPinIndex <<= 1;
        *p_info->iPINMODEReg2 =
            (*p_info->iPINMODEReg2 &
                (~(3<<aPortPinIndex))) |
                (aPull << aPortPinIndex);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_SetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_SetMux(
            void *aWorkspace,
            TUInt8 aPortPinIndex,
            T_gpioMux aMux)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;
    TUInt32 shift;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;
    if (aMux >= 4)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Setup the pin connect block (pins 0-31)
    if (aPortPinIndex < 16) {
        // Pins 0-15
        shift = (aPortPinIndex<<1);
        (*p_info->iPINSELReg1) &= ~(3 << shift);
        (*p_info->iPINSELReg1) |= aMux << shift;
    } else {
        // Pins 16-31
        shift = ((aPortPinIndex-16)<<1);
        (*p_info->iPINSELReg2) &= ~(3 << shift);
        (*p_info->iPINSELReg2) |= aMux << shift;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_GetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Get pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 *      T_gpioMux aMux              -- Mux setting being used
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;
    TUInt32 shift;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Decode the pin connect block (pins 0-31) into 2 bit pairs
    if (aPortPinIndex < 16) {
        // Pins 0-15
        shift = (aPortPinIndex<<1);
        *aMux = ((*p_info->iPINSELReg1) >> shift) & 3;
    } else {
        // Pins 16-31
        shift = ((aPortPinIndex-16)<<1);
        *aMux = ((*p_info->iPINSELReg2) >> shift) & 3;
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC1768_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    PARAM_NOT_USED(aValue);

    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            error = UEZ_ERROR_NONE;
            break;
        case GPIO_CONTROL_ENABLE_OPEN_DRAIN:
            (*p_info->iPINMODE_ODReg) |= ~(1<<aPortPinIndex);
            break;
        case GPIO_CONTROL_DISABLE_OPEN_DRAIN:
            (*p_info->iPINMODE_ODReg) &= ~(1<<aPortPinIndex);
            break;
        case GPIO_CONTROL_SET_CONFIG_BITS:
            // Do several controls at the same time
            error = LPC1768_GPIO_SetMux(p, aPortPinIndex, aValue & 3);
            if (error)
                return error;
            error = LPC1768_GPIO_SetPull(p, aPortPinIndex,
                (T_gpioPull)((aValue >> 2) & 7));
            if (error)
                return error;
            if (aValue & IOCON_OPEN_DRAIN)
                error = LPC1768_GPIO_Control(p, aPortPinIndex,
                    GPIO_CONTROL_ENABLE_OPEN_DRAIN, 0);
            else
                error = LPC1768_GPIO_Control(p, aPortPinIndex,
                    GPIO_CONTROL_DISABLE_OPEN_DRAIN, 0);
            break;
        default:
            error = UEZ_ERROR_NOT_SUPPORTED;
            break;
    }

    return error;
}

T_uezError LPC1768_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
#ifndef NDEBUG
    char buffer [50];
#endif
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        #ifndef NDEBUG
          sprintf (buffer, "PinLock on %s Pin %d", p->iHAL->iInterface.iName, aPortPins);
          UEZFailureMsg(buffer);
        #else
          UEZFailureMsg("PinLock");
        #endif
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError LPC1768_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1768_GPIO_IRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO interrupts have fired off, do the right one.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC1768_GPIO_IRQ)
{
#define IOIntStatus_P0Int       (1<<0)
#define IOIntStatus_P2Int       (1<<2)

    IRQ_START();

    // It's port 0 and/or port 2, is this port 0 firing?
    if (LPC_GPIOINT->IntStatus & IOIntStatus_P0Int)
        LPC1768_GPIO_ProcessIRQ(G_LPC1768Port0Workspace);
    if (LPC_GPIOINT->IntStatus & IOIntStatus_P2Int)
        LPC1768_GPIO_ProcessIRQ(G_LPC1768Port2Workspace);

    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_SetInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the GPIO's interrupt mode (enable if not already enabled)
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      T_gpioInterruptHandler iInterruptCallback -- Routine to call on
 *                                      interrupt.
 *      void *iInterruptCallbackWorkspace -- Workspace of callback
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_ConfigureInterruptCallback(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;

    // Register the interrupt handler if not already registered
    InterruptRegister(EINT3_IRQn, LPC1768_GPIO_IRQ, INTERRUPT_PRIORITY_HIGH,
            "GPIOIrq");
    InterruptEnable(EINT3_IRQn);

    p->iInterruptCallback = aInterruptCallback;
    p->iInterruptCallbackWorkspace = aInterruptCallbackWorkspace;

    return UEZ_ERROR_NONE;
}

T_uezError LPC1768_GPIO_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_ProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO ports is reporting an IRQ.  This processes one
 *      of the ports for falling or rising edge events.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC1768_GPIO_ProcessIRQ(T_LPC1768_GPIO_Workspace *p)
{
    TUInt32 regF;
    TUInt32 regR;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    // Make sure we have a handler
    if (!p->iInterruptCallback)
        InterruptFatalError();

    // Do we have any falling edge interrupts?
    regF = *(p_info->iIOIntStatF);
    regR = *(p_info->iIOIntStatR);
    if (regF)
        p->iInterruptCallback(p->iInterruptCallbackWorkspace, regF,
                GPIO_INTERRUPT_FALLING_EDGE);

    // Do we have any rising edge interrupts?
    if (regR)
        p->iInterruptCallback(p->iInterruptCallbackWorkspace, regR,
                GPIO_INTERRUPT_RISING_EDGE);

    // Now clear all the interrupts processed above
    // (regardless of enabled/disabled state)
    *(p_info->iIOIntClr) = regF | regR;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_EnableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to enable.
 *      T_gpioInterruptType aType   -- Type of interrupts to enable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_EnableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) |= (1 << (aPortPins & 0xFF));
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) |= (1 << (aPortPins & 0xFF));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_DisableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *      T_gpioInterruptType aType   -- Type of interrupts to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_DisableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) &= ~(1 << (aPortPins & 0xFF));
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) &= ~(1 << (aPortPins & 0xFF));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPIO_ClearInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear interrupts on a specific GPIO line
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPIO_ClearInterrupts(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1768_GPIO_Workspace *p = (T_LPC1768_GPIO_Workspace *)aWorkspace;
    const LPC1768_GPIO_PortInfo *p_info = p->iPortInfo;

    (*p_info->iIOIntClr) = (1 << (aPortPins & 0xFF));

    return UEZ_ERROR_NONE;
}

T_uezError LPC1768_GPIO_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC1768_GPIO_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC1768_GPIO_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_LPC1768_Port0_Interface = {
    {
    "LPC1768 GPIO Port 0",
    0x0100,
    LPC1768_GPIO_Port0_InitializeWorkspace,
    sizeof(T_LPC1768_GPIO_Workspace),
    },

    LPC1768_GPIO_Configure,
    LPC1768_GPIO_Activate,
    LPC1768_GPIO_Deactivate,
    LPC1768_GPIO_SetOutputMode,
    LPC1768_GPIO_SetInputMode,
    LPC1768_GPIO_Set,
    LPC1768_GPIO_Clear,
    LPC1768_GPIO_Read,
    LPC1768_GPIO_SetPull,
    LPC1768_GPIO_SetMux,

    // uEZ v2.02
    LPC1768_GPIO_ConfigureInterruptCallback,
    LPC1768_GPIO_EnableInterrupts,
    LPC1768_GPIO_DisableInterrupts,
    LPC1768_GPIO_ClearInterrupts,
    LPC1768_GPIO_Control,
    LPC1768_GPIO_Lock,
    LPC1768_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC1768_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1768_Port1_Interface = {
    {
    "LPC1768 GPIO Port 1",
    0x0100,
    LPC1768_GPIO_Port1_InitializeWorkspace,
    sizeof(T_LPC1768_GPIO_Workspace),
    },

    LPC1768_GPIO_Configure,
    LPC1768_GPIO_Activate,
    LPC1768_GPIO_Deactivate,
    LPC1768_GPIO_SetOutputMode,
    LPC1768_GPIO_SetInputMode,
    LPC1768_GPIO_Set,
    LPC1768_GPIO_Clear,
    LPC1768_GPIO_Read,
    LPC1768_GPIO_SetPull,
    LPC1768_GPIO_SetMux,

    // uEZ v2.02
    LPC1768_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC1768_GPIO_EnableInterrupts_NotSupported,
    LPC1768_GPIO_DisableInterrupts_NotSupported,
    LPC1768_GPIO_ClearInterrupts_NotSupported,
    LPC1768_GPIO_Control,
    LPC1768_GPIO_Lock,
    LPC1768_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC1768_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1768_Port2_Interface = {
    {
    "LPC1768 GPIO Port 2",
    0x0100,
    LPC1768_GPIO_Port2_InitializeWorkspace,
    sizeof(T_LPC1768_GPIO_Workspace),
    },

    LPC1768_GPIO_Configure,
    LPC1768_GPIO_Activate,
    LPC1768_GPIO_Deactivate,
    LPC1768_GPIO_SetOutputMode,
    LPC1768_GPIO_SetInputMode,
    LPC1768_GPIO_Set,
    LPC1768_GPIO_Clear,
    LPC1768_GPIO_Read,
    LPC1768_GPIO_SetPull,
    LPC1768_GPIO_SetMux,

    // uEZ v2.02
    LPC1768_GPIO_ConfigureInterruptCallback,
    LPC1768_GPIO_EnableInterrupts,
    LPC1768_GPIO_DisableInterrupts,
    LPC1768_GPIO_ClearInterrupts,
    LPC1768_GPIO_Control,
    LPC1768_GPIO_Lock,
    LPC1768_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC1768_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1768_Port3_Interface = {
    {
    "LPC1768 GPIO Port 3",
    0x0100,
    LPC1768_GPIO_Port3_InitializeWorkspace,
    sizeof(T_LPC1768_GPIO_Workspace),
    },

    LPC1768_GPIO_Configure,
    LPC1768_GPIO_Activate,
    LPC1768_GPIO_Deactivate,
    LPC1768_GPIO_SetOutputMode,
    LPC1768_GPIO_SetInputMode,
    LPC1768_GPIO_Set,
    LPC1768_GPIO_Clear,
    LPC1768_GPIO_Read,
    LPC1768_GPIO_SetPull,
    LPC1768_GPIO_SetMux,

    // uEZ v2.02
    LPC1768_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC1768_GPIO_EnableInterrupts_NotSupported,
    LPC1768_GPIO_DisableInterrupts_NotSupported,
    LPC1768_GPIO_ClearInterrupts_NotSupported,
    LPC1768_GPIO_Control,
    LPC1768_GPIO_Lock,
    LPC1768_GPIO_Unlock,

    /* uEZ v2.04 */
    LPC1768_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1768_Port4_Interface = {
    {
    "LPC1768 GPIO Port 4",
    0x0100,
    LPC1768_GPIO_Port4_InitializeWorkspace,
    sizeof(T_LPC1768_GPIO_Workspace),
    },

    LPC1768_GPIO_Configure,
    LPC1768_GPIO_Activate,
    LPC1768_GPIO_Deactivate,
    LPC1768_GPIO_SetOutputMode,
    LPC1768_GPIO_SetInputMode,
    LPC1768_GPIO_Set,
    LPC1768_GPIO_Clear,
    LPC1768_GPIO_Read,
    LPC1768_GPIO_SetPull,
    LPC1768_GPIO_SetMux,

    // uEZ v2.02
    LPC1768_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC1768_GPIO_EnableInterrupts_NotSupported,
    LPC1768_GPIO_DisableInterrupts_NotSupported,
    LPC1768_GPIO_ClearInterrupts_NotSupported,
    LPC1768_GPIO_Control,
    LPC1768_GPIO_Lock,
    LPC1768_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC1768_GPIO_GetMux };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_GPIO0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_LPC1768_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void LPC1768_GPIO1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO1 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_LPC1768_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void LPC1768_GPIO2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO2 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_LPC1768_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void LPC1768_GPIO3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO3 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_LPC1768_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void LPC1768_GPIO4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO4 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_LPC1768_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}


/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
