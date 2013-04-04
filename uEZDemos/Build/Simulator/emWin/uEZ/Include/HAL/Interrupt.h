/*-------------------------------------------------------------------------*
 * File:  Interrupt.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for LPC2xxx.
 *-------------------------------------------------------------------------*/
/**
 *  @file   /Include/HAL/Interrupt.h
 *  @brief  uEZ Interrupt HAL Interface
 *
 *  The uEZ Interrupt HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
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
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/*---------------------------------------------------------------------------*
 * Includes:
 *---------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define INTERRUPT_HANDLE_BAD        0xFFFFFFFF

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef TUInt32 T_irqChannel;

typedef enum {
    INTERRUPT_PRIORITY_HIGHEST=0,
    INTERRUPT_PRIORITY_HIGH=3,
    INTERRUPT_PRIORITY_NORMAL=7,
    INTERRUPT_PRIORITY_LOW=10,
    INTERRUPT_PRIORITY_LOWEST=15
} T_irqPriority ;

#if (COMPILER_TYPE==CodeRed)
typedef TFPtr TISRFPtr;
#endif
#if (COMPILER_TYPE==RowleyARM)
typedef TFPtr TISRFPtr;
#endif
#if (COMPILER_TYPE==IAR)
typedef void (*TISRFPtr)(void); 
#endif
#if (COMPILER_TYPE==HEW)
typedef TFPtr TISRFPtr;
#endif
#if (COMPILER_TYPE==Keil4)
typedef TFPtr TISRFPtr;
#endif
#if (COMPILER_TYPE==RenesasRX)
typedef TFPtr TISRFPtr;
#endif
#if (COMPILER_TYPE==VisualC)
typedef TFPtr TISRFPtr;
#endif

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void InterruptsReset(void);
void InterruptRegister(
        TUInt32 aInterruptChannel, 
        TISRFPtr aISR, 
        T_irqPriority aPriority,
        const char * const aName);
void InterruptUnregister(TUInt32 aInterruptChannel);
TBool InterruptIsRegistered(TUInt32 aInterruptChannel);
void InterruptFatalError(void);

void InterruptEnable(T_irqChannel aChannel);
void InterruptDisable(T_irqChannel aChannel);

void InterruptDisableAllRegistered(void);
void InterruptEnableAllRegistered(void);

#endif // _INTERRUPT_H_
/*-------------------------------------------------------------------------*
 * End of File:  Interrupt.h
 *-------------------------------------------------------------------------*/
