/*-------------------------------------------------------------------------*
 * File:  LPC4357_UtilityFuncs.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utilty functions that are specific to the LPC1768 processor.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZProcessor.h>
//#include <uEZ.h>

#include <Source/Processor/NXP/LPC43xx/LPC43xx_UtilityFuncs.h>
//#include <uEZBSP.h>
//#include <uEZGPIO.h>
#include <HAL/Interrupt.h>
//#include <uEZLCD.h>
#include <uEZMemory.h>
#include <uEZPlatform.h>

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC43xx_M0_APP_ProcessInterrupt(T_LPC43xx_M0_APP_Workspace *p);
void LPC43xx_M4_CORE_ProcessInterrupt(T_LPC43xx_M4_CORE_Workspace *p);

T_LPC43xx_M0_APP_Workspace *G_M0_APPWorkspace;
T_LPC43xx_M4_CORE_Workspace *G_M4_COREWorkspace;

/*---------------------------------------------------------------------------*
 * Routine:  ReadLE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Little Endian 32 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt32 -- value read
 *---------------------------------------------------------------------------*/
TUInt32 ReadLE32U(volatile TUInt8 *pmem)
{
    TUInt32 val;

    ((TUInt8 *)&val)[0] = pmem[0];
    ((TUInt8 *)&val)[1] = pmem[1];
    ((TUInt8 *)&val)[2] = pmem[2];
    ((TUInt8 *)&val)[3] = pmem[3];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteLE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 32 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt32 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val)
{
    pmem[0] = ((TUInt8 *)&val)[0];
    pmem[1] = ((TUInt8 *)&val)[1];
    pmem[2] = ((TUInt8 *)&val)[2];
    pmem[3] = ((TUInt8 *)&val)[3];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadBE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Big Endian 32 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt32 -- value read
 *---------------------------------------------------------------------------*/
TUInt32 ReadBE32U(volatile TUInt8 *pmem)
{
    TUInt32 val;

    ((TUInt8 *)&val)[0] = pmem[3];
    ((TUInt8 *)&val)[1] = pmem[2];
    ((TUInt8 *)&val)[2] = pmem[1];
    ((TUInt8 *)&val)[3] = pmem[0];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteBE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 32 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt32 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val)
{
    pmem[0] = ((TUInt8 *)&val)[3];
    pmem[1] = ((TUInt8 *)&val)[2];
    pmem[2] = ((TUInt8 *)&val)[1];
    pmem[3] = ((TUInt8 *)&val)[0];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadLE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Little Endian 16 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt16 -- value read
 *---------------------------------------------------------------------------*/
TUInt16 ReadLE16U(volatile TUInt8 *pmem)
{
    TUInt16 val;

    ((TUInt8 *)&val)[0] = pmem[0];
    ((TUInt8 *)&val)[1] = pmem[1];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteLE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 16 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt16 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val)
{
    pmem[0] = ((TUInt8 *)&val)[0];
    pmem[1] = ((TUInt8 *)&val)[1];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadBE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Big Endian 16 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt16 -- value read
 *---------------------------------------------------------------------------*/
TUInt16 ReadBE16U(volatile TUInt8 *pmem)
{
    TUInt16 val;

    ((TUInt8 *)&val)[0] = pmem[1];
    ((TUInt8 *)&val)[1] = pmem[0];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteBE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Big Endian 16 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt16 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val)
{
    pmem[0] = ((TUInt8 *)&val)[1];
    pmem[1] = ((TUInt8 *)&val)[0];
}

/*-------------------------------------------------------------------------*
 * Function:  CPUDisableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Disables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void)
{

#if (__CORTEX_M >= 0x03)
    __set_BASEPRI(16UL << (8 - 5));
#else
    portDISABLE_INTERRUPTS();
#endif
    /* // In the LPC4357 IAP this was used instead.
    portDISABLE_INTERRUPTS();
    return;
    */
}

/*-------------------------------------------------------------------------*
 * Function:  CPUEnableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Enables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUEnableInterrupts(void)
{

#if (__CORTEX_M >= 0x03)
    __set_BASEPRI(0UL);
#else
    portENABLE_INTERRUPTS();
 #endif
    /* // In the LPC4357 IAP this was used instead.
    portENABLE_INTERRUPTS();
    return;
    */
}

//ToDo: remove these functions
void LPC43xxPowerOn(TUInt32 bits)
{
    PARAM_NOT_USED(bits);
    //extern uint32_t G_LPC43xx_powerSetting;
    //G_LPC43xx_powerSetting |= (bits);
    // Note: this LPC doesn't have a PCONP type register, but older and newer LPCs do!
    //LPC_SC->PCONP = G_LPC43xx_powerSetting;
}

void LPC43xxPowerOff(TUInt32 bits)
{
    PARAM_NOT_USED(bits);
    //extern uint32_t G_LPC43xx_powerSetting;
    //G_LPC43xx_powerSetting &= ~(bits);
    // Note: this LPC doesn't have a PCONP type register, but older and newer LPCs do!
    //LPC_SC->PCONP = G_LPC43xx_powerSetting;
}

void LPC43xx_SCU_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_LPC43xx_SCU_ConfigList *aList,
        TUInt8 aCount)
{
    while (aCount--) {
        if (aList->iPortPin == aPortPin) {
            UEZGPIOLock(aPortPin);
            UEZGPIOControl(aPortPin, GPIO_CONTROL_SET_CONFIG_BITS,
                    aList->iSetting);
            return;
        }
        aList++;
    }
    UEZFailureMsg("Bad Pin");
}

