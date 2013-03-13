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

#if defined(powerc) || defined(__powerc)
#pragma options align=mac68k
#endif

typedef struct s3mform
{
	char             		name[28];
	char             		sig1;
	char             		type;
	char				sig2[2];
	short  			ordernum;
	short   			insnum;
	short      			patnum;
	short      			flags;
	short     			cwtv;
	short      			ffv;
	char             		s3msig[4];
	unsigned char 		mastervol;
	unsigned char    	initialspeed;
	unsigned char    	initialtempo;
	unsigned char    	mastermul;
	unsigned char		ultraclick;
	unsigned char		pantable;
	char            	 	sig3[10];
	unsigned char    	chanset[32];
	unsigned char  		*orders;
	unsigned short		*parapins;
	unsigned short		*parappat;
	struct s3minsform	*insdata;
} s3mform;

typedef struct s3mPatform
{
	Byte	note;		// hi = oct, lo = note
	Byte	intru;
	Byte	vol;
	Byte	SpecialCom;
	Byte	ComArg;
} s3mPatform;

typedef struct s3minsform
{
	unsigned char		instype;
	unsigned char		insdosname[12];
	unsigned char		memsegh;
	unsigned short		memsegl;
	unsigned long		inslength;
	unsigned long		insloopbeg;
	unsigned long		insloopend;
	unsigned char		insvol;
	unsigned char		insdsk;
	unsigned char		inspack;
	unsigned char		insflags;
	unsigned long		c2spd;
	unsigned char		inssig2[4];
	unsigned short		insgvspos;
	unsigned short		insint512;
	unsigned long		insintlastused;
	unsigned char		insname[28];
	unsigned char		inssig[ 4];
} s3minsform;
               
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif
