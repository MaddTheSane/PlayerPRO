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
#ifndef __MED_H_
#define __MED_H_

#pragma pack(push, 2)

typedef struct MMD0 {
	ULONG   id;
	ULONG   modlen;
	ULONG   MMD0songP;              // struct MMD0song *song;
	UWORD   psecnum;        /* for the player routine, MMD2 only */
	UWORD   pseq;           /*  "   "   "   "    */
	ULONG   MMD0BlockPP;    // struct MMD0Block **blockarr;
	ULONG   reserved1;
	ULONG   InstrHdrPP;             // struct InstrHdr **smplarr;
	ULONG   reserved2;
	ULONG   MMD0expP;               // struct MMD0exp *expdata;
	ULONG   reserved3;
	UWORD   pstate;                 // some data for the player routine */
	UWORD   pblock;
	UWORD   pline;
	UWORD   pseqnum;
	WORD    actplayline;
	UBYTE   counter;
	UBYTE   extra_songs;    /* number of songs - 1 */
} MMD0;                                         /* length = 52 bytes */


typedef struct MMD0sample {
	UWORD rep,replen;       /* offs: 0(s), 2(s) */
	UBYTE midich;           /* offs: 4(s) */
	UBYTE midipreset;       /* offs: 5(s) */
	UBYTE svol;                     /* offs: 6(s) */
	BYTE strans;            /* offs: 7(s) */
} MMD0sample;


typedef struct MMD0song {
	MMD0sample sample[63];  /* 63 * 8 bytes = 504 bytes */
	UWORD   numblocks;      /* offs: 504 */
	UWORD   songlen;        /* offs: 506 */
	UBYTE   playseq[256];   /* offs: 508 */
	UWORD   deftempo;       /* offs: 764 */
	BYTE    playtransp;     /* offs: 766 */
	UBYTE   flags;          /* offs: 767 */
	UBYTE   flags2;         /* offs: 768 */
	UBYTE   tempo2;         /* offs: 769 */
	UBYTE   trkvol[16];     /* offs: 770 */
	UBYTE   mastervol;      /* offs: 786 */
	UBYTE   numsamples;     /* offs: 787 */
} MMD0song;                             /* length = 788 bytes */


typedef struct MMD0NOTE{
	UBYTE a,b,c;
} MMD0NOTE;


typedef struct MMD1NOTE{
	UBYTE a,b,c,d;
} MMD1NOTE;


typedef struct InstrHdr {
	ULONG   length;
	WORD    type;
	/* Followed by actual data */
} InstrHdr;

#pragma pack(pop)

#endif
