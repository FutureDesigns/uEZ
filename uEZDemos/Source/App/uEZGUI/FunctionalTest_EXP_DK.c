/*-------------------------------------------------------------------------*
 * File:  FunctionalTest_EXP_DK.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for uEZGUI-EXP-DK
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
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZI2C.h>
#include <uEZTS.h>
#include <uEZDeviceTable.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>
#include <Types/ADC.h>
#include <Types/LCD.h>
#include <Device/ADCBank.h>
#include <uEZSPI.h>
#include <uEZFile.h>
#include <uEZRTOS.h>
#include <uEZFlash.h>
#include <uEZADC.h>
#include "FunctionalTest.h"
#include "FunctionalTest_EXP_DK.h"
#include "AppDemo.h"
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#if (RTOS == FreeRTOS)
#include "FreeRTOS.h"
#endif
#include "task.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Tests/ALS/Vishay/VCNL4010/TestVCNL4010.h>
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Include/Device/Network.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include <uEZLCD.h>
#include <uEZKeypad.h>
#include <HAL/GPIO.h>
#include <HAL/USBHost.h>
#include <Device/ADCBank.h>
#include <Device/Accelerometer.h>
#include <Device/ButtonBank.h>
#include <Device/EEPROM.h>
#include <Device/LEDBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <Types/TimeDate.h>
#include <uEZGPIO.h>

#include <uEZProcessor.h>
#include <uEZPlatform.h>

#define DO_FULL_NOR_FLASH_TEST    0
#define DO_EXTENSIVE_SDRAM_TEST   0

#if UEZ_ENABLE_LCD
void PlayWithBacklight(void);

/*---------------------------------------------------------------------------*
* Constants:
*---------------------------------------------------------------------------*/
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_8_BIT)
#define RGB(r, g, b) \
((((r>>5)&7)<<5)| \
     (((g>>5)&7)<<2)| \
          (((b>>6)&3)<<0))
