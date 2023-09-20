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
#include <stdlib.h>
#include <uEZ.h>
#if 1//APP_MENU_ALLOW_TEST_MODE
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
#include <UEZEEPROM.h>
#include <UEZFile.h>
#include "Configuration_EEPROM.h"
#include "Cell_Modem.h"

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
int UEZGUICmdSPIFI(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdGPIO(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEEPROM(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEEPROM1(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdRTC(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdUSBPort1(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdBacklight(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdLCD(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdTemperature(void *aWorkspace, int argc, char *argv[]);
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
int UEZGUICmdCRC(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdWebpage(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_LED(void *aWorkspace, int argc, char *argv[]);

int UEZGUICmd_LairdWiFi(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_LairdBLE(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_BG121BLE(void *aWorkspace, int argc, char *argv[]);

int UEZGUICmd_CellIMEI(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_CellCCID(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_CellFWV(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_CellMFWV(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmd_CellHWV(void *aWorkspace, int argc, char *argv[]);

int UEZGUICmdPMOD(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdXBEE(void *aWorkspace, int argc, char *argv[]);
int UEZGUICmdEXP(void *aWorkspace, int argc, char *argv[]);

#if SEC_TO_ENABLED //Trusted Objects enabled
int UEZGUICmdSETO(void *aWorkspace, int argc, char *argv[]);
#endif
#if SEC_MC_ENABLED //Microchip enabled
int UEZGUICmdSEMC(void *aWorkspace, int argc, char *argv[]);
#endif
int UEZGUICmdReset(void *aWorkspace, int argc, char *argv[]);

int UEZGUICmdUbiquiosKey(void *aWorkspace, int argc, char *argv[]);

int UEZGUICmdSetBoardVersion(void *aWorkspace, int argc, char *argv[]);

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
        { "SPIFI", UEZGUICmdSPIFI },
        { "GPIO", UEZGUICmdGPIO }, //TODO: Modify this test for GPIO loopback used on GW1 (TE)
        
        { "PMODL", UEZGUICmdPMOD },
        { "XBEEL", UEZGUICmdXBEE },
        { "EXPL", UEZGUICmdEXP },        
        
        { "EEPROM", UEZGUICmdEEPROM },
        { "EEPROM1", UEZGUICmdEEPROM1 }, //For basic testing of the I2C EEPROM, FCT should use "MACADDR"
        
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
        { "USB1", UEZGUICmdUSBPort1 },
        { "VERBOSE", UEZGUICmdVerbose },
        { "TEMP", UEZGUICmdTemperature },
        { "MACADDR", UEZGUICmdMACAddress }, //Modified to write to the I2C EEPROM on GW1
        { "IPADDR", UEZGUICmdIPAddress },
        { "IPMASK", UEZGUICmdIPMaskAddress },
        { "IPGATEWAY", UEZGUICmdIPGatewayAddress },
        { "I2CPROBE", UEZCmdI2CProbe },
        { "I2CWRITE", UEZCmdI2CWrite },
        { "I2CREAD", UEZCmdI2CRead },
        { "FILETESTWRITE", UEZCmdFileTestWrite },
        { "I2CBANG", UEZGUICmdI2CBang },
        { "CRC" , UEZGUICmdCRC},
        { "WEBPAGE", UEZGUICmdWebpage }, 
        { "LED", UEZGUICmd_LED},
        
        { "L_WIFI", UEZGUICmd_LairdWiFi},
        { "L_BLE", UEZGUICmd_LairdBLE},
        { "B_BLE", UEZGUICmd_BG121BLE},
        
        { "C_IMEI", UEZGUICmd_CellIMEI},
        { "C_CCID", UEZGUICmd_CellCCID},
        { "C_HWV", UEZGUICmd_CellHWV},
        { "C_MFWV", UEZGUICmd_CellMFWV},
        { "C_FWV", UEZGUICmd_CellFWV},

        { "UB_KEY", UEZGUICmdUbiquiosKey},

        { "HW_VERSION", UEZGUICmdSetBoardVersion},

        //Note: this functions are not needed for the headless unit but were left
        //for future uses with headed builds.
        //{ "TEMP", UEZGUICmdTemperature },
        //{ "BL", UEZGUICmdBacklight },
        //{ "LCD", UEZGUICmdLCD },
        //{ "TS", UEZGUICmdTouchscreen },
        //{ "TSSTATUS", UEZGUICmdTouchscreenStatus },
        //{ "TSCLEAR", UEZGUICmdTouchscreenClear },
        //{ "COLOR", UEZGUICmdColor },
        //{ "COLORCYCLE", UEZGUICmdColorCycle },
        //{ "ALIGN", UEZGUICmdAlignmentBorder },
        //{ "ACCEL", UEZGUICmdAccelerometer },
        //{ "SPEAKER", UEZGUICmdSpeaker },
#if UEZ_ENABLE_AUDIO_AMP //Audio amp disabled in Config_Build.h, no audio on headless unit
        { "AMP", UEZGUICmdAmplifier },
        { "AUDIODAC", UEZCmdAudioDAC },
        { "WAVPLAY", UEZCmdWAVPlay },
#endif
        
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

int UEZGUICmdSetBoardVersion(void *aWorkspace, int argc, char *argv[])
{
    char message[50];

    if (argc == 2) {
        if(Configuration_EEPROM_Program_HW_V((TUInt8*)argv[1]) == UEZ_ERROR_NONE){
            sprintf(message, "PASS: %s\n", argv[1]);
            FDICmdSendString(aWorkspace, message);
        } else {
            FDICmdSendString(aWorkspace, "FAIL: HW Version Not Set\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_CellIMEI(void *aWorkspace, int argc, char *argv[])
{
    char imei[20];
    char message[50];

    if (argc == 1) {
        if(Cell_Modem_GetIMEI(imei)){
            sprintf(message, "PASS: %s\n", imei);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(15000);
            if(Cell_Modem_GetIMEI(imei)){
                sprintf(message, "PASS: %s\n", imei);
                FDICmdSendString(aWorkspace, message);
            } else {
                FDICmdSendString(aWorkspace, "FAIL: IMEI Not Read\n");
            }
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_CellCCID(void *aWorkspace, int argc, char *argv[])
{
    char ccid[25];
    char message[50];

    if (argc == 1) {
        if(Cell_Modem_GetCCID(ccid)){
            sprintf(message, "PASS: %s\n", ccid);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(20000);
            if(Cell_Modem_GetCCID(ccid)){
                sprintf(message, "PASS: %s\n", ccid);
                FDICmdSendString(aWorkspace, message);
            } else {
                UEZTaskDelay(2000);
                if(Cell_Modem_GetCCID(ccid)){
                    sprintf(message, "PASS: %s\n", ccid);
                    FDICmdSendString(aWorkspace, message);
                } else {
                    FDICmdSendString(aWorkspace, "FAIL: CCID Not Read\n");
                }
            }
            FDICmdSendString(aWorkspace, "FAIL: CCID Not Read\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_CellFWV(void *aWorkspace, int argc, char *argv[])
{
    char FWV[25];
    char message[50];

    if (argc == 1) {
        if(Cell_Modem_GetFWV(FWV)){
            sprintf(message, "PASS: %s\n", FWV);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(20000);
            if(Cell_Modem_GetFWV(FWV)){
                sprintf(message, "PASS: %s\n", FWV);
                FDICmdSendString(aWorkspace, message);
            } else {
                UEZTaskDelay(2000);
                if(Cell_Modem_GetFWV(FWV)){
                    sprintf(message, "PASS: %s\n", FWV);
                    FDICmdSendString(aWorkspace, message);
                } else {
                    FDICmdSendString(aWorkspace, "FAIL: FWV Not Read\n");
                }
            }
            FDICmdSendString(aWorkspace, "FAIL: FWV Not Read\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_CellMFWV(void *aWorkspace, int argc, char *argv[])
{
    char MFWV[25];
    char message[50];

    if (argc == 1) {
        if(Cell_Modem_GetMFWV(MFWV)){
            sprintf(message, "PASS: %s\n", MFWV);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(20000);
            if(Cell_Modem_GetMFWV(MFWV)){
                sprintf(message, "PASS: %s\n", MFWV);
                FDICmdSendString(aWorkspace, message);
            } else {
                UEZTaskDelay(2000);
                if(Cell_Modem_GetMFWV(MFWV)){
                    sprintf(message, "PASS: %s\n", MFWV);
                    FDICmdSendString(aWorkspace, message);
                } else {
                    FDICmdSendString(aWorkspace, "FAIL: MFWV Not Read\n");
                }
            }
            FDICmdSendString(aWorkspace, "FAIL: MFWV Not Read\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_CellHWV(void *aWorkspace, int argc, char *argv[])
{
    char HWV[25];
    char message[50];

    if (argc == 1) {
        if(Cell_Modem_GetHWV(HWV)){
            sprintf(message, "PASS: %s\n", HWV);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(20000);
            if(Cell_Modem_GetHWV(HWV)){
                sprintf(message, "PASS: %s\n", HWV);
                FDICmdSendString(aWorkspace, message);
            } else {
                UEZTaskDelay(2000);
                if(Cell_Modem_GetHWV(HWV)){
                    sprintf(message, "PASS: %s\n", HWV);
                    FDICmdSendString(aWorkspace, message);
                } else {
                    FDICmdSendString(aWorkspace, "FAIL: HWV Not Read\n");
                }
            }
            FDICmdSendString(aWorkspace, "FAIL: HWV Not Read\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmdUbiquiosKey(void *aWorkspace, int argc, char *argv[])
{
    char message[50];
    TUInt8 ubiquiosKey[99];
    TUInt8 i, j = 0;;
    TUInt8 number[3];

    if (argc == 1) {
        if(G_ConfigurationData.iValidConfigurations & UBIQUIOS_KEY_VALID == UBIQUIOS_KEY_VALID){
            sprintf(message, "PASS: %.9s\n", G_ConfigurationData.iUbiquiosKeyNumber);
        } else {
            sprintf(message, "PASS: %s\n", "FFFFFFFFF");
        }
        FDICmdSendString(aWorkspace, message);
    } else if (argc == 3){
        memset((void*)&ubiquiosKey, 0, sizeof(ubiquiosKey));
        memset((void*)&number, 0, sizeof(number));
        for(i = 0; i < 198; i+=2){
            number[0] = argv[2][i];
            number[1] = argv[2][i+1];
            ubiquiosKey[j++] = (TUInt8)strtol((const char*)number, NULL, 16);
        }

        if(Configuration_EEPROM_Program_UbiquiosKey((TUInt8*)argv[1], ubiquiosKey) == UEZ_ERROR_NONE){
            sprintf(message, "PASS: %.9s\n", G_ConfigurationData.iUbiquiosKeyNumber);
        } else {
            sprintf(message, "FAIL: %s\n", "FFFFFFFFF");
        }
        FDICmdSendString(aWorkspace, message);
    }else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
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
#else
    if (argc == 2) {
        // Got no parameters
        // Now do the test
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
    
    UEZPlatform_I2C0_Require();
    UEZPlatform_CRC0_Require();
    
#include <uEZCRC.h>
   T_uezDevice crc;
    
    // turn on power here if needed
    //UEZTaskDelay(10); // Power on delay
    
    if(UEZI2COpen("I2C0", &I2C) != UEZ_ERROR_NONE){
        FDICmdSendString(aWorkspace, "FAIL: Could not open I2C0\n");
    }    
    UEZI2CResetBus(I2C);    
    // send wakeup command
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

    // next send info command to find out chip model and revision
    numBytes = 8;
    dataOut[0] = 0x03; // command mode (not counted in the seven byte count of subsequent bytes)
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
    
    /*FDICmdPrintf(aWorkspace, "TX: %X,%X,%X,%X,%X,%X,%X,%X\n", dataOut[0], dataOut[1], dataOut[2], 
                     dataOut[3], dataOut[4], dataOut[5], dataOut[6], dataOut[7]);*/
    r.iAddr = 0x60;
    r.iSpeed = 400; //kHz
    r.iWriteData = dataOut;
    r.iWriteLength = numBytes; 
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = dataIn;
    r.iReadLength = 7; 
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;    
    UEZI2CTransaction(I2C, &r);        
    //                            L  0  0  M  R  C1 C2
    /*FDICmdPrintf(aWorkspace, "RX: %X,%X,%X,%X,%X,%X,%X\n", dataIn[0], dataIn[1], dataIn[2], dataIn[3]
                 , dataIn[4], dataIn[5], dataIn[6]);*/
    UEZI2CClose(I2C);
    
    // should be 7, 0, 0, 0x60, 1, 0, 3A or similar
    // Now check the model number against known models. Also print out the rev number here
    if(dataIn[3] == 0x60) { // 608 hardware
          FDICmdPrintf(aWorkspace, "PASS: OK 0x60, %u\n", dataIn[4]);
    } else if (dataIn[3] == 0x50) { // 508 hardware
          FDICmdPrintf(aWorkspace, "PASS: OK 0x50, %u\n", dataIn[4]);
    } else { // not or unrecognized type
          FDICmdSendString(aWorkspace, "FAIL: No MC found\n");
    }
        
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

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUICmdCRC
 *---------------------------------------------------------------------------*
 * Description:
 *      
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
 *      When we see PING with no parameters, output "PASS: PING".  If any
 *      parameters, return "FAIL: PING"
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
        //TestModeSendCmd(TEST_MODE_PING); //Removed because no LCD or TS in headless unit

        // No Parameters (other than command) cause PING to pass
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        // Parameters cause PING to fail
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

TBool G_webPageHit;

void FuncTestPageHit(void)
{
    G_webPageHit = ETrue;
}

int UEZGUICmdWebpage(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 start;
    if (argc == 1) {
        G_webPageHit = EFalse;
        start = UEZTickCounterGet();
        while ( (UEZTickCounterGetDelta(start) < 120000) && (G_webPageHit == EFalse) );
        
        if(G_webPageHit == ETrue)
            FDICmdPrintf(aWorkspace, "PASS:");
        else
            FDICmdPrintf(aWorkspace, "FAIL:");
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

#define LEDENABLEn  GPIO_P6_26
#define LED_GREEN   GPIO_P6_27
#define LED_RED     GPIO_P6_28
#define LED_BLUE    GPIO_P6_29
#define LED_AMBER   GPIO_P6_30

int UEZGUICmd_LED(void *aWorkspace, int argc, char *argv[])
{
    char message[50];
    static TBool runOnce = EFalse;

    if(!runOnce){
        UEZGPIOSetMux(LEDENABLEn, 4);
        UEZGPIOOutput(LEDENABLEn);
        UEZGPIOClear(LEDENABLEn);

        UEZGPIOSetMux(LED_GREEN, 4);
        UEZGPIOOutput(LED_GREEN);
        UEZGPIOClear(LED_GREEN);

        UEZGPIOSetMux(LED_RED, 4);
        UEZGPIOOutput(LED_RED);
        UEZGPIOClear(LED_RED);

        UEZGPIOSetMux(LED_BLUE, 4);
        UEZGPIOOutput(LED_BLUE);
        UEZGPIOSet(LED_BLUE);

        UEZGPIOSetMux(LED_AMBER, 4);
        UEZGPIOOutput(LED_AMBER);
        UEZGPIOSet(LED_AMBER);
    }

    if (argc == 1 || argc == 2 || argc > 3) {
        sprintf(message, "FAIL: Invalid command, ex. LED GREEN ON\nRED, GREEN, BLUE, AMBER\n");
        FDICmdSendString(aWorkspace, message);
    } else {
        switch(argv[1][0]){
            case 'g':
            case 'G':
                if(argv[2][1] == 'n' || argv[2][1] == 'N'){
                    UEZGPIOSet(LED_GREEN);
                    UEZGPIOClear(LED_RED);
                } else {
                    UEZGPIOClear(LED_GREEN);
                    //UEZGPIOClear(LED_RED);
                    //UEZGPIOSet(LED_RED);
                }
                break;
            case 'r':
            case 'R':
                if(argv[2][1] == 'n' || argv[2][1] == 'N'){
                    UEZGPIOSet(LED_RED);
                    UEZGPIOClear(LED_GREEN);
                } else {
                    UEZGPIOClear(LED_RED);
                    //UEZGPIOSet(LED_GREEN);
                }
                break;
            case 'b':
            case 'B':
                if(argv[2][1] == 'n' || argv[2][1] == 'N'){
                    UEZGPIOClear(LED_BLUE);
                } else {
                    UEZGPIOSet(LED_BLUE);
                }
                break;
            case 'a':
            case 'A':
                if(argv[2][1] == 'n' || argv[2][1] == 'N'){
                    UEZGPIOClear(LED_AMBER);
                } else {
                    UEZGPIOSet(LED_AMBER);
                }
                break;
            default:
                break;
        }
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    }
    return 0;
}

extern TBool G_BLE_PoweredON;
extern TBool G_WiFi_PoweredON;
extern char G_BLE_MAC[];
extern char G_WIFI_MAC[];
int UEZGUICmd_LairdWiFi(void *aWorkspace, int argc, char *argv[])
{
    char message [50];
    if (argc == 1) {
        if(G_WiFi_PoweredON){
            sprintf(message, "PASS: %s", G_WIFI_MAC);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(5000);
            if(G_WiFi_PoweredON){
                sprintf(message, "PASS: %s", G_WIFI_MAC);
                FDICmdSendString(aWorkspace, message);
            } else {
                FDICmdSendString(aWorkspace, "FAIL: WIFI Did not power on\n");
            }
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int UEZGUICmd_LairdBLE(void *aWorkspace, int argc, char *argv[])
{
    char message [50];
    if (argc == 1) {
        if(G_BLE_PoweredON){
            sprintf(message, "PASS: %s", G_BLE_MAC);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(5000);
            if(G_BLE_PoweredON){
                sprintf(message, "PASS: %s", G_BLE_MAC);
                FDICmdSendString(aWorkspace, message);
            } else {
                FDICmdSendString(aWorkspace, "FAIL: BLE Did not power on\n");
            }
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

extern TBool G_BLE_Device_Found;
extern char G_BG_MAC[20];
int UEZGUICmd_BG121BLE(void *aWorkspace, int argc, char *argv[])
{
    char message [50];
    if (argc == 1) {
        if(G_BLE_Device_Found){
            sprintf(message, "PASS: %s", G_BG_MAC);
            FDICmdSendString(aWorkspace, message);
        } else {
            UEZTaskDelay(5000);
            if(G_BLE_Device_Found){
                sprintf(message, "PASS: %s", G_BG_MAC);
                FDICmdSendString(aWorkspace, message);
            } else {
                FDICmdSendString(aWorkspace, "FAIL: BLE121 Did not power on\n");
            }
        }
    } else {
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

int UEZGUICmdPMOD(void *aWorkspace, int argc, char *argv[]) {
    T_testData testData;
    // Don't care on parameters
    // Now do the test
    IUEZGUICmdRunTest(aWorkspace, FuncTestPMOD, &testData);
    return 0;
}

int UEZGUICmdXBEE(void *aWorkspace, int argc, char *argv[]) {
    T_testData testData;
    // Don't care on parameters
    // Now do the test
    IUEZGUICmdRunTest(aWorkspace, FuncTestXBEE, &testData);
    return 0;
}

int UEZGUICmdEXP(void *aWorkspace, int argc, char *argv[]) {
    T_testData testData;
    testData.iParams[0] = 0;
    //testData.iParams[1] = end;
    //testData.iParams[2] = type;
    testData.iNumParams = 0;
    
    // Don't care on parameters
    // Now do the test
    IUEZGUICmdRunTest(aWorkspace, FuncTestEXP, &testData);
    return 0;
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

int UEZGUICmdEEPROM1(void *aWorkspace, int argc, char *argv[])
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
        if(UEZEEPROMOpen("EEPROM1", &eeprom) == UEZ_ERROR_NONE){
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

#include <UEZFile.h>
int UEZGUICmdSDCard(void *aWorkspace, int argc, char *argv[])
{
    T_testData testData;
    T_uezFile file;

    if (argc == 1) {
        UEZFileMakeDirectory("1:Slides"); //created the expected directory
        //Create the file so the SDCards do not need to be preprogrammed
        if(UEZFileOpen("1:Slides/Slides.txt", FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
            UEZFileClose(file);
        } else {
            //File did not exist, try to create it and let the test run
            if(UEZFileOpen("1:Slides/Slides.txt", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE){
                UEZFileClose(file);
            }
        }
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
#else
    if (argc == 2) {
        // Got no parameters
        // Now do the test
        UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, FDICmdUValue(argv[1]));
        FDICmdSendString(aWorkspace, "PASS: OK\n");
#endif
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}
#endif

int UEZGUICmdRTC(void *aWorkspace, int argc, char *argv[])
{
    T_uezTimeDate td;

    if (argc == 1) {
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2013;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2013, 8:00:00
        UEZTimeDateSet(&td);

        UEZTaskDelay(5000);

        UEZTimeDateGet(&td);
        if ((td.iDate.iMonth==1) &&
                (td.iDate.iDay == 1) &&
                (td.iDate.iYear == 2013) &&
                (td.iTime.iHour == 8) &&
                (td.iTime.iMinute == 0) &&
                (td.iTime.iSecond > 1)) {
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        } else {
            FDICmdSendString(aWorkspace, "FAIL: Not Incrementing\n");
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
#if 0
            mac[0] = emacMACADDR0;
            mac[1] = emacMACADDR1;
            mac[2] = emacMACADDR2;
            mac[3] = emacMACADDR3;
            mac[4] = emacMACADDR4;
            mac[5] = emacMACADDR5;
#else
            FDICmdSendString(aWorkspace,
                                            "FAIL: Command not allowed!\n");
#endif
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

        if (Configuration_EEPROM_Program_MAC(G_nonvolatileSettings.iMACAddr) == UEZ_ERROR_NONE) {
            if (NVSettingsSave()) {
                FDICmdSendString(aWorkspace,
                        "FAIL: MAC address failed to save correctly\n");
            } else {
                FDICmdPrintf(aWorkspace,
                        "PASS: %02X:%02X:%02X:%02X:%02X:%02X\n",
                        G_nonvolatileSettings.iMACAddr[0],
                        G_nonvolatileSettings.iMACAddr[1],
                        G_nonvolatileSettings.iMACAddr[2],
                        G_nonvolatileSettings.iMACAddr[3],
                        G_nonvolatileSettings.iMACAddr[4],
                        G_nonvolatileSettings.iMACAddr[5]);
            }
        } else {
            FDICmdSendString(aWorkspace,
                    "FAIL: MAC address failed to save correctly\n");
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
            FDICmdSendString(aWorkspace, "      Format: I2CBANG <I2C Bus #> <bytes> <speed in kHz>\n");
            return 0;
        }

        if((speed != 100) && (speed != 400)) {
            FDICmdSendString(aWorkspace, "FAIL: speed must be '100' or '400' in kHz\n");
            FDICmdSendString(aWorkspace, "      Format: I2CBANG <I2C Bus #> <bytes> <speed in kHz>\n");
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
