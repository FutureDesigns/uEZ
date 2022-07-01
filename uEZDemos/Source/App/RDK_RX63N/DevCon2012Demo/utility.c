/*-------------------------------------------------------------------------*
 * File:  utility.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
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
#include <uEZNetwork.h>
#include "NVSettings.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezDevice G_network;
T_uezNetworkStatus G_networkStatus;


// Required Basic Web Function
void FuncTestPageHit(void)
{
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Network_Connect
 *---------------------------------------------------------------------------*
 * Description:
 *      When a system needs the network to be running, this routine is
 *      called.  Most of the work is already done in the UEZPlatform.
 *      This routine calls the correct connect routine and gets the
 *      network information.
 *---------------------------------------------------------------------------*/
T_uezError UEZPlatform_Network_Connect(void)
{
    T_uezError error = UEZ_ERROR_NOT_AVAILABLE;

    static TBool connected = EFalse;
    if (!connected) {
        connected = ETrue;
        error = UEZNetworkConnect(
            "WiredNetwork0",
            "lwIP",
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPAddr,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPMask,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPGateway,
            ETrue,
            &G_network,
            &G_networkStatus);
    }

    return error;
}


/*-------------------------------------------------------------------------*
 * End of File:  utility.c
 *-------------------------------------------------------------------------*/
