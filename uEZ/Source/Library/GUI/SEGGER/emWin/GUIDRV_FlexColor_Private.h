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
File        : GUIDRV_FlexColor_Private.h
Purpose     : Private declarations for GUIDRV_FlexColor driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_FlexColor.h"

#ifndef GUIDRV_FLEXCOLOR_PRIVATE_H
#define GUIDRV_FLEXCOLOR_PRIVATE_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef LCD_WRITE_BUFFER_SIZE
  #define LCD_WRITE_BUFFER_SIZE         500
#endif

#define FLEXCOLOR_CF_MANAGE_ORIENTATION (1 << 0)
#define FLEXCOLOR_CF_RAM_ADDR_SET       (1 << 1)
#define FLEXCOLOR_CF_SET_CURSOR         (1 << 2)

#define FLEXCOLOR_CF_DEFAULT            FLEXCOLOR_CF_RAM_ADDR_SET

#define FLEXCOLOR_MAX_NUM_DUMMY_READS   5
#define FLEXCOLOR_NUM_DUMMY_READS       1

#define PUSH_RECT 0
#define POP_RECT  1

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_FLEXCOLOR

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
  int32_t xSize, ySize;   // Display size
  int32_t vxSize, vySize; // Virtual display size
  int32_t x0, y0, x1, y1; // Current rectangle
  int32_t NumDummyReads;  // Number of required dummy reads
  U16 RegEntryMode;   // Can be used for storing additional configuration bits for 'EntryMode' register which is modified by the driver
  U16 Flags;
  LCD_PIXELINDEX IndexMask;
  int32_t FirstSEG, FirstCOM;
  int32_t Orientation;
  int32_t BitsPerPixel;
  int32_t Shift;
  GUI_DEVICE * pDevice;
  const GUI_DEVICE_API * pMemdev_API;
  //
  // Cache
  //
  void * pVRAM;
  int32_t CacheLocked;
  int32_t CacheStat;
  int32_t xPos, yPos;
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
  // Functions for writing single items (data, cmd) regardless of the interface and getting the status
  //
  U16  (* pfReadReg)     (DRIVER_CONTEXT * _pContext);
  void (* pfSetReg)      (DRIVER_CONTEXT * _pContext, U16 _Data);
  void (* pfWritePara)   (DRIVER_CONTEXT * _pContext, U16 _Data);
  void (* pfSetInterface)(DRIVER_CONTEXT * _pContext, int32_t _BusWidth);
  //
  // Setting read mode, required for RA8870
  //
  void (* pfSetReadMode)(DRIVER_CONTEXT * _pContext, int32_t OnOff, int32_t SetCursor);
  //
  // Cache related function pointers
  //
  void (* pfSendCacheRect)(DRIVER_CONTEXT * _pContext, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1);
  U32  (* pfReadData)     (DRIVER_CONTEXT * _pContext);
  void (* pfWriteData)    (DRIVER_CONTEXT * _pContext, U32 _PixelIndex);
  //
  // Controller specific routines
  //
  void (* pfSetRect)            (DRIVER_CONTEXT * _pContext, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1);
  void (* pfSetPhysRAMAddr)     (DRIVER_CONTEXT * _pContext, int32_t _x, int32_t _y);
  void (* pfSetOrientation)     (DRIVER_CONTEXT * _pContext);
  U16  (* pfReadPixel_16bpp_B16)(DRIVER_CONTEXT * _pContext);
  U16  (* pfReadPixel_16bpp_B8) (DRIVER_CONTEXT * _pContext);
  U32  (* pfReadPixel_18bpp_B9) (DRIVER_CONTEXT * _pContext);
  U32  (* pfReadPixel_18bpp_B18)(DRIVER_CONTEXT * _pContext);
  U32  (* pfReadPixel_32bpp_B8) (DRIVER_CONTEXT * _pContext);
  void (* pfReadRect_16bpp_B16) (GUI_DEVICE * _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, U16 * _pBuffer);
  void (* pfReadRect_16bpp_B8)  (GUI_DEVICE * _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, U16 * _pBuffer);
  void (* pfReadRect_18bpp_B9)  (GUI_DEVICE * _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, U32 * _pBuffer);
  void (* pfReadRect_18bpp_B18) (GUI_DEVICE * _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, U32 * _pBuffer);
  void (* pfReadRect_32bpp_B8)  (GUI_DEVICE * _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, U32 * _pBuffer);
  //
  // Custom read functions
  //
  U16 (* pfReadPixelCust_16bpp) (int32_t LayerIndex);
  U32 (* pfReadPixelCust_18bpp) (int32_t LayerIndex);
  void(* pfReadMPixelCust_16bpp)(int32_t LayerIndex, U16 * pBuffer, U32 NumPixels);
  void(* pfReadMPixelCust_18bpp)(int32_t LayerIndex, U32 * pBuffer, U32 NumPixels);
  //
  // Mode dependent drawing functions
  //
  void          (* pfDrawBitmap   )(GUI_DEVICE *  _pDevice, int32_t _x0, int32_t _y0, int32_t _xsize, int32_t _ysize, int32_t _BitsPerPixel, int32_t _BytesPerLine, const U8 * _pData, int32_t _Diff, const LCD_PIXELINDEX * _pTrans);
  void          (* pfFillRect     )(GUI_DEVICE *  _pDevice, int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1);
  LCD_PIXELINDEX(* pfGetPixelIndex)(GUI_DEVICE *  _pDevice, int32_t _x, int32_t _y);
  void          (* pfSetPixelIndex)(GUI_DEVICE *  _pDevice, int32_t _x, int32_t _y, LCD_PIXELINDEX _ColorIndex);
  int32_t           (* pfControlCache )(GUI_DEVICE *  _pDevice, int32_t _Cmd);
  void          (* pfRefresh      )(GUI_DEVICE *  _pDevice);
  //
  // Controller dependent function pointers
  //
  void (* pfReadRect)(void);
  //
  // Orientation
  //
  int32_t  (* pfLog2PhysX)(DRIVER_CONTEXT * _pContext, int32_t _x, int32_t _y);
  int32_t  (* pfLog2PhysY)(DRIVER_CONTEXT * _pContext, int32_t _x, int32_t _y);
  //
  // Function pointer for setting up pfLog2Phys<X,Y>
  //
  void (* pfSetLog2Phys)(DRIVER_CONTEXT * _pContext);
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
int32_t  GUIDRV_FlexColor__ControlCache   (GUI_DEVICE * pDevice, int32_t Cmd);
void GUIDRV_FlexColor__Refresh        (GUI_DEVICE * pDevice);

