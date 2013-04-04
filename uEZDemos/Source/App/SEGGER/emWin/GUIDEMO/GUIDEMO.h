/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2011  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.08 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDEMO.h
Purpose     : Configuration file of GUIDemo
----------------------------------------------------------------------
*/

#ifndef GUIDEMO_H
#define GUIDEMO_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "GUI.h"
#include "FRAMEWIN.h"
#include "PROGBAR.h"
#include "TEXT.h"
#include "BUTTON.h"
#include "SLIDER.h"
#include "HEADER.h"
#include "GRAPH.h"
#include "ICONVIEW.h"
#include "LISTVIEW.h"

#if GUI_WINSUPPORT
  #include "WM.h"
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define CONTROL_SIZE_X  80
#define CONTROL_SIZE_Y  61
#define INFO_SIZE_Y     65
#define BUTTON_SIZE_X   32
#define BUTTON_SIZE_Y   20
#define PROGBAR_SIZE_X  66
#define PROGBAR_SIZE_Y  12
#define TEXT_SIZE_X     69
#define TEXT_SIZE_Y     7
#define SHOW_PROGBAR_AT 100
#define GUI_ID_HALT     GUI_ID_USER + 0
#define GUI_ID_NEXT     GUI_ID_USER + 1

#define BK_COLOR_0      0xFF5555
#define BK_COLOR_1      0x880000

#define NUMBYTES_NEEDED 0x200000

#define CIRCLE_RADIUS   100

/*********************************************************************
*
*       Configuration of modules to be used
*
**********************************************************************
*/
#if 0 // Show all demos
#ifndef   SHOW_GUIDEMO_AUTOMOTIVE
  #define SHOW_GUIDEMO_AUTOMOTIVE        (0)
#endif
#ifndef   SHOW_GUIDEMO_BARGRAPH
  #define SHOW_GUIDEMO_BARGRAPH          (1)
#endif
#ifndef   SHOW_GUIDEMO_BITMAP
  #define SHOW_GUIDEMO_BITMAP            (1)
#endif
#ifndef   SHOW_GUIDEMO_COLORBAR
  #define SHOW_GUIDEMO_COLORBAR          (1)
#endif
#ifndef   SHOW_GUIDEMO_CURSOR
  #define SHOW_GUIDEMO_CURSOR            (1)
#endif
#ifndef   SHOW_GUIDEMO_FADING
  #define SHOW_GUIDEMO_FADING            (1)
#endif
#ifndef   SHOW_GUIDEMO_GRAPH
  #define SHOW_GUIDEMO_GRAPH             (1)
#endif
#ifndef   SHOW_GUIDEMO_ICONVIEW
  #define SHOW_GUIDEMO_ICONVIEW          (1)
#endif
#ifndef   SHOW_GUIDEMO_IMAGEFLOW
  #define SHOW_GUIDEMO_IMAGEFLOW         (1)
#endif
#ifndef   SHOW_GUIDEMO_LISTVIEW
  #define SHOW_GUIDEMO_LISTVIEW          (1)
#endif
#ifndef   SHOW_GUIDEMO_SKINNING
  #define SHOW_GUIDEMO_SKINNING          (1)
#endif
#ifndef   SHOW_GUIDEMO_SPEED
  #define SHOW_GUIDEMO_SPEED             (1)
#endif
#ifndef   SHOW_GUIDEMO_SPEEDOMETER
  #define SHOW_GUIDEMO_SPEEDOMETER       (1)
#endif
#ifndef   SHOW_GUIDEMO_TRANSPARENTDIALOG
  #define SHOW_GUIDEMO_TRANSPARENTDIALOG (1)
#endif
#ifndef   SHOW_GUIDEMO_TREEVIEW
  #define SHOW_GUIDEMO_TREEVIEW          (1)
#endif
#ifndef   SHOW_GUIDEMO_VSCREEN
  #define SHOW_GUIDEMO_VSCREEN           (1)
#endif
#ifndef   SHOW_GUIDEMO_WASHINGMACHINE
  #define SHOW_GUIDEMO_WASHINGMACHINE    (1)
#endif
#ifndef   SHOW_GUIDEMO_ZOOMANDROTATE
  #define SHOW_GUIDEMO_ZOOMANDROTATE     (1)
#endif
#else // Choose a demo
#ifndef   SHOW_GUIDEMO_AUTOMOTIVE
  #define SHOW_GUIDEMO_AUTOMOTIVE        (0)
#endif
#ifndef   SHOW_GUIDEMO_BITMAP
  #define SHOW_GUIDEMO_BITMAP            (0)
#endif
#ifndef   SHOW_GUIDEMO_COLORBAR
  #define SHOW_GUIDEMO_COLORBAR          (0)
