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
#include "NVSettings.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include "GUIManager_PF.h"
#include "Dialog.h"
#include "Calibration.h"

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
    HAL_GPIOPort **p_gpio;
    const TUInt32 pin = 13;
    TBool run = ETrue;

    HALInterfaceFind("GPIO1", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetOutputMode(p_gpio, 1<<pin);
    (*p_gpio)->SetMux(p_gpio, pin, 0); // set to GPIO
    // Blink
    while (run) {
        (*p_gpio)->Set(p_gpio, 1<<pin);
        UEZTaskDelay(250);
        (*p_gpio)->Clear(p_gpio, 1<<pin);
        UEZTaskDelay(250);
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

    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);

#if UEZ_ENABLE_USB_DEVICE_STACK
    if (G_usbIsDevice) {
        // Setup the USB MassStorage device to connect to MS1 (the SD Card)
        USBMSDriveInitialize(
                &usbMSDiskCallbacks,
                0,
                "MS1");
    }
#endif

    AudioStart();
    // Force calibration?
    Calibrate(CalibrateTestIfTouchscreenHeld());

    if(!GUIManager_Start_emWin_PF()){
        GUIManager_Create_All_Active_Windows_PF();
        //start the main window.
        if(G_SystemWindows_PF[HOME_SCREEN]){
            GUIManager_Show_Window_PF(HOME_SCREEN);
            GUI_ExecCreatedDialog(G_SystemWindows_PF[HOME_SCREEN]);
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
    extern T_uezTask G_mainTask;
    SUIInitialize(EFalse, EFalse, EFalse);
      
    UEZPlatform_Standard_Require();

    // USB Flash drive host mode needed?
#if USB_PORT_B_HOST_DETECT_ENABLED
    UEZGPIOLock(GPIO_P0_12);
    UEZGPIOSet(GPIO_P0_12); // disable MIC2025
    UEZGPIOOutput(GPIO_P0_12);
    UEZGPIOInput(GPIO_P0_28);
    UEZGPIOSetMux(GPIO_P0_28, 0);
    G_usbIsDevice = UEZGPIORead(GPIO_P0_28);
    if (G_usbIsDevice) {
        // High for a device
        UEZPlatform_USBDevice_Require();
    } else {
        // Low for a host
        UEZPlatform_USBFlash_Drive_Require(0);
        UEZGPIOClear(GPIO_P0_12);// enable MIC2025
    }
#else
    // USB Device needed?
    UEZPlatform_USBDevice_Require();
#endif

    //  UEZPlatform_USBFlash_Drive_Require(0);

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
