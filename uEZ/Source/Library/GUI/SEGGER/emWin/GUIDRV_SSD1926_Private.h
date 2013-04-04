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
File        : GUIDRV_SSD1926_Private.h
Purpose     : Interface definition for GUIDRV_SSD1926 driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_SSD1926.h"
#include "GUIDRV_NoOpt_1_8.h"

#ifndef GUIDRV_SLIN_PRIVATE_H
#define GUIDRV_SLIN_PRIVATE_H

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct DRIVER_CONTEXT DRIVER_CONTEXT;

/*********************************************************************
*
*       MANAGE_VRAM_CONTEXT
*/
typedef struct {
  //
  // Function pointers for writing to display controller
  //
  void (* pfWriteReg)  (DRIVER_CONTEXT * pContext, U16 Reg, U16 Data);
  void (* pfWriteAddrW)(DRIVER_CONTEXT * pContext, U32 Addr);
  void (* pfWriteAddrB)(DRIVER_CONTEXT * pContext, U32 Addr);
  void (* pfWrite8)    (DRIVER_CONTEXT * pContext, U8  Data);
  void (* pfSetAddr)   (DRIVER_CONTEXT * pContext, U32 Addr);
  U8   (* pfRead8)     (DRIVER_CONTEXT * pContext, U32 Addr);
  void (* pfFlush)     (DRIVER_CONTEXT * pContext);
} MANAGE_VMEM_API;

/*********************************************************************
*
*       DRIVER_CONTEXT
*/
struct DRIVER_CONTEXT {
  //
  // Common data
  //
  int xSize, ySize;
  int vxSize, vySize;
  int UseCache;
  int MemSize;
  //
  // Driver specific data
  //
  int FirstSEG;
  int FirstCOM;
  //
  // Accelerators for calculation
  //
  int BytesPerLine;
  int BitsPerPixel;
  //
  // VRAM
  //
  U8 * pVMEM;
  //
  // Colors
  //
  U32 * pColors;
  //
  // Buffer for one line of data
  //
  U16 * aBuffer;        // Allocated buffer
  U16 * pBuffer;        // Pointer for writing operations
  int NumBytesInBuffer; // Number of bytes in buffer
  U32 Addr;             // Destination address of buffer
  int Odd;              // 1 if the next address to be written is odd
  //
  // Pointer to driver internal initialization routine
  //
  void (* pfInit) (GUI_DEVICE * pDevice);
  void (* pfCheck)(GUI_DEVICE * pDevice);
  //
  // API-Tables
  //
  MANAGE_VMEM_API ManageVMEM_API; // Memory management
  GUI_PORT_API    HW_API;         // Hardware routines
};

/*********************************************************************
*
*       Access macros
*/
#define LCD_WRITE_A0(Data)             pContext->HW_API.pfWrite8_A0(Data)
#define LCD_WRITE_A1(Data)             pContext->HW_API.pfWrite8_A1(Data)
#define LCD_WRITEM_A0(pData, NumItems) pContext->HW_API.pfWriteM8_A0(pData, NumItems)
#define LCD_WRITEM_A1(pData, NumItems) pContext->HW_API.pfWriteM8_A1(pData, NumItems)
#define LCD_READ_A1(Data)              Data = pContext->HW_API.pfRead8_A1()

/*********************************************************************
*
*       LOG2PHYS_xxx
*/
#define LOG2PHYS_X      (                  x    )
#define LOG2PHYS_X_OX   (pContext->xSize - x - 1)
#define LOG2PHYS_X_OY   (                  x    )
#define LOG2PHYS_X_OXY  (pContext->xSize - x - 1)
#define LOG2PHYS_X_OS   (                  y    )
#define LOG2PHYS_X_OSX  (pContext->ySize - y - 1)
#define LOG2PHYS_X_OSY  (                  y    )
#define LOG2PHYS_X_OSXY (pContext->ySize - y - 1)

#define LOG2PHYS_Y      (                  y    )
#define LOG2PHYS_Y_OX   (                  y    )
#define LOG2PHYS_Y_OY   (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OXY  (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OS   (                  x    )
#define LOG2PHYS_Y_OSX  (                  x    )
#define LOG2PHYS_Y_OSY  (pContext->xSize - x - 1)
#define LOG2PHYS_Y_OSXY (pContext->xSize - x - 1)

/*********************************************************************
*
*       _SetPixelIndex_##EXT
*/
#define DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                                      \
static void _SetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y, int PixelIndex) { \
  DRIVER_CONTEXT * pContext;                                                           \
                                                                                       \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                                    \
  _SetPixelIndex(pContext, X_PHYS, Y_PHYS, PixelIndex);                                \
}

/*********************************************************************
*
*       _GetPixelIndex_##EXT
*/
#define DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                              \
static unsigned int _GetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y) { \
  DRIVER_CONTEXT * pContext;                                                   \
  LCD_PIXELINDEX PixelIndex;                                                   \
                                                                               \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                            \
  PixelIndex = _GetPixelIndex(pContext, X_PHYS, Y_PHYS);                       \
  return PixelIndex;                                                           \
}

/*********************************************************************
*
*       _GetDevProp_##EXT
*/
#define DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                    \
static I32 _GetDevProp_##EXT(GUI_DEVICE * pDevice, int Index) { \
  switch (Index) {                                              \
  case LCD_DEVCAP_MIRROR_X: return MX;                          \
  case LCD_DEVCAP_MIRROR_Y: return MY;                          \
  case LCD_DEVCAP_SWAP_XY:  return SWAP;                        \
  }                                                             \
  return _GetDevProp(pDevice, Index);                           \
}

/*********************************************************************
*
*       DEFINE_FUNCTIONS
*/
#define DEFINE_FUNCTIONS(EXT, X_PHYS, Y_PHYS, MX, MY, SWAP) \
  DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                      \
  DEFINE_GUI_DEVICE_API(EXT)

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void (*GUIDRV__SSD1926_GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void);
void   GUIDRV__SSD1926_SetOrg    (GUI_DEVICE *  pDevice,  int x, int y);
I32    GUIDRV__SSD1926_GetDevProp(GUI_DEVICE *  pDevice,  int Index);
void   GUIDRV__SSD1926_GetRect   (GUI_DEVICE *  pDevice,  LCD_RECT * pRect);

#endif

/*************************** End of file ****************************/
