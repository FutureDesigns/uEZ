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
#include <UEZLCD.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GUI.h"
#include "WM.h"
#include <UEZPlatform.h>
#include "WindowManager.h"
#include "Create_Window_Functions.h"
#include <uEZLCD.h>

extern void EnterNameScreenClose();
/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static T_uezDevice _hTouchScreen;
static T_uezQueue _hTSQueue;
static U8 _RequestExit;
static T_uezTask hTouchTask;
static T_uezDevice G_lcd;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
WM_HWIN G_SystemWindows[NUM_WINDOWS];

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
T_uezError WindowManager_Start_emWin()
{
    U32 FrameBufferSize;
    TUInt8 i;
#if MAGELLAN_DEBUG
    printf("Starting emWin\n");
#endif

    //Initialize the memory and start emWin
    FrameBufferSize = UEZ_LCD_DISPLAY_WIDTH * UEZ_LCD_DISPLAY_HEIGHT
            * GUI_NUM_VIRTUAL_DISPLAYS * GUI_PIXEL_WIDTH;
    memset((void*)LCD_FRAME_BUFFER, 0, FrameBufferSize);
    //
    // Check that frame buffer memory fits into VRAM memory and we have empty memory left
    //
    if (GUI_VRAM_SIZE <= FrameBufferSize) {
        return UEZ_ERROR_OUT_OF_MEMORY; // Error, not enough memory
    }
    //
    // Assign memory left to emWin memory
    //
    GUI_pMem = (U32*)(GUI_VRAM_BASE_ADDR + FrameBufferSize);
    GUI_MemSize = (GUI_VRAM_SIZE - FrameBufferSize);///2;
    //
    // emWin startup
    //
    WM_SetCreateFlags(WM_CF_MEMDEV); // Enable memory devices
    GUI_Init();
    WM_SetDesktopColor(GUI_BLACK);
    GUI_MULTIBUF_Config(2);
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering

    //Created the touch screen task
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: no queue for TS!");

#if MAGELLAN_DEBUG
    printf("Creating Touch Task\n");
#endif
    // Open touch screen
    if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: No Touchscreen device!");

    // Create touch task
    if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask", 1024, 0,
            UEZ_PRIORITY_VERY_HIGH, &hTouchTask) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: Could not start TouchTask!");

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
            UEZTaskDelay(10);
        }
    }

    //Zero out the window handler
    for(i = 0; i < NUM_WINDOWS + 1; i++){
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
T_uezError WindowManager_Close_emWin()
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
void WindowMangager_Show_Window(TUInt8 aWindow)
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
