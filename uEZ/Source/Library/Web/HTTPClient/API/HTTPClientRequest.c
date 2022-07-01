#include <stdio.h>
#include "HTTPClientDebug.h"
#include "HTTPClientRequest.h"

///////////////////////////////////////////////////////////////////////////////
//
// Function     : 
// Purpose      : 
// Gets         : 
// Returns      : 
//
///////////////////////////////////////////////////////////////////////////////
int32_t HTTPClientRequestDoGet(HTTPParameters *pClientParams, CHAR *pBuffer, UINT32 *pSize)
{

    INT32                   nRetCode;
    UINT32                  nSize, nTotal = 0;
    HTTP_SESSION_HANDLE     pHTTP;

    static INT32   count = 0;

    do
    {
#ifdef _HTTP_DEBUGGING_
        if(pClientParams->Verbose == TRUE)
        {
            HTTPClientDebug("HTTPClientRequestDo", NULL, 0, "HTTP Client v1.1");
        }
#endif // _HTTP_DEBUGGING_

        // Open the HTTP request handle
        pHTTP = HTTPClientOpenRequest(0);

#ifdef _HTTP_DEBUGGING_
        HTTPClientSetDebugHook(pHTTP, &HTTPClientDebug);
#endif

        // Set the Verb
        if((nRetCode = HTTPClientSetVerb(pHTTP, VerbGet)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }

        // Set authentication
        if(pClientParams->AuthType != AuthSchemaNone)
        {
            if((nRetCode = HTTPClientSetAuth(pHTTP, pClientParams->AuthType, NULL)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }

            // Set authentication
            if((nRetCode = HTTPClientSetCredentials(pHTTP, pClientParams->UserName, pClientParams->Password)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }
        }

        // Use Proxy server
        if(pClientParams->UseProxy == TRUE)
        {
            if((nRetCode = HTTPClientSetProxy(pHTTP, pClientParams->ProxyHost, pClientParams->ProxyPort, NULL, NULL)) != HTTP_CLIENT_SUCCESS)
            {

                break;
            }
        }

        // Send a request for the home page 
        if((nRetCode = HTTPClientSendRequest(pHTTP, pClientParams->Uri, NULL, 0, FALSE, 0, 0)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }

        // Retrieve the the headers and analyze them
        if((nRetCode = HTTPClientRecvResponse(pHTTP, 0)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }

        // Get the data until we get an error or end of stream code
        while(nRetCode == HTTP_CLIENT_SUCCESS)// || nRetCode != HTTP_CLIENT_EOS)
        {
            // Set the size of our buffer
            nSize = *pSize - nTotal;  

            // Get the data
            nRetCode = HTTPClientReadData(pHTTP, &pBuffer[nTotal], nSize, 0, &nSize);
            nTotal += nSize;
        }
        // Print out the results
        // printf("%s\n", pBuffer);

    } while(0); // Run only once

    *pSize = nTotal;

    // put this here so the sockets are always closed
    HTTPClientCloseRequest(&pHTTP);

#ifdef _HTTP_DEBUGGING_
    if(pClientParams->Verbose == TRUE)
    {
        HTTPClientDebug("HTTPClientRequestDo",NULL,0,
               "HTTP Client terminated %s (got %d kb) %d\n" ,HTTPClientDebugReturnCode(nRetCode),
               (int32_t)(nTotal/ 1024), count++);
    }
#endif // _HTTP_DEBUGGING_

    return nRetCode;
}

#ifdef _HTTP_CLIENT_REQUEST_TEST

#define HTTP_CLIENT_BUFFER_SIZE     1024*50  // 50k!!!

// single threaded!  buffer can be used for request and response
static CHAR             Buffer[HTTP_CLIENT_BUFFER_SIZE];

///////////////////////////////////////////////////////////////////////////////
//
// Function     : 
// Purpose      : 
// Gets         : 
// Returns      : 
//
///////////////////////////////////////////////////////////////////////////////
void HTTPClientRequestTestParams(HTTPParameters *pClientParams)
{
    // Reset the parameters structure
    memset(pClientParams,0,sizeof(HTTPParameters));

    // Set up HTTP Client params (pass these in, or provide a callback?)
    pClientParams->Verbose = TRUE;

    // point this at your test resource
    strcpy(pClientParams->Uri,"https://192.168.1.191:1125/HIBridge/HIBridgeService.svc?singleWsdl");

    // Did we got the Proxy parameter
    // pClientParams->UseProxy = TRUE; // So we would know later that we have to use proxy
    // strcpy(pClientParams->ProxyHost,argv[nArg] + 3);
    // pClientParams->ProxyPort = 8080;

    // Do we have the credentials?
    // strcpy(pClientParams->UserName,argv[nArg] + 3);
    // strcpy(pClientParams->Password,pClientParams->UserName + nResult +1);

    // Do we have the authentication method?
    // pClientParams->AuthType = AuthSchemaBasic; 
    // pClientParams->AuthType = AuthSchemaDigest;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : 
// Purpose      : 
// Gets         : 
// Returns      : 
//
///////////////////////////////////////////////////////////////////////////////
int32_t HTTPClientRequestDoTest(void)
{
    HTTPParameters ClientParams;
    UINT32 nSize = sizeof(Buffer);

    HTTPClientRequestTestParams(&ClientParams);

    return HTTPClientRequestDoGet(&ClientParams, Buffer, &nSize);
}

#endif // _HTTP_CLIENT_REQUEST_TEST