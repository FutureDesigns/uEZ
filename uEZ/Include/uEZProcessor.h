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
#include "Config.h"
#include "uEZTypes.h"
#include "uEZErrors.h"

#ifdef __cplusplus
extern "C" {
#endif

/** List of processor type information */

/** Defines for UEZ_PROCESSOR_CORE_TYPE */
#define CORE_TYPE_UNDEFINED                     0
#define CORE_TYPE_ARM7                          1
#define CORE_TYPE_CORTEX_M3                     2
#define CORE_TYPE_CORTEX_M4                     3

/** Defines for UEZ_PROCESS_CORE_MODE */
#define CORE_MODE_UNKNOWN                       0
#define CORE_MODE_ARM                           1
#define CORE_MODE_THUMB                         2

/** Defines for UEZ_PROCESS_CORE_SUBTYPE */
#define CORE_SUBTYPE_NONE                       0
#define CORE_SUBTYPE_ARM7TDMI_S                 1


/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
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
    #if (COMPILER_TYPE==RenesasRX)
		#include <Source/Processor/Renesas/RX62N/uEZProcessor_RX62N.h>
        #include <Source/Processor/Renesas/RX62N/RXToolset/iodefine.h>
        #include <machine.h>
    #endif
#endif

#if (UEZ_PROCESSOR==RENESAS_RX63N)
    #if (COMPILER_TYPE==RenesasRX)
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

#ifdef __cplusplus
}
#endif

#endif // _UEZ_PROCESSOR_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPROCESSOR.h
 *-------------------------------------------------------------------------*/
