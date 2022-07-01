/*-------------------------------------------------------------------------*
 * File:  RX62N_ExternalInterrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <HAL/GPIO.h>
#include "RX62N_ExternalInterrupt.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define NUM_EXTERNAL_INTERRUPTS         16

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt8 iTrigger;
    void *iCallbackWorkspace;
    EINT_Callback iCallbackFunc;
    T_irqPriority iPriority;
    TUInt8 iIsPinSetB;
} T_eintEntry;

typedef struct {
    const HAL_ExternalInterrupt *iDevice;
} T_ExternalInterrupt_Renesas_RX62N_Workspace;

typedef struct {
    TVUInt8 *iPFxIRQReg;
    TUInt8 iPFxIRQBit;
    const char *iGPIONameA;
    TUInt8 iGPIOPinIndexA;
    const char *iGPIONameB;
    TUInt8 iGPIOPinIndexB;
} T_ExternalInterrupt_Renesas_RX62N_Info;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static IRQ_ROUTINE(RX62N_EINT0_Callback);
static IRQ_ROUTINE(RX62N_EINT1_Callback);
static IRQ_ROUTINE(RX62N_EINT2_Callback);
static IRQ_ROUTINE(RX62N_EINT3_Callback);
static IRQ_ROUTINE(RX62N_EINT4_Callback);
static IRQ_ROUTINE(RX62N_EINT5_Callback);
static IRQ_ROUTINE(RX62N_EINT6_Callback);
static IRQ_ROUTINE(RX62N_EINT7_Callback);
static IRQ_ROUTINE(RX62N_EINT8_Callback);
static IRQ_ROUTINE(RX62N_EINT9_Callback);
static IRQ_ROUTINE(RX62N_EINT10_Callback);
static IRQ_ROUTINE(RX62N_EINT11_Callback);
static IRQ_ROUTINE(RX62N_EINT12_Callback);
static IRQ_ROUTINE(RX62N_EINT13_Callback);
static IRQ_ROUTINE(RX62N_EINT14_Callback);
static IRQ_ROUTINE(RX62N_EINT15_Callback);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_eintEntry G_eintEntries[NUM_EXTERNAL_INTERRUPTS];
static const TISRFPtr G_eintFuncs[NUM_EXTERNAL_INTERRUPTS] = {
    RX62N_EINT0_Callback,
    RX62N_EINT1_Callback,
    RX62N_EINT2_Callback,
    RX62N_EINT3_Callback,
    RX62N_EINT4_Callback,
    RX62N_EINT5_Callback,
    RX62N_EINT6_Callback,
    RX62N_EINT7_Callback,
    RX62N_EINT8_Callback,
    RX62N_EINT9_Callback,
    RX62N_EINT10_Callback,
    RX62N_EINT11_Callback,
    RX62N_EINT12_Callback,
    RX62N_EINT13_Callback,
    RX62N_EINT14_Callback,
    RX62N_EINT15_Callback, };

static const char gpio0[] = "GPIO0";
static const char gpio1[] = "GPIO1";
static const char gpio3[] = "GPIO3";
static const char gpio4[] = "GPIO4";
static const char gpioE[] = "GPIOE";

static const T_ExternalInterrupt_Renesas_RX62N_Info G_eintInfo[NUM_EXTERNAL_INTERRUPTS] = {
#if 1 // (RX62N_PACKAGE==RX62N_PACKAGE_BGA176)
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<0),     gpio3,  0,  gpio1,  0},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<1),     gpio3,  1,  gpio1,  1},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<2),     gpio3,  2,  gpio1,  2},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<3),     gpio3,  3,  gpio1,  3},

    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<4),     gpio3,  4,  gpio1,  4},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<5),     gpioE,  5,  gpio1,  5},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<6),     gpioE,  6,  gpio1,  6},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<7),     gpioE,  7,  gpio1,  7},

    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<0),     gpio0,  0,  gpio4,  0},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<1),     gpio0,  1,  gpio4,  1},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<2),     gpio0,  2,  gpio4,  2},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<3),     gpio0,  3,  gpio4,  3},

    { (TVUInt8 *)0,               (1<<4),     gpio4,  4,  gpio4,  4},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<5),     gpio0,  5,  gpio4,  5},
    { (TVUInt8 *)0,               (1<<6),     gpio4,  6,  gpio4,  6},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<7),     gpio0,  7,  gpio4,  7},
#endif
#if 0 //(RX62N_PACKAGE==RX62N_PACKAGE_LQFP100)
    { (TVUInt8 *)0,               (1<<0),     gpio3,  0,  gpio3,  0},
    { (TVUInt8 *)0,               (1<<1),     gpio3,  1,  gpio3,  1},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<2),     gpio3,  2,  gpio1,  2},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<3),     gpio3,  3,  gpio1,  3},

    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<4),     gpio3,  4,  gpio1,  4},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<5),     gpioE,  5,  gpioE,  5},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<6),     gpioE,  6,  gpio1,  6},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<7),     gpioE,  7,  gpioE,  7},

    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<0),     gpio4,  0,  gpio4,  0},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<1),     gpio4,  1,  gpio4,  1},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<2),     gpio4,  2,  gpio4,  2},
    { (TVUInt8 *)&IOPORT.PF9IRQ,  (1<<3),     gpio4,  3,  gpio4,  3},

    { (TVUInt8 *)0,               (1<<4),     gpio4,  4,  gpio4,  4},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<5),     gpio0,  5,  gpio4,  5},
    { (TVUInt8 *)0,               (1<<6),     gpio4,  6,  gpio4,  6},
    { (TVUInt8 *)&IOPORT.PF8IRQ,  (1<<7),     gpio0,  7,  gpio4,  7},
#endif
};

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9555.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_InitializeWorkspace(void *aW)
{
    TUInt8 i;

    //    T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //            (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aW;

    // None are in use yet
    for (i = 0; i < NUM_EXTERNAL_INTERRUPTS; i++)
        G_eintEntries[i].iCallbackFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EINTx_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      The following routines are called from the IRQ and is used to
 *      call the function.  NOTE:  This is still INSIDE the ISR.
 *---------------------------------------------------------------------------*/
