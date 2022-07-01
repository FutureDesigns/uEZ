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
File        : LISTWHEEL.h
Purpose     : LISTWHEEL widget include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef LISTWHEEL_H
#define LISTWHEEL_H

#include "WM.h"
#include "DIALOG_Intern.h"
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {        // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LISTWHEEL_CI_UNSEL 0
#define LISTWHEEL_CI_SEL   1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM LISTWHEEL_Handle;

/*********************************************************************
*
*       Standard member functions
*
**********************************************************************
*/
/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
LISTWHEEL_Handle LISTWHEEL_Create        (const GUI_ConstString * ppText, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Flags);
LISTWHEEL_Handle LISTWHEEL_CreateAsChild (const GUI_ConstString * ppText, WM_HWIN hWinParent, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Flags);
LISTWHEEL_Handle LISTWHEEL_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);
LISTWHEEL_Handle LISTWHEEL_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent,
                                          int32_t WinFlags, int32_t ExFlags, int32_t Id, const GUI_ConstString * ppText);
LISTWHEEL_Handle LISTWHEEL_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent,
                                          int32_t WinFlags, int32_t ExFlags, int32_t Id, const GUI_ConstString * ppText, int32_t NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void LISTWHEEL_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void      LISTWHEEL_AddString      (LISTWHEEL_Handle hObj, const char * s);
GUI_COLOR LISTWHEEL_GetBkColor     (LISTWHEEL_Handle hObj, uint32_t Index);
void *    LISTWHEEL_GetItemData    (LISTWHEEL_Handle hObj, unsigned Index); /* not to be documented */
void      LISTWHEEL_GetItemText    (LISTWHEEL_Handle hObj, unsigned Index, char * pBuffer, int32_t MaxSize);
int32_t       LISTWHEEL_GetItemFromPos (LISTWHEEL_Handle hObj, int32_t yPos);
int32_t       LISTWHEEL_GetLBorder     (LISTWHEEL_Handle hObj);
unsigned  LISTWHEEL_GetLineHeight  (LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetNumItems    (LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetPos         (LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetRBorder     (LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetSel         (LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetSnapPosition(LISTWHEEL_Handle hObj);
int32_t       LISTWHEEL_GetTextAlign   (LISTWHEEL_Handle hObj);
GUI_COLOR LISTWHEEL_GetTextColor   (LISTWHEEL_Handle hObj, uint32_t Index);
int32_t       LISTWHEEL_GetUserData    (LISTWHEEL_Handle hObj, void * pDest, int32_t NumBytes);
int32_t       LISTWHEEL_IsMoving       (LISTWHEEL_Handle hObj);
void      LISTWHEEL_MoveToPos      (LISTWHEEL_Handle hObj, uint32_t Index);
int32_t       LISTWHEEL_OwnerDraw      (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void      LISTWHEEL_SetBkColor     (LISTWHEEL_Handle hObj, uint32_t Index, GUI_COLOR Color);
void      LISTWHEEL_SetDeceleration(LISTWHEEL_Handle hObj, unsigned Deceleration);
void      LISTWHEEL_SetFont        (LISTWHEEL_Handle hObj, const GUI_FONT * pFont);
void      LISTWHEEL_SetItemData    (LISTWHEEL_Handle hObj, unsigned Index, void * pData); /* not to be documented */
void      LISTWHEEL_SetLBorder     (LISTWHEEL_Handle hObj, unsigned BorderSize);
void      LISTWHEEL_SetLineHeight  (LISTWHEEL_Handle hObj, unsigned LineHeight);
void      LISTWHEEL_SetOwnerDraw   (LISTWHEEL_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfOwnerDraw);
void      LISTWHEEL_SetPos         (LISTWHEEL_Handle hObj, uint32_t Index);
void      LISTWHEEL_SetRBorder     (LISTWHEEL_Handle hObj, unsigned BorderSize);
void      LISTWHEEL_SetSel         (LISTWHEEL_Handle hObj, int32_t Sel);
void      LISTWHEEL_SetSnapPosition(LISTWHEEL_Handle hObj, int32_t SnapPosition);
void      LISTWHEEL_SetText        (LISTWHEEL_Handle hObj, const GUI_ConstString * ppText);
void      LISTWHEEL_SetTextAlign   (LISTWHEEL_Handle hObj, int32_t Align);
void      LISTWHEEL_SetTextColor   (LISTWHEEL_Handle hObj, uint32_t Index, GUI_COLOR Color);
void      LISTWHEEL_SetTimerPeriod (LISTWHEEL_Handle hObj, GUI_TIMER_TIME TimerPeriod);
int32_t       LISTWHEEL_SetUserData    (LISTWHEEL_Handle hObj, const void * pSrc, int32_t NumBytes);
void      LISTWHEEL_SetVelocity    (LISTWHEEL_Handle hObj, int32_t Velocity);

const GUI_FONT * LISTWHEEL_GetFont(LISTWHEEL_Handle hObj);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // LISTWHEEL_H

/*************************** End of file ****************************/
