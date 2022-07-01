/*-------------------------------------------------------------------------*
 * File:  sys_arch.c
 *-------------------------------------------------------------------------*
 * Description:
 *     lwIP System Architecture file for uEZ system.
 *-------------------------------------------------------------------------*/

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#if (RTOS == FreeRTOS)
#include <Source/RTOS/FreeRTOS/include/task.h>
#elif (RTOS == SafeRTOS)
#include "SafeRTOS.h"
#include "portmacro.h"

#define portNOP()

#endif
#include <uEZRandom.h>

#include "Source/uEZSystem/uEZHandles.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 4

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
struct timeoutlist {
    sys_thread_t pid;
};

#ifdef NO_DYNAMIC_MEMORY_ALLOC
#define sysarchMAX_MBOX_QUEUES 		( 5 )
#define sysarchMAX_SEMAPHORES		( 10 )

typedef struct tag_MBoxQList
{
    T_uezQueue xQHandle;
    struct tag_MBoxQList *xNext;
}T_xMBoxQList;

typedef struct tag_SemList
{
    T_uezSemaphore xSemaphore;
    struct tag_SemList *xNext;
}T_xSemList;

#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
//static struct timeoutlist timeoutlist[SYS_THREAD_MAX];
static u16_t nextthread = 0;
int32_t intlevel = 0;
int32_t errno;
T_uezRandomStream G_randomstream;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
T_xMBoxQList xMBoxQueues[ sysarchMAX_MBOX_QUEUES ];
T_xMBoxQList *G_FreeMBox;

T_xSemList xSemaphoreQ[ sysarchMAX_SEMAPHORES ];
T_xSemList *G_FreeSemaphore;
#endif

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates a new mailbox
 * Inputs:
 *      int32_t size                -- Size of elements in the mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
err_t sys_mbox_new(sys_mbox_t *mbox, int32_t size)
{

#ifdef NO_DYNAMIC_MEMORY_ALLOC
	if( G_FreeMBox == NULL )
	{
	  	return ERR_MEM;
	}
	else
	{
		mbox = &G_FreeMBox->xQHandle;
                (void)mbox;
		G_FreeMBox = G_FreeMBox->xNext;
	}
#else
    if (UEZQueueCreate(archMESG_QUEUE_LENGTH, sizeof(void *), mbox) != UEZ_ERROR_NONE) {
        return ERR_MEM;
    }
#if UEZ_REGISTER
    else {
        UEZQueueSetName(*mbox, "LwIP", "\0");
    }
#endif
#endif
	
#if SYS_STATS
    ++lwip_stats.sys.mbox.used;
    if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
        lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
    }
#endif /* SYS_STATS */

    return ERR_OK;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a mailbox. If there are messages still present in the
 *      mailbox when the mailbox is deallocated, it is an indication of a
 *      programming error in lwIP and the developer should be notified.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
