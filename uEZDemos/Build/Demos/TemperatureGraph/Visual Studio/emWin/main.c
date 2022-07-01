#include <stdlib.h>
#include <string.h>

#ifndef _WINDOWS
  #include "BSP.h"
#endif

#include "GUI.h"
#include "DIALOG.h"
#include "GRAPH.h"
#include <Config_Build.h>
//#include <emWin/WindowManager.h>

TUInt8 G_frameBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT * GUI_PIXEL_WIDTH * GUI_NUM_VIRTUAL_DISPLAYS];

GUI_CONTEXT      GUI_Context;
extern void TimeDateSettingsScreen();

extern void FDI_emWin_Demo(const T_choice *aChoice);

void MainTask(void);
void MainTask(void)
{
#if GUI_SUPPORT_MEMDEV
    WM_SetCreateFlags(WM_CF_MEMDEV);
#endif

    FDI_emWin_Demo(NULL);
	WindowManager_Start_emWin();
	WindowManager_Create_All_Active_Windows();

	//TimeDateSettingsScreen();

	WindowManager_Show_Window(HOME_SCREEN);
	GUI_ExecCreatedDialog(G_SystemWindows[HOME_SCREEN]);
}
