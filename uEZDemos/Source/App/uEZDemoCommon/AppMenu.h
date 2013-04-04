/*-------------------------------------------------------------------------*
 * File:  AppMenu.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Standard app selection interface
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _AppMenu_H_
#define _AppMenu_H_

#ifndef APP_MENU_ALLOW_TEST_MODE
#define APP_MENU_ALLOW_TEST_MODE		0
#endif

#include <uEZ.h>

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

#endif // _AppMenu_H_
/*-------------------------------------------------------------------------*
 * File:  AppMenu.c
 *-------------------------------------------------------------------------*/
