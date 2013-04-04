/*-------------------------------------------------------------------------*
 * File:  EMAC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EMAC (Ethernet) HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_EMAC_H_
#define _HAL_EMAC_H_

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

#endif // _HAL_EMAC_H_
/*-------------------------------------------------------------------------*
 * End of File:  EMAC.h
 *-------------------------------------------------------------------------*/
