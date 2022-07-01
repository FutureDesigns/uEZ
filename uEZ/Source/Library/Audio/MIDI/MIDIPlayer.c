/*-------------------------------------------------------------------------*
 * File:  MIDIPlayer.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Code for parsing and playing basic midi files
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup MIDIPlayer
 *  @{
 *  @brief     MIDIPlayer
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    Code for parsing and playing basic midi files
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZFile.h>
#include <UEZTypes.h>
#include <UEZErrors.h>
#include <Device/PWM.h>
#include <uEZToneGenerator.h>
#include <uEZPlatform.h>

#include "MIDIPlayer.h"
#include "MIDINoteFreq.h"

#define MIDI_DEBUG		0
 
#define MIDI_BLOCK_SIZE		256
#define MICROSEC_PER_MIN	60000000

T_MIDIEvent G_MidiEvent;
T_MIDIEvent G_PreviousMidiEvent;
TUInt8 G_MidiChannel;
TUInt8 G_Parm1;
TUInt8 G_Parm2;

T_MIDIMetaEvent G_MetaEvent;
int32_t G_MetaLength;
int32_t G_MetaCounter;

T_MIDINextByteType G_NextByteType = MIDI_FIRST_EVENT_BYTE;
int32_t G_DeltaTime = 0;
int32_t G_Tempo = 500000;	// default = 120 bpm or 500000 microseconds per quarter-note
int32_t G_TicksPerBeat;
double G_TimeMultiplier = 1;
int32_t G_LastNote = 0;

void MidiEventByteParser(TUInt8 aNextByte);
void BigE_Int32(TUInt32 *aValue);
void BigE_Int16(TUInt16 *aValue);

T_uezDevice G_MIDI_tg = 0;

/*---------------------------------------------------------------------------*
 * Routine:  playMIDITask
 *---------------------------------------------------------------------------*/
