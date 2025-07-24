/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.36 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.
----------------------------------------------------------------------
File        : GUI_SPY_X_StartServer.c
Purpose     : Start of emWinSPY server via RTT and/or TCP/IP.
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

//#include "RTOS.h"
#include <uEZ.h>
#include <uEZMemory.h>
#include "GUI.h"
//#include "TaskPrio.h"

#if (SEGGER_ENABLE_EMWINSPY_RTT ==1 )  // enable emWinSpy server. (the start function must be called from WindowHandler.c or similar)

#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
  #define USE_RTT (1)
  #define USE_TCP (0)
#else
  #define USE_RTT (0)
  #define USE_TCP (1) // not supported yet
#endif
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Select interface to be used
//
#ifndef   USE_RTT
  #define USE_RTT (1)
#endif
#ifndef   USE_TCP
  #define USE_TCP (0)
#endif

#if (USE_TCP == 1)
  #include "IP.h"         // BSD socket interface
#endif

#if (USE_RTT == 1)
  #include <Source/Library/SEGGER/RTT/SEGGER_RTT.h> // RTT interface
#endif

//
// Check selection
//
#if (USE_TCP == 0) && (USE_RTT == 0)
  #error Please select TCP or RTT
#endif

//
// Port definition for TCP connection
//
#if (USE_TCP == 1)
  #define PORT 2468 // emWinSPY-Port
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if (USE_RTT == 1)

//
// embOS Stack area of the server
//
//static OS_STACKPTR int _StackSPYServer_RTT[0x200];

//
// embOS Task-control-block of the server
//
//static OS_TASK _SPYServer_TCB_RTT;
static T_uezTask _G_SPYServer_Handle;

//
// Up- and down buffer for RTT
//

//static char _acBufferUp  [0x200];
static char _acBufferUp  [0x4000]; // We seem to need a large up buffer to avoid crashing at the end of the normal emWin demo during the automotive gauge part.
static char _acBufferDown[0x20];

static int _IndexUp;
static int _IndexDown;

#endif

#if (USE_TCP == 1)

//
// embOS Stack area of the server
//
static OS_STACKPTR int _StackSPYServer_TCP[0x200];

//
// embOS Task-control-block of the server
//
static OS_TASK _SPYServer_TCB_TCP;

#endif

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Send_RTT
*
*  Function description:
*    Sending data via RTT
*
*  Return value:
*    On success number of successfully transmitted bytes, otherwise -1.
*/
#if (USE_RTT == 1)
static int _Send_RTT(const U8 * buf, int len, void * pConnectInfo) {
  int r, RemBytes, Cnt;

  GUI_USE_PARA(pConnectInfo);
  Cnt = 10000;
  RemBytes = len;
  do {
    r = SEGGER_RTT_Write(_IndexUp, buf, RemBytes);
    buf += r;
    RemBytes -= r;
    if (RemBytes) {
      GUI_X_Delay(1);
    }
  } while ((RemBytes > 0) && (--Cnt));
  return Cnt ? len : -1;
}

/*********************************************************************
*
*       _Recv_RTT
*
*  Function description:
*    Receiving data via RTT
*
*  Return value:
*    On success number of successfully received bytes, otherwise -1.
*/
static int _Recv_RTT(U8 * buf, int len, void * pConnectInfo) {
  int r, RemBytes, Cnt;

  GUI_USE_PARA(pConnectInfo);
  Cnt = 10000;
  RemBytes = len;
  do {
    while (SEGGER_RTT_HasData(_IndexDown) == 0) {
      GUI_X_Delay(1);
    };
    r = SEGGER_RTT_Read(_IndexDown, buf, RemBytes);
    buf += r;
    RemBytes -= r;
  } while ((RemBytes > 0) && (--Cnt));
  return Cnt ? len : -1;
}

/*********************************************************************
*
*       _ServerTask_RTT
*
*  Function description:
*    This task  waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should
*    return if the connection is closed.
*/
//static void _ServerTask_RTT(void) {
TUInt32 _ServerTask_RTT(T_uezTask aMyTask, void *aParams) {
  while (1) {
    //
    // Execute emWinSPY, returns after connection is closed
    //
    GUI_SPY_Process(_Send_RTT, _Recv_RTT, NULL);
    //
    // Wait a while...
    //
    GUI_X_Delay(10);
  }
}
#endif

