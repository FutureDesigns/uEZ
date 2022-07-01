/*-------------------------------------------------------------------------*
 * File:  AtCmdLib.c
 *-------------------------------------------------------------------------*/
/** @addtogroup AtCmdLib
 *       @{ *     @brief The GainSpan AT Command Library (AtCmdLib) provides the 
 *      functions that send AT commands to a GainSpan node and looks for a 
 *      response. Parse commands are provided to interpret the response data.
 */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> /* for sprintf(), strstr(), strlen() , strtok() and strcpy()  */
#include <stdlib.h>
#include <ctype.h>
#include "AtCmdLib.h"
#include "GainSpan_Config.h"

//#define ATLIBGS_DEBUG_ENABLE
#define ConsolePrintf printf

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define ATLIB_RESPONSE_HANDLE_TIMEOUT   20000 /* ms */

#ifndef ATLIBGS_TX_CMD_MAX_SIZE
#error "ATLIBGS_TX_CMD_MAX_SIZE must be defined in platform.h"
#endif
#ifndef ATLIBGS_RX_CMD_MAX_SIZE
#error "ATLIBGS_RX_CMD_MAX_SIZE must be defined in platform.h"
#endif

#define ANSI_OFF "\033[0m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* Receive buffer to save async and response message from S2w App Node */
char MRBuffer[ATLIBGS_RX_CMD_MAX_SIZE];
uint16_t MRBufferIndex = 0;

/* Flag to indicate whether S2w Node is currently associated or not */
static uint8_t nodeAssociationFlag = false;
/* Flag to indicate whether S2w Node has rebooted after initialisation  */
static uint8_t nodeResetFlag = false;
typedef struct {
	uint8_t iState;
		#define CID_STATE_FREE			0
		#define CID_STATE_CONNECTED		1
		#define CID_STATE_DISCONNECTED	2
		#define CID_STATE_CONNECTING	3
	uint8_t iType;
		#define CID_TYPE_NONE		0
		#define CID_TYPE_TCP		1
		#define CID_TYPE_UDP		2
	uint8_t iIsServer;	// 1=server, 0=client

    // Which server created this connection?
    uint8_t server_cid;

    // Newly created cid for communication
    uint8_t cid;

    // IP number of incoming connection
    char ip[16];

    // Connection port (local allocated port number)
    uint16_t port;
} T_cidInfo ;
static T_cidInfo G_cidState[16];

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
void AtLibGs_FlushRxBuffer(void);

/*--------------------------- AT command list --------------------------------------------------------------------------
 _________________________________________________________________________________________________________________________
 AT Command                                                                   Description and AT command library API Name
 _________________________________________________________________________________________________________________________

 ATE[0|1]                                                                             Disable/enable echo
 API Name: AtLibGs_SetEcho

 AT&W[0|1]                                                                            Save Settings to profile 0/1
 API Name: AtLibGs_SaveProfile

 ATZ[0|1]                                                                             Load Settings from profile 0/1
 API Name: AtLibGs_LoadProfile

 AT+EXTPA=[1/0]                                                                       Enable/disable external PA.
 API Name: AtLibGs_EnableExternalPA

 AT+WSYNCINTRL=[interval]                                                             configure the sync loss interval in TBTT interval
 API Name: AtLibGs_SyncLossInterval

 AT+PSPOLLINTRL=[interval]                                                            configure the Ps poll interval in Seconds
 API Name: AtLibGs_PSPollInterval

 AT+BDATA=[0/1]                                                                       Bulk data reception enable/disable
 API Name: AtLibGs_BData

 AT&F                                                                                 Reset to factory defaults
 API Name:AtLibGs_ResetFactoryDefaults

 AT+WM=[0|1]                                                                          Set mode to Infrastructure (0) or ad-hoc (1)
 API Name: AtLibGs_Mode

 AT+WA=[SSID][,[BSSID],[Ch]]                                                      Associate to network or form ad-hoc network
 API Name: AtLibGs_Assoc

 AT+WD or ATH                                                                         Disassociate from current network
 API Name:AtLibGs_DisAssoc

 AT+WRSSI=?                                                                           Query the current RSSI value
 API Name: AtLibGs_GetRssi

 AT+WWPA=passphrase                                                                   Set WPA passphrase (8 - 63 chars)
 API Name: AtLibGs_SetPassPhrase

 AT+WRETRY=n                                                                          Set the wireless retry count
 API Name: AtLibGs_SetWRetryCount

 AT+WRXACTIVE=[0|1]                                                                   Enable/disable the radio
 API Name: AtLibGs_EnableRadio

 AT+WRXPS=[0|1]                                                                       Enable/disable 802.11 power save
 API Name: AtLibGs_EnablePwSave

 AT+WP=[power]                                                                        Set the transmit power
 API Name: AtLibGs_SetTxPower

 AT+NMAC=[?]/[mac]                                                                    Get/Set MAC address and store in flash
 API Name: AtLibGs_GetMAC / AtLibGs_MACSet

 AT+NDHCP=[0|1]                                                                       Disable/Enable DHCP
 API Name: AtLibGs_DHCPSet

 AT+NSET=[IP],[NetMask],[Gateway]                                                     Configure network address
 API Name: AtLibGs_IPSet

 AT+NCTCP=[IP],[Port]                                                                 Open TCP client
 API Name: AtLibGs_TCPClientStart

 AT+NCUDP=[IP],[RemotePort],[LocalPort]                                              Open UDP client
 API Name: AtLibGs_UDPClientStart

 AT+NSTCP=[Port]                                                                      Open TCP server on Port
 API Name: AtLibGs_TCPServer_Start

 AT+NSUDP=[Port]                                                                      Open UDP server on Port
 API Name: AtLibGs_UDPServer_Start

 AT+NCLOSE=cid                                                                        Close the specified connection
 API Name: AtLibGs_Close

 AT+NCLOSEALL                                                                         Close all connections
 API Name: AtLibGs_CloseAll

 AT+WAUTO=[Mode],[SSID],[bssid],[Channel]                                             Configure auto connect wireless settings
 API Name: AtLibGs_StoreWAutoConn

 AT+NAUTO=[Type],[Protocol],[DestIP],[DestPort]                                       Configure auto connection
 Client(0)/server(1), protocol UDP(0)/TCP(1),and host.
 API Name: AtLibGs_StoreNAutoConn

 ATC[0|1]                                                                             Disable/enable auto connection
 API Name: AtLibGs_EnableAutoConnect

 +++                                                                                  Switch to Command mode while in auto-connect mode
 API Name: AtLibGs_SwitchFromAutoToCmd

 ATSparam=value                                                                       Set configuration parameters
 Network connection timeout (0) / Auto associate timeout (1)
 TCP connection timeout (2) / Association retry count (3)
 Nagle algorithm wait time (4)
 API Name: AtLibGs_StoreATS

 AT+PSDPSLEEP                                                                         Enable deep sleep
 API Name: AtLibGs_EnableDeepSleep

 AT+PSSTBY=[n][,[delay time],[alarm1-pol],[alarm2-pol]]                               Standby request for n milliseconds
 API Name: AtLibGs_GotoSTNDBy

 AT+STORENWCONN                                                                       Store the nw context
 API Name: AtLibGs_StoreNwConn

 AT+RESTORENWCONN                                                                     Restore the nw context
 API Name: AtLibGs_ReStoreNwConn

 AT+FWUP=[SrvIp],[SrvPort],[SrcPort]                                                  Start FW Upgrade
 API Name: AtLibGs_FWUpgrade

 AT+WPAPSK=[SSID],[PassPhrase]                                                        Calculate and store the PSK
 API Name: AtLibGs_CalcNStorePSK

 AT+NSTAT=?                                                                           Display current network context
 API Name: AtLibGs_GetNetworkStatus

 AT+VER=?                                                                             Get the Version Info
 API Name: AtLibGs_Version

 AT+DNSLOOKUP=[URL],([RETRY],[TIMEOUT-S])                                             Get the ip from host name
 API Name: AtLibGs_DNSLookup

 AT+DNSSET=[DNS1-IP],[DNS2-IP]                                                      Set static DNS IP addresses
 API Name: AtLibGs_SetDNSServerIP

 AT+MCSTSET=[0/1]                                                                     enable/disable the multicast recv
 API Name: AtLibGs_MultiCast

 AT+BCHKSTRT=[Measure interval]                                                       Start the batt check
 API Name: AtLibGs_BatteryChkStart


 AT+BCHKSTOP                                                                          Stop the batt check
 API Name: AtLibGs_BatteryChkStop

 AT+ERRCOUNT=?                                                                        Get the error counts
 API Name: AtLibGs_GetErrCount

 AT+SETTIME=[dd/mm/yyyy],[HH:MM:SS]                                                   Set the system time
 API Name: AtLibGs_SetTime

 AT+WWPS=[1/2],[wps pin]                                                              Associate to an AP using WPS.
 1 - Push Button mathod.
 2 - PIN mathod. Provide [wps pin] only in case of PIN mathod
 API Name: AtLibGs_StartWPSPUSH / AtLibGs_StartWPSPIN


 AT&Y[0|1]                                                                            Set default power on profile to profile 0/1
 API Name: Not Available

 AT&V                                                                                 Output current configuration
 API Name: Not Available

 AT+WS[=[SSID](,[BSSID],[Ch],[ScanTime])                                         Perform wireless scan
 API Name: Not Available

 AT+WRATE=?                                                                           Query the current WiFi rate used
 API Name: Not Available

 AT+WWEPn=[key]                                                                       Set WEP key (10 or 26 hex digits) with index n (1-4)
 API Name: Not Available

 AT+WAUTH=[authmode>                                                                  Set authmode (1->open,2-]shared)
 API Name: Not Available

 AT+WSTATUS                                                                           Display current Wireless Status
 API Name: AtLibGs_GetNetworkConfiguration

 AT+NMAC2=[?]/[mac]                                                                   Get/Set MAC address and store in RTC
 API Name: Not Available

 AT+SETSOCKOPT=[cid],[type],[parameter],[value],[length]                               Set options of a socket specified by cid
 API Name: Not Available

 ATA                                                                                  Initiate AutoConnect
 API Name: Not available

 ATA2                                                                                 Initiate AutoConnect-tcp/udp level
 API Name: Not available

 ATO                                                                                  Return to Auto Data mode
 API Name: Not available

 ATI[n]                                                                               Display identification number n's text
 API Name: AtLibGs_GetInfo

 AT+WPSK=[PSK]                                                                        Store the PSK
 API Name: Not available

 AT+CID=?                                                                             Display The CID info
 API Name: Not available

 AT+BCHK=[?]/[Measure interval]                                                       Get/Set batt check param
 API Name: Not available

 AT+BATTVALGET                                                                        Get the latest battery value stored in RTC
 API Name: Not available


 AT+BATTLVLSET=[Warning Level],[warning Freq],[Standby Level]                         Set batt warning level, frequency of reporting warning
 and batt standby levl
 API Name: Not available

 AT+PING=[Ip],[Trails],[Interval],[Len],[TOS],[TTL],[PAYLAOD(16 Bytes)]               Starts Ping
 API Name: Not available

 AT+TRACEROUTE=[Ip],[Interval],[MaxHops],[MinHops],[TOS]                              Starts Trace route
 API Name: Not available


 AT+GETTIME=?                                                                         Get the system time in Milli-seconds since Epoch(1970)
 API Name: Not available

 AT+DGPIO=[GPIO_PIN],[1-SET/0-RESET]                                                  Set/reset a gpio pin
 API Name: Not available

 AT+TCERTADD=[name],[format],[size],[location]\n\r[ESC]W[data of size above]          Provisions a certificate.
 format-binary/ascii(0/1),location-FLASH/RAM.
 Follow the escape sequence to send data.
 API Name: Not available

 AT+TCERTDEL=[name]                                                                   Delete a certificate
 API Name: Not available


 AT+WEAPCONF=[outer authtype],[inner authtype],[user name],[password]                 Configure auth type,user name and password for EAP
 API Name: Not available

 AT+WEAP=[type],[format],[size],[location]\n\r[ESC]W[data of size above]              Provision certificate for EAP TLS.
 Type-CA/CLIENT/PUB_KEY(0/1/2),
 format-binary/ascii(0/1),location- flash/RAM(0/1).
 Follow the escape sequence to send data.
 API Name: Not available

 AT+SSLOPEN=[cid],[name]                                                              Opens a ssl connection. name-Name of certificate to use
 API Name: Not available

 AT+SSLCLOSE=[cid]                                                                    Close a SSL connection
 API Name: Not available

 AT+HTTPOPEN=[hostName/ip addr],[port],[secured/non secured],[certificate name]     Opens a http/https connection
 API Name: Not available

 AT+HTTPCLOSE=[cid]                                                                   Closes a http connection
 API Name: Not available
 AT+HTTPSEND=[cid],[Method],[TimeOut],[Page],[Size]\n\r[ESC]H[data of size above]
 Send a Get or POST request.Method- GET/HEAD/POST(1/2/3)
 Follow the escape sequence to send data.
 API Name: Not available

 AT+HTTPCONF=[Param],[Value]                                                          Configures http parameters.
 API Name: Not available

 AT+WEBPROV=[user name],[passwd],[ip addr],[subnet mask],[gateway]                      start web server. username passwd are used for authentication
 The server is atarted with the given ip addr, subnetmask
 and gateway
 API Name: Not available

 AT+WEBPROV=[user name],[passwd]                                                       start web server. username passwd are used for authentication
 API Name: Not available

 AT+WEBLOGOADD=[size>                                                                  add webserver logo of size <size]. After issuing
 the command, send [esc] followed by l/L   and
 send the content of the logo file
 API Name: Not available

 AT+NRAW=[0/1/2]                                                                      Enable Raw Packet transmission.
 API Name: Not available

 ATV[0|1]                                                                             Disable/enable verbose responses
 API Name: Not Available
 _________________________________________________________________________________________________________________________*/

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Check
 *--------------------------------------------------------------------------*/
