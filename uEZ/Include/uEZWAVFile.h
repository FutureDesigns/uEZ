/**
 *	@file 	uEZWAVFile.h
 *  @brief 	uEZ WAV File Definitions
 *
 *	The uEZ routines to play WAV audio files.
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
