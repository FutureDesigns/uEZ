/*-------------------------------------------------------------------------*
 * File: PictureFame_Settings.c
 *-------------------------------------------------------------------------*/
/** @addtogroup PictureFrame
 * @{
 *     @brief
 *
 *     How it works in detail goes here ....
 */
/*-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "string.h"
#include <uEZ.h>
#include "../GUICommon/LookAndFeel.h"
#include "../GUICommon/Keyboard.h"
#include "../GUICommon/Keypad.h"
#include <UEZTimeDate.h>
#include "GUIManager_PF.h"
#include <UEZTimeDate.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define     ID_WINDOW               (GUI_ID_USER + 0x00)
#define     ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define     ID_ALBUMNAME_TEXT       (GUI_ID_USER + 0x02)
#define     ID_ALBUMNAME_BUTTON     (GUI_ID_USER + 0x03)
#define     ID_TIME_TEXT            (GUI_ID_USER + 0x04)
#define     ID_TIME_BUTTON          (GUI_ID_USER + 0x05)
#define     ID_DATE_TEXT            (GUI_ID_USER + 0x06)
#define     ID_DATE_BUTTON          (GUI_ID_USER + 0x07)
#define     ID_CLOSE_BUTTON         (GUI_ID_USER + 0x08)
#define     ID_UPDATE_TIMER         (GUI_ID_USER + 0x09)

#define     UPDATE_TIME_MS          1000

#define     WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define     WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define     WINDOW_XPOS             ((UEZ_LCD_DISPLAY_WIDTH / 2) - (WINDOW_XSIZE / 2))
#define     WINDOW_YPOS             ((UEZ_LCD_DISPLAY_HEIGHT / 2) - (WINDOW_YSIZE / 2))

#define     TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define     TITLE_TEXT_YSIZE        (WINDOW_YSIZE / 10)

#define     SPACING                 5

#define     CLOSE_BUTTON_XSIZE      (UEZ_LCD_DISPLAY_WIDTH/11)
#define     CLOSE_BUTTON_YSIZE      (UEZ_LCD_DISPLAY_WIDTH/11)
#define     CLOSE_BUTTON_XPOS       (0)
#define     CLOSE_BUTTON_YPOS       (0)

#define     TEXT_BUTTONS_XSIZE      ((WINDOW_XSIZE / 2) - SPACING)
#define     TEXT_BUTTONS_YSIZE      (((WINDOW_YSIZE - TITLE_TEXT_YSIZE)) / 9)
#define     TEXT_BUTTONS_XPOS       (15)
#define     TEXT_BUTTONS_YPOS(n)    ((CLOSE_BUTTON_YSIZE + SPACING) + (n * (TEXT_BUTTONS_YSIZE + SPACING)))

#if(UEZ_DEFAULT_LCD == LCD_RES_480x272 || UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define     TITLE_TEXT_FONT         GUI_Font24B_ASCII
#define     TEXT_BUTTON_FONT        GUI_Font24_ASCII
#else
#define     TITLE_TEXT_FONT         GUI_Font32B_ASCII
#define     TEXT_BUTTON_FONT        GUI_Font32_ASCII
#endif

#define     DEFAULT_FONT_COLOR      GUI_BLACK
#define     DEFAULT_BK_COLOR        GUI_WHITE

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleAlbumName(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleTime(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleDate(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleClose(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _iPFSettingsDialog[] = {
  {WINDOW_CreateIndirect    , ""            , ID_WINDOW             , WINDOW_XPOS       , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE          , 0, 0, 0},
  {TEXT_CreateIndirect      , "Album Settings"  , ID_TITLE_TEXT         , 0                 , 0                     , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE      , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
  {TEXT_CreateIndirect      , "Album Name"      , ID_ALBUMNAME_TEXT     , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(0)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , TEXT_CF_VCENTER, 0, 0},
  {BUTTON_CreateIndirect    , ""                , ID_ALBUMNAME_BUTTON   , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(1)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , 0, 0, 0},
  {TEXT_CreateIndirect      , "Time"            , ID_TIME_TEXT          , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(2)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , TEXT_CF_VCENTER, 0, 0},
  {BUTTON_CreateIndirect    , ""                , ID_TIME_BUTTON        , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(3)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , 0, 0, 0},
  {TEXT_CreateIndirect      , "Date"            , ID_DATE_TEXT          , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(4)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , TEXT_CF_VCENTER, 0, 0},
  {BUTTON_CreateIndirect    , ""                , ID_DATE_BUTTON        , TEXT_BUTTONS_XPOS , TEXT_BUTTONS_YPOS(5)  , TEXT_BUTTONS_XSIZE    , TEXT_BUTTONS_YSIZE    , 0, 0, 0},
  {BUTTON_CreateIndirect    , "X"               , ID_CLOSE_BUTTON       , CLOSE_BUTTON_XPOS , CLOSE_BUTTON_YPOS     , CLOSE_BUTTON_XSIZE    , CLOSE_BUTTON_YSIZE    , 0, 0, 0},
};

static T_LAFMapping PFSettingsLayout[] = {
  { ID_WINDOW           , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupWindow  , 0},
  { ID_TITLE_TEXT       , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupText    , 0},
  { ID_ALBUMNAME_TEXT   , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupText    , 0},
  { ID_ALBUMNAME_BUTTON , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton  , IHandleAlbumName},
  { ID_TIME_TEXT        , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupText    , 0},
  { ID_TIME_BUTTON      , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton  , IHandleTime},
  { ID_DATE_TEXT        , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupText    , 0},
  { ID_DATE_BUTTON      , ""     ,DEFAULT_BK_COLOR, DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton  , IHandleDate},
  { ID_CLOSE_BUTTON     , "X"    ,GUI_RED         , DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupButton  , IHandleClose},
  {0},
};

static TBool G_Active = EFalse;
char G_AlbumName[50];
static WM_HTIMER G_UpdateTimer;

/*---------------------------------------------------------------------------*
 * Routine: IUpdateFields
 *---------------------------------------------------------------------------*/
