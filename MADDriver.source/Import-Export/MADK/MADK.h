/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifndef __MADH__
#define __MADH__

#pragma pack(push, 2)

// ***
// ***	PATTERN DESCRIPTION
// ***

/// COMMAND
typedef struct oldCmd {
	MADByte ins;	///< Instrument no	0x00: no ins cmd
	MADByte note;	///< Note, see table	0xFF : no note cmd
	MADByte cmd;	///< Effect cmd
	MADByte arg;	///< Effect argument
	MADByte vol;	///< Volume			0xFF : no volume cmd
	MADByte unused;
} oldCmd;

/// HEADER
typedef struct oldPatHeader {
	/// Length of pattern. Standard is \c 64
	int					size;
	/// Compression mode. No compression is \c 'NONE'
	PatternCompression	compMode;
	/// The name of the pattern
	char				name[32];
	/// Pattern size in bytes
	int					patBytes;
	/// Unused, kept in for future use.
	int					unused2;
} oldPatHeader;

/// DATA STRUCTURE : HEADER + COMMANDS
/// Pattern = 64 notes to play
typedef struct oldPatData {
	oldPatHeader	header;
	oldCmd			Cmds[1];
} oldPatData;

// ***
// ***	INSTRUMENT DESCRIPTION
// ***

/// SAMPLE
typedef struct oldsData {
	/// Sample Length
	int 			size;
	/// The beginning of the loop
	int				loopBeg;
	/// The length of the loop
	int				loopSize;
	/// The base volume
	MADByte			vol;
	/// The sound sample's sample rate
	unsigned short	c2spd;
	/// The loop type, either classic or ping-pong
	MADLoopType		loopType;
	/// The sound sample's amplitude. Currently limited to 8 or 16 bits
	MADByte			amp;
	/// The actual note
	char			realNote;
	/// Sample name
	char 			name[32];
	/// Is the sample stereo?
	MADBool			stereo;
	/// an unsigned 32-bit integer to keep reading/writing sample datas safe.
	uint32_t		data;
} oldsData;

/// Volume Envelope
typedef struct oldEnvRec {
	short pos;	///< pos
	short val;	///< val
} oldEnvRec;

/// INSTRUMENT
typedef struct oldInstrData {
	/// instrument name
	char 	name[32];
	/// Instrument type = 0
	MADByte type;
	/// Instrument number
	MADByte	no;
	
	/// First sample ID in sample list
	short	firstSample;
	/// Number of samples in instrument
	short	numSamples;
	
	/// MIDI Channel Value
	short	MIDI;
	/// 0 Sound output only, 1 MIDI output only, 2 both
	short	MIDIType;
	
	/**/
	
	/// Sample number for all notes
	MADByte	what[96];
	/// Points for volume envelope
	oldEnvRec 	volEnv[12];
	/// Points for panning envelope
	oldEnvRec	pannEnv[12];
	/// Points for panning envelope
	oldEnvRec	pitchEnv[12];

	/// Number of volume points
	MADByte	volSize;
	/// Number of panning points
	MADByte	pannSize;
	/// Number of panning points
	MADByte	pitchSize;
	
	/// Volume sustain point
	MADByte	volSus;
	/// Volume loop start point
	MADByte	volBeg;
	/// Volume loop end point
	MADByte	volEnd;
	
	/// Panning sustain point
	MADByte	pannSus;
	/// Panning loop start point
	MADByte	pannBeg;
	/// Panning loop end point
	MADByte	pannEnd;
	
	/// Pitch sustain point
	MADByte	pitchSus;
	/// Pitch loop start point
	MADByte	pitchBeg;
	/// Pitch loop end point
	MADByte	pitchEnd;
	
	/// Volume type
	EFType	volType;
	/// Panning type
	EFType	pannType;
	
	/// Volume fadeout
	unsigned short	volFade;
	
	MADByte	vibDepth;
	MADByte	vibRate;
} oldInstrData;

typedef struct oldFXBus {
	MADBool	ByPass;
	short	copyId;
	MADBool	Active;
} oldFXBus;

// ***
// ***	MAD FILE HEADER DESCRIPTION
// ***

#define INFOSSIZE 239

typedef struct oldMADSpec {
	/// Mad identification
	MADFourChar	MAD;
	/// Music's name
	char 		name[32];
	/// Informations & author name of the music.
	/// Format is Mac OS Roman unless it begins with the
	/// UTF-8 BOM.
	char		infos[INFOSSIZE];
	/// General panning
	MADByte		generalPan;
	/// Number of channels for multichannel
	MADByte		MultiChanNo;
	/// MultiChannel per tracks?
	MADByte		MultiChan;
	/// New Pitch
	int			EPitch;
	/// New Speed
	int			ESpeed;
	/// Linear pitch table?
	MADBool		XMLinear;
	/// Limit pitch to MOD pitch table
	MADBool		MODMode;
	/// Show \c infos at startup? true or false
	MADBool		showCopyright;
	/// General pitch
	MADByte		generalPitch;
	/// General speed
	MADByte		generalSpeed;
	/// General vlume
	MADByte		generalVol;
	/// Patterns number
	MADByte		numPat;
	/// Channels number
	MADByte		numChn;
	/// Partition length
	MADByte		numPointers;
	/// Instruments number
	MADByte		numInstru;
	/// Samples number
	MADByte		numSamples;
	/// Partition : Patterns ID List
	MADByte		oPointers[MAXPOINTER];
	/// Default speed
	short		speed;
	/// Default tempo
	short		tempo;
	/// Channel settings, from 0 to 256
	MADByte		chanPan[MAXTRACK];
	/// Channel Volume, from 0 to 64
	MADByte		chanVol[MAXTRACK];
	
	/// Global Effects IDs
	int			globalEffect[10];
	/// Global Effects active?
	MADBool		globalFXActive;
	
	/// Channel Effect IDs
	int			chanEffect[MAXTRACK][4];
	/// Channel buses
	oldFXBus	chanBus[MAXTRACK];
} oldMADSpec;

#pragma pack(pop)

#endif
