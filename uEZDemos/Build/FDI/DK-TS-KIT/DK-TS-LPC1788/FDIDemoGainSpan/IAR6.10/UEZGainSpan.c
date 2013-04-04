#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <uEZ.h>
#include <UEZGPIO.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZSPI.h>
#include <uEZTimeDate.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include "UEZGainSpan.h"

void UEZGUIGainSpanProgramEXP4(void);
void UEZGUIGainSpanProgramUEZGUI56(void);
void UEZGUIGainSpanProgramDKAPP(void);

#define APP_MAX_RECEIVED_DATA           (512)
#define TCP_DEMO_REMOTE_TCP_SRVR_PORT   23

#define TEST_START(name) \
        FDICmdPrintf(aWorkspace, "Testing: %s\n", name)

#define TEST_END()
#define TEST_ASSERT(cond, msg)  \
    if (!(cond)) \
        { FDICmdPrintf(aWorkspace, "Test Failed! : " msg "\r\n"); G_numFailures++; }
#define TEST_ASSERT_STRING(v, string)  \
    if (strcmp((const char *)v, (const char *)string)!=0) \
      { FDICmdPrintf(aWorkspace, "Test Failed! : \"%s\" != \"" string "\"\r\n", v); G_numFailures++; }
#define TEST_ASSERT_STRINGS_EQUAL(string1, string2, msg)  \
    if (strcmp((const char *)string1, (const char *)string2)!=0) \
      { FDICmdPrintf(aWorkspace, "Test Failed! (%s:%d): %s\n  \"%s\" != \"%s\"\r\n", __FILE__, __LINE__, msg, string1, string2); G_numFailures++; }
#define TEST_ASSERT_STRINGS_NOT_EQUAL(string1, string2, msg)  \
    if (strcmp((const char *)string1, (const char *)string2)==0) \
      { FDICmdPrintf(aWorkspace, "Test Failed! (%s:%d): %s\n  \"%s\" == \"%s\"\r\n", __FILE__, __LINE__, msg, string1, string2); G_numFailures++; }
