/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.62                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Config_FreeRTOS.c
Purpose : Sample setup configuration of SystemView with FreeRTOS.
Revision: $Rev: 7745 $
*/
#include "FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include <uEZPlatform.h> // Below is platform specific things such as tick rate and any specific timestamp functions that are needed on some platforms.

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

// The target device name
#if (UEZ_PROCESSOR == NXP_LPC1788)
  #define SYSVIEW_DEVICE_NAME     "LPC17XX"
#elif (UEZ_PROCESSOR == NXP_LPC4088)
  #define SYSVIEW_DEVICE_NAME     "LPC40XX"
#elif (UEZ_PROCESSOR == NXP_LPC4357)
  #define SYSVIEW_DEVICE_NAME     "LPC43XXX"
  #ifdef CORE_M4
    #define SYSVIEW_APP_NAME        "uEZ FreeRTOS - Main Core"
    #define SEGGER_SYSVIEW_CORE_NAME       "M4MAIN"
  #else
    #define SYSVIEW_APP_NAME        "uEZ FreeRTOS - App Core"
    #define SEGGER_SYSVIEW_CORE_NAME       "M0APP"
  #endif
#elif (UEZ_PROCESSOR == NXP_LPC546xx)
  #define SYSVIEW_DEVICE_NAME     "LPC546XX"
#else
  #define SYSVIEW_DEVICE_NAME     "Device_Name"
#endif

#ifndef SYSVIEW_APP_NAME
// The application name to be displayed in SystemViewer
#define SYSVIEW_APP_NAME        "uEZ FreeRTOS Application"
//#define SYSVIEW_APP_NAME      "FreeRTOS Demo Application"
#endif

#ifndef SEGGER_SYSVIEW_CORE_NAME
#define SEGGER_SYSVIEW_CORE_NAME       "MAIN"
#endif

// Frequency of the timestamp. Must match SEGGER_SYSVIEW_GET_TIMESTAMP in SEGGER_SYSVIEW_Conf.h
#define SYSVIEW_TIMESTAMP_FREQ  (configCPU_CLOCK_HZ)

// System Frequency. SystemcoreClock is used in most CMSIS compatible projects.
#define SYSVIEW_CPU_FREQ        configCPU_CLOCK_HZ

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (0x10000000) // TODO this may need to be changed for some platforms


#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)

/********************************************************************* 
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
  SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",C="SEGGER_SYSVIEW_CORE_NAME",D="SYSVIEW_DEVICE_NAME",O=FreeRTOS");
//  SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",D="SYSVIEW_DEVICE_NAME",O=FreeRTOS"); // legacy
//  SEGGER_SYSVIEW_SendSysDesc("N=FreeRTOS Application,D=undefined device,O=FreeRTOS"); // Use this line for Toolchains where the defines don't work here.
  SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick");
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void SEGGER_SYSVIEW_Conf(void) {
  SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ, 
                      &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
  SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
}

#endif

/*************************** End of file ****************************/
