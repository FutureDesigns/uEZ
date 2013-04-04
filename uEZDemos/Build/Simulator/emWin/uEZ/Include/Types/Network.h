/*-------------------------------------------------------------------------*
 * File:  Types/Network.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Network HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _Network_TYPES_H_
#define _Network_TYPES_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
    UEZ_NETWORK_TYPE_INFRASTRUCTURE,
    UEZ_NETWORK_TYPE_IBSS,
    UEZ_NETWORK_TYPE_FORCE_UPGRADE_FIRMWARE,
    UEZ_NETWORK_TYPE_ADHOC,
    UEZ_NETWORK_TYPE_LIMITED_AP,
/** special type of operation */ 
} T_uezNetworkType;

/** Types of IBSS (peer to peer) network roles (create or join?) */ 
typedef enum {
    UEZ_NETWORK_IBSS_ROLE_CREATE, UEZ_NETWORK_IBSS_ROLE_JOIN,
} T_uezNetworkIBSSRole;

typedef enum {
    UEZ_NETWORK_SECURITY_MODE_OPEN,
    UEZ_NETWORK_SECURITY_MODE_WPA,
    UEZ_NETWORK_SECURITY_MODE_WPA2,
    UEZ_NETWORK_SECURITY_MODE_WEP,
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

typedef struct {
    /** What type of network is this? */
    T_uezNetworkType iNetworkType;

    /** ------------- Wireless network specific settings --------------
     * Device specific scan channel (0 usually is Auto) */
    TUInt32 iScanChannel;

    /** Automatically connect to open/unsecure networks?  ETrue = yes */
    TBool iAutoConnectToOpen;

    /** Device specific transmittor rate (usually in kHz, 0 = Auto) */
    TUInt32 iTXRate;

    /** Transmit power (usually UEZ_NETWORK_TRANSMITTER_POWER_HIGH) */
    T_uezNetworkPower iTXPower;

    /** Flag to enable DHCP (ETrue = enabled) */
    TBool iEnableDHCP;

    /** Default IP address (if iEnableDHCP == EFalse) */
    T_uezNetworkAddr iIPAddress;
    T_uezNetworkAddr iSubnetMask;
    T_uezNetworkAddr iGatewayAddress;

    /** SSID to scan for specifically or if hidden.
     * Use "" for none specific. */
    char iScanSSID[32];

    /** Automatic joining SSID and PSK (if iAutoConnectToOpen==EFalse) */
    char iJoinSSID[32];
    char iJoinPSK[32];

    /** ------------- Network Type: IBSS (Peer to peer) ----------------
     *  If network type is UEZ_NETWORK_TYPE_IBSS,
     *  device specific IBSS/peer-to-peer channel */
    TUInt32 iIBSSChannel;

    /** If network type is UEZ_NETWORK_TYPE_IBSS (Peer to Peer),
     *  declare if this network is creating or joining. */
    T_uezNetworkIBSSRole iIBSSRole;
} T_uezNetworkSettings;

#endif // _Network_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Network.h
 *-------------------------------------------------------------------------*/
