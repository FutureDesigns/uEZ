/*-------------------------------------------------------------------------*
 * File:  sys_arch.c
 *-------------------------------------------------------------------------*
 * Description:
 *     lwIP System Architecture file for uEZ system.
 *-------------------------------------------------------------------------*/

/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * This file is part of the lwIP TCP/IP stack.
 * Author: Adam Dunkels <adam@sics.se>
 */
// uEZ spefific includes need to come first to get RTOS defines and headers.
#include <uEZ.h>
#include <uEZRandom.h>
#include "Source/uEZSystem/uEZHandles.h"

#if (RTOS == FreeRTOS)
#include <Source/RTOS/FreeRTOS/include/task.h>
#elif (RTOS == SafeRTOS)
#include "SafeRTOS.h"
#include "task.h"
#include "portmacro.h"
#define portNOP()
#endif

#include "semphr.h"
#include "queue.h"

//#include <pthread.h> // On some other RTOS we might use pthreads

// lwIP includes.
#include "arch/sys_arch.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
//#include "arch/vfs_lwip.h"
// TODO include headers for any platform specific logging routines here
//#include "TODO_log.h"
// Note that we can use RTT today to log from normal DIAG defines.

#include <string.h>

// Define these optimizations to be empty if you don't have them.
#define unlikely(x) (x)
#define likely(x) (x) 

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if(UEZ_PROCESSOR == NXP_LPC4357)
/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 40 // increase for max simultaneous connections
#elif(UEZ_PROCESSOR == NXP_LPC4088)
/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 20 // increase for max simultaneous connections
#elif(UEZ_PROCESSOR == NXP_LPC1788)
/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 20 // increase for max simultaneous connections
#else/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 20 // increase for max simultaneous connections
#endif

/** Set this to 1 to enable core locking check functions in this port.
 * For this to work, you'll have to define LWIP_ASSERT_CORE_LOCKED()
 * and LWIP_MARK_TCPIP_THREAD() correctly in your lwipopts.h! */
#ifndef LWIP_FREERTOS_CHECK_CORE_LOCKING
#define LWIP_FREERTOS_CHECK_CORE_LOCKING              0
#endif

#if !configSUPPORT_DYNAMIC_ALLOCATION
# error "lwIP FreeRTOS port requires configSUPPORT_DYNAMIC_ALLOCATION"
#endif
#if !INCLUDE_vTaskDelay
# error "lwIP FreeRTOS port requires INCLUDE_vTaskDelay"
#endif
#if !INCLUDE_vTaskSuspend
# error "lwIP FreeRTOS port requires INCLUDE_vTaskSuspend"
#endif
#if LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX || !LWIP_COMPAT_MUTEX
#if !configUSE_MUTEXES
# error "lwIP FreeRTOS port requires configUSE_MUTEXES"
#endif
#endif

#ifdef NO_DYNAMIC_MEMORY_ALLOC // This is currently untested in uEZ.
#define sysarchMAX_MBOX_QUEUES 		( 5 )
#define sysarchMAX_SEMAPHORES		( 10 )

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

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

T_xMBoxQList xMBoxQueues[ sysarchMAX_MBOX_QUEUES ];
T_xMBoxQList *G_FreeMBox;

T_xSemList xSemaphoreQ[ sysarchMAX_SEMAPHORES ];
T_xSemList *G_FreeSemaphore;
#endif

struct timeoutlist { // track thread handles
    sys_thread_t pid;
};

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static struct timeoutlist timeoutlist[SYS_THREAD_MAX];
T_uezTask G_lwipTask = NULL;
static sys_mutex_t g_lwip_protect_mutex = NULL;
T_uezRandomStream G_randomstream;
static u16_t nextThread = 0;
int32_t errno; // It is required to add this here, defined in include/lwip/errno.h
//int32_t intlevel = 0;

#if (LWIP_NETCONN_SEM_PER_THREAD == 1)
#if configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0
#else
// need pthreads for this
static pthread_key_t sys_thread_sem_key;
static void sys_thread_sem_free(void* data);
#endif
#endif

#if !LWIP_COMPAT_MUTEX

