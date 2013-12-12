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
#include <CoreFoundation/CFString.h>

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

typedef struct Cmd							// COMMAND
{
	Byte	ins;					// Instrument no		0x00: no ins cmd
	Byte 	note;					// Note, see table		0xFF : no note cmd
	Byte 	cmd;					// Effect cmd
	Byte 	arg;					// Effect argument
	Byte	vol;					// Volume				0xFF : no volume cmd
	Byte	unused;
} Cmd;

typedef struct PatHeader					// HEADER
{
	SInt32	size;					// Length of pattern: standard = 64
	OSType	compMode;				// Compression mode, none = 'NONE'
	char	name[32];
	SInt32	patBytes;				// Pattern Size in Bytes
	SInt32	unused2;
} PatHeader;

typedef struct PatData						// DATA STRUCTURE : HEADER + COMMANDS
{									// Pattern = 64 notes to play
	PatHeader	header;
	Cmd			Cmds[1];
} PatData;


// ***	
// ***	INSTRUMENT DESCRIPTION
// ***	


typedef struct sData								// SAMPLE
{
	SInt32 				size;				// Sample length
	SInt32				loopBeg;			// LoopStart
	SInt32				loopSize;			// LoopLength
	Byte 				vol;				// Base volume
	unsigned short		c2spd;				// c2spd
	Byte				loopType;
	Byte				amp;				// 8 or 16 bits
	char				relNote;
	char 				name[32];			// Sample name
	Byte				stereo;				// Stereo
	Ptr					data;				// Used only in memory, not in files
} sData;

//64-bit safe sample structure
//Used for file i/o
typedef struct sData32								// SAMPLE
{
	SInt32 			size;				// Sample length
	SInt32			loopBeg;			// LoopStart
	SInt32			loopSize;			// LoopLength
	Byte 			vol;				// Base volume
	unsigned short	c2spd;				// c2spd
	Byte			loopType;
	Byte			amp;				// 8 or 16 bits
	char			relNote;
	char 			name[32];			// Sample name
	Byte			stereo;				// Stereo
	UInt32			data;				// Used only in memory, not in files
} sData32;

enum
{
	eClassicLoop	= 0,
	ePingPongLoop	= 1
};


typedef struct EnvRec				// Volume Enveloppe
{
	short 	pos;				// pos
	short	val;				// val
} EnvRec;

typedef struct InstrData				// INSTRUMENT
{
	char 	name[32];			// instrument name
	Byte 	type;				// Instrument type = 0
	Byte	no;					// Instrument number
	
	short	firstSample;		// First sample ID in sample list
	short	numSamples;			// Number of samples in instrument
	
	short	MIDI;				// MIDI Channel Value

	short	MIDIType;			// 0 Sound output only, 1 MIDI output only, 2 both
	
	/**/
	
	Byte	what[96];			// Sample number for all notes
	EnvRec 	volEnv[12];			// Points for volume envelope
	EnvRec	pannEnv[12];		// Points for panning envelope
	EnvRec	pitchEnv[12];		// Points for panning envelope

	Byte	volSize;			// Number of volume points
	Byte	pannSize;			// Number of panning points
	Byte	pitchSize;			// Number of panning points
	
	Byte	volSus;				// Volume sustain point
	Byte	volBeg;				// Volume loop start point
	Byte	volEnd;				// Volume loop end point
	
	Byte	pannSus;			// Panning sustain point
	Byte	pannBeg;			// Panning loop start point
	Byte	pannEnd;			// Panning loop end point
	
	Byte	pitchSus;			// Pitch sustain point
	Byte	pitchBeg;			// Pitch loop start point
	Byte	pitchEnd;			// Pitch loop end point
	
	Byte	volType;			// Volume type: bit 0: On; 1: Sustain; 2: Loop
	Byte	pannType;			// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short	volFade;	// Volume fadeout
	
	Byte	vibDepth;
	Byte	vibRate;
} InstrData;


enum
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
	Boolean		ByPass;
	short		copyId;
	Boolean		Active;
	
} FXBus;

typedef struct MADSpec
{
	OSType		MAD;						// Mad Identification
	char 		name[32];					// Music's name
	char		infos[INFOSSIZE];			// Informations & Author name of the music
	Byte		generalPan;					// General Panning
	Byte		MultiChanNo;				// Number of chan for multichannel
	Byte		MultiChan;					// MultiChannel per tracks?
	SInt32		EPitch;						// New Pitch
	SInt32		ESpeed;						// New Speed
	Byte		XMLinear;					// Linear picth table?
	Byte		MODMode;					// Limit pitch to MOD pitch table
	Byte		showCopyright;				// Show infos at startup? true or false
	Byte		generalPitch;				// General Pitch
	Byte		generalSpeed;				// General Speed
	Byte		generalVol;					// Software general volume
	Byte		numPat;						// Patterns number
	Byte		numChn;						// Channels number
	Byte 		numPointers;				// Partition length
	Byte		numInstru;					// Instruments number
	Byte		numSamples;					// Samples number
	Byte		oPointers[MAXPOINTER];		// Partition : Patterns ID List
	short		speed;						// Default speed
	short		tempo;						// Default tempo
	Byte		chanPan[MAXTRACK];			// Channel settings, from 0 to 256
	Byte		chanVol[MAXTRACK];			// Channel Volume, from 0 to 64
	
	SInt32		globalEffect[10];			// Global Effects IDs
	Boolean		globalFXActive;				// Global FX Active?
	
	SInt32		chanEffect[MAXTRACK][4];	// Channel Effect IDs
	FXBus		chanBus[MAXTRACK];
} MADSpec;

typedef struct FXSets
{
	short	track;
	short	id;
	SInt32	FXID;
	short	noArg;
	float	values[100];
	Str63	name;
} FXSets;	// and then float values


#pragma pack(pop)

#endif
