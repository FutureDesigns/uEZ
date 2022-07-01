/*-------------------------------------------------------------------------*
 * File:  TestCmds.c
 *-------------------------------------------------------------------------*
 * Description:
 *      UEZGUI Tester command console
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
#if APP_MENU_ALLOW_TEST_MODE
#include <uEZStream.h>
#include <uEZSPI.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <Source/Library/Console/FDICmd/Commands/FDICmd_AudioDAC.h>
#include <Source/Library/Console/FDICmd/Commands/FDICmd_WAVPlay.h>
#include <Source/Library/Console/FDICmd/Commands/FDICmd_I2C.h>
#include <Source/Library/Console/FDICmd/Commands/FDICmd_File.h>
#include "FuncTestFramework.h"
#include "FunctionalTest.h"
#include "AppDemo.h"
#include <HAL/GPIO.h>
#include <uEZToneGenerator.h>
#include <Device/ToneGenerator.h>
#include "NVSettings.h"
#include <uEZToneGenerator.h>
#include <uEZPlatform.h>
#include <uEZAudioAmp.h>
#include <uEZAudioMixer.h>
#include <uEZLCD.h>
#include <uEZI2C.h>
#include <uEZTimeDate.h>
#include <uEZEEPROM.h>
#include <uEZFile.h>

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
int UEZGUICmdSPIFI(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd5V(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd3VERR(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEEPROM(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdRTC(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort0(void *aWorkspace, int argc, char *argv[]);
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
#if SEC_TO_ENABLED //Trusted Objects enabled
int UEZGUICmdSETO(void *aWorkspace, int argc, char *argv[]);
#endif
#if SEC_MC_ENABLED //Microchip enabled
int UEZGUICmdSEMC(void *aWorkspace, int argc, char *argv[]);
#endif
int UEZGUICmdReset(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdMACAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPMaskAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdIPGatewayAddress(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdGainSpanRun(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdCRC(void *aWorkspace, int argc, char *argv[]);
extern T_uezError UEZToneGeneratorOpen(
            const char * const aName,
            T_uezDevice *aDevice);
extern T_uezError UEZToneGeneratorPlayToneContinuous(
            T_uezDevice aDevice,
            TUInt32 aTonePeriod);
int UEZGUICmdI2CBang(void *aWorkspace, int argc, char *argv[]);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static const T_consoleCmdEntry G_UEZGUICommands[] = {
        { "PING", UEZGUICmdPing },
        { "SDRAM", UEZGUICmdSDRAM },
        { "NOR", UEZGUICmdNOR },
        { "SPIFI", UEZGUICmdSPIFI },
        { "GPIO", UEZGUICmdGPIO },
        { "5V", UEZGUICmd5V },
        { "3VERR", UEZGUICmd3VERR },
        { "EEPROM", UEZGUICmdEEPROM },
        { "TEMP", UEZGUICmdTemperature },
#if SEC_TO_ENABLED //Trusted Objects enabled
        { "SETO", UEZGUICmdSETO },
#endif
#if SEC_MC_ENABLED //Microchip enabled
        { "SEMC", UEZGUICmdSEMC },
#endif
        { "RESET", UEZGUICmdReset },
        { "SDCARD", UEZGUICmdSDCard },
#if LIGHT_SENSOR_ENABLED
        { "LIGHTSENSOR", UEZGUICmdLightSensor },
#endif
        { "RTC", UEZGUICmdRTC },
        { "USB0", UEZGUICmdUSBPort0 },
        { "USB1", UEZGUICmdUSBPort1 },
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
#if UEZ_ENABLE_AUDIO_AMP
        { "AMP", UEZGUICmdAmplifier },
        { "AUDIODAC", UEZCmdAudioDAC },
        { "WAVPLAY", UEZCmdWAVPlay },
#endif
        { "MACADDR", UEZGUICmdMACAddress },
        { "IPADDR", UEZGUICmdIPAddress },
        { "IPMASK", UEZGUICmdIPMaskAddress },
        { "IPGATEWAY", UEZGUICmdIPGatewayAddress },
#if UEZ_WIRELESS_PROGRAM_MODE
        { "GAINSPANPROGRAM", UEZGUICmdGainSpan },
        { "GAINSPANRUNMODE", UEZGUICmdGainSpanRun },
#endif
        { "I2CPROBE", UEZCmdI2CProbe },
        { "I2CWRITE", UEZCmdI2CWrite },
        { "I2CREAD", UEZCmdI2CRead },
        { "FILETESTWRITE", UEZCmdFileTestWrite },
        { "I2CBANG", UEZGUICmdI2CBang },
        { "CRC" , UEZGUICmdCRC},
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
 * Routine:  UEZGUICmdGainSpan
 *---------------------------------------------------------------------------*
 * Description:
 *      Place Unit into passthrough for GainSpan flashing (module flash)
 * Inputs:
 *      void *aWorkspace -- FDICmd workspace for output
 *      int argc -- number of arguments to this command
 *      char *argv[] -- pointer to a list of pointers for arguments
 * Outputs:
 *      int -- return value (0 for no errors)
 *---------------------------------------------------------------------------*/