/**
 * @brief Create a new mutex
 *
 * @param pxMutex pointer of the mutex to create
 * @return ERR_OK on success, ERR_MEM when out of memory
 */
err_t
sys_mutex_new(sys_mutex_t *pxMutex)
{
  *pxMutex = xSemaphoreCreateMutex();
  if (*pxMutex == NULL) {
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sys_mutex_new: out of mem\r\n"));
    return ERR_MEM;
  }

  LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sys_mutex_new: m=%p\n", *pxMutex));

  return ERR_OK;
}

/**
 * @brief Lock a mutex
 *
 * @param pxMutex pointer of mutex to lock
 */
void
sys_mutex_lock(sys_mutex_t *pxMutex)
{
  BaseType_t ret = xSemaphoreTake(*pxMutex, portMAX_DELAY);

  LWIP_ASSERT("failed to take the mutex", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Unlock a mutex
 *
 * @param pxMutex pointer of mutex to unlock
 */
void
sys_mutex_unlock(sys_mutex_t *pxMutex)
{
  BaseType_t ret = xSemaphoreGive(*pxMutex);

  LWIP_ASSERT("failed to give the mutex", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Delete a mutex
 *
 * @param pxMutex pointer of mutex to delete
 */
void
sys_mutex_free(sys_mutex_t *pxMutex)
{
  LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sys_mutex_free: m=%p\n", *pxMutex));
  vSemaphoreDelete(*pxMutex);
  *pxMutex = NULL;
}

#endif /* !LWIP_COMPAT_MUTEX */

/**
 * @brief Creates a new semaphore
 *
 * @param sem pointer of the semaphore
 * @param count initial state of the semaphore
 * @return err_t
 */
err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
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
  LWIP_ASSERT("initial_count invalid (neither 0 nor 1)",
             (count == 0) || (count == 1));

  *sem = xSemaphoreCreateBinary();
  if (*sem == NULL) {
#if SYS_STATS
      ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
      LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sys_sem_new: out of mem\r\n"));
      return ERR_MEM;
  }

  if (count == 1) {
      BaseType_t ret = xSemaphoreGive(*sem);
      LWIP_ASSERT("sys_sem_new: initial give failed", ret == pdTRUE);
      (void)ret;
  }

#if SYS_STATS
    ++lwip_stats.sys.sem.used;
    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
#endif /* SYS_STATS */

  return ERR_OK;
#endif
}

/**
 * @brief Signals a semaphore
 *
 * @param sem pointer of the semaphore
 */
void
sys_sem_signal(sys_sem_t *sem)
{
  BaseType_t ret = xSemaphoreGive(*sem);
  /* queue full is OK, this is a signal only... */
  LWIP_ASSERT("sys_sem_signal: sane return value",
             (ret == pdTRUE) || (ret == errQUEUE_FULL));
  (void)ret;
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore (from ISR)
int
sys_sem_signal_isr(sys_sem_t *sem)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(*sem, &woken);
    return woken == pdTRUE;
}

/**
 * @brief Wait for a semaphore to be signaled
 *
 * @param sem pointer of the semaphore
 * @param timeout if zero, will wait infinitely, or will wait at least for milliseconds specified by this argument
 * @return SYS_ARCH_TIMEOUT when timeout, 0 otherwise
 */
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  BaseType_t ret;

  if (!timeout) {
    /* wait infinite */
    ret = xSemaphoreTake(*sem, portMAX_DELAY);
    LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
  } else {
    /* Round up the number of ticks.
     * Not only we need to round up the number of ticks, but we also need to add 1.
     * Indeed, this function shall wait for AT LEAST timeout, but on FreeRTOS,
     * if we specify a timeout of 1 tick to `xSemaphoreTake`, it will take AT MOST
     * 1 tick before triggering a timeout. Thus, we need to pass 2 ticks as a timeout
     * to `xSemaphoreTake`. */
    TickType_t timeout_ticks = ((timeout + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS) + 1;
    ret = xSemaphoreTake(*sem, timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
      /* timed out */
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
  }

  return 0;
}

/**
 * @brief Delete a semaphore
 *
 * @param sem pointer of the semaphore to delete
 */
void
sys_sem_free(sys_sem_t *sem)
{

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
  vSemaphoreDelete(*sem);
  *sem = NULL;
#endif
}

/**
 * @brief Create an empty mailbox.
 *
 * @param mbox pointer of the mailbox
 * @param size size of the mailbox
 * @return ERR_OK on success, ERR_MEM when out of memory
 */
err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
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

  *mbox = mem_malloc(sizeof(struct sys_mbox_s));
  if (*mbox == NULL){
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("fail to new *mbox\n"));
    return ERR_MEM;
  }

  (*mbox)->os_mbox = xQueueCreate(size, sizeof(void *));

  if ((*mbox)->os_mbox == NULL) {
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("fail to new (*mbox)->os_mbox\n"));
    free(*mbox);
    return ERR_MEM;
  }

#if ESP_THREAD_SAFE
  (*mbox)->owner = NULL;
#endif

  LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("new *mbox ok mbox=%p os_mbox=%p\n", *mbox, (*mbox)->os_mbox));
#endif
	
#if SYS_STATS
    ++lwip_stats.sys.mbox.used;
    if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
        lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
    }
