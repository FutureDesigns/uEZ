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

#if LWIP_2_0_x
#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/stats.h>
#include <lwip/sys.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/pbuf.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <lwip/prot/ip.h>
#include <lwip/raw.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <lwip/prot/autoip.h>
#include <lwip/prot/igmp.h>
#include <lwip/dns.h>
#include <netif/etharp.h>
#else
#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/stats.h>
#include <lwip/sys.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/pbuf.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <ipv4/lwip/ip.h>
#include <lwip/raw.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <ipv4/lwip/autoip.h>
#include <ipv4/lwip/igmp.h>
#include <lwip/dns.h>
#include <netif/etharp.h>
#endif

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

