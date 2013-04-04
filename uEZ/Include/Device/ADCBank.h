/*-------------------------------------------------------------------------*
 * File:  ADCBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ ADCBank Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_ADCBANK_H_
#define _DEVICE_ADCBANK_H_

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
 *  @file   /Include/Device/ADCBank.h
 *  @brief  uEZ ADC Bank Device Interface
 *
 *  The uEZ ADC Bank Device Interface
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
#include <uEZDevice.h>
#include <Types/ADC.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*RequestSingle)(
            void *aWorkspace, 
            ADC_RequestSingle *aRequest);
} DEVICE_ADCBank;

#endif // _DEVICE_ADCBANK_H_
/*-------------------------------------------------------------------------*
 * End of File:  ADCBank.h
 *-------------------------------------------------------------------------*/

