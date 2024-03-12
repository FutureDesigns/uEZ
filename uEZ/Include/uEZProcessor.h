/*-------------------------------------------------------------------------*
 * File:  uEZPROCESSOR.h
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_PROCESSOR_H_
#define _UEZ_PROCESSOR_H_

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
 *  @file   uEZProcessor.h
 *  @brief  uEZ Processor Abstraction Layer
 *
 *  uEZ Processor Abstraction Layer
 */
/** List of processor type information */

#include "Config.h"

/** Defines for UEZ_PROCESSOR_CORE_TYPE */
#define CORE_TYPE_UNDEFINED                     0
#define CORE_TYPE_ARM7                          1
#define CORE_TYPE_CORTEX_M0                     2
#define CORE_TYPE_CORTEX_M0P                    3
#define CORE_TYPE_CORTEX_M1                     4
#define CORE_TYPE_CORTEX_M3                     5
#define CORE_TYPE_CORTEX_M4                     6
#define CORE_TYPE_CORTEX_M7                     7

/** Defines for UEZ_PROCESS_CORE_MODE */
#define CORE_MODE_UNKNOWN                       0
#define CORE_MODE_ARM                           1
#define CORE_MODE_THUMB                         2

/** Defines for UEZ_PROCESS_CORE_SUBTYPE */
#define CORE_SUBTYPE_NONE                       0
#define CORE_SUBTYPE_ARM7TDMI_S                 1
// TODO add correct subtypes if we ever use this.

#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__
     //Ensure the #include is only used by the compiler, and not the assembler.   
#include "uEZTypes.h"
#include "uEZErrors.h"
#define CURRENTLY_IN_IAR_ASM                    0
#endif
#endif

#if (COMPILER_TYPE == GCC_ARM) // 
#include "uEZTypes.h"
#include "uEZErrors.h"
#define CURRENTLY_IN_IAR_ASM                    0
#endif

#if (COMPILER_TYPE == KEIL_UV) // TODO compiler not verified yet
#include "uEZTypes.h"
#include "uEZErrors.h"
#define CURRENTLY_IN_IAR_ASM                    0
#endif

#if (defined __CCRX__) // TODO compiler not verified yet
#include "uEZTypes.h"
#include "uEZErrors.h"
#endif
   
#ifndef CURRENTLY_IN_IAR_ASM
#define CURRENTLY_IN_IAR_ASM                    1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
  
// min 4+ priority levels for CM0/CM0+, min 8+ levels for CM3/CM4
   
#if (CURRENTLY_IN_IAR_ASM == 1)
#if (UEZ_PROCESSOR==NXP_LPC2478)
#endif

#if (UEZ_PROCESSOR==NXP_LPC1768)  // 32 priority levels
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#endif

#if (UEZ_PROCESSOR==RENESAS_H8SX_1668RF)
#endif

#if (UEZ_PROCESSOR == NXP_LPC4088)
#ifdef CORE_M4 // 32 priority levels
#define __CM4_REV                 0x0000            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               5            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT                  1            /*!< FPU present or not                                                    */
#endif

#ifdef CORE_M3 // 32 priority levels
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#endif  
#endif
  
#if (UEZ_PROCESSOR==NXP_LPC1788) // 32 priority levels
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#endif

#if (UEZ_PROCESSOR==RENESAS_RX62N)
#endif

#if (UEZ_PROCESSOR==RENESAS_RX63N)
#endif

#if (UEZ_PROCESSOR==NXP_LPC1756)
#endif

#if (UEZ_PROCESSOR==STMICRO_STM32F105_7)
#endif

#if (UEZ_PROCESSOR == NXP_LPC4357)
#if defined CORE_M4 // 8 priority levels
#define __CM4_REV                 0x0000            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               3            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT                  1           /*!< FPU present or not                                                    */
#define __FPU_USED                     1U           /*!< FPU used or not                                                       */
#endif

#if defined CORE_M0 || defined CORE_M0SUB // 4 priority levels
#define __MPU_PRESENT             0                 /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS          2                 /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig    0                 /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT             0                 /*!< FPU present or not                                                    */
#endif
#endif

#if (UEZ_PROCESSOR == NXP_LPC546xx)
#endif

#endif
  
#if (CURRENTLY_IN_IAR_ASM == 0)
/** NOTE: All processors MUST support these functions: */
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val);
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val);
TUInt16 ReadLE16U(volatile TUInt8 *pmem);
TUInt16 ReadBE16U(volatile TUInt8 *pmem);
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val);
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt32 ReadLE32U (volatile TUInt8 *pmem);
TUInt32 ReadBE32U (volatile TUInt8 *pmem);

#if (UEZ_PROCESSOR==NXP_LPC2478)
#include <Source/Processor/NXP/LPC2478/uEZProcessor_LPC2478.h>
#endif

#if (UEZ_PROCESSOR==NXP_LPC1768)
#include <Source/Processor/NXP/LPC1768/uEZProcessor_LPC1768.h>
#endif

#if (UEZ_PROCESSOR==RENESAS_H8SX_1668RF)
#include "Source/Processor/Renesas/H8SX_1668RF/H8SX_1668RF.h"
#endif

#if (UEZ_PROCESSOR==NXP_LPC1788 || UEZ_PROCESSOR == NXP_LPC4088)
#include <Source/Processor/NXP/LPC17xx_40xx/uEZProcessor_LPC17xx_40xx.h>
#endif

#if (UEZ_PROCESSOR==RENESAS_RX62N)
    #if (COMPILER_TYPE==RENESASRX)
		#include <Source/Processor/Renesas/RX62N/uEZProcessor_RX62N.h>
        #include <Source/Processor/Renesas/RX62N/RXToolset/iodefine.h>
        #include <machine.h>
    #endif
#endif

#if (UEZ_PROCESSOR==RENESAS_RX63N)
    #if (COMPILER_TYPE==RENESASRX)
		#include <Source/Processor/Renesas/RX63N/uEZProcessor_RX63N.h>
        #include <Source/Processor/Renesas/RX63N/RXToolset/iodefine.h>
        #include <machine.h>
    #endif
#endif

#if (UEZ_PROCESSOR==NXP_LPC1756)
#include <Source/Processor/NXP/LPC1756/uEZProcessor_LPC1756.h>
#endif

#if (UEZ_PROCESSOR==STMICRO_STM32F105_7)
#include <Source/Processor/STMicro/STM32F105_7/STM32F105_7_Processor.h>
#endif


#if (UEZ_PROCESSOR == NXP_LPC4357)
#include <Source/Processor/NXP/LPC43xx/uEZProcessor_LPC43xx.h>
#endif

#if (UEZ_PROCESSOR == NXP_LPC546xx)
#include <Source/Processor/NXP/LPC546xx/uEZProcessor_LPC546xx.h>
#endif

#endif // currently in iar asm

/*---------------------------------------------------------------------------*
 * Macros for bsp delay:
 *---------------------------------------------------------------------------*/
#define nop()     NOP()//asm("nop")
#define nops5()    nop();nop();nop();nop();nop()
#define nops10()   nops5();nops5()
#define nops50()   nops10();nops10();nops10();nops10();nops10()
#define nops100()   nops50();nops50();

#ifdef __cplusplus
}
#endif

#endif // _UEZ_PROCESSOR_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPROCESSOR.h
 *-------------------------------------------------------------------------*/
