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
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "Config.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uEZ.h"
#include "Source/uEZSystem/uEZHandles.h"
#include "uEZRTOS.h"
#include <uEZBSP.h>

#if FREERTOS_PLUS_TRACE //LPC1788/LPC4088 only as of uEZ v2.06
#include "trcUser.h"
#endif

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
    RTOS_ENTER_CRITICAL();
    while (1)
        {
            UEZBSP_FatalError(9);
        }    
}

#else
    // Not heavy asserts.  Remove them
    #define UEZAssert(x)
#endif

void UEZTaskInit(void)
{
    // Create a queue of starting tasks for parameter passing
    G_startingTaskReady = xSemaphoreCreateMutex();

    // Only one task can be created at a time (for thread safety)
    G_semTask = xSemaphoreCreateMutex();

}

T_uezError UEZSystemMainLoop(void)
{
    // Note that the RTOS is now running
    G_isRTOSRunning = ETrue;

    // Run the scheduler
    vTaskStartScheduler();

    // Never gets here
    return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * Section: Tasks
 *-------------------------------------------------------------------------*/
static void ITaskStartup(void *aParams)
{
    T_uezHandle taskHandle = (T_uezHandle)aParams;
    TUInt32 paramAddr;
    TUInt32 startAddr;

    // Wait for the creation to complete after xTaskCreate called.
    xSemaphoreTake(G_startingTaskReady, portMAX_DELAY);

    // Handle is ready.  Read the data.
    uEZHandleGet(taskHandle, 0, &startAddr, &paramAddr, 0);

    // Release the starting semaphore and run the program
    // with the right parameters.
    xSemaphoreGive(G_startingTaskReady);

    ((T_uezTaskFunction)startAddr)(taskHandle, (void *)paramAddr);

    // If we come back here, we need to delete the handle
//    uEZHandleFree(taskHandle);

    // Delete this task (we're in it)
    UEZTaskDelete(taskHandle);
    while (1); // wait for this to die
}

T_uezError UEZTaskCreate(
            T_uezTaskFunction aFunction,
            const char * const aName,
            TUInt32 aStackSize,
            void *aParameters,
            T_uezPriority aPriority,
            T_uezTask *aCreatedTask)
{
    xTaskHandle taskCreated;
    signed portBASE_TYPE error;
    T_uezError errorCode = UEZ_ERROR_NONE;
    T_uezHandle taskHandle = 0;

    // Grab the task semaphore (wait forever if necessary)
    xSemaphoreTake(G_semTask, portMAX_DELAY);

    // Grab the semaphore of the starting task.
    // We want to catch the task before it fully starts.
    xSemaphoreTake(G_startingTaskReady, portMAX_DELAY);

    // Create a task handle
    errorCode = uEZHandleAlloc(&taskHandle);

    // Only continue if we got a handle
    if (errorCode == UEZ_ERROR_NONE) {
        // Start a task creation, but make it wait
        error = xTaskCreate(
                    ITaskStartup,
                    (const char *)aName,
                    (TUInt16)(aStackSize+20), // TBD: is this enough for ITaskStartup?
                    (void *)(taskHandle),
                    tskIDLE_PRIORITY+aPriority,
                    &taskCreated);

        if (error == pdPASS)  {
            // Got the task to create
            // Now fill out the handle table entry and make the handle a true task handle.
            uEZHandleSet(
                taskHandle,
                UEZ_HANDLE_TASK,
                (TUInt32)aFunction,
                (TUInt32)aParameters,
                (TUInt32)taskCreated);
            UEZTaskRegister(aName, aPriority, aStackSize);
        } else {
            // Could not create the task
            // TBD: More mapping of errors?  Assuming too many tasks in system.
            errorCode = UEZ_ERROR_TOO_MANY_TASKS;

            uEZHandleFree(taskHandle);
            taskHandle = 0;
        }
    }

    // Done starting up the task.  Move on.
    xSemaphoreGive(G_startingTaskReady);

    // Done creating a task, allow the next one to be created
    xSemaphoreGive(G_semTask);

    if (aCreatedTask)
        *aCreatedTask = taskHandle;

    return errorCode;
}

T_uezTask UEZTaskGetCurrent(void)
{
    T_uezTask h;
    TUInt32 index = 0;
    T_uezError found;
    xTaskHandle currentTask;
    xTaskHandle task;

    // TBD: This is slow!

    // Get the FreeRTOS handle
    currentTask = xTaskGetCurrentTaskHandle();
    do {
        found = uEZHandleFindOfType(UEZ_HANDLE_TASK, &h, &index);

        // Didn't find task?  Return 0
        if (found != UEZ_ERROR_NONE)
            return 0;

        uEZHandleGet(h, 0, 0, 0, (TUInt32 *)&task);
        if (task == currentTask) {
            UEZAssert(uEZHandleGetType(h) == UEZ_HANDLE_TASK);
            return h;
        }
    } while (!found);

    return 0;
}

T_uezError UEZTaskSchedulerSuspend(void)
{
    vTaskSuspendAll();
    return UEZ_ERROR_NONE;
}


T_uezError UEZTaskSchedulerResume(void)
{
    xTaskResumeAll();
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskDelete(T_uezTask aTask)
{
    TUInt32 type;
    TUInt32 taskHandle;
    T_uezError error;

    UEZAssert(aTask != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aTask) == UEZ_HANDLE_TASK);

    // Make sure we don't step on any other actions
    xSemaphoreTake(G_semTask, portMAX_DELAY);

    // Get the original handle's data
    error = uEZHandleGet(aTask, &type, 0, 0, &taskHandle);
    if (error == UEZ_ERROR_NONE) {
        // Got the data, now confirm we are deleting a task
        if ((type & UEZ_HANDLE_TYPE_MASK) == UEZ_HANDLE_TASK)  {
            // Free the handle
            error = uEZHandleFree(aTask);

            // Done deleting task
            xSemaphoreGive(G_semTask);

            // Delete the task (if we are in it, we end here)
            if (error == UEZ_ERROR_NONE)
                vTaskDelete((xTaskHandle)taskHandle);

            return error;
        } else {
            UEZAssert(0);
            error = UEZ_ERROR_HANDLE_INVALID;
        }
    }

    // Done deleting task
    xSemaphoreGive(G_semTask);

    return error;
}

T_uezError UEZTaskPriorityGet(T_uezTask aTask, T_uezPriority *aPriority)
{
    TUInt32 type;
    TUInt32 taskHandle;
    T_uezError error = UEZ_ERROR_NONE;

    if (aTask == UEZ_NULL_HANDLE) {
        // Passing in a null handle will suspend the current task
        *aPriority = (T_uezPriority)(uxTaskPriorityGet((xTaskHandle)0) 
            - tskIDLE_PRIORITY);
    } else {
        // Otherwise, we need to check the task handle
        UEZAssert(uEZHandleGetType(aTask) == UEZ_HANDLE_TASK);

        // Get the original handle's data
        error = uEZHandleGet(aTask, &type, 0, 0, &taskHandle);
        if (error == UEZ_ERROR_NONE)
        {
            // Got the data, now confirm suspending a task
            if ((type & UEZ_HANDLE_TYPE_MASK) == UEZ_HANDLE_TASK)
            {
                // Get the priority
                *aPriority = (T_uezPriority)(uxTaskPriorityGet((xTaskHandle)taskHandle) 
                    - tskIDLE_PRIORITY);
                return error;
            }
            else
            {
                UEZAssert(0);
                error = UEZ_ERROR_HANDLE_INVALID;
            }
        }
    }
    return error;
}

T_uezError UEZTaskPrioritySet(T_uezTask aTask, T_uezPriority aPriority)
{
    TUInt32 type;
    TUInt32 taskHandle;
    T_uezError error = UEZ_ERROR_NONE;

    if (aTask == UEZ_NULL_HANDLE) {
        // Passing in a null handle will suspend the current task
        vTaskPrioritySet((xTaskHandle)0, tskIDLE_PRIORITY+aPriority);
    } else {
        // Otherwise, we need to check the task handle
        UEZAssert(uEZHandleGetType(aTask) == UEZ_HANDLE_TASK);

        // Get the original handle's data
        error = uEZHandleGet(aTask, &type, 0, 0, &taskHandle);
        if (error == UEZ_ERROR_NONE)
        {
            // Got the data, now confirm suspending a task
            if ((type & UEZ_HANDLE_TYPE_MASK) == UEZ_HANDLE_TASK)
            {
                // Set the priority
                vTaskPrioritySet((xTaskHandle)taskHandle, tskIDLE_PRIORITY+aPriority);
                return error;
            }
            else
            {
                UEZAssert(0);
                error = UEZ_ERROR_HANDLE_INVALID;
            }
        }
    }
    return error;
}

T_uezError UEZTaskDelay(TUInt32 aTicks)
{
    if (G_isRTOSRunning)
        vTaskDelay(aTicks);
    else
        UEZBSPDelayMS(aTicks);
    return UEZ_ERROR_NONE;
}

T_uezError UEZTaskSuspend(T_uezTask aTask)
{
    TUInt32 type;
    TUInt32 taskHandle;
    T_uezError error = UEZ_ERROR_NONE;

    if (aTask == UEZ_NULL_HANDLE) {
        // Passing in a null handle will suspend the current task
        vTaskSuspend((xTaskHandle)0);
    } else {
        // Otherwise, we need to check the task handle
        UEZAssert(uEZHandleGetType(aTask) == UEZ_HANDLE_TASK);

        // Get the original handle's data
        error = uEZHandleGet(aTask, &type, 0, 0, &taskHandle);
        if (error == UEZ_ERROR_NONE)
        {
            // Got the data, now confirm suspending a task
            if ((type & UEZ_HANDLE_TYPE_MASK) == UEZ_HANDLE_TASK)
            {
                // Suspend the task
                vTaskSuspend((xTaskHandle)taskHandle);
                return error;
            }
            else
            {
                UEZAssert(0);
                error = UEZ_ERROR_HANDLE_INVALID;
            }
        }
    }
    return error;
}

T_uezError UEZTaskResume(T_uezTask aTask)
{
    TUInt32 type;
    TUInt32 taskHandle;
    T_uezError error;

    UEZAssert(aTask != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aTask) == UEZ_HANDLE_TASK);

    // Get the original handle's data
    error = uEZHandleGet(aTask, &type, 0, 0, &taskHandle);
    if (error == UEZ_ERROR_NONE)
    {
        // Got the data, now confirm resuming a task
        if ((type & UEZ_HANDLE_TYPE_MASK) == UEZ_HANDLE_TASK)
        {
            // Resume the task
            vTaskResume((xTaskHandle)taskHandle);
            return error;
        }
        else
        {
            UEZAssert(0);
            error = UEZ_ERROR_HANDLE_INVALID;
        }
    }
    return error;
}

