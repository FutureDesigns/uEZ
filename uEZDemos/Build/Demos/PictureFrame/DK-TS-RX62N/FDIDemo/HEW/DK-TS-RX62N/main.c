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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <HAL/EMAC.h>
#include <uEZI2C.h>
#include <UEZPlatform.h>
#include <UEZNetwork.h>
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
    const TUInt32 pin = 23;

    HALInterfaceFind("GPIO4", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetOutputMode(p_gpio, 1<<pin);
    (*p_gpio)->SetMux(p_gpio, pin, 0); // set to GPIO
    // Blink
    for (;;) {
        (*p_gpio)->Set(p_gpio, 1<<pin);
        UEZTaskDelay(250);
        (*p_gpio)->Clear(p_gpio, 1<<pin);
        UEZTaskDelay(250);
    }
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
    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);

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
    T_uezError error = UEZ_ERROR_NOT_AVAILABLE;

    static TBool connected = EFalse;
    if (!connected) {
        connected = ETrue;
        error = UEZNetworkConnect(
            "WiredNetwork0",
            "lwIP",
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPAddr,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPMask,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPGateway,
            ETrue,
            &G_network,
            &G_networkStatus);
    }

    return error;
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
    
    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(1);

    // Network needed?
    UEZPlatform_WiredNetwork0_Require();

    SUIInitialize(EFalse, EFalse, EFalse);

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}

/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
