 
#include "HTTPClientWrapper.h"
#include "HttpClient.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: HTTPWrapperIsAscii
// Last updated : 15/05/2005
// Notes            : Same as stdc: isascii
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperIsAscii(int c)
{
    return (!(c & ~0177));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: HTTPWrapperToUpper
// Last updated : 15/05/2005
// Notes            : Convert character to uppercase.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperToUpper(int c)
{
    // -32
    if(HTTPWrapperIsAscii(c) > 0)
    {
        if(c >= 97 && c <= 122)
        {
            return (c - 32);
        }
    }

    return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: HTTPWrapperToLower
// Last updated : 13/06/2006
// Notes               : Convert character to lowercase.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperToLower(int c)
{
    // +32
    if(HTTPWrapperIsAscii(c) > 0)
    {
        if(c >= 65 && c <= 90)
        {
            return (c + 32);
        }
    }

    return c;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: isalpha
// Last updated : 15/05/2005
// Notes            : returns nonzero if c is a particular representation of an alphabetic character
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperIsAlpha(int c)
{

    if(HTTPWrapperIsAscii(c) > 0)
    {
        if( (c >= 97 && c <= 122) || (c >= 65 && c <= 90)) 
        {
            return c;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: isalnum
// Last updated : 15/05/2005
// Notes            : returns nonzero if c is a particular representation of an alphanumeric character
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperIsAlNum(int c)
{
    if(HTTPWrapperIsAscii(c) > 0)
    {

        if(HTTPWrapperIsAlpha(c) > 0)
        {
            return c;
        }

        if( c >= 48 && c <= 57)  
        {
            return c;
        } 

    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_itoa
// Last updated : 15/05/2005
// Notes            : same as stdc itoa() // hmm.. allmost the same
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* HTTPWrapperItoa(char *s,int a)
{
    unsigned int b;

    if(a > 2147483647)
    {
        return 0; // overflow
    }

    if (a < 0) b = -a, *s++ = '-';
    else b = a;
    for(;a;a=a/10) s++;
    for(*s='\0';b;b=b/10) *--s=b%10+'0';
    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_ShutDown
// Last updated : 15/05/2005
// Notes            : Handles parameter changes in the socket shutdown()
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperShutDown (int s,int how) 
{
#ifdef _UEZ_RTOS
    return 0;
#else
    return shutdown(s,how);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetSocketError
// Last updated : 15/05/2005
// Notes            : WSAGetLastError Wrapper (Win32 Specific)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperGetSocketError (int s)
{
#ifdef _WIN32
    return WSAGetLastError();
#elif defined (_UEZ_RTOS)
    return 0;
#else
    return errno;
#endif
}

#ifdef _UEZ_RTOS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetHostByName
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperGetHostByName(int s, char *name, T_uezNetworkAddr *address)
{
    int     iPos = 0, iLen = 0,iNumPos = 0,iDots =0;
    long    iIPElement;
    char    c = 0;
    char    Num[4];
    int     iHostType = 0; // 0 : numeric IP
    P_HTTP_SESSION pHTTPSession = (P_HTTP_SESSION)s;

    // Check if the name is an IP or host 
    iLen = strlen(name);
    for(iPos = 0; iPos <= iLen;iPos++)
    {
        c = name[iPos];
        if((c >= 48 && c <= 57)  || (c == '.') )
        {   
            // c is numeric or dot
            if(c != '.')
            {
                // c is numeric
                if(iNumPos > 3)
                {
                    iHostType++;
                    break;
                }
                Num[iNumPos] = c;
                Num[iNumPos + 1] = 0;
                iNumPos ++;
            }
            else
            {
                iNumPos = 0;
                iDots++;
                iIPElement = atol(Num);
                if(iIPElement > 256 || iDots > 3)
                {
                    return -1; // error invalid IP
                }
            }
        }
        else
        {
            break; // this is an alpha numeric address type
        }
    }

    if(c == 0 && iHostType == 0 && iDots == 3)
    {
        iIPElement = atol(Num);
        if(iIPElement > 256)
        {
            return -1; // error invalid IP
        }
    }
    else
    {
        iHostType++;
    }   

    if(iHostType > 0)
    {
        if (UEZNetworkResolveAddress(pHTTPSession->HttpConnection.HttpNetwork, name, address) == UEZ_ERROR_NONE)
        {
            return HTTP_CLIENT_SUCCESS; // OK
        }
        else
        {
            return -1; // Error
        }
    }

    else // numeric address - no need for DNS resolve
    {
        if (UEZNetworkIPV4StringToAddr(name, address) == UEZ_ERROR_NONE)
        {
            return HTTP_CLIENT_SUCCESS; // OK
        }
        else
        {
            return -1; // Error
        }
    }

    return -1;   // Error
}
#else

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetHostByName
// Last updated : 15/05/2005
// Notes            : gethostbyname for Win32
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long HTTPWrapperGetHostByName(char *name,unsigned long *address)
{
#ifndef _UEZ_RTOS
    HTTP_HOSTNET     *HostEntry;
    int     iPos = 0, iLen = 0,iNumPos = 0,iDots =0;
    long    iIPElement;
    char    c = 0;
    char    Num[4];
    int     iHostType = 0; // 0 : numeric IP

    // Check if the name is an IP or host 
    iLen = strlen(name);
    for(iPos = 0; iPos <= iLen;iPos++)
    {
        c = name[iPos];
        if((c >= 48 && c <= 57)  || (c == '.') )
        {   
            // c is numeric or dot
            if(c != '.')
            {
                // c is numeric
                if(iNumPos > 3)
                {
                    iHostType++;
                    break;
                }
                Num[iNumPos] = c;
                Num[iNumPos + 1] = 0;
                iNumPos ++;
            }
            else
            {
                iNumPos = 0;
                iDots++;
                iIPElement = atol(Num);
                if(iIPElement > 256 || iDots > 3)
                {
                    return 0; // error invalid IP
                }
            }
        }
        else
        {
            break; // this is an alpha numeric address type
        }
    }

    if(c == 0 && iHostType == 0 && iDots == 3)
    {
        iIPElement = atol(Num);
        if(iIPElement > 256)
        {
            return 0; // error invalid IP
        }
    }
    else
    {
        iHostType++;
    }   

    if(iHostType > 0)
    {

        HostEntry = gethostbyname(name); 
        if(HostEntry)
        {
            *(address) = *((u_long*)HostEntry->h_addr_list[0]);

            //*(address) = (unsigned long)HostEntry->h_addr_list[0];
            return 1; // Error 
        }
        else
        {
            return 0; // OK
        }
    }

    else // numeric address - no need for DNS resolve
    {
        *(address) = inet_addr(name);
        return 1;

    }
#endif
    return 0;
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetRandomeNumber
// Last updated : 15/05/2005
// Notes            : GetRandom number for Win32
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HTTPWrapperInitRandomeNumber()
{
    srand((unsigned int)time(NULL));
}

int HTTPWrapperGetRandomeNumber()
{
    int num;
    num = (int)(((double) rand()/ ((double)RAND_MAX+1)) * 16);
    return num;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetRTC
// Last updated : 15/05/2005
// Author Name        : Eitan Michaelson
// Notes            : Get uptime under Win32 & AMT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long HTTPWrapperGetUpTime()
{
#ifdef _WIN32

    long lTime = 0;

    lTime = (GetTickCount() / CLOCKS_PER_SEC);
    return lTime;
#elif defined (_UEZ_RTOS)
    // http client assumes 1000 ticks per second.
    return UEZTickCounterGet()/1000;
#else

        struct timespec tp;

        clock_gettime(CLOCK_MONOTONIC , &tp);
        return tp.tv_sec;

#endif
}

#ifdef _UEZ_RTOS

#ifdef _UEZ_CYA_SSL

int UEZCyaSSL_Send(CYASSL *ssl, char *buf, int sz, void *ctx)
{    
    HTTP_SESSION *pHTTPSession = *(int*)ctx;
    T_uezError error;

    error = UEZNetworkSocketWrite(pHTTPSession->HttpConnection.HttpNetwork, 
              pHTTPSession->HttpConnection.HttpSocket, 
              buf, sz, ETrue, UEZ_TIMEOUT_INFINITE);
        
//printf("UEZCyaSSL_Send s:%d e:%d\n", sz, error);

    if (error == UEZ_ERROR_CLOSED) {
        printf("UEZCyaSSL_Send connection closed\n");
        return CYASSL_CBIO_ERR_CONN_CLOSE;
    } else if (error == UEZ_ERROR_RESETED) {
        printf("UEZCyaSSL_Send connection resetted\n");
        return CYASSL_CBIO_ERR_CONN_RST;
    } else if (error) {
        printf("UEZCyaSSL_Send error #%d\n", error);
        return CYASSL_CBIO_ERR_GENERAL;
    }

    return sz;
}

int UEZCyaSSL_Receive(CYASSL *ssl, char *buf, int sz, void *ctx)
{
    P_HTTP_SESSION pHTTPSession =*(int*)ctx;
    T_uezError error;
    TUInt32 num = 0;

    error = UEZNetworkSocketRead(pHTTPSession->HttpConnection.HttpNetwork, 
                pHTTPSession->HttpConnection.HttpSocket, buf, sz, &num, 10000);

//printf("UEZCyaSSL_Receive b:%d r:%d e:%d\n", sz, num, error);

    if (error == UEZ_ERROR_CLOSED) {
        printf("UEZCyaSSL_Receive connection closed\n");
        return CYASSL_CBIO_ERR_CONN_CLOSE;
    } else if (error == UEZ_ERROR_RESETED) {
        printf("UEZCyaSSL_Receive connection resetted\n");
        return CYASSL_CBIO_ERR_CONN_RST;
    } else if (error == UEZ_ERROR_TIMEOUT) {
        printf("UEZCyaSSL_Receive timeout #%d %d\n", error, num);
        if (num)
        {
            return num;
        }
        else
        {
            return CYASSL_CBIO_ERR_GENERAL;
        }
    }
    else if (error) {
        printf("UEZCyaSSL_Receive error #%d\n", error);
        return CYASSL_CBIO_ERR_GENERAL;
    }

    return num;
}
#endif // _UEZ_CYA_SSL

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLNegotiate(int s, char *hostname)
{
#ifdef _UEZ_CYA_SSL
    static CYASSL_METHOD* method = 0;
    static CYASSL_CTX* ctx = 0;
    static TBool initialized = EFalse;
    P_HTTP_SESSION pHTTPSession = (P_HTTP_SESSION)s;
  
    // we initialilze ssl the first time anyone tries to use it
    if (initialized == EFalse) {
        method = CyaSSLv23_client_method();
        CyaSSL_Init();
        ctx = CyaSSL_CTX_new(method);
        CyaSSL_SetIORecv(ctx, UEZCyaSSL_Receive);
        CyaSSL_SetIOSend(ctx, UEZCyaSSL_Send);
    
        //??SSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
        // load_buffer(ctx, G_CACert, CYASSL_CA);
        CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0); /* TODO: add ca cert */
        initialized = ETrue;
    }

    if (!pHTTPSession->HttpConnection.ssl)
    {
        pHTTPSession->HttpConnection.ssl = CyaSSL_new(ctx);
        if (pHTTPSession->HttpConnection.ssl == NULL)
        {
            printf("null ssl!!!!!\n");
        }
        CyaSSL_set_fd(pHTTPSession->HttpConnection.ssl, pHTTPSession);
    }

    // verify domain name on server certificate
//    CyaSSL_check_domain_name(pHTTPSession->HttpConnection.ssl, "INSPIRON17.teamfdi.net");//hostname);

    if (CyaSSL_connect(pHTTPSession->HttpConnection.ssl) != SSL_SUCCESS) 
    { 
        int err = CyaSSL_get_error(pHTTPSession->HttpConnection.ssl, 0);
        char buffer[80];
        printf("err = %d, %s\n", err, CyaSSL_ERR_error_string(err, buffer));
        printf("SSL_negotiate failed"); /* if you're getting an error here  */
        return SOCKET_ERROR;
    }

    return HTTP_CLIENT_SUCCESS;
#else
    return -1;
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLSend(int s,char *buf, int len,int flags)
{
#ifdef _UEZ_CYA_SSL
    P_HTTP_SESSION pHTTPSession = (P_HTTP_SESSION)s;
//extern unsigned int G_numAlloc;
//extern unsigned int G_totalAlloc;

    //printf("Start Memory: Num: %d, Total: %d bytes\n", G_numAlloc, G_totalAlloc);    

    if (CyaSSL_write(pHTTPSession->HttpConnection.ssl, buf, len) != len) {
        printf("Error sending http request to server\n");
        return SOCKET_ERROR;
    }

    return HTTP_CLIENT_SUCCESS;
#else
    return - 1;
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLRecv(int s,char *buf, int len,int flags)
{
#ifdef _UEZ_CYA_SSL
    P_HTTP_SESSION pHTTPSession = (P_HTTP_SESSION)s;
    int nRet;
    
    // Wait to receive, nRet = NumberOfBytesReceived
    nRet = CyaSSL_read(pHTTPSession->HttpConnection.ssl, buf, len);
    if (nRet <= 0) {
        printf("Error receiving http response from server\n");
        return SOCKET_ERROR;
    }

    return nRet;
#else
    return -1;
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int HTTPWrapperSSLRecvPending(int s)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int HTTPWrapperSSLClose(int s)
{
#ifdef _UEZ_CYA_SSL
    P_HTTP_SESSION pHTTPSession = (P_HTTP_SESSION)s;
        
    CyaSSL_shutdown(pHTTPSession->HttpConnection.ssl);
    CyaSSL_free(pHTTPSession->HttpConnection.ssl);

    pHTTPSession->HttpConnection.ssl = 0;
    
    return 0;
#else
    return -1;
#endif
}
#else
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : TSL Wrapper
// Last updated : 15/05/2005
// Notes            : HTTPWrapper_Sec_Connect
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLConnect(int s,const struct sockaddr *name,int namelen,char *hostname)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLNegotiate(int s,const struct sockaddr *name,int namelen,char *hostname)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLSend(int s,char *buf, int len,int flags)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLRecv(int s,char *buf, int len,int flags)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int HTTPWrapperSSLRecvPending(int s)
{
    return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int HTTPWrapperSSLClose(int s)
{
    return -1;

}
#endif
