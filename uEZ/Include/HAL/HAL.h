/*-------------------------------------------------------------------------*
 * File:  HAL.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Generic HAL interface (used by all other HAL interfaces)
 *-------------------------------------------------------------------------*/
#ifndef _HAL_H_
#define _HAL_H_

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

#endif // _HAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  HAL.h
 *-------------------------------------------------------------------------*/
