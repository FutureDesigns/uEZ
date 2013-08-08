#include <stdlib.h>
#include <string.h>

#ifndef _WINDOWS
  #include "BSP.h"
#endif

#include "GUI.h"
#include "DIALOG.h"
#include "GRAPH.h"
#include <Config_Build.h>

extern void FDI_emWin_Demo(const T_choice *aChoice);

void MainTask(void);
void MainTask(void)
{
#if GUI_SUPPORT_MEMDEV
    WM_SetCreateFlags(WM_CF_MEMDEV);
#endif

    FDI_emWin_Demo(NULL);
}