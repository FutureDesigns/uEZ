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
#include <TSNoiseDetect.h>
#include "Storage_Callbacks.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_LOGO                 (GUI_ID_USER + 0x02)
#define ID_BUTTON_RIGHT_TOP     (GUI_ID_USER + 0x03) // The top button on the right side
#define ID_BUTTON_RIGHT_MIDDLE  (GUI_ID_USER + 0x04) // The middle button on the right side
#define ID_BUTTON_RIGHT_BOTTOM  (GUI_ID_USER + 0x05) // The bottom button on the right side

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

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleButtonRightTop(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleButtonRightMiddle(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleButtonRightBottom(WM_MESSAGE * pMsg, int aNCode, int aID);

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

    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_TOP   , BUTTON_XPOS           , BUTTON_YPOS(1)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_MIDDLE, BUTTON_XPOS           , BUTTON_YPOS(2)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BUTTON_RIGHT_BOTTOM, BUTTON_XPOS           , BUTTON_YPOS(3)        , BUTTON_XSIZE          , BUTTON_YSIZE      , 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping HomeScreenMapping[] = {
    { ID_WINDOW         , ""    , GUI_BLACK , GUI_WHITE , &FONT_SMALL, LAFSetupWindow , 0},
    { ID_LOGO    , ""    , GUI_BLACK , GUI_BLACK , &FONT_SMALL, 0 , 0},
    { ID_TITLE_TEXT     , "Test Application"
                                , GUI_BLACK , GUI_WHITE , &FONT_LARGE, LAFSetupText   , 0},
    { ID_BUTTON_RIGHT_TOP   , "TS Test"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightTop},
    { ID_BUTTON_RIGHT_MIDDLE, "Slideshow"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightMiddle},
    { ID_BUTTON_RIGHT_BOTTOM, "Storage Screen"
                                , GUI_GRAY  , GUI_BLACK , &FONT_LARGE, LAFSetupButton , (TBool (*)(WM_MESSAGE *, int, int))IHandleButtonRightBottom},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightTop
 *---------------------------------------------------------------------------*/
/** Show the Touchscreen Test screen
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
		TS_NoiseDetect();
	}
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightMiddle
 *---------------------------------------------------------------------------*/
/** Show slideshow screen on supported uEZGUIs
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
#if (UEZ_PROCESSOR == NXP_LPC1788)
		// no slideshow on platforms without QSPI/OSPI
#endif
#if (UEZ_PROCESSOR == NXP_LPC4088)
		WindowManager_Show_Window(SLIDESHOW_SCREEN);
#endif
#if (UEZ_PROCESSOR == NXP_LPC4357)
		WindowManager_Show_Window(SLIDESHOW_SCREEN);
#endif
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleButtonRightBottom
 *---------------------------------------------------------------------------*/
/** Show the storage screen
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
        Storage_PrintInfo('0');
        Storage_PrintInfo('1');
        WindowManager_Show_Window(STORAGE_SCREEN);
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
	(void) hItem;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, HomeScreenMapping);
        ISetLogo(pMsg);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RIGHT_MIDDLE);
#if (UEZ_PROCESSOR == NXP_LPC1788)
		WM_HideWindow(hItem); // Hide the middle button until we have a GUI screen here.
#endif
#if (UEZ_PROCESSOR == NXP_LPC4088)		
#endif
#if (UEZ_PROCESSOR == NXP_LPC4357)
#endif

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
		//TEXT_SetText(hItem, "Demo"); // Example how to change the title text at run time.
	    
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
      if(G_Active){
        
      }
      break;
    case WM_POST_PAINT:
		
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
    return GUI_CreateDialogBox(_iHomeScreenDialog, GUI_COUNTOF(_iHomeScreenDialog), &_HomeScreenDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    HomeScreen.c
 *-------------------------------------------------------------------------*/
