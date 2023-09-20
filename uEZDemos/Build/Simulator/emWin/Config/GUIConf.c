/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES  (1024 * 1024) * 12  // x MByte

//
// Define the average block size
//
#define GUI_BLOCKSIZE 0x128

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

//
// 32 bit aligned memory area
//
#ifdef __ICCARM__
  #pragma location="GUI_RAM"
  static __no_init U32 _aMemory[GUI_NUMBYTES / 4];
#endif
#ifdef __CC_ARM
  U32 static _aMemory[GUI_NUMBYTES / 4] __attribute__ ((section ("GUI_RAM"), zero_init));
#endif
#ifdef __GNUC__
  U32 static _aMemory[GUI_NUMBYTES / 4] __attribute__ ((section(".GUI_RAM"))) = { 0 };
#endif
#ifdef _WINDOWS
  static U32 _aMemory[GUI_NUMBYTES / 4];
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
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
  GUI_ALLOC_AssignMemory(_aMemory, GUI_NUMBYTES);
  GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
}

/*************************** End of file ****************************/
