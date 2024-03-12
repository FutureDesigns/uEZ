/*-------------------------------------------------------------------------*
 * File:  Network_ESPWROOM32.c
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
#include <string.h>
#include <stdlib.h>
#include <uEZ.h>
#include <uEZDevice.h>  // This is needed for the RX compiler
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <uEZMemory.h>
#include <uEZStream.h>
#include "Source/uEZSystem/uEZHandles.h"
#include "Network_ESPWROOM32.h"
#include "timers.h"
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>

/*-------------------------------------------------------------------------*
 * Menu callbacks:
 *-------------------------------------------------------------------------*/
static void ESPWROOM32_ModuleStatusHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_SocketConnectionHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_NetworkStatusHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_ScanSSIDHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_GetMACHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_ReceiveSendingDataHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_QueryStationConnectionHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
static void ESPWROOM32_ResolveDomainHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
#if ESPWROOM32_BUFFER_ON_MODULE
static void ESPWROOM32_ReceiveDataHandler(T_ESPWROOM32_ATCmdArgs *pArgs);
#endif
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
const T_ESPWROOM32_ATCmd G_ESPWROOM32_ATCmdResp[] =
{
    {
        .command  = (uint8_t*)"STATUS:",
        .callback = ESPWROOM32_ModuleStatusHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPSTATUS:",
        .callback = ESPWROOM32_SocketConnectionHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPSTA:",
        .callback = ESPWROOM32_NetworkStatusHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPAP:",
        .callback = ESPWROOM32_NetworkStatusHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CWLAP:",
        .callback = ESPWROOM32_ScanSSIDHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPSTAMAC:",
        .callback = ESPWROOM32_GetMACHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPAPMAC:",
        .callback = ESPWROOM32_GetMACHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"\r\nRecv ",
        .callback = ESPWROOM32_ReceiveSendingDataHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CWJAP:",
        .callback = ESPWROOM32_QueryStationConnectionHandler,
        .context  = NULL
    },
    {
        .command  = (uint8_t*)"+CIPDOMAIN:",
        .callback = ESPWROOM32_ResolveDomainHandler,
        .context  = NULL
    },
#if ESPWROOM32_BUFFER_ON_MODULE
    {
        .command  = (uint8_t*)"+CIPRECVDATA:",
        .callback = ESPWROOM32_ReceiveDataHandler,
        .context  = NULL
    },
#endif
};

const T_ESPWROOM32_ATCmdMenu G_ESPWROOM32_RootMenu =
{
    .numCmds = sizeof(G_ESPWROOM32_ATCmdResp) / sizeof(G_ESPWROOM32_ATCmdResp[0]),
    .cmdList = G_ESPWROOM32_ATCmdResp
};

const char DisableECHO[]            = "ATE0\r\n";
const char GetVersionInfo[]         = "AT+GMR\r\n";
const char ListSupportedCommands[]  = "AT+CMD?\r\n";
const char SetWiFiMode[]            = "AT+CWMODE";
const char EnableMultiConnection[]  = "AT+CIPMUX=1\r\n";
const char NoShowRemoteIP[]         = "AT+CIPDINFO=0\r\n";
const char SetProvInStation[]       = "AT+CWJAP";
const char SetDHCPConfig[]          = "AT+CWDHCP";
const char GetIPStatusInStation[]   = "AT+CIPSTA?";
const char GetIPStatusInAP[]        = "AT+CIPAP?";
const char TypeTCP[]                = "\x22TCP\x22";
const char TypeUDP[]                = "\x22UDP\x22";
const char TypeSSL[]                = "\x22SSL\x22";
const char ClientSocketCreate[]     = "AT+CIPSTART";
const char ResolveDomain[]          = "AT+CIPDOMAIN";
const char GetNetworkStat[]         = "AT+CIPSTATUS\r\n";
const char CloseSocketConn[]        = "AT+CIPCLOSE";
const char SocketSend[]             = "AT+CIPSENDEX";
#if ESPWROOM32_BUFFER_ON_MODULE
const char SocketRecv[]             = "AT+CIPRECVDATA";
#endif
const char SetReceiveMode[]         = "AT+CIPRECVMODE";

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct 
{
    DEVICE_Network                      **iDevice;
    T_uezDevice                         iUART;
    T_ESPWROOM32_Network_UARTSettings   iUARTSettings;
    TUInt16                             iMaxESPPacketSize;

    /* Internal state */
    bool                                iOpen;
    bool                                iInit;
    TUInt16                             iRXIndex;
    bool                                iRXIsData;
    T_ESPWROOM32_ATCommandIndex         iCommand;
    TUInt8                              iRXPacketHeader[32];
    TUInt8                              iRXPacketHeaderIndex;
    T_networkPacket                     iRXNetworkPacket[ESPWROOM32_MAX_PACKETS_IN_POOL];
    TUInt8                              iRXNetworkPacketIndex;
    TUInt8                              *pInputBufferLast;
    T_ESPWROOM32_TXMode                 iTransmissionMode;
    TUInt8                              iMaxScanIndex;
    TUInt8                              iPacketIndex;
    T_ESPWROOM32_Packet                 iATInterfacePacket[ESPWROOM32_MAX_AP_NUM_IN_SCAN];
    T_ESPWROOM32_ATCmdMenu const        *pRootMenu;
    TUInt16                             iSizeOfResponse;
    TUInt8                              iRXBuffer[ESPWROOM32_AT_RX_SIZE];
    bool                                iGetNetworkData;
    TInt16                              iRXDataLength;
    TInt16                              iTotalRXDataSize;
    TInt8                               iTCPSocketLinkID;
    T_ESPWROOM32_Socket                 iSocket[ESPWROOM32_SOCKET_MAX_CONNECTIONS];
    T_ESPWROOM32_SocketStatus           iSocketStatus[ESPWROOM32_SOCKET_MAX_CONNECTIONS];
    TInt8                               iSocketNextLinkID;
    TUInt8                              iSocketCount;
    T_ESPWROOM32_Mode                   iProvMode;
    T_ESPWROOM32_StationProvisioning    iProvInfo;
    TUInt32                             iCurrentIPAddress;
    TUInt32                             iCurrentGateway;
    TUInt32                             iCurrentMask;
    T_uezNetworkAddr                    iResolvedAddr;
    T_ESPWROOM32_StationStatus          iStationStatus;
    TInt32                              iListeningConnectionList[ESPWROOM32_SOCKET_MAX_CONNECTIONS];
    TInt32                              iListeningConnectionIndex;
    TInt32                              iMaxTCPServerConnections;
    TInt16                              iTransmitDataLength;
#if ESPWROOM32_BUFFER_ON_MODULE
    TInt32                              iReceivedActualLength;
#endif
    TUInt8                              *pInputBuffer;
    TUInt8                              *pCmdBuffer;
    TUInt32                             iCmdBufferIndex;

    /* RTOS Objects */
    TaskHandle_t                        iCurrentTask;
    T_uezTask                           iATCmdRespTask;
    T_uezTask                           iRXTask;
    T_uezSemaphore                      iTXDoneSem;
    T_uezSemaphore                      iRXDoneSem;
    T_uezSemaphore                      iCmdDoneSem;
    T_uezSemaphore                      iATCmdMutex;
    T_uezSemaphore                      iSocketConnectionUpdateSem;
    T_uezSemaphore                      iAcceptSem;
    T_uezQueue                          iRXDataQueue;
    TimerHandle_t                       iResponseTimer;
    TimerHandle_t                       iSocketStatusTimer;
} T_Network_ESPWROOM32_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_Network_ESPWROOM32_Workspace *G_ESPWROOM32_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/* Internal */
static T_uezError   ESPWROOM32_Configure(void *aWorkspace, 
                                         const T_ESPWROOM32_Network_UARTSettings *aSettings);
static TInt32       ESPWROOM32_CheckForMatch(TUInt8 const * pTest, TUInt8 const * const pRef);
static T_uezError   ESPWROOM32_ATCmdCallback(T_ESPWROOM32_ATCmdMenu const * const pMenu,
                                             TUInt8 const * pInput,
                                             TUInt32 const bytes);
static void         ESPWROOM32_RemoveQuotations(char * str_in, char * str_out);

/* AT Command Functions */
static T_uezError   ESPWROOM32_AT_DisableEcho( void );
static T_uezError   ESPWROOM32_AT_GetVersionInfo( void );
static T_uezError   ESPWROOM32_AT_ListSupportedCommands( void );
static T_uezError   ESPWROOM32_AT_SetWIFIMode( void );
static T_uezError   ESPWROOM32_AT_EnableMultiConnections( void );
static T_uezError   ESPWROOM32_AT_DataWithoutRemoteAddress( void );
static T_uezError   ESPWROOM32_AT_ConnectToAP( void );
static T_uezError   ESPWROOM32_AT_EnableDHCP( void );
static T_uezError   ESPWROOM32_AT_IPAddressGet( void );
static T_uezError   ESPWROOM32_AT_ResolveDomain( const char * aDomain, T_uezNetworkAddr * aAddr );
static T_uezError   ESPWROOM32_AT_ClientSocketCreate( T_uezNetworkSocket aSocket );
static T_uezError   ESPWROOM32_AT_GetConnectionStatus( void );
static T_uezError   ESPWROOM32_AT_SocketSetReceiveMode( void );
static T_uezError   ESPWROOM32_AT_SocketClose( T_uezNetworkSocket aSocket );
static T_uezError   ESPWROOM32_AT_SocketAbort( void );
static T_uezError   ESPWROOM32_AT_SocketSendTo( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 aTimeout );
#if ESPWROOM32_BUFFER_ON_MODULE
static T_uezError   ESPWROOM32_AT_SocketReceiveFrom( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 *aNumBytesRead, TUInt32 aTimeout );
#else
static T_uezError   ESPWROOM32_AT_SocketReceiveFrom( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 aTimeout );
#endif

