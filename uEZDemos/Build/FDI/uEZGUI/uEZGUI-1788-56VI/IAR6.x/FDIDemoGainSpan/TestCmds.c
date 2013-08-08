/*-------------------------------------------------------------------------*
 * File:  UEZGUITestCmds.c
 *-------------------------------------------------------------------------*
 * Description:
 *      UEZGUI Tester command console
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2010 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 *     -   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 *     -   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *     -   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *     NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZStream.h>
#include <uEZSPI.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include "FuncTestFramework.h"
#include "FunctionalTest.h"
#include "AppDemo.h"
#include <HAL/GPIO.h>
#include <uEZToneGenerator.h>
#include <Device/ToneGenerator.h>
#include "NVSettings.h"
#include <uEZToneGenerator.h>
#if UEZ_ENABLE_AUDIO_AMP
#include <uEZAudioAmp.h>
#endif
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include <uEZLCD.h>
#include "UEZGainSpan.h"

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void UEZGUITestAddButtons(T_testData *aData, TUInt16 aButtonTypes);
void UEZGUITestRemoveButtons(T_testData *aData, TUInt16 aButtonTypes);
void UEZGUITestTextLine(T_testData *aData, TUInt16 aLine, const char *aText);
void UEZGUITestShowResult(
        T_testData *aData,
        TUInt16 aLine,
        TUInt16 aResult,
        TUInt32 aDelay);
void UEZGUITestSetTestResult(T_testData *aData, TUInt16 aResult);
void UEZGUITestNextTest(T_testData *aData);

int UEZGUICmdPing(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdSDRAM(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdNOR(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd5V(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd3VERR(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEEPROM(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort2(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdBacklight(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdLCD(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreen(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreenStatus(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreenClear(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdVerbose(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdColorCycle(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdColor(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdColorCycle(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdAlignmentBorder(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEnd(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdSDCard(void *aWorkspace, int argc, char *argv[]);
#if LIGHT_SENSOR_ENABLED
int UEZGUICmdLightSensor(void *aWorkspace, int argc, char *argv[]);
#endif
int UEZGUICmdAccelerometer(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdSpeaker(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdAmplifier(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdMACAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPMaskAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPGatewayAddress(void *aWorkspace, int argc, char *argv[]);
extern T_uezError UEZToneGeneratorOpen(
            const char * const aName,
            T_uezDevice *aDevice);
extern T_uezError UEZToneGeneratorPlayToneContinuous(
            T_uezDevice aDevice,
            TUInt32 aTonePeriod);
int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdRelay(void *aWorkspace, int argc, char *argv[]);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static const T_consoleCmdEntry G_UEZGUICommands[] = {
        { "PING", UEZGUICmdPing },
        { "SDRAM", UEZGUICmdSDRAM },
        { "NOR", UEZGUICmdNOR },
        { "GPIO", UEZGUICmdGPIO },
        { "5V", UEZGUICmd5V },
        { "3VERR", UEZGUICmd3VERR },
        { "EEPROM", UEZGUICmdEEPROM },
        { "TEMP", UEZGUICmdTemperature },
        { "SDCARD", UEZGUICmdSDCard },
#if LIGHT_SENSOR_ENABLED
        { "LIGHTSENSOR", UEZGUICmdLightSensor },
#endif
        { "USB1", UEZGUICmdUSBPort1 },
        { "USB2", UEZGUICmdUSBPort2 },
        { "BL", UEZGUICmdBacklight },
        { "LCD", UEZGUICmdLCD },
        { "TS", UEZGUICmdTouchscreen },
        { "TSSTATUS", UEZGUICmdTouchscreenStatus },
        { "TSCLEAR", UEZGUICmdTouchscreenClear },
        { "VERBOSE", UEZGUICmdVerbose },
        { "COLOR", UEZGUICmdColor },
        { "COLORCYCLE", UEZGUICmdColorCycle },
        { "ALIGN", UEZGUICmdAlignmentBorder },
        { "ACCEL", UEZGUICmdAccelerometer },
        { "SPEAKER", UEZGUICmdSpeaker },
#if 0
        { "AMP", UEZGUICmdAmplifier },
#endif
        { "MACADDR", UEZGUICmdMACAddress },
        { "IPADDR", UEZGUICmdIPAddress },
        { "IPMASK", UEZGUICmdIPMaskAddress },
        { "IPGATEWAY", UEZGUICmdIPGatewayAddress },
        { "GAINSPAN", UEZGUICmdGainSpan },
        { "RELAY", UEZGUICmdRelay },
        { "END", UEZGUICmdEnd },
        { 0, 0 } // Place holder for the last one
};
static void *G_UEZGUIWorkspace;
static TBool G_verbose;

// Interface API to standard functional test routines
static const T_testAPI G_UEZGUITestAPI = {
        UEZGUITestAddButtons,
        UEZGUITestRemoveButtons,
        UEZGUITestTextLine,
        UEZGUITestShowResult,
        UEZGUITestSetTestResult,
        UEZGUITestNextTest, };

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestAddButtons
 *---------------------------------------------------------------------------*
 * Description:
 *      Add options/buttons
 * Inputs:
 *      T_testData *aData -- Current test state
 *      TUInt16 aButtonTypes -- Type of buttons to add/show
 *---------------------------------------------------------------------------*/
void UEZGUITestAddButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    // Do nothing
    PARAM_NOT_USED(aData); PARAM_NOT_USED(aButtonTypes);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestRemoveButtons
 *---------------------------------------------------------------------------*
 * Description:
 *      Remove options/buttons
 * Inputs:
 *      T_testData *aData -- Current test state
 *      TUInt16 aButtonTypes -- Type of buttons to remove/hide
 *---------------------------------------------------------------------------*/
void UEZGUITestRemoveButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    // Do nothing
    PARAM_NOT_USED(aData); PARAM_NOT_USED(aButtonTypes);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestTextLine
 *---------------------------------------------------------------------------*
 * Description:
 *      Show a line of text in the test area.
 * Inputs:
 *      T_testData *aData -- Current test state
 *      TUInt16 aLine -- Line of result data (0=top, 1=next down, etc.)
 *      const char *aText -- Text to show
 *---------------------------------------------------------------------------*/
void UEZGUITestTextLine(T_testData *aData, TUInt16 aLine, const char *aText)
{
    void *aWorkspace = aData->iPrivateData;

    // Just output the text regardless of the line offset requested
    PARAM_NOT_USED(aLine);
    if (G_verbose) {
        FDICmdSendString(aWorkspace, aText);
        FDICmdSendString(aWorkspace, "\n");
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestShowResult
 *---------------------------------------------------------------------------*
 * Description:
 *      Show a result on the screen and possibly delay
 * Inputs:
 *      T_testData *aData -- Current test state
 *      TUInt16 aLine -- Line of result data (0=top, 1=next down, etc.)
 *      TUInt16 aResult -- Result to show as well
 *      TUInt32 aDelay -- Amount of time to delay to show this
 *---------------------------------------------------------------------------*/
void UEZGUITestShowResult(
        T_testData *aData,
        TUInt16 aLine,
        TUInt16 aResult,
        TUInt32 aDelay)
{
    static const char *textResults[] = {
            "In Progress",
            "Done",
            "PASS",
            "FAIL",
            "Skip", };

    // We got a result.  Pass or fail?
    void *aWorkspace = aData->iPrivateData;

    FDICmdSendString(aWorkspace, textResults[aResult]);
    FDICmdSendString(aWorkspace, ": ");

    // Don't delay
    PARAM_NOT_USED(aDelay);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestSetTestResult
 *---------------------------------------------------------------------------*
 * Description:
 *      Called when the test wants to set a general test result.
 * Inputs:
 *      T_testData *aData -- Current test state
 *      TUInt16 aResult -- Result to set to
 *---------------------------------------------------------------------------*/
void UEZGUITestSetTestResult(T_testData *aData, TUInt16 aResult)
{
    // We only care about the current test
    aData->iResult = aResult;
    aData->iGoNext = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestNextTest
 *---------------------------------------------------------------------------*
 * Description:
 *      Called when the test is done and wants to go on to the next test
 * Inputs:
 *      T_testData *aData -- Not used
 *---------------------------------------------------------------------------*/
void UEZGUITestNextTest(T_testData *aData)
{
    aData->iGoNext = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUICmdPing
 *---------------------------------------------------------------------------*
 * Description:
 *      When we see PING with no parameters, output "PASS: PONG".  If any
 *      parameters, return "FAIL: PONG"
 * Inputs:
 *      void *aWorkspace -- FDICmd workspace for output
 *      int argc -- number of arguments to this command
 *      char *argv[] -- pointer to a list of pointers for arguments
 * Outputs:
 *      int -- return value (0 for no errors)
 *---------------------------------------------------------------------------*/
int UEZGUICmdPing(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        TestModeSendCmd(TEST_MODE_PING);

        // No Parameters (other than command) cause PONG to pass
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        // Parameters cause PONG to fail
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

static void IUEZGUICmdRunTest(void *aWorkspace, void(*aMonitorFunction)(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton), T_testData *aTestData)
{
    strcpy(aTestData->iResultValue, "OK");
    TestModeGetWindow(&aTestData->iWin);
    aTestData->iGoNext = EFalse;
    aTestData->iPrivateData = aWorkspace;
    (*aMonitorFunction)(&G_UEZGUITestAPI, aTestData, OPT_INIT);
    while (!aTestData->iGoNext) {
        (*aMonitorFunction)(&G_UEZGUITestAPI, aTestData, 0);
    }
    FDICmdPrintf(aWorkspace, "%s\n", aTestData->iResultValue);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUICmdSDRAM
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a memory test over the given range (start, end) and either do
 *      a fast or a slow check (0=fast, 0=slow).
 * Inputs:
 *      void *aWorkspace -- FDICmd workspace for output
 *      int argc -- number of arguments to this command
 *      char *argv[] -- pointer to a list of pointers for arguments
 * Outputs:
 *      int -- return value (0 for no errors)
 *---------------------------------------------------------------------------*/
int UEZGUICmdSDRAM(void *aWorkspace, int argc, char *argv[])
{
    //    TUInt32 start, end;
    TUInt32 type;
    T_testData testData;

    if (argc == 2) {
        // Got all 2 parameters, parse them
        type = FDICmdUValue(argv[1]);
        //        end = FDICmdUValue(argv[2]);
        //        type = FDICmdUValue(argv[3]);
        testData.iParams[0] = type;
        //        testData.iParams[1] = end;
        //        testData.iParams[2] = type;
        testData.iNumParams = 1;

        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestSDRAM, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdNOR(void *aWorkspace, int argc, char *argv[])
{
    //    TUInt32 start, end;
    TUInt32 type;
    T_testData testData;

    if (argc == 2) {
        // Got all 4 parameters, parse them
        //        start = FDICmdUValue(argv[1]);
        //        end = FDICmdUValue(argv[2]);
        type = FDICmdUValue(argv[1]);

        //        testData.iParams[0] = type;
        //        testData.iParams[1] = end;
        testData.iNumParams = 0;

        // Now do the test
        if (type)
            IUEZGUICmdRunTest(aWorkspace, FuncTestFullFlash0, &testData);
        else
            IUEZGUICmdRunTest(aWorkspace, FuncTestFlash0, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 portA, pinA, portB, pinB;
    T_testData testData;
    extern HAL_GPIOPort **PortNumToPort(TUInt8 aPort);
    HAL_GPIOPort **p_portA;
    HAL_GPIOPort **p_portB;
    TUInt32 high, low;

    if (argc == 5) {
        // Got all 5 parameters, parse them
        portA = FDICmdUValue(argv[1]);
        pinA = FDICmdUValue(argv[2]);
        portB = FDICmdUValue(argv[3]);
        pinB = FDICmdUValue(argv[4]);

        p_portA = PortNumToPort(portA);
        if (!p_portA) {
            FDICmdPrintf(aWorkspace, "FAIL: Unknown Port %d\n", portA);
            return -1;
        }
        p_portB = PortNumToPort(portB);
        if (!p_portB) {
            FDICmdPrintf(aWorkspace, "FAIL: Unknown Port %d\n", portB);
            return -1;
        }

        // Now do the test
        (*p_portA)->SetOutputMode(p_portA, 1 << pinA); // set to Output
        (*p_portA)->SetMux(p_portA, pinA, 0); // set to GPIO

        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
        (*p_portB)->SetMux(p_portB, pinB, 0); // set to GPIO

        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);

        // Pause
        UEZTaskDelay(1);

        // Read high setting
        (*p_portB)->Read(p_portB, 1 << pinB, &high);

        // Now change to a low setting
        (*p_portA)->Clear(p_portA, 1 << pinA);

        // Pause
        UEZTaskDelay(1);

        // Now read again
        (*p_portB)->Read(p_portB, 1 << pinB, &low);

        // Are we stuck high?
        if (high && low) {
            FDICmdPrintf(aWorkspace, "FAIL: Stuck HIGH\n");
            return -1;
        }
        if (!high && !low) {
            FDICmdPrintf(aWorkspace, "FAIL: Stuck LOW\n");
            return -1;
        }
        if (!high && low) {
            FDICmdPrintf(aWorkspace, "FAIL: Reversed?\n");
            return -1;
        }

        // High is high and low is low, good
        FDICmdPrintf(aWorkspace, "PASS: OK\n");
    } else if (argc == 1) {
        IUEZGUICmdRunTest(aWorkspace, FuncTestGPIOs, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmd5V(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestVoltageMonitor, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmd3VERR(void *aWorkspace, int argc, char *argv[])
{
    // Read the GPIO for the 3VERR on P2_25
    HAL_GPIOPort **p_gpio2;
    const TUInt32 pin = 25;
    TUInt32 reading;

    HALInterfaceFind("GPIO2", (T_halWorkspace **)&p_gpio2);

    // All we do is determine if this pin is high or low.  High level is good,
    // low means we are tripping an error.
    (*p_gpio2)->Read(p_gpio2, 1 << pin, &reading);
    if (reading) {
        // High: good
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        // Low: bad
        FDICmdSendString(aWorkspace, "FAIL: Triggered\n");
    }
    return 0;
}

int UEZGUICmdEEPROM(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestEEPROM, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestTemperature, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdSDCard(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestMicroSDCard, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}
#if LIGHT_SENSOR_ENABLED
int UEZGUICmdLightSensor(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestLightSensor, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}
#endif

int UEZGUICmdAccelerometer(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestAccelerometerFast, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

#if UEZ_ENABLE_AUDIO_AMP
int UEZGUICmdAmplifier(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;
    T_uezError error;
    T_uezDevice amp;

    if (argc == 2) {
        // Got no parameters
        // Now do the test
        error = UEZAudioAmpOpen("AMP0", &amp);
        if (error) {
            FDICmdSendString(aWorkspace, "FAIL: No Driver\n");
        } else {
            error = UEZAudioAmpSetLevel(amp, FDICmdUValue(argv[1]));
            if (error) {
                FDICmdSendString(aWorkspace, "FAIL: Set Failed\n");
            } else {
                FDICmdSendString(aWorkspace, "PASS: OK\n");
            }
            UEZAudioAmpClose(amp);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}
#endif
int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestUSBHost1, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdUSBPort2(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        IUEZGUICmdRunTest(aWorkspace, FuncTestUSBHost2, &testData);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdBacklight(void *aWorkspace, int argc, char *argv[])
{
  T_testData testData;

  if (argc == 1) {
    // Got no parameters
    // Now do the test
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
    IUEZGUICmdRunTest(aWorkspace, FuncTestBacklightPWM, &testData);
#else
    IUEZGUICmdRunTest(aWorkspace, FuncTestBacklightMonitor, &testData);
#endif
  } else {
    FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
  }
  return 0;
}

int UEZGUICmdLCD(void *aWorkspace, int argc, char *argv[])
{
    //    T_testData testData;
    //    testData.iLine = 0;

    if (argc == 1) {
        // Got no parameters
        // Now do the test
        //        IUEZGUICmdRunTest(aWorkspace, FuncTestLCD, &testData);
        TestModeSendCmd(TEST_MODE_LCD);
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdTouchscreen(void *aWorkspace, int argc, char *argv[])
{
    TestModeSendCmd(TEST_MODE_TOUCHSCREEN);
    return 0;
}

int UEZGUICmdTouchscreenStatus(void *aWorkspace, int argc, char *argv[])
{
    if (G_mmTestModeTouchscreenCalibrationBusy) {
        FDICmdSendString(aWorkspace, "BUSY\n");
    } else {
        if (G_mmTestModeTouchscreenCalibrationValid)
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        else
            FDICmdSendString(aWorkspace, "FAIL: Invalid\n");
    }
    return 0;
}

int UEZGUICmdTouchscreenClear(void *aWorkspace, int argc, char *argv[])
{
    G_nonvolatileSettings.iNeedRecalibrate = ETrue;
    NVSettingsSave();
    FDICmdSendString(aWorkspace, "PASS: OK\n");
    return 0;
}

int UEZGUICmdVerbose(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 2) {
        if (FDICmdUValue(argv[1]))
            G_verbose = ETrue;
        else
            G_verbose = EFalse;
        FDICmdPrintf(aWorkspace, "PASS: Verbose %s\n", G_verbose ? "ON" : "OFF");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdSpeaker(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 freq;
    T_uezDevice speaker;
    HAL_GPIOPort **p_gpio2 = 0;

    if (argc == 2) {
        HALInterfaceFind("GPIO2", (T_halWorkspace **)&p_gpio2);
        UEZToneGeneratorOpen("Speaker", &speaker);
        freq = FDICmdUValue(argv[1]);
        if (freq) {
            (*p_gpio2)->SetMux(p_gpio2, 1, 1);
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_HZ(freq));
        } else {
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_OFF);
            (*p_gpio2)->SetMux(p_gpio2, 1, 0);
        }
        FDICmdPrintf(aWorkspace, "PASS: %d Hz\n", freq);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdColor(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 red, green, blue;
    extern T_pixelColor G_mmTestModeColor;

    if (argc == 4) {
        red = FDICmdUValue(argv[1]);
        green = FDICmdUValue(argv[2]);
        blue = FDICmdUValue(argv[3]);
        G_mmTestModeColor = RGB(red, green, blue);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdMACAddress(void *aWorkspace, int argc, char *argv[])
{
    char mac[6];
    int macLen;
    char *p;
    char c;
    char hex[] = "$??";
    int hexLen;

    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: %02X:%02X:%02X:%02X:%02X:%02X\n",
                G_nonvolatileSettings.iMACAddr[0],
                G_nonvolatileSettings.iMACAddr[1],
                G_nonvolatileSettings.iMACAddr[2],
                G_nonvolatileSettings.iMACAddr[3],
                G_nonvolatileSettings.iMACAddr[4],
                G_nonvolatileSettings.iMACAddr[5]);
    } else if (argc == 2) {
        p = argv[1];
        if (strcmp(p, "default")==0) {
            mac[0] = emacMACADDR0;
            mac[1] = emacMACADDR1;
            mac[2] = emacMACADDR2;
            mac[3] = emacMACADDR3;
            mac[4] = emacMACADDR4;
            mac[5] = emacMACADDR5;
        } else {
            macLen = 0;
            mac[0] = '\0';
            hexLen = 0;
            while (*p) {
                c = *p;
                if ((c != ':') && (c != '.')) {
                    if (macLen == 6) {
                        FDICmdSendString(aWorkspace,
                                "FAIL: MAC address too long!\n");
                        return 0;
                    }
                    hex[1 + hexLen++] = c;
                    if (hexLen == 2) {
                        mac[macLen++] = FDICmdUValue(hex);
                        hexLen = 0;
                    }
                }
                p++;
            }
            if (macLen != 6) {
                FDICmdSendString(aWorkspace, "FAIL: MAC address too short!\n");
                return 0;
            }
        }
        memcpy(&G_nonvolatileSettings.iMACAddr, mac, 6);
        if (NVSettingsSave()) {
            FDICmdSendString(aWorkspace,
                    "FAIL: MAC address failed to save correctly\n");
        } else {
            FDICmdPrintf(aWorkspace, "PASS: %02X:%02X:%02X:%02X:%02X:%02X\n",
                    G_nonvolatileSettings.iMACAddr[0],
                    G_nonvolatileSettings.iMACAddr[1],
                    G_nonvolatileSettings.iMACAddr[2],
                    G_nonvolatileSettings.iMACAddr[3],
                    G_nonvolatileSettings.iMACAddr[4],
                    G_nonvolatileSettings.iMACAddr[5]);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdIPAddress(void *aWorkspace, int argc, char *argv[])
{
    char ip[4];
    int ipLen;
    char *p;
    char c;
    char num[10];
    int numLen;

    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                G_nonvolatileSettings.iIPAddr[0],
                G_nonvolatileSettings.iIPAddr[1],
                G_nonvolatileSettings.iIPAddr[2],
                G_nonvolatileSettings.iIPAddr[3]);
    } else if (argc == 2) {
        p = argv[1];
        ipLen = 0;
        ip[0] = '\0';
        numLen = 0;
        while (1) {
            c = *p;
            if ((c == '.') || (c == '\0')) {
                if (ipLen == 4) {
                    FDICmdSendString(aWorkspace,
                            "FAIL: IP address too long!\n");
                    return 0;
                }
                ip[ipLen++] = FDICmdUValue(num);
                numLen = 0;
            } else {
                num[numLen++] = c;
                num[numLen] = '\0';
            }
            if (*p == '\0')
                break;
            p++;
        }
        if (ipLen != 4) {
            FDICmdSendString(aWorkspace, "FAIL: IP address too short!\n");
            return 0;
        }
        memcpy(&G_nonvolatileSettings.iIPAddr, ip, 4);
        if (NVSettingsSave()) {
            FDICmdSendString(aWorkspace,
                    "FAIL: IP address failed to save correctly\n");
        } else {
            FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                    G_nonvolatileSettings.iIPAddr[0],
                    G_nonvolatileSettings.iIPAddr[1],
                    G_nonvolatileSettings.iIPAddr[2],
                    G_nonvolatileSettings.iIPAddr[3]);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdIPGatewayAddress(void *aWorkspace, int argc, char *argv[])
{
    char ip[4];
    int ipLen;
    char *p;
    char c;
    char num[10];
    int numLen;

    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                G_nonvolatileSettings.iIPGateway[0],
                G_nonvolatileSettings.iIPGateway[1],
                G_nonvolatileSettings.iIPGateway[2],
                G_nonvolatileSettings.iIPGateway[3]);
    } else if (argc == 2) {
        p = argv[1];
        ipLen = 0;
        ip[0] = '\0';
        numLen = 0;
        while (1) {
            c = *p;
            if ((c == '.') || (c == '\0')) {
                if (ipLen == 4) {
                    FDICmdSendString(aWorkspace,
                            "FAIL: IP Gateway address too long!\n");
                    return 0;
                }
                ip[ipLen++] = FDICmdUValue(num);
                numLen = 0;
            } else {
                num[numLen++] = c;
                num[numLen] = '\0';
            }
            if (*p == '\0')
                break;
            p++;
        }
        if (ipLen != 4) {
            FDICmdSendString(aWorkspace, "FAIL: IP Gateway address too short!\n");
            return 0;
        }
        memcpy(&G_nonvolatileSettings.iIPGateway, ip, 4);
        if (NVSettingsSave()) {
            FDICmdSendString(aWorkspace,
                    "FAIL: IP Gateway address failed to save correctly\n");
        } else {
            FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                    G_nonvolatileSettings.iIPGateway[0],
                    G_nonvolatileSettings.iIPGateway[1],
                    G_nonvolatileSettings.iIPGateway[2],
                    G_nonvolatileSettings.iIPGateway[3]);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdIPMaskAddress(void *aWorkspace, int argc, char *argv[])
{
    char ip[4];
    int ipLen;
    char *p;
    char c;
    char num[10];
    int numLen;

    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                G_nonvolatileSettings.iIPMask[0],
                G_nonvolatileSettings.iIPMask[1],
                G_nonvolatileSettings.iIPMask[2],
                G_nonvolatileSettings.iIPMask[3]);
    } else if (argc == 2) {
        p = argv[1];
        ipLen = 0;
        ip[0] = '\0';
        numLen = 0;
        while (1) {
            c = *p;
            if ((c == '.') || (c == '\0')) {
                if (ipLen == 4) {
                    FDICmdSendString(aWorkspace,
                            "FAIL: IP Mask address too long!\n");
                    return 0;
                }
                ip[ipLen++] = FDICmdUValue(num);
                numLen = 0;
            } else {
                num[numLen++] = c;
                num[numLen] = '\0';
            }
            if (*p == '\0')
                break;
            p++;
        }
        if (ipLen != 4) {
            FDICmdSendString(aWorkspace, "FAIL: IP Mask address too short!\n");
            return 0;
        }
        memcpy(&G_nonvolatileSettings.iIPMask, ip, 4);
        if (NVSettingsSave()) {
            FDICmdSendString(aWorkspace,
                    "FAIL: IP Mask address failed to save correctly\n");
        } else {
            FDICmdPrintf(aWorkspace, "PASS: %d.%d.%d.%d\n",
                    G_nonvolatileSettings.iIPMask[0],
                    G_nonvolatileSettings.iIPMask[1],
                    G_nonvolatileSettings.iIPMask[2],
                    G_nonvolatileSettings.iIPMask[3]);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

void IWaitTouchscreen(void)
{
    T_uezTSReading reading;
    T_uezDevice ts;
    T_uezQueue queue;

    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue) == UEZ_ERROR_NONE) {
            // Wait first for the screen NOT to be touched
            while (1) {
                if (UEZQueueReceive(queue, &reading, 10) != UEZ_ERROR_NONE) {
                    if (!(reading.iFlags & TSFLAG_PEN_DOWN))
                        break;
                }
            }

            // Wait first for the screen to be touched
            while (1) {
                if (UEZQueueReceive(queue, &reading, 10) != UEZ_ERROR_NONE) {
                    if (reading.iFlags & TSFLAG_PEN_DOWN)
                        break;
                }
            }

            // Wait first for the screen NOT to be touched
            while (1) {
                if (UEZQueueReceive(queue, &reading, 10) != UEZ_ERROR_NONE) {
                    if (!(reading.iFlags & TSFLAG_PEN_DOWN))
                        break;
                }
            }

            UEZTSClose(ts, queue);
        }
        UEZQueueDelete(queue);
    }
}

int UEZGUICmdColorCycle(void *aWorkspace, int argc, char *argv[])
{
    extern T_pixelColor G_mmTestModeColor;

    if (argc == 1) {
        G_mmTestModeColor = RGB(255, 0, 0);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
        IWaitTouchscreen();

        G_mmTestModeColor = RGB(0, 255, 0);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
        IWaitTouchscreen();

        G_mmTestModeColor = RGB(0, 0, 255);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
        IWaitTouchscreen();

        G_mmTestModeColor = RGB(255, 255, 255);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);

        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdAlignmentBorder(void *aWorkspace, int argc, char *argv[])
{
    extern T_pixelColor G_mmTestModeColor;

    if (argc == 1) {
        TestModeSendCmd(TEST_MODE_ALIGNMENT_BORDER);
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdRelay(void *aWorkspace, int argc, char *argv[])
{
    UEZGPIOSetMux(GPIO_P0_10, 0);
    UEZGPIOOutput(GPIO_P0_10);
    if (argc == 2) {
        // Got 1 parameter
        if (FDICmdUValue(argv[2])) {
            UEZGPIOSet(GPIO_P0_10);
        } else {
            UEZGPIOClear(GPIO_P0_10);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}


#include <Source/Library/Network/GainSpan/CmdLib/AtCmdLib.h>

int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[])
{
  // todo: Not Finished - Here for future reference
  char messageRxBuf[400] = {0};
  static TBool init_gainspan = EFalse;
  if (!init_gainspan) {
    //GainSpan_SPI_Start();
    init_gainspan = ETrue;
  }
  UEZGUIGainSpan(aWorkspace, argc, argv, messageRxBuf);

  return 0;
}

T_uezError UEZGUICmdGainSpanCMDProc(char *argv[])
{
    T_uezDevice stream;
    T_uezError error;
    void *G_UEZGUIWorkspace;

    // Verbose is off by default
    G_verbose = EFalse;

    // Open the console serial port if available
    error = UEZStreamOpen("Console", &stream);
    if (error)
        return error;

    // Start FDI Cmd Console
    G_UEZGUIWorkspace = 0;
    error = FDICmdStart(stream, &G_UEZGUIWorkspace, 2048, G_UEZGUICommands);
    if (error)
        return error;

    FDICmdProcessCmd(G_UEZGUIWorkspace, (const char *) argv);//"gainspan client");

  return UEZ_ERROR_NONE;
}


int UEZGUICmdEnd(void *aWorkspace, int argc, char *argv[])
{
    // Try to end test mode
    TestModeSendCmd(TEST_MODE_END);
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestCmdsInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Start up UEZGUI Command Console on device "Console" or return an error.
 * Outputs:
 *      T_uezError -- Error code if any.
 *---------------------------------------------------------------------------*/
T_uezError UEZGUITestCmdsInit(void)
{
    T_uezDevice stream;
    T_uezError error;
    void *G_UEZGUIWorkspace;

    // Verbose is off by default
    G_verbose = EFalse;

    // Open the console serial port if available
    error = UEZStreamOpen("Console", &stream);
    if (error)
        return error;

    // Start FDI Cmd Console
    G_UEZGUIWorkspace = 0;
    error = FDICmdStart(stream, &G_UEZGUIWorkspace, 2048, G_UEZGUICommands);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUITestCmdsHalt
 *---------------------------------------------------------------------------*
 * Description:
 *      Stop the UEZGUI Test Cmd Console
 *---------------------------------------------------------------------------*/
T_uezError UEZGUITestCmdsHalt(void)
{
    return FDICmdStop(G_UEZGUIWorkspace);
}

/*-------------------------------------------------------------------------*
 * File:  UEZGUITestCmds.c
 *-------------------------------------------------------------------------*/
