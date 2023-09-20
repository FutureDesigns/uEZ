/*-------------------------------------------------------------------------*
 * File:  WAVFile.h
 *-------------------------------------------------------------------------*
 * Description:
 *      
 * Implementation:
 *      
 *-------------------------------------------------------------------------*/

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
/**
 *  @file   /Include/Types/WAVFile.h
 *  @brief  uEZ WAVFile Types
 *
 *  The uEZ WAVFile Types
 *  Microsoft PCM soundfile format
 *  https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TUInt32 iChunckID;            /** 4 bytes */
  TUInt32 iChunckSize;          /** 4 bytes */
  TUInt32 iFormat;              /** 4 bytes */
  TUInt32 iSubChunck1ID;        /** 4 bytes */
  TUInt32 iSubChunck1Size;      /** 4 bytes */ 
  TUInt16 iAudioFormat;         /** 2 bytes */
  TUInt16 iNumChannels;         /** 2 bytes */
  TUInt32 iSampleRate;          /** 4 bytes */
  TUInt32 iByteRate;            /** 4 bytes */
  TUInt16 iBlockAlign;          /** 4 bytes */
  TUInt16 iBitsPerSample;       /** 2 bytes */
  TUInt32 iSubChunck2ID;        /** 4 bytes */
  TUInt32 iSubChunck2Size;      /** 4 bytes */
  //meta data, if available
  TUInt8 iArtist[20+1];
  TUInt8 iSongName[20+1];
}wavFileHeader;

#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*
 * End of File:  WAVFile.h
 *-------------------------------------------------------------------------*/

