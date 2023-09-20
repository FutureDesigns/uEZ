/*-------------------------------------------------------------------------*
* File:  <FILE_NAME.h>
*-------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
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
#define UEZ_VERSION_MINOR           12
#define UEZ_VERSION_STRING          "2.12.000"
#define UEZ_VERSION_DATE            "10/05/2022" /** MM/DD/YYYY */

/*-------------------------------------------------------------------------*
 * Option: Compiler
 *-------------------------------------------------------------------------*/
/** List of Compilers */
#define RowleyARM                   1
#define IAR                         2
#define CodeRed                     3
#define HEW                         4
#define Keil4                       5
#define RenesasRX                   6
#define VisualC                     7
#define GCC                         8
// Hopefully we don't need to differentiate between GCC and Clang
// So for right now don't use the CLANG define here, but use #ifdef __clang__ 
// for any compatibility issues that crop up.
//#define CLANG    9 

/** List of RTOS's */
#define FreeRTOS                    1
#define SafeRTOS                    2

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

#ifdef __CODE_RED
#define COMPILER_TYPE CodeRed
#endif

/** Select compiler */
#ifndef COMPILER_TYPE
#define COMPILER_TYPE               RowleyARM
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

/*-------------------------------------------------------------------------*
 * Option: Processor
 *-------------------------------------------------------------------------*/
/** List of processor types */
#define NXP_LPC2478                             1
#define NXP_LPC1768                             2
#define RENESAS_H8SX_1668RF                     3
#define NXP_LPC1788                             4
#define RENESAS_RX62N                           5
#define STMICRO_STM32F105_7                     6
#define NXP_LPC1756                             7
#define RENESAS_RX63N                           8
#define NXP_LPC4088                             9
#define NXP_LPC4357                             10
#define NXP_LPC546xx                            11

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
