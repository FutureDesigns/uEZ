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
#include <uEZLCD.h>

#undef Status
#include "AppDemo.h"
#include "GUI.h"
#include "GUIDEMO/GUIDEMO.h"
#include <string.h>
#include <uEZLCD.h>
//#include <Config_App.h>
#include <uEZPlatform.h>


#ifndef EMWIN_LOAD_ONCE
#define EMWIN_LOAD_ONCE 1
#endif

/*********************************************************************
*
*       Local data
*
**********************************************************************
*/


static T_uezDevice _hTouchScreen;
static T_uezQueue  _hTSQueue;
static U8          _RequestExit;
TUInt8 G_emWinLoaded = EFalse;
T_uezTask      G_hTouchTask;


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

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

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
        if (UEZTaskCreate((T_uezTaskFunction)_TouchTask, "TouchTask",  1024, 0, UEZ_PRIORITY_VERY_HIGH, &G_hTouchTask) == UEZ_ERROR_NONE) {

          //
          // emWin application
          //
          GUIDEMO_Main();

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


/*************************** End of file ****************************/
