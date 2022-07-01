/*-------------------------------------------------------------------------*
* File:  AppHTTPServer.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _APPHTTPSERVER_H_
#define _APPHTTPSERVER_H_

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
#include <uEZ.h>
#include <Source/Library/Web/HTTPServer/HTTPServer.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HTTP_SERVER_HTTP_PORT
#define HTTP_SERVER_HTTP_PORT  80
#endif

#ifndef HTTP_SERVER_DRIVE
#define HTTP_SERVER_DRIVE      "1:/HTTPROOT" // "1" for SD card, "0" for USB drive
#endif
 
T_uezError App_HTTPServerStart(T_uezDevice network);

#ifdef __cplusplus
}
#endif

#endif // _APPHTTPSERVER_H_
/*-------------------------------------------------------------------------*
 * End of File:  AppHTTPServer.h
 *-------------------------------------------------------------------------*/
