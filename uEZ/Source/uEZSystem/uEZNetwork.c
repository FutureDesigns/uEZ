/*-------------------------------------------------------------------------*
 * File:  uEZNetwork.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
 *-------------------------------------------------------------------------*/

 /*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZNetwork
 *  @{
 *  @brief     uEZ Network Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ Network interface.
 *
 *  @par Example code:
 *  Example task to send/receive data over the network
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 * 
 *  TUInt32 networkTest(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice Network;
 *      T_uezNetworkSettings aSettings;
 *      T_uezNetworkSocket aSocket;
 *      T_uezNetworkAddr IPaddr = {192,168,1,100};
 *      T_uezNetworkAddr IPMask = {255,255,255,0};
 *      T_uezNetworkAddr IPGateway = {192,168,1,1};
 *      T_uezNetworkStatus aStatus;
 *      if (UEZNetworkOpen("WiredNetwork0", &Network, &aSettings) == UEZ_ERROR_NONE) {
 *          // the device opened properly
 *
 *          UEZNetworkConnect("WiredNetwork0", "lwip", &IPaddr, &IPMask, IPGateway,
 * 		    								 EFalse, &Network, &aStatus);
 *          UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *          UEZNetworkSocketBind(Network,  aSocket, 0, 2000);
 *          UEZNetworkSocketListen(Network, aSocket);
 *          TUInt8 data[1500];
 *          UEZNetworkSocketRead(Network, aSocket, (void *)&data, 1500, aNumBytesRead,5000);
 *          UEZNetworkSocketWrite(Network, aSocket, (void *)&data, 1500, ETrue, 5000);
 *
 *          if (UEZNetworkClose(Network) != UEZ_ERROR_NONE) {
 *              // error closing the device
 *          }
 *      } else {
 *          // an error occurred opening the device
 *      }
 *      return 0;
 *  }
 * @endcode
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZNetwork.h>
#include <Device/Network.h>
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Network display (and turn on).
 *
 *  @param [in]    *aName 			Pointer to name of Network display (usually
 *                                  "Network")
 *  @param [in]    *aNetworkDevice	Pointer to device handle to be returned
 *
 *  @return        T_uezError       If the device is opened, returns
 *                                  UEZ_ERROR_NONE.  If the device cannot be
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  T_uezNetworkSettings aSettings;
 *  UEZNetworkOpen("WiredNetwork0", &Network, &aSettings);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkOpen(
        const char * const aName,
        T_uezDevice *aNetworkDevice)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableFind(aName, aNetworkDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Network display.  May turn off Network if no one
 *  else is using the Network.
 *
 *  @param [in]    aNetworkDevice	Device handle of Network to close
 *
 *  @return        T_uezError		If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.  If the device
 *                                  handle is bad, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  T_uezNetworkSettings aSettings;
 *  UEZNetworkOpen("WiredNetwork0", &Network, &aSettings);
 *  UEZNetworkClose(Network);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkClose(T_uezDevice aNetworkDevice)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkConfigureInfrastructure
 *---------------------------------------------------------------------------*/
/**
 *  Configure a network's infrastructure and default settings in one bulk step.
 *  Usually used once by the network configuration for the application.  Can
 *  be used to set the mode of operation (e.g. secured wireless ad-hoc vs.
 *  wireless connection to existing SSID).  UEZNetworkConnect is then
 *  used to connect a SSID (or "lwIP" if wired).
 *
 *  @param [in]    aNetworkDevice   Device handle of Network to configure
 *  @param [in]    *aSettings       Settings to configure network to use for
 *                                  defaults or auto connect.
 *
 *  @return        T_uezError       If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.  If the device
 *                                  handle is bad, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkConfigureInfrastructure(
    T_uezDevice aNetworkDevice,
    T_uezNetworkSettings *aSettings)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->InfrastructureConfigure((void *)p, aSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkScan
 *---------------------------------------------------------------------------*/
