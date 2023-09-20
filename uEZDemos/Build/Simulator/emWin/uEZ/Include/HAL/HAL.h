/*-------------------------------------------------------------------------*
 * File:  HAL.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Generic HAL interface (used by all other HAL interfaces)
 *-------------------------------------------------------------------------*/
#ifndef _HAL_H_
#define _HAL_H_

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
 *  @file   /Include/HAL/HAL.h
 *  @brief  uEZ HAL Interface
 *
 *  The uEZ HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "uEZTypes.h"
#include "uEZErrors.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void *T_uezInterface;

typedef struct {
    const char *iName;
    TUInt16 iVersion;   /**< Version is 0x103 for version 1.03 */
    T_uezError (*InitializeWorkspace)(void *aWorkspace);
    TUInt32 iWorkspaceSize;
} T_halInterface;

typedef struct {
    T_halInterface *iInterface;
} T_halWorkspace;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define HAL_DEVICE_REQUIRE_ONCE() \
    static TBool created = EFalse; \
    if (created) { return; }  \
        created=ETrue

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void HALInterfaceInit(void);

T_uezError HALInterfaceRegister(
                const char * const aName, 
                T_halInterface *aInterface, 
                T_uezInterface *aInterfaceHandle, 
                T_halWorkspace **aWorkspace);

T_uezError HALInterfaceGetWorkspace(T_uezInterface aInterface, T_halWorkspace **aWorkspace);

T_uezError HALInterfaceFind(const char * const aName, T_halWorkspace **aWorkspace);

#ifdef __cplusplus
}
#endif

#endif // _HAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  HAL.h
 *-------------------------------------------------------------------------*/
