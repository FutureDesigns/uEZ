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
File        : GUIDRV_FlexColor_Private.h
Purpose     : Private declarations for GUIDRV_FlexColor driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_FlexColor.h"

#ifndef GUIDRV_FLEXCOLOR_PRIVATE_H
#define GUIDRV_FLEXCOLOR_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LCD_WRITE_BUFFER_SIZE           500

#define FLEXCOLOR_CF_MANAGE_ORIENTATION (1 << 0)
#define FLEXCOLOR_CF_RAM_ADDR_SET       (1 << 1)

#define FLEXCOLOR_CF_DEFAULT            FLEXCOLOR_CF_RAM_ADDR_SET

#define FLEXCOLOR_MAX_NUM_DUMMY_READS   5
#define FLEXCOLOR_NUM_DUMMY_READS       1

#define PUSH_RECT 0
#define POP_RECT  1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct DRIVER_CONTEXT DRIVER_CONTEXT;

/*********************************************************************
*
*       DRIVER_CONTEXT
*/
struct DRIVER_CONTEXT {
  //
  // Data
  //
  int xSize, ySize;   // Display size
  int vxSize, vySize; // Virtual display size
  int x0, y0, x1, y1; // Current position
  int NumDummyReads;  // Number of required dummy reads
  U16 RegEntryMode;   // Can be used for storing additional configuration bits for 'EntryMode' register which is modified by the driver
  U16 Flags;
  LCD_PIXELINDEX IndexMask;
  int FirstSEG, FirstCOM;
  int Orientation;
  int BitsPerPixel;
  const GUI_DEVICE_API * pMemdev_API;
  //
  // Cache
  //
  void * pVRAM;
  int CacheLocked;
  int CacheStat;
  int xPos, yPos;
  U32 Addr;
  GUI_RECT CacheRect;
  //
  // Buffers
  //
  void * pWriteBuffer;
  void * pLineBuffer;
  U8  aPair_8 [3 + FLEXCOLOR_MAX_NUM_DUMMY_READS];
  U16 aPair_16[3 + FLEXCOLOR_MAX_NUM_DUMMY_READS];
  //
  // Functions for writing single items (data, cmds) regardless of the interface
  //
  void (* pfSetReg)   (DRIVER_CONTEXT * pContext, U16 Data);
  void (* pfWritePara)(DRIVER_CONTEXT * pContext, U16 Data);
  void (* pfSetInterface)(DRIVER_CONTEXT * pContext, int BusWidth);
  //
  // Cache related function pointers
  //
  void (* pfSetCacheRect) (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
  void (* pfSendCacheRect)(DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
  U32  (* pfReadData)     (DRIVER_CONTEXT * pContext);
  void (* pfWriteData)    (DRIVER_CONTEXT * pContext, U32 PixelIndex);
  //
  // Controller specific routines
  //
  void (* pfSetRect)            (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
  void (* pfSetPhysRAMAddr)     (DRIVER_CONTEXT * pContext, int x, int y);
  void (* pfSetOrientation)     (DRIVER_CONTEXT * pContext);
  U16  (* pfReadPixel_16bpp_B16)(DRIVER_CONTEXT * pContext);
  U16  (* pfReadPixel_16bpp_B8) (DRIVER_CONTEXT * pContext);
  U32  (* pfReadPixel_18bpp_B9) (DRIVER_CONTEXT * pContext);
  void (* pfReadRect_16bpp_B16) (GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1, U16 * pBuffer);
  void (* pfReadRect_16bpp_B8)  (GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1, U16 * pBuffer);
  void (* pfReadRect_18bpp_B9)  (GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1, U32 * pBuffer);
  //
  // Mode dependent drawing functions
  //
  void    (* pfDrawBitmap   )(GUI_DEVICE *  pDevice, int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 GUI_UNI_PTR * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void    (* pfFillRect     )(GUI_DEVICE *  pDevice, int x0, int y0, int x1, int y1);
  unsigned(* pfGetPixelIndex)(GUI_DEVICE *  pDevice, int x, int y);
  void    (* pfSetPixelIndex)(GUI_DEVICE *  pDevice, int x, int y, int ColorIndex);
  U8      (* pfControlCache )(GUI_DEVICE *  pDevice, U8 Cmd);
  //
  // Controller dependent function pointers
  //
  void (* pfReadRect)(void);
  //
  // User definable function(s)
  //
  LCD_PIXELINDEX (* pfReadPixel_User)(void); // Currently not used
  //
  // Orientation
  //
  int  (* pfLog2PhysX)(DRIVER_CONTEXT * pContext, int x, int y);
  int  (* pfLog2PhysY)(DRIVER_CONTEXT * pContext, int x, int y);
  //
  // Hardware routines
  //
  GUI_PORT_API HW_API;
};

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void GUIDRV_FlexColor__InitOnce       (GUI_DEVICE * pDevice);
U8   GUIDRV_FlexColor__ControlCache   (GUI_DEVICE * pDevice, U8 Cmd);

void GUIDRV_FlexColor__AddCacheRect   (DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__IncrementCursor(DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__ManageRect     (DRIVER_CONTEXT * pContext, int Cmd);
void GUIDRV_FlexColor__SetCacheAddr   (DRIVER_CONTEXT * pContext, int x, int y);
void GUIDRV_FlexColor__SetCacheRect   (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
void GUIDRV_FlexColor__SetSubRect     (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);

unsigned int GUIDRV_FlexColor__GetPixelIndexCache  (GUI_DEVICE * pDevice, int x, int y);
void         GUIDRV_FlexColor__SetPixelIndexCache  (GUI_DEVICE * pDevice, int x, int y, int PixelIndex);
unsigned int GUIDRV_FlexColor__GetPixelIndexNoCache(GUI_DEVICE * pDevice, int x, int y);
void         GUIDRV_FlexColor__SetPixelIndexNoCache(GUI_DEVICE * pDevice, int x, int y, int PixelIndex);

/*********************************************************************
*
*       Simulation (Segger internal use only)
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)

  extern GUI_PORT_API SIM_FlexColor_HW_API;

  void SIM_FlexColor_Config      (GUI_DEVICE * pDevice, int Orientation, int xSize, int ySize, int FirstSEG, int FirstCOM, int BitsPerPixel, int NumDummyReads);
  void SIM_FlexColor_SetBus8     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus16    (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus9     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66708(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66709(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66712(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66714(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66718(GUI_DEVICE * pDevice);

#endif

#endif /* GUIDRV_FLEXCOLOR_PRIVATE_H */

/*************************** End of file ****************************/
