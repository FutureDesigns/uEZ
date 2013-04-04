/*-------------------------------------------------------------------------*
 * File:  Network_GainSpan.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <uEZ.h>
#include <uEZDevice.h>  // This is needed for the RX compiler
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include <uEZDeviceTable.h>
#include "Network_GainSpan.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#if 1
#define dprintf printf
#else
#define dprintf(...)
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NETWORK_GAINSPAN_NUM_SOCKETS             NETWORK_GAINSPAN_MAX_NUM_SOCKETS
#define NETWORK_GAINSPAN_LOCAL_PORT_START        0x8000
#define NETWORK_GAINSPAN_LOCAL_PORT_END          0x9000
#define APP_MAX_RECEIVED_DATA                    16000

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef enum {
    SOCKET_STATE_FREE, SOCKET_STATE_CREATED, SOCKET_STATE_LISTENING
} T_GainSpanSocketState;

typedef TUInt16 T_GainSpanSocketFlags;
#define SOCKET_FLAG_CONNECTED           (1<<0)

typedef struct {

    T_GainSpanSocketState iState;
    T_uezNetworkSocketType iType;
    TUInt16 iPort;
    volatile T_GainSpanSocketFlags iFlags;
    TUInt16 iSourcePort;

    //struct netconn *iNetconn;
    TUInt8 iReceiveBuffer[APP_MAX_RECEIVED_DATA + 1];
    TUInt16 iReceiveLength;
    TUInt16 iReceiveRemaining;
    ATLIBGS_TCPConnection iConnection;

    uint8_t iCID;
} T_GainSpanSocket;

typedef struct {
    DEVICE_Network **iDevice;
    TUInt32 iNumOpen;
    TUInt32 iNumUp;
    T_uezSemaphore iSem;
    T_uezNetworkInfo iInfo;
    T_uezNetworkJoinStatus iJoinStatus;
    T_uezNetworkScanStatus iScanStatus;

    // Track sockets in the network
    T_GainSpanSocket iSockets[NETWORK_GAINSPAN_NUM_SOCKETS + 1]; // first one is not used
    // use range of NETWORK_GAINSPAN_LOCAL_PORT_START - NETWORK_GAINSPAN_LOCAL_PORT_END

    T_GainSpan_CmdLib_SPISettings iSPISettings;

    uint8_t iReceived[APP_MAX_RECEIVED_DATA + 1];
    unsigned int iReceivedCount;

    // Infrastructure settings
    T_uezNetworkSettings iInfrastructureSettings;
} T_Network_GainSpan_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_Network_GainSpan_Workspace *G_GainSpan_Workspace;

static T_uezError IConvertErrorCode(ATLIBGS_MSG_ID_E aGSError)
{
    switch (aGSError) {
        case ATLIBGS_MSG_ID_NONE: /* No error, everything OK. */
        case ATLIBGS_MSG_ID_OK:
        case ATLIBGS_MSG_ID_ESC_CMD_OK:
        case ATLIBGS_MSG_ID_TCP_SERVER_CONNECT:
            return UEZ_ERROR_NONE;
        case ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL:
            return UEZ_ERROR_OUT_OF_MEMORY;
        case ATLIBGS_MSG_ID_ERROR_IP_CONFIG_FAIL:
            return UEZ_ERROR_INTERNAL_ERROR;
        case ATLIBGS_MSG_ID_DISASSOCIATION_EVENT:
            return UEZ_ERROR_ABORTED;
        case ATLIBGS_MSG_ID_APP_RESET:
            return UEZ_ERROR_RESETED;
        case ATLIBGS_MSG_ID_DISCONNECT:
            return UEZ_ERROR_CLOSED;
        case ATLIBGS_MSG_ID_ESC_CMD_FAIL:
            return UEZ_ERROR_NOT_READY;
        case ATLIBGS_MSG_ID_INVALID_INPUT:
            return UEZ_ERROR_ILLEGAL_VALUE;
        case ATLIBGS_MSG_ID_ERROR:
            return UEZ_ERROR_HANDLE_INVALID;
        case ATLIBGS_MSG_ID_RESPONSE_TIMEOUT:
            return UEZ_ERROR_TIMEOUT;
        case ATLIBGS_MSG_ID_OUT_OF_STBY_ALARM:
        case ATLIBGS_MSG_ID_OUT_OF_STBY_TIMER:
        case ATLIBGS_MSG_ID_OUT_OF_DEEP_SLEEP:
            return UEZ_ERROR_NOT_READY;
        case ATLIBGS_MSG_ID_UNEXPECTED_WARM_BOOT:
            return UEZ_ERROR_INTERNAL_ERROR;
        case ATLIBGS_MSG_ID_WELCOME_MSG:
            return UEZ_ERROR_RESETED;
        case ATLIBGS_MSG_ID_STBY_CMD_ECHO:
            return UEZ_ERROR_BUSY;
        case ATLIBGS_MSG_ID_BULK_DATA_RX:
        case ATLIBGS_MSG_ID_DATA_RX:
        case ATLIBGS_MSG_ID_RAW_DATA_RX:
        case ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX:
            return UEZ_ERROR_NOT_EMPTY;
        case ATLIBGS_MSG_ID_MAX:
            return UEZ_ERROR_OUT_OF_SPACE;
        case ATLIBGS_MSG_ID_GENERAL_MESSAGE:
        default:
            return UEZ_ERROR_UNKNOWN;
    }
    return UEZ_ERROR_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a redpine networking subsystem workspace.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;

    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    p->iNumOpen = 0;
    p->iNumUp = 0;
    memset(p->iSockets, 0, sizeof(p->iSockets));

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_IDLE;
    p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_IDLE;
    p->iReceivedCount = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IStartup
 *---------------------------------------------------------------------------*
 * Description:
 *      Start up the Redpine module
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IStartup(
    T_Network_GainSpan_Workspace *p,
    T_uezNetworkSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    char ip[20];
    char mask[20];
    char gateway[20];
    T_uezNetworkMACAddress nullMAC;
    char mac[30];

    nullMAC.v4[0] = 0;
    nullMAC.v4[1] = 0;
    nullMAC.v4[2] = 0;
    nullMAC.v4[3] = 0;
    nullMAC.v4[4] = 0;
    nullMAC.v4[5] = 0;

    AtLibGs_GetMAC(mac); // for debug

    // Set the MAC address if its non-zero
    if (memcmp(&p->iInfrastructureSettings.iMACAddress, &nullMAC, 6) != 0) {
        AtLibGs_GetMAC2(mac); // for debug
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X\n",
            p->iInfrastructureSettings.iMACAddress.v4[0],
            p->iInfrastructureSettings.iMACAddress.v4[1],
            p->iInfrastructureSettings.iMACAddress.v4[2],
            p->iInfrastructureSettings.iMACAddress.v4[3],
            p->iInfrastructureSettings.iMACAddress.v4[4],
            p->iInfrastructureSettings.iMACAddress.v4[5]);
        AtLibGs_SetMAC(mac);
        AtLibGs_SetMAC2(mac);
    }

    // Only support infrastructure network types
    if ((aSettings->iNetworkType == UEZ_NETWORK_TYPE_INFRASTRUCTURE)
        || (aSettings->iNetworkType == UEZ_NETWORK_TYPE_ADHOC)
        || (aSettings->iNetworkType == UEZ_NETWORK_TYPE_LIMITED_AP)) {
        // Set the IP, Mask, and Gateway (when DHCP is not used)
        sprintf(ip, "%d.%d.%d.%d", aSettings->iIPAddress.v4[0],
            aSettings->iIPAddress.v4[1], aSettings->iIPAddress.v4[2],
            aSettings->iIPAddress.v4[3]);
        sprintf(mask, "%d.%d.%d.%d", aSettings->iSubnetMask.v4[0],
            aSettings->iSubnetMask.v4[1], aSettings->iSubnetMask.v4[2],
            aSettings->iSubnetMask.v4[3]);
        sprintf(gateway, "%d.%d.%d.%d", aSettings->iGatewayAddress.v4[0],
            aSettings->iGatewayAddress.v4[1], aSettings->iGatewayAddress.v4[2],
            aSettings->iGatewayAddress.v4[3]);
        AtLibGs_IPSet(ip, mask, gateway);
    } else {
        return UEZ_ERROR_INCORRECT_TYPE;
    }
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IShutdown
 *---------------------------------------------------------------------------*
 * Description:
 *      Shut down the Redpine module since no one is using it or even
 *      looking for incoming connections.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IShutdown(T_Network_GainSpan_Workspace *p)
{
    PARAM_NOT_USED(p);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open up a new network user
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_Open(void *aWorkspace)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iNumOpen++;
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close out a network that was previously opened.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_Close(void *aWorkspace)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iNumOpen == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        p->iNumOpen--;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_Scan
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a scan and setup a callback routine for receiving events.
 * Inputs:
 *      void *aWorkspace -- Workspace
 *      T_uezNetworkScanCallback *aCallback -- Routine to call when a scan is
 *          found or the scan is complete
 *      void *aCallbackWorkspace -- Workspace to use with the callback
 *      char *aScanSSID -- ID to scan under (null string if none)
 *      TUInt32 aTimeout -- Total time in ms to attempt scan before timing out.
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_Scan(
    void *aWorkspace,
    TUInt32 aChannelNumber,
    const char *aScanSSID,
    T_uezNetworkScanCallback aCallback,
    void *aCallbackWorkspace,
    TUInt32 aTimeout)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_uezNetworkInfo scanInfo;

    PARAM_NOT_USED(aCallback);PARAM_NOT_USED(aCallbackWorkspace);

    // Doing a scan is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    scanInfo.iRSSILevel = 0;
    scanInfo.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_OPEN;
    scanInfo.iChannel = 0;
    strcpy(scanInfo.iName, "GainSpan");
    p->iInfo = scanInfo;
    p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_COMPLETE;

    if (aCallback) {
        aCallback(aCallbackWorkspace, &scanInfo);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_Join
 *---------------------------------------------------------------------------*
 * Description:
 *      Join a network access point
 * Inputs:
 *      void *aWorkspace -- Workspace
 *      char *aJoinName -- Name of network to join
 *      char *aJoinPassword -- Password for access (or empty string for none)
 *      TUInt32 aTimeout -- Time out until abort join
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_Join(
    void *aWorkspace,
    const char *aJoinName,
    const char *aJoinPassword,
    TUInt32 aTimeout)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    ATLIBGS_MSG_ID_E r;
    ATLIBGS_STATIONMODE_E mode;

    // Doing a join is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    while (1) {
        AtLibGs_DisAssoc();
        AtLibGs_FlushIncomingMessage();

        if (p->iInfrastructureSettings.iNetworkType == UEZ_NETWORK_TYPE_ADHOC)
            mode = ATLIBGS_STATIONMODE_AD_HOC;
        else if (p->iInfrastructureSettings.iNetworkType
            == UEZ_NETWORK_TYPE_LIMITED_AP)
            mode = ATLIBGS_STATIONMODE_LIMITED_AP;
        else
            mode = ATLIBGS_STATIONMODE_INFRASTRUCTURE;
        r = AtLibGs_Mode(mode);
        if (r != ATLIBGS_MSG_ID_OK) {
            error = IConvertErrorCode(r);
            break;
        }
        AtLibGs_EnableRadio(1); /* radio always on */

        if (p->iInfrastructureSettings.iEnableDHCP) {
            r = AtLibGs_DHCPSet(1);
        } else {
            // DHCP is NOT enabled
            r = AtLibGs_DHCPSet(0);
        }
        if (r != ATLIBGS_MSG_ID_OK) {
            error = IConvertErrorCode(r);
            break;
        }

        if (p->iInfrastructureSettings.iNetworkType
            == UEZ_NETWORK_TYPE_LIMITED_AP) {
            AtLibGs_CalcNStorePSK((char *)aJoinName, (char *)aJoinPassword);
        }

        // Support one of several security modes
        if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_OPEN) {
            AtLibGs_SetSecurity(ATLIBGS_SMOPEN);
        } else if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_WPA) {
            AtLibGs_SetSecurity(ATLIBGS_SMWPAPSK);
        } else if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_WPA2) {
            AtLibGs_SetSecurity(ATLIBGS_SMWPA2PSK);
        } else if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_WEP) {
            AtLibGs_SetSecurity(ATLIBGS_SMWEP);
        } else if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_WPA_ENTERPRISE) {
            AtLibGs_SetSecurity(ATLIBGS_SMWPAE);
        } else if (p->iInfrastructureSettings.iSecurityMode
            == UEZ_NETWORK_SECURITY_MODE_WPA2_ENTERPRISE) {
            AtLibGs_SetSecurity(ATLIBGS_SMWPA2E);
        } else {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        // Set the password/passphrase/key
        if (aJoinPassword) {
            if (p->iInfrastructureSettings.iNetworkType
                != UEZ_NETWORK_TYPE_LIMITED_AP) {
                AtLibGs_SetPassPhrase(aJoinPassword);
            }
        } else {
            AtLibGs_SetPassPhrase("");
        }
        r = AtLibGs_Assoc(aJoinName, 0, 0);

        if (r == ATLIBGS_MSG_ID_OK) {
            // If DHCPServer is enabled, enable it now
            if (p->iInfrastructureSettings.iEnableDHCPServer)
                AtLibGs_EnableDHCPServer();
            else
                AtLibGs_DisableDHCPServer();
        }


        // You can only join the group "GainSpan"
        if (r != ATLIBGS_MSG_ID_OK) {
            error = UEZ_ERROR_NOT_FOUND;
            p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_FAIL;
        } else {
            // Passwords are ignored
            // Right name
            AtLibGs_SetNodeAssociationFlag();
            p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_SUCCESS;
            error = UEZ_ERROR_NONE;
        }
        break;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

void INetwork_GainSpan_Incoming(uint8_t aByte)
{
    T_Network_GainSpan_Workspace *p = G_GainSpan_Workspace;

//printf(":%02X,%d", aByte, p->iReceivedCount);
    // Put the incoming byte into the receive buffer
    if (p->iReceivedCount < APP_MAX_RECEIVED_DATA) {
//printf(";");
        p->iReceived[p->iReceivedCount++] = aByte;
    }
}


/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the GainSpan (if any).
 *      NOTE: This is a good place for the programming for the MAC address
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
void Network_GainSpan_Configure(
    void *aWorkspace,
    const T_GainSpan_Network_SPISettings *aSettings)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;

    p->iSPISettings.iDataReadyIO = aSettings->iDataReadyIO;
    p->iSPISettings.iProgramMode = aSettings->iProgramMode;
    p->iSPISettings.iRate = aSettings->iRate;
    p->iSPISettings.iSPIChipSelect = aSettings->iSPIChipSelect;
    p->iSPISettings.iSPIDeviceName = aSettings->iSPIDeviceName;
    p->iSPISettings.iSPIMode = aSettings->iSPIMode;
    p->iSPISettings.iSRSTn = aSettings->iSRSTn;
    p->iSPISettings.iIncomingDataFunc = INetwork_GainSpan_Incoming;
    G_GainSpan_Workspace = aWorkspace;

    GainSpan_CmdLib_ConfigureForSPI(&p->iSPISettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current status of the GainSpan network interface
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezNetworkStatus *aStatus -- General status
 *---------------------------------------------------------------------------*/
static T_uezError Network_GainSpan_GetStatus(
    void *aWorkspace,
    T_uezNetworkStatus *aStatus)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    ATLIBGS_NetworkStatus status;

    // Grab and pause the background task (we don't want
    // the background task changing things while we copy over the info)
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Get the network status
    AtLibGs_GetNetworkStatus(&status);

    memset(aStatus, 0, sizeof(*aStatus));
    aStatus->iInfo = p->iInfo;
    aStatus->iInfo.iRSSILevel = status.signal;

    aStatus->iIPAddr.v4[0] = status.addr.ipv4[0];
    aStatus->iIPAddr.v4[1] = status.addr.ipv4[1];
    aStatus->iIPAddr.v4[2] = status.addr.ipv4[2];
    aStatus->iIPAddr.v4[3] = status.addr.ipv4[3];

    aStatus->iGatewayAddress.v4[0] = status.gateway.ipv4[0];
    aStatus->iGatewayAddress.v4[1] = status.gateway.ipv4[1];
    aStatus->iGatewayAddress.v4[2] = status.gateway.ipv4[2];
    aStatus->iGatewayAddress.v4[3] = status.gateway.ipv4[3];

    aStatus->iSubnetMask.v4[0] = status.subnet.ipv4[0];
    aStatus->iSubnetMask.v4[1] = status.subnet.ipv4[1];
    aStatus->iSubnetMask.v4[2] = status.subnet.ipv4[2];
    aStatus->iSubnetMask.v4[3] = status.subnet.ipv4[3];

    aStatus->iJoinStatus = p->iJoinStatus;
    aStatus->iScanStatus = p->iScanStatus;

    // Done, release the background task and continue
    UEZSemaphoreRelease(p->iSem);

    return error;
}

static T_uezNetworkSocket ISocketCreate(
    T_Network_GainSpan_Workspace *p,
    T_uezNetworkSocketType aType,
    ATLIBGS_TCPConnection *aConnection)
{
    T_uezNetworkSocket i;
    T_uezNetworkSocket socket = 0; // none found yet
    for (i = 1; i <= NETWORK_GAINSPAN_NUM_SOCKETS; i++) {
        if (p->iSockets[i].iState == SOCKET_STATE_FREE) {
            // We found a slot
            // Can we get the socket from GainSpan?
            if (aConnection) {
                p->iSockets[i].iConnection = *aConnection;
                p->iSockets[i].iCID = aConnection->cid;
            } else {
                p->iSockets[i].iConnection.cid = 0;
                p->iSockets[i].iConnection.ip[0] = '\0';
                p->iSockets[i].iConnection.port = 0;
                p->iSockets[i].iConnection.server_cid = 0;
            }
            // Allocate this slot and report back this slot
            p->iSockets[i].iState = SOCKET_STATE_CREATED;
            p->iSockets[i].iType = aType;
            p->iSockets[i].iFlags = 0;
            socket = i;
            // Either we got a socket or not.  Either way, stop here.
            break;
        }
    }
    return socket;
}

static void ISocketFree(T_Network_GainSpan_Workspace *p, T_uezNetworkSocket aSocket)
{
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;

    if (aSocket == 0)
        return;

    p_socket->iState = SOCKET_STATE_FREE;
    p_socket->iType = UEZ_NETWORK_SOCKET_TYPE_TCP;
    p_socket->iFlags = 0;
    p_socket->iReceiveLength = 0;
    p_socket->iReceiveRemaining = 0;
    p_socket->iPort = 0;
    p_socket->iSourcePort = 0;
    p_socket->iCID = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_GainSpan_SocketCreate
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a socket for communicating.  At this point, it may just
 *      allocate the socket.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezNetworkStatus *aStatus -- General status
 *---------------------------------------------------------------------------*/
T_uezError Network_GainSpan_SocketCreate(
    void *aWorkspace,
    T_uezNetworkSocketType aType,
    T_uezNetworkSocket *aSocket)
{
    // Find a free socket slot
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;

    // Only support TCP sockets currently
    if ((aType != UEZ_NETWORK_SOCKET_TYPE_TCP) && (aType
        != UEZ_NETWORK_SOCKET_TYPE_UDP))
        return UEZ_ERROR_INVALID_PARAMETER;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    *aSocket = ISocketCreate(p, aType, 0);
    UEZSemaphoreRelease(p->iSem);

    if (!*aSocket)
        return UEZ_ERROR_OUT_OF_HANDLES;
    return UEZ_ERROR_NONE;
}

T_uezError Network_GainSpan_SocketBind(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;
    ATLIBGS_MSG_ID_E r;
    PARAM_NOT_USED(aAddr);

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
        r = AtLibGs_TCPServer_Start(aPort, &p_socket->iCID);
        error = IConvertErrorCode(r);
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketConnect(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;
    char ipAddr[20];
    ATLIBGS_MSG_ID_E r = ATLIBGS_MSG_ID_NONE;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
        //IP4_ADDR(&ip, aAddr->v4[0], aAddr->v4[1], aAddr->v4[2], aAddr->v4[3]);
        sprintf(ipAddr, "%d.%d.%d.%d", aAddr->v4[0], aAddr->v4[1], aAddr->v4[2],
            aAddr->v4[3]);

        // Connect over TCP
        if (p_socket->iType == UEZ_NETWORK_SOCKET_TYPE_TCP) {
            r = AtLibGs_TCPClientStart(ipAddr, aPort, &p_socket->iCID);
        } else {
            // TODO: Need UDP
            UEZFailureMsg("UDP Not supported");
        }

        error = IConvertErrorCode(r);
        if (error == UEZ_ERROR_NONE)
            p_socket->iFlags |= SOCKET_FLAG_CONNECTED;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketListen(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
//        if (error == UEZ_ERROR_NONE) {
            p_socket->iState = SOCKET_STATE_LISTENING;
//        }
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketAccept(
    void *aWorkspace,
    T_uezNetworkSocket aListenSocket,
    T_uezNetworkSocket *aCreatedSocket,
    TUInt32 aTimeout)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    ATLIBGS_TCPConnection connection;
    T_GainSpanSocket *p_socket = p->iSockets + aListenSocket;
    T_GainSpanSocket *p_newSocket;
    ATLIBGS_MSG_ID_E r = ATLIBGS_MSG_ID_NONE;

    // So far, we got nothing
    *aCreatedSocket = 0;

    // Only valid sockets
    if ((aListenSocket == 0) || (aListenSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    if (aCreatedSocket == 0)
        return UEZ_ERROR_INVALID_PARAMETER;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if (p_socket->iState != SOCKET_STATE_LISTENING) {
        error = UEZ_ERROR_NOT_READY;
    } else {
        // Go ahead and create a socket to receive
        // (of the same type as we are listening on)
        UEZSemaphoreRelease(p->iSem);
        if (p_socket->iType == UEZ_NETWORK_SOCKET_TYPE_TCP)
            r = AtLibGs_WaitForTCPConnection(&connection, aTimeout);
        else
            UEZFailureMsg("UDP Server Not supported yet!");
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
        if (r == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT) {
            *aCreatedSocket = ISocketCreate(p, p_socket->iType, &connection);
            if (*aCreatedSocket) {
                p_newSocket = p->iSockets + *aCreatedSocket;
                p_newSocket->iFlags |= SOCKET_FLAG_CONNECTED;
                error = UEZ_ERROR_NONE;
            } else {
                error = UEZ_ERROR_OUT_OF_HANDLES;
            }
        } else {
            error = IConvertErrorCode(r);
            // Free this previously created socket, we had an error
            ISocketFree(p, *aCreatedSocket);
            *aCreatedSocket = 0;
            // If no other reason, then state out of handles
            // (TODO: Is this needed?)
            if (error == UEZ_ERROR_NONE)
                error = UEZ_ERROR_OUT_OF_HANDLES;
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketClose(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;
    ATLIBGS_MSG_ID_E r;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        p_socket->iReceiveLength = 0;
        p_socket->iReceiveRemaining = 0;
        p_socket->iFlags = 0;
        r = AtLibGs_Close(p_socket->iCID);
        error = IConvertErrorCode(r);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketDelete(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) != 0) {
        error = UEZ_ERROR_MUST_CLOSE_FIRST;
    } else {
        ISocketFree(p, aSocket);
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketRead(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesRead,
    TUInt32 aTimeout)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;
    TUInt16 numCopy;
    ATLIBGS_MSG_ID_E r;
    ATLIBGS_TCPMessage msg;

    // No bytes read yet
    *aNumBytesRead = 0;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else if (!AtLibGs_IsNodeAssociated()) {
        error = UEZ_ERROR_NOT_ACTIVE;
    } else {
        // Read segments of the incoming data while we need data
        while (aNumBytes) {
            // Do we have any data waiting to be read?
            if (p_socket->iReceiveRemaining) {
                // Copy over the remaining bytes waiting in the buffer
                if (aNumBytes < p_socket->iReceiveRemaining)
                    numCopy = (TUInt16)aNumBytes;
                else
                    numCopy = p_socket->iReceiveRemaining;
printf("N=%d ", numCopy);
                memcpy(aData, p_socket->iReceiveBuffer + p_socket->iReceiveLength
                    - p_socket->iReceiveRemaining, numCopy);

                // Update counts and pointers
                *aNumBytesRead += numCopy;
                p_socket->iReceiveRemaining -= numCopy;
printf("R=%d ", p_socket->iReceiveRemaining);
                aData = (void *)(((TUInt8 *)aData) + numCopy);

                // Decrement the amount we are waiting on
                aNumBytes -= numCopy;
            }

            // If count goes to zero, then we need to release this pbuf
            // and go to the next.
            if ((p_socket->iReceiveRemaining == 0) && (aNumBytes)) {
                r = AtLibGs_WaitForTCPMessage(aTimeout);
                if (r == ATLIBGS_MSG_ID_DATA_RX) {
                    // Parse the data in the receive buffer
                    AtLibGs_ParseTCPData(p->iReceived, p->iReceivedCount, &msg);

                    // Copy over the data to receive
                    memcpy(p_socket->iReceiveBuffer, msg.message, msg.numBytes);
                    p_socket->iReceiveLength = msg.numBytes;
                    p_socket->iReceiveRemaining = msg.numBytes;
printf("+%d\n", p_socket->iReceiveRemaining);

                    printf("--RESET--\n");
                    // Reset the bytes coming in for the next packet
                    memset(p->iReceived, 0, sizeof(p->iReceived));
                    p->iReceivedCount = 0;
                } else if (r == ATLIBGS_MSG_ID_RESPONSE_TIMEOUT) {
                    // No data yet.  But stop here
                    error = UEZ_ERROR_TIMEOUT;
                    break;
                } else {
                    // Got some type of error.  Assume we closed.
                    p_socket->iFlags &= ~SOCKET_FLAG_CONNECTED;
                    error = IConvertErrorCode(r);
                    break;
                }
            }
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_SocketWrite(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TBool aFlush,
    TUInt32 aTimeout)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_GainSpanSocket *p_socket = p->iSockets + aSocket;
    TUInt16 numWrite;
    ATLIBGS_MSG_ID_E r;

    PARAM_NOT_USED(aTimeout);

    if ((aSocket == 0) || (aSocket > NETWORK_GAINSPAN_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else if (!AtLibGs_IsNodeAssociated()) {
        error = UEZ_ERROR_NOT_ACTIVE;
    } else {
        while (aNumBytes) {
            // Send data up to a 255 bytes at a time
            if (aNumBytes > 255)
                numWrite = 255;
            else
                numWrite = (TUInt16)aNumBytes;

            // Track the remaining number of bytes to write
            aNumBytes -= numWrite;

            // Write out this segment (noting if there is data past this one)
            if (p_socket->iType == UEZ_NETWORK_SOCKET_TYPE_TCP) {
                r = AtLibGs_SendTCPData(p_socket->iCID, aData, numWrite);
                error = IConvertErrorCode(r);
            } else if (p_socket->iType == UEZ_NETWORK_SOCKET_TYPE_UDP) {
                // TODO: UDP goes here
            }

            // Stop on any errors
            if (error != UEZ_ERROR_NONE)
                break;

            // Move up to the next section to write
            aData = (void *)(((char *)aData) + numWrite);
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_AuxControl(
    void *aWorkspace,
    TUInt32 aAuxCommand,
    void *aAuxData)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    switch (aAuxCommand) {
        default:
            error = UEZ_ERROR_NOT_SUPPORTED;
            break;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_Leave(void *aWorkspace, TUInt32 aTimeout)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aTimeout);

    AtLibGs_DisAssoc();

    return UEZ_ERROR_NONE;
}

T_uezError Network_GainSpan_ResolveAddress(
    void *aWorkspace,
    const char *aName,
    T_uezNetworkAddr *aAddr)
{
    char ipAddr[20];
    ATLIBGS_IP ip;
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (AtLibGs_DNSLookup(aName) == ATLIBGS_MSG_ID_OK) {
        if (AtLibGs_ParseDNSLookupResponse(ipAddr)) {
            // Got an IP address
            AtLibGs_ParseIPAddress(ipAddr, &ip);
            aAddr->v4[0] = ip.ipv4[0];
            aAddr->v4[1] = ip.ipv4[1];
            aAddr->v4[2] = ip.ipv4[2];
            aAddr->v4[3] = ip.ipv4[3];
            error = UEZ_ERROR_NONE;
        } else {
            error = UEZ_ERROR_UNKNOWN;
        }
    } else {
        error = UEZ_ERROR_UNKNOWN;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}



void Network_GainSpan_Create(
    const char *aName,
    const T_GainSpan_Network_SPISettings *aSettings)
{
    T_uezDeviceWorkspace *p_wired;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&GainSpan_Network_Interface,
            0,
            (T_uezDeviceWorkspace **)&p_wired);

    Network_GainSpan_Configure(p_wired, aSettings);
}

T_uezError Network_GainSpan_InfrastructureConfigure(
    void *aWorkspace,
    T_uezNetworkSettings *aSettings)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iInfrastructureSettings = *aSettings;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

T_uezError Network_GainSpan_InfrastructureBringUp(void *aWorkspace)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iNumUp == 0) {
        // First time brought up!  Let's initialize the library
        error = IStartup(p, &p->iInfrastructureSettings);
        if (!error)
            p->iNumUp++;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_GainSpan_InfrastructureTakeDown(void *aWorkspace)
{
    T_Network_GainSpan_Workspace *p = (T_Network_GainSpan_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iNumUp == 1) {
        // Last time taken down!  Let's close out and turn off the library
        error = IShutdown(p);
        if (error == UEZ_ERROR_NONE)
            p->iNumUp--;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}


/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Network GainSpan_Network_Interface = { {
// Common device interface
    "Network:GainSpan",
    0x0113,
    Network_GainSpan_InitializeWorkspace,
    sizeof(T_Network_GainSpan_Workspace), },

// Functions
    // added in uEZ v1.07
    Network_GainSpan_Open,
    Network_GainSpan_Close,
    Network_GainSpan_Scan,
    Network_GainSpan_Join,
    Network_GainSpan_GetStatus,
    Network_GainSpan_SocketCreate,
    Network_GainSpan_SocketConnect,
    Network_GainSpan_SocketBind,
    Network_GainSpan_SocketListen,
    Network_GainSpan_SocketAccept,
    Network_GainSpan_SocketClose,
    Network_GainSpan_SocketDelete,
    Network_GainSpan_SocketRead,
    Network_GainSpan_SocketWrite,
    Network_GainSpan_AuxControl,
    // uEZ v1.09
    Network_GainSpan_Leave,
    // uEZ v1.13
    Network_GainSpan_ResolveAddress,

    // uEZ v2.04
    Network_GainSpan_InfrastructureConfigure,
    Network_GainSpan_InfrastructureBringUp,
    Network_GainSpan_InfrastructureTakeDown,
};

/*-------------------------------------------------------------------------*
 * End of File:  Network_GainSpan.c
 *-------------------------------------------------------------------------*/
