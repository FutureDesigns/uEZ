/*-------------------------------------------------------------------------*
 * File:  Network_lwIP.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Network for the PCA9551
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
#include <string.h>
#include <uEZ.h>
#include <uEZDevice.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <uEZDeviceTable.h>
#include "Network_lwIP.h"

#include "arch/lwIP_EMAC.h"
/* lwIP includes. */
//#include "lwip/init.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/stats.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"


#if (!LWIP_SO_RCVTIMEO)
#error "LWIP_SO_RCVTIMEO must be 1 to use device Network::lwIP"
#endif

T_uezTask G_lwipTask;

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
#define NETWORK_LWIP_NUM_SOCKETS             NETWORK_LWIP_MAX_NUM_SOCKETS
#define NETWORK_LWIP_LOCAL_PORT_START        0x8000
#define NETWORK_LWIP_LOCAL_PORT_END          0x9000

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef enum {
    SOCKET_STATE_FREE, SOCKET_STATE_CREATED, SOCKET_STATE_LISTENING
} T_lwIPSocketState;

typedef TUInt16 T_lwIPSocketFlags;
#define SOCKET_FLAG_CONNECTED           (1<<0)

typedef struct {
    T_lwIPSocketState iState;
    T_uezNetworkSocketType iType;
    TUInt16 iPort;
    volatile T_lwIPSocketFlags iFlags;
    TUInt16 iSourcePort;
    struct netconn *iNetconn;
    struct netbuf *iReceiveNetBuf;
    TUInt8 *iReceiveData;
    TUInt16 iReceiveLength;
    TUInt16 iReceiveRemaining;
} T_lwIPSocket;

typedef struct {
    DEVICE_Network **iDevice;
    TUInt32 iNumOpen;
    T_uezSemaphore iSem;
    struct netif EMAC_if;
    T_uezNetworkInfo iInfo;
    T_uezNetworkJoinStatus iJoinStatus;
    T_uezNetworkScanStatus iScanStatus;

    // Track sockets in the network
    T_lwIPSocket iSockets[NETWORK_LWIP_NUM_SOCKETS + 1]; // first one is not used
    // use range of NETWORK_LWIP_LOCAL_PORT_START - NETWORK_LWIP_LOCAL_PORT_END

    T_uezNetworkSettings iInfrastructureSettings;
} T_Network_lwIP_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_uezError IConvertErrorCode(err_t aLWIPError)
{
    switch (aLWIPError) {
        case ERR_OK: /* No error, everything OK. */
            return UEZ_ERROR_NONE;
        case ERR_MEM: /* Out of memory error.     */
            return UEZ_ERROR_OUT_OF_MEMORY;
        case ERR_BUF: /* Buffer error.            */
            return UEZ_ERROR_BUFFER_ERROR;
        case ERR_RTE: /* Routing problem.         */
            return UEZ_ERROR_INTERNAL_ERROR;
        case ERR_ABRT: /* Connection aborted.      */
            return UEZ_ERROR_ABORTED;
        case ERR_RST: /* Connection reset.        */
            return UEZ_ERROR_RESETED;
        case ERR_CLSD: /* Connection closed.       */
            return UEZ_ERROR_CLOSED;
        case ERR_CONN: /* Not connected.           */
            return UEZ_ERROR_COULD_NOT_CONNECT;
        case ERR_VAL: /* Illegal value.           */
            return UEZ_ERROR_ILLEGAL_VALUE;
        case ERR_ARG: /* Illegal argument.        */
            return UEZ_ERROR_ILLEGAL_ARGUMENT;
        case ERR_USE: /* Address in use.          */
            return UEZ_ERROR_ALREADY_EXISTS;
        case ERR_IF: /* Low-level netif error    */
            return UEZ_ERROR_INTERNAL_ERROR;
        case ERR_ISCONN: /* Already connected.       */
            return UEZ_ERROR_ALREADY_OPEN;
        case ERR_TIMEOUT: /* Timeout.                 */
            return UEZ_ERROR_TIMEOUT;
        case ERR_INPROGRESS: /* Operation in progress    */
            return UEZ_ERROR_BUSY;
    }
    return UEZ_ERROR_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a redpine networking subsystem workspace.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_lwIP_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;

    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    p->iNumOpen = 0;
    memset(p->iSockets, 0, sizeof(p->iSockets));

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
#if UEZ_REGISTER
    UEZSemaphoreSetName(p->iSem, "Lwip", "\0");
#endif

    p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_IDLE;
    p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_IDLE;

    return error;
}

