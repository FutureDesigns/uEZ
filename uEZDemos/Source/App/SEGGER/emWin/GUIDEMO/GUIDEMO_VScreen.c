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
File        : GUIDEMO_VScreen.c
Purpose     : Virtual screen demo
----------------------------------------------------------------------
*/

#include <stdlib.h>

#include "GUIDEMO.h"

#if (SHOW_GUIDEMO_VSCREEN)

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Loop
*/
static int _Loop(int y, int d, int n, int Delay) {
  do {
    y += d;
    GUI_SetOrg(0, y);
    GUI_Delay(Delay);
    if (GUIDEMO_CheckCancel()) {
      return 1;
    }
  } while (--n);
  return 0;
}

/*********************************************************************
*
*       _DemoVScreen
*/
static void _DemoVScreen(void) {
  int xSize, ySize, j, n;
  int _aDelay[] = { 20, 5};
  GUI_RECT Rect;
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  GUI_SetFont(&GUI_FontComic24B_ASCII);
  GUI_SetTextMode(GUI_TM_TRANS);
  Rect.x0 = 0;
  Rect.y0 = 0;
  Rect.x1 = xSize - 1;
  Rect.y1 = ySize - 1;
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Default screen", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  GUI_Delay(250);
  Rect.y0 += ySize;
  Rect.y1 += ySize;
  GUI_DrawGradientV(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 0xFFFFFF, 0x000000);
  Rect.y1 -= ySize / 3;
  GUI_SetColor(GUI_DARKGREEN);
  GUI_DispStringInRect("Here is the virtual screen", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  n = ySize / 5;
  for (j = 0; j < GUI_COUNTOF(_aDelay); j++) {
    if (_Loop(0, 5, n, _aDelay[j]) == 1) {
      return;
    }
    GUI_SetOrg(0, ySize);
    GUI_Delay(250);
    if (_Loop(ySize, -5, n, _aDelay[j]) == 1) {
      return;
    }
    GUI_SetOrg(0, 0);
    GUI_Delay(250);
    if (GUIDEMO_CheckCancel()) {
      return;
    }
  }
  for (j = 0; j < 5; j++) {
    GUI_SetOrg(0, ySize);
    GUI_Delay(200);
    GUI_SetOrg(0, 0);
    GUI_Delay(200);
    if (GUIDEMO_CheckCancel()) {
      return;
    }
  }
  GUIDEMO_Delay(1000);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDEMO_VScreen
*/
void GUIDEMO_VScreen(void) {
  int ySize, vySize;

  ySize  = LCD_GetYSize();
  vySize = LCD_GetVYSize();
  if (vySize < (ySize << 1)) {
    return;
  }
  GUIDEMO_ShowIntro("VScreen demo",
                    "Demonstrates how to use\n"
                    "virtual screens");
  GUIDEMO_DrawBk(1);
  _DemoVScreen();
  GUI_SetOrg(0, 0);
}

#else

void GUIDEMO_VScreen(void) {}

#endif

/*************************** End of file ****************************/
