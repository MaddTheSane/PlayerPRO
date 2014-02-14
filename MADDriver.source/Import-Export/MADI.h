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

// COMMAND
typedef struct oldCmd {
	Byte ins;	// Instrument no	0x00: no ins cmd
	Byte note;	// Note, see table	0xFF : no note cmd
	Byte cmd;	// Effect cmd
	Byte arg;	// Effect argument
	Byte vol;	// Volume			0xFF : no volume cmd
	Byte unused;
} oldCmd;

// HEADER
typedef struct oldPatHeader {
	SInt32	size;		// Length of pattern: standard = 64
	OSType	compMode;	// Compression mode, none = 'NONE'
	char	name[32];
	SInt32	patBytes;	// Pattern Size in Bytes
	SInt32	unused2;
} oldPatHeader;

// DATA STRUCTURE : HEADER + COMMANDS
// Pattern = 64 notes to play
typedef struct oldPatData {
	oldPatHeader	header;
	oldCmd			Cmds[1];
} oldPatData;

// ***
// ***	INSTRUMENT DESCRIPTION
// ***

// SAMPLE
typedef struct oldsData {
	int				size;		// Sample length
	int				loopBeg;	// LoopStart
	int				loopSize;	// LoopLength
	Byte 			vol;		// Base volume
	unsigned short	c2spd;		// c2spd
	Byte			loopType;
	Byte			amp;		// 8 or 16 bits
	char			relNote;
	char 			name[32];	// Sample name
	Byte			stereo;		// Stereo
	UInt32			data;		// Used only in memory, not in files
} oldsData;

// Volume Envelope
typedef struct oldEnvRec {
	short pos;	// pos
	short val;	// val
} oldEnvRec;

// INSTRUMENT
typedef struct oldInstrData {
	char		name[32];		// instrument name
	Byte		type;			// Instrument type = 0
	Byte		no;				// Instrument number
	
	short		firstSample;	// First sample ID in sample list
	short		numSamples;		// Number of samples in instrument
	
	/**/
	
	Byte		what[96];		// Sample number for all notes
	oldEnvRec 	volEnv[12];		// Points for volume envelope
	oldEnvRec	pannEnv[12];	// Points for panning envelope
	
	// ENVELOPPES PLUS LONGUES !!!!!!!
	Byte		volSize;		// Number of volume points
	Byte		pannSize;		// Number of panning points
	
	Byte		volSus;			// Volume sustain point
	Byte		volBeg;			// Volume loop start point
	Byte		volEnd;			// Volume loop end point
	
	Byte		pannSus;		// Panning sustain point
	Byte		pannBeg;		// Panning loop start point
	Byte		pannEnd;		// Panning loop end point
	
	Byte		volType;		// Volume type: bit 0: On; 1: Sustain; 2: Loop
	Byte		pannType;		// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short volFade;		// Volume fadeout
	
	Byte		vibDepth;
	Byte		vibRate;
} oldInstrData;

// ***
// ***	MAD FILE HEADER DESCRIPTION
// ***

#define INFOSSIZE 239

typedef struct oldMADSpec {
	OSType	MAD;					// Mad Identification
	char	name[32];				// Music's name
	char	infos[INFOSSIZE];		// Informations & Author name of the music
	Byte	generalPan;				// General Panning
	Byte	MultiChanNo;			// Number of chan for multichannel
	Byte	MultiChan;				// MultiChannel per tracks?
	int		EPitch;					// New Pitch
	int		ESpeed;					// New Speed
	Byte	XMLinear;				// Linear picth table?
	Byte	MODMode;				// Limit pitch to MOD pitch table
	Byte	showCopyright;			// Show infos at startup? true or false
	Byte	generalPitch;			// General Pitch
	Byte	generalSpeed;			// General Speed
	Byte	generalVol;				// Software general volume
	Byte	numPat;					// Patterns number
	Byte	numChn;					// Channels number
	Byte	numPointers;			// Partition length
	Byte	numInstru;				// Instruments number
	Byte	numSamples;				// Samples number
	Byte	oPointers[MAXPOINTER];	// Partition : Patterns ID List
	short	speed;					// Default speed
	short	tempo;					// Default tempo
	Byte	chanPan[MAXTRACK];		// Channel settings, from 0 to 256
	Byte	chanVol[MAXTRACK];		// Channel Volume, from 0 to 64
} oldMADSpec;

#pragma pack(pop)

#endif
