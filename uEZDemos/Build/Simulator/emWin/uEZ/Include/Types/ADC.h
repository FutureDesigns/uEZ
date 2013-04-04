/*-------------------------------------------------------------------------*
 * File:  Types/ADC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     ADC types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _ADC_TYPES_H_
#define _ADC_TYPES_H_

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

#endif // _ADC_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/ADC.h
 *-------------------------------------------------------------------------*/
