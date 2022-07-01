/*-------------------------------------------------------------------------*
 * File:  AtCmdLib.h
 *-------------------------------------------------------------------------*
 * Description:
 *      The GainSpan AT Command Library (AtCmdLib) provides the functions
 *      that send AT commands to a GainSpan node and looks for a response.
 *      Parse commands are provided to interpret the response data.
 *-------------------------------------------------------------------------*/
#ifndef _GS_ATCMDLIB_H_
#define _GS_ATCMDLIB_H_

#include <stdint.h>
#include <stdbool.h>

/* Parsing related defines */
#define  ATLIBGS_UDP_CLIENT_CID_OFFSET_BYTE        (8)  /* CID parameter offset in UDP connection response */
#define  ATLIBGS_RAW_DATA_STRING_SIZE_MAX          (4)  /* Number of octets representing the data length field in raw data transfer message*/
#define  ATLIBGS_HTTP_RESP_DATA_LEN_STRING_SIZE    (4)  /* Number of octets representing the data length field in HTTP data transfer message*/
#define  ATLIBGS_INVALID_CID                    (0xFF) /* invalid CID */

#define ATLIBGS_BSSID_MAX_LENGTH    20
#define ATLIBGS_SSID_MAX_LENGTH     32
#define ATLIBGS_MAC_MAX_LENGTH      18
#define ATLIBGS_PASSWORD_MAX_LENGTH     32      // TODO: Is this correct?
typedef enum {
    ATLIBGS_RET_STATUS_ERROR = 0,
    ATLIBGS_RET_STATUS_OK = 1,
    ATLIBGS_RET_STATUS_VALID_DATA = 2,
    ATLIBGS_RET_STATUS_CONTROL_CODE = 3,
    ATLIBGS_RET_STATUS_MAX
} ATLIBGS_RET_STATUS_E;

typedef enum {
    ATLIBGS_DISABLE = 0, ATLIBGS_ENABLE = 1
} ATLIBGS_STATUS_E;

typedef enum {
    ATLIBGS_CON_TCP_SERVER,
    ATLIBGS_CON_UDP_SERVER,
    ATLIBGS_CON_UDP_CLIENT,
    ATLIBGS_CON_TCP_CLIENT

} ATLIBGS_CON_TYPE;

typedef enum {
    ATLIBGS_RX_STATE_START = 0,
    ATLIBGS_RX_STATE_CMD_RESP,
    ATLIBGS_RX_STATE_ESCAPE_START,
    ATLIBGS_RX_STATE_DATA_HANDLE,
    ATLIBGS_RX_STATE_BULK_DATA_HANDLE,
    ATLIBGS_RX_STATE_HTTP_RESPONSE_DATA_HANDLE,
    ATLIBGS_RX_STATE_RAW_DATA_HANDLE,
    ATLIBGS_RX_STATE_MAX
} ATLIBGS_RX_STATE_E;

typedef enum {
    ATLIBGS_MSG_ID_NONE = 0,
    ATLIBGS_MSG_ID_OK = 1,
    ATLIBGS_MSG_ID_INVALID_INPUT,
    ATLIBGS_MSG_ID_ERROR,
    ATLIBGS_MSG_ID_ERROR_IP_CONFIG_FAIL,
    ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL,
    ATLIBGS_MSG_ID_DISCONNECT,
    ATLIBGS_MSG_ID_DISASSOCIATION_EVENT,
    ATLIBGS_MSG_ID_APP_RESET,
    ATLIBGS_MSG_ID_OUT_OF_STBY_ALARM,
    ATLIBGS_MSG_ID_OUT_OF_STBY_TIMER,
    ATLIBGS_MSG_ID_UNEXPECTED_WARM_BOOT,
    ATLIBGS_MSG_ID_OUT_OF_DEEP_SLEEP,
    ATLIBGS_MSG_ID_WELCOME_MSG,
    ATLIBGS_MSG_ID_STBY_CMD_ECHO,
    ATLIBGS_MSG_ID_TCP_CON_DONE,
    ATLIBGS_MSG_ID_RESPONSE_TIMEOUT,
    ATLIBGS_MSG_ID_BULK_DATA_RX,
    ATLIBGS_MSG_ID_DATA_RX,
    ATLIBGS_MSG_ID_RAW_DATA_RX,
    ATLIBGS_MSG_ID_ESC_CMD_OK,
    ATLIBGS_MSG_ID_ESC_CMD_FAIL,
    ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX,
    ATLIBGS_MSG_ID_MAX,
    ATLIBGS_MSG_ID_TCP_SERVER_CONNECT,
    ATLIBGS_MSG_ID_GENERAL_MESSAGE
} ATLIBGS_MSG_ID_E;

typedef enum {
    ATLIBGS_ID_INFO_OEM = 0,
    ATLIBGS_ID_INFO_HARDWARE_VERSION = 1,
    ATLIBGS_ID_INFO_SOFTWARE_VERSION = 2
} ATLIBGS_ID_INFO_E;

