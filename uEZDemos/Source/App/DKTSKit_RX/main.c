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
#include <UEZNetwork.h>
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <uEZDemoCommon/uEZDemoCommon.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "NVSettings.h"
#include "Audio.h"
#include "Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h"


#define TEST_EEPROM 0

#if TEST_EEPROM
#include <uEZSPI.h>
#include <UEZProcessor.h>
TUInt8 G_eepromData[256];
#endif

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
 * Externals
 *---------------------------------------------------------------------------*/
extern void UEZGUITestCmdsInit();


#if TEST_EEPROM
TUInt32 TestEEPROM()
{  
  T_uezDevice SPI;
  TUInt16 i;
  TUInt8 dataIN[3];
  TUInt8 dataOUT[3];
  SPI_Request SPIRequest;
  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {

      SPIRequest.iNumTransfers = 3;
      SPIRequest.iBitsPerTransfer = 8;
      SPIRequest.iRate = 1000; // 1 MHz
      SPIRequest.iClockOutPolarity = ETrue; // rising edge
      SPIRequest.iClockOutPhase = ETrue;
      HALInterfaceFind("GPIOC", (T_halWorkspace **)&SPIRequest.iCSGPIOPort);
      SPIRequest.iCSGPIOBit = (1UL<<4); // CS on PC4
      SPIRequest.iCSPolarity = EFalse; // Low True
      SPIRequest.iDataMISO = dataIN;
      SPIRequest.iDataMOSI = dataOUT;

	  dataOUT[0] = 3;
	  dataIN[2] = 0xCC;

	  for(i=0;i<256;i++) {
		  dataOUT[1]=i;
	      if (UEZSPITransferPolled(SPI, &SPIRequest) == UEZ_ERROR_NONE) {
	          G_eepromData[i] = dataIN[2];
	      }  	  
	  }

  } else {
      //an error occurred opening SPI
  }
  return 0;
}
#endif

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
        printf("Error Loading NVSettings: Re-Initializing\n");
        NVSettingsInit();
        NVSettingsSave();
    } else {
		printf("NVSettings Loaded Successfully\n");
	}
#if 0 // code to force the IP address to 192.168.50.7
        G_nonvolatileSettings.iIPAddr[0] = 192;
        G_nonvolatileSettings.iIPAddr[1] = 168;
        G_nonvolatileSettings.iIPAddr[2] = 50;
        G_nonvolatileSettings.iIPAddr[3] = 7;
		NVSettingsSave();        
#endif

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

#if TEST_EEPROM
	TestEEPROM();
#endif

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
        memset((void *)0x08080000, 0x00, 0x00080000);
        init = ETrue;
    }
    return 0x08080000;
}

TUInt32 UEZEmWinGetRAMSize(void)
{
    return 0x00080000;
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


/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