/**   Send command:
 *          AT
 *      and wait for response.
 *  @param [in] timeout -- timeout for check to see if module present and ready
 *  @return     ATLIBGS_MSG_ID_E -- response type
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Check(uint32_t timeout)
{
    uint32_t start = MSTimerGet();
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
    uint8_t rxData;

    // Clear out the buffers
    AtLibGs_FlushRxBuffer();
    
    /* Send the command */
    App_Write("AT\r\n", 4);
    MSTimerDelay(100);
    
    /* Wait for OK or a timeout */
    /* Now process the response from S2w App node */
    while (ATLIBGS_MSG_ID_OK != rxMsgId) {
        /* Timeout? */
        if (MSTimerDelta(start) >= timeout) {
            rxMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
            break;
        }
        /* Got a character? Then process it */
        if (App_Read(&rxData, 1, 0)) {
            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if(rxMsgId > ATLIBGS_MSG_ID_OK){
                /* Resend the command if it didn't understand the first one*/
                App_Write("AT\r\n", 4);
                MSTimerDelay(100);
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEcho
 *--------------------------------------------------------------------------*/
/**   Send command to turn on or off the character echo:
 *          ATE[0|1]
 *      and wait for response.
 *  @param [in] mode -- 1=to turn on echo, 0=turn off echo
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEcho(uint8_t mode)
{
    char cmd[10];

    sprintf(cmd, "ATE" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetMAC
 *--------------------------------------------------------------------------*/
/**   Set the network MAC address for this module in flash.
 *      Sends the command:
 *          AT+NMAC=[mac address]
 *      and waits for a response.
 *      [mac address] is the format "00:11:22:33:44:55"
 *  @param [in] pAddr -- max address pointer
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetMAC(char *pAddr)
{
    char cmd[30];

    sprintf(cmd, "AT+NMAC=%s\r\n", pAddr);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetMAC2
 *--------------------------------------------------------------------------*/
/**   Set the network MAC2 address for this module in non-volatile RAM.
 *      Sends the command:
 *          AT+NMAC=[mac address]
 *      and waits for a response.
 *      [mac address] is the format "00:11:22:33:44:55"
 *  @param [in] pAddr -- max address pointer
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetMAC2(char *pAddr)
{
    char cmd[30];

    sprintf(cmd, "AT+NMAC2=%s\r\n", pAddr);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetMAC
 *--------------------------------------------------------------------------*/
/**   Get the network MAC address for this module.
 *      Sends the command:
 *          AT+NMAC=?
 *      and waits for a response.
 *  @param [in] mac -- returned mac found (if any)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetMAC(char *mac)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1] = { 0 };

    rxMsgId = AtLibGs_CommandSendString("AT+NMAC=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1))
        strcpy(mac, lines[0]);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetMAC2
 *--------------------------------------------------------------------------*/
/**   Get the network MAC address for this module.
 *      Sends the command:
 *          AT+NMAC=?
 *      and waits for a response.
 *  @param [in] mac -- returned mac found (if any)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetMAC2(char *mac)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1] = { 0 };

    rxMsgId = AtLibGs_CommandSendString("AT+NMAC2=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1))
        strcpy(mac, lines[0]);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_NCM_AutoConfig
 *--------------------------------------------------------------------------*/
/**   Used to setup the NCM. Use this to setup roaming mode support.
 *    See the GS1100/1500 manual for NCMAUTOCONF for details on usage.
 *      Sends the command:
 *          AT+NCMAUTOCONF=[id],[value]
 *      and waits for a response.
 *  @param [in] id -- parameter ID
 *  @param [in] value -- value to set parameter
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_NCM_AutoConfig(uint32_t id, int32_t value)
{
    char cmd[32];

    sprintf(cmd, "AT+NCMAUTOCONF=" _F32_ "," _F32_ "\r\n", id, value);
 
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CalcNStorePSK
 *--------------------------------------------------------------------------*/
/**   Get the network MAC address for this module.
 *      Sends the command:
 *          AT+WPAPSK=[SSID],[PassPhrase]
 *      and waits for a response.
 *  @param [in] pSsid -- SSID (1 to 32 characters)
 *  @param [in] pPsk -- Pass phrase (8 to 63 characters)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CalcNStorePSK(char *pSsid, char *pPsk)
{
    char cmd[50];

    sprintf(cmd, "AT+WPAPSK=%s,%s\r\n", pSsid, pPsk);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WlanConnStat
 *--------------------------------------------------------------------------*/
/**   Get the network MAC address for this module.
 *      Sends the command:
 *          AT+NSTAT=?
 *      and waits for a response.
 *      Use the routine AtLibGs_ParseNodeIpAddress() or
 *      AtLibGs_ParseRssiResponse() to parse the returned data for RSSI or
 *      AtLibGs_ParseWlanConnStat() to parse for the wireless connection status.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WlanConnStat(void)
{
    return AtLibGs_CommandSendString("AT+NSTAT=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DHCPSet
 *--------------------------------------------------------------------------*/
/**   Enable or disable DHCP client.
 *      Sends the command:
 *          AT+NDHCP=[0|1]
 *      and waits for a response.
 *  @param [in] mode -- 0=disable DHCP, 1=enable DHCP
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DHCPSet(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+NDHCP=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Assoc
 *--------------------------------------------------------------------------*/
/**   Associate to network or form ad-hoc network
 *      Sends the command:
 *          AT+WA=[SSID](,[BSSID],[Ch])
 *      and waits for a response.
 *  @param [in] pSsid -- SSID to connect to (1 to 32 characters)
 *  @param [in] pBssid -- Ad-hoc network id, or 0 for none
 *  @param [in] channel -- Channel of network, 0 for any
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Assoc(const char *pSsid, const char *pBssid, uint8_t channel)
{
    char cmd[100];

    if (channel) {
        sprintf(cmd, "AT+WA=%s,%s," _F8_ "\r\n", pSsid, (pBssid) ? pBssid : "",
                channel);
    } else {
        sprintf(cmd, "AT+WA=%s\r\n", pSsid);
    }

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TCPClientStart
 *--------------------------------------------------------------------------*/
/**   Open TCP client connection.
 *      Sends the command:
 *          AT+NCTCP=[IP],[Port]
 *      and waits for a response.
 *      [IP] is in the format "12:34:56:78"
 *      [Port] is the number of the port connection.
 *  @param [in] pRemoteTcpSrvIp -- IP address string in format 12:34:56:78
 *  @param [in] pRemoteTcpSrvPort -- Port string
 *  @param [in] cid -- CID of client connection opened
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TCPClientStart(
        char *pRemoteTcpSrvIp,
        int16_t pRemoteTcpSrvPort,
        uint8_t *cid)
{
    char cmd[80];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *result;
	  int cidIndex;

    sprintf(cmd, "AT+NCTCP=%s," _F16_ "\r\n", pRemoteTcpSrvIp,
            pRemoteTcpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((result = strstr(MRBuffer, "CONNECT")) != NULL) {
            /* Succesful connection done for TCP client */
            *cid = result[8];
            cidIndex = AtLibGs_CIDToIndex(*cid);
            G_cidState[cidIndex].iState = CID_STATE_CONNECTED;
            G_cidState[cidIndex].iType = CID_TYPE_TCP;
            G_cidState[cidIndex].iIsServer = 0;
        } else {
            /* Not able to extract the CID */
            *cid = ATLIBGS_INVALID_CID;
            rxMsgId = ATLIBGS_MSG_ID_ERROR;
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UDPClientStart
 *--------------------------------------------------------------------------*/
/**   Open UDP client
 *      Sends the command:
 *          AT+NCUDP=[IP],[RemotePort],[LocalPort]
 *      and waits for a response.
 *      [IP] is in the format "12:34:56:78"
 *      [RemotePort] is the number of the port connection.
 *      [LocalPort] is the port on this machine.
 *      Use AtLibGs_ParseUDPClientCid() afterward to parse the response.
 *  @param [in] pRemoteUdpSrvIp -- IP address string in format 12:34:56:78
 *  @param [in] pRemoteUdpSrvPort -- Remote connection port string
 *  @param [in] pUdpLocalPort -- Local port string
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UDPClientStart(
        char *pRemoteUdpSrvIp,
        char *pRemoteUdpSrvPort,
        char *pUdpLocalPort)
{
    char cmd[80];

    sprintf(cmd, "AT+NCUDP=%s,%s,%s\r\n", pRemoteUdpSrvIp, pRemoteUdpSrvPort,
            pUdpLocalPort);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CloseAll
 *--------------------------------------------------------------------------*/
/**   Close all open connections.
 *      Sends the command:
 *          AT+NCLOSEALL
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CloseAll(void)
{
    return AtLibGs_CommandSendString("AT+NCLOSEALL\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryChkStart
 *--------------------------------------------------------------------------*/
/**   Start the batt check.
 *      Sends the command:
 *          AT+BCHKSTRT=[Measure interval]
 *      and waits for a response.
 *  @param [in] interval -- Interval of 1..100
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStart(uint16_t interval)
{
    char cmd[20];

    sprintf(cmd, "AT+BCHKSTRT=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GotoSTNDBy
 *--------------------------------------------------------------------------*/
/**   Start the batt check.
 *      Sends the command:
 *          AT+PSSTBY=x(,[DELAY TIME],[ALARM1 POL],[ALARM2 POL])
 *      and waits for a response.
 *  @param [in] nsec -- Standby time in milliseconds
 *  @param [in] delay -- Delay time in milliseconds before going into
 *                      standby.
 *  @param [in] alarm1_Pol -- polarity of pin 31 that will trigger an alarm
 *  @param [in] alarm2_Pol -- polarity of pin 36 that will trigger an alarm
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GotoSTNDBy(
        char *nsec,
        uint32_t delay,
        uint8_t alarm1_Pol,
        uint8_t alarm2_Pol)
{
    char cmd[100];

    sprintf(cmd, "AT+PSSTBY=%s," _F32_ "," _F8_ "," _F8_ "\r\n", nsec, delay,
            alarm1_Pol, alarm2_Pol);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetPassPhrase
 *--------------------------------------------------------------------------*/
/**   Set WPA passphrase (8 - 63 chars).
 *      Sends the command:
 *          AT+WWPA=[passphrase]
 *      and waits for a response.
 *  @param [in] pPhrase -- Passphrase string (8 - 63 characters)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetPassPhrase(const char *pPhrase)
{
    char cmd[50];

    sprintf(cmd, "AT+WWPA=%s\r\n", pPhrase);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetPassPhrase
 *--------------------------------------------------------------------------*/
/**   Enable deep sleep.
 *      Sends the command:
 *          AT+PSDPSLEEP
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDeepSleep(void)
{
    return AtLibGs_CommandSendString("AT+PSDPSLEEP\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreNwConn
 *--------------------------------------------------------------------------*/
/**   Store the network context.
 *      Sends the command:
 *          AT+STORENWCONN
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreNwConn(void)
{
    return AtLibGs_CommandSendString("AT+STORENWCONN\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReStoreNwConn
 *--------------------------------------------------------------------------*/
/**   Restore the network context.
 *      Sends the command:
 *          AT+STORENWCONN
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReStoreNwConn(void)
{
    return AtLibGs_CommandSendString("AT+RESTORENWCONN\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IPSet
 *--------------------------------------------------------------------------*/
/**   Configure network IP address.
 *      Sends the command:
 *          AT+NSET=[IP],[NetMask],[Gateway]
 *      and waits for a response.
 *      [IP] is in the format ##:##:##:##
 *      [Netmask] is in the format ##:##:##:##
 *      [Gateway] is in the format ##:##:##:##
 *  @param [in] pIpAddr -- IP Address string
 *  @param [in] pSubnet -- Subnet mask string
 *  @param [in] pGateway -- Gateway string
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_IPSet(char *pIpAddr, char *pSubnet, char *pGateway)
{
    char cmd[60];

    sprintf(cmd, "AT+NSET=%s,%s,%s\r\n", pIpAddr, pSubnet, pGateway);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SaveProfile
 *--------------------------------------------------------------------------*/
/**   Save Settings to profile 0 or 1.
 *      Sends the command:
 *          AT&W[0|1]
 *      and waits for a response.
 *  @param [in] profile -- profile 0 or 1
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SaveProfile(uint8_t profile)
{
    char cmd[20];

    sprintf(cmd, "AT&W" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_LoadProfile
 *--------------------------------------------------------------------------*/
/**   Load Settings from profile 0 or 1.
 *      Sends the command:
 *          ATZ[0|1]
 *      and waits for a response.
 *  @param [in] profile -- profile 0 or 1
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_LoadProfile(uint8_t profile)
{
    char cmd[10];

    sprintf(cmd, "ATZ" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ResetFactoryDefaults
 *--------------------------------------------------------------------------*/
/**   Reset factory defaults.
 *      Sends the command:
 *          AT&F
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ResetFactoryDefaults(void)
{
    return AtLibGs_CommandSendString("AT&F\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetRssi
 *--------------------------------------------------------------------------*/
/**   Query the current RSSI value.
 *      Sends the command:
 *          AT+WRSSI=?
 *      and waits for a response.
 *      Call AtLibGs_ParseRssiResponse() to get the final result.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetRssi(void)
{
    return AtLibGs_CommandSendString("AT+WRSSI=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisAssoc
 *--------------------------------------------------------------------------*/
/**   Disassociate from current network.
 *      Sends the command:
 *          AT+WD
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc(void)
{
    return AtLibGs_CommandSendString("AT+WD\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FWUpgrade
 *--------------------------------------------------------------------------*/
/**   Initiate a firmware update to the given server.
 *      Sends the command:
 *          AT+FWUP=[SrvIp],[SrvPort],[SrcPort]
 *      and waits for a response.
 *  @param [in] pSrvip -- IP address of the firmware upgrade server string
 *  @param [in] srvport -- server port number to be used for firmware
 *                      upgrade string
 *  @param [in] srcPort -- adapter port number to be used for firmware
 *                        upgrade.
 *  @param [in] pSrcIP -- Retry is the number of times the node will repeat
 *                       the firmware upgrade attempt if failures
 *                          are encountered.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_FWUpgrade(
        char *pSrvip,
        uint16_t srvport,
        uint16_t srcPort,
        char *pSrcIP)
{
    char cmd[80];

    sprintf(cmd, "AT+FWUP=%s," _F16_ "," _F16_ ",%s\r\n", pSrvip, srvport,
            srcPort, pSrcIP);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryChkStop
 *--------------------------------------------------------------------------*/
/**   Stop the battery check.
 *      Sends the command:
 *          AT+BCHKSTOP
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStop(void)
{
    return AtLibGs_CommandSendString("AT+BCHKSTOP\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_MultiCast
 *--------------------------------------------------------------------------*/
/**   Enable or disable multicast reception.
 *      Sends the command:
 *          AT+MCSTSET=[0|1]
 *      and waits for a response.
 *  @param [in] mode -- 0=disable multicast reception,
 *                      1=enable multicast reception
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_MultiCast(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+MCSTSET=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Version
 *--------------------------------------------------------------------------*/
/**   Get the Version Info.
 *      Sends the command:
 *          AT+VER=?
 *      and waits for a response.
 *
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Version(void)
{
    return AtLibGs_CommandSendString("AT+VER=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Mode
 *--------------------------------------------------------------------------*/
/**   Set mode to Infrastructure or Ad-Hoc
 *      Sends the command:
 *          AT+WM=[0|1|2]
 *      and waits for a response.
 *  @param [in] mode -- Mode of station operation
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Mode(ATLIBGS_STATIONMODE_E mode)
{
    char cmd[30];

    sprintf(cmd, "AT+WM=" _F16_ "\r\n", (uint16_t)mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UDPServer_Start
 *--------------------------------------------------------------------------*/
/**   Open UDP server on given port.
 *      Sends the command:
 *          AT+NSUDP=[Port]
 *      and waits for a response.
 *      The [Port] for the UDP Server to receive packets.
 *      Call AtLibGs_ParseUDPServerStartResponse() to parse the response
 *      into a connection id.
 *  @param [in] pUdpSrvPort -- Port on server string.
 *  @param [in] cid -- Pointer to returned CID for this session.
 *  @return     ATLIBGS_MSG_ID_E -- error code, ATLIBGS_MSG_ID_OK if CID found.
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UDPServer_Start(uint16_t pUdpSrvPort, uint8_t *cid)
{
    char cmd[20];
    char * result = NULL;
    ATLIBGS_MSG_ID_E rxMsgId;

    sprintf(cmd, "AT+NSUDP=" _F16_ "\r\n", pUdpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((result = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
        	int cidIndex;

            *cid = result[8];
            cidIndex = AtLibGs_CIDToIndex(*cid);
            G_cidState[cidIndex].iState = CID_STATE_CONNECTED;
            G_cidState[cidIndex].iType = CID_TYPE_UDP;
            G_cidState[cidIndex].iIsServer = 0;
        } else if (((result = strstr((const char *)MRBuffer, "DISASSOCIATED"))
                != NULL) || (strstr((const char *)MRBuffer, "SOCKET FAILURE")
                != NULL)) {
            /* Failed  */
            return ATLIBGS_MSG_ID_DISCONNECT;
        } else {
            /* Failed  */
            return ATLIBGS_MSG_ID_ERROR;
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TCPServer_Start
 *--------------------------------------------------------------------------*/
/**   Set mode to Infrastructure or Ad-Hoc
 *      Sends the command:
 *          AT+NSTCP=[Port]
 *      and waits for a response.
 *      The [Port] for the TCP Server to receive packets.
 *      Call AtLibGs_ParseTcpServerStartResponse() to parse the response
 *      into a connection id.
 *  @param [in] pTcpSrvPort -- TCP port string
 *  @param [in] cid -- Connection ID returned
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TCPServer_Start(uint16_t pTcpSrvPort, uint8_t *cid)
{
    char cmd[20];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *pSubStr = NULL;

    sprintf(cmd, "AT+NSTCP=" _F16_ "\r\n", pTcpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);

    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((pSubStr = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
        	int cidIndex;

            *cid = pSubStr[8];
            cidIndex = AtLibGs_CIDToIndex(*cid);
            G_cidState[cidIndex].iState = CID_STATE_CONNECTED;
            G_cidState[cidIndex].iType = CID_TYPE_TCP;
            G_cidState[cidIndex].iIsServer = 1;
        } else if (((strstr((const char *)MRBuffer, "DISASSOCIATED")) != NULL)
                || (strstr((const char *)MRBuffer, "SOCKET FAILURE") != NULL)) {
            /* Failed  */
            return ATLIBGS_MSG_ID_DISCONNECT;
        } else {
            /* Failed  */
            return ATLIBGS_MSG_ID_ERROR;
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DNSLookup
 *--------------------------------------------------------------------------*/
/**   Get the IP number from host name.
 *      Sends the command:
 *          AT+DNSLOOKUP=[name]
 *      and waits for a response.
 *      [name] is the URL of the address to lookup.
 *      Call AtLibGs_ParseDNSLookupResponse() to parse the response.
 *      TODO: Needs to return the parsed lookup or an error.
 *  @param [in] pUrl -- URL to parse.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DNSLookup(const char *pUrl)
{
    char cmd[30];

    sprintf(cmd, "AT+DNSLOOKUP=%s\r\n", pUrl);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Close
 *--------------------------------------------------------------------------*/
/**   Close a previously opened connection
 *      Sends the command:
 *          AT+NCLOSE=[cid]
 *      and waits for a response.
 *      [cid] is the connection id
 *  @param [in] cid -- Connection id
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Close(uint8_t cid)
{
    char cmd[20];

#ifdef ATLIBGS_DEBUG_ENABLE
	printf("Request closing of %c\n", cid);
#endif	
    sprintf(cmd, "AT+NCLOSE=%c\r\n", cid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWRetryCount
 *--------------------------------------------------------------------------*/
/**   Set the wireless retry count.
 *      Sends the command:
 *          AT+WRETRY=[n]
 *      and waits for a response.
 *      [n] is the number of retries (default is 5)
 *  @param [in] count -- Number of wireless retries
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWRetryCount(uint16_t count)
{
    char cmd[20];

    sprintf(cmd, "AT+WRETRY=" _F16_ "\r\n", count);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetErrCount
 *--------------------------------------------------------------------------*/
/**   Get the error counts
 *      Sends the command:
 *          AT+ERRCOUNT=?
 *      and waits for a response.
 *  @param [in] errorCount -- Number of errors
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetErrCount(uint32_t *errorCount)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1] = { 0 };

    rxMsgId = AtLibGs_CommandSendString("AT+ERRCOUNT=?\r\n");
    /* TODO: Parse commands here! */
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1)) {
        sscanf(lines[0], _F32_, errorCount);
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableRadio
 *--------------------------------------------------------------------------*/
/**   Enable or disable the radio.
 *      Sends the command:
 *          AT+WRXACTIVE=[mode]
 *      and waits for a response.
 *      [mode] is 0 when disabling the radio, 1 when enabled the radio.
 *  @param [in] mode -- 0 to disable the radio, 1 to enable the radio
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableRadio(uint8_t mode)
{
    char cmd[30];

    sprintf(cmd, "AT+WRXACTIVE=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnablePwSave
 *--------------------------------------------------------------------------*/
/**   Enable or disable the 802.11 power save mode.
 *      Sends the command:
 *          AT+WRXPS=[mode]
 *      and waits for a response.
 *      [mode] is 0 to disable power save mode, 1 to enable power save.
 *          The default is enabled.
 *  @param [in] mode -- 0 to disabe power save mode, 1 to enable
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnablePwSave(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+WRXPS=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTime
 *--------------------------------------------------------------------------*/
/**   Set the system time.
 *      Sends the command:
 *          AT+SETTIME=[date],[time]
 *      and waits for a response.
 *      [date] is in the format "dd/mm/yyyy"
 *      [time] is in the format "HH:MM:SS"
 *  @param [in] pDate -- Date string in format "dd/mm/yyyy"
 *  @param [in] pTime -- Time string in format "HH:MM:SS"
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTime(char *pDate, char *pTime)
{
    char cmd[50];

    sprintf(cmd, "AT+SETTIME=%s,%s\r\n", pDate, pTime);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableExternalPA
 *--------------------------------------------------------------------------*/
/**   This command enables or disables the external PA (patch antenna).
 *      Sends the command:
 *          AT+EXTPA=[mode]
 *      and waits for a response.
 *      [mode] is 1 to enable the external PA, or 0 to disable external PA.
 *      NOTE: If enabled, this command forces the adapter to standby and
 *          comes back immediately causing all configured parameters and
 *          network connections to be lost.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableExternalPA(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+EXTPA=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SyncLossInterval
 *--------------------------------------------------------------------------*/
/**   Configure the sync loss interval in TBTT interval.
 *      Sends the command:
 *          AT+WSYNCINTRL=[interval]
 *      and waits for a response.
 *      [interval] is the sync loss interval with a range of 1 to 65535.
 *          Default value is 30.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SyncLossInterval(uint16_t interval)
{
    char cmd[30];

    sprintf(cmd, "AT+WSYNCINTRL=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_PSPollInterval
 *--------------------------------------------------------------------------*/
/**   Configure the Association Keep Alive Timer (PS) poll interval in
 *      seconds.
 *      Sends the command:
 *          AT+PSPOLLINTRL=[interval]
 *      and waits for a response.
 *      [interval] is the number of seconds to keep alive.  Default is 45.
 *          Range is 0 to 65535 seconds.  A value of 0 disables.
 *  @param [in] interval
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_PSPollInterval(uint16_t interval)
{
    char cmd[30];

    sprintf(cmd, "AT+PSPOLLINTRL=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTxPower
 *--------------------------------------------------------------------------*/
/**   Set the transmit power.
 *      Sends the command:
 *          AT+WP=[power]
 *      and waits for a response.
 *      [power] is the transmit power.
 *      Transmit power can be 0 to 7 for internal PA GS101x with a default of
 *      0.  Transmit power can be 2 to 15 for external PA GS101x with a
 *      default power of 2.
 *  @param [in] power -- Power level (see above notes)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTxPower(uint8_t power)
{
    char cmd[20];

    sprintf(cmd, "AT+WP=" _F8_ "\r\n", power);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetDNSServerIP
 *--------------------------------------------------------------------------*/
/**   Set static DNS IP address(es).
 *      Sends the command:
 *          AT+WSYNCINTRL=[dns1](,[dns2])
 *      and waits for a response.
 *      [dns1] is the primary DNS address in "##.##.##.##" format.
 *      [dns2] is the optional secondary DNS address in "##.##.##.##" format.
 *  @param [in] pDNS1 -- Primary DNS address string in "##.##.##.##" format.
 *  @param [in] pDNS2 -- Secondary DNS address string in "##.##.##.##" format.
 *                       If NULL, no secondary DNS address is used.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetDNSServerIP(char *pDNS1, char *pDNS2)
{
    char cmd[30];

    if (pDNS2 == NULL) {
        sprintf(cmd, "AT+DNSSET=%s", pDNS1);
    } else {
        sprintf(cmd, "AT+DNSSET=%s,%s", pDNS1, pDNS2);
    }

    return AtLibGs_CommandSendString(cmd);

}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableAutoConnect
 *--------------------------------------------------------------------------*/
/**   Disable or enable auto connection.
 *      Sends the command:
 *          AT+ATC[mode]
 *      and waits for a response.
 *      NOTE: The resulting change only takes effect on the next reboot or
 *          an ATA command.
 *  @param [in]  mode -- 0 to disable auto connect, 1 to enable auto connect.
 *                       Default is disabled.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableAutoConnect(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "ATC" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SwitchFromAutoToCmd
 *--------------------------------------------------------------------------*/
/**   Sends the command:
 *          AT+WSYNCINTRL=[interval]
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_SwitchFromAutoToCmd(void)
{
    App_Write("+++", 3);
    App_DelayMS(1000);
    App_Write("\r\n", 2);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreWAutoConn
 *--------------------------------------------------------------------------*/
/**   Set the auto connection wireless parameters for the current profile.
 *      Sends the command:
 *          AT+WSYNCINTRL=[interval]
 *      and waits for a response.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreWAutoConn(char * pSsid, uint8_t channel)
{
    char cmd[50];

    sprintf(cmd, "AT+WAUTO=0,%s,," _F8_ "\r\n", pSsid, channel);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreNAutoConn
 *--------------------------------------------------------------------------*/
/**   Configure auto connection network parameters (for client connection).
 *      Sends the command:
 *          AT+NAUTO=0,0,[ip addr],[port]
 *      and waits for a response.
 *      [ip addr] is the IP address of the remote server
 *      [port] is the port address of the remote server
 *  @param [in] pIpAddr -- IP address string in format "##.##.##.##"
 *  @param [in] pRmtPort -- Port address of remote system
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreNAutoConn(char *pIpAddr, int16_t pRmtPort)
{
    char cmd[50];

    sprintf(cmd, "AT+NAUTO=0,0,%s," _F16_ "\r\n", pIpAddr, pRmtPort);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreATS
 *--------------------------------------------------------------------------*/
/**   Store network and configuration parameters.  See documentation
 *      for configuration parameters.
 *          0=Network Connection Timeout
 *              Maximum time to establish auto connection in 10 ms
 *              intervals.  Default is 1000 or 10 seconds.
 *          1=Auto Associate Timeout
 *              Maximum time to associate to desired wireless network in
 *              10 ms intervals.  Default is 1000 or 10 seconds.
 *          2=TCP Connection Timeout
 *              Maximum time to establish TCP client connection in
 *              10 ms intervals.  Default is 500 or 5 seconds.
 *          3=Association Retry Count
 *              Not currently supported
 *          4=Nagle Algorithm Wait Time
 *              Maximum time for serial data sent in Auto Connect Mode
 *              to be buffered in 10 ms intervals.  Default is 10 or 100 ms.
 *          5=Scan Time
 *              Maximum time for scanning in one radio chanenel in
 *              milliseconds.  Default is 20 or 20 ms.
 *      Sends the command:
 *          AT+ATS[param]=[time]
 *      and waits for a response.
 *      [param] is the numeric parameter from the above.
 *      [time> is the value to be used for the given <param] (see above).
 *  @param [in] param -- Timing parameter to set
 *  @param [in] value -- Timing value to use
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreATS(uint8_t param, uint8_t value)
{
    char cmd[30];

    sprintf(cmd, "ATS" _F8_ "=" _F8_ "\r\n", param, value);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BData
 *--------------------------------------------------------------------------*/
/**   Enable or disable bulk data reception.
 *      Sends the command:
 *          AT+BDATA=[mode]
 *      and waits for a response.
 *      [mode] is 1 to enable, or 0 to disable.
 *  @param [in] mode -- 0 to disable bulk data reception, or 1 to enable.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BData(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+BDATA=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseWPSResult
 *--------------------------------------------------------------------------*/
/**   This internal helper function takes the response from WPS
 *      (either button or pin) and parses the data into the fields.
 *  @param [in] result -- Resulting WPS answer, if any
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
static void AtLibGs_ParseWPSResult(AtLibGs_WPSResult *result)
{
    char * lines[3];
    char * tokens[2];

    // Clear the result
    memset((uint8_t *) result, 0, sizeof(result));

    // Parse the items and stuff them into the structure
    if (AtLibGs_ParseIntoLines(MRBuffer, lines, 3) >= 3) {
        if (AtLibGs_ParseIntoTokens(lines[0], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "SSID") == 0)
                strcpy(result->ssid, tokens[1]);
        }

        if (AtLibGs_ParseIntoTokens(lines[1], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "CHANNEL") == 0)
                result->channel = atoi(tokens[1]);
        }

        if (AtLibGs_ParseIntoTokens(lines[2], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "PASSPHRASE") == 0)
                strcpy(result->password, tokens[1]);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartWPSPUSH
 *--------------------------------------------------------------------------*/
/**   Associate to an access point (AP) using Wi-Fi Protected Setup (WPS)
 *      using the push button method.
 *      Sends the command:
 *          AT+WWPS=1
 *      and waits for a response.
 *      The network then connects to the network.
 *  @param [in] result -- Resulting WPS answer, if any
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartWPSPUSH(AtLibGs_WPSResult *result)
{
    ATLIBGS_MSG_ID_E rxMsgId;

    /* send pushbutton command */
    rxMsgId = AtLibGs_CommandSendString("AT+WWPS=1\r\n");

    /* wait for valid responce then parse the ssid, channel, and passphrase */
    /* might be 20 seconds from buton push till response */
    if (rxMsgId == ATLIBGS_MSG_ID_OK)
        AtLibGs_ParseWPSResult(result);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartWPSPIN
 *--------------------------------------------------------------------------*/
/**   Associate to an access point (AP) using Wi-Fi Protected Setup (WPS)
 *          using the Personal Identification Number (PIN) method.
 *      Sends the command:
 *          AT+WWPS=2,[pin]
 *      and waits for a response.
 *      [pin] is a unique PIN.
 *      AtLibGs_WPSResult *result -- Resulting WPS answer, if any
 *  @param [in] pin -- PIN string to pass in when doing WPS
 *  @param [in] result -- Resulting WPS answer, if any
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartWPSPIN(char *pin, AtLibGs_WPSResult *result)
{
    char cmd[50];
    ATLIBGS_MSG_ID_E rxMsgId;

    sprintf(cmd, "AT+WWPS=2,%s", pin);
    rxMsgId = AtLibGs_CommandSendString(cmd);

    /* wait for valid responce then parse the ssid, channel, and passphrase */
    if (rxMsgId == ATLIBGS_MSG_ID_OK)
        AtLibGs_ParseWPSResult(result);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIntoLines
 *--------------------------------------------------------------------------*/
/**   Convert the given text into a list of pointers to the
 *      lines in that text and null terminate each line.
 *  @param [in] text -- Pointer to text to parse into lines
 *  @param [in] pLines[] -- Pointer to an array of lines
 *  @param [in] maxLines -- Maximum number of lines to return
 *  @return     uint16_t -- Number of lines found (up to maximum number)
 */
/*--------------------------------------------------------------------------*/
uint16_t AtLibGs_ParseIntoLines(char *text, char *pLines[], uint16_t maxLines)
{
    char *p = text;
    uint8_t c;
    int mode = 0;
    uint16_t numLines = 0;

    /* Walk through all the characters and determine where the lines are */
    while ((*p) && (numLines < maxLines)) {
        c = *p;
        switch (mode) {
            case 0: /* looking for start */
                /* Skip any line feeds -- we start a line with a \n */
                if (c == '\n')
                    break;
                pLines[numLines] = (char *)p;
                mode = 1;
                /* Fall into looking for a \n character immediately */
                /* no break */
            case 1:
                /* Did we find the end of the line? */
                if ((c == '\n') || (c == '\r')) {
                    /* Null terminate the line and go to the next line */
                    *p = '\0';
                    numLines++;
                    mode = 0;
                } else {
                    /* looking at normal characters here */
                }
                break;
        }
        p++;
    }

    return numLines;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIntoTokens
 *--------------------------------------------------------------------------*/
/**   Converts a line into a list of tokens.  Skips spaces before a token
 *      starts.
 *  @param [in] line[] -- Pointer to an array of lines
 *  @param [in] deliminator -- Character that separates the fields
 *  @param [in] tokens -- Pointer to an array of tokens
 *  @param [in] maxTokens -- max number of tokens
 *  @return     uint16_t -- Number of lines found (up to maximum number)
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseIntoTokens(
        char *line,
        char deliminator,
        char *tokens[],
        uint8_t maxTokens)
{
    char *p = line;
    char c;
    int mode = 0;
    uint8_t numTokens = 0;
    char *lastNonWhitespace = 0;

    /* Walk through all the characters and determine where the tokens are */
    /* while placing end of token characters on each token */
    while (numTokens < maxTokens) {
        c = *p;
        switch (mode) {
            case 0: /* looking for start */
                tokens[numTokens] = p;
                mode = 1;
                /* Fall into looking for a non-white space character immediately */
                /* no break */
            case 1:
                /* Skip any white space at the beginning (by staying in this state) */
                if (isspace(c))
                    break;
                tokens[numTokens] = p;
                lastNonWhitespace = p;
                mode = 2;
                /* Fall into mode 2 if not a white space and process immediately */
                /* no break */
            case 2:
                /* Did we find the end of the token? */
                if ((c == deliminator) || (c == '\0')) {
                    /* Null terminate the token after the last non-whitespace and */
                    /* go back to finding the next token */
                    lastNonWhitespace[1] = '\0';
                    numTokens++;
                    mode = 0;
                } else {
                    /* looking at normal characters here */
                    if (!isspace(c))
                        lastNonWhitespace = p;
                }
                break;
        }
        /* Reached end of string */
        if (c == '\0')
            break;
        p++;
    }

    return numTokens;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseUDPClientCid
 *--------------------------------------------------------------------------*/
/**   Parses the last line returned for a UDP Client connection.
 *      If a CONNECT [id] is returned, return [id].  Otherwise, respond with
 *      ATLIBGS_INVALID_CID.

 *  @return     uint8_t -- Returned connection id or ATLIBGS_INVALID_CID.
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseUDPClientCid(void)
{
    uint8_t cid;
    char *result = NULL;
    int cidIndex;

    if ((result = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
        /* Succesfull connection done for UDP client */
        cid = result[ATLIBGS_UDP_CLIENT_CID_OFFSET_BYTE];
        cidIndex = AtLibGs_CIDToIndex(cid);
        G_cidState[cidIndex].iState = CID_STATE_CONNECTED;
        G_cidState[cidIndex].iType = CID_TYPE_UDP;
        G_cidState[cidIndex].iIsServer = 0;
    } else {
        /* Not able to extract the CID */
        cid = ATLIBGS_INVALID_CID;
    }

    return cid;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseWlanConnStat
 *--------------------------------------------------------------------------*/
/**   Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command and determines if there is a wireless association.

 *  @return     uint8_t -- Returns true if associated, else false.
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseWlanConnStat(void)
{
    char *pSubStr;

    /* Check whether response message contains the following */
    /* string "BSSID=00:00:00:00:00:00" */
    pSubStr = strstr((const char *)MRBuffer, "BSSID=00:00:00:00:00:00");

    if (pSubStr) {
        /* Not been associated */
        return false;
    } else {
        /* Already associated */
        return true;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseNodeIPv4Address
 *--------------------------------------------------------------------------*/
/**   Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command.  The result is turned into an IP address.
 *  @param [in] ip -- Passed in IP address to compare
 *  @return     uint8_t -- Returns 0 if invalid IP address, 1 if valid ip address
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseNodeIPv4Address(ATLIBGS_IPv4 *ip)
{
    char *pSubStr;
    char ipstr[20];

    pSubStr = strstr((const char *)MRBuffer, "IP addr=");
    if (pSubStr) {
        strcpy((char *)ipstr, strtok((pSubStr + 8), ": "));
        if (ipstr[0] == '0') {
            /* Failed */
            return 0;
        }
    } else {
        /* Failed */
        return 0;
    }

    /* Success */
    return 1;

}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIPv4Address
 *--------------------------------------------------------------------------*/
