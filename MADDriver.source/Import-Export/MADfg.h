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

#pragma pack(push, 2)

struct oldPatHeader {
	int		PatternSize;		// Length of pattern: standard = 64
	OSType	CompressionMode;	// Compression mode, none = 'NONE'
	char	PatternName[20];
	int		PatBytes;			// Pattern Size in Bytes
	int		unused2;
};

struct Command {
	Byte InstrumentNo;	// Instrument no
	Byte AmigaPeriod;	// Note, see table
	Byte EffectCmd;		// Effect cmd
	Byte EffectArg;		// Effect arg
};

// Pattern = 64 notes to play
struct MusicPattern {
	struct oldPatHeader	header;
	struct Command		Commands[];
};

struct FileInstrData {
	char	Filename[32];	// Instrument's filename
	int		insSize;		// Sample length
	Byte	fineTune;
	Byte	volume;			// Base volume
	short	CompCode;		// Compression Code, 0 = nothing, M3 = MAC3, M6 = MAC6
	short	freq;			// Base frequence, simple, double, quadruple
	Byte	amplitude;		// 8 or 16 bits
	int		loopStart;		// LoopStart
	int		loopLenght;		// LoopLength
};

typedef struct oldMADSpec {
	OSType	MADIdentification;		// Mad Identification: MADG in version 2.0
	char	NameSignature[32];		// Music's name
	struct	FileInstrData fid[64];	// 64 instruments descriptor
	Byte	PatMax;
	Byte 	numPointers;	// Patterns number
	Byte	oPointers[128];	// Patterns list
	Byte	Tracks;			// Tracks number
} oldMADSpec;

#pragma pack(pop)

#endif
