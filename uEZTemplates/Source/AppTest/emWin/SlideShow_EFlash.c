/*-------------------------------------------------------------------------*
 * File:  PictureFame_Main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
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
#include <stdlib.h>
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include <uEZTimeDate.h>
#include "Fonts/Fonts.h"
#include "Graphics/Slideshow.h"

// TODO add hidden/transparent emWin buttons to this example

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef STATIC_MEMORY_ALLOC
#define STATIC_MEMORY_ALLOC		0
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    /* Information about what has to be displayed */
    GUI_AUTODEV_INFO AutoDevInfo;
    /* Polygon data */
    GUI_POINT aPoints[7];
    float Angle;
} PARAM;

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define     ID_WINDOW               (GUI_ID_USER + 0x00)
#define     ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define     ID_BUTTON               (GUI_ID_USER + 0x02)
#define     ID_LOGO                 (GUI_ID_USER + 0x03)
#define     ID_UPDATE_TIMER         (GUI_ID_USER + 0x04)
#define     ID_OVERLAY_TIMER        (GUI_ID_USER + 0x05)
#define     ID_PICTURECHANGE_TIMER  (GUI_ID_USER + 0x06)
#define     ID_REVERSE_BUTTON       (GUI_ID_USER + 0x07)
#define     ID_PLAY_PAUSE_BUTTON    (GUI_ID_USER + 0x08)
#define     ID_FORWARD_BUTTON       (GUI_ID_USER + 0x09)

#if(UEZ_DEFAULT_LCD == LCD_RES_480x272 || UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define     SPACING                 (3)
#else
#define     SPACING                 (5)
#endif
#define     WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define     WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define     WINDOW_XPOS             (0)
#define     WINDOW_YPOS             (0)

#define     TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define     TITLE_TEXT_YSIZE        (WINDOW_YSIZE / 8)

#define     BUTTON_XSIZE            (UEZ_LCD_DISPLAY_WIDTH/11)
#define     BUTTON_YSIZE            (UEZ_LCD_DISPLAY_WIDTH/11)
#define     BUTTON_XPOS(n)          (SPACING + ( n * (SPACING + BUTTON_XSIZE)))
#define     BUTTON_YPOS             (UEZ_LCD_DISPLAY_HEIGHT - BUTTON_YSIZE - SPACING)

#define     TIMEDATE_TEXT_XSIZE     ((UEZ_LCD_DISPLAY_WIDTH/3) +10)
#define     TIMEDATE_TEXT_YSIZE     (UEZ_LCD_DISPLAY_WIDTH/11)
#define     TIMEDATE_TEXT_XPOS      ((UEZ_LCD_DISPLAY_WIDTH/2) - (TIMEDATE_TEXT_XSIZE/2) - (SPACING/2))
#define     TIMEDATE_TEXT_YPOS      (UEZ_LCD_DISPLAY_HEIGHT - TIMEDATE_TEXT_YSIZE - SPACING)

#if(UEZ_DEFAULT_LCD == LCD_RES_480x272)
#define     TITLE_TEXT_FONT         GUI_Font24B_ASCII
#elif (UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define     TITLE_TEXT_FONT         GUI_Font16B_ASCII
#else
#define     TITLE_TEXT_FONT         GUI_Font32B_ASCII
#endif

#define     TEXT_BUTTON_FONT        GUI_Font16_ASCII

#define     DEFAULT_FONT_COLOR      GUI_BLACK
#define     DEFAULT_BK_COLOR        GUI_WHITE

#define IMAGE_NAME_BASE "1"

