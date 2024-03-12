/*-------------------------------------------------------------------------*
 * File:  NetworkThroughput.c
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include <uEZ.h>
#include <uEZPlatformAPI.h>
#include <uEZNetwork.h>
#include <sys/socket.h>

#if (RTOS==FreeRTOS)
#include <Source/RTOS/FreeRTOS/include/task.h>
#elif ( RTOS == SafeRTOS )
#include "SafeRTOS_API.h"
#endif

#include "NetworkThroughput.h"

/* The port on which we listen. */
#define NET_TP_PORT        ( 5001 )

static char G_sendBuffer[1400*1];
extern T_uezTask G_lwipTask;


static void INetworkThroughputProcessConnection(
        T_uezDevice aNetwork,
        T_uezNetworkSocket aSocket)
{
    static char receiveBuffer[512];
    TUInt32 receiveLength;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 receiveTotal = 0;
    TUInt32 sendTotal = 0;
    TUInt32 start, end;

    /* We expect to immediately get data. */
    printf("NetworkThroughput: Receiving data ...\n");
    start = UEZTickCounterGet();
    do {
        error = UEZNetworkSocketRead(aNetwork, aSocket, receiveBuffer,
                sizeof(receiveBuffer), &receiveLength, 500);
        if (error != UEZ_ERROR_TIMEOUT) {
            if (error)
                break;
            //printf("  %d %d\n", receiveLength, receiveTotal);
            printf(".");
            receiveTotal += receiveLength;
        } else {
            printf(".");
        }
    } while (receiveTotal < 1*1024*1024);
    end = UEZTickCounterGet();

    if (!error) {
        printf("NetworkThroughput: Received in %d ms\n", end-start);
        printf("NetworkThroughput: Sending data\n");
//vTraceClear();
//vTraceStart();
        start = UEZTickCounterGet();
        for (sendTotal = 0; sendTotal < 1*1024*1024; sendTotal += sizeof(G_sendBuffer)) {
          error = UEZNetworkSocketWrite(aNetwork, aSocket, G_sendBuffer, sizeof(G_sendBuffer), 
                                        EFalse /*((sendTotal+sizeof(G_sendBuffer))>=1024*1024)?ETrue:ETrue*/, 500);
            if (error != UEZ_ERROR_TIMEOUT) {
                if (error)
                    break;
            }
        }
        end = UEZTickCounterGet();
//vTraceStop();

        if (error) {
            printf("NetworkThroughput: Send Error %d\n", error);
        } else {
            printf("NetworkThroughput: Sent in %d ms\n", end-start);
        }
    } else {
        printf("NetworkThroughput: Receive Error %d\n", error);
    }
    UEZNetworkSocketClose(aNetwork, aSocket);
    UEZNetworkSocketDelete(aNetwork, aSocket);
}

static void INetworkThroughputWrite(
        T_uezDevice aNetwork,
        T_uezNetworkSocket aSocket)
{
    static char receiveBuffer[512];
    TUInt32 receiveLength;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 receiveTotal = 0;
    TUInt32 sendTotal = 0;
    TUInt32 start, end;
    (void) receiveBuffer;
    PARAM_NOT_USED(receiveLength);
    PARAM_NOT_USED(receiveTotal);

    printf("NetworkThroughput: Sending data\n");
    start = UEZTickCounterGet();
    for (sendTotal = 0; sendTotal < 1024*1024; sendTotal += sizeof(G_sendBuffer)) {
      error = UEZNetworkSocketWrite(aNetwork, aSocket, G_sendBuffer, sizeof(G_sendBuffer), 
                                    EFalse /*((sendTotal+sizeof(G_sendBuffer))>=1024*1024)?ETrue:ETrue*/, 500);
        if (error != UEZ_ERROR_TIMEOUT) {
            if (error)
                break;
        }
    }
    end = UEZTickCounterGet();

    if (error) {
        printf("NetworkThroughput: Send Error %d\n", error);
    } else {
        printf("NetworkThroughput: Sent in %d ms\n", end-start);
    }
}

/*------------------------------------------------------------*/

void NetworkThroughputServerTask(void)
{
    T_uezNetworkSocket socket;
    T_uezNetworkSocket newSocket;
    TUInt32 i;
    T_uezDevice network;
    T_uezNetworkAddr addr = {0};
    PARAM_NOT_USED(addr);
    (void) INetworkThroughputWrite;

    if(G_lwipTask != NULL) {
      lwip_socket_thread_init(); // if lwip init make sure to init per thread objects
    }

    for (i=0; i<sizeof(G_sendBuffer); i++) {
//        G_sendBuffer[i] = i & 0xFF;
        G_sendBuffer[i] = 'A' + (i % 26);
    }

    if (UEZNetworkOpen("WiredNetwork0", &network) != UEZ_ERROR_NONE) {
        printf("NetworkThroughput: Could not open network!\n");
    } else {
        /* Create a new tcp connection handle */
        if (UEZNetworkSocketCreate(network, UEZ_NETWORK_SOCKET_TYPE_TCP,
                &socket) == UEZ_ERROR_NONE) {
#if 1
            // Setup the socket to be on the HTTP port
            UEZNetworkSocketBind(network, socket, 0, NET_TP_PORT);
            // Put the socket into listen mode
            UEZNetworkSocketListen(network, socket);

            /* Loop forever */
            for (;;) {
                printf("NetworkThroughput: Waiting for connection\n");
                /* Wait for connection. */
                UEZNetworkSocketAccept(network, socket, &newSocket,
                        UEZ_TIMEOUT_INFINITE);

                printf("NetworkThroughput: Accepted connection\n");

                /* Service connection. */
                if (newSocket) {
                    /* Service connection. */
                    INetworkThroughputProcessConnection(network, newSocket);
                    UEZNetworkSocketDelete(network, newSocket);
                }
                printf("NetworkThroughput: Connection Closed\n");
            }
#else
    // Client
    addr.v4[0] = 192;
    addr.v4[1] = 168;
    addr.v4[2] = 10;
    addr.v4[3] = 21;
    UEZNetworkSocketConnect(network, socket, &addr, 5001);
    INetworkThroughputWrite(network, socket);
    UEZNetworkSocketClose(network, socket);
    UEZNetworkSocketDelete(network, socket);
#endif
        }
    }

    while (1) {
        // Sit here doing nothing
        UEZTaskDelay(1000);
    }
}

T_uezError NetworkThroughputStart(void)
{
    printf("Throughput testing started\n");
    return UEZTaskCreate((T_uezTaskFunction)NetworkThroughputServerTask, "NetworkThroughput",
            UEZ_TASK_STACK_BYTES(1500), (void *)0, UEZ_PRIORITY_NORMAL, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  NetworkThroughput.c
 *-------------------------------------------------------------------------*/
