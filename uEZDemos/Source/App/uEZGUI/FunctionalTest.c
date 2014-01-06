/* File:  FunctionalTest.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for ARM Carrier board with ARM7DIMM
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
#include "AppDemo.h"
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#if (RTOS == FreeRTOS)
#include "FreeRTOS.h"
#endif
#include "task.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#if LIGHT_SENSOR_ENABLED
#include <Source/Devices/Light Sensor/ROHM/BH1721FVC/Light_Sensor_BH1721FVC.h>
#include <Device/LightSensor.h>
#endif
#include <UEZLCD.h>
#include <uEZKeypad.h>
#include <HAL/GPIO.h>
#include <DEVICE/ADCBank.h>
#include <Device/Accelerometer.h>
#include <Device/ButtonBank.h>
#include <Device/EEPROM.h>
#include <Device/LEDBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <Types/TimeDate.h>
#include <UEZGPIO.h>

#include <uEZProcessor.h>
#include <uEZPlatform.h>

#define DO_FULL_NOR_FLASH_TEST    0
#define DO_EXTENSIVE_SDRAM_TEST   0

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
static void ITestPatternColors(T_testData *aData);
extern void Beep(void);
extern void BeepError(void);
extern void ButtonClick(void);
extern void PutPointRaw(TUInt8 aPage, int x, int y, TUInt16 pixel);
extern void DrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
extern void IHidePage0(void);
extern void IShowPage0(void);
extern void PlayAudio(TUInt32 aHz, TUInt32 aMS);
extern void FunctionalTestScreen(T_testData *aData);
extern TBool UEZGUIIsLoopbackBoardConnected();

extern T_uezError USBHost_SwitchPort(void *aWorkspace, TUInt32 aPort);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_testState G_testStates[] = {
    //{ "GPIOs", FuncTestGPIOs, 3, ETrue, ETrue, ETrue },
    { "RTC", FuncTestExternalRTC, 1, ETrue, EFalse, EFalse },
    { "Speaker", FuncTestSpeaker, 1, EFalse, EFalse, EFalse },
    { "LCD", FuncTestLCD, 10, EFalse, EFalse, EFalse },
#if LIGHT_SENSOR_ENABLED
{ "Light Sensor", FuncTestLightSensor, 3, ETrue, EFalse, EFalse },
#endif
    { "Accelerometer", FuncTestAccelerometerFast, 1, EFalse, EFalse, EFalse },
    { "SDRAM", FuncTestSDRAM, 2, ETrue, EFalse, EFalse },
    { "Micro SDCard", FuncTestMicroSDCard, 3, ETrue, EFalse, EFalse },
    { "Temperature Sensor", FuncTestTemperature, 1, ETrue, EFalse, EFalse },
    { "EEPROM", FuncTestEEPROM, 1, ETrue, EFalse, EFalse },
    { "NOR Flash", FuncTestFlash0, 3, ETrue, EFalse, EFalse },
//    { "Serial", FuncTestSerial, 1, ETrue },
    { "Test Complete", FuncTestComplete, 1000, EFalse, EFalse },
    { 0 }
};
T_testData G_td;
static TUInt8 G_results[50];

void FuncTestExternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        TUInt32 iStart;
        DEVICE_RTC **iRTC;
    } T_rtcTest;
    T_rtcTest *p = (T_rtcTest *)aData->iData;
    T_uezTimeDate td;

    if (aButton == OPT_INIT) {
        UEZDeviceTableFind("RTC", &p->iDevice);
        UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iRTC);
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2009;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2009, 8:00:00
        (*p->iRTC)->Set(p->iRTC, &td);

        // Now wait a few seconds
        p->iStart = UEZTickCounterGet();

        aAPI->iTextLine(aData, 0, "Checking External RTC ...");
    } else if (aButton == 0) {
        // Show the current time
        // Has it been three seconds?
        if (UEZTickCounterGetDelta(p->iStart) >= 3000) {
            // Times up!
            // Read the RTC clock and make sure the time is what we
            // generally expected
            (*p->iRTC)->Get(p->iRTC, &td);
            if ((td.iDate.iMonth==1) &&
                    (td.iDate.iDay == 1) &&
                    (td.iDate.iYear == 2009) &&
                    (td.iTime.iHour == 8) &&
                    (td.iTime.iMinute == 0) &&
                    (td.iTime.iSecond > 1)) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            }
            aAPI->iNextTest(aData);
        }
    }
}


void FuncTestMicroSDCard(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
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
    }
}

void FuncTestLightSensor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
#define BH1721FVC_I2C_ADDR              (0x23)
#define BH1721FVC_I2C_SPEED             400    // Speed in kHz
#define BH1721FVC_POWER_DOWN		0x00
#define BH1721FVC_POWER_UP		0x01
#define BH1721FVC_COUNTINOUS_AUTO_RES	0x20   // Only mode used in first pass
#define BH1721FVC_COUNTINOUS_HIGH_RES	0x22
#define BH1721FVC_COUNTINOUS_LOW_RES	0x23
  //  TUInt32 G_Backlight_LowLevel = 50, G_Backlight_HighLevel = 150;
  T_uezError error = UEZ_ERROR_NONE;
  T_uezDevice iInterfaceDevice;
  TUInt8 data[2];
  char iLine[80];
  TUInt8 dataIn[2];
  TUInt32 lums;

  if (aButton == OPT_INIT) {
    aAPI->iTextLine(aData, 0, "Checking Light Sensor ... ");
    aAPI->iTextLine(aData, 1, "  Make sure there is light shining on the sensor.");

    error = UEZI2COpen("I2C1", &iInterfaceDevice);//Open I2C device
    data[0] = BH1721FVC_POWER_UP; //Send command to power on the light sensor
    error = UEZI2CWrite(iInterfaceDevice, BH1721FVC_I2C_ADDR,
                        BH1721FVC_I2C_SPEED, data, 1, 50);//UEZ_TIMEOUT_INFINITE);
    data[0] = BH1721FVC_COUNTINOUS_HIGH_RES;
    error = UEZI2CWrite(iInterfaceDevice, BH1721FVC_I2C_ADDR,
                        BH1721FVC_I2C_SPEED, data, 1, 50);//UEZ_TIMEOUT_INFINITE);

    error = UEZI2CRead(iInterfaceDevice, BH1721FVC_I2C_ADDR,
                       BH1721FVC_I2C_SPEED, dataIn, 2, UEZ_TIMEOUT_INFINITE);//send command to read the High and Low bytes
    lums = dataIn[0] << 8;
    lums |= dataIn[1]; //combine them
    lums = (lums * 10) / 12; //multiply by 10 and then divide the result by 12
    if (error)
    {
      lums = 0;
    }
    printf("%u Lux\n",lums); // command console
    sprintf(iLine, "Checking Light Sensor ... %u Lux", lums);
    aAPI->iTextLine(aData, 2, iLine);
    UEZTaskDelay(200);
  } else if (aButton == OPT_BUTTON_OK) {
    aAPI->iNextTest(aData);
  }
  if (error == UEZ_ERROR_NONE) {
    aAPI->iTextLine(aData, 2, "  Checking Light Sensor ... Found!");
    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
    sprintf(aData->iResultValue, "OK");
  }
  if (error) {
    aAPI->iTextLine(aData, 0, "Checking Light Sensor ... Not found!");
    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
    sprintf(aData->iResultValue, "Not found");
  }
}

void FuncTestAccelerometer(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Accelerometer **iAccel;
        TUInt8 iSubstep;
        AccelerometerReading iReading;
        char iLine[80];
    } T_accelTest;
    T_accelTest *p = (T_accelTest *)aData->iData;
    T_uezError error;
    TInt32 y;

    if (aButton == OPT_INIT) {
        p->iSubstep = 0;
        aAPI->iTextLine(aData, 0, "Checking Accelerometer ... ");
        error = UEZDeviceTableFind("Accel0", &p->iDevice);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iAccel);
        if (error) {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Not found!");
            sprintf(aData->iResultValue, "Not found");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
    } else if (aButton == 0) {
        (*p->iAccel)->ReadXYZ(p->iAccel, &p->iReading, 250);
        y = p->iReading.iY>>10;
        if (p->iSubstep == 0) {
            sprintf(p->iLine, "  Tilt the board to the back (%d)", y);
            aAPI->iTextLine(aData, 1, p->iLine);
            // Looking for left tilt
            if (y < -10) {
                aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                p->iSubstep = 1;
            }
        } else {
            sprintf(p->iLine, "  Tilt the board to the front (%d)", y);
            aAPI->iTextLine(aData, 2, p->iLine);
            // Looking for right tilt
            if (y > 10) {
                aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                sprintf(aData->iResultValue, "OK");
            }
        }
        UEZTaskDelay(20); // give it a chance to appear on the screen
    }
}

void FuncTestAccelerometerFast(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Accelerometer **iAccel;
        TUInt8 iSubstep;
        AccelerometerReading iReading;
        char iLine[80];
    } T_accelTest;
    T_accelTest *p = (T_accelTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        p->iSubstep = 0;
        aAPI->iTextLine(aData, 0, "Checking Accelerometer ... ");
        error = UEZDeviceTableFind("Accel0", &p->iDevice);
        if (!error) {
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iAccel);
            if (!error) {
                error = (*p->iAccel)->ReadXYZ(p->iAccel, &p->iReading, 2500);
            }
        }
        if (error) {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Not found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Not found");
            return;
        } else {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Found.");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            sprintf(aData->iResultValue, "OK");
        }
    }
}

void FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt8 iSubstep;
    } T_speakerTest;
    T_speakerTest *p = (T_speakerTest *)aData->iData;
    static const TUInt32 tones[8] = { 523, 659, 783, 1046, 783, 659, 523, 0 };

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Speaker ... ");
        aAPI->iTextLine(aData, 1, "  Do you hear the tones?");
        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
        p->iSubstep = 0;
    } else if (aButton == 0) {
        PlayAudio(tones[p->iSubstep], 100);
        p->iSubstep = (p->iSubstep+1)&7;
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iTextLine(aData, 1, "  Tones are heard");
        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        sprintf(aData->iResultValue, "OK");
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iTextLine(aData, 1, "  Tones are NOT heard");
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        sprintf(aData->iResultValue, "Not heard");
    }
}

void FuncTestInternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        TUInt32 iStart;
        DEVICE_RTC **iRTC;
    } T_rtcTest;
    T_rtcTest *p = (T_rtcTest *)aData->iData;
    T_uezTimeDate td;

    if (aButton == OPT_INIT) {
        UEZDeviceTableFind("RTC", &p->iDevice);
        UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iRTC);
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2009;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2009, 8:00:00
        (*p->iRTC)->Set(p->iRTC, &td);

        // Now wait a few seconds
        p->iStart = UEZTickCounterGet();

        aAPI->iTextLine(aData, 0, "Checking Internal RTC ...");
    } else if (aButton == 0) {
        // Show the current time
        // Has it been three seconds?
        if (UEZTickCounterGetDelta(p->iStart) >= 3000) {
            // Times up!
            // Read the RTC clock and make sure the time is what we
            // generally expected
            (*p->iRTC)->Get(p->iRTC, &td);
            if ((td.iDate.iMonth==1) &&
                    (td.iDate.iDay == 1) &&
                    (td.iDate.iYear == 2009) &&
                    (td.iTime.iHour == 8) &&
                    (td.iTime.iMinute == 0) &&
                    (td.iTime.iSecond > 1)) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                sprintf(aData->iResultValue, "OK");
            } else {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "Bad Time");
            }
            aAPI->iNextTest(aData);
        }
    }
}

void FuncTestEEPROM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    TUInt8 eepromData[32];
    const TUInt32 baseEEPROMAddr = 0x41;
    DEVICE_EEPROM **p_eeprom;
    T_uezDevice eeprom;
    T_uezError error;
#if 1
    static const TUInt8 G_correct[32] = {
        0xAA, 0xAA, 0x55, 0x55, 0x55, 0x01, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA,
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    };
#endif
    static TUInt8 previousContents[32];

    error = UEZDeviceTableFind("EEPROM0", &eeprom);
    if (error) {
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        return;
    }
    error = UEZDeviceTableGetWorkspace(eeprom, (T_uezDeviceWorkspace **)&p_eeprom);
    if (error) {
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        return;
    }

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking EEPROM ...");

        // First, read the section of the EEPROM we are going to use
        error = (*p_eeprom)->Read(p_eeprom, baseEEPROMAddr+0, previousContents, 32);
        if (error != UEZ_ERROR_NONE) {
            // Failure to read?  Then mark failed
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Read Error");
            return;
        }

        // Write test pattern to EEPROM (with data over boundaries)
        // Writes a group of 0xAA across a 32 byte area
        // and then writes 0x55 over a subset of those 32 bytes.
        // Should test boundary conditions and writes within a page.
        memset(eepromData, 0xAA, sizeof(eepromData));
        error = (*p_eeprom)->Write(p_eeprom, baseEEPROMAddr+0, eepromData, 32);
        memset(eepromData, 0x55, sizeof(eepromData));
        if (!error)
            error = (*p_eeprom)->Write(p_eeprom, baseEEPROMAddr+2, eepromData, 20);
        // Throw one more byte into the mix with the single byte write
        eepromData[0] = 0x01;
        if (!error)
            (*p_eeprom)->Write(p_eeprom, baseEEPROMAddr+5, eepromData, 1);
        if (error != UEZ_ERROR_NONE)  {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Write Error");
            return;
        }
    } else if (aButton == 0) {
        // Now check it
        // Now read the eeprom and see what pattern we got
        // Should look like this:
        // 00: AA AA 55 55 55 01 55 55
        // 08: 55 55 55 55 55 55 55 55
        // 10: 55 55 55 55 55 55 AA AA
        // 18: AA AA AA AA AA AA AA AA
        memset(eepromData, 0, sizeof(eepromData));
        error = (*p_eeprom)->Read(p_eeprom, baseEEPROMAddr+0, eepromData, 32);
        if (error != UEZ_ERROR_NONE)  {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Read Error");
        } else {
            if (memcmp(G_correct, eepromData, 32) == 0) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "Verify Error");
            }
        }

        // Regardless, try to put back the original values
        (*p_eeprom)->Write(p_eeprom, baseEEPROMAddr+0, previousContents, 32);
    }
}

void FuncTestTemperature(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Temperature **iTemp;
        TUInt32 iCount;
        TInt32 iMinV;
        TInt32 iMaxV;
        char iLine[80];
    } T_tempTest;
    T_tempTest *p = (T_tempTest *)aData->iData;

    T_uezError error;
    TInt32 v;
    TUInt32 frac;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Temperature ...");

        error = UEZDeviceTableFind("Temp0", &p->iDevice);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iTemp);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Not Found");
            return;
        }
        p->iMinV = 25<<16;
        p->iMaxV = 25<<16;
        p->iCount = 0;
    } else if (aButton == 0) {
        error = (*p->iTemp)->Read(p->iTemp, &v);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Read Failed");
            return;
        }
        if (v > p->iMaxV)
            p->iMaxV = v;
        if (v < p->iMinV)
            p->iMinV = v;
        // Get 3 bits of fractional part and multiply by 125 for
        // 0 --
        if (v < 0)
            frac = -v;
        else
            frac = v;
        frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
        sprintf(p->iLine, "Checking Temperature ... %d.%03d C", v>>16, frac);
        aAPI->iTextLine(aData, 0, p->iLine);
        UEZTaskDelay(200);

        p->iCount++;
        if (p->iCount > 5) {
            // Done?  Is it in range
            if (p->iMinV < (15<<16)) {
                v = p->iMinV;
                if (v < 0)
                    frac = -v;
                else
                    frac = v;
                frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
                sprintf(p->iLine, "Checking Temperature ... %d.%03d C (COLD!)", v>>16, frac);
                aAPI->iTextLine(aData, 0, p->iLine);

                // Too cold!
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "%d.%03d C (COLD)", v>>16, frac);
            } else if (p->iMaxV > (40<<16)) {
                v = p->iMaxV;
                if (v < 0)
                    frac = -v;
                else
                    frac = v;
                frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
                sprintf(p->iLine, "Checking Temperature ... %d.%03d C (HOT!)", v>>16, frac);
                aAPI->iTextLine(aData, 0, p->iLine);

                // Too hot!
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "%d.%03d C (HOT)", v>>16, frac);
            } else {
                // Valid!
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                sprintf(aData->iResultValue, "%d.%03d C", v>>16, frac);
            }
        }
    }
}

void FuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
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

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Serial Port ...");

        error = UEZDeviceTableFind("Console", &p->iDevice);
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

        p->iReceived[0] = '.';

        // Do again, send out a 4 character string to the serial
        error = (*p->iStream)->Write(p->iStream, "Test", 4, &num, 100);
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
            if (memcmp(p->iReceived, "Test", 4) == 0) {
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


TUInt32 MemorySize(void)
{
    TUInt8 mem[100];
    TUInt8 prime;
    volatile TUInt8 *Base = (TUInt8 *)0xA0000000;
    volatile TUInt8 *Check = (TUInt8 *)0xA0010000;
    TUInt32 i;
    TUInt32 j;

    RTOS_ENTER_CRITICAL();

    while (1) {
        // Write a pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            mem[i] = Check[i];
            Check[i] = prime;
        }
        // Check the pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }
        // Changing base should not change the pattern here
        // (and thrash the cache)
        for (j=0; j<100; j++)  {
            for (i=0; i<1024; i++)
                Base[i] = 0xFF;
        }

        // Check the pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }

        // Replace the memory
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            Check[i] = mem[i];
        }
        Check += 0x10000;
    }
exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-((TUInt8 *)0xA0000000);
}

#if UEZBSP_SDRAM
TUInt32 SDRAMMemoryTest(TUInt32 aSize)
{
    TUInt8 mem[100];
    TUInt8 prime;
    volatile TUInt8 *Base = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR;
    volatile TUInt8 *Check = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR+0x10000;
//volatile TUInt8 *Check = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR+0x400000;
    TUInt32 i;
    TUInt32 j;

    RTOS_ENTER_CRITICAL();

    while (Check < (Base + UEZBSP_SDRAM_SIZE)) {
        // Write a pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            mem[i] = Check[i];
            Check[i] = prime;
        }
        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }
        // Changing base should not change the pattern here
        // (and thrash the cache)
        for (j=0; j<100; j++)  {
            for (i=0; i<1024; i++)
                Base[i] = 0xFF;
        }

        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }

        // Replace the memory
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            Check[i] = mem[i];
        }
        Check += 0x10000;
    }
exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-((TUInt8 *)0xA0000000);
}

// This version messes up the screen to ensure uniqueness across the whole memory
TUInt32 SDRAMMemoryTestExtensive(TUInt32 aSize)
{
    TUInt32 prime;
    volatile TUInt32 *Base = (TUInt32 *)UEZBSP_SDRAM_BASE_ADDR;
    volatile TUInt32 *Check = (TUInt32 *)UEZBSP_SDRAM_BASE_ADDR+0x10000;
    TUInt32 i;
    TUInt32 j;
    const TUInt32 checkSize = 0x10000/4;

    RTOS_ENTER_CRITICAL();

     // Fill the memory with a mostly unique pattern
    Check = (TUInt32 *)(UEZBSP_SDRAM_BASE_ADDR+0x10000);
    while (Check < (Base + UEZBSP_SDRAM_SIZE/4)) {
        // Write a pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<checkSize; i++, prime+=7) {
            Check[i] = (TUInt32)(Check+i);
        }
        Check += 0x10000/4;
    }
    // Changing base should not change the pattern here
    // (and thrash the cache)
    for (j=0; j<10; j++)  {
        for (i=0; i<checkSize; i++)
            Base[i] = 0xFFFFFFFF;
    }

    Check = (TUInt32 *)(UEZBSP_SDRAM_BASE_ADDR+0x10000);
    while (Check < (Base + UEZBSP_SDRAM_SIZE/4)) {
        // Check the pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<checkSize; i++, prime+=7) {
            if (Check[i] != ((TUInt32)(Check+i)))
                goto exit;
        }
        Check += 0x10000/4;
    }

exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return ((TUInt32)Check)-0xA0000000;
}

void FuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 size;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking SDRAM ...");

        // Just do it quickly and size it
#if DO_EXTENSIVE_SDRAM_TEST
        size = SDRAMMemoryTestExtensive(UEZBSP_SDRAM_SIZE);
#else
        size = SDRAMMemoryTest(UEZBSP_SDRAM_SIZE);
#endif
        sprintf(p->iLine, "Checking SDRAM ... Size: %d bytes", size);
        aAPI->iTextLine(aData, 0, p->iLine);
//        aAPI->iTextLine(aData, 1, "  Is this the correct size?");
//        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
        if (size == UEZBSP_SDRAM_SIZE) {
            aAPI->iTextLine(aData, 1, "  Correct size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            sprintf(aData->iResultValue, "0x%08X", size);
        } else {
            aAPI->iTextLine(aData, 1, "  **INCORRECT** size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "0x%08X", size);
        }
    }
}
#endif

TUInt32 NORFlashMemoryTest(
        const T_testAPI *aAPI,
        T_testData *aData,
        T_uezDevice dev,
        TUInt32 aSize)
{
    typedef struct {
        char iLine[80];
    } T_norFlashTest;
    T_uezError error;
    TUInt32 reg;
    TUInt8 buffer[256];
    TUInt32 i;
#if DO_FULL_NOR_FLASH_TEST
    TUInt32 flip;
    TUInt32 base;
    TUInt32 addrBit;
#endif

    aAPI->iTextLine(aData, 1, "  Checking QRY");

    // Start with determining if there is a 'QRY' set of characters
    sprintf(aData->iResultValue, "Not Found");
    error = UEZFlashQueryReg(dev, 0x10, &reg);
    if (error || reg != 'Q')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x11, &reg);
    if (error || reg != 'R')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x12, &reg);
    if (error || reg != 'Y')
        return 1001;

    sprintf(aData->iResultValue, "Block Erase");
    aAPI->iTextLine(aData, 1, "  Block Erase");
    error = UEZFlashBlockErase(dev, 0x00700000, 256*1024); // first 256K
    if (error)
        return 1002;
    error = UEZFlashRead(dev, 0x00700000, buffer, 128);
    if (error)
        return 1002;
    // Should be all 0xFF
    for (i=0; i<128; i++)
        if (buffer[i] != 0xFF)
            return 1003;

    // Do write/read test over 128K boundary
    sprintf(aData->iResultValue, "Block Write");
    aAPI->iTextLine(aData, 1, "  Write test");
    error = UEZFlashWrite(dev, 0x00700000+128*1024-4, "Hello ", 6);
    if (error)
        return 1004;
    error = UEZFlashWrite(dev, 0x00700000+128*1024-4+6, "World ", 6);
    if (error)
        return 1004;
    error = UEZFlashRead(dev, 0x00700000+128*1024-4, buffer, 128);
    if (error)
        return 1004;
    if (memcmp(buffer, "Hello World ", 12) != 0)
        return 1004;

#if DO_FULL_NOR_FLASH_TEST
    // Now do a more extensive test over the whole chip, but do
    // blocks of 256 bytes for testing.
    for (flip=0; flip<2; flip++) {
        for (base=0,addrBit=1; base<aSize; base=(base)?(base<<1):2, addrBit++) {
            sprintf((char*)buffer, "  A%d - Data line %s Test", addrBit-1, flip?"Zero":"One");
            aAPI->iTextLine(aData, 1, (const char*)buffer);
            // Set up a pattern to write moving either a 1 or 0
            // in the data.
            if (flip) {
                for (i=0; i<256; i++)
                    buffer[i] = i ^ 0xFF;
            } else {
                for (i=0; i<256; i++)
                    buffer[i] = i;
            }
            if (UEZFlashBlockErase(dev, base, 256))
                return 1005;
            if (UEZFlashWrite(dev, base, buffer, 256))
                return 1005;
            memset(buffer, 0xCC, 256);
            if (UEZFlashRead(dev, base, buffer, 256))
                return 1005;
            if (flip) {
                for (i=0; i<256; i++)
                    if (buffer[i] != (i ^ 0xFF))
                        return 1006;
            } else {
                for (i=0; i<256; i++)
                    if (buffer[i] != i)
                        return 1006;
            }
            if (UEZFlashBlockErase(dev, base, 256))
                return 1005;
        }
    }
#if 0
    // Start with a good ol' fashion chip erase
    error = UEZFlashChipErase(dev);
    if (error)
        return 1009;
#endif
#endif

    sprintf(aData->iResultValue, "OK");

    // Got here with no errors, good!
    return 0;
}

void FuncTestFlash0(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 errorCode;
    T_uezDevice dev;
    T_FlashChipInfo info;

    if (aButton == OPT_INIT) {
        // Just do it quickly and size it
        aAPI->iTextLine(aData, 0, "Checking NOR Flash Memory ...");
        if (UEZFlashOpen("Flash0", &dev)) {
            errorCode = 1000;
        } else {
            UEZFlashGetChipInfo(dev, &info);
            errorCode = NORFlashMemoryTest(aAPI, aData, dev, info.iNumBytesLow);
            UEZFlashClose(dev);
        }

        if (errorCode==0) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        } else {
            sprintf(p->iLine, "  Error Code %d!", errorCode & 0x7FFF);
            aAPI->iTextLine(aData, 2, p->iLine);
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        }
    }
}

static void ITestPatternColors(T_testData *aData)
{
    int x, y;
    TUInt16 i=0;

    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if 0//(UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01 || UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T1DG28)
            swim_set_pen_color(&aData->iWin, (y<<11));
#elif  (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(&aData->iWin, (y<<10)+((x&128)?0x8000:0));
#else
            swim_set_pen_color(&aData->iWin, (y<<10));
#endif
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40);
        }
    }
    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01)
            swim_set_pen_color(&aData->iWin, (y<<5));
#elif(UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(&aData->iWin, (y<<5));
#else
            swim_set_pen_color(&aData->iWin, (y<<5));
#endif
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40+32);
        }
    }
    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01 )
            swim_set_pen_color(&aData->iWin, (y<<0));
#elif (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(&aData->iWin, (y<<0));
#else
            swim_set_pen_color(&aData->iWin, (y<<0));
#endif
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40+64);
        }
    }
}

void FuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking LCD Colors ...");
        ITestPatternColors(aData);
        aAPI->iTextLine(aData, 9, "Do the red-green-blue appear in order and smooth?");
        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iShowResult(aData, 9, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        return;
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iShowResult(aData, 9, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        return;
    }
}

#include <HAL/USBHost.h>

void FuncTestUSBHost1(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
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
}

void FuncTestUSBHost2(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
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
    }
}


void FuncTestBacklightMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_backlightTest;
    T_backlightTest *p = (T_backlightTest *)aData->iData;
    T_uezDevice adc;
    T_uezError error;
    ADC_RequestSingle r;
    TUInt32 reading;
    TUInt32 percent1;
    TUInt32 percent2;
    TUInt32 percent;
    T_uezDevice lcd;
    TUInt32 i;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Backlight Monitor ...");
    UEZLCDOpen("LCD", &lcd);

        // Just do it quickly
        error = UEZADCOpen("ADC0", &adc);
        if (error) {
            aAPI->iTextLine(aData, 0, "Backlight Monitor ADC not Found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Not Found");
        } else {
            // Take a reading
            r.iADCChannel = 0;
            r.iBitSampleSize = 10;
            r.iTrigger = ADC_TRIGGER_NOW;
            r.iCapturedData = &reading;
            error = UEZADCRequestSingle(adc, &r);
            if (error) {
                aAPI->iTextLine(aData, 0, "Backlight Monitor ADC bad reading!");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "No Reading");
            } else {
                // TBD: Is 0x180 the mid point of the expected range?
                percent = reading * 100 / 0x180;
                percent1 = percent;
                percent2 = 0;
                sprintf(p->iLine, "Backlight Monitor (on: %d %%, off: %d %%)", percent1, percent2);
                aAPI->iTextLine(aData, 0, p->iLine);
                sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);

                // We'll be generous and allow +/- 20%
                if ((percent < 80) || (percent > 120)) {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    sprintf(aData->iResultValue, "%d%% (NOT ON)", percent1);
                } else {
                    // Now turn off the LCD
                    UEZLCDOff(lcd);
                    UEZLCDBacklight(lcd, 0);
                    // Settling time
                    UEZTaskDelay(100);
                    UEZADCRequestSingle(adc, &r);
                    percent = reading * 100 / 0x180;
                    UEZLCDOn(lcd);
                    for (i=0; i<256; i+=2) {
                        UEZLCDBacklight(lcd, i);
                        UEZTaskDelay(10);
                    }

                    percent2 = percent;
                    sprintf(p->iLine, "Backlight Monitor (on: %d %%, off: %d %%)", percent1, percent2);
                    aAPI->iTextLine(aData, 0, p->iLine);
                    sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);

                    if (percent > 50) {
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        sprintf(aData->iResultValue, "%d%% (NOT OFF)", percent2);
                    } else {
                        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                        sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);
                    }
                }
            }
        }
        UEZADCClose(adc);
        UEZLCDClose(lcd);
    }
}
#if (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)

void FuncTestBacklightPWM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
#if 0
  // char iLine[80];
  // UEZ_LCD_BACKLIGHT_PERIOD_COUNT 400000 max 100%
  TUInt32 percent;
  if (aButton == OPT_INIT) {
    aAPI->iTextLine(aData, 0, "Backlight PWM setting ...");
    percent = UEZ_LCD_BACKLIGHT_PERIOD_COUNT/400000*100;
    if (percent > 90) {
      aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
      aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
      sprintf(aData->iResultValue, "%d%%", percent);
    } else {
      aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
      aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
      sprintf(aData->iResultValue, "%d%%", percent);
    }
  }
#endif
}
#endif
void FuncTestVoltageMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_backlightTest;
    T_backlightTest *p = (T_backlightTest *)aData->iData;
    T_uezDevice adc;
    T_uezError error;
    ADC_RequestSingle r;
    TUInt32 reading;
    TUInt32 percent;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Voltage Monitor ...");

        // Just do it quickly
        error = UEZADCOpen("ADC0", &adc);
        if (error) {
            aAPI->iTextLine(aData, 0, "Voltage Monitor ADC not Found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Not Found");
        } else {
            // Take a reading
            r.iADCChannel = 2;
            r.iBitSampleSize = 10;
            r.iTrigger = ADC_TRIGGER_NOW;
            r.iCapturedData = &reading;
            error = UEZADCRequestSingle(adc, &r);
            if (error) {
                aAPI->iTextLine(aData, 0, "Voltage Monitor ADC bad reading!");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "No Reading");
            } else {
                // Generate percentage based on perfect 5V input
                // divided by 2, but compared to a A/D value of 0 - 0x3FFF
                // with a reference voltage of 3.3V
                //   reading = (voltage / 5 V) * 0x3FF
                //   percent = 100 * (reading / (0x3FF * 3.3V/5V))
                percent = reading * 100 / 0x307;
                sprintf(p->iLine, "Voltage Monitor (%d %%)", percent);
                aAPI->iTextLine(aData, 0, p->iLine);
                // We'll be generous and allow +/- 20%
                if ((percent < 80) || (percent > 120)) {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                } else {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                }
            }
        }
        UEZADCClose(adc);
    }
}

typedef unsigned long long TUInt64;

typedef struct {
    TUInt64 iPinsDrive;
    TUInt64 iPinsChange;
    const char *iMsg;
} T_gpioTestEntry;

typedef struct {
    TUInt8 iMapPort;
    TUInt32 iMapPin;
    const char *iText;
} T_gpioMapping;
#if 1
static const T_gpioMapping G_gpioMapping[] = {
    #define FCTPIN_P1_0            (1<<0)
    { 1,    0, "P1[0]" },
    #define FCTPIN_P1_1            (1<<1)
    { 1,    1, "P1[1]" },
    #define FCTPIN_P1_4            (1<<2)
    { 1,    4, "P1[4]" },
    #define FCTPIN_P1_8            (1<<3)
    { 1,    8, "P1[8]" },
    #define FCTPIN_P1_9            (1<<4)
    { 1,    9, "P1[9]" },
    #define FCTPIN_P1_10           (1<<5)
    { 1,   10, "P1[10]" },
    #define FCTPIN_P1_14           (1<<6)
    { 1,   14, "P1[14]" },
    #define FCTPIN_P1_15           (1<<7)
    { 1,   15, "P1[15]" },
    #define FCTPIN_P1_16           (1<<8)
    { 1,   16, "P1[16]" },
    #define FCTPIN_P1_17           (1<<9)
    { 1,   17, "P1[17]" },
//    #define FCTPIN_P1_31           (1<<10) // used on Sharp DG28
//    { 1,   31, "P1[31]" },
    #define FCTPIN_P0_26           (1<<10)
    { 0,   26, "P0[26]" },
    #define FCTPIN_P0_4            (1<<11)
    { 0,    4, "P0[4]" },
    #define FCTPIN_P0_5            (1<<12)
    { 0,    5, "P0[5]" },
    #define FCTPIN_P0_6            (1<<13)
    { 0,    6, "P0[6]" },
    #define FCTPIN_P0_7            (1<<14)
    { 0,    7, "P0[7]" },
    #define FCTPIN_P0_8            (1<<15)
    { 0,    8, "P0[8]" },
    #define FCTPIN_P0_9            (1<<16)
    { 0,    9, "P0[9]" },
    #define FCTPIN_P1_19           (1<<17)
    { 1,   19, "P1[19]" },
    #define FCTPIN_P4_27           (1<<18)
    { 4,   27, "P4[27]" },
    #define FCTPIN_P4_26           (1<<19)
    { 4,   26, "P4[26]" },
    #define FCTPIN_P0_29           (1<<20)
    { 0,   29, "P0[29]" },
    #define FCTPIN_P0_30           (1<<21)
    { 0,   30, "P0[30]" },
    #define FCTPIN_P0_15           (1<<22)
    { 0,   15, "P0[15]" },
    #define FCTPIN_P0_16           (1<<23)
    { 0,   16, "P0[16]" },
    #define FCTPIN_P0_17           (1<<24)
    { 0,   17, "P0[17]" },
    #define FCTPIN_P0_22           (1<<25)
    { 0,   22, "P0[22]" },
    #define FCTPIN_P0_19           (1<<26)
    { 0,   19, "P0[19]" },
    #define FCTPIN_P0_20           (1<<27)
    { 0,   20, "P0[20]" },
    #define FCTPIN_P0_10           (1<<28)
    { 0,   10, "P0[10]" },
    #define FCTPIN_P0_11           (1<<29)
    { 0,   11, "P0[11]" },
    //    #define FCTPIN_P5_4             (((TUInt64)1)<<31)
    //    { 5,    4, "P5[4]" },
    //#define FCTPIN_P5_3             (((TUInt64)1)<<32)
    //{ 5,    3, "P5[3]" },
    //#define FCTPIN_P5_2             (((TUInt64)1)<<33)
    //{ 5,    2, "P5[2]" },
    #define FCTPIN_P1_12            (((TUInt64)1)<<30)
    { 1,   12, "P1[12]" },
    #define FCTPIN_P1_11            (((TUInt64)1)<<31)
    { 1,   11, "P1[11]" },
    #define FCTPIN_P1_7             (((TUInt64)1)<<32)
    { 1,    7, "P1[7]" },
    #define FCTPIN_P1_6             (((TUInt64)1)<<33)
    { 1,    6, "P1[6]" },
    #define FCTPIN_P1_5             (((TUInt64)1)<<34)
    { 1,    5, "P1[5]" },
    #define FCTPIN_P1_3             (((TUInt64)1)<<35)
    { 1,    3, "P1[3]" },
    #define FCTPIN_P0_0             (((TUInt64)1)<<36)
    { 0,    0, "P0[0]" },
    #define FCTPIN_P0_1             (((TUInt64)1)<<37)
    { 0,    1, "P0[1]" },
//    #define FCTPIN_P0_13            (((TUInt64)1)<<39)
//    { 0,   13, "P0[13]" },
//    #define FCTPIN_P0_12            (((TUInt64)1)<<40)
//    { 0,   12, "P0[12]" },
//    #define FCTPIN_P0_25            (((TUInt64)1)<<41)
//    { 0,   25, "P0[25]" },
    //#define FCTPIN_P0_2           (((TUInt64)1)<<31)
    //{ 0,   2, "P0[2]" },
    //#define FCTPIN_P0_3           (((TUInt64)1)<<32)
    //{ 0,   3, "P0[3]" },
};

const T_gpioTestEntry G_gpioTestArray[] = {
    //{ FCTPIN_P0_2,          FCTPIN_P0_11,               "P0[2] -> P0[11]" },
    //{ FCTPIN_P0_3,          FCTPIN_P0_10,               "P0[3] -> P0[10]" },
    { FCTPIN_P0_10,         FCTPIN_P1_16|FCTPIN_P4_26,  "P0[10] -> P1[16]/P4[26]" },
    { FCTPIN_P1_0,          FCTPIN_P0_20,               "P1[0] -> P0[20]" },
    { FCTPIN_P1_1,          FCTPIN_P0_19,               "P1[1] -> P0[19]" },
    { FCTPIN_P1_4,          FCTPIN_P0_22,               "P1[4] -> P0[22]" },
    { FCTPIN_P1_8,          FCTPIN_P0_17,               "P1[8] -> P0[17]" },
    { FCTPIN_P1_9,          FCTPIN_P0_16,               "P1[9] -> P0[16]" },
    { FCTPIN_P1_10,         FCTPIN_P0_15,               "P1[10] -> P0[15]" },
    { FCTPIN_P1_14,         FCTPIN_P0_30,               "P1[14] -> P0[30]" },
    { FCTPIN_P1_15,         FCTPIN_P0_29,               "P1[15] -> P0[29]" },
    { FCTPIN_P1_16,         FCTPIN_P4_26|FCTPIN_P0_10,  "P1[16] -> P4[26]/P0[10]" },
    { FCTPIN_P1_17,         FCTPIN_P4_27,               "P1[17] -> P4[27]" },
//    { FCTPIN_P1_31,         FCTPIN_P1_19,               "P1[31] -> P1[19]" },  // Sharp DG28
    //{ FCTPIN_P0_26,         FCTPIN_P0_6|FCTPIN_P0_9,    "P0[26] -> P0[6]/P0[9]" },
    { FCTPIN_P0_4,          FCTPIN_P0_8,                "P0[4] -> P0[8]" },
    { FCTPIN_P0_5,          FCTPIN_P0_7,                "P0[5] -> P0[7]" },
    //{ FCTPIN_P0_6,          FCTPIN_P0_9|FCTPIN_P0_26,   "P0[6] -> P0[9]/P0[26]" },
    { FCTPIN_P0_7,          FCTPIN_P0_5,                "P0[7] -> P0[5]" },
    { FCTPIN_P0_8,          FCTPIN_P0_4,                "P0[8] -> P0[4]" },
    //{ FCTPIN_P0_9,          FCTPIN_P0_6|FCTPIN_P0_26,   "P0[6] -> P0[26]/P0[9]" },
//    { FCTPIN_P1_19,         FCTPIN_P1_31,               "P1[19] -> P1[31]" }, // Sharp DG28
    { FCTPIN_P4_27,         FCTPIN_P1_17,               "P4[27] -> P1[17]" },
    { FCTPIN_P4_26,         FCTPIN_P0_10|FCTPIN_P1_16,  "P4[26] -> P0[10]/P1[16]" },
//    { FCTPIN_P0_29,         FCTPIN_P1_15,               "P0[29] -> P1[15]" },
//    { FCTPIN_P0_30,         FCTPIN_P1_14,               "P0[30] -> P1[14]" },
    { FCTPIN_P0_15,         FCTPIN_P1_10,               "P0[15] -> P1[10]" },
    { FCTPIN_P0_16,         FCTPIN_P1_9,                "P0[16] -> P1[9]" },
    { FCTPIN_P0_17,         FCTPIN_P1_8,                "P0[17] -> P1[8]" },
    { FCTPIN_P0_22,         FCTPIN_P1_4,                "P0[22] -> P1[4]" },
    { FCTPIN_P0_19,         FCTPIN_P1_1,                "P0[19] -> P1[1]" },
    { FCTPIN_P0_20,         FCTPIN_P1_0,                "P0[20] -> P1[0]" },
//    { FCTPIN_P0_10,         FCTPIN_P0_3,                "P0[10] -> P0[3]" },
//    { FCTPIN_P0_11,         FCTPIN_P0_2,                "P0[11] -> P0[2]" },

    // Secondary Connector:
    //{ FCTPIN_P5_4,          FCTPIN_P0_25,               "P5[4] -> P0[25]"},
    //{ FCTPIN_P5_3,          FCTPIN_P0_12,               "P5[3] -> P0[12]"},
    //{ FCTPIN_P5_2,          FCTPIN_P0_13,               "P5[2] -> P0[13]"},
    { FCTPIN_P1_12,         FCTPIN_P0_0,                "P1[12] -> P0[0]"},
    { FCTPIN_P1_11,         FCTPIN_P0_1,                "P1[11] -> P0[1]"},
    { FCTPIN_P1_7,          FCTPIN_P1_3,                "P1[7] -> P1[3]"},
    { FCTPIN_P1_6,          FCTPIN_P1_5,                "P1[6] -> P1[5]"},
    { FCTPIN_P1_5,          FCTPIN_P1_6,                "P1[5] -> P1[6]"},
    { FCTPIN_P1_3,          FCTPIN_P1_7,                "P1[3] -> P1[7]"},
    { FCTPIN_P0_1,          FCTPIN_P1_11,               "P0[1] -> P1[11]"},
    { FCTPIN_P0_0,          FCTPIN_P1_12,               "P0[0] -> P1[12]"},
    //{ FCTPIN_P0_13,         FCTPIN_P5_2,                "P0[13] -> P5[2]"},
    //{ FCTPIN_P0_12,         FCTPIN_P5_3,                "P0[12] -> P5[3]"},
    //{ FCTPIN_P0_25,         FCTPIN_P5_4,                "P0[25] -> P5[4]"}
};

#else
static const T_gpioMapping G_gpioMapping[] = {
    #define FCTPIN_P1_4            (1<<0)
    { 1,    4, "P1[4]" },
    #define FCTPIN_P1_8            (1<<1)
    { 1,    8, "P1[8]" },
    #define FCTPIN_P1_9            (1<<2)
    { 1,    9, "P1[9]" },
    #define FCTPIN_P1_10           (1<<3)
    { 1,   10, "P1[10]" },
    #define FCTPIN_P1_14           (1<<4)
    { 1,   14, "P1[14]" },
    #define FCTPIN_P1_15           (1<<5)
    { 1,   15, "P1[15]" },
    #define FCTPIN_P1_16           (1<<6)
    { 1,   16, "P1[16]" },
    #define FCTPIN_P1_17           (1<<7)
    { 1,   17, "P1[17]" },
    #define FCTPIN_P1_31           (1<<8)
    { 1,   31, "P1[31]" },
    #define FCTPIN_P0_26           (1<<9)
    { 0,   26, "P0[26]" },
    #define FCTPIN_P0_4            (1<<10)
    { 0,    4, "P0[4]" },
    #define FCTPIN_P0_5            (1<<11)
    { 0,    5, "P0[5]" },
    #define FCTPIN_P0_6            (1<<12)
    { 0,    6, "P0[6]" },
    #define FCTPIN_P0_7            (1<<13)
    { 0,    7, "P0[7]" },
    #define FCTPIN_P0_8            (1<<14)
    { 0,    8, "P0[8]" },
    #define FCTPIN_P0_9            (1<<15)
    { 0,    9, "P0[9]" },
   #define FCTPIN_P1_19           (1<<16)
    { 1,   19, "P1[19]" },
    #define FCTPIN_P4_27           (1<<17)
    { 4,   27, "P4[27]" },
    #define FCTPIN_P4_26           (1<<18)
    { 4,   26, "P4[26]" },
    #define FCTPIN_P0_29           (1<<19)
    { 0,   29, "P0[29]" },
    #define FCTPIN_P0_30           (1<<20)
    { 0,   30, "P0[30]" },
    #define FCTPIN_P4_22           (1<<21)
    { 4,   22, "P4[22]" },
    #define FCTPIN_P4_23           (1<<22)
    { 4,   23, "P4[23]" },
    #define FCTPIN_P4_20           (1<<23)
    { 4,   20, "P4[20]" },
    #define FCTPIN_P4_21           (1<<24)
    { 4,   21, "P4[21]" },
    #define FCTPIN_P5_4             (((TUInt64)1)<<25)
    { 5,    4, "P5[4]" },
    #define FCTPIN_P5_3             (((TUInt64)1)<<26)
    { 5,    3, "P5[3]" },
    #define FCTPIN_P5_2             (((TUInt64)1)<<27)
    { 5,    2, "P5[2]" },
    #define FCTPIN_P0_13            (((TUInt64)1)<<28)
    { 0,   13, "P0[13]" },
    #define FCTPIN_P0_12            (((TUInt64)1)<<29)
    { 0,   12, "P0[12]" },
    #define FCTPIN_P0_25            (((TUInt64)1)<<30)
    { 0,   25, "P0[25]" },
};

const T_gpioTestEntry G_gpioTestArray[] = {
//    { FCTPIN_P0_2,          FCTPIN_P0_11,               "P0[2] -> P0[11]" },
//    { FCTPIN_P0_3,          FCTPIN_P0_10,               "P0[3] -> P0[10]" },
//    { FCTPIN_P0_10,         FCTPIN_P1_16|FCTPIN_P4_26,  "P0[10] -> P1[16]/P4[26]" },
//    { FCTPIN_P1_0,          FCTPIN_P0_20,               "P1[0] -> P0[20]" },
//    { FCTPIN_P1_1,          FCTPIN_P0_19,               "P1[1] -> P0[19]" },
//    { FCTPIN_P1_4,          FCTPIN_P4_21,               "P1[4] -> P4[21]" },
    { FCTPIN_P1_8,          FCTPIN_P4_20,               "P1[8] -> P4[20]" },
    { FCTPIN_P1_9,          FCTPIN_P4_23,               "P1[9] -> P4[23]" },
    { FCTPIN_P1_10,         FCTPIN_P4_22,               "P1[10] -> P4[22]" },
//    { FCTPIN_P1_14,         FCTPIN_P0_30,               "P1[14] -> P0[30]" },
//    { FCTPIN_P1_15,         FCTPIN_P0_29,               "P1[15] -> P0[29]" },
    { FCTPIN_P1_16,         FCTPIN_P4_26,               "P1[16] -> P4[26]" },
    { FCTPIN_P1_17,         FCTPIN_P4_27,               "P1[17] -> P4[27]" },
    { FCTPIN_P1_31,         FCTPIN_P1_19,               "P1[31] -> P1[19]" },  // Sharp DG28
//    { FCTPIN_P0_26,         FCTPIN_P0_6|FCTPIN_P0_9,    "P0[26] -> P0[6]/P0[9]" },
    { FCTPIN_P0_4,          FCTPIN_P0_8,                "P0[4] -> P0[8]" },
//    { FCTPIN_P0_5,          FCTPIN_P0_7,                "P0[5] -> P0[7]" },
//    { FCTPIN_P0_6,          FCTPIN_P0_9|FCTPIN_P0_26,   "P0[6] -> P0[9]/P0[26]" },
    { FCTPIN_P0_7,          FCTPIN_P0_5,                "P0[7] -> P0[5]" },
    { FCTPIN_P0_8,          FCTPIN_P0_4,                "P0[8] -> P0[4]" },
//    { FCTPIN_P0_9,          FCTPIN_P0_6|FCTPIN_P0_26,   "P0[6] -> P0[26]/P0[9]" },
    { FCTPIN_P1_19,         FCTPIN_P1_31,               "P1[19] -> P1[31]" },
    { FCTPIN_P4_27,         FCTPIN_P1_17,               "P4[27] -> P1[17]" },
    { FCTPIN_P4_26,         FCTPIN_P1_16,               "P4[26] -> P1[16]" },
//    { FCTPIN_P0_29,         FCTPIN_P1_15,               "P0[29] -> P1[15]" },
//    { FCTPIN_P0_30,         FCTPIN_P1_14,               "P0[30] -> P1[14]" },
    { FCTPIN_P4_22,         FCTPIN_P1_10,               "P4[22] -> P1[10]" },
    { FCTPIN_P4_23,         FCTPIN_P1_9,                "P4[23] -> P1[9]" },
    { FCTPIN_P4_20,         FCTPIN_P1_8,                "P4[20] -> P1[8]" },
//    { FCTPIN_P4_21,         FCTPIN_P1_4,                "P4[21] -> P1[4]" },
//    { FCTPIN_P0_19,         FCTPIN_P1_1,                "P0[19] -> P1[1]" },
//    { FCTPIN_P0_20,         FCTPIN_P1_0,                "P0[20] -> P1[0]" },
//    { FCTPIN_P0_10,         FCTPIN_P0_3,                "P0[10] -> P0[3]" },
//    { FCTPIN_P0_11,         FCTPIN_P0_2,                "P0[11] -> P0[2]" },

     //Secondary Connector:
//    { FCTPIN_P5_4,          FCTPIN_P0_25,               "P5[4] -> P0[25]"},
//    { FCTPIN_P5_3,          FCTPIN_P0_12,               "P5[3] -> P0[12]"},
    { FCTPIN_P5_2,          FCTPIN_P0_13,               "P5[2] -> P0[13]"},
//    { FCTPIN_P1_12,         FCTPIN_P0_0,                "P1[12] -> P0[0]"},
//    { FCTPIN_P1_11,         FCTPIN_P0_1,                "P1[11] -> P0[1]"},
//    { FCTPIN_P1_7,          FCTPIN_P1_3,                "P1[7] -> P1[3]"},
//    { FCTPIN_P1_6,          FCTPIN_P1_5,                "P1[6] -> P1[5]"},
//    { FCTPIN_P1_5,          FCTPIN_P1_6,                "P1[5] -> P1[6]"},
//    { FCTPIN_P1_3,          FCTPIN_P1_7,                "P1[3] -> P1[7]"},
//    { FCTPIN_P0_1,          FCTPIN_P1_11,               "P0[1] -> P1[11]"},
//    { FCTPIN_P0_0,          FCTPIN_P1_12,               "P0[0] -> P1[12]"},
    { FCTPIN_P0_13,         FCTPIN_P5_2,                "P0[13] -> P5[2]"},
//    { FCTPIN_P0_12,         FCTPIN_P5_3,                "P0[12] -> P5[3]"},
//    { FCTPIN_P0_25,         FCTPIN_P5_4,                "P0[25] -> P5[4]"},
};
#endif
HAL_GPIOPort **PortNumToPort(TUInt8 aPort)
{
    HAL_GPIOPort **p=0;

    if (aPort == 0) {
        HALInterfaceFind("GPIO0", (T_halWorkspace **)&p);
    } else if (aPort == 1) {
        HALInterfaceFind("GPIO1", (T_halWorkspace **)&p);
    } else if (aPort == 2) {
        HALInterfaceFind("GPIO2", (T_halWorkspace **)&p);
    } else if (aPort == 3) {
        HALInterfaceFind("GPIO3", (T_halWorkspace **)&p);
    } else if (aPort == 4) {
        HALInterfaceFind("GPIO4", (T_halWorkspace **)&p);
    } else if (aPort == 5) {
        HALInterfaceFind("GPIO5", (T_halWorkspace **)&p);
    }

    return p;
}

void FuncTestGPIOs(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
        TUInt32 iState;
        TUInt32 iStart;
    } T_gpioTest;
    T_gpioTest *p = (T_gpioTest *)aData->iData;
    TUInt32 reading;
    const T_gpioTestEntry *p_test;
    HAL_GPIOPort **p_port;
    const T_gpioMapping *p_pin;
    TUInt64 pre;
    TUInt64 post;
    TUInt64 diff;
    TUInt64 pin;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking GPIOs ... ");

        // Read the tick counter
//        p->iStart = UEZTickCounterGet();

        // Setup all the port pins to be MUX'd for GPIO and input only
        for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {

            p_pin = G_gpioMapping+pin;
            sprintf(p->iLine, "  Prep %s", p_pin->iText);
            aAPI->iTextLine(aData, 1, p->iLine);
            UEZTaskDelay(10);
            p_port = PortNumToPort(p_pin->iMapPort);
            (*p_port)->SetPull(p_port, p_pin->iMapPin, GPIO_PULL_UP); // set to Pull up
            (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Input
            (*p_port)->SetMux(p_port, p_pin->iMapPin, 0); // set to GPIO
        }

        p->iState = 0;
    } else if (aButton == 0) {
        // Try one each time through this loop
        if (p->iState < ARRAY_COUNT(G_gpioTestArray)) {
            p_test = &G_gpioTestArray[p->iState++];
            sprintf(p->iLine, "  Testing %s", p_test->iMsg);
            aAPI->iTextLine(aData, 1, p->iLine);
            sprintf(aData->iResultValue, "%s", p_test->iMsg);

            // Go through and setup the pins for GPIO output on the drive pin
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                if (p_test->iPinsDrive & (((TUInt64)1)<<pin)) {
                    // Drive 1 on this pin
                    (*p_port)->Set(p_port, 1<<p_pin->iMapPin); // set to 1
                    (*p_port)->SetOutputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                } else {
                    (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                    (*p_port)->SetPull(p_port, 1<<p_pin->iMapPin, GPIO_PULL_UP);
                }
            }

            UEZTaskDelay(10);

            // Now, read all the pins and create a single word showing if pins are
            // high or low
            pre = 0;
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                (*p_port)->Read(p_port, 1<<p_pin->iMapPin, &reading);
                if (reading)
                    pre |= (((TUInt64)1)<<pin);
            }

            // Now drive it to a 0
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                if (p_test->iPinsDrive & (((TUInt64)1)<<pin)) {
                    // Drive 0 on this pin
                    (*p_port)->Clear(p_port, 1<<p_pin->iMapPin); // set to 1
                    (*p_port)->SetOutputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                } else {
                    (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                    (*p_port)->SetPull(p_port, 1<<p_pin->iMapPin, GPIO_PULL_UP);
                }
            }

            UEZTaskDelay(10);

            // Now, read all the pins and create a single word showing if pins are
            // high or low
            post = 0;
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                (*p_port)->Read(p_port, 1<<p_pin->iMapPin, &reading);
                if (reading)
                    post |= (((TUInt64)1)<<pin);
            }

            // What pins changed?
            diff = pre ^ post;

            // Which pins were we testing (both driving and receiving were tested,
            // all other should not have changed!)
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                if ((p_test->iPinsDrive & (((TUInt64)1)<<pin)) || (p_test->iPinsChange & (((TUInt64)1)<<pin))) {
                    // This pin should have changed
                    if (!(diff & (((TUInt64)1)<<pin))) {
                        // This pin did not change when it should have
                        sprintf(p->iLine, "  %s stuck %s", p_pin->iText, (pre & (((TUInt64)1)<<pin))?"HIGH":"LOW");
                        aAPI->iTextLine(aData, 2, p->iLine);
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        break;
                    }
                } else {
                    // This pin should NOT have changed
                    if (diff & (((TUInt64)1)<<pin)) {
                        // This pin changed when it should not have
                        sprintf(p->iLine, "  %s toggled!", p_pin->iText);
                        aAPI->iTextLine(aData, 2, p->iLine);
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        break;
                    }
                }
            }
        } else {
            // Got to the end, must have passed
            aAPI->iTextLine(aData, 1, "  GPIO Testing Complete.");
            aAPI->iTextLine(aData, 2, "  All GPIOs lines passed.");
            sprintf(aData->iResultValue, "OK");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        }
    }
}

/* Test that each page of the flash memory can be written to/read from */
TUInt32 NORFlashFullMemoryTest(
        const T_testAPI *aAPI,
        T_testData *aData,
        T_uezDevice dev,
        T_FlashChipInfo info)
{
    typedef struct {
        char iLine[80];
    } T_norFlashTest;
    T_uezError error;
    TUInt32 reg;
    TUInt8 flip;
    T_norFlashTest *p = (T_norFlashTest *)aData->iData;
    TUInt32 passNumber;
    TUInt32 region;
    TUInt32 blockInRegion;
    TUInt32 offset;
    TUInt32 byteOffset;
	int actualBlockSize;
    int blockSize = 0;
    unsigned char *pBlockMemory = 0;

    aAPI->iTextLine(aData, 1, "  Checking QRY");

    // Start with determining if there is a 'QRY' set of characters
    // If the following fails, report "Not Found" error
    sprintf(aData->iResultValue, "Not Found");
    error = UEZFlashQueryReg(dev, 0x10, &reg);
    if (error || reg != 'Q')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x11, &reg);
    if (error || reg != 'R')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x12, &reg);
    if (error || reg != 'Y')
        return 1001;

    for (flip=0; flip<2; flip++)
    {
        //NOTE: Probably better to erase on a per-block basis to test the block erase
        //      is working correctly

         sprintf(p->iLine, "  flip %d)", flip);
         aAPI->iTextLine(aData, 1, p->iLine);
#if 0
        // Start with a good ol' fashion chip erase
         sprintf(p->iLine, "  Chip Erase (flip %d)", flip);
         aAPI->iTextLine(aData, 1, p->iLine);
        error = UEZFlashChipErase(dev);
        if (error)
            return 1009;
#endif
        // passNumber == 0 write pass
        // passNumber == 1 read pass
        for (passNumber = 0; passNumber < 2; passNumber++)
        {
            // flash address
            int address = 0;
            int overallBlock = 0;

            sprintf(p->iLine, "  passNumber %d", passNumber);
            aAPI->iTextLine(aData, 1, p->iLine);

            for (region = 0; region < info.iNumRegions; region++)
            {

// Temporarily override this
//info.iRegions[region].iNumEraseBlocks = 128-2;
               sprintf(p->iLine, "  region %d", region);
               aAPI->iTextLine(aData, 1, p->iLine);

               // ensure that our memory is the correct size
                actualBlockSize = info.iRegions[region].iSizeEraseBlock;
                if(blockSize != actualBlockSize)
                {
                    blockSize = actualBlockSize;
                    pBlockMemory = (unsigned char*)0xA0400000;

                    sprintf(p->iLine, "  pBlockMemory %p, blockSize %d", pBlockMemory, blockSize);
                    aAPI->iTextLine(aData, 3, p->iLine);
                }

                for (blockInRegion = 0; blockInRegion < info.iRegions[region].iNumEraseBlocks; blockInRegion++)
                {
                    int expectedValue;
                    if(flip == 0)
                    {
                        expectedValue = overallBlock;
                    } else
                    {
                        // invert bits
                        expectedValue = overallBlock ^ 0xFF;
                    }

                    // perform the write if passNumber == 0 or the read/compare if passNumber == 1
                    if(passNumber == 0)
                    {
                        sprintf(p->iLine, "  passNumber %d, overallBlock %d (WRITING)", passNumber, overallBlock);
                        aAPI->iTextLine(aData, 1, p->iLine);
                        sprintf(aData->iResultValue, "0x%08X (Writing)", address);

                        // fill the memory with the appropriate value
                        for (offset = 0; offset < blockSize; offset++)
                        {
                            pBlockMemory[offset] = expectedValue;
                        }

                        // erase the block
                        if (UEZFlashBlockErase(dev, address, blockSize))
                            return 10056;

                        // write the block
                        if (UEZFlashWrite(dev, address, pBlockMemory, blockSize))
                            return 10057;
                    } else
                    {
                        sprintf(p->iLine, "  passNumber %d, overallBlock %d (READING)", passNumber, overallBlock);
                        aAPI->iTextLine(aData, 1, p->iLine);
                        sprintf(aData->iResultValue, "0x%08X (Reading)", address);

                        // read the block into the block memory buffer
                        UEZFlashRead(dev, address, pBlockMemory, blockSize);

                        // compare bytes
                        for (byteOffset = 0; byteOffset < blockSize; byteOffset++)
                        {
                            if(pBlockMemory[byteOffset] != expectedValue)
                            {
                                sprintf(p->iLine, "  overallBlock %d, byteOffset %d expected 0x%x, got 0x%x",
                                        overallBlock, byteOffset, expectedValue, pBlockMemory[byteOffset]);
                                aAPI->iTextLine(aData, 2, p->iLine);
                                return 1025;
                            }
                        }
                    }

                    overallBlock++;

                    // advance the address
                    address += blockSize;

                } // foreach(blockInRegion)
            } // foreach(region)
        } // foreach(passNumber)
    } // foreach(flip)

    // Got here with no errors, good!
    sprintf(aData->iResultValue, "OK");
    return 0;
}

