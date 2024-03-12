/*-------------------------------------------------------------------------*
 * File: StorageWindow.c
 *-------------------------------------------------------------------------*/
/** @addtogroup Storage
 * @{
 *     @brief Implements Layout and functionality of StorageWindow.c.
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
#include <stdlib.h>
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"
#include "../Storage_Callbacks.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x02)
#define ID_STORAGE_LOG          (GUI_ID_USER + 0x03)
#define ID_BUTTON_FTSMALL       (GUI_ID_USER + 0x04)
#define ID_BUTTON_FTMEDIUM      (GUI_ID_USER + 0x05)
#define ID_BUTTON_FTLARGE       (GUI_ID_USER + 0x06)
#define ID_BUTTON_SCREENCAP     (GUI_ID_USER + 0x07)
#define ID_BUTTON_FORMAT        (GUI_ID_USER + 0x08)
#define ID_TEXT_SELMED          (GUI_ID_USER + 0x09)
#define ID_RADIO_SELMED         (GUI_ID_USER + 0x0A)
#define ID_TEXT_TESTLEN         (GUI_ID_USER + 0x0B)
#define ID_RADIO_TESTLEN        (GUI_ID_USER + 0x0C)
#define ID_BUTTON_STRESS_BEGIN  (GUI_ID_USER + 0x0D)
#define ID_BUTTON_STRESS_END    (GUI_ID_USER + 0x0E)
#define ID_TEXT_SDCARDAVAIL     (GUI_ID_USER + 0x0F)
#define ID_TEXT_USBAVAIL        (GUI_ID_USER + 0x10)

// auto screen sizing defines
#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#define DIVIDER                 (10)
#define STORAGE_TEXT_SIZE       &FONT_MEDIUM
#else
#define SPACING                 (5)
#define DIVIDER                 (10)
#define STORAGE_TEXT_SIZE       &FONT_SMALL
#endif

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/DIVIDER))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define BACK_BUTTON_XSIZE       (WINDOW_XSIZE/SPACING)
#define BACK_BUTTON_YSIZE       (WINDOW_YSIZE/SPACING)
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - BACK_BUTTON_YSIZE - SPACING)

#define TEXT_XSIZE              ((WINDOW_XSIZE - (8 * SPACING))/5)
#define TEXT_YSIZE              ((DIVIDER/2 + (4 * SPACING))/2)
#define TEXT_XPOS(n)            (SPACING + (n * (SPACING + TEXT_XSIZE)))
#define TEXT_YPOS(n)            (0 + ( n * (TEXT_YSIZE + DIVIDER)))

#define BUTTON_XSIZE       (WINDOW_XSIZE/DIVIDER+2*DIVIDER+8*SPACING)
#define BUTTON_YSIZE       (WINDOW_YSIZE/DIVIDER)
#define BUTTON_XPOS(n)          ((SPACING+0) + (n * (BUTTON_XSIZE + SPACING)))
#define BUTTON_YPOS(n)          ((DIVIDER + TITLE_TEXT_YSIZE) + (n * (BUTTON_YSIZE + SPACING)))

#define TEXTBOX_XPOS        (BUTTON_XPOS(2)+SPACING)
#define TEXTBOX_XSIZE       (WINDOW_XSIZE-(TEXTBOX_XPOS)-SPACING)
#define TEXTBOX_YSIZE       ((WINDOW_YSIZE - BUTTON_YSIZE*2-(4 * SPACING))/1)

// other defines

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleStorageTest(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleCapture(WM_MESSAGE * pMsg, int aNCode, int aID);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
WM_HWIN hMultiedit;

extern TBool G_SDCard_inserted;
extern TBool G_USBFlash_inserted;

/** Structure to hold all of the widgets used in this dialog */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {               //XP          YP               XS           YS
  { WINDOW_CreateIndirect, "Window", ID_WINDOW,                         0, 0, WINDOW_XSIZE, WINDOW_YSIZE, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text_Storage", ID_TITLE_TEXT,                 (WINDOW_XSIZE-TEXT_XSIZE)/2, SPACING, TEXT_XSIZE, TITLE_TEXT_YSIZE, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text_SelectMedium", ID_TEXT_SELMED,           BUTTON_XPOS(0), TEXT_YPOS(1), TEXT_XSIZE, TEXT_YSIZE, 0, 0x64, 0 },
  { RADIO_CreateIndirect, "Radio_SelectMedium", ID_RADIO_SELMED,        BUTTON_XPOS(0), TEXT_YPOS(2), TEXT_XSIZE, 3*TEXT_YSIZE, 0, 0x1402, 0 },
  { TEXT_CreateIndirect, "Text_TestLength", ID_TEXT_TESTLEN,            BUTTON_XPOS(0), TEXT_YPOS(4),TEXT_XSIZE, TEXT_YSIZE, 0, 0x64, 0 },
  { RADIO_CreateIndirect, "Radio", ID_RADIO_TESTLEN,                    BUTTON_XPOS(0), TEXT_YPOS(5), TEXT_XSIZE, 5*TEXT_YSIZE, 0, 0x1403, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_USBAVAIL,                          BUTTON_XPOS(0), TEXT_YPOS(8),TEXT_XSIZE, TEXT_YSIZE, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_SDCARDAVAIL,                       BUTTON_XPOS(0), TEXT_YPOS(9),TEXT_XSIZE, TEXT_YSIZE, 0, 0x64, 0 },
  //{ BUTTON_CreateIndirect, "Button_FormatMedium", ID_BUTTON_FORMAT,     BUTTON_XPOS(0), BUTTON_YPOS(4), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_FormatMedium", ID_BUTTON_FORMAT,     (WINDOW_XSIZE-BUTTON_XSIZE-SPACING)/2, WINDOW_YSIZE-BUTTON_YSIZE-SPACING, BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_SmallFileTest", ID_BUTTON_FTSMALL,   BUTTON_XPOS(1), BUTTON_YPOS(0), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_MediumFileTest", ID_BUTTON_FTMEDIUM, BUTTON_XPOS(1), BUTTON_YPOS(1), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_LargeFileTest", ID_BUTTON_FTLARGE,   BUTTON_XPOS(1), BUTTON_YPOS(2), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_StressBegin", ID_BUTTON_STRESS_BEGIN,BUTTON_XPOS(1), BUTTON_YPOS(3), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_StressEnd", ID_BUTTON_STRESS_END,    BUTTON_XPOS(1), BUTTON_YPOS(4), BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Multiedit_Log", ID_STORAGE_LOG,          TEXTBOX_XPOS, BUTTON_YPOS(0), TEXTBOX_XSIZE, TEXTBOX_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_CaptureScreen", ID_BUTTON_SCREENCAP, WINDOW_XSIZE-BUTTON_XSIZE-SPACING,  WINDOW_YSIZE-BUTTON_YSIZE-SPACING, BUTTON_XSIZE, BUTTON_YSIZE, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button_Back", ID_BACK_BUTTON,               BACK_BUTTON_XPOS, BACK_BUTTON_YPOS, BACK_BUTTON_XSIZE, BACK_BUTTON_YSIZE, 0, 0x0, 0 },
};

