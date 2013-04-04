/*-------------------------------------------------------------------------*
 * File:  HTTPServer.h
 *-------------------------------------------------------------------------*
 * Description:
 *      HTTP Server Implementation
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

typedef T_uezError (*T_HTTPCallbackGetVarFunc)(void *aHTTPState, const char *aVarName);
typedef T_uezError (*T_HTTPCallbackSetVarFunc)(
            void *aHTTPState, 
            const char *aVarName, 
            const char *aValue);

typedef struct {
    // uEZ v1.00
    T_HTTPCallbackGetVarFunc iGet;
    T_HTTPCallbackSetVarFunc iSet;

    // uEZ v2.04
    T_uezDevice iNetwork;
} T_httpServerParameters;

TUInt32 HTTPServer(
            T_uezTask aMyTask, 
            void *aParameters);
T_uezError HTTPServerSetVar(
                void *aHTTPState, 
                const char *aVar, 
                const char *aValue);
const char *HTTPServerGetVar(
                void *aHTTPState, 
                const char *aVar);
T_uezError WebServerStart(T_httpServerParameters *aParams);

#endif

/*-------------------------------------------------------------------------*
 * End of File:  HTTPServer.h
 *-------------------------------------------------------------------------*/
