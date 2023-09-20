/*-------------------------------------------------------------------------*
 * File:  EMAC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EMAC (Ethernet) HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_EMAC_H_
#define _HAL_EMAC_H_

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
 *  @file   /Include/HAL/EMAC.h
 *  @brief  uEZ EMAC HAL Interface
 *
 *  The uEZ EMAC HAL Interface
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
#include <HAL/HAL.h>
#include <Include/Types/EMAC.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_uezEMACReceiveInterruptCallback)(void *aCallbackWorkspace);

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Configure)(void *aWorkspace, T_EMACSettings *iSettings);
    TBool (*CheckFrameReceived)(void *aWorkspace);
    TUInt16 (*StartReadFrame)(void *aWorkspace);
    void (*EndReadFrame)(void *aWorkspace);
    void (*RequestSend)(void *aWorkspace);
    void (*DoSend)(void *aWorkspace, TUInt16 aFrameSize);
    void (*CopyToFrame)(void *aWorkspace, void *aSource, TUInt32 aSize);
    void (*CopyFromFrame)(void *aWorkspace, void *aDestination, TUInt32 aSize);

    // uEZ v1.10
    void (*EnableReceiveInterrupt)(
        void *aWorkspace,
        T_uezEMACReceiveInterruptCallback aCallback,
        void *aCallbackWorkspace);
    void (*DisableReceiveInterrupt)(void *aWorkspace);
} HAL_EMAC;

#ifdef __cplusplus
}
#endif

#endif // _HAL_EMAC_H_
/*-------------------------------------------------------------------------*
 * End of File:  EMAC.h
 *-------------------------------------------------------------------------*/
