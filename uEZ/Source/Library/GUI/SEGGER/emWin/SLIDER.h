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
File        : SLIDER.h
Purpose     : SLIDER include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef SLIDER_H
#define SLIDER_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/************************************************************
*
*       Defines
*
*************************************************************
*/
/************************************************************
*
*       States
*/
#define SLIDER_STATE_PRESSED    WIDGET_STATE_USER0

/************************************************************
*
*       Create / Status flags
*/
#define SLIDER_CF_HORIZONTAL 0
#define SLIDER_CF_VERTICAL   WIDGET_CF_VERTICAL

/************************************************************
*
*       Skinning property indices
*/
#define SLIDER_SKINFLEX_PI_PRESSED   0
#define SLIDER_SKINFLEX_PI_UNPRESSED 1

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM SLIDER_Handle;

typedef struct {
  GUI_COLOR aColorFrame[2];
  GUI_COLOR aColorInner[2];
  GUI_COLOR aColorShaft[3];
  GUI_COLOR ColorTick;
  GUI_COLOR ColorFocus;
  int32_t TickSize;
  int32_t ShaftSize;
} SLIDER_SKINFLEX_PROPS;

typedef struct {
  int32_t Width;
  int32_t NumTicks;
  int32_t Size;
  int32_t IsPressed;
  int32_t IsVertical;
} SLIDER_SKINFLEX_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
SLIDER_Handle SLIDER_Create        (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t Id, int32_t WinFlags, int32_t SpecialFlags);
SLIDER_Handle SLIDER_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id);
SLIDER_Handle SLIDER_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t NumExtraBytes);
SLIDER_Handle SLIDER_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void SLIDER_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void      SLIDER_Dec            (SLIDER_Handle hObj);
void      SLIDER_EnableFocusRect(SLIDER_Handle hObj, int32_t OnOff);
GUI_COLOR SLIDER_GetBarColor    (SLIDER_Handle hObj);
GUI_COLOR SLIDER_GetBkColor     (SLIDER_Handle hObj);
U8        SLIDER_GetFlag        (SLIDER_Handle hObj, U8 Flag);
GUI_COLOR SLIDER_GetFocusColor  (SLIDER_Handle hObj);
void      SLIDER_GetRange       (SLIDER_Handle hObj, int32_t * pMin, int32_t * pMax);
GUI_COLOR SLIDER_GetTickColor   (SLIDER_Handle hObj);
int32_t       SLIDER_GetUserData    (SLIDER_Handle hObj, void * pDest, int32_t NumBytes);
int32_t       SLIDER_GetValue       (SLIDER_Handle hObj);
void      SLIDER_Inc            (SLIDER_Handle hObj);
void      SLIDER_SetBarColor    (SLIDER_Handle hObj, GUI_COLOR Color);
void      SLIDER_SetBkColor     (SLIDER_Handle hObj, GUI_COLOR Color);
GUI_COLOR SLIDER_SetFocusColor  (SLIDER_Handle hObj, GUI_COLOR Color);
void      SLIDER_SetNumTicks    (SLIDER_Handle hObj, int32_t NumTicks);
void      SLIDER_SetRange       (SLIDER_Handle hObj, int32_t Min, int32_t Max);
void      SLIDER_SetTickColor   (SLIDER_Handle hObj, GUI_COLOR Color);
int32_t       SLIDER_SetUserData    (SLIDER_Handle hObj, const void * pSrc, int32_t NumBytes);
void      SLIDER_SetValue       (SLIDER_Handle hObj, int32_t v);
void      SLIDER_SetWidth       (SLIDER_Handle hObj, int32_t Width);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void SLIDER_GetSkinFlexProps     (SLIDER_SKINFLEX_PROPS * pProps, int32_t Index);
void SLIDER_SetSkinClassic       (SLIDER_Handle hObj);
void SLIDER_SetSkin              (SLIDER_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int32_t  SLIDER_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void SLIDER_SetSkinFlexProps     (const SLIDER_SKINFLEX_PROPS * pProps, int32_t Index);
void SLIDER_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * SLIDER_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define SLIDER_SKIN_FLEX    SLIDER_DrawSkinFlex

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR SLIDER_GetDefaultBkColor   (void);
GUI_COLOR SLIDER_GetDefaultBarColor  (void);
GUI_COLOR SLIDER_GetDefaultFocusColor(void);
GUI_COLOR SLIDER_GetDefaultTickColor (void);
void      SLIDER_SetDefaultBkColor   (GUI_COLOR Color);
void      SLIDER_SetDefaultBarColor  (GUI_COLOR Color);
GUI_COLOR SLIDER_SetDefaultFocusColor(GUI_COLOR Color);
void      SLIDER_SetDefaultTickColor (GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SLIDER_H

/*************************** End of file ****************************/
