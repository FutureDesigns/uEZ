/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */
/*
 * Copyright (c) 2017 Simon Goldschmidt
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
 * Author: Simon Goldschmdit <goldsimon@gmx.de>
 *
 */
/*-------------------------------------------------------------------------*
 * File:  sys_arch.h
 *-------------------------------------------------------------------------*
 * Description:
 *     lwIP System Architecture file for uEZ system.
 *-------------------------------------------------------------------------*/
/*
 * Author: Adam Dunkels <adam@sics.se>
 * ESP-IDF for updated examples.
 *
 */
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include <uEZ.h>
#include <SEGGER_RTT_SYSVIEW_Config.h>

#ifndef __FILE_NAME__ // put this before assert define
#define __FILE_NAME__ __FILE__
#endif

#if (SEGGER_ENABLE_RTT == 1) 
// Could run asserts on RTT, but DIAG is used for all of the running information automatically already.
// See thte above cc.h include for the LWIP_PLATFORM_DIAG printfs
  #if 0
    #define LWIP_PLATFORM_ASSERT(x) do {SEGGER_RTT_printf_0("lwIP Assert \"%s\" failed L%d in %s\n", \
                                     x, __LINE__, __FILE_NAME__); /*fflush(NULL);*/ UEZFailureMsg(x);} while(0)
  #endif
                                     
#else // not RTT

#endif

// __write in flush is currently missing, don't use it.
#define LWIP_PLATFORM_ASSERT(x) do {printf("lwIP Assert \"%s\" failed L%d in %s\n", \
                                     x, __LINE__, __FILE_NAME__); /*fflush(NULL);*/ UEZFailureMsg(x);} while(0)
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lwipopts.h"
#include "lwip/arch.h" // make sure that assert define is before this include
#include <stdbool.h>

/** This is returned by _fromisr() sys functions to tell the outermost function
 * that a higher priority task was woken and the scheduler needs to be invoked.
 */
#define ERR_NEED_SCHED 123

/* This port includes FreeRTOS headers in sys_arch.c only.
 *  FreeRTOS uses pointers as object types. We use wrapper structs instead of
 * void pointers directly to get a tiny bit of type safety.
 */

#define SYS_MBOX_NULL (T_uezQueue)0
#define SYS_SEM_NULL  (T_uezSemaphore)0

#ifdef __cplusplus
extern "C" {
#endif
  
/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	( 15 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned portLONG ) 10000 )
  
extern T_uezTask G_lwipTask;

//typedef T_uezSemaphore sys_sem_t;
//typedef T_uezSemaphore sys_mutex_t;
//typedef T_uezQueue sys_mbox_t;
typedef T_uezTask sys_thread_t; // use uEZ task, but not uEZ semaphores

typedef SemaphoreHandle_t sys_sem_t; // use FreeRTOS semaphores, but not direct task API
typedef SemaphoreHandle_t sys_mutex_t;
//typedef TaskHandle_t sys_thread_t;

typedef struct sys_mbox_s {
  QueueHandle_t os_mbox;
  void *owner;
}* sys_mbox_t;

typedef enum {
    LWIP_CORE_LOCK_QUERY_HOLDER,
    LWIP_CORE_LOCK_MARK_HOLDER,
    LWIP_CORE_LOCK_UNMARK_HOLDER,
    LWIP_CORE_MARK_TCPIP_TASK,
    LWIP_CORE_IS_TCPIP_INITIALIZED,
} sys_thread_core_lock_t;

//#define sys_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )  // not working
//#define sys_sem_set_invalid( x ) ( ( *x ) = NULL )

#define sys_sem_valid(sem)             (((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_valid_val(sem)         ((sem) != NULL)
#define sys_sem_set_invalid(sem)       do { if((sem) != NULL) { *(sem) = NULL; }}while(0)
#define sys_sem_set_invalid_val(sem)   do { (sem) = NULL; }while(0)

