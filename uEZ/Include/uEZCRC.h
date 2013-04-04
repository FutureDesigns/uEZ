/**
 *	@file 	uEZCRC.h
 *  @brief 	uEZ CRC Interface
 *
 *	The uEZ interface which maps to the low level CRC drivers.
 */
#ifndef _UEZ_CRC_H_
#define _UEZ_CRC_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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

#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/CRC.h>

/* High level functions */
T_uezError UEZCRCOpen(const char * const aName, 
            T_uezDevice *aDevice); 
T_uezError UEZCRCClose(T_uezDevice aDevice);

/* Advanced functions: */
T_uezError UEZCRCSetComputationType(
        T_uezDevice aDevice,
        T_uezCRCComputationType aType);
T_uezError UEZCRCAddModifier(T_uezDevice aDevice, T_uezCRCModifier aModifier);
T_uezError UEZCRCRemoveModifier(T_uezDevice aDevice, T_uezCRCModifier aModifier);
T_uezError UEZCRCSetSeed(T_uezDevice aDevice, TUInt32 aSeed);
T_uezError UEZCRCComputeData(
        T_uezDevice aDevice,
        void *aData,
        T_uezCRCDataElementSize aSize,
        TUInt32 aNumDataElements);
T_uezError UEZCRCReadChecksum(T_uezDevice aDevice, TUInt32 *aValue);

#endif // _UEZ_CRC_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZCRC.h
 *-------------------------------------------------------------------------*/