/*
#if (UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define PLAY_BUTTON         bmplaybutton
#define PAUSE_BUTTON        bmpausebutton
#define FORWARD_BUTTON      bmforwardbutton
#define REVERSE_BUTTON      bmrewindbutton
#elif (UEZ_DEFAULT_LCD == LCD_RES_VGA)
#define PLAY_BUTTON         bmplaybutton_M
#define PAUSE_BUTTON        bmpausebutton_M
#define FORWARD_BUTTON      bmforwardbutton_M
#define REVERSE_BUTTON      bmrewindbutton_M
#elif (UEZ_DEFAULT_LCD == LCD_RES_480x272)
#define PLAY_BUTTON         bmplaybutton_S
#define PAUSE_BUTTON        bmpausebutton_S
#define FORWARD_BUTTON      bmforwardbutton_S
#define REVERSE_BUTTON      bmrewindbutton_S
#elif (UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define PLAY_BUTTON         bmplaybutton_XS
#define PAUSE_BUTTON        bmpausebutton_XS
#define FORWARD_BUTTON      bmforwardbutton_XS
#define REVERSE_BUTTON      bmrewindbutton_XS
#else
#error "Icons Not Defined"
#endif
*/

//#define IMAGE_BUFFER_SIZE           (800*480*2)
#define OVERLAY_TIMEOUT_MS          3000
#define UPDATE_TIME_MS              1000
#define SLIDESHOW_TIMEOUT_MS        5000//7000


