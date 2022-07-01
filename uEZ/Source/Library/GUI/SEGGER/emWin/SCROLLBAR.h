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
File        : SCROLLBAR.h
Purpose     : SCROLLBAR include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef SCROLLBAR_H
#define SCROLLBAR_H

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
#define SCROLLBAR_CI_THUMB 0
#define SCROLLBAR_CI_SHAFT 1
#define SCROLLBAR_CI_ARROW 2

/*********************************************************************
*
*       States
*/
#define SCROLLBAR_STATE_PRESSED   WIDGET_STATE_USER0

/*********************************************************************
*
*       Create / Status flags
*/
#define SCROLLBAR_CF_VERTICAL     WIDGET_CF_VERTICAL
#define SCROLLBAR_CF_FOCUSABLE    WIDGET_STATE_FOCUSABLE

#define SCROLLBAR_CF_FOCUSSABLE   SCROLLBAR_CF_FOCUSABLE

/************************************************************
*
*       Skinning property indices
*/
#define SCROLLBAR_SKINFLEX_PI_PRESSED   0
#define SCROLLBAR_SKINFLEX_PI_UNPRESSED 1

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM SCROLLBAR_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorUpper[2];
  GUI_COLOR aColorLower[2];
  GUI_COLOR aColorShaft[2];
  GUI_COLOR ColorArrow;
  GUI_COLOR ColorGrasp;
} SCROLLBAR_SKINFLEX_PROPS;

typedef struct {
  int32_t IsVertical;
  int32_t State;
} SCROLLBAR_SKINFLEX_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
SCROLLBAR_Handle SCROLLBAR_Create        (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t Id, int32_t WinFlags, int32_t SpecialFlags);
SCROLLBAR_Handle SCROLLBAR_CreateAttached(WM_HWIN hParent, int32_t SpecialFlags);
SCROLLBAR_Handle SCROLLBAR_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id);
SCROLLBAR_Handle SCROLLBAR_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t NumExtraBytes);
SCROLLBAR_Handle SCROLLBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void SCROLLBAR_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

/* Methods changing properties */

void      SCROLLBAR_AddValue   (SCROLLBAR_Handle hObj, int32_t Add);
void      SCROLLBAR_Dec        (SCROLLBAR_Handle hObj);
void      SCROLLBAR_Inc        (SCROLLBAR_Handle hObj);
int32_t       SCROLLBAR_GetUserData(SCROLLBAR_Handle hObj, void * pDest, int32_t NumBytes);
GUI_COLOR SCROLLBAR_SetColor   (SCROLLBAR_Handle hObj, int32_t Index, GUI_COLOR Color);
void      SCROLLBAR_SetNumItems(SCROLLBAR_Handle hObj, int32_t NumItems);
void      SCROLLBAR_SetPageSize(SCROLLBAR_Handle hObj, int32_t PageSize);
void      SCROLLBAR_SetValue   (SCROLLBAR_Handle hObj, int32_t v);
int32_t       SCROLLBAR_SetWidth   (SCROLLBAR_Handle hObj, int32_t Width);
void      SCROLLBAR_SetState   (SCROLLBAR_Handle hObj, const WM_SCROLL_STATE* pState);
int32_t       SCROLLBAR_SetUserData(SCROLLBAR_Handle hObj, const void * pSrc, int32_t NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void SCROLLBAR_GetSkinFlexProps     (SCROLLBAR_SKINFLEX_PROPS * pProps, int32_t Index);
void SCROLLBAR_SetSkinClassic       (SCROLLBAR_Handle hObj);
void SCROLLBAR_SetSkin              (SCROLLBAR_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int32_t  SCROLLBAR_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void SCROLLBAR_SetSkinFlexProps     (const SCROLLBAR_SKINFLEX_PROPS * pProps, int32_t Index);
void SCROLLBAR_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * SCROLLBAR_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define SCROLLBAR_SKIN_FLEX    SCROLLBAR_DrawSkinFlex

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
int32_t       SCROLLBAR_GetDefaultWidth(void);
GUI_COLOR SCROLLBAR_SetDefaultColor(GUI_COLOR Color, uint32_t Index); /* Not yet documented */
int32_t       SCROLLBAR_SetDefaultWidth(int32_t DefaultWidth);

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
int32_t       SCROLLBAR_GetThumbSizeMin(void);
int32_t       SCROLLBAR_SetThumbSizeMin(int32_t ThumbSizeMin);

/*********************************************************************
*
*       Query state
*
**********************************************************************
*/
GUI_COLOR SCROLLBAR_GetColor   (SCROLLBAR_Handle hObj, int32_t Index);
int32_t       SCROLLBAR_GetNumItems(SCROLLBAR_Handle hObj);
int32_t       SCROLLBAR_GetPageSize(SCROLLBAR_Handle hObj);
int32_t       SCROLLBAR_GetValue   (SCROLLBAR_Handle hObj);

/*********************************************************************
*
*       Macros for compatibility
*
**********************************************************************
*/
#define SCROLLBAR_BKCOLOR0_DEFAULT SCROLLBAR_COLOR_ARROW_DEFAULT
#define SCROLLBAR_BKCOLOR1_DEFAULT SCROLLBAR_COLOR_SHAFT_DEFAULT
#define SCROLLBAR_COLOR0_DEFAULT   SCROLLBAR_COLOR_THUMB_DEFAULT

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SCROLLBAR_H

/*************************** End of file ****************************/
