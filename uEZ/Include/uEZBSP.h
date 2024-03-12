/*-------------------------------------------------------------------------*
 * File:  uEZBSP.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_BSP_H_
#define _UEZ_BSP_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   uEZBSP.h
 *  @brief  uEZ BSP
 *
 *  uEZ BSP
 */
#include "Config.h"
#include "uEZTypes.h"
#include "uEZRTOS.h"

/* ---------- Assembly-Language Function Inline Expansion Macros ---------- */
#if defined (__IAR_SYSTEMS_ICC__)
  #define PRAGMA(A) _Pragma(#A)
#endif

#if  defined(__GNUC__)
// TODO inline ASM BSP

#elif defined(__IAR_SYSTEMS_ICC__)
// TODO inline ASM BSP
#endif

#define FDI_BSP_ASM_INTERNAL_USED(p)        ((void)(p));

#if defined(__CDT_PARSER__) // hide in hopefully all eclipse parsers

#define FDI_BSP_ASM_LINE(...)            /* none */
#define FDI_BSP_ASM_LAB_NEXT(n)     /* none */
#define FDI_BSP_ASM_LAB_PREV(n)     /* none */
#define FDI_BSP_ASM_LAB(n_colon)    /* none */
#define FDI_BSP_ASM_FUNC_BEGIN           /* none */
#define FDI_BSP_ASM_FUNC_END             /* none */

#else // not using parser

#if defined(__GNUC__)

#define _FDI_BSP_ASM(...)           #__VA_ARGS__
#define FDI_BSP_ASM_LINE(...)       _FDI_BSP_ASM(__VA_ARGS__\n)
#define FDI_BSP_ASM_LINE2(...)      #__VA_ARGS__
#define FDI_BSP_ASM_LAB_NEXT(n)     n+
#define FDI_BSP_ASM_LAB_PREV(n)     n-
#define FDI_BSP_ASM_LAB(n_colon)    FDI_BSP_ASM_LINE(n_colon:)
#define FDI_BSP_ASM_FUNC_BEGIN      __asm__ volatile (
#define FDI_BSP_ASM_FUNC_END        );

#elif defined(__IAR_SYSTEMS_ICC__)
// For IAR the format should look as follows with required newlines inserted
// at the end of each statemett inside of the quote marks.
// IAR requires all statements in a single asm(); to find lables correctly.
// asm("lN R20, 0x37 \n"     "looplabel: \n"     "RJMP looplabel \n");
// You must keep this diag suppressed or it will trigger on every used line.
#pragma diag_suppress=Pe007 // IAR will complain about the \n if you we don't suppress this one.
#define _FDI_BSP_ASM(...)           #__VA_ARGS__
#define FDI_BSP_ASM_LINE(...)       _FDI_BSP_ASM(__VA_ARGS__\n) 
//#define FDI_BSP_ASM_LAB_NEXT(n)     _lab##n // TODO
//#define FDI_BSP_ASM_LAB_PREV(n)     _lab##n // TODO
#define FDI_BSP_ASM_LAB(n_colon)    FDI_BSP_ASM_LINE(n_colon:)
#define FDI_BSP_ASM_FUNC_BEGIN      asm volatile(
#define FDI_BSP_ASM_FUNC_END        );

#endif

#endif /* defined(__CDT_PARSER__) */

/* ---------- Assembly-Language Function Inline Expansion Macros ---------- */

#ifdef __cplusplus
extern "C" {
#endif

void UEZBSP_Startup(void);
void UEZBSP_Pre_PLL_SystemInit(void);
void UEZBSP_PLLConfigure(void);
void UEZBSP_CPU_PinConfigInit(void);
void UEZBSP_Post_SystemInit(void);
void UEZBSP_InterruptsReset(void);
void UEZBSP_RAMInit(void);
void UEZBSP_ROMInit(void);
void UEZBSP_FatalError(int32_t aNumBlinks);

/** Utility routine to delay a number of ms (approximately). */
void UEZBSPDelayMS(uint32_t aMilliseconds);
void UEZBSPDelayUS(uint32_t aMicroseconds);

void uEZProcessorServicesInit(void);
void uEZPlatformInit(void);

#if (configGENERATE_RUN_TIME_STATS == 1)
T_uezError UEZBSP_ConfigureTimerForRunTimeStats(void);
configRUN_TIME_COUNTER_TYPE UEZBSP_GetGetRunTimeStatsCounter(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //_UEZ_BSP_H_

/*-------------------------------------------------------------------------*
 * File:  uEZBSP.h
 *-------------------------------------------------------------------------*/

