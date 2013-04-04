/*-------------------------------------------------------------------------*
 * File:  GPDMA.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ GPDMA Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_GPDMA_H_
#define _DEVICE_GPDMA_H_

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
/**
 *  @file   /Include/Device/GPDMA.h
 *  @brief  uEZ GPDMA Device Interface
 *
 *  The uEZ GPDMA Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZDevice.h>
#include <Types/GPDMA.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*ProcessRequest)(
            void *aWorkspace, 
            const T_gpdmaRequest *aRequest);

    T_uezError (*PrepareRequest)(
            void *aWorkspace,
            const T_gpdmaRequest *aRequest);
    T_uezError (*RunPrepared)(void *aWorkspace);
    T_uezError (*WaitTilDone)(void *aWorkspace, TUInt32 aTimeout);
    T_uezError (*Stop)(void *aWorkspace);
} DEVICE_GPDMA;

#endif // _DEVICE_GPDMA_H_
/*-------------------------------------------------------------------------*
 * End of File:  GPDMA.h
 *-------------------------------------------------------------------------*/

