
#ifndef _HTTP_CLIENT_REQUEST
#define _HTTP_CLIENT_REQUEST

#include "HTTPClient.h" 

// uncomment below to enable test functions
// #define _HTTP_CLIENT_REQUEST_TEST

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP Client Request global definitions
//
///////////////////////////////////////////////////////////////////////////////

typedef struct _HTTPParameters
{
    CHAR                    Uri[1024];        
    CHAR                    ProxyHost[1024];  
    UINT32                  UseProxy ;  
    UINT32                  ProxyPort;
    UINT32                  Verbose;
    CHAR                    UserName[64];
    CHAR                    Password[64];
    HTTP_AUTH_SCHEMA        AuthType;
} HTTPParameters;

#ifdef _HTTP_CLIENT_REQUEST_TEST
// test function
int HTTPClientRequestDoTest(void);
#endif // _HTTP_CLIENT_REQUEST_TEST

int HTTPClientRequestDoGet(HTTPParameters *pClientParams, CHAR *pBuffer, UINT32 *pSize);
 
#endif  // _HTTP_CLIENT_REQUEST
