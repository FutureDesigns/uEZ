/*-------------------------------------------------------------------------*
 * File: StandardPlayScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup StandardPlayScreen
 * @{
 *     @brief Implements Layout and functionality of StandardPlayScreen.c.
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

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x01)  //Code Modified by IMM - Changed ID_MENU_BUTTON to ID_BACK_BUTTON
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
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x12)
#define ID_BOWL_BUTTON          (GUI_ID_USER + 0x13)

#define ID_REMOVE_BOWLER_TEXT   (GUI_ID_USER + 0x14)
#define ID_THEME_TEXT           (GUI_ID_USER + 0x15)
#define ID_OPTIONS_TEXT         (GUI_ID_USER + 0x16)
#define ID_SERVICE_TEXT         (GUI_ID_USER + 0x17)
#define ID_REMOVE_BOWLER_BUTTON (GUI_ID_USER + 0x18)
#define ID_THEME_BUTTON         (GUI_ID_USER + 0x19)
#define ID_OPTIONS_BUTTON       (GUI_ID_USER + 0x1A)
#define ID_SERVICE_BUTTON       (GUI_ID_USER + 0x1B)

#define SPACING                 (10)
#define MENU_XSIZE              (80)//Edited by IMM - Changed the size for the larger bitmap
#define MENU_YSIZE              (60)//Edited by IMM - Changed the size for the larger bitmap
#define MENU_XPOS               (UEZ_LCD_DISPLAY_WIDTH - MENU_XSIZE - SPACING)
#define MENU_YPOS               (UEZ_LCD_DISPLAY_HEIGHT - MENU_YSIZE - SPACING)

#define BOWLER_TEXT_XSIZE       (20)
#define BOWLER_TEXT_YSIZE       (58)
#define BOWLER_TEXT_XPOS        (SPACING)
#define BOWLER_TEXT_YPOS(n)     (5 + (n * ( BOWLER_TEXT_YSIZE + 2)))

#define BOWLER_BUTTON_XSIZE     (229)
#define BOWLER_BUTTON_YSIZE     (58)
#define BOWLER_BUTTON_XPOS      (BOWLER_TEXT_XSIZE + SPACING)
#define BOWLER_BUTTON_YPOS(n)   (5 + (n * (BOWLER_BUTTON_YSIZE + 2)))

#define TITLE_TEXT_XSIZE        (175)
#define TITLE_TEXT_YSIZE        (30)
#define TITLE_TEXT_XPOS         ((UEZ_LCD_DISPLAY_WIDTH/2) + (UEZ_LCD_DISPLAY_WIDTH/6) - (TITLE_TEXT_XSIZE/2))
#define TITLE_TEXT_YPOS         (SPACING)

#define BOWL_BUTTON_XSZIE       (135)
#define BOWL_BUTTON_YSZIE       (125)
#define BOWL_BUTTON_XPOS        ((UEZ_LCD_DISPLAY_WIDTH/2) + (UEZ_LCD_DISPLAY_WIDTH/6) - (BOWL_BUTTON_XSZIE/2))
#define BOWL_BUTTON_YPOS        (SPACING + TITLE_TEXT_YPOS + TITLE_TEXT_YSIZE)

#define CONTROL_TEXT_XSIZE      (150)
#define CONTROL_TEXT_YSIZE      (20)
#define CONTROL_TEXT_XPOS(n)    (((UEZ_LCD_DISPLAY_WIDTH/2) - (CONTROL_TEXT_XSIZE/2)) + ( n * (CONTROL_TEXT_XSIZE + 100)))
#define CONTROL_TEXT_YPOS(n)    ( BOWL_BUTTON_YPOS + BOWL_BUTTON_YSZIE + SPACING + ( n * ((CONTROL_TEXT_YSIZE + SPACING) + CONTROL_BUTTONS_YSIZE + SPACING)))

#define CONTROL_BUTTONS_XSIZE   (95)
#define CONTROL_BUTTONS_YSIZE   (95)
#define CONTROL_BUTTONS_XPOS(n) (((UEZ_LCD_DISPLAY_WIDTH/2) - (CONTROL_BUTTONS_XSIZE/2)) + ( n * (CONTROL_TEXT_XSIZE + 100)))
#define CONTROL_BUTTONS_YPOS(n) ( BOWL_BUTTON_YPOS + BOWL_BUTTON_YSZIE + (2*SPACING) + CONTROL_TEXT_YSIZE + ( n * ( CONTROL_BUTTONS_XSIZE + CONTROL_TEXT_YSIZE + (2*SPACING))))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleMenu(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleBowler(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleBowl(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleRemoveBowler(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleTheme(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleOptions(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleSerice(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iStandardPlayScreenDialog[] = {
  //Function                    Name        ID                          XP                       YP     				 XS                       YS
    { WINDOW_CreateIndirect     , ""        , ID_WINDOW                 , 0                         , 0 		            , UEZ_LCD_DISPLAY_WIDTH         , UEZ_LCD_DISPLAY_HEIGHT, 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BACK_BUTTON            , MENU_XPOS                 , MENU_YPOS             , MENU_XSIZE                    , MENU_YSIZE            , 0, 0, 0},    //Code Modified by IMM - Changed ID_MENU_BUTTON to ID_BACK_BUTTON
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
    { TEXT_CreateIndirect       , ""        , ID_TITLE_TEXT             , TITLE_TEXT_XPOS           , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE              , TITLE_TEXT_YSIZE      , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BOWL_BUTTON            , BOWL_BUTTON_XPOS          , BOWL_BUTTON_YPOS      , BOWL_BUTTON_XSZIE             , BOWL_BUTTON_YSZIE     , 0, 0, 0},

    { TEXT_CreateIndirect       , ""        , ID_REMOVE_BOWLER_TEXT     , CONTROL_TEXT_XPOS(0)      , CONTROL_TEXT_YPOS(0)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_THEME_TEXT             , CONTROL_TEXT_XPOS(1)      , CONTROL_TEXT_YPOS(0)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_OPTIONS_TEXT           , CONTROL_TEXT_XPOS(0)      , CONTROL_TEXT_YPOS(1)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_SERVICE_TEXT           , CONTROL_TEXT_XPOS(1)      , CONTROL_TEXT_YPOS(1)      , CONTROL_TEXT_XSIZE         , CONTROL_TEXT_YSIZE   , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},

    { BUTTON_CreateIndirect     , ""        , ID_REMOVE_BOWLER_BUTTON   , CONTROL_BUTTONS_XPOS(0)   , CONTROL_BUTTONS_YPOS(0)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_THEME_BUTTON           , CONTROL_BUTTONS_XPOS(1)   , CONTROL_BUTTONS_YPOS(0)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_OPTIONS_BUTTON         , CONTROL_BUTTONS_XPOS(0)   , CONTROL_BUTTONS_YPOS(1)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_SERVICE_BUTTON         , CONTROL_BUTTONS_XPOS(1)   , CONTROL_BUTTONS_YPOS(1)   , CONTROL_BUTTONS_XSIZE     , CONTROL_BUTTONS_YSIZE , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping StandardPlayScreenMapping[] = {
    { ID_WINDOW                 , ""    , GUI_BLACK      , GUI_BLACK     , &MAGELLAN_FONT_SMALL  , LAFSetupWindow    , 0},              //Edited by IMM - Changed the Menu background color
    { ID_BACK_BUTTON            , "Back", GUI_GRAY       , GUI_BLACK     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleMenu},    //Code Modified by IMM - Changed ID_MENU_BUTTON to ID_BACK_BUTTON
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
    { ID_TITLE_TEXT             , "Standard Play"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},
    { ID_BOWL_BUTTON            , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleBowl},

    { ID_REMOVE_BOWLER_TEXT     , "Remove Bowler"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , IHandleRemoveBowler},
    { ID_THEME_TEXT             , "Theme"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_OPTIONS_TEXT           , "Options"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_SERVICE_TEXT           , "Service"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},

    { ID_REMOVE_BOWLER_BUTTON   , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleRemoveBowler},
    { ID_THEME_BUTTON           , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleTheme},
    { ID_OPTIONS_BUTTON         , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleOptions},
    { ID_SERVICE_BUTTON         , ""    , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleSerice},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static T_Bowler G_BowlersList[MAX_NUM_BOWLERS];
static TBool G_RemoveBowlerMode = EFalse;
static TUInt8 G_NumBowlers = 0;

static TBool IHandleTheme(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}
static TBool IHandleOptions(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}
static TBool IHandleSerice(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {

    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
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

    /*  Code Removed by IMM - This code draws the bitmap for the ID_BACK_BUTTON, but the  button is now generated by emWIN
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BACK_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmmenu, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmmenu, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmmenu, 0, 0);
    */

    for (i = ID_BOWLER_1_BUTTON; i <= ID_BOWLER_8_BUTTON; i++){
        hItem = WM_GetDialogItem(pMsg->hWin, i);
        BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmNameBar, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmNameBar, 1, 1);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmNameBar, 0, 0);
    }

    hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWL_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmBowl, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmBowl, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmBowl, 0, 0);

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
 * Routine: IHandleMenu
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
static TBool IHandleMenu(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowMangager_Show_Window(MAIN_SCREEN);
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
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
    TUInt8 pos = (aID - ID_BOWLER_1_BUTTON) + 1;
    T_Bowler bowler;
    WM_HWIN hItem;
    
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if (G_RemoveBowlerMode){
            G_RemoveBowlerMode = EFalse;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_REMOVE_BOWLER_BUTTON);
            BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmRemoveBowler, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmRemoveBowler, 1, 1);
            BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmRemoveBowler, 0, 0);
        } else {
            if ( G_NumBowlers < MAX_NUM_BOWLERS){
                memset((void*)&bowler, 0, sizeof(T_Bowler));
                if ( EnterNameScreen(bowler.iName, pos)){
                    AddBowler(&bowler, G_NumBowlers);
                    G_NumBowlers++;
                }
            }
        }
        IUpdateFields(pMsg);
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleBowl
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
static TBool IHandleBowl(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if (G_NumBowlers > 0){
            WindowMangager_Show_Window(BOWLING);
        }
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
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
    TUInt8 count = 0;

    GetBowlers(G_BowlersList);

    if(G_RemoveBowlerMode){
        for ( i = 0; i < MAX_NUM_BOWLERS; i ++){
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWLER_1_BUTTON + i);
            if ( G_BowlersList[i].iName[0] != '\0'){
                sprintf(buttonText, "%s  -", G_BowlersList[i].iName);
                BUTTON_SetText(hItem, buttonText);
                WM_InvalidateWindow(hItem);
                count++;
            } else {
                BUTTON_SetText(hItem, "");
                WM_InvalidateWindow(hItem);
            }
        }
    } else {
        for ( i = 0; i < MAX_NUM_BOWLERS; i ++){
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWLER_1_BUTTON + i);
            if ( G_BowlersList[i].iName[0] != '\0'){
                sprintf(buttonText, "%s", G_BowlersList[i].iName);
                BUTTON_SetText(hItem, buttonText);
                WM_InvalidateWindow(hItem);
                count++;
            } else {
                BUTTON_SetText(hItem, "");
                WM_InvalidateWindow(hItem);
            }
        }
    }

    if ( count < MAX_NUM_BOWLERS && !G_RemoveBowlerMode){
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BOWLER_1_BUTTON + count);
        BUTTON_SetText(hItem, "+add bowler");
    }

}

/*---------------------------------------------------------------------------*
 * Routine:	_StandardPlayScreen.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _StandardPlayScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, StandardPlayScreenMapping);
        ISetButtonIcons(pMsg);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(StandardPlayScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
	case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;
    case WM_APP_GAINED_FOCUS:
    	G_Active = ETrue;
    	G_NumBowlers = 0;
#if MAGELLAN_DEBUG
        printf("Standard Play Active\n");
#endif
    	break;
    case WM_APP_LOST_FOCUS:
    	G_Active = EFalse;
#if MAGELLAN_DEBUG
        printf("Standard Play Inactive\n");
#endif
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	StandardPlayScreen_Create
 *---------------------------------------------------------------------------*/
/** Create the StandardPlayScreen. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN StandardPlayScreen_Create()
{
    return GUI_CreateDialogBox(_iStandardPlayScreenDialog, GUI_COUNTOF(_iStandardPlayScreenDialog), &_StandardPlayScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    StandardPlayScreen.c
 *-------------------------------------------------------------------------*/