typedef enum {
    ATLIBGS_PROFILE_ACTIVE = 0,
    ATLIBGS_PROFILE_STORED_0 = 1,
    ATLIBGS_PROFILE_STORED_1 = 2,
} ATLIBGS_PROFILE_E;

typedef enum {
    ATLIBGS_REGDOMAIN_FCC = 0,
    ATLIBGS_REGDOMAIN_ETSI = 1,
    ATLIBGS_REGDOMAIN_TELEC = 2,
    ATLIBGS_REGDOMAIN_UNKNOWN = -1
} ATLIBGS_REGDOMAIN_E;

typedef enum {
    ATLIBGS_AUTHMODE_NONE_WPA = 0,
    ATLIBGS_AUTHMODE_OPEN_WEP = 1,
    ATLIBGS_AUTHMODE_SHARED_WEP = 2,
    ATLIBGS_AUTHMODE_UNKNOWN = -1
} ATLIBGS_AUTHMODE_E;

typedef enum {
    ATLIBGS_WEPKEY1 = 1,
    ATLIBGS_WEPKEY2 = 2,
    ATLIBGS_WEPKEY3 = 3,
    ATLIBGS_WEPKEY4 = 4,
    ATLIBGS_WEPKEY_UNKNOWN = -1
} ATLIBGS_WEPKEY_E;

typedef enum {
    ATLIBGS_OAEAPFAST = 43,
    ATLIBGS_OAEAPTLS = 13,
    ATLIBGS_OAEAPTTLS = 21,
    ATLIBGS_OAEAPPEAP = 25,
    ATLIBGS_IAEAPMSCHAP = 26,
    ATLIBGS_IAEAPGTC = 6,
    ATLIBGS_AU_UNKNOWN = -1
} ATLIBGS_INOUAUTH_E;

typedef enum {
    ATLIBGS_SMAUTO = 0,
    ATLIBGS_SMOPEN = 1,
    ATLIBGS_SMWEP = 2,
    ATLIBGS_SMWPAPSK = 4,
    ATLIBGS_SMWPA2PSK = 8,
    ATLIBGS_SMWPAE = 16,
    ATLIBGS_SMWPA2E = 32,
    ATLIBGS_SM_UNKNOWN = -1
} ATLIBGS_SECURITYMODE_E;

typedef enum {
    ATLIBGS_HTTP_HE_AUTH = 2,
    ATLIBGS_HTTP_HE_CONN = 3,
    ATLIBGS_HTTP_HE_CON_ENC = 4,
    ATLIBGS_HTTP_HE_CON_LENGTH = 5,
    ATLIBGS_HTTP_HE_CON_RANGE = 6,
    ATLIBGS_HTTP_HE_CON_TYPE = 7,
    ATLIBGS_HTTP_HE_DATE = 8,
    ATLIBGS_HTTP_HE_EXPIRES = 9,
    ATLIBGS_HTTP_HE_FROM = 10,
    ATLIBGS_HTTP_HE_HOST = 11,
    ATLIBGS_HTTP_HE_IF_MODIF_SIN = 12,
    ATLIBGS_HTTP_HE_LAST_MODIF = 13,
    ATLIBGS_HTTP_HE_LOCATION = 14,
    ATLIBGS_HTTP_HE_PRAGMA = 15,
    ATLIBGS_HTTP_HE_RANGE = 16,
    ATLIBGS_HTTP_HE_REF = 17,
    ATLIBGS_HTTP_HE_SERVER = 18,
    ATLIBGS_HTTP_HE_TRANS_ENC = 19,
    ATLIBGS_HTTP_HE_USER_A = 20,
    ATLIBGS_HTTP_HE_WWW_AUTH = 21,
    ATLIBGS_HTTP_REQ_URL = 23,
    ATLIBGS_HTTP_UNKNOWN = -1
} ATLIBGS_HTTPCLIENT_E;

typedef enum {
    ATLIBGS_RAW_ETH_DIS = 0,
    ATLIBGS_RAW_ETH_NONSNAP = 1,
    ATLIBGS_RAW_ETH_ENABLEALL = 2,
    ATLIBGS_RAW_ETH_UNKNOWN = -1
} ATLIBGS_RAW_ETH_E;

typedef enum {
    ATLIBGS_ASPWMD_ACTIVE_ON = 0,
    ATLIBGS_ASPWMD_MODE_1 = 1,
    ATLIBGS_ASPWMD_MODE_2 = 2,
    ATLIBGS_ASPWMD_MODE_3 = 3,
    ATLIBGS_ASPWMD_UNKNOWN = -1
} ATLIBGS_ASSOCPOWMODE_E;

typedef enum {
    ATLIBGS_SPI_PHASE = 0, ATLIBGS_SPI_PHASE_180 = 1,
} ATLIBGS_SPI_PHASE_E;

