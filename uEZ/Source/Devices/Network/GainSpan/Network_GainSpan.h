/*-------------------------------------------------------------------------*
 * File:  Network_GainSpan.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef NETWORK_GAINSPAN_H_
#define NETWORK_GAINSPAN_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/Network.h>

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

#endif // NETWORK_GAINSPAN_H_
/*-------------------------------------------------------------------------*
 * End of File:  Network_GainSpan.h
 *-------------------------------------------------------------------------*/
