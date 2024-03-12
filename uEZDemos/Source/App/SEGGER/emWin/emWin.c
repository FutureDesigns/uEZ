/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2011  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.09 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : emWin.c
Purpose     : emWin demo
---------------------------END-OF-HEADER------------------------------
*/

#include <uEZ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GUI.h"
#include "WM.h"
#include <uEZLCD.h>
#include "uEZFile.h"
#include <uEZMemory.h>
#include "Audio.h" // audio for screenshot button tone
#include <uEZToneGenerator.h>
#include <uEZAudioMixer.h>

#undef Status
#include "AppDemo.h"
#include "GUI.h"
#include "GUIDEMO/GUIDEMO.h"
#include <string.h>
#include <uEZPlatform.h>

#include "emWin/WindowManager.h"
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <Config_App.h>
#include "NVSettings.h"

WM_HWIN MQTT_Create(void);
extern WM_HWIN MQTT_Create(void);

#ifndef EMWIN_LOAD_ONCE
#define EMWIN_LOAD_ONCE 1
#endif

#ifndef USING_SEGGER_EMWIN_SPY
#define USING_SEGGER_EMWIN_SPY 0
#endif

#ifndef UEZ_AWS_IOT_CLIENT_DEMO
#define UEZ_AWS_IOT_CLIENT_DEMO  0
#endif

#if (INCLUDE_EMWIN == 1)
WM_HWIN G_SystemWindows[NUM_WINDOWS];
static TUInt8 G_CurrentWindow = 0;
static T_uezSemaphore WM_TochSem = 0;
static GUI_PID_STATE G_State;
static TBool G_DemoWindowDone = EFalse;

/*********************************************************************
*
*       Local data
*
**********************************************************************
*/


static T_uezDevice _hTouchScreen;
static T_uezQueue  _hTSQueue;
static U8          _RequestExit;
TBool             G_emWinLoaded = EFalse;
T_uezTask          G_hTouchTask = (T_uezTask )NULL;
//static T_uezDevice G_lcd;


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