typedef enum {
    ATLIBGS_SPI_POLARITY_LOW = 0, ATLIBGS_SPI_POLARITY_HIGH = 1,
} ATLIBGS_SPI_POLARITY_E;

typedef enum {
    ATLIBGS_APP0_BIN = 1,
    ATLIBGS_APP1_BIN = 2,
    ATLIBGS_APP01_BINS = 3,
    ATLIBGS_WLAN_BIN = 4,
    ATLIBGS_ALL_BINS = 7,
    ATLIBGS_FWBIN_UNKNOWN = -1
} ATLIBGS_FIRMWARE_BINARIES_E;

typedef enum {
    ATLIBGS_GPIO10 = 10,
    ATLIBGS_GPIO11 = 11,
    ATLIBGS_GPIO30 = 30,
    ATLIBGS_GPIO31 = 31,
} ATLIBGS_GPIO_PIN_E;

typedef enum {
    ATLIBGS_LOW = 0, ATLIBGS_HIGH = 1,
} ATLIBGS_GPIO_STATE_E;

typedef enum {
    ATLIBGS_FWUPPARAM_SERVERIP = 0,
    ATLIBGS_FWUPPARAM_SERVERPORT = 1,
    ATLIBGS_FWUPPARAM_PROXYPRESENT = 2,
    ATLIBGS_FWUPPARAM_PROXYIP = 3,
    ATLIBGS_FWUPPARAM_PROXYPORT = 4,
    ATLIBGS_FWUPPARAM_SSLENABLE = 5,
    ATLIBGS_FWUPPARAM_CACERT = 6,
    ATLIBGS_FWUPPARAM_WLAN = 7,
    ATLIBGS_FWUPPARAM_APP0 = 8,
    ATLIBGS_FWUPPARAM_APP1 = 9,
    ATLIBGS_FWUPPARAM_UNKNOWN = -1
} ATLIBGS_FWUPGRADEPARAM_E;

typedef enum {
    ATLIBGS_EAPTLS_CAC = 0,
    ATLIBGS_EAPTLS_CLIC = 1,
    ATLIBGS_EAPTLS_PRK = 2,
    ATLIBGS_EAPTLS_BIN = 0,
    ATLIBGS_EAPTLS_HEX = 1,
    ATLIBGS_EAPTLS_FLASH = 0,
    ATLIBGS_EAPTLS_RAM = 1,
    ATLIBGS_EAPTLS_UNKNOWN = -1
} ATLIBGS_EAPTLS_E;

typedef enum {
    ATLIBGS_SOCKET_OPTION_TYPE_SOCK = 65535,
    ATLIBGS_SOCKET_OPTION_TYPE_IP = 0,
    ATLIBGS_SOCKET_OPTION_TYPE_TCP = 6,
    ATLIBGS_SOCKET_OPTION_TYPE_UNKNOWN = -1
} ATLIBGS_SOCKET_OPTION_TYPE_E;

typedef enum {
    ATLIBGS_SOCKET_OPTION_PARAM_TCP_MAXRT = 0x10,
    ATLIBGS_SOCKET_OPTION_PARAM_TCP_KEEPALIVE = 0x4001,
    ATLIBGS_SOCKET_OPTION_PARAM_SO_KEEPALIVE = 0x8,
    ATLIBGS_SOCKET_OPTION_PARAM_TCP_KEEPALIVE_COUNT = 0x4005,
    ATLIBGS_SOCKET_OPTION_PARAM_UNKNOWN = -1
} ATLIBGS_SOCKET_OPTION_PARAM_E;

typedef enum {
    ATLIBGS_HTTPSEND_GET = 1,
    ATLIBGS_HTTPSEND_POST = 3,
    ATLIBGS_HTTPSEND_UNKNOWN = -1
} ATLIBGS_HTTPSEND_E;

typedef enum {
    ATLIBGS_TRAUTO = 0,
    ATLIBGS_TR1MBPS = 2,
    ATLIBGS_TR2MBPS = 4,
    ATLIBGS_TR5p5MBPS = 11,
    ATLIBGS_TR11MBPS = 22,
    ATLIBGS_TR_UNKNOWN = -1
} ATLIBGS_TRANSRATE_E;

typedef enum {
    ATLIBGS_UNSOLICITEDTX_RT1 = 130,
    ATLIBGS_UNSOLICITEDTX_RT2 = 132,
    ATLIBGS_UNSOLICITEDTX_RT55 = 139,
    ATLIBGS_UNSOLICITEDTX_RT11 = 150,
    ATLIBGS_UNSOLICITEDTX_UNKNOWN = -1
} ATLIBGS_UNSOLICITEDTX_E;

typedef enum {
    ATLIBGS_RFWAVETXSTART_RATE1 = 0,
    ATLIBGS_RFWAVETXSTART_RATE2 = 1,
    ATLIBGS_RFWAVETXSTART_RATE55 = 2,
    ATLIBGS_RFWAVETXSTART_RATE11 = 3,
    ATLIBGS_RFWAVETXSTART_UNKNOWN = -1
} ATLIBGS_RFWAVETXSTART_E;

