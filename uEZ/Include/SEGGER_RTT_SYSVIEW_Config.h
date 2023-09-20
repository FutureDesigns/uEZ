/*
 * SEGGER_RTT_SYSVIEW_Config.h
 *
 *  Created on: Jan 4, 2021
 *      Author: bill.fleming
 */

#ifndef SEGGER_RTT_SYSVIEW_CONFIG_H_
#define SEGGER_RTT_SYSVIEW_CONFIG_H_

// When we change RTT or Systemview we need to do a full clean and rebuild of both library and app project!
#define SEGGER_ENABLE_RTT           0
#define SEGGER_ENABLE_SYSTEM_VIEW   0

// With accurate Timestamp even with task and ISR turned off we still have accurate time reporting on the graph for printout functions. So more useful than RTT.
#define SEGGER_ENABLE_SV_ISR        1 // 1 == Enable ISR reporting. Some FreeRTOS stuff may not be setup correctly for the ISR systemview timestamps. Also when we do Systemview print from inside ISR it seems to miss most of them. So not useful for printing currently.
#define SEGGER_ENABLE_SV_TASK       1 // 1 == Enable task reporting. This requires faster JTAG/SWD speeds

#define SEGGER_ENABLE_PRINTF        0 // Not recommended for micros with smaller memories, some printf types don't actually work.
#define SEGGER_ENABLE_INPUT         0 // TODO enable command receive routine/console

#if (SEGGER_ENABLE_RTT ==1 )
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#endif

#endif /* SEGGER_RTT_SYSVIEW_CONFIG_H_ */
