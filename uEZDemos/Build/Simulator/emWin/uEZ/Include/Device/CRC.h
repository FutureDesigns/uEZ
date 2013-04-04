/*-------------------------------------------------------------------------*
 * File:  Device/CRC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     CRC Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_CRC_H_
#define _DEVICE_CRC_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZDevice.h>
#include <Types/CRC.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    /* High level functions */
    T_uezError (*Open)();
    T_uezError (*Close)();
    
    /* Advanced functions: */
    T_uezError (*SetComputationType)(
        void *aWorkspace,
        T_uezCRCComputationType aType);
    T_uezError (*AddModifier)(void *aWorkspace, T_uezCRCModifier aModifier);
    T_uezError (*RemoveModifier)(void *aWorkspace, T_uezCRCModifier aModifier);
    T_uezError (*SetSeed)(void *aWorkspace, TUInt32 aSeed);
    T_uezError (*ComputeData)(
        void *aWorkspace,
        void *aData,
        T_uezCRCDataElementSize aSize,
        TUInt32 aNumDataElements);
    T_uezError (*ReadChecksum)(void *aWorkspace, TUInt32 *aValue);
} DEVICE_CRC;

#endif // _DEVICE_CRC_H_
/*-------------------------------------------------------------------------*
 * End of File:  Device/CRC.h
 *-------------------------------------------------------------------------*/

