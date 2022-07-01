/*-------------------------------------------------------------------------*
 * File: SecondScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup SecondScreen
 * @{
 *     @brief Implements Layout and functionality of SecondScreen.c.
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
#define ID_TEMP_TEXT            (GUI_ID_USER + 0x02)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x03)

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)
   
#define TEMP_TEXT_XSIZE         (WINDOW_XSIZE/3)
#define TEMP_TEXT_YSIZE         ((WINDOW_YSIZE/5))
#define TEMP_TEXT_XPOS          (WINDOW_XSIZE/3)
#define TEMP_TEXT_YPOS          ((WINDOW_YSIZE/10)*4)

#define BACK_BUTTON_XSIZE       (WINDOW_XSIZE/5)
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/6))
#define BACK_BUTTON_XPOS        (0)
#define BACK_BUTTON_YPOS        ((WINDOW_YSIZE/6)*5)

// Define spacing and font size based on screen resolution
#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#define FONTSIZE                &FONT_LARGE
#else
#define SPACING                 (5)
#define FONTSIZE                &FONT_SMALL
#endif


/*---------------------------------------------------------------------------*
 * Global Data:
 *---------------------------------------------------------------------------*/
int G_WhichWindow; // Variable to hold the ID of the active window
static TBool G_Active = EFalse; // Active Flag, tell the dialog when it receives messages that the screen is in the foreground

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
void UpdateTemp(char *myString);
static TBool IHandleBackButton(WM_MESSAGE * pMsg, int aNCode, int aID);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iSecondScreenDialog[] = {
    //Function                  Name            ID                  XP                      YP                      XS                      YS
    { WINDOW_CreateIndirect     , ""            , ID_WINDOW         , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE         , 0, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_TITLE_TEXT     , TITLE_TEXT_XPOS       , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE     , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_TEMP_TEXT      , TEMP_TEXT_XPOS        , TEMP_TEXT_YPOS        , TEMP_TEXT_XSIZE       , TEMP_TEXT_YSIZE      , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BACK_BUTTON    , BACK_BUTTON_XPOS      , BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE     , BACK_BUTTON_YSIZE    , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping SecondScreenMapping[] = {
    // ID                       Text                                Background Color        Text Color              Font Size               Setup Function         Handle Function
    { ID_WINDOW                 , ""                                , GUI_BLACK             , GUI_WHITE             , &FONT_SMALL           , LAFSetupWindow       , 0},
    { ID_TITLE_TEXT             , "Temperature Screen"              , GUI_BLACK             , GUI_WHITE             , &FONT_LARGE           , LAFSetupText         , 0},
    { ID_TEMP_TEXT              , "Temp"                            , GUI_BLACK             , GUI_WHITE             , &FONT_LARGE           , LAFSetupText         , 0},
    { ID_BACK_BUTTON            , "Back"                            , GUI_GRAY              , GUI_BLACK             , &FONT_LARGE           , LAFSetupButton       , (TBool (*)(WM_MESSAGE *, int, int)) IHandleBackButton},
    {0},
};

/*-------------------------------------------------------------------------*
 * Routine:    UpdateTemp
 *-------------------------------------------------------------------------*
 * Description:
 *      Update the temperature text with the passed in string.
 *-------------------------------------------------------------------------*/
void UpdateTemp(char *myString)
{
    TEXT_SetText(WM_GetDialogItem(G_WhichWindow, ID_TEMP_TEXT), (const char*)myString);
}


/*-------------------------------------------------------------------------*
 * Routine:    IHandleBackButton
 *-------------------------------------------------------------------------*
 * Description:
 *      Change to the home screen when the back button is pressed.
 *-------------------------------------------------------------------------*/
static TBool IHandleBackButton(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowManager_Show_Window(HOME_SCREEN);
    }
    return EFalse;
}


/*---------------------------------------------------------------------------*
 * Routine:    _SecondScreenDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _SecondScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
    
    switch (pMsg->MsgId) {
    
    // Initialize the dialog.  
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, SecondScreenMapping);
        G_WhichWindow = pMsg->hWin; // Create a global ID for the active window to use in the update function
        break;
        
    // Notify the parent object that an event has occured    
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(SecondScreenMapping, pMsg, NCode, Id)){
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
 * Routine:    SecondScreen_Create
 *---------------------------------------------------------------------------*/
/** Wrapper to create the SecondScreen. This is an emWin function.
 *  This function should only be called by the window manager and never 
 *  by user code.
 *      
 *  @param [in] p_choice    void
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN SecondScreen_Create()
{
    return GUI_CreateDialogBox(_iSecondScreenDialog, GUI_COUNTOF(_iSecondScreenDialog), &_SecondScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    SecondScreen.c
 *-------------------------------------------------------------------------*/