/*-------------------------------------------------------------------------*
 * File: BowlingScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup BowlingScreen
 * @{
 *     @brief Implements Layout and functionality of BowlingScreen.c.
 * 
 *     How it works in detail goes here ....
 */
/*-------------------------------------------------------------------------*/
//Includes needed by emWin
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "string.h"
#include <stdio.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Fonts/Fonts.h"
#include "Graphics/Graphics.h"
#include "../Bowlers.h"
#include <UEZRandom.h>
#include "Audio.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_BOWLER_1_TEXT        (GUI_ID_USER + 0x02)
#define ID_BOWLER_2_TEXT        (GUI_ID_USER + 0x03)
#define ID_BOWLER_3_TEXT        (GUI_ID_USER + 0x04)
#define ID_BOWLER_4_TEXT        (GUI_ID_USER + 0x05)
#define ID_BOWLER_5_TEXT        (GUI_ID_USER + 0x06)
#define ID_BOWLER_6_TEXT        (GUI_ID_USER + 0x07)
#define ID_BOWLER_7_TEXT        (GUI_ID_USER + 0x08)
#define ID_BOWLER_8_TEXT        (GUI_ID_USER + 0x09)
#define ID_BOWLER_1_BUTTON      (GUI_ID_USER + 0x0A)
#define ID_BOWLER_2_BUTTON      (GUI_ID_USER + 0x0B)
#define ID_BOWLER_3_BUTTON      (GUI_ID_USER + 0x0C)
#define ID_BOWLER_4_BUTTON      (GUI_ID_USER + 0x0D)
#define ID_BOWLER_5_BUTTON      (GUI_ID_USER + 0x0E)
#define ID_BOWLER_6_BUTTON      (GUI_ID_USER + 0x0F)
#define ID_BOWLER_7_BUTTON      (GUI_ID_USER + 0x10)
#define ID_BOWLER_8_BUTTON      (GUI_ID_USER + 0x11)
#define ID_SKIP_BOWLER_TEXT     (GUI_ID_USER + 0x12)
#define ID_EDIT_SCORE_TEXT      (GUI_ID_USER + 0x13)
#define ID_REMOVE_BOWLER_TEXT   (GUI_ID_USER + 0x14)
#define ID_THEME_TEXT           (GUI_ID_USER + 0x15)
#define ID_OPTIONS_TEXT         (GUI_ID_USER + 0x16)
#define ID_SERVICE_TEXT         (GUI_ID_USER + 0x17)
#define ID_SKIP_BOWLER_BUTTON   (GUI_ID_USER + 0x18)
#define ID_EDIT_SCORE_BUTTON    (GUI_ID_USER + 0x19)
#define ID_REMOVE_BOWLER_BUTTON (GUI_ID_USER + 0x1A)
#define ID_THEME_BUTTON         (GUI_ID_USER + 0x1B)
#define ID_OPTIONS_BUTTON       (GUI_ID_USER + 0x1C)
#define ID_SERVICE_BUTTON       (GUI_ID_USER + 0x1D)
#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x1E)
#define ID_SCORE_TIMER          (GUI_ID_USER + 0x1F)
#define ID_DESCRIPTION_TEXT     (GUI_ID_USER + 0x20)

#define SPACING                 (10)

#define BOWLER_TEXT_XSIZE       (20)
#define BOWLER_TEXT_YSIZE       (58)
#define BOWLER_TEXT_XPOS        (SPACING)
#define BOWLER_TEXT_YPOS(n)     (5 + (n * ( BOWLER_TEXT_YSIZE + 2)))

#define BOWLER_BUTTON_XSIZE     (229)
#define BOWLER_BUTTON_YSIZE     (58)
#define BOWLER_BUTTON_XPOS      (BOWLER_TEXT_XSIZE + SPACING)
#define BOWLER_BUTTON_YPOS(n)   (5 + (n * (BOWLER_BUTTON_YSIZE + 2)))

#define CONTROL_TEXT_XSIZE      (150)
#define CONTROL_TEXT_YSIZE      (20)
#define CONTROL_TEXT_XPOS(n)    (((UEZ_LCD_DISPLAY_WIDTH/2) - (CONTROL_TEXT_XSIZE/2)) + ( n * (CONTROL_TEXT_XSIZE + 100)))
#define CONTROL_TEXT_YPOS(n)    ( (4*SPACING) + ( n * ((CONTROL_TEXT_YSIZE + SPACING) + CONTROL_BUTTONS_YSIZE + SPACING)))