#endif /* SYS_STATS */

  return ERR_OK;
}

/**
 * @brief Send message to mailbox
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 */
void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  BaseType_t ret = xQueueSendToBack((*mbox)->os_mbox, &msg, portMAX_DELAY);
  LWIP_ASSERT("mbox post failed", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Try to post a message to mailbox
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 * @return ERR_OK on success, ERR_MEM when mailbox is full
 */
err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  err_t xReturn;

  if (xQueueSend((*mbox)->os_mbox, &msg, 0) == pdTRUE) {
    xReturn = ERR_OK;
  } else {
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("trypost mbox=%p fail\n", (*mbox)->os_mbox));
    xReturn = ERR_MEM;
  }

  return xReturn;
}

/**
 * @brief Try to post a message to mailbox from ISR
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 * @return  ERR_OK on success
 *          ERR_MEM when mailbox is full
 *          ERR_NEED_SCHED when high priority task wakes up
 */
err_t
sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  BaseType_t ret;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  ret = xQueueSendFromISR((*mbox)->os_mbox, &msg, &xHigherPriorityTaskWoken);
  if (ret == pdTRUE) {
    if (xHigherPriorityTaskWoken == pdTRUE) {
      return ERR_NEED_SCHED;
    }
    return ERR_OK;
  } else {
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    LWIP_ASSERT("mbox trypost failed", ret == errQUEUE_FULL);
    return ERR_MEM;
  }
}

/**
 * @brief Fetch message from mailbox
 *
 * @param mbox pointer of mailbox
 * @param msg pointer of the received message, could be NULL to indicate the message should be dropped
 * @param timeout if zero, will wait infinitely; or will wait milliseconds specify by this argument
 * @return SYS_ARCH_TIMEOUT when timeout, 0 otherwise
 */
u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  BaseType_t ret;
  void *msg_dummy;

  if (msg == NULL) {
    msg = &msg_dummy;
  }

  if (timeout == 0) {
    /* wait infinite */
    ret = xQueueReceive((*mbox)->os_mbox, &(*msg), portMAX_DELAY);
    LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
  } else {
    TickType_t timeout_ticks = timeout / portTICK_PERIOD_MS;
    ret = xQueueReceive((*mbox)->os_mbox, &(*msg), timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
      /* timed out */
      *msg = NULL;
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
  }

  return 0;
}

/**
 * @brief try to fetch message from mailbox
 *
 * @param mbox pointer of mailbox
 * @param msg pointer of the received message
 * @return SYS_MBOX_EMPTY if mailbox is empty, 1 otherwise
 */
u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  BaseType_t ret;
  void *msg_dummy;

  if (msg == NULL) {
    msg = &msg_dummy;
  }
  ret = xQueueReceive((*mbox)->os_mbox, &(*msg), 0);
  if (ret == errQUEUE_EMPTY) {
    *msg = NULL;
    return SYS_MBOX_EMPTY;
  }
  LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);

  return 0;
}

