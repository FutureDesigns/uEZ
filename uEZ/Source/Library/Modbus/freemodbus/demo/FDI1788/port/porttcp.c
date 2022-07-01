/*
 * FreeModbus Libary: lwIP Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttcp.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <string.h>

#include "port.h"

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/api.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502           /* TCP listening port. */
#define MB_TCP_BUF_SIZE     ( 256 + 7 )   /* Must hold a complete Modbus TCP frame. */

#define MB_TCP_CONNECTION_TIMEOUT       120     /* connection timeout in seconds */
#define MB_TCP_CONNECTION_READ_TIMEOUT  20      /* read timeout in ms */

#define MB_TCP_MAX_CLIENTS 5

#define MB_TCP_SERVER_TASK_STACK_SIZE   UEZ_TASK_STACK_BYTES(4096)
#define MB_TCP_LISTENER_TASK_STACK_SIZE UEZ_TASK_STACK_BYTES(4096)

/* ----------------------- Prototypes ---------------------------------------*/
void            vMBPortEventClose( void );

/* ----------------------- Static variables ---------------------------------*/
static T_uezTask _ModbusTCPIPTask;
static USHORT usPort;

typedef struct {
  T_uezDevice         aNetwork;
  T_uezNetworkSocket  aSocket;
  T_uEZNetworkConnectionInfo aConnection;
  T_uezSemaphore      aSemaphore;
  T_uezTask           aTask;
  UCHAR               aucTCPBuf[MB_TCP_BUF_SIZE];
  USHORT              usTCPBufPos;
} xMBTCPClient;

static xMBTCPClient  xClients[MB_TCP_MAX_CLIENTS];

static USHORT     usTCPConnectionTimeout = MB_TCP_CONNECTION_TIMEOUT;

static pxMBTCPConnectionAttemptCB pxConnectionAttemptCB = NULL;
static pxMBTCPConnectionCloseCB pxConnectionCloseCB = NULL;

/* ----------------------- Static functions ---------------------------------*/
static void     prvvMBPortReleaseClient( xMBTCPClient *pClientHandle );
static void     prvvMBPortReleaseSocket( T_uezDevice aNetwork, T_uezNetworkSocket aSocket );

/* ----------------------- Begin implementation -----------------------------*/


