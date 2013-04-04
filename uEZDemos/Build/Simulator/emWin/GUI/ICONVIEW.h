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

** emWin V5.16 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
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
/* Status- and create flags */
#define ICONVIEW_CF_AUTOSCROLLBAR_V (1 << 1)
#define ICONVIEW_SF_AUTOSCROLLBAR_V ICONVIEW_CF_AUTOSCROLLBAR_V

/* Color indices */
#define ICONVIEW_CI_BK       0
#define ICONVIEW_CI_UNSEL    0
#define ICONVIEW_CI_SEL      1
#define ICONVIEW_CI_DISABLED 2

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
ICONVIEW_Handle ICONVIEW_CreateEx      (int x0, int y0, int xsize, int ysize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int xSizeItems, int ySizeItem);
ICONVIEW_Handle ICONVIEW_CreateUser    (int x0, int y0, int xsize, int ysize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int xSizeItems, int ySizeItems, int NumExtraBytes);
ICONVIEW_Handle ICONVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

int  ICONVIEW_AddBitmapItem           (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText);
int  ICONVIEW_AddStreamedBitmapItem   (ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText);
void ICONVIEW_DeleteItem              (ICONVIEW_Handle hObj, unsigned Index);
void ICONVIEW_EnableStreamAuto        (void);
U32  ICONVIEW_GetItemUserData         (ICONVIEW_Handle hObj, int Index);
int  ICONVIEW_GetNumItems             (ICONVIEW_Handle hObj);
int  ICONVIEW_GetItemText             (ICONVIEW_Handle hObj, int Index, char * pBuffer, int MaxSize);
int  ICONVIEW_GetSel                  (ICONVIEW_Handle hObj);
int  ICONVIEW_GetUserData             (ICONVIEW_Handle hObj, void * pDest, int NumBytes);
int  ICONVIEW_InsertBitmapItem        (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText, int Index);
int  ICONVIEW_InsertStreamedBitmapItem(ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText, int Index);
int  ICONVIEW_SetBitmapItem           (ICONVIEW_Handle hObj, int Index, const GUI_BITMAP * pBitmap);
void ICONVIEW_SetBkColor              (ICONVIEW_Handle hObj, int Index, GUI_COLOR Color);
void ICONVIEW_SetFont                 (ICONVIEW_Handle hObj, const GUI_FONT GUI_UNI_PTR * pFont);
void ICONVIEW_SetFrame                (ICONVIEW_Handle hObj, int Coord, int Value);
void ICONVIEW_SetItemText             (ICONVIEW_Handle hObj, int Index, const char * pText);
void ICONVIEW_SetItemUserData         (ICONVIEW_Handle hObj, int Index, U32 UserData);
void ICONVIEW_SetSel                  (ICONVIEW_Handle hObj, int Sel);
void ICONVIEW_SetSpace                (ICONVIEW_Handle hObj, int Coord, int Value);
int  ICONVIEW_SetStreamedBitmapItem   (ICONVIEW_Handle hObj, int Index, const void * pStreamedBitmap);
void ICONVIEW_SetTextAlign            (ICONVIEW_Handle hObj, int TextAlign);
void ICONVIEW_SetTextColor            (ICONVIEW_Handle hObj, int Index, GUI_COLOR Color);
int  ICONVIEW_SetUserData             (ICONVIEW_Handle hObj, const void * pSrc, int NumBytes);
void ICONVIEW_SetWrapMode             (ICONVIEW_Handle hObj, GUI_WRAPMODE WrapMode);

void ICONVIEW_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif   /* if GUI_WINSUPPORT */
#endif   /* ICONVIEW_H */

/*************************** End of file ****************************/
