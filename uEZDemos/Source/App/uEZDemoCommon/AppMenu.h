/*-------------------------------------------------------------------------*
 * File:  AppMenu.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Standard app selection interface
 *-------------------------------------------------------------------------*/
#ifndef _AppMenu_H_
#define _AppMenu_H_

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
#ifndef APP_MENU_ALLOW_TEST_MODE
#define APP_MENU_ALLOW_TEST_MODE		0
#endif

#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef void (*T_appMenuFunc)(const T_choice *aChoice);

typedef struct _tag_appMenuEntry {
    const char *iName;    // or zero if end of entry list
    T_appMenuFunc iFunction;
    const TUInt8 *iIcon;
    void *iData;
} T_appMenuEntry ;

typedef struct {
    const char *iTitle;
    const T_appMenuEntry *iEntries;
    TBool iCanExit;
    TUInt32 (*iIdleFunction)(void);
    #define APP_MENU_REDRAW_SCREEN     (1<<0)
} T_appMenu;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void AppMenu(const T_appMenu *aMenu);
void AppSubmenu(const T_choice *aChoice);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#if APP_MENU_ALLOW_TEST_MODE
    extern TBool volatile G_mmTestMode;
#endif

#ifdef __cplusplus
}
#endif

#endif // _AppMenu_H_
/*-------------------------------------------------------------------------*
 * File:  AppMenu.c
 *-------------------------------------------------------------------------*/