#define IS_INDEXABLE(arg) (sizeof(arg[0]))
#define IS_ARRAY(arg) (IS_INDEXABLE(arg) && (((void *) &arg) == ((void *) arg)))
#define ARRAYSIZE(arr) (IS_ARRAY(arr) ? (sizeof(arr) / sizeof(arr[0])) : 0)


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleReverse(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandlePlayPause(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleForward(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IHideButtonsAndText(WM_MESSAGE * pMsg);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _iPFMainDialog[] = {
    {WINDOW_CreateIndirect    , ""              , ID_WINDOW             , WINDOW_XPOS           , WINDOW_YPOS           , WINDOW_XSIZE          , WINDOW_YSIZE          , 0, 0, 0},
    {TEXT_CreateIndirect      , ""              , ID_TITLE_TEXT         , WINDOW_XPOS           , WINDOW_YPOS           , TITLE_TEXT_XSIZE      , TITLE_TEXT_YSIZE      , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    {IMAGE_CreateIndirect      , ""            , ID_LOGO           , 0             , 0             , WINDOW_XSIZE            , WINDOW_YSIZE        , 0, 0, 0},
    
    {BUTTON_CreateIndirect    , ""              , ID_REVERSE_BUTTON     , BUTTON_XPOS(0)  , BUTTON_YPOS  , BUTTON_XSIZE , BUTTON_YSIZE , 0, 0, 0},
    {BUTTON_CreateIndirect    , ""              , ID_PLAY_PAUSE_BUTTON  , BUTTON_XPOS(1)  , BUTTON_YPOS  , BUTTON_XSIZE , BUTTON_YSIZE , 0, 0, 0},
    {BUTTON_CreateIndirect    , ""              , ID_FORWARD_BUTTON     , BUTTON_XPOS(2)  , BUTTON_YPOS  , BUTTON_XSIZE , BUTTON_YSIZE , 0, 0, 0},
};

static T_LAFMapping PFMainLayout[] = {
    { ID_WINDOW               , ""     , DEFAULT_BK_COLOR , DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupWindow    , 0},
    { ID_TITLE_TEXT           , ""     , GUI_TRANSPARENT  , DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupText      , 0},
    { ID_LOGO                 , ""    , GUI_BLACK , GUI_BLACK , &TITLE_TEXT_FONT, 0 , 0},
    
    { ID_REVERSE_BUTTON       , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandleReverse},
    { ID_PLAY_PAUSE_BUTTON    , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandlePlayPause},
    { ID_FORWARD_BUTTON       , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandleForward},
    {0},
};

static WM_HTIMER G_OverlayTimer;
static WM_HTIMER G_PictureChangeTimer;
static WM_HTIMER G_UpdateTimer;

static const GUI_BITMAP* G_SlidePtrs[] = {&bm1, &bm2, &bm3, &bm4, &bm5};//, &bm6, &bm7, &bm8, &bm9, &bm10, &bm11};
TUInt8 G_NumImagesOnCard = ARRAYSIZE(G_SlidePtrs);
static TUInt8 G_CurrentImage_Number = 0;
static TBool G_AutoPlayOn = ETrue;


//TUInt32 *G_CurrentImage, *G_PreviousImage, *G_NextImage;
//static TBool G_Active = EFalse;

//extern char G_AlbumName[];

//static T_uezQueue G_ImageLoadQueue;
//static T_uezSemaphore G_LoadingSemaphore;

typedef enum{
    IMAGE_INITILIZE = 0,
    IMAGE_ADVANCED = 1,
    IMAGE_REVERSED = 2,
}T_ImageMessage;

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
    PARAM_NOT_USED(pMsg);
    //WM_HWIN hItem;
    //hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
    //TEXT_SetText(hItem, G_AlbumName);
}

/*---------------------------------------------------------------------------*
 * Routine: IHandlePlayPause
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
static TBool IHandlePlayPause(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    //WM_HWIN hItem;
    PARAM_NOT_USED(pMsg);
    PARAM_NOT_USED(aID);

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        IHideButtonsAndText(pMsg);
        G_AutoPlayOn = !G_AutoPlayOn;
        WM_RestartTimer(G_PictureChangeTimer, SLIDESHOW_TIMEOUT_MS);
        if(G_AutoPlayOn) {
            //hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &PAUSE_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &PAUSE_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &PAUSE_BUTTON);
        } else {
            //hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &PLAY_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &PLAY_BUTTON);
            //BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &PLAY_BUTTON);
        }
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleReverse
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
static TBool IHandleReverse(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    //T_ImageMessage message;
    //TUInt32 *tempPointer;
    PARAM_NOT_USED(pMsg);
    PARAM_NOT_USED(aID);
                   
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        /*
        UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
        UEZSemaphoreRelease(G_LoadingSemaphore);*/
       /* message = IMAGE_REVERSED;
        UEZQueueSend(G_ImageLoadQueue,
                (void*)&message,
                50);*/
        if (G_CurrentImage_Number > 0){
            G_CurrentImage_Number--;
        } else {
            G_CurrentImage_Number = (G_NumImagesOnCard-1);
        }
        IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_LOGO), G_SlidePtrs[G_CurrentImage_Number]);

        //WM_InvalidateWindow(pMsg->hWin);
        //IHideButtonsAndText(pMsg);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleForward
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
static TBool IHandleForward(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    PARAM_NOT_USED(pMsg);
    PARAM_NOT_USED(aID);
    //T_ImageMessage message;
    //TUInt32 *tempPointer;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
    /*  UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
        UEZSemaphoreRelease(G_LoadingSemaphore);*/
        /*
        message = IMAGE_ADVANCED;
        UEZQueueSend(G_ImageLoadQueue,
                (void*)&message,
                50);*/  
        if (G_CurrentImage_Number < (G_NumImagesOnCard-1)){
            G_CurrentImage_Number++;
        } else {
            G_CurrentImage_Number = 0;
        }
        IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_LOGO), G_SlidePtrs[G_CurrentImage_Number]);

        //WM_InvalidateWindow(pMsg->hWin);
        //IHideButtonsAndText(pMsg);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHideButtonsAndText
 *---------------------------------------------------------------------------*/