#endif
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
#define RGB(r, g, b)      \
( (((r>>3)&0x1F)<<11)| \
     (((g>>2)&0x3F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#endif
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
#define RGB(r, g, b)      \
( (((r>>3)&0x1F)<<10)| \
     (((g>>3)&0x1F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#endif
#define ICON_TEXT_COLOR         YELLOW
#define SELECT_ICON_COLOR       YELLOW

/*---------------------------------------------------------------------------*
* Types:
*---------------------------------------------------------------------------*/
#if 0
#define OPT_BUTTON_SKIP             0x0001
#define OPT_BUTTON_YES              0x0002
#define OPT_BUTTON_NO               0x0004
#define OPT_BUTTON_CANCEL           0x0008
#define OPT_BUTTON_OK               0x0010
#define OPT_INIT                    0x8000

#define TEST_RESULT_IN_PROGRESS          0x0000
#define TEST_RESULT_DONE                 0x0001
#define TEST_RESULT_PASS                 0x0002
#define TEST_RESULT_FAIL                 0x0003
#define TEST_RESULT_SKIP                 0x0004
#define TEST_RESULT_IGNORE               0x0005

typedef struct {
     SWIM_WINDOW_T iWin;
     TUInt32 iTestNum;
     TBool iNeedInit;
     TUInt16 iButtons;
     TUInt16 iResult;
     TUInt8 iData[100];
     TBool iGoNext;
     TBool iExit;
     TUInt16 iLine;
     TBool iAllPass;
     TBool iPauseComplete;
} T_testData;

typedef struct {
     void (*iAddButtons)(T_testData *aData, TUInt16 aButtonTypes);
     void (*iRemoveButtons)(T_testData *aData, TUInt16 aButtonTypes);
     void (*iTextLine)(T_testData *aData, TUInt16 aLine, const char *aText);
     void (*iShowResult)(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay);
     void (*iSetTestResult)(T_testData *aData, TUInt16 aResult);
     void (*iNextTest)(T_testData *aData);
} T_testAPI;

typedef struct {
     const char *iTitle;
     void (*iMonitorFunction)(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
     TUInt16 iLinesNeeded;
     TBool iPauseOnFail;
     TBool iIgnoreIfExpansionBoard;
     TBool iIgnoreIfNoLoopbackBoard;
} T_testState;
#endif

/*---------------------------------------------------------------------------*
* Macros:
*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
* Prototypes:
*---------------------------------------------------------------------------*/
extern void Beep(void);
extern void BeepError(void);
extern void ButtonClick(void);
extern void PutPointRaw(TUInt8 aPage, int x, int y, TUInt16 pixel);
extern void DrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
extern void IHidePage0(void);
extern void IShowPage0(void);
extern void PlayAudio(TUInt32 aHz, TUInt32 aMS);
extern void FunctionalTestScreen_EXP_DK(T_testData *aData);
extern T_uezError USBHost_SwitchPort(void *aWorkspace, TUInt32 aPort);

/*---------------------------------------------------------------------------*
* Globals:
*---------------------------------------------------------------------------*/
const T_testState G_testStates_EXP_DK[] = {
     { "RS232", EXP_DK_FuncTestRS232, 1, EFalse, EFalse, EFalse }, // finished
     { "RS485", EXP_DK_FuncTestRS485, 1, EFalse, EFalse, EFalse }, // finished
     { "CAN", EXP_DK_FuncTestCAN, 1, ETrue, EFalse, EFalse }, // finished
     { "PMOD", EXP_DK_FuncTestPMOD, 1, EFalse, EFalse, EFalse }, // finished     
//   { "USB_Host_A", EXP_DK_FuncTestUSBHostA, 1, EFalse, EFalse, EFalse }, // manual test
//   { "USB_Host_B", EXP_DK_FuncTestUSBHostB, 1, EFalse, EFalse, EFalse }, // manual test
//   { "I2C", EXP_DK_FuncTestI2C_C_D, 1, ETrue, EFalse, EFalse }, // ? maybe use button board?
//   { "Ethernet", EXP_DK_FuncTestEthernet, 1, EFalse, EFalse, EFalse }, // manual test for now
     { "FET", EXP_DK_FuncTestFET, 1, ETrue, EFalse, EFalse }, // finished
     { "POT", EXP_DK_FuncTestPotentiometer, 1, ETrue, EFalse, EFalse }, // finished
     { "GPIO", EXP_DK_FuncTestButtonLED, 1, ETrue, EFalse, EFalse }, // finished
     { "ALS", EXP_DK_FuncTestALS, 6, ETrue, EFalse, EFalse }, // finished
//   { "DALI", EXP_DK_FuncTestDALI, 1, ETrue, EFalse, EFalse }, // Current not loaded so no test yet
     { "Test Complete", EXP_DK_FuncTestComplete, 1000, EFalse, EFalse },
     { 0 }     
//   { "SDcard", EXP_DK_FuncTestMicroSDCard4bit, 3, ETrue, EFalse, EFalse }, // video test
//   { "Audio", EXP_DK_FuncTestAudio, 1, EFalse, EFalse, EFalse }, // video test
//   { "Headphone", EXP_DK_FuncTestHeadphone, 1, EFalse, EFalse, EFalse }, //video test
//   { "Speaker", EXP_DK_FuncTestSpeaker, 1, EFalse, EFalse, EFalse }, // video test
};

T_testData G_td_EXP_DK;
static TUInt8 G_results_EXP_DK[50];

// TODO: change nothing?
void EXP_DK_FuncTestUSBHostA(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     /*typedef struct {
     TUInt32 iStart;
     T_uezFile iFile;
     TUInt32 iNumRetries;
     TUInt32 iRecoveryCount;
} T_usbhostTest;
     T_usbhostTest *p = (T_usbhostTest *)aData->iData;
     T_uezError error;
     HAL_USBHost **p_usbhost;
     
     if (aButton == OPT_INIT) {
     aAPI->iTextLine(aData, 0, "Checking USB Host (Port A)... ");
     aAPI->iTextLine(aData, 1, "  Insert flash drive with TESTUSB.TXT file.");
     aAPI->iTextLine(aData, 2, "  Looking ...");
     
     HALInterfaceFind("USBHost:PortA", (T_halWorkspace **)&p_usbhost);
     p->iNumRetries = 0;
     p->iRecoveryCount = 0;
     //        USBHost_SwitchPort(p_usbhost, 0);
     
     // Read the tick counter
     p->iStart = UEZTickCounterGet();
} else if (aButton == OPT_BUTTON_OK) {
     aAPI->iNextTest(aData);
} else if (aButton == 0) {
     // Try once a second
     if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
     p->iStart = UEZTickCounterGet();
     error = UEZFileOpen("0:TESTUSB.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
     if (error == UEZ_ERROR_NONE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Found!");
     UEZFileClose(p->iFile);
     aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
     sprintf(aData->iResultValue, "OK");
} else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
     sprintf(aData->iResultValue, "Not plugged in");
} else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
     sprintf(aData->iResultValue, "Internal Error");
} else if (error == UEZ_ERROR_NOT_READY) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
     sprintf(aData->iResultValue, "Not Ready");
} else if (error == UEZ_ERROR_NOT_FOUND) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
     sprintf(aData->iResultValue, "Not Found");
} else if (error == UEZ_ERROR_NOT_AVAILABLE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
     sprintf(aData->iResultValue, "Not available");
} else {
     aAPI->iTextLine(aData, 2, "  Looking ...");
     sprintf(aData->iResultValue, "No TESTUSB.TXT");
}
     if (error != UEZ_ERROR_NONE) {
     p->iRecoveryCount++;
     if (p->iRecoveryCount == 5) {
     p->iRecoveryCount = 0;
     p->iNumRetries++;
     if (p->iNumRetries == 6) {
     aAPI->iTextLine(aData, 2, "  Looking ... Timeout!");
     aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
     sprintf(aData->iResultValue, "Timeout");
} else {
     // Try to reset again
     HALInterfaceFind("USBHost:PortA", (T_halWorkspace **)&p_usbhost);
     //                        USBHost_SwitchPort(p_usbhost, 0);
}
}
}
}
}
     */
}

// TODO: change to host detect on EXP_DK R2
void EXP_DK_FuncTestUSBHostB(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     /*    typedef struct {
     TUInt32 iStart;
     T_uezFile iFile;
     TUInt32 iNumRetries;
     TUInt32 iRecoveryCount;
} T_usbhostTest;
     T_usbhostTest *p = (T_usbhostTest *)aData->iData;
     T_uezError error;
     HAL_USBHost **p_usbhost;
     
     if (aButton == OPT_INIT) {
     aAPI->iTextLine(aData, 0, "Checking USB Host (Port B)... ");
     aAPI->iTextLine(aData, 1, "  Insert flash drive with TESTUSB.TXT file.");
     aAPI->iTextLine(aData, 2, "  Looking ...");
     
     HALInterfaceFind("USBHost:PortB", (T_halWorkspace **)&p_usbhost);
     p->iNumRetries = 0;
     p->iRecoveryCount = 0;
     //        USBHost_SwitchPort(p_usbhost, 1);
     
     // Read the tick counter
     p->iStart = UEZTickCounterGet();
} else if (aButton == OPT_BUTTON_OK) {
     aAPI->iNextTest(aData);
} else if (aButton == 0) {
     // Try once a second
     if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
     p->iStart = UEZTickCounterGet();
     error = UEZFileOpen("0:TESTUSB.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
     if (error == UEZ_ERROR_NONE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Found!");
     UEZFileClose(p->iFile);
     aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
     sprintf(aData->iResultValue, "OK");
} else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
     sprintf(aData->iResultValue, "Not plugged in");
} else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
     sprintf(aData->iResultValue, "Internal Error");
} else if (error == UEZ_ERROR_NOT_READY) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
     sprintf(aData->iResultValue, "Not Ready");
} else if (error == UEZ_ERROR_NOT_FOUND) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
     sprintf(aData->iResultValue, "Not Found");
} else if (error == UEZ_ERROR_NOT_AVAILABLE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
     sprintf(aData->iResultValue, "Not available");
} else {
     aAPI->iTextLine(aData, 2, "  Looking ...");
     sprintf(aData->iResultValue, "No TESTUSB.TXT");
}
     if (error != UEZ_ERROR_NONE) {
     p->iRecoveryCount++;
     if (p->iRecoveryCount == 5) {
     p->iRecoveryCount = 0;
     p->iNumRetries++;
     if (p->iNumRetries == 6) {
     aAPI->iTextLine(aData, 2, "  Looking ... Timeout!");
     aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
     sprintf(aData->iResultValue, "Timeout");
} else {
     // Try to reset again
     HALInterfaceFind("USBHost:PortB", (T_halWorkspace **)&p_usbhost);
     //                        USBHost_SwitchPort(p_usbhost, 1);
}
}
}
}
}     */
}

// TODO: change for 4 bit using MCI in platform
void EXP_DK_FuncTestMicroSDCard4bit(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     /*   typedef struct {
     TUInt32 iStart;
     T_uezFile iFile;
     TUInt32 iNumRetries;
} T_usbhostTest;
     T_usbhostTest *p = (T_usbhostTest *)aData->iData;
     T_uezError error;
     
     if (aButton == OPT_INIT) {
     aAPI->iTextLine(aData, 0, "Checking Micro SDCard ... ");
     aAPI->iTextLine(aData, 1, "  Insert Micro SDCard with SLIDES/SLIDES.TXT file.");
     aAPI->iTextLine(aData, 2, "  Looking ...");
     
     // Read the tick counter
     p->iStart = UEZTickCounterGet();
     p->iNumRetries = 0;
} else if (aButton == OPT_BUTTON_OK) {
     aAPI->iNextTest(aData);
} else if (aButton == 0) {
     // Try once a second
     if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
     p->iStart = UEZTickCounterGet();
     error = UEZFileOpen("1:SLIDES/SLIDES.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
     if (error == UEZ_ERROR_NONE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Found!");
     UEZFileClose(p->iFile);
     aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
     sprintf(aData->iResultValue, "OK");
} else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
     sprintf(aData->iResultValue, "Not plugged in");
} else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
     aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
     sprintf(aData->iResultValue, "Internal Error");
} else if (error == UEZ_ERROR_NOT_READY) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
     sprintf(aData->iResultValue, "Not Ready");
} else if (error == UEZ_ERROR_NOT_FOUND) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
     sprintf(aData->iResultValue, "Not Found");
} else if (error == UEZ_ERROR_NOT_AVAILABLE) {
     aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
     sprintf(aData->iResultValue, "Not available");
} else if (error == UEZ_ERROR_READ_WRITE_ERROR) {
     aAPI->iTextLine(aData, 2, "  Looking ... Error!");
     sprintf(aData->iResultValue, "Read/Write Error");
     aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
} else {
     aAPI->iTextLine(aData, 2, "  Looking ... Error!");
     sprintf(aData->iResultValue, "No SLIDES.TXT");
     aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
}
     if (error != UEZ_ERROR_NONE) {
     p->iNumRetries++;
     if (p->iNumRetries >= 15) {
     aAPI->iTextLine(aData, 2, "  Looking ... Timeout!");
     aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
     aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
     sprintf(aData->iResultValue, "Timeout");
}
}
}
}*/
}

// TODO: change to ping other unit? 
void EXP_DK_FuncTestEthernet(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{

}


// TODO: change to turn on 
void EXP_DK_FuncTestI2C_C_D(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     
}

// TODO: change to turn on 
void EXP_DK_FuncTestAudio(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{

}

// TODO: change to turn on 
void EXP_DK_FuncTestHeadphone(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{

}

// TODO: change to turn on 
void EXP_DK_FuncTestDALI(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{

}

// TODO: change to turn on speaker only on EXP-DK R2!
void EXP_DK_FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     
}

// finished
void EXP_DK_FuncTestPMOD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{    
	 int i; 							 
     ATLIBGS_NetworkStatus status;
     char mac[9] = {'0','0',':','1','d',':','c','9','\0'}; // 00:1D:C9 is MAC address registed to GainSpan
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking PMOD for GainSpan WAB-GW...");
          aAPI->iTextLine(aData, 1, "There is a 20 second timeout period on this test.");
          aAPI->iTextLine(aData, 2, "If the test hangs, assume failure.");
     } else if (aButton == 0) { // check MAC address of module
          UEZGUI_EXP_DK_GainSpan_PMOD_Require();
          AtLibGs_GetNetworkStatus(&status); // Get the status of the WAB-GW
          // mac address returned to make sure that it isn't zero or bogus number.
          for(i = 0 ; i < 8; i++){
               if(status.mac[i] == mac[i]){// correct number and not garbage
               } else { // failure
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    return;
               }
          }
          // Pass if make it through the entire for loop
          aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
          aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
     }
}

// finished
void EXP_DK_FuncTestALS(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          TUInt8 iSubstep;
          char iLine[80];
          TUInt32 ambient;
          TUInt32 proximity;
     } T_ALSTest;
     T_ALSTest *p = (T_ALSTest *)aData->iData;
     if (aButton == OPT_INIT) {
          p->iSubstep = 0;
          aAPI->iTextLine(aData, 0, "Checking Light Sensor ... ");
          aAPI->iTextLine(aData, 1, "  Make sure there is light shining on the sensor.");
     } else if (aButton == 0) {
          if (TestVCNL4010FCT("EXPDK_I2C-B", 0x13, &p->ambient, &p->proximity) == UEZ_ERROR_NONE){
               // can print ambient and proximity on screen now
               if (p->iSubstep == 0) {
                    aAPI->iTextLine(aData, 2, "  Move your finger close to the sensor.");
                    sprintf(p->iLine, "    Lux (%d), Proximity (%d)", p->ambient, p->proximity);
                    aAPI->iTextLine(aData, 3, p->iLine);
                    if ((p->ambient < 150) && (p->proximity > 50000)) { // Looking for object closing in
                         aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                         p->iSubstep = 1;
                    }
               } else {
                    aAPI->iTextLine(aData, 4, "  Move your finger away from to the sensor. ");
                    sprintf(p->iLine, "    Lux (%d), Proximity (%d)",p->ambient,p->proximity);
                    aAPI->iTextLine(aData, 5, p->iLine);
                    if ((p->ambient > 1200) && (p->proximity < 4000)) {       // Looking for object moving away
                         aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                         aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                         aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                         sprintf(aData->iResultValue, "OK");
                    }
               }
               UEZTaskDelay(20); // give it a chance to appear on the screen
          } else {
               aAPI->iTextLine(aData, 0, "Checking Light Sensor ... Not found!");
               sprintf(aData->iResultValue, "Not found");
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
     }
}

// finished
void EXP_DK_FuncTestRS485(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          T_uezDevice iDevice;
          DEVICE_STREAM **iStream;
          TUInt8 iReceived[5];
          TUInt32 iCount;
          TUInt32 iStart;
     } T_serialTest;
     T_serialTest *p = (T_serialTest *)aData->iData;
     T_uezError error;
     TUInt32 num;
     memset(p->iReceived, 0, 5);
     
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking RS485 Port ...");
          
          error = UEZDeviceTableFind("EXP_RS485", &p->iDevice);
          if (!error)
               error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iStream);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          p->iCount = 0;
          
          // Flush the input buffer
          while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {
               
          }
          
          // Send out a 1 character string to the serial
          error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          
          // Flush the input buffer
          while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {
               
          }
          
          // Do again, send out a 1 character string to the serial
          error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          
          p->iStart = UEZTickCounterGet();
     } else if (aButton == 0) {
          // Have we timed out?
          if (UEZTickCounterGetDelta(p->iStart) > 2000) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          num = 4 - p->iCount;
          (*p->iStream)->Read(p->iStream, p->iReceived + p->iCount, num, &num, 500);
          p->iCount += num;
          if (p->iCount >= 4) {
               // Should show the beginning of "J.Harris"
               if (memcmp(p->iReceived, "J.Ha", 4) == 0) {
                    // Correct response
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                    return;
               } else {
                    // Incorrect response
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    return;
               }
          }
     }
}

// finished
void EXP_DK_FuncTestRS232(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          T_uezDevice iDevice;
          DEVICE_STREAM **iStream;
          TUInt8 iReceived[5];
          TUInt32 iCount;
          TUInt32 iStart;
     } T_serialTest;
     T_serialTest *p = (T_serialTest *)aData->iData;
     T_uezError error;
     TUInt32 num;
     memset(p->iReceived, 0, 5);
     
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking RS232 Port ...");
          
          error = UEZDeviceTableFind("EXPDK_RS232", &p->iDevice);
          if (!error)
               error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iStream);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          p->iCount = 0;
          
          // Flush the input buffer
          while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {
               
          }
          
          // Send out a 1 character string to the serial
          error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          
          // Flush the input buffer
          while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {
               
          }
          
          // Do again, send out a 1 character string to the serial
          error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
          if (error) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          
          p->iStart = UEZTickCounterGet();
     } else if (aButton == 0) {
          // Have we timed out?
          if (UEZTickCounterGetDelta(p->iStart) > 2000) {
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
          num = 4 - p->iCount;
          (*p->iStream)->Read(p->iStream, p->iReceived + p->iCount, num, &num, 500);
          p->iCount += num;
          if (p->iCount >= 4) {
               // Should show the beginning of "J.Harris"
               if (memcmp(p->iReceived, "J.Ha", 4) == 0) {
                    // Correct response
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                    return;
               } else {
                    // Incorrect response
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    return;
               }
          }
     }
}

// finished
void EXP_DK_FuncTestCAN(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          T_uezDevice iDevice;
          DEVICE_STREAM **iStream;
          TUInt32 iCount;
          TUInt32 iStart;
     } T_serialTest;
     T_serialTest *p = (T_serialTest *)aData->iData;
     unsigned int a, b;
     
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking CAN Port ...");
          
          // Send a CAN message
          CANSend8(0x11223344, 0x55667788);
          
          // Setup a timeout period
          p->iStart = UEZTickCounterGet();
          p->iCount = 0;
     } else if (aButton == 0) {
          // Did we get a packet back?
          a = b = 0;
          if (CANReceive8(&a, &b)) {
               // Got something, is it right?
               if ((a==(0x11223344+0x01010101)) && (b==(0x55667788+0x01010101))) {
                    // Matches!
                    // We're done
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
               }
          }
          
          // Have we timed out?
          if (UEZTickCounterGetDelta(p->iStart) > 2000) {
               p->iCount++;
               // Too many tries?
               if (p->iCount == 3) {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    return;
               }
               // Send again
               CANSend8(0x11223344, 0x55667788);
               // Reset timeout
               p->iStart = UEZTickCounterGet();
          }
     }
}

// Finished
void EXP_DK_FuncTestButtonLED(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          TUInt8 oldv;
          TUInt8 newv;
          TUInt8 bits;
          DEVICE_LEDBank **p_leds;
          T_uezDevice leds;
          DEVICE_ButtonBank **p_buttons;
          T_uezDevice buttons;
          TUInt8 pushed;
          TUInt8 iSubstep;
     } T_ledsButtonsTest;
     T_ledsButtonsTest *p = (T_ledsButtonsTest *)aData->iData;
     T_uezError error;
     
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking LEDs and Buttons ... ");
          error = UEZDeviceTableFind("LEDBank0", &p->leds);
          if (!error)
               error = UEZDeviceTableGetWorkspace(p->leds, (T_uezDeviceWorkspace **)&p->p_leds);
          if (!error)
               error = UEZDeviceTableFind("ButtonBank0", &p->buttons);
          if (!error)
               error = UEZDeviceTableGetWorkspace(p->buttons, (T_uezDeviceWorkspace **)&p->p_buttons);
          if (error) {
               aAPI->iTextLine(aData, 0, "LEDs and Buttons ... Not Found!");
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
          }
          
          aAPI->iTextLine(aData, 1, "  Push all 4 buttons and turn on the LEDs");
          (*p->p_buttons)->SetActiveButtons(p->p_buttons, 0x0F);
          
          p->oldv = 0xFF;
          p->newv = 0xFF;
          p->pushed = 0;
          p->bits = 0;
          p->iSubstep = 0;
     } else if (aButton == 0) {
          TUInt32 butbits;
          TUInt8 v;
          
          (*p->p_leds)->On(p->p_leds, (p->bits & 0x0F)<<4);
          (*p->p_leds)->Off(p->p_leds, ((~p->bits) & 0x0F)<<4);
          (*p->p_buttons)->Read(p->p_buttons, &butbits);
          p->newv = butbits;
          v = ~p->newv & p->oldv;
          p->bits ^= v;
          p->oldv = p->newv;
          if (p->iSubstep == 0) {
               if ((p->bits & 0x0F) == 0x0F) {
                    p->iSubstep = 1;
                    aAPI->iTextLine(aData, 1, "  Are all 4 leds on?");
                    aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
               }
          }
     } else if (aButton == OPT_BUTTON_YES) {
          aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
          aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
     } else if (aButton == OPT_BUTTON_NO) {
          aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
          aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
     }
}

