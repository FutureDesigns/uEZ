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

#define portNOP()

#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 4

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
struct timeoutlist
{
    struct sys_timeouts timeouts;
    sys_thread_t pid;
};

#ifdef NO_DYNAMIC_MEMORY_ALLOC
#define sysarchMAX_MBOX_QUEUES 		( 5 )
#define sysarchMAX_SEMAPHORES		( 10 )

typedef struct tag_MBoxQList
{
  	T_uezQueue xQHandle;
	struct tag_MBoxQList *xNext;
} T_xMBoxQList;


typedef struct tag_SemList
{
  	T_uezSemaphore xSemaphore;
	struct tag_SemList *xNext;
} T_xSemList;

#endif


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static struct timeoutlist timeoutlist[SYS_THREAD_MAX];
static u16_t nextthread = 0;
int intlevel = 0;

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
 *      int size                -- Size of elements in the mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
sys_mbox_t sys_mbox_new(int size)
{
    T_uezQueue mbox=0;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
	if( G_FreeMBox == NULL )
	{
	  	return SYS_MBOX_NULL;
	}
	else
	{
		mbox = G_FreeMBox->xQHandle;
		G_FreeMBox = G_FreeMBox->xNext;
	}
#else
    if (UEZQueueCreate(archMESG_QUEUE_LENGTH, sizeof(void *), &mbox) != UEZ_ERROR_NONE) {
        return SYS_MBOX_NULL;
    }
#if UEZ_REGISTER
    else {
        UEZQueueSetName(mbox, "LwIP", "\0");
    }
#endif
#endif
	
#if SYS_STATS
    ++lwip_stats.sys.mbox.used;
    if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
        lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
    }
#endif /* SYS_STATS */

    return mbox;
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
void sys_mbox_free(sys_mbox_t mbox)
{
    TUInt32 count;

    if (mbox == SYS_MBOX_NULL)
        return;

//MemCheckAllBlocks();
    UEZQueueGetCount(mbox, &count);
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
		  	if( xMBoxQueues[ usIdx ].xQHandle == mbox )
			{
			  	xMBoxQueues[ usIdx ].xNext = G_FreeMBox;
			  	G_FreeMBox = &xMBoxQueues[ usIdx ];
				break;
			}
			usIdx++;
		}
	}
#else
    UEZQueueDelete(mbox);
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
void sys_mbox_post(sys_mbox_t mbox, void *data)
{
    // TBD: What happens when this routine times out?
    UEZQueueSend(
        mbox,
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
err_t sys_mbox_trypost(sys_mbox_t mbox, void *msg)
{
    err_t result;

    if (UEZQueueSend(mbox, &msg, 0) == UEZ_ERROR_NONE) {
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
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
    void *dummyptr;
    portTickType StartTime, EndTime, Elapsed;

    StartTime = UEZTickCounterGet();

    if (msg == NULL)
        msg = &dummyptr;

    if (timeout != 0) {
        if (UEZ_ERROR_NONE == UEZQueueReceive(mbox, &(*msg), timeout)) {
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
        while(UEZ_ERROR_NONE != UEZQueueReceive(mbox, &(*msg), 10000))
            {}

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
u32_t sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **msg)
{
    void *dummyptr;

    if (msg == 0)
        msg = &dummyptr;

    if (UEZ_ERROR_NONE == UEZQueueReceive(mbox, &(*msg), 0)) {
        return ERR_OK;
    } else {
        return SYS_MBOX_EMPTY;
    }
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
sys_sem_t sys_sem_new(u8_t count)
{
    T_uezSemaphore xSemaphore = 0;
    T_uezError error;
	
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	if( G_FreeSemaphore == NULL )
	{
	  	error = UEZ_ERROR_OUT_OF_MEMORY;
	}
	else
	{
		xSemaphore = G_FreeSemaphore->xSemaphore;
		G_FreeSemaphore = G_FreeSemaphore->xNext;
		error = UEZ_ERROR_NONE;
	}
#else
    error = UEZSemaphoreCreateBinary(&xSemaphore);
#if UEZ_REGISTER
    UEZSemaphoreSetName(xSemaphore, "LwIP", "\0");
#endif
#endif
	
    if (error != UEZ_ERROR_NONE) {
#if SYS_STATS
        ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
        return 0;
    }
    // Means it can't be taken
    if (count == 0)	
        UEZSemaphoreGrab(xSemaphore,1);

#if SYS_STATS
    ++lwip_stats.sys.sem.used;
    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
	return xSemaphore;
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
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
    portTickType StartTime, EndTime, Elapsed;

    StartTime = UEZTickCounterGet();
    if (timeout != 0) {
        if (UEZSemaphoreGrab(sem, timeout) == UEZ_ERROR_NONE) {
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
        while(UEZSemaphoreGrab(sem, 10000) != UEZ_ERROR_NONE)
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
void sys_sem_signal(sys_sem_t sem)
{
    if (!sem)
        return;
    UEZSemaphoreRelease(sem);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 *---------------------------------------------------------------------------*/
void sys_sem_free(sys_sem_t sem)
{
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
		  	if(  xSemaphoreQ[ usIdx ].xSemaphore == sem )
			{
			  	xSemaphoreQ[ usIdx ].xNext = G_FreeSemaphore;
			  	G_FreeSemaphore = &xSemaphoreQ[ usIdx ];
				break;
			}
			usIdx++;
		}
	}
#else	
    UEZSemaphoreDelete(sem);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize sys arch
 *---------------------------------------------------------------------------*/
void sys_init(void)
{
    int i;

    // Initialize the the per-thread sys_timeouts structures
    // make sure there are no valid pids in the list
    for (i = 0; i < SYS_THREAD_MAX; i++)
        timeoutlist[i].pid = 0;

    // keep track of how many threads have been created
    nextthread = 0;
	
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	// set up the list of mailbox queues
	for( i = 0; i < ( sysarchMAX_MBOX_QUEUES - 1 ); i++ )
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

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_timeouts
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns a pointer to the per-thread sys_timeouts structure. In lwIP,
 *      each thread has a list of timeouts which is represented as a linked
 *      list of sys_timeout structures. The sys_timeouts structure holds a
 *      pointer to a linked list of timeouts. This function is called by
 *      the lwIP timeout scheduler and must not return a NULL value.
 *
 *      In a single threaded sys_arch implementation, this function will
 *      simply return a pointer to a global sys_timeouts variable stored in
 *      the sys_arch module.
 * Outputs:
 *      sys_timeouts *          -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
struct sys_timeouts *sys_arch_timeouts(void)
{
    int i;
    T_uezTask pid;
    struct timeoutlist *tl;

    pid = UEZTaskGetCurrent();

    for (i = 0; i < nextthread; i++) {
        tl = &timeoutlist[i];
        if (tl->pid == pid)
            return &(tl->timeouts);
    }

    // Error
    return 0;
}

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
 *      int stacksize           -- Required stack amount in bytes
 *      int prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
sys_thread_t sys_thread_new(
        char *name,
        void (* thread)(void *arg),
        void *arg,
        int stacksize,
        int prio)
{
    T_uezTask CreatedTask;
    int result;

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
            timeoutlist[nextthread++].pid = CreatedTask;
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
    RTOS_ENTER_CRITICAL();
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
    (void) pval;
    RTOS_EXIT_CRITICAL();
}

/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/