/**   Parses the the given line in "###.###.###.###" format into a IPV4
 *      address.
 *  @param [in] line -- Line to parse
 *  @param [in] ip -- IP address parsed
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ParseIPv4Address(const char *line, ATLIBGS_IPv4 *ip)
{
    const char *p = line;
    uint8_t n;

    *ip[0] = *ip[1] = *ip[2] = *ip[3] = 0;
    for (n = 0; (n < 4) && p; n++) {
        *ip[n] = atoi(p);
        p = strchr(p, '.');
        if (!p)
            break;
        p++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseRssiResponse
 *--------------------------------------------------------------------------*/
/**   Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command.  The RSSI value is returned by reference.
 *  @param [in] pRssi -- Returned passed in value
 *  @return     uint8_t -- Returns 1 if RSSI value found and valid, else 0.
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseRssiResponse(int16_t *pRssi)
{
    char *pSubStr;

    if ((pSubStr = strstr((const char *)MRBuffer, "-")) != NULL) {
        *pRssi = atoi((pSubStr));

        return 1;
    } else {
        return 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseDNSLookupResponse
 *--------------------------------------------------------------------------*/
/**   Parses the last line returned after doing a AtLibGs_DNSLookup()
 *      command.  If connected, parses the TCP server connection id.
 *      Otherwise, no connection is returned.
 *  @param [in] ipAddr -- IP address
 *  @return     uint8_t -- Returns 1 if connected, else 0.
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseDNSLookupResponse(char *ipAddr)
{
    char *pSubStr = NULL;

    pSubStr = strstr((const char *)MRBuffer, "IP:");
    if (pSubStr) {
        strcpy(ipAddr, (pSubStr + 3));

        return 1; /* Success */
    } else {
        return 0; /* Failed  */
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CommandSendString
 *--------------------------------------------------------------------------*/
/**   Sends a raw command to the module and waits for a response.  If
 *      data is returned, it is collected into MRBuffer.
 *  @param [in] aString -- String to send
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CommandSendString(char *aString)
{
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf(">%s\n", aString);
#endif

    App_EnsureFlow();
    /* Now send the command to S2w App node */
    App_Write((char *)aString, strlen(aString));

    /* Wait for the response while collecting data into the MRBuffer */
    return AtLibGs_ResponseHandle(ATLIBGS_INVALID_CID);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DataSend
 *--------------------------------------------------------------------------*/
/**   Send data to the module.
 *  @param [in] pTxData -- Data to send
 *  @param [in] dataLen -- Length of data to send
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_DataSend(const void *pTxData, uint16_t dataLen)
{
    App_Write(pTxData, dataLen);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SendTCPData
 *--------------------------------------------------------------------------*/
/**   Send data to the given TCP connection.  The data is converted into
 *      the following byte format:
 *          [ESC]['S'][cid][N bytes][ESC]['E']
 *              [ESC] is the escape character 0x1B
 *              ['S'] is the letter 'S'
 *              [cid] is the connection ID.
 *              [N bytes] is a number of bytes
 *              ['E'] is the letter 'E'
 *  @param [in] cid -- Connection ID
 *  @param [in] txBuf -- Data to send to the TCP connection
 *  @param [in] dataLen -- Length of data to send
 *  @return     ATLIBGS_MSG_ID_E -- ATLIBGS_MSG_ID_OK if properly sent, else
 *              error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SendTCPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen)
{
    char cmd[20];
    ATLIBGS_MSG_ID_E rxMsgId;
    uint16_t i;
    uint8_t *p;
    int cidIndex;

    if (cid == ATLIBGS_INVALID_CID)
        return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;

    cidIndex = AtLibGs_CIDToIndex(cid);

    // Are we still a valid connection?
    if (G_cidState[cidIndex].iState != CID_STATE_CONNECTED)
    	return ATLIBGS_MSG_ID_DISCONNECT;

    /* Construct the data start indication message */
    sprintf(cmd, "%c%c%c", ATLIBGS_ESC_CHAR, 'S', cid);

    /* Now send the data START indication message  to S2w node */
    App_Write(cmd, 3);

    /* Look for an immediate OK or Fail response */
	rxMsgId = AtLibGs_ResponseHandle(cid);
    if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK) {
        /* Now send the actual data, but replace ESC with two ESC */
        //AtLibGs_DataSend(txBuf, dataLen);
        p = (uint8_t *)txBuf;
        for (i=0; i<dataLen; i++, p++) {
            AtLibGs_DataSend(p, 1);
            if (*p == ATLIBGS_ESC_CHAR)
                AtLibGs_DataSend(p, 1);
        }

        /* Construct the data end indication message */
        sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

        /* Now send the data END indication message  to S2w node */
        App_Write(cmd, 2);

        rxMsgId = AtLibGs_ResponseHandle(cid);
        if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
            rxMsgId = ATLIBGS_MSG_ID_OK;
    } else {
        // Abort, but first end this communications
        /* Construct the data start indication message */
        sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

        /* Now send the data END indication message  to S2w node */
        App_Write(cmd, 2);

        // Get the response, but ignore it (we already have an error)
        AtLibGs_ResponseHandle(cid);
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SendTCPBulkData
 *--------------------------------------------------------------------------*/
/**   Send data to the given TCP connection.  The data is converted into
 *      the following byte format:
 *          [ESC]['Z'][cid][len][N bytes]
 *              [ESC] is the escape character 0x1B
 *              ['Z'] is the letter 'Z'
 *              [cid] is the connection ID.
 *              [len] is the num bytes to send (4 digits)
 *              [N bytes] is a number of bytes
 *  @param [in] cid -- Connection ID
 *  @param [in] txBuf -- Data to send to the TCP connection
 *  @param [in] dataLen -- Length of data to send
 *  @return     ATLIBGS_MSG_ID_E -- ATLIBGS_MSG_ID_OK if properly sent, else
 *              error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SendTCPBulkData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen)
{
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_OK;
    int cidIndex;

    if (cid == ATLIBGS_INVALID_CID)
        return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;

    App_EnsureFlow();

    // Are we still a valid connection?
    cidIndex = AtLibGs_CIDToIndex(cid);
    if (G_cidState[cidIndex].iState != CID_STATE_CONNECTED)
    	return ATLIBGS_MSG_ID_DISCONNECT;

    rxMsgId = AtLibGs_BulkDataTransfer(cid, txBuf, dataLen);

    //rxMsgId = AtLibGs_ResponseHandle(cid);
    if (rxMsgId == ATLIBGS_MSG_ID_DISCONNECT) {
    	rxMsgId = AtLibGs_ResponseHandle(cid); // get the following fail message too
    }
    if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
        rxMsgId = ATLIBGS_MSG_ID_OK;

    return rxMsgId;
}


/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SendUDPData
 *--------------------------------------------------------------------------*/
/**   Send data to the given UDP connection.  The data is converted into
 *      the following byte format when UDP client:
 *          [ESC]['S'][cid][N bytes][ESC]['E']
 *      When UDP server, transmit in the following format:
 *          [ESC]['U'][cid][ip][port][N bytes][ESC]['E']
 *      [ESC] is the escape character 0x1B
 *      ['S'] is the letter 'S'
 *      [cid] is the connection ID.
 *      [N bytes] is a number of bytes
 *      ['E'] is the letter 'E'
 *      ['U'] is the letter 'U'
 *      [ip] is the ip number of the client
 *      [port] is the port number of the client
 *  @param [in] cid -- Connection ID
 *  @param [in] txBuf -- Data to send to the TCP connection
 *  @param [in] dataLen -- Length of data to send
 *  @param [in] conType -- Type of connection
 *              (ATLIBGS_CON_UDP_SERVER or ATLIBGS_CON_UDP_CLIENT)
 *  @param [in] pUdpClientIP -- String with client IP number
 *  @param [in] udpClientPort -- Port id of client
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SendUDPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen,
        ATLIBGS_CON_TYPE conType,
        const char *pUdpClientIP,
        uint16_t udpClientPort)
{
    char cmd[30];
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_INVALID_INPUT;
    int cidIndex;

    if (ATLIBGS_INVALID_CID != cid) {
        // Are we still a valid connection?
    	cidIndex = AtLibGs_CIDToIndex(cid);

        if (G_cidState[cidIndex].iState != CID_STATE_CONNECTED)
        	return ATLIBGS_MSG_ID_DISCONNECT;

        App_EnsureFlow();

        /* Construct the data start indication message */
        if (ATLIBGS_CON_UDP_SERVER == conType) {
            /* [ESC] [ U]  [cid] [ip address][:] [port numer][:] [data] [ESC] [ E] */
            sprintf(cmd, "%c%c%c%s:" _F16_ ":", ATLIBGS_ESC_CHAR, 'U',
                    cid, pUdpClientIP, udpClientPort);
        } else {
            /* [ESC] [ S]  [cid]  [data] [ESC] [ E] */
            sprintf(cmd, "%c%c%c", ATLIBGS_ESC_CHAR, 'S', cid);
        }

        /* Now send the data START indication message  to S2w node */
        App_Write(cmd, strlen(cmd));
        rxMsgId = AtLibGs_ResponseHandle(cid);
        if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK) {

            /* Now send the actual data */
            AtLibGs_DataSend(txBuf, dataLen);

            /* Construct the data start indication message  */
            sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

            /* Now send the data END indication message  to S2w node */
            App_Write(cmd, strlen(cmd));

            rxMsgId = AtLibGs_ResponseHandle(cid);
            if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
                rxMsgId = ATLIBGS_MSG_ID_OK;
        } else {
            /* Construct the data end indication message  */
            sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

            /* Now send the data END indication message  to S2w node */
            App_Write(cmd, strlen(cmd));

            // Get the response, but ignore it (we already have an error)
            AtLibGs_ResponseHandle(cid);
        }

    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BulkDataTransfer
 *--------------------------------------------------------------------------*/
