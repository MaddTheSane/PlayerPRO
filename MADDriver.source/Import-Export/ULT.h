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

typedef struct ULTEvt
{
	Byte			note;
	Byte			ins;
	Byte			eff1;
	Byte			eff2;
	short		var;
} ULTEvt;

typedef struct ULTIns
{
	char		name[ 32];
	char		dosname[ 12];
	SInt32		loopStart;
	SInt32		loopEnd;
	SInt32		sizeStart;
	SInt32		sizeEnd;
	Byte		volume;
	Byte		Bidi;
	short	finetune;
} ULTIns;

typedef struct ULTForm
{
	char				ID[ 15];
	char             		name[ 32];
	char             		reserved;
} ULTForm;

typedef struct ULTSuite
{
	Byte  			NOS;
	ULTIns			*ins;
	
	//-----------
	
	Byte				pattSeq[ 256];
	Byte				NOC;
	Byte				NOP;
} ULTSuite;

#pragma pack(pop)