extern TUInt32 G_romChecksum;
extern TBool G_romChecksumCalculated;
int UEZGUICmdCRC(void *aWorkspace, int argc, char *argv[])
{
    char message[20];

    if(argc == 1){
        sprintf(message, "0x%08X\n", G_romChecksumCalculated ? G_romChecksum : 0);
        FDICmdSendString(aWorkspace, message);
    } else if(argc == 2){
        sprintf(message, "0x%08X", G_romChecksumCalculated ? G_romChecksum : 0);

        if(strcmp(message, argv[1]) == 0){
            sprintf(message, "PASS: 0x%08X\n", G_romChecksum);
            FDICmdSendString(aWorkspace, message);
        } else {
            sprintf(message, "FAIL: 0x%08X\n", G_romChecksumCalculated ? G_romChecksum : 0);
            FDICmdSendString(aWorkspace, message);
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUICmdGainSpan
 *---------------------------------------------------------------------------*
 * Description:
 *      Place Unit into passthrough for GainSpan flashing (module flash)
 * Inputs:
 *      void *aWorkspace -- FDICmd workspace for output
 *      int argc -- number of arguments to this command
 *      char *argv[] -- pointer to a list of pointers for arguments
 * Outputs:
 *      int -- return value (0 for no errors)
 *---------------------------------------------------------------------------*/
int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        // No Parameters (other than command) cause CMD to pass
        FDICmdSendString(aWorkspace, "PASS: OK\n");
        UEZPlatform_WiFiProgramMode(EFalse);
    } else {
        // Parameters cause CMD to fail
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUICmdGainSpanRun
 *---------------------------------------------------------------------------*
 * Description:
 *      Place Unit into passthrough for GainSpan run mode, for ext NOR flash
 * Inputs:
 *      void *aWorkspace -- FDICmd workspace for output
 *      int argc -- number of arguments to this command
 *      char *argv[] -- pointer to a list of pointers for arguments
 * Outputs:
 *      int -- return value (0 for no errors)
 *---------------------------------------------------------------------------*/
int UEZGUICmdGainSpanRun(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        // No Parameters (other than command) cause CMD to pass
        FDICmdSendString(aWorkspace, "PASS: OK\n");
        UEZPlatform_WiFiProgramMode(ETrue);
    } else {
        // Parameters cause CMD to fail
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

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
#include <uEZFlash.h>
int UEZGUICmdSPIFI(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 i;
    T_uezDevice flash;
    T_uezError error;
    TUInt8 buffer[256];

    if (argc == 1) {
        if(UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE){
            error = UEZFlashBlockErase(flash, 0x00700000, 256*1024); // first 256K
            if (error){
                FDICmdSendString(aWorkspace, "FAIL: Failed to erase flash\n");
                return -1;
            }
            error = UEZFlashRead(flash, 0x00700000, buffer, 128);
            if (error){
                FDICmdSendString(aWorkspace, "FAIL: Failed to read flash\n");
                return -1;
            }
            // Should be all 0xFF
            for (i=0; i<128; i++)
                if (buffer[i] != 0xFF){
                    FDICmdSendString(aWorkspace, "FAIL: Flash not Erased\n");
                    return -1;
                }

            // Do write/read test over 256K boundary
            error = UEZFlashWrite(flash, 0x00700000+256*1024-4, (TUInt8*)"Hello ", 6);
            if (error){
                FDICmdSendString(aWorkspace, "FAIL: Failed to write flash\n");
                return -1;
            }
            error = UEZFlashWrite(flash, 0x00700000+256*1024-4+6, (TUInt8*)"World ", 6);
            if (error){
                FDICmdSendString(aWorkspace, "FAIL: Failed to write flash\n");
                return -1;
            }
            error = UEZFlashRead(flash, 0x00700000+256*1024-4, buffer, 128);
            if (error){
                FDICmdSendString(aWorkspace, "FAIL: Failed to read flash\n");
                return -1;
            }
            if (memcmp(buffer, "Hello World ", 12) != 0){
                FDICmdSendString(aWorkspace, "FAIL: Memory Compare Failed\n");
                return -1;
            }
            FDICmdSendString(aWorkspace, "PASS: OK\n");
            UEZFlashClose(flash);
        } else {
            FDICmdSendString(aWorkspace, "FAIL: Flash Not Enabled\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 portA, pinA, portB, pinB, portC, pinC;
    T_testData testData;
    extern HAL_GPIOPort **PortNumToPort(TUInt8 aPort);
    HAL_GPIOPort **p_portA;
    HAL_GPIOPort **p_portB;
    HAL_GPIOPort **p_portC;
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
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portA)->SetMux(p_portA, pinA, 0); // set to GPIO
#else
        (*p_portA)->SetMux(p_portA, pinA, (pinA > 4)? 4 : 0); // set to GPIO
        TUInt32 value = SCU_NORMAL_DRIVE_DEFAULT(0);
        value |= (pinA > 4)? 4 :0;
        (*p_portA)->Control(p_portA, pinA, GPIO_CONTROL_SET_CONFIG_BITS, value);
#endif
        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
        (*p_portB)->SetMux(p_portB, pinB, 0); // set to GPIO
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portB)->SetMux(p_portB, pinA, 0); // set to GPIO
#else
        (*p_portB)->SetMux(p_portB, pinB, (pinB > 4)? 4 : 0); // set to GPIO
        value = SCU_NORMAL_DRIVE_DEFAULT(0);
        value |= (pinB > 4)? 4 :0;
        (*p_portB)->Control(p_portB, pinB, GPIO_CONTROL_SET_CONFIG_BITS, value);
#endif

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
    } else if (argc == 7) {
        // Got all 7 parameters, parse them
        portA = FDICmdUValue(argv[1]);
        pinA = FDICmdUValue(argv[2]);
        portB = FDICmdUValue(argv[3]);
        pinB = FDICmdUValue(argv[4]);
        portC = FDICmdUValue(argv[5]);
        pinC = FDICmdUValue(argv[6]);

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
        p_portC = PortNumToPort(portC);
        if (!p_portC) {
            FDICmdPrintf(aWorkspace, "FAIL: Unknown Port %d\n", portC);
            return -1;
        }

        // Reset pins back to default state
        (*p_portA)->SetInputMode(p_portA, 1 << pinA);            // set to Input
        (*p_portA)->SetPull(p_portA, 1 << pinA, GPIO_PULL_NONE); // set to floating input mode
        (*p_portA)->SetMux(p_portA, pinA, 0);                    // set to GPIO
        (*p_portB)->SetInputMode(p_portB, 1 << pinB);            // set to Input
        (*p_portB)->SetPull(p_portB, 1 << pinB, GPIO_PULL_NONE); // set to floating input mode
        (*p_portB)->SetMux(p_portB, pinB, 0);                    // set to GPIO
        (*p_portC)->SetInputMode(p_portC, 1 << pinC);            // set to Input
        (*p_portC)->SetPull(p_portC, 1 << pinC, GPIO_PULL_NONE); // set to floating input mode
        (*p_portC)->SetMux(p_portC, pinC, 0);                    // set to GPIO

        // Test Pair AB
        (*p_portA)->SetOutputMode(p_portA, 1 << pinA); // set to Output
        (*p_portA)->SetMux(p_portA, pinA, 0); // set to GPIO
        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
        (*p_portB)->SetMux(p_portB, pinB, 0); // set to GPIO
        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);
        UEZTaskDelay(1);         // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &high); // Read high setting
        (*p_portA)->Clear(p_portA, 1 << pinA); // Now change to a low setting
        UEZTaskDelay(1); // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &low); // Now read again
        if (high && low) { // Are we stuck high or low?
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

        // Reset pins back to default state
        (*p_portA)->SetInputMode(p_portA, 1 << pinA);            // set to Input
        (*p_portA)->SetPull(p_portA, 1 << pinA, GPIO_PULL_NONE); // set to floating input mode
        (*p_portB)->SetInputMode(p_portB, 1 << pinB);            // set to Input
        (*p_portB)->SetPull(p_portB, 1 << pinB, GPIO_PULL_NONE); // set to floating input mode
        (*p_portC)->SetInputMode(p_portC, 1 << pinC);            // set to Input
        (*p_portC)->SetPull(p_portC, 1 << pinC, GPIO_PULL_NONE); // set to floating input mode

        // Test Pair BC
        (*p_portC)->SetOutputMode(p_portC, 1 << pinC); // set to Output
        (*p_portC)->SetMux(p_portC, pinC, 0); // set to GPIO
        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
        (*p_portB)->SetMux(p_portB, pinB, 0); // set to GPIO
        // Drive 1 on this pin
        (*p_portC)->Set(p_portC, 1 << pinC);
        UEZTaskDelay(1);         // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &high); // Read high setting
        (*p_portC)->Clear(p_portC, 1 << pinC); // Now change to a low setting
        UEZTaskDelay(1); // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &low); // Now read again
        if (high && low) { // Are we stuck high or low?
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

        // Reset pins back to default state
        (*p_portA)->SetInputMode(p_portA, 1 << pinA);            // set to Input
        (*p_portA)->SetPull(p_portA, 1 << pinA, GPIO_PULL_NONE); // set to floating input mode
        (*p_portB)->SetInputMode(p_portB, 1 << pinB);            // set to Input
        (*p_portB)->SetPull(p_portB, 1 << pinB, GPIO_PULL_NONE); // set to floating input mode
        (*p_portC)->SetInputMode(p_portC, 1 << pinC);            // set to Input
        (*p_portC)->SetPull(p_portC, 1 << pinC, GPIO_PULL_NONE); // set to floating input mode

        // Test Pair AC
        (*p_portA)->SetOutputMode(p_portA, 1 << pinA); // set to Output
        (*p_portA)->SetMux(p_portA, pinA, 0); // set to GPIO
        (*p_portC)->SetPull(p_portC, pinC, GPIO_PULL_UP); // set to Pull up
        (*p_portC)->SetInputMode(p_portC, 1 << pinC); // set to Input
        (*p_portC)->SetMux(p_portC, pinC, 0); // set to GPIO
        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);
        UEZTaskDelay(1);         // Pause
        (*p_portC)->Read(p_portC, 1 << pinC, &high); // Read high setting
        (*p_portA)->Clear(p_portA, 1 << pinA); // Now change to a low setting
        UEZTaskDelay(1); // Pause
        (*p_portC)->Read(p_portC, 1 << pinC, &low); // Now read again
        if (high && low) { // Are we stuck high or low?
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
    // Read the GPIO for the 3VERR on P2_25 LPC1788_4088
#if (UEZ_PROCESSOR != NXP_LPC4357)
    T_uezGPIOPortPin gpioPin = GPIO_P2_25;
#else
    T_uezGPIOPortPin gpioPin = GPIO_P4_10;
    TUInt32 value;
#endif

    TUInt32 reading;

#if (UEZ_PROCESSOR != NXP_LPC4357)
    UEZGPIOSetMux(gpioPin, 0);
#else
    value = ((gpioPin >> 8) & 0x7) >= 5 ? 4 : 0;
    value |= SCU_EPD_DISABLE | SCU_EPUN_DISABLE | SCU_EHS_FAST | SCU_EZI_ENABLE | SCU_ZIF_ENABLE;
    UEZGPIOControl(gpioPin, GPIO_CONTROL_SET_CONFIG_BITS, value);
#endif

    // All we do is determine if this pin is high or low.  High level is good,
    // low means we are tripping an error.
    reading = UEZGPIORead(gpioPin);
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
#if (UEZ_PROCESSOR != NXP_LPC4357)
    T_testData testData;
#else
    T_uezDevice eeprom;
    TUInt8 data[15] = "Hello World!";
    TUInt8 read[15] = {0};
#endif

    if (argc == 1) {
        // Got no parameters
        // Now do the test
#if (UEZ_PROCESSOR != NXP_LPC4357)
        IUEZGUICmdRunTest(aWorkspace, FuncTestEEPROM, &testData);
#else
        if(UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE){
            if(UEZEEPROMWrite(eeprom, data, (1 * 1024), sizeof(data)) == UEZ_ERROR_NONE){
                if(UEZEEPROMRead(eeprom, read, (1 * 1024), sizeof(read)) == UEZ_ERROR_NONE){
                    if(memcmp((void*)data, (void*)read, sizeof(data)) == 0){
                        FDICmdSendString(aWorkspace, "PASS: OK\n");
                    } else {
                        FDICmdSendString(aWorkspace, "FAIL: Compare\n");
                    }
                } else {
                    FDICmdSendString(aWorkspace, "FAIL: to read\n");
                }
            } else {
                FDICmdSendString(aWorkspace, "FAIL: to write\n");
            }
            UEZEEPROMClose(eeprom);
        } else {
            FDICmdSendString(aWorkspace, "FAIL: to open\n");
        }
#endif
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
#if 0
    T_testData testData;
    T_uezError error;
    T_uezDevice amp;

    if (argc == 2) {
        // Got parameters
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
#else
    if (argc == 2) {
        // Got parameters
        // Now do the test. This insures we use the mixer to scale the correct maximum volume for the onboard speaker.
        UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, FDICmdUValue(argv[1]));
        FDICmdSendString(aWorkspace, "PASS: OK\n");
#endif
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}
#endif

#if SEC_TO_ENABLED //Trusted Objects enabled
int UEZGUICmdSETO(void *aWorkspace, int argc, char *argv[])
{  
    //T_uezError error;
    //TUInt32 value;
    //TUInt32 reading;
    //char printString[30];
    
    /*TUInt32 numBytes;
    T_uezDevice I2C;
    TUInt8 dataIn[4];
    TUInt8 dataOut[5] = {0};
    I2C_Request r;

    dataOut[0] = ;
    dataOut[1] = ;
    dataOut[2] = ;
    dataOut[3] = ;
    dataOut[4] = ;
    
    numBytes = 5;

    if(UEZI2COpen("I2C1", &I2C) != UEZ_ERROR_NONE){
        FDICmdSendString(aWorkspace, "FAIL: Could not open I2C1\n");
    }

    r.iAddr = 0x50;
    r.iSpeed = 400; //kHz
    r.iWriteData = &dataOut[0];
    r.iWriteLength = numBytes; 
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = &dataIn[0];
    r.iReadLength = 4; 
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;
    
    UEZI2CTransaction(I2C, &r);*/
        
    FDICmdSendString(aWorkspace, "FAIL: No Driver\n");
    // 
    // 
    //error = UEZSETOOpen("I2C0", &I2C);
    /*if (error) {
        FDICmdSendString(aWorkspace, "FAIL: No Driver\n");
    }  else {
        //error = UEZSecCalc(I2C, );
        if (error) {
              FDICmdSendString(aWorkspace, "FAIL: \n");
          } else {
              FDICmdSendString(aWorkspace, "PASS: OK\n");
          }
      //    UEZSETOClose(I2C);
    }*/
    return 0;
}
#endif

#if SEC_MC_ENABLED //Microchip enabled
int UEZGUICmdSEMC(void *aWorkspace, int argc, char *argv[])
{  
    //T_uezError error;
    TUInt32 value;
    //TUInt32 reading;
    //char printString[30];
    
    TUInt32 numBytes;
    T_uezDevice I2C;
    TUInt8 dataIn[8] = {0};
    TUInt8 dataOut[8] = {0};
    I2C_Request r;
    
    UEZPlatform_I2C1_Require();
    UEZPlatform_CRC0_Require();
    
#include <uEZCRC.h>
   T_uezDevice crc;
    
    FDICmdPrintf(aWorkspace, "Set pins high\n");
     
    UEZGPIOOutput(GPIO_P4_13); 
    UEZGPIOSetMux(GPIO_P4_13, (GPIO_P4_13 >> 8) >= 5 ? 4 : 0);
    value = ((GPIO_P4_13 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(GPIO_P4_13, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOSet(GPIO_P4_13);
        
    UEZGPIOOutput(GPIO_P4_14); 
    UEZGPIOSetMux(GPIO_P4_14, (GPIO_P4_14 >> 8) >= 5 ? 4 : 0);
    value = ((GPIO_P4_14 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(GPIO_P4_14, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOSet(GPIO_P4_14);
    
    UEZTaskDelay(10); // Power on delay
    
    if(UEZI2COpen("I2C1", &I2C) != UEZ_ERROR_NONE){
        FDICmdSendString(aWorkspace, "FAIL: Could not open I2C1\n");
    }
    UEZI2CResetBus(I2C);    
    dataOut[0] = 0x00;
    dataOut[1] = 0x00;    
    r.iAddr = 0x60;
    r.iSpeed = 100; //kHz
    r.iWriteData = dataOut;
    r.iWriteLength = 2; 
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = dataIn;
    r.iReadLength = 0; 
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;    
    UEZI2CTransaction(I2C, &r); // we only get ACK on write if we do this reset command first to wake it up. But this may not be the proper wake sequence.

    numBytes = 8;
    dataOut[0] = 0x03; // command mode
    dataOut[1] = 7; // There will be seven bytes sent. 4 bytes of command from above, 2 checksum bytes, and the 1 byte which is this length byte
    dataOut[2] = 0x30; // info command
    dataOut[3] = 0x00; // revision mode
    dataOut[4] = 0x00;
    dataOut[5] = 0x00;
    
    // use UEZ_CRC_16_IBM polynomial 0x8005
   if(UEZCRCOpen("CRC0", &crc) == UEZ_ERROR_NONE){
     	// the device opened properly
      UEZCRCSetComputationType(crc, UEZ_CRC_16_IBM); // poly not supported
      UEZCRCComputeData(crc, &dataOut[0], UEZ_CRC_DATA_SIZE_UINT16, 5); // size not supported
      UEZCRCReadChecksum(crc, &value); // must calculate using some other mechanism
      // See http://www.zorc.breitbandkatze.de/crc.html CRC-16, uncheck reverse crc result before final XOR
      value = 0x5D03; 
      dataOut[6] = (TUInt8) (value & 0xFF);
      dataOut[7] = (TUInt8) (value >> 8);      
           
      UEZCRCClose(crc);
   }else{
       FDICmdSendString(aWorkspace, "FAIL: Could not open CRC0\n");
   }
    
    FDICmdPrintf(aWorkspace, "TX: %X,%X,%X,%X,%X,%X,%X,%X\n", dataOut[0], dataOut[1], dataOut[2], 
                     dataOut[3], dataOut[4], dataOut[5], dataOut[6], dataOut[7]);    
    r.iAddr = 0x60;
    r.iSpeed = 400; //kHz
    r.iWriteData = dataOut;
    r.iWriteLength = numBytes; 
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = dataIn;
    r.iReadLength = 7; 
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;    
    UEZI2CTransaction(I2C, &r);        
    FDICmdPrintf(aWorkspace, "RX: %X,%X,%X,%X,%X,%X,%X\n", dataIn[0], dataIn[1], dataIn[2], dataIn[3]
                 , dataIn[4], dataIn[5], dataIn[6]);    
    UEZI2CClose(I2C);
    
    //FDICmdSendString(aWorkspace, "FAIL: No Driver\n");
    // 
    // 
    //error = UEZSEMCOpen("I2C0", &I2C);
    /*if (error) {
        FDICmdSendString(aWorkspace, "FAIL: No Driver\n");
    }  else {
        //error = UEZSecCalc(I2C, );
        if (error) {
              FDICmdSendString(aWorkspace, "FAIL: \n");
          } else {
              FDICmdSendString(aWorkspace, "PASS: OK\n");
          }
      //    UEZSEMCClose(I2C);
    }*/
    return 0;
}
#endif

int UEZGUICmdReset(void *aWorkspace, int argc, char *argv[])
{  
    FDICmdSendString(aWorkspace, "PASS: OK\n");
    UEZTaskDelay(500);         
    UEZPlatform_System_Reset();
    return 0;
}

int UEZGUICmdRTC(void *aWorkspace, int argc, char *argv[])
{
    T_uezTimeDate td;    
    char *p;

    if (argc == 1) {
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2018;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2018, 8:00:00
        UEZTimeDateSet(&td);

        UEZTaskDelay(5000);

        UEZTimeDateGet(&td);
        if ((td.iDate.iMonth==1) &&
                (td.iDate.iDay == 1) &&
                (td.iDate.iYear == 2018) &&
                (td.iTime.iHour == 8) &&
                (td.iTime.iMinute == 0) &&
                (td.iTime.iSecond > 1)) {
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        } else {
            FDICmdSendString(aWorkspace, "FAIL: Not Incrementing\n");
        }
    } else if (argc == 2) {
        p = argv[1];
        if (strcmp(p, "set")==0) {        
          td.iDate.iMonth = 1;
          td.iDate.iDay = 1;
          td.iDate.iYear = 2018;
          td.iTime.iHour = 8;
          td.iTime.iMinute = 0;
          td.iTime.iSecond = 0;
          // Set the time to 1/1/2018, 8:00:00
          if(UEZTimeDateSet(&td)) { // takes some time on LPC43XXX
            FDICmdSendString(aWorkspace, "FAIL: Not Set\n");
          } else {
            FDICmdSendString(aWorkspace, "PASS: OK\n");
          }          
        } else if (strcmp(p, "test")==0) {
          UEZTimeDateGet(&td);
          if ((td.iDate.iMonth==1) &&
                  (td.iDate.iDay == 1) &&
                  (td.iDate.iYear == 2018) &&
                  (td.iTime.iHour == 8) &&
                  (td.iTime.iMinute == 0) &&
                  (td.iTime.iSecond > 2) &&
                  (td.iTime.iSecond < 6)) {
                  FDICmdSendString(aWorkspace, "PASS: OK");
                  FDICmdPrintf(aWorkspace, ", Seconds: %u\n", td.iTime.iSecond);
          } else {
            FDICmdSendString(aWorkspace, "FAIL: Not Incrementing");
            FDICmdPrintf(aWorkspace, ", Seconds: %u\n", td.iTime.iSecond);
          }
        } else {
          FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
        }        
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[])
{
#if(UEZ_PROCESSOR != NXP_LPC4357)
    T_testData testData;
#else
    T_uezFile file;
    T_uezError error;
#endif

    if (argc == 1) {
        // Got no parameters
        // Now do the test
#if(UEZ_PROCESSOR != NXP_LPC4357)
        IUEZGUICmdRunTest(aWorkspace, FuncTestUSBHost1, &testData);
#else
        error = UEZFileOpen("0:TESTUSB.TXT", FILE_FLAG_READ_ONLY, &file);
        switch(error){
            case UEZ_ERROR_NONE:
                UEZFileClose(file);
                FDICmdSendString(aWorkspace, "PASS: OK\n");
                break;
            case UEZ_ERROR_NOT_AVAILABLE:
            case UEZ_ERROR_NOT_READY:
                FDICmdSendString(aWorkspace, "FAIL: Drive not Found\n");
                break;
            case UEZ_ERROR_NOT_FOUND:
                FDICmdSendString(aWorkspace, "FAIL: File not Found\n");
                break;
            default:
                FDICmdSendString(aWorkspace, "FAIL: Error unknown\n");
                break;
        }

#endif
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdUSBPort0(void *aWorkspace, int argc, char *argv[])
{
#if(UEZ_PROCESSOR != NXP_LPC4357)
    T_testData testData;
#endif

    if (argc == 1) {
        // Got no parameters
        // Now do the test
#if(UEZ_PROCESSOR != NXP_LPC4357)
        IUEZGUICmdRunTest(aWorkspace, FuncTestUSBHost2, &testData);
#else
        FDICmdSendString(aWorkspace, "FAIL: Not Supported\n");
#endif
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int UEZGUICmdBacklight(void *aWorkspace, int argc, char *argv[])
{
  T_testData testData;
  T_uezDevice lcd;

  if (argc == 1) {
    // Got no parameters
    // Now do the test
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
    IUEZGUICmdRunTest(aWorkspace, FuncTestBacklightPWM, &testData);
#else
    IUEZGUICmdRunTest(aWorkspace, FuncTestBacklightMonitor, &testData);
#endif
  } else if(argc == 2){
      if(UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE){
          UEZLCDBacklight(lcd, FDICmdUValue(argv[1]));
          UEZLCDClose(lcd);
          FDICmdSendString(aWorkspace, "PASS: OK\n");
      }
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
    if (argc == 1) {
        TestModeSendCmd(TEST_MODE_TOUCHSCREEN);
    } else if (argc == 2) {
        G_mmTestModeTouchscreenCalibrationBusy = ETrue;
        TestModeSendCmd(TEST_MODE_TOUCHSCREEN);

        while(G_mmTestModeTouchscreenCalibrationBusy);

        if (G_mmTestModeTouchscreenCalibrationValid)
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        else
            FDICmdSendString(aWorkspace, "FAIL: Invalid\n");
    }
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
            UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_HZ(freq));
        } else {
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_OFF);
            UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
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
    T_uezInputEvent inputEvent;
    T_uezDevice ts;
    T_uezQueue queue;

    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "TestCMDs", "\0");
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue) == UEZ_ERROR_NONE) {
            // Wait first for the screen NOT to be touched
            while (1) {
                if (UEZQueueReceive(queue, &inputEvent, 10) != UEZ_ERROR_NONE) {
                    if (inputEvent.iEvent.iXY.iAction == XY_ACTION_RELEASE)
                        break;
                }
            }

            // Wait first for the screen to be touched
            while (1) {
                if (UEZQueueReceive(queue, &inputEvent, 10) != UEZ_ERROR_NONE) {
                    if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD)
                        break;
                }
            }

            // Wait first for the screen NOT to be touched
            while (1) {
                if (UEZQueueReceive(queue, &inputEvent, 10) != UEZ_ERROR_NONE) {
                    if (inputEvent.iEvent.iXY.iAction == XY_ACTION_RELEASE)
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
#if UEZ_ENABLE_BUTTON_BOARD
        UEZTaskDelay(3000);
#else
        IWaitTouchscreen();
#endif

        G_mmTestModeColor = RGB(0, 255, 0);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
#if UEZ_ENABLE_BUTTON_BOARD
        UEZTaskDelay(3000);
#else
        IWaitTouchscreen();
#endif

        G_mmTestModeColor = RGB(0, 0, 255);
        TestModeSendCmd(TEST_MODE_FILL_COLOR);
#if UEZ_ENABLE_BUTTON_BOARD
        UEZTaskDelay(3000);
#else
        IWaitTouchscreen();
#endif

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


int UEZGUICmdI2CBang(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 I2CNumber;
    TUInt32 speed, numBytes, i;
    T_uezDevice I2C;
    TUInt8 dataIn;
    TUInt8 dataOut = 0;
    char printString[30];
    I2C_Request r;

    if (argc == 4) {
        I2CNumber = FDICmdUValue(argv[1]);
        numBytes = FDICmdUValue(argv[2]);
        speed = FDICmdUValue(argv[3]);

        if(I2CNumber == 0) {
            if(UEZI2COpen("I2C0", &I2C) != UEZ_ERROR_NONE)
                FDICmdSendString(aWorkspace, "FAIL: Could not open I2C0\n");
        } else if(I2CNumber == 1) {
            if(UEZI2COpen("I2C1", &I2C) != UEZ_ERROR_NONE)
                FDICmdSendString(aWorkspace, "FAIL: Could not open I2C1\n");
        } else if(I2CNumber == 2) {
            if(UEZI2COpen("I2C2", &I2C) != UEZ_ERROR_NONE)
                FDICmdSendString(aWorkspace, "FAIL: Could not open I2C2\n");
        } else {
            FDICmdSendString(aWorkspace, "FAIL: bus number must be 0, 1, or 2\n");
            FDICmdSendString(aWorkspace, "      Format: I2CBANG <I2C Bus #> <speed in kHz>\n");
            return 0;
        }

        if((speed != 100) && (speed != 400)) {
            FDICmdSendString(aWorkspace, "FAIL: speed must be '100' or '400' in kHz\n");
            FDICmdSendString(aWorkspace, "      Format: I2CBANG <I2C Bus #> <speed in kHz>\n");
            return 0;
        }

        r.iAddr = 0x48>>1;
        r.iSpeed = speed; //kHz
        r.iWriteData = &dataOut;
        r.iWriteLength = 1; // send 1 byte
        r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
        r.iReadData = &dataIn;
        r.iReadLength = 1; // read 1 byte
        r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

        for(i=0; i<numBytes; i++) {
            if((i%1000)==0) {
                FDICmdSendString(aWorkspace, ".");
            }

            dataOut++;
            UEZI2CTransaction(I2C, &r);

            if(dataIn != dataOut) {
                sprintf(printString, "\nTest Failed on byte %d\n", i);
                FDICmdSendString(aWorkspace, printString);
            }
        }

        UEZI2CClose(I2C);

    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters.\n");
        FDICmdSendString(aWorkspace, "      Format: I2CBANG <I2C Bus #> <speed in kHz>\n");
    }
    return 0;
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

#endif
/*-------------------------------------------------------------------------*
 * File:  TestCmds.c
 *-------------------------------------------------------------------------*/
