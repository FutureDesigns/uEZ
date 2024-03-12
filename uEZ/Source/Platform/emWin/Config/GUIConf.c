/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.24 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#define GUICONF_C

#include "GUI.h"
#include <uEZ.h>
#include "Source/uEZSystem/uEZHandles.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

static T_uezSemaphore G_uez_emWin_sem = UEZ_NULL_HANDLE;

static void uEZ_emWin_SignalEvent(void)
{
    UEZSemaphoreRelease(G_uez_emWin_sem);
}
static void uEZ_emWin_WaitEvent(void)
{
    UEZSemaphoreGrab(G_uez_emWin_sem, UEZ_TIMEOUT_INFINITE);
}
static void uEZ_emWin_WaitEventTimed(int32_t Period)
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

  if(G_uez_emWin_sem != UEZ_NULL_HANDLE) {
      UEZSemaphoreDelete(G_uez_emWin_sem);
  }
  UEZSemaphoreCreateBinary(&G_uez_emWin_sem);
  UEZSemaphoreRelease(G_uez_emWin_sem);
  GUI_SetSignalEventFunc(uEZ_emWin_SignalEvent);
  GUI_SetWaitEventFunc(uEZ_emWin_WaitEvent);
  GUI_SetWaitEventTimedFunc(uEZ_emWin_WaitEventTimed);
  //
  // Set default font
  //
  GUI_SetDefaultFont(GUI_FONT_6X8);
}

/*************************** End of file ****************************/
