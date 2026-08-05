/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.62                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_FreeRTOS.c
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 7947 $
*/
#include "FreeRTOS.h"
#include "task.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#include "string.h" // Required for memset


#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)

typedef struct SYSVIEW_FREERTOS_TASK_STATUS SYSVIEW_FREERTOS_TASK_STATUS;

struct SYSVIEW_FREERTOS_TASK_STATUS {
  U32         xHandle;
  const char* pcTaskName;
  unsigned    uxCurrentPriority;
  U32         pxStack;
  unsigned    uStackHighWaterMark;
};

static SYSVIEW_FREERTOS_TASK_STATUS _aTasks[SYSVIEW_FREERTOS_MAX_NOF_TASKS];
static unsigned _NumTasks;

/*********************************************************************
*
*       _cbSendTaskList()
*
*  Function description
*    This function is part of the link between FreeRTOS and SYSVIEW.
*    Called from SystemView when asked by the host, it uses SYSVIEW
*    functions to send the entire task list to the host.
*/
static void _cbSendTaskList(void) {
  unsigned n;

  for (n = 0; n < _NumTasks; n++) {
#if INCLUDE_uxTaskGetStackHighWaterMark // Report Task Stack High Watermark
    _aTasks[n].uStackHighWaterMark = uxTaskGetStackHighWaterMark((TaskHandle_t)_aTasks[n].xHandle);
#endif
    SYSVIEW_SendTaskInfo((U32)_aTasks[n].xHandle, _aTasks[n].pcTaskName, (unsigned)_aTasks[n].uxCurrentPriority, (U32)_aTasks[n].pxStack, (unsigned)_aTasks[n].uStackHighWaterMark);
  }
}