#endif
#ifndef   SHOW_GUIDEMO_CURSOR
  #define SHOW_GUIDEMO_CURSOR            (0)
#endif
#ifndef   SHOW_GUIDEMO_FADING
  #define SHOW_GUIDEMO_FADING            (0)
#endif
#ifndef   SHOW_GUIDEMO_GRAPH
  #define SHOW_GUIDEMO_GRAPH             (1)
#endif
#ifndef   SHOW_GUIDEMO_ICONVIEW
  #define SHOW_GUIDEMO_ICONVIEW          (0)
#endif
#ifndef   SHOW_GUIDEMO_IMAGEFLOW
  #define SHOW_GUIDEMO_IMAGEFLOW         (0)
#endif
#ifndef   SHOW_GUIDEMO_LISTVIEW
  #define SHOW_GUIDEMO_LISTVIEW          (1)
#endif
#ifndef   SHOW_GUIDEMO_SKINNING
  #define SHOW_GUIDEMO_SKINNING          (0)
#endif
#ifndef   SHOW_GUIDEMO_SPEED
  #define SHOW_GUIDEMO_SPEED             (1)
#endif
#ifndef   SHOW_GUIDEMO_SPEEDOMETER
  #define SHOW_GUIDEMO_SPEEDOMETER       (0)//crashes
#endif
#ifndef   SHOW_GUIDEMO_TRANSPARENTDIALOG
  #define SHOW_GUIDEMO_TRANSPARENTDIALOG (0)
#endif
#ifndef   SHOW_GUIDEMO_TREEVIEW
  #define SHOW_GUIDEMO_TREEVIEW          (0)
#endif
#ifndef   SHOW_GUIDEMO_VSCREEN
  #define SHOW_GUIDEMO_VSCREEN           (0)
#endif
#ifndef   SHOW_GUIDEMO_WASHINGMACHINE
  #define SHOW_GUIDEMO_WASHINGMACHINE    (0)
#endif
#ifndef   SHOW_GUIDEMO_ZOOMANDROTATE
  #define SHOW_GUIDEMO_ZOOMANDROTATE     (0)
#endif
#endif

/*********************************************************************
*
*       Internal functions
*
**********************************************************************
*/
void    GUIDEMO_AddIntToString   (char * acText, unsigned int Number);
void    GUIDEMO_AddStringToString(char * acText, const char * acAdd);
int     GUIDEMO_CheckCancel      (void);
void    GUIDEMO_ClearText        (char * acText);
void    GUIDEMO_Delay            (int t);
void    GUIDEMO_DrawBk           (int DrawLogo);
int     GUIDEMO_GetTime          (void);
void    GUIDEMO_HideControlWin   (void);
void    GUIDEMO_HideInfoWin      (void);
void    GUIDEMO_NotifyStartNext  (void);
void    GUIDEMO_ShowControlWin   (void);
void    GUIDEMO_ShowInfo         (const char * acInfo);
void    GUIDEMO_ShowInfoWin      (void);
void    GUIDEMO_ShowIntro        (const char * acText, const char * acDescription);
void    GUIDEMO_UpdateControlText(void);
void    GUIDEMO_Wait             (int TimeWait);

/*********************************************************************
*
*       Demo modules
*
**********************************************************************
*/
void GUIDEMO_Main             (void);
void GUIDEMO_Automotive       (void);
void GUIDEMO_Bitmap           (void);
void GUIDEMO_ColorBar         (void);
void GUIDEMO_Cursor           (void);
void GUIDEMO_Fading           (void);
void GUIDEMO_Graph            (void);
void GUIDEMO_IconView         (void);
void GUIDEMO_ImageFlow        (void);
void GUIDEMO_Intro            (void);
void GUIDEMO_Listview         (void);
void GUIDEMO_Skinning         (void);
void GUIDEMO_Speed            (void);
void GUIDEMO_Speedometer      (void);
void GUIDEMO_TransparentDialog(void);
void GUIDEMO_Treeview         (void);
void GUIDEMO_VScreen          (void);
void GUIDEMO_WashingMachine   (void);
void GUIDEMO_ZoomAndRotate    (void);

/*********************************************************************
*
*       Configuration macros
*
**********************************************************************
*/
#ifndef   GUIDEMO_USE_VNC
  #define GUIDEMO_USE_VNC         (0)
#endif
#ifndef   GUIDEMO_SHOW_SPRITES
  #define GUIDEMO_SHOW_SPRITES    (0)
#endif

/*********************************************************************
*
*       Externs
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmSeggerLogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmSeggerLogo70x35;

extern GUI_CONST_STORAGE GUI_FONT   GUI_FontRounded16;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontRounded22;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontSouvenir18;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontD6x8;


#if defined(__cplusplus)
  }
#endif

#endif // avoid multiple inclusion
