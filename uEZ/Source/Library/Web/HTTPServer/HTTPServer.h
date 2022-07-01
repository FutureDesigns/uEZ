/*-------------------------------------------------------------------------*
 * File:  HTTPServer.h
 *-------------------------------------------------------------------------*
 * Description:
 *      HTTP Server Implementation
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
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
 
    // uEZ v2.05
    const char *iDrivePrefix;
    TUInt8 iPort; // usually Port 80
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
