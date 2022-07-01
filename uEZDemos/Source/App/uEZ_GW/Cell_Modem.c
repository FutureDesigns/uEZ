/*-------------------------------------------------------------------------*
 * File:  Cell_Modem.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <UEZStream.h>
#include <UEZGPIO.h>
#include <Types/Serial.h>
#include <UEZProcessor.h>
#include <UEZPlatform.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define POWER_PIN           (GPIO_P7_3)
#define RESET_PIN           (GPIO_P7_4)
#define CTS_PIN             (GPIO_P6_1)
#define RTS_PIN             (GPIO_P6_2)
#define DTR_PIN             (GPIO_P7_2)

#define CELL_UART           "UART1"

//#define DEBUG   1

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezDevice G_CellUART = 0;
static T_Serial_Settings G_SerialSettings;
static char G_IMEI[20];
static char G_CCID[25];
static char G_MFWV[30];
static char G_FWV[15];
static char G_HWV[15];
static TBool G_IMEISet = EFalse;
static TBool G_CCIDSet = EFalse;
static TBool G_FWVSet = EFalse;
static TBool G_MFWVSet = EFalse;
static TBool G_HWVSet = EFalse;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

static void set_baudRate(TUInt32 aBuad)
{
    G_SerialSettings.iBaud = aBuad;
    UEZStreamControl(G_CellUART, STREAM_CONTROL_SET_SERIAL_SETTINGS,
            &G_SerialSettings);
}

static T_uezError modem_send(char *aString, TUInt32 aBytes, TUInt32 *aNumBytes)
{
    T_uezError error = UEZStreamWrite(G_CellUART, (void*)aString, aBytes,
            aNumBytes, 100);
    UEZStreamFlush(G_CellUART);
    return error;
}

static T_uezError modem_receive(char *aBuffer, TUInt32 *aNumBytes)
{
    T_uezError error;
    TUInt8 byte;
    TUInt32 i = 0;
    TUInt32 numBytes;

    while ((error = UEZStreamRead(G_CellUART, (void*)&byte, 1, &numBytes, 1000))
            == UEZ_ERROR_NONE) {
        //if(byte == '\r'){
        //    UEZStreamRead(G_CellUART, (void*)&byte, 1, aNumBytes, 1000);
        //    break;
        //}
        aBuffer[i++] = byte;
        *aNumBytes++;
    }
    aBuffer[i] = '\0';
    if (i) {
        return UEZ_ERROR_NONE;
    } else {
        return error;
    }
}

static void modem_clear_buffer()
{
    TUInt8 byte;
    TUInt32 numBytes;
    while (UEZStreamRead(G_CellUART, (void*)&byte, 1, &numBytes, 1000)
            == UEZ_ERROR_NONE)
        ;
}

static TUInt32 Cell_Modem_Task(T_uezTask aMyTask, void *aParams)
{
    //TBool forever = ETrue;
    TUInt32 bytes;
    char message[200];
    char m_out[200];

#if 0 //Nimblelink
    set_baudRate(9600);
    modem_send("AT+IPR=115200\r\n", 15, &bytes);
    modem_receive(message, &bytes);
    UEZTaskDelay(100);

    set_baudRate(115200);
    modem_send("AT+IPR=115200\r\n", 15, &bytes);
    modem_receive(message, &bytes);
    UEZTaskDelay(100);

    //modem_receive(message, &bytes);
    modem_clear_buffer();

    modem_send("AT^SPOW=1,0\r\n", 13, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    modem_send("ATI\n\r", 5, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    modem_send("AR+CESQ=?\n\r", 11, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    UEZTaskDelay(1000);

    //Read IMEI
    modem_send("AT+CGSN=0\n\r", 11, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    memset((void*)message, 0, sizeof(message));

    modem_send("AT+CCID?\n\r", 10, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    modem_send("AT^SMONP\n\r", 10, &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

#if 0
    modem_send("AT+SQNSCFG=3,3,300,90,600,5\r\n", &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    modem_send("AT+CGDCONT=3,\"IP\",\"vzwinternet\"\r\n", &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }

    modem_send("AT+CGACT=1,3", &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }
    modem_send("AT+SQNSD=3,0,80,\"google.com\"", &bytes);
    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
        dprintf("%s\n", message);
    }
#else
//    modem_send("AT+CGDCONT=3,\"IPV4V6\",\"vxwinternet\"\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }
//
//    modem_send("AT^SISS=0,\"srvType\",\"Socket\"\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }
//
//    modem_send("AT^SISS=0,\"conId\",3\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }
//
//    modem_send("AT^SISS=0,\"address\",\"socktcp://www.google.com:80\"\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }
//
//    modem_send("AT^SISO=0\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }
//
//    modem_send("AT^SISC=0\n\r", &bytes);
//    while(modem_receive(message, &bytes) == UEZ_ERROR_NONE){
//        dprintf("%s\n", message);
//    }

#endif
#else //Digi
    modem_clear_buffer();

    do {
        modem_send("+++", 3, &bytes);
        UEZTaskDelay(1000);
        while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
            dprintf("%s\n", message);
        }

        //Get IMEI
        if(!G_IMEISet){
            memset((void*)message, 0, sizeof(message));
            sprintf(m_out, "%s\r", "AT IM");
            modem_send(m_out, 6, &bytes);
            while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
                if (strlen(message) > 5) {
                    sprintf(G_IMEI, "%.15s", message);
                    G_IMEISet = ETrue;
                }
                dprintf("%s\n", message);
            }
        }

        //Get SIM CCIS
        if(!G_CCIDSet){
            memset((void*)message, 0, sizeof(message));
            sprintf(m_out, "%s\r", "AT S#");
            modem_send(m_out, 6, &bytes);
            while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
                if (strlen(message) > 5) {
                    sprintf(G_CCID, "%.20s", message);
                    G_CCIDSet = ETrue;
                }
                dprintf("%s\n", message);
            }
        }
        
        //Get Digi FW version
        if(!G_FWVSet){
            memset((void*)message, 0, sizeof(message));
            sprintf(m_out, "%s\r", "ATVR");
            modem_send(m_out, 5, &bytes);
            while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
                if (strlen(message) > 3) {
                    sprintf(G_FWV, "%.4s", message);
                    G_FWVSet = ETrue;
                }
                dprintf("%s\n", message);
            }
        }
        
        //Get Modem FW version
        if(!G_MFWVSet){
            memset((void*)message, 0, sizeof(message));
            sprintf(m_out, "%s\r", "ATMV");
            modem_send(m_out, 5, &bytes);
            while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
                if (strlen(message) > 3) {
                    sprintf(G_MFWV, "%s", message);
                    G_MFWVSet = ETrue;
                }
                dprintf("%s\n", message);
            }
        }
        
        //Get HW version
        if(!G_HWVSet){
            memset((void*)message, 0, sizeof(message));
            sprintf(m_out, "%s\r", "ATHV");
            modem_send(m_out, 5, &bytes);
            while (modem_receive(message, &bytes) == UEZ_ERROR_NONE) {
                if (strlen(message) > 3) {
                    sprintf(G_HWV, "%s", message);
                    G_HWVSet = ETrue;
                }
                dprintf("%s\n", message);
            }
        }
    } while (!G_FWVSet || !G_HWVSet || !G_CCIDSet || !G_IMEISet || !G_MFWVSet);

#endif
    return 0;
}

TBool Cell_Modem_GetIMEI(char *aIMEI)
{
    if(G_IMEISet){
        strcpy(aIMEI, G_IMEI);
    }
    return G_IMEISet;
}

TBool Cell_Modem_GetCCID(char *aCCID)
{
    if(G_CCIDSet){
        strcpy(aCCID, G_CCID);
    }
    return G_CCIDSet;
}

TBool Cell_Modem_GetFWV(char *aFWV)
{
    if(G_FWVSet){
        strcpy(aFWV, G_FWV);
    }
    return G_FWVSet;
}

TBool Cell_Modem_GetMFWV(char *aMFWV)
{
    if(G_MFWVSet){
        strcpy(aMFWV, G_MFWV);
    }
    return G_MFWVSet;
}

TBool Cell_Modem_GetHWV(char *aHWV)
{
    if(G_HWVSet){
        strcpy(aHWV, G_HWV);
    }
    return G_HWVSet;
}

void Cell_Modem_Start(void)
{
    static TBool haveRun = EFalse;
    TUInt32 config;

    if (!haveRun) {
        haveRun = ETrue;

        UEZPlatform_FullDuplex_UART1_Require(1024, 1024);

#ifdef POWER_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
        config = SCU_NORMAL_DRIVE_DEFAULT((POWER_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
        UEZGPIOControl(POWER_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
        UEZGPIOSet(POWER_PIN);
        UEZGPIOOutput(POWER_PIN);
#endif

#ifdef RESET_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
        config = SCU_NORMAL_DRIVE_DEFAULT((RESET_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
        UEZGPIOControl(RESET_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
        UEZGPIOSet(RESET_PIN);
        UEZGPIOOutput(RESET_PIN);
#endif

#ifdef CTS_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
        config = SCU_NORMAL_DRIVE_DEFAULT((CTS_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
        UEZGPIOControl(CTS_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
        UEZGPIOClear(CTS_PIN);
        //UEZGPIOOutput(CTS_PIN);
        UEZGPIOInput(CTS_PIN);
#endif

#ifdef DTR_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
        config = SCU_NORMAL_DRIVE_DEFAULT((DTR_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
        UEZGPIOControl(DTR_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
        UEZGPIOClear(DTR_PIN);
        UEZGPIOOutput(DTR_PIN);
#endif

#ifdef RTS_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
        config = SCU_NORMAL_DRIVE_DEFAULT((RTS_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
        UEZGPIOControl(RTS_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
        UEZGPIOClear(RTS_PIN);
        UEZGPIOOutput(RTS_PIN);
#endif

        UEZGPIOSet(POWER_PIN);
        UEZTaskDelay(1);
        UEZGPIOClear(POWER_PIN);
        UEZTaskDelay(1000);
        UEZGPIOInput(POWER_PIN);

        UEZGPIOClear(RESET_PIN);
        UEZTaskDelay(10);
        UEZGPIOSet(RESET_PIN);
        UEZTaskDelay(1000);

        memset((void*)G_IMEI, 0, sizeof(G_IMEI));
        memset((void*)G_CCID, 0, sizeof(G_CCID));
        if (UEZStreamOpen(CELL_UART, &G_CellUART) == UEZ_ERROR_NONE) {
            G_SerialSettings.iBaud = 9600;
            G_SerialSettings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;
            G_SerialSettings.iParity = SERIAL_PARITY_NONE;
            G_SerialSettings.iStopBit = SERIAL_STOP_BITS_1;
            G_SerialSettings.iWordLength = 8;

            UEZStreamControl(G_CellUART, STREAM_CONTROL_SET_SERIAL_SETTINGS,
                    &G_SerialSettings);

            UEZTaskCreate((T_uezTaskFunction)Cell_Modem_Task, "Cell Modem",
                    UEZ_TASK_STACK_BYTES(1024), (void *)0, UEZ_PRIORITY_NORMAL,
                    0);
        }
    }
}
/*-------------------------------------------------------------------------*
 * End of File:  Cell_Modem.c
 *-------------------------------------------------------------------------*/
