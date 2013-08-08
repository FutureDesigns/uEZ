/*-------------------------------------------------------------------------*
 * File:  uEZDACWAVFile.h
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ DAC wav file player
 * Implementation:
 *      The driver is timer based and plays 11025Hz, Mono, 8-bit 
 *      Windows based wav files.
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

T_uezError UEZDACWAVConfig(const char* aTimer);
T_uezError UEZDACWAVStop();
T_uezError UEZDACWAVPlay(char* aFileName);
void UEZDACWAVCleanUp();
TBool UEZDACWAVGetStatus();
T_uezError UEZDACWAVPlayBuffer(TUInt8 *aBuffer, TUInt32 aSize);
void UEZDACWaveSetVolume(TUInt16 aVolume);
TUInt32 UEZDACWAVGetSamplePos(void);
TBool UEZDACWAVIsDone(void);
T_uezError UEZDACWAVPlayPause(TBool aBool);
