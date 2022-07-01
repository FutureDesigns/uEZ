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
File        : DROPDOWN.h
Purpose     : Multiple choice object include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "LISTBOX.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/************************************************************
*
*       Create flags
*/
#define DROPDOWN_CF_AUTOSCROLLBAR   (1 << 0)
#define DROPDOWN_CF_UP              (1 << 1)

/*********************************************************************
*
*       Color indices
*/
#define DROPDOWN_CI_UNSEL    0
#define DROPDOWN_CI_SEL      1
#define DROPDOWN_CI_SELFOCUS 2

#define DROPDOWN_CI_ARROW    0
#define DROPDOWN_CI_BUTTON   1

/*********************************************************************
*
*       Skinning property indices
*/
#define DROPDOWN_SKINFLEX_PI_EXPANDED 0
#define DROPDOWN_SKINFLEX_PI_FOCUSED  1
#define DROPDOWN_SKINFLEX_PI_ENABLED  2
#define DROPDOWN_SKINFLEX_PI_DISABLED 3

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM DROPDOWN_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorUpper[2];
  GUI_COLOR aColorLower[2];
  GUI_COLOR ColorArrow;
  GUI_COLOR ColorText;
  GUI_COLOR ColorSep;
  int32_t Radius;
} DROPDOWN_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
DROPDOWN_Handle DROPDOWN_Create        (WM_HWIN hWinParent, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Flags);
DROPDOWN_Handle DROPDOWN_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id);
DROPDOWN_Handle DROPDOWN_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t NumExtraBytes);
DROPDOWN_Handle DROPDOWN_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK* cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void DROPDOWN_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void             DROPDOWN_AddKey           (DROPDOWN_Handle hObj, int32_t Key);
void             DROPDOWN_AddString        (DROPDOWN_Handle hObj, const char* s);
void             DROPDOWN_Collapse         (DROPDOWN_Handle hObj);
void             DROPDOWN_DecSel           (DROPDOWN_Handle hObj);
void             DROPDOWN_DecSelExp        (DROPDOWN_Handle hObj);
void             DROPDOWN_DeleteItem       (DROPDOWN_Handle hObj, uint32_t Index);
void             DROPDOWN_Expand           (DROPDOWN_Handle hObj);
GUI_COLOR        DROPDOWN_GetBkColor       (DROPDOWN_Handle hObj, uint32_t Index);
GUI_COLOR        DROPDOWN_GetColor         (DROPDOWN_Handle hObj, uint32_t Index);
const GUI_FONT * DROPDOWN_GetFont          (DROPDOWN_Handle hObj);
unsigned         DROPDOWN_GetItemDisabled  (DROPDOWN_Handle hObj, unsigned Index);
unsigned         DROPDOWN_GetItemSpacing   (DROPDOWN_Handle hObj);
int32_t              DROPDOWN_GetItemText      (DROPDOWN_Handle hObj, unsigned Index, char * pBuffer, int32_t MaxSize);
LISTBOX_Handle   DROPDOWN_GetListbox       (DROPDOWN_Handle hObj);
int32_t              DROPDOWN_GetNumItems      (DROPDOWN_Handle hObj);
int32_t              DROPDOWN_GetSel           (DROPDOWN_Handle hObj);
int32_t              DROPDOWN_GetSelExp        (DROPDOWN_Handle hObj);
GUI_COLOR        DROPDOWN_GetTextColor     (DROPDOWN_Handle hObj, uint32_t Index);
int32_t              DROPDOWN_GetUserData      (DROPDOWN_Handle hObj, void * pDest, int32_t NumBytes);
void             DROPDOWN_IncSel           (DROPDOWN_Handle hObj);
void             DROPDOWN_IncSelExp        (DROPDOWN_Handle hObj);
void             DROPDOWN_InsertString     (DROPDOWN_Handle hObj, const char* s, uint32_t Index);
void             DROPDOWN_SetAutoScroll    (DROPDOWN_Handle hObj, int32_t OnOff);
void             DROPDOWN_SetBkColor       (DROPDOWN_Handle hObj, uint32_t Index, GUI_COLOR color);
void             DROPDOWN_SetColor         (DROPDOWN_Handle hObj, uint32_t Index, GUI_COLOR Color);
void             DROPDOWN_SetFont          (DROPDOWN_Handle hObj, const GUI_FONT * pfont);
void             DROPDOWN_SetItemDisabled  (DROPDOWN_Handle hObj, unsigned Index, int32_t OnOff);
void             DROPDOWN_SetItemSpacing   (DROPDOWN_Handle hObj, unsigned Value);
int32_t              DROPDOWN_SetListHeight    (DROPDOWN_Handle hObj, unsigned Height);
void             DROPDOWN_SetScrollbarColor(DROPDOWN_Handle hObj, unsigned Index, GUI_COLOR Color);
void             DROPDOWN_SetScrollbarWidth(DROPDOWN_Handle hObj, unsigned Width);
void             DROPDOWN_SetSel           (DROPDOWN_Handle hObj, int32_t Sel);
void             DROPDOWN_SetSelExp        (DROPDOWN_Handle hObj, int32_t Sel);
void             DROPDOWN_SetTextAlign     (DROPDOWN_Handle hObj, int32_t Align);
void             DROPDOWN_SetTextColor     (DROPDOWN_Handle hObj, uint32_t index, GUI_COLOR color);
void             DROPDOWN_SetTextHeight    (DROPDOWN_Handle hObj, unsigned TextHeight);
int32_t              DROPDOWN_SetUpMode        (DROPDOWN_Handle hObj, int32_t OnOff);
int32_t              DROPDOWN_SetUserData      (DROPDOWN_Handle hObj, const void * pSrc, int32_t NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void DROPDOWN_GetSkinFlexProps     (DROPDOWN_SKINFLEX_PROPS * pProps, int32_t Index);
void DROPDOWN_SetSkinClassic       (DROPDOWN_Handle hObj);
void DROPDOWN_SetSkin              (DROPDOWN_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int32_t  DROPDOWN_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void DROPDOWN_SetSkinFlexProps     (const DROPDOWN_SKINFLEX_PROPS * pProps, int32_t Index);
void DROPDOWN_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * DROPDOWN_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define DROPDOWN_SKIN_FLEX    DROPDOWN_DrawSkinFlex

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        DROPDOWN_GetDefaultBkColor       (int32_t Index);
GUI_COLOR        DROPDOWN_GetDefaultColor         (int32_t Index);
const GUI_FONT * DROPDOWN_GetDefaultFont          (void);
GUI_COLOR        DROPDOWN_GetDefaultScrollbarColor(int32_t Index);
void             DROPDOWN_SetDefaultFont          (const GUI_FONT * pFont);
GUI_COLOR        DROPDOWN_SetDefaultBkColor       (int32_t Index, GUI_COLOR Color);
GUI_COLOR        DROPDOWN_SetDefaultColor         (int32_t Index, GUI_COLOR Color);
GUI_COLOR        DROPDOWN_SetDefaultScrollbarColor(int32_t Index, GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // DROPDOWN_H

/*************************** End of file ****************************/
