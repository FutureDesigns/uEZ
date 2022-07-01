/*-------------------------------------------------------------------------*
 * File: MainScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup MainScreen
 * @{
 *     @brief Implements Layout and functionality of MainScreen.c.
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
#include "Config_Build.h"
#include <uEZ.h>
#include "WindowManager.h"
#include "LookAndFeel.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"


/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_LABLE          (GUI_ID_USER + 0x01)  //Code Modified by IMM - Changed name form ID_LANE_TEXT to ID_TITLE_LABLE
#define ID_QUICK_START_TEXT     (GUI_ID_USER + 0x02)
#define ID_STANDARD_PLAY_TEXT   (GUI_ID_USER + 0x03)
#define ID_PRACTICE_TEXT        (GUI_ID_USER + 0x04)
#define ID__TEXT                (GUI_ID_USER + 0x05)
#define ID_QUICK_START_BUTTON   (GUI_ID_USER + 0x06)
#define ID_STANDARD_PLAY_BUTTON (GUI_ID_USER + 0x07)
#define ID_PRACTICE_BUTTON      (GUI_ID_USER + 0x08)
#define ID_QS_DEC_TEXT          (GUI_ID_USER + 0x09)
#define ID_SP_DEC_TEXT          (GUI_ID_USER + 0x0A)
#define ID_P_DEC_TEXT           (GUI_ID_USER + 0x0B)
#define ID_DESCRIPTION_TEXT     (GUI_ID_USER + 0x0C)  //Code Added by IMM - Add a text label to the bottom of the screen that describes the intent of the program
#define ID_EXIT_BUTTON          (GUI_ID_USER + 0x0D)  //Code Added by IMM - Exit button for the Main Screen

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)  //Code added by IMM - Used to define the size of the window for spacing out buttons in the layout
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT) //Code added by IMM - Used to define the size of the window for spacing out buttons in the layout

#define SPACING                 (10)
#define GAME_TEXT_XPOS(n)       (GAME_TEXT_XSIZE + SPACING + (n * ( GAME_TEXT_XSIZE + SPACING)))
#define GAME_TEXT_YPOS          (UEZ_LCD_DISPLAY_HEIGHT/4)
#define GAME_TEXT_XSIZE         ((UEZ_LCD_DISPLAY_WIDTH - (5 * SPACING))/5)
#define GAME_TEXT_YSIZE         (25)

#define GAME_BUTTONS_XPOS(n)    (GAME_BUTTONS_XSIZE + (SPACING*3) + (n * ( GAME_BUTTONS_XSIZE + (SPACING * 3))))
#define GAME_BUTTONS_YPOS       ((UEZ_LCD_DISPLAY_HEIGHT/4) + (2 * SPACING) + GAME_TEXT_YSIZE)
#define GAME_BUTTONS_XSIZE      (133)  //Code Added by IMM - Changed the size of the button
#define GAME_BUTTONS_YSIZE      (123)  //Code Added by IMM - Changed the size of the button

#define GAME_DESC_XPOS(n)       (GAME_TEXT_XSIZE + SPACING + (n * ( GAME_TEXT_XSIZE + SPACING)))
#define GAME_DESC_YPOS          ((UEZ_LCD_DISPLAY_HEIGHT/4) + (4 * SPACING) + GAME_TEXT_YSIZE + GAME_BUTTONS_YSIZE)
#define GAME_DESC_XSIZE         ((UEZ_LCD_DISPLAY_WIDTH - (5 * SPACING))/5)
#define GAME_DESC_YSIZE         (35)

#define BACK_BUTTON_XSIZE       ((WINDOW_XSIZE/6))
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/8))
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - SPACING - BACK_BUTTON_YSIZE)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleQuickStart(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleStandardPlay(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandlePractic(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);
#if RENAME_INI
static TBool IHandleExit(WM_MESSAGE * pMsg, int aNCode, int aID);
#endif
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iMainScreenDialog[] = {
    //Function                    Name                ID                            XP                        YP                      XS                       YS
    { WINDOW_CreateIndirect       , ""                , ID_WINDOW                 , 0                         , 0                     , UEZ_LCD_DISPLAY_WIDTH     , UEZ_LCD_DISPLAY_HEIGHT, 0, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_TITLE_LABLE            , 0                         , 0                     , UEZ_LCD_DISPLAY_WIDTH     , 35                    , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_DESCRIPTION_TEXT       , 0                         , 400                   , UEZ_LCD_DISPLAY_WIDTH     , 60                    , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},//Code Added by IMM - To add description label WARNING: CODE IS NOT SET TO SCALE TO SMALLER BOARDS!

    { TEXT_CreateIndirect         , ""                , ID_QUICK_START_TEXT       , GAME_TEXT_XPOS(0)         , GAME_TEXT_YPOS         , GAME_TEXT_XSIZE          , GAME_TEXT_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_STANDARD_PLAY_TEXT     , GAME_TEXT_XPOS(1)         , GAME_TEXT_YPOS         , GAME_TEXT_XSIZE          , GAME_TEXT_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_PRACTICE_TEXT          , GAME_TEXT_XPOS(2)         , GAME_TEXT_YPOS         , GAME_TEXT_XSIZE          , GAME_TEXT_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect       , ""                , ID_QUICK_START_BUTTON     , GAME_BUTTONS_XPOS(0)      , GAME_BUTTONS_YPOS      , GAME_BUTTONS_XSIZE       , GAME_BUTTONS_YSIZE    , 0, 0, 0},
    { BUTTON_CreateIndirect       , ""                , ID_STANDARD_PLAY_BUTTON   , GAME_BUTTONS_XPOS(1)      , GAME_BUTTONS_YPOS      , GAME_BUTTONS_XSIZE       , GAME_BUTTONS_YSIZE    , 0, 0, 0},
    { BUTTON_CreateIndirect       , ""                , ID_PRACTICE_BUTTON        , GAME_BUTTONS_XPOS(2)      , GAME_BUTTONS_YPOS      , GAME_BUTTONS_XSIZE       , GAME_BUTTONS_YSIZE    , 0, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_QS_DEC_TEXT            , GAME_DESC_XPOS(0)         , GAME_DESC_YPOS         , GAME_DESC_XSIZE          , GAME_DESC_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_SP_DEC_TEXT            , GAME_DESC_XPOS(1)         , GAME_DESC_YPOS         , GAME_DESC_XSIZE          , GAME_DESC_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect         , ""                , ID_P_DEC_TEXT             , GAME_DESC_XPOS(2)         , GAME_DESC_YPOS         , GAME_DESC_XSIZE          , GAME_DESC_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    #if RENAME_INI
    {BUTTON_CreateIndirect        , "Back"            , ID_EXIT_BUTTON            , BACK_BUTTON_XPOS          , BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE     , BACK_BUTTON_YSIZE , 0, 0, 0},
    #endif
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping MainScreenMapping[] = {
    { ID_WINDOW               ,""                         , GUI_BLACK           , GUI_BLACK , &MAGELLAN_FONT_SMALL  , LAFSetupWindow    , 0},
    { ID_TITLE_LABLE            , "Bowling"                 , GUI_TRANSPARENT     , GUI_WHITE , &MAGELLAN_FONT_LARGE  , LAFSetupText      , 0},    //Code added by IMM - Changed the Heading label to show the name of the program
    { ID_DESCRIPTION_TEXT     , "The intent of this program is to show how uEZ can be used\nto provide keyboard and data entry capabilities", GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupText      , 0},//Code added by IMM - Add Description Label to show the intent of the program
    { ID_QUICK_START_TEXT     , "Quick Start"             , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_STANDARD_PLAY_TEXT   , "Standard Play"           , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_PRACTICE_TEXT        , "Practice"                , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupText      , 0},
    { ID_QUICK_START_BUTTON   , ""                        , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupButton    , IHandleQuickStart},
    { ID_STANDARD_PLAY_BUTTON , ""                        , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupButton    , IHandleStandardPlay},
    { ID_PRACTICE_BUTTON      , ""                        , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupButton    , IHandlePractic},
    { ID_QS_DEC_TEXT          , "The fastest\nway to fun" , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupText      , 0},
    { ID_SP_DEC_TEXT          , "For the bowler\nin you"  , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupText      , 0},
    { ID_P_DEC_TEXT           , "It makes\nperfect"       , GUI_TRANSPARENT    , GUI_WHITE , &MAGELLAN_FONT_SMALL  , LAFSetupText      , 0},
    #if RENAME_INI
    { ID_EXIT_BUTTON          , "Back"                    , GUI_GRAY           , GUI_WHITE , &MAGELLAN_FONT_LARGE  , LAFSetupButton    , IHandleExit},
    #endif
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;

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

    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_ENABLED);
    BUTTON_SetSkinFlexProps(&pressedButtonProps, BUTTON_SKINFLEX_PI_PRESSED);
    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_FOCUSSED);
    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_DISABLED);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_QUICK_START_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmQuickStart, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmQuickStart, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmQuickStart, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_STANDARD_PLAY_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmStandardPlay, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmStandardPlay, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmStandardPlay, 0, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_PRACTICE_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmPracticeDisabled, 0, 0);//Edited by IMM - Changed the bitmap to add a Practice disabled button becuase it is not implemented yet.
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmPracticeDisabled, 1, 1);  //Edited by IMM - Changed the bitmap to add a Practice disabled button becuase it is not implemented yet.
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmPracticeDisabled, 0, 0); //Edited by IMM - Changed the bitmap to add a Practice disabled button becuase it is not implemented yet.
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleQuickStart
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
static TBool IHandleQuickStart(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowMangager_Show_Window(QUICK_START);
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleStandardPlay
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
static TBool IHandleStandardPlay(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowMangager_Show_Window(STANDARD_PLAY);
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandlePractic
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
static TBool IHandlePractic(WM_MESSAGE * pMsg, int aNCode, int aID)
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
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields(WM_MESSAGE * pMsg)
{

}

/*---------------------------------------------------------------------------*
 * Routine:    _MainScreen.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _MainScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
    WM_HWIN hItem;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        hItem = pMsg->hWin;
        WM_SetHasTrans(hItem);
        LAFSetup(pMsg->hWin, ID_WINDOW, MainScreenMapping);
        ISetButtonIcons(pMsg);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_STANDARD_PLAY_BUTTON);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(MainScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
#if 0
        GUI_DrawBitmap(&bmlogo, (UEZ_LCD_DISPLAY_WIDTH/2) - (bmlogo.XSize/2),
                UEZ_LCD_DISPLAY_HEIGHT - bmlogo.YSize - SPACING);
#endif
        break;
    case WM_APP_GAINED_FOCUS:
        G_Active = ETrue;
        ClearBowlers();
#if MAGELLAN_DEBUG
        printf("Main Screen Active\n");
#endif
        break;
    case WM_APP_LOST_FOCUS:
#if MAGELLAN_DEBUG
        printf("Main Screen Inactive\n");
#endif
        G_Active = EFalse;
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}
/*---------------------------------------------------------------------------*
 * Routine:    IHandleExit
 *---------------------------------------------------------------------------*/
/** Exits the program
 *
 *
 *  @param [in] p_choice    void
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
#if RENAME_INI
static TBool IHandleExit(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    T_uezDevice watchdog;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
      if(UEZWatchdogOpen("Watchdog", &watchdog) == UEZ_ERROR_NONE){
        UEZWatchdogSetMaxTime(watchdog, 100);
        UEZWatchdogSetMinTime(watchdog, 1);
        UEZWatchdogStart(watchdog);
        UEZWatchdogFeed(watchdog);
        UEZWatchdogTrip(watchdog);
      }
    }
    return EFalse;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:    MainScreen_Create
 *---------------------------------------------------------------------------*/
/** Create the MainScreen. This function should only be called by the
 *  window manager and never by user code.
 *
 *  @param [in] p_choice    void
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN MainScreen_Create()
{
    return GUI_CreateDialogBox(_iMainScreenDialog, GUI_COUNTOF(_iMainScreenDialog), &_MainScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    MainScreen.c
 *-------------------------------------------------------------------------*/