/*********************************************************************
*
*       _TouchTask
*
* Function description:
*   Handle touch screen input
*
*/
static U32 _TouchTask(T_uezTask aMyTask, void *aParameters) 
{
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


void WindowManager_Get_Touch( GUI_PID_STATE *aState)
{
    if (WM_TochSem == 0){
        UEZSemaphoreCreateBinary(&WM_TochSem);
    }
    UEZSemaphoreGrab(WM_TochSem, 500);
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

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

static void runDemoScreenTillHomeExit(void)
{
    G_DemoWindowDone = EFalse;
    while (!G_DemoWindowDone) {
        GUI_Exec();
        GUI_X_ExecIdle(); // By default this is just a task delay.
    }
}

/*********************************************************************
*
*       emWin
*
* Function description:
*   emWin demo application
*
*/
void emWin(const T_choice *aChoice) {

  T_uezDevice    hLCD;

  (void)aChoice;
  (void) runDemoScreenTillHomeExit;
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
      memset((void *)UEZPlatform_GetBaseAddress(), 0x0, FRAME_SIZE*2);
      memset((void *)EMWIN_BASE_ADDRESS, 0x0, EMWIN_RAM_SIZE);
      init = ETrue;
    }
    
  // Check that frame buffer memory fits into VRAM memory and we have empty memory left
  if (GUI_VRAM_SIZE <= (FRAME_SIZE*3)) {
    return;  // Error, not enough memory
  }
  
  //
  // emWin startup
  //
#if EMWIN_LOAD_ONCE
  if(G_emWinLoaded == EFalse) {
    WM_SetCreateFlags(WM_CF_MEMDEV);  // Enable memory devices
    GUI_Init();
    WM_SetDesktopColor(GUI_BLACK);
    //GUI_MULTIBUF_Config(1);
    GUI_MULTIBUF_Config(2);
    WM_MULTIBUF_Enable(1);
    G_emWinLoaded = ETrue;
  }
#else
  WM_SetCreateFlags(WM_CF_MEMDEV);  // Enable memory devices
  GUI_Init();
#endif
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
        if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask",  1024, 0, UEZ_PRIORITY_HIGH, &G_hTouchTask) == UEZ_ERROR_NONE) {

#if (USING_SEGGER_EMWIN_SPY == 1)        
         GUI_SPY_X_StartServer(); // Run a user provided function to start RTT or network emSPY task to allow monitoring emWin from a PC.
         UEZTaskDelay(10000); // Delay to allow RTT connection to be made to the GUI before it starts running, to capture the initial memory usage.
#endif
         //
         // emWin application or demo from selection
         //
#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)
         if(memcmp(aChoice->iText, "MQTT Demo", 9) == 0) { // run FDI MQTT demo screen in app menu
            WindowManager_Create_All_Active_Windows();
            WindowManager_Show_Window(MQTT_SCREEN);

            runDemoScreenTillHomeExit();
         }
#endif
#if 0
         if(memcmp(aChoice->iText, "TODO Demo", 9) == 0) { 
            WindowManager_Create_All_Active_Windows();
            WindowManager_Show_Window(NEW_DEMO_SCREEN);

            runDemoScreenTillHomeExit();
         }
#endif
         if(memcmp(aChoice->iText, "emWin Demo", 10) == 0) { // Run SEGGER's GUIDEMO scren from the main menu.
           GUIDEMO_Main();
         }
         
         _RequestExit = 1; // common mechanism for multiple demos
         while (_RequestExit == 1) {
           UEZTaskDelay(1);  // Wait for touch task to terminate
         }
#if (USING_SEGGER_EMWIN_SPY == 1)
         GUI_SPY_X_StopServer();
#endif
        }
        //
        // Close touch screen and emwin
        //
        WindowManager_Close_emWin();
        init = EFalse;
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
#if !EMWIN_LOAD_ONCE
  GUI_Exit();
#endif
}


/*---------------------------------------------------------------------------*
* Required by emWin: Need to find a better place for this
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

TUInt32 UEZEmWinGetRAMSize(void)
{
return EMWIN_RAM_SIZE;;
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

    if(!iHaveRun) {        
        //MQTT_Initialize();
    	//G_SystemWindows[HOME_SCREEN] = 1;
    	G_SystemWindows[MQTT_SCREEN] = MQTT_Create();
        //TODO add new window creation function here.
    }
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
    //_RequestExit = 1;

    //clean up the touch screen task
    UEZTSClose(_hTouchScreen, _hTSQueue);
    //UEZQueueDelete(_hTSQueue);

    //emWin Clean up and restart example
    // currently we need to clear and re-init emWin between our demo and GUIDemo
    GUI_Exit();
    G_emWinLoaded = EFalse; // make sure that GUI_Init is called again

    //Close the LCD
    //UEZLCDClose(G_lcd);

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
                G_CurrentWindow = aWindow;
            }
        } else {
          G_DemoWindowDone = ETrue;
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
 * Routine: WindowManager_GetCurrent_Window
 *---------------------------------------------------------------------------*/
/** Returns the index of the current window from 0 to NUM_WINDOWS.
 *  Multiple windows can run void XXXX_Dialog(WM_MESSAGE *pMsg) when not 
 *  focused on that window, so use this restrict behavior to when the window is
 *  really active (or not active).
 *
 *  @return                 Index of current window
 */
 /*---------------------------------------------------------------------------*/
TUInt8 WindowManager_GetCurrent_Window(void)
{
  return G_CurrentWindow;
}

#if 0
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
#endif


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
UEZ_ALIGN_VAR(256,static U8 ssBuffer[512]);
static TUInt32 ssindex;
static U8 sscount = 0;
static TUInt32 sstick = 0;

static void _WriteByte2File(U8 Data, void * p)
{
  PARAM_NOT_USED(p);
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

#endif

/*************************** End of file ****************************/
