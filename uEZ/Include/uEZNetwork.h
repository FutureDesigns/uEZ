/*-------------------------------------------------------------------------*
* File:  uEZNetwork.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level Network drivers.
*-------------------------------------------------------------------------*/

/**
 *  @file   uEZNetwork.h
 *  @brief   uEZ Network Interface
 *
 *  The uEZ interface which maps to low level Network drivers.
 */
#ifndef UEZNETWORK_H_
#define UEZNETWORK_H_

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
#include <uEZ.h>
#include <Types/Network.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Open up access to the Network (and turn on).
 *
 *  @param [in]   *aName            Pointer to name of Network (usually
 *                                  "WiredNetwork0" or "WirelessNetwork0")
 *  @param [out]  *aNetworkDevice   Pointer to device handle to be returned
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkOpen(
        const char * const aName,
        T_uezDevice *aNetworkDevice);

/**
 *  End access to the Network.  May turn off Network if no one
 *  else is using the Network.
 *
 *  @param [in]  aNetworkDevice    Device handle of Network to close
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkClose(T_uezDevice aNetworkDevice);

/**
 *  Configure a network's infrastructure and default connection in one bulk step.
 *  Usually used once by the network configuration for the application.  Can
 *  be used to set the mode of operation (e.g. secured wireless ad-hoc vs.
 *  wireless connection to existing SSID).  UEZNetworkConnect is then
 *  used to connect a SSID (or "lwIP" if wired).
 *
 *  @param [out]  *aNetworkDevice   Pointer to device handle to be returned
 *  @param [out]  *aSettings        Pointer to settings to be used
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkConfigureInfrastructure(
    T_uezDevice aNetworkDevice,
    T_uezNetworkSettings *aSettings);

/**
 *  Scan the network for possible access points to join.
 *
 *  @param [in]   aNetworkDevice        Device handle of Network to close
 *  @param [in]   aChannelNumber        Channel number to scan (0 = auto usually)
 *  @param [in]   *aScanName            Name of access point (if hidden), or null/empty.
 *  @param [in]   *aCallback            Pointer to function to be called
 *                                    per network access point found.
 *  @param [in]   *aCallbackWorkspace  Pointer to data for callback
 *  @param [in]   aTimeout              Time in ms until scan is aborted.
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkScan(
        T_uezDevice aNetworkDevice,
        TUInt32 aChannelNumber,
        const char *aScanName,
        T_uezNetworkScanCallback aCallback,
        void *aCallbackWorkspace,
        TUInt32 aTimeout);

/**
 *  Join a network.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network to join
 *  @param [in]   *aJoinName        Name of network service to join
 *  @param [in]   *aJoinPassword    Password to pass to network service
 *  @param [in]   aTimeout          Timeout to join
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkJoin(
        T_uezDevice aNetworkDevice,
        const char *aJoinName,
        const char *aJoinPassword,
        TUInt32 aTimeout);

/**
 *  Leave a network.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network to leave
 *  @param [in]   aTimeout          Time in millliseconds to leave properly
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkLeave(T_uezDevice aNetworkDevice, TUInt32 aTimeout);

/**
 *  Get the status of the current network device.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network to get status
 *  @param [in]   *aStatus          Status of network
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkGetStatus(
        T_uezDevice aNetworkDevice,
        T_uezNetworkStatus *aStatus);

/**
 *  Create a socket of the given type.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network to create socket
 *  @param [in]   aType            Type of socket, usually TCP/UDP
 *  @param [in]   *aSocket          Socket created, or 0 returned
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketCreate(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocketType aType,
        T_uezNetworkSocket *aSocket);

/**
 *  Bind the socket to the given address and port.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network
 *  @param [in]   aSocket          Socket handle
 *  @param [in]   *aAddr            Address to bind to, or 0 for current address
 *  @param [in]   aPort            Local port address to bind to
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketBind(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort);

/**
 *  Listen for incoming connections on this socket
 *
 *  @param [in]   aNetworkDevice    Device handle of Network
 *  @param [in]   aSocket          Socket handle
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketListen(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket);

/**
 *  Accept the next incoming connection on this socket.
 *
 *  @param [in]   aNetworkDevice    Device handle of Network
 *  @param [in]   aListenSocket    Socket handle
 *  @param [in]   *aFoundSocket    Returned incoming socket handle (open)
 *  @param [in]   aTimeout          How long to wait for next accepted socket
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketAccept(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aListenSocket,
        T_uezNetworkSocket *aFoundSocket,
        TUInt32 aTimeout);

/**
 *  Close an opened socket
 *
 *  @param [in]   aNetworkDevice    Device handle of Network
 *  @param [in]   aSocket          Socket handle
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketClose(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket);

/**
 *  Delete an inactive/closed socket
 *
 *  @param [in]   aNetworkDevice    Device handle of Network to close
 *  @param [in]   aSocket          Socket handle
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketDelete(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket);

/**
 *  Read data coming in from a socket, waiting up to the timeout if
 *  necessary.
 *
 *  @param [in]     aNetworkDevice    Network device driver
 *  @param [in]     aSocket            Socket handle
 *  @param [out]  *aData            Place to store the data
 *  @param [in]    aNumBytes          Number of bytes requested
 *  @param [out]  *aNumBytesRead    Number of bytes returned
 *  @param [in]    aTimeout          Timeout period
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketRead(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesRead,
        TUInt32 aTimeout);

/**
 *  Write any outgoing data
 *
 *  @param [in]     aNetworkDevice    Network device driver
 *  @param [in]     aSocket            Socket handle
 *  @param [in]    *aData            Pointer to data to send
 *  @param [in]    aNumBytes          Number of bytes to write
 *  @param [in]    aFlush            ETrue if flush output
 *  @param [in]    aTimeout          Timeout period
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketWrite(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TBool aFlush,
        TUInt32 aTimeout);

/**
 *  Connect an outgoing socket to another server or system.
 *
 *  @param [in]     aNetworkDevice    Network device driver
 *  @param [in]     aSocket            Socket handle
 *  @param [in]    *aAddr            Address of computer to connect to
 *  @param [in]    aPort              Port number on other computer
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkSocketConnect(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort);

/**
 *  Auxiliary command to send the network device driver a non-standard
 *  command.  Exact implementation depends on the specific device
 *  driver being used.
 *
 *  @param [in]     aNetworkDevice    Network device driver
 *  @param [in]     aAuxCommand        Command number to execute
 *  @param [in]    *aAuxData          Additional data for command or 0 for none.
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkAuxCommand(
        T_uezDevice aNetworkDevice,
        TUInt32 aAuxCommand,
        void *aAuxData);

/**
 *  Convert an IPV4 numeric string into a network address structure.
 *
 *  @param [in]     *aString      String containing address
 *  @param [in]     *aAddr        Pointer to address structure to fill
 *
 *  @return    T_uezError
 */