/**
 *  Scan the network for possible access points to join.
 *
 *  @param [in]    aNetworkDevice 		Device handle of Network to close
 *
 *  @param [in]    aChannelNumber		Channel number to scan (0 = auto usually)
 *
 *  @param [in]    *aScanSSID 			Name of access point (if hidden),
 * 										or null/empty.
 *  @param [in]    *aCallback 			Pointer to function to be called
 *                  					per network access point found.
 *  @param [in]    *aCallbackWorkspace 	Pointer to data for callback
 *
 *  @param [in]    aTimeout 			Time in ms until scan is aborted.
 *
 *  @return        T_uezError       	If the device is successfully closed,
 *                                  	returns UEZ_ERROR_NONE.  If the device
 *                                  	handle is bad, returns
 *                                  	UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkScanCallback aCallback;
 *  UEZNetworkScan(Network,0,"", aCallback, (void *) aCallbackWorkspace,10000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkScan(
        T_uezDevice aNetworkDevice,
        TUInt32 aChannelNumber,
        const char *aScanSSID,
        T_uezNetworkScanCallback aCallback,
        void *aCallbackWorkspace,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Scan((void *)p, aChannelNumber, aScanSSID, aCallback,
            aCallbackWorkspace, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkJoin
 *---------------------------------------------------------------------------*/
/**
 *  Join a network
 *
 *  @param [in]    aNetworkDevice 		Network device driver
 *
 *  @param [in]    *aJoinName 			Name of network service to join
 *
 *  @param [in]    *aJoinPassword 		Password to pass to network service
 *
 *  @param [in]    aTimeout 			Timeout to join
 *
 *  @return        T_uezError 			Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  UEZNetworkJoin(Network, "name", "password", 5000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkJoin(
        T_uezDevice aNetworkDevice,
        const char *aJoinName,
        const char *aJoinPassword,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Join((void *)p, aJoinName, aJoinPassword, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkJoin
 *---------------------------------------------------------------------------*/
/**
 *  Join a network
 *
 *  @param [in]    aNetworkDevice       Network device driver
 *
 *  @param [in]    aTimeout             How long to try to leave
 *
 *  @return        T_uezError           Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  extern T_uezDevice Network; // previously joined network
 *
 *  UEZNetworkLeave(Network, 5000);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkLeave(T_uezDevice aNetworkDevice, TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Leave((void *)p, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkGetStatus
 *---------------------------------------------------------------------------*/
/**
 *  Get the status of the current network device.
 *
 *  @param [in]    aNetworkDevice	Network device driver
 *
 *  @param [in]    *aStatus			Status of network
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkStatus aStatus;
 *  UEZNetworkGetStatus(Network, &aStatus);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkGetStatus(
        T_uezDevice aNetworkDevice,
        T_uezNetworkStatus *aStatus)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice,
            (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetStatus((void *)p, aStatus);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketCreate
 *---------------------------------------------------------------------------*/
/**
 *  Create a socket of the given type.
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aType 			Type of socket, usually TCP/UDP
 *
 *  @param [in]    *aSocket			Socket created, or 0 returned
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketBind(Network,  aSocket, 0, 2000);
 *  UEZNetworkSocketListen(Network, aSocket);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketCreate(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocketType aType,
        T_uezNetworkSocket *aSocket)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketCreate((void *)p, aType, aSocket);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketBind
 *---------------------------------------------------------------------------*/
/**
 *  Bind the socket to the given address and port.
 *
 *  @param [in]    aNetworkDevice  Network device driver
 *
 *  @param [in]    aSocket 		   Socket handle
 *
 *  @param [in]    *aAddr 		   Address to bind to, or 0 for current address
 *
 *  @param [in]    aPort 		   Local port address to bind to
 *
 *  @return        T_uezError 	   Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketBind(Network,  aSocket, 0, 2000);
 *  UEZNetworkSocketListen(Network, aSocket);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketBind(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketBind((void *)p, aSocket, aAddr, aPort);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketListen
 *---------------------------------------------------------------------------*/
/**
 *  Listen for incoming connections on this socket
 *
 *  @param [in]    aNetworkDevice 		Network device driver
 *
 *  @param [in]    aSocket 				Socket handle
 *
 *  @return        T_uezError 			Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketBind(Network,  aSocket, 0, 2000);
 *  UEZNetworkSocketListen(Network, aSocket);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketListen(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketListen((void *)p, aSocket);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketAccept
 *---------------------------------------------------------------------------*/
