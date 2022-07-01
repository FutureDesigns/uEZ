/*-------------------------------------------------------------------------*
 * File:
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#include <stdlib.h>
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "string.h"
#include <uEZ.h>
#include "LookAndFeel.h"
#include "Keyboard.h"
#include "Keypad.h"
#include <uEZLCD.h>         //For Backlight control
#include <UEZTimeDate.h>    //For RTC Display
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if (COMPILER_TYPE != VisualC)
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#endif


/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
//IDs used by emWin
#define DEMO_WINDOW             (GUI_ID_USER + 0x00)
#define DEMO_TITLE_TEXT         (GUI_ID_USER + 0x01)
#define DEMO_KEYBOARD_BUTTON    (GUI_ID_USER + 0x02)
#define DEMO_KEYPAD_BUTTON      (GUI_ID_USER + 0x03)
#define DEMO_CLOSE_BUTTON       (GUI_ID_USER + 0x04)
#define DEMO_BACKLIGHT_TEXT     (GUI_ID_USER + 0x05)
#define DEMO_BACKLIGHT_SLIDER   (GUI_ID_USER + 0x06)
#define DEMO_RTC_TEXT           (GUI_ID_USER + 0x07)
#define DEMO_RTC_UPDATE_TIMER   (GUI_ID_USER + 0x08)

//Button layout macros
#define BUTTON_SPACING          20
#define BUTTON_XSIZE            (UEZ_LCD_DISPLAY_WIDTH / 2)
#define BUTTON_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT / 10)
#define BUTTON_XPOS(n)          BUTTON_SPACING + (n * (BUTTON_XSIZE + BUTTON_SPACING))
#define BUTTON_YPOS(n)          BUTTON_SPACING + (n * (BUTTON_YSIZE + BUTTON_SPACING))

//Title Text Field macros
#define TEXT_XSIZE              (UEZ_LCD_DISPLAY_WIDTH / 2)
#define TEXT_YSIZE              (UEZ_LCD_DISPLAY_HEIGHT / 10)
#define TEXT_XPOS               ((UEZ_LCD_DISPLAY_WIDTH / 2) - (TEXT_XSIZE / 2))
#define TEXT_YPOS               (UEZ_LCD_DISPLAY_HEIGHT / 16)

//Slider layout macros
#define SLIDER_TEXT_XSIZE       (UEZ_LCD_DISPLAY_WIDTH / 4)
#define SLIDER_TEXT_YSIZE       (UEZ_LCD_DISPLAY_HEIGHT / 10)
#define SLIDER_TEXT_XPOS        ((UEZ_LCD_DISPLAY_WIDTH / 2) + (UEZ_LCD_DISPLAY_WIDTH / 4))
#define SLIDER_TEXT_YPOS        TEXT_YPOS

#define SLIDER_XSIZE            (UEZ_LCD_DISPLAY_WIDTH / 8)
#define SLIDER_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT - (2 * TEXT_YSIZE))
#define SLIDER_XPOS             ((UEZ_LCD_DISPLAY_WIDTH / 2) + (UEZ_LCD_DISPLAY_WIDTH / 4) + (SLIDER_XSIZE / 2))
#define SLIDER_YPOS             ((SLIDER_TEXT_YPOS + SLIDER_TEXT_YSIZE) + 5)

#define SLIDER_MAX              255
#define SLIDER_MIN              0

//Default Font
#if (UEZ_DEFAULT_LCD == LCD_RES_480x272)
#define BUTTON_DEFAULT_FONT     GUI_Font16_ASCII
#define TEXT_DEFAULT_FONT       GUI_Font16B_ASCII
#else
#define BUTTON_DEFAULT_FONT     GUI_Font24_ASCII
#define TEXT_DEFAULT_FONT       GUI_Font24_ASCII
#endif

//Default colors
#define WINDOW_BK_COLOR         GUI_WHITE
#define BUTTON_PRESSES_COLOR    GUI_LIGHTGRAY
#define BUTTON_UNPRESSED_COLOR  GUI_GRAY
#define BUTTON_FONT_COLOR       GUI_BLACK
#define TEXT_BK_COLOR           GUI_WHITE
#define TEXT_FONT_COLOR         GUI_BLACK

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleKeyboardButton(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleKeypadButton(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleCloseButton(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleValueChanged(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IHandleUpdateRTC(WM_MESSAGE * pMsg);
void iSetSlider(const T_LAFMapping *p, WM_HWIN hItem);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _iDemoDialog[] = {
  //Function                 Text                       ID                      XP                  YP                      XS                      YS
    {WINDOW_CreateIndirect    , ""                      , DEMO_WINDOW           , 0                 , 0                 , UEZ_LCD_DISPLAY_WIDTH     , UEZ_LCD_DISPLAY_HEIGHT    , 0, 0, 0},
    {TEXT_CreateIndirect      , "FDI emWin Demo"        , DEMO_TITLE_TEXT       , TEXT_XPOS         , TEXT_YPOS         , TEXT_XSIZE                 , TEXT_YSIZE               , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    {BUTTON_CreateIndirect    , "Enter Text Here..."    , DEMO_KEYBOARD_BUTTON  , BUTTON_XPOS(0)    , BUTTON_YPOS(1)    , BUTTON_XSIZE              , BUTTON_YSIZE              , 0, 0, 0},
    {BUTTON_CreateIndirect    , "Enter Numbers Here.."  , DEMO_KEYPAD_BUTTON    , BUTTON_XPOS(0)    , BUTTON_YPOS(2)    , BUTTON_XSIZE              , BUTTON_YSIZE              , 0, 0, 0},
    {BUTTON_CreateIndirect    , "Close"                 , DEMO_CLOSE_BUTTON     , BUTTON_XPOS(0)    , BUTTON_YPOS(3)    , BUTTON_XSIZE              , BUTTON_YSIZE              , 0, 0, 0},
    {TEXT_CreateIndirect      , "Backlight Level"       , DEMO_BACKLIGHT_TEXT   , SLIDER_TEXT_XPOS  , SLIDER_TEXT_YPOS  , SLIDER_TEXT_XSIZE         , SLIDER_TEXT_YSIZE         , TEXT_CF_HCENTER | TEXT_CF_VCENTER , 0, 0},
    {SLIDER_CreateIndirect    , ""                      , DEMO_BACKLIGHT_SLIDER , SLIDER_XPOS       , SLIDER_YPOS       , SLIDER_XSIZE              , SLIDER_YSIZE              , SLIDER_CF_VERTICAL, 0, 0},
    {TEXT_CreateIndirect      , ""                      , DEMO_RTC_TEXT         , BUTTON_XPOS(0)    , BUTTON_YPOS(4)    , BUTTON_XSIZE              , BUTTON_YSIZE              , TEXT_CF_HCENTER | TEXT_CF_VCENTER , 0, 0},
};

static T_LAFMapping demoMapping[] = {
    { DEMO_WINDOW           , ""     , WINDOW_BK_COLOR        , WINDOW_BK_COLOR   , &TEXT_DEFAULT_FONT    , LAFSetupWindow    , 0},
    { DEMO_TITLE_TEXT       , ""     , TEXT_BK_COLOR          , TEXT_FONT_COLOR   , &TEXT_DEFAULT_FONT    , LAFSetupText      , 0},
    { DEMO_KEYBOARD_BUTTON  , ""     , BUTTON_UNPRESSED_COLOR , TEXT_FONT_COLOR   , &BUTTON_DEFAULT_FONT  , LAFSetupButton    , IHandleKeyboardButton},
    { DEMO_KEYPAD_BUTTON    , ""     , BUTTON_UNPRESSED_COLOR , BUTTON_FONT_COLOR , &BUTTON_DEFAULT_FONT  , LAFSetupButton    , IHandleKeypadButton},
    { DEMO_CLOSE_BUTTON     , ""     , BUTTON_UNPRESSED_COLOR , BUTTON_FONT_COLOR , &BUTTON_DEFAULT_FONT  , LAFSetupButton    , IHandleCloseButton},
    { DEMO_BACKLIGHT_TEXT   , ""     , TEXT_BK_COLOR          , TEXT_FONT_COLOR   , &TEXT_DEFAULT_FONT    , LAFSetupText      , 0},
    { DEMO_BACKLIGHT_SLIDER , ""     , BUTTON_UNPRESSED_COLOR , BUTTON_FONT_COLOR , &BUTTON_DEFAULT_FONT  , iSetSlider        , IHandleValueChanged},
    { DEMO_RTC_TEXT         , ""     , TEXT_BK_COLOR          , TEXT_FONT_COLOR   , &TEXT_DEFAULT_FONT    , LAFSetupText      , 0},
    {0},
};

//Required for emWin Initialization
static T_uezDevice _hTouchScreen;
static T_uezQueue  _hTSQueue;
static U8          _RequestExit;

//For Backlight control
static T_uezDevice hLCD;
static TUInt16 G_Backligh_Level;

//For RTC
static WM_HTIMER rtcTimer;

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static U32 _TouchTask(T_uezTask aMyTask, void *aParameters) {
    T_uezInputEvent inputEvent;
    GUI_PID_STATE  State = { 0 };
    
    (void)aMyTask;
    (void)aParameters;
  
    while (1) {
      if (_RequestExit == 1) {
          _RequestExit = 0;
          break;
      }
      //
      // Wait for 100ms for a new touch event to occur. Else skip over to give the
      // task a chance to respond to an exit request.
      //
      if (UEZQueueReceive(_hTSQueue, &inputEvent, 100) == UEZ_ERROR_NONE) {
          if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
              State.x       = inputEvent.iEvent.iXY.iX;
              State.y       = inputEvent.iEvent.iXY.iY;
              State.Pressed = 1;
          } else {
              State.x       = -1;
              State.y       = -1;
              State.Pressed =  0;
          }
          GUI_PID_StoreState(&State);
        }
    }
    return UEZ_ERROR_NONE;
}

void iSetSlider(const T_LAFMapping *p, WM_HWIN hItem)
{
    SLIDER_SetRange(hItem, SLIDER_MIN, SLIDER_MAX);
    //SLIDER_SetValue(hItem, G_Backligh_Level);
}

static void IHandleUpdateRTC(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    char text[50];
    T_uezTimeDate timedate;
    TBool isPM;
    TUInt32 hours;
    TUInt32 minutes;

    hItem = WM_GetDialogItem(pMsg->hWin, DEMO_RTC_TEXT);
    if (UEZTimeDateGet(&timedate) == UEZ_ERROR_NONE) {
        // Convert 24 hour time to 12 hour time
        hours = timedate.iTime.iHour;
        minutes = timedate.iTime.iMinute;
        if (hours >= 12) {
            hours -= 12;
            isPM = ETrue;
        } else {
            isPM = EFalse;
        }
        if (hours == 0)
            hours = 12;

        // Put up the 12 hour format with an AM/PM tail
        sprintf(text, "%02d:%02d:%02d %s %02d/%02d/%04d\0", hours, minutes, timedate.iTime.iSecond, (isPM)?"PM":"AM", timedate.iDate.iMonth, timedate.iDate.iDay, timedate.iDate. iYear);
    } else {
        // Error getting the RTC, put up something funny
        strcpy(text, "??:??:?? PM ?? / ?? / ??");
    }
    TEXT_SetText(hItem, text);
}
/*---------------------------------------------------------------------------*
 * Routine: IHandleValueChanged
 *---------------------------------------------------------------------------*
 * Description:
 *          Handles the value of the slider being changed and
 *          changes the backlight intensities.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IHandleValueChanged(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;

    if (aNCode == WM_NOTIFICATION_VALUE_CHANGED)
    {
        hItem = WM_GetDialogItem(pMsg->hWin, DEMO_BACKLIGHT_SLIDER);
        G_Backligh_Level = SLIDER_GetValue(hItem);
        //Invert the number
        G_Backligh_Level = 255 - G_Backligh_Level;
        //Set backlight
        UEZLCDBacklight(hLCD, G_Backligh_Level);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static TBool IHandleKeyboardButton(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;
    char data[25 + 1];
    TUInt32 showKeys = KEYBOARD_USE_UPPER_CASE | KEYBOARD_USE_LOWER_CASE | KEYBOARD_USE_DIGITS | KEYBOARD_USE_SYMBOLS | KEYBOARD_USE_SPACE;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        hItem = WM_GetDialogItem(pMsg->hWin, DEMO_KEYBOARD_BUTTON);
        data[0] = '\0';
        if(Keyboard(data, "Enter Text", 25, showKeys)){
            BUTTON_SetText(hItem, data);
        }
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static TBool IHandleKeypadButton(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;
    char data[15 + 1];

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        hItem = WM_GetDialogItem(pMsg->hWin, DEMO_KEYPAD_BUTTON);
        data[0] = '\0';
        if(Keypad(data, 15, "Enter Numeric Data", "")){ //not using formating
            BUTTON_SetText(hItem, data);
        }
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static TBool IHandleCloseButton(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;
    T_LAFMapping *p = demoMapping;
    p++;
    
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        UEZLCDBacklight(hLCD, 0);
        while( p->iID){
            hItem = WM_GetDialogItem(pMsg->hWin,p->iID);
            WM_HideWindow(hItem);
            p++;
        }
        GUI_EndDialog(pMsg->hWin, 0);
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static void _DemoDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
	WM_HWIN hItem;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, DEMO_WINDOW, demoMapping);
        rtcTimer = WM_CreateTimer(pMsg->hWin, DEMO_RTC_UPDATE_TIMER, 0, 0);
		hItem = WM_GetDialogItem(pMsg->hWin, DEMO_KEYBOARD_BUTTON);
		BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(demoMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if (NCode == rtcTimer) {
            WM_RestartTimer(rtcTimer, 1000);
            IHandleUpdateRTC(pMsg);
        }
        break;
    case WM_POST_PAINT:
        TouchscreenClearPresses();
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
void FDI_emWin_Demo(const T_choice *aChoice)
{
    //T_uezDevice    hLCD;
    U32            FrameBufferSize;
    T_uezTask      hTouchTask;
    SWIM_WINDOW_T window;
    T_pixelColor *pixels;

    G_Backligh_Level = 255;
    //UEZLCDOpen("LCD", &hLCD);
  
    (void)aChoice;
#if (COMPILER_TYPE != VisualC)
    FrameBufferSize = UEZ_LCD_DISPLAY_WIDTH * UEZ_LCD_DISPLAY_HEIGHT * GUI_NUM_VIRTUAL_DISPLAYS * GUI_PIXEL_WIDTH;
    memset((void*)GUI_VRAM_BASE_ADDR, 0, FrameBufferSize);
    //
    // Check that frame buffer memory fits into VRAM memory and we have empty memory left
    //
    if (GUI_VRAM_SIZE <= FrameBufferSize) {
      return;  // Error, not enough memory
    }
    //
    // Assign memory left to emWin memory
    //
    GUI_pMem    = (U32*)(GUI_VRAM_BASE_ADDR + FrameBufferSize);
    GUI_MemSize =       (GUI_VRAM_SIZE      - FrameBufferSize);
#endif
    //
    // emWin startup
    //
    WM_SetCreateFlags(WM_CF_MEMDEV);  // Enable memory devices
    GUI_Init();
    WM_SetDesktopColor(GUI_BLACK);
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering
    //
    // Open the LCD
    //
    if (UEZLCDOpen("LCD", &hLCD) == UEZ_ERROR_NONE)  {
      //
      // Create touch screen queue
      //
      if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &_hTSQueue) == UEZ_ERROR_NONE) {
        //
        // Open touch screen
        //
        if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) == UEZ_ERROR_NONE) {
          //
          // Create touch task
          //
          if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask",  1024, 0, UEZ_PRIORITY_VERY_HIGH, &hTouchTask) == UEZ_ERROR_NONE) {
            //
            // emWin application
            //
            GUI_ExecDialogBox(_iDemoDialog, GUI_COUNTOF(_iDemoDialog), &_DemoDialog, 0,0,0);
                        
            UEZLCDGetFrame(hLCD, 0, (void **)&pixels);
#if (COMPILER_TYPE != VisualC)
            swim_window_open(&window,
                  DISPLAY_WIDTH,
                  DISPLAY_HEIGHT,
                  pixels,
                  0,
                  0,
                  DISPLAY_WIDTH-1,
                  DISPLAY_HEIGHT-1,
                  2,
                  BLACK,
                  RGB(0, 0, 0),
                  RED);
#endif
            _RequestExit = 1;
            while (_RequestExit == 1) {
              UEZTaskDelay(1);  // Wait for touch task to terminate
            }
          }
          //
          // Close touch screen
          //
          UEZTSClose(_hTouchScreen, _hTSQueue);
        }
        UEZQueueDelete(_hTSQueue);
      }
      //
      // Close the LCD
      //
      UEZLCDBacklight(hLCD, 255); //Reset Backlight level.
      UEZLCDClose(hLCD);
    }
    //
    // emWin cleanup
    //
}
