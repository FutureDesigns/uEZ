/*-------------------------------------------------------------------------*
 * File:  Network_GainSpan.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_GAINSPAN_H_
#define NETWORK_GAINSPAN_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/Network.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_GAINSPAN_MAX_NUM_SOCKETS
#define NETWORK_GAINSPAN_MAX_NUM_SOCKETS          8
#endif

#ifndef NETWORK_GAINSPAN_RECV_QUEUE_SIZE
#define NETWORK_GAINSPAN_RECV_QUEUE_SIZE          1024 // bytes
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const char *iSPIDeviceName;
    TUInt32 iRate; // Hz rate to talk SPI

    // Pins.
    T_uezGPIOPortPin iSPIChipSelect;
    T_uezGPIOPortPin iDataReadyIO;
    T_uezGPIOPortPin iProgramMode;
    T_uezGPIOPortPin iSPIMode;
    T_uezGPIOPortPin iSRSTn;
    T_uezGPIOPortPin iWIFIFactoryRestore;
} T_GainSpan_Network_SPISettings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Network_GainSpan_Create(
    const char *aName,
    const T_GainSpan_Network_SPISettings *aSettings);
static T_uezError Network_GainSpan_GetStatus(
    void *aWorkspace,
    T_uezNetworkStatus *aStatus);
extern const DEVICE_Network GainSpan_Network_Interface;

#ifdef __cplusplus
}
#endif

#endif // NETWORK_GAINSPAN_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network_GainSpan.h
 *-------------------------------------------------------------------------*/
