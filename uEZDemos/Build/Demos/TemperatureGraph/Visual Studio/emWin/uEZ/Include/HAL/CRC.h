/*-------------------------------------------------------------------------*
 * File:  CRC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef HAL_CRC_H_
#define HAL_CRC_H_

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
/**
 *  @file   /Include/HAL/CRC.h
 *  @brief  uEZ CRC HAL Interface
 *
 *  The uEZ CRC HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/HAL.h>
#include <Types/CRC.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
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
} HAL_CRC;

#ifdef __cplusplus
}
#endif

#endif // HAL_CRC_H_
/*-------------------------------------------------------------------------*
 * End of File:  CRC.h
 *-------------------------------------------------------------------------*/
