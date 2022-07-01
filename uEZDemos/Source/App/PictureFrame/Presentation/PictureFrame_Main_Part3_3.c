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
#include <stdio.h>
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "string.h"
#include <uEZ.h>
#include "../GUICommon/LookAndFeel.h"
#include "Graphics/PF_graphics.h"
#include <UEZFile.h>
#include "WindowManager_PF.h"
#include <UEZTimeDate.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define     ID_WINDOW               (GUI_ID_USER + 0x00)
#define     ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define     ID_BUTTON               (GUI_ID_USER + 0x02)
#define     ID_TIMEDATE_TEXT        (GUI_ID_USER + 0x03)
#define     ID_SETTINGS_BUTTON      (GUI_ID_USER + 0x04)
#define     ID_OVERLAY_TIMER        (GUI_ID_USER + 0x05)
#define     ID_PICTURECHANGE_TIMER  (GUI_ID_USER + 0x06)
#define     ID_REVERSE_BUTTON       (GUI_ID_USER + 0x07)
#define     ID_PLAY_PAUSE_BUTTON    (GUI_ID_USER + 0x08)
#define     ID_FORWARD_BUTTON       (GUI_ID_USER + 0x09)
#define     ID_UPDATE_TIMER         (GUI_ID_USER + 0x0A)

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

#define PLAY_BUTTON         bmplaybutton_S
#define PAUSE_BUTTON        bmpausebutton_S
#define FORWARD_BUTTON      bmforwardbutton_S
#define REVERSE_BUTTON      bmrewindbutton_S
#define SETTINGS_BUTTON     bmsettings_S

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

#define IMAGE_NAME_DIR  "1:/images/"
#define IMAGE_NAME_BASE "WQSLID"
#define IMAGE_EXT   ".bmp"

