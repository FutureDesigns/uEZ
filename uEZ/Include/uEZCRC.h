/*-------------------------------------------------------------------------*
* File:  uEZCRC.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to the low level CRC drivers.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_CRC_H_
#define _UEZ_CRC_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/CRC.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _UEZ_CRC_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZCRC.h
 *-------------------------------------------------------------------------*/
