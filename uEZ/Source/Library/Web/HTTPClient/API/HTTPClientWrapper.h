
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
    int32_t                                 HTTPWrapperIsAscii              (int32_t c);
    int32_t                                 HTTPWrapperToUpper              (int32_t c);
    int32_t                                 HTTPWrapperToLower              (int32_t c);
    int32_t                                 HTTPWrapperIsAlpha              (int32_t c);
    int32_t                                 HTTPWrapperIsAlNum              (int32_t c);
    char*                               HTTPWrapperItoa                 (char *buff,int32_t i);
    void                                HTTPWrapperInitRandomeNumber    ();
    long                                HTTPWrapperGetUpTime            ();
    int32_t                                 HTTPWrapperGetRandomeNumber     ();
    int32_t                                 HTTPWrapperGetSocketError       (int32_t s);
#ifdef _UEZ_RTOS
    int32_t                                 HTTPWrapperGetHostByName        (int32_t s,char *name,T_uezNetworkAddr *address);
#else
    unsigned long                       HTTPWrapperGetHostByName        (char *name,unsigned long *address);
#endif
    int32_t                                 HTTPWrapperShutDown             (int32_t s,int32_t in);  
    // SSL Wrapper prototypes
#ifndef _UEZ_RTOS
    int32_t                                 HTTPWrapperSSLConnect           (int32_t s,const struct sockaddr *name,int32_t namelen,char *hostname);
    int32_t                                 HTTPWrapperSSLNegotiate         (int32_t s,const struct sockaddr *name,int32_t namelen,char *hostname);
#else
    int32_t                                 HTTPWrapperSSLNegotiate         (int32_t s,char *hostname);
#endif
    int32_t                                 HTTPWrapperSSLSend              (int32_t s,char *buf, int32_t len,int32_t flags);
    int32_t                                 HTTPWrapperSSLRecv              (int32_t s,char *buf, int32_t len,int32_t flags);
    int32_t                                 HTTPWrapperSSLClose             (int32_t s);
    int32_t                                 HTTPWrapperSSLRecvPending       (int32_t s);
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
typedef int32_t                          BOOL;
//typedef unsigned long                ULONG;

// Global socket structures and definitions
#define                              HTTP_INVALID_SOCKET (0)    // TODO was -1
typedef struct sockaddr_in           HTTP_SOCKADDR_IN;
typedef struct timeval               HTTP_TIMEVAL; 
typedef struct hostent               HTTP_HOSTNET;
typedef struct sockaddr              HTTP_SOCKADDR;
typedef struct in_addr               HTTP_INADDR;


#endif // HTTP_CLIENT_WRAPPER
