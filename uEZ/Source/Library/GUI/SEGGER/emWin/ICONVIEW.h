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
File        : ICONVIEW.h
Purpose     : ICONVIEW include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef ICONVIEW_H
#define ICONVIEW_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Status- and create flags
//
#define ICONVIEW_CF_AUTOSCROLLBAR_V (1 << 1)
#define ICONVIEW_SF_AUTOSCROLLBAR_V ICONVIEW_CF_AUTOSCROLLBAR_V

//
// Color indices
//
#define ICONVIEW_CI_BK              0
#define ICONVIEW_CI_UNSEL           0
#define ICONVIEW_CI_SEL             1
#define ICONVIEW_CI_DISABLED        2

//
// Icon alignment flags, horizontal
//
#define ICONVIEW_IA_HCENTER         (0 << 0)
#define ICONVIEW_IA_LEFT            (1 << 0)
#define ICONVIEW_IA_RIGHT           (2 << 0)

//
// Icon alignment flags, vertical
//
#define ICONVIEW_IA_VCENTER         (0 << 2)
#define ICONVIEW_IA_BOTTOM          (1 << 2)
#define ICONVIEW_IA_TOP             (2 << 2)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM ICONVIEW_Handle;

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
ICONVIEW_Handle ICONVIEW_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t xSizeItems, int32_t ySizeItems);
ICONVIEW_Handle ICONVIEW_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t xSizeItems, int32_t ySizeItems, int32_t NumExtraBytes);
ICONVIEW_Handle ICONVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);

int32_t              ICONVIEW_AddBitmapItem           (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText);
int32_t              ICONVIEW_AddBMPItem              (ICONVIEW_Handle hObj, const U8 * pBMP, const char * pText);
int32_t              ICONVIEW_AddBMPItemEx            (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, const char * pText);
int32_t              ICONVIEW_AddStreamedBitmapItem   (ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText);
void             ICONVIEW_DeleteItem              (ICONVIEW_Handle hObj, unsigned Index);
//void             ICONVIEW_EnableStreamAuto        (void);
GUI_COLOR        ICONVIEW_GetBkColor              (ICONVIEW_Handle hObj, int32_t Index);
const GUI_FONT * ICONVIEW_GetFont                 (ICONVIEW_Handle hObj);
U32              ICONVIEW_GetItemUserData         (ICONVIEW_Handle hObj, int32_t Index);
int32_t              ICONVIEW_GetNumItems             (ICONVIEW_Handle hObj);
int32_t              ICONVIEW_GetItemText             (ICONVIEW_Handle hObj, int32_t Index, char * pBuffer, int32_t MaxSize);
int32_t              ICONVIEW_GetSel                  (ICONVIEW_Handle hObj);
GUI_COLOR        ICONVIEW_GetTextColor            (ICONVIEW_Handle hObj, int32_t Index);
int32_t              ICONVIEW_GetUserData             (ICONVIEW_Handle hObj, void * pDest, int32_t NumBytes);
GUI_BITMAP *     ICONVIEW_GetItemBitmap           (ICONVIEW_Handle hObj, int32_t ItemIndex);
int32_t              ICONVIEW_GetReleasedItem         (ICONVIEW_Handle hObj);
int32_t              ICONVIEW_InsertBitmapItem        (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText, int32_t Index);
int32_t              ICONVIEW_InsertBMPItem           (ICONVIEW_Handle hObj, const U8 * pBMP, const char * pText, int32_t Index);
int32_t              ICONVIEW_InsertBMPItemEx         (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, const char * pText, int32_t Index);
int32_t              ICONVIEW_InsertStreamedBitmapItem(ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText, int32_t Index);
int32_t              ICONVIEW_OwnerDraw               (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int32_t              ICONVIEW_SetBitmapItem           (ICONVIEW_Handle hObj, int32_t Index, const GUI_BITMAP * pBitmap);
void             ICONVIEW_SetBkColor              (ICONVIEW_Handle hObj, int32_t Index, GUI_COLOR Color);
int32_t              ICONVIEW_SetBMPItem              (ICONVIEW_Handle hObj, const U8 * pBMP, int32_t Index);
int32_t              ICONVIEW_SetBMPItemEx            (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, int32_t Index);
void             ICONVIEW_SetFont                 (ICONVIEW_Handle hObj, const GUI_FONT * pFont);
void             ICONVIEW_SetFrame                (ICONVIEW_Handle hObj, int32_t Coord, int32_t Value);
void             ICONVIEW_SetItemText             (ICONVIEW_Handle hObj, int32_t Index, const char * pText);
void             ICONVIEW_SetItemUserData         (ICONVIEW_Handle hObj, int32_t Index, U32 UserData);
void             ICONVIEW_SetOwnerDraw            (ICONVIEW_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void             ICONVIEW_SetSel                  (ICONVIEW_Handle hObj, int32_t Sel);
void             ICONVIEW_SetSpace                (ICONVIEW_Handle hObj, int32_t Coord, int32_t Value);
int32_t              ICONVIEW_SetStreamedBitmapItem   (ICONVIEW_Handle hObj, int32_t Index, const void * pStreamedBitmap);
void             ICONVIEW_SetIconAlign            (ICONVIEW_Handle hObj, int32_t IconAlign);
void             ICONVIEW_SetTextAlign            (ICONVIEW_Handle hObj, int32_t TextAlign);
void             ICONVIEW_SetTextColor            (ICONVIEW_Handle hObj, int32_t Index, GUI_COLOR Color);
int32_t              ICONVIEW_SetUserData             (ICONVIEW_Handle hObj, const void * pSrc, int32_t NumBytes);
void             ICONVIEW_SetWrapMode             (ICONVIEW_Handle hObj, GUI_WRAPMODE WrapMode);

void             ICONVIEW_Callback                (WM_MESSAGE * pMsg);

//
// Compatibility macro
//
#define ICONVIEW_EnableStreamAuto() GUI_DrawStreamedEnableAuto()

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // ICONVIEW_H

/*************************** End of file ****************************/