/*-------------------------------------------------------------------------*
 * Section: Semaphores
 *-------------------------------------------------------------------------*/
//typedef T_uezHandle T_uezSemaphore;

T_uezError UEZSemaphoreCreateBinary(T_uezSemaphore *aSemaphore)
{
    T_uezError error;
    T_uezHandle handle=0;
    xSemaphoreHandle sem;

    UEZAssert(aSemaphore != NULL);
    *aSemaphore = UEZ_NULL_HANDLE;

    // Allocate a handle
    error = uEZHandleAlloc(&handle);
    if (error == UEZ_ERROR_NONE)  {
        // Handle is allocated
        *aSemaphore = (T_uezSemaphore)handle;
        // Try to allocate a semaphore from the base RTOS
        vSemaphoreCreateBinary(sem);
        if (sem == NULL)  {
            // Semaphore did not get created.  Clean up
            // and report error.
            uEZHandleFree(handle);
            // Force back to a null handle
            *aSemaphore = UEZ_NULL_HANDLE;
            return UEZ_ERROR_OUT_OF_MEMORY;
        } else {
            // Set data in the handle list
            uEZHandleSet(
                handle,
                UEZ_HANDLE_SEMAPHORE,
                (TUInt32)sem,
                UEZ_SEMAPHORE_BINARY,
                0);
        }
    }

    return error;
}

