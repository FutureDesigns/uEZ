/*-------------------------------------------------------------------------*
 * File: QuickStart.c
 *-------------------------------------------------------------------------*/
/** @addtogroup QuickStart
 * @{
 *     @brief Implements Layout and functionality of QuickStart.c.
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
#include <string.h>
#include <stdio.h>

//uEZ and Application Includes
#include "Config_Build.h"
#include <uEZ.h>
#include "WindowManager.h"
#include "LookAndFeel.h"
#include "Fonts/Fonts.h"
#include "Graphics/Graphics.h"
#include "../Bowlers.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW           (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT       (GUI_ID_USER + 0x01)
#define ID_DECS_TEXT        (GUI_ID_USER + 0x02)
#define ID_BACK_BUTTON      (GUI_ID_USER + 0x03)  //Code modified by IMM - Changed ID_MENU_BUTTON to ID_BACK_BUTTON
#define ID_START_BUTTON     (GUI_ID_USER + 0x04)

#define ID_1_KEY            (GUI_ID_USER + 0x05)
#define ID_2_KEY            (GUI_ID_USER + 0x06)
#define ID_3_KEY            (GUI_ID_USER + 0x07)
#define ID_4_KEY            (GUI_ID_USER + 0x08)
#define ID_5_KEY            (GUI_ID_USER + 0x09)
#define ID_6_KEY            (GUI_ID_USER + 0x0A)
#define ID_7_KEY            (GUI_ID_USER + 0x0B)
#define ID_8_KEY            (GUI_ID_USER + 0x0C)
#define ID_9_KEY            (GUI_ID_USER + 0x0D)

#define SPACING             (10)

#define TITLE_TEXT_YSIZE    (40)

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)  //Code added by IMM - Used to define the size of the window for spacing out buttons in the layout
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT) // Code added by IMM - Used to define the size of the window for spacing out buttons in the layout


#define DECS_TEXT_YPOS      (50)
#define DECS_TEXT_YSIZE     (50)

/*
#define MENU_XSIZE          (80)//Edited by IMM - Changed the size for the larger bitmap
#define MENU_YSIZE          (60)//Edited by IMM - Changed the size for the larger bitmap
#define MENU_XPOS           (SPACING)
#define MENU_YPOS           (UEZ_LCD_DISPLAY_HEIGHT - SPACING - MENU_YSIZE)
*/

#define BACK_BUTTON_XSIZE       ((WINDOW_XSIZE/6))
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/8))
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - SPACING - BACK_BUTTON_YSIZE)

#define START_XSIZE         (134)
#define START_YSIZE         (124)
#define START_XPOS          (UEZ_LCD_DISPLAY_WIDTH - SPACING - START_XSIZE - (SPACING * 4))
#define START_YPOS          ((UEZ_LCD_DISPLAY_HEIGHT/2) - (START_YSIZE/2))

