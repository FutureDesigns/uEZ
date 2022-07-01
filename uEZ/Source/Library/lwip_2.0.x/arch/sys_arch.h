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

#define SYS_MBOX_NULL (T_uezQueue)0
#define SYS_SEM_NULL  (T_uezSemaphore)0

typedef T_uezSemaphore sys_sem_t;
typedef T_uezQueue sys_mbox_t;
typedef T_uezTask sys_thread_t; 

/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	( 15 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned portLONG ) 10000 )


#endif /* __LWIP_ARCH_CC_H__ */
/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.h
 *-------------------------------------------------------------------------*/
