/********************						***********************/
//
//	Player PRO 6.0 - DRIVER SOURCE CODE -
//
//	Library Version 6.0
//
//	COPYRIGHT C.W. "Madd The Sane" Betts, 2016
//
//	Uses code from libmikmod,
//		copyright (c) 1998, 1999, 2000, 2001, 2002 Miodrag Vallat and others
//
/********************						***********************/

#include <stdint.h>
#include <PlayerPROCore/MADDefs.h>

typedef struct ITHEADER {
	char		songname[26];
	MADByte		blank01[2];
	uint16_t	ordnum;
	uint16_t	insnum;
	uint16_t	smpnum;
	uint16_t	patnum;
	uint16_t	cwt;		/* Created with tracker (y.xx = 0x0yxx) */
	uint16_t	cmwt;		/* Compatible with tracker ver > than val. */
	uint16_t	flags;
	uint16_t	special;	/* bit 0 set = song message attached */
	MADByte		globvol;
	MADByte		mixvol;		/* mixing volume [ignored] */
	MADByte		initspeed;
	MADByte		inittempo;
	MADByte		pansep;		/* panning separation between channels */
	MADByte		zerobyte;
	uint16_t	msglength;
	uint32_t	msgoffset;
	MADByte		blank02[4];
	MADByte		pantable[64];
	MADByte		voltable[64];
} ITHEADER;

/* sample information */
typedef struct ITSAMPLE {
	char		filename[12];
	MADByte		zerobyte;
	MADByte		globvol;
	MADByte		flag;
	MADByte		volume;
	MADByte		panning;
	char		sampname[28];
	uint16_t	convert;	/* sample conversion flag */
	uint32_t	length;
	uint32_t	loopbeg;
	uint32_t	loopend;
	uint32_t	c5spd;
	uint32_t	susbegin;
	uint32_t	susend;
	uint32_t	sampoffset;
	MADByte		vibspeed;
	MADByte		vibdepth;
	MADByte		vibrate;
	MADByte		vibwave;	/* 0=sine, 1=rampdown, 2=square, 3=random (speed ignored) */
} ITSAMPLE;

/* instrument information */

#define ITENVCNT 25
#define ITNOTECNT 120
typedef struct ITINSTHEADER {
	uint32_t	size;			/* (dword) Instrument size */
	char		filename[12];	/* (char) Instrument filename */
	MADByte		zerobyte;		/* (byte) Instrument type (always 0) */
	MADByte		volflg;
	MADByte		volpts;
	MADByte		volbeg;			/* (byte) Volume loop start (node) */
	MADByte		volend;			/* (byte) Volume loop end (node) */
	MADByte		volsusbeg;		/* (byte) Volume sustain begin (node) */
	MADByte		volsusend;		/* (byte) Volume Sustain end (node) */
	MADByte		panflg;
	MADByte		panpts;
	MADByte		panbeg;			/* (byte) channel loop start (node) */
	MADByte		panend;			/* (byte) channel loop end (node) */
	MADByte		pansusbeg;		/* (byte) channel sustain begin (node) */
	MADByte		pansusend;		/* (byte) channel Sustain end (node) */
	MADByte		pitflg;
	MADByte		pitpts;
	MADByte		pitbeg;			/* (byte) pitch loop start (node) */
	MADByte		pitend;			/* (byte) pitch loop end (node) */
	MADByte		pitsusbeg;		/* (byte) pitch sustain begin (node) */
	MADByte		pitsusend;		/* (byte) pitch Sustain end (node) */
	uint16_t	blank;
	MADByte		globvol;
	MADByte		chanpan;
	uint16_t	fadeout;		/* Envelope end / NNA volume fadeout */
	MADByte		dnc;			/* Duplicate note check */
	MADByte		dca;			/* Duplicate check action */
	MADByte		dct;			/* Duplicate check type */
	MADByte		nna;			/* New Note Action [0,1,2,3] */
	uint16_t	trkvers;		/* tracker version used to save [files only] */
	MADByte		ppsep;			/* Pitch-pan Separation */
	MADByte		ppcenter;		/* Pitch-pan Center */
	MADByte		rvolvar;		/* random volume varations */
	MADByte		rpanvar;		/* random panning varations */
	uint16_t	numsmp;			/* Number of samples in instrument [files only] */
	char		name[26];		/* Instrument name */
	MADByte		blank01[6];
	uint16_t	samptable[ITNOTECNT];/* sample for each note [note / samp pairs] */
	MADByte		volenv[200];	     /* volume envelope (IT 1.x stuff) */
	MADByte		oldvoltick[ITENVCNT];/* volume tick position (IT 1.x stuff) */
	MADByte		volnode[ITENVCNT];   /* amplitude of volume nodes */
	uint16_t	voltick[ITENVCNT];   /* tick value of volume nodes */
	int8_t		pannode[ITENVCNT];   /* panenv - node points */
	uint16_t	pantick[ITENVCNT];   /* tick value of panning nodes */
	int8_t		pitnode[ITENVCNT];   /* pitchenv - node points */
	uint16_t	pittick[ITENVCNT];   /* tick value of pitch nodes */
} ITINSTHEADER;

/* unpacked note */

typedef struct ITNOTE {
	MADByte note,ins,volpan,cmd,inf;
} ITNOTE;
