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
#ifndef _UEZ_AudioAmp_H_
#define _UEZ_AudioAmp_H_

#include <uEZTypes.h>
#include <uEZErrors.h>
 
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

#endif // _UEZ_AudioAmp_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioAmp.h
 *-------------------------------------------------------------------------*/
