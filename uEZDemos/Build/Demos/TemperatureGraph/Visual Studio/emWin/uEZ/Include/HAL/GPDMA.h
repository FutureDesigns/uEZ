/*-------------------------------------------------------------------------*
* File:  GPDMA.h
*-------------------------------------------------------------------------*
* Description:
*      HAL implementation of the LPC2478 GP DMA controller.
*-------------------------------------------------------------------------*/
#ifndef GPDMA_H_
#define GPDMA_H_

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

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // GPDMA_H_
/*-------------------------------------------------------------------------*
* End of File:  GPDMA.h
*-------------------------------------------------------------------------*/
