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

#ifndef __MADI__
#define __MADI__

#ifdef __GNUC__
#define HAS_LONG_LONG 1
#define HAS_LONG_DOUBLE 1
#endif

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP extern
#endif
#else
#define PPEXPORT extern __attribute__((visibility("default")))
#ifdef __cplusplus
#define EXP extern "C" __attribute__((visibility("default")))
#else
#define EXP PPEXPORT
#endif
#endif
#endif

#ifdef _MSC_VER
#define HAS_LONG_LONG 1
//MSVC's long double datatype is the same size as a regular double
#undef HAS_LONG_DOUBLE
#endif

#if TARGET_OS_IPHONE
//iOS also has long double the same size as double
#undef HAS_LONG_DOUBLE
#endif

#if !defined(__BIG_ENDIAN__) && defined(WORDS_BIGENDIAN)
#define __BIG_ENDIAN__ 1
#endif

//////////////////////////////////////////////////////////////////////
#if defined(__APPLE__)			// MACINTOSH
#define _MAC_H

//////////////////////////////////////////////////////////////////////
#else 			// WIN32 - 95/NT

#ifndef DEPRECATED_ATTRIBUTE
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#else
#define DEPRECATED_ATTRIBUTE
#endif
#endif

#ifdef LINUX
#define __UNIX__ 1
#endif

#endif
//////////////////////////////////////////////////////////////////////

#include "PPDefs.h"

#ifdef WIN32
#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP extern
#endif
#else
#ifdef __cplusplus
#define EXP extern "C" __declspec(dllexport)
#else
#define EXP extern __declspec(dllexport)
#endif
#ifdef BUILDINGPPRO
#define PPEXPORT extern __declspec(dllexport)
#else
#define PPEXPORT extern __declspec(dllimport)
#ifdef _MSC_VER
#pragma comment(lib, "PlayerPROCore.lib")
#endif
#endif
#endif
#endif

//Final checks
#ifndef EXP
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP PPEXPORT
#endif
#endif

#if !defined(_MAC_H)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum {
	noErr = 0
};

#endif

//////////////////////////////////////////////////////////////////////

#pragma pack(push, 2)

#define DEFAULT_VOLFADE		300
#define MAXINSTRU			255
#define MAXPOINTER			999
#define MAXTRACK			256
//#define MAXTRACKMULTI		99
#define MADID				'MADK'

// ***	
// ***	PATTERN DESCRIPTION
// ***	

typedef struct Cmd			// COMMAND
{
	MADByte	ins;					// Instrument no		0x00: no ins cmd
	MADByte note;					// Note, see table		0xFF : no note cmd
	MADByte cmd;					// Effect cmd
	MADByte arg;					// Effect argument
	MADByte	vol;					// Volume				0xFF : no volume cmd
	MADByte	unused;
} Cmd;

typedef struct PatHeader	// HEADER
{
	int			size;					// Length of pattern: standard = 64
	MADFourChar	compMode;				// Compression mode, none = 'NONE'
	char		name[32];
	int			patBytes;				// Pattern Size in Bytes
	int			unused2;
} PatHeader;

typedef struct PatData		// DATA STRUCTURE : HEADER + COMMANDS
{							// Pattern = 64 notes to play
	PatHeader	header;
	Cmd			Cmds[1];
} PatData;

// ***
// ***	INSTRUMENT DESCRIPTION
// ***

typedef struct sData		// SAMPLE
{
	int 			size;		// Sample length
	int				loopBeg;	// LoopStart
	int				loopSize;	// LoopLength
	MADByte			vol;		// Base volume
	unsigned short	c2spd;		// c2spd
	MADByte			loopType;
	MADByte			amp;		// 8 or 16 bits
	char			relNote;
	char 			name[32];	// Sample name
	MADByte			stereo;		// Stereo
	char			*data;		// Used only in memory, not in files
} sData;

//64-bit safe sample structure
//Used for file i/o
typedef struct sData32
{
	int				size;		// Sample length
	int				loopBeg;	// LoopStart
	int				loopSize;	// LoopLength
	MADByte 			vol;		// Base volume
	unsigned short	c2spd;		// c2spd
	MADByte			loopType;
	MADByte			amp;		// 8 or 16 bits
	char			relNote;
	char 			name[32];	// Sample name
	MADByte			stereo;		// Stereo
	uint32_t		data;		// Used only in memory, not in files
} sData32;

enum
{
	eClassicLoop	= 0,
	ePingPongLoop	= 1
};

typedef struct EnvRec			// Volume Enveloppe
{
	short 	pos;				// pos
	short	val;				// val
} EnvRec;

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
	
	MADByte	volType;			// Volume type: bit 0: On; 1: Sustain; 2: Loop
	MADByte	pannType;			// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short	volFade;	// Volume fadeout
	
	MADByte	vibDepth;
	MADByte	vibRate;
} InstrData;

enum EFTypes
{
	EFON		= 1,
	EFSUSTAIN	= 1 << 1,
	EFLOOP		= 1 << 2,
	EFNOTE		= 1 << 3
};

// ***
// ***	MAD FILE HEADER DESCRIPTION
// ***

#define INFOSSIZE 239

typedef struct FXBus
{
	unsigned char	ByPass;// Boolean
	short			copyId;
	unsigned char	Active; //Boolean
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
	
	int				globalEffect[10];			// Global Effects IDs
	unsigned char	globalFXActive;				// Global FX Active?
	
	int			chanEffect[MAXTRACK][4];	// Channel Effect IDs
	FXBus		chanBus[MAXTRACK];
} MADSpec;

typedef struct FXSets {
	short	track;
	short	id;
	int		FXID;
	short	noArg;
	float	values[100];
	unsigned char	name[64];
} FXSets;	// and then float values

#pragma pack(pop)

#endif
