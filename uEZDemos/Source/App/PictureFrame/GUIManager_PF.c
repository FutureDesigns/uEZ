/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *     The code in this file takes care of starting and stopping emWin,
 *     emWin is never closed in the application but the code was put in
 *     any. It also handles opening the LCD and turning on the backlight
 *     for the first time, when emWin is started. The screens are managed
 *     by showing the desired dialog, hiding is taken care of internally.
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
#include <uEZ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GUI.h"
#include "WM.h"
#include <UEZLCD.h>

#include "GUIManager_PF.h"
#include "Create_Window_Functions.h"

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
WM_HWIN G_SystemWindows_PF[NUM_WINDOWS];

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
T_uezError GUIManager_Start_emWin_PF()
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

    //Created the touch screen task
    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: no queue for TS!");

    // Open touch screen
    if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: No Touchscreen device!");

    // Create touch task
    if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask", 1024, 0,
            UEZ_PRIORITY_VERY_HIGH, &hTouchTask) != UEZ_ERROR_NONE)
        UEZFailureMsg("Main: Could not start TouchTask!");

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
        G_SystemWindows_PF[i] = 0;
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
T_uezError GUIManager_Close_emWin_PF()
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
 * Routine: GUIManager_Hide_Other_Windows
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 *---------------------------------------------------------------------------*/
static void GUIManager_Hide_Other_Windows(TUInt8 aWindow)
{
    TUInt8 i=0;

    while(G_SystemWindows_PF[i]){
        if(i != aWindow){
            if(G_SystemWindows_PF[i]){
                WM_SendMessageNoPara(G_SystemWindows_PF[i], WM_APP_LOST_FOCUS);
                WM_HideWindow(G_SystemWindows_PF[i]);
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
void GUIManager_Show_Window_PF(TUInt8 aWindow)
{
    GUIManager_Hide_Other_Windows(aWindow);

    if(aWindow != HIDE_ALL){
        WM_SendMessageNoPara(G_SystemWindows_PF[aWindow], WM_APP_GAINED_FOCUS);
        WM_ShowWindow(G_SystemWindows_PF[aWindow]);
        WM_BringToTop(G_SystemWindows_PF[aWindow]);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: GUIManager_Create_All_Active_Windows
 *---------------------------------------------------------------------------*/
/** Create all the windows that the system will use.
 *
 */
 /*---------------------------------------------------------------------------*/
void GUIManager_Create_All_Active_Windows_PF(void)
{
    static TBool iHaveRun = EFalse;

    if(!iHaveRun){
        G_SystemWindows_PF[HOME_SCREEN] = PictureFame();
        G_SystemWindows_PF[SETTINGS_SCREEN] = PictureFrameSettings();
    }
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    MainWindow.c
 *-------------------------------------------------------------------------*/