#define KEY_XSIZE           (78)
#define KEY_YSIZE           (74)
#define KEY_XPOS(n)         ((UEZ_LCD_DISPLAY_WIDTH/2) - (KEY_XSIZE/2) - KEY_XSIZE + ( n * (KEY_XSIZE + 1)))
#define KEY_YPOS(n)         ((UEZ_LCD_DISPLAY_HEIGHT/2) - (KEY_YSIZE/2) - KEY_YSIZE + ( n * (KEY_YSIZE + 1)))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleStart(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleMenu(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleDigit(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iQuickStartDialog[] = {
  //Function                Name    ID                      XP          YP     				 XS                       YS
  { WINDOW_CreateIndirect   , ""    , ID_WINDOW             , 0             , 0                 , UEZ_LCD_DISPLAY_WIDTH     , UEZ_LCD_DISPLAY_HEIGHT, 0, 0, 0},
  { TEXT_CreateIndirect     , ""    , ID_TITLE_TEXT         , 0             , 0                 , UEZ_LCD_DISPLAY_WIDTH     , TITLE_TEXT_YSIZE      , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
  { TEXT_CreateIndirect     , ""    , ID_DECS_TEXT          , 0             , DECS_TEXT_YPOS    , UEZ_LCD_DISPLAY_WIDTH     , DECS_TEXT_YSIZE       , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_BACK_BUTTON        ,  BACK_BUTTON_XPOS      ,  BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE , BACK_BUTTON_YSIZE ,          0, 0, 0},  //Code modified by IMM - Changed the ID from ID_MENU_BUTTON to ID_BACK_BUTTON and changed the size and spacing code
  { BUTTON_CreateIndirect   , ""    , ID_START_BUTTON       , START_XPOS    , START_YPOS        , START_XSIZE               , START_YSIZE           , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_1_KEY              , KEY_XPOS(0)   , KEY_YPOS(0)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_2_KEY              , KEY_XPOS(1)   , KEY_YPOS(0)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_3_KEY              , KEY_XPOS(2)   , KEY_YPOS(0)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_4_KEY              , KEY_XPOS(0)   , KEY_YPOS(1)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_5_KEY              , KEY_XPOS(1)   , KEY_YPOS(1)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_6_KEY              , KEY_XPOS(2)   , KEY_YPOS(1)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_7_KEY              , KEY_XPOS(0)   , KEY_YPOS(2)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_8_KEY              , KEY_XPOS(1)   , KEY_YPOS(2)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
  { BUTTON_CreateIndirect   , ""    , ID_9_KEY              , KEY_XPOS(2)   , KEY_YPOS(2)       , KEY_XSIZE                 , KEY_YSIZE              , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping QuickStartMapping[] = {
  { ID_WINDOW       ,""                 , GUI_BLACK         , GUI_BLACK     , &MAGELLAN_FONT_SMALL      , LAFSetupWindow    , 0},
  { ID_TITLE_TEXT   , "Quick Start"     , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_LARGE      , LAFSetupText      , 0},
  { ID_DECS_TEXT    , "select the number of bowlers\n"
                      "an icon will be automatically assigned to each bowler"
                                        , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupText      , 0},
  { ID_BACK_BUTTON  , "Back"            , GUI_GRAY          , GUI_BLACK     , &MAGELLAN_FONT_LARGE     , LAFSetupButton    , IHandleMenu},    //Code modified by IMM - Implement back button generated by emWIN in consistent format
  { ID_START_BUTTON , ""                , GUI_TRANSPARENT   , GUI_WHITE     , &MAGELLAN_FONT_SMALL      , LAFSetupButton    , IHandleStart},
  { ID_1_KEY        , "1"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_2_KEY        , "2"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_3_KEY        , "3"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_4_KEY        , "4"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_5_KEY        , "5"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_6_KEY        , "6"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_7_KEY        , "7"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_8_KEY        , "8"               , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , IHandleDigit},
  { ID_9_KEY        , ""                , GUI_GRAY          , GUI_WHITE     , &MAGELLAN_FONT_MEDIUM     , LAFSetupButton    , 0},
  {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static TUInt8 G_NumBowlers = 0;

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
    TUInt32 i = 0;
/*  Code removed by IMM - Used to draw a bitmap that we are not using anymore. Replacing with a consistently sized back button
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BACK_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmmenu, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmmenu, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmmenu, 0, 0);
*/
    hItem = WM_GetDialogItem(pMsg->hWin, ID_START_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmStart, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmStart, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmStart, 0, 0);
#if 0
    for ( i = ID_1_KEY; i < ID_9_KEY + 1; i ++){
        hItem = WM_GetDialogItem(pMsg->hWin, i);
        BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmKeypadKey, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmKeypadKeyPressed, 1, 1);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmKeypadKey, 0, 0);
    }
#endif
    hItem = WM_GetDialogItem(pMsg->hWin, ID_9_KEY);
    WM_DisableWindow(hItem);
    BUTTON_SetBkColor(hItem, BUTTON_CI_DISABLED, GUI_GRAY);
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleStart
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
static TBool IHandleStart(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        QuickStartBowlers(G_NumBowlers);
        WindowMangager_Show_Window(BOWLING);
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleMenu
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
 * Routine: IHandleDigit
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
static TBool IHandleDigit(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;
    char message [2];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        hItem = WM_GetDialogItem(pMsg->hWin, aID);
        BUTTON_GetText(hItem, message, 2);
        G_NumBowlers = atoi(message);
        IUpdateFields(pMsg);
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
    TUInt8 i;

    //Change the color of the selected number of bowlers
    for(i = 0; i < 8; i ++){
        hItem = WM_GetDialogItem(pMsg->hWin, ID_1_KEY + i);
        if ((i + 1) == G_NumBowlers){
            //Set to different color
            //BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
            BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_RED);
            BUTTON_SetTextColor(hItem, BUTTON_CI_PRESSED, GUI_RED);
        } else {
            //Set to default color
            //BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_GRAY);
            BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_WHITE);
            BUTTON_SetTextColor(hItem, BUTTON_CI_PRESSED, GUI_WHITE);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:	_QuickStart.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _QuickStartDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, QuickStartMapping);
        ISetButtonIcons(pMsg);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(QuickStartMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
	case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;
    case WM_APP_GAINED_FOCUS:
        G_NumBowlers = 0;
        IUpdateFields(pMsg);
#if MAGELLAN_DEBUG
        printf("Quick Start Active\n");
#endif
    	G_Active = ETrue;
    	break;
    case WM_APP_LOST_FOCUS:
#if MAGELLAN_DEBUG
        printf("Quick Start Inactive\n");
#endif
    	G_Active = EFalse;
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	QuickStart_Create
 *---------------------------------------------------------------------------*/
/** Create the QuickStart. This function should only be called by the
 *  window manager and never by user code.
 *
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN QuickStart_Create()
{
    return GUI_CreateDialogBox(_iQuickStartDialog, GUI_COUNTOF(_iQuickStartDialog), &_QuickStartDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    QuickStart.c
 *-------------------------------------------------------------------------*/