// Finished
void EXP_DK_FuncTestFET(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          char iLine[80];
     } T_FETTest;
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Checking  FET... ");
          aAPI->iTextLine(aData, 1, "  Did the connected device and LED turn on, off, then back on?");
          aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
          // test FET driver
          UEZGPIOSet(GPIO_P1_5);          //turn on unit
          UEZGPIOOutput(GPIO_P1_5);
          UEZTaskDelay(2000);             // on for 2 seconds
          UEZGPIOClear(GPIO_P1_5);        // turn on unit
          UEZTaskDelay(2000);             // off for 2 seconds
          UEZGPIOSet(GPIO_P1_5);          // leave unit on
     } else if (aButton == 0) {
     } else if (aButton == OPT_BUTTON_YES) {
          aAPI->iTextLine(aData, 1, "  Pass");
          aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
          aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
          sprintf(aData->iResultValue, "OK");
     } else if (aButton == OPT_BUTTON_NO) {
          aAPI->iTextLine(aData, 1, "  FAIL");
          aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
          aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
          sprintf(aData->iResultValue, "Not found");
     }
}

// Finished
void EXP_DK_FuncTestPotentiometer(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     typedef struct {
          T_uezDevice iDevice;
          DEVICE_ADCBank **iADC;
          TUInt8 iSubstep;
          ADC_RequestSingle iRequest;
          char iLine[80];
     } T_accelTest;
     T_accelTest *p = (T_accelTest *)aData->iData;
     T_uezError error;
     TUInt32 ADCReading; 
     TUInt32 percent;
     if (aButton == OPT_INIT) {
          p->iSubstep = 0;
          aAPI->iTextLine(aData, 0, "Checking ADC/Potentiometer... ");
          error = UEZDeviceTableFind("ADC0", &p->iDevice);
          if (!error)
               error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iADC);
          if (error) {
               aAPI->iTextLine(aData, 0, "Checking ADC ... Not found!");
               sprintf(aData->iResultValue, "Not found");
               aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
               aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
               return;
          }
     } else if (aButton == 0) {
          (p->iRequest).iADCChannel = 2;
          (p->iRequest).iBitSampleSize = 10;
          (p->iRequest).iTrigger = ADC_TRIGGER_NOW;
          (p->iRequest).iCapturedData = &ADCReading;
          (*p->iADC)->RequestSingle(p->iADC, &p->iRequest);
          percent = ADCReading * 100 / 1024; // got ADCReading, convert to percent
          if (p->iSubstep == 0) {
               sprintf(p->iLine, "  Move the Potentiometer to the left. (%d)", percent);
               aAPI->iTextLine(aData, 1, p->iLine);
               if (percent < 3) {         // Looking for left scroll
                    aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                    p->iSubstep = 1;
               }
          } else {
               sprintf(p->iLine, "  Move the Potentiometer to the right. (%d)", percent);
               aAPI->iTextLine(aData, 2, p->iLine);
               if (percent > 97) {       // Looking for right scroll
                    aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                    sprintf(aData->iResultValue, "OK");
               }
          }
          UEZTaskDelay(20); // give it a chance to appear on the screen
     }
}

