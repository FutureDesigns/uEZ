/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.

	A special exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the Philips ARM7 port.
 *----------------------------------------------------------*/

/*
	Changes from V3.2.2

	+ Bug fix - The prescale value for the timer setup is now written to T0PR
	  instead of T0PC.  This bug would have had no effect unless a prescale
	  value was actually used.
*/

/* Standard includes. */
#include <stdlib.h>
#include <intrinsics.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>

/* Constants required to setup the tick ISR. */
#define portENABLE_TIMER			( ( unsigned portCHAR ) 0x01 )
#define portPRESCALE_VALUE			0x00
#define portINTERRUPT_ON_MATCH		( ( unsigned portLONG ) 0x01 )
#define portRESET_COUNT_ON_MATCH	( ( unsigned portLONG ) 0x02 )


/* Constants required to setup the initial stack. */
#define portINITIAL_SPSR				( ( portSTACK_TYPE ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT				( ( portSTACK_TYPE ) 0x20 )
#define portINSTRUCTION_SIZE			( ( portSTACK_TYPE ) 4 )
#define portNO_CRITICAL_SECTION_NESTING	( ( portSTACK_TYPE ) 0 )

/* Constants required to setup the PIT. */
#define portPIT_CLOCK_DIVISOR			( ( unsigned portLONG ) 16 )
#define portPIT_COUNTER_VALUE			( ( ( configCPU_CLOCK_HZ / portPIT_CLOCK_DIVISOR ) / 1000UL ) * portTICK_RATE_MS )

/* Constants required to handle interrupts. */
#define portTIMER_MATCH_ISR_BIT		( ( unsigned portCHAR ) 0x01 )
#define portCLEAR_VIC_INTERRUPT		( ( unsigned portLONG ) 0 )

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING 		( ( unsigned portLONG ) 0 )


#define portINT_LEVEL_SENSITIVE  0
#define portPIT_ENABLE      	( ( unsigned portSHORT ) 0x1 << 24 )
#define portPIT_INT_ENABLE     	( ( unsigned portSHORT ) 0x1 << 25 )

/* Constants required to setup the VIC for the tick ISR. */
#define portTIMER_VIC_CHANNEL		( ( unsigned portLONG ) 0x0004 )
#define portTIMER_VIC_CHANNEL_BIT	( ( unsigned portLONG ) 0x0010 )
#define portTIMER_VIC_ENABLE		( ( unsigned portLONG ) 0x0020 )

/*-----------------------------------------------------------*/

/* Setup the PIT to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );

/* ulCriticalNesting will get set to zero when the first task starts.  It
cannot be initialised to 0 as this will cause interrupts to be enabled
during the kernel initialisation process. */
unsigned portLONG ulCriticalNesting = ( unsigned portLONG ) 9999;

/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
portSTACK_TYPE *pxOriginalTOS;

	pxOriginalTOS = pxTopOfStack;

	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( portSTACK_TYPE ) pxCode + portINSTRUCTION_SIZE;		
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0xaaaaaaaa;	/* R14 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x01010101;	/* R1 */
	pxTopOfStack--;	

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The status register is set for system mode, with interrupts enabled. */
	*pxTopOfStack = ( portSTACK_TYPE ) portINITIAL_SPSR;

	if( ( ( unsigned long ) pxCode & 0x01UL ) != 0x00 )
	{
		/* We want the task to start in thumb mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}

	pxTopOfStack--;
        
	/* Interrupt flags cannot always be stored on the stack and will
	instead be stored in a variable, which is then saved as part of the
	tasks context. */
	*pxTopOfStack = portNO_CRITICAL_NESTING;

	return pxTopOfStack;	
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
extern void vPortStartFirstTask( void );

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	vPortStartFirstTask();	

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 0

	/* The cooperative scheduler requires a normal IRQ service routine to
	simply increment the system tick. */
	static __arm __irq void vPortNonPreemptiveTick( void );
	static __arm __irq void vPortNonPreemptiveTick( void )
	{
		/* Increment the tick count - which may wake some tasks but as the
		preemptive scheduler is not being used any woken task is not given
		processor time no matter what its priority. */
		vTaskIncrementTick();
		
		/* Ready for the next interrupt. */
		T0IR = portTIMER_MATCH_ISR_BIT;
		VICVectAddr = portCLEAR_VIC_INTERRUPT;
	}

#else

	/* This function is called from an asm wrapper, so does not require the __irq
	keyword. */
	void vPortPreemptiveTick( void );
	void vPortPreemptiveTick( void )
	{
		/* Increment the tick counter. */
		vTaskIncrementTick();
	
		/* The new tick value might unblock a task.  Ensure the highest task that
		is ready to execute is the task that will execute when the tick ISR
		exits. */
		vTaskSwitchContext();
	
		/* Ready for the next interrupt. */
		T0IR = portTIMER_MATCH_ISR_BIT;
		VICVectAddr = portCLEAR_VIC_INTERRUPT;
	}

#endif

/*-----------------------------------------------------------*/
IRQ_ROUTINE(vPreemptiveTick)
{
    IRQ_START();
	/* Increment the RTOS tick count, then look for the highest priority 
	task that is ready to run. */
	vTaskIncrementTick();
	vTaskSwitchContext();

	/* Ready for the next interrupt. */
#if (configTIMER_INDEX==0)
	T0IR = 2;
#endif
#if (configTIMER_INDEX==1)
	T1IR = 2;
#endif
    IRQ_END();
}

#if (configTIMER_INDEX==0)
/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
unsigned portLONG ulCompareMatch;

	PCLKSEL0 = (PCLKSEL0 & (~(0x3<<2))) | (0x01 << 2);
	T0TCR  = 2;         /* Stop and reset the timer */
	T0CTCR = 0;         /* Timer mode               */
	
	/* A 1ms tick does not require the use of the timer prescale.  This is
	defaulted to zero but can be used if necessary. */
	T0PR = portPRESCALE_VALUE;

	/* Calculate the match value required for our wanted tick rate. */
	ulCompareMatch = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	/* Protect against divide by zero.  Using an if() statement still results
	in a warning - hence the #if. */
	#if portPRESCALE_VALUE != 0
	{
		ulCompareMatch /= ( portPRESCALE_VALUE + 1 );
	}
	#endif
	T0MR1 = ulCompareMatch;

	/* Generate tick with timer 0 compare match. */
	T0MCR  = (3 << 3);  /* Reset timer on match and generate interrupt */

	/* The ISR installed depends on whether the preemptive or cooperative
	scheduler is being used. */
	#if configUSE_PREEMPTION == 1
	{
		extern void ( vPreemptiveTick )( void );
//		VICVectAddr4 = ( portLONG ) vPreemptiveTick;
        InterruptRegister(
            INTERRUPT_CHANNEL_TIMER0, 
            vPreemptiveTick, 
            INTERRUPT_PRIORITY_NORMAL,
            "TICK");
	} 
	#else
	{
		extern void ( vNonPreemptiveTick )( void );
//		VICVectAddr4 = ( portLONG ) vNonPreemptiveTick;
        InterruptRegister(
            INTERRUPT_CHANNEL_TIMER0, 
            vNonPreemptiveTick, 
            INTERRUPT_PRIORITY_NORMAL,
            "TICK");
	}
	#endif

	/* Setup the VIC for the timer. */
	VICVectCntl4 = 1;
	VICIntEnable = 0x00000010;	

	/* Start the timer - interrupts are disabled when this function is called
	so it is okay to do this here. */
	T0TCR = portENABLE_TIMER;
}
#endif