/**
 *  Accept the next incoming connection on this socket.
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aListenSocket 	Socket handle
 *
 *  @param [in]    *aFoundSocket 	Returned incoming socket handle (open)
 *
 *  @param [in]    aTimeout 		How long to wait for next accepted socket
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aListenSocket;
 *  T_uezNetworkSocket aFoundSocket;
 *  UEZNetworkSocketAccept(Network, &aListenSocket, &aFoundSocket, 5000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketAccept(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aListenSocket,
        T_uezNetworkSocket *aFoundSocket,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketAccept((void *)p, aListenSocket, aFoundSocket, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketClose
 *---------------------------------------------------------------------------*/
/**
 *  Close an opened socket
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aSocket 			Socket handle
 *
 *  @return        T_uezError		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketClose(Network, aSocket);
 *  UEZNetworkSocketDelete(Network, aSocket);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketClose(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketClose((void *)p, aSocket);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketDelete
 *---------------------------------------------------------------------------*/
/**
 *  Delete an inactive/closed socket
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aSocket 			Socket handle
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketClose(Network, aSocket);
 *  UEZNetworkSocketDelete(Network, aSocket);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketDelete(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketDelete((void *)p, aSocket);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketRead
 *---------------------------------------------------------------------------*/
/**
 *  Read data coming in from a socket, waiting up to the timeout if
 *  necessary.
 *
 *  @param [in]    aNetworkDevice 		Network device driver
 *
 *  @param [in]    aSocket 				Socket handle
 *
 *  @param [in]    *aData 				Place to store the data
 *
 *  @param [in]    aNumBytes 			Number of bytes requested
 *
 *  @param [in]    *aNumBytesRead 		Number of bytes returned
 *
 *  @param [in]    aTimeout 			Timeout period
 *
 *  @return        T_uezError 			Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  TUInt8 data[1500];
 *  T_uezNetworkAddr addr;
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  TUInt32 aNumBytesRead;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketRead(Network, aSocket, (void *)&data, 1500, aNumBytesRead,5000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketRead(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesRead,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketRead((void *)p, aSocket, aData, aNumBytes,
            aNumBytesRead, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write any outgoing data
 *
 *  @param [in]    aNetworkDevice	Network device driver
 *
 *  @param [in]    aSocket 			Socket handle
 *
 *  @param [in]    *aData			Pointer to data to send
 *
 *  @param [in]    aNumBytes 		Number of bytes to write
 *
 *  @param [in]    aFlush 			ETrue if flush output
 *
 *  @param [in]    aTimeout 		Timeout value in milliseconds
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  TUInt8 data[1500];
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketWrite(Network, aSocket, (void *)&data, 1500, ETrue, 5000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketWrite(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TBool aFlush,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketWrite((void *)p, aSocket, aData, aNumBytes, aFlush, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkAuxCommand
 *---------------------------------------------------------------------------*/
/**
 *  Auxiliary command to send the network device driver a non-standard
 *      command.  Exact implementation depends on the specific device
 *      driver being used.
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aAuxCommand 		Command number to execute
 *
 *  @param [in]    *aAuxData		Additional data for command or 0 for none.
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  UEZNetworkAuxCommand(Network, 0, 0);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkAuxCommand(
        T_uezDevice aNetworkDevice,
        TUInt32 aAuxCommand,
        void *aAuxData)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->AuxControl((void *)p, aAuxCommand, aAuxData);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkSocketConnect
 *---------------------------------------------------------------------------*/
/**
 *  Connect an outgoing socket to another server or system.
 *
 *  @param [in]    aNetworkDevice 	Network device driver
 *
 *  @param [in]    aSocket			Socket handle
 *
 *  @param [in]    *aAddr 			Address of computer to connect to
 *
 *  @param [in]    aPort 			Port number on other computer
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezNetworkAddr addr = {192, 168, 1, 1};
 *  T_uezDevice Network;
 *  T_uezNetworkSocket aSocket;
 *  UEZNetworkSocketCreate(Network, UEZ_NETWORK_SOCKET_TYPE_TCP, &aSocket);
 *  UEZNetworkSocketConnect(Network, aSocket,  &addr, 2000); 
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkSocketConnect(
        T_uezDevice aNetworkDevice,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SocketConnect((void *)p, aSocket, aAddr, aPort);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkIPV4StringToAddr
 *---------------------------------------------------------------------------*/
