/*-------------------------------------------------------------------------*
 * File: TimeDate.c
 *-------------------------------------------------------------------------*/
/** @addtogroup TimeDate
 * @{
 *     @brief Implements Layout and functionality of TimeDate.c.
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
#include <stdlib.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"
#include "../TimeDate_Callbacks.h"
#include "Keypad.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x02)
#define ID_TIME_BUTTON          (GUI_ID_USER + 0x03)
#define ID_DATE_BUTTON          (GUI_ID_USER + 0x04)
#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x05)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#else
#define SPACING                 (5)
#endif

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define DIVIDER                 (10)
#else
#define DIVIDER                 (5)
#endif

#define BACK_BUTTON_XSIZE       (WINDOW_XSIZE/DIVIDER)
#define BACK_BUTTON_YSIZE       (WINDOW_YSIZE/DIVIDER)
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - BACK_BUTTON_YSIZE - SPACING)

#define BUTTON_XSIZE            ((WINDOW_XSIZE/3))
#define BUTTON_YSIZE            ((WINDOW_YSIZE - TITLE_TEXT_YSIZE - (6 * SPACING)) / 5)
#define BUTTON_XPOS             ((WINDOW_XSIZE/2) - (BUTTON_XSIZE/2))
#define BUTTON_YPOS(n)          (TITLE_TEXT_YSIZE + SPACING + (n * (BUTTON_YSIZE + SPACING)))

#define UPDATE_TIME_MS          (1000)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleDateTime(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iTimeDateDialog[] = {
    //Function                  Name        ID                  XP                      YP                      XS                       YS
    { WINDOW_CreateIndirect     , ""        , ID_WINDOW         , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE      , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_TITLE_TEXT     , TITLE_TEXT_XPOS       , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE  , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BACK_BUTTON    , BACK_BUTTON_XPOS      , BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE     , BACK_BUTTON_YSIZE , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_TIME_BUTTON    , BUTTON_XPOS           , BUTTON_YPOS(1)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_DATE_BUTTON    , BUTTON_XPOS           , BUTTON_YPOS(2)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping TimeDateMapping[] = {
    { ID_WINDOW         ,""     , GUI_BLACK , GUI_WHITE , &FONT_LARGE , LAFSetupWindow, 0},
    { ID_TITLE_TEXT     , "Time/Date Settings"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_BACK_BUTTON    , "Back", GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleBack},
    { ID_TIME_BUTTON    , ""    , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleDateTime},
    { ID_DATE_BUTTON    , ""    , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleDateTime},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static WM_HTIMER G_UpdateTimer = 0;

/*---------------------------------------------------------------------------*
 * Routine: IHandleBack
 *---------------------------------------------------------------------------*/
/** Return to the home screen
 *      
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowManager_Show_Window(HOME_SCREEN);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleDateTime
 *---------------------------------------------------------------------------*/
/** Handle setting the date and time
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleDateTime(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    T_TimeDateSettings TimeDate;
    char message[25];
    char a[3], b[3], c[5];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        TimeDate_GetSettings(&TimeDate);

        if(aID == ID_TIME_BUTTON){
            sprintf(message, "%02d%02d%02d",  TimeDate.iTimeDate.iTime.iHour,
                                                TimeDate.iTimeDate.iTime.iMinute,
                                                TimeDate.iTimeDate.iTime.iSecond);
            if(Keypad(message, 6, "Enter the Time", "##:##:##")){
                a[0] = message[0];
                a[1] = message[1];
                a[2] = '\0';

                b[0] = message[2];
                b[1] = message[3];
                b[2] = '\0';

                c[0] = message[4];
                c[1] = message[5];
                c[2] = '\0';

                TimeDate.iTimeDate.iTime.iHour = atoi(a);
                if(TimeDate.iTimeDate.iTime.iHour > 23)
                    TimeDate.iTimeDate.iTime.iHour = 23;
                TimeDate.iTimeDate.iTime.iMinute = atoi(b);
                if(TimeDate.iTimeDate.iTime.iMinute > 59)
                    TimeDate.iTimeDate.iTime.iMinute = 59;
                TimeDate.iTimeDate.iTime.iSecond = atoi(c);
                if(TimeDate.iTimeDate.iTime.iSecond > 59)
                    TimeDate.iTimeDate.iTime.iSecond = 59;
                TimeDate_SetSettings(&TimeDate);
            }
        } else if (aID == ID_DATE_BUTTON){
            sprintf(message, "%02d%02d%04d",  TimeDate.iTimeDate.iDate.iMonth,
                                                TimeDate.iTimeDate.iDate.iDay,
                                                TimeDate.iTimeDate.iDate.iYear);
            if(Keypad(message, 8, "Enter the Date", "##/##/####")){
                a[0] = message[0];
                a[1] = message[1];
                a[2] = '\0';

                b[0] = message[2];
                b[1] = message[3];
                b[2] = '\0';

                c[0] = message[4];
                c[1] = message[5];
                c[2] = message[6];
                c[3] = message[7];
                c[4] = '\0';

                TimeDate_GetSettings(&TimeDate);

                TimeDate.iTimeDate.iDate.iMonth = atoi(a);
                TimeDate.iTimeDate.iDate.iDay = atoi(b);
                TimeDate.iTimeDate.iDate.iYear = atoi(c);
                TimeDate_SetSettings(&TimeDate);
            }
        }
        IUpdateFields(pMsg);
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
    T_TimeDateSettings TimeDate;
    char message[25];

    TimeDate_GetSettings(&TimeDate);

    sprintf(message, "%02d:%02d:%02d",  TimeDate.iTimeDate.iTime.iHour,
                                        TimeDate.iTimeDate.iTime.iMinute,
                                        TimeDate.iTimeDate.iTime.iSecond);
    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_TIME_BUTTON), message);

    sprintf(message, "%02d/%02d/%04d",  TimeDate.iTimeDate.iDate.iMonth,
                                        TimeDate.iTimeDate.iDate.iDay,
                                        TimeDate.iTimeDate.iDate.iYear);
    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_DATE_BUTTON), message);
}

/*---------------------------------------------------------------------------*
 * Routine:	_TimeDate.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _TimeDateDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, TimeDateMapping);
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, 0, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(TimeDateMapping, pMsg, NCode, Id)){
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
        if (NCode == G_UpdateTimer) {
            WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
            if(G_Active){
                IUpdateFields(pMsg);
            }
        }
        break;
    case WM_APP_GAINED_FOCUS:
    	G_Active = ETrue;
    	break;
    case WM_APP_LOST_FOCUS:
    	G_Active = EFalse;
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	TimeDate_Create
 *---------------------------------------------------------------------------*/
/** Create the TimeDate. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN TimeDate_Create()
{
    return GUI_CreateDialogBox(_iTimeDateDialog, GUI_COUNTOF(_iTimeDateDialog), &_TimeDateDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    TimeDate.c
 *-------------------------------------------------------------------------*/