/*********************************************************************
*
*       _cbGetTime()
*
*  Function description
*    This function is part of the link between FreeRTOS and SYSVIEW.
*    Called from SystemView when asked by the host, returns the
*    current system time in micro seconds.
*/
static U64 _cbGetTime(void) {
  U64 Time;
#if SYSVIEW_PORT_PROVIDES_CONTEXT_CHECK
  if (xPortIsInsideInterrupt() == pdTRUE) {
    Time = xTaskGetTickCountFromISR();
  } else {
    Time = xTaskGetTickCount();
  }
#else
  Time = xTaskGetTickCountFromISR(); // TODO if still needed on RX set vSetVarulMaxPRIGROUPValue to fix from ISR version
#endif
  Time *= portTICK_PERIOD_MS;
  Time *= 1000;
  return Time;
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       SYSVIEW_AddTask()
*
*  Function description
*    Add a task to the internal list and record its information.
*/
void SYSVIEW_AddTask(U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32  pxStack, unsigned uStackHighWaterMark) {
  
  if (memcmp(pcTaskName, "IDLE", 5) == 0) {
    return;
  }
  
  if (_NumTasks >= SYSVIEW_FREERTOS_MAX_NOF_TASKS) {
    SEGGER_SYSVIEW_Warn("SYSTEMVIEW: Could not record task information. Maximum number of tasks reached.");
    return;
  }

  _aTasks[_NumTasks].xHandle = xHandle;
  _aTasks[_NumTasks].pcTaskName = pcTaskName;
  _aTasks[_NumTasks].uxCurrentPriority = uxCurrentPriority;
  _aTasks[_NumTasks].pxStack = pxStack;
  _aTasks[_NumTasks].uStackHighWaterMark = uStackHighWaterMark;

  _NumTasks++;

  SYSVIEW_SendTaskInfo(xHandle, pcTaskName,uxCurrentPriority, pxStack, uStackHighWaterMark);

}

/*********************************************************************
*
*       SYSVIEW_UpdateTask()
*
*  Function description
*    Update a task in the internal list and record its information.
*/
void SYSVIEW_UpdateTask(U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32 pxStack, unsigned uStackHighWaterMark) {
  unsigned n;
  
  if (memcmp(pcTaskName, "IDLE", 5) == 0) {
    return;
  }

  for (n = 0; n < _NumTasks; n++) {
    if (_aTasks[n].xHandle == xHandle) {
      break;
    }
  }
  if (n < _NumTasks) {
    _aTasks[n].pcTaskName = pcTaskName;
    _aTasks[n].uxCurrentPriority = uxCurrentPriority;
    _aTasks[n].pxStack = pxStack;
    _aTasks[n].uStackHighWaterMark = uStackHighWaterMark;

    SYSVIEW_SendTaskInfo(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
  } else {
    SYSVIEW_AddTask(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
  }
}

/*********************************************************************
*
*       SYSVIEW_DeleteTask()
*
*  Function description
*    Delete a task from the internal list.
*/
void SYSVIEW_DeleteTask(U32 xHandle) {
  unsigned n;
  
  if (_NumTasks == 0) {
    return; // Early out
  }  
  for (n = 0; n < _NumTasks; n++) {
    if (_aTasks[n].xHandle == xHandle) {
      break;
    }
  }
  if (n == (_NumTasks - 1)) {  
    //
    // Task is last item in list.
    // Simply zero the item and decrement number of tasks.
    //
    memset(&_aTasks[n], 0, sizeof(_aTasks[n]));
    _NumTasks--;
  } else if (n < _NumTasks) {
    //
    // Task is in the middle of the list.
    // Move last item to current position and decrement number of tasks.
    // Order of tasks does not really matter, so no need to move all following items.
    //
    _aTasks[n].xHandle             = _aTasks[_NumTasks - 1].xHandle;
    _aTasks[n].pcTaskName          = _aTasks[_NumTasks - 1].pcTaskName;
    _aTasks[n].uxCurrentPriority   = _aTasks[_NumTasks - 1].uxCurrentPriority;
    _aTasks[n].pxStack             = _aTasks[_NumTasks - 1].pxStack;
    _aTasks[n].uStackHighWaterMark = _aTasks[_NumTasks - 1].uStackHighWaterMark;
    memset(&_aTasks[_NumTasks - 1], 0, sizeof(_aTasks[_NumTasks - 1]));
    _NumTasks--;
  }
}

/*********************************************************************
*
*       SYSVIEW_SendTaskInfo()
*
*  Function description
*    Record task information.
*/
void SYSVIEW_SendTaskInfo(U32 TaskID, const char* sName, unsigned Prio, U32 StackBase, unsigned StackSize) {
  SEGGER_SYSVIEW_TASKINFO TaskInfo;

  memset(&TaskInfo, 0, sizeof(TaskInfo)); // Fill all elements with 0 to allow extending the structure in future version without breaking the code
  TaskInfo.TaskID     = TaskID;
  TaskInfo.sName      = sName;
  TaskInfo.Prio       = Prio;
  TaskInfo.StackBase  = StackBase;
  TaskInfo.StackSize  = StackSize;
  SEGGER_SYSVIEW_SendTaskInfo(&TaskInfo);
}

/*********************************************************************
*
*       Public API structures
*
**********************************************************************
*/
// Callbacks provided to SYSTEMVIEW by FreeRTOS
const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI = {
  _cbGetTime,
  _cbSendTaskList,
};
#include "SEGGER_SYSVIEW_Conf.h" // Bring in the name information here.


/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/* Some MCUs lack a standardized tick interrupt used by FreeRTOS, so in addition
 * to supplying that interrupt, you must supply additional SystemView functions. */
#if (defined __RX)      // Renesas RX
  #define SYSVIEW_INTERRUPT_ID_FUNC_NEEDED 1
#elif (defined CORE_M0) // Cortex-M0
  #define SYSVIEW_INTERRUPT_ID_FUNC_NEEDED 1
#else
  #define SYSVIEW_INTERRUPT_ID_FUNC_NEEDED 0
#endif

// Note that this function hasn't been fully checked for accuracy yet.
U32 SEGGER_SYSVIEW_X_GetInterruptId(void) {
  U32 IntId;
#ifdef __RX  // Renesas CCRX
  IntId = (get_psw() & 0x0F000000) >> 24u;
	// may have to manually assign interrupt numbers such as i2c = 0, uart = 1, etc to use this feature.
	//return (INTC_SIR_IRQ & (0x7Fu)); // INTC_SIR_IRQ[6:0]: ActiveIRQ
#endif
#ifdef CORE_M0 // Cortex-M0 example
  __asm volatile ("mrs %0, ipsr"
                  : "=r" (IntId)
                  );
  IntId &= 0x3F;
#endif
#if (SYSVIEW_INTERRUPT_ID_FUNC_NEEDED == 0)
  IntId = 1; // prevent warning
#endif
  return IntId;
}

/* If SYSVIEW_INTERRUPT_ID_FUNC_NEEDED=1 supply the SEGGER_SYSVIEW_X_GetTimestamp_Port_Specific()
 * in the application project to match the chosen tick timer and count up/down direction. Only on these 
 * ports is it needed to increment SEGGER_SYSVIEW_TickCnt in the tick handler. Most ports J-Link handles automatically.
 * Below is an example timer ISR and example get timestamp implementation for both up/down. */
U32 SEGGER_SYSVIEW_X_GetTimestamp(void) {
	return SEGGER_SYSVIEW_X_GetTimestamp_Port_Specific();
}

#if 0 // example of the interrupt and get timestamp for an up or down timer (to be implemented in application/BSP)
static void ExampleCustomRtosTickTimerIRQ(void) {
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
    SEGGER_SYSVIEW_TickCnt++; // <<-- Increment SEGGER_SYSVIEW_TickCnt asap.
#endif
    SysTick_Handler(); // map the RTOS tick to this interrupt
}

U32 SEGGER_SYSVIEW_X_GetTimestamp_Port_Specific(void) {
// Get the cycles of the current system tick. 
// For implementing this manually on timers both the up and down counting timer versions are shown.
// If switching to a differnet timer peripheral you may need to swap which code is included.

// If using SysTick down-counting, subtract the current value from the number of cycles per tick, otherwise it is the count.
//  U32 Cycles = (CyclesPerTick - LPC_RITIMER->COUNTER); // cycles per tick minus current value
  U32 Cycles = (LPC_RITIMER->COUNTER); // this timer is up counting, so no subtraction

  U32 TickCount = SEGGER_SYSVIEW_TickCnt;
  if (NVIC_GetPendingIRQ(Example_Timer_IRQn)) {   // Check if timer interrupt pending ...
   // Interrupt pending, re-read timer and adjust result
    //Cycles = (SYSVIEW_CYCLES_PER_TICK - LPC_RITIMER->COUNTER); // if a down counter was used subtract
    Cycles = (LPC_RITIMER->COUNTER); // if an up counter is used no subtraction
    TickCount++;
  }

  Cycles += TickCount * SYSVIEW_CYCLES_PER_TICK;
  return Cycles;
}
#endif // end example

#endif // #if (SEGGER_ENABLE_SYSTEM_VIEW == 1)

#include "FreeRTOSConfig.h"

/*************************** End of file ****************************/