void GUIDRV_FlexColor__AddCacheRect   (DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__ManageRect     (DRIVER_CONTEXT * pContext, int32_t Cmd);
void GUIDRV_FlexColor__SetCacheAddr   (DRIVER_CONTEXT * pContext, int32_t x, int32_t y);
void GUIDRV_FlexColor__SetCacheRect   (DRIVER_CONTEXT * pContext, int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void GUIDRV_FlexColor__SetLog2Phys    (DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__SetSubRect     (DRIVER_CONTEXT * pContext, int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void GUIDRV_FlexColor__ClearCacheRect (DRIVER_CONTEXT * pContext);

LCD_PIXELINDEX GUIDRV_FlexColor__GetPixelIndexCache  (GUI_DEVICE * pDevice, int32_t x, int32_t y);
void           GUIDRV_FlexColor__SetPixelIndexCache  (GUI_DEVICE * pDevice, int32_t x, int32_t y, LCD_PIXELINDEX PixelIndex);
LCD_PIXELINDEX GUIDRV_FlexColor__GetPixelIndexNoCache(GUI_DEVICE * pDevice, int32_t x, int32_t y);
void           GUIDRV_FlexColor__SetPixelIndexNoCache(GUI_DEVICE * pDevice, int32_t x, int32_t y, LCD_PIXELINDEX PixelIndex);

void GUIDRV_FlexColor__SetFunc66712(GUI_DEVICE * pDevice, U16 AndMask_SetAddrRAM);

/*********************************************************************
*
*       Simulation (Segger internal use only)
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)

  extern GUI_PORT_API SIM_FlexColor_HW_API;

  void SIM_FlexColor_Config      (GUI_DEVICE * pDevice, int32_t Orientation, int32_t xSize, int32_t ySize, int32_t FirstSEG, int32_t FirstCOM, int32_t BitsPerPixel, int32_t NumDummyReads);
  void SIM_FlexColor_SetBus8     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus9     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus16    (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus32    (GUI_DEVICE * pDevice, int32_t Shift);
  void SIM_FlexColor_SetBus8_24  (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66702(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66708(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66709(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66712(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66714(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66715(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66718(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66719(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66720(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66721(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66722(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66723(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66724(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66772(GUI_DEVICE * pDevice);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_FLEXCOLOR_PRIVATE_H */

/*************************** End of file ****************************/
