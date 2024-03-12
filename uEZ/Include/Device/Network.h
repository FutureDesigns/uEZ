/*-------------------------------------------------------------------------*
 * File:  Network.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Network Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Network_H_
#define _DEVICE_Network_H_

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
/**
 *  @file   /Include/Device/Network.h
 *  @brief  uEZ Network Device Interface
 *
 *  The uEZ Network Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZDevice.h>
#include <Types/Network.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    /**
     * Functions uEZ v1.08
     */
    T_uezError (*Open)(void *aWorkspace);  // v2.04 modified
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Scan)(void *aWorkspace, TUInt32 aChannelNumber, // 0 = auto
        const char *aScanSSID,
        T_uezNetworkScanCallback aCallback,
        void *aCallbackWorkspace,
        TUInt32 aTimeout);
    T_uezError (*Join)(
        void *aWorkspace,
        const char *aJoinName,
        const char *aJoinPassword,
        TUInt32 aTimeout);
    T_uezError (*GetStatus)(void *aWorkspace, T_uezNetworkStatus *aStatus);
    T_uezError (*SocketCreate)(
        void *aWorkspace,
        T_uezNetworkSocketType aType,
        T_uezNetworkSocket *aSocket);
    T_uezError (*SocketConnect)(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort);
    T_uezError (*SocketBind)(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort);
    T_uezError
    (*SocketListen)(void *aWorkspace, T_uezNetworkSocket aSocket);
    T_uezError (*SocketAccept)(
        void *aWorkspace,
        T_uezNetworkSocket aListenSocket,
        T_uezNetworkSocket *aCreatedSocket,
        TUInt32 aTimeout);
    T_uezError (*SocketClose)(void *aWorkspace, T_uezNetworkSocket aSocket);
    T_uezError
    (*SocketDelete)(void *aWorkspace, T_uezNetworkSocket aSocket);
    T_uezError (*SocketRead)(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesRead,
        TUInt32 aTimeout);
    T_uezError (*SocketWrite)(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TBool aFlush,
        TUInt32 aTimeout);
    T_uezError (*AuxControl)(
        void *aWorkspace,
        TUInt32 aAuxCommand,
        void *aAuxData);

    /**
     * Functions uEZ v1.09
     */
    T_uezError (*Leave)(void *aWorkspace, TUInt32 aTimeout);

    /**
     * Functions uEZ v1.13
     */
    T_uezError (*ResolveAddress)(
        void *aWorkspace,
        const char *aName,
        T_uezNetworkAddr *aAddr);

    /**
     * Functions uEZ v2.04
     */
    T_uezError (*InfrastructureConfigure)(
        void *aWorkspace,
        T_uezNetworkSettings *aSettings);
    T_uezError (*InfrastructureBringUp)(void *aWorkspace);
    T_uezError (*InfrastructureTakeDown)(void *aWorkspace);
    /*
     * Functions uEZ v2.07
     */
    T_uezError (*GetConnectionInfo)(void *aWorkspace,
    		T_uezNetworkSocket aSocket,
    		T_uEZNetworkConnectionInfo *aConnection);
    /*
     * Functions uEZ v2.14
     */        
    T_uezError (*InfrastructureRestart)(void *aWorkspace);
} DEVICE_Network;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_Network_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network.h
 *-------------------------------------------------------------------------*/
