/*-------------------------------------------------------------------------*
 * File:  Types/Network.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Network HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _Network_TYPES_H_
#define _Network_TYPES_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Types/Network.h
 *  @brief  uEZ Network Types
 *
 *  The uEZ Network Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define UEZ_NETWORK_INFO_NAME_LENGTH        32

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/** Several transmitter power modes to allow */ 
typedef enum {
    UEZ_NETWORK_TRANSMITTER_POWER_LOW,
    UEZ_NETWORK_TRANSMITTER_POWER_MEDIUM,
    UEZ_NETWORK_TRANSMITTER_POWER_HIGH
} T_uezNetworkPower;

/** Types of networks to configure between */ 
typedef enum {
    UEZ_NETWORK_TYPE_INFRASTRUCTURE=0,
    UEZ_NETWORK_TYPE_WIRED=0,
    UEZ_NETWORK_TYPE_IBSS=1,
    UEZ_NETWORK_TYPE_FORCE_UPGRADE_FIRMWARE=2,
    UEZ_NETWORK_TYPE_ADHOC=3,
    UEZ_NETWORK_TYPE_LIMITED_AP=4,
/** special type of operation */ 
} T_uezNetworkType;

/** Types of IBSS (peer to peer) network roles (create or join?) */ 
typedef enum {
    UEZ_NETWORK_IBSS_ROLE_CREATE,
    UEZ_NETWORK_IBSS_ROLE_JOIN,
    UEZ_NETWORK_IBSS_ROLE_NONE
} T_uezNetworkIBSSRole;

typedef enum {
    UEZ_NETWORK_SECURITY_MODE_OPEN,
    UEZ_NETWORK_SECURITY_MODE_WPA,
    UEZ_NETWORK_SECURITY_MODE_WPA2,
    UEZ_NETWORK_SECURITY_MODE_WEP,
    UEZ_NETWORK_SECURITY_MODE_WPA_ENTERPRISE,
    UEZ_NETWORK_SECURITY_MODE_WPA2_ENTERPRISE,
    UEZ_NETWORK_SECURITY_MODE_UNKNOWN
} T_uezNetworkSecurityMode;

/** Network types to allow */
typedef TUInt8 T_uezNetworkAddrIPV4[4];

typedef struct {
    char iName[UEZ_NETWORK_INFO_NAME_LENGTH + 1];
    TInt32 iRSSILevel;
    TUInt32 iChannel;
    T_uezNetworkSecurityMode iSecurityMode;
} T_uezNetworkInfo;

typedef TBool (*T_uezNetworkScanCallback)(
    void *aCallbackWorkspace,
    T_uezNetworkInfo *aNetworkInfo);

/** List of possible network addresses (currently on IPv4 compatible) */
typedef union {
    T_uezNetworkAddrIPV4 v4;
} T_uezNetworkAddr;

/** Network socket id (always non-zero, zero means bad socket) */
typedef TUInt16 T_uezNetworkSocket;

typedef enum {
    UEZ_NETWORK_SOCKET_TYPE_TCP,
	UEZ_NETWORK_SOCKET_TYPE_UDP,
/** TODO: UDP has not been tested! */
} T_uezNetworkSocketType;

typedef enum {
    UEZ_NETWORK_SCAN_STATUS_IDLE,
    UEZ_NETWORK_SCAN_STATUS_BUSY,
    UEZ_NETWORK_SCAN_STATUS_FAIL,
    UEZ_NETWORK_SCAN_STATUS_COMPLETE,
} T_uezNetworkScanStatus;

typedef enum {
    UEZ_NETWORK_JOIN_STATUS_IDLE,
    UEZ_NETWORK_JOIN_STATUS_BUSY,
    UEZ_NETWORK_JOIN_STATUS_FAIL,
    UEZ_NETWORK_JOIN_STATUS_SUCCESS,
    UEZ_NETWORK_JOIN_STATUS_DISCONNECTED,
} T_uezNetworkJoinStatus;

typedef struct {
    T_uezNetworkInfo iInfo;
    T_uezNetworkAddr iIPAddr;
    T_uezNetworkAddr iSubnetMask;
    T_uezNetworkAddr iGatewayAddress;
    T_uezNetworkJoinStatus iJoinStatus;
    T_uezNetworkScanStatus iScanStatus;
} T_uezNetworkStatus;

typedef union {
    TUInt8 v4[6];
} T_uezNetworkMACAddress;

typedef struct {
    /** What type of network is this? */
    T_uezNetworkType iNetworkType;

    /* -------------- General Network configuration ---------------- */
    /** MAC address (if not hardware defined) */
    T_uezNetworkMACAddress iMACAddress;

    /** Default IP address (if iEnableDHCP == EFalse) */
    T_uezNetworkAddr iIPAddress;
    T_uezNetworkAddr iSubnetMask;
    T_uezNetworkAddr iGatewayAddress;

    /** ------------- Wireless network specific settings --------------
     * Device specific scan channel (0 usually is Auto) */
    TUInt32 iScanChannel;

    /** Device specific transmittor rate (usually in kHz, 0 = Auto) */
    TUInt32 iTXRate;

    /** Transmit power (usually UEZ_NETWORK_TRANSMITTER_POWER_HIGH) */
    T_uezNetworkPower iTXPower;

    /** Flag to enable DHCP (ETrue = enabled) */
    TBool iEnableDHCP;

    /** Security mode */
    T_uezNetworkSecurityMode iSecurityMode;

    /** SSID to scan for specifically or if hidden (when auto connecting)
     * Use "" for none specific. */
    char iSSID[32];

    /** If this network driver allows it, try to enable as a DHCP Server too */
    TBool iEnableDHCPServer;

    /** ------------- Network Type: IBSS (Peer to peer) ----------------
     *  If network type is UEZ_NETWORK_TYPE_IBSS,
     *  device specific IBSS/peer-to-peer channel */
    TUInt32 iIBSSChannel;

    /** If network type is UEZ_NETWORK_TYPE_IBSS (Peer to Peer),
     *  declare if this network is creating or joining. */
    T_uezNetworkIBSSRole iIBSSRole;

    /* ------------- Wired network specific settings -------------- */

} T_uezNetworkSettings;

typedef struct{
	T_uezNetworkAddrIPV4 iIPConnectedAddr;
	TUInt16 iConnectedPort;
}T_uEZNetworkConnectionInfo;

#ifdef __cplusplus
}
#endif

#endif // _Network_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Network.h
 *-------------------------------------------------------------------------*/
