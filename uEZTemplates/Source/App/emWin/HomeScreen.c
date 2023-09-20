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
#include <uEZPlatform.h>
#include <uEZWatchdog.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <uEZResourceCache.h>

#if (INCLUDE_UEZ_RESOURCE_EXAMPLE==1)
#include "HImg.h"
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_LOGO                 (GUI_ID_USER + 0x02)
#define ID_BUTTON_RIGHT_TOP     (GUI_ID_USER + 0x03) // The top button on the right side
#define ID_BUTTON_RIGHT_MIDDLE  (GUI_ID_USER + 0x04) // The hidden middle button on the right side
#define ID_BUTTON_RIGHT_BOTTOM  (GUI_ID_USER + 0x05) // The bottom button on the right side
#define ID_BUTTON_LEFT_BACK     (GUI_ID_USER + 0x06) // The back button on the left side
#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x07)

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#else
#define SPACING                 (5)
#endif

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define LOGO_XSIZE              (160) //Taken from image resolution
#define LOGO_YSIZE              (72)
#define LOGO_XPOS               (SPACING*10)
#define LOGO_YPOS               ((WINDOW_YSIZE/2) - (LOGO_YSIZE/2))

#define BUTTON_XSIZE            ((WINDOW_XSIZE/2) - (2 * SPACING))
#define BUTTON_YSIZE            ((WINDOW_YSIZE / 4) - (4 * SPACING)) //Allow for 4 vertical buttons on the left side of the screen
#define BUTTON_XPOS             ((WINDOW_XSIZE/2) + SPACING)
#define BUTTON_YPOS(n)          ((SPACING + TITLE_TEXT_YSIZE) + (n * (BUTTON_YSIZE + SPACING)))

