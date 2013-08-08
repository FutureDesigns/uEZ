/*-------------------------------------------------------------------------*
 * File:  main.c
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
#include <uEZGPIO.h>
#include <uEZPlatform.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>

#include <Source/App/uEZDemoCommon/uEZDemoCommon.h>
#include "AppDemo.h"
#include "NVSettings.h"
#include "../uEZDemoCommon/NetworkStartup.h"

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
T_uezDevice G_network;
T_uezNetworkStatus G_networkStatus;

/*---------------------------------------------------------------------------*
 * Externals
 *---------------------------------------------------------------------------*/
extern void MainMenu(void);
extern void UEZGUITestCmdsInit();

/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
	UEZGPIOOutput(GPIO_P15);
	
    // Blink
    for (;;) {
        UEZGPIOSet(GPIO_P15);
        UEZTaskDelay(250);
        UEZGPIOClear(GPIO_P15);
        UEZTaskDelay(250);
    }

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
void SetupTasks(void)
{
    UEZTaskCreate(
          (T_uezTaskFunction)NetworkStartup,
          "NetStart",
          UEZ_TASK_STACK_BYTES(1024),
          (void *)0,
          UEZ_PRIORITY_NORMAL,
          0);

#if UEZ_ENABLE_COMMAND_CONSOLE
	// Start up the command parser task (ignore error)
   	UEZGUITestCmdsInit();
#endif
}

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
	printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner
	
	// Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
	
    NVSettingsInit();
    // Load the settings from non-volatile memory
    if (NVSettingsLoad() == UEZ_ERROR_CHECKSUM_BAD) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }
	
	// Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

	// initialize command console for test commands
    UEZGUITestCmdsInit();

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
        memset((void *)0x08080000, 0x00, 0x00200000);
        init = ETrue;
    }
    return 0x08080000;
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
    extern T_uezTask G_mainTask;
    
    UEZPlatform_Standard_Require();
    SUIInitialize((SIMPLEUI_DOUBLE_SIZED_ICONS)?ETrue:EFalse, EFalse, EFalse);

	#if UEZ_ENABLE_USB_DEVICE_STACK
    // USB Flash drive needed?
    UEZPlatform_USBFlash_Drive_Require(0);
	#endif

	#if UEZ_ENABLE_USB_HOST_STACK
	    // USB Device needed?
	    UEZPlatform_USBDevice_Require();
	#endif

    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(1);

	#if UEZ_ENABLE_TCPIP_STACK
	    // Network needed?
	    UEZPlatform_WiredNetwork0_Require();
	#endif

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}