void EXP_DK_FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
     TUInt32 i=3;
     const T_testState *p;
     char *line = (char *)G_td_EXP_DK.iData;
     TUInt32 c=0;
     
     if (aButton == OPT_INIT) {
          aAPI->iTextLine(aData, 0, "Functional Test is Complete");
          if (aData->iAllPass)
               aAPI->iTextLine(aData, 1, "  Final Result:  ^G** PASSES **^Y");
          else
               aAPI->iTextLine(aData, 1, "  Final Result:  ^R** FAILS **^Y");
          
          // Remove all buttons
          aAPI->iRemoveButtons(aData, 0xFFFF);
          // Put back on only the OK button
          aAPI->iAddButtons(aData, OPT_BUTTON_OK);
          
          if (!aData->iAllPass) {
               // Output failed
               aAPI->iTextLine(aData, i++, "Failed: ");
               strcpy(line, "    ");
               c=0;
               for (p=G_testStates_EXP_DK; p->iMonitorFunction != EXP_DK_FuncTestComplete; p++) {
                    if (G_results_EXP_DK[p-G_testStates_EXP_DK] == TEST_RESULT_FAIL) {
                         if (c)
                              strcat(line, ", ");
                         if (c == 4) {
                              aAPI->iTextLine(aData, i++, line);
                              strcpy(line, "    ");
                              c = 0;
                         }
                         strcat(line, p->iTitle);
                         c++;
                    }
               }
               if (c) {
                    aAPI->iTextLine(aData, i++, line);
                    strcpy(line, "");
                    c = 0;
               }
               
               // Output skipped
               aAPI->iTextLine(aData, i++, "Skipped: ");
               strcpy(line, "    ");
               c=0;
               for (p=G_testStates_EXP_DK; p->iMonitorFunction != EXP_DK_FuncTestComplete; p++) {
                    if (G_results_EXP_DK[p-G_testStates_EXP_DK] == TEST_RESULT_SKIP) {
                         if (c)
                              strcat(line, ", ");
                         if (c == 4) {
                              aAPI->iTextLine(aData, i++, line);
                              strcpy(line, "    ");
                              c = 0;
                         }
                         strcat(line, p->iTitle);
                         c++;
                    }
               }
               if (c) {
                    aAPI->iTextLine(aData, i++, line);
                    strcpy(line, "");
                    c = 0;
               }
          }
     } else if (aButton == OPT_BUTTON_OK) {
          aAPI->iNextTest(aData);
     }
}

