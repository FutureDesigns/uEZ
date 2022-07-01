
#ifndef HTTP_CLIENT_WRAPPER
#define HTTP_CLIENT_WRAPPER

#define _UEZ_RTOS
#define _UEZ_CYA_SSL

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Microsoft Windows Support
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#pragma warning (disable: 4996) // 'function': was declared deprecated (VS 2005)
#include        <stdlib.h>
#include        <string.h>
#include        <memory.h>
#include        <stdio.h>
#include        <ctype.h>
#include        <time.h>
#include        <winsock.h>

// Sockets (Winsock wrapper)
#define                HTTP_ECONNRESET     (WSAECONNRESET)
#define                HTTP_EINPROGRESS    (WSAEINPROGRESS)
#define                HTTP_EWOULDBLOCK    (WSAEWOULDBLOCK)

// Kluge alert: redefining strncasecmp() as memicmp() for Windows.
//
#define                strncasecmp                        memicmp
#define                strcasecmp                        stricmp

#elif defined (_UEZ_RTOS)

#include <uEZ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h> 
#include <time.h>
#include <uEZNetwork.h>
#include <uEZTickCounter.h>

#define                SOCKET_ERROR                        -1

// Sockets (Winsock wrapper)
#define                HTTP_EINPROGRESS    (EINPROGRESS)
#define                HTTP_EWOULDBLOCK    (EWOULDBLOCK)

// Generic types
typedef TUInt32                UINT32;
typedef TInt32                 INT32; 

#define isascii(c)  (HTTPWrapperIsAscii(c))

// CyaSSL enabled for UEZ
#ifdef _UEZ_CYA_SSL
#include "Source/Library/Encryption/CyaSSL/cyassl/internal.h"
#include "Source/Library/Encryption/CyaSSL/cyassl/ctaocrypt/types.h"
#endif

#else // Non Win32 : GCC Linux

#include        <unistd.h>
#include        <errno.h>
#include        <pthread.h>
#include        <stdlib.h>
#include        <stdio.h>
#include        <string.h>
#include        <ctype.h>
#include        <time.h>
#include        <sys/socket.h>
#include        <sys/un.h>
#include        <netinet/in.h>
#include        <netinet/tcp.h>
#include        <netdb.h>
#include        <arpa/inet.h>
#include        <sys/ioctl.h>
#include        <errno.h>
#include        <stdarg.h>

#define                SOCKET_ERROR                        -1

// Sockets (Winsock wrapper)
#define                HTTP_EINPROGRESS    (EINPROGRESS)
#define                HTTP_EWOULDBLOCK    (EWOULDBLOCK)

// Generic types
typedef unsigned long                UINT32;
typedef long                         INT32; 

#endif        // #ifdef _HTTP_BUILD_WIN32

// Note: define this to prevent timeouts while debugging.
// #define                                                         NO_TIMEOUTS

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Functions that are not supported by the AMT stdc framework
//                So they had to be specificaly added.
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus 
extern "C" { 
#endif

    // STDC Wrapper implimentation
    int                                 HTTPWrapperIsAscii              (int c);
    int                                 HTTPWrapperToUpper              (int c);
    int                                 HTTPWrapperToLower              (int c);
    int                                 HTTPWrapperIsAlpha              (int c);
    int                                 HTTPWrapperIsAlNum              (int c);
    char*                               HTTPWrapperItoa                 (char *buff,int i);
    void                                HTTPWrapperInitRandomeNumber    ();
    long                                HTTPWrapperGetUpTime            ();
    int                                 HTTPWrapperGetRandomeNumber     ();
    int                                 HTTPWrapperGetSocketError       (int s);
#ifdef _UEZ_RTOS
    int                                 HTTPWrapperGetHostByName        (int s,char *name,T_uezNetworkAddr *address);
#else
    unsigned long                       HTTPWrapperGetHostByName        (char *name,unsigned long *address);
#endif
    int                                 HTTPWrapperShutDown             (int s,int in);  
    // SSL Wrapper prototypes
#ifndef _UEZ_RTOS
    int                                 HTTPWrapperSSLConnect           (int s,const struct sockaddr *name,int namelen,char *hostname);
    int                                 HTTPWrapperSSLNegotiate         (int s,const struct sockaddr *name,int namelen,char *hostname);
#else
    int                                 HTTPWrapperSSLNegotiate         (int s,char *hostname);
#endif
    int                                 HTTPWrapperSSLSend              (int s,char *buf, int len,int flags);
    int                                 HTTPWrapperSSLRecv              (int s,char *buf, int len,int flags);
    int                                 HTTPWrapperSSLClose             (int s);
    int                                 HTTPWrapperSSLRecvPending       (int s);
    // Global wrapper Functions
#define                             IToA                            HTTPWrapperItoa
#define                             GetUpTime                       HTTPWrapperGetUpTime
#define                             SocketGetErr                    HTTPWrapperGetSocketError 
#define                             HostByName                      HTTPWrapperGetHostByName
#define                             InitRandomeNumber               HTTPWrapperInitRandomeNumber
#define                             GetRandomeNumber                HTTPWrapperGetRandomeNumber

#ifdef __cplusplus 
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Global type definitions
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

#define VOID                         void
#ifndef NULL
#define NULL                         0
#endif
#ifndef TRUE
#define TRUE                         1
#endif
#ifndef FALSE
#define FALSE                        0
#endif
typedef char                         CHAR;
typedef unsigned short               UINT16;
typedef int                          BOOL;
//typedef unsigned long                ULONG;

// Global socket structures and definitions
#define                              HTTP_INVALID_SOCKET (0)    // TODO was -1
typedef struct sockaddr_in           HTTP_SOCKADDR_IN;
typedef struct timeval               HTTP_TIMEVAL; 
typedef struct hostent               HTTP_HOSTNET;
typedef struct sockaddr              HTTP_SOCKADDR;
typedef struct in_addr               HTTP_INADDR;


#endif // HTTP_CLIENT_WRAPPER