void LPC43xx_SCU_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_LPC43xx_SCU_ConfigList *aList,
        TUInt8 aCount)
{
    if (aPortPin == GPIO_NONE)
        return;
    while (aCount--) {
        if (aList->iPortPin == aPortPin) {
            UEZGPIOLock(aPortPin);
            UEZGPIOControl(aPortPin, GPIO_CONTROL_SET_CONFIG_BITS,
                    aList->iSetting);
            return;
        }
        aList++;
    }
    UEZFailureMsg("Bad Pin");
}

// For now override these weak functions in application projects.
/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_M0_APP_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Process an interrupt for this workspace.  This is ISR on M4 FROM M0.
 * Inputs:
 *      void *aWorkspace          -- M0_APP Workspace
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
__WEAK void LPC43xx_M0_APP_ProcessInterrupt(T_LPC43xx_M0_APP_Workspace *p)
{
    //InterruptDisable(M0APP_IRQn);
        
    // Determine the type of interrupt
    //TUInt32 flags = No flags on this ISR;

    // TODO application callback for ISR
    //uEZPlatform_IPC_Callback(); // TODO

    // Clear any interrupts processed by this interrupt
    //LPC_->INTCLR = flags; // No flags on this ISR
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_M4_CORE_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Process an interrupt for this workspace. This is ISR on M0 FROM M4.
 * Inputs:
 *      void *aWorkspace          -- M4_CORE Workspace
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
__WEAK void LPC43xx_M4_CORE_ProcessInterrupt(T_LPC43xx_M4_CORE_Workspace *p)
{
    //InterruptDisable(M0_M4CORE_IRQn);
        
    // Determine the type of interrupt
    //TUInt32 flags = No flags on this ISR;

    // TODO application callback for ISR

    // Clear any interrupts processed by this interrupt
    //LPC_->INTCLR = flags; // No flags on this ISR
}

/*---------------------------------------------------------------------------*
 * Interrupt:  LPC43xx_M0_APP_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      A M0_APP Interrupt has come in.  Process the one workspace attached
 *      to this interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC43xx_M0_APP_Interrupt)
{
    IRQ_START();
    LPC43xx_M0_APP_ProcessInterrupt(G_M0_APPWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Interrupt:  LPC43xx_M4_CORE_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      A M4_CORE Interrupt has come in.  Process the one workspace attached
 *      to this interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC43xx_M4_CORE_Interrupt)
{
    IRQ_START();
    LPC43xx_M4_CORE_ProcessInterrupt(G_M4_COREWorkspace);
    IRQ_END();
}

T_uezError LPC43xx_InterCore_InitializeWorkspace(void *aWorkspace)
{
#ifdef CORE_M4 // TODO
    T_LPC43xx_M0_APP_Workspace *p = (T_LPC43xx_M0_APP_Workspace *)aWorkspace;
    p->iCore = 0;
    p->iTest = 0;

    G_M0_APPWorkspace = p;
#endif
#ifdef CORE_M0 // TODO
    T_LPC43xx_M4_CORE_Workspace *p = (T_LPC43xx_M4_CORE_Workspace *)aWorkspace;
    p->iCore = 1;
    p->iTest = 0;

    G_M4_COREWorkspace = p;
#endif
#ifdef CORE_M0SUB // TODO
    p->iCore = 2;
#endif

    return UEZ_ERROR_NONE;
}

void LPC43xxSetupInterCoreInterrupts(void){

#ifdef CORE_M4 // TODO
// Setup interrupt for the M0APP
    InterruptRegister(M0APP_IRQn, LPC43xx_M0_APP_Interrupt, 
      INTERRUPT_PRIORITY_HIGH, "M0_APP");
    InterruptEnable(M0APP_IRQn);
#endif
#ifdef CORE_M0 // TODO
// Setup interrupt for the M4
    InterruptRegister(M0_M4CORE_IRQn, LPC43xx_M4_CORE_Interrupt, 
      INTERRUPT_PRIORITY_HIGH, "M4_CORE");
    InterruptEnable(M0_M4CORE_IRQn);
#endif
#ifdef CORE_M0SUB // TODO
#endif
}

// Note that uEZBSP Delays are only accurate for around level 2 or medium optimization.
// With no optimization it will be about double the time. So we will always optimize them.
// For IAR application projects set the optimization level of this file to medium or high.

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayUS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelayMS() when in a task instead.
 *---------------------------------------------------------------------------*/
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
__WEAK void UEZBSPDelay1US(void)
{
#ifdef CORE_M4
/* 	Measured to 1.004 microseconds GPIO switch time: (RTOS off)
 *  -   Based on release build with high optimization speed setting, inline
 *      small functions enabled, with GPIO on/off verified with a scope.
 *      Use the following code for direct GPIO switching (example GPIO_P5_3):
 *          LPC_GPIO_PORT->SET[5] = 8; 
 *          UEZBSPDelay1US();        
 *          LPC_GPIO_PORT->CLR[5] = 8; 
 */
#if 1 //( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
    nops100();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
    nop();
    nop();
    nop();
    nop();
#endif
#if 0 // ( PROCESSOR_OSCILLATOR_FREQUENCY == 200000000) // TODO not validated yet.
    nops100();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
    nop();
#endif
#endif

#ifdef CORE_M0 // 
// 	Measured to 1.005s from gcc build UEZBSPDelayMS(1000) call.
#if 1 // ( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
    nops100();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
#endif
#if 0 //( PROCESSOR_OSCILLATOR_FREQUENCY == 200000000) // TODO not validated yet.
    nops100();
    nops50();
    nops10();
    nops10();
    nops10();
    nops5();
    nop();
    nop();
#endif
#endif
})

UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
__WEAK void UEZBSPDelayUS(uint32_t aMicroseconds)
{
    while (aMicroseconds--)
        UEZBSPDelay1US();
})

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayMS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelayMS() when in a task instead.
 *---------------------------------------------------------------------------*/
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
__WEAK void UEZBSPDelay1MS(void)
{    
/* 	Measured to 1.000 milliseconds GPIO switch time: (RTOS off)
 *  -   Based on release build with high optimization speed setting, inline
 *      small functions enabled, with GPIO on/off verified with a scope.
 *      Use the following code for direct GPIO switching (example GPIO_P5_3):
 *          LPC_GPIO_PORT->SET[5] = 8; 
 *          UEZBSPDelay1MS();        
 *          LPC_GPIO_PORT->CLR[5] = 8; 
 */  
    TUInt32 i;

    // Approximate delays here    
    for (i = 0; i < 995; i++)
        UEZBSPDelay1US();
})

UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
__WEAK void UEZBSPDelayMS(uint32_t aMilliseconds)
{
    while (aMilliseconds--) {
        UEZBSPDelay1MS();
    }
})

/*-------------------------------------------------------------------------*
 * End of File:  LPC4357_UtilityFuncs.c
 *-------------------------------------------------------------------------*/

