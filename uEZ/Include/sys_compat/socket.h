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
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

/**
 *  @file   /Include/sys_compat/socket.h
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

#endif // _SYS_SOCKET_H_