/**   Send bulk data to a current transfer.  Bulk data is transferred in
 *      the following format:
 *          [ESC]['Z'][cid][data length][N bytes][ESC]['E']
 *      [ESC] is the escape character 0x1B
 *      ['Z'] is the letter 'Z'
 *      [cid] is the connection ID
 *      [data length] is 4 ASCII characters with the data length
 *      [N bytes] is a number of bytes, <= 1400 bytes
 *      ['E'] is the letter 'E'
 *  @param [in] cid -- Connection ID
 *  @param [in] pData -- Data to send to the TCP connection
 *  @param [in] dataLen -- Length of data to send
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BulkDataTransfer(uint8_t cid, const void *pData, uint16_t dataLen)
{
	ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_OK;

    /*[Esc] [Z] [Cid] [Data Length xxxx 4 ascii char] [data] */
    char digits[5];
    char cmd[20];

    App_EnsureFlow();

    /* Construct the bulk data start indication message  */
    sprintf(cmd, "%c%c%c", ATLIBGS_ESC_CHAR, ATLIBGS_DATA_MODE_BULK_START_CHAR_Z, cid);
    App_Write(cmd, strlen(cmd));

#if 0
    for (int i=0; i<50; i++) {
	if (GainSpan_SPI_IsDataReady(0)) {
		printf("+");
	} else {
		printf("-");
	}
	UEZTaskDelay(1);
}
#endif
    /* wait for GS1011 to process above command */
    /// Do we really need this?  App_DelayMS(10);
	rxMsgId = AtLibGs_ResponseHandle(cid);
    if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK) {
        AtLibGs_ConvertNumberTo4DigitASCII(dataLen, digits);
        sprintf(cmd, "%s", digits);

        /* Now send the bulk data START indication message  to S2w node */
        App_Write(cmd, strlen(cmd));

		/* Now send the actual data */
		App_Write(pData, dataLen);
    } else {


    }

    return rxMsgId;
}

