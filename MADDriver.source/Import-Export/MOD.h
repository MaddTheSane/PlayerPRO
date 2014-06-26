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

#pragma pack(push, 2)

#define MAXPTRS 128

struct MODCom
{
    MADByte a, b, c, d;
};

struct MODPat
{
	struct MODCom Commands[64][4];
};

struct FileInstrDataz
{
	char			Filename[22];
	unsigned short	numWords;
    MADByte			fineTune;
    MADByte			volume;
	unsigned short	loopWord;
	unsigned short	loopWords;
};

typedef struct MODDef
{
	char			NameSignature[20];
	struct			FileInstrDataz fid[31];
    MADByte			numPointers;
    MADByte			maxPointers;
    MADByte			oPointers[MAXPTRS];
	MADFourChar		longFmtSignature;
	struct MODPat	patterns[];
} MODDef;

#pragma pack(pop)
