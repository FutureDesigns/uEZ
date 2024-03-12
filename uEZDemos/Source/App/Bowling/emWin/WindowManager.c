/*-------------------------------------------------------------------------*
 * File: WindowManager.c
 *-------------------------------------------------------------------------*/
/** @addtogroup WindowMangaerPictureFame
 * @{
 *     @brief Handle the management of windows on the screen.
 *
 *     The code in this file takes care of starting and stopping emWin,
 *     emWin is never closed in the application but the code was put in
 *     any. It also handles opening the LCD and turning on the backlight
 *     for the first time, when emWin is started. The screens are managed
 *     by showing the desired dialog, hiding is taken care of internally.
 */
/*-------------------------------------------------------------------------*/
#include "Config_Build.h"
#if (RTOS != WIN32)
#include <uEZ.h>
#include <uEZLCD.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GUI.h"
#include "WM.h"
#include <uEZPlatform.h>
#include <uEZMemory.h>
#include "WindowManager.h"
#include "Create_Window_Functions.h"
#include <uEZLCD.h>

#ifndef EMWIN_LOAD_ONCE
#define EMWIN_LOAD_ONCE 1
#endif

extern void EnterNameScreenClose();
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static T_uezDevice _hTouchScreen;
static T_uezQueue  _hTSQueue;
static U8          _RequestExit;
TUInt8             G_emWinLoaded = EFalse;
static T_uezTask   G_hTouchTask;
static T_uezDevice G_lcd;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
WM_HWIN G_SystemWindows[NUM_WINDOWS];


/*********************************************************************
*
*       Local code
*
**********************************************************************
*/
// See platform file for general LCD framebuffer static allocation.

//Allocate emWin memory (general purpose GUI memory plus additional LCD frames memory)
UEZ_PUT_SECTION(".emWin", static TUInt8 _emWinMemory [EMWIN_RAM_SIZE]);
TUInt8 *_emWinMemoryptr = _emWinMemory;

#define EMWIN_BASE_ADDRESS (TUInt32) _emWinMemoryptr //Keep this define for now

/*---------------------------------------------------------------------------*
 * Routine:  _TouchTask
 *---------------------------------------------------------------------------*/
/** This is the touch task that feed emWin the Touch coordinates
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aMyTask     Not used.
 *  @param [in] aParameters Not used.
 *  @return                 Return Code
 */
 /*---------------------------------------------------------------------------*/
static U32 _TouchTask(T_uezTask aMyTask, void *aParameters)
{
    T_uezInputEvent inputEvent;
    GUI_PID_STATE State = { 0 };

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
                State.x = inputEvent.iEvent.iXY.iX;
                State.y = inputEvent.iEvent.iXY.iY;
                State.Pressed = 1;
            } else {
                State.x = -1;
                State.y = -1;
                State.Pressed = 0;
            }
            GUI_PID_StoreState(&State);
        }
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:    GUIManager_Start_emWin
 *---------------------------------------------------------------------------*/
/** Function to start emWin and initialize the memory. Also opens the LCD and
 *  turns on the backlight.
 *
 *  @return                 uEZ error code
 */
 /*---------------------------------------------------------------------------*/
T_uezError WindowManager_Start_emWin(void)
{
  TUInt8 i;

#if MAGELLAN_DEBUG
    printf("Starting emWin\n");
#endif
  
  // Keep various memory location declarations from optimizing out
  ((TUInt8 volatile *)_emWinMemoryptr)[0] = _emWinMemoryptr[0];
    
  // Assign memory left to emWin memory (emWin section only)
  GUI_pMem    = (U32*)EMWIN_BASE_ADDRESS;
  GUI_MemSize = (EMWIN_RAM_SIZE);

    //Initialize the memory and start emWin
    
    /* TODO: If GUI_NUM_VIRTUAL_DISPLAYS is ever different from MAX_NUM_FRAMES
    you should adjust the numbers in Config_App.h

    Note that below we zero the framebuffer every time we call this function. (old functionality)
    But we don't have the ability to zero it anywhere else or track if we already zeroed it.
    To support some low power modes we may want to track if zeroed or not,
    either to prevent zeroing again or to re-zero if we lose data.
    This should then be handled possibly in the platform file depending on the type of RAM.
    */

    // New routine:
    static TBool init = EFalse;
    if (!init) {
      // Clear the frame buffer and emWin RAM
      memset((void *)UEZPlatform_GetBaseAddress(), 0x0, FRAME_SIZE);
      memset((void *)EMWIN_BASE_ADDRESS, 0x0, EMWIN_RAM_SIZE);
      init = ETrue;
    }
    
  // Check that frame buffer memory fits into VRAM memory and we have empty memory left
  if (GUI_VRAM_SIZE <= (FRAME_SIZE)) {
    return UEZ_ERROR_OUT_OF_MEMORY;  // Error, not enough memory
  }
  
  //
  // emWin startup
  //
#if EMWIN_LOAD_ONCE
  if(G_emWinLoaded == EFalse) {
    WM_SetCreateFlags(WM_CF_MEMDEV);  // Enable memory devices
    GUI_Init();
    G_emWinLoaded = ETrue;
  }
#else
  WM_SetCreateFlags(WM_CF_MEMDEV);  // Enable memory devices
  GUI_Init();
#endif


    WM_SetDesktopColor(GUI_BLACK);
    GUI_MULTIBUF_Config(2);
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering

    //Created the touch screen task
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &_hTSQueue) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: no queue for TS!");
    }