/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
#if (USE_TCP == 1)
static int _ListenAtTcpAddr(unsigned short Port) {
  int sock;
  struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(Port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(sock, 1);
  return sock;
}

/*********************************************************************
*
*       _Send_TCP
*
*  Function description:
*    Sending data via TCP
*
*  Return value:
*    On success number of successfully transmitted bytes, otherwise -1.
*/
static int _Send_TCP(const U8 * buf, int len, void * pConnectInfo) {
  int r;

  r = send((long)pConnectInfo, (const char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _Recv_TCP
*
*  Function description:
*    Receiving data via TCP
*
*  Return value:
*    On success number of successfully received bytes, otherwise -1.
*/
static int _Recv_TCP(U8 * buf, int len, void * pConnectInfo) {
    int r;

  r = recv((long)pConnectInfo, (char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _ServerTask_TCP
*
*  Function description:
*    This task  waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should
*    return if the connection is closed.
*/
static void _ServerTask_TCP(void) {
  static struct sockaddr_in Addr;
  int s, Sock, AddrLen;

  //
  // Loop until we get a socket into listening state
  //
  do {
    s = _ListenAtTcpAddr(PORT);
    if (s != -1) {
      break;
    }
    OS_Delay(100); // Try again
  } while (1);
  //
  // Loop once per client and create a thread for the actual server
  //
  while (1) {
    //
    // Wait for an incoming connection
    //
    AddrLen = sizeof(Addr);
    if ((Sock = accept(s, (struct sockaddr*)&Addr, &AddrLen)) == SOCKET_ERROR) {
      continue; // Error
    }
    //
    // Execute emWinSPY
    //
    GUI_SPY_Process(_Send_TCP, _Recv_TCP, (void *)Sock);
    //
    // Close the connection
    //
    closesocket(Sock);
    memset(&Addr, 0, sizeof(struct sockaddr_in));
  }
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SPY_X_StartServer
*/
int GUI_SPY_X_StartServer(void) {

  // Set up a memory handler so that memory used to run emWinSPY doesn't increase the normal emWin memory usage.
  // The goal is to get an accurate number of how much memory is needed (not how much plus additional debugging only memory)
  GUI_SPY_SetMemHandler (UEZMemAlloc, UEZMemFree);

#if (USE_RTT == 1)
  //
  // Allocate buffers for RTT
  //
  _IndexUp   = SEGGER_RTT_AllocUpBuffer  ("emWinSPY", _acBufferUp,   sizeof(_acBufferUp),   SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  if (_IndexUp < 0) {
    return 1;
  }
  _IndexDown = SEGGER_RTT_AllocDownBuffer("emWinSPY", _acBufferDown, sizeof(_acBufferDown), SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  if (_IndexDown < 0) {
    return 1;
  }
  //
  // Create task for RTT Server
  //
//  OS_CREATETASK(&_SPYServer_TCB_RTT, "SPY-Server(RTT)", _ServerTask_RTT, 100, _StackSPYServer_RTT);
  UEZTaskCreate(_ServerTask_RTT, "SPY-RTT", 2048, (void *)0, UEZ_PRIORITY_HIGH, &_G_SPYServer_Handle);
#endif
#if (USE_TCP == 1)
  //
  // Create task for TCP/IP server
  //
  OS_CREATETASK(&_SPYServer_TCB_TCP, "SPY-Server(TCP)", _ServerTask_TCP, 230, _StackSPYServer_TCP);
#endif
  //
  // O.k., server(s) started
  //
  return 0;
}

// FDI provided additional functions.

int GUI_SPY_X_StopServer(void) {
  UEZTaskSuspend(_G_SPYServer_Handle);
  return 0;
}

#else // if emWinSPY disabled then use empty functions.

int GUI_SPY_X_StartServer(void) {
  return 0;
}

int GUI_SPY_X_StopServer(void) {
  return 0;
}

#endif

/*************************** End of file ****************************/