static const struct { char *iText; T_pixelColor iColor; } G_textResults_EXP_DK[] = {
    { "In Progress",    YELLOW, },
    { "Done",           GREEN,  },
    { "Pass",           GREEN,  },
    { "Fail",           RED,    },
    { "Skip",           YELLOW, },
    { "Ignored",        YELLOW, },
};

#if UEZ_DEFAULT_LCD_RES_QVGA
#define TEST_RESULT_LEFT_COLUMN         10
#define TEST_RESULT_RIGHT_COLUMN        270
#define TEST_RESULT_RIGHT_EDGE          310
#define TEST_RESULT_LINE_HEIGHT         15
#define TEST_RESULT_LINE_START          10
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
#define TEST_RESULT_LEFT_COLUMN         20
#define TEST_RESULT_RIGHT_COLUMN        540
#define TEST_RESULT_RIGHT_EDGE          620
#define TEST_RESULT_LINE_HEIGHT         30
#define TEST_RESULT_LINE_START          20
#endif
#if UEZ_DEFAULT_LCD_RES_WVGA
#define TEST_RESULT_LEFT_COLUMN         20
#define TEST_RESULT_RIGHT_COLUMN        540
#define TEST_RESULT_RIGHT_EDGE          780
#define TEST_RESULT_LINE_HEIGHT         30
#define TEST_RESULT_LINE_START          20
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
#define TEST_RESULT_LEFT_COLUMN         10
#define TEST_RESULT_RIGHT_COLUMN        270
#define TEST_RESULT_RIGHT_EDGE          310
#define TEST_RESULT_LINE_HEIGHT         15
#define TEST_RESULT_LINE_START          10
#endif

