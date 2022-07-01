/*-------------------------------------------------------------------------*
 * File:  uEZRTOS.c
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ RTOS Operation System Abstraction Layer
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "Config.h"
#include "FreeRTOS.h"
#include "uEZ.h"
#include "uEZRTOS.h"

typedef int xSemaphoreHandle;

static xSemaphoreHandle G_startingTaskReady;
static xSemaphoreHandle G_semTask;
static TBool G_isRTOSRunning = EFalse;

#ifndef UEZ_DEBUG_HEAVY_ASSERTS
#define UEZ_DEBUG_HEAVY_ASSERTS     1
#endif

#if UEZ_DEBUG_HEAVY_ASSERTS
    #define UEZAssert(x)        if (!(x)) UEZHalt()

static void UEZHalt(void)
{
    while (1)
        {
        }    
}

#else
    // Not heavy asserts.  Remove them
    #define UEZAssert(x)
#endif

void UEZTaskInit(void)
{


}

T_uezError UEZSystemMainLoop(void)
{
    return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * Section: Tasks
 *-------------------------------------------------------------------------*/
static void ITaskStartup(void *aParams)
{

}

T_uezError UEZTaskCreate(
            T_uezTaskFunction aFunction,
            const char * const aName,
            TUInt32 aStackSize,
            void *aParameters,
            T_uezPriority aPriority,
            T_uezTask *aCreatedTask)
{
    return UEZ_ERROR_NONE;
}

T_uezTask UEZTaskGetCurrent(void)
{
    return 0;
}

T_uezError UEZTaskSchedulerSuspend(void)
{
    return UEZ_ERROR_NONE;
}


T_uezError UEZTaskSchedulerResume(void)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskDelete(T_uezTask aTask)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskDelay(TUInt32 aTicks)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskSuspend(T_uezTask aTask)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskResume(T_uezTask aTask)
{
    return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * Section: Semaphores
 *-------------------------------------------------------------------------*/
//typedef T_uezHandle T_uezSemaphore;

T_uezError UEZSemaphoreCreateBinary(T_uezSemaphore *aSemaphore)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreCreateCounting(
            T_uezSemaphore *aSemaphore,
            TUInt32 aMaxCount,
            TUInt32 aInitialCount)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreCreateMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreDelete(T_uezSemaphore aSemaphore)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreGrab(
            T_uezSemaphore aSemaphore,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError _isr_UEZSemaphoreGrab(T_uezSemaphore aSemaphore)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreRelease(
            T_uezSemaphore aSemaphore)
{
    return UEZ_ERROR_NONE;
}

T_uezError _isr_UEZSemaphoreRelease(
            T_uezSemaphore aSemaphore)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreCreateRecursiveMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreRecursiveGrab(
            T_uezSemaphore aSemaphore,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreRecursiveRelease(
            T_uezSemaphore aSemaphore)
{
    return UEZ_ERROR_NONE;
}


/*-------------------------------------------------------------------------*
 * Section: Queues
 *-------------------------------------------------------------------------*/
T_uezError UEZQueueCreate(
            TUInt32 aNumItems,
            TUInt32 aItemSize,
            T_uezQueue *aQueue)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueDelete(T_uezQueue aQueue)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError _isr_UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueSendFront(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError _isr_UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueuePeek(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueGetCount(T_uezQueue aQueue, TUInt32 *aCount)
{
    return UEZ_ERROR_NONE;
}


TUInt32 UEZTickCounterGet(void)
{
    return 1;
}

TUInt32 UEZTickCounterGetDelta(TUInt32 aStart)
{
    return 1;
}


// Immediately context switch to another task inside the interrupt routine
void _isr_UEZTaskContextSwitch(void)
{
    
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZRTOS.c
 *-------------------------------------------------------------------------*/