#define TEST_ASSERT_VALUE(v, number) \
    if (v != number) \
      { FDICmdPrintf(aWorkspace, "Test Failed! : " #v " (%d) != " #number "\r\n", v); G_numFailures++; }
#define TEST_ASSERT_OK(r, msg) \
        if (r != ATLIBGS_MSG_ID_OK) \
          { FDICmdPrintf(aWorkspace, "Test Failed! : %s\n", msg); G_numFailures++; }
#define TEST_ASSERT_NOT_OK(r, msg) \
        if (r == ATLIBGS_MSG_ID_OK) \
          { FDICmdPrintf(aWorkspace, "Test Failed! : %s\n", msg); G_numFailures++; }

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezDevice SPI;
static SPI_Request SPIRequest;
static int G_myflag;
int G_numFailures = 0;
uint8_t G_received[APP_MAX_RECEIVED_DATA + 1];
unsigned int G_receivedCount = 0;

static void ISetMyFlag(void *aWorkspace, SPI_Request *r)
{
    if (aWorkspace != (void *)&G_myflag)
        UEZFailureMsg("wrong flag");

    G_myflag = 1;
}

void TestGetInfo(void *aWorkspace)
{
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
    static char content[512];

    TEST_START("GetInfo");

    rxMsgId = AtLibGs_GetInfo(ATLIBGS_ID_INFO_OEM, content, sizeof(content));
    TEST_ASSERT(rxMsgId==ATLIBGS_MSG_ID_OK, "ATI0 did not respond with OK");
    TEST_ASSERT_STRING(content, "GainSpan");

    rxMsgId = AtLibGs_GetInfo(ATLIBGS_ID_INFO_HARDWARE_VERSION, content,
        sizeof(content));
    TEST_ASSERT(rxMsgId==ATLIBGS_MSG_ID_OK, "ATI0 did not respond with OK");
    TEST_ASSERT_STRING(content, "GS1011B0");

    rxMsgId = AtLibGs_GetInfo(ATLIBGS_ID_INFO_SOFTWARE_VERSION, content,
        sizeof(content));
    TEST_ASSERT(rxMsgId==ATLIBGS_MSG_ID_OK, "ATI0 did not respond with OK");
    TEST_ASSERT_STRING(content, "2.3.5");

    TEST_END();
}

void IUEZGUIGainSpan_IncomingData(uint8_t aByte)
{
    // Put the incoming byte into the receive buffer
    if (G_receivedCount < APP_MAX_RECEIVED_DATA)
        G_received[G_receivedCount++] = aByte;
}

T_uezError UEZGUIGainSpanStatus(void *aWorkspace)
{
    ATLIBGS_NetworkStatus status;
    AtLibGs_GetNetworkStatus(&status);
    if (status.connected) {
        FDICmdPrintf(aWorkspace, "Connected (Mode %d)\n", status.mode);
        FDICmdPrintf(aWorkspace, "SSID: %s\n", status.ssid);
        FDICmdPrintf(aWorkspace, "Mac: %s\n", status.mac);
        FDICmdPrintf(aWorkspace, "BSSID: %s\n", status.bssid);
        FDICmdPrintf(aWorkspace, "IP Address: %d.%d.%d.%d\n",
            status.addr.ipv4[0], status.addr.ipv4[1],
            status.addr.ipv4[2], status.addr.ipv4[3]);
    } else {
        FDICmdSendString(aWorkspace, "Not connected\n");
        return UEZ_ERROR_COULD_NOT_CONNECT;
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanAdHoc1(void *aWorkspace)
{
    // Setup an adhoc network
    AtLibGs_DHCPSet(0);       // no DHCP
    AtLibGs_Mode(ATLIBGS_STATIONMODE_AD_HOC);
    //AtLibGs_CalcNStorePSK("TestRouterNoInternet", "fdifdifdi");
    if (AtLibGs_Assoc("MWave09142012", 0, 0) == ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Connected to Ad Hoc!\n"); // association success
        AtLibGs_IPSet("192.168.10.1", "255.255.255.0",
            "192.168.10.0");
        UEZGUIGainSpanStatus(aWorkspace);
    } else {
        FDICmdSendString(aWorkspace,
            "*Not* connected to Ad Hoc!\n");
        return UEZ_ERROR_COULD_NOT_CONNECT;
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanAdHoc2(void *aWorkspace)
{
    // Setup an adhoc network
    AtLibGs_DHCPSet(0);       // no DHCP
    AtLibGs_Mode(ATLIBGS_STATIONMODE_AD_HOC);
    //AtLibGs_CalcNStorePSK("TestRouterNoInternet", "fdifdifdi");
    if (AtLibGs_Assoc("MWave09142012", 0, 0) == ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Connected to Ad Hoc!\n"); // association success
        AtLibGs_IPSet("192.168.10.2", "255.255.255.0",
            "192.168.10.0");
        UEZGUIGainSpanStatus(aWorkspace);
    } else {
        FDICmdSendString(aWorkspace,
            "*Not* connected to Ad Hoc!\n");
        return UEZ_ERROR_COULD_NOT_CONNECT;
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanLAP1(void *aWorkspace)
{
    // Setup an adhoc network
    AtLibGs_DHCPSet(0);       // no DHCP
    AtLibGs_Mode(ATLIBGS_STATIONMODE_LIMITED_AP);
    AtLibGs_CalcNStorePSK("MWave09142012AP", "fdifdifdiFDI");
    AtLibGs_SetSecurity(ATLIBGS_SMWPAPSK);
    if (AtLibGs_Assoc("MWave09142012AP", 0, 0) == ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Serving limited AP!\n"); // association success
        AtLibGs_IPSet("192.168.10.1", "255.255.255.0",
            "192.168.10.0");
        UEZGUIGainSpanStatus(aWorkspace);
    } else {
        FDICmdSendString(aWorkspace,
            "*Not* connected to Ad Hoc!\n");
        return UEZ_ERROR_COULD_NOT_CONNECT;
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanLAP2(void *aWorkspace)
{
    // Setup an adhoc network
    AtLibGs_DHCPSet(0);       // no DHCP
    AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE);
    //AtLibGs_CalcNStorePSK("TestRouterNoInternet", "fdifdifdi");
    if (AtLibGs_Assoc("MWave09142012AP", 0, 0) == ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Connected to AP!\n"); // association success
        AtLibGs_IPSet("192.168.10.2", "255.255.255.0",
            "192.168.10.0");
        UEZGUIGainSpanStatus(aWorkspace);
    } else {
        FDICmdSendString(aWorkspace,
            "*Not* connected to Ad Hoc!\n");
        return UEZ_ERROR_COULD_NOT_CONNECT;
    }
    return UEZ_ERROR_NONE;
}


T_uezError UEZGUIGainSpanServer(void *aWorkspace)
{
    bool connected = false;
    ATLIBGS_TCPConnection connection;
    ATLIBGS_TCPMessage msg;
    uint8_t cid = 0;
    uint8_t server_cid = 0;
    T_uezError error;
    ATLIBGS_MSG_ID_E rxMsgId;

    FDICmdSendString(aWorkspace, "Connecting to network... ");
    //error = UEZGUIGainSpanAdHoc1(aWorkspace);
    error = UEZGUIGainSpanLAP1(aWorkspace);
    if (error != UEZ_ERROR_NONE)
        return error;
    rxMsgId = AtLibGs_TCPServer_Start(TCP_DEMO_REMOTE_TCP_SRVR_PORT,
            &server_cid);
    if (rxMsgId != ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Could not open server!\n");
    } else {
        // Sit and wait for an incoming connection
        FDICmdSendString(aWorkspace, "Waiting ... ");
        while (1) {
            if (!connected) {
                if (AtLibGs_WaitForTCPConnection(&connection, 250)
                        == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT) {
                    // Got a connection!  What cid?
                    cid = connection.cid;
                    connected = true;
                } else {
                    FDICmdSendString(aWorkspace, ".");
                }
            }
            if (connected) {
                if (AtLibGs_WaitForTCPMessage(250) == ATLIBGS_MSG_ID_DATA_RX) {
                    AtLibGs_ParseTCPData(G_received, G_receivedCount, &msg);
                    FDICmdPrintf(aWorkspace, "Data: [%s]\n", msg.message);
                    G_receivedCount = 0;
                    G_received[0] = 0;
                    memset(G_received, 0, sizeof(G_received));
                    AtLibGs_SendTCPData(cid, (uint8_t *)"Test", 4);
//                    AtLibGs_Close(cid);
                    break;
                } else {
                    FDICmdSendString(aWorkspace, "?");
                }
            }
        }
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanRelayServer(void *aWorkspace)
{
    bool connected = false;
    ATLIBGS_TCPConnection connection;
    ATLIBGS_TCPMessage msg;
    uint8_t cid = 0;
    uint8_t server_cid = 0;
    T_uezError error;
    ATLIBGS_MSG_ID_E rxMsgId;
    TUInt32 timeout;
    TBool shutout = EFalse;
    TUInt32 shutouttime = 0;
    TBool active = EFalse;

    UEZGPIOClear(GPIO_P0_10);
    UEZGPIOOutput(GPIO_P0_10);
    UEZGPIOSetMux(GPIO_P0_10, 0); // ensure GPIO pin

    FDICmdSendString(aWorkspace, "Connecting to network... ");
    //error = UEZGUIGainSpanAdHoc1(aWorkspace);
    error = UEZGUIGainSpanLAP1(aWorkspace);
    if (error != UEZ_ERROR_NONE)
        return error;
    rxMsgId = AtLibGs_TCPServer_Start(TCP_DEMO_REMOTE_TCP_SRVR_PORT,
            &server_cid);
    if (rxMsgId != ATLIBGS_MSG_ID_OK) {
        FDICmdSendString(aWorkspace, "Could not open server!\n");
    } else {
        // Sit and wait for an incoming connection
        FDICmdSendString(aWorkspace, "Waiting ... ");
        while (1) {
            if (!connected) {
                if (AtLibGs_WaitForTCPConnection(&connection, 250)
                        == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT) {
                    // Got a connection!  What cid?
                    cid = connection.cid;
                    connected = true;
                } else {
                    FDICmdSendString(aWorkspace, ".");
                }
            }
            if (connected) {
                if (AtLibGs_WaitForTCPMessage(250) == ATLIBGS_MSG_ID_DATA_RX) {
                    AtLibGs_ParseTCPData(G_received, G_receivedCount, &msg);
                    //FDICmdPrintf(aWorkspace, "Data: [%s]\n", msg.message);
                    if (msg.message[0] == '0') {
                        FDICmdPrintf(aWorkspace, "OFF\n");
                        if (!shutout) {
                            UEZGPIOClear(GPIO_P0_10);
                            active = EFalse;
                        }
                    }
                    if (msg.message[0] == '1') {
                        FDICmdPrintf(aWorkspace, "ON\n");
                        if (!shutout) {
                            UEZGPIOSet(GPIO_P0_10);
                            active = ETrue;
                        }
                    }
                    G_receivedCount = 0;
                    G_received[0] = 0;
                    memset(G_received, 0, sizeof(G_received));
                    AtLibGs_SendTCPData(cid, (uint8_t *)"*", 1);
                    timeout = 0;
                } else {
                    FDICmdSendString(aWorkspace, "?");
                    timeout+=250;

                    if (shutout) {
                        if (shutouttime >= 250) {
                            shutouttime -= 250;
                        } else {
                            shutouttime = 0;
                            shutout = EFalse;
                            FDICmdPrintf(aWorkspace, "Shutout complete.\n");
                        }
                    }

                    if (timeout >= 5000) {
                        // No communications for 5 seconds!
                        if ((!shutout) && (active)) {
                            FDICmdPrintf(aWorkspace, "Shutout for 30 seconds!\n");
                            shutouttime = 30000;
                            shutout = ETrue;
                            UEZGPIOClear(GPIO_P0_10);
                            active = EFalse;
                        }
                        // Reset timeout
                        timeout = 0;
                    }
                }
            }
        }
    }
    return UEZ_ERROR_NONE;
}

T_uezError UEZGUIGainSpanClient(void *aWorkspace)
{
    bool connected = false;
    ATLIBGS_TCPMessage msg;
    uint8_t cid = 0;
    T_uezError error;
    ATLIBGS_NetworkStatus status;

    //
    if (AtLibGs_GetNetworkStatus(&status) != ATLIBGS_MSG_ID_OK)
        return UEZ_ERROR_INTERNAL_ERROR;
    if (!status.connected) {
        FDICmdSendString(aWorkspace, "Connecting to network... ");
        //error = UEZGUIGainSpanAdHoc2(aWorkspace);
        error = UEZGUIGainSpanLAP2(aWorkspace);
        if (error != UEZ_ERROR_NONE)
            return error;
    }
    while (1) {
        if (!connected) {
            FDICmdSendString(aWorkspace, "Connecting ... ");
            if (AtLibGs_TCPClientStart((char *)"192.168.10.1",
                TCP_DEMO_REMOTE_TCP_SRVR_PORT, &cid) == ATLIBGS_MSG_ID_OK) {
                // Got a connection!  What cid?
                connected = true;
                FDICmdSendString(aWorkspace, "Connected!\n");
                UEZTaskDelay(1000);
            } else {
                FDICmdSendString(aWorkspace, ".");
            }
        }
        if (connected) {
            AtLibGs_SendTCPData(cid, (uint8_t *)"Package", 7);
            if (AtLibGs_WaitForTCPMessage(250) == ATLIBGS_MSG_ID_DATA_RX) {
                AtLibGs_ParseTCPData(G_received, G_receivedCount, &msg);
                FDICmdPrintf(aWorkspace, "Data: [%s]\n", msg.message);
                G_receivedCount = 0;
                G_received[0] = 0;
                memset(G_received, 0, sizeof(G_received));
                AtLibGs_SendTCPData(cid, (uint8_t *)"Test", 4);
                AtLibGs_Close(cid);
                break;
            } else {
                FDICmdSendString(aWorkspace, "?");
            }
        }
    }
    AtLibGs_DisAssoc();
    return UEZ_ERROR_NONE;
}

#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_SPI.h>
void UEZGUIGainSpanPassThrough(void *aWorkspace)
{
    uint8_t c;
    TUInt32 num;

    FDICmdPrintf(aWorkspace, "Pass through mode (over SPI):\n");
    AtLibGs_SetEcho(ATLIBGS_ENABLE);
    while (1) {
        if (UEZStreamRead(StdinGet(), &c, 1, &num, 0) == UEZ_ERROR_NONE) {
            while (!GainSpan_SPI_SendByte(c))
                GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
        }
        if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, &c))
            if (c != GAINSPAN_SPI_CHAR_IDLE)
                FDICmdPrintf(aWorkspace, "%c", c);
        GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
    }
}

/*---------------------------------------------------------------------------*
 * Routines
 *--------------------------------------------------------------------------*/
int UEZGUIGainSpan(void *aWorkspace, int argc, char *argv[], char *rxBuf)
{
    // todo: Not Finished - Here for future reference
    TUInt8 txData = 0xF5;
    TUInt8 atCmd[] = "AT\r\n";
    TUInt8 rxData = 0;
    TUInt8 printStr[2];
    TUInt32 timeout = 0;
    TUInt32 i = 0;
    ATLIBGS_MSG_ID_E rxMsgId;
    static bool G_didInit = false;

//    GPIO_P2_10, // iSPIChipSelect = WIFI SPI CS
//    GPIO_P1_5, // iDataReadyIO = WIFI IRQ
//    GPIO_P2_21, // iProgramMode = WIFI PROGRAM OFF
//    chip select  P2[10] set low
//    SCK0         62 P0[15]
//    MISO         64 P0[17]
//    MOSO         65 P0[18]
//    WiFi IRQ     80 P1[5]
//    RESET OUT   RESET OUT

    const T_GainSpan_CmdLib_SPISettings settings = { "SSP0", 1000000, // 1 MHz
        GPIO_P2_10, // iSPIChipSelect = WIFI SPI CS
        GPIO_P1_5, // iDataReadyIO = WIFI IRQ
        GPIO_P2_21, // iProgramMode = WIFI PROGRAM OFF
        GPIO_NONE, // iSPIMode = WIFI_MODE SPI
        GPIO_NONE, // iSRSTn = WIFI_SRSTn;

        IUEZGUIGainSpan_IncomingData
        };

    memset(G_received, 0, sizeof(G_received));

if ((argc == 2) && (!strcmp(argv[1], "program"))) {
     // PROGRAMMING Mode
#if CONFIG_USE_EXP4
      UEZGUIGainSpanProgramEXP4();
#elif CONFIG_USE_DK_APP
      UEZGUIGainSpanProgramDKAPP();
#else
      UEZGUIGainSpanProgramUEZGUI56();
#endif
    }

    if (!G_didInit) {
        GainSpan_CmdLib_ConfigureForSPI(&settings);
        G_didInit = true;
    }

    if (UEZSPIOpen("SSP0", &SPI) == UEZ_ERROR_NONE) {

        SPIRequest.iNumTransfers = 1;
        SPIRequest.iBitsPerTransfer = 8;
        SPIRequest.iRate = 1000; // 1 MHz
        SPIRequest.iClockOutPolarity = EFalse; // falling edge
        SPIRequest.iClockOutPhase = ETrue;
        HALInterfaceFind("GPIO2", (T_halWorkspace **)&SPIRequest.iCSGPIOPort);
        SPIRequest.iCSGPIOBit = (1UL << 23);
        SPIRequest.iCSPolarity = EFalse;
        SPIRequest.iDataMISO = &rxData;
        SPIRequest.iDataMOSI = &txData;

        printStr[1] = '\0'; // null terminated string
        if (argc == 1) { // SPI Ping Test, not using AT command, this is legacy
            while (timeout < 1000) {
                // Is Data Ready?
                if (UEZGPIORead(GPIO_P1_5) == ETrue) {
                    // Retrieve a single byte from the WIFI Module
                    if (UEZSPITransferPolled(SPI, &SPIRequest)
                        == UEZ_ERROR_NONE) {
                        // Filter out Idle Bytes
                        if (rxData != 0xF5) {
                            printStr[0] = rxData;
                            FDICmdSendString(aWorkspace,
                                (const char *)printStr);
                        }
                    }
                    timeout = 0;
                }
                timeout++;
            }

            // Sent "AT\r\n" to WIFI Module
            for (i = 0; i < 4; i++) {
                SPIRequest.iDataMOSI = &atCmd[i];
                UEZSPITransferPolled(SPI, &SPIRequest);
            }

            timeout = 0;
            SPIRequest.iDataMOSI = &txData;
            while (timeout < 1000) {
                // Is Data Ready?
                if (UEZGPIORead(GPIO_P1_5) == ETrue) {
                    // Retrieve a single byte from the WIFI Module
                    if (UEZSPITransferPolled(SPI, &SPIRequest)
                        == UEZ_ERROR_NONE) {
                        // Filter out Idle Bytes
                        if (rxData != 0xF5) {
                            printStr[0] = rxData;
                            FDICmdSendString(aWorkspace,
                                (const char *)printStr);
                        }
                    }
                    timeout = 0;
                }
                timeout++;
            }
            FDICmdSendString(aWorkspace, "PASS: OK\n");

        } else if (argc == 2) {
            if (strcmp(argv[1], "at") == 0) {
                TUInt32 start = UEZTickCounterGet();
                // AT sent and watch for response
                SPIRequest.iDataMOSI = "AT\r\n";
                SPIRequest.iNumTransfers = 4;
                G_myflag = 0;
                UEZSPITransferNoBlock(SPI, &SPIRequest, ISetMyFlag, &G_myflag);
                while (UEZSPIIsBusy(SPI)) {
                    UEZTaskDelay(1);
                }
                FDICmdPrintf(aWorkspace, "Sent: %d ms, flag %d\n",
                    UEZTickCounterGetDelta(start), G_myflag);
            } else if (!strcmp(argv[1], "check")) {  // ATLIBGS command test
                rxMsgId = AtLibGs_Check(1000);
                if (rxMsgId == ATLIBGS_MSG_ID_OK) {
                    FDICmdSendString(aWorkspace, "PASS: OK\n");
                } else {
                    FDICmdPrintf(aWorkspace, "ERROR: %u", rxMsgId);
                }
            } else if (!strcmp(argv[1], "scan")) {   // ATLIBGS command test
                ATLIBGS_NetworkScanEntry entries[5];
                uint8_t numentries = 0;
                rxMsgId = AtLibGs_NetworkScan(0, 0, 0, entries, 5, &numentries);
                for (i = 0; i < numentries; i++) {
                    FDICmdPrintf(aWorkspace,
                        "Signal: %d dB, channel: %u, MAC: ", entries[i].signal,
                        entries[i].channel);
                    FDICmdSendString(aWorkspace, entries[i].bssid);
                    FDICmdSendString(aWorkspace, " SSID:  ");
                    FDICmdSendString(aWorkspace, entries[i].ssid);
                    FDICmdSendString(aWorkspace, "\r\n");
                }
                FDICmdPrintf(aWorkspace, "Returned: %u", rxMsgId);
            } else if (!strcmp(argv[1], "scanlong")) {   // ATLIBGS command test
                ATLIBGS_NetworkScanEntry entries[5];
                uint8_t numentries = 0;
                rxMsgId = AtLibGs_NetworkScan(0, 0, 10000, entries, 5,
                    &numentries);
                for (i = 0; i < numentries; i++) {
                    FDICmdPrintf(aWorkspace,
                        "Signal: %d dB, channel: %u, MAC: ", entries[i].signal,
                        entries[i].channel);
                    FDICmdSendString(aWorkspace, entries[i].bssid);
                    FDICmdSendString(aWorkspace, " SSID:  ");
                    FDICmdSendString(aWorkspace, entries[i].ssid);
                    FDICmdSendString(aWorkspace, "\r\n");
                }
                FDICmdPrintf(aWorkspace, "Returned: %u", rxMsgId);
            } else if (!strcmp(argv[1], "scanrepeat")) { // ATLIBGS command test
                rxMsgId = ATLIBGS_MSG_ID_OK;
                uint32_t repeat = 0;
                while (rxMsgId == ATLIBGS_MSG_ID_OK) {
                    ATLIBGS_NetworkScanEntry entries[5];
                    uint8_t numentries = 0;
                    rxMsgId = AtLibGs_NetworkScan(0, 0, 0, entries, 5,
                        &numentries);
                    FDICmdSendString(aWorkspace, "\r\n");
                    for (i = 0; i < numentries; i++) {
                        FDICmdPrintf(aWorkspace,
                            "Signal: %d dB, channel: %u, MAC: ",
                            entries[i].signal, entries[i].channel);
                        FDICmdSendString(aWorkspace, entries[i].bssid);
                        FDICmdSendString(aWorkspace, " SSID:  ");
                        FDICmdSendString(aWorkspace, entries[i].ssid);
                        FDICmdSendString(aWorkspace, "\r\n");
                    }
                    FDICmdPrintf(aWorkspace,
                        "Returned: %u error code, Scan number: %u", rxMsgId,
                        ++repeat);
                }
                FDICmdPrintf(aWorkspace, "Passed %u times.", repeat);
            } else if (!strcmp(argv[1], "atrepeat")) { // ATLIBGS command test
                rxMsgId = ATLIBGS_MSG_ID_OK;
                uint32_t repeat = 0;
                while (rxMsgId == ATLIBGS_MSG_ID_OK) {
                    FDICmdPrintf(aWorkspace, "Checking #%d ... ", ++repeat);
                    rxMsgId = AtLibGs_Check(5000);
                    FDICmdSendString(aWorkspace, "OK\r\n");
                }
                FDICmdPrintf(aWorkspace, "Passed %u times.", repeat);
            } else if (!strcmp(argv[1], "connectAP")) {
                // connect doesn't work yet

                AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE); // connect to AP mode
                AtLibGs_DHCPSet(1);       // set DHCP

                AtLibGs_CalcNStorePSK("TestRouterNoInternet", "fdifdifdi");
                if (AtLibGs_Assoc("TestRouterNoInternet", "", 0)
                    == ATLIBGS_MSG_ID_OK) {
                    FDICmdSendString(aWorkspace, "Connected to AP!"); // association success
                } else {
                    FDICmdSendString(aWorkspace, "Could not connect to AP.");
                }
            } else if (!strcmp(argv[1], "adhoc1")) {
                UEZGUIGainSpanAdHoc1(aWorkspace);
            } else if (!strcmp(argv[1], "adhoc2")) {
                UEZGUIGainSpanAdHoc2(aWorkspace);
            } else if (!strcmp(argv[1], "server")) {
                UEZGUIGainSpanServer(aWorkspace);
            } else if (!strcmp(argv[1], "rserver")) {
                UEZGUIGainSpanRelayServer(aWorkspace);
            } else if (!strcmp(argv[1], "client")) {
                UEZGUIGainSpanClient(aWorkspace);
            } else if (!strcmp(argv[1], "status")) {
                UEZGUIGainSpanStatus(aWorkspace);
            } else if (!strcmp(argv[1], "pt")) {
                UEZGUIGainSpanPassThrough(aWorkspace);
            } else {
                FDICmdSendString(aWorkspace, "Not a valid command.");
            }

        } else if (argc == 3) {
            if (!strcmp(argv[1], "test")) {
                if (!strcmp(argv[2], "newcommands")) {
//                    TestNewCommands();
                } else if (!strcmp(argv[2], "networkscan")) {
//                    TestNetworkScan();
                } else if (!strcmp(argv[2], "regulatorydomain")) {
//                    TestRegulatoryDomain();
                } else if (!strcmp(argv[2], "getprofile")) {
//                    TestGetProfile();
                } else if (!strcmp(argv[2], "echo")) {
//                    TestEchoOff();
//                    TestEchoOn();
                } else if (!strcmp(argv[2], "getinfo")) {
                    TestGetInfo(aWorkspace);
                } else {
                    FDICmdSendString(aWorkspace,
                        "Test command not recognized.");
                }
            } else if (!strcmp(argv[1], "rtest")) {
                uint32_t repeat = 0;
                G_numFailures = 0;
                while (!G_numFailures) {
                    repeat++;
                    FDICmdPrintf(aWorkspace, "Test #%d: ", repeat);
                    if (!strcmp(argv[2], "newcommands")) {
                        //                    TestNewCommands();
                    } else if (!strcmp(argv[2], "networkscan")) {
                        //                    TestNetworkScan();
                    } else if (!strcmp(argv[2], "regulatorydomain")) {
                        //                    TestRegulatoryDomain();
                    } else if (!strcmp(argv[2], "getprofile")) {
                        //                    TestGetProfile();
                    } else if (!strcmp(argv[2], "echo")) {
                        //                    TestEchoOff();
                        //                    TestEchoOn();
                    } else if (!strcmp(argv[2], "getinfo")) {
                        TestGetInfo(aWorkspace);
                    } else {
                        FDICmdSendString(aWorkspace,
                            "Test command not recognized.");
                    }
                    if (G_numFailures == 0)
                        FDICmdPrintf(aWorkspace, "PASS");
                }
            } else {
                FDICmdSendString(aWorkspace, "Not a valid command.");
            }

        } else if (argc == 4) { // type in "GainSpan connectAP SSID Password" to connect
            if (!strcmp(argv[1], "connectAP")) {

                AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE); // connect to AP mode
                AtLibGs_DHCPSet(1);       // set DHCP

                AtLibGs_CalcNStorePSK(argv[2], argv[3]);
                if (AtLibGs_Assoc(argv[2], 0, 0) == ATLIBGS_MSG_ID_OK) {
                    ATLIBGS_NetworkStatus status;

                    FDICmdSendString(aWorkspace, "Connected to AP!\n"); // association success
                    AtLibGs_GetNetworkStatus(&status);
                    FDICmdPrintf(aWorkspace, "IP Address: %d.%d.%d.%d\n",
                        status.addr.ipv4[0], status.addr.ipv4[1],
                        status.addr.ipv4[2], status.addr.ipv4[3]);
                } else {
                    FDICmdSendString(aWorkspace, "Could not connect to AP.");
                }
            } else {
                FDICmdSendString(aWorkspace, "Not a valid command.");
            }
        } else {
            FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
        }
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Error opening SPI\n");
    }
    return 0;
}

// programming mode for DK Carrier R5 App header
void UEZGUIGainSpanProgramDKAPP(void)
{
    printf("Programming mode ...\n");
    UEZTaskDelay(500);

    RTOS_ENTER_CRITICAL()
    ;

//    UEZGPIOClear(GPIO_P1_16);        // WIFI_SRSTn
//    UEZGPIOSetMux(GPIO_P1_16, 0);
//    UEZGPIOOutput(GPIO_P1_16);       // WIFI_SRSTn

    UEZGPIOInput(GPIO_P1_5);       // WIFI IRQ

    UEZGPIOSet(GPIO_P2_21);          // WIFI PROGRAM ON
    UEZGPIOOutput(GPIO_P2_21);       // WIFI PROGRAM ON

    UEZGPIOUnlock(GPIO_P0_10);      // 1788 TX
    UEZGPIOSetMux(GPIO_P0_10, 0);
    UEZGPIOOutput(GPIO_P0_10);

    UEZGPIOUnlock(GPIO_P0_11);      // 1788 RX
    UEZGPIOSetMux(GPIO_P0_11, 0);
    UEZGPIOInput(GPIO_P0_11);

    UEZGPIOUnlock(GPIO_P0_2);       // WIFI TX
    UEZGPIOSetMux(GPIO_P0_2, 0);
    UEZGPIOOutput(GPIO_P0_2);

    UEZGPIOUnlock(GPIO_P0_3);       // WIFI RX
    UEZGPIOSetMux(GPIO_P0_3, 0);
    UEZGPIOInput(GPIO_P0_3);

    UEZTaskDelay(1000);

//    UEZGPIOSet(GPIO_P1_16);          // WIFI_SRSTn
// No Reset signal, so push reset switch on GS WAB board.

    CPUDisableInterrupts();
    while (1) {
        if (LPC_GPIO0->PIN & (1 << 11))  // 1788 RX = GPIO_P0_11
            LPC_GPIO0->SET = (1 << 2);   // WIFI TX = GPIO_P0_2
        else
            LPC_GPIO0->CLR = (1 << 2);   // WIFI TX = GPIO_P0_2

        if (LPC_GPIO0->PIN & (1 << 3))   // WIFI RX = GPIO_P0_3
            LPC_GPIO0->SET = (1 << 10);  // 1788 TX = GPIO_P0_10
        else
            LPC_GPIO0->CLR = (1 << 10);  // 1788 TX = GPIO_P0_10
    }
}

// PROGRAMMING Mode for uEZGUI 56
void UEZGUIGainSpanProgramUEZGUI56(void)
{
    printf("Programming mode ...\n");
    UEZTaskDelay(500);

    RTOS_ENTER_CRITICAL()
    ;

    UEZGPIOClear(GPIO_P1_5);        // WIFI_SRSTn
    UEZGPIOSetMux(GPIO_P1_5, 0);
    UEZGPIOOutput(GPIO_P1_5);       // WIFI_SRSTn

    UEZGPIOInput(GPIO_P1_5);       // WIFI IRQ

    UEZGPIOSet(GPIO_P1_6);          // WIFI PROGRAM ON
    UEZGPIOOutput(GPIO_P1_6);       // WIFI PROGRAM ON

    UEZGPIOUnlock(GPIO_P0_2);       // 1788 TX
    UEZGPIOSetMux(GPIO_P0_2, 0);
    UEZGPIOOutput(GPIO_P0_2);

    UEZGPIOUnlock(GPIO_P0_3);       // 1788 RX
    UEZGPIOSetMux(GPIO_P0_3, 0);
    UEZGPIOInput(GPIO_P0_3);

    UEZGPIOUnlock(GPIO_P0_15);      // WIFI TX
    UEZGPIOSetMux(GPIO_P0_15, 0);
    UEZGPIOOutput(GPIO_P0_15);

    UEZGPIOUnlock(GPIO_P0_16);      // WIFI RX
    UEZGPIOSetMux(GPIO_P0_16, 0);
    UEZGPIOInput(GPIO_P0_16);

    UEZTaskDelay(1000);

    UEZGPIOSet(GPIO_P1_5);          // WIFI_SRSTn

    CPUDisableInterrupts();
    while (1) {
        if (LPC_GPIO0->PIN & (1 << 3)) // 1788 RX = GPIO_P0_3
            LPC_GPIO0->SET = (1 << 15); // WIFI TX = GPIO_P0_15
        else
            LPC_GPIO0->CLR = (1 << 15);

        if (LPC_GPIO0->PIN & (1 << 16)) // WIFI RX = GPIO_P0_16
            LPC_GPIO0->SET = (1 << 2);  // 1788 TX = GPIO_P0_2
        else
            LPC_GPIO0->CLR = (1 << 2);  // 1788 TX = GPIO_P0_2
    }
}

void UEZGUIGainSpanProgramEXP4(void)
{
    printf("Programming mode ...\n");
    UEZTaskDelay(500);

    RTOS_ENTER_CRITICAL()
    ;

    UEZGPIOClear(GPIO_P1_16);        // WIFI_SRSTn
    UEZGPIOSetMux(GPIO_P1_16, 0);
    UEZGPIOOutput(GPIO_P1_16);       // WIFI_SRSTn

    UEZGPIOInput(GPIO_P0_5);       // WIFI IRQ

    UEZGPIOSet(GPIO_P1_14);          // WIFI PROGRAM ON
    UEZGPIOOutput(GPIO_P1_14);       // WIFI PROGRAM ON

    UEZGPIOUnlock(GPIO_P0_2);       // 1788 TX
    UEZGPIOSetMux(GPIO_P0_2, 0);
    UEZGPIOOutput(GPIO_P0_2);

    UEZGPIOUnlock(GPIO_P0_3);       // 1788 RX
    UEZGPIOSetMux(GPIO_P0_3, 0);
    UEZGPIOInput(GPIO_P0_3);

    UEZGPIOUnlock(GPIO_P0_15);      // WIFI TX
    UEZGPIOSetMux(GPIO_P0_15, 0);
    UEZGPIOOutput(GPIO_P0_15);

    UEZGPIOUnlock(GPIO_P0_16);      // WIFI RX
    UEZGPIOSetMux(GPIO_P0_16, 0);
    UEZGPIOInput(GPIO_P0_16);

    UEZTaskDelay(1000);

    UEZGPIOSet(GPIO_P1_16);          // WIFI_SRSTn

    CPUDisableInterrupts();
    while (1) {
        if (LPC_GPIO0->PIN & (1 << 3)) // 1788 RX = GPIO_P0_3
            LPC_GPIO0->SET = (1 << 15); // WIFI TX = GPIO_P0_15
        else
            LPC_GPIO0->CLR = (1 << 15);

        if (LPC_GPIO0->PIN & (1 << 16)) // WIFI RX = GPIO_P0_16
            LPC_GPIO0->SET = (1 << 2);  // 1788 TX = GPIO_P0_2
        else
            LPC_GPIO0->CLR = (1 << 2);  // 1788 TX = GPIO_P0_2
    }
}
