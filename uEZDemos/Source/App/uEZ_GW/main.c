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
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <uEZDemoCommon.h>
#include <uEZEEPROM.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "TestCmds.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include "UbiquiOS_Test.h"
#include "BluegigaTest.h"
#include "Configuration_EEPROM.h"
#include "Cell_Modem.h"

#if COMPILE_OPTION_USB_SDCARD_DISK
#if(UEZ_PROCESSOR != NXP_LPC4357)
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#else
#include <Source/Library/USBDevice/LPCUSBLib/MassStorage/USBMSDrive.h>
#endif
#endif

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
#include <trcUser.h>
#endif

#if UEZ_ENABLE_USB_DEVICE_STACK && UEZ_ENABLE_VIRTUAL_COM_PORT //LPC4357
#include <Source/Library/USBDevice/LPCUSBLib/VirtualComm/VirtualComm.h>
#endif
/*---------------------------------------------------------------------------*
 * Externals:
 *---------------------------------------------------------------------------*/
extern T_uezTask G_mainTask;

#if (UEZ_PROCESSOR == NXP_LPC4357)
TBool G_OnBoardUSBIsHost = EFalse;
#endif

#if APP_MENU_ALLOW_TEST_MODE
TBool volatile G_mmTestMode;
#endif

TUInt32 G_romChecksum;
TBool G_romChecksumCalculated;

/*---------------------------------------------------------------------------*
 * Routine:  ROMChecksumCalculate
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the checksum over the executable area excluding
 *      bootloader and vector table (which can change).
 * Outputs:
 *      TUInt32                   -- 32-bit additive checksum
 *---------------------------------------------------------------------------*/
TUInt32 ROMChecksumCalculate()
{
#if (UEZ_PROCESSOR != NXP_LPC4357)
    TUInt8 *p = (TUInt8 *)0x00000000;
#else
    TUInt8 *p = (TUInt8 *)0x1A000000;
#endif
    TUInt32 checksum = 0;
    TUInt32 count;

    for (count = 32; count < 0x7E000; count++) {
        checksum += *(p++);
    }

    return checksum;
}

/*---------------------------------------------------------------------------*
 * Routine:  CalcChecksumTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate a checksum in a separate task
 * Inputs:
 *      T_uezTask aMyTask         -- This task
 *      void *aParams             -- Passed in parameters
 * Outputs:
 *      TUInt32                   -- Returned code
 *---------------------------------------------------------------------------*/
void CalcChecksumTask(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
    G_romChecksum = ROMChecksumCalculate();
    G_romChecksumCalculated = ETrue;
}

/*---------------------------------------------------------------------------*
 * Task:  MainTask
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
    //TUInt8 i;

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

    //Read the MAC Address from the I2C EEPROM
    if(Configuration_EEPROM_Init() != UEZ_ERROR_NONE){
        printf("Configuration Not Valid!\n");
    }

#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
#if (UEZ_PROCESSOR != NXP_LPC4357)
    if (UEZDeviceTableIsRegistered("USBDevice"))
    USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1");
#else
    if(!G_OnBoardUSBIsHost) {
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1", 0, 1);
    }
#endif
#endif

    // Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    Ubiquios_Start();
    Cell_Modem_Start();
    //Bluegiga_Start();

    // Setup DAC audio if available - ignore error
    UEZDACWAVConfig("Timer2");

#if APP_MENU_ALLOW_TEST_MODE
    // initialize command console for test commands
    UEZGUITestCmdsInit();
#endif

    G_romChecksumCalculated = EFalse;
    UEZTaskCreate((T_uezTaskFunction)CalcChecksumTask, "Chksum",
            UEZ_TASK_STACK_BYTES(256), (void *)0, UEZ_PRIORITY_NORMAL, 0);

    // We should not exit main unless we want to reset the board
    return 0;
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
#endif

    UEZPlatform_Standard_Require();

    UEZPlatform_Timer0_Require();

#if UEZ_ENABLE_WIRED_NETWORK
    UEZPlatform_WiredNetwork0_Require();
#endif

#if UEZ_WIRELESS_PROGRAM_MODE
    UEZPlatform_WiFiProgramMode(EFalse);
#endif

#if UEZ_ENABLE_WIRELESS_NETWORK
    //UEZPlatform_WirelessNetwork0_Require();
#endif

#if UEZ_ENABLE_USB_HOST_STACK
    UEZPlatform_USBHost_PortB_Require();
#if(UEZ_PROCESSOR == NXP_LPC4357)
    G_OnBoardUSBIsHost = ETrue;
#endif
    UEZPlatform_USBFlash_Drive_Require(0);
#endif

    UEZPlatform_SDCard_Drive_Require(1);

    SUIInitialize(SIMPLEUI_DOUBLE_SIZED_ICONS, EFalse, EFalse); // SWIM not flipped

#if FREERTOS_PLUS_TRACE //LPC1788/4088 only as of uEZ v2.06
    uiTraceStart();
    //vTraceStartStatusMonitor(); //Removed on new version of Trace
    traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
    printf("%x", traceAddressInMemory);
#endif

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
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
 * Required by emWin: Need to find a better place for this
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

TUInt32 UEZEmWinGetRAMSize(void)
{
    return 0x00200000;
}

// Is this required?
void UEZBSP_VectorTableInit()
{ /* No Implementation */
}
/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
