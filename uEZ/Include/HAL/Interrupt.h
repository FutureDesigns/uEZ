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


#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Includes:
 *---------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

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
    INTERRUPT_PRIORITY_HIGH=1,
    INTERRUPT_PRIORITY_NORMAL=3,
    INTERRUPT_PRIORITY_LOW=4,
    INTERRUPT_PRIORITY_LOWEST=5
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
#if (COMPILER_TYPE==GCC)
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

#ifdef __cplusplus
}
#endif

#endif // _INTERRUPT_H_
/*-------------------------------------------------------------------------*
 * End of File:  Interrupt.h
 *-------------------------------------------------------------------------*/
