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
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <uEZDemoCommon.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "NVSettings.h"
#include "Audio.h"
#include "Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h"

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


  // initialize command console for test commands
  //UEZGUITestCmdsInit();

#include "UEZGainSpan.h"
    // Gainspan here:
    char rxBuf[128];
    void *G_UEZGUIWorkspace;
    G_UEZGUIWorkspace = 0;

   char* argv[2];
   argv[0]= "gainspan";
    argv[1] = "program";
//    argv[1] = "check";

UEZGUIGainSpan(G_UEZGUIWorkspace,
               2,
               (char **)argv,
               (char *)&rxBuf);

    // Pass control to the main menu
    MainMenu();

    // We should not exit main unless we want to reset the board
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Network_Connect
 *---------------------------------------------------------------------------*
 * Description:
 *      When a system needs the network to be running, this routine is
 *      called.  Most of the work is already done in the UEZPlatform.
 *      This routine calls the correct connect routine and gets the
 *      network information.
 *---------------------------------------------------------------------------*/
T_uezDevice G_network;
T_uezNetworkStatus G_networkStatus;
T_uezError UEZPlatform_Network_Connect(void)
{
    static TBool connected = EFalse;
    T_uezError error = UEZ_ERROR_NOT_AVAILABLE;

    if (!connected) {
        connected = ETrue;
        error = UEZPlatform_WiredNetwork0_Connect(&G_network, &G_networkStatus);
    }
    return error;
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

T_uezError UEZPlatform_WiredNetwork0_Connect(
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus)
{
    UEZPlatform_WiredNetwork0_Require();
    return UEZNetworkConnect(
            "WiredNetwork0",
            "lwIP",
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPAddr,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPMask,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPGateway,
            ETrue,
            aNetwork,
            aStatus);
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

    UEZPlatform_Standard_Require();
    #if (SIMPLEUI_DOUBLE_SIZED_ICONS)
      SUIInitialize(ETrue, EFalse, EFalse);
    #else
      SUIInitialize(EFalse, EFalse, EFalse);
    #endif

    // USB Flash drive needed?
    UEZPlatform_USBFlash_Drive_Require(0);

    // USB Device needed?
    UEZPlatform_USBDevice_Require();

    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(1);

    // Network needed?
    UEZPlatform_WiredNetwork0_Require();

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}


/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
