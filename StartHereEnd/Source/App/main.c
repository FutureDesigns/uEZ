/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that tests the LCD, Queue, Semaphore, and
 *      touchscreen features.
 *
 * Implementation:
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*
* Includes:
*-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <UEZPlatform.h>
#include "NVSettings.h"
#include <uEZProcessor.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <UEZPlatform.h>
#include "emWin/WindowManager.h"
#include "MyTask.h"
#include <Calibration.h>

#if COMPILE_OPTION_USB_SDCARD_DISK
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif

#if FREERTOS_PLUS_TRACE
#include <trcUser.h>
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Task:  GUIInterfaceTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that starts the emWin GUI
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 GUIInterfaceTask(T_uezTask aMyTask, void *aParams)
{
    TBool done = EFalse;

    if (WindowManager_Start_emWin() != UEZ_ERROR_NONE) {
        UEZFailureMsg("Failed to start emWin!");
    }
        
    WindowManager_Create_All_Active_Windows();

    WindowManager_Show_Window(HOME_SCREEN);

    while (!done) {
        GUI_Exec();
        GUI_X_ExecIdle();
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Task:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      In the uEZ system, main() is a task.  Do not exit this task
 *      unless you want to the board to reset.  This function should
 *      setup the system and then run the main loop of the program.
 *---------------------------------------------------------------------------*/
void MainTask(void)
{
    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

// Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

// Calibration is only needed for resistive touch and not capacitive or no touch.
#if ((UEZ_DEFAULT_TOUCH != TOUCH_PCAP) || USE_RESISTIVE_TOUCH)
    Calibrate(CalibrateTestIfTouchscreenHeld());
#endif

    // Run the heartbeat task located in My.Task.c
    UEZTaskCreate(HeartbeatTask, "Heart", 1024, (void *)0, UEZ_PRIORITY_NORMAL, 0);
    
    // Run the teperature reading task located in My.Task.c
    UEZTaskCreate(TemperatureLoopTask, "Temperature", 1024, (void *)0, UEZ_PRIORITY_NORMAL, 0);
    
    //Start emWin
    UEZTaskCreate(GUIInterfaceTask, "GUIInterface", (4 * 1024), (void *)0,
            UEZ_PRIORITY_NORMAL, 0);


    // Loop forever so that the main task never exits
    while(1) {
        UEZTaskDelay(1000);
    }
}

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*      Configure the uEZGUI without an expanion board connected
*---------------------------------------------------------------------------*/
void uEZPlatformStartup_NO_EXP()
{
    TBool usbIsDevice = ETrue; //Default value

    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();
    UEZPlatform_SDCard_Drive_Require(1);

#if USB_PORT_B_HOST_DETECT_ENABLED
    usbIsDevice = UEZPlatform_Host_Port_B_Detect();
#endif
    if (usbIsDevice) {
        #if UEZ_ENABLE_USB_DEVICE_STACK
            #if (UEZ_PROCESSOR != NXP_LPC4357)
            UEZPlatform_USBDevice_Require();
            #else				
            #endif //(UEZ_PROCESSOR != NXP_LPC4357)
        #endif //UEZ_ENABLE_USB_DEVICE_STACK
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif //UEZ_ENABLE_USB_HOST_STACK
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }    
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformStartup
 *---------------------------------------------------------------------------*
 * Description:
 *      When uEZ starts, a special Startup task is created and called.
 *      This task brings up the all the hardware, reports any problems,
 *      starts the main task, and then exits.
 *---------------------------------------------------------------------------*/
TUInt32 uEZPlatformStartup(T_uezTask aMyTask, void *aParameters)
{
    extern T_uezTask G_mainTask;

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
    TUInt32 traceAddressInMemory = 0;
#endif

    UEZPlatform_Standard_Require();
    SUIInitialize(EFalse, EFalse, EFalse); // SWIM not flipped

    // Startup the desired platform configuration
#if UEZGUI_EXPANSION_DEVKIT
    uEZPlatformStartup_EXP_DK();
#else
    uEZPlatformStartup_NO_EXP();
#endif

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
    uiTraceStart();
    //vTraceStartStatusMonitor(); //Removed on new version of Trace
    traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
    printf("%x", traceAddressInMemory);
#endif


    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", 1024, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}

#if (UEZ_PROCESSOR != NXP_LPC4357)
#define EMWIN_BASE_ADDRESS  0xA0200000
#else
#define EMWIN_BASE_ADDRESS  0x28200000
#endif
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
          memset((void *)EMWIN_BASE_ADDRESS, 0x00, 0x00200000);
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
    return 0x00200000;
}

/*---------------------------------------------------------------------------*
 * Routine:  ConfigureTimerForRunTimeStats
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for freeRTOS that configures a timer for
 *      run time stats.
 *---------------------------------------------------------------------------*/
void ConfigureTimerForRunTimeStats(void){ /* no config needed */ 
}

/*---------------------------------------------------------------------------*
 * Routine:  GetTimerForRunTimeStats
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for freeRTOS that returns a timer for
 *      run time stats.
 *---------------------------------------------------------------------------*/
unsigned long GetTimerForRunTimeStats(void)
{
    return UEZTickCounterGet();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_VectorTableInit
 *---------------------------------------------------------------------------*
 * Description:
 *      This function hook is for any custom vector table initialization
 *---------------------------------------------------------------------------*/
void UEZBSP_VectorTableInit()
{
    /* No current implementation */
}

/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
