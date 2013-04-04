/*-------------------------------------------------------------------------*
 * File:  WAVFile.h
 *-------------------------------------------------------------------------*
 * Description:
 *      
 * Implementation:
 *      
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
typedef struct {
  TUInt32 chunckID;            /** 4 bytes */
  TUInt32 chunckSize;          /** 4 bytes */
  TUInt32 format;              /** 4 bytes */
  TUInt32 subChunck1ID;        /** 4 bytes */
  TUInt32 subChunck1Size;      /** 4 bytes */ 
  TUInt16 audioFormat;         /** 2 bytes */
  TUInt16 numChannels;         /** 2 bytes */
  TUInt32 sampleRate;          /** 4 bytes */
  TUInt32 byteRate;            /** 4 bytes */
  TUInt16 blockAlign;          /** 4 bytes */
  TUInt16 bitsPerSample;       /** 2 bytes */
  TUInt32 subChunck2ID;        /** 4 bytes */
  TUInt32 subChunck2Size;      /** 4 bytes */
}wavFileHeader;
