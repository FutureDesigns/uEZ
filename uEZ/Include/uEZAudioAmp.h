/*-------------------------------------------------------------------------*
* File:  uEZAudioAmp.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level Audio Amp drivers.
*-------------------------------------------------------------------------*/

/**
 *	@file 	uEZAudioAmp.h
 *  @brief 	uEZ AudioAmp Interface
 *
 *	The uEZ interface which maps to lower level Audio Amp drivers.
 *
 *	Example:
 *  @code
 *	T_uezDevice device;
 *	UEZAudioAmpOpen("AMP0", &device);
 *	UEZAudioAmpMute(device);
 *	UEZAudioAmpUnMute(device);
 *	UEZAudioAmpSetLevel(device, 4);
 *	UEZAudioAmpGetLevel(device);
 *	UEZAudioAmpClose(device);
 *	@endcode
 */
#ifndef _UEZ_AudioAmp_H_
#define _UEZ_AudioAmp_H_

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
 
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start access to the Audio Amp.
 *
 *	@param [in]		*aName			Pointer to name of Audio Amp display (usually "AMP0")
 *	@param [out]	*aDevice		Pointer to device handle to be returned
 *
 *	@return	T_uezError
 */
T_uezError UEZAudioAmpOpen(const char * const aName, T_uezDevice *aDevice);

/*! 
 *	End access to the Audio Amp.  
 *
 *	@param [in]	aDevice		Device handle of Audio Amp
 *
 *	@return	T_uezError
 */
T_uezError UEZAudioAmpClose(T_uezDevice aDevice);

/*! 
 *	Mute the Audio Amp
 *
 *	@param [in]	aDevice		Device handle of Audio Amp
 *
 *	@return	T_uezError
 */
T_uezError UEZAudioAmpMute(T_uezDevice aDevice);

/*! 
 *	UnMute the Audio Amp
 *
 *	@param [in]	aDevice		Device handle of Audio Amp
 *
 *	@return	T_uezError
 */
T_uezError UEZAudioAmpUnMute(T_uezDevice aDevice);

/*! 
 *	Change the level of the Audio Amp up or down
 *
 *	@param[in]	aDevice		Device handle of Audio Amp
 *
 *	@param[in]	aLevel		Volume level
 *
 *	@return	T_uezError
 */
T_uezError UEZAudioAmpSetLevel(T_uezDevice aDevice, TUInt8 aLevel);

/*! 
 *	Get the current level of the amp.
 *
 *	@param[in]	aDevice		Device handle of Audio Amp
 *
 *	@return	T_uezError
 */
TUInt8 UEZAudioAmpGetLevel(T_uezDevice aDevice);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_AudioAmp_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioAmp.h
 *-------------------------------------------------------------------------*/