static void IRX62N_ProcessExternalInterrupt(TUInt8 aChannel)
{
    if (G_eintEntries[aChannel].iCallbackFunc(
        G_eintEntries[aChannel].iCallbackWorkspace, aChannel))
        ICU.IR[VECT_ICU_IRQ0 + aChannel].BIT.IR = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EINT?_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      These stubs call IRX62N_ProcessExternalInterrupt and tell
 *      where they came from.
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(RX62N_EINT0_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(0);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT1_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(1);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT2_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(2);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT3_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(3);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT4_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(4);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT5_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(5);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT6_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(6);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT7_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(7);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT8_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(8);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT9_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(9);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT10_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(10);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT11_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(11);
    IRQ_END();
}

static IRQ_ROUTINE(RX62N_EINT12_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(12);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT13_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(13);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT14_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(14);
    IRQ_END();
}
static IRQ_ROUTINE(RX62N_EINT15_Callback)
{
    IRQ_START();
    IRX62N_ProcessExternalInterrupt(15);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the rate of a given blink register (as close as possible).
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 *      TUInt8 aTrigger             -- Trigger type (if used)
 *      EINTCallback aCallbackFunc  -- Function to call when trigger.
 *      void *aCallbackWorkspace    -- Parameter to pass to callback function.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_Set(
    void *aWorkspace,
    TUInt32 aChannel,
    T_EINTTrigger aTrigger,
    EINT_Callback aCallbackFunc,
    void *aCallbackWorkspace,
    T_irqPriority aPriority,
    const char *aName)
{
    TUInt32 irqNum;
    //    T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //       (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aWorkspace;
    T_eintEntry *p_entry;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 edge;
    const T_ExternalInterrupt_Renesas_RX62N_Info *p_info;
    HAL_GPIOPort **p_gpio;
    TUInt32 gpioPinIndex;

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt already in use?
        p_entry = G_eintEntries + aChannel;
        if (p_entry->iCallbackFunc) {
            error = UEZ_ERROR_ALREADY_EXISTS;
            break;
        }

        // Is this interrupt already registered?
        irqNum = VECT_ICU_IRQ0 + aChannel;
        if (InterruptIsRegistered(irqNum)) {
            error = UEZ_ERROR_NOT_AVAILABLE;
            break;
        }

        // Interrupt is available.  Register it
        p_entry->iPriority = aPriority;
        p_entry->iTrigger = aTrigger;
        p_entry->iCallbackWorkspace = aCallbackWorkspace;
        p_entry->iCallbackFunc = aCallbackFunc;

        // Start disabled
        InterruptDisable(irqNum);

        // Find the info about this pin
        p_info = G_eintInfo + aChannel;
        error = HALInterfaceFind((p_entry->iIsPinSetB) ? p_info->iGPIONameB
            : p_info->iGPIONameA, (T_halWorkspace **)&p_gpio);
        if (error)
            break;
        gpioPinIndex = (p_entry->iIsPinSetB) ? p_info->iGPIOPinIndexB
            : p_info->iGPIOPinIndexA;
        (*p_gpio)->SetInputMode(p_gpio, (1<<gpioPinIndex));
        (*p_gpio)->Control(p_gpio, gpioPinIndex, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0);

        // Map the trigger type to a constant
        switch (aTrigger) {
            case EINT_TRIGGER_EDGE_FALLING:
                edge = (1 << 3);
                break;
            case EINT_TRIGGER_EDGE_RISING:
                edge = (2 << 3);
                break;
            case EINT_TRIGGER_LEVEL_LOW:
                edge = (0 << 3);
                break;
            case EINT_TRIGGER_EDGE_BOTH:
                edge = (3 << 3);
                break;
            case EINT_TRIGGER_LEVEL_HIGH:
            default:
                edge = 0xFF;
                break;
        }
        if (edge == 0xFF) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }
        ICU.IRQCR[aChannel].BYTE = edge;

        // Clear the interrupt
        ICU.IR[irqNum].BIT.IR = 0;

        InterruptRegister(irqNum, G_eintFuncs[aChannel], aPriority, aName);

        // Do not loop
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Unregister the external interrupt handler.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_Reset(
    void *aWorkspace,
    TUInt32 aChannel)
{
    TUInt32 irqNum;
    //    T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //        (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aWorkspace;
    T_eintEntry *p_entry;
    T_uezError error = UEZ_ERROR_NONE;

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt already in cleared?
        p_entry = G_eintEntries + aChannel;
        if (!p_entry->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Unregister it
        irqNum = VECT_ICU_IRQ0 + aChannel;
        InterruptDisable(irqNum);
        InterruptUnregister(irqNum);
        p_entry->iPriority = INTERRUPT_PRIORITY_NORMAL;
        p_entry->iTrigger = 0;
        p_entry->iCallbackWorkspace = 0;
        p_entry->iCallbackFunc = 0;

        // Do not loop
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the flag for this external interrupt case
 * Inputs:
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_Clear(
    void *aWorkspace,
    TUInt32 aChannel)
{
    TUInt32 irqNum;
    //T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //    (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aWorkspace;
    T_eintEntry *p_entry;
    T_uezError error = UEZ_ERROR_NONE;

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_entry = G_eintEntries + aChannel;
        if (!p_entry->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = VECT_ICU_IRQ0 + aChannel;
        ICU.IR[irqNum].BIT.IR = 0;

        // Do not loop
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable the given external interrupt
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_Disable(
    void *aWorkspace,
    TUInt32 aChannel)
{
    TUInt32 irqNum;
    //T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //    (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aWorkspace;
    T_eintEntry *p_entry;
    T_uezError error = UEZ_ERROR_NONE;

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_entry = G_eintEntries + aChannel;
        if (!p_entry->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = VECT_ICU_IRQ0 + aChannel;
        InterruptDisable(irqNum);

        // Do not loop
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the given external interrupt
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_Enable(
    void *aWorkspace,
    TUInt32 aChannel)
{
    TUInt32 irqNum;
    //T_ExternalInterrupt_Renesas_RX62N_Workspace *p =
    //    (T_ExternalInterrupt_Renesas_RX62N_Workspace *)aWorkspace;
    T_eintEntry *p_entry;
    T_uezError error = UEZ_ERROR_NONE;

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // Is this external interrupt available?
        p_entry = G_eintEntries + aChannel;
        if (!p_entry->iCallbackFunc) {
            error = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Disable it
        irqNum = VECT_ICU_IRQ0 + aChannel;
        InterruptEnable(irqNum);

        // Do not loop
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ExternalInterrupt_Renesas_RX62N_ConfigurePinSet
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the pin to be on IRQn-A (EFalse) or IRQn-B (ETrue).
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aChannel            -- EINT channel (0 to 15)
 *      TBool aIsPinSetB            -- ETrue = IRQn-B, EFalse = IRQn-A
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ExternalInterrupt_Renesas_RX62N_ConfigurePinSet(
    void *aWorkspace,
    TUInt32 aChannel,
    TBool aIsPinSetB)
{
    T_uezError error = UEZ_ERROR_NONE;
    PARAM_NOT_USED(aWorkspace);

    // Fake loop
    while (1) {
        // Is this a valid channel?
        if (aChannel >= NUM_EXTERNAL_INTERRUPTS) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        G_eintEntries[aChannel].iIsPinSetB = aIsPinSetB;
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ExternalInterrupt ExternalInterrupt_Renesas_RX62N_Interface = { {
// Common device interface
    "EINT:Renesas:RX62N",
    0x0100,
    ExternalInterrupt_Renesas_RX62N_InitializeWorkspace,
    sizeof(T_ExternalInterrupt_Renesas_RX62N_Workspace), },

// Functions
    ExternalInterrupt_Renesas_RX62N_Set,
    ExternalInterrupt_Renesas_RX62N_Reset,
    ExternalInterrupt_Renesas_RX62N_Clear,
    ExternalInterrupt_Renesas_RX62N_Disable,
    ExternalInterrupt_Renesas_RX62N_Enable, 
};

void RX62N_ExternalInterrupts_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("EINT", (T_halInterface *)&ExternalInterrupt_Renesas_RX62N_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_ExternalInterrupt.c
 *-------------------------------------------------------------------------*/
