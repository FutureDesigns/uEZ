/**
 *  @file   MIDIPlayer.h
 *  @brief  Code for parsing and playing basic midi files
 *
 *  Code for parsing and playing basic midi files
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2011 Future Designs, Inc.
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
 
#ifndef MIDI_PLAYER_H
#define MIDI_PLAYER_H

TUInt32 playMIDITask(T_uezTask aMyTask, void *aParameters);
T_uezError playMIDI(char * aFilename, TBool *aRun);

typedef struct {
    char * iFilename;
	TBool * iRun;
	T_uezSemaphore *iSem;
} T_midiPlayParameters;

typedef struct {
	char IChunkID[4];		/// Always "MThd"
	TUInt32 IChunkSize;		/// Always 6 (0x00000006)
	TUInt16 IFormatType;	/// 0: one track that contains all MIDI events
							///		including song title, time signature, tempo and music events
							/// 1: two or more tracks
							/// 2: sort of a combination of the other two types
	TUInt16 INumTracks;		/// Simply the number of tracks
	TUInt16 ITimeDivision;	/// First bit (Mask: 0x8000)
							///		0: ticks per beat (or quarter note) Command values range: 48 to 960
							/// Next 15 bits (Mask: 0x7FFF)
							///		Value as described by the first bit

							/// First bit (Mask: 0x8000)
							///		1: frames per second
							/// Next 7 bits (Mask: 0x7F00)
							///		Number of SMPTE frames (can be 24, 25, 29, or 30)
							/// Next 8 bits (Mask: 0x00FF)
							///		Clock ticks per frame
} T_MidiHeader;


typedef struct {	
	char IChunkID[4];		/// Always "MTrd"
	TUInt32 IChunkSize;		/// Number of bytes contained in the track
} T_MidiTrackHeader;


typedef enum 
{
	MIDI_NOTE_OFF			= 0x8,	/// The Note ON Event is used to signal when a MIDI key is pressed
	MIDI_NOTE_ON			= 0x9,	/// The Note OFF Event is used to signal when a MIDI key is pressed
	MIDI_NOTE_AFTERTOUCH	= 0xA,	/// Indicates a pressure change on one of the currently pressed MIDI keys
	MIDI_CONTROLLER			= 0xB,	/// Signals the change in a MIDI channels state.
	MIDI_PROGRAM_CHANGE		= 0xC,	/// Indicates which instrument/patch should be played on the MIDI channel.
	MIDI_CHANNEL_AFTERTOUCH	= 0xD,	/// Similar to note after touch except effects all keys currently pressed
	MIDI_PITCH_BEND			= 0xE,	/// Similar to a controller event, except that it is a unique MIDI Channel 
									/// 		Event that has two bytes to describe it's value
	MIDI_META_AND_SYS		= 0xF	/// Events that are not to be sent or received over a MIDI port
} T_MIDIEvent;

typedef enum
{
	MIDI_META_TEXT		= 0x01,		/// Meta Text
	MIDI_META_CPYRI		= 0x02,		/// Meta Copyright
	MIDI_META_TRACK		= 0x03,		/// Meta Track Name
	MIDI_META_INSTR		= 0x04,		/// Meta Instrument Name
	MIDI_META_LYRICS	= 0x05,		/// Meta Lyrics
	MIDI_META_MARKER	= 0x06,		/// Meta Marker
	MIDI_META_CUE		= 0x07,		/// Meta Cue Point
	MIDI_META_CHANPRE	= 0x20,		/// Meta Channel Prefix
	MIDI_META_END_TRACK	= 0x2F,		/// Meta End Track
	MIDI_META_SET_TEMPO	= 0x51,		/// Meta Set Tempo
	MIDI_META_SMPTE_OS	= 0x54,		/// Meta SMPTE Offset
	MIDI_META_TIME_SIG	= 0x58,		/// Meta Time Signature
	MIDI_META_KEY_SIG	= 0x59,		/// Meta Key Signature
	MIDI_META_SEQUEN_SP	= 0x7F		/// Meta Sequencer Specific
} T_MIDIMetaEvent;


typedef enum 
{
	MIDI_NEW_TRACK,
	MIDI_FIRST_EVENT_BYTE,
	MIDI_DTIME_BYTE,	/// Delta Time Byte
	MIDI_ETYPE_BYTE,	/// Event Type Byte
	MIDI_PARM1_BYTE,
	MIDI_PARM2_BYTE,
	MIDI_META_EVENT_BYTE,
	MIDI_META_SIZE_BYTE,
	MIDI_META_DATA_BYTE,
	MIDI_END_TRACK_BYTE,
	MIDI_SET_TEMPO_BYTE,
	MIDI_UNKNOWN_BYTE
} T_MIDINextByteType;

#endif