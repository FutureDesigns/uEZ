
///////////////////////////////////////////////////////////////////////////////
//
// Module Name:                                                                
//   HTTPClientDebug.c                                                        
//                                                                             
// Abstract: Debug functions  
//                                                                             
// Platform: Any that supports standard C calls                                
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "HTTPClientDebug.h"

#ifdef _HTTP_DEBUGGING_

#ifdef _UEZ_RTOS
static T_uezSemaphore      HTTPClientDebugSem;
static BOOL bHTTPClientDebugSemInit = FALSE;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientDebug
// Purpose      : HTTP API Debugging callback
// Gets         : arguments
// Returns      : void
//
///////////////////////////////////////////////////////////////////////////////
VOID HTTPClientDebug(const CHAR* FunctionName,const CHAR *DebugDump,UINT32 iLength,CHAR *DebugDescription,...) // Requested operation
{

    va_list            pArgp;
    static char        szBuffer[2048];

#ifdef _UEZ_RTOS
    if (bHTTPClientDebugSemInit == FALSE)
    {
        UEZSemaphoreCreateBinary(&HTTPClientDebugSem);
        bHTTPClientDebugSemInit = TRUE;
    }

    UEZSemaphoreGrab(HTTPClientDebugSem, UEZ_TIMEOUT_INFINITE);
#endif

    memset(szBuffer,0,2048);
    va_start(pArgp, DebugDescription);
    vsprintf((char*)szBuffer, DebugDescription, pArgp); //Copy Data To The Buffer
    va_end(pArgp);

    printf("%s %s %s\n", FunctionName,DebugDump,szBuffer);

#ifdef _UEZ_RTOS
    UEZSemaphoreRelease(HTTPClientDebugSem);
#endif
}

const CHAR *HTTPClientDebugReturnCode(INT32 nRetCode)
{
    switch (nRetCode) 
    {
        case HTTP_CLIENT_SUCCESS:
            return "Success";

        case HTTP_CLIENT_UNKNOWN_ERROR:
            return "Unknown Error";

        case HTTP_CLIENT_ERROR_INVALID_HANDLE:
            return "Invalid Handle";

        case HTTP_CLIENT_ERROR_NO_MEMORY:
            return "No Memory";

        case HTTP_CLIENT_ERROR_SOCKET_INVALID:
            return "Invalid Socket";

        case HTTP_CLIENT_ERROR_SOCKET_CANT_SET:
            return "Can't Set Socket Params";

        case HTTP_CLIENT_ERROR_SOCKET_RESOLVE:
            return "Socket Resolve Error";

        case HTTP_CLIENT_ERROR_SOCKET_CONNECT:
            return "Socket Connect Error";

        case HTTP_CLIENT_ERROR_SOCKET_TIME_OUT:
            return "Socket Timeout";

        case HTTP_CLIENT_ERROR_SOCKET_RECV:
            return "Socket Receive Error";

        case HTTP_CLIENT_ERROR_SOCKET_SEND:
            return "Socket Send Error";

        case HTTP_CLIENT_ERROR_HEADER_RECV:
            return "Header Receive Error";

        case HTTP_CLIENT_ERROR_HEADER_NOT_FOUND:
            return "Header Not Found";

        case HTTP_CLIENT_ERROR_HEADER_BIG_CLUE:
            return "Header Big Clue Error";

        case HTTP_CLIENT_ERROR_HEADER_NO_LENGTH:
            return "Header No Length Error";

        case HTTP_CLIENT_ERROR_CHUNK_TOO_BIG:
            return "Chunk Too Big";

        case HTTP_CLIENT_ERROR_AUTH_HOST:
             return "Could not authenticate with the remote host";

        case HTTP_CLIENT_ERROR_AUTH_PROXY:
             return "Could not authenticate with the remote proxy";

        case HTTP_CLIENT_ERROR_BAD_VERB:
            return "Bad Verb";

        case HTTP_CLIENT_ERROR_LONG_INPUT:
            return "Long Input";

        case HTTP_CLIENT_ERROR_BAD_STATE:
            return "Bad State";

        case HTTP_CLIENT_ERROR_CHUNK:
            return "Chunk Error";

        case HTTP_CLIENT_ERROR_BAD_URL:
            return "Bad URL";

        case HTTP_CLIENT_ERROR_BAD_HEADER:
            return "Bad Header";

        case HTTP_CLIENT_ERROR_BUFFER_RSIZE:
            return "Buffer Resize Error";

        case HTTP_CLIENT_ERROR_BAD_AUTH:
            return "Unsupported Auth Scheme";

        case HTTP_CLIENT_ERROR_AUTH_MISMATCH:
            return "Auth Mismatch";

        case HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN:
            return "No Digest Token";

        case HTTP_CLIENT_ERROR_NO_DIGEST_ALG:
            return "Digest Algorithm Not Supported";

        case HTTP_CLIENT_ERROR_SOCKET_BIND:
            return "SOcket Bind Error";
        
        case HTTP_CLIENT_ERROR_TLS_NEGO:
            return "TLS Negotiation Error";
        
        case HTTP_CLIENT_ERROR_NOT_IMPLEMENTED:
            return "Not Implemented";

        case HTTP_CLIENT_EOS:
            return "EOS";
    }

    return "No Match";
}
#endif // _HTTP_DEBUGGING_