const char *IGetLastLine(const char *p)
{
    const char *p_last = p;
    char c;
    int isCRLF = 0;

    while (*p) {
        c = *p;
        if (isCRLF) {
            if ((c == '\r') || (c == '\n')) {
                // Do nothing, just skip
            } else {
                // Found a character past a \r and/or \n
                // Must be the start of another line
                p_last = p;
                isCRLF = 0;
            }
        } else {
            if ((c == '\r') || (c == '\n')) {
                // Seeing an end of line, start tracking this
                isCRLF = 1;
            } else {
                // Just more characters in the line.  Ignore.
            }
        }
        // Next character
        p++;
    }

    // Return the last (or the very first) line found
    return p_last;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_checkEOFMessage
 *--------------------------------------------------------------------------*/
/**   This functions is used to check the completion of Commands
 *      This function will be called after receiving each line.
 *  @param [in] pBuffer -- Line of data to check
 *  @return     ATLIBGS_MSG_ID_E -- error code, ATLIBGS_MSG_ID_NONE is returned
 *          if the passed line is not identified.
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_checkEOFMessage(const char *pBufferInput)
{
    const char *p;
    uint8_t numSpaces;

    const char *pBuffer = IGetLastLine(pBufferInput);

    if ((strstr((const char *)pBuffer, "OK") != NULL)) {
        return ATLIBGS_MSG_ID_OK;
    } else if ((strstr((const char *)pBuffer, "INVALID INPUT") != NULL)) {
        return ATLIBGS_MSG_ID_INVALID_INPUT;
    } else if ((strstr((const char *)pBuffer, "DISASSOCIATED") != NULL)) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        return ATLIBGS_MSG_ID_DISASSOCIATION_EVENT;
    } else if ((strstr((const char *)pBuffer, "ERROR: IP CONFIG FAIL") != NULL)) {
        return ATLIBGS_MSG_ID_ERROR_IP_CONFIG_FAIL;
    } else if (strstr((const char *)pBuffer, "ERROR: SOCKET FAILURE") != NULL) {
    	if (pBuffer[21] == ' ') {
    		// Socket failure on a connection also causes it to close
			int cid = pBuffer[22];
			int cidIndex;

			cidIndex = AtLibGs_CIDToIndex(cid);

			// Mark the connection as disconnected
			G_cidState[cidIndex].iState = CID_STATE_DISCONNECTED;
		}

        /* Reset the local flags */
        return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;
    } else if ((strstr((const char *)pBuffer, "APP Reset-APP SW Reset"))
            != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();

        // There are no connections anymore, either
        memset(G_cidState, 0, sizeof(G_cidState));

        return ATLIBGS_MSG_ID_APP_RESET;
    } else if ((strstr((const char *)pBuffer, "DISCONNECT")) != NULL) {
        /* Reset the local flags */
    	int cid = pBuffer[11];
    	int cidIndex;
#ifdef ATLIBGS_DEBUG_ENABLE
		printf("Disconnecting CID [%c]\n", cid);
#endif		

		cidIndex = AtLibGs_CIDToIndex(cid);

		// Mark the connection as disconnected
		G_cidState[cidIndex].iState = CID_STATE_DISCONNECTED;

		// Report that a disconnection has been detected.  It is up to the caller
		// to decide to go again or stop immediately.
        return ATLIBGS_MSG_ID_DISCONNECT;
    } else if ((strstr((const char *)pBuffer, "Disassociation Event")) != NULL) {
        /* reset the association flag */
        AtLibGs_ClearNodeAssociationFlag();
        return ATLIBGS_MSG_ID_DISASSOCIATION_EVENT;
    } else if ((strstr((const char *)pBuffer, "Out of StandBy-Alarm")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_STBY_ALARM;
    } else if ((strstr((const char *)pBuffer, "Out of StandBy-Timer")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_STBY_TIMER;
    } else if ((strstr((const char *)pBuffer, "UnExpected Warm Boot")) != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();
        return ATLIBGS_MSG_ID_UNEXPECTED_WARM_BOOT;
    } else if ((strstr((const char *)pBuffer, "Out of Deep Sleep")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_DEEP_SLEEP;
    } else if ((strstr((const char *)pBuffer, "Serial2WiFi APP")) != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();
        return ATLIBGS_MSG_ID_WELCOME_MSG;
    } else if ((pBuffer[0] == 'A') && (pBuffer[1] == 'T')
            && (pBuffer[2] == '+')) {
        /* Handle the echoed back AT Command, if Echo is enabled.  "AT+" . */
        return ATLIBGS_MSG_ID_NONE;
    } else if (strstr((const char *)pBuffer, "CONNECT ") != NULL) {
        /* Determine if this CONNECT line is of the type CONNECT [server id] [cid] [ip] [port] */
        /* by counting spaces in between the words.  A standard CONNECT [cid] is not the same */
        p = pBuffer;
        numSpaces = 0;
        while ((*p) && (*p != '\n')) {
            if (*p == ' ')
                numSpaces++;
            if (numSpaces >= 4) {
            	char buffer[100];
                char *tokens[6];
            	int cid = pBuffer[10];
            	int cidIndex;
            	strcpy(buffer, pBuffer);

#ifdef ATLIBGS_DEBUG_ENABLE
				printf("Connecting CID %c\n", cid);
#endif				
                AtLibGs_ParseIntoTokens(buffer, ' ', tokens, 6);

                cidIndex = AtLibGs_CIDToIndex(cid);

                // Parse the connection information
                G_cidState[cidIndex].server_cid = *tokens[1];
                G_cidState[cidIndex].cid = *tokens[2];
				strcpy(G_cidState[cidIndex].ip, tokens[3]);
				G_cidState[cidIndex].port = atoi(tokens[4]);

				// and mark this as now formally connecting (not accepted yet)
				G_cidState[cidIndex].iIsServer = 0;
				G_cidState[cidIndex].iState = CID_STATE_CONNECTING;
				G_cidState[cidIndex].iType = CID_TYPE_TCP;

                return ATLIBGS_MSG_ID_TCP_SERVER_CONNECT;
            }
            p++;
        }
    } else if ((strstr((const char *)pBuffer, "ERROR") != NULL)) {
        return ATLIBGS_MSG_ID_ERROR;
    }

    return ATLIBGS_MSG_ID_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForUDPMessage
 *--------------------------------------------------------------------------*/
/**   Waits for and parses a UDP message into CID, IP address, Port Number, and
 *      the message itself.
 *  @param [in] timeout -- Timeout in milliseconds, or 0 for no timeout
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT on timeout,
 *          or ATLIBGS_MSG_ID_OK if a message is returned
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForUDPMessage(uint32_t timeout)
{
    uint8_t rxData;
    uint32_t start = MSTimerGet();

    /* wait until message received */
    while (1) {
        /* Read one byte at a time - Use non-blocking call */
        while (App_Read(&rxData, 1, 0)) {
            /* Restart the timeout */
            start = MSTimerGet();

            /* Process the received data */
            if (AtLibGs_ReceiveDataProcess(rxData) == ATLIBGS_MSG_ID_DATA_RX) {
                break;
            }
        }

        /* Is there a timeout (if any defined)? */
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
    }
#if (COMPILER_TYPE!=IAR)    
    return ATLIBGS_MSG_ID_OK;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForTCPMessage
 *--------------------------------------------------------------------------*/
/**   Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 *  @param [in] connection -- Connection structure to fill
 *  @param [in] timeout -- Timeout in milliseconds, 0 for no timeout
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_DATA_RX if TCP message found.
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPConnection(
        ATLIBGS_TCPConnection *connection,
        uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();
    char *p;
    char *tokens[6];
    uint16_t numTokens;
    int i;

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            break;

        // First, check to see if there are any connecting sockets waiting for processing
        for (i=0; i<16; i++) {
        	if (G_cidState[i].iState == CID_STATE_CONNECTING) {
        		// Found one!
                // Copy over the connection information
                connection->server_cid = G_cidState[i].server_cid;
                connection->cid = G_cidState[i].cid;
                strcpy(connection->ip, G_cidState[i].ip);
                connection->port = G_cidState[i].port;

                // Mark the port as now connected
                G_cidState[i].iState = CID_STATE_CONNECTED;
#ifdef ATLIBGS_DEBUG_ENABLE
				printf("Changing connecting to connected [%c]\n", G_cidState[i].cid);
#endif				
                return ATLIBGS_MSG_ID_TCP_SERVER_CONNECT;
        	}
        }

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT) {
                // Now parse out the TCP connection information
                p = strstr(MRBuffer, "CONNECT");
                numTokens = AtLibGs_ParseIntoTokens(p, ' ', tokens, 6);
                if (numTokens >= 5) {
                	int cidIndex;

                    // Parse the connection information
                    connection->server_cid = *tokens[1];
                    connection->cid = *tokens[2];
                    strcpy(connection->ip, tokens[3]);
                    connection->port = atoi(tokens[4]);

                    cidIndex = AtLibGs_CIDToIndex(connection->cid);

                    G_cidState[cidIndex].cid = connection->cid;
                    G_cidState[cidIndex].iIsServer = 0;
                    G_cidState[cidIndex].iState = CID_STATE_CONNECTED;
                    G_cidState[cidIndex].iType = CID_TYPE_TCP;
#ifdef ATLIBGS_DEBUG_ENABLE
					printf("CONNECT [%c]\n", G_cidState[cidIndex].cid);
#endif					

                    return rxMsgId;
                } else {
                    return ATLIBGS_MSG_ID_ERROR;
                }
            }
        }

        ///What do we do instead?
        UEZTaskDelay(1);
    }
    return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForTCPMessage
 *--------------------------------------------------------------------------*/
/**   Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 *  @param [in] cid -- CID we're currently watching closely.  If it closes,
 *  	report it closed.  (If another connection closes, we don't care yet)
 *  @param [in] timeout -- Timeout in milliseconds, 0 for no timeout
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_DATA_RX if TCP message found.
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPMessage(uint8_t cid, uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();
    int cidIndex;

    cidIndex = AtLibGs_CIDToIndex(cid);

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            break;

        // Check to see if the connection is still open here
        if (G_cidState[cidIndex].iState != CID_STATE_CONNECTED) {
        	// The connection went away, stop messing this this
        	return ATLIBGS_MSG_ID_DISCONNECT;
        }

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_DATA_RX)
                return rxMsgId;
        }
    }
    return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForHTTPMessage
 *--------------------------------------------------------------------------*/
/**   Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 *  @param [in] timeout -- Timeout in milliseconds, 0 for no timeout
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX if HTTP message found.
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForHTTPMessage(uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            break;

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX)
                return rxMsgId;
        }
    }
    return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseTCPData
 *--------------------------------------------------------------------------*/
/**   Parses a packet of data returned from the TCP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForTCPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 *  @param [in] received -- pointer to received bytes
 *  @param [in] length -- number of bytes received
 *  @param [in] msg -- pointer to structure to fill with
 *                  TCP message info.
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ParseTCPData(
        const uint8_t *received,
        uint16_t length,
        ATLIBGS_TCPMessage *msg)
{
    uint8_t *p = (uint8_t *)received;

    if (length >= 1) {
        msg->cid = p[0];
        msg->numBytes = length - 1;
        msg->message = &p[1];
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
        msg->cid = 0xFF;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseUDPData
 *--------------------------------------------------------------------------*/
/**   Parses a packet of data returned from the UDP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForUDPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 *  @param [in] received -- pointer to received bytes
 *  @param [in] length -- number of bytes received
 *  @param [in] msg -- pointer to structure to fill with
 *                  UDP message info.
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ParseUDPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_UDPMessage *msg)
{
    int i;
    int n;
    char port[6];

    if (length >= 1) {
        /* set CID */
        msg->cid = received[0];

        /* clear IP array */
        for (n = 0; n <= 16; n++)
            msg->ip[n] = 0x00;

        /* Read in the IP address (16 bytes) */
        for (i = 0; i < 16; i++) {
            if (received[i + 1] != 0x20) {
                /* save IP to array */
                msg->ip[i] = received[i + 1];
            } else {
                /* ignore spaces */
                msg->ip[i] = '\0';
                break;
            }
        }
        i = i + 2;

        for (n = 0; n < 5; n++) {
            if (received[i] != 0x09) {
                /* save port to array */
                port[n] = received[i++];
            } else {
                /* ignore tab */
                break;
            }
        }
        port[n] = '\0';
        msg->port = atoi(port);
        /* save message */
        msg->message = (uint8_t *)(received + i + 1);
        msg->numBytes = length - (i + 1);
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseHTTPData
 *--------------------------------------------------------------------------*/
/**   Parses a packet of data returned from the HTTP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForHTTPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 *  @param [in] received -- pointer to received bytes
 *  @param [in] length -- number of bytes received
 *  @param [in] msg -- pointer to structure to fill with
 *                  TCP message info.
 *  @return     ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ParseHTTPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_HTTPMessage *msg)
{
    uint8_t *p = (uint8_t *)received;

    if (length >= 1) {
        msg->cid = p[0];
        msg->numBytes = length - 1;
        msg->message = &p[1];
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
    }
}

#if 0
static void PrintByte(uint8_t c)
{
    if ((c >= 0x20) && (c <= 0x7F))
        printf("%c", c);
    else
        printf("[%02X]", c);
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReceiveDataProcess
 *--------------------------------------------------------------------------*/
/**   Process individually received characters
 *  @param [in] rxData -- Character to process
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReceiveDataProcess(uint8_t rxData)
{
    /* receive data handling state */
    static ATLIBGS_RX_STATE_E receive_state = ATLIBGS_RX_STATE_START;
    static uint16_t dataLength = 0;
    static uint16_t dataLengthCharCount = 0;
    uint16_t datalencount;
    static bool esc2 = true;

    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;

#ifdef ATLIBGS_DEBUG_ENABLE
    if ((isprint(rxData)) || (isspace(rxData)))
        ConsolePrintf("%c", rxData);
#endif    

    /* Process the received data */
    switch (receive_state) {
        case ATLIBGS_RX_STATE_START:
            switch (rxData) {
                case ATLIBGS_CR_CHAR:
                case ATLIBGS_LF_CHAR:
                    /* CR and LF at the begining, just ignore it */
                    MRBufferIndex = 0;
                    //printf(ANSI_MAGENTA "CR{" ANSI_OFF);
                    break;

                case ATLIBGS_ESC_CHAR:
                    /* ESCAPE sequence detected */
                    receive_state = ATLIBGS_RX_STATE_ESCAPE_START;
                    MRBufferIndex = 0;
                    dataLength = 0;
                    //printf(ANSI_MAGENTA "ESC{" ANSI_OFF);
                    break;

                default:
                    /* Not start of ESC char, not start of any CR or NL */
                    MRBufferIndex = 0;
                    MRBuffer[MRBufferIndex] = rxData;
#if 0
    if ((rxData >= 0x20) && (rxData <= 0x7F))
        printf(ANSI_MAGENTA "DEF{%c" ANSI_OFF, rxData);
    else {
        printf(ANSI_MAGENTA "DEF{[%02X]" ANSI_OFF, rxData);
    }
#endif

                    MRBufferIndex++;
                    receive_state = ATLIBGS_RX_STATE_CMD_RESP;
                    break;
            }
            break;

        case ATLIBGS_RX_STATE_CMD_RESP:
            if (ATLIBGS_LF_CHAR == rxData) {
                /* LF detected - Messages from S2w node are terminated with LF/CR character */
                MRBuffer[MRBufferIndex] = rxData;
#if 0
    if ((rxData >= 0x20) && (rxData <= 0x7F))
        printf(ANSI_MAGENTA "%c" ANSI_OFF, rxData);
    else {
        printf(ANSI_MAGENTA "[%02X]" ANSI_OFF, rxData);
    }
#endif

                /* terminate string with NULL for strstr() */
                MRBufferIndex++;
                MRBuffer[MRBufferIndex] = '\0';
//printf(ANSI_MAGENTA "} EOFMSG " ANSI_OFF);
                rxMsgId = AtLibGs_checkEOFMessage(MRBuffer);
//printf(ANSI_MAGENTA "rxMsgId=%d " ANSI_OFF, rxMsgId);

                if (ATLIBGS_MSG_ID_NONE != rxMsgId) {
                    /* command echo or end of response detected */
                    /* Now reset the  state machine */
                    receive_state = ATLIBGS_RX_STATE_START;
                }
            } else if (ATLIBGS_ESC_CHAR == rxData) {
                /* Defensive check - This should not happen */
                receive_state = ATLIBGS_RX_STATE_START;
                /* end of message hits here             */
                if (esc2 == true) {
                    esc2 = false;
                    return ATLIBGS_MSG_ID_GENERAL_MESSAGE;
                } else {
                    esc2 = true;
                }

            } else {
                if(rxData){
                    MRBuffer[MRBufferIndex] = rxData;
                } else {
                    MRBuffer[MRBufferIndex] = 0x20;
                }				
                MRBufferIndex++;
#if 0
    if ((rxData >= 0x20) && (rxData <= 0x7F))
        printf(ANSI_MAGENTA "%c" ANSI_OFF, rxData);
    else {
        printf(ANSI_MAGENTA "[%02X]" ANSI_OFF, rxData);
    }
#endif

                if (MRBufferIndex >= ATLIBGS_RX_CMD_MAX_SIZE) {
                    /* Message buffer overflow. Something seriousely wrong. */
                    MRBufferIndex = 0;
//printf(ANSI_MAGENTA "} MAX! " ANSI_OFF);

                    /* Now reset the  state machine */
                    receive_state = ATLIBGS_RX_STATE_START;
                }
            }
            break;

        case ATLIBGS_RX_STATE_ESCAPE_START:
            if (ATLIBGS_DATA_MODE_BULK_START_CHAR_H == rxData) {
                /* HTTP Bulk data handling start */
                /* [Esc]H[1 Byte - CID][4 bytes - Length of the data][data] */
                receive_state = ATLIBGS_RX_STATE_HTTP_RESPONSE_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_BULK_START_CHAR_Z == rxData) {
                /* Bulk data handling start */
                /* [Esc]Z[Cid][Data Length xxxx 4 ascii char][data]   */
                receive_state = ATLIBGS_RX_STATE_BULK_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_NORMAL_START_CHAR_S == rxData) {
                /* Start of data */
                /* ESC S  cid  [----data --- ] ESC E  */
                receive_state = ATLIBGS_RX_STATE_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL == rxData) {
                /* Start of raw data  */
                /* ESC R : datalen : [----data --- ]
                 Unlike other data format, there is no ESC E at the end .
                 So extract datalength to find out the incoming data size */
                receive_state = ATLIBGS_RX_STATE_RAW_DATA_HANDLE;
                dataLength = 0;
            } else if (ATLIBGS_DATA_MODE_ESC_OK_CHAR_O == rxData) {
                /* ESC command response OK */
                /* Note: No need to take any action. Its just an data reception */
                /* acknowledgement S2w node */
                receive_state = ATLIBGS_RX_STATE_START;
                rxMsgId = ATLIBGS_MSG_ID_ESC_CMD_OK;
                //printf(ANSI_YELLOW "Time: %d" ANSI_OFF, UEZTickCounterGet());
            } else if (ATLIBGS_DATA_MODE_ESC_FAIL_CHAR_F == rxData) {
                /* ESC command response FAILED */
                /* Note: Error reported from S2w node, you can use it */
                /* for debug purpose. */
                receive_state = ATLIBGS_RX_STATE_START;
                rxMsgId = ATLIBGS_MSG_ID_ESC_CMD_FAIL;
            } else if (ATLIBGS_DATA_MODE_UDP_START_CHAR_u == rxData) {
                /* Start of UDP data */
                /* ESC u  cid  [----data with info---] ESC E  */
                receive_state = ATLIBGS_RX_STATE_DATA_HANDLE;
            } else {
                /* ESC sequence parse error !  */
                /* Reset the receive buffer */
                receive_state = ATLIBGS_RX_STATE_START;
            }
            break;

        case ATLIBGS_RX_STATE_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);
            //PrintByte(rxData);

            /* Keep receiving data till you get ESC E */
            do {
                App_Read(&rxData, 1, 1);
                //PrintByte(rxData);

                /* Is this char an ESC? */
                while (rxData == ATLIBGS_ESC_CHAR) {
                    //printf("[ESC!]");
                    /* What is the next character? */
                    App_Read(&rxData, 1, 1);
					//PrintByte(rxData);
                    /* Is it an 'E'? */
                    if (rxData == ATLIBGS_DATA_MODE_NORMAL_END_CHAR_E) {
					//printf("[END!]");
                        /* End of data detected */
                        App_EndIncomingData();
                        receive_state = ATLIBGS_RX_STATE_START;
                        rxMsgId = ATLIBGS_MSG_ID_DATA_RX;
                        return rxMsgId;
                    }
                    /* Go ahead and store the ESC character */
                    App_ProcessIncomingData(ATLIBGS_ESC_CHAR);

                    /* Repeat if the second charater was an ESC char */
                }

                /* Store whatever character we were left holding */
                App_ProcessIncomingData(rxData);
            } while (receive_state != ATLIBGS_RX_STATE_START);
            break;

        case ATLIBGS_RX_STATE_HTTP_RESPONSE_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);

            /* Get HTTP response length */
            dataLength = 0;
            for (datalencount = 0; datalencount < 4; datalencount++) {
                App_Read(&rxData, 1, 1);
                dataLength = (dataLength * 10) + ((rxData) - '0');
            }

            /* Now read actual data */
            while (dataLength--) {
                App_Read(&rxData, 1, 1);
                App_ProcessIncomingData(rxData);
            }

            receive_state = ATLIBGS_RX_STATE_START;
            rxMsgId = ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX;
            break;

        case ATLIBGS_RX_STATE_BULK_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);

            /* Get HTTP response length */
            dataLength = 0;
            for (datalencount = 0; datalencount < 4; datalencount++) {
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);
                dataLength = (dataLength * 10) + ((rxData) - '0');
            }

            /* Now read actual data */
            while (dataLength--) {
                /* Read one byte at a time */
                App_Read(&rxData, 1, 1);
                App_ProcessIncomingData(rxData);
            }
            receive_state = ATLIBGS_RX_STATE_START;
            rxMsgId = ATLIBGS_MSG_ID_DATA_RX;
            break;

        case ATLIBGS_RX_STATE_RAW_DATA_HANDLE:
            if (ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL == rxData) {
                dataLengthCharCount = 0;
            }

            /* Now reset the buffer and state machine */
            receive_state = ATLIBGS_RX_STATE_START;

            do {
                /* extracting the rx data length*/
                /* Read one byte at a time */
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);

                if (rxData != ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL) {
                    dataLength = (dataLength * 10) + ((rxData) - '0');
                    dataLengthCharCount++;
                }
            } while ((rxData != ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL)
                    && (dataLengthCharCount < 4));

            /* Now read actual data */
            while (dataLength) {
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);
                dataLength--;
                App_ProcessIncomingData(rxData);
            }

            receive_state = ATLIBGS_RX_STATE_START;
            break;

        default:
            /* This case will not be executed */
            break;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ResponseHandle
 *--------------------------------------------------------------------------*/
/**   Wait for a response after sending a command.  Keep parsing the
 *      data until a response is found.

 *  @param [in] cid -- CID of current thread looking for response.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ResponseHandle(uint8_t cid)
{
    ATLIBGS_MSG_ID_E responseMsgId;
    uint8_t rxData;
    uint8_t gotData = 0;
    uint32_t iStart;

    App_EnsureFlow();

    /* Reset the message ID */
    responseMsgId = ATLIBGS_MSG_ID_NONE;

    /* Now process the response from S2w App node */
    while (ATLIBGS_MSG_ID_NONE == responseMsgId) {
        gotData = 1;
        /* Read one byte at a time - non-blocking call, block here */
        iStart = UEZTickCounterGet(); // Setup a timeout period
        while (!App_Read(&rxData, 1, 0)) {
            if (UEZTickCounterGetDelta(iStart) >= ATLIB_RESPONSE_HANDLE_TIMEOUT) {
                gotData = 0;
                responseMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
                break;
            }
        }
        if (gotData) {
            /* Process the received data */
        	while (1) {
        		responseMsgId = AtLibGs_ReceiveDataProcess(rxData);
        		if (((responseMsgId == ATLIBGS_MSG_ID_DISCONNECT) || (responseMsgId == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT)) && (cid != ATLIBGS_INVALID_CID)) {
        			// Was this us?  If still connected, ignore it and repeat.
        			// Otherwise, we'll fall through and report the disconnect
        			int cidIndex = AtLibGs_CIDToIndex(cid);
        			if (G_cidState[cidIndex].iState == CID_STATE_CONNECTED)
        				continue;
        		}

        		// Unless repeated above, always fall out (fancy goto)
        		break;
        	}
            if (responseMsgId != ATLIBGS_MSG_ID_NONE) {
                /* A message was successfully received from S2w App node */
                break;
            }
        }
    }

    return responseMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ProcessRxChunk
 *--------------------------------------------------------------------------*/
