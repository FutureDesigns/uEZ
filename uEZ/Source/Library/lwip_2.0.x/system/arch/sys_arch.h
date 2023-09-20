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
 *
 */
#ifndef __LWIP_SYS_ARCH_H__
#define __LWIP_SYS_ARCH_H__

#include <uEZ.h>
//#include "lwip/lwipopt.h"
#include "lwipopts.h"
#include "lwip/arch.h"

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

typedef T_uezSemaphore sys_sem_t;
typedef T_uezSemaphore sys_mutex_t;
typedef T_uezQueue sys_mbox_t;
typedef T_uezTask sys_thread_t; 

/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	( 15 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned portLONG ) 10000 )
TUInt32 GetRandomNumber(void);


#if LWIP_NETCONN_SEM_PER_THREAD
sys_sem_t* sys_arch_netconn_sem_get(void);
void sys_arch_netconn_sem_alloc(void);
void sys_arch_netconn_sem_free(void);
#define LWIP_NETCONN_THREAD_SEM_GET()   sys_arch_netconn_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_arch_netconn_sem_alloc()
#define LWIP_NETCONN_THREAD_SEM_FREE()  sys_arch_netconn_sem_free()
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

#endif /* __LWIP_ARCH_CC_H__ */
/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.h
 *-------------------------------------------------------------------------*/