/* Modbus TCP Listener Task, reads from an active connection */
static void
prvMBTCPListenerTask( T_uezTask aMyTask, void *arg )
{
    TUInt32 numBytes;
    UCHAR buffer[512];
    USHORT usLength;
    UCHAR *pBuf = NULL;
    USHORT usConsume = 0;
    T_uezError error;
    USHORT usTimeouts;
    xMBTCPClient    *pClientHandle = (xMBTCPClient *)arg;

    for (;;)
    {
        /* uEZ has a memory leak with tasks being created/deleted.  To overcome this we use a
         * semaphore to block this task from running when not in use.  Grab the semaphore when
         * it is time to run.  Release it when finished.  If we error out, close the socket but
         * keep the semaphore.  When a new connection is created the Semaphore will be released
         * letting this task run again.*/
        UEZSemaphoreGrab(pClientHandle->aSemaphore, UEZ_TIMEOUT_INFINITE);

        usTimeouts = 0;

        /* forever loop, while socket connection is active */
        for (;;)
        {
            /* read the socket */
            error = UEZNetworkSocketRead(pClientHandle->aNetwork, pClientHandle->aSocket, buffer, sizeof(buffer), 
                                         &numBytes, MB_TCP_CONNECTION_READ_TIMEOUT);

            UEZTaskDelay(1);
#ifdef MB_TCP_DEBUG
            //vMBPortLog( MB_LOG_DEBUG,
            //            "MBTCP-READ", "Socket: %d bytes: %d error: %d ticks: %d\r\n",
            //            pClientHandle->aSocket, numBytes, error, UEZTickCounterGet());
#endif

mb_tcp_receive_buffer_parsing:
            /* was this a timeout? */
            if ((numBytes == 0) && (error == UEZ_ERROR_TIMEOUT))
            {
                usTimeouts++;

                if (usTimeouts > ((MB_TCP_CONNECTION_TIMEOUT*1000)/MB_TCP_CONNECTION_READ_TIMEOUT))
                {
                    /* the socket has timed out */
#ifdef MB_TCP_DEBUG
                    vMBPortLog( MB_LOG_DEBUG, 
                                "MBTCP-TIMEOUT", "Connection deemed inactive! Dropping it.\r\n");
#endif
                    prvvMBPortReleaseClient(pClientHandle);
                    break;
                }
                else
                {
                    continue;
                }
            }
            else if ((error != UEZ_ERROR_NONE) && (error != UEZ_ERROR_TIMEOUT))
            {

#ifdef MB_TCP_DEBUG
                    vMBPortLog( MB_LOG_DEBUG, 
                                "MBTCP-ERROR", "Socket read returned error %d.\r\n", error);
#endif
                 /* the socket has an error, close it */
                prvvMBPortReleaseClient(pClientHandle);
                break;
            }

            usTimeouts = 0;
            
            /* copy as much into our working buffer as possible */
            if( ( pClientHandle->usTCPBufPos + numBytes ) >= MB_TCP_BUF_SIZE )
            {
                usConsume = MB_TCP_BUF_SIZE - pClientHandle->usTCPBufPos;
                memcpy( &pClientHandle->aucTCPBuf[pClientHandle->usTCPBufPos], buffer, 
                        usConsume );
                pClientHandle->usTCPBufPos += usConsume;
                pBuf = &buffer[usConsume];         
            }
            else
            {
                memcpy( &pClientHandle->aucTCPBuf[pClientHandle->usTCPBufPos], buffer, numBytes );
                pClientHandle->usTCPBufPos += numBytes;
                pBuf = &buffer[numBytes]; 
            }

            /* If we have received the MBAP header we can analyze it and calculate
             * the number of bytes left to complete the current request. If complete
             * notify the protocol stack.
             */
            if( pClientHandle->usTCPBufPos >= MB_TCP_FUNC )
            {
                /* Length is a byte count of Modbus PDU (function code + data) and the
                 * unit identifier. */
                usLength = pClientHandle->aucTCPBuf[MB_TCP_LEN] << 8U;
                usLength |= pClientHandle->aucTCPBuf[MB_TCP_LEN + 1];
                
                usLength += MB_TCP_UID;

                /* Is the frame already complete. */
                if( pClientHandle->usTCPBufPos < ( usLength ) )
                {
                    /* not enough, so keep getting more */
                }
                else
                {
#ifdef MB_TCP_DEBUG
                    prvvMBTCPLogFrame( (UCHAR *)"MBTCP-RECV", &pClientHandle->aucTCPBuf[0], usLength);
#endif
                    /* see if we had extra data */
                    if( pClientHandle->usTCPBufPos > ( usLength ) )
                    {
                        /* if we have some left over, copy it back into buffer */
                        memcpy( buffer, &pClientHandle->aucTCPBuf[usLength],
                                pClientHandle->usTCPBufPos - usLength );

                        /* anything left in the buffer from the original read that didn't fit? */
                        if (pBuf)
                        {
                            memcpy( &buffer[pClientHandle->usTCPBufPos - usLength], pBuf,
                                    &buffer[numBytes] - pBuf);
                           
                            numBytes = (pClientHandle->usTCPBufPos - usLength) + (&buffer[numBytes] - pBuf);
                        }
                        else
                        {
                            numBytes = (pClientHandle->usTCPBufPos - usLength);
                        }
                    }

                    /* pass message off for handling, the response uses the same buffer */
                    if (eMBTCPHandleMessage( pClientHandle, &pClientHandle->aucTCPBuf[MB_TCP_FUNC], 
                        pClientHandle->usTCPBufPos - MB_TCP_FUNC ) == MB_EIO)
                    {
                        /* error */
                        prvvMBPortReleaseClient(pClientHandle);
                        break;
                    }

                    /* see if we had extra data */
                    if( pClientHandle->usTCPBufPos > ( usLength ) )
                    {
                        /* reset buffer position for next frame */
                        pClientHandle->usTCPBufPos = 0; 
                         
                        goto mb_tcp_receive_buffer_parsing;

                    }

                    /* reset buffer position for next frame */
                    pClientHandle->usTCPBufPos = 0;  
                }
            }
        } /* interior forever loop, when we have the semaphore */

        /* we made it here, the semaphore is still grabbed, so we won't re-enter the interior loop
         * until we have a new connection and the server task has released this semaphore */

    } /* outside forever loop, to keep the task forever active */

    /* how did we get here? try to exit gracefully */
    UEZTaskDelete(pClientHandle->aTask);
}

