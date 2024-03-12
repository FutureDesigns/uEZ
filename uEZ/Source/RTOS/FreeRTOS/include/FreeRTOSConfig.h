/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//#include <Config.h>

// Get prio bits, mpu present, core type, fpu present, and maybe even systick settings from MCU header.
#include <uEZProcessor.h> // Avoid including UtilityFuncs.h as it leads to malloc redefine!
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

// Select the RTOS timer here: 
// Platform specific

// Set USING_TICK_SUPRESSION to 1 to run with tickless idle mode for lower power.
#define USING_TICK_SUPRESSION		0 // TODO not tested in any included ports yet

#define configUSE_TICKLESS_IDLE		USING_TICK_SUPRESSION

#ifndef configCPU_CLOCK_HZ
    #define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) PROCESSOR_OSCILLATOR_FREQUENCY )
#endif

#define configTICK_RATE_HZ		( ( TickType_t ) 1000 )
#ifndef configTOTAL_HEAP_SIZE
 // Default size, should be overriden in application project. Many GUI projects require larger heap.
   #define configTOTAL_HEAP_SIZE        ( ( size_t ) (( 3000 * 1024 ) - 64))
#endif
#ifndef configMAX_TASK_NAME_LEN
    #define configMAX_TASK_NAME_LEN     ( 16 )
#endif
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES		1
#define configUSE_RECURSIVE_MUTEXES	1
#define configUSE_COUNTING_SEMAPHORES   1
#define configQUEUE_REGISTRY_SIZE	30 // Allow a few named queue entries
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_PREEMPTION		1
#define configLIST_VOLATILE         volatile

#define configMAX_PRIORITIES		( 7 )

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#ifdef DEBUG // Debug Build Unique Settings

#define configUSE_IDLE_HOOK			0 // TODO we may need this enabled for some things?
#define configUSE_TICK_HOOK			0

#define configUSE_MALLOC_FAILED_HOOK		1

// See the comment at end of file for description of each setting.
//#define configCHECK_FOR_STACK_OVERFLOW	1
#define configCHECK_FOR_STACK_OVERFLOW		2
#define INCLUDE_uxTaskGetStackHighWaterMark	1 //watermark2 is same but for configurable configSTACK_DEPTH_TYPE, we use default value
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0 // leave off for task inspection in debugger

#else // Release Build Unique Settings
   
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0

#define configUSE_MALLOC_FAILED_HOOK		0 // For release build we should know that we aren't overflowing at boot-up.
#define configCHECK_FOR_STACK_OVERFLOW		0
#define INCLUDE_uxTaskGetStackHighWaterMark	0

#ifdef CORE_M0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0 // see portmacro, this port doesn't have optimizated task selection
#else
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1 // will interfer with task inspection tools
#endif

#endif /* #ifdef DEBUG */

/* Software timer definitions - only included when timer module is enabled. */
#if (USING_TICK_SUPRESSION == 1)
	#define configUSE_TIMERS		0
#else

#ifdef FREERTOS_PLUS_TRACE
  #define configUSE_TRACE_FACILITY            1
  #define configUSE_TIMERS                    1
  #define SELECTED_PORT PORT_ARM_CortexM
  #define configTIMER_INDEX                   1    // 0=Timer0 or 1=Timer1
  #define configTIMER_TASK_PRIORITY	( 3 )
  #define configTIMER_QUEUE_LENGTH	5
  #define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE )
#else
  #if (DISABLE_FEATURES_FOR_BOOTLOADER == 1) // Disable some features for smaller bootloader projects.
    #define configUSE_TIMERS            0
  #else
    #define configUSE_TIMERS		1 // Use starting in 2.14 demos
  #endif
    #define configTIMER_INDEX           1    // 0=Timer0 or 1=Timer1
    #define configTIMER_TASK_PRIORITY	( 3 )
    #define configTIMER_QUEUE_LENGTH	5
    #define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE )
#endif
#endif /* USING_TICK_SUPRESSION */


#ifdef FREERTOS_PLUS_TRACE
//#include <Include/trcKernelPort.h> // TODO need to update FreeRTOS-Plus-Trace to be able to use this again, but we don't have license.
#endif

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1) // Disable some features for smaller bootloader projects.
    #define INCLUDE_vTaskDelete                 1
    #define INCLUDE_xTaskDelayUntil             0
    #define INCLUDE_xTaskGetSchedulerState      1

    #define INCLUDE_vTaskPrioritySet            0
    #define INCLUDE_xTaskGetIdleTaskHandle      0
    #define INCLUDE_vTaskSuspend                0
    #define INCLUDE_pxTaskGetStackStart         0
    #define INCLUDE_uxTaskPriorityGet           0
    #define INCLUDE_xTaskGetCurrentTaskHandle	0
    #define INCLUDE_xTaskGetHandle              0

    #define configSUPPORT_STATIC_ALLOCATION     1
    #define configUSE_C_RUNTIME_TLS_SUPPORT     0
