/*-------------------------------------------------------------------------*
* File:  uEZADC.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level ADC drivers.
*-------------------------------------------------------------------------*/
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
#ifndef _UEZ_ADC_H_
#define _UEZ_ADC_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/ADC.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
			
#endif // _UEZ_ADC_H_

// @}
/*-------------------------------------------------------------------------*
 * End of File:  uEZADC.h
 *-------------------------------------------------------------------------*/