void
sys_mbox_set_owner(sys_mbox_t *mbox, void* owner)
{
  if (mbox && *mbox) {
    (*mbox)->owner = owner;
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("set mbox=%p owner=%p", *mbox, owner));
  }
}

/**
 * @brief Delete a mailbox
 *
 * @param mbox pointer of the mailbox to delete
 */
void
sys_mbox_free(sys_mbox_t *mbox)
{
  if ((NULL == mbox) || (NULL == *mbox)) {
    return;
  }

  UBaseType_t msgs_waiting = uxQueueMessagesWaiting((*mbox)->os_mbox);
  LWIP_ASSERT("mbox quence not empty", msgs_waiting == 0);
#if SYS_STATS
  if(msgs_waiting != 0) {
     lwip_stats.sys.mbox.err++;
  }
#endif /* SYS_STATS */

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

  vQueueDelete((*mbox)->os_mbox); // already called vfree in queue SW!
  //free(*mbox);
  *mbox = NULL;

  (void)msgs_waiting;  
#endif

#if SYS_STATS
     --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/**
 * @brief Create a new thread
 *
 * @param name thread name
 * @param thread thread function
 * @param arg thread arguments
 * @param stacksize stacksize of the thread
 * @param prio priority of the thread
 * @return thread ID
 */
sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  BaseType_t ret;
  T_uezTask CreatedTask = NULL;

  /* LwIP's lwip_thread_fn matches FreeRTOS' TaskFunction_t, so we can pass the
     thread function without adaption here. */
  //ret = xTaskCreatePinnedToCore(
    //      thread, name, stacksize, arg, prio, &rtos_task,
          //CONFIG_LWIP_TCPIP_TASK_AFFINITY
          //);

  ret = UEZTaskCreate(
                (T_uezTaskFunction)thread,
                name,
                stacksize,
                arg,
                (T_uezPriority)prio,
                &CreatedTask);
  
  if(strcmp(name, TCPIP_THREAD_NAME) == 0){
    G_lwipTask = CreatedTask;
  }

  LWIP_DEBUGF(TCPIP_DEBUG, ("new lwip task : %x, prio:%d,stack:%d\n",
             (u32_t)G_lwipTask, prio, stacksize));

  if (ret == UEZ_ERROR_NONE) {
    // For each task created, store the task handle (pid) in the timers array.
    // This scheme doesn't allow for threads to be deleted
    if(nextThread < SYS_THREAD_MAX) {
      timeoutlist[nextThread++].pid = CreatedTask;
    }
    return CreatedTask;
  } else {
    return NULL;
  }
}

sys_thread_t getTimeoutList(uint16_t index)
{
  return timeoutlist[index].pid;
}

/**
 * @brief Initialize the sys_arch layer
 *
 */
