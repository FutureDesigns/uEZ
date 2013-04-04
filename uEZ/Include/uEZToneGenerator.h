/**
 *	@file 	uEZToneGenerator.h
 *  @brief 	uEZ ToneGenerator Interface
 *
 *	The uEZ interface which maps to lower level ToneGenerator drivers.
 */
#ifndef _UEZ_TONE_GENERATOR_H_
#define _UEZ_TONE_GENERATOR_H_

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

#include "uEZTypes.h"
#include "uEZErrors.h"
#include <Device/ToneGenerator.h>

/**
 *	Open up access to the Tone Generator.
 *
 *	@param [in] 	*aName		Pointer to name of ToneGenerator display (usually
 *                          ToneGenerator")
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZToneGeneratorOpen(
            const char * const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the Tone Generator.
 *
 *	@param [in]	aDevice		Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZToneGeneratorClose(T_uezDevice aDevice);

/**
 *	Play the given tone on the tone generator
 *
 *	@param [in]	aDevice				Handle to opened ToneGenerator device.
 *  @param [in]	aTonePeriod   Number of cycles for tone (1/Frequency)
 *  @param [in]	aDuration     Number of milliseconds to play tone
 *
 *	@return		T_uezError
 */
T_uezError UEZToneGeneratorPlayTone(
            T_uezDevice aDevice, 
            TUInt32 aTonePeriod,
            TUInt32 aDuration);

/**
 *	Play the given tone on the tone generator forever
 *
 *	@param [in]	aDevice				Handle to opened ToneGenerator device.
 *  @param [in]	aTonePeriod   Number of cycles for tone (1/Frequency)
 *
 *	@return		T_uezError
 */
T_uezError UEZToneGeneratorPlayToneContinuous(
            T_uezDevice aDevice,
            TUInt32 aTonePeriod);

#endif // _UEZ_TONE_GENERATOR_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZToneGenerator.h
 *-------------------------------------------------------------------------*/
