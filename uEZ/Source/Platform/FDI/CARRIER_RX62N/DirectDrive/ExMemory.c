/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :ExMemory.c                                            */
/*  DATE        :Wen, Feb 20, 2008                                     */
/*  DESCRIPTION :External Memory Manager tasks                         */
/*               ...coordinate MCU/ExDMA Accesses                      */
/*                                                                     */
/*  CPU TYPE    :H8S/H8SX                                              */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
*
************************************************************************/
#include "Config.h"
#include "DirectLCD_Types.h"
#include "OsLayer.h"
#include "ExMemory.h"

static void ExMemoryMonitor(void * pvParameters);      // The task of ExMemory Monitor
RLCD_TaskHandle      ExDMACHnd;

/*****************************************************************************/
//
// FUNCTION    :ExMemoryMonitorInit
// PARAMETERS  :init
// RETURNS     :void
//
// DESCRIPTION : Initialize external memory system
//
//
/*****************************************************************************/
void ExMemoryMonitorInit(void)
{
  // Create ExMemoryMonitor task
  (void)RLCD_TaskCreate(ExMemoryMonitor, "a:ExDMAC", demotaskSTACK_SIZE, NULL, mainBUSMONITOR_PRIORITY, ExDMACHnd);  
}

/*****************************************************************************/
//
// FUNCTION    :ExMemoryMonitor
// PARAMETERS  :(void *)pvParameters
// RETURNS     :
//
// DESCRIPTION :
//
//
/*****************************************************************************/
#define MAX_EXMEMORY 16
static RLCD_TaskHandle ExMemoryList[MAX_EXMEMORY];

static void ExMemoryMonitor(void *pvParameters)
{
  uI08 index;
  
  // Initialize list of tasks to suspend on data transfer
  for (index=0; index < MAX_EXMEMORY; index++)
    ExMemoryList[index] = NULL;

  for ( ;; )
  {    
    if (LCD_BusActive != 0)
    {
      // Suspend tasks currently using external memory
      for (index=0; index < MAX_EXMEMORY; index++)
      {
        if (ExMemoryList[index] != NULL)
          RLCD_TaskSuspend( ExMemoryList[index] );
      }
    }
    else
    {
      // Resume tasks currently using external memory
      for (index=0; index < MAX_EXMEMORY; index++)
      {
        if (ExMemoryList[index] != NULL)
          RLCD_TaskResume( ExMemoryList[index] );
      }
    }
    RLCD_TaskSuspend( NULL ); // Suspend ourselves.
  }
}

/*****************************************************************************/
//
// FUNCTION    :ExMemoryAcquire
// PARAMETERS  :RLCD_TaskHandle handle
// RETURNS     :
//
// DESCRIPTION :Lets the system know that this task is currently using the external memory.
//              The idea being that when the ExDMA block transfer is started, that we suspend
//              this task until the next Vertical Blanking period.
//
/*****************************************************************************/
void ExMemoryAcquire(RLCD_TaskHandle handle)
{
  uI08 index;
  uI08 return_flag = 0;

  for (;;)
  {
    // Search for an open slot
    for (index=0; index < MAX_EXMEMORY; index++)
    {
      RLCD_ENTER_CRITICAL;
      if (ExMemoryList[index] == NULL)
      {
        // Success, store and return
        ExMemoryList[index] = handle;
        return_flag = 1;
      }
      RLCD_EXIT_CRITICAL;
      if (return_flag)
      {
        // If the bus is already busy, we must manually suspend ourselves, 
        // The ExMemory Monitor task will resume us.
        if (LCD_BusActive != 0)
          RLCD_TaskSuspend( NULL );
        return;
      }
    }
    // No open slot found...we must wait
    RLCD_TaskYield();
  }
}

/*****************************************************************************/
//
// FUNCTION    :ExMemoryRelease
// PARAMETERS  :RLCD_TaskHandle handle
// RETURNS     :
//
// DESCRIPTION :Let's the system know that this task has completed using the external memory.
//
/*****************************************************************************/
void ExMemoryRelease(RLCD_TaskHandle handle)
{
  uI08 index;

  // Search for our slot and set to NULL
  for (index=0; index < MAX_EXMEMORY; index++)
  {
    if (ExMemoryList[index] == handle)
      ExMemoryList[index] = NULL;
  }
}

