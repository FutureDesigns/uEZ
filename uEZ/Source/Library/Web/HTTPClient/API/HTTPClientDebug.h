
#ifndef _HTTP_CLIENT_DEBUG
#define _HTTP_CLIENT_DEBUG

#include "HTTPClient.h" 

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP Client Debug global definitions
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _HTTP_DEBUGGING_

VOID HTTPClientDebug(const CHAR* FunctionName,const CHAR *DebugDump,UINT32 iLength,CHAR *DebugDescription,...);
const CHAR *HTTPClientDebugReturnCode(INT32 nRetCode);

#endif  // _HTTP_DEBUGGING_
 
#endif  // _HTTP_CLIENT_DEBUG
