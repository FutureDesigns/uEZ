/*-------------------------------------------------------------------------*
 * File:  SEGGER_RTT_SYSVIEW_Config.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Configur RTT, Systemview, and some RTOS parameters from a central
 * header file. We can use this same header in both Library and App projects.
 * This simplified header avoids build issues when trying to configure RTT
 * in a config build project. Also typical Config_Build.h files can't be used
 * in both library and application build projects.
 *-------------------------------------------------------------------------*/

 /*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2023 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#ifndef SEGGER_RTT_SYSVIEW_CONFIG_H_
#define SEGGER_RTT_SYSVIEW_CONFIG_H_

#ifdef DEBUG // Debug Build Unique Settings
  #define configUSE_TRACE_FACILITY      1 // This doesn't seem to negatively affect even video player so we can leave it on by default. May slow down Ethernet?
  #define configGENERATE_RUN_TIME_STATS 0 // enable trace facility to use this with stats formatting functions.

  // When we change RTT or Systemview we need to do a full clean and rebuild of both library and app project!
  #define SEGGER_ENABLE_RTT              0
  #define SEGGER_ENABLE_EMWINSPY_RTT     0
  #define SEGGER_ENABLE_SYSTEM_VIEW      0
  //#define configPRINTF( X )   DEBUG_RTT_Printf0(X) // uncomment to enable the FreeRTOS printf,  mainly for TCP and AWS SW

#else // Release Build Unique Settings
  #define configUSE_TRACE_FACILITY       0
  #define configGENERATE_RUN_TIME_STATS  0

// When we change RTT or Systemview we need to do a full clean and rebuild of both library and app project!
  #define SEGGER_ENABLE_RTT              0
  #define SEGGER_ENABLE_EMWINSPY_RTT     0
  #define SEGGER_ENABLE_SYSTEM_VIEW      0
// #define configPRINTF( X )   DEBUG_RTT_Printf0(X) // uncomment to enable the FreeRTOS printf,  mainly for TCP and AWS SW
#endif


// Common Debug/Release library build settings.

// With accurate Timestamp even with task and ISR turned off we still have accurate time reporting on the graph for printout functions. So more useful than RTT.
#define SEGGER_ENABLE_SV_ISR        1 // 1 == Enable ISR reporting. Some FreeRTOS stuff may not be setup correctly for the ISR systemview timestamps. Also when we do Systemview print from inside ISR it seems to miss most of them. So not useful for printing currently.
#define SEGGER_ENABLE_SV_TASK       1 // 1 == Enable task reporting. This requires faster JTAG/SWD speeds

#define SEGGER_ENABLE_PRINTF        1 // Not recommended for micros with smaller memories, some printf types don't actually work.
#define SEGGER_ENABLE_INPUT         0 // TODO enable command receive routine/console

#if (SEGGER_ENABLE_RTT ==1 )
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#endif

#endif /* SEGGER_RTT_SYSVIEW_CONFIG_H_ */
