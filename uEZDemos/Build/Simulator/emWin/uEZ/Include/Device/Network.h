/*-------------------------------------------------------------------------*
 * File:  Network.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Network Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Network_H_
#define _DEVICE_Network_H_

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
        T_uezError (*Open)(void *aWorkspace, T_uezNetworkSettings *aSettings);
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
        T_uezError (*Leave)(
                void *aWorkspace,
                TUInt32 aTimeout);

       /**
        * Functions uEZ v1.13
        */
        T_uezError (*ResolveAddress)(
            void *aWorkspace,
            const char *aName,
            T_uezNetworkAddr *aAddr);
} DEVICE_Network;

#endif // _DEVICE_Network_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network.h
 *-------------------------------------------------------------------------*/
