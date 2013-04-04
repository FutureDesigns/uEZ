/*-------------------------------------------------------------------------*
 * File:  GUIManager
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

#ifndef _GUIMANAGER_H
#define _GUIMANAGER_H
#include <uEZ.h>
#include <GUI.h>
#include <WM.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NUM_WINDOWS        9 //currently 9 windows in the system to tack
#define C_ORANGE  0x00008CFF//new orange

//each window has a unique identifier
#define HOME_SCREEN             0
#define SETTINGS_SCREEN         1

#define HIDE_ALL            0xFF

#define WM_APP_GAINED_FOCUS         (WM_USER + 0x1000)
#define WM_APP_LOST_FOCUS           (WM_USER + 0x1001)

//Keyboard and key pad are modal and not in the system all the time.

typedef struct{
    TUInt32 iID;
    TUInt8 iNumChar;
    char iStaticText[10];
    char iMessage[20];
} T_ButtonData;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern WM_HWIN G_SystemWindows_PF[];

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
T_uezError GUIManager_Start_emWin_PF();
T_uezError GUIManager_Close_emWin();
void GUIManager_Show_Window_PF(TUInt8 aWindow);
void GUIManager_Create_All_Active_Windows_PF(void);

#endif /* _GUIManager_H */
/*-------------------------------------------------------------------------*
 * End of File:  GUIManager
 *-------------------------------------------------------------------------*/
