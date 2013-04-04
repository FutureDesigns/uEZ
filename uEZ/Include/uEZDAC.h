/**
 *	@file 	uEZDAC.h
 *  @brief 	uEZ DAC Interface
 *
 *	The uEZ interface which maps to lower level DAC drivers.
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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
#ifndef _UEZ_DAC_H_
#define _UEZ_DAC_H_

#include <uEZTypes.h>
#include <uEZErrors.h>

/**
 *	Open up access to the DAC bank.
 *
 *	@param [in]		*aName		Pointer to name of DAC bank (usually "DACx" 
 *													where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return	T_uezError
 */
T_uezError UEZDACOpen(
            const char *const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the ADC bank.
 *
 *	@param [in]	aDevice		Device handle of DAC to close
 *
 *	@return	T_uezError
 */
T_uezError UEZDACClose(T_uezDevice aDevice);

/**
 *  @ingroup UEZDAC
 *	Turn on or off the bias voltage.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aBias			On (ETrue) or Off (EFalse)
 *
 *	@return	T_uezError
 */
T_uezError UEZDACSetBias(
            T_uezDevice aDevice,
            TBool aBias);
						
/**
 *  @ingroup UEZDAC
 *	Set internal voltage reference.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aVRef			Voltage reference in millivolts.
 *
 *	@return	T_uezError
 */
T_uezError UEZDACSetVRef(
            T_uezDevice aDevice,
            TUInt32 aVRef);

/**
 *  @ingroup UEZDAC
 *	Write value to the DAC in millivolts.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aV				Value in millivolts to be written to DAC
 *
 *	@return	T_uezError
 */						
T_uezError UEZDACWriteMilliVolts(
        T_uezDevice aDevice,
        TUInt32 aV);

#endif // _UEZ_DAC_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZDAC.h
 *-------------------------------------------------------------------------*/