typedef enum {
    ATLIBGS_RFFRAMETXSTART_RATE1 = 2,
    ATLIBGS_RFFRAMETXSTART_RATE2 = 4,
    ATLIBGS_RFFRAMETXSTART_RATE55 = 11,
    ATLIBGS_RFFRAMETXSTART_RATE11 = 22,
    ATLIBGS_RFFRAMETXSTART_UNKNOWN = -1
} ATLIBGS_RFFRAMETXSTART_E;

typedef enum {
    ATLIBGS_STATIONMODE_INFRASTRUCTURE = 0,
    ATLIBGS_STATIONMODE_AD_HOC = 1,
    ATLIBGS_STATIONMODE_LIMITED_AP = 2,
    ATLIBGS_STATIONMODE_UNKNOWN = -1
} ATLIBGS_STATIONMODE_E;

typedef uint8_t ATLIBGS_IPv4[4];

typedef union {
    ATLIBGS_IPv4 ipv4;
//  ATLIBGS_IPv6 ipv6; // placeholder
} ATLIBGS_IP;

typedef struct {
    char bssid[ATLIBGS_BSSID_MAX_LENGTH + 1];
    char ssid[ATLIBGS_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    ATLIBGS_STATIONMODE_E station;
    int8_t signal;
    ATLIBGS_SECURITYMODE_E security;
} ATLIBGS_NetworkScanEntry;

typedef struct {
    char mac[ATLIBGS_MAC_MAX_LENGTH + 1];
    uint8_t connected;
    ATLIBGS_STATIONMODE_E mode;
    char bssid[ATLIBGS_BSSID_MAX_LENGTH + 1];
    char ssid[ATLIBGS_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    ATLIBGS_SECURITYMODE_E security;
    int8_t signal;
    ATLIBGS_IP addr;
    ATLIBGS_IP subnet;
    ATLIBGS_IP gateway;
    ATLIBGS_IP dns1;
    ATLIBGS_IP dns2;
    uint32_t rxCount;
    uint32_t txCount;
} ATLIBGS_NetworkStatus;

typedef struct {
    char ssid[ATLIBGS_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    uint8_t conn_type;
    char password[ATLIBGS_PASSWORD_MAX_LENGTH + 1];
    ATLIBGS_STATIONMODE_E station;
    ATLIBGS_SECURITYMODE_E security;
    uint8_t dhcp_enable;
    uint8_t auto_dns_enable;
    ATLIBGS_IPv4 ip;
    ATLIBGS_IPv4 subnet;
    ATLIBGS_IPv4 gateway;
    ATLIBGS_IPv4 dns1;
    ATLIBGS_IPv4 dns2;
    uint8_t tcpIPClientHostIP;
} ATLIBGS_WEB_PROV_SETTINGS;

#define  ATLIBGS_CR_CHAR          0x0D     /* octet value in hex representing Carriage return    */
#define  ATLIBGS_LF_CHAR          0x0A     /* octet value in hex representing Line feed             */
#define  ATLIBGS_ESC_CHAR         0x1B     /* octet value in hex representing application level ESCAPE sequence */

/* Special characters used for data mode handling */
#define  ATLIBGS_DATA_MODE_NORMAL_START_CHAR_S      'S'
#define  ATLIBGS_DATA_MODE_NORMAL_END_CHAR_E        'E'
#define  ATLIBGS_DATA_MODE_BULK_START_CHAR_Z        'Z'
#define  ATLIBGS_DATA_MODE_BULK_START_CHAR_H        'H'
#define  ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL  ':'
#define  ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_R    'R'
#define  ATLIBGS_DATA_MODE_ESC_OK_CHAR_O            'O'
#define  ATLIBGS_DATA_MODE_ESC_FAIL_CHAR_F          'F'
#define  ATLIBGS_DATA_MODE_UDP_START_CHAR_u         'u'

/************* Following control octets are  used by SPI driver layer *****************************/
#define  ATLIBGS_SPI_ESC_CHAR               (0xFB)    /* Start transmission indication */
#define  ATLIBGS_SPI_IDLE_CHAR              (0xF5)    /* synchronous IDLE */
#define  ATLIBGS_SPI_XOFF_CHAR              (0xFA)    /* Stop transmission indication */
#define  ATLIBGS_SPI_XON_CHAR               (0xFD)    /* Start transmission indication */
#define  ATLIBGS_SPI_INVALID_CHAR_ALL_ONE   (0xFF)    /* Invalid character possibly recieved during reboot */
#define  ATLIBGS_SPI_INVALID_CHAR_ALL_ZERO  (0x00)    /* Invalid character possibly recieved during reboot */
#define  ATLIBGS_SPI_READY_CHECK            (0xF3)    /* SPI link ready check */
#define  ATLIBGS_SPI_READY_ACK              (0xF3)    /* SPI link ready response */
#define  ATLIBGS_SPI_CTRL_BYTE_MASK         (0x80)    /* Control byte mask */

/************* End of  SPI driver layer  specific defines*****************************/
#define ATLIBGS_RX_MAX_LINES                20

/* this should be tied to a timer, faster processor larger the number */
//#define  ATLIBGS_RESPONSE_TIMEOUT_COUNT    (0xFFFFFFFF) /* Time out value for reponse message */
#define  ATLIBGS_RESPONSE_TIMEOUT_COUNT    (0x2FFFFFF) // OK for SPI
/*************<Function prototye forward delarations >***************/

ATLIBGS_MSG_ID_E AtLibGs_Check(uint32_t timeout);
ATLIBGS_MSG_ID_E AtLibGs_SetEcho(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_SetMAC(char *pAddr);
ATLIBGS_MSG_ID_E AtLibGs_SetMAC2(char *pAddr);
ATLIBGS_MSG_ID_E AtLibGs_GetMAC(char *mac);
ATLIBGS_MSG_ID_E AtLibGs_GetMAC2(char *mac);
ATLIBGS_MSG_ID_E AtLibGs_NCM_AutoConfig(uint32_t id, int32_t value);
ATLIBGS_MSG_ID_E AtLibGs_CalcNStorePSK(char *pSsid, char *pPsk);
//ATLIBGS_MSG_ID_E AtLibGs_WlanConnStat( ATLIBGS_N);
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkStatus(ATLIBGS_NetworkStatus *pStatus);
ATLIBGS_MSG_ID_E AtLibGs_DHCPSet(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_Assoc(
    const char *pSsid,
    const char *pBssid,
    uint8_t channel);
ATLIBGS_MSG_ID_E AtLibGs_TCPClientStart(
        char *pRemoteTcpSrvIp,
        int16_t pRemoteTcpSrvPort,
        uint8_t *cid);
ATLIBGS_MSG_ID_E AtLibGs_UDPClientStart(
        char *pRemoteUdpSrvIp,
        char *pRemoteUdpSrvPort,
        char *pUdpLocalPort);

ATLIBGS_MSG_ID_E AtLibGs_CloseAll(void);

ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStart(uint16_t interval);
ATLIBGS_MSG_ID_E AtLibGs_GotoSTNDBy(
        char *nsec,
        uint32_t dealy,
        uint8_t alarm1_Pol,
        uint8_t alarm2_Pol);
ATLIBGS_MSG_ID_E AtLibGs_PSK(char *pPsk);
ATLIBGS_MSG_ID_E AtLibGs_EnableDeepSleep(void);
ATLIBGS_MSG_ID_E AtLibGs_StoreNwConn(void);
ATLIBGS_MSG_ID_E AtLibGs_ReStoreNwConn(void);
ATLIBGS_MSG_ID_E AtLibGs_IPSet(char *pIpAddr, char *pSubnet, char *pGateway);

ATLIBGS_MSG_ID_E AtLibGs_SaveProfile(uint8_t profile);
ATLIBGS_MSG_ID_E AtLibGs_LoadProfile(uint8_t profile);
ATLIBGS_MSG_ID_E AtLibGs_ResetFactoryDefaults(void);
ATLIBGS_MSG_ID_E AtLibGs_GetRssi(void);
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc(void);
ATLIBGS_MSG_ID_E AtLibGs_FWUpgrade(
        char *pSrvip,
        uint16_t srvport,
        uint16_t srcPort,
        char *pSrcIP);
ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStop(void);
ATLIBGS_MSG_ID_E AtLibGs_MultiCast(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_Version(void);
ATLIBGS_MSG_ID_E AtLibGs_Mode(ATLIBGS_STATIONMODE_E mode);
ATLIBGS_MSG_ID_E AtLibGs_UDPServer_Start(uint16_t pUdpSrvPort, uint8_t *cid);
ATLIBGS_MSG_ID_E AtLibGs_TCPServer_Start(uint16_t pTcpSrvPort, uint8_t *cid);
ATLIBGS_MSG_ID_E AtLibGs_DNSLookup(const char* pUrl);
ATLIBGS_MSG_ID_E AtLibGs_Close(uint8_t cid);
ATLIBGS_MSG_ID_E AtLibGs_SetWRetryCount(uint16_t count);
ATLIBGS_MSG_ID_E AtLibGs_GetErrCount(uint32_t *errorCount);
ATLIBGS_MSG_ID_E AtLibGs_EnableRadio(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_EnablePwSave(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_SetTime(char* pDate, char *pTime);
ATLIBGS_MSG_ID_E AtLibGs_EnableExternalPA(uint8_t mode);
ATLIBGS_MSG_ID_E AtLibGs_SyncLossInterval(uint16_t interval);
ATLIBGS_MSG_ID_E AtLibGs_PSPollInterval(uint16_t interval);
ATLIBGS_MSG_ID_E AtLibGs_SetTxPower(uint8_t power);
ATLIBGS_MSG_ID_E AtLibGs_SetDNSServerIP(char *pDNS1, char *pDNS2);
ATLIBGS_MSG_ID_E AtLibGs_EnableAutoConnect(uint8_t mode);
void AtLibGs_SwitchFromAutoToCmd(void);
ATLIBGS_MSG_ID_E AtLibGs_StoreWAutoConn(char * pSsid, uint8_t channel);
ATLIBGS_MSG_ID_E AtLibGs_StoreNAutoConn(char* pIpAddr, int16_t pRmtPort);
ATLIBGS_MSG_ID_E AtLibGs_StoreATS(uint8_t param, uint8_t value);
ATLIBGS_MSG_ID_E AtLibGs_BData(uint8_t mode);
typedef struct {
    char ssid[ATLIBGS_SSID_MAX_LENGTH + 1];
    char password[ATLIBGS_PASSWORD_MAX_LENGTH + 1];
    uint16_t channel;
} AtLibGs_WPSResult;

ATLIBGS_MSG_ID_E AtLibGs_StartWPSPUSH(AtLibGs_WPSResult *result);
ATLIBGS_MSG_ID_E AtLibGs_StartWPSPIN(char* pin, AtLibGs_WPSResult *result);
ATLIBGS_MSG_ID_E AtLibGs_GetInfo(
        ATLIBGS_ID_INFO_E info,
        char* infotext,
        int16_t maxchars);
ATLIBGS_MSG_ID_E AtLibGs_GetScanTimes(uint16_t *min, uint16_t *max);
ATLIBGS_MSG_ID_E AtLibGs_SetScanTimes(uint16_t min, uint16_t max);
ATLIBGS_MSG_ID_E AtLibGs_SetDefaultProfile(uint8_t profile);
ATLIBGS_MSG_ID_E AtLibGs_GetProfile(
        ATLIBGS_PROFILE_E profile,
        char *text,
        uint16_t maxChars);
ATLIBGS_MSG_ID_E AtLibGs_GetRegulatoryDomain(ATLIBGS_REGDOMAIN_E *regDomain);
ATLIBGS_MSG_ID_E AtLibGs_SetRegulatoryDomain(ATLIBGS_REGDOMAIN_E regDomain);
ATLIBGS_MSG_ID_E AtLibGs_NetworkScan(
        const char *SSID,
        uint8_t channel,
        uint16_t scantime,
        ATLIBGS_NetworkScanEntry *entries,
        uint8_t maxEntries,
        uint8_t *numEntries);

typedef struct {
    // IP address string of unit on other side
    char ip[17];

    // Local port id
    uint16_t port;

    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATLIBGS_UDPMessage;

typedef struct {
    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    const uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATLIBGS_TCPMessage;

typedef struct {
    // Which server created this connection?
    uint8_t server_cid;

    // Newly created cid for communication
    uint8_t cid;

    // IP number of incoming connection
    char ip[16];

    // Connection port (local allocated port number)
    uint16_t port;
} ATLIBGS_TCPConnection;

ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPMessage(uint8_t cid, uint32_t timeout);
void AtLibGs_ParseTCPData(
        const uint8_t *received,
        uint16_t length,
        ATLIBGS_TCPMessage *rxm);
ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPConnection(
        ATLIBGS_TCPConnection *connection,
        uint32_t timeout);

typedef struct {
    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    const uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATLIBGS_HTTPMessage;

ATLIBGS_MSG_ID_E AtLibGs_WaitForHTTPMessage(uint32_t timeout);
void AtLibGs_ParseHTTPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_HTTPMessage *rxm);

ATLIBGS_MSG_ID_E AtLibGs_WaitForUDPMessage(uint32_t timeout);
void AtLibGs_ParseUDPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_UDPMessage *msg);

// TODO: Put all functions in same order/grouping as in command reference list
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc2(void);
ATLIBGS_MSG_ID_E AtLibGs_GetTransmitRate(void);
ATLIBGS_MSG_ID_E AtLibGs_GetClientInfo(void);
ATLIBGS_MSG_ID_E AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_E authmode);
ATLIBGS_MSG_ID_E AtLibGs_SetWEPKey(ATLIBGS_WEPKEY_E keynum, char key[]);
ATLIBGS_MSG_ID_E AtLibGs_SetWPAKey(char key[]);
ATLIBGS_MSG_ID_E
AtLibGs_SetListenBeaconInterval(bool enable, uint16_t interval);
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitRate(ATLIBGS_TRANSRATE_E transrate);
ATLIBGS_MSG_ID_E AtLibGs_EnableDHCPServer(void);
ATLIBGS_MSG_ID_E AtLibGs_DisableDHCPServer(void);
ATLIBGS_MSG_ID_E AtLibGs_EnableDNSServer(char server[]);
ATLIBGS_MSG_ID_E AtLibGs_DisableDNSServer(void);
ATLIBGS_MSG_ID_E AtLibGs_SetEAPConfiguration(
        ATLIBGS_INOUAUTH_E oa,
        ATLIBGS_INOUAUTH_E ia,
        char user[],
        char pass[]);
ATLIBGS_MSG_ID_E AtLibGs_AddSSLCertificate(
        char name[],
        bool hex,
        uint16_t size,
        bool ram);
ATLIBGS_MSG_ID_E AtLibGs_DeleteSSLCertificate(char name[]);
ATLIBGS_MSG_ID_E AtLibGs_SetSecurity(ATLIBGS_SECURITYMODE_E security);
ATLIBGS_MSG_ID_E AtLibGs_SSLOpen(uint8_t cid, char name[]);
ATLIBGS_MSG_ID_E AtLibGs_SSLClose(uint8_t cid);
ATLIBGS_MSG_ID_E AtLibGs_HTTPConf(ATLIBGS_HTTPCLIENT_E param, char value[]);
ATLIBGS_MSG_ID_E AtLibGs_HTTPConfDel(ATLIBGS_HTTPCLIENT_E param);
ATLIBGS_MSG_ID_E AtLibGs_HTTPClose(uint8_t cid);
ATLIBGS_MSG_ID_E AtLibGs_RawETHFrameConf(ATLIBGS_RAW_ETH_E enable);
ATLIBGS_MSG_ID_E AtLibGs_BulkDataTrans(bool enable);
ATLIBGS_MSG_ID_E AtLibGs_SetBatteryCheckFreq(uint16_t freq);
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryCheckFreq(uint16_t *freq);
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryValue(void);
ATLIBGS_MSG_ID_E AtLibGs_EnterDeepSleep(uint32_t milliseconds);
ATLIBGS_MSG_ID_E AtLibGs_AssocPowerSaveMode(ATLIBGS_ASSOCPOWMODE_E mode);
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitPower(uint8_t power);
ATLIBGS_MSG_ID_E AtLibGs_StartAutoConnect(void);
ATLIBGS_MSG_ID_E AtLibGs_ConfigAPAutoConnect(void);
ATLIBGS_MSG_ID_E AtLibGs_ReturnAutoConnect(void);
ATLIBGS_MSG_ID_E AtLibGs_WebProv(char username[], char password[]);
ATLIBGS_MSG_ID_E AtLibGs_WebLogoAdd(uint16_t size, const void *txBuf);
ATLIBGS_MSG_ID_E AtLibGs_RFAsyncFrameRX(uint16_t channel);
ATLIBGS_MSG_ID_E AtLibGs_RFStop(void);
ATLIBGS_MSG_ID_E AtLibGs_SetPassPhrase(const char *pPhrase);
ATLIBGS_MSG_ID_E AtLibGs_SPIConf(
        ATLIBGS_SPI_POLARITY_E polarity,
        ATLIBGS_SPI_PHASE_E phase);
ATLIBGS_MSG_ID_E AtLibGs_Reset(void);
ATLIBGS_MSG_ID_E AtLibGs_MemoryTrace(void);
ATLIBGS_MSG_ID_E AtLibGs_StartFirmwareUpdate(ATLIBGS_FIRMWARE_BINARIES_E bins);
ATLIBGS_MSG_ID_E AtLibGs_GetTime(void);
ATLIBGS_MSG_ID_E AtLibGs_SetGPIO(
        ATLIBGS_GPIO_PIN_E GPIO,
        ATLIBGS_GPIO_STATE_E state);
ATLIBGS_MSG_ID_E AtLibGs_EnableEnhancedAsync(bool on);
ATLIBGS_MSG_ID_E AtLibGs_EnableVerbose(bool on);
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkConfiguration(void);
ATLIBGS_MSG_ID_E AtLibGs_FWUpgradeConfig(
        ATLIBGS_FWUPGRADEPARAM_E param,
        char value[]);
ATLIBGS_MSG_ID_E AtLibGs_GetCIDInfo(void);
ATLIBGS_MSG_ID_E AtLibGs_BatterySetWarnLevel(
        uint16_t level,
        uint16_t freq,
        uint16_t standby);
ATLIBGS_MSG_ID_E AtLibGs_SetEAPCertificate(
        ATLIBGS_EAPTLS_E cert,
        ATLIBGS_EAPTLS_E binhex,
        uint16_t size,
        ATLIBGS_EAPTLS_E flashram);
ATLIBGS_MSG_ID_E AtLibGs_Ping(
        char ip[],
        uint16_t trails,
        uint16_t timeout,
        uint16_t len,
        uint8_t tos,
        uint8_t ttl,
        char payload[]);
ATLIBGS_MSG_ID_E AtLibGs_TraceRoute(
        char ip[],
        uint16_t interval,
        uint8_t maxhops,
        uint8_t minhops,
        uint8_t tos);
ATLIBGS_MSG_ID_E AtLibGs_SNTPsync(
        bool enable,
        char ip[],
        uint8_t timeout,
        bool periodic,
        uint32_t frequency);
ATLIBGS_MSG_ID_E AtLibGs_SetSocketOptions(
        uint8_t cid,
        ATLIBGS_SOCKET_OPTION_TYPE_E type,
        ATLIBGS_SOCKET_OPTION_PARAM_E param,
        uint32_t value);
ATLIBGS_MSG_ID_E AtLibGs_HTTPOpen(
        char host[],
        uint16_t port,
        bool sslenabled,
        char certname[],
        char proxy[],
        uint16_t timeout,
        uint8_t *cid);
ATLIBGS_MSG_ID_E AtLibGs_HTTPSend(
        uint8_t cid,
        ATLIBGS_HTTPSEND_E type,
        uint16_t timeout,
        char page[],
        uint16_t size,
        const void *txBuf);
ATLIBGS_MSG_ID_E AtLibGs_UnsolicitedTXRate(
        uint16_t frame,
        uint16_t seq,
        uint8_t chan,
        ATLIBGS_UNSOLICITEDTX_E rate,
        uint32_t wmm,
        char mac[],
        char bssid[],
        uint16_t len);
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
        char srcMac[]);
ATLIBGS_MSG_ID_E AtLibGs_RFWaveTXStart(
        bool isModulated,
        uint16_t channel,
        ATLIBGS_RFWAVETXSTART_E rate,
        bool longPreamble,
        bool scramblerOn,
        bool continousWaveRX,
        uint8_t power,
        char ssid[]);

uint8_t AtLibGs_ParseUDPClientCid(void);
uint8_t AtLibGs_ParseWlanConnStat(void);
uint8_t AtLibGs_ParseNodeIPv4Address(ATLIBGS_IPv4 *ip);
void AtLibGs_ParseIPv4Address(const char *line, ATLIBGS_IPv4 *ip);
void AtLibGs_IPv4AddressToString(ATLIBGS_IPv4 *ip, char *string);
uint8_t AtLibGs_ParseRssiResponse(int16_t *pRssi);
uint8_t AtLibGs_ParseUDPServerStartResponse(uint8_t *pConnId);
uint8_t AtLibGs_ParseTCPServerStartResponse(uint8_t *pConnId);
uint8_t AtLibGs_ParseDNSLookupResponse(char *ipAddr);
uint16_t AtLibGs_ParseIntoLines(char *text, char *pLines[], uint16_t maxLines);
uint8_t AtLibGs_ParseIntoTokens(
        char *line,
        char deliminator,
        char *tokens[],
        uint8_t maxTokens);
ATLIBGS_SECURITYMODE_E AtLibGs_ParseSecurityMode(const char *string);
void AtLibGs_ParseIPAddress(const char *string, ATLIBGS_IP *ip);

ATLIBGS_MSG_ID_E AtLibGs_CommandSend(void);
ATLIBGS_MSG_ID_E AtLibGs_CommandSendString(char *aString);
void AtLibGs_DataSend(const void *pTxData, uint16_t dataLen);
ATLIBGS_MSG_ID_E AtLibGs_SendTCPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen);
ATLIBGS_MSG_ID_E AtLibGs_SendTCPBulkData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen);
ATLIBGS_MSG_ID_E AtLibGs_SendUDPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen,
        ATLIBGS_CON_TYPE conType,
        const char *pUdpClientIP,
        uint16_t udpClientPort);

ATLIBGS_MSG_ID_E AtLibGs_BulkDataTransfer(uint8_t cid, const void *pData, uint16_t dataLen);
ATLIBGS_MSG_ID_E AtLibGs_checkEOFMessage(const char *pBuffer);
ATLIBGS_MSG_ID_E AtLibGs_ReceiveDataProcess(uint8_t rxData);
ATLIBGS_MSG_ID_E AtLibGs_ResponseHandle(uint8_t cid);
ATLIBGS_MSG_ID_E AtLibGs_ProcessRxChunk(const void *rxBuf, uint16_t bufLen);
void AtLibGs_LinkCheck(void);
void AtLibGs_FlushIncomingMessage(void);
uint8_t AtLibGs_IsNodeResetDetected(void);
void AtLibGs_SetNodeResetFlag(void);
void AtLibGs_ClearNodeResetFlag(void);
uint8_t AtLibGs_IsNodeAssociated(void);
void AtLibGs_SetNodeAssociationFlag(void);
void AtLibGs_ClearNodeAssociationFlag(void);

void AtLibGs_FlushRxBuffer(void);
void AtLibGs_ConvertNumberTo4DigitASCII(uint16_t myNum, char *pStr);
ATLIBGS_MSG_ID_E AtLibGs_GetWebProvSettings(
        ATLIBGS_WEB_PROV_SETTINGS *wp,
        uint32_t timeout);
int AtLibGs_CIDToIndex(uint8_t cid);

extern void AtLibGs_Init(void);

#endif /* _GS_ATCMDLIB_H_ */

/*-------------------------------------------------------------------------*
 * End of File:  AtCmdLib.h
 *-------------------------------------------------------------------------*/