/* Timer or Task Callbacks */
static void         ESPWROOM32_ResponseTimeoutHandler( TimerHandle_t pxTimer );
static void         ESPWROOM32_SocketStatusHandler( TimerHandle_t pxTimer );
TUInt32             ESPWROOM32_ATCmdRespHandler(T_uezTask aMyTask, void *aParams);
TUInt32             ESPWROOM32_RXHandler(T_uezTask aMyTask, void *aParams);

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_InitializeWorkspace
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    p->iOpen = false;
    p->iInit = false;
    p->iRXIndex = 0;
    p->iRXIsData = false;
    p->iCommand = ESPWROOM32_AT_CMD_INDEX_NONE;
    memset(p->iRXPacketHeader, 0, sizeof(p->iRXPacketHeader));
    p->iRXPacketHeaderIndex = 0;
    memset(p->iRXNetworkPacket, 0, sizeof(p->iRXNetworkPacket));
    p->iRXNetworkPacketIndex = 0;
    p->pInputBufferLast = 0;
    p->iTransmissionMode = ESPWROOM32_TX_MODE_NORMAL;
    p->iMaxScanIndex = 0;
    p->iPacketIndex = 0;
    memset(p->iATInterfacePacket, 0, sizeof(p->iATInterfacePacket));
    p->pRootMenu = &G_ESPWROOM32_RootMenu;
    p->iSizeOfResponse = 0;
    memset(p->iRXBuffer, 0, sizeof(p->iRXBuffer));
    p->iGetNetworkData = false;
    p->iRXDataLength = 0;
    p->iTCPSocketLinkID = -1;
    p->iSocketCount = 0;
    memset(p->iSocket, 0, sizeof(p->iSocket));
    memset(p->iSocketStatus, 0, sizeof(p->iSocketStatus));
    for(TUInt8 i = 0; i < ESPWROOM32_SOCKET_MAX_CONNECTIONS; i++)
    {
        p->iSocket[i].linkID = -1;
        p->iSocketStatus[i] = ESPWROOM32_SOCKET_STATUS_CLOSED;
    }
    p->iSocketNextLinkID = 0;
    p->iProvMode = (T_ESPWROOM32_Mode) 0;
    memset(&p->iProvInfo, 0, sizeof(p->iProvInfo));
    p->iCurrentIPAddress = 0;
    p->iCurrentGateway = 0;
    p->iCurrentMask = 0;
    memset(&p->iResolvedAddr, 0, sizeof(p->iResolvedAddr));
    p->iStationStatus = (T_ESPWROOM32_StationStatus) 0;
    memset(&p->iListeningConnectionList, 0, sizeof(p->iListeningConnectionList));
    p->iListeningConnectionIndex = 0;
    p->iMaxTCPServerConnections = 0;
    p->iTransmitDataLength = 0;
    p->iReceivedActualLength = 0;
#if 0
    p->pInputBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
    if(NULL == p->pInputBuffer)
    {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }
#else
    p->pInputBuffer = 0;
#endif
    p->pCmdBuffer = 0;
    p->iCmdBufferIndex = 0;


    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iRXDoneSem, ESPWROOM32_MAX_PACKETS_IN_POOL, 0);
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iTXDoneSem, 1, 0);
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iCmdDoneSem, 1, 0);
    }
#if 0
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateMutex(&p->iATCmdMutex, ESPWROOM32_AT_CMD_MUTEX_PRIORITY);
    }
#else
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iATCmdMutex, 1, 1);
    }
#endif
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iSocketConnectionUpdateSem, 1, 0);
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreCreateCounting(&p->iAcceptSem, 1, 0);
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZQueueCreate((ESPWROOM32_MAX_PACKETS_IN_POOL * 4), 4, &p->iRXDataQueue);
    }
    if(UEZ_ERROR_NONE == error)
    {
        p->iResponseTimer = xTimerCreate("AT Response Timer", 
                                         ESPWROOM32_RESPONSE_TIMEOUT, 
                                         pdFALSE, 
                                         0, 
                                         ESPWROOM32_ResponseTimeoutHandler);
        if(NULL == p->iResponseTimer)
        {
            error = UEZ_ERROR_OUT_OF_HANDLES;
        }
    }
    if(UEZ_ERROR_NONE == error)
    {
        p->iSocketStatusTimer = xTimerCreate("Socket Status Change Timer", 
                                             ESPWROOM32_SOCKET_STATUS_DELAY, 
                                             pdFALSE, 
                                             0, 
                                             ESPWROOM32_SocketStatusHandler);
        if(NULL == p->iSocketStatusTimer)
        {
            error = UEZ_ERROR_HANDLE_INVALID;
        }
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZTaskCreate((T_uezTaskFunction)ESPWROOM32_ATCmdRespHandler, 
                          "ESPWROOM32 ATCmdResp Task", 
                          ESPWROOM32_AT_CMD_THREAD_STACK_SIZE, 
                          aWorkspace, 
                          ESPWROOM32_AT_CMD_THREAD_PRIORITY, 
                          &p->iATCmdRespTask);
    }
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZTaskCreate((T_uezTaskFunction)ESPWROOM32_RXHandler, 
                          "ESPWROOM32 RX Task", 
                          ESPWROOM32_RX_THREAD_STACK_SIZE, 
                          aWorkspace, 
                          ESPWROOM32_RX_THREAD_PRIORITY, 
                          &p->iRXTask);
    }

    if(UEZ_ERROR_NONE != error)
    {
        UEZTaskDelete(G_ESPWROOM32_Workspace->iRXTask);
        UEZTaskDelete(G_ESPWROOM32_Workspace->iATCmdRespTask);
        xTimerDelete(p->iSocketStatusTimer, 0);
        xTimerDelete(p->iResponseTimer, 0);
        UEZQueueDelete(p->iRXDataQueue);
        UEZSemaphoreDelete(p->iAcceptSem);
        UEZSemaphoreDelete(p->iSocketConnectionUpdateSem);
        UEZSemaphoreDelete(p->iATCmdMutex);
        UEZSemaphoreDelete(p->iCmdDoneSem);
        UEZSemaphoreDelete(p->iTXDoneSem);
        UEZSemaphoreDelete(p->iRXDoneSem);
#if 0
        UEZMemFree(p->pInputBuffer);
        p->pInputBuffer = 0;
#endif
    }
    else
    {
        p->iInit = true;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Open
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Open(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    TaskHandle_t taskHandle;
    TUInt8 data[6] = { 0 };

    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;
    p->iCurrentTask = xTaskGetCurrentTaskHandle();    

    UEZStreamOpen(p->iUARTSettings.iUARTDeviceName, &p->iUART);

    error = uEZHandleGet(G_ESPWROOM32_Workspace->iRXTask, 0, 0, 0, (TUInt32 *)&taskHandle);
    if (error == UEZ_ERROR_NONE)
    {
        xTaskNotify((TaskHandle_t)taskHandle,0, 0);
    }

    /* Reset the module by pulling the enable pin low */
    UEZGPIOClear(p->iUARTSettings.iResetPin);
    UEZTaskDelay(10);
    UEZGPIOSet(p->iUARTSettings.iResetPin);
    UEZTaskDelay(10);

    /* Wait for 'ready' from module */
    UEZStreamRead(p->iUART, &data, 5, NULL, 5000);

    error = ESPWROOM32_AT_DisableEcho();
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_Open::ESPWROOM32_AT_DisableEcho, error=%d\n", error);
    }

#if (SEGGER_ENABLE_RTT == 1)
    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_GetVersionInfo();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_Open::ESPWROOM32_AT_GetVersionInfo, error=%d\n", error);
        }
    }
#if 0
    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_ListSupportedCommands();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_Open::ESPWROOM32_AT_GetVersionInfo, error=%d\n", error);
        }
    }
