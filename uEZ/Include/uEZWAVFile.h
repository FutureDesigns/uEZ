/*-------------------------------------------------------------------------*
* File:  uEZWAVFile.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ routines to play WAV audio files.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZWAVFile.h
 *  @brief 	uEZ WAV File Definitions
 *
 *	The uEZ routines to play WAV audio files.
 */
 
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

#ifdef __cplusplus
extern "C" {
#endif


 /**
 *	Start playing the selected WAV File
 */
void UEZStartWAV();

/**
 *	Start playing the WAV File
 */
void UEZStopWAV();

/**
 *	Clean up a WAV file
 */
void UEZWAVCleanUp();

/**
 *	Set the output volume
 *
 *	@param [in]		onBoardSpeakerVolume		the speaker volume
 */
void UEZWAVConfig(TUInt8 onBoardSpeakerVolume);

/**
 *	Play the specified WAV File
 *
 *	@param [in]		*fileName		the filename
 */
T_uezError UEZWAVPlayFile(char* fileName);

/**
 *	Change to the specified WAV File
 *
 *	@param [in]		*newFile		the filename
 */
T_uezError UEZWAVChangeFile(char* newFile);

/**
 *	Gets the WAV status
 *
 *	@return TBool
 */
TBool UEZWAVGetStatus();

/**
 *  Set the digital volume level control (default is 100%)
 *
 *  @param [in] aVolume     Range of volume from 0 (0%) to 256 (100%)
 */
void UEZDACWaveSetVolume(TUInt16 aVolume);

#ifdef __cplusplus
}
#endif