T_uezError UEZSemaphoreCreateCounting(
            T_uezSemaphore *aSemaphore,
            TUInt32 aMaxCount,
            TUInt32 aInitialCount)
{
    T_uezError error;
    T_uezHandle handle=0;
    xSemaphoreHandle sem;

    UEZAssert(aSemaphore != NULL);
    *aSemaphore = UEZ_NULL_HANDLE;

    // Allocate a handle
    error = uEZHandleAlloc(&handle);
    if (error == UEZ_ERROR_NONE)  {
        // Handle is allocated
        *aSemaphore = (T_uezSemaphore)handle;
        // Try to allocate a semaphore from the base RTOS
        sem = xSemaphoreCreateCounting(aMaxCount, aInitialCount);
        if (sem == NULL)  {
            // Semaphore did not get created.  Clean up
            // and report error.
            uEZHandleFree(handle);
            aSemaphore = UEZ_NULL_HANDLE;
            return UEZ_ERROR_OUT_OF_MEMORY;
        } else {
            // Set data in the handle list
            uEZHandleSet(
                handle,
                UEZ_HANDLE_SEMAPHORE,
                (TUInt32)sem,
                UEZ_SEMAPHORE_COUNTING,
                aMaxCount);
        }
    }

    return error;
}

T_uezError UEZSemaphoreCreateMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority)
{
    T_uezError error;
    T_uezHandle handle=0;
    xSemaphoreHandle sem;

    UEZAssert(aSemaphore != NULL);
    *aSemaphore = UEZ_NULL_HANDLE;

    // Allocate a handle
    error = uEZHandleAlloc(&handle);
    *aSemaphore = (T_uezSemaphore)handle;
    if (error == UEZ_ERROR_NONE)  {
        // Handle is allocated
        // Try to allocate a semaphore from the base RTOS
        // (NOTE: aPriority is NOT used with FreeRTOS)
        sem = xSemaphoreCreateMutex();
        if (sem == NULL)  {
            // Semaphore did not get created.  Clean up
            // and report error.
            uEZHandleFree(handle);
            return UEZ_ERROR_OUT_OF_MEMORY;
        } else {
            // Set data in the handle list
            uEZHandleSet(
                handle,
                UEZ_HANDLE_SEMAPHORE,
                (TUInt32)sem,
                UEZ_SEMAPHORE_MUTEX,
                0);
        }
    }

    return error;
}