#endif
#endif

    if(UEZ_ERROR_NONE == error)
    {
        p->iOpen = true;
        UEZTaskDelay(1);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Close
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Close(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_Close not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Scan
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Scan(
    void *aWorkspace,
    TUInt32 aChannelNumber,
    const char *aScanSSID,
    T_uezNetworkScanCallback aCallback,
    void *aCallbackWorkspace,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_Scan not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Join
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Join(
    void *aWorkspace,
    const char *aJoinName,
    const char *aJoinPassword,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NONE;

    error = ESPWROOM32_AT_ConnectToAP();
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_Join::ESPWROOM32_AT_ConnectToAP, error=%d\n", error);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_GetStatus
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_GetStatus(
    void *aWorkspace,
    T_uezNetworkStatus *aStatus)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    error = ESPWROOM32_AT_IPAddressGet();
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_GetStatus::ESPWROOM32_AT_IPAddressGet, error=%d\n", error);
    }
    else
    {
        aStatus->iIPAddr.v4[0] = (p->iCurrentIPAddress >> 24) & 0xFF;
        aStatus->iIPAddr.v4[1] = (p->iCurrentIPAddress >> 16) & 0xFF;
        aStatus->iIPAddr.v4[2] = (p->iCurrentIPAddress >> 8) & 0xFF;
        aStatus->iIPAddr.v4[3] = p->iCurrentIPAddress & 0xFF;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketCreate
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketCreate(
    void *aWorkspace,
    T_uezNetworkSocketType aType,
    T_uezNetworkSocket *aSocket)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    if(p->iSocketCount > ESPWROOM32_SOCKET_MAX_CONNECTIONS)
    {
        error = UEZ_ERROR_NOT_AVAILABLE;
    }

    if(UEZ_ERROR_NONE == error)
    {
        for(TUInt8 socket_index = 0; socket_index < ESPWROOM32_SOCKET_MAX_CONNECTIONS; socket_index++)
        {
            if(p->iSocket[socket_index].linkID == -1)
            {
                switch(aType)
                {
                    case UEZ_NETWORK_SOCKET_TYPE_TCP:
                        p->iSocket[socket_index].protocolType = ESPWROOM32_SOCKET_PROTOCOL_TYPE_STREAM;
                        *aSocket = (T_uezNetworkSocket) socket_index;
                        break;
                    default:
                        error = UEZ_ERROR_NOT_SUPPORTED;
                        break;
                }
                break;
            }
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketConnect
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketConnect(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;
    
    for(TUInt8 socket_index = 0; socket_index < ESPWROOM32_SOCKET_MAX_CONNECTIONS; socket_index++)
    {
        if(p->iSocket[socket_index].linkID == p->iSocketNextLinkID)
        {
            p->iSocketNextLinkID++;
            p->iSocketNextLinkID = p->iSocketNextLinkID % (TInt8)ESPWROOM32_SOCKET_MAX_CONNECTIONS;
        }
    }

    p->iSocket[aSocket].linkID          = p->iSocketNextLinkID;
    p->iSocket[aSocket].networkAddr     = *aAddr;
    p->iSocket[aSocket].port            = aPort;
    p->iSocket[aSocket].connectionType  = ESPWROOM32_SOCKET_CONNECTION_TYPE_CLIENT;

    error = ESPWROOM32_AT_ClientSocketCreate(aSocket);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_SocketConnect::ESPWROOM32_AT_ClientSocketCreate, error=%d\n", error);
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_GetConnectionStatus();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_SocketConnect::ESPWROOM32_AT_GetConnectionStatus, error=%d\n", error);
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketBind
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketBind(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_SocketBind not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketListen
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketListen(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_SocketListen not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketAccept
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketAccept(
    void *aWorkspace,
    T_uezNetworkSocket aListenSocket,
    T_uezNetworkSocket *aCreatedSocket,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_SocketAccept not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketClose
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketClose(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_uezError error = UEZ_ERROR_NONE;

    error = ESPWROOM32_AT_SocketClose(aSocket);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_SocketClose::ESPWROOM32_AT_SocketClose, error=%d\n", error);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketDelete
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketDelete(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_uezError error = UEZ_ERROR_NONE;

    error = ESPWROOM32_AT_SocketClose(aSocket);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_SocketDelete::ESPWROOM32_AT_SocketClose, error=%d\n", error);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketRead
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketRead(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesRead,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;
    UNUSED(p);
#if 0
    if(*p->iSocket[aSocket].status == ESPWROOM32_SOCKET_STATUS_CLOSED)
    {
        p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EISNOTCONN;
        error = UEZ_ERROR_NOT_OPEN;
    }
#endif

#if ESPWROOM32_BUFFER_ON_MODULE
    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_SocketReceiveFrom( aSocket, aData, aNumBytes, aNumBytesRead, aTimeout);
    }
#else
    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_SocketReceiveFrom( aSocket, aData, aNumBytes, aTimeout);
    }

    if((aNumBytesRead != NULL) && (UEZ_ERROR_NONE == error))
    {
        *aNumBytesRead = aNumBytes;
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_SocketWrite
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_SocketWrite(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TBool aFlush,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    if(*p->iSocket[aSocket].status == ESPWROOM32_SOCKET_STATUS_CLOSED)
    {
        p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EISNOTCONN;
        error = UEZ_ERROR_NOT_OPEN;
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_SocketSendTo( aSocket, aData, aNumBytes, aTimeout);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_AuxControl
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_AuxControl(
    void *aWorkspace,
    TUInt32 aAuxCommand,
    void *aAuxData)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `esp_socket_recv` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_AuxControl not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Leave
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Leave(void *aWorkspace, TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_Leave not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_ResolveAddress
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_ResolveAddress(
    void *aWorkspace,
    const char *aName,
    T_uezNetworkAddr *aAddr)
{
    T_uezError error = UEZ_ERROR_NONE;

    error = ESPWROOM32_AT_ResolveDomain(aName, aAddr);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_ResolveAddress::ESPWROOM32_AT_ResolveDomain, error=%d\n", error);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_InfrastructureConfigure
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_InfrastructureConfigure(
    void *aWorkspace,
    T_uezNetworkSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    p->iProvMode = ESPWROOM32_MODE_STATION;
    memset(&p->iProvInfo, 0, sizeof(p->iProvInfo));

    error = ESPWROOM32_AT_SetWIFIMode();
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_InfrastructureConfigure::ESPWROOM32_AT_SetWIFIMode, error=%d\n", error);
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_EnableMultiConnections();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_InfrastructureConfigure::ESPWROOM32_AT_EnableMultiConnections, error=%d\n", error);
        }
    }

#if ESPWROOM32_BUFFER_ON_MODULE
    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_SocketSetReceiveMode();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_SocketConnect::ESPWROOM32_AT_SocketSetReceiveMode, error=%d\n", error);
        }
    }
#endif

    if(UEZ_ERROR_NONE == error)
    {
        error = ESPWROOM32_AT_DataWithoutRemoteAddress();
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_InfrastructureConfigure::ESPWROOM32_AT_DataWithoutRemoteAddress, error=%d\n", error);
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_InfrastructureBringUp
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_InfrastructureBringUp(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)aWorkspace;

    p->iProvMode = ESPWROOM32_MODE_STATION;
    memset(&p->iProvInfo, 0, sizeof(p->iProvInfo));

    error = ESPWROOM32_AT_EnableDHCP();
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed Network_ESPWROOM32_InfrastructureBringUp::ESPWROOM32_AT_EnableDHCP, error=%d\n", error);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_InfrastructureTakeDown
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_InfrastructureTakeDown(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_InfrastructureTakeDown not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_GetConnectionInfo
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_GetConnectionInfo(void *aWorkspace,
		T_uezNetworkSocket aSocket,
		T_uEZNetworkConnectionInfo *aConnection)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    /* TODO: Implement based on `???` */

    DEBUG_RTT_Printf(0, "Network_ESPWROOM32_GetConnectionInfo not implemented!\n");

    __BKPT(0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_ESPWROOM32_Create
 *---------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Create(
    const char *aName,
    const T_ESPWROOM32_Network_UARTSettings *aSettings)
{
    T_uezDeviceWorkspace *p_wireless;

    UEZDeviceTableRegister(aName,
                          (T_uezDeviceInterface *)&ESPWROOM32_Network_Interface,
                          0,
                          (T_uezDeviceWorkspace **)&p_wireless);

    return ESPWROOM32_Configure(p_wireless, aSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_Configure
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_Configure(
    void *aWorkspace,
    const T_ESPWROOM32_Network_UARTSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    TaskHandle_t taskHandle;

    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)aWorkspace;
    p->iUARTSettings                    = *aSettings;
    p->iMaxESPPacketSize                = ESPWROOM32_MAX_PACKET_WINDOW - 4;

    G_ESPWROOM32_Workspace              = aWorkspace;

    // Get the original handle's data
    error = uEZHandleGet(G_ESPWROOM32_Workspace->iATCmdRespTask, 0, 0, 0, (TUInt32 *)&taskHandle);
    if (error == UEZ_ERROR_NONE)
    {
        xTaskNotify((TaskHandle_t)taskHandle, 0, 0);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_CheckForMatch
 *---------------------------------------------------------------------------*/
static TInt32 ESPWROOM32_CheckForMatch(TUInt8 const * pTest, TUInt8 const * const pRef)
{
    TInt32 i = 0;

    while(('\0' != (pRef[i])) && ('\0' != (pTest[i])))
    {
        TInt32 temp1 = (int32_t)pRef[i];
        TInt32 temp2 = (int32_t)pTest[i];

        if(temp1 != temp2)
        {
            return 0;
        }

        i++;
    }

    return i;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ATCmdCallback
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_ATCmdCallback(T_ESPWROOM32_ATCmdMenu const * const pMenu,
                                             TUInt8 const * pInput,
                                             TUInt32 const bytes)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;

    for(TUInt32 i = 0; i < pMenu->numCmds; i++)
    {
        TInt32 length = ESPWROOM32_CheckForMatch(pInput, pMenu->cmdList[i].command);
        if(length>0)
        {
            if(NULL != pMenu->cmdList[i].callback)
            {
                T_ESPWROOM32_ATCmdArgs args;
                args.pRemainingString = &pInput[length];
                args.bytes = bytes-(uint32_t)length;
                pMenu->cmdList[i].callback(&args);

                error = UEZ_ERROR_NONE;
            }
        }
    }
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_RemoveQuotations
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_RemoveQuotations(char * str_in, char * str_out)
{
    int j=0;
    for(int i=0; str_in[i]!= '\0'; i++)
    {
        if (str_in[i] != 0x22)
        {
            str_out[j] = str_in[i];
            j++;
        }
    }

    str_out[j] = '\0';
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ModuleStatusHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ModuleStatusHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    char ATResponse[100];
    char *str;

    strncpy(ATResponse, (const char*)pArgs->pRemainingString, pArgs->bytes);
    str = (char *)strtok(ATResponse,"\r");
    p->iStationStatus = (T_ESPWROOM32_StationStatus) atoi(str);
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_SocketConnectionHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_SocketConnectionHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    char        ATResponse[100];
    char        *str;
    int8_t      link_id;
    uint8_t     i, tetype;
    char        type[5];
    char        IPAddress[17];
    static      uint8_t socket_count = 0;

    strncpy(ATResponse,(const char*)pArgs->pRemainingString, pArgs->bytes);
    str = (char *)strtok(ATResponse,",");
    link_id = (int8_t) atoi(str);
    for(i = 0; i < ESPWROOM32_SOCKET_MAX_CONNECTIONS; i++)
    {
        if(p->iSocket[i].linkID == link_id)
        {
            str = (char *)strtok(NULL,",");
            strncpy(type, str, strlen((char *)str));
            ESPWROOM32_RemoveQuotations(type, type);
            if(strcmp((const char *) type, (const char *)"TCP") == 0)
            {
                p->iSocket[i].protocolType = ESPWROOM32_SOCKET_PROTOCOL_TYPE_STREAM;
            }
            else if(strcmp((const char *) type, (const char *)"UDP") == 0)
            {
                p->iSocket[i].protocolType = ESPWROOM32_SOCKET_PROTOCOL_TYPE_DGRAM;
            }

            str = (char *)strtok(NULL,",");
            strncpy(IPAddress, str, strlen((char *)str));
            ESPWROOM32_RemoveQuotations(IPAddress, IPAddress);

            str = (char *)strtok(NULL,",");
            p->iSocket[i].port = (uint16_t) atoi(str);
            /* TODO: Do we need local IP address? */
#if 0
            str = (char *)strtok(NULL,",");
            p->iSocket[i].local_ip_addr.sin_port = (uint16_t) atoi(str);
#endif
            str = (char *)strtok(NULL,"\r");
            tetype = (uint8_t) atoi(str);
            if(tetype == 0)
            {
                p->iSocket[i].connectionType = ESPWROOM32_SOCKET_CONNECTION_TYPE_CLIENT;
            }
            else if(tetype == 1)
            {
                p->iSocket[i].connectionType = ESPWROOM32_SOCKET_CONNECTION_TYPE_SERVER;
            }

            str = (char *)strtok(IPAddress,".");
            p->iSocket[i].networkAddr.v4[0] = (uint8_t) atoi(str);
            str = (char *)strtok(NULL,".");
            p->iSocket[i].networkAddr.v4[1] = (uint8_t) atoi(str);
            str = (char *)strtok(NULL,".");
            p->iSocket[i].networkAddr.v4[2] = (uint8_t) atoi(str);
            str = (char *)strtok(NULL,"\0");
            p->iSocket[i].networkAddr.v4[3] = (uint8_t) atoi(str);
            socket_count++;
        }
    }

    if(socket_count >= p->iSocketCount)
    {
        UEZSemaphoreRelease(p->iCmdDoneSem);
        socket_count = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_NetworkStatusHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_NetworkStatusHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    char ATResponse[100];
    char *str;
    TUInt8 add_1;
    TUInt8 add_2;
    TUInt8 add_3;
    TUInt8 add_4;

    if(p->iCommand == ESPWROOM32_AT_CMD_INDEX_CIPSTATUS)
    {
        return;
    }

    strncpy(ATResponse,(const char*)pArgs->pRemainingString, pArgs->bytes);
    str = strtok(ATResponse,"\x22");
    if(strcmp((const char *)"ip:", (const char *) str) == 0)
    {
        str = strtok(NULL,".");
        add_1 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_2 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_3 = (TUInt8) atoi(str);
        str = strtok(NULL,"\x22");
        add_4 = (TUInt8) atoi(str);
        p->iCurrentIPAddress = (TUInt32) ((((TUInt32) add_1) << 24) | (((TUInt32) add_2) << 16) | (((TUInt32) add_3) << 8) | ((TUInt32) add_4));
    }
    else if(strcmp((const char *)"gateway:", (const char *) str) == 0)
    {
        str = strtok(NULL,".");
        add_1 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_2 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_3 = (TUInt8) atoi(str);
        str = strtok(NULL,"\x22");
        add_4 = (TUInt8) atoi(str);
        p->iCurrentGateway = (TUInt32) ((((TUInt32) add_1) << 24) | (((TUInt32) add_2) << 16) | (((TUInt32) add_3) << 8) | ((TUInt32) add_4));
    }
    else if(strcmp((const char *)"netmask:", (const char *) str) == 0)
    {
        str = strtok(NULL,".");
        add_1 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_2 = (TUInt8) atoi(str);
        str = strtok(NULL,".");
        add_3 = (TUInt8) atoi(str);
        str = strtok(NULL,"\x22");
        add_4 = (TUInt8) atoi(str);
        p->iCurrentMask = (TUInt32) ((((TUInt32) add_1) << 24) | (((TUInt32) add_2) << 16) | (((TUInt32) add_3) << 8) | ((TUInt32) add_4));

        UEZSemaphoreRelease(p->iCmdDoneSem);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ScanSSIDHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ScanSSIDHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    UNUSED(p);
    UNUSED(error);

    /* TODO: Implement based on `scan_ssid_handler` */
    
    DEBUG_RTT_Printf(0, "ESPWROOM32_ScanSSIDHandler not implemented!\n");

    __BKPT(0);

    return;// error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_GetMACHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_GetMACHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    UNUSED(p);
    UNUSED(error);

    /* TODO: Implement based on `get_mac_handler` */
    
    DEBUG_RTT_Printf(0, "ESPWROOM32_GetMACHandler not implemented!\n");

    __BKPT(0);
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ReceiveSendingDataHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ReceiveSendingDataHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    UNUSED(p);
    char ATResponse[100];
    char *str;

    //DEBUG_RTT_Printf(0, "\nESPWROOM32_ReceiveSendingDataHandler\n");

    strncpy(ATResponse, (const char*)pArgs->pRemainingString, pArgs->bytes);
    str = (char *)strtok(ATResponse," ");
    p->iTransmitDataLength = (int16_t)atoi(str);
    UEZSemaphoreRelease(p->iCmdDoneSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_QueryStationConnectionHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_QueryStationConnectionHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    UNUSED(p);
    UNUSED(error);

    /* TODO: Implement based on `query_station_connection` */
    
    DEBUG_RTT_Printf(0, "ESPWROOM32_QueryStationConnectionHandler not implemented!\n");

    __BKPT(0);
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ResolveDomainHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ResolveDomainHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    char ATResponse[100];
    char *str;

    strncpy(ATResponse,(const char*)pArgs->pRemainingString, pArgs->bytes);
    str = strtok(ATResponse,".");
    p->iResolvedAddr.v4[0] = (TUInt8) atoi(str + 1);
    str = strtok(NULL,".");
    p->iResolvedAddr.v4[1] = (TUInt8) atoi(str);
    str = strtok(NULL,".");
    p->iResolvedAddr.v4[2] = (TUInt8) atoi(str);
    str = strtok(NULL,"\x22");
    p->iResolvedAddr.v4[3] = (TUInt8) atoi(str);

    UEZSemaphoreRelease(p->iCmdDoneSem);

    return;
}

#if ESPWROOM32_BUFFER_ON_MODULE
/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ReceiveDataHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ReceiveDataHandler(T_ESPWROOM32_ATCmdArgs *pArgs)
{
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    char ATResponse[100];
    char *str;

    if(pArgs->bytes < 100)
    {
        strncpy(ATResponse,(const char*)pArgs->pRemainingString, pArgs->bytes);
    }
    else
    {
        strncpy(ATResponse,(const char*)pArgs->pRemainingString, 100);
    }
    str = strtok(ATResponse,",");
    p->iReceivedActualLength = (TInt32) atoi(str);

    UEZSemaphoreRelease(p->iCmdDoneSem);

    return;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_DisableATEcho
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_DisableEcho( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    
    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_ATE;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) DisableECHO;
        OutLength = sizeof(DisableECHO)-1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_GetVersionInfo
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_GetVersionInfo( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    
    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_GMR;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) GetVersionInfo;
        OutLength = sizeof(GetVersionInfo)-1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_ListSupportedCommands
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_ListSupportedCommands( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    
    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CMD;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) ListSupportedCommands;
        OutLength = sizeof(ListSupportedCommands)-1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SetWIFIMode
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_SetWIFIMode( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[16]={'\0'};

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CWMODE;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        switch(p->iProvMode)
        {
            case ESPWROOM32_MODE_MIXED:
                sprintf(Connection_command,"%s=%d\r\n#",SetWiFiMode, 3);
                break;

            case ESPWROOM32_MODE_STATION:
                sprintf(Connection_command,"%s=%d\r\n#",SetWiFiMode, 1);
                break;

            case ESPWROOM32_MODE_SOFTAP:
                sprintf(Connection_command,"%s=%d\r\n#",SetWiFiMode, 2);
                break;
        }

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_EnableMultiConnections
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_EnableMultiConnections( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPMUX;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) EnableMultiConnection;
        OutLength = sizeof(EnableMultiConnection)-1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_DataWithoutRemoteAddress
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_DataWithoutRemoteAddress( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPDINFO;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) NoShowRemoteIP;
        OutLength = sizeof(NoShowRemoteIP)-1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_ConnectToAP
 *---------------------------------------------------------------------------*/
static T_uezError   ESPWROOM32_AT_ConnectToAP( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[64]={'\0'};

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CWJAP;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        sprintf(Connection_command,"%s=\x22%s\x22,\x22%s\x22\r\n#",SetProvInStation, ESPWROOM32_SSID, ESPWROOM32_PW);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_EnableDHCP
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_EnableDHCP( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[20]={'\0'};

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CWDHCP;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        switch(p->iProvMode)
        {
            case ESPWROOM32_MODE_MIXED:
                sprintf(Connection_command,"%s=%d,%d\r\n#",SetDHCPConfig, 1, 3);
                break;

            case ESPWROOM32_MODE_STATION:
                sprintf(Connection_command,"%s=%d,%d\r\n#",SetDHCPConfig, 1, 1);
                break;

            case ESPWROOM32_MODE_SOFTAP:
                sprintf(Connection_command,"%s=%d,%d\r\n#",SetDHCPConfig, 1, 2);
                break;
        }

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_IPAddressGet
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_IPAddressGet( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[20]={'\0'};

    p->iCmdBufferIndex = 0;
    p->pCmdBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
    if(NULL == p->pCmdBuffer)
    {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }

    if(UEZ_ERROR_NONE == error)
    {
        switch(p->iProvMode)
        {
            case ESPWROOM32_MODE_MIXED:
                error = UEZ_ERROR_NOT_SUPPORTED;
                break;

            case ESPWROOM32_MODE_STATION:
                sprintf(Connection_command,"%s\r\n#",GetIPStatusInStation);
                break;

            case ESPWROOM32_MODE_SOFTAP:
                sprintf(Connection_command,"%s\r\n#",GetIPStatusInAP);
                break;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    }

    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = true;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPSTA;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        xTimerChangePeriod(p->iResponseTimer, 200, 0);
        xTimerStart(p->iResponseTimer, 0);

        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iCmdDoneSem, 1000);
    }

    UEZMemFree(p->pCmdBuffer);
    p->pCmdBuffer = 0;
    p->iRXIsData = false;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_ResolveDomain
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_ResolveDomain( const char * aDomain, T_uezNetworkAddr * aAddr )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[80]={'\0'};

    p->iCmdBufferIndex = 0;
    p->pCmdBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
    if(NULL == p->pCmdBuffer)
    {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    }

    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = true;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPDOMAIN;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        sprintf(Connection_command,"%s=\"%s\",2\r\n#", ResolveDomain, aDomain);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);
 
        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        xTimerChangePeriod(p->iResponseTimer, 500, 0);
        xTimerStart(p->iResponseTimer, 0);

        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iCmdDoneSem, 1000);
    }

    if(UEZ_ERROR_NONE == error)
    {
        *aAddr = p->iResolvedAddr;
    }

    UEZMemFree(p->pCmdBuffer);
    p->pCmdBuffer = 0;
    p->iRXIsData = false;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_ClientSocketCreate
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_ClientSocketCreate( T_uezNetworkSocket aSocket )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[64]={'\0'};
    char ip_add[20]={'\0'};

    if(p->iSocket[aSocket].linkID == -1)
    {
        error = UEZ_ERROR_INVALID_PARAMETER;
    }

    if(UEZ_ERROR_NONE == error)
    {
        /** set IP address */
        sprintf(ip_add,"\x22%d.%d.%d.%d\x22", (uint8_t) (p->iSocket[aSocket].networkAddr.v4[0]), 
                                              (uint8_t) (p->iSocket[aSocket].networkAddr.v4[1]), 
                                              (uint8_t) (p->iSocket[aSocket].networkAddr.v4[2]),
                                              (uint8_t) (p->iSocket[aSocket].networkAddr.v4[3]));

        switch(p->iSocket[aSocket].protocolType)
        {
            case ESPWROOM32_SOCKET_PROTOCOL_TYPE_STREAM:
                if(p->iSocket[aSocket].tcpKeepAlive)
                {
                    sprintf(Connection_command, "%s=%i,%s,%s,%u,%d\r\n#", ClientSocketCreate,
                            (uint8_t)(p->iSocket[aSocket].linkID), TypeTCP,
                            ip_add, (uint32_t) p->iSocket[aSocket].port, 7200);
                }
                else
                {
                    sprintf(Connection_command,"%s=%i,%s,%s,%u,%d\r\n#", ClientSocketCreate,
                            (uint8_t)(p->iSocket[aSocket].linkID), TypeTCP,
                            ip_add, (uint32_t) p->iSocket[aSocket].port, 0);
                }

                break;

            case ESPWROOM32_SOCKET_PROTOCOL_TYPE_DGRAM:
                sprintf(Connection_command,"%s=%i,%s,%s,%u\r\n#", ClientSocketCreate,
                                        (uint8_t)(p->iSocket[aSocket].linkID), TypeUDP,
                                        ip_add, (uint32_t) p->iSocket[aSocket].port);
                break;

            case ESPWROOM32_SOCKET_PROTOCOL_TYPE_SSL:
                sprintf(Connection_command,"%s=%i,%s,%s,%u\r\n#",ClientSocketCreate,
                                        (uint8_t)(p->iSocket[aSocket].linkID), TypeSSL,
                                        ip_add, (uint32_t) p->iSocket[aSocket].port);
                break;

            default:
                error = UEZ_ERROR_NOT_SUPPORTED;
                break;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    }

    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPSTART;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        error = UEZSemaphoreGrab(p->iSocketConnectionUpdateSem, 500);
    }

    if(UEZ_ERROR_NONE == error)
    {
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_GetConnectionStatus
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_GetConnectionStatus( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;

    p->iCmdBufferIndex = 0;
    p->pCmdBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
    if(NULL == p->pCmdBuffer)
    {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    }

    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = true;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPSTATUS;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        pOutData = (void *) GetNetworkStat;
        OutLength = sizeof(GetNetworkStat) - 1;

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        xTimerChangePeriod(p->iResponseTimer, 200, 0);
        xTimerStart(p->iResponseTimer, 0);

        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iCmdDoneSem, 1000);
    }

    UEZMemFree(p->pCmdBuffer);
    p->pCmdBuffer = 0;
    p->iRXIsData = false;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SocketSetReceiveMode
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_SocketSetReceiveMode( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[64]={'\0'};

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPRECVMODE;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();

        //sprintf(Connection_command, "%s=5,1\r\n#", SetReceiveMode);
        sprintf(Connection_command, "%s=1\r\n#", SetReceiveMode);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\n%s", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SocketClose
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_SocketClose( T_uezNetworkSocket aSocket )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[20]={'\0'};

    DEBUG_RTT_Printf(0, "\r\nESPWROOM32_AT_SocketClose\r\n");

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPCLOSE;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        sprintf(Connection_command,"%s=%d\r\n#", CloseSocketConn, (uint8_t)p->iSocket[aSocket].linkID);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SocketAbort
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_SocketAbort( void )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[20]={'\0'};

    error = UEZSemaphoreGrab(p->iATCmdMutex, 500);
    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPCLOSE;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();    

        sprintf(Connection_command,"%s=%d\r\n#", CloseSocketConn, (uint8_t)p->iTCPSocketLinkID);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SocketSendTo
 *---------------------------------------------------------------------------*/
static T_uezError ESPWROOM32_AT_SocketSendTo( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 aTimeout )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[32]={'\0'};

    if(aNumBytes > p->iMaxESPPacketSize)
    {
        error = UEZ_ERROR_OUT_OF_SPACE;
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "\nWaiting for iATCmdMutex...");
        error = UEZSemaphoreGrab(p->iATCmdMutex, aTimeout);
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "Got it!\n");
        p->iRXIndex     = 0;
        p->iRXIsData    = false;
        p->iCommand     = ESPWROOM32_AT_CMD_INDEX_CIPSENDEX;
        p->iCurrentTask = xTaskGetCurrentTaskHandle();

        if(aSocket != 0)
        {
            __BKPT(0);
        }

        sprintf(Connection_command,"%s=%i,%u\r\n#", SocketSend, aSocket, aNumBytes);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        //DEBUG_RTT_Printf(0, "\nWaiting for Task Notify...");
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EDATATIMEO;
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "Got it!\n");
        //DEBUG_RTT_Printf(0, "\nWaiting for iCmdDoneSem...");
        error = UEZSemaphoreGrab(p->iCmdDoneSem, aTimeout);
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "Got it!\n");
        p->iCmdBufferIndex = 0;
        p->pCmdBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
        if(NULL == p->pCmdBuffer)
        {
            error = UEZ_ERROR_OUT_OF_MEMORY;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "\nWaiting for iATCmdMutex...");
        error = UEZSemaphoreGrab(p->iATCmdMutex, aTimeout);
    }

    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "Got it!\n");
        p->iRXIsData = true;

        DEBUG_RTT_Write(0, aData, aNumBytes);
        for(TUInt32 aByte = 0; aByte < aNumBytes; aByte++)
        {
            TUInt8 data[2] = { 0 };
            TUInt8 len = 1;
            switch(((TUInt8 *)aData)[aByte])
            {
                case '\\':
                    data[0] = '\\';
                    data[1] = ((TUInt8 *)aData)[aByte];
                    len = 2;
                    break;
                default:
                    data[0] = ((TUInt8 *)aData)[aByte];
                    break;
            }
            error = UEZStreamWrite(p->iUART, data, len, NULL, 500);
        }
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        /* Need to wait long enough so that iRXIsData is still "true" when '\r\nRecv' is received,
           but not TOO long where it happens after the +IPD from data being received */
        xTimerChangePeriod(p->iResponseTimer, 200, 0);
        xTimerStart(p->iResponseTimer, 0);
    }
#if ESPWROOM32_BUFFER_ON_MODULE
    if(UEZ_ERROR_NONE == error)
    {
        //DEBUG_RTT_Printf(0, "\nWaiting for iCmdDoneSem...");
        error = UEZSemaphoreGrab(p->iCmdDoneSem, aTimeout);
    }
#endif
    if(UEZ_ERROR_NONE == error)
    {
#if 0
        DEBUG_RTT_Printf(0, "Got it!\n");
#endif
        //DEBUG_RTT_Printf(0, "\nWaiting for Task Notify...");
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EDATATIMEO;
            error = UEZ_ERROR_TIMEOUT;
        }
        else
        {
            //DEBUG_RTT_Printf(0, "Got it!\n");
            /* TODO: Need iTransmitDataLength? */
            /* ret = p->iTransmitDataLength; */
        }
    }

    UEZMemFree(p->pCmdBuffer);
    p->pCmdBuffer = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_AT_SocketReceiveFrom
 *---------------------------------------------------------------------------*/
#if ESPWROOM32_BUFFER_ON_MODULE
static T_uezError ESPWROOM32_AT_SocketReceiveFrom( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 * aNumBytesRead, TUInt32 aTimeout )
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace *p   = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[32]={'\0'};

    p->iCmdBufferIndex = 0;
    p->pCmdBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
    if(NULL == p->pCmdBuffer)
    {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iATCmdMutex, aTimeout);
    }

    if(UEZ_ERROR_NONE == error)
    {
        p->iRXIndex                 = 0;
        p->iRXIsData                = true;
        p->iCommand                 = ESPWROOM32_AT_CMD_INDEX_CIPRECVDATA;
        p->iCurrentTask             = xTaskGetCurrentTaskHandle();
        p->iReceivedActualLength    = 0;
            

        sprintf(Connection_command,"%s=%i,%u\r\n#", SocketRecv, aSocket, aNumBytes);

        pOutData = strtok(Connection_command,"#");
        OutLength = strlen((char *)pOutData);

        error = UEZStreamWrite(p->iUART, 
                               pOutData, 
                               OutLength, 
                               NULL, 
                               500);
        UEZSemaphoreRelease(p->iATCmdMutex);
    }

    if(UEZ_ERROR_NONE == error)
    {
        xTimerChangePeriod(p->iResponseTimer, 500, 0);
        xTimerStart(p->iResponseTimer, 0);

        DEBUG_RTT_Printf(0, "%s\n", pOutData);
        BaseType_t notify = xTaskNotifyWait(0, 0, NULL, 500);
        if(pdFAIL == notify)
        {
            error = UEZ_ERROR_TIMEOUT;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZSemaphoreGrab(p->iCmdDoneSem, aTimeout);
    }

    if(UEZ_ERROR_NONE == error)
    {
        if(p->iReceivedActualLength == 0)
        {
            error = UEZ_ERROR_NOT_READY;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        TUInt8 *pData = p->pCmdBuffer;
        while(*pData++ != ',');

#if SEGGER_ENABLE_RTT && 0
        for(TUInt32 write_index = 0; write_index < p->iReceivedActualLength; write_index++)
        {
            if(pData[write_index] > 31)
            {
                DEBUG_RTT_Printf(0, "%c", (char)pData[write_index]);
                //DEBUG_RTT_Printf(0, "%c%c%d%c", (char)data, (p->iRXIsData == true)?'1':'0', p->iCommand, (p->iGetNetworkData)?'1':'0');
            }
            else if(pData[write_index] == 0x0A)
            {
                DEBUG_RTT_Printf(0, "LF");
            }
            else if(pData[write_index] == 0x0D)
            {
                DEBUG_RTT_Printf(0, "CR");
            } 
            else
            {
                DEBUG_RTT_Printf(0, "%02X", (uint8_t)pData[write_index]);
            }
            UEZTaskDelay(1);
        }
        DEBUG_RTT_Printf(0, "\n");
        UEZTaskDelay(1000);
#endif
        memcpy(aData, pData, p->iReceivedActualLength);
        if(NULL != aNumBytesRead)
        {
            *aNumBytesRead = p->iReceivedActualLength;
        }
    }

    UEZMemFree(p->pCmdBuffer);
    p->pCmdBuffer = 0;
    p->iRXIsData = false;

    return error;    
}
#else
static T_uezError ESPWROOM32_AT_SocketReceiveFrom( T_uezNetworkSocket aSocket, void *aData, TUInt32 aNumBytes, TUInt32 aTimeout )
{
    T_uezError                      error           = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace  *p              = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;
    T_networkPacket                 *pNetworkPacket = { 0 };
    TInt16                          total_length    = 0;
    T_networkPacket                 *packet         = NULL;
    T_networkPacket                 *next_packet    = NULL;
#if 0
    void * pOutData = 0;
    TUInt32 OutLength = 0;
    char Connection_command[32]={'\0'};
#endif

    error = UEZSemaphoreGrab(p->iRXDoneSem, aTimeout);
    if(UEZ_ERROR_NONE == error)
    {
        DEBUG_RTT_Printf(0, "\nESPWROOM32_AT_SocketReceiveFrom::UEZSemaphoreGrab p->iRXDoneSem\n");
        error = UEZQueueReceive(p->iRXDataQueue, (void *)&pNetworkPacket, aTimeout);
        if(UEZ_ERROR_TIMEOUT == error)
        {
            p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EDATATIMEO;
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        packet = pNetworkPacket;

        do
        {
            total_length = (int16_t)(total_length + packet->iDataLength);
            packet = packet->iNextNetworkPacket;
        } while(packet != NULL);

        if(total_length > aNumBytes)
        {
            total_length = (int16_t)aNumBytes;
        }

        packet = pNetworkPacket;

        int16_t data_len = 0;
        while(packet != NULL)
        {
            if(total_length >= packet->iDataLength)
            {
                memcpy(aData + data_len, packet->iPacketDataStart, (size_t)packet->iDataLength);
                data_len = (int16_t)(data_len + packet->iDataLength);
                total_length = (int16_t)(total_length - packet->iDataLength);
            }
            else
            {
                memcpy(aData + data_len, packet->iPacketDataStart, (size_t)total_length);
            }

            packet = packet->iNextNetworkPacket;
        }

        if(p->iSocket[aSocket].linkID != pNetworkPacket->iLinkID)
        {
            p->iSocket[aSocket].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_EBADF;
            error = UEZ_ERROR_INVALID_PARAMETER;
        }

        /** release packet allocation */
        next_packet = pNetworkPacket;
        while(next_packet != NULL)
        {
            UEZMemFree(next_packet->iPacketDataStart);
            next_packet->iPacketDataStart = 0;

            packet = next_packet;
            next_packet = packet->iNextNetworkPacket;
            packet->iDataLength = 0;
            packet->iLinkID = 0;
            packet->iPrePacketIndex = 0;
            packet->iNextNetworkPacket = NULL;
        }
    }

    return error;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ResponseTimeoutHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_ResponseTimeoutHandler( TimerHandle_t pxTimer )
{
    T_uezError                      error   = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace  *p      = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;

    DEBUG_RTT_Printf(0, "\r\nESPWROOM32_ResponseTimeoutHandler\r\n");

    p->iRXIndex = 0;
    p->iRXIsData = false;
    p->iSizeOfResponse = 0;
    error = UEZSemaphoreRelease(p->iTXDoneSem);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "Failed ESPWROOM32_ResponseTimeoutHandler::UEZSemaphoreRelease, error=%d\n", error);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_SocketStatusHandler
 *---------------------------------------------------------------------------*/
static void ESPWROOM32_SocketStatusHandler( TimerHandle_t pxTimer )
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Network_ESPWROOM32_Workspace  *p = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;

    for(uint8_t i = 0; i < ESPWROOM32_SOCKET_MAX_CONNECTIONS; i++)
    {
        if(p->iSocket[i].linkID == p->iTCPSocketLinkID)
        {
            if(p->iSocketStatus[p->iTCPSocketLinkID] == ESPWROOM32_SOCKET_STATUS_CLOSED)
            {
                DEBUG_RTT_Printf(0, "Setting iSocket[%d] to ESPWROOM32_SOCKET_STATUS_CLOSED");
                p->iSocket[i].linkID = -1;
                p->iSocket[i].protocolType = ESPWROOM32_SOCKET_PROTOCOL_TYPE_RAW;
                p->iSocket[i].connectionType = ESPWROOM32_SOCKET_CONNECTION_TYPE_DISCONNECT;
                p->iSocket[i].errorCode = ESPWROOM32_SOCKET_ERROR_CODE_NONE;
                p->iSocket[i].tcpKeepAlive = false;
                /* TODO: Do we need local IP address? */
#if 0
                memset ((char *) &p->iSocket[i].local_ip_addr, 0,
                        sizeof(p->iSocket[i].local_ip_addr));
#endif
                memset ((char *) &p->iSocket[i].networkAddr, 0,
                                        sizeof(p->iSocket[i].networkAddr));
               p->iSocket[i].status = NULL;
               p->iSocketCount--;
               UEZSemaphoreRelease(p->iSocketConnectionUpdateSem); // TODO: Create socket_connection_update_semaphore
               return;
            }
            else if(p->iSocketStatus[p->iTCPSocketLinkID] == ESPWROOM32_SOCKET_STATUS_ESTABLISHED)
            {
               if(p->iSocket[i].connectionType == ESPWROOM32_SOCKET_CONNECTION_TYPE_CLIENT)
               {
                  p->iSocketCount++;
                  p->iSocket[i].status = &p->iSocketStatus[p->iTCPSocketLinkID];
                  UEZSemaphoreRelease(p->iSocketConnectionUpdateSem); // TODO: Create based on socket_connection_update_semaphore
               }

               return;
            }
        }
    }

    for(uint8_t i = 0; i < ESPWROOM32_SOCKET_MAX_CONNECTIONS; i++)
    {
        if((p->iSocket[i].linkID == -1) && (p->iSocketStatus[p->iTCPSocketLinkID] == ESPWROOM32_SOCKET_STATUS_ESTABLISHED))
        {
            /** assign a new socket for the connection from TCP clients */
            if((p->iSocket[i].connectionType == ESPWROOM32_SOCKET_CONNECTION_TYPE_SERVER) && (p->iListeningConnectionList[p->iListeningConnectionIndex] == i))
            {
                if(p->iListeningConnectionIndex < p->iMaxTCPServerConnections)
                {
                    p->iSocket[i].linkID = p->iTCPSocketLinkID;
                    p->iSocket[i].status = &p->iSocketStatus[p->iTCPSocketLinkID];
                    p->iSocketCount++;
                    p->iListeningConnectionIndex++;
                    UEZSemaphoreRelease(p->iAcceptSem); // TODO: Create based on accept_semaphore

                }
                else
                {
                    error = ESPWROOM32_AT_SocketAbort();
                    if(UEZ_ERROR_NONE != error)
                    {
                        DEBUG_RTT_Printf(0, "Failed ESPWROOM32_SocketStatusHandler::ESPWROOM32_AT_SocketAbort, error=%d\n", error);
                    }
                }

                return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_ATCmdRespHandlerTask
 *---------------------------------------------------------------------------*/
TUInt32 ESPWROOM32_ATCmdRespHandler(T_uezTask aMyTask, void *aParams)
{
    T_uezError                      error       = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace  *p          = 0;
    TUInt8                          strIndex    = 0;
    TUInt16                         maxBytes    = 0;
    TUInt16                         bytes       = 0;
    TUInt8                          length      = 0;

    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    p = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;

    memset ((char *) p->iRXPacketHeader, 0x00, sizeof(p->iRXPacketHeader));
    memset ((char *) p->iRXNetworkPacket, 0x00, sizeof(p->iRXNetworkPacket));

    while(1)
    {
        error = UEZSemaphoreGrab(p->iTXDoneSem, UEZ_TIMEOUT_INFINITE);
        if(UEZ_ERROR_NONE == error)
        {
#if (ESPWROOM32_BUFFER_ON_MODULE == 0)
            if(p->iCommand == ESPWROOM32_AT_CMD_INDEX_IPD)
            {
                error = UEZSemaphoreRelease(p->iRXDoneSem);
                if(UEZ_ERROR_NONE != error)
                {
                    UEZSemaphoreGrab(p->iRXDoneSem, 500);
                    DEBUG_RTT_Printf(0, "\nESPWROOM32_ATCmdRespHandler::UEZSemaphoreGrab p->iRXDoneSem\n");
                    UEZMemFree(p->pInputBufferLast);
                    p->pInputBufferLast = 0;
                }
                else
                {
                    DEBUG_RTT_Printf(0, "\nESPWROOM32_ATCmdRespHandler::UEZSemaphoreRelease p->iRXDoneSem\n");
                    //DEBUG_RTT_Printf(0, "\nESPWROOM32_ATCmdRespHandler::UEZSemaphoreRelease p->iRXDoneSem\n");
                    p->pInputBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
                    if(NULL == p->pInputBuffer)
                    {
                        error = UEZ_ERROR_OUT_OF_MEMORY;
                    }
                    else
                    {
                        p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iPacketDataStart = p->pInputBufferLast;
                        T_networkPacket *firstPacket = NULL;
                        TUInt8 prePacketIndex = p->iRXNetworkPacketIndex;
                        do
                        {
                            if(p->iRXNetworkPacket[prePacketIndex].iPrePacketIndex == 0)
                            {
                                firstPacket = &p->iRXNetworkPacket[prePacketIndex];
                            }
                            else
                            {
                                prePacketIndex = p->iRXNetworkPacket[prePacketIndex].iPrePacketIndex;
                            }
                        } while (firstPacket == NULL);

                        error = UEZQueueSend(p->iRXDataQueue, &firstPacket, UEZ_TIMEOUT_NONE);
                        if(UEZ_ERROR_NONE == error)
                        {
                            p->iRXNetworkPacketIndex++;
                            p->iRXNetworkPacketIndex = p->iRXNetworkPacketIndex % ESPWROOM32_MAX_PACKETS_IN_POOL;
                        }
                    }
                }

                p->iCommand = ESPWROOM32_AT_CMD_INDEX_NONE;
                //DEBUG_RTT_Printf(0, "\r\nESPWROOM32_AT_CMD_INDEX_NONE\r\n");
                UEZSemaphoreRelease(p->iATCmdMutex);
            }
            else
#endif
            {
                if(p->iTransmissionMode == ESPWROOM32_TX_MODE_NORMAL)
                {
                    if(p->iCommand == ESPWROOM32_AT_CMD_INDEX_CWLAP)
                    {
                        p->iMaxScanIndex = p->iPacketIndex;
                    }

                    for(strIndex = 0; strIndex < p->iPacketIndex; strIndex++)
                    {
                        //DEBUG_RTT_Printf(0, "\nstrIndex=%d\n", strIndex);
                        //DEBUG_RTT_Printf(0, "\niPacketLength=%d\n", p->iATInterfacePacket[strIndex].iPacketLength);
                        maxBytes = p->iATInterfacePacket[strIndex].iPacketLength;
                        length = 0;
                        bytes = maxBytes;
                        while(length < maxBytes)
                        {
                            error = ESPWROOM32_ATCmdCallback(p->pRootMenu, p->iATInterfacePacket[strIndex].pPacketDataStart, bytes);
                            if(UEZ_ERROR_NONE == error)
                            {
                                break;
                            }

                            length++;
                            bytes = maxBytes - length;
                        }
                    }

                    p->iPacketIndex = 0;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ESPWROOM32_RXHandler
 *---------------------------------------------------------------------------*/
TUInt32 ESPWROOM32_RXHandler(T_uezTask aMyTask, void *aParams)
{
    T_uezError                      error       = UEZ_ERROR_NONE;
    T_Network_ESPWROOM32_Workspace  *p          = 0;
    TUInt8                          data        = 0;
    TaskHandle_t                    taskHandle  = 0;
    char                            *str;
    UNUSED(taskHandle);
    UNUSED(str);
    UNUSED(p);
    
    UNUSED(ESPWROOM32_AT_ListSupportedCommands);
    UNUSED(ESPWROOM32_AT_GetVersionInfo);

    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    p = (T_Network_ESPWROOM32_Workspace *)G_ESPWROOM32_Workspace;

    while(1)
    {
        error = UEZStreamRead(p->iUART, &data, 1, NULL, portMAX_DELAY);
        if(UEZ_ERROR_NONE != error)
        {
            DEBUG_RTT_Printf(0, "Failed ESPWROOM32_RXHandler::UEZStreamRead, error=%d\n", error);
        }
        else
        {
#if SEGGER_ENABLE_RTT && 1
            if(data > 31)
            {
                DEBUG_RTT_Printf(0, "%c", (char)data);
                //DEBUG_RTT_Printf(0, "%c%c%d%c", (char)data, (p->iRXIsData == true)?'1':'0', p->iCommand, (p->iGetNetworkData)?'1':'0');
            }
            else if(data == 0x0A)
            {
                DEBUG_RTT_Printf(0, "\n");
            }
            else if(data == 0x0D)
            {
                //DEBUG_RTT_printf(0, "\r");
            } 
            else
            {
                DEBUG_RTT_Printf(0, "0x%02x", (uint8_t)data);
            }
#endif
            p->iRXBuffer[p->iRXIndex] = data;
            p->iRXIndex++;
            if(p->iRXIndex > (ESPWROOM32_AT_RX_SIZE - 1))
            {
                p->iRXIndex = 0;
            }

            if((data == 0x3E) && (p->iCommand == ESPWROOM32_AT_CMD_INDEX_CIPSENDEX))
            {
                /** ESP32 start to receive serial data */
                UEZSemaphoreRelease(p->iCmdDoneSem);
                p->iRXIndex = 0;
            }

            if(p->iRXIndex >= 2)
            {
                if((p->iRXBuffer[p->iRXIndex-2] == 'O') && (p->iRXBuffer[p->iRXIndex-1] == 'K'))
                {
                    xTaskNotify(G_ESPWROOM32_Workspace->iCurrentTask, 0, 0);
                    
                    p->iRXIndex = 0;
                }
            }

            if(p->iRXIsData == true)
            {
#if (ESPWROOM32_BUFFER_ON_MODULE == 0)
                if(p->iCommand == ESPWROOM32_AT_CMD_INDEX_IPD)
                {
                    if(p->iGetNetworkData)
                    {
                        *(p->pInputBuffer + p->iRXDataLength) = data;
                        p->iRXDataLength++;

#if SEGGER_ENABLE_RTT && 0
                        if(data > 31)
                        {
                            DEBUG_RTT_Printf(0, "%c", (char)data);
                        }
                        else if(data == 0x0A)
                        {
                            DEBUG_RTT_Printf(0, "\n");
                        }
                        else if(data == 0x0D)
                        {
                            //DEBUG_RTT_printf(0, "\r");
                        } 
                        else
                        {
                            DEBUG_RTT_Printf(0, "%02X", (uint8_t)data);
                        }
#endif
                        if(p->iRXDataLength == p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iDataLength)
                        {                            
                            p->pInputBufferLast = p->pInputBuffer;
                            /** remove setting p->iRXIndex = 0, no necessary */
                            p->iRXIsData = false;
                            p->iRXDataLength = 0;
                            p->iTotalRXDataSize = 0;
                            p->iGetNetworkData = false;
#if 1
                            xTimerChangePeriod(p->iResponseTimer, 500, 0);
                            xTimerStart(p->iResponseTimer, 1);
#else
                            UEZSemaphoreRelease(p->iTXDoneSem);
#endif
                        }
                        else if(p->iRXDataLength == p->iMaxESPPacketSize)
                        {
                            /** allocate a new block to store next coming RX packet */
                            p->iRXNetworkPacket[(p->iRXNetworkPacketIndex+1)%(uint8_t)ESPWROOM32_MAX_PACKETS_IN_POOL].iLinkID = p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iLinkID;
                            p->iRXNetworkPacket[(p->iRXNetworkPacketIndex+1)%(uint8_t)ESPWROOM32_MAX_PACKETS_IN_POOL].iDataLength = (int16_t)(p->iTotalRXDataSize - p->iMaxESPPacketSize);
                            p->iRXNetworkPacket[(p->iRXNetworkPacketIndex+1)%(uint8_t)ESPWROOM32_MAX_PACKETS_IN_POOL].iPrePacketIndex = p->iRXNetworkPacketIndex;
                            p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iPacketDataStart = p->pInputBuffer;
                            p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iNextNetworkPacket = &p->iRXNetworkPacket[(p->iRXNetworkPacketIndex+1)%(uint8_t)ESPWROOM32_MAX_PACKETS_IN_POOL];
                            p->pInputBuffer = UEZMemAlloc(ESPWROOM32_MAX_PACKET_WINDOW);
                            p->iRXNetworkPacketIndex++;
                            p->iRXNetworkPacketIndex = p->iRXNetworkPacketIndex % (uint8_t)ESPWROOM32_MAX_PACKETS_IN_POOL;
                            p->iRXDataLength = 0;
                        }
                    }
                    else
                    {
                        p->iRXPacketHeader[p->iRXPacketHeaderIndex] = (uint8_t)data;
                        p->iRXPacketHeaderIndex++;
                        if((data == 0x3A))
                        {
                            str = (char *)strtok((char *)p->iRXPacketHeader,",");
                            p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iLinkID = (int8_t) atoi(str);
                            str = (char *)strtok(NULL,",");
                            p->iTotalRXDataSize = (int16_t)atoi(str);
                            if(p->iTotalRXDataSize > p->iMaxESPPacketSize)
                            {
                                p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iDataLength = p->iMaxESPPacketSize;
                            }
                            else
                            {
                                p->iRXNetworkPacket[p->iRXNetworkPacketIndex].iDataLength = p->iTotalRXDataSize;
                            }
                            p->iGetNetworkData = true;
                            p->iRXPacketHeaderIndex = 0;
                        }
                    }
                }
                else
#endif
                {
                    if((p->pCmdBuffer != 0) && (p->iCmdBufferIndex < ESPWROOM32_MAX_PACKET_WINDOW))
                    {
                        if(p->iRXIndex == 0)
                        {
                            p->pCmdBuffer[p->iCmdBufferIndex] = p->iRXBuffer[ESPWROOM32_AT_RX_SIZE - 1];
#if SEGGER_ENABLE_RTT && 0
                            if(data > 31)
                            {
                                DEBUG_RTT_Printf(0, "%c", (char)data);
                                //DEBUG_RTT_Printf(0, "%c%c%d%c", (char)data, (p->iRXIsData == true)?'1':'0', p->iCommand, (p->iGetNetworkData)?'1':'0');
                            }
                            else if(data== 0x0A)
                            {
                                DEBUG_RTT_Printf(0, "LF");
                            }
                            else if(data == 0x0D)
                            {
                                DEBUG_RTT_Printf(0, "CR");
                            } 
                            else
                            {
                                DEBUG_RTT_Printf(0, "%02X", (uint8_t)data);
                            }
#endif
                        }
                        else
                        {
                            p->pCmdBuffer[p->iCmdBufferIndex] = p->iRXBuffer[p->iRXIndex - 1];
#if SEGGER_ENABLE_RTT && 0
                            if(data > 31)
                            {
                                DEBUG_RTT_Printf(0, "%c", (char)data);
                                //DEBUG_RTT_Printf(0, "%c%c%d%c", (char)data, (p->iRXIsData == true)?'1':'0', p->iCommand, (p->iGetNetworkData)?'1':'0');
                            }
                            else if(data == 0x0A)
                            {
                                DEBUG_RTT_Printf(0, "LF");
                            }
                            else if(data == 0x0D)
                            {
                                DEBUG_RTT_Printf(0, "CR");
                            } 
                            else
                            {
                                DEBUG_RTT_Printf(0, "%02X", (uint8_t)data);
                            }
#endif
                        }

                        p->iCmdBufferIndex++;
                    }

                    if((data == 0x0A) && (p->iRXIndex > 6))
                    {
#if 0
                        if(p->pCmdBuffer != 0)
                        {
                            if((p->iSizeOfResponse + p->iRXIndex) < ESPWROOM32_MAX_PACKET_WINDOW)
                            {
                                DEBUG_RTT_printf(0, "\nRX1:");
                                DEBUG_RTT_Write(0, p->iRXBuffer, p->iRXIndex);
                                DEBUG_RTT_PutChar(0, '\n');
                                memcpy(p->pCmdBuffer + p->iSizeOfResponse, p->iRXBuffer, p->iRXIndex);
                            }
                        }
#endif
                        if(p->iPacketIndex < ESPWROOM32_MAX_AP_NUM_IN_SCAN)
                        {
                            p->iATInterfacePacket[p->iPacketIndex].iPacketLength = p->iRXIndex;
                            p->iATInterfacePacket[p->iPacketIndex].pPacketDataStart = p->pCmdBuffer + p->iSizeOfResponse;
                            p->iPacketIndex++;

                            //DEBUG_RTT_Printf(0, "\niSizeOfResponse=%d\n", p->iSizeOfResponse);
                            //DEBUG_RTT_Printf(0, "\niPacketIndex=%d\n", p->iPacketIndex);
                        }
                        p->iSizeOfResponse = (uint16_t)(p->iSizeOfResponse + p->iRXIndex);
                        p->iRXIndex = 0;
                    }
                }
            }
            else
            {
#if (ESPWROOM32_BUFFER_ON_MODULE == 0)
#if 0
                if(p->iRXIndex >= 5)
                {
                    if((p->iRXBuffer[p->iRXIndex-5]=='+') && (p->iRXBuffer[p->iRXIndex-4]=='I') &&
                            (p->iRXBuffer[p->iRXIndex-3]=='P') && (p->iRXBuffer[p->iRXIndex-2]=='D') && (p->iRXBuffer[p->iRXIndex-1]==','))
                    {
                        p->iRXIndex = 0;
                        p->iRXIsData = true;
                        p->iCommand = ESPWROOM32_AT_CMD_INDEX_IPD;
                    }
                }
#elif 1
                if(p->iRXIndex >= 5)
                {
                    if((p->iRXBuffer[p->iRXIndex-5]=='+') && (p->iRXBuffer[p->iRXIndex-4]=='I') &&
                            (p->iRXBuffer[p->iRXIndex-3]=='P') && (p->iRXBuffer[p->iRXIndex-2]=='D') && (p->iRXBuffer[p->iRXIndex-1]==','))
                    {
                        error = UEZSemaphoreGrab(p->iATCmdMutex, 1);
                        if(UEZ_ERROR_NONE != error)
                        {
                            DEBUG_RTT_Printf(0, "\nFailed ESPWROOM32_RXHandler::UEZSemaphoreGrab iATCmdMutex error=%d\n", error);
                        }
                        if(pdFALSE != xTimerIsTimerActive(p->iResponseTimer))
                        {
                            xTimerStop(p->iResponseTimer, 1);
                            xTimerReset(p->iResponseTimer, 1);
                        }
                        p->iRXIndex = 0;
                        p->iRXIsData = true;
                        p->iCommand = ESPWROOM32_AT_CMD_INDEX_IPD;
                    }
                }
#endif
#endif
                if(p->iRXIndex >= 8)
                {
                    if((p->iRXBuffer[p->iRXIndex-7]==',') && (p->iRXBuffer[p->iRXIndex-6]=='C') &&
                            (p->iRXBuffer[p->iRXIndex-5]=='L') && (p->iRXBuffer[p->iRXIndex-4]=='O') &&
                            (p->iRXBuffer[p->iRXIndex-3]=='S') && (p->iRXBuffer[p->iRXIndex-2]=='E') && (p->iRXBuffer[p->iRXIndex-1]=='D'))
                    {
                        p->iTCPSocketLinkID = (int8_t) atoi((char *) &p->iRXBuffer[p->iRXIndex-8]);
                        p->iSocketStatus[p->iTCPSocketLinkID] = ESPWROOM32_SOCKET_STATUS_CLOSED;
                        xTimerStart(p->iSocketStatusTimer, 0);
                        p->iRXIndex = 0;
                    }
                }

                if(p->iRXIndex >= 9)
                {
                    if((p->iRXBuffer[p->iRXIndex-8]==',') && (p->iRXBuffer[p->iRXIndex-7]=='C') &&
                            (p->iRXBuffer[p->iRXIndex-6]=='O') && (p->iRXBuffer[p->iRXIndex-5]=='N') &&
                            (p->iRXBuffer[p->iRXIndex-4]=='N') && (p->iRXBuffer[p->iRXIndex-3]=='E') &&
                            (p->iRXBuffer[p->iRXIndex-2]=='C') && (p->iRXBuffer[p->iRXIndex-1]=='T'))
                    {
                        p->iTCPSocketLinkID = (int8_t) atoi((char *) &p->iRXBuffer[p->iRXIndex-9]);
                        p->iSocketStatus[p->iTCPSocketLinkID] = ESPWROOM32_SOCKET_STATUS_ESTABLISHED;
                        xTimerStart(p->iSocketStatusTimer, 0);
                        p->iRXIndex = 0;
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Network ESPWROOM32_Network_Interface = { {
// Common device interface
    "Network:ESPWROOM32",
    0x0001,
    Network_ESPWROOM32_InitializeWorkspace,
    sizeof(T_Network_ESPWROOM32_Workspace), },

// Functions
    // added in uEZ v1.07
    Network_ESPWROOM32_Open,
    Network_ESPWROOM32_Close,
    Network_ESPWROOM32_Scan,
    Network_ESPWROOM32_Join,
    Network_ESPWROOM32_GetStatus,
    Network_ESPWROOM32_SocketCreate,
    Network_ESPWROOM32_SocketConnect,
    Network_ESPWROOM32_SocketBind,
    Network_ESPWROOM32_SocketListen,
    Network_ESPWROOM32_SocketAccept,
    Network_ESPWROOM32_SocketClose,
    Network_ESPWROOM32_SocketDelete,
    Network_ESPWROOM32_SocketRead,
    Network_ESPWROOM32_SocketWrite,
    Network_ESPWROOM32_AuxControl,
    // uEZ v1.09
    Network_ESPWROOM32_Leave,
    // uEZ v1.13
    Network_ESPWROOM32_ResolveAddress,

    // uEZ v2.04
    Network_ESPWROOM32_InfrastructureConfigure,
    Network_ESPWROOM32_InfrastructureBringUp,
    Network_ESPWROOM32_InfrastructureTakeDown,

    //uEZ v2.07
    Network_ESPWROOM32_GetConnectionInfo,
};

/*-------------------------------------------------------------------------*
 * End of File:  Network_ESPWROOM32.c
 *-------------------------------------------------------------------------*/