#if (configTIMER_INDEX==1)
/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
unsigned portLONG ulCompareMatch;

	PCLKSEL0 = (PCLKSEL0 & (~(0x3<<4))) | (0x01 << 4);
	T1TCR  = 2;         /* Stop and reset the timer */
	T1CTCR = 0;         /* Timer mode               */
	
	/* A 1ms tick does not require the use of the timer prescale.  This is
	defaulted to zero but can be used if necessary. */
	T1PR = portPRESCALE_VALUE;

	/* Calculate the match value required for our wanted tick rate. */
	ulCompareMatch = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	/* Protect against divide by zero.  Using an if() statement still results
	in a warning - hence the #if. */
	#if portPRESCALE_VALUE != 0
	{
		ulCompareMatch /= ( portPRESCALE_VALUE + 1 );
	}
	#endif
	T1MR1 = ulCompareMatch;

	/* Generate tick with timer 0 compare match. */
	T1MCR  = (3 << 3);  /* Reset timer on match and generate interrupt */

	/* The ISR installed depends on whether the preemptive or cooperative
	scheduler is being used. */
	#if configUSE_PREEMPTION == 1
	{
		extern void ( vPreemptiveTick )( void );
//		VICVectAddr4 = ( portLONG ) vPreemptiveTick;
        InterruptRegister(
            INTERRUPT_CHANNEL_TIMER1, 
            vPreemptiveTick, 
            INTERRUPT_PRIORITY_NORMAL,
            "TICK");
	} 
	#else
	{
		extern void ( vNonPreemptiveTick )( void );
//		VICVectAddr4 = ( portLONG ) vNonPreemptiveTick;
        InterruptRegister(
            INTERRUPT_CHANNEL_TIMER1, 
            vNonPreemptiveTick, 
            INTERRUPT_PRIORITY_NORMAL,
            "TICK");
	}
	#endif

	/* Setup the VIC for the timer. */
	VICVectCntl5 = 1;
	VICIntEnable = 0x00000020;

	/* Start the timer - interrupts are disabled when this function is called
	so it is okay to do this here. */
	T1TCR = portENABLE_TIMER;
}
#endif
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	/* Disable interrupts first! */
	__disable_interrupt();

	/* Now interrupts are disabled ulCriticalNesting can be accessed
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as we are leaving a critical section. */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			__enable_interrupt();
		}
	}
}
/*-----------------------------------------------------------*/






#ifdef THUMB_INTERWORK

	__task __arm void vPortDisableInterruptsFromThumb( void ) /* __attribute__ ((naked))*/;
	__task __arm void vPortEnableInterruptsFromThumb( void ) /*__attribute__ ((naked))*/;

	__task __arm void vPortDisableInterruptsFromThumb( void )
	{
		__asm volatile ( 
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/
			"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.						*/
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/
			"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
			"BX		R14" );					/* Return back to thumb.					*/
	}
			
	__task __arm void vPortEnableInterruptsFromThumb( void )
	{
		__asm volatile ( 
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/	
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/	
			"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.							*/	
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/	
			"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
			"BX		R14" );					/* Return back to thumb.					*/
	}

#endif /* THUMB_INTERWORK */

	void Test(int v)
	{
	    volatile int k;
	    k = v;
	}

void AsmVector(void)
{
    Test(5);
}
