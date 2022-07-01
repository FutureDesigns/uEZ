/*-------------------------------------------------------------------------*
 * File:  GUIManager
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _GUIMANAGER_H
#define _GUIMANAGER_H

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <GUI.h>
#include <WM.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _GUIManager_H */
/*-------------------------------------------------------------------------*
 * End of File:  GUIManager
 *-------------------------------------------------------------------------*/