/** Routine to disable buttons that will not have any actions.
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IHideButtonsAndText(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
    WM_HideWindow(hItem);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_REVERSE_BUTTON);
    WM_HideWindow(hItem);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
    WM_HideWindow(hItem);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_FORWARD_BUTTON);
    WM_HideWindow(hItem);
}

/*---------------------------------------------------------------------------*
 * Routine: _PFMainDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _PFMainDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
    WM_HWIN hItem;
    GUI_PID_STATE *p_touchState;
    T_ImageMessage message;
	(void) hItem;
	(void) message;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, PFMainLayout);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_REVERSE_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &REVERSE_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &REVERSE_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &REVERSE_BUTTON);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &PLAY_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &PLAY_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &PLAY_BUTTON);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_FORWARD_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &FORWARD_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &FORWARD_BUTTON);
        //BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &FORWARD_BUTTON);

        G_OverlayTimer = WM_CreateTimer(pMsg->hWin, ID_OVERLAY_TIMER, OVERLAY_TIMEOUT_MS, 0);
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, UPDATE_TIME_MS, 0);
        G_PictureChangeTimer = WM_CreateTimer(pMsg->hWin, ID_PICTURECHANGE_TIMER, SLIDESHOW_TIMEOUT_MS, 0);

/*
        if (!G_Current) {
            G_Current = GUI_MEMDEV_Create(0, 0,
                    UEZ_LCD_DISPLAY_WIDTH, UEZ_LCD_DISPLAY_HEIGHT);
            GUI_MEMDEV_Select(G_Current);
            GUI_BMP_Draw(G_CurrentImage, 0, 0);
            GUI_MEMDEV_Select(0);
        }*/
        
        IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_LOGO), G_SlidePtrs[0]);

        IHideButtonsAndText(pMsg);
        WM_SetHasTrans(pMsg->hWin);

        message = IMAGE_INITILIZE;
        /*UEZQueueSend(G_ImageLoadQueue,
                (void*)&message,
                50);*/
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(PFMainLayout, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        //if (G_Active) {
            if (NCode == G_OverlayTimer) {
                IHideButtonsAndText(pMsg);
                //Grab all the touches
                WM_SetCapture(pMsg->hWin, 0);
            } else if (NCode == G_PictureChangeTimer) {
                //put up new picture
                if(G_AutoPlayOn) {
                    IHandleForward(pMsg, WM_NOTIFICATION_RELEASED, ID_FORWARD_BUTTON);
                    WM_RestartTimer(G_PictureChangeTimer, SLIDESHOW_TIMEOUT_MS);
                }
            } else if (NCode == G_UpdateTimer) {
                IUpdateFields(pMsg);
                WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
            }
        //}
        break;
    case WM_TOUCH:
        p_touchState = (GUI_PID_STATE *)(pMsg->Data.p);
        if (!p_touchState->Pressed) { // if main window is touched, then released

            if(WindowManager_GetCurrent_Window() == SLIDESHOW_SCREEN) {
              WindowManager_Show_Window(HOME_SCREEN);
            }

            // TODO finish implementing on-screen buttons with transparency
            WM_RestartTimer(G_OverlayTimer, OVERLAY_TIMEOUT_MS);
            //Show all the hidden text on buttons
            /*hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
            WM_ShowWindow(hItem);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_REVERSE_BUTTON);
            WM_ShowWindow(hItem);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
            WM_ShowWindow(hItem);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_FORWARD_BUTTON);
            WM_ShowWindow(hItem);*/
            WM_ReleaseCapture();
            IUpdateFields(pMsg);
        }
        break;
    case WM_POST_PAINT:
        break;
/*    case WM_NOTIFICATION_GOT_FOCUS:
        G_Active = ETrue;
        WM_SetCapture(pMsg->hWin, 0);
        IHideButtonsAndText(pMsg);
        IUpdateFields(pMsg);
        break;
    case WM_NOTIFICATION_LOST_FOCUS:
        G_Active = EFalse;
        IHideButtonsAndText(pMsg);
        WM_ReleaseCapture();
        G_AutoPlayOn = EFalse;
        break;*/
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine: PictureFame
 *---------------------------------------------------------------------------*/
/**
 *
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN SlideShowWindow_Create(void)
{
    //sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_CurrentImage_Number);

    return GUI_CreateDialogBox(_iPFMainDialog, GUI_COUNTOF(_iPFMainDialog), &_PFMainDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    PictureFame_Main.c
 *-------------------------------------------------------------------------*/