#define IMAGE_BUFFER_SIZE           200000
#define OVERLAY_TIMEOUT_MS          3000
#define UPDATE_TIME_MS              1000

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleSettings(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleReverse(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandlePlayPause(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleForward(WM_MESSAGE * pMsg, int aNCode, int aID);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _iPFMainDialog[] = {
    {WINDOW_CreateIndirect    , "", 			ID_WINDOW,				WINDOW_XPOS, 		WINDOW_YPOS, 		WINDOW_XSIZE, 		WINDOW_YSIZE, 		0, 0, 0},
    {TEXT_CreateIndirect      , "Album Name", 	ID_TITLE_TEXT, 			WINDOW_XPOS, 		WINDOW_YPOS, 		TITLE_TEXT_XSIZE, 	TITLE_TEXT_YSIZE, 	TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    {BUTTON_CreateIndirect    , "", 			ID_REVERSE_BUTTON, 		BUTTON_XPOS(0), 	BUTTON_YPOS, 		BUTTON_XSIZE, 		BUTTON_YSIZE, 		0, 0, 0},
    {BUTTON_CreateIndirect    , "", 			ID_PLAY_PAUSE_BUTTON, 	BUTTON_XPOS(1), 	BUTTON_YPOS, 		BUTTON_XSIZE, 		BUTTON_YSIZE, 		0, 0, 0},
    {BUTTON_CreateIndirect    , "", 			ID_FORWARD_BUTTON, 		BUTTON_XPOS(2), 	BUTTON_YPOS, 		BUTTON_XSIZE, 		BUTTON_YSIZE, 		0, 0, 0},
    {BUTTON_CreateIndirect    , "", 			ID_SETTINGS_BUTTON, 	BUTTON_XPOS(9), 	BUTTON_YPOS, 		BUTTON_XSIZE, 		BUTTON_YSIZE, 		0, 0, 0},
    {TEXT_CreateIndirect      , "Time / Date", 	ID_TIMEDATE_TEXT, 		TIMEDATE_TEXT_XPOS, TIMEDATE_TEXT_YPOS, TIMEDATE_TEXT_XSIZE,TIMEDATE_TEXT_YSIZE, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
};

static T_LAFMapping PFMainLayout[] = {
  { ID_WINDOW               , ""     , DEFAULT_BK_COLOR , DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupWindow    , 0},
  { ID_TITLE_TEXT           , ""     , GUI_TRANSPARENT  , DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupText      , 0},
  { ID_REVERSE_BUTTON       , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandleReverse},
  { ID_PLAY_PAUSE_BUTTON    , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandlePlayPause},
  { ID_FORWARD_BUTTON       , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandleForward},
  { ID_SETTINGS_BUTTON      , ""     , GUI_WHITE        , DEFAULT_FONT_COLOR, &TEXT_BUTTON_FONT , LAFSetupButton    , IHandleSettings},
  { ID_TIMEDATE_TEXT    , ""     ,GUI_TRANSPARENT, DEFAULT_FONT_COLOR, &TITLE_TEXT_FONT  , LAFSetupText    , 0},
  {0},
};
static TUInt32 read;

#pragma section Images
static TUInt8 _G_CurrentImage[IMAGE_BUFFER_SIZE];
static TUInt8 _G_PreviousImage[IMAGE_BUFFER_SIZE];
static TUInt8 _G_NextImage[IMAGE_BUFFER_SIZE];
#pragma section

TUInt32 *G_CurrentImage, *G_PreviousImage, *G_NextImage;

static TUInt8 G_CurrentImage_Number = 1;

extern char G_AlbumName[];

TUInt8 G_NumImagesOnCard;
static T_uezQueue G_ImageLoadQueue;
static T_uezSemaphore G_LoadingSemaphore;

typedef enum{
    IMAGE_INITILIZE = 0,
    IMAGE_ADVANCED = 1,
    IMAGE_REVERSED = 2,
}T_ImageMessage;

/*---------------------------------------------------------------------------*
 * Task:  ImageLoadTask
 *---------------------------------------------------------------------------*/
/**
 *
 * @param [in] aMyTask -- not used
 * @param [in] aParams -- not used
 * @return     never returns
 */
 /*---------------------------------------------------------------------------*/
static TUInt32 ImageLoadTask(T_uezTask aMyTask, void *aParams)
{
    TBool run = ETrue;
    T_uezFile file;
    char filename[50];
    TUInt8 i = 1;
    T_ImageMessage QueueMessage;

    //create the queue to receive messages about
    //loading images from the SC card
    UEZQueueCreate(1,
            sizeof(TUInt32),
            &G_ImageLoadQueue);

    UEZSemaphoreCreateBinary(&G_LoadingSemaphore);

    //find how many images are on the SC card
    //file names only allows 99 files
    while(i <= 99){
        sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, i);
        if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
            i++;
            UEZFileClose(file);
        } else {
            G_NumImagesOnCard = i-1;
            break;
        }
    }

    if(G_NumImagesOnCard == 0){
        //nothing to do. no images found on card
        return 1;
    }
    //load the images to the SD Card
    while(run){
        UEZQueueReceive(G_ImageLoadQueue,
                (void*)&QueueMessage,
                UEZ_TIMEOUT_INFINITE);

        switch(QueueMessage){
            case IMAGE_INITILIZE:
                //the current image has already been loaded
                //so load the next and previous images
                UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
                sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_CurrentImage_Number + 1);
                if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
                    UEZFileRead(file, G_NextImage, IMAGE_BUFFER_SIZE,&read);
                    UEZFileClose(file);
                }

                sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_NumImagesOnCard);
                if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
                    UEZFileRead(file, G_PreviousImage, IMAGE_BUFFER_SIZE,&read);
                    UEZFileClose(file);
                }
                UEZSemaphoreRelease(G_LoadingSemaphore);
                break;
            case IMAGE_ADVANCED:
                UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
                if( G_CurrentImage_Number < G_NumImagesOnCard){
                    sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_CurrentImage_Number + 1);
                } else {
                    sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, 1);
                }
                if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
                    UEZFileRead(file, G_NextImage, IMAGE_BUFFER_SIZE,&read);
                    UEZFileClose(file);
                }
                UEZSemaphoreRelease(G_LoadingSemaphore);
                break;
            case IMAGE_REVERSED:
                UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
                if (G_CurrentImage_Number > 1){
                    sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_CurrentImage_Number - 1);
                } else {
                    sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, G_NumImagesOnCard);
                }

                if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
                    UEZFileRead(file, G_PreviousImage, IMAGE_BUFFER_SIZE,&read);
                    UEZFileClose(file);
                }
                UEZSemaphoreRelease(G_LoadingSemaphore);
                break;
            default:
                break;
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleSettings
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
static TBool IHandleSettings(WM_MESSAGE * pMsg, int aNCode, int aID)
{
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
    T_ImageMessage message;
    TUInt32 *tempPointer;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
        tempPointer = G_NextImage;
        G_NextImage = G_CurrentImage;
        G_CurrentImage = G_PreviousImage;
        G_PreviousImage = tempPointer;
        if (G_CurrentImage_Number > 1){
            G_CurrentImage_Number--;
        } else {
            G_CurrentImage_Number = G_NumImagesOnCard;
        }
        UEZSemaphoreRelease(G_LoadingSemaphore);

        message = IMAGE_REVERSED;
        UEZQueueSend(G_ImageLoadQueue,
                (void*)&message,
                50);
        WM_InvalidateWindow(pMsg->hWin);
        //IHideButtonsAndText(pMsg);
    }
    return EFalse;
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
    T_ImageMessage message;
    TUInt32 *tempPointer;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        UEZSemaphoreGrab(G_LoadingSemaphore, UEZ_TIMEOUT_INFINITE);
        tempPointer = G_PreviousImage;
        G_PreviousImage = G_CurrentImage;
        G_CurrentImage = G_NextImage;
        G_NextImage = tempPointer;
        if (G_CurrentImage_Number < G_NumImagesOnCard){
            G_CurrentImage_Number++;
        } else {
            G_CurrentImage_Number = 1;
        }
        UEZSemaphoreRelease(G_LoadingSemaphore);

        message = IMAGE_ADVANCED;
        UEZQueueSend(G_ImageLoadQueue,
                (void*)&message,
                50);
        WM_InvalidateWindow(pMsg->hWin);
        //IHideButtonsAndText(pMsg);
    }
    return EFalse;
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
	WM_HWIN hItem;
	int Id, NCode;
	
    switch (pMsg->MsgId) {
	    case WM_INIT_DIALOG:
			Id    = WM_GetId(pMsg->hWinSrc);
	        NCode = pMsg->Data.v;
	        LAFSetup(pMsg->hWin, ID_WINDOW, PFMainLayout);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_REVERSE_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &REVERSE_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &REVERSE_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &REVERSE_BUTTON);

	        hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_PAUSE_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &PLAY_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &PLAY_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &PLAY_BUTTON);

	        hItem = WM_GetDialogItem(pMsg->hWin, ID_FORWARD_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &FORWARD_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &FORWARD_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &FORWARD_BUTTON);

	        hItem = WM_GetDialogItem(pMsg->hWin, ID_SETTINGS_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &SETTINGS_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &SETTINGS_BUTTON);
	        BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &SETTINGS_BUTTON);
	
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
		    TEXT_SetText(hItem, "Family Album");

		    hItem = WM_GetDialogItem(pMsg->hWin, ID_TIMEDATE_TEXT);
		    TEXT_SetText(hItem, "Time / Date");
	
	        break;
	    case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
	        NCode = pMsg->Data.v;
	        if( !LAFHandleEvent(PFMainLayout, pMsg, NCode, Id)){
	            //Handle special cases here
	        }
	        break;
	    case WM_TIMER:
	        break;
	    case WM_TOUCH:
	        break;
	    case WM_POST_PAINT:
			GUI_BMP_Draw(G_CurrentImage, 0, 0);
	        break;
	    case WM_APP_GAINED_FOCUS:
	        break;
	    case WM_APP_LOST_FOCUS:
	        break;
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
WM_HWIN PictureFame()
{
    T_uezFile file;
    char filename[50];

    sprintf(filename, IMAGE_NAME_DIR IMAGE_NAME_BASE "%02d" IMAGE_EXT, 5);

    if(UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
		G_CurrentImage = (TUInt32 *)_G_CurrentImage; // UEZMemAlloc(IMAGE_BUFFER_SIZE);
        G_NextImage = (TUInt32 *)_G_NextImage; // UEZMemAlloc(IMAGE_BUFFER_SIZE);
        G_PreviousImage = (TUInt32 *)_G_PreviousImage; // UEZMemAlloc(IMAGE_BUFFER_SIZE);

		if(G_CurrentImage){
            UEZFileRead(file, G_CurrentImage, IMAGE_BUFFER_SIZE,&read);
        }
        UEZFileClose(file);
    } else {
        UEZFailureMsg("No Images On SD Card");
    }

    UEZTaskCreate(ImageLoadTask, "ImageLoad", 2048, (void *)0, UEZ_PRIORITY_NORMAL, 0);

    return GUI_CreateDialogBox(_iPFMainDialog, GUI_COUNTOF(_iPFMainDialog), &_PFMainDialog, 0,0,0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    PictureFame_Main.c
 *-------------------------------------------------------------------------*/