#define LWIP_COMPAT_MUTEX 0

#if !LWIP_COMPAT_MUTEX
//#define sys_mutex_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
//#define sys_mutex_set_invalid( x ) ( ( *x ) = NULL )
#define sys_mutex_valid(mutex)         sys_sem_valid(mutex)
#define sys_mutex_set_invalid(mutex)   sys_sem_set_invalid(mutex)
#endif

/* Define the sys_mbox_set_invalid() to empty to support lock-free mbox in ESP LWIP.
 *
 * The basic idea about the lock-free mbox is that the mbox should always be valid unless
 * no socket APIs are using the socket and the socket is closed. ESP LWIP achieves this by
 * following two changes to official LWIP:
 * 1. Postpone the deallocation of mbox to netconn_free(), in other words, free the mbox when
 *    no one is using the socket.
 * 2. Define the sys_mbox_set_invalid() to empty if the mbox is not actually freed.

 * The second change is necessary. Consider a common scenario: the application task calls
 * recv() to receive packets from the socket, the sys_mbox_valid() returns true. Because there
 * is no lock for the mbox, the LWIP CORE can call sys_mbox_set_invalid() to set the mbox at
 * anytime and the thread-safe issue may happen.
 *
 * However, if the sys_mbox_set_invalid() is not called after sys_mbox_free(), e.g. in netconn_alloc(),
 * we need to initialize the mbox to invalid explicitly since sys_mbox_set_invalid() now is empty.
 */
//#define sys_mbox_set_invalid( x )  *x = NULL
//#define sys_mbox_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )

#define sys_mbox_valid(mbox)           sys_sem_valid(mbox)
#define sys_mbox_valid_val(mbox)       sys_sem_valid_val(mbox)
#define sys_mbox_set_invalid(mbox)     sys_sem_set_invalid(mbox)
#define sys_mbox_set_invalid_val(mbox) sys_sem_set_invalid_val(mbox)

void sys_delay_ms(uint32_t ms);

bool sys_thread_tcpip(sys_thread_core_lock_t type);

#if (LWIP_NETCONN_SEM_PER_THREAD == 1)
  #if configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0

  sys_sem_t* sys_arch_netconn_sem_get(void);
  sys_sem_t* sys_arch_netconn_sem_alloc(void);
  void sys_arch_netconn_sem_free(void);

  #define LWIP_NETCONN_THREAD_SEM_GET() sys_arch_netconn_sem_get()
  #define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_arch_netconn_sem_alloc()
  #define LWIP_NETCONN_THREAD_SEM_FREE() sys_arch_netconn_sem_free()
  #else

  sys_sem_t* sys_thread_sem_init(void);
  sys_sem_t* sys_thread_sem_get(void);
  void sys_thread_sem_deinit(void);

  //#if PTHREADS_THREAD_SAFE
  #define LWIP_NETCONN_THREAD_SEM_GET() sys_thread_sem_get()
  #define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
  #define LWIP_NETCONN_THREAD_SEM_FREE() sys_thread_sem_deinit()
  //#endif

  #endif
#endif


#if !NO_SYS
void sys_check_core_locking(void);
#define LWIP_ASSERT_CORE_LOCKED()  sys_check_core_locking()
void sys_mark_tcpip_thread(void);
#define LWIP_MARK_TCPIP_THREAD()   sys_mark_tcpip_thread()

#if (LWIP_TCPIP_CORE_LOCKING == 1)
void sys_lock_tcpip_core(void);
#define LOCK_TCPIP_CORE()          sys_lock_tcpip_core()
void sys_unlock_tcpip_core(void);
#define UNLOCK_TCPIP_CORE()        sys_unlock_tcpip_core()
#endif

#endif


TUInt32 GetRandomNumber(void); // part of the uEZ specific sys_arch. implementation.

sys_thread_t getTimeoutList(uint16_t index);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_ARCH_H__ */
