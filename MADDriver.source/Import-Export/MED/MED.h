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

#include <stdint.h>
#include <stdlib.h>

#pragma pack(push, 2)

typedef struct MMD0 {
	uint32_t id;
	uint32_t modlen;
	uint32_t MMD0songP;		///< struct MMD0song *song;
	uint16_t psecnum;		/*!< for the player routine, MMD2 only */
	uint16_t pseq;			/*!<  "   "   "   "    */
	uint32_t MMD0BlockPP;	//!< struct MMD0Block **blockarr;
	uint32_t reserved1;
	uint32_t InstrHdrPP;	//!< struct InstrHdr **smplarr;
	uint32_t reserved2;
	uint32_t MMD0expP;		//!< struct MMD0exp *expdata;
	uint32_t reserved3;
	uint16_t pstate;		///< some data for the player routine */
	uint16_t pblock;
	uint16_t pline;
	uint16_t pseqnum;
	int16_t  actplayline;
	uint8_t  counter;
	uint8_t  extra_songs;	/*!< number of songs - 1 */
} MMD0;						/* length = 52 bytes */


typedef struct MMD0sample {
	uint16_t rep,replen;	/*!< offs: 0(s), 2(s) */
	uint8_t midich;			/*!< offs: 4(s) */
	uint8_t midipreset;		/*!< offs: 5(s) */
	uint8_t svol;			/*!< offs: 6(s) */
	int8_t strans;			/*!< offs: 7(s) */
} MMD0sample;


typedef struct MMD0song {
	MMD0sample sample[63];		/*!< 63 * 8 bytes = 504 bytes */
	uint16_t   numblocks;		/*!< offs: 504 */
	uint16_t   songlen;			/*!< offs: 506 */
	uint8_t    playseq[256];	/*!< offs: 508 */
	uint16_t   deftempo;		/*!< offs: 764 */
	int8_t	   playtransp;		/*!< offs: 766 */
	uint8_t    flags;			/*!< offs: 767 */
	uint8_t    flags2;			/*!< offs: 768 */
	uint8_t    tempo2;			/*!< offs: 769 */
	uint8_t    trkvol[16];		/*!< offs: 770 */
	uint8_t    mastervol;		/*!< offs: 786 */
	uint8_t    numsamples;		/*!< offs: 787 */
} MMD0song;						/* length = 788 bytes */


typedef struct MMD0NOTE{
	uint8_t a,b,c;
} MMD0NOTE;


typedef struct MMD1NOTE{
	uint8_t a,b,c,d;
} MMD1NOTE;


typedef struct InstrHdr {
	uint32_t length;
	int16_t	 type;
	/* Followed by actual data */
} InstrHdr;


typedef struct MMD0exp {
	uint32_t nextmod;			//!< File offset of next Hdr
	uint32_t exp_smp;			//!< Pointer to extra instrument data
	uint16_t s_ext_entries;		//!< Number of extra instrument entries
	uint16_t s_ext_entrsz;		//!< Size of extra instrument data
	uint32_t annotxt;
	uint32_t annolen;
	uint32_t iinfo;				//!< Instrument names
	uint16_t i_ext_entries;
	uint16_t i_ext_entrsz;
	uint32_t jumpmask;
	uint32_t rgbtable;
	uint8_t  channelsplit[4];	//!< Only used if 8ch_conv (extra channel for every nonzero entry)
	uint32_t n_info;
	uint32_t songname;			//!< Song name
	uint32_t songnamelen;
	uint32_t dumps;
	uint32_t mmdinfo;
	uint32_t mmdrexx;
	uint32_t mmdcmd3x;
	uint32_t trackinfo_ofs;		//!< ptr to song->numtracks ptrs to tag lists
	uint32_t effectinfo_ofs;	//!< ptr to group ptrs
	uint32_t tag_end;
} MMD0exp;

#pragma pack(pop)

#endif
