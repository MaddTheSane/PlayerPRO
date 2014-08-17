/********************						***********************/
//
//	Player PRO 5.0x -- MAD Music Driver Definition -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:		(+41 22) 346 11 97
//	PHONE: 		(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

/*!
 *	@header		MAD.h
 *	@abstract	The main header that defines the MADK structures
 */

#ifndef __MADI__
#define __MADI__

#include "PPDefs.h"

#pragma pack(push, 2)

#define DEFAULT_VOLFADE		300
#define MAXINSTRU			255
#define MAXPOINTER			999
#define MAXTRACK			256
//#define MAXTRACKMULTI		99
static const MADFourChar MADID = 'MADK';

// ***	
// ***	PATTERN DESCRIPTION
// ***	


/*!
 *	\struct		Cmd
 *	\abstract	command function used in a pattern
 *	\var		ins
 *				Instrument number \c 0x00 is no instrument command
 *	\var		note
 *				Note, see table. \c 0xFF is no note command
 *	\var		cmd
 *				effect command
 *	\var		arg
 *				Effect argument
 *	\var		vol
 *				Volume of the effect. \c 0xFF is no volume command
 *	\var		unused
 *				Unused, kept in for future use.
 */
typedef struct Cmd {
	MADByte	ins;
	MADByte note;
	MADByte cmd;
	MADByte arg;
	MADByte	vol;
	MADByte	unused;
} Cmd;

/*!
 *	@struct		PatHeader
 *	@abstract	Pattern header
 *	@var		size
 *				Length of pattern. Standard is 64
 *	@var		compMode
 *				Compression mode. No compression is \c'NONE'
 *	@var		name
 *				The name of the pattern
 *	@var		patBytes
 *				Pattern size in bytes
 *	@var		unused
 *				Unused, kept in for future use.
 */
typedef struct PatHeader {
	int			size;
	MADFourChar	compMode;
	char		name[32];
	int			patBytes;
	int			unused2;
} PatHeader;

typedef struct PatData		// DATA STRUCTURE : HEADER + COMMANDS
{							// Pattern = 64 notes to play
	PatHeader	header;
	Cmd			Cmds[1];
} PatData;

//Internal Pattern Data
//Because Swift is a dumb-dumb about C arrays in structs
typedef struct IntPatData {
	PatHeader	header;
	Cmd			*Cmds;
} IntPatData;

// ***
// ***	INSTRUMENT DESCRIPTION
// ***

/*!
 *	@enum		MADLoopType
 *	@constant	eClassicLoop
 *				Classic looping, starting over from the beginning after reaching the end.
 *	@constant	ePingPongLoop
 *				Ping-pong looping, changing the playback direction of the sound when it reaches the beginning or end.
 *
 */
typedef MADENUM(MADByte, MADLoopType)
{
	eClassicLoop	= 0,
	ePingPongLoop	= 1
};

/*!
 *	@struct		sData
 *	@abstract	Sample Data
 *	@var		size
 *				Sample Length
 *	@var		loopBeg
 *				The beginning of the loop
 *	@var		loopSize
 *				The length of the loop
 *	@var		vol
 *				The base volume
 *	@var		c2spd
 *				the sound sample's sample rate
 *	@var		loopType
 *				The loop type, either classic or ping-pong
 *	@var		amp
 *				the sound sample's amplitude. Currently limited to 8 or 16 bits
 *	@var		relNote
 *				Relative note
 *	@var		name
 *				Sample name
 *	@var		stereo
 *				Is the sample stereo?
 *	@var		data
 *				the data that represents the actual sound data. See <code>sData32</code>
 *				for more information.
 *	@seealso	sData32
 */
typedef struct sData {
	int 			size;
	int				loopBeg;
	int				loopSize;
	MADByte			vol;
	unsigned short	c2spd;
	MADLoopType		loopType;
	MADByte			amp;
	char			relNote;
	char 			name[32];
	MADBool			stereo;
	char			*data;
} sData;

/*!
 *	@struct		sData32
 *	@abstract	64-bit-safe sample data
 *				Used for file I/O
 *	@discussion	Only use this struct for reading/writing an <code>sData</code>
 *				struct from a saved file.
 *				To read a sample data structure's data, it is directly after the <code>sData32</code> struct,
 *				with the length of <code>size</code>.
 *	@var		data
 *				an unsigned 32-bit integer to keep reading/writing sample datas safe.
 *	@seealso	sData
 */