/** Generic Mapping of Screen Layout */
static T_LAFMapping StorageWindowMapping[] = {
    { ID_WINDOW,"", GUI_BLACK , GUI_WHITE , &FONT_LARGE , LAFSetupWindow, 0},
    { ID_BUTTON_FTSMALL, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BUTTON_FTMEDIUM, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BUTTON_FTLARGE, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BUTTON_FORMAT, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BUTTON_STRESS_BEGIN, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BUTTON_STRESS_END, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleStorageTest
    },
    { ID_BACK_BUTTON, "Back", GUI_GRAY, GUI_BLACK, &FONT_LARGE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleBack
    },
    { ID_BUTTON_SCREENCAP, 0, GUI_GRAY, GUI_BLACK, STORAGE_TEXT_SIZE, LAFSetupButton,
      (TBool (*)(WM_MESSAGE *, int, int))IHandleCapture
    },
    { ID_TITLE_TEXT  , "" , GUI_BLACK, GUI_WHITE, &FONT_MEDIUM, LAFSetupText, 0},
    { ID_TEXT_SELMED  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupText, 0},
    { ID_RADIO_SELMED  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupRadioText, 0},
    { ID_TEXT_TESTLEN  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupText, 0},
    { ID_RADIO_TESTLEN  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupRadioText, 0},
    { ID_TEXT_SDCARDAVAIL  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupText, 0},
    { ID_TEXT_USBAVAIL  , "" , GUI_BLACK, GUI_WHITE, STORAGE_TEXT_SIZE, LAFSetupText, 0},
    {0},
};

/*---------------------------------------------------------------------------*
 * Routine:	_cbDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    LAFSetup(pMsg->hWin, ID_WINDOW, StorageWindowMapping);

    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00000000));
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SELMED);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x0000FFFF));
    TEXT_SetText(hItem, "Test Medium");
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED);
    RADIO_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    RADIO_SetText(hItem, "USB Host", 0);
    RADIO_SetText(hItem, "microSD", 1);
    RADIO_SetValue(hItem, 1);
    
    hMultiedit = hItem = WM_GetDialogItem(pMsg->hWin, ID_STORAGE_LOG);
    MULTIEDIT_SetBufferSize(hItem, STORAGE_ME_BUFF_SIZE);
    MULTIEDIT_SetMaxNumChars(hItem, STORAGE_ME_BUFF_SIZE-1);
    MULTIEDIT_SetWrapWord(hItem);
    MULTIEDIT_SetText(hItem, "");
    Storage_SetLogHandel(hMultiedit);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_FTSMALL);
    BUTTON_SetText(hItem, "Small File Test");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_FTMEDIUM);
    BUTTON_SetText(hItem, "Medium File Test");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_FTLARGE);
    BUTTON_SetText(hItem, "Large File Test");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SCREENCAP);
    BUTTON_SetText(hItem, "Capture Screen");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_FORMAT);
    BUTTON_SetText(hItem, "Format Medium");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_TOP);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x0000FFFF));
    TEXT_SetText(hItem, "Storage");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BACK_BUTTON);
    BUTTON_SetText(hItem, "Back");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TESTLEN);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x0000FFFF));
    TEXT_SetText(hItem, "Test Length");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_TESTLEN);
    RADIO_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    RADIO_SetText(hItem, "Short", 0);
    RADIO_SetText(hItem, "Medium", 1);
    RADIO_SetText(hItem, "Long", 2);
    RADIO_SetValue(hItem, 0);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_STRESS_BEGIN);
    BUTTON_SetText(hItem, "Begin Stress Test");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_STRESS_END);
    BUTTON_SetText(hItem, "End Stress Test");

    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
        if( !LAFHandleEvent(StorageWindowMapping, pMsg, NCode, Id))
        {
            //Handle special cases here
        }
    break;
  case WM_PAINT:
    //Id    = WM_GetId(pMsg->hWinSrc); // This causes a crash only on certain units?
    NCode = pMsg->Data.v;
    hItem = pMsg->hWin;

    if(G_SDCard_inserted == ETrue) {
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SDCARDAVAIL);
      TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFF00));
      TEXT_SetText(hItem, "SDC Avail");
    }
    if(G_USBFlash_inserted == ETrue) {
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_USBAVAIL);
      TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFF00));
      TEXT_SetText(hItem, "USB Avail");
    }

    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN CreateWindow(void);
WM_HWIN CreateWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

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
    PARAM_NOT_USED(pMsg);
    PARAM_NOT_USED(aID);
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        WindowManager_Show_Window(HOME_SCREEN);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleCapture
 *---------------------------------------------------------------------------*/
/** Screen Capture
 *      
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleCapture(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    PARAM_NOT_USED(pMsg);
    PARAM_NOT_USED(aID);
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        Storage_CaptureScreen(RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED)));
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleStorageTest
 *---------------------------------------------------------------------------*/
/** Handle storage testing functionality
 *      
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleStorageTest(WM_MESSAGE * pMsg, int aNCode, int aID)
{
  if (aNCode == WM_NOTIFICATION_RELEASED)
  {
    if (aID == ID_BUTTON_FORMAT) {
      Storage_FormatMedium(RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED)));
    } else if (aID == ID_BUTTON_STRESS_BEGIN) {
      Storage_BeginStressTest();
    } else if (aID == ID_BUTTON_STRESS_END) {
      Storage_EndStressTest();
    } else if (aID == ID_BUTTON_FTSMALL) {
      Storage_FileTest(RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED)),
                        STORAGE_FILE_TEST_SMALL,
                        RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_TESTLEN)) );
    } else if (aID == ID_BUTTON_FTMEDIUM) {
      Storage_FileTest(RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED)),
                        STORAGE_FILE_TEST_MEDIUM,
                        RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_TESTLEN)) );
    } else if (aID == ID_BUTTON_FTLARGE) {
      Storage_FileTest(RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_SELMED)),
                        STORAGE_FILE_TEST_LARGE,
                        RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, ID_RADIO_TESTLEN)) );
    }
  }
  return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:    Storage_Create
 *---------------------------------------------------------------------------*/
/** Create the Storage. This function should only be called by the
 *  window manager and never by user code.
 *     
 *  @param [in] p_choice    void
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN StorageWindow_Create(void)
{
    return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDialog, 0,0,0);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    StorageWindow.c
 *-------------------------------------------------------------------------*/
