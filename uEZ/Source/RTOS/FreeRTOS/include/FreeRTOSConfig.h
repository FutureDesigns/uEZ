/*
	FreeRTOS V4.6.1 - Copyright (C) 2003-2005 Richard Barry.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license
	and contact details.  Please ensure to read the configuration and relevant
	port sections of the online documentation.
	***************************************************************************
*/

//                        ******************************
//                        *This file is now deprecated!*
//                        * Just put these settings in *
//                        * normal Config_xxx files    *
//                        ******************************

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <Config.h>

#define vPortYieldProcessor swi_handler
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#ifndef configUSE_PREEMPTION
    #define configUSE_PREEMPTION		1
#endif

#ifndef configUSE_IDLE_HOOK
    #define configUSE_IDLE_HOOK         0
#endif

#ifndef configUSE_TICK_HOOK
    #define configUSE_TICK_HOOK         0
#endif

#ifndef configCPU_CLOCK_HZ
    #define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) PROCESSOR_OSCILLATOR_FREQUENCY )
#endif

#ifndef configTICK_RATE_HZ
    #define configTICK_RATE_HZ          ( ( portTickType ) 1000 )
#endif

#ifndef configMAX_PRIORITIES
    #define configMAX_PRIORITIES		( 7 )
#endif

#ifndef configMINIMAL_STACK_SIZE
    #define configMINIMAL_STACK_SIZE	( 128 )
#endif

#ifndef configTOTAL_HEAP_SIZE
    #define configTOTAL_HEAP_SIZE		( ( size_t ) (( 23 * 1024 ) - 64)) // was 32 K
    //#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 33 * 1024 ) - 64)) // was 32 K
#endif

#ifndef configMAX_TASK_NAME_LEN
    #define configMAX_TASK_NAME_LEN		( 16 )
#endif

#ifndef configUSE_TRACE_FACILITY
    #define configUSE_TRACE_FACILITY	0 // needed for vTaskList
#endif

#if configUSE_TRACE_FACILITY
    #if ((COMPILER_TYPE!=IAR)||(defined(__ICCARM__)))
        #include <Include/trcKernelPort.h>
    #endif
#endif

#ifndef configUSE_16_BIT_TICKS
    #define configUSE_16_BIT_TICKS		0
#endif

#ifndef configIDLE_SHOULD_YIELD
    #define configIDLE_SHOULD_YIELD		1
#endif

#ifndef configUSE_MUTEXES
    #define configUSE_MUTEXES           1
#endif

#ifndef configGENERATE_RUN_TIME_STATS
    #define configGENERATE_RUN_TIME_STATS	0
#endif

#ifndef configUSE_COUNTING_SEMAPHORES
    #define configUSE_COUNTING_SEMAPHORES   1
#endif

#ifndef configUSE_RECURSIVE_MUTEXES
    #define configUSE_RECURSIVE_MUTEXES     1
#endif

/* Co-routine definitions. */
#ifndef configUSE_CO_ROUTINES
    #define configUSE_CO_ROUTINES 		0
#endif

#ifndef configMAX_CO_ROUTINE_PRIORITIES
    #define configMAX_CO_ROUTINE_PRIORITIES ( 2 )
#endif


/* Software timer definitions. */
#ifndef configUSE_TIMERS
    #define configUSE_TIMERS				0
#endif

#ifndef configTIMER_TASK_PRIORITY
    #define configTIMER_TASK_PRIORITY		( 3 )
#endif

#ifndef configTIMER_QUEUE_LENGTH
    #define configTIMER_QUEUE_LENGTH		5
#endif

#ifndef configTIMER_TASK_STACK_DEPTH
    #define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE )
#endif


#ifndef configTIMER_INDEX
    #define configTIMER_INDEX                 1    // 0=Timer0 or 1=Timer1
#endif


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#ifndef INCLUDE_xTaskGetIdleTaskHandle  
    #define INCLUDE_xTaskGetIdleTaskHandle      1
#endif
#ifndef INCLUDE_pxTaskGetStackStart 
    #define INCLUDE_pxTaskGetStackStart         1
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
#ifndef INCLUDE_vTaskDelayUntil
    #define INCLUDE_vTaskDelayUntil             1
#endif
#ifndef INCLUDE_vTaskDelay
    #define INCLUDE_vTaskDelay                  1
