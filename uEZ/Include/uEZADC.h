/**
 *	@file 	uEZADC.h
 *  @brief 	uEZ ADC Interface
 *
 *	The uEZ interface which maps to low level ADC drivers.
 *
 *	Example:
 *  @code
 *	T_uezDevice device;
 *	ADC_RequestSingle request;
 *
 *	UEZADCOpen("ADC0", &device);
 *	UEZADCRequestSingle(device, request);
 *	UEZADCClose(device);
 *	@endcode
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_ADC_H_
#define _UEZ_ADC_H_

#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/ADC.h>

/**
 *	Open up access to the ADC bank.
 *
 *	@param [in] 	*aName		Pointer to name of ADC bank (usually "ADCx" 
 *													where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZADCOpen(
            const char *const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the ADC bank.
 *
 *	@param [in]	aDevice		ADC Device Handle
 *
 *	@return		T_uezError
 */
T_uezError UEZADCClose(T_uezDevice aDevice);

/**
 *	Requests a single ADC poll.
 *
 *	@param [in]		aDevice		 	ADC Device handle
 *	@param [out]	*aRequest  	Structure of read and/or write ADC transaction 
 *														information.
 *
 *	@return		T_uezError
 */
T_uezError UEZADCRequestSingle(
            T_uezDevice aDevice, 
            ADC_RequestSingle *aRequest);

#endif // _UEZ_ADC_H_

// @}
/*-------------------------------------------------------------------------*
 * End of File:  uEZADC.h
 *-------------------------------------------------------------------------*/
