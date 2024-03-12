/*-------------------------------------------------------------------------*
 * File:  Keypad.h
 *-------------------------------------------------------------------------*/
/** @addtogroup Keypad
 * @{
 *     @brief Generic Keyboard to hand alpha numeric entry.
 *
 */
/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include "GUI.h"
#include "GUI_FontIntern.h"
#include <BUTTON.h>
#include <TEXT.h>
#include <WM.h>
#include <DIALOG.h>
#include "EDIT.h"
#include <uEZ.h>
#include "string.h"
#include "LookAndFeel.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
  int iID;
  const int iChar;
  const char *iTextOnButton;
  TBool (*iCommand)(WM_MESSAGE * pMsg, int aID, int aNCode);
} T_keypadMapping;

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if (UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING      (5)
#else
#define SPACING      (3)
#endif

#define ID_WINDOW_0         (GUI_ID_USER + 0x00)
#define ID_TEXT_BOX         (GUI_ID_USER + 0x01)
#define ID_BUTTON_1         (GUI_ID_USER + 0x02)
#define ID_BUTTON_2         (GUI_ID_USER + 0x03)
#define ID_BUTTON_3         (GUI_ID_USER + 0x04)
#define ID_BUTTON_4         (GUI_ID_USER + 0x05)
#define ID_BUTTON_5         (GUI_ID_USER + 0x06)
#define ID_BUTTON_6         (GUI_ID_USER + 0x07)
#define ID_BUTTON_7         (GUI_ID_USER + 0x08)
#define ID_BUTTON_8         (GUI_ID_USER + 0x09)
#define ID_BUTTON_9         (GUI_ID_USER + 0x0A)
#define ID_BUTTON_CANCEL    (GUI_ID_USER + 0x0B)
#define ID_BUTTON_0         (GUI_ID_USER + 0x0C)
#define ID_BUTTON_ENTER     (GUI_ID_USER + 0x0D)
#define ID_MESSAGE          (GUI_ID_USER + 0x0E)
#define ID_CLOSE_TIMER      (GUI_ID_USER + 0x0F)

#define KEYPAD_COLUMNS              (3)

#if (UEZ_DEFAULT_LCD == LCD_RES_480x272)
#define DIVIDER                     (10)
#elif (UEZ_DEFAULT_LCD == LCD_RES_VGA)
#define DIVIDER                     (12)
#elif (UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define DIVIDER                     (8)
#elif (UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define DIVIDER                     (160)
#endif
#define KEYPAD_BUTTON_HEIGHT        ((UEZ_LCD_DISPLAY_HEIGHT - (SPACING * 6)) / 5)
#define KEYPAD_BUTTON_WIDTH         ((((UEZ_LCD_DISPLAY_WIDTH/ 2 ) - (UEZ_LCD_DISPLAY_WIDTH / DIVIDER)) - (SPACING + (KEYPAD_COLUMNS + 1))) / KEYPAD_COLUMNS)
#define KEYPAD_BUTTON_X_POS(n)      (SPACING + ( n * (KEYPAD_BUTTON_WIDTH + SPACING)))
#define KEYPAD_BUTTON_Y_POS(n)      (SPACING + KEYPAD_BUTTON_HEIGHT + SPACING + ( n * (KEYPAD_BUTTON_HEIGHT + SPACING)))

#define KEYPAD_G_textBox_HEIGHT     (KEYPAD_BUTTON_HEIGHT/2)

#define KEYPAD_X_POS                (UEZ_LCD_DISPLAY_WIDTH - ((SPACING * (KEYPAD_COLUMNS + 1)) + (KEYPAD_BUTTON_WIDTH * KEYPAD_COLUMNS)))
#define KEYPAD_Y_POS                (0)
#define KEYPAD_X_SIZE               (SPACING * 4) + (KEYPAD_BUTTON_WIDTH * 3)
#define KEYPAD_Y_SIZE               UEZ_LCD_DISPLAY_HEIGHT

#define MAX_NUMBER_OF_CHARS 35
#if (UEZ_DEFAULT_LCD == LCD_RES_480x272)
#define KEYPAD_DEFAULT_FONT         GUI_Font20_ASCII
#define KEYPAD_DEFAULT_SMALL_FONT   GUI_Font20_ASCII
#elif (UEZ_DEFAULT_LCD == LCD_RES_QVGA)
#define KEYPAD_DEFAULT_FONT         GUI_Font20_ASCII
#define KEYPAD_DEFAULT_SMALL_FONT   GUI_Font16_ASCII
#else
#define KEYPAD_DEFAULT_FONT         GUI_Font32_ASCII
#define KEYPAD_DEFAULT_SMALL_FONT   GUI_Font32_ASCII
#endif
#define KEYPAD_BACKGROUND_COLOR         GUI_BLACK
#define KEYPAD_TEXT_COLOR               GUI_WHITE
#define KEY_BACKGROUND_COLOR_PRESSED    GUI_LIGHTGRAY
#define KEY_BACKGROUND_COLOR_UNPRESSED  GUI_GRAY
#define CLOSE_TIME_MS           (25*1000)

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IKeypadHandleEnter(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IKeypadHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID);
void KeypadClose(void);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static TBool G_enteringData;
static char G_textBox[MAX_NUMBER_OF_CHARS+1];
static TUInt8 G_textBoxIndex;
static TUInt8 G_maxSize;
static TBool G_isActive = EFalse;
static const char* G_message;
static WM_HWIN G_window;
static const char* G_format;
static char* G_data;
static TBool firstKey;
static WM_HWIN G_notifyWindow;
static int G_notifyMessage;
static int G_returnCode;

static WM_HTIMER G_CloseTimer;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  //                                                        XP                               YP                                                    XS                              YS
  { WINDOW_CreateIndirect   , "Keypad"      , ID_WINDOW_0       , KEYPAD_X_POS           , KEYPAD_Y_POS                                          , KEYPAD_X_SIZE                 , KEYPAD_Y_SIZE                        , 0, 0, 0},
  { TEXT_CreateIndirect     , ""            , ID_MESSAGE        , KEYPAD_BUTTON_X_POS(0) , SPACING                                               , KEYPAD_X_SIZE                 , ((KEYPAD_BUTTON_HEIGHT/2))         , 0, 0, 0},
  { EDIT_CreateIndirect     , "Return Text" , ID_TEXT_BOX       , KEYPAD_BUTTON_X_POS(0) , (SPACING + (KEYPAD_BUTTON_HEIGHT/2))    , KEYPAD_X_SIZE - (SPACING * 2) , ((KEYPAD_BUTTON_HEIGHT/2)+SPACING)   , 0, 0, 0},
  //Row 1                                      XP                                                                               YP                                                             XS                     YS
  { BUTTON_CreateIndirect   , "1"           , ID_BUTTON_1       , KEYPAD_BUTTON_X_POS(0) , KEYPAD_BUTTON_Y_POS(0) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "2"           , ID_BUTTON_2       , KEYPAD_BUTTON_X_POS(1) , KEYPAD_BUTTON_Y_POS(0) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "3"           , ID_BUTTON_3       , KEYPAD_BUTTON_X_POS(2) , KEYPAD_BUTTON_Y_POS(0) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 2
  { BUTTON_CreateIndirect   , "4"           , ID_BUTTON_4       , KEYPAD_BUTTON_X_POS(0) , KEYPAD_BUTTON_Y_POS(1) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "5"           , ID_BUTTON_5       , KEYPAD_BUTTON_X_POS(1) , KEYPAD_BUTTON_Y_POS(1) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "6"           , ID_BUTTON_6       , KEYPAD_BUTTON_X_POS(2) , KEYPAD_BUTTON_Y_POS(1) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 3
  { BUTTON_CreateIndirect   , "7"           , ID_BUTTON_7       , KEYPAD_BUTTON_X_POS(0) , KEYPAD_BUTTON_Y_POS(2) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "8"           , ID_BUTTON_8       , KEYPAD_BUTTON_X_POS(1) , KEYPAD_BUTTON_Y_POS(2) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "9"           , ID_BUTTON_9       , KEYPAD_BUTTON_X_POS(2) , KEYPAD_BUTTON_Y_POS(2) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 4
  { BUTTON_CreateIndirect   , "Cancel"      , ID_BUTTON_CANCEL  , KEYPAD_BUTTON_X_POS(0) , KEYPAD_BUTTON_Y_POS(3) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "0"           , ID_BUTTON_0       , KEYPAD_BUTTON_X_POS(1) , KEYPAD_BUTTON_Y_POS(3) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect   , "Enter"       , ID_BUTTON_ENTER   , KEYPAD_BUTTON_X_POS(2) , KEYPAD_BUTTON_Y_POS(3) , KEYPAD_BUTTON_WIDTH, KEYPAD_BUTTON_HEIGHT, 0, 0, 0 },
};

static T_keypadMapping G_keypadMapping[] = {
  { ID_BUTTON_1, '1', "1", 0},
  { ID_BUTTON_2, '2', "2", 0},
  { ID_BUTTON_3, '3', "3", 0},
  { ID_BUTTON_4, '4', "4", 0},
  { ID_BUTTON_5, '5', "5", 0},
  { ID_BUTTON_6, '6', "6", 0},
  { ID_BUTTON_7, '7', "7", 0},
  { ID_BUTTON_8, '8', "8", 0},
  { ID_BUTTON_9, '9', "9", 0},
  { ID_BUTTON_CANCEL, 0, "Cancel", IKeypadHandleCancel},
  { ID_BUTTON_0, '0', "0", 0},
  { ID_BUTTON_ENTER, 0, "Enter", IKeypadHandleEnter},
  {0},
};

static T_keypadMapping *G_keypadCurrentMapping = G_keypadMapping;

/*---------------------------------------------------------------------------*
 * Routine:     IKeypadHandleEnter
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the dialog passing the current string back.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeypadHandleEnter(WM_MESSAGE * pMsg, int aNCode, int aID)
{
     PARAM_NOT_USED(pMsg);
     PARAM_NOT_USED(aNCode);
     PARAM_NOT_USED(aID);
    G_enteringData = EFalse;
    G_textBox[G_textBoxIndex] = '\0';
    G_returnCode = 1;
    GUI_EndDialog(pMsg->hWin, 1);
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeypadHandleCancel
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the dialog and do not pass current string back.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeypadHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID)
{
     PARAM_NOT_USED(pMsg);
     PARAM_NOT_USED(aNCode);
     PARAM_NOT_USED(aID);
    KeypadClose();
    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:     IPrettyFormat
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the dialog and do not pass current string back.
 * Inputs:
 *      WM_HWIN hItem -- emWin handle of the edit field.
 *      const char* aText -- text to pretty up for display.
 *      const char *aFormat -- format string to conform to, fillable
 *                          area indicated by '#'
 *---------------------------------------------------------------------------*/
static void IPrettyFormat(WM_HWIN hItem, const char *aText, const char *aFormat)
{
    const char *p = aFormat;
    const char *p_text = aText;
    char buffer[MAX_NUMBER_OF_CHARS];
    int i = 0;

    while (*p) {
        if (*p == '#') {
            if (!*p_text)
                break;
            buffer[i++] = *(p_text++);
        } else {
            buffer[i++] = *p;
        }
        p++;
    }
    buffer[i] = '\0';
    EDIT_SetText(hItem, buffer);
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleEvent
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic event handler.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeypadHandleEvent(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    TBool handled = EFalse;
    T_keypadMapping *p = G_keypadCurrentMapping;
    WM_HWIN hItem;
    TUInt8 i;

    /* Find the code in the table */
    while (p->iID) {
        /* Is this the ID we want? */
        if (p->iID == aID) {
            if (aNCode == WM_NOTIFICATION_CLICKED) {
                WM_RestartTimer(G_CloseTimer, CLOSE_TIME_MS);
            } else if (aNCode == WM_NOTIFICATION_RELEASED) {
                /* If this button has a special command, go process it */
                if (p->iCommand) {
                    /* Call the command associated with this ID */
                    handled = (p->iCommand)(pMsg, aNCode, aID);
                } else {
                    /* No command, do the standard code */
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOX);
                    //if (G_textBoxIndex < G_maxSize){
                    //G_textBox[G_textBoxIndex] = p->iChar;
                    //G_textBox[G_textBoxIndex + 1] = '\0';
                    //G_textBoxIndex++;
                    if (!firstKey) {
#if 0
                        G_textBoxIndex = 0;
                        G_textBox[G_textBoxIndex] = p->iChar;
                        G_textBox[G_textBoxIndex+1] = '\0';
                        G_textBoxIndex++;
#else
                        for (i = 0; i < G_maxSize - 1; i++) {
                            G_textBox[i] = '0';
                        }
                        G_textBox[i] = p->iChar;
                        G_textBoxIndex = i + 1;
                        G_textBox[G_textBoxIndex] = '\0';
#endif
                        firstKey = ETrue;
                    } else if (G_textBoxIndex < G_maxSize) {
                        G_textBox[G_textBoxIndex] = p->iChar;
                        G_textBox[G_textBoxIndex + 1] = '\0';
                        G_textBoxIndex++;
                    } else {
                        for (i = 0; i < G_maxSize - 1; i++) {
                            G_textBox[i] = G_textBox[i + 1];
                        }
                        G_textBox[i] = p->iChar;
                    }
                    IPrettyFormat(hItem, G_textBox, G_format);
                }
            }
            break;
        }
        p++;
    }
    return handled;
}

void IHandleWindowClosing(WM_MESSAGE *pMsg)
{
     PARAM_NOT_USED(pMsg);
    TUInt16 i, n, j;
    char buffer[MAX_NUMBER_OF_CHARS + 1];
    WM_MESSAGE msg;

    // Only do this processing immediately if we have someone
    // to notify.  Otherwise, we may be inside of a ExecDialog command
    if (G_notifyWindow) {
        n = G_maxSize - G_textBoxIndex;
        for (i = 0; i < n; i++) {
            buffer[i] = '0';
        }
        j = 0;
        for (; i < G_maxSize; i++) {
            buffer[i] = G_textBox[j++];
        }

        memcpy((void*)G_data, (void*)buffer, G_maxSize + 1);

        G_isActive = EFalse;

        // Now that the data is stored, call the waiting dialog
        msg.MsgId = G_notifyMessage;
        msg.Data.v = G_returnCode;
        WM_SendMessage(G_notifyWindow, &msg);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:     _cbDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int Id, NCode;
    T_keypadMapping *p = G_keypadCurrentMapping;

    switch (pMsg->MsgId) {
        case WM_INIT_DIALOG:
            //
            // Initialization of 'Window'
            //
            hItem = pMsg->hWin;
            G_window = hItem;
            WINDOW_SetBkColor(hItem, KEYPAD_BACKGROUND_COLOR);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOX);
            EDIT_SetMaxLen(hItem, MAX_NUMBER_OF_CHARS);
            EDIT_SetFont(hItem, &KEYPAD_DEFAULT_FONT);
            IPrettyFormat(hItem, G_textBox, G_format);
            //G_textBoxIndex = EDIT_GetNumChars(hItem);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_MESSAGE);
            TEXT_SetFont(hItem, &KEYPAD_DEFAULT_FONT);
            TEXT_SetTextColor(hItem, KEYPAD_TEXT_COLOR);
            TEXT_SetBkColor(hItem, KEYPAD_BACKGROUND_COLOR);
            TEXT_SetText(hItem, G_message);
            WM_MakeModal(pMsg->hWin);

            while (p->iID) {
                hItem = WM_GetDialogItem(pMsg->hWin, p->iID);
                if( p->iID == ID_BUTTON_CANCEL || p->iID == ID_BUTTON_ENTER){
                    BUTTON_SetFont(hItem, &KEYPAD_DEFAULT_SMALL_FONT);
                } else {
                    BUTTON_SetFont(hItem, &KEYPAD_DEFAULT_FONT);
                }
                BUTTON_SetFocussable(hItem, 0);
                BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED,
                        KEY_BACKGROUND_COLOR_UNPRESSED);
                BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED,
                        KEY_BACKGROUND_COLOR_PRESSED);
                p++;
            }
            G_CloseTimer = WM_CreateTimer(pMsg->hWin, ID_CLOSE_TIMER,
                    CLOSE_TIME_MS, 0);
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            if (!IKeypadHandleEvent(pMsg, NCode, Id)) {
                // Special cases go here
                // Nothing happens otherwise
            }
            break;
        case WM_POST_PAINT:
            // Make sure the all presses up til now are gone
            TouchscreenClearPresses();
            break;
        case WM_DELETE:
            IHandleWindowClosing(pMsg);
            break;
        case WM_TIMER:
            NCode = pMsg->Data.v;
            if (NCode == G_CloseTimer) {
                KeypadClose();
            }
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}
void KeypadClose(void)
{
    // Only close if we are entering data
    if (G_isActive) {
        G_enteringData = EFalse;
        G_textBox[0] = '\0';
        G_textBoxIndex = 0;
        G_returnCode = 0;
        GUI_EndDialog(G_window, 0);
    }
}

T_uezError KeypadNotifyOnExit(
        char* aData,
        TUInt8 maxSize,
        const char* message,
        const char *format,
        WM_HWIN aNotifyWindow,
        int aNotifyMessage)
{
    G_enteringData = ETrue;

    if (!G_isActive) {
        G_isActive = ETrue;
        G_message = message;
        G_maxSize = maxSize;
        G_format = format;
        G_data = aData;
        G_returnCode = 0;
        G_textBoxIndex = maxSize;
        G_notifyWindow = aNotifyWindow;
        G_notifyMessage = aNotifyMessage;
        memcpy((void*)G_textBox, (void*)aData, maxSize + 1);
        firstKey = EFalse;
        G_window = GUI_CreateDialogBox(_aDialogCreate,
                GUI_COUNTOF(_aDialogCreate), &_cbDialog, 0, 0, 0);
        WM_ShowWindow(G_window); // it'll come up modal
        return UEZ_ERROR_NONE;
    }
    return UEZ_ERROR_ALREADY_OPEN;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the keyboard dialog and stay here until closed.
 * Inputs:
 *      char* aData -- string to hold current value and to pass back new value.
 *      char* message -- String to tell the type of data being entered.
 *      TUInt16 aNumChars -- number of expected characters to return.
 *      TUInt8 aShowKeys -- integer to disable keys, used for serial number.
 * Outputs:
 *    TUInt16 -- return code of how dialog closes.
 *---------------------------------------------------------------------------*/
TBool Keypad(
        char* aData,
        TUInt8 maxSize,
        const char* message,
        const char *format)
{
	TUInt8 returnCode = 0;
	TUInt8 i, n, j;
	char buffer[MAX_NUMBER_OF_CHARS];
	
    G_enteringData = ETrue;

    if (!G_isActive) {
        G_isActive = ETrue;
        G_message = message;
        G_maxSize = maxSize;
        G_format = format;
        G_returnCode = 0;
        G_textBoxIndex = maxSize;
        G_notifyWindow = 0; // no notification in this version
        memcpy((void*)G_textBox, (void*)aData, maxSize + 1);
        firstKey = EFalse;
        while (G_enteringData) {
            returnCode = GUI_ExecDialogBox(_aDialogCreate,
                    GUI_COUNTOF(_aDialogCreate), &_cbDialog, 0, 0, 0);
        }
        G_isActive = EFalse;
#if 0
        if(G_textBoxIndex < maxSize) {
            for(i = maxSize-1; i > 0; i--) {
                G_textBox[i] = G_textBox[i-1];
            }
            G_textBox[0] = '0';
        }
#endif

        n = maxSize - G_textBoxIndex;
        for (i = 0; i < n; i++) {
            buffer[i] = '0';
        }
        j = 0;
        for (; i < maxSize; i++) {
            buffer[i] = G_textBox[j++];
        }
        buffer[i] = '\0';

        if (returnCode) {
            memcpy((void*)aData, (void*)buffer, maxSize + 1);
            return ETrue;
        }
    }

    return EFalse; //Text entry canceled
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  Keypad.c
 *-------------------------------------------------------------------------*/
