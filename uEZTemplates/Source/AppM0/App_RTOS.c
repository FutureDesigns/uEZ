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

/******************************************************************************
 * Do any custom pin or hardware setup here before starting tasks.
 * Then start tasks.
 *
 * This file implements the code that is not app specific, including the
 * hardware setup and FreeRTOS hook functions.
 */

/* Kernel includes. */
//#include <configuration.h>
#include "FreeRTOS.h"
#include "task.h"
#include <uEZBSP.h>
#include <uEZProcessor.h>
//#include "task_handles.h"

/* Platform includes. */

/*-----------------------------------------------------------*/
/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/* Task Handles*/
//TaskHandle_t ;
//TaskHandle_t ;
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
volatile TaskHandle_t G_task_handle_failed_task;
#endif

/*-----------------------------------------------------------*/
// Note that at this point the RTOS hasn't started running yet, so task delay calls will not work.
void Processing_Before_Start_Kernel()
{
  //clear_reset_reasons(); // if you had code to check reset reason (from bootloader, low power, etc) 
  // you could check and clear it here before starting the RTOS
  
// Optionally you can start RTT and Systemview before starting the first task, and can then record the RTOS startup.
/*
  // Note that RTT and Systemview should be able to be enabled at the begining of this function if desired.
  // You can enable RTT and SystemView before the RTOS, so that you can see any issues during bootup.
#if (SEGGER_ENABLE_RTT == 1 )  // enable RTT
  #if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //Systemview will auto init RTT
    (void)_SEGGER_RTT; // GCC complains if we don't use this.
    SEGGER_RTT_Init();
    //SEGGER_RTT_Write(0, "\n\n RTT\n", 9);
    //SEGGER_RTT_WriteString(0, "Hello World RTT 0!\n"); // Test RTT Interface
	
	//DEBUG_RTT_SetTerminal(0); // test second terminal
	//DEBUG_RTT_TerminalOut(1,"RTT\n"); // Second terminal only test
	
  #endif
#endif
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // Start SystemView if enabled
  #if (SEGGER_ENABLE_RTT == 0)
    #error "RTT Must be enabled to use SystemView!"
  #endif
  SEGGER_SYSVIEW_Conf(); // This runs SEGGER_SYSVIEW_Init and SEGGER_RTT_Init
  SEGGER_SYSVIEW_Start(); // Start recording events
  //SEGGER_RTT_WriteString(0, "Hello World RTT 0!\n"); // Test RTT Interface
  // These "DEBUG_SV_" defines only compile in when systemview is enabled.
  // So you can leave them in the application for release builds.
  // Warnings and errors show different graphical icons and colors for debug.
  // Must add #include <SEGGER/SystemView/SEGGER_SYSVIEW.h> even when turned off
  DEBUG_SV_Print("SystemView Started"); // SystemView terminal
  DEBUG_SV_PrintW("Warn Test"); // example warning
  DEBUG_SV_PrintE("Error Test"); // example error
#endif
*/

// start first task here

}

/*-----------------------------------------------------------*/
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	G_task_handle_failed_task = pxTask; 
	//softReset();
        
        // from UEZHalt();
        RTOS_ENTER_CRITICAL();
        while (1) {
            UEZBSP_FatalError(9);
        }

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is
	called if a stack overflow is detected. */
	//taskDISABLE_INTERRUPTS();
	//for( ;; );
}
#endif

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c are used, then the size
	of the heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE
	in FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

#if (configUSE_IDLE_HOOK == 1)
void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
#endif
/*-----------------------------------------------------------*/

#if (configUSE_TICK_HOOK == 1)
void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}
#endif
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
volatile unsigned long ul = 0;

	taskENTER_CRITICAL();
	{
		/* Set ul to a non-zero value using the debugger to step out of this
		function. */
		while( ul == 0 )
		{
			NOP();
		}
	}
	taskEXIT_CRITICAL();
}

void vPreSleepProcessing( unsigned long ulExpectedIdleTime )
{
	/* Called by the kernel before it places the MCU into a sleep mode because
	configPRE_SLEEP_PROCESSING() is #defined to vPreSleepProcessing(). */

	/* Avoid compiler warnings about the unused parameter. */
	( void ) ulExpectedIdleTime;

	/* Is the MCU about to enter deep sleep mode or software standby mode? */
	/*if(  == 0 ) {
	} else {
	}*/
	// TODO add code here to put into low power mode
}
/*-----------------------------------------------------------*/

void vPostSleepProcessing( unsigned long ulExpectedIdleTime )
{
	/* Called by the kernel when the MCU exits a sleep mode because
	configPOST_SLEEP_PROCESSING is #defined to vPostSleepProcessing(). */

	/* Avoid compiler warnings about the unused parameter. */
	( void ) ulExpectedIdleTime;
	// TODO add code to wake up from a low power mode
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
