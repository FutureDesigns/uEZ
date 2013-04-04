/*-------------------------------------------------------------------------*
 * File:  AppMenu.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present an application menu
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
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZTS.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include "uEZDemoCommon.h"
#include <UEZLCD.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
#define ICON_WIDTH      64
#define ICON_HEIGHT     64
#define EXIT_ICON_HEIGHT  32
#define EXIT_ICON_WIDTH   64
#define ICON_MIN_PADDING_X        30
#define ICON_MIN_PADDING_Y        20
#define SCREEN_EDGE_MIN_PADDING   10
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
#define ICON_WIDTH      128
#define ICON_HEIGHT     128
#define EXIT_ICON_HEIGHT  64
#define EXIT_ICON_WIDTH   128
#define ICON_MIN_PADDING_X        50
#define ICON_MIN_PADDING_Y        50
#define SCREEN_EDGE_MIN_PADDING   20
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
#define ICON_WIDTH      64
#define ICON_HEIGHT     64
#define EXIT_ICON_HEIGHT  32
#define EXIT_ICON_WIDTH   64
#define ICON_MIN_PADDING_X        30
#define ICON_MIN_PADDING_Y        40
#define SCREEN_EDGE_MIN_PADDING   10
#endif

#define SCREEN_USABLE_WIDTH     (UEZ_LCD_DISPLAY_WIDTH-2*SCREEN_EDGE_MIN_PADDING)
#define SCREEN_USABLE_HEIGHT    (UEZ_LCD_DISPLAY_HEIGHT-2*SCREEN_EDGE_MIN_PADDING)
#define MAX_ICONS_HORIZONTAL    (SCREEN_USABLE_WIDTH/(ICON_MIN_PADDING_X+ICON_WIDTH))
#define MAX_ICONS_VERTICAL      (SCREEN_USABLE_HEIGHT/(ICON_MIN_PADDING_Y+ICON_HEIGHT))
#define MAX_ICONS               (MAX_ICONS_HORIZONTAL * MAX_ICONS_VERTICAL)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const T_appMenu *iMenu;
    T_uezDevice iLCD;
    SWIM_WINDOW_T iWin;
    TBool iDoExit;
    T_choice iChoices[MAX_ICONS + 2];
} T_appMenuWorkspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#if APP_MENU_ALLOW_TEST_MODE
TBool volatile G_mmTestMode = EFalse;
#endif

static void IAMExit(const T_choice *aChoice)
{
    // Mark the workspace as exiting
    ((T_appMenuWorkspace *)(aChoice->iData))->iDoExit = ETrue;
}

extern void TestMode(void);
/*---------------------------------------------------------------------------*
 * Routine:  MainMenuScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the main menu with all its icons.
 *---------------------------------------------------------------------------*/