static xMBTCPClient *
prvvMBPortNewClient(T_uezDevice aNetwork, T_uezNetworkSocket aSocket, T_uEZNetworkConnectionInfo *aConnection)
{
  USHORT  index;

  for (index = 0; index < MB_TCP_MAX_CLIENTS; index++)
  {
    if ((xClients[index].aNetwork == NULL) &&
        (xClients[index].aSocket == NULL))
    {
      // found an empty slot
      xClients[index].aNetwork = aNetwork;
      xClients[index].aSocket = aSocket;
      xClients[index].usTCPBufPos = 0;
      memcpy(&xClients[index].aConnection, aConnection, sizeof(xClients[index].aConnection));

      return &xClients[index];
    }
  }

  // TODO if we made it here, no empty slots.  we could eventually terminate oldest client
  // but connection timeouts will do that automatically
  // and would allow a new connection at a later point
  return NULL;
}

static BOOL 
prvvMBPortFreeClient(xMBTCPClient *pClientHandle)
{
  USHORT  index;

  for (index = 0; index < MB_TCP_MAX_CLIENTS; index++)
  {
    if ((xClients[index].aNetwork == pClientHandle->aNetwork) &&
        (xClients[index].aSocket == pClientHandle->aSocket))
    {
      // found client
      xClients[index].aNetwork = NULL;
      xClients[index].aSocket = NULL;
      xClients[index].usTCPBufPos = 0;
      memset(&xClients[index].aConnection, 0, sizeof(xClients[index].aConnection));

      return TRUE;
    }
  }

  // if we made it here, we don't have this client and cannot free it
  return FALSE;
}

static void
prvvMBPortReleaseSocket( T_uezDevice aNetwork, T_uezNetworkSocket aSocket )
{
    UEZNetworkSocketClose( aNetwork, aSocket );
    UEZNetworkSocketDelete( aNetwork, aSocket );
}

static void
prvvMBPortReleaseClient( xMBTCPClient *pClientHandle )
{
    if( pClientHandle != NULL )
    {
        /* if an application callback has been registered, call it */
        if (pxConnectionCloseCB)
        { 
            pxConnectionCloseCB(&pClientHandle->aConnection);
        }

        prvvMBPortReleaseSocket(pClientHandle->aNetwork, pClientHandle->aSocket );
        
        prvvMBPortFreeClient( pClientHandle );

        {
#ifdef MB_TCP_DEBUG
            vMBPortLog( MB_LOG_DEBUG, "MBTCP-CLOSE", "Closed connection\r\n");
                        // to %d.%d.%d.%d. %d\r\n",
                        // ip4_addr1( &( pxPCB->remote_ip ) ),
                        // ip4_addr2( &( pxPCB->remote_ip ) ),
                        // ip4_addr3( &( pxPCB->remote_ip ) ),
                        // ip4_addr4( &( pxPCB->remote_ip ) ),
                        // pxPCB->remote_port );
#endif
        }
    }
}

void 
prvvMBPortReleaseAllClients(  )
{
    USHORT index;

    /* Shutdown any open client sockets. */
    for (index = 0; index < MB_TCP_MAX_CLIENTS; index++)
    {
      if (xClients[index].aSocket)
      {
          prvvMBPortReleaseClient( &xClients[index] );
      }
    }
}

void
vMBTCPPortClose(  )
{
    /* Shutdown any open client sockets. */
    prvvMBPortReleaseAllClients(  );

    /* Shutdown our listening socket. */
    // prvvMBPortReleaseClient( pxPCBListen );

    /* Release resources for the event queue. */
    vMBPortEventClose(  );
}

void
vMBTCPPortDisable( void )
{
    prvvMBPortReleaseAllClients(  );
}

