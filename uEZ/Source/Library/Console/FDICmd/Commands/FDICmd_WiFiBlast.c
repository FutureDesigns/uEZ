/*-------------------------------------------------------------------------*
 * File:  FDICmd_WiFiBlast.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZI2C.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <uEZPlatform.h>
#include <uEZNetwork.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static TBool G_wifiBlastStarted = EFalse;

void WiFiBlastTask(void)
{
    T_uezDevice network = 0;
    T_uezError error;
    T_uezNetworkStatus status;
    T_uezNetworkSocket socket;
    printf("WifiBlast: Starting...\n");
    T_uezNetworkAddr addr;
    char buffer[80];
    TUInt32 count = 0;

    // Hardcoded to send UDP packets to 192.168.1.2, port 10080
    addr.v4[0] = 192;
    addr.v4[1] = 168;
    addr.v4[2] = 1;
    addr.v4[3] = 2;

    // Get access to the network
    error = UEZNetworkOpen("WirelessNetwork0", &network);
    if (error)
        goto stop;

    // Wait/Ensure we have joined a network (can be done with other commands)
    do {
        error = UEZNetworkGetStatus(network, &status);
        if (error)
            goto stop;

        if (status.iJoinStatus != UEZ_NETWORK_JOIN_STATUS_SUCCESS) {
            printf("WifiBlast: Waiting for network join\n");
            UEZTaskDelay(1000);
        }
    } while (status.iJoinStatus != UEZ_NETWORK_JOIN_STATUS_SUCCESS);

    // Go ahead and start spamming the network with as many packets as we can
    while (1) {
        error = UEZNetworkSocketCreate(network, UEZ_NETWORK_SOCKET_TYPE_TCP,
            &socket);
        if (error) {
            printf("WiFiBlast: Cannot create socket!\n");
            UEZTaskDelay(1000);
        } else {
            error = UEZNetworkSocketConnect(network, socket, &addr, 8010);
            if (error) {
                printf("WiFiBlast: Cannot connect!\n");
            } else {
                while (1) {
                    // Good, we connected (which really just sets up a destination);
                    // Let's send a packet to the target
                    sprintf(buffer, "WiFiBlast: Packet %d\r\n", ++count);
                    error = UEZNetworkSocketWrite(network, socket, buffer,
                        strlen(buffer), ETrue, 5000);
                    if (error)
                        break;
                    // Delay a little bit to share the CPU a bit even though the
                    // above may delay.  Our goal is to keep the transmitter
                    // energy level high.
                    UEZTaskDelay(1);

                    // Print a + to the display to show we sent a packet
                    printf("+");
                }
            }
            // Close the connection and try to reopen
            UEZNetworkSocketClose(network, socket);
            UEZNetworkSocketDelete(network, socket);
        }
    }

stop:
    if (error)
        printf("WifiError: %d\n", error);
    if (network)
        UEZNetworkClose(network);
    printf("WifiBlast: Ended.\n");
    G_wifiBlastStarted = EFalse;
}

T_uezError IWiFiBlastStart(void)
{
    T_uezError error = UEZ_ERROR_NONE;

    if (G_wifiBlastStarted)
        return UEZ_ERROR_ALREADY_EXISTS;

    error = UEZTaskCreate((T_uezTaskFunction)WiFiBlastTask, "WiFiBlast",
        UEZ_TASK_STACK_BYTES(4096), (void *)0, UEZ_PRIORITY_NORMAL, 0);
    if (error == UEZ_ERROR_NONE) {
        G_wifiBlastStarted = ETrue;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdWiFiBlast
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      WiFiBlast (wavefile)
 *
 *  Where,
 *      (wavfile) Name of file to play
 *
 *  Plays a 16-bit 22 kHz WAV file out the I2S (or whatever is hooked up to
 *  UEZWiFiBlast).
 *
 *  @param [in]     *aWorkspace     FDICmd workspace, needed for outputs
 *
 *  @param [in]     argc            Number of parsed arguments
 *
 *  @param [in]     *argv[]         Parsed arguments (strings)
 *
 *  @return         TUInt32         0 if success, non-zero for error.
 */
/*---------------------------------------------------------------------------*/
int UEZCmdWiFiBlast(void *aWorkspace, int argc, char *argv[])
{
    T_uezError error;

    if (argc == 1) {
        error = IWiFiBlastStart();
        if (error == UEZ_ERROR_ALREADY_EXISTS) {
            FDICmdPrintf(aWorkspace, "FAIL: WIFI Blast already running!\n");
        } else if (error == UEZ_ERROR_NONE) {
            FDICmdPrintf(aWorkspace, "PASS: WIFI Blast task started.\n");
        } else {
            FDICmdPrintf(aWorkspace,
                "FAIL: WIFI Blast task failed to start. (Error=%d)\n", error);
        }
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nUSAGE: WiFiBlast\n");
    }

    return UEZ_ERROR_NONE;
}


/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_WiFiBlast.c
 *-------------------------------------------------------------------------*/
