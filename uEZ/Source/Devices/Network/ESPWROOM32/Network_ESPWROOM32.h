/*-------------------------------------------------------------------------*
 * File:  Network_ESPWROOM32.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_ESPWROOM32_H_
#define NETWORK_ESPWROOM32_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
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
#include <uEZ.h>
#include <uEZRTOS.h>
#include <Device/Network.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define ESPWROOM32_RX_THREAD_STACK_SIZE     (1024)
#define ESPWROOM32_RX_THREAD_PRIORITY       (UEZ_PRIORITY_CRITICAL)
#define ESPWROOM32_AT_CMD_THREAD_STACK_SIZE (1024)
#define ESPWROOM32_AT_CMD_THREAD_PRIORITY   (UEZ_PRIORITY_VERY_HIGH)
#define ESPWROOM32_AT_CMD_MUTEX_PRIORITY    (UEZ_PRIORITY_CRITICAL)
#define ESPWROOM32_MAX_PACKET_WINDOW        (4096)
#define ESPWROOM32_MAX_PACKETS_IN_POOL      (8)
#define ESPWROOM32_RESPONSE_TIMEOUT         (1000)
#define ESPWROOM32_SOCKET_STATUS_DELAY      (50)
#define ESPWROOM32_MAX_AP_NUM_IN_SCAN       (10)
#define ESPWROOM32_AT_RX_SIZE               (64)
#define ESPWROOM32_SOCKET_MAX_CONNECTIONS   (5)
#define ESPWROOM32_SSID                     ("iot_2.4")
#define ESPWROOM32_PW                       ("fdi_cloud")

#define ESPWROOM32_BUFFER_ON_MODULE         (1)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct 
{
    const char          *iUARTDeviceName;
    T_uezGPIOPortPin    iResetPin;
} T_ESPWROOM32_Network_UARTSettings;

typedef enum E_ESPWROOM32_ATCommandIndex
{
    ESPWROOM32_AT_CMD_INDEX_NONE,
    ESPWROOM32_AT_CMD_INDEX_RST,
    ESPWROOM32_AT_CMD_INDEX_IPD,
    ESPWROOM32_AT_CMD_INDEX_CPI,
    ESPWROOM32_AT_CMD_INDEX_CWMODE,
    ESPWROOM32_AT_CMD_INDEX_CWJAP,
    ESPWROOM32_AT_CMD_INDEX_ATE,
    ESPWROOM32_AT_CMD_INDEX_CIPMUX,
    ESPWROOM32_AT_CMD_INDEX_CIPDINFO,
    ESPWROOM32_AT_CMD_INDEX_CIPSTA,
    ESPWROOM32_AT_CMD_INDEX_CWDHCP,
    ESPWROOM32_AT_CMD_INDEX_CWDHCPS,
    ESPWROOM32_AT_CMD_INDEX_CIPSTATUS,
    ESPWROOM32_AT_CMD_INDEX_CWLAP,
    ESPWROOM32_AT_CMD_INDEX_CIPMAC,
    ESPWROOM32_AT_CMD_INDEX_CWQAP,
    ESPWROOM32_AT_CMD_INDEX_CIPCLOSE,
    ESPWROOM32_AT_CMD_INDEX_CIPSTART,
    ESPWROOM32_AT_CMD_INDEX_CIPSERVER,
    ESPWROOM32_AT_CMD_INDEX_CIPSENDEX,
    ESPWROOM32_AT_CMD_INDEX_CIPDOMAIN,
    ESPWROOM32_AT_CMD_INDEX_CIPRECVMODE,
    ESPWROOM32_AT_CMD_INDEX_CIPRECVDATA,
    ESPWROOM32_AT_CMD_INDEX_GMR,
    ESPWROOM32_AT_CMD_INDEX_CMD,
} T_ESPWROOM32_ATCommandIndex;

typedef struct ST_networkPacket
{
    TInt8                          iLinkID;
    TUInt8                       * iPacketDataStart;
    struct ST_networkPacket      * iNextNetworkPacket;
    TUInt8                         iPrePacketIndex;
    TInt16                         iDataLength;
} T_networkPacket;

typedef enum E_ESPWROOM32_TXMode
{
    ESPWROOM32_TX_MODE_NORMAL = 0,  ///< Command Mode: be used for AT Command operation
    ESPWROOM32_TX_MODE_TT     = 1,  ///< Data Mode: only be used for Transparent Transmission
} T_ESPWROOM32_TXMode;

typedef struct ST_ESPWROOM32_Packet
{
    TUInt16     iPacketLength;
    TUInt8      *pPacketDataStart;
} T_ESPWROOM32_Packet;

typedef struct ST_ESPWROOM32_ATCmdArgs
{
    TUInt8 const    *pRemainingString;
    TUInt8 const    *context;
    TUInt32         bytes;
} T_ESPWROOM32_ATCmdArgs;

typedef struct ST_ESPWROOM32_ATCmd
{
    TUInt8      *command;
    void        (* callback)(T_ESPWROOM32_ATCmdArgs * pArgs);
    void const  *context;
} T_ESPWROOM32_ATCmd;

typedef struct ST_ESPWROOM32_ATCmdMenu
{
    TUInt32                     numCmds;
    T_ESPWROOM32_ATCmd const    *cmdList;
} T_ESPWROOM32_ATCmdMenu;

typedef enum E_ESPWROOM32_SocketStatus
{
    ESPWROOM32_SOCKET_STATUS_LISTEN = 0,
    ESPWROOM32_SOCKET_STATUS_ESTABLISHED = 1,
    ESPWROOM32_SOCKET_STATUS_CLOSED = 2,
} T_ESPWROOM32_SocketStatus;

typedef enum E_ESPWROOM32_Mode
{
    ESPWROOM32_MODE_SOFTAP  = 0,
    ESPWROOM32_MODE_STATION = 1,
    ESPWROOM32_MODE_MIXED   = 2,
} T_ESPWROOM32_Mode;

typedef struct ST_ESPWROOM32_StationProvisioning
{
    TUInt8  channel;
    TUInt8  ssid[33];
    TUInt8  bssid[33];
    TUInt16 rssi;
} T_ESPWROOM32_StationProvisioning;

typedef enum E_ESPWROOM32_SocketProtocolType
{
    ESPWROOM32_SOCKET_PROTOCOL_TYPE_RAW     = 0,
    ESPWROOM32_SOCKET_PROTOCOL_TYPE_STREAM  = 1,
    ESPWROOM32_SOCKET_PROTOCOL_TYPE_DGRAM   = 2,
    ESPWROOM32_SOCKET_PROTOCOL_TYPE_SSL     = 3,
} T_ESPWROOM32_SocketProtocolType;

typedef enum E_ESPWROOM32_SocketConnectionType
{
    ESPWROOM32_SOCKET_CONNECTION_TYPE_DISCONNECT    = 0,
    ESPWROOM32_SOCKET_CONNECTION_TYPE_SERVER        = 1,
    ESPWROOM32_SOCKET_CONNECTION_TYPE_CLIENT        = 2,
} T_ESPWROOM32_SocketConnectionType;

typedef enum E_ESPWROOM32_SocketErrorCode
{
    ESPWROOM32_SOCKET_ERROR_CODE_NONE           = 0,
    ESPWROOM32_SOCKET_ERROR_CODE_EINVAL         = 1,  ///< The specified option is invalid at the specified socket level or the socket has been shut down
    ESPWROOM32_SOCKET_ERROR_CODE_EDATATIMEO     = 2,  ///< The data can't be completely transmitted/received in the send timeout period
    ESPWROOM32_SOCKET_ERROR_CODE_EISCONN        = 3,  ///< The socket is already connected, and a specified option cannot be set while the socket is connected
    ESPWROOM32_SOCKET_ERROR_CODE_EISNOTCONN     = 4,  ///< The socket connection has timed out
    ESPWROOM32_SOCKET_ERROR_CODE_ENOPROTOOPT    = 5,  ///< The option is not supported by the protocol.
    ESPWROOM32_SOCKET_ERROR_CODE_EBADF          = 6,  ///< The socket argument is not a valid file descriptor.
} T_ESPWROOM32_SocketErrorCode;

typedef struct ST_ESPWROOM32_Socket
{
    TInt8                               linkID;
    T_ESPWROOM32_SocketStatus           *status;
    T_ESPWROOM32_SocketProtocolType     protocolType;
    T_ESPWROOM32_SocketConnectionType   connectionType;
    T_ESPWROOM32_SocketErrorCode        errorCode;
    T_uezNetworkAddr                    networkAddr;
    TUInt16                             port;
    TBool                               tcpKeepAlive;
} T_ESPWROOM32_Socket;

typedef enum E_ESPWROOM32_StationStatus
{
    ESPWROOM32_STATION_STATUS_AP_ESTABLISHED                = 1,
    ESPWROOM32_STATION_STATUS_IP_CONFIGURED                 = 2,
    ESPWROOM32_STATION_STATUS_TCP_UDP_SOCKET_CONNECTED      = 3,
    ESPWROOM32_STATION_STATUS_TCP_UDP_SOCKET_DISCONNECTED   = 4,
    ESPWROOM32_STATION_STATUS_DISCONNECTED                  = 5
} T_ESPWROOM32_StationStatus;
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError Network_ESPWROOM32_Create(
    const char *aName,
    const T_ESPWROOM32_Network_UARTSettings *aSettings);

extern const DEVICE_Network ESPWROOM32_Network_Interface;

#ifdef __cplusplus
}
#endif

#endif // NETWORK_ESPWROOM32_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network_ESPWROOM32.h
 *-------------------------------------------------------------------------*/
