#ifndef _APPHTTPSERVER_H_
#define _APPHTTPSERVER_H_
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
#include <uEZ.h>
#include <Source\Library\Web\HTTPServer\HTTPServer.h>

#ifndef HTTP_SERVER_HTTP_PORT
#define HTTP_SERVER_HTTP_PORT  80
#endif

#ifndef HTTP_SERVER_DRIVE
#define HTTP_SERVER_DRIVE      "1:/HTTPROOT" // "1" for SD card, "0" for USB drive
#endif
 
T_uezError App_HTTPServerStart(T_uezDevice network);

#endif // _APPHTTPSERVER_H_
