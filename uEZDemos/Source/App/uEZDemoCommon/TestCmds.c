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
#include <stdarg.h>
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
#include <uEZGPIO.h>

// For GPIO Array loopback test
#define FCT_TEST_INFO_LENGTH                (FDICMD_MAX_LINE_LENGTH - 8)
#define LOOPBACK_TEST_ERROR_NUM_MAX         (LOOPBACK_TEST_NUM_PINS_A * LOOPBACK_TEST_NUM_PINS_A)
#define LOOPBACK_TEST_ERROR_STRING_LENGTH   (5)
#define GPIO_TEST_RETRIES		    (3)

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
#if (UEZ_ENABLE_LOOPBACK_TEST == 1) // remove to save space
int UEZGUICmdGPIOArray(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdClock(void *aWorkspace, int argc, char *argv[]);
#endif
int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd5V(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd3VERR(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEEPROM(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdRTC(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort0(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdVerbose(void *aWorkspace, int argc, char *argv[]);

#if (configUSE_TRACE_FACILITY == 1)
int UEZGUICmdTaskInfo(void *aWorkspace, int argc, char *argv[]);
#endif

#if (UEZ_ENABLE_LCD == 1)
int UEZGUICmdBacklight(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdLCD(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreen(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreenStatus(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTouchscreenClear(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdColorCycle(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdColor(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdAlignmentBorder(void *aWorkspace, int argc, char *argv[]);
#endif
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
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum e_fct_test_result
{
    FCT_TEST_RESULT_PASSED = 0,
    FCT_TEST_RESULT_FAILED,
    FCT_TEST_RESULT_SKIPPED,
    FCT_TEST_RESULT_ABORTED,
    FCT_TEST_RESULT_OUT_OF_RANGE,
    FCT_TEST_RESULT_TIMEOUT,
    FCT_TEST_RESULT_ERROR,
} fct_test_result_t;

typedef struct
{
    T_uezError          uez_err;
    fct_test_result_t   fct_test_result;
    char                fct_test_message[FDICMD_MAX_LINE_LENGTH];
    char                fct_temp_message[FCT_TEST_INFO_LENGTH];
    uint32_t            fct_temp_message_index;
} fct_test_return_t;

void                fct_test_return_message_start(void);
void                fct_test_return_message_set_uez_err(T_uezError uez_err);
void                fct_test_return_message_set_fct_err(fct_test_result_t fct_err);
fct_test_result_t   fct_test_return_message_get_fct_err(void);
void                fct_test_return_message_info_append(const char * format, ...);
void                fct_test_return_message_finish(void);


/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static const T_consoleCmdEntry G_UEZGUICommands[] = {
        { "PING", UEZGUICmdPing },
        { "SDRAM", UEZGUICmdSDRAM },
        { "NOR", UEZGUICmdNOR },
        { "SPIFI", UEZGUICmdSPIFI },
#if (UEZ_ENABLE_LOOPBACK_TEST == 1) // remove to save space
        // { "CLK", UEZGUICmdClock }, Currently not creating a platform clock function for a feature that we only use for testing/validation.
        { "GPIOARRAY", UEZGUICmdGPIOArray },
#endif
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
        { "VERBOSE [0/1]", UEZGUICmdVerbose },
#if (UEZ_ENABLE_LCD == 1)
        { "BL", UEZGUICmdBacklight },
        { "LCD", UEZGUICmdLCD },
#if (configUSE_TRACE_FACILITY == 1)
        { "TASK", UEZGUICmdTaskInfo },
#endif
        { "TS", UEZGUICmdTouchscreen },
        { "TSSTATUS", UEZGUICmdTouchscreenStatus },
        { "TSCLEAR", UEZGUICmdTouchscreenClear },
        { "COLOR", UEZGUICmdColor },
        { "COLORCYCLE", UEZGUICmdColorCycle },
        { "ALIGN", UEZGUICmdAlignmentBorder },
#endif
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
void *G_UEZGUIWorkspace;
static TBool G_verbose;

// Interface API to standard functional test routines
static const T_testAPI G_UEZGUITestAPI = {
        UEZGUITestAddButtons,
        UEZGUITestRemoveButtons,
        UEZGUITestTextLine,
        UEZGUITestShowResult,
        UEZGUITestSetTestResult,
        UEZGUITestNextTest, };

/* Global variable used to hold status */
volatile fct_test_return_t g_fct_test_return =
{
    UEZ_ERROR_NONE,
    FCT_TEST_RESULT_PASSED,
    { 0 },
    { 0 },
    0
};
#if (UEZ_ENABLE_LOOPBACK_TEST == 1) // remove to save space
static char loopback_error_matrix[LOOPBACK_TEST_ERROR_NUM_MAX][LOOPBACK_TEST_ERROR_STRING_LENGTH];
#endif

static void strupr(char *string)
{
    while (*string)  {
        *string = toupper(*string);
        string++;
    }
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
    (void) strupr;
    if (argc == 1) {
#if UEZ_ENABLE_LCD
        TestModeSendCmd(TEST_MODE_PING);
#endif

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


#if (UEZ_ENABLE_LOOPBACK_TEST == 1) // remove to save space or when we can't use it
extern T_uezTask G_mainTask;
extern T_uezTask G_hTouchTask;
extern T_uezTask G_tsMonitorTask;
extern T_uezTask G_usbHostTask;
extern T_uezTask G_heartBeatTask;
extern T_uezTask G_DACAudioTask;

// Currently not creating a platform clock function for a feature that we only use for testing/validation.

int UEZGUICmdClock(void *aWorkspace, int argc, char *argv[])
{    
    char *p;
    (void) strupr;
#if(UEZ_PROCESSOR == NXP_LPC4357)          
     LPC_SCU->SFSCLK_0 = 1; // Set CLK0 to output pin
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif

    if (argc == 1) {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    } else if (argc == 2) {
        p = argv[1];
        strupr(p);
        if (strcmp(p, "LF")==0) {
          FDICmdSendString(aWorkspace, "Starting Low Frequency Oscillator Clock out of CLK OUT Pin!\n");
#if(UEZ_PROCESSOR == NXP_LPC4357)
          LPC_CGU->BASE_OUT_CLK = (0<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // LF Crystal
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif
        
        } else if (strcmp(p, "HF")==0) {
          FDICmdSendString(aWorkspace, "Starting High Frequency Oscillator Clock out of CLK OUT Pin!\n");
#if(UEZ_PROCESSOR == NXP_LPC4357)
          LPC_CGU->BASE_OUT_CLK = (6<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // Crystal
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif
        
        } else if (strcmp(p, "SYS")==0) {
          FDICmdSendString(aWorkspace, "Starting System Clock PLL1 out of CLK OUT Pin!\n");
#if(UEZ_PROCESSOR == NXP_LPC4357)
          LPC_CGU->BASE_OUT_CLK = (0x9<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // PLL1
          UEZTaskDelay(3000);
          FDICmdSendString(aWorkspace, "Starting DIVA out of CLK OUT Pin!\n");
          LPC_CGU->BASE_OUT_CLK = (0xC<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // DIVA
          UEZTaskDelay(3000);
          FDICmdSendString(aWorkspace, "Starting DIVB out of CLK OUT Pin!\n");
          LPC_CGU->BASE_OUT_CLK = (0xD<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // DIVB
          UEZTaskDelay(3000);
          FDICmdSendString(aWorkspace, "Starting DIVC out of CLK OUT Pin!\n");
          LPC_CGU->BASE_OUT_CLK = (0xE<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // DIVC
          UEZTaskDelay(3000);
          FDICmdSendString(aWorkspace, "Starting DIVD out of CLK OUT Pin!\n");
          LPC_CGU->BASE_OUT_CLK = (0xF<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // DIVD
          UEZTaskDelay(3000);
          FDICmdSendString(aWorkspace, "Starting DIVE out of CLK OUT Pin!\n");
          LPC_CGU->BASE_OUT_CLK = (0x10<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // DIVE
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif
          
        } else if (strcmp(p, "USB")==0) {
          FDICmdSendString(aWorkspace, "Starting USB Clock out of CLK OUT Pin!\n");
#if(UEZ_PROCESSOR == NXP_LPC4357)
          // Seems we need to disable USB first to output it.
          //LPC_CGU->BASE_OUT_CLK = (7<<24) | CGU_BASE_CGU_OUT0_CLK_AUTOBLOCK_Msk; // USB
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif

        } else if (strcmp(p, "BSP")==0) {
          FDICmdSendString(aWorkspace, "Starting BSP Delay Measure out of Heartbeat Pin!\n");
    
    RTOS_ENTER_CRITICAL();
  for(TUInt32 i  = 0; i < 5; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZBSPDelayMS(1000);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZBSPDelayMS(500);
    }
    RTOS_EXIT_CRITICAL();

        }  else if (strcmp(p, "TASK")==0) {
          FDICmdSendString(aWorkspace, "Starting Task Delay Measure out of Heartbeat Pin!\n");
#if(UEZ_PROCESSOR == NXP_LPC4357)
          LPC_CGU->BASE_OUT_CLK = 1;      // Power down output pin, 32KHz crystal, no autoblock
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif

// no mechanism in freertos to suspend all other tasks so do it manually.
  if(G_mainTask != 0) {
    UEZTaskSuspend(G_mainTask);
  }
  if(G_heartBeatTask != 0) {
    UEZTaskSuspend(G_heartBeatTask);
  }
  if(G_tsMonitorTask != 0) {
    UEZTaskSuspend(G_tsMonitorTask);
  }
  if(G_usbHostTask != 0) {
    UEZTaskSuspend(G_usbHostTask);
  }
  if(G_DACAudioTask != 0) {
    UEZTaskSuspend(G_DACAudioTask);
  }

for(TUInt32 i  = 0; i < 10; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(2);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(1);
    }

  for(TUInt32 i  = 0; i < 5; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(1000);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(500);
    }

  if(G_mainTask != 0) {
    UEZTaskResume(G_mainTask);
  }
  if(G_heartBeatTask != 0) {
    UEZTaskResume(G_heartBeatTask);
  }
  if(G_tsMonitorTask != 0) {
    UEZTaskResume(G_tsMonitorTask);
  }
  if(G_usbHostTask != 0) {
    UEZTaskResume(G_usbHostTask);
  }
  if(G_DACAudioTask != 0) {
    UEZTaskResume(G_DACAudioTask);
  }

#if(UEZ_PROCESSOR == NXP_LPC4357)
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif
        } else{
          FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
          
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }


    UEZTaskDelay(10000);

#if(UEZ_PROCESSOR == NXP_LPC4357)          
    LPC_CGU->BASE_OUT_CLK = 1;      // Power down output pin, 32KHz crystal, no autoblock
    LPC_SCU->SFSCLK_0 = 0; // set CKL pin back to reset state
#endif

#if(UEZ_PROCESSOR == NXP_LPC4088)          
#endif
    return 0;
}

int UEZGUICmdGPIOArray(void *aWorkspace, int argc, char *argv[]) {
  T_uezError uez_err = UEZ_ERROR_NONE;

  /* Set retries to 4 to get it to run up to 4 times (3 retries) */
  uint8_t retries = GPIO_TEST_RETRIES;
  uint8_t write_pin_index = 0;
  uint8_t read_pin_index = 0;
  
  TUInt32 new_timeout_value, pin_array_to_test, num_pins_in_tested_array;
  T_uezGPIOPortPin * array_to_test_ptr;
  uint8_t (*loopback_connected_ptr)[LOOPBACK_TEST_NUM_PINS_A*LOOPBACK_TEST_NUM_PINS_A];
  (void) loopback_connected_ptr;
  (void) array_to_test_ptr;

  /* Arrays for holding read values */
  TBool loopback_test_pin_level_low[LOOPBACK_TEST_NUM_PINS_A];
  TBool loopback_test_pin_level_high[LOOPBACK_TEST_NUM_PINS_A];
  TBool isPinLocked = ETrue;

  fct_test_return_message_start();

  fct_test_return_message_set_fct_err(FCT_TEST_RESULT_PASSED);

  
  if (argc > 1) { // Got parameter to change the timeout value      /
      new_timeout_value = FDICmdUValue(argv[1]);
      if (argc > 2) { // got parameter to test a different pin array
          pin_array_to_test = FDICmdUValue(argv[2]);
          if(pin_array_to_test == 1) {
            array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_A;
            loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_A;
            num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_A;
          } else if(pin_array_to_test == 2) { // tested ok in Crossworks project
            array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_B;
            loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_B;
            num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_B;
          } else if(pin_array_to_test == 3) {
            array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_C;
            loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_C;
            num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_C;
          } else {
            array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_A;
            loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_A;
            num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_A;
          }
          if (argc > 3) { // future use
          }
      } else { // no parameters so test default array
        array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_A;
        loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_A;
        num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_A;
      }
  } else { // no parameters so test default array at default timing value.
      new_timeout_value = 100;
      array_to_test_ptr = (T_uezGPIOPortPin *) &g_loopback_pins_A;
      loopback_connected_ptr = (uint8_t (*)[]) g_loopback_connected_A;
      num_pins_in_tested_array = LOOPBACK_TEST_NUM_PINS_A;
  }
  
  // Check all pins for in-use
  for (write_pin_index = 0; write_pin_index < num_pins_in_tested_array; write_pin_index++) {
    isPinLocked = UEZGPIOGetLock(array_to_test_ptr[write_pin_index]);
    if (isPinLocked == ETrue) {
      fct_test_return_message_set_fct_err(FCT_TEST_RESULT_ERROR);
      // Append to error string
      fct_test_return_message_info_append("Pin Index %d/%u is locked/in-use ", write_pin_index, array_to_test_ptr[write_pin_index]);
    }
  }

  if (fct_test_return_message_get_fct_err() == FCT_TEST_RESULT_PASSED) { // If any pin is locked don't try to run the test!
    while ((retries > 0)) {
      fct_test_return_message_set_fct_err(FCT_TEST_RESULT_PASSED);

      if (retries != GPIO_TEST_RETRIES) {
        fct_test_return_message_info_append(", ");
      }
      retries--;

      /* Clear the arrays that will be used to hold the pin levels */
      memset(loopback_test_pin_level_low, 0xFF, sizeof(loopback_test_pin_level_low));
      memset(loopback_test_pin_level_high, 0xFF, sizeof(loopback_test_pin_level_high));
      memset(loopback_error_matrix, 0x00, sizeof(loopback_error_matrix));

      /* For each pin, perform the following: */
      for (write_pin_index = 0; write_pin_index < num_pins_in_tested_array; write_pin_index++) {
        /* Configure all pins as inputs */
        for (read_pin_index = 0; read_pin_index < num_pins_in_tested_array; read_pin_index++) {

#if(UEZ_PROCESSOR != NXP_LPC4357)
          uez_err = UEZGPIOSetMux(array_to_test_ptr[read_pin_index], 0);
#else     // TODO this might work but not tested yet
          UEZGPIOSetMux(array_to_test_ptr[read_pin_index], (array_to_test_ptr[read_pin_index] >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
          UEZGPIOControl(array_to_test_ptr[read_pin_index], GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

          uez_err = UEZGPIOInput(array_to_test_ptr[read_pin_index]);
          uez_err = UEZGPIOSetPull(array_to_test_ptr[read_pin_index], GPIO_PULL_UP); // may not work on lpc4357, but pins default to pull-up mode
        }

        /* Set write pin as output initial high */
        uez_err = UEZGPIOOutput(array_to_test_ptr[write_pin_index]);
        uez_err = UEZGPIOSet(array_to_test_ptr[write_pin_index]);

        UEZBSPDelayMS(new_timeout_value); // there may be issues in uEZ where other tasks can switch pin states.

        /* Read all the pins */
        for (read_pin_index = 0; read_pin_index < num_pins_in_tested_array; read_pin_index++) {
          loopback_test_pin_level_high[read_pin_index] = UEZGPIORead(array_to_test_ptr[read_pin_index]);
        }

        /* Set write pin as output initial low */
        uez_err = UEZGPIOOutput(array_to_test_ptr[write_pin_index]);
        uez_err = UEZGPIOClear(array_to_test_ptr[write_pin_index]);

        UEZBSPDelayMS(new_timeout_value); // there may be issues in uEZ where other tasks can switch pin states.

        /* Read all the pins */
        for (read_pin_index = 0; read_pin_index < num_pins_in_tested_array; read_pin_index++) {
          loopback_test_pin_level_low[read_pin_index] = UEZGPIORead(array_to_test_ptr[read_pin_index]);
        }

        /* Validate the values */
        for (read_pin_index = 0; read_pin_index < num_pins_in_tested_array; read_pin_index++) {
          /* If this read_pin is connected to this write_pin... */
          if (loopback_connected_ptr[0][(write_pin_index*num_pins_in_tested_array)+read_pin_index] == 1) {
            /* If it didn't toggle like it should have */
            if (loopback_test_pin_level_high[read_pin_index] == loopback_test_pin_level_low[read_pin_index]) {
              /* Open circuit between pins read_pin_index and write_pin_index */
              fct_test_return_message_set_fct_err(FCT_TEST_RESULT_ERROR);

              /* Append to error string */
              fct_test_return_message_info_append("O%d_%d-%c   ", write_pin_index, read_pin_index, (loopback_test_pin_level_high[read_pin_index] == ETrue) ? 'H' : 'L');
            }
          } else {
            /* If it did toggle when it shouldn't have */
            if (loopback_test_pin_level_high[read_pin_index] != loopback_test_pin_level_low[read_pin_index]) {
              /* Short circuit between pins read_pin_index and write_pin_index */
              fct_test_return_message_set_fct_err(FCT_TEST_RESULT_ERROR);

              /* Append to error string */
              fct_test_return_message_info_append("S%d_%d   ", write_pin_index, read_pin_index);
            }
          }
        }
      }

      fct_test_return_message_set_uez_err(uez_err);
      if (fct_test_return_message_get_fct_err() == FCT_TEST_RESULT_PASSED) {
        fct_test_return_message_info_append("OK");
        break;
      }
    } // end retry loop

    fct_test_return_message_info_append(", %d retries", (GPIO_TEST_RETRIES - 1 - retries));
  } else { // One or more pins were locked and we can't run this function.
  }

  fct_test_return_message_finish();

  /* Review results of test */
  switch (g_fct_test_return.fct_test_result) {
  case FCT_TEST_RESULT_ABORTED:
    FDICmdPrintf(aWorkspace, "FAIL: Incorrect parameters");
    break;
  case FCT_TEST_RESULT_PASSED:
  case FCT_TEST_RESULT_FAILED:
  case FCT_TEST_RESULT_ERROR:
  case FCT_TEST_RESULT_SKIPPED:
  case FCT_TEST_RESULT_OUT_OF_RANGE:
  case FCT_TEST_RESULT_TIMEOUT:
  default:
    FDICmdPrintf(aWorkspace, (const char *)&g_fct_test_return.fct_test_message);
    break;
  }
  return 0;
}
#endif

int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 portA, pinA, portB, pinB, portC, pinC;
    //T_testData testData;
    extern HAL_GPIOPort **PortNumToPort(TUInt8 aPort);
    HAL_GPIOPort **p_portA;
    HAL_GPIOPort **p_portB;
    HAL_GPIOPort **p_portC;
    TUInt32 high, low;
    TBool isPinLocked = ETrue;
#if(UEZ_PROCESSOR != NXP_LPC4357)
     
#else
    //TUInt32 value;
#endif

    if (argc == 5) { // 2-way test only supports LPC17XX_40XX and maybe LPC546XX, partial support for LPC4357
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

        isPinLocked = UEZGPIOGetLock(UEZ_GPIO_PORT_PIN(portA,pinA)); // Check pin for in-use
        if (isPinLocked == ETrue) {
          FDICmdPrintf(aWorkspace, "FAIL: First GPIO Locked and in-use!\n");
          return -1;
        }

        isPinLocked = UEZGPIOGetLock(UEZ_GPIO_PORT_PIN(portB,pinB)); // Check pin for in-use
        if (isPinLocked == ETrue) {
          FDICmdPrintf(aWorkspace, "FAIL: Second GPIO Locked and in-use!\n");
          return -1;
        }

        // Now do the test

        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portB)->SetMux(p_portB, pinB, 0); // set to GPIO
#else
        //(*p_portB)->SetMux(p_portB, pinB, (pinB > 4)? 4 : 0); // set to GPIO
        //value = SCU_NORMAL_DRIVE_DEFAULT(0);
        //value |= (pinB > 4)? 4 :0;
        //(*p_portB)->Control(p_portB, pinB, GPIO_CONTROL_SET_CONFIG_BITS, value);
        
        // TODO this might work
        //UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portB,pinB), (UEZ_GPIO_PORT_PIN(portB,pinB) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        //UEZGPIOControl(UEZ_GPIO_PORT_PIN(portB,pinB), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);

        (*p_portA)->SetOutputMode(p_portA, 1 << pinA); // set to Output
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portA)->SetMux(p_portA, pinA, 0); // set to GPIO
#else
        //(*p_portA)->SetMux(p_portA, pinA, (pinA > 4)? 4 : 0); // set to GPIO
        //value = SCU_NORMAL_DRIVE_DEFAULT(0);
        //value |= (pinA > 4)? 4 :0;
        //(*p_portA)->Control(p_portA, pinA, GPIO_CONTROL_SET_CONFIG_BITS, value);
        
        // TODO this might work
        //UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portA,pinA), (UEZ_GPIO_PORT_PIN(portA,pinA) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        //UEZGPIOControl(UEZ_GPIO_PORT_PIN(portA,pinA), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

        // Pause
        UEZBSPDelayMS(GPIO_PIN_SETTLE_TIME);

        // Read high setting
        (*p_portB)->Read(p_portB, 1 << pinB, &high);

        // Now change to a low setting
        (*p_portA)->Clear(p_portA, 1 << pinA);

        // Pause
        UEZBSPDelayMS(GPIO_PIN_SETTLE_TIME);

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
    } else if (argc == 7) { // 3-way test only supports LPC17XX_40XX and maybe LPC546XX
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
        
        isPinLocked = UEZGPIOGetLock(UEZ_GPIO_PORT_PIN(portA,pinA)); // Check pin for in-use
        if (isPinLocked == ETrue) {
          FDICmdPrintf(aWorkspace, "FAIL: First GPIO Locked and in-use!\n");
          return -1;
        }

        isPinLocked = UEZGPIOGetLock(UEZ_GPIO_PORT_PIN(portB,pinB)); // Check pin for in-use
        if (isPinLocked == ETrue) {
          FDICmdPrintf(aWorkspace, "FAIL: Second GPIO Locked and in-use!\n");
          return -1;
        }

        isPinLocked = UEZGPIOGetLock(UEZ_GPIO_PORT_PIN(portC,pinC)); // Check pin for in-use
        if (isPinLocked == ETrue) {
          FDICmdPrintf(aWorkspace, "FAIL: Third GPIO Locked and in-use!\n");
          return -1;
        }

        // Reset pins back to default state
        (*p_portA)->SetInputMode(p_portA, 1 << pinA);            // set to Input
        (*p_portA)->SetPull(p_portA, 1 << pinA, GPIO_PULL_NONE); // set to floating input mode

#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portA)->SetMux(p_portA, pinA, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portA,pinA), (UEZ_GPIO_PORT_PIN(portA,pinA) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portA,pinA), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

        (*p_portB)->SetInputMode(p_portB, 1 << pinB);            // set to Input
        (*p_portB)->SetPull(p_portB, 1 << pinB, GPIO_PULL_NONE); // set to floating input mode

#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portB)->SetMux(p_portB, pinB, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portB,pinB), (UEZ_GPIO_PORT_PIN(portB,pinB) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portB,pinB), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

        (*p_portC)->SetInputMode(p_portC, 1 << pinC);            // set to Input
        (*p_portC)->SetPull(p_portC, 1 << pinC, GPIO_PULL_NONE); // set to floating input mode

#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portC)->SetMux(p_portC, pinC, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portC,pinC), (UEZ_GPIO_PORT_PIN(portC,pinC) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portC,pinC), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif

        // Test Pair AB
        (*p_portA)->SetOutputMode(p_portA, 1 << pinA); // set to Output
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portA)->SetMux(p_portA, pinA, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portA,pinA), (UEZ_GPIO_PORT_PIN(portA,pinA) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portA,pinA), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portB)->SetMux(p_portB, pinB, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portB,pinB), (UEZ_GPIO_PORT_PIN(portB,pinB) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portB,pinB), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);
        UEZBSPDelayMS(1);         // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &high); // Read high setting
        (*p_portA)->Clear(p_portA, 1 << pinA); // Now change to a low setting
        UEZBSPDelayMS(1); // Pause
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
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portC)->SetMux(p_portC, pinC, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portC,pinC), (UEZ_GPIO_PORT_PIN(portC,pinC) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portC,pinC), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        (*p_portB)->SetPull(p_portB, pinB, GPIO_PULL_UP); // set to Pull up
        (*p_portB)->SetInputMode(p_portB, 1 << pinB); // set to Input
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portB)->SetMux(p_portB, pinB, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portB,pinB), (UEZ_GPIO_PORT_PIN(portB,pinB) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portB,pinB), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        // Drive 1 on this pin
        (*p_portC)->Set(p_portC, 1 << pinC);
        UEZBSPDelayMS(1);         // Pause
        (*p_portB)->Read(p_portB, 1 << pinB, &high); // Read high setting
        (*p_portC)->Clear(p_portC, 1 << pinC); // Now change to a low setting
        UEZBSPDelayMS(1); // Pause
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
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portA)->SetMux(p_portA, pinA, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portA,pinA), (UEZ_GPIO_PORT_PIN(portA,pinA) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portA,pinA), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        (*p_portC)->SetPull(p_portC, pinC, GPIO_PULL_UP); // set to Pull up
        (*p_portC)->SetInputMode(p_portC, 1 << pinC); // set to Input
#if(UEZ_PROCESSOR != NXP_LPC4357)
        (*p_portC)->SetMux(p_portC, pinC, 0);                    // set to GPIO
#else
        // TODO this might work
        UEZGPIOSetMux(UEZ_GPIO_PORT_PIN(portC,pinC), (UEZ_GPIO_PORT_PIN(portC,pinC) >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        UEZGPIOControl(UEZ_GPIO_PORT_PIN(portC,pinC), GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
#endif
        // Drive 1 on this pin
        (*p_portA)->Set(p_portA, 1 << pinA);
        UEZBSPDelayMS(1);         // Pause
        (*p_portC)->Read(p_portC, 1 << pinC, &high); // Read high setting
        (*p_portA)->Clear(p_portA, 1 << pinA); // Now change to a low setting
        UEZBSPDelayMS(1); // Pause
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
    } else if (argc == 4) { // Port Pin H/L command goes here:
        // Got all 3 parameters, parse them
        portA = FDICmdUValue(argv[1]);
        pinA = FDICmdUValue(argv[2]);
        //level = FDICmdUValue(argv[3]); // TODO
        









    } else if (argc == 1) {
        //IUEZGUICmdRunTest(aWorkspace, FuncTestGPIOs, &testData); // not working currently
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
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

        //UEZTaskDelay(5000);
        UEZTaskDelay(3000);

        UEZTimeDateGet(&td);
        if ((td.iDate.iMonth==1) &&
            (td.iDate.iDay == 1) &&
            (td.iDate.iYear == 2018) &&
            (td.iTime.iHour == 8) &&
            (td.iTime.iMinute == 0) &&
            (td.iTime.iSecond > 1))
        {
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

#if (UEZ_ENABLE_LCD == 1)
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
#endif

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

#if (configUSE_TRACE_FACILITY == 1)
#include <task.h>
int UEZGUICmdTaskInfo(void *aWorkspace, int argc, char *argv[])
{  
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	uint32_t ulTotalRunTime;
        
	// Take a snapshot of the number of tasks in case it changes while this function is executing.
	uxArraySize = uxTaskGetNumberOfTasks();

	// Allocate a TaskStatus_t structure for each task. An array could be allocated statically at compile time.
	pxTaskStatusArray = (TaskStatus_t *) pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

	if( pxTaskStatusArray != NULL ) {
		// Generate raw status information about each task.
		uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

#if (configGENERATE_RUN_TIME_STATS == 1)
		uint32_t  ulStatsAsPercentage; // Not reporting runtime currently
		ulTotalRunTime /= 100UL; // For percentage calculations.
#endif

		for( x = 0; x < uxArraySize; x++ ) { // For each populated position in the pxTaskStatusArray array
			printf("%10s: ", pxTaskStatusArray[x].pcTaskName);

			switch(pxTaskStatusArray[x].eCurrentState){
			case 0:
				printf("RUN");
				break;
			case 1:
				printf("RDY");
				break;
			case 2:
				printf("BLK");
				break;
			case 3:
				printf("SUS");
				break;
			case 4:
				printf("DEL");
				break;
			case 5:
			default:
				printf("INV");
				break;
			}
			printf(", Prio: %02u, Base: %02u, ", (uint32_t) pxTaskStatusArray[x].uxCurrentPriority, (uint32_t) pxTaskStatusArray[x].uxBasePriority);
			printf("Free Stack Remaining: %04u\n", pxTaskStatusArray[x].usStackHighWaterMark);

#if (configGENERATE_RUN_TIME_STATS == 1)
			if( ulTotalRunTime > 0 ) {
				// What percentage of the total run time has the task used? This will always be rounded down to the nearest integer.
				// ulTotalRunTimeDiv100 has already been divided by 100.
				ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter / ulTotalRunTime;

				if( ulStatsAsPercentage > 0UL )	{
					printf("%s\t\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[x].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
				} else {
					// If the percentage is zero here then the task has consumed less than 1% of the total run time.
					printf("%s\t\t%lu\t\t<1%%\r\n", pxTaskStatusArray[x].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
				}
			}
#endif
		}
		vPortFree( pxTaskStatusArray ); // The array is no longer needed, free the memory it consumes.
                
                //printf("Free Heap Remaining: %04u, MinEver Heap Size:  %04u\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize()); // TODO this requires HEAP 4 or 5.
		printf("PASS: OK");
	} else {
		printf("FAIL: Mem Alloc");
	}

	//if(G_Verbose) {}
    return 0;
}
#endif

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

#if (UEZ_ENABLE_LCD == 1)
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
#endif

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

    // Verbose is off by default
    G_verbose = EFalse;

    // Open the console serial port if available
    error = UEZStreamOpen("Console", &stream);
    if (error)
        return error;

    // Start FDI Cmd Console
    G_UEZGUIWorkspace = 0;
#if (MAX_NUM_FRAMES > 0) // SDRAM is used
error = FDICmdStart(stream, &G_UEZGUIWorkspace, 3072, G_UEZGUICommands);
#else // SDRAM not used, use smaller size
error = FDICmdStart(stream, &G_UEZGUIWorkspace, 2560, G_UEZGUICommands);
#endif    
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


#define STORAGE_DIVIDER     1024

T_uezError Storage_PrintInfo(char driveLetter, TBool printToConsole)
{
    char drivePath[4] = {driveLetter,':','/',0};
    uint64_t totalBytes = 0;
    float sizeDecimal = 0.0;

    if(printToConsole == ETrue){
      printf("\nDrive %c information:\n", driveLetter);
    }

    T_msSizeInfo aDeviceInfo;
    T_uezFileSystemVolumeInfo aFSInfo;
    if (UEZFileSystemGetStorageInfo(drivePath, &aDeviceInfo) == UEZ_ERROR_NONE) {
        if(printToConsole == ETrue){
          totalBytes = ((uint64_t) aDeviceInfo.iNumSectors) * aDeviceInfo.iSectorSize;
          sizeDecimal = totalBytes/STORAGE_DIVIDER/STORAGE_DIVIDER; // MB
          printf("Storage Medium Report:\n  Sectors: %u\n", aDeviceInfo.iNumSectors);
          printf("  Sector Size: %u\n  Block Size: %u\n",
              aDeviceInfo.iSectorSize, aDeviceInfo.iBlockSize);

          if(sizeDecimal > STORAGE_DIVIDER) {
            sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // GB 
            if(sizeDecimal > STORAGE_DIVIDER) {
              sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // TB 
              if(sizeDecimal > STORAGE_DIVIDER) {
                sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // PB
                printf("  Total Size: %f PB\n", (double) sizeDecimal); // In a real project you probably want to force printf into normal float.
              } else {          
                printf("  Total Size: %f TB\n", (double) sizeDecimal);
              }
            } else {          
              printf("  Total Size: %f GB\n", (double) sizeDecimal);
            }
          } else {
            printf("  Total Size: %f MB\n", (double) sizeDecimal);
          }
        }
            
        if (UEZFileSystemGetVolumeInfo(drivePath, &aFSInfo) == UEZ_ERROR_NONE) {
            if(printToConsole == ETrue){
                printf("File System Report:\n");
                printf("  Bytes Per Sector: %u\n  Sectors Per Cluster: %u\n",
                    aFSInfo.iBytesPerSector, aFSInfo.iSectorsPerCluster);
                printf("  Num Clusters Total: %u\n  Num Clusters Free: %u\n",
                    aFSInfo.iNumClustersTotal, aFSInfo.iNumClustersFree);                
            }
                return UEZ_ERROR_NONE; // SD card found with filesystem
        } else {
           if(printToConsole == ETrue){         
              printf("  Could not read file system\n");
           }
        }
    } else {
       if(printToConsole == ETrue){
            printf("  Not initialized yet\n");
       }
    }
    return UEZ_ERROR_NOT_FOUND; // didn't find an SD card/flash drive and/or file system.
}

void fct_test_return_message_start(void)
{
    /* Initialize return result */
    memset ((void *)&g_fct_test_return, 0, sizeof(g_fct_test_return));
}

void fct_test_return_message_set_uez_err(T_uezError uez_err)
{
    g_fct_test_return.uez_err = uez_err;
}

void fct_test_return_message_set_fct_err(fct_test_result_t fct_err)
{
    g_fct_test_return.fct_test_result = fct_err;
}

fct_test_result_t fct_test_return_message_get_fct_err(void)
{
    return g_fct_test_return.fct_test_result;
}

void fct_test_return_message_info_append(const char * format, ...)
{
    va_list args;
    int len = 0;

    if(g_fct_test_return.fct_temp_message_index < FCT_TEST_INFO_LENGTH)
    {		
        va_start(args, format);
        len = vsprintf((char *) &g_fct_test_return.fct_temp_message[g_fct_test_return.fct_temp_message_index],
                format, args);

        g_fct_test_return.fct_temp_message_index += len;
        va_end(args);
    }
}

void fct_test_return_message_finish(void)
{
    switch(g_fct_test_return.fct_test_result)
    {
        case FCT_TEST_RESULT_PASSED:
            sprintf ((char *) g_fct_test_return.fct_test_message, "PASS: %s", g_fct_test_return.fct_temp_message);
            break;
        default:
            sprintf ((char *) g_fct_test_return.fct_test_message, "FAIL: %s", g_fct_test_return.fct_temp_message);
            break;
    }
}


#endif
/*-------------------------------------------------------------------------*
 * File:  TestCmds.c
 *-------------------------------------------------------------------------*/
