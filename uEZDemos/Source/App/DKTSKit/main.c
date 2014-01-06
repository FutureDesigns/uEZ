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
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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

#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <HAL/EMAC.h>
#include <uEZI2C.h>
#include <UEZPlatform.h>
#include <uEZDemoCommon.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "NVSettings.h"
#include "Audio.h"
#include "Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h"

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
#include <trcUser.h>
#endif

extern T_uezTask G_mainTask;

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Additional Includes:
 *---------------------------------------------------------------------------*/
#if COMPILE_OPTION_USB_SDCARD_DISK
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Task:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      In the uEZ system, main() is a task.  Do not exit this task
 *      unless you want to the board to reset.  This function should
 *      setup the system and then run the main loop of the program.
 * Outputs:
 *      int                     -- Output error code
 *---------------------------------------------------------------------------*/
int MainTask(void)
{
#if COMPILE_OPTION_USB_SDCARD_DISK
    T_USBMSDriveCallbacks usbMSDiskCallbacks = { 0 };
#endif

    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
    USBMSDriveInitialize(
        &usbMSDiskCallbacks,
        0,
        "MS1");
#endif

    // Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    AudioStart();

    // Pass control to the main menu
    MainMenu();

    // We should not exit main unless we want to reset the board
    return 0;
}

#include <string.h>
TUInt32 UEZEmWinGetRAMAddr(void)
{
    static TBool init = EFalse;
    if (!init) {
        memset((void *)0xA0200000, 0x00, 0x00200000);
        init = ETrue;
    }
    return 0xA0200000;
}

TUInt32 UEZEmWinGetRAMSize(void)
{
    return 0x00200000;
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
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
     TUInt32 traceAddressInMemory = 0;
	 vTraceInitTraceData();
#endif
    
    UEZPlatform_Standard_Require();
    #if (SIMPLEUI_DOUBLE_SIZED_ICONS)
      SUIInitialize(ETrue, EFalse, EFalse);
    #else
      SUIInitialize(EFalse, EFalse, EFalse);
    #endif

    // USB Flash drive needed?
    UEZPlatform_USBHost_PortA_Require();
    UEZPlatform_USBFlash_Drive_Require(0);

    // USB Device needed?
    UEZPlatform_USBDevice_Require();

    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(1);

    // Network needed?
#if UEZ_ENABLE_WIRED_NETWORK
     UEZPlatform_WiredNetwork0_Require();
#endif
     
#if UEZ_ENABLE_WIRELESS_NETWORK
#if UEZ_WIRELESS_PROGRAM_MODE
     UEZPlatform_WiFiProgramMode();
#endif
     UEZPlatform_WirelessNetwork0_Require();
#endif
     
#if ENABLE_UEZ_BUTTON
    UEZPlatform_Keypad_Require();
#endif
         
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
     uiTraceStart();
     
     traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
     printf("%x", traceAddressInMemory);
#endif
    
    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}


/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
