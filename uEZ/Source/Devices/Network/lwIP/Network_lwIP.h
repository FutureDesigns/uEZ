/*-------------------------------------------------------------------------*
 * File:  Network_lwIP.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_LWIP_H_
#define NETWORK_LWIP_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/Network.h>
#include "arch/sys_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_LWIP_RECV_QUEUE_SIZE
// Whatever this was it is no longer used.
//#define NETWORK_LWIP_RECV_QUEUE_SIZE          1024 // bytes
#endif

#ifndef NETWORK_LWIP_MAX_NUM_SOCKETS
//Can define in Config_Build.h
#define NETWORK_LWIP_MAX_NUM_SOCKETS          16
#endif

// Aux Commands:
#define NETWORK_LWIP_AUX_COMMAND_GET_VERSION     1

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Network_lwIP_Configure(void *aWorkspace);
void Network_lwIP_Create(const char *aName);

extern const DEVICE_Network lwIP_Network_Interface;

#ifdef __cplusplus
}
#endif

#endif // NETWORK_LWIP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network_lwIP.h
 *-------------------------------------------------------------------------*/
