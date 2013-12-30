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

#ifndef __OKTA_H_
#define __OKTA_H_

typedef struct sectheader
{
	SInt32	name;
	SInt32	length;
} sectheader;

typedef struct OktaInstru
{
      char				name[20];
      SInt32			length;
      unsigned short 	repeat;
      unsigned short 	replen;
      char 				pad1;
      unsigned char 	vol;
      short 			pad2;
} OktaInstru;

typedef struct OktaPattern
{
	Byte	b1;
	Byte	b2;
	Byte	b3;
	Byte	b4;
} OktaPattern;

typedef struct OktaHeader
{
	char		magic[12];
	SInt32		noChannel;
	SInt32		chanFlag;
	SInt32		SampleDirectory;
	SInt32		SampleLen;
	int			splitted[4];
	
	int		samp_count;
	int		linesize;
	int		speed;
  	int		slen;
  	int		plen;

	Ptr		pbod[128];
	int		pbodlen[128];
       
	unsigned char *patt;
	int		pointer;
	int		patty;
	int		nextpt;
	int		actspeed;

	int		note[8];
	int		vol[8];
	int		per;
} OktaHeader;

static short FreqOktaTable[ 40] =
		{
			0x0358,0x0328,0x02FB,0x02D0,0x02A7,0x0281,0x025D,0x023B,0x021B,0x01FD,0x01E0,0x01C5,
			0x01AC,0x0194,0x017D,0x0168,0x0154,0x0141,0x012F,0x011E,0x010E,0x00FE,0x00F0,0x00E3,
			0x00D6,0x00CA,0x00BF,0x00B4,0x00AA,0x00A0,0x0097,0x008F,0x0087,0x007F,0x0078,0x0071
		};

#endif
