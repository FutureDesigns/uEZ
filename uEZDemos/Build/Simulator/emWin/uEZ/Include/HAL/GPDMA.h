/*-------------------------------------------------------------------------*
* File:  GPDMA.h
*-------------------------------------------------------------------------*
* Description:
*      HAL implementation of the LPC2478 GP DMA controller.
*-------------------------------------------------------------------------*/
#ifndef GPDMA_H_
#define GPDMA_H_

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
 *  @file   /Include/HAL/GPDMA.h
 *  @brief  uEZ GPDMA HAL Interface
 *
 *  The uEZ GPDMA HAL Interface
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
#include <Types/GPDMA.h>

/*-------------------------------------------------------------------------*
* Defines:
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
* Types:
*-------------------------------------------------------------------------*/

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*PrepareRequest)(
        void *aWorkspace, 
        const T_gpdmaRequest *aRequest,
        T_gpdmaCallback aISRCallback,
        void *aISRCallbackWorkspace);
    T_uezError (*Start)(void *aWorkspace);
    T_uezError (*Stop)(void *aWorkspace);
    TBool (*IsValidAddress)(TUInt32 aAddr);
} HAL_GPDMA;


#endif // GPDMA_H_
/*-------------------------------------------------------------------------*
* End of File:  GPDMA.h
*-------------------------------------------------------------------------*/