/* Full flash memory test */
void FuncTestFullFlash0(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 errorCode;
    T_uezDevice dev;
    T_FlashChipInfo info;

    if (aButton == OPT_INIT) {
        // Just do it quickly and size it
        aAPI->iTextLine(aData, 0, "Full check of NOR Flash Memory ...");
        if (UEZFlashOpen("Flash0", &dev)) {
            errorCode = 1000;
        } else {
            UEZFlashGetChipInfo(dev, &info);
            errorCode = NORFlashFullMemoryTest(aAPI, aData, dev, info);
            UEZFlashClose(dev);
        }

        if (errorCode==0) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        } else {
            sprintf(p->iLine, "  Error Code %d!", errorCode & 0x7FFF);
            aAPI->iTextLine(aData, 2, p->iLine);
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        }
    }
}

void FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    TUInt32 i=3;
    const T_testState *p;
    char *line = (char *)G_td.iData;
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
            for (p=G_testStates; p->iMonitorFunction != FuncTestComplete; p++) {
                if (G_results[p-G_testStates] == TEST_RESULT_FAIL) {
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
            for (p=G_testStates; p->iMonitorFunction != FuncTestComplete; p++) {
                if (G_results[p-G_testStates] == TEST_RESULT_SKIP) {
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

static const struct { char *iText; T_pixelColor iColor; } G_textResults[] = {
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

static const T_button G_testButtons[] = {
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

void TestDrawButtons(
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
    for (p_button=G_testButtons; p_button->iBit; p_button++)  {
        if (aButtons & p_button->iBit) {
            // Draw box button
            swim_set_fill_transparent(&G_td.iWin, 0);
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

void TestButtonClick(T_testData *aData, TUInt16 aButton)
{
    TestDrawButtons(aData, aButton, TEST_COLOR_BUTTON_CLICK);
    ButtonClick();
    UEZTaskDelay(100);
    TestDrawButtons(aData, aButton, TEST_COLOR_BACKGROUND);
}

void TestAddButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    TUInt16 newButtons = aButtonTypes ^ aData->iButtons;

    TestDrawButtons(aData, newButtons, TEST_COLOR_BACKGROUND);

    aData->iButtons |= aButtonTypes;
}

void TestRemoveButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    const T_button *p_button;
    TUInt16 buttons = aData->iButtons & aButtonTypes;

    // Erase the buttons
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td.iWin, 0);

    // Go through and remove buttons
    for (p_button=G_testButtons; p_button->iBit; p_button++)  {
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

void TestTextLine(T_testData *aData, TUInt16 aLine, const char *aText)
{
    int mode = 0;
    const char *p_text;
    TUInt16 y = ICalcY(aLine+aData->iLine);

    // Draw a black rectangle over the area
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td.iWin, 0);

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

void TestShowResult(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay)
{
    // Draw the result on the screen
    swim_set_pen_color(&aData->iWin, G_textResults[aResult].iColor);
    swim_put_text_xy(
        &aData->iWin,
        G_textResults[aResult].iText,
        TEST_RESULT_RIGHT_COLUMN,
        ICalcY(aLine+aData->iLine));

    // Show for a moment, then go on
    UEZTaskDelay(aDelay);
}

void TestSetTestResult(T_testData *aData, TUInt16 aPass)
{
    aData->iResult = aPass;
    if ((aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))  {
        // If any test is failed or skipped, not an 'all pass'
        G_td.iAllPass = EFalse;
    }
    if ((aPass == TEST_RESULT_PASS)||(aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))
        aData->iGoNext = ETrue;
}

void TestNext(T_testData *aData)
{
    aData->iGoNext = ETrue;
}

const T_testAPI G_testAPI = {
    TestAddButtons,
    TestRemoveButtons,
    TestTextLine,
    TestShowResult,
    TestSetTestResult,
    TestNext,
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
void FunctionalTest(const T_choice *aChoice)
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
    const T_testState *testState = &G_testStates[0];
    const T_testState *lastTestState = 0;
    TBool isCancelled = EFalse;
    TBool isPausing = EFalse;
    TBool haveLoopback = UEZGUIIsLoopbackBoardConnected();
#if ENABLE_UEZ_BUTTON
    T_uezDevice keypadDevice;
#endif
    
    // Start with the first test
    testState = G_testStates;

    memset(&G_td, 0, sizeof(G_td));

    // All tests pass until otherwise noted
    G_td.iAllPass = ETrue;
    G_td.iPauseComplete = EFalse;

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
                FunctionalTestScreen(&G_td);

                for (;(testState->iTitle) && !isCancelled;) {
                    if ((testState->iIgnoreIfExpansionBoard)) {
                        G_results[testState-G_testStates] = TEST_RESULT_IGNORE;
                        testState++;
                        continue;
                    }

                    if ((testState->iIgnoreIfNoLoopbackBoard) && (!haveLoopback)) {
                        G_results[testState-G_testStates] = TEST_RESULT_IGNORE;
                        testState++;
                        continue;
                    }

                    // New test state?
                    if (testState != lastTestState) {
                        // Check that we have room for these lines
                        if ((G_td.iLine + testState->iLinesNeeded) >= TEST_RESULT_NUM_LINES) {
                            TUInt32 i;
                            // Back to the start
                            G_td.iLine = 0;
                            // Clear all the lines
                            for (i=0; i<TEST_RESULT_NUM_LINES; i++)
                                G_testAPI.iTextLine(&G_td, i, 0);
                        }

                        // Remove all buttons but Skip and Cancel
                        TestRemoveButtons(&G_td, 0xFFFF&~(OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL));
                        TestAddButtons(&G_td, OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL);
                        testState->iMonitorFunction(&G_testAPI, &G_td, OPT_INIT);
                        lastTestState = testState;
                    }

                    // Wait forever until we receive a touchscreen event
                    // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                    // Check every 50 ms
                    if (UEZQueueReceive(queue, &inputEvent, 10)==UEZ_ERROR_NONE) {
                        winX = inputEvent.iEvent.iXY.iX;
                        winY = inputEvent.iEvent.iXY.iY;
                        swim_get_virtual_xy(&G_td.iWin, &winX, &winY);

                        // Is this a touching event?
                        if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD)  {
                            // We are touching the screen.
                            // Is this a different position than before?
                            if ((inputEvent.iEvent.iXY.iX != lastX) || (inputEvent.iEvent.iXY.iY != lastY))  {
                                x = inputEvent.iEvent.iXY.iX;
                                y = inputEvent.iEvent.iXY.iY;
                                // Determine which choice we are in
                                for (p_button=G_testButtons; p_button->iBit; p_button++)  {
                                    if ((x >= p_button->iX1) && (x <= p_button->iX2) &&
                                            (y >= p_button->iY1) && (y <= p_button->iY2)) {
                                        break;
                                    }
                                }
                                if (p_button != p_lastButton) {
                                    // See if we got a button
                                    if (p_button->iBit & G_td.iButtons) {
                                        // Click it
                                        TestButtonClick(&G_td, p_button->iBit);

                                        // Now process the command
                                        if (isPausing) {
                                            if (p_button->iBit & OPT_BUTTON_OK) {
                                                // Clicked OK, just go on, already have a failure
                                                G_td.iGoNext = ETrue;
                                                G_td.iPauseComplete = ETrue;
                                            }
                                        } else {
                                            testState->iMonitorFunction(&G_testAPI, &G_td, p_button->iBit);
                                            if (p_button->iBit & OPT_BUTTON_SKIP) {
                                                G_td.iGoNext = ETrue;
                                                G_testAPI.iShowResult(&G_td, 0, TEST_RESULT_SKIP, 250);
                                                G_testAPI.iSetTestResult(&G_td, TEST_RESULT_SKIP);

                                                // If any test is skipped, does not pass
                                                G_td.iAllPass = EFalse;
                                            }
                                        }
                                        if (p_button->iBit & OPT_BUTTON_CANCEL) {
                                            // If any test is cancelled, does not pass
                                            G_td.iAllPass = EFalse;
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
                    if (G_td.iGoNext) {
                        // Did we fail?  And do we need to pause?
                        if ((G_td.iResult == TEST_RESULT_FAIL) && (testState->iPauseOnFail)) {
                            // Did we pause?
                            if (!G_td.iPauseComplete) {
                                // Have not paused, put up the OK and Cancel buttons
                                TestRemoveButtons(&G_td, 0xFFFF&~(OPT_BUTTON_CANCEL));
                                TestAddButtons(&G_td, OPT_BUTTON_OK|OPT_BUTTON_CANCEL);

                                // We're not done yet
                                G_td.iGoNext = EFalse;

                                // And we're in pause mode
                                isPausing = ETrue;

                                // Play negative tone
                                BeepError();
                            } else {
                                // Done pausing, reset everything
                                G_td.iPauseComplete = EFalse;
                                isPausing = EFalse;
                            }
                        }
                    }
                    // Still going on?
                    if (G_td.iGoNext) {
                        // Record the result
                        G_results[testState-G_testStates] = G_td.iResult;

                        // Move to the next lines
                        G_td.iLine+=testState->iLinesNeeded;
                        testState++;
                        G_td.iGoNext = EFalse;
                    } else {
                        // Only monitor if we are not pausing
                        if (!isPausing)
                            testState->iMonitorFunction(&G_testAPI, &G_td, 0);
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

void FuncTestEXP1(void){ // uEZGUI-EXP1 functional test
#include <uEZStream.h>
	//EXP1 test serial input start
    T_uezDevice serial, serial2;
    char byte, bytein;
	T_uezError error = UEZ_ERROR_NONE;
    HAL_GPIOPort **iDataEnablePort;
    TUInt32 iDataEnableBit;
    HAL_GPIOPort **iReceiveEnablePort;
    TUInt32 iReceiveEnableBit;
    printf("\f");
    printf("Press any letter key to start\n");
    UEZStreamOpen("Console", &serial);
    UEZStreamRead(serial, (void*) &byte, 1, NULL, UEZ_TIMEOUT_INFINITE);
    printf("Is this the key you pressed? (y for yes n for no)    ");
    UEZStreamWrite(serial, (void*) &byte, 1, NULL, UEZ_TIMEOUT_INFINITE);
    printf("\n");

    UEZStreamRead(serial, (void*) &byte, 1, NULL, UEZ_TIMEOUT_INFINITE);

    if(byte == 'y') printf("Pass\n");
    else{
        printf("This board has failed the serial input test");
        while(1);
    }

    UEZGPIOSetMux(GPIO_P0_17, 0); // set to GPIO mode
    UEZGPIOSetMux(GPIO_P0_22, 0);
    // set to ouput mode
    error = HALInterfaceFind("GPIO0", (T_halWorkspace **)&iReceiveEnablePort);
    iReceiveEnableBit = (1UL<<17); // P0[17]
    error = HALInterfaceFind("GPIO0", (T_halWorkspace **)&iDataEnablePort);
    iDataEnableBit = (1UL<<22);    // P0[22]
    if(error){while(1);} // if either errors then will stay stuck in loop
    (*iReceiveEnablePort)->SetOutputMode(iReceiveEnablePort, iReceiveEnableBit);
    (*iDataEnablePort)->SetOutputMode(iDataEnablePort, iDataEnableBit);

    (*iReceiveEnablePort)->Clear(iReceiveEnablePort, iReceiveEnableBit); // pin 17 low
    (*iDataEnablePort)->Set(iDataEnablePort, iDataEnableBit);            // pin 22 high
    UEZStreamOpen("UART1", &serial2);

    UEZStreamWrite(serial2, (void*) &byte, 1, NULL, 500);//UEZ_TIMEOUT_INFINITE);
    bytein = '\0';
    UEZStreamRead(serial2, (void*) &bytein, 1, NULL, 500);//UEZ_TIMEOUT_INFINITE);
    if( bytein != byte) {
        printf("Failed RS485 test, confirm the RS485 adapter is connected.\n");
        printf("If the adapter is connected then the board has failed.\n");
        while(1);
    }
    printf("Pass RS485\n");
    UEZTaskDelay(1000);
    UEZStreamClose(serial2);
}

void FunctionalTestScreen(T_testData *aData)
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

void FuncTestPageHit(void)
{
}

/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.c
 *-------------------------------------------------------------------------*/
