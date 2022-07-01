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
File        : GUIConf.h
Purpose     : Configuration of available features and default values
----------------------------------------------------------------------
*/

#ifndef GUICONF_H
#define GUICONF_H

#include <Config.h>
#include <uEZTypes.h>
#include "SEGGER.h"

/*********************************************************************
*
*       Memory
*
**********************************************************************
*/

//
// Define memory space for emWin
//
extern TUInt32 UEZEmWinGetRAMAddr(void);
extern TUInt32 UEZEmWinGetRAMSize(void);

#ifndef EMWIN_RAM_BASE_ADDR
#define EMWIN_RAM_BASE_ADDR  UEZEmWinGetRAMAddr() // 0xA0200000
#endif
#ifndef EMWIN_RAM_SIZE
#define EMWIN_RAM_SIZE       UEZEmWinGetRAMSize() // 0x00200000
#endif

#define GUI_VRAM_BASE_ADDR  EMWIN_RAM_BASE_ADDR
#define GUI_VRAM_SIZE       EMWIN_RAM_SIZE

//
// Define the number of available virtual displays for the GUI
//
#define GUI_NUM_VIRTUAL_DISPLAYS  2

//
// Pixel width in bytes
//
#define GUI_PIXEL_WIDTH  2

//
// Define the average block size
//
#define GUI_BLOCKSIZE 0x80

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
#ifndef GUI_SUPPORT_MEMDEV
  #define GUI_SUPPORT_MEMDEV        (1)  // Memory devices available
#endif
#define GUI_SUPPORT_AA            (1)  // Anti aliasing available
#define WM_SUPPORT_STATIC_MEMDEV  (1)  // Static memory devices available
// new 5.24
#define GUI_SUPPORT_DEVICES  1    // Enable use of device pointers

/*********************************************************************
*
*       Default font
*/
#define GUI_DEFAULT_FONT          &GUI_Font6x8

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
#ifndef GUICONF_C
  #define EXTERN extern
#else
  #define EXTERN
#endif

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
EXTERN U32 * GUI_pMem;
EXTERN U32   GUI_MemSize;
// new 5.24
//#define GUI_MEMCPY(dst,src,size)   emWin_memcpy(dst,src,size)
#define GUI_MEMCPY(dst,src,size)   memcpy(dst,src,size)
extern void *emWin_memcpy(void *pDst, const void *pSrc, long size);


#endif  /* Avoid multiple inclusion */

/*************************** End of file ****************************/
