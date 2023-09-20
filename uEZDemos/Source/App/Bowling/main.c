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
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZDeviceTable.h>
#include <uEZDemoCommon.h>
#include <uEZToneGenerator.h>
#include <uEZAudioMixer.h>
#include <Config_Build.h>
#include "NVSettings.h"
//#include "AppTasks.h"
#//include "AppDemo.h"
#include "TestCmds.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include "Bowlers.h"
#include "DIALOG.h"
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include "emWin/WindowManager.h"
#include "Calibration.h"

#include <HAL/GPIO.h>

#define HEARTBEAT_BLINK_DELAY			 250

TBool G_usbIsDevice;

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
* Externals:
*---------------------------------------------------------------------------*/
extern T_uezTask G_mainTask;

#if (UEZ_PROCESSOR == NXP_LPC4357)
static TBool G_OnBoardUSBIsHost = EFalse;
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*/
/**
 *      Task to blink the heartbeat LED. Indicates the board is still alive.
 * @param [in] aMyTask -- not used
 * @param [in] aParams -- not used
 * @return     never returns
 */
 /*---------------------------------------------------------------------------*/
static TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
    UEZGPIOOutput(GPIO_HEARTBEAT_LED);
    UEZGPIOSetMux(GPIO_HEARTBEAT_LED, 0);

    // initial quick blink at bootup
    for (int i = 0; i < 20; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
    }

    // Blink
    while(1) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
    }
    return 0;
}

extern TBool BowlingIsActive();
extern void BowlingHasFinished();
#include <uEZRandom.h>

static TUInt32 FakeScore(T_uezTask aMyTask, void *aParams)
{
    TBool forever = ETrue;
    TUInt8 i;
    TUInt8 count = 0;
    T_Bowler bowlers[MAX_NUM_BOWLERS];
    T_uezRandomStream randomStream;

    memset((void*)bowlers, 0, sizeof(T_Bowler) * MAX_NUM_BOWLERS);

    UEZRandomStreamCreate(&randomStream, 0x565, UEZ_RANDOM_PSUEDO);

    while(forever){
       if ( BowlingIsActive()){
           for( i = 0; i < MAX_NUM_BOWLERS; i++){
               bowlers[i].iScore += (UEZRandomSigned32Bit(&randomStream)%11);
               SetBowlerScore(i, bowlers[i].iScore);
           }
           count++;
           if(count == 9){
               BowlingHasFinished();
               memset((void*)bowlers, 0, sizeof(T_Bowler) * MAX_NUM_BOWLERS);
               count = 0;
           }
       }
       UEZTaskDelay(5000);
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
 * Outputs:
 *      int                     -- Output error code
 *---------------------------------------------------------------------------*/
void MainTask(void)
{
#if COMPILE_OPTION_USB_SDCARD_DISK
    T_USBMSDriveCallbacks usbMSDiskCallbacks = {0};
#endif
#if RENAME_INI
    UEZFileRename("1:/INSTALL.FIN", "1:/INSTALL.INI");
#endif

    printf("\fuEZ v" UEZ_VERSION_STRING "\n"); // clear serial screen and put up banner
    printf(PROJECT_NAME " v" VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 128, (void *)0, UEZ_PRIORITY_NORMAL, 0);

    //UEZTaskCreate(FakeScore, "FakeScore", 256, (void *)0, UEZ_PRIORITY_NORMAL, 0);

#if UEZ_ENABLE_USB_DEVICE_STACK
    if (G_usbIsDevice) {
        // Setup the USB MassStorage device to connect to MS1 (the SD Card)
        USBMSDriveInitialize(
                &usbMSDiskCallbacks,
                0,
                "MS1");
    }
#endif

    //AudioStart(); // Used to call tonegenerator open
	
    PlayAudio(523, 100); // play audio will automatically unmute speaker
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);
    
    // Force calibration?
#if (USE_RESISTIVE_TOUCH == 1)
    Calibrate(CalibrateTestIfTouchscreenHeld());
#endif

    BowlersInit();

    if(!WindowManager_Start_emWin()){
        WindowManager_Create_All_Active_Windows();
        //start the main window.
        if(G_SystemWindows[MAIN_SCREEN]){
            WindowManager_Show_Window(MAIN_SCREEN);
            GUI_ExecCreatedDialog(G_SystemWindows[MAIN_SCREEN]);
        } else {
            UEZFailureMsg("Failed to create windows!");
        }
    } else {
        UEZFailureMsg("emWin Failed to Start!");
    }
    // We should not exit main unless we want to reset the board
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
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
void uEZPlatformStartup_NO_EXP()
{
#if UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
    static T_vcommCallbacks vcommCallbacks = {
            0, // SpeedChange
            0, // LineState
            0, // EmptyOutput
    };
#endif
    TBool usbIsDevice = ETrue; //Default value

    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();

#if USB_PORT_B_HOST_DETECT_ENABLED
    usbIsDevice = UEZPlatform_Host_Port_B_Detect();
#endif
    if (usbIsDevice) {
        #if UEZ_ENABLE_USB_DEVICE_STACK
            #if (UEZ_PROCESSOR != NXP_LPC4357)
            UEZPlatform_USBDevice_Require();
            #else
            #if UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
            VirtualCommInitialize(&vcommCallbacks, 0, 1); // USB0, force full speed for best integrity
            #endif //UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
            #endif //(UEZ_PROCESSOR != NXP_LPC4357)
        #endif //UEZ_ENABLE_USB_DEVICE_STACK
        #if UEZ_ENABLE_USB_HOST_STACK
          #if (USB_PORT_A_HOST_ENABLED == 1)
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
          #endif
        #endif //UEZ_ENABLE_USB_HOST_STACK
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
        #if(UEZ_PROCESSOR == NXP_LPC4357)
            G_OnBoardUSBIsHost = ETrue;
        #endif
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }

    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
        UEZPlatform_WiredNetwork0_Require();
    #endif

    #if UEZ_WIRELESS_PROGRAM_MODE
        UEZPlatform_WiFiProgramMode(EFalse);
    #endif

    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
        UEZPlatform_WirelessNetwork0_Require();
    #endif

    UEZPlatform_SDCard_Drive_Require(1);
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
    UEZPlatform_Timer0_Require();

    // Startup the desired platform configuration
    #if UEZGUI_EXPANSION_DEVKIT
        uEZPlatformStartup_EXP_DK();
    #elif UEZGUI_EXP_BRK_OUT
        uEZPlatformStartup_EXP_BRKOUT();
    #else
        uEZPlatformStartup_NO_EXP();
    #endif

    #if UEZ_ENABLE_BUTTON_BOARD
        UEZPlatform_ButtonBoard_Require();
	#endif

    SUIInitialize(SIMPLEUI_DOUBLE_SIZED_ICONS, EFalse, EFalse); // SWIM not flipped

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}

/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
