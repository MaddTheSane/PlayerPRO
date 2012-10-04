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

#ifndef __MADFG__
#define __MADFG__

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

struct oldPatHeader {
	long	PatternSize;			// Length of pattern: standard = 64
	OSType	CompressionMode;		// Compression mode, none = 'NONE'
	char	PatternName[ 20];
	long	PatBytes;				// Pattern Size in Bytes
	long	unused2;
};

struct Command {
	Byte	InstrumentNo;		// Instrument no
	Byte 	AmigaPeriod;		// Note, see table
	Byte 	EffectCmd;			// Effect cmd
	Byte 	EffectArg;			// Effect arg
};

struct MusicPattern
{								// Pattern = 64 notes to play
	struct oldPatHeader	header;
	struct Command	Commands[];
};

struct FileInstrData {
	char 	Filename[ 32];		// Instrument's filename
	long 	insSize;			// Sample length
	Byte 	fineTune;
	Byte 	volume;				// Base volume
	short	CompCode;			// Compression Code, 0 = nothing, M3 = MAC3, M6 = MAC6
	short	freq;				// Base frequence, simple, double, quadruple
	Byte	amplitude;			// 8 or 16 bits
	long	loopStart;			// LoopStart
	long	loopLenght;			// LoopLength
};

struct oldMADSpec
{
	OSType	MADIdentification;		// Mad Identification: MADG in version 2.0
	char 	NameSignature[ 32];		// Music's name
	struct 	FileInstrData fid[ 64];	// 64 instruments descriptor
	Byte	PatMax;
	Byte 	numPointers;			// Patterns number
	Byte	oPointers[128];			// Patterns list
	Byte	Tracks;					// Tracks number
};
typedef struct oldMADSpec oldMADSpec;

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif

#endif
