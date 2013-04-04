/*-------------------------------------------------------------------------*
 * File:  Interrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for LPC2xxx.
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
#include <string.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <uEZ.h>
#include <uEZRTOS.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iInterruptChannel;
    TISRFPtr iISR;
#if LPC2478_INTERRUPT_TRACK_NAMES
    const char *iName;
#endif
} T_irqHandleStruct;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_irqHandleStruct G_isrArray[UEZ_MAX_IRQ_CHANNELS];

/*---------------------------------------------------------------------------*
 * Routine:  InterruptFatalError
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt has failed to process correctly.
 *---------------------------------------------------------------------------*/
void InterruptFatalError(void)
{
    extern void UEZBSP_FatalError(TUInt32);

    // Disable all interrupts
    VICIntEnClr = 0xFFFFFFFF;
    while(1)  {
        UEZBSP_FatalError(10);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptsReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset all registered interrupts
 *---------------------------------------------------------------------------*/
void InterruptsReset(void)
{
    TUInt32 channel;

    // Disable all interrupts
    VICIntEnClr = 0xFFFFFFFF;

    // Reset the array
    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++) {
        // Register ALL interrupts to go to InterruptFatalError so if they 
        // are unexpectedly called, the system crashes nicely
        G_isrArray[channel].iISR = InterruptFatalError;
        (&VICVectAddr0)[channel] = (TUInt32)InterruptFatalError;
        (&VICVectPriority0)[channel] = INTERRUPT_PRIORITY_LOWEST;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Register an interrupt.
 * Inputs:
 *      TUInt32 aInterruptChannel -- Channel of interrupt to register
 *      TFPtr aISR                -- Pointer to interrupt service routine.
 *      T_irqPriority aPriority   -- Priority of interrupt.
 *      const char *aName         -- Name/descriptor of interrupt.
 *---------------------------------------------------------------------------*/
void InterruptRegister(
        TUInt32 aInterruptChannel,
        TISRFPtr aISR,
        T_irqPriority aPriority,
        const char * const aName)
{
    T_irqHandleStruct *p;

    // Reject attempts to allocate more ISRs than available
    if (aInterruptChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    // Create a new entry and return the handle
    p = G_isrArray+aInterruptChannel;
    // Interrupt already exists for this one!
    if (p->iISR != InterruptFatalError)
        InterruptFatalError();

    p->iInterruptChannel = aInterruptChannel;
    p->iISR = aISR;
#if LPC2478_INTERRUPT_TRACK_NAMES
    p->iName = aName;
#endif

    // Activate it in the VIC
#if (COMPILER_TYPE==RowleyARM)
    (&VICVectAddr0)[aInterruptChannel] = (unsigned int)aISR;
#endif
#if (COMPILER_TYPE==IAR)
    typedef void (__arm __irq *TIRQFPtr)(void);
    extern __arm __irq void AsmVector0(void);
    extern __arm __irq void AsmVector1(void);
    extern __arm __irq void AsmVector2(void);
    extern __arm __irq void AsmVector3(void);
    extern __arm __irq void AsmVector4(void);
    extern __arm __irq void AsmVector5(void);
    extern __arm __irq void AsmVector6(void);
    extern __arm __irq void AsmVector7(void);
    extern __arm __irq void AsmVector8(void);
    extern __arm __irq void AsmVector9(void);
    extern __arm __irq void AsmVector10(void);
    extern __arm __irq void AsmVector11(void);
    extern __arm __irq void AsmVector12(void);
    extern __arm __irq void AsmVector13(void);
    extern __arm __irq void AsmVector14(void);
    extern __arm __irq void AsmVector15(void);
    extern __arm __irq void AsmVector16(void);
    extern __arm __irq void AsmVector17(void);
    extern __arm __irq void AsmVector18(void);
    extern __arm __irq void AsmVector19(void);
    extern __arm __irq void AsmVector20(void);
    extern __arm __irq void AsmVector21(void);
    extern __arm __irq void AsmVector22(void);
    extern __arm __irq void AsmVector23(void);
    extern __arm __irq void AsmVector24(void);
    extern __arm __irq void AsmVector25(void);
    extern __arm __irq void AsmVector26(void);
    extern __arm __irq void AsmVector27(void);
    extern __arm __irq void AsmVector28(void);
    extern __arm __irq void AsmVector29(void);
    extern __arm __irq void AsmVector30(void);
    extern __arm __irq void AsmVector31(void);
    static const TIRQFPtr isrHandlerArray[UEZ_MAX_IRQ_CHANNELS] = {
        AsmVector0,
        AsmVector1,
        AsmVector2,
        AsmVector3,
        AsmVector4,
        AsmVector5,
        AsmVector6,
        AsmVector7,
        AsmVector8,
        AsmVector9,
        AsmVector10,
        AsmVector11,
        AsmVector12,
        AsmVector13,
        AsmVector14,
        AsmVector15,
        AsmVector16,
        AsmVector17,
        AsmVector18,
        AsmVector19,
        AsmVector20,
        AsmVector21,
        AsmVector22,
        AsmVector23,
        AsmVector24,
        AsmVector25,
        AsmVector26,
        AsmVector27,
        AsmVector28,
        AsmVector29,
        AsmVector30,
        AsmVector31,
    };
    (&VICVectAddr0)[aInterruptChannel] = (unsigned int)(isrHandlerArray[aInterruptChannel]);
#endif
#if (COMPILER_TYPE==Keil4)
    (&VICVectAddr0)[aInterruptChannel] = (unsigned int)aISR;
#endif
    (&VICVectPriority0)[aInterruptChannel] = aPriority;
}

#if (COMPILER_TYPE==IAR)
void InterruptHandlerX(unsigned int aIRQChannel)
{
    (*G_isrArray[aIRQChannel].iISR)();
    VICAddress = 0;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  InterruptUnregister
 *---------------------------------------------------------------------------*
 * Description:
 *      The interrupt is no longer needed.  Disable and remove it.
 * Inputs:
 *      TUInt32 aInterruptChannel -- Channel of interrupt to unregister
 *---------------------------------------------------------------------------*/
void InterruptUnregister(TUInt32 aInterruptChannel)
{
    T_irqHandleStruct *p;

    if (aInterruptChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    // Look up the channel
    p = G_isrArray+aInterruptChannel;

    // Is there an interrupt here?
    if (p->iISR == InterruptFatalError)
        InterruptFatalError();

    // Ensure interrrupt is turned off
    VICIntEnClr = (1<<(aInterruptChannel));

    // Deactivate it from the VIC
    (&VICVectAddr0)[aInterruptChannel] = (TUInt32)InterruptFatalError;
    (&VICVectPriority0)[aInterruptChannel] = INTERRUPT_PRIORITY_LOWEST;

    // Mark it as freed
    p->iISR = InterruptFatalError;
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable an existing interrupt
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to enable.
 *---------------------------------------------------------------------------*/
void InterruptEnable(T_irqChannel aChannel)
{
    if (aChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    VICIntEnable = (1<<(aChannel));
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable an existing interrupt.
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptDisable(T_irqChannel aChannel)
{
    if (aChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    VICIntEnClr = (1<<(aChannel));
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptDisableAllRegistered
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable all interrupts that were registered with this system
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptDisableAllRegistered(void)
{
    TUInt32 channel;
    T_irqHandleStruct *p;

    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++)  {
        p = G_isrArray+channel;
        if (p->iISR != InterruptFatalError)
            InterruptDisable((T_irqChannel)channel);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnableAllRegistered
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable all interrupts that were registered with this system
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptEnableAllRegistered(void)
{
    TUInt32 channel;
    T_irqHandleStruct *p;

    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++)  {
        p = G_isrArray+channel;
        if (p->iISR != InterruptFatalError)
            InterruptEnable((T_irqChannel)channel);
    }
}

/*---------------------------------------------------------------------------*
* Routine:  InterruptIsRegistered
*---------------------------------------------------------------------------*
* Description:
*      Check if an interrupt is registered.
* Inputs:
*      TUInt32 aInterruptChannel -- Channel of interrupt to register
*      
* Outputs:
*      TBool                  -- Registered ETrue else EFalse 
*---------------------------------------------------------------------------*/
TBool InterruptIsRegistered(TUInt32 aInterruptChannel)
{
    T_irqHandleStruct *p;

    // Reject attempts to access out of array data. 
    if (aInterruptChannel <= UEZ_MAX_IRQ_CHANNELS)
    {
        // Create a pointer to the the interrupt handler.
        p = G_isrArray+aInterruptChannel;
        
        // Interrupt already exists for this one.
        if (p->iISR != InterruptFatalError)
        {
            return ETrue;
        }
    }
  
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC2478_SharedEXT3IRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Shared handler for EXT3 interrupt and GPIO interrupts.
 *      Checks for interrupts from either and calls them as appropriate.
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC2478_SharedEXT3IRQ)
{
    extern void LPC2478_GPIO_ISR(void);

    IRQ_START();
    // Determine the source of this IRQ
    // Is this an GPIO irq?
    if (IOIntStatus & (IOIntStatus_P0Int|IOIntStatus_P2Int))
        LPC2478_GPIO_ISR();
    else {
        // TBD: for now, clear the EINT3 flag
        EXTMODE |= (1<<3);
        EXTINT = (1<<3);
    }


    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_InterruptEnsureSharedEXT3Registered
 *---------------------------------------------------------------------------*
 * Description:
 *      Registers and enables the EXT3/GPIO interrupt handler.
 *---------------------------------------------------------------------------*/
void LPC2478_InterruptEnsureSharedEXT3Registered(void)
{
    if (!InterruptIsRegistered(INTERRUPT_CHANNEL_EINT3)) {
        InterruptRegister(
                INTERRUPT_CHANNEL_EINT3,
                LPC2478_SharedEXT3IRQ,
                INTERRUPT_PRIORITY_HIGH,
                "EXT3");
        InterruptEnable(INTERRUPT_CHANNEL_EINT3);
    }
}

TVUInt32 G_dabort_dump[18];
void LPC2478_AbortHandler(void)
{
    extern void UEZBSP_FatalError(TUInt32);
    UEZBSP_FatalError(6);    
}

void LPC2478_PrefetchHandler(void)
{
    extern void UEZBSP_FatalError(TUInt32);
    UEZBSP_FatalError(5);    
}

/*-------------------------------------------------------------------------*
 * Function:  CPUDisableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Disables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void)
{
#if (COMPILER_TYPE == Keil4)
    __disable_irq()
#elif (COMPILER_TYPE == IAR)
    __disable_interrupt();
#elif ((COMPILER_TYPE == GCC) || (COMPILER_TYPE == RowleyARM))
    asm volatile (                                                                                                                        \
            "STMDB        SP!, {R0}                \n\t"        /* Push R0.                                                */        \
            "MRS        R0, CPSR                \n\t"        /* Get CPSR.                                        */        \
            "ORR        R0, R0, #0xC0        \n\t"        /* Disable IRQ, FIQ.                        */        \
            "MSR        CPSR, R0                \n\t"        /* Write back modified value.        */        \
            "LDMIA        SP!, {R0}                        " );        /* Pop R0.                                                */
#else
#error "CPUDisableInterrupts does not have proper disable function!"
#endif
}

/*-------------------------------------------------------------------------*
 * Function:  CPUEnableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Enables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUEnableInterrupts(void)
{
#if (COMPILER_TYPE == Keil4)
    __enable_irq()
#elif (COMPILER_TYPE == IAR)
    __enable_interrupt();
#elif ((COMPILER_TYPE == GCC) || (COMPILER_TYPE == RowleyARM))
    asm volatile (                                                                                                                        \
                            "STMDB        SP!, {R0}                \n\t"        /* Push R0.                                                */        \
                            "MRS        R0, CPSR                \n\t"        /* Get CPSR.                                        */        \
                            "BIC        R0, R0, #0xC0        \n\t"        /* Enable IRQ, FIQ.                                */        \
                            "MSR        CPSR, R0                \n\t"        /* Write back modified value.        */        \
                            "LDMIA        SP!, {R0}                        " );        /* Pop R0.                                                */
#else
#error "CPUDisableInterrupts does not have proper disable function!"
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  Interrupt.c
 *-------------------------------------------------------------------------*/