T_uezError UEZSemaphoreDelete(T_uezSemaphore aSemaphore)
{
    xQueueHandle semHandle;
    T_uezError error;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    error = uEZHandleGet(aSemaphore, 0, (TUInt32 *)&semHandle, 0, 0);
    if (error)
        return error;

    vTaskSuspendAll();
    vQueueDelete(semHandle);
    error = uEZHandleFree(aSemaphore);
    xTaskResumeAll();

    return UEZ_ERROR_NONE;
}

T_uezError UEZSemaphoreSetName(T_uezSemaphore aSemaphore, char *pcSemaphoreName, const char* aInterfaceName)
{
    xQueueHandle Semaphore;
    TUInt32 ulHandleType;
    T_uezError error;
#if FREERTOS_PLUS_TRACE
    char name[50];
    char data[50];
    char interface[20];
#endif

    error = uEZHandleGet(aSemaphore, &ulHandleType, (TUInt32 *)&Semaphore, 0, 0);

    if (error == UEZ_ERROR_NONE) {
        if (pcSemaphoreName == '\0') {
            if((ulHandleType & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE){
                error = UEZ_ERROR_HANDLE_INVALID;
            } else {
                vQueueAddToRegistry(Semaphore, (const char*)'\0');
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
                vTraceSetSemaphoreName(Semaphore, '\0');
#endif
            }
        } else {
            if((ulHandleType & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE){
                error = UEZ_ERROR_HANDLE_INVALID;
            } else {
                vQueueAddToRegistry(Semaphore, (char *)pcSemaphoreName);
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
                if(aInterfaceName[0] != '\0'){
                    sscanf(aInterfaceName, "%s %s", data, interface);
                    sprintf(name, "%s_%s", pcSemaphoreName, interface);
                } else {
                    sprintf(name, "%s", pcSemaphoreName);
                }
                vTraceSetSemaphoreName(Semaphore, (const char*)name);
#endif
            }
        }
    }

    return error;
}

T_uezError UEZSemaphoreGrab(
            T_uezSemaphore aSemaphore,
            TUInt32 aTimeout)
{
    xSemaphoreHandle sem;
    TUInt32 type;
    T_uezError error;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    error = uEZHandleGet(aSemaphore, &type, (TUInt32 *)&sem, 0, 0);
    if (error)
        return error;
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    if (xSemaphoreTake(sem, aTimeout)) {
        // Got the semaphore
        return UEZ_ERROR_NONE;
    } else {
        // Did not get the semaphore.
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError _isr_UEZSemaphoreGrab(T_uezSemaphore aSemaphore)
{
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    // TBD: May need to change this, but currently waits no time only
    return UEZSemaphoreGrab(aSemaphore, 0);
}

T_uezError UEZSemaphoreRelease(
            T_uezSemaphore aSemaphore)
{
    xSemaphoreHandle sem;
    TUInt32 type;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    uEZHandleGet(aSemaphore, &type, (TUInt32 *)&sem, 0, 0);
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (xSemaphoreGive(sem)) {
        // semaphore given
        return UEZ_ERROR_NONE;
    } else {
        // Something went wrong.
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }
}

T_uezError _isr_UEZSemaphoreRelease(
            T_uezSemaphore aSemaphore)
{
    xSemaphoreHandle sem;
    TUInt32 type;
    portBASE_TYPE isWoken;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    uEZHandleGetFromISR(aSemaphore, &type, (TUInt32 *)&sem, 0, 0);
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (xSemaphoreGiveFromISR(sem, &isWoken)) {
        // semaphore given
        return UEZ_ERROR_NONE;
    } else {
        // Something went wrong.
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }
}

T_uezError UEZSemaphoreCreateRecursiveMutex(
            T_uezSemaphore *aSemaphore,
            T_uezPriority aPriority)
{
    T_uezError error;
    T_uezHandle handle=0;
    xSemaphoreHandle sem;

    UEZAssert(aSemaphore != 0);

    // Allocate a handle
    error = uEZHandleAlloc(&handle);
    *aSemaphore = (T_uezSemaphore)handle;
    if (error == UEZ_ERROR_NONE)  {
        // Handle is allocated
        // Try to allocate a semaphore from the base RTOS
        // (NOTE: aPriority is NOT used with FreeRTOS)
        sem = xSemaphoreCreateRecursiveMutex();
        if (sem == NULL)  {
            // Semaphore did not get created.  Clean up
            // and report error.
            uEZHandleFree(handle);
            return UEZ_ERROR_OUT_OF_MEMORY;
        } else {
            // Set data in the handle list
            uEZHandleSet(
                handle,
                UEZ_HANDLE_SEMAPHORE,
                (TUInt32)sem,
                UEZ_SEMAPHORE_MUTEX,
                0);
        }
    }

    return error;
}

T_uezError UEZSemaphoreRecursiveGrab(
            T_uezSemaphore aSemaphore,
            TUInt32 aTimeout)
{
    xSemaphoreHandle sem;
    TUInt32 type;
    T_uezError error;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    error = uEZHandleGet(aSemaphore, &type, (TUInt32 *)&sem, 0, 0);
    if (error)
        return error;
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    if (xSemaphoreTakeRecursive(sem, aTimeout)) {
        // Got the semaphore
        return UEZ_ERROR_NONE;
    } else {
        // Did not get the semaphore.
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError UEZSemaphoreRecursiveRelease(
            T_uezSemaphore aSemaphore)
{
    xSemaphoreHandle sem;
    TUInt32 type;

    UEZAssert(aSemaphore != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aSemaphore) == UEZ_HANDLE_SEMAPHORE);

    uEZHandleGet(aSemaphore, &type, (TUInt32 *)&sem, 0, 0);
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (xSemaphoreGiveRecursive(sem)) {
        // semaphore given
        return UEZ_ERROR_NONE;
    } else {
        // Something went wrong.
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }
}


/*-------------------------------------------------------------------------*
 * Section: Queues
 *-------------------------------------------------------------------------*/
T_uezError UEZQueueCreate(
            TUInt32 aNumItems,
            TUInt32 aItemSize,
            T_uezQueue *aQueue)
{
    T_uezError error;
    T_uezHandle handle;
    xQueueHandle qHandle;

    UEZAssert(aQueue != 0);

    // Allocate a handle
    error = uEZHandleAlloc(&handle);
    *aQueue = (T_uezQueue)handle;
    if (error == UEZ_ERROR_NONE)  {
        qHandle = xQueueCreate(aNumItems, aItemSize);
        if (qHandle)  {
            // Queue created
            // Store the queue data
            uEZHandleSet(
                handle,
                UEZ_HANDLE_QUEUE,
                (TUInt32)qHandle,
                0,
                0);
        } else {
            // Queue not created
            // Delete the handle
            uEZHandleFree(handle);

            // and report an error (out of memory)
            error = UEZ_ERROR_OUT_OF_MEMORY;
        }
    }

    return error;
}

T_uezError UEZQueueDelete(T_uezQueue aQueue)
{
    xQueueHandle qHandle;
    T_uezError error;

    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGet(aQueue, 0, (TUInt32 *)&qHandle, 0, 0);
    if (error) {
        return error;
    }

    vQueueDelete(qHandle);
    error = uEZHandleFree(aQueue);

    return UEZ_ERROR_NONE;
}

T_uezError UEZQueueSetName( T_uezQueue aQueue, char *pcQueueName, const char* aInterfaceName)
{
    xQueueHandle Queue;
    TUInt32 ulHandleType;
    T_uezError error;
#if FREERTOS_PLUS_TRACE
    char name[50];
    char data[50];
    char interface[20];
#endif

    error = uEZHandleGet(aQueue, &ulHandleType, (TUInt32 *)&Queue, 0, 0);

    if (error == UEZ_ERROR_NONE) {
        if (pcQueueName == '\0') {
            if((ulHandleType & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE){
                error = UEZ_ERROR_HANDLE_INVALID;
            } else {
                vQueueAddToRegistry(Queue, (const char*)'\0');
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
                vTraceSetQueueName(Queue, '\0');
#endif
            }
        } else {
            if((ulHandleType & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE){
                error = UEZ_ERROR_HANDLE_INVALID;
            } else {
                vQueueAddToRegistry(Queue, (char *)pcQueueName);
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
                if(aInterfaceName[0] != '\0'){
                    sscanf(aInterfaceName, "%s %s", data, interface);
                    sprintf(name, "%s_%s", pcQueueName, interface);
                } else {
                    sprintf(name, "%s", pcQueueName);
                }
                vTraceSetQueueName(Queue, (const char*)name);
#endif
            }
        }
    }

    return error;
}

T_uezError UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;
    portBASE_TYPE result;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGet(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error) {
        return error;
    }
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE) {
        return UEZ_ERROR_HANDLE_INVALID;
    }
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    result = xQueueSend(queue, (const void *)aItem, aTimeout);
    if (result == pdPASS) {
        // The queue item got queued
        return UEZ_ERROR_NONE;
    } else if (result == errQUEUE_FULL) {
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    } else {
        // Some other strange error
        return UEZ_ERROR_INTERNAL_ERROR;
    }
}

T_uezError _isr_UEZQueueSend(
            T_uezQueue aQueue,
            void *aItem)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;
    portBASE_TYPE higherTaskWoken;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGetFromISR(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error)
        return error;
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (xQueueSendFromISR(queue, (const void *)aItem, &higherTaskWoken)) {
//        if (higherTaskWoken)
//            taskYIELD_FROM_ISR();
        // The queue item got queued
        return UEZ_ERROR_NONE;
    } else {
//        if (higherTaskWoken)
//            taskYIELD_FROM_ISR();
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError UEZQueueSendFront(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGet(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error) {
        return error;
    }
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE) {
        return UEZ_ERROR_HANDLE_INVALID;
    }
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    if (xQueueSendToFront(queue, (const void *)aItem, aTimeout)) {
        // The queue item got queued
        return UEZ_ERROR_NONE;
    } else {
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGet(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error) {
        return error;
    }
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE) {
        return UEZ_ERROR_HANDLE_INVALID;
    }
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    if (xQueueReceive(queue, aItem, aTimeout)) {
        // The queue item got queued
        return UEZ_ERROR_NONE;
    } else {
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError _isr_UEZQueueReceive(
            T_uezQueue aQueue,
            void *aItem)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;
    portBASE_TYPE higherTaskWoken;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGetFromISR(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error)
        return error;
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)
        return UEZ_ERROR_HANDLE_INVALID;
    if (xQueueReceiveFromISR(queue, aItem, &higherTaskWoken)) {
        // The queue item got queued
//        if (higherTaskWoken)
//            taskYIELD_FROM_ISR();
        return UEZ_ERROR_NONE;
    } else {
//        if (higherTaskWoken)
//            taskYIELD_FROM_ISR();
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError UEZQueuePeek(
            T_uezQueue aQueue,
            void *aItem,
            TUInt32 aTimeout)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;

    UEZAssert(aItem != 0);
    UEZAssert(aQueue != UEZ_NULL_HANDLE);
    UEZAssert(uEZHandleGetType(aQueue) == UEZ_HANDLE_QUEUE);

    error = uEZHandleGet(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (error) {
        return error;
    }
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE) {
        return UEZ_ERROR_HANDLE_INVALID;
    }
    if (aTimeout == UEZ_TIMEOUT_INFINITE)
        aTimeout = portMAX_DELAY;
    if (xQueuePeek(queue, aItem, aTimeout)) {
        // The queue item got queued
        return UEZ_ERROR_NONE;
    } else {
        // Queue item not queued (timeout)
        return UEZ_ERROR_TIMEOUT;
    }
}

T_uezError UEZQueueGetCount(T_uezQueue aQueue, TUInt32 *aCount)
{
    xQueueHandle queue;
    TUInt32 type;
    T_uezError error;

    error = uEZHandleGet(aQueue, &type, (TUInt32 *)&queue, 0, 0);
    if (UEZ_ERROR_NONE == error)
    {
        if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)
        {
            error = UEZ_ERROR_HANDLE_INVALID;
        }
        else
        {
            // Get number of waiting messages.
            *aCount = (TUInt32)uxQueueMessagesWaiting(queue);
            error = UEZ_ERROR_NONE;
        }
    }
    return error;
}


TUInt32 UEZTickCounterGet(void)
{
    return xTaskGetTickCount();
}

TUInt32 UEZTickCounterGetDelta(TUInt32 aStart)
{
    return xTaskGetTickCount()-aStart;
}
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    UEZHalt();
}

// Immediately context switch to another task inside the interrupt routine
void _isr_UEZTaskContextSwitch(void)
{
    portEND_SWITCHING_ISR(1);
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZRTOS.c
 *-------------------------------------------------------------------------*/