/**   Process a group of received bytes looking for a response message.
 *  @param [in] rxBuf -- Pointer to bytes
 *  @param [in] bufLen -- Number of bytes in receive buffer
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ProcessRxChunk(const void *rxBuf, uint16_t bufLen)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    const uint8_t *rx = (uint8_t *)rxBuf;

    rxMsgId = ATLIBGS_MSG_ID_NONE;

    /* Parse the received data and check whether any valid message present in the chunk */
    while (bufLen) {
        /* Process the received data */
        rxMsgId = AtLibGs_ReceiveDataProcess(*rx);
        if (rxMsgId != ATLIBGS_MSG_ID_NONE) {
            /* Message received from S2w App node */
            break;
        }
        rx++;
        bufLen--;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FlushIncomingMessage
 *--------------------------------------------------------------------------*/
/**   Read bytes until no more come in for 100 ms.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_FlushIncomingMessage(void)
{
    /* This function will read all incoming data until nothing happens */
    /* for 100 ms */
    uint8_t rxData;
    uint32_t start;

    /* Read one byte at a time - non-blocking call */
    start = MSTimerGet();
    while (MSTimerDelta(start) < 100) {
        if (App_Read(&rxData, 1, 0)) {
            start = MSTimerGet();
#ifdef ATLIBGS_DEBUG_ENABLE
            /* If required you can print the received characters on debug UART port. */
            ConsolePrintf("%c", rxData);
#endif
        }
    };
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetNodeResetFlag
 *--------------------------------------------------------------------------*/
/**   Set the module reset flag.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_SetNodeResetFlag(void)
{
    nodeResetFlag = true;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ClearNodeResetFlag
 *--------------------------------------------------------------------------*/
/**   Clear the module reset flag.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ClearNodeResetFlag(void)
{
    nodeResetFlag = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IsNodeResetDetected
 *--------------------------------------------------------------------------*/
/**   Return flag telling if module has been reset.

 *  @return     uint8_t -- true if reset detected, else false
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_IsNodeResetDetected(void)
{
    return nodeResetFlag;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IsNodeAssociated
 *--------------------------------------------------------------------------*/
/**   Return flag that tells if module is associated with a network.

 *  @return     uint8_t -- true if associated with network, else false.
 */
/*--------------------------------------------------------------------------*/
uint8_t AtLibGs_IsNodeAssociated(void)
{
    return nodeAssociationFlag;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetNodeAssociationFlag
 *--------------------------------------------------------------------------*/
/**   Set the flag for associated to network.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_SetNodeAssociationFlag(void)
{
    nodeAssociationFlag = true;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ClearNodeAssociationFlag
 *--------------------------------------------------------------------------*/
/**   Clear the flag for associated to network.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ClearNodeAssociationFlag(void)
{
    nodeAssociationFlag = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ConvertNumberTo4DigitASCII
 *--------------------------------------------------------------------------*/
/**   Equivalent of sprintf("%04d"), number to convert a number to
 *      four characters.
 *  @param [in] myNum -- Number to convert to text
 *  @param [in] pStr -- Place to store characters
 *  @return     int32_t -- result of comparison.  0 if matches.  <0 if s1 before s2.
 *          >0 if s1 after s2.
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ConvertNumberTo4DigitASCII(uint16_t myNum, char *pStr)
{
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    uint8_t digit4;

    digit1 = myNum / 1000;
    digit2 = (myNum % 1000) / 100;
    digit3 = ((myNum % 1000) % 100) / 10;
    digit4 = ((myNum % 1000) % 100) % 10;

    sprintf((char *)pStr, _F8_ _F8_ _F8_ _F8_, digit1, digit2, digit3, digit4);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FlushRxBuffer
 *--------------------------------------------------------------------------*/
/**   Flush by clearing the receiving buffer (MRBuffer).

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_FlushRxBuffer(void)
{
    /* Reset the response receive buffer */
    /* TODO: What do we do here now? */
    MRBufferIndex = 0;
    memset(MRBuffer, '\0', ATLIBGS_RX_CMD_MAX_SIZE);
//printf(ANSI_MAGENTA " FLUSH " ANSI_OFF);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Init
 *--------------------------------------------------------------------------*/
/**   Prepare the AtLib.

 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_Init(void)
{
    /* Reset the command receive buffer */
    AtLibGs_FlushRxBuffer();

    /* Reset the flags */
    nodeAssociationFlag = false;
    nodeResetFlag = false;
    memset(G_cidState, 0, sizeof(G_cidState));
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetInfo
 *--------------------------------------------------------------------------*/
/**   Send command to get information about the device:
 *          ATIn
 *      where n is one of the following:
 *          ATLIBGS_ID_INFO_OEM = returns OEM name, (e.g., "GainSpan")
 *          ATLIBGS_ID_INFO_HARDWARE_VERSION = returns hardware version/model
 *              (e.g., "GS1011A1")
 *          ATLIBGS_ID_INFO_SOFTWARE_VERSION = returns version number
 *              (e.g., "2.3.5")
 *  @param [in] info
 *  @param [in] infotext
 *  @param [in] maxchars
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetInfo(
        ATLIBGS_ID_INFO_E info,
        char *infotext,
        int16_t maxchars)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char cmd[20];
    char *lines[5];
    uint16_t numLines;

    sprintf(cmd, "ATI" _F8_ "\r\n", info);

    rxMsgId = AtLibGs_CommandSendString(cmd);

    infotext[0] = 0;
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 5);
        if (numLines >= 2)
            strncpy((char *)infotext, lines[0], maxchars);
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetScanTimes
 *--------------------------------------------------------------------------*/
/**   Send command to get WiFi scan times:
 *          AT+WST=?
 *      The response will be
 *          MinScanTime=[min]
 *          MaxScanTime=[max]
 *          OK
 *      Parse this data and return.
 *  @param [in] min -- Minimum time in milliseconds
 *  @param [in] max -- Maximum time in milliseconds
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetScanTimes(uint16_t *min, uint16_t *max)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[5];
    uint16_t numLines;

    *min = 0;
    *max = 0;
    rxMsgId = AtLibGs_CommandSendString("AT+WST=?\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 5);
        if (numLines >= 2) {
            if ((strncmp((char *)(lines[0]), "MinScanTime=", 12) == 0)
                    && (strncmp((char *)(lines[1]), "MaxScanTime=", 12) == 0)) {
                *min = atoi(lines[0] + 12);
                *max = atoi(lines[1] + 12);
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetScanTimes
 *--------------------------------------------------------------------------*/
/**   Send command to set WiFi scan times:
 *          AT+WST=[min],[max]
 *      The response will be
 *          OK
 *  @param [in] min -- Minimum time in milliseconds
 *  @param [in] max -- Maximum time in milliseconds
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetScanTimes(uint16_t min, uint16_t max)
{
    char cmd[30];

    sprintf(cmd, "AT+WST=" _F16_ "," _F16_ "\r\n", min, max);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetDefaultProfile
 *--------------------------------------------------------------------------*/
/**   Set the default profile (0 or 1).
 *      Sends the command:
 *          AT&Y[0|1]
 *      and waits for a response.
 *  @param [in] profile -- index of profile to use
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetDefaultProfile(uint8_t profile)
{
    char cmd[20];

    sprintf(cmd, "AT&Y" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetProfile
 *--------------------------------------------------------------------------*/
/**   Gets the profile settings of one of the stored or active profiles.
 *      The profile setting is returned as a single string of all the
 *      AT command settings.
 *      Sends the command:
 *          AT&V
 *      and waits for a response.
 *  @param [in] profile
 *  @param [in] text
 *  @param [in] maxChars
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetProfile(
        ATLIBGS_PROFILE_E profile,
        char *text,
        uint16_t maxChars)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[50];
    uint16_t numLines;
    uint16_t offset = 0;
    uint16_t i;
    int mode;
    int len;
    static const char *sections[3] = {
            "ACTIVE PROFILE",
            "STORED PROFILE 0",
            "STORED PROFILE 1" };

    rxMsgId = AtLibGs_CommandSendString("AT&V\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 50);
        mode = 0;
        text[0] = '\0';
        for (i = 0; (i < numLines) && (mode != -1) && (offset < maxChars); i++) {
            switch (mode) {
                case 0: /* mode 0 = looking for matching profile */
                    if (strcmp(lines[i], sections[profile]) == 0) {
                        /* Found a match, start appending data */
                        mode = 1;
                    }
                    break;
                case 1:
                    if (strcmp(lines[i], "") == 0) {
                        /* Stop processing the data */
                        mode = -1;
                    } else {
                        /* Append a space if we added text */
                        if (offset) {
                            text[offset++] = ' ';
                            text[offset] = '\0';
                        }
                        /* How long is the new data? */
                        len = strlen(lines[i]);
                        /* Will this string fit? */
                        if ((len + offset) < maxChars) {
                            strcpy(text + offset, lines[i]);
                            offset += len;
                        } else {
                            /* ran out of room, stop here */
                            mode = -1;
                        }
                    }
                    break;
            }
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetRegulatoryDomain
 *--------------------------------------------------------------------------*/
/**   Gets the current regulatory domain in use.
 *      Sends the command:
 *          AT+WREGDOMAIN=?
 *      and waits for a response.  The response is then parsed for errors
 *      and the regulatory domain used
 *  @param [in] regDomain -- Regulatory domain in use
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetRegulatoryDomain(ATLIBGS_REGDOMAIN_E *regDomain)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+WREGDOMAIN=?\r\n");
    *regDomain = ATLIBGS_REGDOMAIN_UNKNOWN;
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
        if (strcmp(lines[0], "REG_DOMAIN=FCC") == 0)
            *regDomain = ATLIBGS_REGDOMAIN_FCC;
        else if (strcmp(lines[0], "REG_DOMAIN=ETSI") == 0)
            *regDomain = ATLIBGS_REGDOMAIN_ETSI;
        else if (strncmp(lines[0], "REG_DOMAIN=TELEC", 16) == 0)
            *regDomain = ATLIBGS_REGDOMAIN_TELEC;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetRegulatoryDomain
 *--------------------------------------------------------------------------*/
/**   Sets the regulatory domain in use.
 *      Sends the command:
 *          AT+WREGDOMAIN=n
 *      where n is
 *          0 for FCC,
 *          1 for ETSI, or
 *          2 for TELEC
 *      and waits for a response.
 *  @param [in] regDomain -- Regulatory domain to use
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetRegulatoryDomain(ATLIBGS_REGDOMAIN_E regDomain)
{
    char cmd[30];
    sprintf(cmd, "AT+WREGDOMAIN=" _F8_ "\r\n", regDomain);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_NetworkScan
 *--------------------------------------------------------------------------*/
/**   Scans for a list of networks.
 *      Sends the command:
 *          AT+WS=[SSID],[channel],[scantime]
 *      and waits for a response.
 *      The SSID, channel, and scantime values are optional.
 *  @param [in] SSID -- 0 for complete network list, or string for
 *               particular SSID.
 *  @param [in] channel -- Channel to perform scan on, or 0 for all channels
 *  @param [in] scantime -- Millisecond scan time
 *  @param [in] entries -- pointer to network scan entry
 *  @param [in] maxEntries -- Max number of entries
 *  @param [in] numEntries -- number of entries
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_NetworkScan(
        const char *SSID,
        uint8_t channel,
        uint16_t scantime,
        ATLIBGS_NetworkScanEntry *entries,
        uint8_t maxEntries,
        uint8_t *numEntries)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[50];
    int numLines;
    char text[50];
    char *tokens[20];
    uint8_t numTokens;
    int i;
    ATLIBGS_NetworkScanEntry *entry = entries;
    char cmd[60];

    *numEntries = 0;

    strcpy(cmd, "AT+WS");
#if 0
    if (SSID != NULL) {
        strcat(cmd, "=");
        strcat(cmd, SSID);
        strcat(cmd, ",");
        if (channel) {
            sprintf(text, _F8_, channel);
            strcat(cmd, text);
            strcat(cmd, ",");
        }
        if (scantime != 0) {
            sprintf(text, _F8_, scantime);
            strcat(cmd, text);
        }
    }
#endif
    if (SSID != NULL){
        strcat(cmd, "=");
        strcat(cmd, SSID);
    }
    if (channel) {
        strcat(cmd, ",");
        sprintf(text, _F8_, channel);
        strcat(cmd, text);
    }
    if (scantime != 0) {
        //strcat(cmd, ",");
        //sprintf(text, _F8_, scantime);
        //strcat(cmd, text);
    }
    strcat(cmd, "\r\n");

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 50);
        /* Skip the first line and parse all the entries */
        for (i = 1; i < numLines; i++) {
            if (*numEntries >= maxEntries)
                break;
            numTokens = AtLibGs_ParseIntoTokens(lines[i], ',', tokens, 20);
            if (numTokens == 6) {
                /* Got a line, store it in the structure */
                strncpy(entry->bssid, tokens[0], ATLIBGS_BSSID_MAX_LENGTH);
                strncpy(entry->ssid, tokens[1], ATLIBGS_SSID_MAX_LENGTH);
                entry->channel = atoi(tokens[2]);
                if (strcmp(tokens[3], "ADHOC") == 0)
                    entry->station = ATLIBGS_STATIONMODE_AD_HOC;
                else
                    entry->station = ATLIBGS_STATIONMODE_INFRASTRUCTURE;
                entry->signal = atoi(tokens[4]);
                entry->security = AtLibGs_ParseSecurityMode(tokens[5]);
                entry++;
                (*numEntries)++;
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisAssoc2
 *--------------------------------------------------------------------------*/
/**   Dissociate from the current network
 *      Sends the command:
 *          ATH
 *
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc2(void)
{
    return AtLibGs_CommandSendString("ATH\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetTransmitRate
 *--------------------------------------------------------------------------*/
/**   Returns the current wireless transmit rate
 *      Sends the command:
 *          AT+WRATE=?
 *
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetTransmitRate(void)
{
    return AtLibGs_CommandSendString("AT+WRATE=?\r\n");
    /* TODO: Need to parse wireless rate! */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetClientInfo
 *--------------------------------------------------------------------------*/
/**   Get the information about the clients associated to the adapter
 *       when it act as a Limited AP.
 *      Sends the command:
 *          AT+APCLIENTINFO=?
 *     example output
 *          No.Of Stations Connected=1
 *          No     MacAddr                   IP
 *          1      2c:44:01:c5:e7:df         192.168.1.2
 *
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetClientInfo(void)
{
    char *lines[3];
    ATLIBGS_MSG_ID_E rxMsgId;
    rxMsgId = AtLibGs_CommandSendString("AT+APCLIENTINFO=?\r\n");

    /* TODO: What is this to do? */
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 3) >= 1)) {
#ifdef ATLIBGS_DEBUG_ENABLE
        /* do something with the client info */
        ConsolePrintf("L1: %s\r\n", lines[0]);
        ConsolePrintf("L2: %s\r\n", lines[1]);
        ConsolePrintf("L3: %s\r\n", lines[2]);
#endif
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetAuthentictionMode
 *--------------------------------------------------------------------------*/
/**   Sets the Authentication mode when using WEP, set to 0 for WPA/WPA2
 *      Sends the command:
 *          AT+WAUTH=N
 *  @param [in] authmode N=0(None/WPA), N=1(Open WEP), N=2(Shared WEP)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_E authmode)
{
    char cmd[20];

    sprintf(cmd, "AT+WAUTH=" _F8_ "\r\n", authmode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWEPKey
 *--------------------------------------------------------------------------*/
/**   Sets WEP keys 1-4, key must be 10 or 26 HEX digits
 *      Sends the command:
 *          AT+WWEPn=[key] where [key] is the 10 or 26 digit WEP key
 *  @param [in] keynum n = 1-4, 
 *  @param [in] key 10 or 26 HEX digit key (use NULL terminated char array)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWEPKey(ATLIBGS_WEPKEY_E keynum, char key[])
{
    char cmd[20];

    sprintf(cmd, "AT+WWEP" _F8_ "=%s\r\n", keynum, key);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWPAKey
 *--------------------------------------------------------------------------*/
/**   Sets the WPA2 pre-shared key to the [PSK]. After the PSK has been
 *      entered, the commands AT&W (to save the relevant profile) and AT&Y
 *      (to ensure that the profile containing the new PSK is the default
 *      profile) should be issued. The PSK will then be available when the
 *      adapter awakens from Standby. Refer to sections 0 and 4.6.3 for more
 *      information on profile management.
 *      Sends the command:
 *          AT+WPSK=[key] where [key] is the 32 byte NULL terminated key
 *  @param [in] key 32 byte NULL terminated char array
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWPAKey(char key[])
{
    char cmd[20];
    sprintf(cmd, "AT+WPSK=%s\r\n", key);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetListenBeaconInterval
 *--------------------------------------------------------------------------*/
/**   Sets the Listen Beacon Interval
 *      Sends the command:
 *        AT+WIEEEPSPOLL=n[,I]: n is 0 or 1, and I is a 16 bit int iff n == 1
 *  @param [in] enable
 *  @param [in] interval 16bit unsigned int
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetListenBeaconInterval(bool enable, uint16_t interval)
{
    char cmd[30];
    if (enable) {
        sprintf(cmd, "AT+WIEEEPSPOLL=1," _F16_ "\r\n", interval);
    } else {
        sprintf(cmd, "AT+WIEEEPSPOLL=0\r\n");
    }
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTransmitRate
 *--------------------------------------------------------------------------*/
/**   Sets the Transmit rate
 *      Sends the command:
 *          AT+WRATE=n, where n is the integer corresponding to the rate
 *  @param [in] transrate [0(Auto),2(1mbps),4(2mbps),11(5.5mbps),22(11mbps)]
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitRate(ATLIBGS_TRANSRATE_E transrate)
{
    char cmd[20];
    sprintf(cmd, "AT+WRATE=" _F8_ "\r\n", transrate);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableDHCPServer
 *--------------------------------------------------------------------------*/
/**   Enables the DHCP server
 *      Sends the command:
 *          AT+DHCPSRVR=1

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDHCPServer(void)
{
    return AtLibGs_CommandSendString("AT+DHCPSRVR=1\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisableDHCPServer
 *--------------------------------------------------------------------------*/
/**   Disables the DHCP server
 *      Sends the command:
 *          AT+DHCPSRVR=0

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisableDHCPServer(void)
{
    return AtLibGs_CommandSendString("AT+DHCPSRVR=0\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisableDNSServer
 *--------------------------------------------------------------------------*/
/**   Disables the DNS server
 *      Sends the command:
 *          AT+DNS=0

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisableDNSServer()
{
    return AtLibGs_CommandSendString("AT+DNS=0\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableDNSServer
 *--------------------------------------------------------------------------*/
/**   Disables the DNS server
 *      Sends the command:
 *          AT+DNS=1,[server] where [server] is the NULL terminated server
 *  @param [in] server NULL terminated char array
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDNSServer(char server[])
{
    char cmd[50];

    sprintf(cmd, "AT+DNS=1,%s\r\n", server);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEAPConfiguration
 *--------------------------------------------------------------------------*/
/** Configures EAP security
 *    Sends the command:
 *     AT+WEAPCONF=[Outer Authentication],[Inner Authentication],[user],[pass]
 *  @param [in] oa -- Outer Authentication
 *  @param [in] ia Inner Authentication
 *  @param [in] user -- username
 *  @param [in] pass -- password (both 1-32 char ASCII str)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEAPConfiguration(
        ATLIBGS_INOUAUTH_E oa,
        ATLIBGS_INOUAUTH_E ia,
        char user[],
        char pass[])
{
    char cmd[80];

    sprintf(cmd, "AT+WEAPCONF=" _F8_ "," _F8_ ",%s,%s\r\n", oa, ia, user, pass);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_AddSSLCertificate
 *--------------------------------------------------------------------------*/
/** Adds an SSL certificate, with ASCII name, bin/hex format, size, and
 *    location (flash/ram). To add the certificate you need to...
 *    Sends the command:
 *     AT+TCERTADD=[name],[Inner Authentication],[user],[pass]
 *  @param [in] name (char ASCII str)
 *  @param [in] hex binary(0)/hex(1)
 *  @param [in] size 16bit uint size
 *  @param [in] ram flash(0)/ram(1)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_AddSSLCertificate(
        char name[],
        bool hex,
        uint16_t size,
        bool ram)
{
    char cmd[80];

    sprintf(cmd, "AT+TCERTADD=%s," _F8_ "," _F16_ "," _F8_ "\r\n", name, hex,
            size, ram);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DeleteSSLCertificate
 *--------------------------------------------------------------------------*/
/** Deletes named SSL Certificate
 *    Sends the command:
 *     AT+TCERTDEL=[name]
 *  @param [in] name (char ASCII str)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DeleteSSLCertificate(char name[])
{
    char cmd[50];

    /* TODO: Does this require a comma? */
    sprintf(cmd, "AT+TCERTDEL=%s,\r\n", name);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetSecurity
 *--------------------------------------------------------------------------*/
/** Sets the Security mode
 *    Sends the command:
 *     AT+WSEC=n
 *  @param [in] security mode (see ATLIBGS_SECURITYMODE_E for values)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetSecurity(ATLIBGS_SECURITYMODE_E security)
{
    char cmd[20];

    sprintf(cmd, "AT+WSEC=" _F16_ "\r\n", security);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SSLOpen
 *--------------------------------------------------------------------------*/
/** Opens SSL Connection
 *    Sends the command:
 *     AT+SSLOPEN=[cid],[cert name]
 *  @param [in] cid
 *  @param [in] name (certificate name)(from AtLibGs_AddSSLCertificate)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SSLOpen(uint8_t cid, char name[])
{
    char cmd[50];

    sprintf(cmd, "AT+SSLOPEN=%c,%s\r\n", cid, name);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SSLClose
 *--------------------------------------------------------------------------*/
/** Closes SSL Connection
 *    Sends the command:
 *     AT+SSLCLOSE=[cid],[cert name]
 *  @param [in] cid
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SSLClose(uint8_t cid)
{
    char cmd[20];

    sprintf(cmd, "AT+SSLCLOSE=%c\r\n", cid);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPConf
 *--------------------------------------------------------------------------*/
/** Configures HTTP client
 *    Sends the command:
 *     AT+HTTPCONF=[Param],[Value]
 *  @param [in] param
 *  @param [in] value 
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPConf(ATLIBGS_HTTPCLIENT_E param, char value[])
{
    char cmd[30];

    sprintf(cmd, "AT+HTTPCONF=" _F16_ ",", param);
    App_Write(cmd, strlen(cmd));
    App_Write(value, strlen(value));
    App_Write("\r\n", 2);
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf(">%s%s\n", cmd, value);
#endif
    /* Wait for the response while collecting data into the MRBuffer */
    return AtLibGs_ResponseHandle(ATLIBGS_INVALID_CID);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPConfDel
 *--------------------------------------------------------------------------*/
/** Deletes config param from HTTP client
 *    Sends the command:
 *     AT+HTTPCONFDEL=[Param]
 *  @param [in] param
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPConfDel(ATLIBGS_HTTPCLIENT_E param)
{
    char cmd[30];

    sprintf(cmd, "AT+HTTPCONFDEL=" _F16_ "\r\n", param);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPClose
 *--------------------------------------------------------------------------*/
/** Closes the HTTP Client connection
 *    Sends the command:
 *     AT+HTTPCLOSE=[cid]
 *  @param [in] cid
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPClose(uint8_t cid)
{
    char cmd[30];

    /* TODO: Is CID a character or a number? */
    sprintf(cmd, "AT+HTTPCLOSE=%c\r\n", cid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RawETHFrameConf
 *--------------------------------------------------------------------------*/
/** Delet
 *    Sends the command:
 *     AT+NRAW=n
 *  @param [in] enable Raw ethernet frame enable flag
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RawETHFrameConf(ATLIBGS_RAW_ETH_E enable)
{
    char cmd[20];

    sprintf(cmd, "AT+NRAW=" _F8_ "\r\n", enable);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BulkDataTrans
 *--------------------------------------------------------------------------*/
/** Enables/Disables bulk transfer mode
 *    Sends the command:
 *     AT+BDATA=n
 *  @param [in] enable 0 (disable)/ 1 (enable)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BulkDataTrans(bool enable)
{
    char cmd[20];

    sprintf(cmd, "AT+BDATA=" _F8_ "\r\n", enable);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetBatteryCheckFreq
 *--------------------------------------------------------------------------*/
/** Sets the battery check freq
 *    Sends the command:
 *     AT+BCHK=n
 *  @param [in] freq -- Number of seconds between battery readings
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetBatteryCheckFreq(uint16_t freq)
{
    char cmd[20];

    sprintf(cmd, "AT+BCHK=" _F16_ "\r\n", freq);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetBatteryCheckFreq
 *--------------------------------------------------------------------------*/
/** Sets the battery check freq
 *    Sends the command:
 *     AT+BCHK=n
 *  @param [in] freq -- Number of seconds between battery readings
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryCheckFreq(uint16_t *freq)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1] = { 0 };

    rxMsgId = AtLibGs_CommandSendString("AT+BCHK=?\r\n");

    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if (AtLibGs_ParseIntoLines(MRBuffer, lines, 1) >= 1) {
            *freq = atoi(lines[0]);
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryValGet
 *--------------------------------------------------------------------------*/
/**   Sets the battery check freq
 *    Sends the command:
 *     AT+BATTVALGET
 *    This command should return a message with the latest value,
 *    e.g. Battery Value: 3.4 V, followed by the standard command response.
 *    If this command is issued before issuing the command to start battery
 *    checks, it returns ERROR or 1, depending on the current verbose setting.

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryValue(void)
{
    /* TODO: Need to return full value */
    return AtLibGs_CommandSendString("AT+BATTVALGET\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnterDeepSleep
 *--------------------------------------------------------------------------*/
/**   Enters Deep sleep mode for givin amount of milliseconds
 *    Sends the command:
 *     AT+PSDPSLEEP=[milliseconds]
 *  @param [in] milliseconds
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnterDeepSleep(uint32_t milliseconds)
{
    char cmd[30];
    /* TODO: Does this really take a number of miliseconds? */
    sprintf(cmd, "AT+PSDPSLEEP=" _F32_ "\r\n", milliseconds);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_AssocPowerSaveMode
 *--------------------------------------------------------------------------*/
/** Configure 802.11 Power Save Mode to be used during the association.
 *    Sends the command:
 *     AT+WAPSM=[value]
 *  @param [in] mode (modes 0-3)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_AssocPowerSaveMode(ATLIBGS_ASSOCPOWMODE_E mode)
{
    char cmd[20];

    sprintf(cmd, "AT+WAPSM=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTransmitPower
 *--------------------------------------------------------------------------*/
/** Sets the internal/external transmit power level
 *    Sends the command:
 *       AT+WP=[value]
 *  @param [in] power level (int 0-7, ext 2-15)**Lower number == higher PWR**
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitPower(uint8_t power)
{
    char cmd[20];

    sprintf(cmd, "AT+WP=" _F8_ "\r\n", power);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartAutoConnect
 *--------------------------------------------------------------------------*/
/** Start Auto Connect, including association.
 *    Sends the command:
 *       ATA

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATA\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ConfigAPAutoConnect
 *--------------------------------------------------------------------------*/
/** Start Auto Connect using existing association.
 *    Sends the command:
 *     ATA2

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ConfigAPAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATA2\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReturnAutoConnect
 *--------------------------------------------------------------------------*/
/** Return to a previous Auto Connect session; returns an error if no such
 *    session exists.
 *    Sends the command:
 *     ATO

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReturnAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATO\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WebProv
 *--------------------------------------------------------------------------*/
/** Provisioning through web pages
 *    Sends the command:
 *     AT+WEBPROV=
 *  @param [in] username
 *  @param [in] password (NULL terminated char array, 16 char limit)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WebProv(char username[], char password[])
{
    char cmd[80];

    sprintf(cmd, "AT+WEBPROV=%s,%s\r\n", username, password);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WebLogoAdd
 *--------------------------------------------------------------------------*/
/** The adapter supports adding the Logo that will appear on the web pages
 *      used for provisioning. [size] is measured in bytes and the maximum size
 *      is 1788 bytes. This command is typically done at the manufacturing line
 *      in the factory. This command can be done ONLY once. There is no command
 *      to delete the Logo. This command returns standard command response
 *      (section 4) to the serial interface.
 *    Sends the command:
 *     AT+WEBLOGOADD=[size]
 *           [Esc]L[Actual File content]
 *  @param [in] size
 *  @param [in] txBuf -- pointer to transmit buffer
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WebLogoAdd(uint16_t size,
                                    const void *txBuf)
{
    char cmd[25];
    sprintf(cmd, "AT+WEBLOGOADD=" _F16_ "\r\n", size);
    AtLibGs_CommandSendString(cmd);
    // Construct the data start indication message 
    sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'L');
    // Now send the data START indication message  to S2w node 
    App_Write(cmd, 2);
    // Now send the actual image data data 
    AtLibGs_DataSend(txBuf, size);
    MSTimerDelay(10000); // wait 10 seconds for programming to finish.
    return ATLIBGS_MSG_ID_OK;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFAsyncFrameRX
 *--------------------------------------------------------------------------*/
/**  Enable the asynchronous frame reception
 *    Sends the command:
 *     AT+RFRXSTART=[channel]
 *  @param [in] channel
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFAsyncFrameRX(uint16_t channel)
{
    char cmd[20];

    sprintf(cmd, "AT+RFRXSTART=" _F16_ "\r\n", channel);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFStop
 *--------------------------------------------------------------------------*/
/**  Stop any of the RF tests transmission/reception
 *    Sends the command:
 *     AT+RFSTOP

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFStop(void)
{
    char cmd[30];

    sprintf(cmd, "AT+RFSTOP\r\n");

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SPIConf
 *--------------------------------------------------------------------------*/
/**  The command to set the SPI clock phase and clock polarity parameter.
 *      If clock polarity is 0, then inactive state of serial clock is low.
 *      If clock polarity is 1, then inactive state of serial clock is high.
 *      If clock phase is 0, then data is captured on the first toggling edge of the
 *      serial clock (clock phase zero), after the falling edge of slave select signal.
 *      If clock phase is 1, then data is captured on the second edge of the serial
 *      clock (clock phase 180), after the falling edge of slave select signal.
 *      Default is clock polarity 0 and clock phase 0. The new SPI parameters take
 *      effect after node reset/restart. However, they are stored in RAM and will be
 *      lost when power is lost unless they are saved to a profile using AT&W
 *      (section 4.6.1). The profile used in that command must also be set as the
 *      power-on profile using AT&Y (section 4.6.3).
 *    Sends the command:
 *     AT+SPICONF=[polarity],[phase]
 *  @param [in] polarity -- polarity of SPI transmissions
 *  @param [in] phase -- phase of SPI transmissions
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SPIConf(
        ATLIBGS_SPI_POLARITY_E polarity,
        ATLIBGS_SPI_PHASE_E phase)
{
    char cmd[30];

    sprintf(cmd, "AT+SPICONF=" _F8_ "," _F8_ "\r\n", polarity, phase);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Reset
 *--------------------------------------------------------------------------*/
/**   Resets the module
 *      Sends the command:
 *          AT+RESET

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Reset(void)
{
    return AtLibGs_CommandSendString("AT+RESET\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_MemoryTrace
 *--------------------------------------------------------------------------*/
/**  Sends memory trace to serial interface
 *    Sends the command:
 *     AT+MEMTRACE
 *
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_MemoryTrace(void)
{
    /* TODO: Can we parse this? */
    return AtLibGs_CommandSendString("AT+MEMTRACE\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartFirmwareUpdate
 *--------------------------------------------------------------------------*/
/**  Starts the firmware update on either the APP FWs, the WLAN FW, or all 3
 *    Sends the command:
 *     AT+SOTAFWUPSTART=[firmwaretoupdate]
 *  @param [in] bins -- enum to set which firmwares are being updated
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartFirmwareUpdate(ATLIBGS_FIRMWARE_BINARIES_E bins)
{
    char cmd[30];
    sprintf(cmd, "AT+SOTAFWUPSTART=" _F16_ "\r\n", bins);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetTime
 *--------------------------------------------------------------------------*/
/**  Gets the system time. shown formatted[dd/mm/yyyy],[HH:MM:SS],
 *     followed by system time in milliseconds since epoch(1970)
 *    Sends the command:
 *      AT+GETTIME=?

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetTime(void)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+GETTIME=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
        /* TODO: Parse the time! */
        //ConsolePrintf("Time: %s\r\n", lines[0]);
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetGPIO
 *--------------------------------------------------------------------------*/
/**  Sets 1 of 4 GPIOs high or low, unless it is being used (for SPI/UART)
 *    Sends the command:
 *     AT+DGPIO=[GPIO number],0/1
 *  @param [in] gpio number, high/low (1/0) setting
 *  @param [in] state
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetGPIO(
        ATLIBGS_GPIO_PIN_E gpio,
        ATLIBGS_GPIO_STATE_E state)
{
    char cmd[20];

    sprintf(cmd, "AT+DGPIO=" _F16_ "," _F8_ "\r\n", gpio, state);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableEnhancedAsync
 *--------------------------------------------------------------------------*/
/**  Sets enhanced asynchronous notifications on if true.
 *    Sends the command:
 *     AT+ASYNCMSGFMT=n, where n is 0/1
 *     NOTE: THe AtLibGs library does not directly support enhanced
 *      asynchronous notifications.
 *  @param [in] on -- If true then enable, else disable.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableEnhancedAsync(bool on)
{
    char cmd[20];

    sprintf(cmd, "AT+ASYNCMSGFMT=" _F8_ "\r\n", (on) ? 1 : 0);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableVerbose
 *--------------------------------------------------------------------------*/
/**   Sets verbose on or off
 *      Sends the command:
 *        ATVn, where n is 0/1
 *  @param [in] on -- If true then enable, else disable.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableVerbose(bool on)
{
    char cmd[20];

    sprintf(cmd, "ATV" _F8_ "\r\n", (on) ? 1 : 0);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetNetworkConfiguration
 *--------------------------------------------------------------------------*/
/**  Gets the current network configuration status
 *    Sends the command:
 *      AT+WSTATUS

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkConfiguration(void)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+WSTATUS\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
        /* do something with the status */
        //ConsolePrintf("Network Config Status: %s\r\n", lines[0]);
        /* TODO: Parse this response! */
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FWUpgradeConfig
 *--------------------------------------------------------------------------*/
/** Configures parameters for FW update
 *    Sends the command:
 *       AT+SOTAFWUPCONF=[param],[value]
 *  @param [in] param
 *  @param [in] value (string)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_FWUpgradeConfig(
        ATLIBGS_FWUPGRADEPARAM_E param,
        char value[])
{
    char cmd[80];

    sprintf(cmd, "AT+SOTAFWUPCONF=" _F8_ ",%s\r\n", param, value);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetCIDInfo
 *--------------------------------------------------------------------------*/
/**  Returns list of CIDs configured
 *    Sends the command:
 *     AT+CID=?

 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetCIDInfo(void)
{
    /* TODO: Parse CID response! */
    return AtLibGs_CommandSendString("AT+CID=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatterySetWarnLevel
 *--------------------------------------------------------------------------*/
/**   Set the battery warning/standby level to enable the adaptor’s
 *      internal battery measuring logic
 *    Sends the command:
 *     AT+BATTLVLSET=
 *  @param [in] level -- warning level (mV)
 *  @param [in] freq -- warning freq
 *  @param [in] standby -- standby level (mV)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatterySetWarnLevel(
        uint16_t level,
        uint16_t freq,
        uint16_t standby)
{
    char cmd[30];

    sprintf(cmd, "AT+BATTLVLSET=" _F16_ "," _F16_ "," _F16_ "\r\n", level,
            freq, standby);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEAPCertificate
 *--------------------------------------------------------------------------*/
/**   Configure certificate for EAP-TLS
 *      Sends the command:
 *         AT+WEAP=[ Type ],[ Format ],[ Size ],[ Location ]
 *  @param [in] cert CA certificate(0)/ Client certificate(1)/ Private Key(2),
 *  @param [in] binhex    Binary(0)/Hex(1),
 *  @param [in] size of the file to be transferred, 
 *  @param [in] flashram Flash(0)/Ram(1)
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEAPCertificate(
        ATLIBGS_EAPTLS_E cert,
        ATLIBGS_EAPTLS_E binhex,
        uint16_t size,
        ATLIBGS_EAPTLS_E flashram)
{
    char cmd[50];

    sprintf(cmd, "AT+WEAP=" _F8_ "," _F8_ "," _F16_ "," _F8_ "\r\n", cert,
            binhex, size, flashram);

    return AtLibGs_CommandSendString(cmd);
    /* TODO: Send certificate [ESC] W sequence after this! */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Ping
 *--------------------------------------------------------------------------*/
/** Sends a ping (section 4.16.13 in programmer guide)
 *    Sends the command:
 *       AT+PING=[Ip],([Trails],[timeout],[Len],[TOS],[TTL],[PAYLOAD])
 *    Example output:
 *      Pinging for 192.168.0.100 with 56 bytes of data
 *      OK
 *      Reply from 192.168.0.100: bytes=56 time=118 ms TTL 30
 *      Ping Statistics for 192.168.0.100:
 *           Packets: Sent = 1, Received = 1, Lost = 0 percent
 *      Approximate round trip times in milliseconds
 *           Minimum = 2ms, Maximum = 118ms, Average = 26ms
 *  @param [in] ip
 *  @param [in] trails
 *  @param [in] timeout
 *  @param [in] len
 *  @param [in] tos
 *  @param [in] ttl
 *  @param [in] payload
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Ping(
        char ip[],
        uint16_t trails,
        uint16_t timeout,
        uint16_t len,
        uint8_t tos,
        uint8_t ttl,
        char payload[])
{
    char cmd[80];

    sprintf(cmd,
            "AT+PING=%s" _F16_ "," _F16_ "," _F16_ "," _F8_ "," _F8_ ",%s\r\n",
            ip, trails, timeout, len, tos, ttl, payload);

    return AtLibGs_CommandSendString(cmd);
    /* TODO: Need some type of callback per ping or at least the summary */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TraceRoute
 *--------------------------------------------------------------------------*/
/**  Starts a trace route sequence
 *     Sends the command:
 *        AT+TRACEROUTE=[Ip],([Interval],[MaxHops],[MinHops],[TOS])
 *  @param [in] ip
 *  @param [in] interval
 *  @param [in] maxhops
 *  @param [in] minhops
 *  @param [in] tos
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TraceRoute(
        char ip[],
        uint16_t interval,
        uint8_t maxhops,
        uint8_t minhops,
        uint8_t tos)
{
    char cmd[80];

    sprintf(cmd, "AT+TRACEROUTE=%s" _F16_ "," _F8_ "," _F8_ "," _F8_ "\r\n",
            ip, interval, maxhops, minhops, tos);
    return AtLibGs_CommandSendString(cmd);
    /* TODO: Need some type of callback per traceroute or at least the summary */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SNTPsync
 *--------------------------------------------------------------------------*/
/**   Sets adapter time using namer server
 *    Sends the command:
 *     AT+NTIMESYNC= [Enable],([Server IP],[Timeout],[Periodic],[frequency])
 *  @param [in] enable
 *  @param [in] ip
 *  @param [in] timeout
 *  @param [in] periodic
 *  @param [in] frequency
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SNTPsync(
        bool enable,
        char ip[],
        uint8_t timeout,
        bool periodic,
        uint32_t frequency)
{
    char cmd[30];

    sprintf(cmd, "AT+NTIMESYNC=" _F8_ ",%s," _F8_ "," _F8_ "," _F32_ "\r\n",
            enable, ip, timeout, periodic, frequency);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetSocketOptions
 *--------------------------------------------------------------------------*/
/**   Configures a socket by CID
 *    Sends the command:
 *     AT+SETSOCKOPT=[CID],[Type],[Parameter],[Value],[Length]
 *  @param [in] cid -- CID
 *  @param [in] type -- Type of socket option
 *                    (socket, IP, TCP)
 *  @param [in] param -- Parameter type being set
 *                  (TCP MAXRT, TCP_KA, SO_KA, TCO_KA_CNT)
 *  @param [in] value -- Value to be set in seconds
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetSocketOptions(
        uint8_t cid,
        ATLIBGS_SOCKET_OPTION_TYPE_E type,
        ATLIBGS_SOCKET_OPTION_PARAM_E param,
        uint32_t value)
{
    char cmd[80];
    const uint16_t length = 4;

    sprintf(
            cmd,
            "AT+SETSOCKOPT=%c," _F16_ "," _F16_ "," _F32_ "," _F16_ "\r\n",
            cid, type, param, value, length);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPOpen
 *--------------------------------------------------------------------------*/
/**   Open and HTTP client connection
 *      Sends the command:
 *          AT+HTTPOPEN=[host](, [Port Number], [SSL Flag], [certificate name],
 *          [proxy],[Connection Timeout])
 *     returns cid of http client on success
 *  @param [in] host -- Host address to connect to
 *  @param [in] port -- Port on host to connect to
 *  @param [in] sslenabled -- true if using a SSL certificate
 *  @param [in] certname -- name of certificate (if any)
 *  @param [in] proxy -- name of proxy (if any)
 *  @param [in] timeout -- Amount of time to try opening (in seconds)
 *  @param [in] cid -- CID
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPOpen(
        char host[],
        uint16_t port,
        bool sslenabled,
        char certname[],
        char proxy[],
        uint16_t timeout,
        uint8_t *cid)
{
    char cmd[80];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[4];
    uint8_t numLines;

    sprintf(cmd, "AT+HTTPOPEN=%s," _F16_ "," _F8_ ",%s,%s," _F16_ "\r\n", host,
            port, (sslenabled) ? 1 : 0, certname, proxy, timeout);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 4);
        if (numLines >= 2) {
            // Convert the returned CID into a number
            *cid = *lines[1];
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPSend
 *--------------------------------------------------------------------------*/
/**   Get/post data on HTTP client connection
 *      Sends the command:
 *          AT+HTTPSEND=[CID],[Type],[Timeout],[Page](,[Size of the content])
 *  @param [in] cid
 *  @param [in] type
 *  @param [in] timeout
 *  @param [in] page
 *  @param [in] size -- Size of the content
 *  @param [in] txBuf -- pointer to transmit buffer
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPSend(
        uint8_t cid,
        ATLIBGS_HTTPSEND_E type,
        uint16_t timeout,
        char page[],
        uint16_t size,
        const void *txBuf)
{
    char cmd[50];
    ATLIBGS_MSG_ID_E msg = ATLIBGS_MSG_ID_INVALID_INPUT;
    if (ATLIBGS_INVALID_CID != cid) {
        // Are we still a valid connection?
    	int cidIndex = AtLibGs_CIDToIndex(cid);
        if (G_cidState[cidIndex].iState != CID_STATE_CONNECTED)
        	return ATLIBGS_MSG_ID_DISCONNECT;

#ifdef ATLIBGS_DEBUG_ENABLE
        ConsolePrintf(">AT+HTTPSEND=%c," _F8_ "," _F16_ ",", cid, type,
                timeout);
        ConsolePrintf("%s," _F16_ "\r\n", page, size);
#endif
        sprintf(cmd, "AT+HTTPSEND=%c," _F8_ "," _F16_ ",", cid, type,
                timeout);
        App_Write(cmd, strlen(cmd));
        App_Write(page, strlen(page));
        sprintf(cmd, "," _F16_ "\r\n", size);
        App_Write(cmd, strlen(cmd));
        msg = AtLibGs_ResponseHandle(cid);
        if (msg == ATLIBGS_MSG_ID_OK) {
            /* Construct the data start indication message */
            sprintf(cmd, "%c%c%c", ATLIBGS_ESC_CHAR, 'H', cid);
            /* Now send the data START indication message  to S2w node */
            App_Write(cmd, 3);
            /* Now send the actual data */
            AtLibGs_DataSend(txBuf, size);
        }
    }
    return msg;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UnsolicitedTXRate
 *--------------------------------------------------------------------------*/
/**   Enables unsolicited data transmission
 *    Sends the command:
 *     AT+UNSOLICITEDTX=[Frame Control],[Sequence Control],[Channel], [Rate],[WmmInfo],
 *             [Receiver Mac],[Bssid of AP],[Frame Length]
 *  @param [in] frame -- Frame Control
 *  @param [in] seq -- Sequence Control
 *  @param [in] chan -- channel
 *  @param [in] rate
 *  @param [in] wmm -- WmmInfo
 *  @param [in] mac -- Receiver Mac
 *  @param [in] bssid -- Bssid of AP
 *  @param [in] len -- Frame Length
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UnsolicitedTXRate(
        uint16_t frame,
        uint16_t seq,
        uint8_t chan,
        ATLIBGS_UNSOLICITEDTX_E rate,
        uint32_t wmm,
        char mac[],
        char bssid[],
        uint16_t len)
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+UNSOLICITEDTX=" _F16_ "," _F16_ "," _F8_ "," _F8_ 
			"," _F32_ ",%s,%s," _F16_ "\r\n",
            frame, seq, chan, rate, wmm, mac, bssid, len);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFAsyncFrameTX
 *--------------------------------------------------------------------------*/
/**   Enable asynch frame transmission
 *     Sends the command:
 *     AT+RFFRAMETXSTART=[Channel],[Power],[Rate],
 *       [No.Of.Times],[Fr.Intrvel],[FrameControl],
 *        [DurationId],[Sequence Control],[frameLen],
 *          [Preamble],[Scrambler],[DstMac],[Src Mac]
 *  @param [in] channel
 *  @param [in] power
 *  @param [in] rate
 *  @param [in] times -- No.Of.Times
 *  @param [in] interval -- Fr.Intrvel
 *  @param [in] frameControl
 *  @param [in] durationId
 *  @param [in] sequenceControl
 *  @param [in] frameLen
 *  @param [in] shortPreamble
 *  @param [in] scramblerOff
 *  @param [in] dstMac
 *  @param [in] srcMac
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFAsyncFrameTX(
        uint8_t channel,
        uint8_t power,
        ATLIBGS_RFFRAMETXSTART_E rate,
        uint16_t times,
        uint16_t interval,
        uint8_t frameControl,
        uint16_t durationId,
        uint16_t sequenceControl,
        uint16_t frameLen,
        bool shortPreamble,
        bool scramblerOff,
        char dstMac[],
        char srcMac[])
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+RFFRAMETXSTART=" _F8_ "," _F8_ "," _F8_ "," _F16_ "," _F16_ 
			"," _F8_ "," _F16_ "," _F16_ "," _F16_ "," _F8_ "," _F8_ ",%s,%s\r\n",
            channel, power, rate, times, interval, frameControl, durationId,
            sequenceControl, frameLen, shortPreamble, scramblerOff, dstMac,
            srcMac);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFWaveTXStart
 *--------------------------------------------------------------------------*/
/**   Enable modulated/unmodulated wave transmission
 *    Sends the command:
 *     AT+RFWAVETXSTART=[Modulated],[Channel],[Rate],
 *     [PreambleLong],[ScamblerOff],[Cont.Tx],
 *      [Power],[Ssid]
 *  @param [in] isModulated
 *  @param [in] channel
 *  @param [in] rate
 *  @param [in] longPreamble
 *  @param [in] scramblerOn
 *  @param [in] continousWaveRX 
 *  @param [in] power
 *  @param [in] ssid
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFWaveTXStart(
        bool isModulated,
        uint16_t channel,
        ATLIBGS_RFWAVETXSTART_E rate,
        bool longPreamble,
        bool scramblerOn,
        bool continousWaveRX,
        uint8_t power,
        char ssid[])
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+RFWAVETXSTART=" _F8_ "," _F16_ "," _F8_ "," _F8_ "," _F8_ 
			"," _F8_ "," _F8_ "%s\r\n",
            isModulated, channel, rate, longPreamble, scramblerOn,
            continousWaveRX, power, ssid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetNetworkStatus
 *--------------------------------------------------------------------------*/
/**   Get the network status and return in a structure
 *  @param [in] pStatus -- Pointer to structure status to fill.
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkStatus(ATLIBGS_NetworkStatus *pStatus)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[10];
    uint8_t numLines;
    char *tokens[10];
    uint8_t numTokens;
    char *values[2];
    uint8_t numValues;
    uint8_t i, t;
    uint8_t rx = 0;

    memset(pStatus, 0, sizeof(*pStatus));
    rxMsgId = AtLibGs_CommandSendString("AT+NSTAT=?\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 10);
        for (i = 0; i < numLines; i++) {
            numTokens = AtLibGs_ParseIntoTokens(lines[i], ' ', tokens, 10);
            for (t = 0; t < numTokens; t++) {
                numValues = AtLibGs_ParseIntoTokens(tokens[t], '=', values, 2);
                if (numValues == 2) {
                    if (strcmp(values[0], "MAC") == 0) {
                        strcpy(pStatus->mac, values[1]);
                    } else if (strcmp(tokens[t], "WSTATE") == 0) {
                        if (strcmp(values[1], "CONNECTED") == 0)
                            pStatus->connected = 1;
                    } else if (strcmp(values[0], "BSSID") == 0) {
                        strcpy(pStatus->bssid, values[1]);
                    } else if (strcmp(values[0], "SSID") == 0) {
                        strncpy(pStatus->ssid, values[1] + 1, strlen(values[1])
                                - 2);
                    } else if (strcmp(values[0], "CHANNEL") == 0) {
                        pStatus->channel = atoi(values[1]);
                    } else if (strcmp(values[0], "RSSI") == 0) {
                        pStatus->signal = atoi(values[1]);
                    } else if (strcmp(values[0], "SECURITY") == 0) {
                        pStatus->security
                                = AtLibGs_ParseSecurityMode(values[1]);
                    } else if (strcmp(values[0], /* IP */"addr") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->addr);
                    } else if (strcmp(values[0], "SubNet") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->subnet);
                    } else if (strcmp(values[0], "Gateway") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->gateway);
                    } else if (strcmp(values[0], "DNS1") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->dns1);
                    } else if (strcmp(values[0], "DNS2") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->dns2);
                    } else if (strcmp(values[0], "Count") == 0) {
                        if (rx) {
                            pStatus->rxCount = atoi(values[1]);
                        } else {
                            pStatus->txCount = atoi(values[1]);
                        }
                    }
                } else if (numValues == 1) {
                    if (strcmp(values[0], "Rx") == 0) {
                        rx = 1;
                    } else if (strcmp(values[0], "Tx") == 0) {
                        rx = 0;
                    }
                }
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseSecurityMode
 *--------------------------------------------------------------------------*/
