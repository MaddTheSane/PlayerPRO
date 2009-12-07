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

//#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
//#endif


struct PasByte {
	Byte	aByte[ 3];
};
typedef struct PasByte PasByte;

struct PatCmd {
	unsigned short AmigaPeriod : 6;
	unsigned short Instru : 6;
	unsigned short Volume : 4;
	unsigned short EffectCmd : 4;
	unsigned short EffectArg : 4;
};

struct PatSix {
		struct PatCmd Cmds[ 64][ 8];
};

struct	SampleInfo	{
	//	char	InstruFilename[ 13];
		unsigned	short	length;
		unsigned	short	loopStart;
		unsigned	short	loopEnd;
};
typedef		struct SampleInfo	SampleInfo;

struct	SixSixNine	{
		short		marker;
		char		message[ 108];
		Byte		NOS;
		Byte		NOP;
		Byte		loopOrder;
		Byte		orderList[ 0x80];
		Byte		tempoList[ 0x80];
		Byte		breakList[ 0x80];
		SampleInfo	fid[];
};
typedef		struct SixSixNine	SixSixNine;

struct	Partition669	{
		SixSixNine		*theFile;
		SampleInfo		*sampleInfo;
		Ptr				samplePtr[ 128];
		Ptr				patternPtr[ 128];
};
typedef		struct Partition669	Partition669;

//#if defined(powerc) || defined(__powerc)
#pragma options align=reset
//#endif
