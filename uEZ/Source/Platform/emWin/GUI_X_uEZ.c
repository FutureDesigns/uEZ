/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2010  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.06 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X_uEZ.c
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include <uEZ.h>
#include <stdio.h>
#include <string.h>
#include "uEZRTOS.h"
#include "GUI.h"
#include "task.h"
//#include <intrinsics.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if (RTOS==SafeRTOS)
#else
static T_uezSemaphore _RSema;
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*      Timing:
*                 GUI_GetTime()
*                 GUI_Delay(int32_t)

  Some timing dependent routines require a GetTime
  and delay funtion. Default time unit (tick), normally is
  1 ms.
*/

int32_t GUI_X_GetTime(void) {
  return UEZTickCounterGet();
}

void GUI_X_Delay(int32_t Period) {
  UEZTaskDelay(Period);
}

/*********************************************************************
*
*       GUI_X_ExecIdle()
*
*/
void GUI_X_ExecIdle(void) {
  UEZTaskDelay(1);
}

/*********************************************************************
*
*      Multitasking:
*
*                 GUI_X_InitOS()
*                 GUI_X_GetTaskId()
*                 GUI_X_Lock()
*                 GUI_X_Unlock()
*
* Note:
*   The following routines are required only if emWin is used in a
*   true multi task environment, which means you have more than one
*   thread using the emWin API.
*   In this case the
*                       #define GUI_OS 1
*  needs to be in GUIConf.h
*/
#if (RTOS==SafeRTOS)
// TODO: Not so safe, eh?  These need recursive mutexes for something
void GUI_X_InitOS(void)    { }
void GUI_X_Unlock(void)    { }
void GUI_X_Lock(void)      { }
U32  GUI_X_GetTaskId(void) { return UEZTaskGetCurrent(); }
#else
void GUI_X_InitOS(void)    { UEZSemaphoreCreateRecursiveMutex(&_RSema, UEZ_PRIORITY_VERY_HIGH); }
void GUI_X_Unlock(void)    { UEZSemaphoreRecursiveRelease(_RSema); }
void GUI_X_Lock(void)      { UEZSemaphoreRecursiveGrab(_RSema, UEZ_TIMEOUT_INFINITE); }
U32  GUI_X_GetTaskId(void) { return ( uint32_t )xTaskGetCurrentTaskHandle();} //return UEZTaskGetCurrent(); } // emWin constantly calls locking functions to check the current task, so don't search all of the uEZ handles constantly
#endif

/*********************************************************************
*
*      GUI_X_Init()
*
* Note:
*   This routine is called from GUI_Init() in any case whether there
*   is an RTOS or not. You can use it for additional initializations
*   needed.
*/

void GUI_X_Init(void) {
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/
#define OS_SendString(s) GUI_USE_PARA(s)
void GUI_X_Log     (const char *s) { OS_SendString(s); }
void GUI_X_Warn    (const char *s) { OS_SendString(s); }
void GUI_X_ErrorOut(const char *s) { OS_SendString(s); }

void *emWin_memcpy(void *pDst, const void *pSrc, long size)
{
	return (void*)memcpy(pDst, pSrc, size);
}
/*************************** End of file ****************************/