/**   Convert a security mode string into a security enumerated type.
 *  @param [in] string -- String with security mode
 *  @return     ATLIBGS_SECURITYMODE_E -- Final generated error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_SECURITYMODE_E AtLibGs_ParseSecurityMode(const char *string)
{
    if (strcmp(string, "WPA2-PERSONAL") == 0) {
        return ATLIBGS_SMWPA2PSK;
    } else if (strcmp(string, "WPA-PERSONAL") == 0) {
        return ATLIBGS_SMWPAPSK;
    } else if (strcmp(string, "WPA-ENTERPRISE") == 0) {
        return ATLIBGS_SMWPAE;
    } else if (strcmp(string, "WPA2-ENTERPRISE") == 0) {
        return ATLIBGS_SMWPA2E;
    } else if (strncmp(string, "WEP", 3) == 0) {
        return ATLIBGS_SMWEP;
    } else if (strcmp(string, "NONE") == 0) {
        return ATLIBGS_SMOPEN;
    }
    return ATLIBGS_SM_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIPAddress
 *--------------------------------------------------------------------------*/
/**   Take the given string and parse into ip numbers
 *  @param [in] string -- String with IP number
 *  @param [in] ip -- Structure to receive parsed IP number
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_ParseIPAddress(const char *string, ATLIBGS_IP *ip)
{
    int v1, v2, v3, v4;

    /* Currently only parses ipv4 addresses */
    sscanf(string, _F8_ "." _F8_ "." _F8_ "." _F8_, &v1, &v2, &v3, &v4);
    ip->ipv4[0] = v1;
    ip->ipv4[1] = v2;
    ip->ipv4[2] = v3;
    ip->ipv4[3] = v4;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetWebProvSettings
 *--------------------------------------------------------------------------*/
