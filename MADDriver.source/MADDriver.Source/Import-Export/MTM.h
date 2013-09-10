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

struct	MTMTrack
{
	//FIXME: is this little-endian safe?
	unsigned short pitch : 6;
	unsigned short instru : 6;
	unsigned short EffectCmd : 4;
	unsigned short EffectArg : 8;
};

struct Instru
{
	char	name[22];
	long	size;
	long	loopBegin;
	long	loopEnd;
	char	fineTune;
	char	volume;
	char	sampleSize;
};

struct MTMDef
{
	char			Id[ 3];
	char			vers;
	char 			songname[ 20];
	unsigned short	tracks;
	char			patNo;
	char			positionNo;
	unsigned short	comments;	
	char			NOS;
	char			attribute;
	char			beats;
	char			trackback;
	char			voicePos[ 32];
};
typedef struct MTMDef MTMDef;

#pragma pack(pop)