void
sys_init(void)
{
  if (!g_lwip_protect_mutex) {
    if (ERR_OK != sys_mutex_new(&g_lwip_protect_mutex)) {
      LWIP_PLATFORM_ERROR("sys_init: failed to init lwip protect mutex\n");
    }
  }

#if (LWIP_NETCONN_SEM_PER_THREAD == 1)
#if configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0
#else
  // Create the pthreads key for the per-thread semaphore storage
  pthread_key_create(&sys_thread_sem_key, sys_thread_sem_free);
  #endif
  #endif

  //esp_vfs_lwip_sockets_register();

// Initialize the the per-thread sys_timeouts structures
// make sure there are no valid pids in the list
    for (uint16_t i = 0; i < SYS_THREAD_MAX; i++) {
        timeoutlist[i].pid = 0;
    }

    T_uezRandomStream randomstream;
    UEZRandomStreamCreate(&randomstream, UEZTickCounterGet(),
            UEZ_RANDOM_PSUEDO);

    // keep track of how many threads have been created
    nextThread = 0;

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

/**
 * @brief Get system ticks
 *
 * @return system tick counts
 */
u32_t
sys_jiffies(void)
{
  return xTaskGetTickCount();
}

/**
 * @brief Get current time, in miliseconds
 *
 * @return current time
 */
u32_t
sys_now(void)
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/**
 * @brief Protect critical region
 *
 * @note This function is only called during very short critical regions.
 *
 * @return previous protection level
 */
sys_prot_t
sys_arch_protect(void)
{
  if (unlikely(!g_lwip_protect_mutex)) {
    sys_mutex_new(&g_lwip_protect_mutex);
  }
  sys_mutex_lock(&g_lwip_protect_mutex);
  return (sys_prot_t) 1;
}

/**
 * @brief Unprotect critical region
 *
 * @param pval protection level
 */
void
sys_arch_unprotect(sys_prot_t pval)
{
  LWIP_UNUSED_ARG(pval);
  sys_mutex_unlock(&g_lwip_protect_mutex);
}

void
sys_delay_ms(uint32_t ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

bool
sys_thread_tcpip(sys_thread_core_lock_t type)
{
    static sys_thread_t lwip_task = NULL;
#if LWIP_TCPIP_CORE_LOCKING
    static sys_thread_t core_lock_holder = NULL;
#endif
    switch (type) {
        default:
            return false;
        case LWIP_CORE_IS_TCPIP_INITIALIZED:
            return lwip_task != NULL;
        case LWIP_CORE_MARK_TCPIP_TASK:
            LWIP_ASSERT("LWIP_CORE_MARK_TCPIP_TASK: lwip_task == NULL", (lwip_task == NULL));
            lwip_task = (sys_thread_t) xTaskGetCurrentTaskHandle();
            return true;
#if LWIP_TCPIP_CORE_LOCKING
        case LWIP_CORE_LOCK_QUERY_HOLDER:
            return lwip_task ? core_lock_holder == (sys_thread_t) xTaskGetCurrentTaskHandle() : true;
        case LWIP_CORE_LOCK_MARK_HOLDER:
            core_lock_holder = (sys_thread_t) xTaskGetCurrentTaskHandle();
            return true;
        case LWIP_CORE_LOCK_UNMARK_HOLDER:
            core_lock_holder = NULL;
            return true;
#else
        case LWIP_CORE_LOCK_QUERY_HOLDER:
            return lwip_task == NULL || lwip_task == (sys_thread_t) xTaskGetCurrentTaskHandle();
#endif /* LWIP_TCPIP_CORE_LOCKING */
    }
    //return true;
}


#if (LWIP_NETCONN_SEM_PER_THREAD == 1)
#if configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0

sys_sem_t*
sys_arch_netconn_sem_get(void)
{
  //void* ret;
  sys_sem_t * ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = (sys_sem_t *) pvTaskGetThreadLocalStoragePointer(task, 0);
  return ret;
}

sys_sem_t*
sys_arch_netconn_sem_alloc(void)
{
  void *ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = pvTaskGetThreadLocalStoragePointer(task, 0);
  if(ret == NULL) {
    sys_sem_t *sem;
    err_t err;
    (void) ((err));
    /* need to allocate the memory for this semaphore */
    sem = mem_malloc(sizeof(sys_sem_t));
    LWIP_ASSERT("sem != NULL", sem != NULL);
    err = sys_sem_new(sem, 0);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
    LWIP_ASSERT("sem invalid", sys_sem_valid(sem));
    vTaskSetThreadLocalStoragePointer(task, 0, sem);
  }
  return ret;
}

void sys_arch_netconn_sem_free(void)
{
  void* ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = pvTaskGetThreadLocalStoragePointer(task, 0);
  if(ret != NULL) {
    sys_sem_t *sem = ret;
    sys_sem_free(sem); // calls vSemaphoreDelete
    mem_free(sem);
    vTaskSetThreadLocalStoragePointer(task, 0, NULL);
  }
}

#else /* configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 */
#error LWIP_NETCONN_SEM_PER_THREAD needs configNUM_THREAD_LOCAL_STORAGE_POINTERS

// implementing (from esp-idf example ) of using pthreads intead of thread local sems
/*
 * get per thread semaphore
 */
sys_sem_t*
sys_thread_sem_get(void)
{
  sys_sem_t *sem = pthread_getspecific(sys_thread_sem_key);

  if (!sem) {
      sem = sys_thread_sem_init();
  }
  LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sem_get s=%p\n", sem));
  return sem;
}
sys_sem_t*
sys_thread_sem_init(void)
{
  sys_sem_t *sem = (sys_sem_t*)mem_malloc(sizeof(sys_sem_t*));

  if (!sem){
    LWIP_PLATFORM_ASSERT("thread_sem_init: out of memory");
    return 0;
  }

  *sem = xSemaphoreCreateBinary();
  if (!(*sem)){
    free(sem);
    LWIP_PLATFORM_ASSERT("thread_sem_init: out of memory");
    return 0;
  }

  pthread_setspecific(sys_thread_sem_key, sem);
  return sem;
}

void
sys_thread_sem_deinit(void)
{
  sys_sem_t *sem = pthread_getspecific(sys_thread_sem_key);
  if (sem != NULL) {
    sys_thread_sem_free(sem);
    pthread_setspecific(sys_thread_sem_key, NULL);
  }

  static void
sys_thread_sem_free(void* data) // destructor for TLS semaphore
{
  sys_sem_t *sem = (sys_sem_t*)(data);

  if (sem && *sem){
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sem del, sem=%p\n", *sem));
    vSemaphoreDelete(*sem);
  }

  if (sem) {
    LWIP_DEBUGF(THREAD_SAFE_DEBUG, ("sem pointer del, sem_p=%p\n", sem));
    free(sem);
  }
}
}
#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 */

#endif /* LWIP_NETCONN_SEM_PER_THREAD */

TUInt32 GetRandomNumber(void)
{
    return UEZRandomSigned32Bit(&G_randomstream);
}


#if (LWIP_FREERTOS_CHECK_CORE_LOCKING == 1)
#if (LWIP_TCPIP_CORE_LOCKING == 1)

/** Flag the core lock held. A counter for recursive locks. */
static u8_t lwip_core_lock_count;
static TaskHandle_t lwip_core_lock_holder_thread; // Note: need multiple tcpip tasks to prevent variable from optimizing away
extern sys_sem_t lock_tcpip_core;

void
sys_lock_tcpip_core(void)
{
  (void) ((lwip_core_lock_holder_thread));
   sys_mutex_lock(&lock_tcpip_core);
   if (lwip_core_lock_count == 0) {
     lwip_core_lock_holder_thread = xTaskGetCurrentTaskHandle();
   }
   lwip_core_lock_count++;
}

void
sys_unlock_tcpip_core(void)
{
   lwip_core_lock_count--;
   if (lwip_core_lock_count == 0) {
       lwip_core_lock_holder_thread = 0;
   }
   sys_mutex_unlock(&lock_tcpip_core);
}

#endif /* LWIP_TCPIP_CORE_LOCKING */

#if !NO_SYS
static TaskHandle_t lwip_tcpip_thread;
#endif

void
sys_mark_tcpip_thread(void)
{
#if !NO_SYS
  lwip_tcpip_thread = xTaskGetCurrentTaskHandle();
#endif
}

void
sys_check_core_locking(void)
{
  /* Embedded systems should check we are NOT in an interrupt context here */
  /* E.g. core Cortex-M3/M4 ports:
         configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );

     Instead, we use more generic FreeRTOS functions here, which should fail from ISR: */
  taskENTER_CRITICAL();
  taskEXIT_CRITICAL();

#if !NO_SYS
  if (lwip_tcpip_thread != 0) {
    TaskHandle_t current_thread = xTaskGetCurrentTaskHandle();
    UNUSED(current_thread);
#if LWIP_TCPIP_CORE_LOCKING // If you hit this assert add LOCK_TCPIP_CORE() and UNLOCK_TCPIP_CORE() too app.
    LWIP_ASSERT("Function called without core lock",
                current_thread == lwip_core_lock_holder_thread && lwip_core_lock_count > 0);
#else /* LWIP_TCPIP_CORE_LOCKING */
    LWIP_ASSERT("Function called from wrong thread", current_thread == lwip_tcpip_thread);
#endif /* LWIP_TCPIP_CORE_LOCKING */
  }
#endif /* !NO_SYS */
}

#endif /* LWIP_FREERTOS_CHECK_CORE_LOCKING*/
