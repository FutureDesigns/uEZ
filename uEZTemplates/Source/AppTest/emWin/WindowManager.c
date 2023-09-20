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
#include <HAL/EMAC.h>
#include <uEZMemory.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <uEZGPIO.h>
#include "emWin/WindowManager.h"
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <Config_App.h>
#include "NVSettings.h"
#include "uEZFile.h"
#include "Audio.h"
#include <uEZToneGenerator.h>
#include <uEZAudioMixer.h>
   
#include "WindowManager.h"
#include "Create_Window_Functions.h"
#include "../Storage_Callbacks.h"
     
/*---------------------------------------------------------------------------*
 * Memory placement section:
 *---------------------------------------------------------------------------*/
// See platform file for general LCD framebuffer static allocation.

//Allocate emWin memory (general purpose GUI memory plus additional LCD frames memory)
UEZ_PUT_SECTION(".emWin", static TUInt8 _emWinMemory [EMWIN_RAM_SIZE]);
TUInt8 *_emWinMemoryptr = _emWinMemory;

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

#define BACKLIGHT_TIMEOUT   30000 //300 seconds
#define BACKLIGHT_TIMEOUT_ENABLED  0

#define EMWIN_BASE_ADDRESS (TUInt32) _emWinMemoryptr //Keep this define for now

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
WM_HWIN G_SystemWindows[NUM_WINDOWS];
// backlight management examples
static TUInt8 G_backlightLevel = 0;
#if (BACKLIGHT_TIMEOUT_ENABLED == 1)
static TBool G_backlightState = ETrue;
static TUInt16 Lcd_Backlight_Timeout = BACKLIGHT_TIMEOUT; 
#endif

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
        Storage_Initialize();
    	G_SystemWindows[HOME_SCREEN] = HomeScreen_Create();
        G_SystemWindows[STORAGE_SCREEN] = StorageWindow_Create();
#if (UEZ_PROCESSOR == NXP_LPC1788)
		// no slideshow on platforms without QSPI/OSPI
#endif
#if (UEZ_PROCESSOR == NXP_LPC4088)
		G_SystemWindows[SLIDESHOW_SCREEN] = SlideShowWindow_Create();
#endif
#if (UEZ_PROCESSOR == NXP_LPC4357)
		G_SystemWindows[SLIDESHOW_SCREEN] = SlideShowWindow_Create();
#endif

    }
}

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
#if (BACKLIGHT_TIMEOUT_ENABLED == 1)
        if(Lcd_Backlight_Timeout == 0) {//turn off backlight
          if(G_backlightState) { // only turn off if backlight was on
            WindowManager_BackLight_Off();
            G_backlightState = EFalse;
          }
        }
#endif
        //
        // Wait for 100ms for a new touch event to occur. Else skip over to give the
        // task a chance to respond to an exit request.
        //
        if (UEZQueueReceive(_hTSQueue, &inputEvent, 100) == UEZ_ERROR_NONE) {
#if (BACKLIGHT_TIMEOUT_ENABLED == 1)
            Lcd_Backlight_Timeout--;
#endif
            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
                State.x = inputEvent.iEvent.iXY.iX;
                State.y = inputEvent.iEvent.iXY.iY;
                State.Pressed = 1;
#if (BACKLIGHT_TIMEOUT_ENABLED == 1)
                Lcd_Backlight_Timeout = BACKLIGHT_TIMEOUT;
#endif
            } else {
                State.x = -1;
                State.y = -1;
                State.Pressed = 0;
            }
#if (BACKLIGHT_TIMEOUT_ENABLED == 1)
            if(!G_backlightState) {
              if(Lcd_Backlight_Timeout == BACKLIGHT_TIMEOUT) {
                WindowManager_BackLight_On(G_backlightLevel);
                G_backlightState = ETrue;
              }
            }