/**   Wait for web provisioning settings.
 *  @param [in] wp -- Structure to receive web provision
 *                        response settings.
 *  @param [in] timeout -- Maximum time to wait in milliseconds until
 *                 complete, or 0 for no timeout
 *  @return     ATLIBGS_MSG_ID_E -- error code
 */
/*--------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetWebProvSettings(
        ATLIBGS_WEB_PROV_SETTINGS *wp,
        uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char line[80 + 1];
    uint16_t len;
    uint8_t c;
    char *p;
    bool done;
    uint32_t start = MSTimerGet();

    /* Example response (SPI style): */
    /*    SSID=FDIOutsideG */
    /*    CHNL=11 */
    /*    CONN_TYPE=0 */
    /*    MODE=0 */
    /*    SECURITY=3 */
    /*    PSK_PASS_PHRASE=FDIFDIFDI */
    /*    DHCP_ENBL=0 */
    /*    STATIC_IP=192.168.25.100 */
    /*    SUBNT_MASK=192.168.25.101 */
    /*    GATEWAY_IP=192.168.25.102 */
    /*    AUTO_DNS_ENBL=0 */
    /*    PRIMERY_DNS_IP=192.168.25.103 */
    /*    SECNDRY_DNS_IP=192.168.25.103 */
    /*      */
    /*    APP Reset-APP SW Reset */

    done = false;
    while (!done) {
        /* Read a line */
        len = 0;
        while (1) {
            /* Out of time? */
            if ((MSTimerDelta(start) >= timeout) && (timeout != 0)) {
                /* Timeout! */
                rxMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
                done = true;
                break;
            }
            /* Block waiting for a character */
            if (App_Read(&c, 1, 0)) {
                /* Ignore \r */
                if (c == '\r')
                    continue;
                if (c == '\n') {
                    /* Cap the end of the string and restart the string length */
                    line[len] = '\0';

                    /* Blank line?  Then we're done */
                    if ((len == 0) || (strncmp(line, "APP Reset", 9) == 0)) {
                        done = true;
                        break;
                    }
                    len = 0;

                    /* Got a line with some data */
                    /* Parse and only process if there are two sides to this */
                    p = strchr(line, '=');
                    if (p) {
                        /* Have two parts, left and right.  Null the equal */
                        *p = '\0';
                        p++;
#ifdef ATLIBGS_DEBUG_ENABLE
                        ConsolePrintf("%s -> [%s]\n", line, p);
#endif
                        /* Look at the field and setup (in order they will appear): */
                        if (strcmp(line, "SSID") == 0) {
                            strncpy(wp->ssid, p, ATLIBGS_SSID_MAX_LENGTH);
                        } else if (strcmp(line, "CHNL") == 0) {
                            wp->channel = atoi(p);
                        } else if (strcmp(line, "CONN_TYPE") == 0) {
                            wp->conn_type = atoi(p);
                        } else if (strcmp(line, "MODE") == 0) {
                            wp->station = (ATLIBGS_STATIONMODE_E)atoi(p);
                        } else if (strcmp(line, "SECURITY") == 0) {
                            wp->security = (ATLIBGS_SECURITYMODE_E)atoi(p);
                        } else if (strcmp(line, "PSK_PASS_PHRASE") == 0) {
                            strncpy(wp->password, p,
                                    ATLIBGS_PASSWORD_MAX_LENGTH);
                        } else if (strcmp(line, "DHCP_ENBL") == 0) {
                            wp->dhcp_enable = atoi(p);
                            if (wp->dhcp_enable) {
                                done = true;
                                break;
                            }
                        } else if (strcmp(line, "STATIC_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->ip);
                        } else if (strcmp(line, "SUBNT_MASK") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->subnet);
                        } else if (strcmp(line, "GATEWAY_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->gateway);
                        } else if (strcmp(line, "AUTO_DNS_ENBL") == 0) {
                            wp->auto_dns_enable = atoi(p);
                        } else if (strcmp(line, "PRIMERY_DNS_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->dns1);
                        } else if (strcmp(line, "SECNDRY_DNS_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->dns2);
                            /* This is the last one we'll get! */
                            done = true;
                            break;
                        }
                    }
                } else {
                    /* Append the character to the line */
                    if (len < 80)
                        line[len++] = c;
                }
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IPv4AddressToString
 *--------------------------------------------------------------------------*/
/**   Convert a IPv4 address into a string in the format "###.###.###.###"
 *  @param [in] ip -- IP address to convert
 *  @param [in] string -- String to receive the IP address.
 *  @return     void
 */
/*--------------------------------------------------------------------------*/
void AtLibGs_IPv4AddressToString(ATLIBGS_IPv4 *ip, char *string)
{
    sprintf(string, "%d.%d.%d.%d", (*ip)[0], (*ip)[1], (*ip)[2], (*ip)[3]);
}

int AtLibGs_CIDToIndex(uint8_t cid)
{
	// Convert '0'..'9' to 0..9
	if ((cid >= '0') && (cid <= '9'))
		return cid - '0';
	// Convert 'a'..'f' to 10..15
	if ((cid >= 'a') && (cid <= 'f'))
		return 10 + (cid - 'a');
	// Convert 'A'..'F' to 10..15
	if ((cid >= 'A') && (cid <= 'F'))
		return 10 + (cid - 'A');

	// For all else, return 15
	return 15;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  AtCmdLib.c
 *-------------------------------------------------------------------------*/
