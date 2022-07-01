/*-------------------------------------------------------------------------*
* File:  socket.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/**
 *  @file   /Include/sys/socket.h
 *  @brief  uEZ system socket Interface
 *
 *  The uEZ system socket Interface
 *
 *  NOTE: In order to use this code, you must include the directories
 *   Source/Library/lwIP/src/include and
 *   Source/Library/lwIP/include
 *   Source/Library/lwIP/src
 * in the include diretory list for the project.
 * If using IPv4, include directory
 *   Source/Library/lwIP/src/include/ipv4
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
#include "Config.h"

#include <Source/Library/lwIP/src/include/lwip/opt.h>
#include <Source/Library/lwIP/src/include/lwip/init.h>
#include <Source/Library/lwIP/src/include/lwip/stats.h>
#include <Source/Library/lwIP/src/include/lwip/sys.h>
#include <Source/Library/lwIP/src/include/lwip/mem.h>
#include <Source/Library/lwIP/src/include/lwip/memp.h>
#include <Source/Library/lwIP/src/include/lwip/pbuf.h>
#include <Source/Library/lwIP/src/include/lwip/netif.h>
#include <Source/Library/lwIP/src/include/lwip/sockets.h>
#include <Source/Library/lwIP/src/include/ipv4/lwip/ip.h>
#include <Source/Library/lwIP/src/include/lwip/raw.h>
#include <Source/Library/lwIP/src/include/lwip/udp.h>
#include <Source/Library/lwIP/src/include/lwip/tcp.h>
#include <Source/Library/lwIP/src/include/ipv4/lwip/autoip.h>
#include <Source/Library/lwIP/src/include/ipv4/lwip/igmp.h>
#include <Source/Library/lwIP/src/include/lwip/dns.h>
#include <Source/Library/lwIP/src/include/netif/etharp.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif // _SYS_SOCKET_H_
/*-------------------------------------------------------------------------*
 * End of File:  socket.h
 *-------------------------------------------------------------------------*/

