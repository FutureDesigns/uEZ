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
//#include "Keyboard.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NUM_WINDOWS        9 //currently 9 windows in the system to tack

//each window has a unique identifier
#define HOME_SCREEN                         (0)
#define SECOND_SCREEN                       (1)

#define HIDE_ALL                            (0xFF)

#define WM_APP_GAINED_FOCUS                 (WM_USER + 0x1000)
#define WM_APP_LOST_FOCUS                   (WM_USER + 0x1001)
#define WM_UPDATE_COMMON_ELEMENTS           (WM_USER + 0x1002)
#define WM_TOUCH_RESET                      (WM_USER + 0x1003)

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

#define BACK_XSIZE      (128)
#define BACK_YSIZE      (128)
#define BACK_XPOS       (SPACING * 2)
#define BACK_YPOS       (WINDOW_YSIZE - SPACING - BACK_YSIZE)


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
void WindowManager_Show_ErrorDialog();
void WindowManager_Hide_ErrorDialog();
void WindowManager_ShowSystemSetupScreen();

#define BACKGROUND      (GUI_BLACK)//(0x00B42603)//(GUI_BLUE)
#define FONT            (GUI_WHITE)
#define BUTTON          (GUI_GRAY)//(0x001C49F1)//(GUI_ORANGE)

#endif /* _WindowManager_H */
/*-------------------------------------------------------------------------*
 * End of File:  WindowManager
 *-------------------------------------------------------------------------*/
