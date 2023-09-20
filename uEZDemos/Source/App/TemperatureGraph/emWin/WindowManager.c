/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
/** @addtogroup WindowMangaer
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
#include <uEZ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GUI.h"
#include "WM.h"
#include <uEZLCD.h>

#include "WindowManager.h"


/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static T_uezDevice _hTouchScreen;
static T_uezQueue _hTSQueue;
static U8 _RequestExit;
static T_uezTask hTouchTask;
static T_uezDevice G_lcd;
static T_uezSemaphore WM_TochSem = 0;
static GUI_PID_STATE G_State;
/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
WM_HWIN G_SystemWindows[NUM_WINDOWS];

void WindowManager_Get_Touch( GUI_PID_STATE *aState)
{
    if (WM_TochSem == 0){
        UEZSemaphoreCreateBinary(&WM_TochSem);
    }
    UEZSemaphoreGrab(WM_TochSem, UEZ_TIMEOUT_INFINITE);
    GUI_PID_GetState(&G_State);
    memcpy((void*)aState, (void*)&G_State, sizeof(GUI_PID_STATE));
    UEZSemaphoreRelease(WM_TochSem);
}

void WindowManager_ClearTouches(void)
{
    GUI_PID_STATE state;
    TUInt8 i;

    WindowManager_Get_Touch(&state);

    while ( state.Pressed ){
        WindowManager_Get_Touch(&state);
    }

    for(i = 0; i < 5; i ++ ){
        WindowManager_Get_Touch(&state);
    }
}

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
#if(COMPILER_TYPE != VisualC)
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
#endif

/*---------------------------------------------------------------------------*
 * Routine:    WindowManager_Start_emWin
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
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering

	#if(COMPILER_TYPE != VisualC)
    //Created the touch screen task
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: no queue for TS!");

    // Open touch screen
    if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: No Touchscreen device!");

    // Create touch task
    if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask", 1024, 0,
            UEZ_PRIORITY_VERY_HIGH, &hTouchTask) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: Could not start TouchTask!");
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
    for(i = 0; i < NUM_WINDOWS + 1; i++){
        G_SystemWindows[i] = 0;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:    WindowManager_Close_emWin
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
 * Routine: WindowManager_Hide_Other_Windows
 *---------------------------------------------------------------------------*
 * Description:
 *  Local function to hide windows not being shown.
 * Inputs:
 * aWindow  -- ID of the window not being hidden
 *---------------------------------------------------------------------------*/
static void WindowManager_Hide_Other_Windows(TUInt8 aWindow)
{
    TUInt8 i=0;

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
 * Routine: WindowManager_Show_Window
 *---------------------------------------------------------------------------*/
/** Shows the desired window
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
 * Routine: WindowManager_Create_All_Active_Windows
 *---------------------------------------------------------------------------*/
/** Create all the windows that the system will use.
 *
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_Create_All_Active_Windows(void)
{
    static TBool iHaveRun = EFalse;

    if(!iHaveRun){
       
    	G_SystemWindows[HOME_SCREEN] = Temperature_Create();
       //G_SystemWindows[OPTION_SCREEN] = Temperature_Create();//Wrong code, see if this works though

    }
}

/*---------------------------------------------------------------------------*
 * Routine: WindowManager_UpdateCommonElements
 *---------------------------------------------------------------------------*/
/** Function used to set the common UI elements across all screens.
 *
 *@param [in] aMode      Mode the screen should be set to.
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_UpdateCommonElements(T_CommonMode aMode)
{

   
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    MainWindow.c
 *-------------------------------------------------------------------------*/