static void
prvMBTCPProcessConnection(T_uezDevice aNetwork, T_uezNetworkSocket aSocket)
{
    xMBTCPClient    *pClientHandle;
    T_uEZNetworkConnectionInfo aConnection;

    /* get the remote ip, port */
    UEZNetworkGetConnectionInfo(aNetwork, aSocket, &aConnection);

    /* if an application callback has been registered, call it 
     * a return value of TRUE means accept connection, FALSE means reject */
    if (pxConnectionAttemptCB && 
        (pxConnectionAttemptCB(&aConnection) == FALSE))
    {
#ifdef MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "New client rejected by application callback\r\n");
                    // %d.%d.%d.%d %d\r\n",
                    // ip4_addr1( &( pxPCB->remote_ip ) ),
                    // ip4_addr2( &( pxPCB->remote_ip ) ),
                    // ip4_addr3( &( pxPCB->remote_ip ) ), 
                    // ip4_addr4( &( pxPCB->remote_ip ) ), 
                    //pxPCB->remote_port);
#endif
        prvvMBPortReleaseSocket( aNetwork, aSocket );
        return;
    }

    /* accept the new client if we can. */
    pClientHandle = prvvMBPortNewClient(aNetwork, aSocket, &aConnection);

    if( pClientHandle )
    {
        /* see if this client already has a listener task */
        if (!pClientHandle->aTask)
        {
            /* create a semaphore for the task to be paced with.  uEZ has a memory leak, so we don't
             * want to create/destroy these tasks often.  For now create once, and keep using forever. */
            if (UEZSemaphoreCreateBinary(&pClientHandle->aSemaphore) != UEZ_ERROR_NONE)
            {        
#ifdef MB_TCP_DEBUG
                vMBPortLog( MB_LOG_ERROR, "MBTCP-ACCEPT", "Unable to create semaphore\r\n");
#endif
            }

            /* grab the semaphore, it keeps the listener task from running */
            UEZSemaphoreGrab(pClientHandle->aSemaphore, UEZ_TIMEOUT_INFINITE);

            /* Create a task the Modbus Server task */
            UEZTaskCreate((T_uezTaskFunction)prvMBTCPListenerTask, "Modbus_TCPIP_Listener", MAIN_TASK_STACK_SIZE, 
                           pClientHandle, UEZ_PRIORITY_NORMAL, &pClientHandle->aTask);
        }

        /* release the semaphore so the listener task can now run */
        UEZSemaphoreRelease(pClientHandle->aSemaphore);

#ifdef MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Accepted new client\r\n");
                    // %d.%d.%d.%d %d\r\n",
                    // ip4_addr1( &( pxPCB->remote_ip ) ),
                    // ip4_addr2( &( pxPCB->remote_ip ) ),
                    // ip4_addr3( &( pxPCB->remote_ip ) ), 
                    // ip4_addr4( &( pxPCB->remote_ip ) ), 
                    // pxPCB->remote_port);
#endif

    }
    else
    {
    
#ifdef MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Could not accept new client, no room\r\n");
                    // %d.%d.%d.%d\r\n",
                    // ip4_addr1( &( pxPCB->remote_ip ) ),
                    // ip4_addr2( &( pxPCB->remote_ip ) ),
                    // ip4_addr3( &( pxPCB->remote_ip ) ), 
                    // ip4_addr4( &( pxPCB->remote_ip ) ) );
#endif
        prvvMBPortReleaseSocket( aNetwork, aSocket );
    }
}

BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
    return TRUE;
}

BOOL
xMBTCPPortSendResponse( void *pvSession, const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL            bFrameSent = FALSE;
    xMBTCPClient    *pClientHandle = (xMBTCPClient *)pvSession;

    if( pClientHandle )
    {
        if ( UEZNetworkSocketWrite(pClientHandle->aNetwork, pClientHandle->aSocket, 
                                   (void *)pucMBTCPFrame, usTCPLength, TRUE, UEZ_TIMEOUT_INFINITE) == UEZ_ERROR_NONE )
        {
#ifdef MB_TCP_DEBUG
            vMBPortLog( MB_LOG_DEBUG, 
                        "MBTCP-WRITE", "Socket: %d bytes: %d ticks: %d\r\n", 
                        pClientHandle->aSocket, usTCPLength, UEZTickCounterGet());
#endif
#ifdef MB_TCP_DEBUG
            prvvMBTCPLogFrame( (UCHAR *) "MBTCP-SENT", (UCHAR *) pucMBTCPFrame, usTCPLength );
#endif
            bFrameSent = TRUE;
        }
    }
    return bFrameSent;
}