/**
 *  Convert an IPV4 numeric string into a network address structure.
 *
 *  @param [in]    *aString -- String containing address
 *
 *  @param [in]    *aAddr -- Pointer to address structure to fill
 *
 *  @return        T_uezError -- Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezNetworkAddr addr;
 *  UEZNetworkIPV4StringToAddr("192.168.1.1", &addr);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkIPV4StringToAddr(
        const char *aString,
        T_uezNetworkAddr *aAddr)
{
    unsigned int a[4];//a1, a2, a3, a4;
    char octet[4];
    TUInt8 i = 0;
    TUInt8 j = 0;
    TUInt8 pos = 0;

    if(aString[i] == '\"'){
        i++;
    }
    while(1){
        while(aString[i] != '.' && aString[i] != '\0'){
            octet[j++] = aString[i++];
        }
        octet[j] = '\0';
        a[pos] = atoi(octet);
        j = 0;
        i++;
        if(a[pos++] > 256){
            return UEZ_ERROR_INVALID_PARAMETER;
        }
        if(pos == 4){
            aAddr->v4[0] = a[0];
            aAddr->v4[1] = a[1];
            aAddr->v4[2] = a[2];
            aAddr->v4[3] = a[3];
            return UEZ_ERROR_NONE;
        }
    }


#if 0
    a1 = a2 = a3 = a4 = 256;
    sscanf(aString, "%d.%d.%d.%d", &a1, &a2, &a3, &a4);

    if ((a1 >= 256) || (a2 >= 256) || (a3 >= 256) || (a4 >= 256))
        return UEZ_ERROR_INVALID_PARAMETER;

    aAddr->v4[0] = a1;
    aAddr->v4[1] = a2;
    aAddr->v4[2] = a3;
    aAddr->v4[3] = a4;
#endif
    
    //return UEZ_ERROR_NONE;
}

#if 0
/*---------------------------------------------------------------------------*
 * Routine:  IScanGetNetworkName
 *---------------------------------------------------------------------------*/
/**
 *  General routine to catch a list of scanned network names and copy
 *  them into the given structure (a single name). 
 *  This is the simplest implementation.
 *
 *  @param [in]    *aCallbackWorkspace 	Pointer to data for this scan state
 *
 *  @param [in]    *aNetworkInfo 		Info about a network found
 *
 *  @return        TBool				ETrue if scan should immediately stop
 * 										here, else EFalse
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezNetworkInfo aNetworkInfo;
 *  if (IScanGetNetworkName(void *aCallbackWorkspace, &aNetworkInfo) == ETrue) {
 *  // found network
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TBool IScanGetNetworkName(
    void *aCallbackWorkspace,
    T_uezNetworkInfo *aNetworkInfo)
{
    memcpy(aCallbackWorkspace, aNetworkInfo->iName,
        UEZ_NETWORK_INFO_NAME_LENGTH + 1);
    return ETrue;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkConnect
 *---------------------------------------------------------------------------*/
