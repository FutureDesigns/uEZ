/*-------------------------------------------------------------------------*
 * File:  uEZRTOS.h
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_RTOS_H_
#define _UEZ_RTOS_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   uEZRTOS.h
 *  @brief  uEZ RTOS
 *
 *  uEZ Operation System Abstraction Layer
 */
#include "Config.h"
#include "uEZTypes.h"
#include "uEZErrors.h"
#include "uEZTickCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * FreeRTOS specific definitions.
 *-------------------------------------------------------------------------*/
#if (RTOS==FreeRTOS)
#include "FreeRTOS.h"

typedef enum {
    UEZ_PRIORITY_IDLE=0,
    UEZ_PRIORITY_VERY_LOW=1,
    UEZ_PRIORITY_LOW=2,
    UEZ_PRIORITY_NORMAL=3,
    UEZ_PRIORITY_HIGH=4,
    UEZ_PRIORITY_VERY_HIGH=5,
    UEZ_PRIORITY_CRITICAL=6,
} T_uezPriority;

#define RTOS_SAVE_CONTEXT() 	portSAVE_CONTEXT()
#define RTOS_RESTORE_CONTEXT()  portRESTORE_CONTEXT()
#define RTOS_ENTER_CRITICAL()   portENTER_CRITICAL();
#define RTOS_EXIT_CRITICAL()    portEXIT_CRITICAL();

/*-------------------------------------------------------------------------*
 * SafeRTOS specific definitions.
 *-------------------------------------------------------------------------*/
#elif (RTOS==SafeRTOS)

#include "SafeRTOS_API.h"

#define	UEZ_PRIORITY_PRIV		( 0x80 )
#define UEZ_TASK_PRIORITY_MASK	( 0x7F )

typedef enum {
    UEZ_APP_PRIORITY_IDLE 		= 0,
    UEZ_APP_PRIORITY_VERY_LOW 	= 1,
    UEZ_APP_PRIORITY_LOW 		= 2,
    UEZ_APP_PRIORITY_NORMAL 	= 3,
    UEZ_APP_PRIORITY_HIGH		= 4,
    UEZ_APP_PRIORITY_VERY_HIGH	= 5,
    UEZ_APP_PRIORITY_CRITICAL	= 6,    
	UEZ_PRIORITY_IDLE			= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_IDLE,
    UEZ_PRIORITY_VERY_LOW		= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_VERY_LOW,
    UEZ_PRIORITY_LOW			= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_LOW,
    UEZ_PRIORITY_NORMAL			= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_NORMAL,
    UEZ_PRIORITY_HIGH			= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_HIGH,
    UEZ_PRIORITY_VERY_HIGH		= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_VERY_HIGH,
    UEZ_PRIORITY_CRITICAL		= UEZ_PRIORITY_PRIV | UEZ_APP_PRIORITY_CRITICAL,
} T_uezPriority;

#define RTOS_ENTER_CRITICAL()   portENTER_CRITICAL();
#define RTOS_EXIT_CRITICAL()    portEXIT_CRITICAL();

#endif

/*-------------------------------------------------------------------------*
 * Section: Tasks
 *-------------------------------------------------------------------------*/
typedef T_uezHandle T_uezTask;

typedef TUInt32 (*T_uezTaskFunction)(T_uezTask aMyTask, void *aParameters);

T_uezError UEZTaskCreate(
            T_uezTaskFunction aFunction,
            const char * const aName,
            TUInt32 aStackSize,
            void *aParameters,
            T_uezPriority aPriority,
            T_uezTask *aCreatedTask);

T_uezError UEZTaskDelete(T_uezTask aTask);

T_uezError UEZTaskDelay(TUInt32 aTicks);

T_uezTask UEZTaskGetCurrent(void);

T_uezError UEZTaskSchedulerSuspend(void);

T_uezError UEZTaskSchedulerResume(void);

T_uezError UEZTaskSuspend(T_uezTask aTask);
T_uezError UEZTaskResume(T_uezTask aTask);
void _isr_UEZTaskContextSwitch(void);

#if (RTOS == FreeRTOS)
#define UEZ_TASK_STACK_BYTES(x)             (x/sizeof(long))
#elif (RTOS == SafeRTOS)
#define UEZ_TASK_STACK_BYTES(x)             (x)
#else
#define UEZ_TASK_STACK_BYTES(x)             (x)
#endif

/*-------------------------------------------------------------------------*
 * Section: Semaphores
 *-------------------------------------------------------------------------*/
typedef T_uezHandle T_uezSemaphore;

T_uezError UEZSemaphoreCreateBinary(T_uezSemaphore *aSemaphore);
T_uezError UEZSemaphoreCreateCounting(
            T_uezSemaphore *aSemaphore,
            TUInt32 aMaxCount,
            TUInt32 aInitialCount);
T_uezError UEZSemaphoreCreateMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority);

T_uezError UEZSemaphoreDelete(T_uezSemaphore aSemaphore);

T_uezError UEZSemaphoreGrab(T_uezSemaphore aSemaphore, TUInt32 aTimeout);
T_uezError _isr_UEZSemaphoreGrab(T_uezSemaphore aSemaphore);
T_uezError UEZSemaphoreRelease(T_uezSemaphore aSemaphore);
T_uezError _isr_UEZSemaphoreRelease(T_uezSemaphore aSemaphore);

/** Note: SafeRTOS does not support a priority inheritance mechanism, therefore 
mutexes are not available. Hence UEZ API functions UEZSemaphoreCreateMutex,
UEZSemaphoreCreateRecursiveMutex, UEZSemaphoreRecursiveGrab and
UEZSemaphoreRecursiveRelease are not supported. */
T_uezError UEZSemaphoreCreateRecursiveMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority);
T_uezError UEZSemaphoreRecursiveGrab(T_uezSemaphore aSemaphore, TUInt32 aTimeout);
T_uezError UEZSemaphoreRecursiveRelease(T_uezSemaphore aSemaphore);

/*-------------------------------------------------------------------------*
 * Section: Queues
 *-------------------------------------------------------------------------*/
typedef T_uezHandle T_uezQueue;

T_uezError UEZQueueCreate(
            TUInt32 aNumItems,
            TUInt32 aItemSize,
            T_uezQueue *aQueue);
T_uezError UEZQueueDelete(T_uezQueue aQueue);
T_uezError UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout);
T_uezError _isr_UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem);
T_uezError UEZQueueSendFront(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout);
T_uezError _isr_UEZQueueSendFront(
            T_uezQueue aQueue,
            void *aItem);
T_uezError UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout);
T_uezError _isr_UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem);
T_uezError UEZQueuePeek(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout);
T_uezError UEZQueueGetCount(T_uezQueue aQueue, TUInt32 *aCount);

/**
 * The queue registry is just a means for kernel aware debuggers to locate
 * queue structures.  It has no other purpose so is an optional component.
 */
#if (configQUEUE_REGISTRY_OPTION == configQUEUE_REGISTRY_FULL)

T_uezError UEZQueueAddToRegistry( T_uezQueue aQueue, signed char *pcQueueName );
T_uezError UEZQueueUnregisterQueue( T_uezQueue aQueue );

#else

#define UEZQueueAddToRegistry( xQueue, pcQueueName )
#define UEZQueueUnregisterQueue( xQueue )

#endif

void UEZGetTaskList(char* aBuffer);
void UEZTaskRegister(const char * const aName, T_uezPriority aPriority, TUInt32 aStackSize);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_RTOS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZRTOS.h
 *-------------------------------------------------------------------------*/