void
xMBTCPPortSetConnectionTimeout( USHORT usTimeout )
{
    /* set the timeout for inactive TCP connection */
    usTCPConnectionTimeout = usTimeout;
}

void
xMBTCPPortRegisterConnectionAttemptCB( pxMBTCPConnectionAttemptCB pCB )
{  
    pxConnectionAttemptCB = pCB;
}

void            
xMBTCPPortRegisterConnectionCloseCB( pxMBTCPConnectionCloseCB pCB )
{
    pxConnectionCloseCB = pCB;
}

BOOL
xMBTCPPortGetConnectionInfo( T_uEZNetworkConnectionInfo *aConnection )
{
    USHORT index;
    T_uezTask task;
    
    /* find our current task handle */
    task = UEZTaskGetCurrent();

    for (index = 0; index < MB_TCP_MAX_CLIENTS; index++)
    {
        /* is this the client record for this task? */
        if (xClients[index].aTask == task)
        {
            memcpy(aConnection, &xClients[index].aConnection, sizeof(xClients[index].aConnection));

            return TRUE;
        }
    }

  /* we did not find a client to match this task */
  return FALSE;

}

/* helper function for retrieving the UID out of the request frame */
UCHAR
xMBTCPPortGetUnitAddress( void )
{
    USHORT index;
    T_uezTask task;
    
    /* find our current task handle */
    task = UEZTaskGetCurrent();

    for (index = 0; index < MB_TCP_MAX_CLIENTS; index++)
    {
        /* is this the client record for this task? */
        if (xClients[index].aTask == task)
        {
            return xClients[index].aucTCPBuf[MB_TCP_UID];
        }
    }

    /* we did not find a client to match this task */
    return MB_TCP_ERROR_ADDRESS;
}

/* Modbus TCP Server Task, opens a socket and waits for client connections */
static void
prvMBTCPServerTask( T_uezTask aMyTask, void *arg )
{
    T_uezNetworkSocket socket;
    T_uezNetworkSocket newSocket;
    T_uezDevice network;

    /* clear out the client info */
    memset(xClients, 0, sizeof(xClients));

    /* Get the network handle */
    UEZNetworkOpen("WiredNetwork0", &network);

    /* create the TCP socket */
    if (UEZNetworkSocketCreate(network, UEZ_NETWORK_SOCKET_TYPE_TCP, &socket) == UEZ_ERROR_NONE) 
    {
        /* Setup the socket to be on the Modbus TCP port */
        UEZNetworkSocketBind(network, socket, 0, usPort);

        /* Put the socket into listen mode */
        UEZNetworkSocketListen(network, socket);

#ifdef MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Protocol stack ready.\r\n" );
#endif

        /* Loop forever */
        for (;;) 
        {
            /* Wait for connection. */
            if (UEZNetworkSocketAccept(network, socket, &newSocket, UEZ_TIMEOUT_INFINITE) != UEZ_ERROR_NONE)
            {
                break;
            }

#ifdef MB_TCP_DEBUG
            vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Socket accepted.\r\n" );
#endif

            /* Service connection. */
            if (socket) 
            {
                prvMBTCPProcessConnection(network, newSocket);
            }
        }
    }
    while (1) 
    {
        // Sit here doing nothing
#ifdef MB_TCP_DEBUG
        vMBPortLog( MB_LOG_ERROR, "MBTCP-SERVER", "Task failed!\r\n" );
#endif
        UEZTaskDelay(5000);
    }
}

/* start the Modbus TCP Server Task */
BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
    static TBool haveRun = EFalse;

    if(!haveRun){
        haveRun = ETrue;

        /* set up the port number to use */
        if( usTCPPort == MB_TCP_PORT_USE_DEFAULT )
        {
            usPort = MB_TCP_DEFAULT_PORT;
        }
        else
        {
            usPort = ( USHORT ) usTCPPort;
        }

        /* Create a task the Modbus Server task */
        UEZTaskCreate((T_uezTaskFunction)prvMBTCPServerTask, "Modbus_TCPIP", MAIN_TASK_STACK_SIZE, 0,
                      UEZ_PRIORITY_NORMAL, &_ModbusTCPIPTask);
    }
    return TRUE;
}
