/*-------------------------------------------------------------------------*
* File:  uEZToneGenerator.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level ToneGenerator drivers.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZToneGenerator.h
 *  @brief 	uEZ ToneGenerator Interface
 *
 *	The uEZ interface which maps to lower level ToneGenerator drivers.
 */
#ifndef _UEZ_TONE_GENERATOR_H_
#define _UEZ_TONE_GENERATOR_H_

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
#include "uEZTypes.h"
#include "uEZErrors.h"
#include <Device/ToneGenerator.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
	
#endif // _UEZ_TONE_GENERATOR_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZToneGenerator.h
 *-------------------------------------------------------------------------*/