typedef struct sData32 {
	int				size;
	int				loopBeg;
	int				loopSize;
	MADByte			vol;
	unsigned short	c2spd;
	MADLoopType		loopType;
	MADByte			amp;
	char			relNote;
	char 			name[32];
	MADBool			stereo;
	uint32_t		data;
} sData32;

/*!
 *	@struct		EnvRec
 *	@abstract	Volume Envelope
 *	@var		pos
 *				position
 *	@var		val
 *				value
 */
typedef struct EnvRec {
	short 	pos;
	short	val;
} EnvRec;

typedef MADOPTIONS(MADByte, EFType) {
	EFON		= 1 << 0,
	EFSUSTAIN	= 1 << 1,
	EFLOOP		= 1 << 2,
	EFNOTE		= 1 << 3
};

typedef struct InstrData		// INSTRUMENT
{
	char 	name[32];			// instrument name
	MADByte type;				// Instrument type = 0
	MADByte	no;					// Instrument number
	
	short	firstSample;		// First sample ID in sample list
	short	numSamples;			// Number of samples in instrument
	
	short	MIDI;				// MIDI Channel Value

	short	MIDIType;			// 0 Sound output only, 1 MIDI output only, 2 both
	
	/**/
	
	MADByte	what[96];			// Sample number for all notes
	EnvRec 	volEnv[12];			// Points for volume envelope
	EnvRec	pannEnv[12];		// Points for panning envelope
	EnvRec	pitchEnv[12];		// Points for panning envelope

	MADByte	volSize;			// Number of volume points
	MADByte	pannSize;			// Number of panning points
	MADByte	pitchSize;			// Number of panning points
	
	MADByte	volSus;				// Volume sustain point
	MADByte	volBeg;				// Volume loop start point
	MADByte	volEnd;				// Volume loop end point
	
	MADByte	pannSus;			// Panning sustain point
	MADByte	pannBeg;			// Panning loop start point
	MADByte	pannEnd;			// Panning loop end point
	
	MADByte	pitchSus;			// Pitch sustain point
	MADByte	pitchBeg;			// Pitch loop start point
	MADByte	pitchEnd;			// Pitch loop end point
	
	EFType	volType;			// Volume type: bit 0: On; 1: Sustain; 2: Loop
	EFType	pannType;			// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short	volFade;	// Volume fadeout
	
	MADByte	vibDepth;
	MADByte	vibRate;
} InstrData;

// ***
// ***	MAD FILE HEADER DESCRIPTION
// ***

#define INFOSSIZE 239

typedef struct FXBus {
	MADBool	ByPass;
	short	copyId;
	MADBool	Active;
} FXBus;

typedef struct MADSpec
{
	MADFourChar	MAD;						// Mad Identification
	char 		name[32];					// Music's name
	char		infos[INFOSSIZE];			// Informations & Author name of the music
	MADByte		generalPan;					// General Panning
	MADByte		MultiChanNo;				// Number of chan for multichannel
	MADByte		MultiChan;					// MultiChannel per tracks?
	int			EPitch;						// New Pitch
	int			ESpeed;						// New Speed
	MADByte		XMLinear;					// Linear picth table?
	MADByte		MODMode;					// Limit pitch to MOD pitch table
	MADByte		showCopyright;				// Show infos at startup? true or false
	MADByte		generalPitch;				// General Pitch
	MADByte		generalSpeed;				// General Speed
	MADByte		generalVol;					// Software general volume
	MADByte		numPat;						// Patterns number
	MADByte		numChn;						// Channels number
	MADByte		numPointers;				// Partition length
	MADByte		numInstru;					// Instruments number
	MADByte		numSamples;					// Samples number
	MADByte		oPointers[MAXPOINTER];		// Partition : Patterns ID List
	short		speed;						// Default speed
	short		tempo;						// Default tempo
	MADByte		chanPan[MAXTRACK];			// Channel settings, from 0 to 256
	MADByte		chanVol[MAXTRACK];			// Channel Volume, from 0 to 64
	
	int			globalEffect[10];			// Global Effects IDs
	MADBool		globalFXActive;				// Global FX Active?
	
	int			chanEffect[MAXTRACK][4];	// Channel Effect IDs
	FXBus		chanBus[MAXTRACK];
} MADSpec;

typedef struct FXSets {
	short			track;
	short			id;
	int				FXID;
	short			noArg;
	float			values[100];
	unsigned char	name[64];
} FXSets;	// and then float values

#pragma pack(pop)

#endif
