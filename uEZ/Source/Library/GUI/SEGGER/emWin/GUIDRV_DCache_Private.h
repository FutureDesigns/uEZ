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

** emWin V5.14 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_DCache_Private.h
Purpose     : Private declarations for GUIDRV_DCache driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_FlexColor.h"

#ifndef GUIDRV_DCACHE_PRIVATE_H
#define GUIDRV_DCACHE_PRIVATE_H

#include "GUIDRV_DCache.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       DRIVER_CONTEXT
*/
typedef struct {
  //
  // Data
  //
  int xSize, ySize;       // Display size
  int vxSize, vySize;     // Virtual display size
  int BitsPerPixelDriver;
  int BitsPerPixel;
  int NumColors;
  LCD_PIXELINDEX IndexMask;
  U32 MemSize;
  GUI_RECT rDirty;
  const GUI_DEVICE_API * pMemdev_API;
  //
  // Cache mamagement
  //
  void     (* pfFlush        )(GUI_DEVICE * pDevice);
  void     (* pfSendCacheRect)(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1);
  //
  // Setting the rectangle to be filled up within the real driver
  //
  void     (* pfSetRect      )(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1, int OnOff);
  //
  // Mode dependent drawing functions
  //
  void     (* pfDrawBitmap   )(GUI_DEVICE * pDevice, int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 GUI_UNI_PTR * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void     (* pfFillRect     )(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1);
  unsigned (* pfGetPixelIndex)(GUI_DEVICE * pDevice, int x, int y);
  void     (* pfSetPixelIndex)(GUI_DEVICE * pDevice, int x, int y, int ColorIndex);
  //
  // Request information
  //
  I32      (* pfGetDevProp   )(GUI_DEVICE * pDevice, int Index);
  //
  // Initialization
  //
  void     (* pfInit)         (GUI_DEVICE * pDevice);
  //
  // Conversion array from cache to real display driver
  //
  LCD_PIXELINDEX * pConvert;
  LCD_PIXELINDEX * pIndex;
  //
  // Cache
  //
  U8 * pVRAM;
  U8 * pVRAM_Lock;
  int CacheLocked;
  int CacheStat;
  int CacheDirty;
  //
  // The driver which is used for the actual drawing operations
  //
  GUI_DEVICE * pDriver;
} DRIVER_CONTEXT;

/*********************************************************************
*
*       Private interface
*
**********************************************************************
*/
void GUIDRV_DCache__AddDirtyRect  (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
void GUIDRV_DCache__ClearDirtyRect(DRIVER_CONTEXT * pContext);
void GUIDRV_DCache__InitOnce      (GUI_DEVICE * pDevice);

#endif /* GUIDRV_DCACHE_PRIVATE_H */

/*************************** End of file ****************************/

