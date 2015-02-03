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

#include "MADDefs.h"

#pragma pack(push, 2)

#define DEFAULT_VOLFADE		300
#define MAXINSTRU			255
#define MAXPOINTER			999
#define MAXTRACK			256
//#define MAXTRACKMULTI		99
#define MADID				'MADK'

#pragma mark Effects ID
	
typedef MADENUM(MADByte, MADEffectID) {
	MADEffectArpeggio		= 0,	//	0x00
	MADEffectDownslide		= 1,	//	0x01
	MADEffectUpslide 		= 2,	//	0x02
	MADEffectPortamento 	= 3,	//	0x03
	MADEffectVibrato 		= 4,	//	0x04
	MADEffectPortaSlide 	= 5,	//	0x05
	MADEffectVibratoSlide	= 6,	//	0x06
	MADEffectNothing 		= 7,	//	0x07
	MADEffectPanning		= 8,	//	0x08
	MADEffectOffset 		= 9,	//	0x09
	MADEffectSlideVolume 	= 10,	//	0x0A
	MADEffectFastSkip		= 11,	//	0x0B
	MADEffectVolume 		= 12,	//	0x0C
	MADEffectSkip			= 13,	//	0x0D
	MADEffectExtended		= 14,	//	0x0E
	MADEffectSpeed			= 15,	//	0x0F
	MADEffectNoteOff 		= 16,	//	0x0G
	MADEffectLoop			= 17,	// 	0x0L
	MADEffectNOffset		= 18	//  0x0O
};

#pragma mark PATTERN DESCRIPTION

/*!
 *	\struct		Cmd
 *	\abstract	command function used in a pattern
 */
typedef struct Cmd {
	/// Instrument number. \c 0x00 is no instrument command
	MADByte	ins;
	/// Note, see table. \c 0xFF is no note command
	MADByte note;
	/// Effect command
	MADEffectID cmd;
	/// Effect argument
	MADByte arg;
	/// Volume of the effect. \c 0xFF is no volume command
	MADByte	vol;
	/// Unused, kept in for future use and/or padding.
	MADByte	unused;
} Cmd;

typedef MADENUM(MADFourChar, PatternCompression) {
	/// No compression
	PatternCompressionNone = 'NONE',
	/// MAD1 compression
	PatternCompressionMAD1 = 'MAD1'
};

/*!
 *	@struct		PatHeader
 *	@abstract	Pattern header
 */
typedef struct PatHeader {
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
} PatHeader;

/*!
 *	@struct		PatData
 *	@abstract	data structure: header + commands
 *				Pattern = 64 notes to play
 */
typedef struct PatData {
	PatHeader	header;
	Cmd			Cmds[1];
} PatData;

///Internal Pattern Data
///Because Swift is a dumb-dumb about C arrays in structs
typedef struct IntPatData {
	PatHeader	*header;
	Cmd			*Cmds;
} IntPatData;

#pragma mark INSTRUMENT DESCRIPTION

/*!
 *	@enum		MADLoopType
 */
typedef MADENUM(MADByte, MADLoopType) {
	/// Classic looping, starting over from the beginning after reaching the end.
	MADLoopTypeClassic	= 0,
	/// Ping-pong looping, changing the playback direction of the sound when it reaches the beginning or end.
	MADLoopTypePingPong	= 1
};

/*!
 *	@struct		sData
 *	@abstract	Sample Data
 *	@seealso	sData32
 */
typedef struct sData {
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
	/// Relative note
	char			relNote;
	/// Sample name
	char 			name[32];
	/// Is the sample stereo?
	bool			stereo;
	/** 
	 *	The pointer to the sound data.
	 *	@discussion See \c sData32 for more information.
	 */
	char			*data;
} sData;

/*!
 *	@struct		sData32
 *	@abstract	64-bit-safe sample data
 *				Used for file I/O
 *	@discussion	Only use this struct for reading/writing an \c sData
 *				struct from a saved file.<br>
 *				To read a sample data structure's data, it is directly after the \c sData32 struct,
 *				with the length of <code>size</code>.
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
	bool			stereo;
	/// an unsigned 32-bit integer to keep reading/writing sample datas safe.
	uint32_t		data;
} sData32;

/*!
 *	@struct		EnvRec
 *	@abstract	Envelope
 */
typedef struct EnvRec {
	/// position
	short 	pos;
	/// value
	short	val;
} EnvRec;

/// Effect Types?
typedef MADOPTIONS(MADByte, EFType) {
	/// On
	EFTypeOn		= 1 << 0,
	/// Sustain
	EFTypeSustain	= 1 << 1,
	/// Loop
	EFTypeLoop		= 1 << 2,
	/// Note
	EFTypeNote		= 1 << 3
};

#pragma mark Instrument

/// Instrument
typedef struct InstrData
{
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
	
	// MIDI Channel Value
	short	MIDI;
	// 0 Sound output only, 1 MIDI output only, 2 both
	short	MIDIType;
	
	/**/
	
	/// Sample number for all notes
	MADByte	what[96];
	/// Points for volume envelope
	EnvRec 	volEnv[12];
	/// Points for panning envelope
	EnvRec	pannEnv[12];
	/// Points for panning envelope
	EnvRec	pitchEnv[12];		

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
} InstrData;

#pragma mark MAD FILE HEADER DESCRIPTION

#define INFOSSIZE 239

typedef struct FXBus {
	bool	ByPass;
	short	copyId;
	bool	Active;
} FXBus;

/*!
 *	@struct		MADSpec
 *	@abstract	Basic MAD structure
 */
typedef struct MADSpec {
	/// Mad identification
	MADFourChar	MAD;
	/// Music's name
	char 		name[32];
	/// Informations & Author name of the music
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
	bool		XMLinear;
	/// Limit pitch to MOD pitch table
	bool		MODMode;
	/// Show \c infos at startup? true or false
	bool		showCopyright;
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
	bool		globalFXActive;
	
	/// Channel Effect IDs
	int			chanEffect[MAXTRACK][4];
	/// Channel buses
	FXBus		chanBus[MAXTRACK];
} MADSpec;

typedef struct FXSets {
	short		track;
	short		id;
	int			FXID;
	short		noArg;
	float		values[100];
	MADPStr63	name;
} FXSets;	// and then float values

#pragma pack(pop)

#endif
