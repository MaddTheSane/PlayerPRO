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

typedef struct ULTEvt {
	MADByte	note;
	MADByte	ins;
	MADByte	eff1;
	MADByte	eff2;
	short	var;
} ULTEvt;

typedef struct ULTIns {
	char	name[32];
	char	dosname[12];
	int		loopStart;
	int		loopEnd;
	int		sizeStart;
	int		sizeEnd;
	MADByte	volume;
	MADByte	Bidi;
	short	finetune;
} ULTIns;

typedef struct ULTForm {
	char ID[15];
	char name[32];
	char reserved;
} ULTForm;

typedef struct ULTSuite {
	MADByte	NOS;
	ULTIns	*ins;
	MADByte	pattSeq[256];
	MADByte	NOC;
	MADByte	NOP;
} ULTSuite;

#pragma pack(pop)
