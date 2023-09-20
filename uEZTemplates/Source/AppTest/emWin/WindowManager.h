/*-------------------------------------------------------------------------*
 * File:  WindowManager
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

#ifndef _WindowManager_H
#define _WindowManager_H
#include <uEZ.h>
#include <GUI.h>
#include <WM.h>
#include "Keyboard.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NUM_WINDOWS        9 //currently 9 windows in the system to tack

//each window has a unique identifier
#define HOME_SCREEN                      (0)
#define STORAGE_SCREEN                   (1)
#define SLIDESHOW_SCREEN                 (2)

#define HIDE_ALL                            (0xFF)

#define WM_APP_GAINED_FOCUS                 (WM_USER + 0x1000)
#define WM_APP_LOST_FOCUS                   (WM_USER + 0x1001)
#define WM_UPDATE_COMMON_ELEMENTS           (WM_USER + 0x1002)

typedef struct{
    TUInt32 iID;
    TUInt8 iNumChar;
    char iStaticText[10];
    char iMessage[20];
} T_ButtonData;

typedef enum {
    SHOW_ERROR = 0,
    HIDE_ERROR = 1,
    SET_ENABLED = 2,
    SET_DISABLED = 3,
    SET_REMOTE = 4,
    SET_LOCAL = 5
}T_CommonMode;

typedef void (*function)(WM_MESSAGE *pMsg);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern WM_HWIN G_SystemWindows[];

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
T_uezError WindowManager_Start_emWin();
T_uezError WindowManager_Close_emWin();
void WindowManager_Create_All_Active_Windows(void);

void WindowManager_Show_Window(TUInt8 aWindow);
void WindowManager_Get_Touch( GUI_PID_STATE *aState);
void WindowManager_UpdateCommonElements(T_CommonMode aMode);
void WindowManager_ClearTouches(void);
void WindowManager_Show_ErrorDialog(void);
void WindowManager_Hide_ErrorDialog(void);
void WindowManager_ShowSystemSetupScreen(void);
void WindowManager_BackLight_On(TUInt8 alevel);
void WindowManager_BackLight_Off(void);
void WindowManager_SaveScreenShotBMP(char driveNumber);
void WindowManager_InvalidateCurrentWindow(void);


#endif /* _WindowManager_H */
/*-------------------------------------------------------------------------*
 * End of File:  WindowManager
 *-------------------------------------------------------------------------*/
