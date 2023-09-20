/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 2002         SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

**** emWin/GSC Grafical user interface for embedded applications ****
emWin is protected by international copyright laws. Knowledge of the
source code may not be used to write a similar product. This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.h
Purpose     : Configuration of available features and default values
----------------------------------------------------------------------
*/

#ifndef GUICONF_H
#define GUICONF_H

/*********************************************************************
*
*       Multi layer/display support
*/
#define GUI_NUM_LAYERS            (16) // Maximum number of available layers

/*********************************************************************
*
*       Multi tasking support
*/
#define GUI_OS                    (1)  // Compile with multitasking support

/*********************************************************************
*
*       Configuration of available packages
*/
#ifndef   GUI_SUPPORT_TOUCH
  #define GUI_SUPPORT_TOUCH       (1)  // Support touchscreen
#endif
#define GUI_SUPPORT_MOUSE         (1)  // Support a mouse
#define GUI_SUPPORT_UNICODE       (1)  // Support mixed ASCII/UNICODE strings
#define GUI_WINSUPPORT            (1)  // Window manager package available
#define GUI_SUPPORT_MEMDEV        (1)  // Memory devices available
#define GUI_SUPPORT_AA            (1)  // Anti aliasing available
#define WM_SUPPORT_STATIC_MEMDEV  (1)  // Static memory devices available

/*********************************************************************
*
*       Default font
*/
#define GUI_DEFAULT_FONT          &GUI_Font6x8

#endif  // Avoid multiple inclusion

/*************************** End of file ****************************/
