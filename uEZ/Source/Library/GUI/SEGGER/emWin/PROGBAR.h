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
File        : PROGBAR.h
Purpose     : Progressbar include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef PROGBAR_H        /* Avoid multiple inclusion  */
#define PROGBAR_H

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
/*********************************************************************
*
*       Create flags
*/
#define PROGBAR_CF_HORIZONTAL (0 << 0)
#define PROGBAR_CF_VERTICAL   (1 << 0)
#define PROGBAR_CF_USER       (1 << 1)

/*********************************************************************
*
*       Skinning constants
*/
#define PROGBAR_SKINFLEX_L 0
#define PROGBAR_SKINFLEX_R 1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM PROGBAR_Handle;

typedef struct {
  GUI_COLOR aColorUpperL[2];
  GUI_COLOR aColorLowerL[2];
  GUI_COLOR aColorUpperR[2];
  GUI_COLOR aColorLowerR[2];
  GUI_COLOR ColorFrame;
  GUI_COLOR ColorText;
} PROGBAR_SKINFLEX_PROPS;

typedef struct {
  int32_t IsVertical;
  int32_t Index;
  const char * pText;
} PROGBAR_SKINFLEX_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

PROGBAR_Handle PROGBAR_Create        (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Flags);
PROGBAR_Handle PROGBAR_CreateAsChild (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t Id, int32_t Flags);
PROGBAR_Handle PROGBAR_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id);
PROGBAR_Handle PROGBAR_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t NumExtraBytes);
PROGBAR_Handle PROGBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void PROGBAR_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Individual member functions
*
**********************************************************************
*/
GUI_COLOR        PROGBAR_GetBarColor(PROGBAR_Handle hObj, uint32_t Index);
const GUI_FONT * PROGBAR_GetFont     (PROGBAR_Handle hObj);
void             PROGBAR_GetMinMax   (PROGBAR_Handle hObj, int32_t * pMin, int32_t * pMax);
int32_t              PROGBAR_GetUserData (PROGBAR_Handle hObj, void * pDest, int32_t NumBytes);
GUI_COLOR        PROGBAR_GetTextColor(PROGBAR_Handle hObj, uint32_t Index);
int32_t              PROGBAR_GetValue    (PROGBAR_Handle hObj);
void             PROGBAR_SetBarColor (PROGBAR_Handle hObj, uint32_t index, GUI_COLOR color);
void             PROGBAR_SetFont     (PROGBAR_Handle hObj, const GUI_FONT * pfont);
void             PROGBAR_SetMinMax   (PROGBAR_Handle hObj, int32_t Min, int32_t Max);
void             PROGBAR_SetText     (PROGBAR_Handle hObj, const char* s);
void             PROGBAR_SetTextAlign(PROGBAR_Handle hObj, int32_t Align);
void             PROGBAR_SetTextColor(PROGBAR_Handle hObj, uint32_t index, GUI_COLOR color);
void             PROGBAR_SetTextPos  (PROGBAR_Handle hObj, int32_t XOff, int32_t YOff);
void             PROGBAR_SetValue    (PROGBAR_Handle hObj, int32_t v);
int32_t              PROGBAR_SetUserData (PROGBAR_Handle hObj, const void * pSrc, int32_t NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void PROGBAR_GetSkinFlexProps     (PROGBAR_SKINFLEX_PROPS * pProps, int32_t Index);
void PROGBAR_SetSkinClassic       (PROGBAR_Handle hObj);
void PROGBAR_SetSkin              (PROGBAR_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int32_t  PROGBAR_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void PROGBAR_SetSkinFlexProps     (const PROGBAR_SKINFLEX_PROPS * pProps, int32_t Index);
void PROGBAR_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * PROGBAR_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define PROGBAR_SKIN_FLEX    PROGBAR_DrawSkinFlex

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // PROGBAR_H

/*************************** End of file ****************************/
