/*-------------------------------------------------------------------------*
 * File:  Types/ADC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     ADC types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _ADC_TYPES_H_
#define _ADC_TYPES_H_

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

/**
 *  @file   /Include/Types/ADC.h
 *  @brief  uEZ ADC Types
 *
 *  The uEZ ADC Types
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    ADC_TRIGGER_STOP=0,
    ADC_TRIGGER_NOW=1,
    ADC_TRIGGER_GPIO_A,
    ADC_TRIGGER_GPIO_B,
    ADC_TRIGGER_GPIO_C,
    ADC_TRIGGER_GPIO_D,
    ADC_TRIGGER_GPIO_E,
    ADC_TRIGGER_GPIO_F,
} T_adcTrigger;

typedef struct {
   
    TUInt32 iADCChannel;  			/** Channel to read */ 

    TUInt8 iBitSampleSize;     		/** Number of bits in ADC */
    
    T_adcTrigger iTrigger; 			/** When to trigger (or immediately if NOW) */

    TBool iTriggerOnFallingEdge;	/** Trigger type if using GPIO pin */

    TUInt32 *iCapturedData;     	/** Data when read is stored here */
} ADC_RequestSingle;

#ifdef __cplusplus
}
#endif

#endif // _ADC_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/ADC.h
 *-------------------------------------------------------------------------*/