#endif
            GUI_PID_StoreState(&State);
        }
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:    WindowManager_Start_emWin
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
    //static TBool init = EFalse;
    //if (!init) {    
          memset((void *)UEZPlatform_GetBaseAddress(), 0x0, LCD_FRAMES_SIZE);
          //init = ETrue;
    //}
            
    //
    // Check that frame buffer memory fits into VRAM memory and we have empty memory left
    //
    if (GUI_VRAM_SIZE <= (FRAME_SIZE*3)) {
      return UEZ_ERROR_OUT_OF_MEMORY;  // Error, not enough memory
    }

    //
    // emWin startup
    //
    WM_SetCreateFlags(WM_CF_MEMDEV); // Enable memory devices
    GUI_Init();
    WM_SetDesktopColor(GUI_BLACK);
    GUI_MULTIBUF_Config(2);
    WM_MULTIBUF_Enable(1); // enable automatic mult-buffering

    //Created the touch screen task
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &_hTSQueue) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: no queue for TS!");
    }

    // Open touch screen
    if (UEZTSOpen("Touchscreen", &_hTouchScreen, &_hTSQueue) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: No Touchscreen device!");
    }

    // Create touch task
    if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask", 1100, 0,
            UEZ_PRIORITY_VERY_HIGH, &hTouchTask) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: Could not start TouchTask!");
    }

    //Turn on the LCD
    if (UEZLCDOpen("LCD", &G_lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("Main: No LCD device!");
    }

    //Zero out the window handler
    for(i = 0; i < NUM_WINDOWS; i++) {
        G_SystemWindows[i] = 0;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEmWinGetRAMAddr
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for emWin that returns the GUI RAM address.
 *---------------------------------------------------------------------------*/
TUInt32 UEZEmWinGetRAMAddr(void)
{
     static TBool init = EFalse;
     if (!init) {
          memset((void *)EMWIN_BASE_ADDRESS, 0x00, EMWIN_RAM_SIZE);
          init = ETrue;
     }
     return EMWIN_BASE_ADDRESS;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEmWinGetRAMSize
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for emWin that returns the GUI RAM size.
 *---------------------------------------------------------------------------*/
TUInt32 UEZEmWinGetRAMSize(void)
{
    return EMWIN_RAM_SIZE;
}

/*---------------------------------------------------------------------------*
 * Routine:    WindowManager_Close_emWin
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

/*---------------------------------------------------------------------------*
 * Routine: WindowManager_BackLight_On
 *---------------------------------------------------------------------------*/
/** Set the backlight from off to the new desired level, save this level
 *
 *@param [in] alevel      New backlight level to set
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_BackLight_On(TUInt8 alevel)
{
  if(G_lcd != 0) {
    if(alevel > G_backlightLevel) { // going up
      for (TUInt8 i = 0; i < alevel; i++) {
          UEZLCDBacklight(G_lcd, i);
          UEZTaskDelay(2);
      }
    } else { // going down
      for (TUInt8 i = G_backlightLevel; i >= alevel; i--) {
          UEZLCDBacklight(G_lcd, i);
          UEZTaskDelay(2);
      }
    }
    G_backlightLevel = alevel;
  }
}

/*---------------------------------------------------------------------------*
 * Routine: WindowManager_BackLight_Off
 *---------------------------------------------------------------------------*/
/** Set backlight from current level down to zero gradually 
 */
 /*---------------------------------------------------------------------------*/
void WindowManager_BackLight_Off(void) 
{
  if(G_lcd != 0) {
    for (TUInt8 i = G_backlightLevel; i > 0; i--) {
        UEZLCDBacklight(G_lcd, i);
        UEZTaskDelay(2);
    }
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  _WriteByte2File
 *---------------------------------------------------------------------------*
 * Description:
 *      Helper function for WindowManager_SaveScreenShotBMP. Added for uEZ
 *      from SEGGER example code.
 *---------------------------------------------------------------------------*/
static T_uezFile dumpfile;
// At least with current Crossworks project it will always align to 4 byte boundary anyway.
// But just in case lets do some forced alignment to 0xXXXXXX00.
UEZ_ALIGN_VAR(256,static U8 ssBuffer[512]); //
static TUInt32 ssindex;
static U8 sscount = 0;
static TUInt32 sstick = 0;

static void _WriteByte2File(U8 Data, void * p)
{	
  TUInt32 numWritten;
  ssBuffer[ssindex++]=Data;
  if(ssindex==sizeof(ssBuffer)) {
    UEZFileWrite(dumpfile, &ssBuffer, ssindex, &numWritten);
    printf("*");
    ssindex = 0;
    memset(&ssBuffer, 0, sizeof(ssBuffer));
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  WindowManager_SaveScreenShotBMP
 *---------------------------------------------------------------------------*
 * Description:
 *      This function will print the current screen onto a BMP file on SD card
 *---------------------------------------------------------------------------*/
void WindowManager_SaveScreenShotBMP(char driveNumber){
    TUInt32 numWritten;
    T_uezDevice tone;
    char fname [20];
    char syncPath[3] = {driveNumber,':',0}; // first char is drive number path
  
    if(UEZTickCounterGetDelta(sstick) > 1000) {
    /* This function avoids callback functions to be called and 
     * therefore avoids processing paint events. - SEGGER Adrian*/
    WM_Deactivate(); 

    sprintf(fname,"%c:cap%d.bmp", driveNumber, sscount++);
    printf("\n\nWriting: %s...\n",fname);
    if (UEZFileOpen(fname, FILE_FLAG_WRITE, &dumpfile) == UEZ_ERROR_NONE) {
     if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
       UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
       UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1500), 25);

        ssindex = 0;
        memset(&ssBuffer, 0, sizeof(ssBuffer));
        GUI_BMP_Serialize( _WriteByte2File, &dumpfile);
        UEZFileWrite(dumpfile, &ssBuffer, ssindex, &numWritten);
        if (UEZFileSystemSync(syncPath) != UEZ_ERROR_NONE) {
        }
        UEZFileClose(dumpfile);

       UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 25);
       UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
       UEZToneGeneratorClose(tone);
     }
    }

    WM_Activate();  // re-activate the WM
    sstick = UEZTickCounterGet();
    }
}

void WindowManager_InvalidateCurrentWindow(void)
{
    //WM_InvalidateWindow(hWin); // per window
    GUI_RECT Rect = {.x0 = 0, .y0 = 0, .x1 = UEZ_LCD_DISPLAY_WIDTH, .y1 = UEZ_LCD_DISPLAY_HEIGHT}; // whole area, any window
    WM_InvalidateArea(&Rect);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    MainWindow.c
 *-------------------------------------------------------------------------*/
