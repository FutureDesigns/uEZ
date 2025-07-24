/*-------------------------------------------------------------------------*
 * File:  emWinVnc.c
 *-------------------------------------------------------------------------*
 * Description: emWin VNC activation example
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include "NetworkStartup.h"
#include <NVSettings.h>
#include <Config_Build.h>
#include <sys/socket.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h> // rtt debug usage
#include <GUI.h>
#include <GUI_VNC.h>
   
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if (UEZ_ENABLE_TCPIP_STACK == 1)
T_uezTask G_emWin_Vnc_Server_Task = (T_uezTask) NULL; // server task handle
GUI_VNC_CONTEXT    G_emWin_Vnc_Context; // configuration context
unsigned char vncReceiveBuffer[GUI_VNC_BUFFER_SIZE] = {0}; // defined in emWin header, unless overridden

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/** @brief  Retrieves the IP addr. of the currently connected VNC client.
  * @param  Addr: IP address
  * @retval None  */
void GUI_VNC_X_getpeername(int ServerIndex, U32 * Addr) 
{
  // TODO retreive real client IP addresses and show in uEZGUI.
  *Addr = INADDR_ANY; //_Addr.sin_addr.s_addr;
}

/** @brief  Called by the server to send data
  * @param  buf: buffer to be sent.
  * @param  len: length of buf.
  * @param  pConnectionInfo: Connection info
  * @retval transmit status.  */
static int _Send(const U8 * buf, int len, void * pConnectionInfo) 
{  
    T_uezError error = UEZNetworkSocketWrite(
      NetworkGetPrimaryDevice(),
      (T_uezNetworkSocket) ((uint32_t) pConnectionInfo),
      (void *) buf, len,
      EFalse,//ETrue,
      2000);

    if (error == UEZ_ERROR_NONE) {
      return len; 
    } else {
      return 0;
    }
}

/** @brief  Called by the server when data is received
  * @param  buf: buffer to get the received data.
  * @param  len: length of received data.
  * @param  pConnectionInfo: Connection info
  * @retval receive status. */
static int _Recv(U8 * buf, int len, void * pConnectionInfo) 
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 receiveLength = len;
    TUInt32 receivedLength = 0;
    PARAM_NOT_USED(error);
    if(receiveLength == 0) {
        return 0; // something went wrong in RFB buffer initialization
    }
    if(buf == 0) {
        return 0; // something went wrong in RFB buffer initialization        
    } else {
            error = UEZNetworkSocketRead(NetworkGetPrimaryDevice(),
            (T_uezNetworkSocket) ((uint32_t) pConnectionInfo), (void *) buf,
            (TUInt32)receiveLength, &receivedLength, 3000);    
    }
    return receivedLength;
}

TUInt32 VNCServerTask(T_uezTask aMyTask, void *aParameters)
{
    T_uezNetworkSocket socket;
    T_uezNetworkSocket newSocket;
    T_uezDevice aNetwork = (T_uezDevice)aParameters; // TODO we need to re-init if switching interfaces using NetworkGetPrimaryDevice()
    PARAM_NOT_USED(aMyTask);
    U16 Port = 5900 + G_emWin_Vnc_Context.ServerIndex; // VNC starting port

    lwip_socket_thread_init(); // initialize per thread semaphore if used, creates new semaphore for this thread
    /* Create a new tcp connection handle */
    if (UEZNetworkSocketCreate(aNetwork, UEZ_NETWORK_SOCKET_TYPE_TCP,
            &socket) == UEZ_ERROR_NONE) {
        
        UEZNetworkSocketBind(aNetwork, socket, 0, Port); // Setup the socket to be on the VNC port
       
        UEZNetworkSocketListen(aNetwork, socket);  // Put the socket into listen mode
       
        for (;;) {            
            UEZNetworkSocketAccept(aNetwork, socket, &newSocket, 1000); // Wait for a connection.
            
            if (newSocket) { // Service the new connection.
              //Network_lwIP_GetConnectionInfo() // TODO can retrieve client IP address for emWin, but it isn't required                
                GUI_VNC_Process(&G_emWin_Vnc_Context, _Send, _Recv, (void *) ((uint32_t)newSocket)); // Run the emWin VNC server
                UEZNetworkSocketDelete(aNetwork, newSocket);
            }
        }
    }
    lwip_socket_thread_cleanup(); // clean up semaphores
    while (1) { // Sit here doing nothing
        UEZTaskDelay(1000);
    }
#ifdef __GNUC__
    return 0;
#endif
}

// Configure emWin VNC server
void setup_emWin_VNC_Context(int emWinLayerIndex)
{
  G_emWin_Vnc_Context.ServerIndex = 0; // this is the starting port, 0 for 5900
  G_emWin_Vnc_Context.LayerIndex = emWinLayerIndex; // emWin layer index to use (normally 0)
  G_emWin_Vnc_Context.pNext = NULL;
  GUI_VNC_AttachToLayer(&G_emWin_Vnc_Context, 0); // resets some context settings
  // Make sure to set this AFTER the attach to layer call.
  G_emWin_Vnc_Context.pBuffer = &vncReceiveBuffer[0];
  G_emWin_Vnc_Context.SizeOfBuffer = GUI_VNC_BUFFER_SIZE;
  G_emWin_Vnc_Context.BytesPerPixel = 2; // Is this needed?
}

// Start emWin VNC server task
void setup_emWin_VNC_NetworkTask(void)
{
  UEZTaskCreate((T_uezTaskFunction)VNCServerTask, "emWinVNC",
            UEZ_TASK_STACK_BYTES(5000), (void *)NetworkGetPrimaryDevice(), UEZ_PRIORITY_NORMAL, &G_emWin_Vnc_Server_Task);
  // Now, you connect to the server with emVNC.exe and enter the IP/port of your target, such as 192.168.10.39:5900
}
#else // networking disabled
void setup_emWin_VNC_Context(int emWinLayerIndex)
{
  PARAM_NOT_USED(emWinLayerIndex);
}
void setup_emWin_VNC_NetworkTask(void)
{
}
#endif
/*-------------------------------------------------------------------------*
 * End of File:  emWinVnc.c
 *-------------------------------------------------------------------------*/
