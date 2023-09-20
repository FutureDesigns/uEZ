/*********************************************************************
* DISCLAIMER:                                                        *
* The software supplied by Renesas Technology America Inc. is        *
* intended and supplied for use on Renesas Technology products.      *
* This software is owned by Renesas Technology America, Inc. or      *
* Renesas Technology Corporation and is protected under applicable   *
* copyright laws. All rights are reserved.                           *
*                                                                    *
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, *
* IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO IMPLIED     *
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE *
* APPLY TO THIS SOFTWARE. RENESAS TECHNOLOGY AMERICA, INC. AND       *
* AND RENESAS TECHNOLOGY CORPORATION RESERVE THE RIGHT, WITHOUT      *
* NOTICE, TO MAKE CHANGES TO THIS SOFTWARE. NEITHER RENESAS          *
* TECHNOLOGY AMERICA, INC. NOR RENESAS TECHNOLOGY CORPORATION SHALL, *
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR        *
* CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER ARISING OUT OF THE *
* USE OR APPLICATION OF THIS SOFTWARE.                               *
*********************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :OSLayer.H                                             */
/*  DATE        :Tue, Mar 25, 2008                                     */
/*  DESCRIPTION :Header file for the RTOS adaptation layer             */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description

************************************************************************/
#ifndef OSLAYER_H
#define OSLAYER_H

/* Include files are used to support multiple RTOS platforms. */
#define FREERTOS   1
#define embOS      2
#define UCOSII     3

#define RTOS_PLATFORM FREERTOS

#if (RTOS_PLATFORM == FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define RLCD_QueueHandle            xQueueHandle
#define RLCD_SemaphoreHandle        xSemaphoreHandle
#define RLCD_TaskHandle             xTaskHandle 

#define RLCD_TaskCreate(func, name, stksize, param, priority, handle) \
        xTaskCreate(func, name, stksize, param, priority, &handle);
        
#define RLCD_TaskDelayUntil(start, duration) \
        vTaskDelayUntil(&start, duration);
        
#define RLCD_TaskDelay              vTaskDelay
#define RLCD_TaskSuspend            vTaskSuspend
#define RLCD_TaskYield              taskYIELD
#define RLCD_TaskResume             vTaskResume
#define RLCD_TaskResumeFromISR      xTaskResumeFromISR
#define RLCD_GetTaskHandle          xTaskGetCurrentTaskHandle
#define RLCD_GetTaskTickCount       xTaskGetTickCount
#define RLCD_Malloc                 pvPortMalloc
#define RLCD_TaskPrioritySet        vTaskPrioritySet
#define RLCD_TaskPriorityGet        uxTaskPriorityGet

#define RLCD_WaitEvent              
#define RLCD_SignalEvent            

#define RLCD_QueueCreate(queue, size, msg_type) \
        queue = xQueueCreate(size, (unsigned portBASE_TYPE)sizeof(msg_type));
        
#define RLCD_QueueSend(retval, queue, msg) \
        retval = (sI16)xQueueSend(queue, (void *)&msg, 0UL);
              
#define RLCD_QueueReceive(retval, queue, msg, timeout, dummyPtr) \
        retval = (sI16)xQueueReceive(queue, &msg, timeout);

#define RLCD_BinarySemaphoreCreate  vSemaphoreCreateBinary

#define RLCD_BinarySemaphoreTake(sema, timeout) xSemaphoreTake(sema, timeout)
        
#define RLCD_BinarySemaphoreGive    xSemaphoreGive

#define RLCD_OSStart                vTaskStartScheduler
#define RLCD_ENTER_CRITICAL         portENTER_CRITICAL()
#define RLCD_EXIT_CRITICAL          portEXIT_CRITICAL()
#define RLCD_ENTER_SWITCHING_ISR    portENTER_SWITCHING_ISR
#define RLCD_EXIT_SWITCHING_ISR     portEXIT_SWITCHING_ISR

#define demotaskSTACK_SIZE          configMINIMAL_STACK_SIZE
#define mainBUSMONITOR_PRIORITY      ( tskIDLE_PRIORITY + 3 )
#define mainAUDIOTASKS_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define mainDEMOTASKS_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define TICK_RATE_MS                portTICK_RATE_MS
#define TickType                    portTickType

#elif (RTOS_PLATFORM == embOS)
#include "RTOS.h"

#define pdPASS                      1
#define pdTRUE                      1
#define pdFALSE                     0

#define RLCD_QueueHandle            OS_Q*
#define RLCD_SemaphoreHandle        OS_RSEMA*
#define RLCD_TaskHandle             OS_TASK*

#define RLCD_TaskCreate(func, name, stksize, param, priority, handle) \
        handle = (OS_TASK*)OS_malloc(sizeof(OS_TASK)); \
        if ( handle) OS_CREATETASK_EX(handle, name, func, priority, MERGE2(handle, Stk), param);
       
#define RLCD_TaskDelayUntil(start, duration) \
        OS_DelayUntil((start += duration)); 

#define RLCD_TaskDelay              OS_Delay
#define RLCD_TaskSuspend            OS_Suspend
#define RLCD_TaskYield              OS_Yield
#define RLCD_TaskResume             OS_Resume
#define RLCD_GetTaskHandle          OS_GetpCurrentTask
#define RLCD_GetTaskTickCount       OS_GetTime
#define RLCD_Malloc                 OS_malloc

#define RLCD_WaitEvent              OS_WaitSingleEventTimed
#define RLCD_SignalEvent            OS_SignalEvent

#define RLCD_QueueCreate(queue) \
        queue = (OS_Q*)OS_malloc(sizeof(OS_Q)); \
        if (queue) OS_Q_Create(queue, QBuffer, sizeof(QBuffer));
        
#define RLCD_QueueSend(retval, queue, msg) \
        retval = OS_Q_Put(queue, (void *)&msg, sizeof(EVENT_MSG));

#define RLCD_QueueReceive(retval, queue, msg, timeout, dummyPtr) \
        retval = OS_Q_GetPtrTimed(queue, (void *)&dummyPtr, 0); \
        if (retval) { \
          memcpy( (void*)&msg, (void*)dummyPtr, sizeof(EVENT_MSG)); \
          OS_Q_Purge(queue); \
          retval = 1; \
        } else retval = 0; 
           
#define RLCD_QueuePurge             OS_Q_Purge

#define RLCD_BinarySemaphoreCreate(sema) \
        sema = (OS_RSEMA*)OS_malloc(sizeof(OS_RSEMA)); \
        if (sema) OS_CREATERSEMA(sema);
          
#define RLCD_BinarySemaphoreTake(sema, timeout) OS_Use(sema)
#define RLCD_BinarySemaphoreGive(sema)          OS_Unuse(sema)

#define RLCD_OSStart                OS_Start

#define demotaskSTACK_SIZE          256
#define mainTOUCHCAL_PRIORITY       200
#define mainBUSMONITOR_PRIORITY      100
#define mainEVENTMGR_PRIORITY        50
#define mainDEMOTASKS_PRIORITY      50
#define TICK_RATE_MS                1

#endif

#endif // OSLAYER_H
