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
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#define GUICONF_C

#include "GUI.h"
#include <uEZ.h>

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

static T_uezSemaphore G_uez_emWin_sem;

static void uEZ_emWin_SignalEvent(void)
{
    UEZSemaphoreRelease(G_uez_emWin_sem);
}
static void uEZ_emWin_WaitEvent(void)
{
    UEZSemaphoreGrab(G_uez_emWin_sem, UEZ_TIMEOUT_INFINITE);
}
static void uEZ_emWin_WaitEventTimed(int Period)
{
    UEZSemaphoreGrab(G_uez_emWin_sem, (TUInt32)Period);
}

/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(GUI_pMem, GUI_MemSize);
  GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);

  UEZSemaphoreCreateBinary(&G_uez_emWin_sem);
  UEZSemaphoreRelease(G_uez_emWin_sem);
  GUI_SetSignalEventFunc(uEZ_emWin_SignalEvent);
  GUI_SetWaitEventFunc(uEZ_emWin_WaitEvent);
  GUI_SetWaitEventTimedFunc(uEZ_emWin_WaitEventTimed);
}

/*************************** End of file ****************************/