#define TEST_COLOR_TEXT                 YELLOW
#define TEST_COLOR_BACKGROUND           BLACK
#define TEST_COLOR_BUTTON_EDGE          YELLOW
#define TEST_COLOR_BUTTON_CLICK         GREEN
#define TEST_RESULT_NUM_LINES           10

typedef struct {
     TUInt16 iBit;
     TUInt16 iX1, iY1, iX2, iY2;
     const char *iText;
} T_button;

#if UEZ_DEFAULT_LCD_RES_QVGA
#define BUTTONS_LEFT_EDGE           10
#define BUTTONS_RIGHT_EDGE          305
#define BUTTONS_TOP                 170
#define BUTTONS_BOTTOM              210
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             10
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
#define BUTTONS_LEFT_EDGE           20
#define BUTTONS_RIGHT_EDGE          610
#define BUTTONS_TOP                 340
#define BUTTONS_BOTTOM              420
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             20
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
#define BUTTONS_LEFT_EDGE           20
#define BUTTONS_RIGHT_EDGE          460
#define BUTTONS_TOP                 160
#define BUTTONS_BOTTOM              240
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             20
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
#define BUTTONS_LEFT_EDGE           20
#define BUTTONS_RIGHT_EDGE          770
#define BUTTONS_TOP                 340
#define BUTTONS_BOTTOM              420
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             20
#endif

#define BUTTON_WIDTH                ((BUTTONS_RIGHT_EDGE-BUTTONS_LEFT_EDGE-((BUTTONS_NUM-1)*BUTTONS_PADDING))/BUTTONS_NUM)
#define BUTTON_X_SPACING            (BUTTON_WIDTH+BUTTONS_PADDING)

#define BUTTON_LEFT(n)              ((BUTTON_X_SPACING*(n))+BUTTONS_LEFT_EDGE)
#define BUTTON_TOP(n)               (BUTTONS_TOP)
#define BUTTON_RIGHT(n)             (BUTTON_LEFT(n) + BUTTON_WIDTH - 1)
#define BUTTON_BOTTOM(n)            (BUTTONS_BOTTOM)

#define BUTTON_COORDS(n)            BUTTON_LEFT(n), BUTTON_TOP(n), BUTTON_RIGHT(n), BUTTON_BOTTOM(n)

static const T_button G_testButtons_EXP_DK[] = {
     { OPT_BUTTON_CANCEL,    BUTTON_COORDS(0), "Cancel" },
     { OPT_BUTTON_YES,       BUTTON_COORDS(1), "Yes" },
     { OPT_BUTTON_NO,        BUTTON_COORDS(2), "No" },
     { OPT_BUTTON_SKIP,      BUTTON_COORDS(3), "Skip" },
     { OPT_BUTTON_OK,        BUTTON_COORDS(4), "OK" },
     { 0, /* ends here */ }
};

static TUInt16 ICalcY(TUInt16 aLine)
{
    return aLine * TEST_RESULT_LINE_HEIGHT + TEST_RESULT_LINE_START;
}

void TestDrawButtons_EXP_DK(
        T_testData *aData,
        TUInt16 aButtons,
        T_pixelColor aBackground)
{
    const T_button *p_button;
    TUInt16 width, height;
    TUInt16 left, top, middle;
    const char *p;

    // Set the button colors
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BUTTON_EDGE);
    swim_set_fill_color(&aData->iWin, aBackground);
    swim_set_bkg_color(&aData->iWin, aBackground);

    // Draw each of the buttons that have appeared
    // Go through and remove buttons
    for (p_button=G_testButtons_EXP_DK; p_button->iBit; p_button++)  {
        if (aButtons & p_button->iBit) {
            // Draw box button
            swim_set_fill_transparent(&G_td_EXP_DK.iWin, 0);
            swim_put_box(
                &aData->iWin,
                p_button->iX1,
                p_button->iY1,
                p_button->iX2,
                p_button->iY2);

            // Now draw the text in it
            swim_set_fill_transparent(&aData->iWin, 1);

            // Calculate width of text
            width = 0;
            for (p=p_button->iText; *p; p++)
                width += swim_get_font_char_width(&aData->iWin, *p);

            // Cut width in half
            width >>= 1;

            // Now draw centered below.
            swim_set_pen_color(&aData->iWin, ICON_TEXT_COLOR);
            middle = (p_button->iX1 + p_button->iX2-1)>>1;
            left = middle-width;
            height = swim_get_font_height(&aData->iWin);
            top = ((p_button->iY1 + p_button->iY2-1)-height)>>1;
            swim_put_text_xy(&aData->iWin, p_button->iText, left, top);
        }
    }
}

void TestButtonClick_EXP_DK(T_testData *aData, TUInt16 aButton)
{
    TestDrawButtons_EXP_DK(aData, aButton, TEST_COLOR_BUTTON_CLICK);
    ButtonClick();
    UEZTaskDelay(100);
    TestDrawButtons_EXP_DK(aData, aButton, TEST_COLOR_BACKGROUND);
}

void TestAddButtons_EXP_DK(T_testData *aData, TUInt16 aButtonTypes)
{
    TUInt16 newButtons = aButtonTypes ^ aData->iButtons;

    TestDrawButtons_EXP_DK(aData, newButtons, TEST_COLOR_BACKGROUND);

    aData->iButtons |= aButtonTypes;
}

