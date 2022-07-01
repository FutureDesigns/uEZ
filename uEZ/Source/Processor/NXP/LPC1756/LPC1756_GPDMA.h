/*-------------------------------------------------------------------------*
 * File:  LPC1756_GPDMA.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_GPDMA_H_
#define LPC1756_GPDMA_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/GPDMA.h>

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
extern const HAL_GPDMA LPC1756_GPDMA_Channel0_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel1_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel2_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel3_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel4_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel5_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel6_Interface;
extern const HAL_GPDMA LPC1756_GPDMA_Channel7_Interface;
void LPC1756_GPDMA0_Require(void);
void LPC1756_GPDMA1_Require(void);
void LPC1756_GPDMA2_Require(void);
void LPC1756_GPDMA3_Require(void);
void LPC1756_GPDMA4_Require(void);
void LPC1756_GPDMA5_Require(void);
void LPC1756_GPDMA6_Require(void);
void LPC1756_GPDMA7_Require(void);

#ifdef __cplusplus
}
#endif

#endif // LPC1756_GPDMA_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_GPDMA.h
 *-------------------------------------------------------------------------*/
