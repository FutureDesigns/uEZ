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
File        : GUIDRV_S1D13781_Private.h
Purpose     : Interface definition for GUIDRV_S1D13781 driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_S1D13781.h"
#include "GUIDRV_NoOpt_1_8.h"

#ifndef GUIDRV_S1D13781_PRIVATE_H
#define GUIDRV_S1D13781_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef   LCD_WRITE_BUFFER_SIZE
  #define LCD_WRITE_BUFFER_SIZE 500
#endif

//
// BitBLT registers
//
#define REG_BITBLT_CR0    0x60880
#define REG_BITBLT_CR1    0x60882
#define REG_BITBLT_SR     0x60884
#define REG_BITBLT_CR     0x60886
#define REG_BITBLT_SSAR0  0x60888
#define REG_BITBLT_SSAR1  0x6088A
#define REG_BITBLT_DSAR0  0x6088C
#define REG_BITBLT_DSAR1  0x6088E
#define REG_BITBLT_ROR    0x60890
#define REG_BITBLT_WR     0x60892
#define REG_BITBLT_HR     0x60894
#define REG_BITBLT_BCR0   0x60896
#define REG_BITBLT_BCR1   0x60898
#define REG_BITBLT_FCR0   0x6089A
#define REG_BITBLT_FCR1   0x6089C

#define REG_BITBLT_CR_FILL   (1 << 1)
#define REG_BITBLT_CR0_START (1 << 0)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct DRIVER_CONTEXT_S1D13781 DRIVER_CONTEXT_S1D13781;

/*********************************************************************
*
*       MANAGE_VMEM_API
*/
typedef struct {
  //
  // Private function pointers
  //
  void (* pfWriteDataAuto_8) (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr, U8 Data);
  void (* pfWriteDataFlush_8)(DRIVER_CONTEXT_S1D13781 * pContext);

  void (* pfWriteDataM_8) (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr, U8 Data, U32 NumItems);
  void (* pfWriteDataMP_8)(DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr, U8 * pData, U32 NumItems);
  U8 * (* pfReadDataM_8)  (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr, U8 * pData, U32 NumItems);
  U8   (* pfReadData_8)   (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr);
  void (* pfWriteReg)     (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr, U16 Data);
  U16  (* pfReadReg)      (DRIVER_CONTEXT_S1D13781 * pContext, U32 Addr);

} MANAGE_VMEM_API;

/*********************************************************************
*
*       DRIVER_CONTEXT_S1D13781
*/
struct DRIVER_CONTEXT_S1D13781 {
  //
  // Common data
  //
  int xSize, ySize;
  int vxSize, vySize;
  int xPos, yPos;
  int MemSize;
  int SwapXY;
  int XOff;
  U32 BufferOffset;
  int NumBytesInBuffer;
  int IsVisible;
  U32 Addr;
  //
  // Driver specific data
  //
  int WriteBufferSize;
  int UseLayer;
  //
  // Accelerators for calculation
  //
  int BitsPerPixel;
  int BytesPerPixel;
  //
  // VRAM
  //
  U8 * pReadBuffer;
  U8 * pWriteBuffer;
  U8 * pWrite;
  //
  // API-Tables
  //
  MANAGE_VMEM_API ManageVMEM_API; // Memory management
  GUI_PORT_API    HW_API;         // Hardware routines
};

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
  DRIVER_CONTEXT_S1D13781 * pContext;                                                  \
                                                                                       \
  pContext = (DRIVER_CONTEXT_S1D13781 *)pDevice->u.pContext;                           \
  _SetPixelIndex(pContext, X_PHYS, Y_PHYS, PixelIndex);                                \
}

/*********************************************************************
*
*       _GetPixelIndex_##EXT
*/
#define DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                              \
static unsigned int _GetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y) { \
  DRIVER_CONTEXT_S1D13781 * pContext;                                          \
  LCD_PIXELINDEX PixelIndex;                                                   \
                                                                               \
  pContext = (DRIVER_CONTEXT_S1D13781 *)pDevice->u.pContext;                   \
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
void (*GUIDRV__S1D13781_GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void);
void   GUIDRV__S1D13781_SetOrg    (GUI_DEVICE *  pDevice,  int x, int y);
I32    GUIDRV__S1D13781_GetDevProp(GUI_DEVICE *  pDevice,  int Index);
void   GUIDRV__S1D13781_GetRect   (GUI_DEVICE *  pDevice,  LCD_RECT * pRect);
int    GUIDRV__S1D13781_InitOnce  (GUI_DEVICE *  pDevice);
int    GUIDRV__S1D13781_Init      (GUI_DEVICE *  pDevice);

U32    GUIDRV__S1D13781_GetAddr   (DRIVER_CONTEXT_S1D13781 * pContext, int x, int y);

#endif /* GUIDRV_S1D13781_PRIVATE_H */

/*************************** End of file ****************************/