#if LWIP_DHCP
static TUInt32 DHCP(T_uezTask aMyTask, void *aParams)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace*)aParams;
    TUInt32 mscnt = 0;

    while(p->EMAC_if.ip_addr.addr == 0){
        UEZTaskDelay(DHCP_FINE_TIMER_MSECS);
        mscnt += DHCP_FINE_TIMER_MSECS;
        if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
               dhcp_coarse_tmr();
               mscnt = 0;
        }
        //autoip_tmr();
    }
    //LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("pbuf->len = %"U16_F"\n", p->len));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("\nDone with DHCP"));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("\nAcquired IP ="));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("%d", 0xff&(p->EMAC_if.ip_addr.addr)));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("."));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("%d", 0xff&(p->EMAC_if.ip_addr.addr>>8)));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("."));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("%d", 0xff&(p->EMAC_if.ip_addr.addr>>16)));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("."));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("%d", 0xff&(p->EMAC_if.ip_addr.addr>>24)));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("\n"));
    return 0;
}
#endif

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
    T_Network_lwIP_Workspace *p,
    T_uezNetworkSettings *aSettings)
{
    extern err_t ethernetif_init(struct netif *netif);
    T_uezError error = UEZ_ERROR_NONE;
#if LWIP_2_0_x
    ip_addr_t xIpAddr, xNetMast, xGateway;//, xPriDnsSrv, xSecDnsSrv;
#else
    struct ip_addr xIpAddr, xNetMast, xGateway;
#endif

    // Only support infrastructure network types
    if (aSettings->iNetworkType != UEZ_NETWORK_TYPE_INFRASTRUCTURE)
        return UEZ_ERROR_INCORRECT_TYPE;

G_lwipTask = tcpip_init(NULL, NULL);
    if(G_lwipTask == 0) { // incresae your heap or lower TCPIP_THREAD_STACKSIZE
      return UEZ_ERROR_OUT_OF_MEMORY;
    }
    
#if LWIP_DHCP
    /* Create and configure the EMAC interface. */
    if(aSettings->iEnableDHCP){
        IP4_ADDR(&xIpAddr, 0, 0, 0, 0);
        IP4_ADDR(&xNetMast, 0, 0, 0, 0);
        IP4_ADDR(&xGateway, 0, 0, 0, 0);
        UEZTaskCreate(DHCP, "DHCP", 1024, (void *)p, UEZ_PRIORITY_NORMAL,
                0);

    } else {
#endif
        IP4_ADDR(&xIpAddr, aSettings->iIPAddress.v4[0],
            aSettings->iIPAddress.v4[1], aSettings->iIPAddress.v4[2],
            aSettings->iIPAddress.v4[3]);
        IP4_ADDR(&xNetMast, aSettings->iSubnetMask.v4[0],
            aSettings->iSubnetMask.v4[1], aSettings->iSubnetMask.v4[2],
            aSettings->iSubnetMask.v4[3]);
        IP4_ADDR(&xGateway, aSettings->iGatewayAddress.v4[0],
            aSettings->iGatewayAddress.v4[1], aSettings->iGatewayAddress.v4[2],
            aSettings->iGatewayAddress.v4[3]);
#if LWIP_DHCP
    }
#endif

    /* uEZ v2.04 -- The MAC address is now passed into via the Network Settings
     *              instead of using a confusing callback. */
#if LWIP_2_0_x
    p->EMAC_if.hwaddr_len = 6;
#else
    p->EMAC_if.hwaddr_len_padding = 6;
#endif
    p->EMAC_if.hwaddr[0] = aSettings->iMACAddress.v4[0];
    p->EMAC_if.hwaddr[1] = aSettings->iMACAddress.v4[1];
    p->EMAC_if.hwaddr[2] = aSettings->iMACAddress.v4[2];
    p->EMAC_if.hwaddr[3] = aSettings->iMACAddress.v4[3];
    p->EMAC_if.hwaddr[4] = aSettings->iMACAddress.v4[4];
    p->EMAC_if.hwaddr[5] = aSettings->iMACAddress.v4[5];

    netif_add(&p->EMAC_if, &xIpAddr, &xNetMast, &xGateway, NULL,
        ethernetif_init, tcpip_input);

    /* make it the default interface */
    netif_set_default(&p->EMAC_if);

    /* bring it up */
    netif_set_up(&p->EMAC_if);

#if LWIP_DHCP
    if(aSettings->iEnableDHCP){
        dhcp_start(&p->EMAC_if);
    }
    else { /* // TODO does this require more integration somewhere else?
          IP4_ADDR(&xPriDnsSrv, aSettings->iPriDnsServer.v4[0],
            aSettings->iPriDnsServer.v4[1], aSettings->iPriDnsServer.v4[2],
            aSettings->iPriDnsServer.v4[3]);
          IP4_ADDR(&xSecDnsSrv, aSettings->iSecDnsServer.v4[0],
            aSettings->iSecDnsServer.v4[1], aSettings->iSecDnsServer.v4[2],
            aSettings->iSecDnsServer.v4[3]);
          // Added static DNS server 
          dns_setserver(0, &xPriDnsSrv);
          dns_setserver(1, &xSecDnsSrv); */
    }
#endif

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
static T_uezError IShutdown(T_Network_lwIP_Workspace *p)
{
    PARAM_NOT_USED(p);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open up a new network user
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_lwIP_Open(void *aWorkspace)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iNumOpen++;
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close out a network that was previously opened.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_lwIP_Close(void *aWorkspace)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iNumOpen == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        if (p->iNumOpen == 1) {
            // Last user done!  Let's close out and turn off the library
            error = IShutdown(p);
            if (error == UEZ_ERROR_NONE)
              p->iNumOpen--;
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_Scan
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
T_uezError Network_lwIP_Scan(
    void *aWorkspace,
    TUInt32 aChannelNumber,
    const char *aScanSSID,
    T_uezNetworkScanCallback aCallback,
    void *aCallbackWorkspace,
    TUInt32 aTimeout)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_uezNetworkInfo scanInfo;

    PARAM_NOT_USED(aCallback);PARAM_NOT_USED(aCallbackWorkspace);

    // Doing a scan is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    scanInfo.iRSSILevel = 0;
    scanInfo.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_OPEN;
    scanInfo.iChannel = 0;
    strcpy(scanInfo.iName, "lwIP");
    p->iInfo = scanInfo;
    p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_COMPLETE;

    if (aCallback) {
        aCallback(aCallbackWorkspace, &scanInfo);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_Join
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
T_uezError Network_lwIP_Join(
    void *aWorkspace,
    const char *aJoinName,
    const char *aJoinPassword,
    TUInt32 aTimeout)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Doing a join is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // You can only join the group "lwIP"
    if (strcmp(aJoinName, "lwIP")) {
        error = UEZ_ERROR_NOT_FOUND;
        p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_FAIL;
    } else {
        // Passwords are ignored
        // Right name
        p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_SUCCESS;
        error = UEZ_ERROR_NONE;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the lwIP (if any).
 *      NOTE: This is a good place for the programming for the MAC address
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
void Network_lwIP_Configure(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current status of the lwIP network interface
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezNetworkStatus *aStatus -- General status
 *---------------------------------------------------------------------------*/
static T_uezError Network_lwIP_GetStatus(
    void *aWorkspace,
    T_uezNetworkStatus *aStatus)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 addr;

    // Grab and pause the background task (we don't want
    // the background task changing things while we copy over the info)
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    memset(aStatus, 0, sizeof(*aStatus));
    aStatus->iInfo = p->iInfo;
    addr = ntohl(p->EMAC_if.ip_addr.addr);
    aStatus->iIPAddr.v4[0] = (addr >> 24) & 0xFF;
    aStatus->iIPAddr.v4[1] = (addr >> 16) & 0xFF;
    aStatus->iIPAddr.v4[2] = (addr >> 8) & 0xFF;
    aStatus->iIPAddr.v4[3] = (addr >> 0) & 0xFF;
    addr = ntohl(p->EMAC_if.gw.addr);
    aStatus->iGatewayAddress.v4[0] = (addr >> 24) & 0xFF;
    aStatus->iGatewayAddress.v4[1] = (addr >> 16) & 0xFF;
    aStatus->iGatewayAddress.v4[2] = (addr >> 8) & 0xFF;
    aStatus->iGatewayAddress.v4[3] = (addr >> 0) & 0xFF;
    addr = ntohl(p->EMAC_if.netmask.addr);
    aStatus->iSubnetMask.v4[0] = (addr >> 24) & 0xFF;
    aStatus->iSubnetMask.v4[1] = (addr >> 16) & 0xFF;
    aStatus->iSubnetMask.v4[2] = (addr >> 8) & 0xFF;
    aStatus->iSubnetMask.v4[3] = (addr >> 0) & 0xFF;
    aStatus->iJoinStatus = p->iJoinStatus;
    aStatus->iScanStatus = p->iScanStatus;

    // Done, release the background task and continue
    UEZSemaphoreRelease(p->iSem);

    return error;
}

static T_uezNetworkSocket ISocketCreate(
    T_Network_lwIP_Workspace *p,
    T_uezNetworkSocketType aType,
    struct netconn *aAllocNetconn)
{
    T_uezNetworkSocket i;
    T_uezNetworkSocket socket = 0; // none found yet
    for (i = 1; i <= NETWORK_LWIP_NUM_SOCKETS; i++) {
        if (p->iSockets[i].iState == SOCKET_STATE_FREE) {
            // We found a slot
            // Can we get the socket from lwIP?
            if (aAllocNetconn) {
                p->iSockets[i].iNetconn = aAllocNetconn;
            } else {
                p->iSockets[i].iNetconn
                    = netconn_new((aType == UEZ_NETWORK_SOCKET_TYPE_TCP)?NETCONN_TCP:NETCONN_UDP);
                tcp_nagle_disable(p->iSockets[i].iNetconn->pcb.tcp);
            }
            if (p->iSockets[i].iNetconn) {
                // Allocate this slot and report back this slot
                p->iSockets[i].iState = SOCKET_STATE_CREATED;
                p->iSockets[i].iType = aType;
                p->iSockets[i].iFlags = 0;
                if ((aType == UEZ_NETWORK_SOCKET_TYPE_TCP) && (p->iSockets[i].iNetconn->pcb.tcp)) // TODO: Is there a better way to ensure TF_NODELAY when tcp doesn't exist yet?
                    p->iSockets[i].iNetconn->pcb.tcp->flags |= TF_NODELAY;
                socket = i;
            }
            // Either we got a socket or not.  Either way, stop here.
            break;
        }
    }
    return socket;
}

static void ISocketFree(T_Network_lwIP_Workspace *p, T_uezNetworkSocket aSocket)
{
    T_lwIPSocket *p_socket = p->iSockets + aSocket;

    p_socket->iState = SOCKET_STATE_FREE;
    p_socket->iType = UEZ_NETWORK_SOCKET_TYPE_TCP;
    p_socket->iFlags = 0;
    if (p_socket->iReceiveNetBuf) {
        netbuf_delete(p_socket->iReceiveNetBuf);
        p_socket->iReceiveNetBuf = 0;
    }
    p_socket->iReceiveData = 0;
    p_socket->iReceiveLength = 0;
    p_socket->iReceiveRemaining = 0;
    p_socket->iPort = 0;
    p_socket->iNetconn = 0;
    p_socket->iSourcePort = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_lwIP_SocketCreate
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a socket for communicating.  At this point, it may just
 *      allocate the socket.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezNetworkStatus *aStatus -- General status
 *---------------------------------------------------------------------------*/
T_uezError Network_lwIP_SocketCreate(
    void *aWorkspace,
    T_uezNetworkSocketType aType,
    T_uezNetworkSocket *aSocket)
{
    // Find a free socket slot
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;

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

T_uezError Network_lwIP_SocketBind(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;
#if LWIP_2_0_x
    ip_addr_t ip;
#else
    struct ip_addr ip;
#endif

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
        if (aAddr)
            IP4_ADDR(&ip, aAddr->v4[0], aAddr->v4[1], aAddr->v4[2],
                aAddr->v4[3]);
        error = IConvertErrorCode(netconn_bind(p_socket->iNetconn,
            (aAddr) ? &ip : 0, aPort));
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketConnect(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    T_uezNetworkAddr *aAddr,
    TUInt16 aPort)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;
#if LWIP_2_0_x
    ip_addr_t ip;
#else
    struct ip_addr ip;
#endif

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
        IP4_ADDR(&ip, aAddr->v4[0], aAddr->v4[1], aAddr->v4[2], aAddr->v4[3]);

        error = IConvertErrorCode(netconn_connect(p_socket->iNetconn, &ip,
            aPort));
        if (error == UEZ_ERROR_NONE)
            p_socket->iFlags |= SOCKET_FLAG_CONNECTED;

    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketListen(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else {
        error = IConvertErrorCode(netconn_listen(p_socket->iNetconn));
        if (error == UEZ_ERROR_NONE) {
            p_socket->iState = SOCKET_STATE_LISTENING;
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketAccept(
    void *aWorkspace,
    T_uezNetworkSocket aListenSocket,
    T_uezNetworkSocket *aCreatedSocket,
    TUInt32 aTimeout)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    struct netconn *p_acceptedNetconn = 0;
    T_lwIPSocket *p_socket = p->iSockets + aListenSocket;
    T_lwIPSocket *p_newSocket;
#if LWIP_2_0_x
    err_t err = ERR_OK;
#endif

    // So far, we got nothing
    *aCreatedSocket = 0;

    // Only valid sockets
    if ((aListenSocket == 0) || (aListenSocket > NETWORK_LWIP_NUM_SOCKETS))
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
        p_socket->iNetconn->recv_timeout = aTimeout;
#if LWIP_2_0_x
        if (err == ERR_TIMEOUT)
            err = ERR_OK;
#else
        if (p_socket->iNetconn->err == ERR_TIMEOUT)
            p_socket->iNetconn->err = ERR_OK;
#endif
        UEZSemaphoreRelease(p->iSem);
#if LWIP_2_0_x
        err = netconn_accept(p_socket->iNetconn, &p_acceptedNetconn);
#else
        p_acceptedNetconn = netconn_accept(p_socket->iNetconn);
#endif
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
        if (p_acceptedNetconn) {
            *aCreatedSocket = ISocketCreate(p, p_socket->iType, p_acceptedNetconn);
            if (*aCreatedSocket) {
                p_newSocket = p->iSockets + *aCreatedSocket;
                p_newSocket->iNetconn = p_acceptedNetconn;
                p_newSocket->iFlags |= SOCKET_FLAG_CONNECTED;
                error = UEZ_ERROR_NONE;
            } else {
                error = UEZ_ERROR_OUT_OF_HANDLES;
            }
        } else {
#if LWIP_2_0_x
            error = IConvertErrorCode(err);
#else
            error = IConvertErrorCode(p_socket->iNetconn->err);
#endif
            // Free this previously created socket, we had an error
            ISocketFree(p, *aCreatedSocket);
            *aCreatedSocket = 0;
            // If no other reason, then state out of handles
            if (error == UEZ_ERROR_NONE)
                error = UEZ_ERROR_OUT_OF_HANDLES;
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketClose(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        // Delete any receive buffers still open
        if (p_socket->iReceiveNetBuf) {
            netbuf_delete(p_socket->iReceiveNetBuf);
        }
        p_socket->iReceiveData = 0;
        p_socket->iReceiveLength = 0;
        p_socket->iReceiveRemaining = 0;
        p_socket->iReceiveNetBuf = 0;
        p_socket->iFlags = 0;
        error = IConvertErrorCode(netconn_close(p_socket->iNetconn));
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketDelete(
    void *aWorkspace,
    T_uezNetworkSocket aSocket)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) != 0) {
        error = UEZ_ERROR_MUST_CLOSE_FIRST;
    } else {
        netconn_delete(p_socket->iNetconn);
        ISocketFree(p, aSocket);
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketRead(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesRead,
    TUInt32 aTimeout)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;
    TUInt16 numCopy;
#if LWIP_2_0_x
    err_t err = ERR_OK;
#endif
    //    TUInt32 timeStart = UEZTickCounterGet();

    // No bytes read yet
    *aNumBytesRead = 0;

    // Only valid sockets
    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
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
                memcpy(aData, p_socket->iReceiveData + p_socket->iReceiveLength
                    - p_socket->iReceiveRemaining, numCopy);

                // Update counts and pointers
                *aNumBytesRead += numCopy;
                p_socket->iReceiveRemaining -= numCopy;
                aData = (void *)(((TUInt8 *)aData) + numCopy);

                // Decrement the amount we are waiting on
                aNumBytes -= numCopy;

                // If count goes to zero, then we need to release this pbuf
                // and go to the next.
                if (p_socket->iReceiveRemaining == 0) {
                    if (netbuf_next(p_socket->iReceiveNetBuf) >= 0) {
                        // Got more data!  Setup the info for it
#if LWIP_2_0_x
                        err = netbuf_data(p_socket->iReceiveNetBuf,
                            (void **)&p_socket->iReceiveData, &p_socket->iReceiveLength);
#else
                        netbuf_data(p_socket->iReceiveNetBuf,
                            (void **)&p_socket->iReceiveData, &p_socket->iReceiveLength);
#endif
                        p_socket->iReceiveRemaining = p_socket->iReceiveLength;
                    } else {
                        // Done with this netbuf, delete it
                        netbuf_delete(p_socket->iReceiveNetBuf);
                        p_socket->iReceiveNetBuf = 0;
                        p_socket->iReceiveData = 0;
                    }
                }
            }

            // Do we need another packet of dat?  If so, we need to fetch more data
            if ((aNumBytes) && (p_socket->iReceiveRemaining == 0)) {
                // Change the timeout for this connection
                p_socket->iNetconn->recv_timeout = aTimeout;
#if LWIP_2_0_x
                if (err == ERR_TIMEOUT)
                    err = ERR_OK;
                err = netconn_recv(p_socket->iNetconn, &p_socket->iReceiveNetBuf);
#else
                if (p_socket->iNetconn->err == ERR_TIMEOUT)
                    p_socket->iNetconn->err = ERR_OK;
                p_socket->iReceiveNetBuf = netconn_recv(p_socket->iNetconn);
#endif
                if (p_socket->iReceiveNetBuf == 0) {
                    // Received no data, possibly timed out?
                    // Stop with the given returned data
#if LWIP_2_0_x
                    error = IConvertErrorCode(err);
#else
                    error = IConvertErrorCode(p_socket->iNetconn->err);
#endif
                    break;
                }
                // We did get data, pull out the pertinent info
#if LWIP_2_0_x
                err = netbuf_data(p_socket->iReceiveNetBuf, (void **)&p_socket->iReceiveData,
                        &p_socket->iReceiveLength);
#else
                netbuf_data(p_socket->iReceiveNetBuf, (void **)&p_socket->iReceiveData,
                    &p_socket->iReceiveLength);
#endif
                p_socket->iReceiveRemaining = p_socket->iReceiveLength;
            }
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_SocketWrite(
    void *aWorkspace,
    T_uezNetworkSocket aSocket,
    void *aData,
    TUInt32 aNumBytes,
    TBool aFlush,
    TUInt32 aTimeout)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_lwIPSocket *p_socket = p->iSockets + aSocket;
    TUInt16 numWrite;
    PARAM_NOT_USED(aTimeout);

    if ((aSocket == 0) || (aSocket > NETWORK_LWIP_NUM_SOCKETS))
        return UEZ_ERROR_HANDLE_INVALID;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p_socket->iState == SOCKET_STATE_FREE) {
        error = UEZ_ERROR_HANDLE_INVALID;
    } else if ((p_socket->iFlags & SOCKET_FLAG_CONNECTED) == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        while (aNumBytes) {
            //if (aNumBytes > 0xFFFF) // Send data up to a 16-bit value
                //numWrite = 0xFFFF;
            if (aNumBytes > 1440) // Send data up to MSS value
                numWrite = 1440;
            else
                numWrite = (TUInt16)aNumBytes;
            aNumBytes -= numWrite;

#ifndef LWIP_2_0_x
            // Clean up any previous timeout errors
            if (p_socket->iNetconn->err == ERR_TIMEOUT)
                p_socket->iNetconn->err = ERR_OK;
#else       // Clean up any previous timeout errors // TODO clear ERR_INPROGRESS
#endif
            // Write out this segment (noting if there is data past this one)
            error
                = IConvertErrorCode(netconn_write(p_socket->iNetconn, aData,
                    numWrite, ((aFlush) ? NETCONN_COPY
                        : NETCONN_MORE)));
            // Stop on any errors
            if (error != UEZ_ERROR_NONE)
                break;

            aData = (void *)(((char *)aData) + numWrite);
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_AuxControl(
    void *aWorkspace,
    TUInt32 aAuxCommand,
    void *aAuxData)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
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

T_uezError Network_lwIP_Leave(void *aWorkspace, TUInt32 aTimeout)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aTimeout);

    // doesn't do anything in this implementation

    return UEZ_ERROR_NONE;
}

T_uezError Network_lwIP_ResolveAddress(
    void *aWorkspace,
    const char *aName,
    T_uezNetworkAddr *aAddr)
{
#if LWIP_DNS
    struct hostent* hp;
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    hp = (struct hostent*)lwip_gethostbyname(aName);
    if (hp->h_length == 4)
        memcpy(aAddr->v4, (char *)hp->h_addr, hp->h_length);
    else
        error = UEZ_ERROR_MISMATCH;

    UEZSemaphoreRelease(p->iSem);
    return error;
#else
	return UEZ_ERROR_NOT_AVAILABLE;
#endif
}



void Network_lwIP_Create(const char *aName)
{
    T_uezDeviceWorkspace *p_wired;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&lwIP_Network_Interface,
            0,
            (T_uezDeviceWorkspace **)&p_wired);

    Network_lwIP_Configure(p_wired);
}

T_uezError Network_lwIP_InfrastructureConfigure(
    void *aWorkspace,
    T_uezNetworkSettings *aSettings)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iInfrastructureSettings = *aSettings;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

T_uezError Network_lwIP_InfrastructureBringUp(void *aWorkspace)
{
    T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = IStartup(p, &p->iInfrastructureSettings);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_lwIP_InfrastructureTakeDown(void *aWorkspace)
{
    // Not currently supported, yet
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Network_lwIP_GetConnectionInfo(void *aWorkspace,
		T_uezNetworkSocket aSocket,
		T_uEZNetworkConnectionInfo *aConnection)
{
	T_uezError error = UEZ_ERROR_NONE;
	T_Network_lwIP_Workspace *p = (T_Network_lwIP_Workspace *)aWorkspace;
	T_lwIPSocket *p_socket = p->iSockets + aSocket;
#if LWIP_2_0_x
    ip_addr_t ip;
#else
    struct ip_addr ip;
#endif

    error = IConvertErrorCode(netconn_getaddr(p_socket->iNetconn, &ip, &aConnection->iConnectedPort, 0));
    aConnection->iIPConnectedAddr[3] = (ip.addr>> 24) &0xFF;
    aConnection->iIPConnectedAddr[2] = (ip.addr>> 16) &0xFF;
    aConnection->iIPConnectedAddr[1] = (ip.addr>> 8) &0xFF;
    aConnection->iIPConnectedAddr[0] = (ip.addr>> 0) &0xFF;

	return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Network lwIP_Network_Interface = { {
// Common device interface
    "Network:lwIP",
    0x0113,
    Network_lwIP_InitializeWorkspace,
    sizeof(T_Network_lwIP_Workspace), },

// Functions
    // added in uEZ v1.07
    Network_lwIP_Open,
    Network_lwIP_Close,
    Network_lwIP_Scan,
    Network_lwIP_Join,
    Network_lwIP_GetStatus,
    Network_lwIP_SocketCreate,
    Network_lwIP_SocketConnect,
    Network_lwIP_SocketBind,
    Network_lwIP_SocketListen,
    Network_lwIP_SocketAccept,
    Network_lwIP_SocketClose,
    Network_lwIP_SocketDelete,
    Network_lwIP_SocketRead,
    Network_lwIP_SocketWrite,
    Network_lwIP_AuxControl,
    // uEZ v1.09
    Network_lwIP_Leave,
    // uEZ v1.13
    Network_lwIP_ResolveAddress,
    // uEZ v2.04
    Network_lwIP_InfrastructureConfigure,
    Network_lwIP_InfrastructureBringUp,
    Network_lwIP_InfrastructureTakeDown,
    //uEZ v2.07
    Network_lwIP_GetConnectionInfo,
};

/*-------------------------------------------------------------------------*
 * End of File:  Network_lwIP.c
 *-------------------------------------------------------------------------*/
