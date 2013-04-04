/*-------------------------------------------------------------------------*
 * File:  GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 GPIO Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_GPIO.h>
#include <uEZPlatform.h>
#include <uEZGPIO.h>

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
    TVUInt32 *iIOIntEnR;
    TVUInt32 *iIOIntEnF;
    TVUInt32 *iIOIntStatR;
    TVUInt32 *iIOIntStatF;
    TVUInt32 *iIOIntClr;
} LPC2478_GPIO_PortInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const LPC2478_GPIO_PortInfo *iPortInfo;
    T_gpioInterruptHandler iInterruptCallback;
    void *iInterruptCallbackWorkspace;
    TUInt32 iLocked;
} T_LPC2478_GPIO_Workspace;

const LPC2478_GPIO_PortInfo GPIO_LPC2478_Port0_PortInfo = {
    (TVUInt32 *)0xE002C000, // PINSEL0
    (TVUInt32 *)0xE002C004, //&PINSEL1,
    (TVUInt32 *)0xE002C040, // &PINMODE0,
    (TVUInt32 *)0xE002C044, // &PINMODE1,
    (TVUInt32 *)0xE0028000, // &IO0PIN,
    (TVUInt32 *)0xE0028004, // &IO0SET,
    (TVUInt32 *)0xE002800C, // &IO0CLR,
    (TVUInt32 *)0xE0028008, //&IO0DIR,
    (TVUInt32 *)0xE0028090, //&IO0IntEnR,
    (TVUInt32 *)0xE0028094, //&IO0IntEnF,
    (TVUInt32 *)0xE0028084, //&IO0IntStatR,
    (TVUInt32 *)0xE0028088, //&IO0IntStatF,
    (TVUInt32 *)0xE002808C, //&IO0IntClr,
};

const LPC2478_GPIO_PortInfo GPIO_LPC2478_Port1_PortInfo = {
    (TVUInt32 *)0xE002C008, //&PINSEL2,
    (TVUInt32 *)0xE002C00C, //&PINSEL3,
    (TVUInt32 *)0xE002C048, //&PINMODE2,
    (TVUInt32 *)0xE002C04C, //&PINMODE3,
    (TVUInt32 *)0xE0028010, //&IO1PIN,
    (TVUInt32 *)0xE0028014, //&IO1SET,
    (TVUInt32 *)0xE002801C, //&IO1CLR,
    (TVUInt32 *)0xE0028018, //&IO1DIR,
    0,
    0,
    0,
    0,
    0
};

const LPC2478_GPIO_PortInfo GPIO_LPC2478_Port2_PortInfo = {
    (TVUInt32 *)0xE002C010, //&PINSEL4,
    (TVUInt32 *)0xE002C014, //&PINSEL5,
    (TVUInt32 *)0xE002C050, //&PINMODE4,
    (TVUInt32 *)0xE002C050, //&PINMODE5,
    (TVUInt32 *)0x3FFFC054, //&FIO2PIN,
    (TVUInt32 *)0x3FFFC058, //&FIO2SET,
    (TVUInt32 *)0x3FFFC05C, //&FIO2CLR,
    (TVUInt32 *)0x3FFFC040, //&FIO2DIR,
    (TVUInt32 *)0xE00280B0, //&IO2IntEnR,
    (TVUInt32 *)0xE00280B4, //&IO2IntEnF,
    (TVUInt32 *)0xE00280A4, //&IO2IntStatR,
    (TVUInt32 *)0xE00280A8, //&IO2IntStatF,
    (TVUInt32 *)0xE00280AC, //&IO2IntClr,
};

const LPC2478_GPIO_PortInfo GPIO_LPC2478_Port3_PortInfo = {
    (TVUInt32 *)0xE002C018, //&PINSEL6,
    (TVUInt32 *)0xE002C01C, //&PINSEL7,
    (TVUInt32 *)0xE002C058, //&PINMODE6,
    (TVUInt32 *)0xE002C05C, //&PINMODE7,
    (TVUInt32 *)0x3FFFC074, //&FIO3PIN,
    (TVUInt32 *)0x3FFFC078, //&FIO3SET,
    (TVUInt32 *)0x3FFFC07C, //&FIO3CLR,
    (TVUInt32 *)0x3FFFC060, //&FIO3DIR,
    0,
    0,
    0,
    0,
    0
};

const LPC2478_GPIO_PortInfo GPIO_LPC2478_Port4_PortInfo = {
    (TVUInt32 *)0xE002C020, //&PINSEL8,
    (TVUInt32 *)0xE002C024, //&PINSEL9,
    (TVUInt32 *)0xE002C060, //&PINMODE8,
    (TVUInt32 *)0xE002C064, //&PINMODE9,
    (TVUInt32 *)0x3FFFC094, //&FIO4PIN,
    (TVUInt32 *)0x3FFFC098, //&FIO4SET,
    (TVUInt32 *)0x3FFFC09C, //&FIO4CLR,
    (TVUInt32 *)0x3FFFC080, //&FIO4DIR,
    0,
    0,
    0,
    0,
    0
};

static T_LPC2478_GPIO_Workspace *G_LPC2478Port0Workspace=0;
static T_LPC2478_GPIO_Workspace *G_LPC2478Port2Workspace=0;

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Port0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC2478_Port0_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC2478Port0Workspace = p;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Port1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 GPIO Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Port1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC2478_Port1_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Port2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 GPIO Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Port2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC2478_Port2_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC2478Port2Workspace = p;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Port3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 GPIO Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Port3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC2478_Port3_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Port4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 GPIO Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Port4_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC2478_Port4_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iIODIR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iIODIR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOSET) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOCLR) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_Read
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
T_uezError LPC2478_GPIO_Read(
            void *aWorkspace,
            TUInt32 aPortPins,
            TUInt32 *aPinsRead)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Get the bits
    *aPinsRead = ((*p_info->iIOPIN) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_SetPull(
            void *aWorkspace,
            TUInt8 aPortPinIndex,
            T_gpioPull aPull)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

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
 * Routine:  LPC2478_GPIO_SetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_SetMux(
            void *aWorkspace,
            TUInt8 aPortPinIndex,
            T_gpioMux aMux)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;
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
 * Routine:  LPC2478_GPIO_SetInterrupt
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
T_uezError LPC2478_GPIO_ConfigureInterruptCallback(
        void *aWorkspace,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    extern void LPC2478_InterruptEnsureSharedEXT3Registered(void);
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;

    // Register the interrupt handler if not already registered
    LPC2478_InterruptEnsureSharedEXT3Registered();
    p->iInterruptCallback = aInterruptCallback;
    p->iInterruptCallbackWorkspace = aInterruptCallbackWorkspace;
    return UEZ_ERROR_NONE;
}

T_uezError LPC2478_GPIO_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aInterruptCallback);
    PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_ProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO ports is reporting an IRQ.  This processes one
 *      of the ports for falling or rising edge events.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC2478_GPIO_ProcessIRQ(T_LPC2478_GPIO_Workspace *p)
{
    TUInt32 regF;
    TUInt32 regR;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    // Make sure we have a handler
    if (!p->iInterruptCallback)
        InterruptFatalError();

    // Do we have any falling edge interrupts?
    regF = *(p_info->iIOIntStatF);
    regR = *(p_info->iIOIntStatR);
    if (regF)
        p->iInterruptCallback(
                p->iInterruptCallbackWorkspace,
                regF,
                GPIO_INTERRUPT_FALLING_EDGE);

    // Do we have any rising edge interrupts?
    if (regR)
        p->iInterruptCallback(
                p->iInterruptCallbackWorkspace,
                regR,
                GPIO_INTERRUPT_RISING_EDGE);

    // Now clear all the interrupts processed above
    // (regardless of enabled/disabled state)
    *(p_info->iIOIntClr) = regF|regR;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO interrupts have fired off, do the right one.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC2478_GPIO_ISR(void)
{
    // It's port 0 and/or port 2, is this port 0 firing?
    if (IOIntStatus & IOIntStatus_P0Int)
        LPC2478_GPIO_ProcessIRQ(G_LPC2478Port0Workspace);
    if (IOIntStatus & IOIntStatus_P2Int)
        LPC2478_GPIO_ProcessIRQ(G_LPC2478Port2Workspace);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_EnableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to enable.
 *      T_gpioInterruptType aType   -- Type of interrupts to enable.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_EnableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) |= aPortPins;
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) |= aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_DisableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *      T_gpioInterruptType aType   -- Type of interrupts to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_DisableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) &= ~aPortPins;
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_ClearInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear interrupts on a specific GPIO line
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_GPIO_ClearInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;

	(*p_info->iIOIntClr) = aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError LPC2478_GPIO_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
	PARAM_NOT_USED(aWorkspace);
	PARAM_NOT_USED(aPortPins);
	PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC2478_GPIO_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
	PARAM_NOT_USED(aWorkspace);
	PARAM_NOT_USED(aPortPins);
	PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC2478_GPIO_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
	PARAM_NOT_USED(aWorkspace);
	PARAM_NOT_USED(aPortPins);
        return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC2478_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    //T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;

    PARAM_NOT_USED(aValue);

    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
#if (COMPILER_TYPE != IAR)
            break;
#endif
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC2478_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        UEZFailureMsg("PinLock");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError LPC2478_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_GPIO_GetMux
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
T_uezError LPC2478_GPIO_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    T_LPC2478_GPIO_Workspace *p = (T_LPC2478_GPIO_Workspace *)aWorkspace;
    const LPC2478_GPIO_PortInfo *p_info = p->iPortInfo;
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

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_GPIO0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_LPC2478_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void LPC2478_GPIO1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO1 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_LPC2478_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void LPC2478_GPIO2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO2 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_LPC2478_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void LPC2478_GPIO3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO3 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_LPC2478_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void LPC2478_GPIO4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO4 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_LPC2478_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_LPC2478_Port0_Interface = {
    "LPC2478 GPIO Port 0",
    0x0105,
    LPC2478_GPIO_Port0_InitializeWorkspace,
    sizeof(T_LPC2478_GPIO_Workspace),

    0,
    0,
    0,
    LPC2478_GPIO_SetOutputMode,
    LPC2478_GPIO_SetInputMode,
    LPC2478_GPIO_Set,
    LPC2478_GPIO_Clear,
    LPC2478_GPIO_Read,
    LPC2478_GPIO_SetPull,
    LPC2478_GPIO_SetMux,
    LPC2478_GPIO_ConfigureInterruptCallback,
    LPC2478_GPIO_EnableInterrupts,
    LPC2478_GPIO_DisableInterrupts,
    LPC2478_GPIO_ClearInterrupts,
    LPC2478_GPIO_Control,
    LPC2478_GPIO_Lock,
    LPC2478_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC2478_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC2478_Port1_Interface = {
    "LPC2478 GPIO Port 1",
    0x0105,
    LPC2478_GPIO_Port1_InitializeWorkspace,
    sizeof(T_LPC2478_GPIO_Workspace),

    0,
    0,
    0,
    LPC2478_GPIO_SetOutputMode,
    LPC2478_GPIO_SetInputMode,
    LPC2478_GPIO_Set,
    LPC2478_GPIO_Clear,
    LPC2478_GPIO_Read,
    LPC2478_GPIO_SetPull,
    LPC2478_GPIO_SetMux,
    LPC2478_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC2478_GPIO_EnableInterrupts_NotSupported,
    LPC2478_GPIO_DisableInterrupts_NotSupported,
    LPC2478_GPIO_ClearInterrupts_NotSupported,
    LPC2478_GPIO_Control,
    LPC2478_GPIO_Lock,
    LPC2478_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC2478_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC2478_Port2_Interface = {
    "LPC2478 GPIO Port 2",
    0x0105,
    LPC2478_GPIO_Port2_InitializeWorkspace,
    sizeof(T_LPC2478_GPIO_Workspace),

    0,
    0,
    0,
    LPC2478_GPIO_SetOutputMode,
    LPC2478_GPIO_SetInputMode,
    LPC2478_GPIO_Set,
    LPC2478_GPIO_Clear,
    LPC2478_GPIO_Read,
    LPC2478_GPIO_SetPull,
    LPC2478_GPIO_SetMux,
    LPC2478_GPIO_ConfigureInterruptCallback,
    LPC2478_GPIO_EnableInterrupts,
    LPC2478_GPIO_DisableInterrupts,
    LPC2478_GPIO_ClearInterrupts,
    LPC2478_GPIO_Control,
    LPC2478_GPIO_Lock,
    LPC2478_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC2478_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC2478_Port3_Interface = {
    "LPC2478 GPIO Port 3",
    0x0105,
    LPC2478_GPIO_Port3_InitializeWorkspace,
    sizeof(T_LPC2478_GPIO_Workspace),

    0,
    0,
    0,
    LPC2478_GPIO_SetOutputMode,
    LPC2478_GPIO_SetInputMode,
    LPC2478_GPIO_Set,
    LPC2478_GPIO_Clear,
    LPC2478_GPIO_Read,
    LPC2478_GPIO_SetPull,
    LPC2478_GPIO_SetMux,
    LPC2478_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC2478_GPIO_EnableInterrupts_NotSupported,
    LPC2478_GPIO_DisableInterrupts_NotSupported,
    LPC2478_GPIO_ClearInterrupts_NotSupported,
    LPC2478_GPIO_Control,
    LPC2478_GPIO_Lock,
    LPC2478_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC2478_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC2478_Port4_Interface = {
    "LPC2478 GPIO Port 4",
    0x0105,
    LPC2478_GPIO_Port4_InitializeWorkspace,
    sizeof(T_LPC2478_GPIO_Workspace),

    0,
    0,
    0,
    LPC2478_GPIO_SetOutputMode,
    LPC2478_GPIO_SetInputMode,
    LPC2478_GPIO_Set,
    LPC2478_GPIO_Clear,
    LPC2478_GPIO_Read,
    LPC2478_GPIO_SetPull,
    LPC2478_GPIO_SetMux,
    LPC2478_GPIO_ConfigureInterruptCallback_NotSupported,
    LPC2478_GPIO_EnableInterrupts_NotSupported,
    LPC2478_GPIO_DisableInterrupts_NotSupported,
    LPC2478_GPIO_ClearInterrupts_NotSupported,
    LPC2478_GPIO_Control,
    LPC2478_GPIO_Lock,
    LPC2478_GPIO_Unlock,
    /* uEZ v2.04 */
    LPC2478_GPIO_GetMux };

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