#endif


#ifndef configSUPPORT_STATIC_ALLOCATION
    #define configSUPPORT_STATIC_ALLOCATION     1
#endif

#ifndef configSUPPORT_DYNAMIC_ALLOCATION
    #define configSUPPORT_DYNAMIC_ALLOCATION    1
#endif

#ifndef configUSE_C_RUNTIME_TLS_SUPPORT
    #define configUSE_C_RUNTIME_TLS_SUPPORT     0 // TODO
#endif

#if ( configUSE_C_RUNTIME_TLS_SUPPORT == 1 )
    #ifndef configTLS_BLOCK_TYPE
      #define configTLS_BLOCK_TYPE // TODO
    #endif

    #ifndef configINIT_TLS_BLOCK
      #define configINIT_TLS_BLOCK // TODO
    #endif

    #ifndef configSET_TLS_BLOCK
      #define configSET_TLS_BLOCK // TODO
    #endif

    #ifndef configDEINIT_TLS_BLOCK
      #define configDEINIT_TLS_BLOCK // TODO
    #endif
#endif

#ifndef INCLUDE_vTaskPrioritySet
    #define INCLUDE_vTaskPrioritySet            1
#endif
#ifndef INCLUDE_uxTaskPriorityGet
    #define INCLUDE_uxTaskPriorityGet           1
#endif
#ifndef INCLUDE_vTaskDelete
    #define INCLUDE_vTaskDelete                 1
#endif
#ifndef INCLUDE_vTaskCleanUpResources
    #define INCLUDE_vTaskCleanUpResources       0
#endif
#ifndef INCLUDE_vTaskSuspend
    #define INCLUDE_vTaskSuspend                1
#endif
#ifndef INCLUDE_xTaskDelayUntil
    #define INCLUDE_xTaskDelayUntil             1
#endif
#ifndef INCLUDE_vTaskDelay
    #define INCLUDE_vTaskDelay                  1
#endif
#ifndef INCLUDE_xTaskGetCurrentTaskHandle
    #define INCLUDE_xTaskGetCurrentTaskHandle	1
#endif
#ifndef INCLUDE_xTaskGetHandle
    #define INCLUDE_xTaskGetHandle              0
#endif
#ifndef INCLUDE_xTaskGetSchedulerState
    #define INCLUDE_xTaskGetSchedulerState      1
#endif

#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__  //Ensure the following is only used by the compiler, and not the assembler.

/* The configPRE_SLEEP_PROCESSING() and configPOST_SLEEP_PROCESSING() macros
allow the application writer to add additional code before and after the MCU is
placed into the low power state respectively.  The implementations provided in
this demo can be extended to save even more power - for example the analog
input used by the low power demo could be switched off in the pre-sleep macro
and back on again in the post sleep macro. */
void vPreSleepProcessing( unsigned long xExpectedIdleTime );
void vPostSleepProcessing( unsigned long xExpectedIdleTime );
#define configPRE_SLEEP_PROCESSING( xExpectedIdleTime ) vPreSleepProcessing( xExpectedIdleTime );
#define configPOST_SLEEP_PROCESSING( xExpectedIdleTime ) vPostSleepProcessing( xExpectedIdleTime );

// New to FreeRTOS 7.
extern void vAssertCalled( void );

#endif
#endif

/* configCHECK_FOR_STACK_OVERFLOW settings: 0, 1, or 2
 * Stack Overflow Detection - Method 1
It is likely that the stack will reach its greatest (deepest) value after the RTOS
kernel has swapped the task out of the Running state because this is when the
stack will contain the task context. At this point the RTOS kernel can check that
the processor stack pointer remains within the valid stack space. The stack
overflow hook function is called if the stack pointer contain a value that is
outside of the valid stack range. This method is quick but not guaranteed to
catch all stack overflows.
*
*  Stack Overflow Detection - Method 2
When a task is first created its stack is filled with a known value. When swapping
a task out of the Running state the RTOS kernel can check the last 16 bytes within
the valid stack range to ensure that these known values have not been overwritten
by the task or interrupt activity. The stack overflow hook function is called
should any of these 16 bytes not remain at their initial value.
This method is less efficient than method one, but still fairly fast. It is very
likely to catch stack overflows but is still not guaranteed to catch all overflows.*/