/**
 *  This routine provides an easy all in one call to connect to the
 *      given network and then join the given subnetwork.  Settings
 *      for IP address, mask, and gateway are provided as well enabling
 *      the DHCP for dynamic IP address assignment.  After connecting,
 *      the network handle is returned and the status too.
 *
 *  @param [in]    *aNetworkName 	Name of network driver to connect with
 *
 *  @param [in]    *aJoinName		Name of subnetwork to join
 *
 *  @param [in]    *aIPAddr			IP address to assign, or 0 to not
 *          						assign an IP address.
 *  @param [in]    *aIPMask 		Subnet mask to use, or 0 to use
 *         							the defaults of the network.
 *  @param [in]    *aIPGateway 		IP address of gateway in subnet,
 *         							or 0 to use the defaults.
 *  @param [in]    aEnableDHCP 		ETrue if DHCP is to be used, else EFalse.
 *          						NOTE: May not work in all network
 * 									connections.
 *  @param [in]    *aNetwork 		Network device driver connected.
 *
 *  @param [in]    aStatus 			Resulting status of network connection.
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezDevice Network;
 *  T_uezNetworkAddr IPaddr = {192,168,1,100};
 *  T_uezNetworkAddr IPMask = {255,255,255,0};
 *  T_uezNetworkAddr IPGateway = {192,168,1,1};
 *  T_uezNetworkStatus aStatus;
 *  UEZNetworkConnect("Network", "lwip", &IPaddr, &IPMask, IPGateway,
 * 										 EFalse, &Network, &aStatus);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkConnect(
        const char *aNetworkName,
        const char *aJoinName,
        const T_uezNetworkAddr *aIPAddr,
        const T_uezNetworkAddr *aIPMask,
        const T_uezNetworkAddr *aIPGateway,
        TBool aEnableDHCP,
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus)
{
#if 0
    T_uezNetworkSettings settings;
    T_uezError error = UEZ_ERROR_NONE;
    char networkName[32 + 1];
    T_uezDevice network;

    *aNetwork = 0;

    memset(&settings, 0, sizeof(settings));
    memset(aStatus, 0, sizeof(T_uezNetworkStatus));

    settings.iNetworkType = UEZ_NETWORK_TYPE_INFRASTRUCTURE;
    settings.iScanChannel = 0; // auto
    settings.iAutoConnectToOpen = ETrue;
    settings.iTXRate = 0;
    settings.iTXPower = UEZ_NETWORK_TRANSMITTER_POWER_HIGH;
    settings.iEnableDHCP = aEnableDHCP;
    settings.iScanSSID[0] = '\0';
    settings.iJoinSSID[0] = '\0';
    if (aIPAddr)
        memcpy(settings.iIPAddress.v4, aIPAddr->v4, 4);
    if (aIPMask)
        memcpy(settings.iSubnetMask.v4, aIPMask->v4, 4);
    if (aIPGateway)
        memcpy(settings.iGatewayAddress.v4, aIPGateway->v4, 4);

    error = UEZNetworkOpen(aNetworkName, &network, &settings);
    if (error)
        return error;

    if (aNetwork)
        *aNetwork = network;

    error = UEZNetworkScan(network, 0, 0, IScanGetNetworkName,
        (void *)networkName, 5000);
    if (error) {
        UEZNetworkClose(network);
        network = 0;
        return error;
    }

    if (aJoinName) {
        error = UEZNetworkJoin(network, aJoinName, "fdifdifdiFDI", 5000);
        if (error) {
            UEZNetworkClose(network);
            network = 0;
            return error;
        }
    }

    error = UEZNetworkGetStatus(network, aStatus);
    if (error) {
        UEZNetworkClose(network);
        network = 0;
        return error;
    }
    return error;
#else
    // Deprecated!
    return UEZ_ERROR_NOT_SUPPORTED;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZNetworkResolveAddress
 *---------------------------------------------------------------------------*/
/**
 *  Resolve a string from the Dynamic Name Server (DNS) from a characters
 *      into a network address (e.g. "some.computer.net" -> aaa.bbb.ccc.ddd)
 *
 *  @param [in]    *aNetworkDevice 		Network device driver connected.
 *
 *  @param [in]    *aString 			String containing address
 *
 *  @param [in]    *aAddr				Pointer to address structure to fill
 *
 *  @return        T_uezError			Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZNetwork.h>
 *
 *  T_uezNetworkAddr addr;
 *  T_uezDevice Network;
 *  UEZNetworkResolveAddress(Network,"google.com", &addr);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZNetworkResolveAddress(
    T_uezDevice aNetworkDevice,
    const char *aString,
    T_uezNetworkAddr *aAddr)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ResolveAddress((void *)p, aString, aAddr);
}

T_uezError UEZNetworkInfrastructureConfigure(
    T_uezDevice aNetworkDevice,
    T_uezNetworkSettings *aSettings)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice,
        (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->InfrastructureConfigure((void *)p, aSettings);
}

T_uezError UEZNetworkInfrastructureBringUp(T_uezDevice aNetworkDevice)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice,
        (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->InfrastructureBringUp((void *)p);
}

T_uezError UEZNetworkInfrastructureTakeDown(T_uezDevice aNetworkDevice)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice,
        (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->InfrastructureTakeDown((void *)p);
}

T_uezError UEZNetworkGetConnectionInfo(T_uezDevice aNetworkDevice,
		T_uezNetworkSocket aSocket,
		T_uEZNetworkConnectionInfo *aConnection)
{
    T_uezError error;
    DEVICE_Network **p;

    error = UEZDeviceTableGetWorkspace(aNetworkDevice,
        (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetConnectionInfo((void *)p, aSocket, aConnection);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZNetwork.c
 *-------------------------------------------------------------------------*/