/**
 *  Setup MIDI task
 * 
 *  @param [in] 	aMyTask			uEZ Task
 *                          
 *	@param [in]		*aParameters	Parameters for task
 *
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 */
/*---------------------------------------------------------------------------*/
TUInt32 playMIDITask(T_uezTask aMyTask, void *aParameters)
{
	T_midiPlayParameters *params = (T_midiPlayParameters *)aParameters;
	
	#if MIDI_DEBUG
		printf("--- NEW MIDI PLAYER TASK ---\n");
		printf("File Name: %s\n", params->iFilename);
	#endif
	
	playMIDI(params->iFilename, params->iRun);

	UEZSemaphoreRelease(*params->iSem);
        
        return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  playMIDI
 *---------------------------------------------------------------------------*/
/**
 *  Play MIDI file
 *
 *	@param [in] 	*aFilename		Pointer to filename to play
 *                          
 *	@param [in]		*aRun			Pointer to bool, true is play
 *
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 */
/*---------------------------------------------------------------------------*/
T_uezError playMIDI(char * aFilename, TBool *aRun)
{
	T_uezError error;
	T_uezFile file;
	T_MidiHeader header;
	T_MidiTrackHeader trackHeader;
	
	char midiBytes[MIDI_BLOCK_SIZE];
	TUInt32 numBytesRead;
	TUInt32 totalTrackBytesRead;
	TUInt32 loopCount;
	TUInt8 trackCount;
    
	error = UEZToneGeneratorOpen("Speaker", &G_MIDI_tg);
	if(error){
		#if MIDI_DEBUG
			printf("Error %d: Failed open tone generator\n", error);
		#endif
		return error;
	}
	
	error = UEZFileOpen(aFilename, FILE_FLAG_READ_ONLY, &file);
	if(error){
		#if MIDI_DEBUG
			printf("Error %d: Failed open midi file\n", error);
		#endif
		return error;
	}
	
	error = UEZFileRead(file, &header, sizeof(T_MidiHeader)-2, &numBytesRead);
	if(error){
		#if MIDI_DEBUG
			printf("Error %d: Failed read midi header\n", error);
		#endif
		return error;
	}
	
	if(strcmp(header.IChunkID, "MThd")!=0){
		//Incorrect MIDI Header
		#if MIDI_DEBUG
			printf("Incorrect midi header [%s]\n", header.IChunkID);
		#endif
		return error;
	}
	
	BigE_Int32(&header.IChunkSize);
	BigE_Int16(&header.IFormatType);
	BigE_Int16(&header.INumTracks);
	BigE_Int16(&header.ITimeDivision);
	
	#if MIDI_DEBUG
		printf("- MIDI Header ----------------\n");
		printf("Chunk ID: %4.4s\n", header.IChunkID);
		printf("Chunk Size: %d\n", header.IChunkSize);
		printf("Format Type: %d\n", header.IFormatType);
		printf("Number of Tracks: %d\n", header.INumTracks);
	#endif
	
	if(!(header.ITimeDivision & 0x8000)){
		// Ticks per beat timing
		G_TicksPerBeat = header.ITimeDivision & 0x7FFF;
	}
	else{
		// Frame per second timing not supported
		#if MIDI_DEBUG
			printf("Frame per second timing not supported\n");
		#endif
		return error;
	}
	
	G_TimeMultiplier = ((MICROSEC_PER_MIN / G_Tempo) * G_TicksPerBeat) / (double)60000; // (beats per minute * ticks per beat) / (60000 milliseconds)
	
	*aRun = ETrue;
	
	trackCount = 0;
	while(header.INumTracks > trackCount++)
	{
		error = UEZFileRead(file, &trackHeader, sizeof(T_MidiTrackHeader), &numBytesRead);
		if(error)
			return error;
		
		if(strcmp(trackHeader.IChunkID, "MTrk")!=0){
			//Incorrect MIDI Track Header
			#if MIDI_DEBUG
				printf("Incorrect MIDI Track Header [%s]\n", trackHeader.IChunkID);
			#endif
			return error;
		}
	
		BigE_Int32(&trackHeader.IChunkSize);

		#if MIDI_DEBUG
			printf("- MIDI Track Header -------\n");
			printf("Chunk ID: %4.4s\n", trackHeader.IChunkID);
			printf("Chunk Size: %d\n", trackHeader.IChunkSize);
		#endif
	
		G_NextByteType = MIDI_FIRST_EVENT_BYTE;
		totalTrackBytesRead = 0;
		while(totalTrackBytesRead < trackHeader.IChunkSize)
	    {
			TUInt32 bytesToRead = MIDI_BLOCK_SIZE;
			if((trackHeader.IChunkSize - totalTrackBytesRead) < MIDI_BLOCK_SIZE){
				bytesToRead = trackHeader.IChunkSize - totalTrackBytesRead;
			}
			
			#if MIDI_DEBUG
				printf("Bytes left in track: %d\n", trackHeader.IChunkSize - totalTrackBytesRead);
				printf("Bytes to Read: %d\n", bytesToRead);
			#endif
		
			error = UEZFileRead(file, midiBytes, bytesToRead, &numBytesRead);
			if(error)
			{
				#if MIDI_DEBUG
					printf("Failed to read MIDI track\n");
				#endif
				return error;
			}
			
	        loopCount = 0;
	        while(loopCount < bytesToRead)
	        {
	            MidiEventByteParser(midiBytes[loopCount]);
				midiBytes[loopCount] = ' ';
	            ++loopCount;
				
				if(!*aRun)
					break;
	        }
		
			totalTrackBytesRead += numBytesRead;
			
			if(!*aRun)
				break;
		}
		 
		if(!*aRun)
			break; 
		
	} // while(header.INumTracks > trackCount++)
	
	UEZToneGeneratorPlayToneContinuous(
		G_MIDI_tg,
		MIDI_NOTE_0FF
	);
	
    UEZFileClose(file);
	
	return error;
}


TUInt32 G_delay;
void MidiEventByteParser(TUInt8 aNextByte)
{
	switch(G_NextByteType)
	{
		case MIDI_FIRST_EVENT_BYTE:
			G_DeltaTime = 0;
			// Continue to next case..
		case MIDI_DTIME_BYTE:
		{
			G_NextByteType = MIDI_DTIME_BYTE;
			// Add byte to variable time
			G_DeltaTime <<= 7;
			G_DeltaTime += (aNextByte & 0x7F);

			if(!(aNextByte & 0x80))
			{
				TUInt32 delayMS = (TUInt32)(G_DeltaTime / (double)G_TimeMultiplier); // Temporary - 100BPM
				G_delay = delayMS;
				UEZTaskDelay(delayMS);
				G_NextByteType = MIDI_ETYPE_BYTE;
				
				#if MIDI_DEBUG
					printf("== START EVENT [Delta Time = %d] ==\n", delayMS);
				#endif
			}		
			break;
		}
		case MIDI_ETYPE_BYTE:
		{
			TUInt16 midiEvent = (aNextByte & 0xF0)>>4;
			if(midiEvent < MIDI_NOTE_OFF)
			{
				// Same as Previous Event
				G_Parm1 = aNextByte;
				G_NextByteType = MIDI_PARM2_BYTE;
			}
			else // New Event Type
			{
				G_MidiEvent = (T_MIDIEvent)(midiEvent);
				G_MidiChannel = (aNextByte & 0x0F);
			
				switch(G_MidiEvent)
				{
					case MIDI_NOTE_OFF:
					case MIDI_NOTE_ON:
					case MIDI_NOTE_AFTERTOUCH:
					case MIDI_CONTROLLER:
					case MIDI_PITCH_BEND:
						G_NextByteType = MIDI_PARM1_BYTE;
						break;
					case MIDI_PROGRAM_CHANGE:
					case MIDI_CHANNEL_AFTERTOUCH:
						G_NextByteType = MIDI_PARM2_BYTE;
						break;
					case MIDI_META_AND_SYS:
					{
						switch(G_MidiChannel)
						{
						case 0x0:
						case 0x7:
							G_NextByteType = MIDI_META_SIZE_BYTE;	// Just displaying system data
							break;
						case 0xF:
							G_NextByteType = MIDI_META_EVENT_BYTE;
							break;
						default:
							printf("Unknown System Event: 0x%01X%01X\n", G_MidiEvent, G_MidiChannel);
						}
						break;
					}
					default:
					{
						printf("Unknown Event Type: 0x%01X\n", G_MidiEvent);
						G_NextByteType = MIDI_UNKNOWN_BYTE;
						break;
					}
				}
			}
			
			#if MIDI_DEBUG
				printf("Event Type = %02X\n", aNextByte);
			#endif
			break;
		}
		case MIDI_PARM1_BYTE:
		{
			G_Parm1 = aNextByte;
			G_NextByteType = MIDI_PARM2_BYTE;
			
			
			break;
		}
		case MIDI_PARM2_BYTE:
		{
			G_Parm2 = aNextByte;

			switch(G_MidiEvent)
			{
				case MIDI_NOTE_OFF:
				{
					if(G_LastNote == G_Parm1)
					{
						UEZToneGeneratorPlayToneContinuous(
							G_MIDI_tg,
							MIDI_NOTE_0FF
						);
					} 
					break;
				}
				case MIDI_NOTE_ON:
				{
					if(G_Parm1 < 128)
					{
						TUInt32 freq = MIDI_HZ(G_MIDINoteFreqTable[G_Parm1]);
						G_LastNote = G_Parm1;
						UEZToneGeneratorPlayToneContinuous(
							G_MIDI_tg,
							freq
						);
					}
					break;
				}
				case MIDI_NOTE_AFTERTOUCH:
					#if MIDI_DEBUG
						printf("Note Aftertouch - Unused\n");
					#endif
					break;
				case MIDI_CONTROLLER:
					#if MIDI_DEBUG
						printf("Controller - Unused\n");
					#endif
					break;
				case MIDI_PITCH_BEND:
					#if MIDI_DEBUG
						printf("Pitch Bend - Unused\n");
					#endif
					break;
				case MIDI_PROGRAM_CHANGE:
					G_Parm1 = aNextByte;	// Only takes 1 parm
					
					#if MIDI_DEBUG
						printf("Program Change - Unused\n", G_MidiEvent);
					#endif
					break;
				case MIDI_CHANNEL_AFTERTOUCH:
					G_Parm1 = aNextByte;	// Only takes 1 parm
					
					#if MIDI_DEBUG
						printf("Channel Aftertouch - Unused\n", G_MidiEvent);
					#endif
					break;
				default:
					G_NextByteType = MIDI_UNKNOWN_BYTE;
					
					#if MIDI_DEBUG
						printf("Unknown Event Type: 0x%01X\n", G_MidiEvent);
					#endif
					break;
			}
			G_NextByteType = MIDI_FIRST_EVENT_BYTE;
			
			#if MIDI_DEBUG
				printf("Parm 1 = %02X\n", G_Parm1);
				printf("Parm 2 = %02X\n", G_Parm2);
			#endif
			
			break;
		}
		case MIDI_META_EVENT_BYTE:
		{
			G_MetaEvent = (T_MIDIMetaEvent)aNextByte;
			G_MetaCounter = 0;

			switch(G_MetaEvent)
			{
				case MIDI_META_END_TRACK:
					#if MIDI_DEBUG
						printf("End of Track\n");
					#endif
					G_NextByteType = MIDI_END_TRACK_BYTE;
					break;
				case MIDI_META_SET_TEMPO:
				case MIDI_META_TEXT:
				case MIDI_META_CPYRI:
				case MIDI_META_TRACK:
				case MIDI_META_INSTR:
				case MIDI_META_LYRICS:
				case MIDI_META_MARKER:
				case MIDI_META_CUE:
				case MIDI_META_SEQUEN_SP:
				case MIDI_META_CHANPRE:			
				case MIDI_META_SMPTE_OS:
				case MIDI_META_TIME_SIG:
				case MIDI_META_KEY_SIG:
					G_NextByteType = MIDI_META_SIZE_BYTE;
					break;
				default:
					G_NextByteType = MIDI_UNKNOWN_BYTE;
					break;
			}
			
			#if MIDI_DEBUG
				printf("Meta Event = %02X\n", aNextByte);
			#endif
			
			break;
		}
		case MIDI_META_SIZE_BYTE:
		{
			G_MetaLength = aNextByte;
			switch(G_MetaEvent)
			{
				case MIDI_META_SET_TEMPO:
					G_NextByteType = MIDI_SET_TEMPO_BYTE;
					G_Tempo = 0;
					break;
				default:
					G_NextByteType = MIDI_META_DATA_BYTE;
			}
			
			#if MIDI_DEBUG
				printf("Meta Size = %02X\n", aNextByte);
			#endif
			
			break;
		}
		case MIDI_META_DATA_BYTE:
		{
			G_MetaCounter++;
			if(G_MetaCounter == G_MetaLength)
				G_NextByteType = MIDI_FIRST_EVENT_BYTE;
			
			#if MIDI_DEBUG
				printf("Meta Data = %c\n", aNextByte);
			#endif
				
			break;
		}
		case MIDI_SET_TEMPO_BYTE:
		{
			G_MetaCounter++;
			G_Tempo <<= 8;
			G_Tempo += aNextByte;
			
			#if MIDI_DEBUG
				printf("Tempo Byte = %02X\n", aNextByte);
			#endif
			
			if(G_MetaCounter == G_MetaLength)
			{
				G_TimeMultiplier = ((MICROSEC_PER_MIN/G_Tempo) * G_TicksPerBeat) / (double)60000; // (beats per minute * ticks per beat) / (60000 milliseconds)
				
				#if MIDI_DEBUG
					printf("Set Tempo = %d bpm\n", G_Tempo);
				#endif
				
				G_NextByteType = MIDI_FIRST_EVENT_BYTE;
			}
									
			break;
		}
		case MIDI_END_TRACK_BYTE:
		{
			break;
		}
		case MIDI_UNKNOWN_BYTE:
		{
			#if MIDI_DEBUG
				printf("Unknown Byte = %02X\n", aNextByte);
			#endif
			break;
		}
		default:
		{
			printf("Unknown Next Byte\n");
			break;
		}
	}
}

void BigE_Int32(TUInt32 *aValue)
{
	TUInt32 bigEValue = 0;
	bigEValue += (*aValue & 0xFF000000) >> 24;
	bigEValue += (*aValue & 0x00FF0000) >> 8;
	bigEValue += (*aValue & 0x0000FF00) << 8;
	bigEValue += (*aValue & 0x000000FF) << 24;
	*aValue = bigEValue;
}

void BigE_Int16(TUInt16 *aValue)
{
	TUInt16 bigEValue = 0;
	bigEValue += (*aValue & 0xFF00) >> 8;
	bigEValue += (*aValue & 0x00FF) << 8;
	*aValue = bigEValue;
}
/** @} */

/*-------------------------------------------------------------------------*
 * End of File:  MIDIPlayer.c
 *-------------------------------------------------------------------------*/