static void AppMenuScreen(T_appMenuWorkspace *aWS)
{
    T_pixelColor *pixels;
    T_choice *p = aWS->iChoices;
    T_region r;
    T_region rsub;
    T_region ricon;
    T_region r2;
    const T_appMenuEntry *p_entry;
    TUInt32 x, y;
    TUInt32 h, v;
    TUInt32 fontHeight;

    UEZLCDGetFrame(aWS->iLCD, 0, (void **)&pixels);
       
    SUIHidePage0();

    // setup a standard window
    swim_window_open(&aWS->iWin, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixels, 0, 0,
            DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 2, YELLOW, RGB(0, 0, 0), RED);
    swim_set_font(&aWS->iWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&aWS->iWin, aWS->iMenu->iTitle, BLUE);
    swim_set_fill_color(&aWS->iWin, BLACK);
    fontHeight = swim_get_font_height(&aWS->iWin);

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = aWS->iWin.xvsize;
    r.iBottom = aWS->iWin.yvsize;

    RegionShrink(&r, SCREEN_EDGE_MIN_PADDING - 2);

    p_entry = aWS->iMenu->iEntries;

    // If this menu has an exit button, pull that off the bottom first
    if (aWS->iMenu->iCanExit) {
        RegionSplitFromBottom(&r, &rsub, fontHeight + EXIT_ICON_HEIGHT + 2, 2);
        RegionCenterLeftRight(&rsub, &ricon, EXIT_ICON_WIDTH);
        p->iLeft = ricon.iLeft;
        p->iTop = ricon.iTop;
        p->iRight = ricon.iLeft + EXIT_ICON_WIDTH - 1;
        p->iBottom = ricon.iTop + EXIT_ICON_HEIGHT - 1;
        p->iText = "Exit";
        p->iAction = IAMExit;
        p->iIcon = G_exitIcon;
        p->iData = (void *)aWS;
        p->iDraw = 0; // standard icon drawing
        p->iFlags = 0;
        p++;
    }

    // Take off an amount from the top we expect to need for centering
    v = (1 + r.iBottom - r.iTop) - (MAX_ICONS_VERTICAL * (ICON_MIN_PADDING_Y
            + ICON_HEIGHT) - ICON_MIN_PADDING_Y + (fontHeight + 2));
    RegionSplitFromTop(&r, &r2, v / 2, 0);

    // Create all the choices (until we run out of app entry items or out of space)
    for (y = 0; ((y < MAX_ICONS_VERTICAL) && (p_entry->iName)); y++) {
        // Grab a line off the main region
        RegionSplitFromTop(&r, &rsub, ICON_HEIGHT, ICON_MIN_PADDING_Y);

        // How much do we need to take off the left edge to center these icons?
        h = (1 + r.iRight - r.iLeft) - (MAX_ICONS_HORIZONTAL
                * (ICON_MIN_PADDING_X + ICON_WIDTH) - ICON_MIN_PADDING_X);
        RegionSplitFromLeft(&rsub, &r2, h / 2, 0);

        // Okay, rsub is now the proper line for a group of icons
        for (x = 0; ((x < MAX_ICONS_HORIZONTAL) && (p_entry->iName)); x++) {
            RegionSplitFromLeft(&rsub, &ricon, ICON_WIDTH, ICON_MIN_PADDING_X);
            // Got an icon position, place one here
            p->iLeft = ricon.iLeft;
            p->iTop = ricon.iTop;
            p->iRight = ricon.iRight;
            p->iBottom = ricon.iBottom;
            p->iText = p_entry->iName;
            p->iAction = p_entry->iFunction;
            p->iIcon = p_entry->iIcon;
            p->iData = p_entry->iData;
            p->iDraw = 0; // standard icon drawing
            p->iFlags = 0;
            p++;
            p_entry++;
        }
    }
    // Last one is done
    p->iText = 0;
    ChoicesDraw(&aWS->iWin, aWS->iChoices);

    //SUIShowPage0Fancy();
    if(!UEZLCDScreensaverIsActive())    
        SUIShowPage0Fancy();

//SUIShowPage0();
}

#ifdef NO_DYNAMIC_MEMORY_ALLOC	

/* Declare data structures used for menu workspaces when we are not using dynamic
 memory allocation. */

#define appmenuMAX_NESTED_MENUS		( 3 )
typedef struct {
    T_appMenuWorkspace ws;
    T_uezQueue queue;
}tAppMenuData;

tAppMenuData xAppMenuData[ appmenuMAX_NESTED_MENUS ];

#endif // NO_DYNAMIC_MEMORY_ALLOC
/*---------------------------------------------------------------------------*
 * Routine:  GetWorkspaceAndQueue
 *---------------------------------------------------------------------------*
 * Description:
 *      Search for and return a free workspace/queue buffer set for use by
 *      Appmenu.
 *---------------------------------------------------------------------------*/