void TestRemoveButtons_EXP_DK(T_testData *aData, TUInt16 aButtonTypes)
{
    const T_button *p_button;
    TUInt16 buttons = aData->iButtons & aButtonTypes;

    // Erase the buttons
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td_EXP_DK.iWin, 0);

    // Go through and remove buttons
    for (p_button=G_testButtons_EXP_DK; p_button->iBit; p_button++)  {
        if (buttons & p_button->iBit) {
            swim_put_box(
                &aData->iWin,
                p_button->iX1,
                p_button->iY1,
                p_button->iX2,
                p_button->iY2);
        }
    }

    // Remove those buttons from the logic
    aData->iButtons &= ~aButtonTypes;
}



void TestTextLine_EXP_DK(T_testData *aData, TUInt16 aLine, const char *aText)
{
    int mode = 0;
    const char *p_text;
    TUInt16 y = ICalcY(aLine+aData->iLine);

    // Draw a black rectangle over the area
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td_EXP_DK.iWin, 0);

    // Draw the black box for the background
    swim_put_box(
        &aData->iWin,
        TEST_RESULT_LEFT_COLUMN,
        y,
        TEST_RESULT_RIGHT_EDGE-1,
        y+TEST_RESULT_LINE_HEIGHT-1);

    if (aText) {
        // Now draw the text that goes here
        swim_set_pen_color(&aData->iWin, TEST_COLOR_TEXT);
#if 0
        swim_put_text_xy(
            &aData->iWin,
            aText,
            TEST_RESULT_LEFT_COLUMN,
            y);
#else
        swim_set_xy(&aData->iWin, TEST_RESULT_LEFT_COLUMN, y);
        p_text = aText;
        while (*p_text) {
            if (mode == 0) {
                if (*p_text == '^')
                    mode = 1;
                else
                    swim_put_char(&aData->iWin, *p_text);
            } else {
                if (*p_text == 'G')
                    swim_set_pen_color(&aData->iWin, GREEN);
                else if (*p_text == 'R')
                    swim_set_pen_color(&aData->iWin, RED);
                else if (*p_text == 'Y')
                    swim_set_pen_color(&aData->iWin, YELLOW);
                else
                    swim_set_pen_color(&aData->iWin, TEST_COLOR_TEXT);
                mode = 0;
            }
            p_text++;
        }
#endif
    }
}

void TestShowResult_EXP_DK(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay)
{
    // Draw the result on the screen
    swim_set_pen_color(&aData->iWin, G_textResults_EXP_DK[aResult].iColor);
    swim_put_text_xy(
        &aData->iWin,
        G_textResults_EXP_DK[aResult].iText,
        TEST_RESULT_RIGHT_COLUMN,
        ICalcY(aLine+aData->iLine));

    // Show for a moment, then go on
    UEZTaskDelay(aDelay);
}

void TestSetTestResult_EXP_DK(T_testData *aData, TUInt16 aPass)
{
    aData->iResult = aPass;
    if ((aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))  {
        // If any test is failed or skipped, not an 'all pass'
        G_td_EXP_DK.iAllPass = EFalse;
    }
    if ((aPass == TEST_RESULT_PASS)||(aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))
        aData->iGoNext = ETrue;
}

void TestNext_EXP_DK(T_testData *aData)
{
    aData->iGoNext = ETrue;
}

const T_testAPI G_testAPI_EXP_DK = {
    TestAddButtons_EXP_DK,
    TestRemoveButtons_EXP_DK,
    TestTextLine_EXP_DK,
    TestShowResult_EXP_DK,
    TestSetTestResult_EXP_DK,
    TestNext_EXP_DK,
};

