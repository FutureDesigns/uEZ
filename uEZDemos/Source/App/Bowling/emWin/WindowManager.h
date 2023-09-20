/*-------------------------------------------------------------------------*
 * File:  WindowManager.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <GUI.h>
#include <Dialog.h>
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define WM_APP_GAINED_FOCUS         (WM_USER + 0x1000)
#define WM_APP_LOST_FOCUS           (WM_USER + 0x1001)

#define HIDE_ALL                    (0xFF)

#define MAIN_SCREEN         1
#define QUICK_START         2
#define STANDARD_PLAY       3
#define BOWLING             4

#define NUM_WINDOWS         6
/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern WM_HWIN G_SystemWindows[];

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError WindowManager_Start_emWin();
T_uezError WindowManager_Close_emWin();
void WindowMangager_Show_Window(TUInt8 aWindow);
void WindowManager_Create_All_Active_Windows(void);

TUInt32 EnterNameScreen(char *aName, TUInt8 aBowler);

#endif // WINDOWMANAGER_H_
/*-------------------------------------------------------------------------*
 * End of File:  WindowManager.h
 *-------------------------------------------------------------------------*/