static T_appMenuWorkspace *GetWorkspaceAndQueue(T_uezQueue *q)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
    TUInt16 usLoop = 0;
    T_appMenuWorkspace *xWs = NULL;

    /* Search for an unused entry in xAppMenuData. */
    while( ( xAppMenuData[usLoop].ws.iMenu != NULL ) && ( usLoop < appmenuMAX_NESTED_MENUS ) )
    {
        usLoop++;
    }
    if( usLoop != appmenuMAX_NESTED_MENUS)
    {
        xWs = &xAppMenuData[usLoop].ws;

        /* Has the queue already been created. */
        if( xAppMenuData[usLoop].queue == NULL )
        {
            if (UEZQueueCreate(1, sizeof(T_uezTSReading), &xAppMenuData[usLoop].queue) != UEZ_ERROR_NONE)
            {
                xAppMenuData[usLoop].queue = NULL;
            }
        }
        *q = xAppMenuData[usLoop].queue;
        /* Register the queue so that the IAR Stateviewer Plugin knows about it. */
        UEZQueueAddToRegistry( xAppMenuData[usLoop].queue, "AppMenu TS" );
    }
#else
    T_appMenuWorkspace *xWs = UEZMemAlloc(sizeof(T_appMenuWorkspace));
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), q) != UEZ_ERROR_NONE)
        *q = NULL;
#endif // NO_DYNAMIC_MEMORY_ALLOC
    return xWs;
}

/*---------------------------------------------------------------------------*
 * Routine:  AppMenu
 *---------------------------------------------------------------------------*
 * Description:
 *      Present a menu of application choices (or folders)
 *---------------------------------------------------------------------------*/
static void FreeWorkspace(T_appMenuWorkspace *ws)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC	

    TUInt16 usLoop = 0;

    /* Search for the correct entry in xAppMenuData. */
    while( ( xAppMenuData[usLoop].ws.iMenu != ws->iMenu ) && ( usLoop < appmenuMAX_NESTED_MENUS ) )
    {
        usLoop++;
    }
    if( usLoop != appmenuMAX_NESTED_MENUS)
    {
        /* Mark the entry as free. */
        xAppMenuData[usLoop].ws.iMenu = NULL;
    }
#else
    UEZMemFree(ws);
#endif // NO_DYNAMIC_MEMORY_ALLOC
}

/*---------------------------------------------------------------------------*
 * Routine:  AppMenu
 *---------------------------------------------------------------------------*
 * Description:
 *      Present a menu of application choices (or folders)
 *---------------------------------------------------------------------------*/

void AppMenu(const T_appMenu *aMenu)
{
    T_uezDevice ts;
    T_uezQueue queue;
	T_appMenuWorkspace *p_ws;
    
    p_ws = GetWorkspaceAndQueue(&queue);
    if (!p_ws)
        return;
    memset(p_ws, 0, sizeof(*p_ws));
    p_ws->iMenu = aMenu;

    //TS_MC_AR1020_InitializeWorkspace(*p_ws);///REMOVE
    // Setup queue to receive touchscreen events
    if (queue != NULL) {
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue) == UEZ_ERROR_NONE) {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &p_ws->iLCD) == UEZ_ERROR_NONE) {
                AppMenuScreen(p_ws);

                while (!p_ws->iDoExit) {
                    // Wait forever until we receive a touchscreen event
                    // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                    if (aMenu->iIdleFunction)
                        if (aMenu->iIdleFunction() & APP_MENU_REDRAW_SCREEN)
                            AppMenuScreen(p_ws);
                    if (ChoicesUpdate(&p_ws->iWin, p_ws->iChoices, queue,
                            UEZ_TIMEOUT_INFINITE)) {
                        // Did a choice and returned
                        if (!p_ws->iDoExit)
                            AppMenuScreen(p_ws);
                    }
                    
#if APP_MENU_ALLOW_TEST_MODE
                    if (G_mmTestMode) {
                        // Go into this test mode
                        TestMode();
                        // Redraw screen
                        AppMenuScreen(p_ws);
                        // End test mode
                        G_mmTestMode = EFalse;
                    }
#endif
                }
                UEZLCDClose(p_ws->iLCD);
            }
            UEZTSClose(ts, queue);
        }
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
        UEZQueueDelete(queue);
#endif
    }
    FreeWorkspace(p_ws);
}

void AppSubmenu(const T_choice *aChoice)
{
    AppMenu((const T_appMenu *)(aChoice->iData));
}

/*-------------------------------------------------------------------------*
 * End of File:  AppMenu.c
 *-------------------------------------------------------------------------*/
