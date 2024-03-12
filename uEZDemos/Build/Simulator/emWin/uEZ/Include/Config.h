/*-------------------------------------------------------------------------*
* File:  <Config.h>
*-------------------------------------------------------------------------*/

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
 *  @file   Include/Config.h
 *  @brief  uEZ Config
 *
 *  Configuration of all accepted modes of operation
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#define UEZ_VERSION_MAJOR           2
#define UEZ_VERSION_MINOR           14
#define UEZ_VERSION_STRING          "2.14.000"
#define UEZ_VERSION_DATE            "12/05/2023" /** MM/DD/YYYY */

/*-------------------------------------------------------------------------*
 * Option: Compiler
 *-------------------------------------------------------------------------*/
/** List of Compilers */
#define GCC_ARM                     1
#define IAR                         2
#define KEIL_UV                     3
#define RENESASRX                   4
#define RENESASRX_GCC               5
#define VisualC                     6

// Hopefully we don't need to differentiate between GCC and Clang
// So for right now don't use the CLANG define here, but use #ifdef __clang__ 
// for any compatibility issues that crop up.
//#define CLANG    9 

/** List of RTOS's */
#define FreeRTOS                    1
#define SafeRTOS                    2

/*-------------------------------------------------------------------------*
 * Option: Processor
 *-------------------------------------------------------------------------*/
/** List of processor types */
#define NXP_LPC2478                             1
#define NXP_LPC1768                             2
#define RENESAS_H8SX_1668RF                     3
#define NXP_LPC1788                             4  // actively supported/tested
#define RENESAS_RX62N                           5
#define STMICRO_STM32F105_7                     6
#define NXP_LPC1756                             7
#define RENESAS_RX63N                           8
#define NXP_LPC4088                             9  // actively supported/tested
#define NXP_LPC4357                             10 // actively supported/tested
#define NXP_LPC546xx                            11

/** Include any specific compile options for this build */
#include <Config_Build.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Option: RTOS
 *-------------------------------------------------------------------------*/
/** Select RTOS: */
#ifndef RTOS
#define RTOS                        FreeRTOS
#endif

/** The RTOS definition is needed in Config_Build and Config_App and hence must
 *  be declared here. */
#ifndef RTOS
#error "An RTOS must be defined."
#endif

/** Select compiler */
#ifndef COMPILER_TYPE
#define COMPILER_TYPE               0
#endif

/** Get the application's configuration and requirements for the
 *  platform and processor. */
#ifndef UEZ_LIBRARY
#include <Config_App.h>
#endif

#ifndef TAG_BUILD_DATE
// #define TAG_BUILD_DATE          __DATE__
#define TAG_BUILD_DATE          UEZ_VERSION_DATE
#endif
#ifndef RELEASE_DATE
// #define RELEASE_DATE            __DATE__
#define RELEASE_DATE            UEZ_VERSION_DATE
#endif

// Select processor:
#ifndef UEZ_PROCESSOR
#error "UEZ_PROCESSOR must be defined!"
#endif

/*-------------------------------------------------------------------------*
 * Option: uEZ settings
 *-------------------------------------------------------------------------*/
#ifndef UEZ_NUM_HANDLES
#define UEZ_NUM_HANDLES           150
#endif

#ifdef __cplusplus
}
#endif

#endif // __CONFIG_H
/*-------------------------------------------------------------------------*
 * End of File:  Config.h
 *-------------------------------------------------------------------------*/