void sys_mbox_free(sys_mbox_t *mbox)
{
    TUInt32 count;

    if (mbox == SYS_MBOX_NULL)
        return;

//MemCheckAllBlocks();
    UEZQueueGetCount(*mbox, &count);
    if (count) {
        /* Line for breakpoint.  Should never break here! */
        portNOP();
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    }

#ifdef NO_DYNAMIC_MEMORY_ALLOC
	{
	TUInt16	usIdx = 0;
	
		while( usIdx < sysarchMAX_MBOX_QUEUES )
		{
		  	if( &xMBoxQueues[ usIdx ].xQHandle == mbox )
			{
			  	xMBoxQueues[ usIdx ].xNext = G_FreeMBox;
			  	G_FreeMBox = &xMBoxQueues[ usIdx ];
				break;
			}
			usIdx++;
		}
	}
#else
    UEZQueueDelete(*mbox);
#endif    

#if SYS_STATS
     --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_post
 *---------------------------------------------------------------------------*
 * Description:
 *      Post the "msg" to the mailbox.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *data              -- Pointer to data to post
 *---------------------------------------------------------------------------*/
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
    // TBD: What happens when this routine times out?
    UEZQueueSend(
        *mbox,
        &data,
        (portTickType)(archPOST_BLOCK_TIME_MS / portTICK_RATE_MS));
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_trypost
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to post the "msg" to the mailbox.  Returns immediately with
 *      error if cannot.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *msg               -- Pointer to data to post
 * Outputs:
 *      err_t                   -- ERR_OK if message posted, else ERR_MEM
 *                                  if not.
 *---------------------------------------------------------------------------*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
    err_t result;

    //Add checks for valid handle
    if ((*mbox >= UEZ_NUM_HANDLES) || (*mbox == UEZ_NULL_HANDLE) ||
            ((G_handles[*mbox].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)){
        return ERR_MEM;
    }

    if (UEZQueueSend(*mbox, &msg, 0) == UEZ_ERROR_NONE) {
        result = ERR_OK;
    } else {
        // could not post, queue must be full
        result = ERR_MEM;
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    }

    return result;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_fetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread until a message arrives in the mailbox, but does
 *      not block the thread longer than "timeout" milliseconds (similar to
 *      the sys_arch_sem_wait() function). The "msg" argument is a result
 *      parameter that is set by the function (i.e., by doing "*msg =
 *      ptr"). The "msg" parameter maybe NULL to indicate that the message
 *      should be dropped.
 *
 *      The return values are the same as for the sys_arch_sem_wait() function:
 *      Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
 *      timeout.
 *
 *      Note that a function with a similar name, sys_mbox_fetch(), is
 *      implemented by lwIP.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- SYS_ARCH_TIMEOUT if timeout, else number
 *                                  of milliseconds until received.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    void *dummyptr;
    portTickType StartTime, EndTime, Elapsed;

    StartTime = UEZTickCounterGet();

    if (msg == NULL)
        msg = &dummyptr;

    if (timeout != 0) {
        if (UEZ_ERROR_NONE == UEZQueueReceive(*mbox, &(*msg), timeout)) {
            // Got message.  Calculate time it took
            EndTime = UEZTickCounterGet();
            Elapsed = EndTime - StartTime;
            if (Elapsed == 0)
                Elapsed = 1;
            return (Elapsed);
        } else {
            // timed out blocking for message
            *msg = 0;
            return SYS_ARCH_TIMEOUT;
        }
    } else {
        // block forever for a message.
        while (UEZ_ERROR_NONE != UEZQueueReceive(*mbox, &(*msg), 10000)) {
        }

        // Calculate time it took for the message to come in
        EndTime = UEZTickCounterGet();
        Elapsed = EndTime - StartTime;
        if (Elapsed == 0)
            Elapsed = 1;

        // return time blocked TBD test
        return (Elapsed);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_tryfetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Similar to sys_arch_mbox_fetch, but if message is not ready
 *      immediately, we'll return with SYS_MBOX_EMPTY.  On success, 0 is
 *      returned.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 * Outputs:
 *      u32_t                   -- SYS_MBOX_EMPTY if no messages.  Otherwise,
 *                                  return ERR_OK.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    void *dummyptr;

    if (msg == 0)
        msg = &dummyptr;

    if ((*mbox >= UEZ_NUM_HANDLES) || (*mbox == UEZ_NULL_HANDLE) ||
            ((G_handles[*mbox].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)){
        return SYS_MBOX_EMPTY;
    }
    if (uEZHandleGetType(*mbox) != UEZ_HANDLE_QUEUE) {
        return SYS_MBOX_EMPTY;
    }

    if (UEZ_ERROR_NONE == UEZQueueReceive(*mbox, &(*msg), 0)) {
        return ERR_OK;
    } else {
        return SYS_MBOX_EMPTY;
    }
}

/*----------------------------------------------------------------------------------*/
int32_t sys_mbox_valid(sys_mbox_t *mbox)
{
    if ((*mbox >= UEZ_NUM_HANDLES) || (*mbox == UEZ_NULL_HANDLE) ||
                ((G_handles[*mbox].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_QUEUE)){
        return 0;
    }

    if(*mbox == SYS_MBOX_NULL)
        return 0;
    else
        return 1;
}

/*-----------------------------------------------------------------------------------*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = SYS_MBOX_NULL;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates and returns a new semaphore. The "count" argument specifies
 *      the initial state of the semaphore.
 *      NOTE: Currently this routine only creates counts of 1 or 0
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      u8_t count              -- Initial count of semaphore (1 or 0)
 * Outputs:
 *      sys_sem_t               -- Created semaphore or 0 if could not create.
 *---------------------------------------------------------------------------*/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	if( G_FreeSemaphore == NULL )
	{
#if SYS_STATS
        ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
        return ERR_MEM;
	}
	else
	{
		sem = &G_FreeSemaphore->xSemaphore;
                (void)sem;
		G_FreeSemaphore = G_FreeSemaphore->xNext;
#if SYS_STATS
        ++lwip_stats.sys.sem.used;
        if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
            lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
	return ERR_OK;
	}
#else
    if(UEZSemaphoreCreateBinary(sem) != UEZ_ERROR_NONE){
#if SYS_STATS
        ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
        return ERR_MEM;
    }
#if UEZ_REGISTER
    else{
        UEZSemaphoreSetName(*sem, "LwIP", "\0");
    }
#endif
	
    // Means it can't be taken
    //if (count == 0)
    //    UEZSemaphoreGrab(*sem,1);

#if SYS_STATS
    ++lwip_stats.sys.sem.used;
    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
	return ERR_OK;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_sem_wait
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread while waiting for the semaphore to be
 *      signaled. If the "timeout" argument is non-zero, the thread should
 *      only be blocked for the specified time (measured in
 *      milliseconds).
 *
 *      If the timeout argument is non-zero, the return value is the number of
 *      milliseconds spent waiting for the semaphore to be signaled. If the
 *      semaphore wasn't signaled within the specified time, the return value is
 *      SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 *      (i.e., it was already signaled), the function may return zero.
 *
 *      Notice that lwIP implements a function with a similar name,
 *      sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to wait on
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- Time elapsed or SYS_ARCH_TIMEOUT.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    portTickType StartTime, EndTime, Elapsed;

    if ((*sem >= UEZ_NUM_HANDLES) || (*sem == UEZ_NULL_HANDLE) ||
                ((G_handles[*sem].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)){
        return SYS_ARCH_TIMEOUT;
    }

    StartTime = UEZTickCounterGet();
    if (timeout != 0) {
        if (UEZSemaphoreGrab(*sem, timeout) == UEZ_ERROR_NONE) {
            // return time blocked
            EndTime = UEZTickCounterGet();
            Elapsed = EndTime - StartTime;
            if (Elapsed == 0)
                Elapsed = 1;
            return (Elapsed); 	
        } else {
            // Timed out
            return SYS_ARCH_TIMEOUT;
        }
    } else {
        // must block without a timeout
        while(UEZSemaphoreGrab(*sem, 10000) != UEZ_ERROR_NONE)
            {}
        EndTime = UEZTickCounterGet();
        Elapsed = EndTime - StartTime;
        if (Elapsed == 0)
            Elapsed = 1;

        // return time blocked
        return (Elapsed);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_signal
 *---------------------------------------------------------------------------*
 * Description:
 *      Signals (releases) a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to signal
 *---------------------------------------------------------------------------*/
void sys_sem_signal(sys_sem_t *sem)
{
    if ((*sem >= UEZ_NUM_HANDLES) || (*sem == UEZ_NULL_HANDLE) ||
                ((G_handles[*sem].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)){
        return;
    }

    if (!sem)
        return;
    UEZSemaphoreRelease(*sem);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 *---------------------------------------------------------------------------*/
void sys_sem_free(sys_sem_t *sem)
{
    if ((*sem >= UEZ_NUM_HANDLES) || (*sem == UEZ_NULL_HANDLE) ||
                ((G_handles[*sem].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)){
        return;
    }
    if (!sem)
        return;
#if SYS_STATS
    --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
	
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	{
	TUInt16	usIdx = 0;
	
		while( usIdx < sysarchMAX_SEMAPHORES )
		{
		  	if(  &xSemaphoreQ[ usIdx ].xSemaphore == sem )
			{
			  	xSemaphoreQ[ usIdx ].xNext = G_FreeSemaphore;
			  	G_FreeSemaphore = &xSemaphoreQ[ usIdx ];
				break;
			}
			usIdx++;
		}
	}
#else	
    UEZSemaphoreDelete(*sem);
    sem = 0;
#endif
}

int32_t sys_sem_valid(sys_sem_t *sem)
{
    if ((*sem >= UEZ_NUM_HANDLES) || (*sem == UEZ_NULL_HANDLE) ||
                ((G_handles[*sem].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_SEMAPHORE)){
        return 0;
    }

  if (*sem == SYS_SEM_NULL)
    return 0;
  else
    return 1;
}

/*-----------------------------------------------------------------------------------*/
void sys_sem_set_invalid(sys_sem_t *sem) {
    *sem = SYS_SEM_NULL;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize sys arch
 *---------------------------------------------------------------------------*/
void sys_init(void)
{
// Initialize the the per-thread sys_timeouts structures
// make sure there are no valid pids in the list
//    for (i = 0; i < SYS_THREAD_MAX; i++)
//        timeoutlist[i].pid = 0;

    T_uezRandomStream randomstream;
    UEZRandomStreamCreate(&randomstream, UEZTickCounterGet(),
            UEZ_RANDOM_PSUEDO);

    // keep track of how many threads have been created
    nextthread = 0;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
    uint32_t i;
    // set up the list of mailbox queues
    for(i = 0; i < ( sysarchMAX_MBOX_QUEUES - 1 ); i++ )
    {
        UEZQueueCreate(archMESG_QUEUE_LENGTH, sizeof(void *), &( xMBoxQueues[ i ].xQHandle ) );
        xMBoxQueues[ i ].xNext = &xMBoxQueues[ i + 1 ];
    }
    UEZQueueCreate(archMESG_QUEUE_LENGTH, sizeof(void *), &( xMBoxQueues[ i ].xQHandle ) );
    xMBoxQueues[ i ].xNext = NULL;
    G_FreeMBox = &xMBoxQueues[ 0 ];

    // set up the list of mailbox queues
    for( i = 0; i < ( sysarchMAX_SEMAPHORES - 1 ); i++ )
    {
        UEZSemaphoreCreateBinary(&( xSemaphoreQ[ i ].xSemaphore ));
        xSemaphoreQ[ i ].xNext = &xSemaphoreQ[ i + 1 ];
    }
    UEZSemaphoreCreateBinary(&( xSemaphoreQ[ i ].xSemaphore ));
    xSemaphoreQ[ i ].xNext = NULL;
    G_FreeSemaphore = &xSemaphoreQ[ 0 ];
#endif
}

#if LWIP_COMPAT_MUTEX == 0
/* Create a new mutex*/
err_t sys_mutex_new(sys_mutex_t *mutex){
    return sys_sem_new(mutex, 0);
}
/*-----------------------------------------------------------------------------------*/
/* Deallocate a mutex*/
void sys_mutex_free(sys_mutex_t *mutex){
    sys_sem_free(mutex);
}
/*-----------------------------------------------------------------------------------*/
/* Lock a mutex*/
void sys_mutex_lock(sys_mutex_t *mutex){
    sys_arch_sem_wait(mutex, 0);
}

/*-----------------------------------------------------------------------------------*/
/* Unlock a mutex*/
void sys_mutex_unlock(sys_mutex_t *mutex){
    sys_sem_signal(mutex);
}
#endif /*LWIP_COMPAT_MUTEX*/

/*---------------------------------------------------------------------------*
 * Routine:  sys_thread_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts a new thread with priority "prio" that will begin its
 *      execution in the function "thread()". The "arg" argument will be
 *      passed as an argument to the thread() function. The id of the new
 *      thread is returned. Both the id and the priority are system
 *      dependent.
 * Inputs:
 *      char *name              -- Name of thread
 *      void (* thread)(void *arg) -- Pointer to function to run.
 *      void *arg               -- Argument passed into function
 *      int32_t stacksize           -- Required stack amount in bytes
 *      int32_t prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
        int32_t stacksize, int32_t prio) {
    T_uezTask CreatedTask;
    int32_t result;

    if (nextthread < SYS_THREAD_MAX) {
        result = UEZTaskCreate(
                (T_uezTaskFunction)thread,
                name,
                stacksize,
                arg,
                (T_uezPriority)prio,
                &CreatedTask);

        if (result == UEZ_ERROR_NONE) {
            // For each task created, store the task handle (pid) in the timers array.
            // This scheme doesn't allow for threads to be deleted
            //timeoutlist[nextthread++].pid = CreatedTask;
            return CreatedTask;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect(void)
{
    RTOS_ENTER_CRITICAL()
    ;
    return 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect(sys_prot_t pval)
{
    (void)pval;
    RTOS_EXIT_CRITICAL()
    ;
}

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert(const char *msg)
{
    (void)msg;
    /*FSL:only needed for debugging
     printf(msg);
     printf("\n\r");
     */
    RTOS_ENTER_CRITICAL();//portSVC_ENTER_CRITICAL;//vPortEnterCritical();
    for (;;)
        ;
}

u32_t sys_now(void)
{
    return UEZTickCounterGet();
}

TUInt32 GetRandomNumber(void)
{
    return UEZRandomSigned32Bit(&G_randomstream);
}

/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/
