/*-------------------------------------------------------------------------*
 * File:  TS_Test.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Implements Layout and functionality of TS_Test.c.
 * 
 *     This runs a touchscreen test using an emWin GUI. Output is also 
 *     printed to the serial console as well. This test is geared towards
 *     capacitive touch displays to check that they are properly calibrated.
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
#include <string.h>
#include <stdio.h>

#include <uEZDeviceTable.h>
#include <HAL/HAL.h>
#include <uEZGPIO.h>
#include <uEZProcessor.h>
#include <UEZPlatform.h>
#include <stdlib.h>
#include <stdio.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "../GUICommon/LookAndFeel.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW                   (GUI_ID_USER + 0x00)

#define WINDOW_XSIZE                (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE                (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS                 (0)
#define WINDOW_YPOS                 (0)

#define BUTTON_XSIZE                (40)
#define BUTTON_YSIZE                (40)
#define BUTTON_XPOS(n)              (0 + (n * (BUTTON_XSIZE + 0)))
#define BUTTON_YPOS(n)              (0 + (n * (BUTTON_YSIZE + 0)))

#define NUM_COL                     (WINDOW_XSIZE / (BUTTON_XSIZE + 0))
#define NUM_ROW                     (WINDOW_YSIZE / (BUTTON_YSIZE + 0))

#define NUM_BUTTONS                 (NUM_COL * NUM_ROW)

#define BUTTON_COLOR_SELECTED       (GUI_BLUE)
#define BUTTON_COLOR_NOT_SLECTED    (GUI_WHITE)

static TUInt32 G_Button_ID = (GUI_ID_USER + 0x01);
static int lastX = 0;
static int lastY = 0;
static int expectedX = 0;
static int expectedY = 0;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleButton(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static GUI_WIDGET_CREATE_INFO G_InfoTemplate[1] = {
        { BUTTON_CreateIndirect, "",    0 , 0, 0, BUTTON_XSIZE, BUTTON_YSIZE, 0, 0, 0},
};

static T_LAFMapping G_LandFTemplate[1] = {
        { 0   , "", BUTTON_COLOR_NOT_SLECTED, GUI_BLACK   , &GUI_Font24_ASCII, LAFSetupButton, IHandleButton},
};
/** Structure to hold all of the widgets used in this dialog*/
static GUI_WIDGET_CREATE_INFO _iTS_TestDialog[NUM_BUTTONS + 1] = {
    //Function                Name          ID                 XP                   YP                      XS                       YS
    { WINDOW_CreateIndirect    , ""          , ID_WINDOW        , WINDOW_XPOS       , WINDOW_YPOS         , WINDOW_XSIZE     , WINDOW_YSIZE, 0, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping TS_TestMapping[NUM_BUTTONS + 2] = {
    { ID_WINDOW     ,""     ,GUI_WHITE, GUI_BLACK, &GUI_Font24_ASCII , LAFSetupWindow, 0},

    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
//static TBool G_Active = EFalse;

/*---------------------------------------------------------------------------*
 * Routine:  IHandleButton
 *---------------------------------------------------------------------------*/
/** 
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID			ID of widget that caused the event.
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleButton(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if(BUTTON_GetBkColor(WM_GetDialogItem(pMsg->hWin, aID), BUTTON_CI_UNPRESSED) == BUTTON_COLOR_NOT_SLECTED){
            BUTTON_SetBkColor(WM_GetDialogItem(pMsg->hWin, aID), BUTTON_CI_UNPRESSED, BUTTON_COLOR_SELECTED);
        } else {
            BUTTON_SetBkColor(WM_GetDialogItem(pMsg->hWin, aID), BUTTON_CI_UNPRESSED, BUTTON_COLOR_NOT_SLECTED);
        }
        int buttonNum = aID - GUI_ID_USER;
        expectedX = _iTS_TestDialog[buttonNum].x0 + (BUTTON_XSIZE/2);
        expectedY = _iTS_TestDialog[buttonNum].y0 + (BUTTON_YSIZE/2);
        printf("%03d,%03d,%03d,%03d,%03d\n\r", expectedX, lastX, expectedY, lastY, buttonNum);
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
    PARAM_NOT_USED(pMsg);
}

/*---------------------------------------------------------------------------*
 * Routine:	_TS_TestDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _TS_TestDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, TS_TestMapping);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(TS_TestMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
	case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

static void IFillStructures(void)
{
    TUInt32 i, j;
    TUInt32 count = 1;

    for(i = 0; i < NUM_ROW; i++){
        for(j = 0; j < NUM_COL; j++){
            memcpy((void*)&_iTS_TestDialog[count], (void*)&G_InfoTemplate, sizeof(G_InfoTemplate));
            _iTS_TestDialog[count].Id = G_Button_ID;
            _iTS_TestDialog[count].x0 = BUTTON_XPOS(j);
            _iTS_TestDialog[count].y0 = BUTTON_XPOS(i);
            memcpy((void*)&TS_TestMapping[count] , (void*)&G_LandFTemplate, sizeof(G_LandFTemplate));
            TS_TestMapping[count++].iID = G_Button_ID++;
        }
    }
    memset((void*)&TS_TestMapping[count], 0, sizeof(G_LandFTemplate));
}

/*---------------------------------------------------------------------------*
 * Routine:	TS_Test_Create
 *---------------------------------------------------------------------------*/
/** Create the TS_Test. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN TS_Test_Create(void)
{
    IFillStructures();
    return GUI_CreateDialogBox(_iTS_TestDialog, GUI_COUNTOF(_iTS_TestDialog), &_TS_TestDialog, 0,0,0);
}

static U32 _TouchTask(T_uezTask aMyTask, void *aParameters);

#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>

//Required for emWin Initialization
static T_uezDevice _hTouchScreen;
static T_uezQueue  _hTSQueue;
static U8          _RequestExit;
static T_uezDevice hLCD;

void TS_Test_Demo(const T_choice *aChoice)
{
    //T_uezDevice    hLCD;
    U32            FrameBufferSize;
    T_uezTask      hTouchTask;

    IFillStructures();

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
    GUI_SetFont(&GUI_Font32_ASCII);
    WM_SetDesktopColor(GUI_BLACK);
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering
    GUI_DispStringHCenterAt("TS APP Demo Task.\n Reset is required to exit the APP!", (WINDOW_XSIZE / 2), (WINDOW_YSIZE / 2));
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
            GUI_ExecDialogBox(_iTS_TestDialog, GUI_COUNTOF(_iTS_TestDialog), &_TS_TestDialog, 0,0,0);

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
      UEZLCDClose(hLCD);
    }
    //
    // emWin cleanup
    //
    GUI_Exit();
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
static U32 _TouchTask(T_uezTask aMyTask, void *aParameters) {
    T_uezInputEvent inputEvent;
    static GUI_PID_STATE  State = { 0 };

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
              lastX = State.x;
              lastY = State.y;              
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
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    TS_Test.c
 *-------------------------------------------------------------------------*/