T_uezError UEZNetworkIPV4StringToAddr(
        const char *aString,
        T_uezNetworkAddr *aAddr);

/**
 *  DEPRECATED! DO NOT USE THIS FUNCTION!  Instead, use UEZNetworkOpen,
 *  UEZNetworkConfigure, UEZNetworkScan, and UEZNetworkJoin.
 *  Connect the given network to its hardware medium and join the
 *  named service.
 *
 *  @param [in]      aNetwork      Returned handle to this connected network
 *  @param [in]     *aJoinName     Name of subnetwork to join, or 0 to not join,
 *                                  or network type
 *                                  (e.g., SSID or "lwIP" for wired networking
 *                                  using lwIP drivers.
 *  @param [in]     *aIPAddr       IP address to assign or 0 for none
 *  @param [in]     *aIPMask       IP mask to assign or 0 for none
 *  @param [in]     *aIPGateway    IP address of gateway to assign or 0 for none
 *  @param [out]    *aEnableDHCP   If ETrue enable DCHP
 *  @param [out]    *aStatus       Resulting connection status (if no error)
 *
 *  @return     T_uezError
 */
T_uezError UEZNetworkConnect(
        const char *aNetworkName,
        const char *aJoinName,
        const T_uezNetworkAddr *aIPAddr,
        const T_uezNetworkAddr *aIPMask,
        const T_uezNetworkAddr *aIPGateway,
        TBool aEnableDHCP,
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus);

T_uezError UEZNetworkInfrastructureConfigure(
    T_uezDevice aNetworkDevice,
    T_uezNetworkSettings *aSettings);
T_uezError UEZNetworkInfrastructureBringUp(T_uezDevice aNetworkDevice);
T_uezError UEZNetworkInfrastructureTakeDown(T_uezDevice aNetworkDevice);

T_uezError UEZNetworkGetConnectionInfo(T_uezDevice aNetworkDevice,
		T_uezNetworkSocket aSocket,
		T_uEZNetworkConnectionInfo *aConnection);

#ifdef __cplusplus
}
#endif

#endif // UEZNETWORK_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZNetwork.h
 *-------------------------------------------------------------------------*/
