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
File        : EDIT.h
Purpose     : EDIT include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef EDIT_H
#define EDIT_H

#include "WM.h"
#include "DIALOG_Intern.h" // Required for Create indirect data structure

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {             // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defaults for configuration switches
*
*  The following are defaults for config switches which affect the
*  interface specified in this module
*
**********************************************************************
*/
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Create / Status flags
//
#define EDIT_CF_LEFT    GUI_TA_LEFT
#define EDIT_CF_RIGHT   GUI_TA_RIGHT
#define EDIT_CF_HCENTER GUI_TA_HCENTER
#define EDIT_CF_VCENTER GUI_TA_VCENTER
#define EDIT_CF_TOP     GUI_TA_TOP
#define EDIT_CF_BOTTOM  GUI_TA_BOTTOM

//
// Color indices
//
#define EDIT_CI_DISABLED 0
#define EDIT_CI_ENABLED  1
#define EDIT_CI_CURSOR   2

//
// Signed or normal mode
//
#define GUI_EDIT_NORMAL                  (0 << 0)
#define GUI_EDIT_SIGNED                  (1 << 0)
#define GUI_EDIT_SUPPRESS_LEADING_ZEROES (1 << 1)

//
// Cursor coloring
//
#define GUI_EDIT_SHOWCURSOR              (1 << 2)
#define GUI_EDIT_CUSTCOLORMODE           (1 << 3)

//
// Edit modes
//
#define GUI_EDIT_MODE_INSERT    0
#define GUI_EDIT_MODE_OVERWRITE 1

//
// Compatibility macros
//
#define EDIT_CI_DISABELD EDIT_CI_DISABLED
#define EDIT_CI_ENABELD  EDIT_CI_ENABLED

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM EDIT_Handle;
typedef void tEDIT_AddKeyEx    (EDIT_Handle hObj, int32_t Key);
typedef void tEDIT_UpdateBuffer(EDIT_Handle hObj);

/*********************************************************************
*
*             Create functions
*/
EDIT_Handle EDIT_Create        (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Id, int32_t MaxLen, int32_t Flags);
EDIT_Handle EDIT_CreateAsChild (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t Id, int32_t Flags, int32_t MaxLen);
EDIT_Handle EDIT_CreateEx      (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t MaxLen);
EDIT_Handle EDIT_CreateUser    (int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, WM_HWIN hParent, int32_t WinFlags, int32_t ExFlags, int32_t Id, int32_t MaxLen, int32_t NumExtraBytes);
EDIT_Handle EDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int32_t x0, int32_t y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void EDIT_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
void EDIT_SetDefaultBkColor  (uint32_t Index, GUI_COLOR Color);
void EDIT_SetDefaultFont     (const GUI_FONT * pFont);
void EDIT_SetDefaultTextAlign(int32_t Align);
void EDIT_SetDefaultTextColor(uint32_t Index, GUI_COLOR Color);

/*********************************************************************
*
*             Individual member functions
*/
//
// Query preferences
//
GUI_COLOR        EDIT_GetDefaultBkColor(uint32_t Index);
const GUI_FONT * EDIT_GetDefaultFont(void);
int32_t              EDIT_GetDefaultTextAlign(void);
GUI_COLOR        EDIT_GetDefaultTextColor(uint32_t Index);
//
// Methods changing properties
//
void EDIT_AddKey           (EDIT_Handle hObj, int32_t Key);
void EDIT_EnableBlink      (EDIT_Handle hObj, int32_t Period, int32_t OnOff);
GUI_COLOR EDIT_GetBkColor  (EDIT_Handle hObj, uint32_t Index);
void EDIT_SetBkColor       (EDIT_Handle hObj, uint32_t Index, GUI_COLOR color);
void EDIT_SetCursorAtChar  (EDIT_Handle hObj, int32_t Pos);
void EDIT_SetCursorAtPixel (EDIT_Handle hObj, int32_t xPos);
void EDIT_SetFont          (EDIT_Handle hObj, const GUI_FONT * pFont);
int32_t  EDIT_SetInsertMode    (EDIT_Handle hObj, int32_t OnOff);
void EDIT_SetMaxLen        (EDIT_Handle hObj, int32_t MaxLen);
void EDIT_SetpfAddKeyEx    (EDIT_Handle hObj, tEDIT_AddKeyEx * pfAddKeyEx);
void EDIT_SetpfUpdateBuffer(EDIT_Handle hObj, tEDIT_UpdateBuffer * pfUpdateBuffer);
void EDIT_SetText          (EDIT_Handle hObj, const char * s);
void EDIT_SetTextAlign     (EDIT_Handle hObj, int32_t Align);
GUI_COLOR EDIT_GetTextColor(EDIT_Handle hObj, uint32_t Index);
void EDIT_SetTextColor     (EDIT_Handle hObj, uint32_t Index, GUI_COLOR Color);
void EDIT_SetSel           (EDIT_Handle hObj, int32_t FirstChar, int32_t LastChar);
int32_t  EDIT_SetUserData      (EDIT_Handle hObj, const void * pSrc, int32_t NumBytes);
int32_t  EDIT_EnableInversion  (EDIT_Handle hObj, int32_t OnOff);
//
// Get/Set user input
//
int32_t   EDIT_GetCursorCharPos  (EDIT_Handle hObj);
void  EDIT_GetCursorPixelPos (EDIT_Handle hObj, int32_t * pxPos, int32_t * pyPos);
float EDIT_GetFloatValue     (EDIT_Handle hObj);
const GUI_FONT * EDIT_GetFont(EDIT_Handle hObj);
int32_t   EDIT_GetNumChars       (EDIT_Handle hObj);
void  EDIT_GetText           (EDIT_Handle hObj, char * sDest, int32_t MaxLen);
int32_t   EDIT_GetTextAlign      (EDIT_Handle hObj);
I32   EDIT_GetValue          (EDIT_Handle hObj);
void  EDIT_SetFloatValue     (EDIT_Handle hObj, float Value);
int32_t   EDIT_GetUserData       (EDIT_Handle hObj, void * pDest, int32_t NumBytes);
void  EDIT_SetValue          (EDIT_Handle hObj, I32 Value);

#define EDIT_SetFocussable EDIT_SetFocusable
#define EDIT_SetFocusable  WIDGET_SetFocusable

/*********************************************************************
*
*             Routines for editing values
*
**********************************************************************
*/
void  EDIT_SetHexMode  (EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);
void  EDIT_SetBinMode  (EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);
void  EDIT_SetDecMode  (EDIT_Handle hEdit, I32 Value, I32 Min, I32 Max, int32_t Shift, U8 Flags);
void  EDIT_SetFloatMode(EDIT_Handle hEdit, float Value, float Min, float Max, int32_t Shift, U8 Flags);
void  EDIT_SetTextMode (EDIT_Handle hEdit);
void  EDIT_SetUlongMode(EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);

U32   GUI_EditHex      (U32 Value, U32 Min, U32 Max, int32_t Len, int32_t xSize);
U32   GUI_EditBin      (U32 Value, U32 Min, U32 Max, int32_t Len, int32_t xSize);
I32   GUI_EditDec      (I32 Value, I32 Min, I32 Max, int32_t Len, int32_t xSize, int32_t Shift, U8 Flags);
float GUI_EditFloat    (float Value, float Min, float Max, int32_t Len, int32_t xSize, int32_t Shift, U8 Flags);
void  GUI_EditString   (char * pString, int32_t Len, int32_t xSize);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // EDIT_H

/*************************** End of file ****************************/
