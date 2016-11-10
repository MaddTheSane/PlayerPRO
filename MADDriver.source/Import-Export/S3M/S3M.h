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

typedef struct s3mform {
	char				name[28];
	char				sig1;
	char				type;
	char				sig2[2];
	short				ordernum;
	short				insnum;
	short				patnum;
	short				flags;
	short				cwtv;
	short				ffv;
	char				s3msig[4];
	unsigned char		mastervol;
	unsigned char		initialspeed;
	unsigned char		initialtempo;
	unsigned char		mastermul;
	unsigned char		ultraclick;
	unsigned char		pantable;
	char				sig3[10];
	unsigned char		chanset[32];
	unsigned char		*orders; //TODO: check for pointer size inconsistencies.
	unsigned short		*parapins;
	unsigned short		*parappat;
	struct s3minsform	*insdata;
} s3mform;

typedef struct s3mPatform {
	MADByte	note;		///< hi = oct, lo = note
	MADByte	intru;
	MADByte	vol;
	MADByte	SpecialCom;
	MADByte	ComArg;
} s3mPatform;

typedef struct s3minsform {
	unsigned char	instype;
	unsigned char	insdosname[12];
	unsigned char	memsegh;
	unsigned short	memsegl;
	unsigned int	inslength;
	unsigned int	insloopbeg;
	unsigned int	insloopend;
	unsigned char	insvol;
	unsigned char	insdsk;
	unsigned char	inspack;
	unsigned char	insflags;
	unsigned int	c2spd;
	unsigned char	inssig2[4];
	unsigned short	insgvspos;
	unsigned short	insint512;
	unsigned int	insintlastused;
	unsigned char	insname[28];
	unsigned char	inssig[4];
} s3minsform;

#pragma pack(pop)