#define CONTROL_BUTTONS_XSIZE   (95)
#define CONTROL_BUTTONS_YSIZE   (95)
#define CONTROL_BUTTONS_XPOS(n) (((UEZ_LCD_DISPLAY_WIDTH/2) - (CONTROL_BUTTONS_XSIZE/2)) + ( n * (CONTROL_TEXT_XSIZE + 100)))
#define CONTROL_BUTTONS_YPOS(n) ( (5*SPACING) + CONTROL_TEXT_YSIZE + ( n * ( CONTROL_BUTTONS_XSIZE + CONTROL_TEXT_YSIZE + (2*SPACING))))

#define UPDATE_TIME_MS          (500)
#define UPDATE_SCORE_MS         (5000)
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleBowler(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleSkipBowler(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleEditScore(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleRemoveBowler(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleTheme(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleOptions(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleService(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

void BowlingHasFinished();
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iBowlingScreenDialog[] = {
    //Function                  Name          ID                        XP                          YP     				    XS                              YS
    { WINDOW_CreateIndirect     , ""        , ID_WINDOW                 , 0                         , 0 		    , UEZ_LCD_DISPLAY_WIDTH         , UEZ_LCD_DISPLAY_HEIGHT, 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_DESCRIPTION_TEXT       , 120                       , 430                   , UEZ_LCD_DISPLAY_WIDTH         , 60                    , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},//Code Added by IMM - To add description label WARNING: CODE IS NOT SET TO SCALE TO SMALLER BOARDS!
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_1_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(0)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_2_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(1)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_3_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(2)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_4_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(3)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_5_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(4)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_6_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(5)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_7_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(6)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_BOWLER_8_TEXT          , BOWLER_TEXT_XPOS          , BOWLER_TEXT_YPOS(7)   , BOWLER_TEXT_XSIZE             , BOWLER_TEXT_YSIZE     , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_1_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(0) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_2_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(1) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_3_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(2) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_4_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(3) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_5_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(4) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_6_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(5) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_7_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(6) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWLER_8_BUTTON        , BOWLER_BUTTON_XPOS        , BOWLER_BUTTON_YPOS(7) , BOWLER_BUTTON_XSIZE           , BOWLER_BUTTON_YSIZE   , 0, 0, 0},

    { TEXT_CreateIndirect       , ""        , ID_SKIP_BOWLER_TEXT       , CONTROL_TEXT_XPOS(0)      , CONTROL_TEXT_YPOS(0)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_EDIT_SCORE_TEXT        , CONTROL_TEXT_XPOS(1)      , CONTROL_TEXT_YPOS(0)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_REMOVE_BOWLER_TEXT     , CONTROL_TEXT_XPOS(0)      , CONTROL_TEXT_YPOS(1)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_THEME_TEXT             , CONTROL_TEXT_XPOS(1)      , CONTROL_TEXT_YPOS(1)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_OPTIONS_TEXT           , CONTROL_TEXT_XPOS(0)      , CONTROL_TEXT_YPOS(2)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_SERVICE_TEXT           , CONTROL_TEXT_XPOS(1)      , CONTROL_TEXT_YPOS(2)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},

    { BUTTON_CreateIndirect     , ""        , ID_SKIP_BOWLER_BUTTON     , CONTROL_BUTTONS_XPOS(0)   , CONTROL_BUTTONS_YPOS(0)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_EDIT_SCORE_BUTTON      , CONTROL_BUTTONS_XPOS(1)   , CONTROL_BUTTONS_YPOS(0)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_REMOVE_BOWLER_BUTTON   , CONTROL_BUTTONS_XPOS(0)   , CONTROL_BUTTONS_YPOS(1)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_THEME_BUTTON           , CONTROL_BUTTONS_XPOS(1)   , CONTROL_BUTTONS_YPOS(1)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_OPTIONS_BUTTON         , CONTROL_BUTTONS_XPOS(0)   , CONTROL_BUTTONS_YPOS(2)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_SERVICE_BUTTON         , CONTROL_BUTTONS_XPOS(1)   , CONTROL_BUTTONS_YPOS(2)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping BowlingScreenMapping[] = {
    { ID_WINDOW                 , ""    , GUI_BLACK         , GUI_BLACK     , &MAGELLAN_FONT_SMALL  , LAFSetupWindow    , 0},
    { ID_DESCRIPTION_TEXT       , "The program is now simulating the players bowling.\nThis screen will terminate when the simulation is done."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_SMALL  , LAFSetupText      , 0},
    { ID_BOWLER_1_TEXT          , "1."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_2_TEXT          , "2."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_3_TEXT          , "3."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_4_TEXT          , "4."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_5_TEXT          , "5."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_6_TEXT          , "6."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_7_TEXT          , "7."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_8_TEXT          , "8."  , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWLER_1_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_2_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_3_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_4_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_5_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_6_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_7_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},
    { ID_BOWLER_8_BUTTON        , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowler},

    { ID_SKIP_BOWLER_TEXT       , "Skip Bowler"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_EDIT_SCORE_TEXT        , "Edit Score"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_REMOVE_BOWLER_TEXT     , "Remove Bowler"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_THEME_TEXT             , "Theme"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_OPTIONS_TEXT           , "Options"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_SERVICE_TEXT           , "Service"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},

    { ID_SKIP_BOWLER_BUTTON     , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleSkipBowler},
    { ID_EDIT_SCORE_BUTTON      , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleEditScore},
    { ID_REMOVE_BOWLER_BUTTON   , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleRemoveBowler},
    { ID_THEME_BUTTON           , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleTheme},
    { ID_OPTIONS_BUTTON         , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleOptions},
    { ID_SERVICE_BUTTON         , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleService},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static T_Bowler G_BowlersList[MAX_NUM_BOWLERS];
static TBool G_RemoveBowlerMode = EFalse;
static WM_HTIMER G_UpdateTimer = 0;
static WM_HTIMER G_ScoringTimer = 0;
static TUInt8 G_CurrentBowler = 0;
static T_uezSemaphore G_UpdateSem = 0;
static T_uezRandomStream G_RandomStream;
static TBool G_Done = EFalse;

static void IGrab()
{
#if MAGELLAN_DEBUG
    printf("BS Sem Grab\n");
#endif
    UEZSemaphoreGrab(G_UpdateSem, UEZ_TIMEOUT_INFINITE);
}

static void IRelease()
{
#if MAGELLAN_DEBUG
    printf("BS Sem Release\n");
#endif
    UEZSemaphoreRelease(G_UpdateSem);
}

/*---------------------------------------------------------------------------*
 * Routine: ISetButtonIcons
 *---------------------------------------------------------------------------*/
/** 
 *      
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static void ISetButtonIcons(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    TUInt32 i;

    for (i = ID_BOWLER_1_BUTTON; i <= ID_BOWLER_8_BUTTON; i++){
        hItem = WM_GetDialogItem(pMsg->hWin, i);
        BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmNameBar, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmNameBar, 1, 1);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmNameBar, 0, 0);
    }

    hItem = WM_GetDialogItem(pMsg->hWin, ID_SKIP_BOWLER_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmSkipBowler, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmSkipBowler, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmSkipBowler, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SCORE_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmEditScore, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmEditScore, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmEditScore
            , 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_REMOVE_BOWLER_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmRemoveBowler, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmRemoveBowler, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmRemoveBowler, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_THEME_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmTheme, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmTheme, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmTheme, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_OPTIONS_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmOptions, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmOptions, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmOptions, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_SERVICE_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmService, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmService, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmService, 0, 0);
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleBowler
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleBowler(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if (G_RemoveBowlerMode){
            DeleteBowler(aID - ID_BOWLER_1_BUTTON);
            G_RemoveBowlerMode = EFalse;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_REMOVE_BOWLER_BUTTON);
            BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmRemoveBowler, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmRemoveBowler, 1, 1);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmRemoveBowler, 0, 0);
        }
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleSkipBowler
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID			ID of widget that caused the event.
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleSkipBowler(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleEditScore
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleEditScore(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleRemoveBowler
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleRemoveBowler(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        G_RemoveBowlerMode = ~G_RemoveBowlerMode;

        if(!G_RemoveBowlerMode){
            hItem = WM_GetDialogItem(pMsg->hWin, ID_REMOVE_BOWLER_BUTTON);
            BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmRemoveBowler, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmRemoveBowler, 1, 1);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmRemoveBowler, 0, 0);
        } else {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_REMOVE_BOWLER_BUTTON);
            BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmRemoveBowlerSelected, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmRemoveBowlerSelected, 1, 1);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmRemoveBowlerSelected, 0, 0);
        }
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleSkipBowler
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleTheme(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleOptions
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleOptions(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleService
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleService(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IUpdateFields
 *---------------------------------------------------------------------------*/
/** Update all the fields on the screen.
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    char buttonText[20];
    TUInt8 i;

    IGrab();
    GetBowlers(G_BowlersList);

    if(G_BowlersList[0].iName[0] == '\0'){
        //Exit Bowling Mode
        G_RemoveBowlerMode = EFalse;
        WindowMangager_Show_Window(MAIN_SCREEN);
    }

    if(G_RemoveBowlerMode){
        for ( i = 0; i < MAX_NUM_BOWLERS; i ++){
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWLER_1_BUTTON + i);
            if ( G_BowlersList[i].iName[0] != '\0'){
                sprintf(buttonText, "%s  -", G_BowlersList[i].iName);
                BUTTON_SetText(hItem, buttonText);
                WM_InvalidateWindow(hItem);
            } else {
                BUTTON_SetText(hItem, "");
                WM_InvalidateWindow(hItem);
            }
        }
    } else {
        for ( i = 0; i < MAX_NUM_BOWLERS; i ++){
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWLER_1_BUTTON + i);
            if ( G_BowlersList[i].iName[0] != '\0'){
                sprintf(buttonText, "%s  %d", G_BowlersList[i].iName, G_BowlersList[i].iScore);
                BUTTON_SetText(hItem, buttonText);
                WM_InvalidateWindow(hItem);
            } else {
                BUTTON_SetText(hItem, "");
                WM_InvalidateWindow(hItem);
            }
        }
    }
    IRelease();
}

/*---------------------------------------------------------------------------*
 * Routine:	_BowlingScreen.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _BowlingScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
    WM_HWIN hItem;
    TUInt32 i;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, BowlingScreenMapping);
        ISetButtonIcons(pMsg);
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, UPDATE_TIME_MS, 0);
        G_ScoringTimer = WM_CreateTimer(pMsg->hWin, ID_SCORE_TIMER, 0, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(BowlingScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
	case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if(NCode == G_UpdateTimer){
            if(G_Active){
                IUpdateFields(pMsg);
            }
            WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
        } else if ( NCode == G_ScoringTimer){
            if(G_Active && !G_RemoveBowlerMode){
                if (G_Done == ETrue){
                    BowlingHasFinished();
                }
                IGrab();
                for (i = ID_BOWLER_1_BUTTON; i <= ID_BOWLER_8_BUTTON; i++){
                    hItem = WM_GetDialogItem(pMsg->hWin, i);
                    if ( (G_CurrentBowler + ID_BOWLER_1_BUTTON) == i){
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmNameBarSelected, 0, 0);
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmNameBarSelected, 1, 1);
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmNameBarSelected, 0, 0);
                    }else {
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmNameBar, 0, 0);
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmNameBar, 1, 1);
                        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmNameBar, 0, 0);
                    }
                }
                G_BowlersList[G_CurrentBowler].iScore += (UEZRandomSigned32Bit(&G_RandomStream)%11);
                SetBowlerScore(G_CurrentBowler, G_BowlersList[G_CurrentBowler].iScore);
                if ( G_CurrentBowler < MAX_NUM_BOWLERS){
                    G_CurrentBowler++;
                    if ( G_BowlersList[G_CurrentBowler].iName[0] == '\0'){
                        G_CurrentBowler = 0;
                        G_Done = ETrue;
                    }
                } else {
                    G_CurrentBowler = 0;
                    G_Done = ETrue;
                }
                IRelease();
            }

            WM_RestartTimer(G_ScoringTimer, UPDATE_SCORE_MS);
        }
        break;
    case WM_APP_GAINED_FOCUS:
    	G_Active = ETrue;
    	G_CurrentBowler = 0;
    	G_Done = EFalse;
#if MAGELLAN_DEBUG
        printf("Bowling Screen Active\n");
#endif
    	break;
    case WM_APP_LOST_FOCUS:
    	G_Active = EFalse;
#if MAGELLAN_DEBUG
        printf("Bowling Screen Inactive\n");
#endif
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	BowlingScreen_Create
 *---------------------------------------------------------------------------*/
/** Create the BowlingScreen. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN BowlingScreen_Create()
{
    UEZSemaphoreCreateBinary(&G_UpdateSem);
    UEZRandomStreamCreate(&G_RandomStream, 0x565, UEZ_RANDOM_PSUEDO);
    return GUI_CreateDialogBox(_iBowlingScreenDialog, GUI_COUNTOF(_iBowlingScreenDialog), &_BowlingScreenDialog, 0,0,0);
}

TBool BowlingIsActive()
{
    if (G_Active && !G_RemoveBowlerMode){
        return ETrue;
    } else {
        return EFalse;
    }
}

void BowlingHasFinished()
{
    //Exit Bowling Mode
    G_RemoveBowlerMode = EFalse;
    WindowMangager_Show_Window(MAIN_SCREEN);
}

void BowlingScreenGetCurrentBowler(T_Bowler *aBowler)
{
    IGrab();
    memcpy((void*)aBowler, (void*)&G_BowlersList[G_CurrentBowler], sizeof(T_Bowler));
    IRelease();
}

void BowlingScreenSetCurrentBowlerScore(T_Bowler *aBowler)
{
    IGrab();

    memcpy((void*)&G_BowlersList[G_CurrentBowler], (void*)aBowler, sizeof(T_Bowler));

    if ( G_CurrentBowler < MAX_NUM_BOWLERS){
        G_CurrentBowler++;
        if ( G_BowlersList[G_CurrentBowler].iName[0] == '\0'){
            G_CurrentBowler = 0;
        }
    } else {
        G_CurrentBowler = 0;
    }
    IRelease();
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    BowlingScreen.c
 *-------------------------------------------------------------------------*/
