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
	MADByte ins;	///< Instrument no		0x00: no ins cmd
	MADByte note;	///< Note, see table		0xFF : no note cmd
	MADByte cmd;	///< Effect cmd
	MADByte arg;	///< Effect argument
	MADByte vol;	///< Volume				0xFF : no volume cmd
	MADByte unused;
} oldCmd;

/// HEADER
typedef struct oldPatHeader {
	int			size;		///< Length of pattern: standard = 64
	MADFourChar	compMode;	///< Compression mode, none = \c 'NONE'
	char		name[32];
	int			patBytes;	///< Pattern Size in Bytes
	int			unused2;
} oldPatHeader;

/// DATA STRUCTURE : HEADER + COMMANDS
/// Pattern = 64 notes to play
typedef struct oldPatData {
	oldPatHeader	header;
	oldCmd			Cmds[];
} oldPatData;

// ***
// ***	INSTRUMENT DESCRIPTION
// ***

/// SAMPLE
typedef struct oldsData {
	int				size;		///< Sample length
	int				loopBeg;	///< LoopStart
	int				loopSize;	///< LoopLength
	MADByte			vol;		///< Base volume
	unsigned short	c2spd;		///< c2spd
	MADByte			loopType;
	MADByte			amp;		///< 8 or 16 bits
	MADByte			panning;
	char			realNote;
	char			name[32];	///< Sample name
	uint32_t		data;		///< Used only in memory, not in files
} oldsData;

/// Volume Envelope
typedef struct oldEnvRec {
	short pos;	///< pos
	short val;	///< val
} oldEnvRec;

/// INSTRUMENT
typedef struct oldInstrData {
	char	name[32];			///< instrument name
	MADByte	type;				///< Instrument type = 0

	short	numSamples;		///< Number of samples in instrument

	/**/

	MADByte		what[96];		///< Sample number for all notes
	oldEnvRec	volEnv[12];		///< Points for volume envelope
	oldEnvRec	pannEnv[12];	///< Points for panning envelope
	
	MADByte	volSize;	///< Number of volume points
	MADByte	pannSize;	///< Number of panning points
	
	MADByte	volSus;		///< Volume sustain point
	MADByte	volBeg;		///< Volume loop start point
	MADByte	volEnd;		///< Volume loop end point
	
	MADByte	pannSus;	///< Panning sustain point
	MADByte	pannBeg;	///< Panning loop start point
	MADByte	pannEnd;	///< Panning loop end point

	MADByte	volType;	///< Volume type: bit 0: On; 1: Sustain; 2: Loop
	MADByte	pannType;	///< Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short volFade;	///< Volume fadeout
	
	MADByte	vibDepth;
	MADByte	vibRate;
} oldInstrData;

// ***	
// ***	MAD FILE HEADER DESCRIPTION
// ***	

typedef struct oldMADSpec {
	MADFourChar		MAD;			///< Mad Identification: MADG in version 2.0
	char			name[32];		///< Music's name
	char			infos[256];		///< Informations & Author name of the music
	MADByte			numPat;			///< Patterns number
	MADByte			numChn;			///< Channels number
	MADByte			numPointers;	///< Partition length
	MADByte			oPointers[256];	///< Partition : Patterns ID List
	short			speed;			///< Default speed
	short			tempo;			///< Default tempo
	oldInstrData	fid[64];		///< Instruments description
} oldMADSpec;

#pragma pack(pop)

#endif
