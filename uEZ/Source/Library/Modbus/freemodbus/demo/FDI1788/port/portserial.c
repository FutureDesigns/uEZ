/*-------------------------------------------------------------------------*
 * File:  portserial.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEZ.h>
#include <uEZStream.h>
#include <Types/Serial.h>
#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct{
    T_uezDevice iSerial;
    TUInt8 iComPort[8];
    TBool iRxEnable;
    TBool iTXEnable;
    TUInt32 iTimeout;
}T_ModbusRTUWorkspace;

static T_ModbusRTUWorkspace G_workspace;
static char G_Byte;
static T_uezSemaphore G_Sem = 0;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern void vMBPortTimerPoll();

static TUInt32 RTU_Mon(T_uezTask aMyTask, void *aParams)
{
    TBool forever = ETrue;
    TUInt32 bytesRead;

    while(forever){
        if(G_workspace.iRxEnable){
            if(UEZStreamRead(G_workspace.iSerial, (void*)&G_Byte, 1, &bytesRead, 1) == UEZ_ERROR_NONE){
                pxMBFrameCBByteReceived(  );
                UEZSemaphoreGrab(G_Sem, UEZ_TIMEOUT_INFINITE);
                UEZTaskDelay(1);
            } else {
                vMBPortTimerPoll();
            }
        } else{
            UEZTaskDelay(1);
        }
    }
    return 0;
}

BOOL xMBPortSerialInit(
        UCHAR ucPort,
        ULONG ulBaudRate,
        UCHAR ucDataBits,
        eMBParity eParity)
{
    BOOL success = TRUE;
    T_Serial_Settings settings;

    memset((void*)&G_workspace, 0, sizeof(G_workspace));

    switch(ucPort){
        case 0:
          sprintf((char*)G_workspace.iComPort, "Console");
          break;
        case 1:
        case 2:
        case 3:
        case 4:
            sprintf((char*)G_workspace.iComPort, "UART%d", ucPort);
            break;
        default:
            success = EFalse;
            break;
    }

    if(success){
        if(UEZStreamOpen((const char*)G_workspace.iComPort, &G_workspace.iSerial) == UEZ_ERROR_NONE){
            settings.iBaud = ulBaudRate;
            settings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;
            settings.iParity = (T_Serial_Parity)eParity;
            settings.iStopBit = SERIAL_STOP_BITS_1;
            settings.iWordLength = 8;
            UEZStreamControl(G_workspace.iSerial, STREAM_CONTROL_SET_SERIAL_SETTINGS, &settings);
        } else {
            success = EFalse;
        }
    }

    if(G_Sem == 0){
        UEZSemaphoreCreateBinary(&G_Sem);
        UEZSemaphoreGrab(G_Sem, UEZ_TIMEOUT_INFINITE);
        UEZTaskCreate(RTU_Mon, "RTU_Mon", (1 * 1024), (void *) 0, UEZ_PRIORITY_NORMAL, 0);
    }
    return success;
}

void vMBPortClose(void)
{
    UEZStreamFlush(G_workspace.iSerial);
}

void xMBPortSerialClose(void)
{
    if(G_workspace.iSerial != 0){
        UEZStreamClose(G_workspace.iSerial);
        G_workspace.iSerial = 0;
    }
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    G_workspace.iRxEnable = xRxEnable;
    G_workspace.iTXEnable = xTxEnable;
    if(xTxEnable == TRUE){
        pxMBFrameCBTransmitterEmpty();
    }
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
    BOOL success = FALSE;
    //TUInt32 bytesRead;

//    if(UEZStreamRead(G_workspace.iSerial, (void*)pucByte, 1, &bytesRead, G_workspace.iTimeout) == UEZ_ERROR_NONE){
//        success = TRUE;
//    }
    *pucByte = G_Byte;
    success = TRUE;
    UEZSemaphoreRelease(G_Sem);

    return success;
}

BOOL xMBPortSerialPutByte(CHAR *ucByte, TUInt32 aSize)
{
    BOOL success = FALSE;
    TUInt32 bytesSent;

    if(UEZStreamWrite(G_workspace.iSerial, (void*)ucByte, aSize, &bytesSent, G_workspace.iTimeout) == UEZ_ERROR_NONE){
        //UEZStreamFlush(G_workspace.iSerial);
        if(bytesSent == aSize){
            success = ETrue;
        }
    }

    return success;
}

BOOL xMBPortSerialSetTimeout( TUInt32 dwTimeoutMs )
{
    BOOL success = TRUE;

    G_workspace.iTimeout = (TUInt32)dwTimeoutMs;

    return success;
}

/*-------------------------------------------------------------------------*
 * End of File:  portserial.c
 *-------------------------------------------------------------------------*/