#ifndef UEZ_DEBUG_HEAVY_ASSERTS
#define UEZ_DEBUG_HEAVY_ASSERTS     1
#endif

#if (UEZ_DEBUG_HEAVY_ASSERTS == 1)
#define configASSERT( x ) if( ( x ) == 0 ) \
    { extern void UEZBSP_FatalError(int32_t aCode); \
        UEZBSP_FatalError(25); }
    //#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled(); // new application specific call
#else // disable the freeRTOS assert (defned in FreeRTOSConfig.h to empty by default)    
#endif
      
#ifdef __NVIC_PRIO_BITS    /* Use the system definition, if there is one */
    #define configPRIO_BITS       __NVIC_PRIO_BITS // Can be vender/MCU specific
#else // Let us FORCE using the __NVIC_PRIO_BITS to avoid mismatch.
    #error Missing prio bits define!
#endif
     
/* The interrupt priority used by the kernel itself for the tick interrupt and
the pended interrupt.  This would normally be the lowest priority. */

/* The maximum interrupt priority from which FreeRTOS API calls can be made.
Interrupts that use a priority above this will not be effected by anything the
kernel is doing. */

#ifdef CORE_M0    
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) ) /* The lowest priority. */
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#ifndef configMINIMAL_STACK_SIZE
    #define configMINIMAL_STACK_SIZE	( ( unsigned short )128 )
    #define configUSE_TASK_FPU_SUPPORT            0
#endif
#endif

#ifdef CORE_M3
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) ) /* The lowest priority. */
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#ifndef configMINIMAL_STACK_SIZE
    #define configMINIMAL_STACK_SIZE	( ( unsigned short )128 ) // 100 still ok?
    #define configUSE_TASK_FPU_SUPPORT            0
#endif
#endif

#ifdef CORE_M4
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) ) /* The lowest priority. */
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#ifndef configMINIMAL_STACK_SIZE
    #define configMINIMAL_STACK_SIZE	( ( unsigned short )128 )
#endif
    #define configUSE_TASK_FPU_SUPPORT            1
#endif

#ifdef CORE_M33
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) ) /* The lowest priority. */
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#ifndef configMINIMAL_STACK_SIZE
    #define configMINIMAL_STACK_SIZE	( ( unsigned short )192 )
    #define configUSE_TASK_FPU_SUPPORT            1
#endif
#endif

// Set these paths specific to your project.

#include "SEGGER_RTT_SYSVIEW_Config.h"

#ifdef DEBUG // Debug Build Unique Settings

#ifndef configUSE_TRACE_FACILITY
  #define configUSE_TRACE_FACILITY	        1 // needed for vTaskList
#endif
#ifndef configGENERATE_RUN_TIME_STATS
  #define configGENERATE_RUN_TIME_STATS		1 // 
#endif

#else // Release Build Unique Settings

#ifndef configUSE_TRACE_FACILITY
 #define configUSE_TRACE_FACILITY      0
#endif
#ifndef configGENERATE_RUN_TIME_STATS
 #define configGENERATE_RUN_TIME_STATS 0
#endif

#endif /* #ifdef DEBUG */

#if (configUSE_TRACE_FACILITY == 1)
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#else

#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS  UEZConfigureTimerForRunTimeStats
#define portGET_RUN_TIME_COUNTER_VALUE          UEZGetRunTimeStatsCounter
#endif

#ifdef FREERTOS_PLUS_TRACE
  // Don't enable SystemView with FreeRTOS+Trace
#else // Otherwise SystemView can be enabled
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // Only include if SystemView is enabled
// Force these RTOS options on for Systemview, otherwise optional.
#define INCLUDE_eTaskGetState                                   1
#define INCLUDE_xTaskGetIdleTaskHandle                          1
#define INCLUDE_pxTaskGetStackStart                             1

#if (COMPILER_TYPE == GCC_ARM) 
#ifdef __clang__ // include in case CLANG needs special treatment
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif
      
#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__  
     //Ensure the #include is only used by the compiler, and not the assembler.
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif
#endif
      
#if (COMPILER_TYPE == KEIL_UV) // TODO compiler not verified yet
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#if (defined __CCRX__) // TODO compiler not verified yet
     #include <Source/Library/SEGGER//SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#endif
#endif


#ifndef INCLUDE_pxTaskGetStackStart 
    #define INCLUDE_pxTaskGetStackStart         1
#endif
#ifndef INCLUDE_eTaskGetState
    #define INCLUDE_eTaskGetState               1
#endif
#ifndef INCLUDE_xTaskGetIdleTaskHandle  
    #define INCLUDE_xTaskGetIdleTaskHandle      1
#endif


#endif /* FREERTOS_CONFIG_H */