#endif
#ifndef INCLUDE_xTaskGetCurrentTaskHandle
    #define INCLUDE_xTaskGetCurrentTaskHandle	1
#endif

#ifndef configCHECK_FOR_STACK_OVERFLOW
//#define configCHECK_FOR_STACK_OVERFLOW 0 // turn off by default
#define configCHECK_FOR_STACK_OVERFLOW 2
#endif

// We cannot include //#include <uEZProcessor.h> here so we must set Priobits manually per MCU if needed!

#ifdef GCC_ARMCM0
    /* Use the system definition, if there is one */
    #ifdef __NVIC_PRIO_BITS
        #define configPRIO_BITS       __NVIC_PRIO_BITS // Cccan be vender specific
    #else
        #define configPRIO_BITS       2        // min 4+ priority levels for CM0/CM0+
    #endif

    /* The lowest priority. */
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) )
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#endif

#ifdef GCC_ARMCM3
    /* Use the system definition, if there is one */
    #ifdef __NVIC_PRIO_BITS
        #define configPRIO_BITS       __NVIC_PRIO_BITS
    #else
        #define configPRIO_BITS       5        /* 32 priority levels */ // TODO Cortex-M3 is minimum 3, so make this 3 if specific number not known
    #endif

    /* The lowest priority. */
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) )
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#endif

#ifdef GCC_ARMCM4F
    /* Use the system definition, if there is one */
    #ifdef __NVIC_PRIO_BITS
        #define configPRIO_BITS       __NVIC_PRIO_BITS
    #else
        #define configPRIO_BITS       5        /* 32 priority levels */ // TODO Cortex-M4 is minimum 3, so make this 3 if specific number not known
    #endif

    /* The lowest priority. */
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) )
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#endif

#if (COMPILER_TYPE==IAR)
    /* Use the system definition, if there is one */
    #ifdef __NVIC_PRIO_BITS
        #define configPRIO_BITS       __NVIC_PRIO_BITS
    #else
        #if (UEZ_PROCESSOR == NXP_LPC4357)      //CS, LPC43xx not setup properly
          #define configPRIO_BITS       3        /* 8 priority levels */
        #else
          #define configPRIO_BITS       5        /* 32 priority levels */
        #endif
    #endif

    /* The lowest priority. */
    #define configKERNEL_INTERRUPT_PRIORITY     ( 31 << (8 - configPRIO_BITS) )
    /* Priority 5, or 160 as only the top three bits are implemented. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << (8 - configPRIO_BITS) )
#endif

#if 0 // ???
#ifndef sbi
#define sbi(x,y)	x|=(1 << (y))
#endif

#ifndef cbi
#define cbi(x,y)	x&=~(1 << (y))
#endif

#ifndef tstb
#define tstb(x,y)	(x & (1 << (y)) ? 1 : 0)
#endif

#ifndef toggle
#define toggle(x,y)	x^=(1 << (y))
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))
#endif

#endif

#define VICVectAddr    VICAddress
#define VICVectCntl0 VICVectPriority0
#define VICVectCntl4 VICVectPriority4
#define VICVectCntl5 VICVectPriority5

// New to FreeRTOS 7.
//ToDo: turn back on and figure out why this doesn't work
#define configASSERT( x ) if( ( x ) == 0 ) \
    { extern void UEZBSP_FatalError(int aCode); \
        UEZBSP_FatalError(25); }

// Allow a few named queue entries
#define configQUEUE_REGISTRY_SIZE			( 30 )

// Currently we are missing vApplicationMallocFailedHook in the code
#define configUSE_MALLOC_FAILED_HOOK        ( 0 ) //CS Added

#if FREERTOS_PLUS_TRACE 
  // Don't enable SystemView with FreeRTOS+Trace
#else // Otherwise SystemView can be enabled
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // Only include if SystemView is enabled

#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__  
     //Ensure the #include is only used by the compiler, and not the assembler.
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif
#endif

#if (COMPILER_TYPE == RowleyARM) // TODO compiler not verified yet
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#if (COMPILER_TYPE == HEW) // TODO compiler not verified yet
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#if (COMPILER_TYPE == Keil4) // TODO compiler not verified yet
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#if (COMPILER_TYPE == GCC) // TODO compiler not verified yet
#ifdef __clang__ // include in case CLANG needs special treatment
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif
     #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW_FreeRTOS.h>
#endif

#endif
#endif

#endif /* FREERTOS_CONFIG_H */
