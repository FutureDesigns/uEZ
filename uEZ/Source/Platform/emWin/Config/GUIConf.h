/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
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
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              NXP Semiconductors, 1109 McKay Dr, M/S 76, San Jose, CA 95131, USA
Licensed SEGGER software: emWin
License number:           GUI-00186
License model:            emWin License Agreement, dated August 20th 2011 and Amendment, dated October 19th 2017
Licensed platform:        NXP's ARM 7/9, Cortex-M0, M3, M4, M7, A7
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2011-08-19 - 2018-09-02
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUIConf.h
Purpose     : Configures emWins abilities, fonts etc.
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
#define EMWIN_RAM_BASE_ADDR  UEZEmWinGetRAMAddr()
#endif
#ifndef EMWIN_RAM_SIZE
#define EMWIN_RAM_SIZE       UEZEmWinGetRAMSize()
#endif

#define GUI_VRAM_BASE_ADDR  EMWIN_RAM_BASE_ADDR
#define GUI_VRAM_SIZE       EMWIN_RAM_SIZE

//
// Define the number of available virtual displays for the GUI
//
#define GUI_NUM_VIRTUAL_DISPLAYS  MAX_NUM_FRAMES // default of 2

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
*         Configuration of available packages
*/
#ifndef   GUI_SUPPORT_TOUCH
  #define GUI_SUPPORT_TOUCH       (1)  // Support a touch screen (req. win-manager)
#endif
#define GUI_SUPPORT_MOUSE         (1)  // Support a mouse
#define GUI_SUPPORT_UNICODE       (1)  // Support mixed ASCII/UNICODE strings
#define GUI_WINSUPPORT            (1)  // Use Window Manager
#ifndef GUI_SUPPORT_MEMDEV
  #define GUI_SUPPORT_MEMDEV      (1)  // Memory devices available
#endif
#define GUI_SUPPORT_AA            (1)  // Anti aliasing available
#define WM_SUPPORT_STATIC_MEMDEV  (1)  // Static memory devices available
// new 5.24
#define GUI_SUPPORT_DEVICES        1    // Enable use of device pointers
// new 5.30
//#define GUI_USE_ARGB             1    // Enable ARGB mode, default in 5.48+     

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


#endif  // Avoid multiple inclusion

/*************************** End of file ****************************/