/*---------------------------------------------------------------------------*
* Task:  FunctionalTest
*---------------------------------------------------------------------------*
* Description:
*      This tasks draws a crosshairs and target each time the user
*      touches the screen.
* Inputs:
*      T_uezTask aMyTask            -- Handle to this task
*      void *aParams               -- Parameters.  Not used.
*---------------------------------------------------------------------------*/
void FunctionalTest_EXP_DK(const T_choice *aChoice)
{
     T_uezInputEvent inputEvent;
     T_uezDevice ts;
     T_uezDevice lcd;
     TUInt32 lastX=0, lastY=0;
     T_pixelColor *pixels;
     T_uezQueue queue;
     INT_32 winX, winY;
     const T_button *p_button;
     const T_button *p_lastButton = 0;
     TUInt16 x, y;
     const T_testState *testState = &G_testStates_EXP_DK[0];
     const T_testState *lastTestState = 0;
     TBool isCancelled = EFalse;
     TBool isPausing = EFalse;
     
#if ENABLE_UEZ_BUTTON
     T_uezDevice keypadDevice;
#endif
     
     // Start with the first test
     testState = G_testStates_EXP_DK;
     
     memset(&G_td_EXP_DK, 0, sizeof(G_td_EXP_DK));
     
     // All tests pass until otherwise noted
     G_td_EXP_DK.iAllPass = ETrue;
     G_td_EXP_DK.iPauseComplete = EFalse;
     
     // Setup queue to receive touchscreen events
     if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if ENABLE_UEZ_BUTTON
          UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
          // Open up the touchscreen and pass in the queue to receive events
          if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
               // Open the LCD and get the pixel buffer
               if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
                    
                    // Clear the screen
                    //                IFillRect(pixels, 0, 0, RESOLUTION_X, RESOLUTION_Y, 0);
                    FunctionalTestScreen_EXP_DK(&G_td_EXP_DK);
                    
                    for (;(testState->iTitle) && !isCancelled;) {
                         if ((testState->iIgnoreIfExpansionBoard)) {
                              G_results_EXP_DK[testState-G_testStates_EXP_DK] = TEST_RESULT_IGNORE;
                              testState++;
                              continue;
                         }
                         
                         // New test state?
                         if (testState != lastTestState) {
                              // Check that we have room for these lines
                              if ((G_td_EXP_DK.iLine + testState->iLinesNeeded) >= TEST_RESULT_NUM_LINES) {
                                   TUInt32 i;
                                   // Back to the start
                                   G_td_EXP_DK.iLine = 0;
                                   // Clear all the lines
                                   for (i=0; i<TEST_RESULT_NUM_LINES; i++)
                                        G_testAPI_EXP_DK.iTextLine(&G_td_EXP_DK, i, 0);
                              }
                              
                              // Remove all buttons but Skip and Cancel
                              TestRemoveButtons_EXP_DK(&G_td_EXP_DK, 0xFFFF&~(OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL));
                              TestAddButtons_EXP_DK(&G_td_EXP_DK, OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL);
                              testState->iMonitorFunction(&G_testAPI_EXP_DK, &G_td_EXP_DK, OPT_INIT);
                              lastTestState = testState;
                         }
                         
                         // Wait forever until we receive a touchscreen event
                         // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                         // Check every 50 ms
                         if (UEZQueueReceive(queue, &inputEvent, 10)==UEZ_ERROR_NONE) {
                              winX = inputEvent.iEvent.iXY.iX;
                              winY = inputEvent.iEvent.iXY.iY;
                              swim_get_virtual_xy(&G_td_EXP_DK.iWin, &winX, &winY);
                              
                              // Is this a touching event?
                              if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD)  {
                                   // We are touching the screen.
                                   // Is this a different position than before?
                                   if ((inputEvent.iEvent.iXY.iX != lastX) || (inputEvent.iEvent.iXY.iY != lastY))  {
                                        x = inputEvent.iEvent.iXY.iX;
                                        y = inputEvent.iEvent.iXY.iY;
                                        // Determine which choice we are in
                                        for (p_button=G_testButtons_EXP_DK; p_button->iBit; p_button++)  {
                                             if ((x >= p_button->iX1) && (x <= p_button->iX2) &&
                                                 (y >= p_button->iY1) && (y <= p_button->iY2)) {
                                                      break;
                                                 }
                                        }
                                        if (p_button != p_lastButton) {
                                             // See if we got a button
                                             if (p_button->iBit & G_td_EXP_DK.iButtons) {
                                                  // Click it
                                                  TestButtonClick_EXP_DK(&G_td_EXP_DK, p_button->iBit);
                                                  
                                                  // Now process the command
                                                  if (isPausing) {
                                                       if (p_button->iBit & OPT_BUTTON_OK) {
                                                            // Clicked OK, just go on, already have a failure
                                                            G_td_EXP_DK.iGoNext = ETrue;
                                                            G_td_EXP_DK.iPauseComplete = ETrue;
                                                       }
                                                  } else {
                                                       testState->iMonitorFunction(&G_testAPI_EXP_DK, &G_td_EXP_DK, p_button->iBit);
                                                       if (p_button->iBit & OPT_BUTTON_SKIP) {
                                                            G_td_EXP_DK.iGoNext = ETrue;
                                                            G_testAPI_EXP_DK.iShowResult(&G_td_EXP_DK, 0, TEST_RESULT_SKIP, 250);
                                                            G_testAPI_EXP_DK.iSetTestResult(&G_td_EXP_DK, TEST_RESULT_SKIP);
                                                            
                                                            // If any test is skipped, does not pass
                                                            G_td_EXP_DK.iAllPass = EFalse;
                                                       }
                                                  }
                                                  if (p_button->iBit & OPT_BUTTON_CANCEL) {
                                                       // If any test is cancelled, does not pass
                                                       G_td_EXP_DK.iAllPass = EFalse;
                                                       isCancelled = ETrue;
                                                  }
                                             }
                                        }
                                        p_lastButton = p_button;
                                   }
                              } else {
                                   p_lastButton = 0;
                              }
                         }
                         
                         // Request to go on?
                         if (G_td_EXP_DK.iGoNext) {
                              // Did we fail?  And do we need to pause?
                              if ((G_td_EXP_DK.iResult == TEST_RESULT_FAIL) && (testState->iPauseOnFail)) {
                                   // Did we pause?
                                   if (!G_td_EXP_DK.iPauseComplete) {
                                        // Have not paused, put up the OK and Cancel buttons
                                        TestRemoveButtons_EXP_DK(&G_td_EXP_DK, 0xFFFF&~(OPT_BUTTON_CANCEL));
                                        TestAddButtons_EXP_DK(&G_td_EXP_DK, OPT_BUTTON_OK|OPT_BUTTON_CANCEL);
                                        
                                        // We're not done yet
                                        G_td_EXP_DK.iGoNext = EFalse;
                                        
                                        // And we're in pause mode
                                        isPausing = ETrue;
                                        
                                        // Play negative tone
                                        BeepError();
                                   } else {
                                        // Done pausing, reset everything
                                        G_td_EXP_DK.iPauseComplete = EFalse;
                                        isPausing = EFalse;
                                   }
                              }
                         }
                         // Still going on?
                         if (G_td_EXP_DK.iGoNext) {
                              // Record the result
                              G_results_EXP_DK[testState-G_testStates_EXP_DK] = G_td_EXP_DK.iResult;
                              
                              // Move to the next lines
                              G_td_EXP_DK.iLine+=testState->iLinesNeeded;
                              testState++;
                              G_td_EXP_DK.iGoNext = EFalse;
                         } else {
                              // Only monitor if we are not pausing
                              if (!isPausing)
                                   testState->iMonitorFunction(&G_testAPI_EXP_DK, &G_td_EXP_DK, 0);
                         }
                    }
                    UEZLCDClose(lcd);
               }
               UEZTSClose(ts, queue);
          }
#if ENABLE_UEZ_BUTTON
          UEZKeypadClose(keypadDevice, &queue);
#endif
          UEZQueueDelete(queue);
     }
}

void FunctionalTestScreen_EXP_DK(T_testData *aData)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;

    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDOn(lcd);
        UEZLCDBacklight(lcd, 256);

        SUIHidePage0();

        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        swim_window_open(
            &aData->iWin,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            pixels,
            0,
            0,
            DISPLAY_WIDTH-1,
            DISPLAY_HEIGHT-1,
            2,
            YELLOW,
            RGB(0, 0, 0),
            RED);
        swim_set_font(&aData->iWin, &APP_DEMO_DEFAULT_FONT);
        swim_set_title(&aData->iWin, "Functional Test", BLUE);

        SUIShowPage0();
    }
}

void FuncTestPageHit_EXP_DK(void)
{
}

#endif

/*-------------------------------------------------------------------------*
* File:  FunctionalTest_EXP_DK.c
*-------------------------------------------------------------------------*/
