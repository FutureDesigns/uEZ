/*-------------------------------------------------------------------------*
 * File: HomeScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup HomeScreen
 * @{
 *     @brief Implements Layout and functionality of HomeScreen.c.
 * 
 *     How it works in detail goes here ....
 */
/*-------------------------------------------------------------------------*/
//Includes needed by emWin
#include "GUI.h"
#include "BUTTON.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include <string.h>
#include <stdio.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_STARTHERE_TEXT       (GUI_ID_USER + 0x02)
#define ID_FDIINFO_TEXT         (GUI_ID_USER + 0x03)

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#define FONTSIZE                &FONT_LARGE
#else
#define SPACING                 (5)
#define FONTSIZE                &FONT_SMALL
#endif

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define STARTHERE_TEXT_XSIZE    (WINDOW_XSIZE)
#define STARTHERE_TEXT_YSIZE    ((WINDOW_YSIZE/10))
#define STARTHERE_TEXT_XPOS     (0)
#define STARTHERE_TEXT_YPOS     ((WINDOW_YSIZE/10)*4 + 10)

#define FDIINFO_TEXT_XSIZE      (WINDOW_XSIZE)
#define FDIINFO_TEXT_YSIZE      ((WINDOW_YSIZE/10))
#define FDIINFO_TEXT_XPOS       (0)
#define FDIINFO_TEXT_YPOS       ((WINDOW_YSIZE/10)*9 + 0)


/*---------------------------------------------------------------------------*
 * Global Data:
 *---------------------------------------------------------------------------*/
static TBool G_Active = EFalse; // Active Flag, tell the dialog when it receives messages that the screen is in the foreground

   
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iHomeScreenDialog[] = {
    //Function                  Name            ID                  XP                      YP                      XS                      YS
    { WINDOW_CreateIndirect     , ""            , ID_WINDOW         , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE         , 0, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_TITLE_TEXT     , TITLE_TEXT_XPOS       , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE     , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_STARTHERE_TEXT , STARTHERE_TEXT_XPOS   , STARTHERE_TEXT_YPOS   , STARTHERE_TEXT_XSIZE  , STARTHERE_TEXT_YSIZE , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_FDIINFO_TEXT   , FDIINFO_TEXT_XPOS     , FDIINFO_TEXT_YPOS     , FDIINFO_TEXT_XSIZE    , FDIINFO_TEXT_YSIZE   , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping HomeScreenMapping[] = {
    // ID                       Text                                 Background Color        Text Color              Font Size               Setup Function          Handle Function
    { ID_WINDOW                 , ""                                 , GUI_BLACK             , GUI_WHITE             , &FONT_SMALL           , LAFSetupWindow        , 0},
    { ID_TITLE_TEXT             , "My Main Screen"                   , GUI_BLACK             , GUI_WHITE             , &FONT_LARGE           , LAFSetupText          , 0},
    { ID_STARTHERE_TEXT         , "\"START HERE\""                   , GUI_BLACK             , GUI_WHITE             , &FONT_LARGE           , LAFSetupText          , 0},
    { ID_FDIINFO_TEXT           , "Future Designs, Inc. 2016                       www.TeamFDI.com"
                                                                     , GUI_BLACK             , GUI_WHITE             , FONTSIZE              , LAFSetupText          , 0},
    {0},
};


/*---------------------------------------------------------------------------*
 * Routine:    _HomeScreen.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _HomeScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId) {
    
    // Initialize the dialog.   
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, HomeScreenMapping);
        break;
    
    // Notify the parent object that an event has occured     
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(HomeScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    
    // Pre-paint (before the first draw)    
    case WM_PRE_PAINT:
        break;
    
    // Sent to a window when the timer has expired     
    case WM_TIMER:
      if(G_Active){}
      break;
    
    // Post-paint (after the last draw)   
    case WM_POST_PAINT:		
        break;
    
    // Object is coming into focus (not emWin)    
    case WM_APP_GAINED_FOCUS:
        G_Active = ETrue;
        break;
    
    // Object is going out of focus (not emWin)    
    case WM_APP_LOST_FOCUS:
        G_Active = EFalse;
        break;
    
    // Default     
    default:
      WM_DefaultProc(pMsg);
    break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:    HomeScreen_Create
 *---------------------------------------------------------------------------*/
/** Wrapper to create the SecondScreen. This is an emWin function.
 *  This function should only be called by the window manager and never 
 *  by user code.
 *      
 *  @param [in] p_choice    void
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN HomeScreen_Create()
{
    return GUI_CreateDialogBox(_iHomeScreenDialog, GUI_COUNTOF(_iHomeScreenDialog), &_HomeScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    HomeScreen.c
 *-------------------------------------------------------------------------*/
