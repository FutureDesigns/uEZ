/*-------------------------------------------------------------------------*
 * File:  ADCBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *     ADC HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_

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
 *  @file   /Include/HAL/ADCBank.h
 *  @brief  uEZ ADC HAL Interface
 *
 *  The uEZ ADC HAL Interface
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
#include <Types/ADC.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/**
 * Callback routine that hands off the captured data.  Data should be copied
 * out of the array at this point for processing later as it might immediately
 * disappear OR calls above should be blocked until the data is processed.
 */
typedef struct {
    void (*CaptureComplete)(
            void *aCallbackWorkspace, 
            ADC_RequestSingle *aRequest);
} T_adcBankCallbackInterface;

/*-------------------------------------------------------------------------*
 * HAL Interface:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Configure)(
            void *aWorkspace, 
            void *aCallbackWorkspace,
            const T_adcBankCallbackInterface * const aCallbackAPI);
    T_uezError (*RequestSingle)(
            void *aWorkspace, 
            ADC_RequestSingle *aRequest);
} HAL_ADCBank;

#endif // _HAL_ADC_H_
/*-------------------------------------------------------------------------*
 * End of File:  ADC.h
 *-------------------------------------------------------------------------*/