#define BUTTON_ENABLE_TIME_MS   2000 // time before exit button is pressable.

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleButtonRightTop(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleButtonRightMiddle(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleButtonRightBottom(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleButtonLeftBottom(WM_MESSAGE * pMsg, int aNCode, int aID);

static void IUpdateFields(WM_MESSAGE * pMsg);
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iHomeScreenDialog[] = {
    //Function                  Name            ID                  XP                      YP                      XS                       YS
    { WINDOW_CreateIndirect     , ""            , ID_WINDOW         , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE      , 0, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_TITLE_TEXT     , TITLE_TEXT_XPOS       , TITLE_TEXT_YPOS       , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE  , TEXT_CF_HCENTER| TEXT_CF_VCENTER, 0, 0},
    { IMAGE_CreateIndirect      , ""            , ID_LOGO           , LOGO_XPOS             , LOGO_YPOS             , LOGO_XSIZE            , LOGO_YSIZE        , 0, 0, 0},

    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_TOP   , BUTTON_XPOS       , BUTTON_YPOS(1)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_MIDDLE, BUTTON_XPOS       , BUTTON_YPOS(2)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_BOTTOM, BUTTON_XPOS       , BUTTON_YPOS(3)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_LEFT_BACK, SPACING              , (WINDOW_YSIZE-SPACING-BUTTON_YSIZE), BUTTON_XSIZE-(14*SPACING), BUTTON_YSIZE      , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping HomeScreenMapping[] = {
    { ID_WINDOW         , ""    , GUI_BLACK , GUI_WHITE , &FONT_SMALL, LAFSetupWindow , 0},
    { ID_LOGO    , ""    , GUI_BLACK , GUI_BLACK , &FONT_SMALL, 0 , 0},
    { ID_TITLE_TEXT     , "Project Maker"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_BUTTON_RIGHT_TOP   , "Sensors"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightTop},
    { ID_BUTTON_RIGHT_MIDDLE, "GUI Builder"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightMiddle},
    { ID_BUTTON_RIGHT_BOTTOM, "Time/Date"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightBottom},
    { ID_BUTTON_LEFT_BACK, "Back/Reset"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonLeftBottom},
    {0},
};

static WM_HTIMER G_UpdateTimer;

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightTop
 *---------------------------------------------------------------------------*/
/** Show the Sensor screen
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleButtonRightTop(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowManager_Show_Window(SENSOR_SCREEN);

    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightMiddle
 *---------------------------------------------------------------------------*/
/** TODO can show a different GUI here, such as from the GUI Builder.
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleButtonRightMiddle(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
		// TODO can show a different window here
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightBottom
 *---------------------------------------------------------------------------*/
/** Show the time date setting page
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleButtonRightBottom(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowManager_Show_Window(TIMEDATE_SCREEN);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonLeftBottom
 *---------------------------------------------------------------------------*/
/** Reset the unit and return to bootloader if applicable
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleButtonLeftBottom(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
      UEZPlatform_Watchdog_Require();
      T_uezDevice watchdog;

      // Force a restart by tripping the watchdog
      UEZWatchdogOpen("Watchdog", &watchdog);
      if (!UEZIsResetFromWatchdog(watchdog)) {
          //G_clearWatchdogBit = 1;
          }
      UEZWatchdogSetMaxTime(watchdog, 100);
      UEZWatchdogStart(watchdog);
      UEZWatchdogTrip(watchdog);

      //UEZPlatform_System_Reset();
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
 * Routine: ISetupButtonsOptions
 *---------------------------------------------------------------------------*/
/** Setup any non standard button options like callbacks for graphics.
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void ISetLogo(WM_MESSAGE * pMsg)
{
    IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_LOGO), &bmlogo);
}

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
    WM_HWIN hItem;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, HomeScreenMapping);
        ISetLogo(pMsg);
        
        // This will set the newer emWin mode to help prevent accidental clicks
        // See menual 6.2.5.1.1 BUTTON_REACT_ON_LEVEL
        // See manual "Example for an unwanted BUTTON click"
        BUTTON_SetReactOnLevel();
		
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RIGHT_MIDDLE);
        WM_HideWindow(hItem); // Hide the middle button until we have a GUI screen here.

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
        //TEXT_SetText(hItem, "Demo"); // Example how to change the title text at run time.
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LEFT_BACK);        
        ButtonDisablePresses(hItem); // disable being able to press the button

        // For this button additionally use a timer as user can still accidentally press it on screen switch.
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, BUTTON_ENABLE_TIME_MS, 0);
		
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(HomeScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if(G_Active){
        
        }
        if (NCode == G_UpdateTimer) { // timer expired, enable the button
          hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LEFT_BACK);
          ButtonEnablePresses(hItem);
        }

      break;
    case WM_POST_PAINT:
		
        break;
    case WM_APP_GAINED_FOCUS:
        G_Active = ETrue;
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LEFT_BACK);
        // Disable the button every time we switch to the screen
        ButtonDisablePresses(hItem); // Use timer to enable button after timeout
        WM_RestartTimer(G_UpdateTimer, BUTTON_ENABLE_TIME_MS);
        break;
    case WM_APP_LOST_FOCUS:
        G_Active = EFalse;
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LEFT_BACK);
        // Disable the button every time we switch away from the screen.
        ButtonDisablePresses(hItem);
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:    HomeScreen_Create
 *---------------------------------------------------------------------------*/
/** Create the HomeScreen. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN HomeScreen_Create()
{
#if (INCLUDE_UEZ_RESOURCE_EXAMPLE==1)
    // Example to load a single image from the resource file
    T_uezDevice resources;
    void *p_image;
    if (UEZResourceCacheOpen("Resources", &resources)) {
        UEZFailureMsg("Resources error!");
    }
    if (UEZResourceCacheLockByName(resources, "TITLE.TGA", &p_image)) {
        UEZFailureMsg("Resource TITLE.TGA not found!");
    }
    
    /* This example loads a Targa file. For emWin usage normally you would use
     * BMP/PNG/JPEG/GIF from emWin image converter. See /emWin/IMAGE.h */
    SUIDrawIcon(p_image, (DISPLAY_WIDTH-320)/2, (DISPLAY_HEIGHT-240)/2);

    // In emWin here are some of the functions for loading common image formats.
    //IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_LOGO), &bmlogo);
    //IMAGE_SetGIF(IMAGE_Handle hObj, const void * pData, U32 FileSize);
    //IMAGE_SetJPEG(IMAGE_Handle hObj, const void * pData, U32 FileSize);
    //IMAGE_SetPNG(IMAGE_Handle hObj, const void * pData, U32 FileSize);

    UEZTaskDelay(17);
    
    // Turn on backlight after initializing first screen to prevent screen tear
    WindowManager_BackLight_On(255);
    
    UEZTaskDelay(5000); // Delay to show icon before loading home screen.
#endif

    return GUI_CreateDialogBox(_iHomeScreenDialog, GUI_COUNTOF(_iHomeScreenDialog), &_HomeScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    HomeScreen.c
 *-------------------------------------------------------------------------*/