#if MAGELLAN_DEBUG
    printf("Creating Touch Task\n");
#endif

    // Open touch screen
    if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: No Touchscreen device!");
    }

    // Create touch task
    if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask", 1024, 0,
            UEZ_PRIORITY_VERY_HIGH, &G_hTouchTask) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: Could not start TouchTask!");
    }

#if MAGELLAN_DEBUG
    printf("Turning on LCD and Backlight\n");
#endif

    //Turn on the LCD
    if (UEZLCDOpen("LCD", &G_lcd) == UEZ_ERROR_NONE) {
        UEZLCDBacklight(G_lcd, 0);
        UEZLCDOff(G_lcd);
        UEZLCDOn(G_lcd);
        for (i = 0; i < 255; i++) {
            UEZLCDBacklight(G_lcd, i);
            UEZTaskDelay(1);
        }
    }

    //Zero out the window handler
    for(i = 0; i < NUM_WINDOWS; i++){
        G_SystemWindows[i] = 0;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:    GUIManager_Close_emWin
 *---------------------------------------------------------------------------*/
/** Function to free the memory and shut down emWin.
 *
 *  @return                 uEZ error code
 */
 /*---------------------------------------------------------------------------*/
T_uezError WindowManager_Close_emWin(void)
{
    //end the touch task
    _RequestExit = 1;
#if MAGELLAN_DEBUG
    printf("Closing emWin\n");
#endif

    //clean up the touch screen task
    UEZTSClose(_hTouchScreen, _hTSQueue);
    UEZQueueDelete(_hTSQueue);

    //emWin Clean up
    GUI_Exit();

    //Close the LCD
    UEZLCDClose(G_lcd);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: GUIManager_Hide_Other_Windows
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 *---------------------------------------------------------------------------*/
static void WindowManager_Hide_Other_Windows(TUInt8 aWindow)
{
    TUInt8 i = 1;

    EnterNameScreenClose();

    while(G_SystemWindows[i]){
        if(i != aWindow){
            if(G_SystemWindows[i]){
                WM_SendMessageNoPara(G_SystemWindows[i], WM_APP_LOST_FOCUS);
                WM_HideWindow(G_SystemWindows[i]);
            }
        }
        i++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine: GUIManager_Show_Window
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] aWindow     ID of the Window to show, see .h file.
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_Show_Window(TUInt8 aWindow)
{
    WindowManager_Hide_Other_Windows(aWindow);

    if(aWindow != HIDE_ALL){
        WM_SendMessageNoPara(G_SystemWindows[aWindow], WM_APP_GAINED_FOCUS);
        WM_ShowWindow(G_SystemWindows[aWindow]);
        WM_BringToTop(G_SystemWindows[aWindow]);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: GUIManager_Create_All_Active_Windows
 *---------------------------------------------------------------------------*/
/** Create all the windows that the system will use.
 *
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_Create_All_Active_Windows(void)
{
    static TBool iHaveRun = EFalse;

    if(!iHaveRun){
#if MAGELLAN_DEBUG
        printf("Creating System Window\n");
#endif
        G_SystemWindows[MAIN_SCREEN] = MainScreen_Create();
        G_SystemWindows[QUICK_START] = QuickStart_Create();
        G_SystemWindows[STANDARD_PLAY] = StandardPlayScreen_Create();
        G_SystemWindows[BOWLING] = BowlingScreen_Create();
    }
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    MainWindow.c
 *-------------------------------------------------------------------------*/
