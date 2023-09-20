/*-------------------------------------------------------------------------*
 * File: Sensor.c
 *-------------------------------------------------------------------------*/
/** @addtogroup Sensor
 * @{
 *     @brief Implements Layout and functionality of Sensor.c.
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
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"
#include "../Sensor_Callbacks.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x02)
#define ID_INT_ACCEL_LABLE       (GUI_ID_USER + 0x03)
#define ID_INT_ACCEL_TEXT        (GUI_ID_USER + 0x04)
#define ID_INT_TEMP_LABLE       (GUI_ID_USER + 0x05)
#define ID_INT_TEMP_TEXT        (GUI_ID_USER + 0x06)
#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x07)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#define DIVIDER                 (10)
#else
#define SPACING                 (5)
#define DIVIDER                 (5)
#endif

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define BACK_BUTTON_XSIZE       (WINDOW_XSIZE/DIVIDER)
#define BACK_BUTTON_YSIZE       (WINDOW_YSIZE/DIVIDER)
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - BACK_BUTTON_YSIZE - SPACING)

#define TEXT_XSIZE              ((WINDOW_XSIZE - (8 * SPACING))/7)
#define TEXT_YSIZE              ((WINDOW_YSIZE - TITLE_TEXT_YSIZE - (5 * SPACING))/4)
#define TEXT_XPOS(n)            (SPACING + (n * (SPACING + TEXT_XSIZE)))
#define TEXT_YPOS(n)            (((WINDOW_YSIZE - TITLE_TEXT_YSIZE)/2 - (TEXT_YSIZE/2)) + ( n * (TEXT_YSIZE + SPACING)))

#define UPDATE_TIME_MS          (100)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);
static WM_HTIMER G_UpdateTimer = 0;

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iSensorDialog[] = {
    //Function                  Name        ID                  XP                      YP                      XS                       YS
    { WINDOW_CreateIndirect     , ""        , ID_WINDOW         , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE      , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_TITLE_TEXT     , TITLE_TEXT_XPOS       , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE  , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""        , ID_BACK_BUTTON    , BACK_BUTTON_XPOS      , BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE     , BACK_BUTTON_YSIZE , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_INT_ACCEL_LABLE , TEXT_XPOS(0)         , TEXT_YPOS(0)          , TEXT_XSIZE * 4        , TEXT_YSIZE        , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_INT_ACCEL_TEXT  , TEXT_XPOS(3)         , TEXT_YPOS(0)          , TEXT_XSIZE * 5        , TEXT_YSIZE        , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_INT_TEMP_LABLE , TEXT_XPOS(0)          , TEXT_YPOS(1)          , TEXT_XSIZE * 4        , TEXT_YSIZE        , 0, 0, 0},
    { TEXT_CreateIndirect       , ""        , ID_INT_TEMP_TEXT  , TEXT_XPOS(4)          , TEXT_YPOS(1)          , TEXT_XSIZE * 2        , TEXT_YSIZE        , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping SensorMapping[] = {
    { ID_WINDOW         ,""     , GUI_BLACK , GUI_WHITE , &FONT_LARGE , LAFSetupWindow, 0},
    { ID_TITLE_TEXT     , "Sensor"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_BACK_BUTTON    , "Back", GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleBack},
    { ID_INT_ACCEL_LABLE , "Accelerometer:"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_INT_ACCEL_TEXT  , ""    , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_INT_TEMP_LABLE , "Internal Temp:"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_INT_TEMP_TEXT  , ""    , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;

/*---------------------------------------------------------------------------*
 * Routine: IHandleBack
 *---------------------------------------------------------------------------*/
/** Return to the home screen
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID			ID of widget that caused the event.
 *  @return					The emWin Handle to this window
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
    T_SensorSettings SensorSettings;

    Sensor_GetSettings(&SensorSettings);

    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_INT_ACCEL_TEXT), SensorSettings.iAccelerometer);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_INT_TEMP_TEXT), SensorSettings.iBoardTemp);
}

/*---------------------------------------------------------------------------*
 * Routine:	_Sensor.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _SensorDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, SensorMapping);
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, 0, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(SensorMapping, pMsg, NCode, Id)){
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
 * Routine:	Sensor_Create
 *---------------------------------------------------------------------------*/
/** Create the Sensor. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN Sensor_Create()
{
    return GUI_CreateDialogBox(_iSensorDialog, GUI_COUNTOF(_iSensorDialog), &_SensorDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    Sensor.c
 *-------------------------------------------------------------------------*/