/** Routine to disable buttons that will not have any actions.
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    char message[50];
    T_uezTimeDate currentTimeDate;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_ALBUMNAME_BUTTON);
    BUTTON_SetText(hItem, G_AlbumName);

    UEZTimeDateGet(&currentTimeDate);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TIME_BUTTON);
    sprintf(message, "%02d:%02d:%02d",
            currentTimeDate.iTime.iHour,
            currentTimeDate.iTime.iMinute,
            currentTimeDate.iTime.iSecond);
    BUTTON_SetText(hItem, message);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_DATE_BUTTON);
    sprintf(message, "%02d/%02d/%02d",
            currentTimeDate.iDate.iMonth,
            currentTimeDate.iDate.iDay,
            currentTimeDate.iDate.iYear);
    BUTTON_SetText(hItem, message);
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleAlbumName
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
static TBool IHandleAlbumName(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    char message[50];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        strcpy(message, G_AlbumName);
        if(Keyboard(message,
                "Enter the Album Name",
                50,
                KEYBOARD_USE_ALL)){
            strcpy(G_AlbumName, message);
        }
    }
    IUpdateFields(pMsg);
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleTime
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
static TBool IHandleTime(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    char message[10];
    T_uezTimeDate currentTimeDate;
    char hour[3];
    char minute[3];
    char second[3];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        //Stop the screen from updating while the time is being set
        WM_RestartTimer(G_UpdateTimer, 0xFFFFFFF);
        UEZTimeDateGet(&currentTimeDate);
        sprintf(message, "%02d%02d%02d",
                currentTimeDate.iTime.iHour,
                currentTimeDate.iTime.iMinute,
                currentTimeDate.iTime.iSecond);
        if(Keypad(message, 6,
                "Time (HH:MM:SS)",
                "##:##:##")){
            hour[0] = message[0];
            hour[1] = message[1];
            hour[2] = '\0';
            currentTimeDate.iTime.iHour = atoi(hour);

            minute[0] = message[2];
            minute[1] = message[3];
            minute[2] = '\0';
            currentTimeDate.iTime.iMinute = atoi(minute);

            second[0] = message[4];
            second[1] = message[5];
            second[2] = '\0';
            currentTimeDate.iTime.iSecond = atoi(second);
            UEZTimeDateSet(&currentTimeDate);
        }
        //update the screen now and reset the update time
        WM_RestartTimer(G_UpdateTimer, 1);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleDate
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
static TBool IHandleDate(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    char message[10];
    T_uezTimeDate currentTimeDate;
    char month[3];
    char day[3];
    char year[5];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        UEZTimeDateGet(&currentTimeDate);
        sprintf(message, "%02d%02d%02d",
                currentTimeDate.iDate.iMonth,
                currentTimeDate.iDate.iDay,
                currentTimeDate.iDate.iYear);
        if(Keypad(message, 8,
                "Date (MM/DD/YYYY)",
                "##/##/####")){
            month[0] = message[0];
            month[1] = message[1];
            month[2] = '\0';

            day[0] = message[2];
            day[1] = message[3];
            day[2] = '\0';

            year[0] = message[4];
            year[1] = message[5];
            year[2] = message[6];
            year[3] = message[7];
            year[4] = '\0';

            UEZTimeDateGet(&currentTimeDate);
            currentTimeDate.iDate.iMonth = atoi(month);
            currentTimeDate.iDate.iDay = atoi(day);
            currentTimeDate.iDate.iYear = atoi(year);
            UEZTimeDateSet(&currentTimeDate);
        }
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleClose
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
static TBool IHandleClose(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        GUIManager_Show_Window_PF(HOME_SCREEN);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: _PFSettingsDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _PFSettingsDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, PFSettingsLayout);

        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, UPDATE_TIME_MS, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(PFSettingsLayout, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_POST_PAINT:
        break;
    case WM_APP_GAINED_FOCUS:
        G_Active = ETrue;
        IUpdateFields(pMsg);
        WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
        break;
    case WM_APP_LOST_FOCUS:
        G_Active = EFalse;
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if(NCode == G_UpdateTimer) {
            if (G_Active){
                IUpdateFields(pMsg);
                WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
            }
        }
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine: PictureFrameSettings
 *---------------------------------------------------------------------------*/
/**
 *
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN PictureFrameSettings()
{
    return GUI_CreateDialogBox(_iPFSettingsDialog, GUI_COUNTOF(_iPFSettingsDialog), &_PFSettingsDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    PictureFame_Settings.c
 *-------------------------------------------------------------------------*/
