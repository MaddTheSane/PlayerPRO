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

#if defined(__APPLE__) && !(defined(EMBEDPLUGS) && EMBEDPLUGS)
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "MADFileUtils.h"
#endif

#if defined(EMBEDPLUGS) && EMBEDPLUGS
#include "embeddedPlugs.h"
#endif

#ifdef __BLOCKS__
#include <dispatch/dispatch.h>
#endif

#include "MED.h"
#include <iconv.h>

struct MEDInfo {
	MMD0 		*mh;
	MMD0song 	*ms;
	uint32_t 	*ba;
	MMD0exp		*mi;
	
	MMD0NOTE 	*mmd0pat;
	MMD1NOTE 	*mmd1pat;
	char		*theMEDRead;
};

static bool hasHiBit(const char* theStr, size_t len) {
	int i;
	bool hiBitPresent = false;
	for (i = 0; i <= len; i++) {
		if (theStr[i] < 0) {
			hiBitPresent = true;
			break;
		}
	}
	return hiBitPresent;
}

static void byteSwapMMD0(MMD0 *toSwap) {
	MADBE32(&toSwap->id);
	MADBE32(&toSwap->modlen);
	MADBE32(&toSwap->MMD0songP);
	MADBE16(&toSwap->psecnum);
	MADBE16(&toSwap->pseq);
	MADBE32(&toSwap->MMD0BlockPP);
	MADBE32(&toSwap->reserved1);
	MADBE32(&toSwap->InstrHdrPP);
	MADBE32(&toSwap->reserved2);
	MADBE32(&toSwap->MMD0expP);
	MADBE32(&toSwap->reserved3);
	MADBE16(&toSwap->pstate);
	MADBE16(&toSwap->pblock);
	MADBE16(&toSwap->pline);
	MADBE16(&toSwap->pseqnum);
	MADBE16(&toSwap->actplayline);
}

static void byteSwapMMD0exp(MMD0exp *toSwap) {
	MADBE32(&toSwap->nextmod);
	MADBE32(&toSwap->exp_smp);
	MADBE16(&toSwap->s_ext_entries);
	MADBE16(&toSwap->s_ext_entrsz);
	MADBE32(&toSwap->annotxt);
	MADBE32(&toSwap->annolen);
	MADBE32(&toSwap->iinfo);
	MADBE16(&toSwap->i_ext_entries);
	MADBE16(&toSwap->i_ext_entrsz);
	MADBE32(&toSwap->jumpmask);
	MADBE32(&toSwap->rgbtable);
	MADBE32(&toSwap->n_info);
	MADBE32(&toSwap->songname);
	MADBE32(&toSwap->songnamelen);
	MADBE32(&toSwap->dumps);
	MADBE32(&toSwap->mmdinfo);
	MADBE32(&toSwap->mmdrexx);
	MADBE32(&toSwap->mmdcmd3x);
	MADBE32(&toSwap->trackinfo_ofs);
	MADBE32(&toSwap->effectinfo_ofs);
	MADBE32(&toSwap->tag_end);
}

static void byteSwapMMD0sample(MMD0sample *toSwap) {
	MADBE16(&toSwap->rep);
	MADBE16(&toSwap->replen);
}

static void byteSwapMMD0song(MMD0song *toSwap) {
	int i;
	for (i = 0; i < 63; i++) {
		byteSwapMMD0sample(&toSwap->sample[i]);
	}
	MADBE16(&toSwap->numblocks);
	MADBE16(&toSwap->songlen);
	MADBE16(&toSwap->deftempo);
}

static void byteSwapInstrHdr(InstrHdr *toSwap) {
	MADBE32(&toSwap->length);
	MADBE16(&toSwap->type);
}

#define d0note(row,col) medInfo->mmd0pat[(row*(uint16_t) theMAD->header->numChn)+col]
#define d1note(row,col) medInfo->mmd1pat[(row*(uint16_t) theMAD->header->numChn)+col]

#define READMEDFILE(dst, size)	{memcpy(dst, medInfo->theMEDRead, size);	medInfo->theMEDRead += (long) size;}

static bool MED_Init(MADDriverSettings *init, struct MEDInfo *medInfo)
{
	medInfo->mh = NULL;
	medInfo->ms = NULL;
	medInfo->ba = NULL;		// blockarr
	medInfo->mi = NULL;
	medInfo->mmd0pat = NULL;
	medInfo->mmd1pat = NULL;
	
	if (!(medInfo->mh = (MMD0 *)malloc(sizeof(MMD0))))
		return 0;
	if (!(medInfo->ms = (MMD0song *)malloc(sizeof(MMD0song))))
		return 0;
	return 1;
}

static void MED_Cleanup(struct MEDInfo *medInfo)
{
	if (medInfo->mh != NULL) {
		free(medInfo->mh);
		medInfo->mh = NULL;
	}
	if (medInfo->ms != NULL) {
		free(medInfo->ms);
		medInfo->ms = NULL;
	}
	if (medInfo->ba != NULL) {
		free(medInfo->ba);
		medInfo->ba = NULL;
	}
	if (medInfo->mi != NULL) {
		free(medInfo->mi);
		medInfo->mi = NULL;
	}
	
	if (medInfo->mmd0pat != NULL) {
		free(medInfo->mmd0pat);
		medInfo->mmd0pat = NULL;
	}
	if (medInfo->mmd1pat != NULL) {
		free(medInfo->mmd1pat);
		medInfo->mmd1pat = NULL;
	}
}

static void EffectCvt(uint8_t eff, uint8_t dat, Cmd *aCmd)
{
	switch(eff){
			
			// 0x0 0x1 0x2 0x3 0x4		// PT effects
			
		case 0x5:		// PT vibrato with speed/depth nibbles swapped
			aCmd->cmd = 4;
			aCmd->arg = (dat>>4) | ((dat&0xf)<<4);
			break;
			
		case 0x6:		// not used
		case 0x7:		// not used
		case 0x8:		// midi hold/decay
			break;
			
		case 0x9:
			if(dat<=0x20)
			{
				aCmd->cmd = 0xf;
				aCmd->arg = dat;
			}
			break;
			
			// 0xa 0xb 0xc all PT effects
			
		case 0xd:		// same as PT volslide
			aCmd->cmd = 0xa;
			aCmd->arg = dat;
			break;
			
		case 0xe:		// synth jmp - midi
			break;
			
		case 0xf:
			
			// F00 does patternbreak with med
			
			if(dat==0)
			{
				aCmd->cmd = 0xd;
				aCmd->arg = 0;
			}
			else if(dat<=0xa)
			{
				aCmd->cmd = 0xf;
				aCmd->arg = dat;
			}
			else if(dat<0xf1)
			{
				aCmd->cmd = 0xf;
				aCmd->arg = ((int)dat*125) / 33;
			}
			else if(dat==0xff)
			{
				// stop note
				aCmd->note = 0xFE;
			}
			break;
			
		default:		// all normal PT effects are handled here :)
			aCmd->cmd = eff;
			aCmd->arg = dat;
			break;
	}
}

static void MED_Convert1(short col, short patID, MADMusic *theMAD, struct MEDInfo *medInfo)
{
	int t;
	uint8_t a,b,c,d,inst,note,eff,dat;
	MMD1NOTE *n;
	Cmd	*aCmd;
	
	for(t=0;t<theMAD->partition[patID]->header.size;t++){
		
		n= &d1note(t,col);
		
		a = n->a;
		b = n->b;
		c = n->c;
		d = n->d;
		
		note=a&0x7f;
		inst=b&0x3f;
		eff=c&0xf;
		dat=d;
		
		aCmd = GetMADCommand(t, col, theMAD->partition[patID]);
		
		aCmd->ins 	= inst;
		if(note != 0)
		{
			aCmd->note = note+35;
		}
		else aCmd->note = 0xFF;
		
		aCmd->cmd 	= 0;
		aCmd->arg 	= 0;
		aCmd->vol	= 0xFF;
		
		EffectCvt(eff, dat, aCmd);
	}
}



static void MED_Convert0(short patID, MADMusic *theMAD, struct MEDInfo *medInfo)
{
	int 		t, zz;
	uint8_t 	a,b,c,inst,note,eff,dat/*, temp*/;
	MMD0NOTE 	*n;
	Cmd			*aCmd;
	
	for (t = 0 ; t < theMAD->partition[patID]->header.size; t++)
	{
		for (zz = 0 ; zz < theMAD->header->numChn; zz++)
		{
			n= &d0note(t, zz);
			
			a = n->a;
			b = n->b;
			c = n->c;
			
			note	=		a&0x3f;
			
			a		>>=		6;
			a		=		((a&1)<<1)|(a>>1);
			
			inst	=		(b>>4)|(a<<4);
			eff		=		b&0xf;
			dat		=		c;
			
			aCmd = GetMADCommand(t, zz, theMAD->partition[patID]);
			
			aCmd->ins 	= inst;
			
			if(note != 0)
			{
				aCmd->note = note+35;
			}
			else aCmd->note = 0xFF;
			
			aCmd->cmd 	= 0;
			aCmd->arg 	= 0;
			aCmd->vol	= 0xFF;
			
			EffectCvt(eff, dat, aCmd);
		}
	}
}

static MADErr LoadMMD0Patterns(MADMusic *theMAD, char* theMED, MADDriverSettings *init, struct MEDInfo *medInfo)
{
	int			t, row /*,col*/;
	uint16_t	numtracks, numlines, maxlines=0;
	char		tC;
	int			x;
	
	// first, scan patterns to see how many channels are used
	
	for (t = 0; t < theMAD->header->numPat; t++)
	{
		medInfo->theMEDRead = theMED + medInfo->ba[t];
		READMEDFILE(&tC, sizeof(char));	numtracks = tC;
		READMEDFILE(&tC, sizeof(char));	numlines = tC;
		
		if(numtracks > theMAD->header->numChn) theMAD->header->numChn = numtracks;
		if(numlines  > maxlines) maxlines = numlines;
	}
	
	medInfo->mmd0pat = (MMD0NOTE *) calloc(theMAD->header->numChn * (maxlines+1) * sizeof(MMD0NOTE), 1);
	if (medInfo->mmd0pat == NULL) //Debugger();
		return MADNeedMemory;
	
	/* second read: no more mr. nice guy,
	 really read and convert patterns */
	
	for (t = 0 ; t < theMAD->header->numPat; t++) {
		medInfo->theMEDRead = theMED + medInfo->ba[t];
		READMEDFILE(&tC, sizeof(char));	numtracks = tC;
		READMEDFILE(&tC, sizeof(char));	numlines = tC;
		
		numlines++;
		
		theMAD->partition[t] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * numlines * sizeof(Cmd), 1);
		if (theMAD->partition[t] == NULL) return MADNeedMemory;
		
		theMAD->partition[t]->header.size 		= numlines;
		theMAD->partition[t]->header.compMode 	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[t]->header.name[x] = 0;
		
		theMAD->partition[t]->header.patBytes = 0;		theMAD->partition[t]->header.unused2 = 0;
		
		//memset(mmd0pat, 0, of.numchn * maxlines * sizeof(MMD0NOTE));
		
		for (x = 0; x < theMAD->header->numChn * maxlines * sizeof(MMD0NOTE); x++) {
			((char*)medInfo->mmd0pat)[x] = 0;
		}
		
		for (row = 0 ; row < numlines; row++) {
			short ztz;
			
			for (ztz = 0; ztz < numtracks; ztz++)
				READMEDFILE(&d0note(row, ztz), 3);		// sizeof(MMD0NOTE) != 3 probleme compilation alignement
		}
		
		MED_Convert0(t, theMAD, medInfo);
	}
	
	return MADNoErr;
}

static MADErr LoadMMD1Patterns(MADMusic *theMAD, char* theMED, MADDriverSettings *init, struct MEDInfo *medInfo)
{
	int			t,row,col;
	uint16_t	numtracks,numlines,maxlines=0,track=0;
	int			x;
	
	// first, scan patterns to see how many channels are used
	
	for (t = 0 ; t < theMAD->header->numPat; t++)
	{
		medInfo->theMEDRead = theMED + medInfo->ba[t];
		READMEDFILE(&numtracks, sizeof(short));
		READMEDFILE(&numlines, sizeof(short));
		MADBE16(&numtracks);
		MADBE16(&numlines);
		
		if (numtracks > theMAD->header->numChn) theMAD->header->numChn = numtracks;
		if (numlines  > maxlines) maxlines = numlines;
		
		if (numlines > 999) {
			printf("Can't load patterns > 999 rows");
			return MADFileNotSupportedByThisPlug;
		}
	}
	
	medInfo->mmd1pat = (MMD1NOTE *) calloc(theMAD->header->numChn * (maxlines+1) * sizeof(MMD1NOTE), 1);
	if (medInfo->mmd1pat == NULL) //Debugger();
		return MADNeedMemory;
	
	/* second read: no more mr. nice guy,
	 really read and convert patterns */
	
	for (t = 0; t < theMAD->header->numPat; t++){
		
		medInfo->theMEDRead = theMED + medInfo->ba[t];
		READMEDFILE(&numtracks, sizeof(short));
		READMEDFILE(&numlines, sizeof(short));
		MADBE16(&numtracks);
		MADBE16(&numlines);

		medInfo->theMEDRead += 4L;
		
		numlines++;
		
		theMAD->partition[t] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * numlines * sizeof(Cmd), 1);
		if (theMAD->partition[t] == NULL) return MADNeedMemory;
		
		theMAD->partition[t]->header.size 		= numlines;
		theMAD->partition[t]->header.compMode 	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[t]->header.name[x] = 0;
		
		theMAD->partition[t]->header.patBytes = 0;		theMAD->partition[t]->header.unused2 = 0;
		
		
		
		for (x = 0; x < theMAD->header->numChn * maxlines * sizeof(MMD0NOTE); x++)
		{
			((char*) medInfo->mmd1pat)[x] = 0;
		}
		
		for (row = 0 ; row < numlines ; row++)
		{
			READMEDFILE(&d1note(row, 0), sizeof(MMD1NOTE) * numtracks);
		}
		
		for(col=0;col< theMAD->header->numChn;col++)
		{
			MED_Convert1(col, t, theMAD, medInfo);
			track++;
		}
	}
	
	return MADNoErr;
}

static MADErr MED_Load(char* theMED, long MEDSize, MADMusic *theMAD, MADDriverSettings *init, struct MEDInfo *medInfo)
{
	//TODO: get file name and instruments names
	int			t, i;
	uint32_t	sa[64];
	InstrHdr	s;
	int			inOutCount;
	MADErr		iErr = noErr;
	
	medInfo->theMEDRead = theMED;
	
	/*********************/
	/** READ MED HEADER **/
	/*********************/
	
	READMEDFILE(medInfo->mh, sizeof(MMD0));
	byteSwapMMD0(medInfo->mh);
	
	/*************************/
	/** READ MMD0exp struct **/
	/*************************/

	if (medInfo->mh->MMD0expP && MEDSize >= (sizeof(MMD0exp) + medInfo->mh->MMD0expP)) {
		medInfo->mi = malloc(sizeof(MMD0exp));
		medInfo->theMEDRead = theMED + medInfo->mh->MMD0expP;
		READMEDFILE(medInfo->mi, sizeof(MMD0exp));
		byteSwapMMD0exp(medInfo->mi);
	}
	
	/**************************/
	/** READ MMD0song struct **/
	/**************************/
	
	medInfo->theMEDRead = theMED + medInfo->mh->MMD0songP;
	if (MEDSize < (sizeof(MMD0) + medInfo->mh->MMD0songP)) {
		return MADIncompatibleFile;
	}
	READMEDFILE(medInfo->ms, sizeof(MMD0song));
	byteSwapMMD0song(medInfo->ms);
	
	/***************************/
	/** READ SamplePtr struct **/
	/***************************/
	
	medInfo->theMEDRead = theMED + medInfo->mh->InstrHdrPP;
	READMEDFILE(sa, sizeof(uint32_t)*medInfo->ms->numsamples);
	for (i = 0; i < medInfo->ms->numsamples; i++) {
		MADBE32(&sa[i]);
	}
	
	/***************************/
	/**    BLOCK PTR ARRAY    **/
	/***************************/
	
	medInfo->ba = (uint32_t*) calloc(medInfo->ms->numblocks, sizeof(uint32_t));
	if (medInfo->ba == NULL) return MADNeedMemory;
	
	medInfo->theMEDRead = theMED + medInfo->mh->MMD0BlockPP;
	READMEDFILE(medInfo->ba, sizeof(uint32_t)*medInfo->ms->numblocks);
	for (i = 0; i < medInfo->ms->numblocks; i++) {
		MADBE32(&medInfo->ba[i]);
	}
	
	
	/********************/
	/********************/
	/** MAD ALLOCATION **/
	/********************/
	/********************/
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*) calloc(inOutCount, 1);
	if (theMAD->header == NULL) //DebugStr("\pHeader: I NEED MEMORY !!! NOW !");
		return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	if (medInfo->mi && (medInfo->mi->songname != 0 && medInfo->mi->songnamelen != 0 && MEDSize >= medInfo->mi->songname + medInfo->mi->songnamelen)) {
		char * songName = calloc(medInfo->mi->songnamelen+1, sizeof(char));
		medInfo->theMEDRead = theMED + medInfo->mi->songname;
		READMEDFILE(songName, medInfo->mi->songnamelen);
		if (hasHiBit(songName, medInfo->mi->songnamelen)) {
			//todo: iconv
			strncpy(theMAD->header->name, songName, sizeof(theMAD->header->name));
		} else {
			strncpy(theMAD->header->name, songName, sizeof(theMAD->header->name));
		}
		free(songName);
	} else {
		theMAD->header->name[0] = 0;
	}

	if (medInfo->mi && (medInfo->mi->annotxt != 0 && medInfo->mi->annolen != 0 && MEDSize >= medInfo->mi->annotxt + medInfo->mi->annolen)) {
		char * songInfo = calloc(medInfo->mi->annolen+1, sizeof(char));
		medInfo->theMEDRead = theMED + medInfo->mi->annotxt;
		READMEDFILE(songInfo, medInfo->mi->annolen);
		if (hasHiBit(songInfo, medInfo->mi->annolen)) {
			//todo: iconv
			strncpy(theMAD->header->infos, songInfo, sizeof(theMAD->header->infos));
		} else {
			strncpy(theMAD->header->infos, songInfo, sizeof(theMAD->header->infos));
		}
		free(songInfo);
	} else {
		strncpy(theMAD->header->infos, "Converted by PlayerPRO MED Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	}
	
	theMAD->header->speed			= 	medInfo->ms->tempo2;
	theMAD->header->tempo			=	(medInfo->ms->deftempo * 125) / 33;
	theMAD->header->numChn			=	0;		// will be counted later
	theMAD->header->numPat			=	medInfo->ms->numblocks;
	theMAD->header->numPointers		=	medInfo->ms->songlen;
	if (medInfo->ms->numsamples >= MAXINSTRU) medInfo->ms->numsamples = MAXINSTRU-1;
	
	if (theMAD->header->numPointers > 128) theMAD->header->numPointers = 128;
	
	for (i = 0; i < medInfo->ms->songlen; i++)
	{
		theMAD->header->oPointers[i] = medInfo->ms->playseq[i];
		if (theMAD->header->oPointers[i] >= theMAD->header->numPat)
			theMAD->header->oPointers[i] = theMAD->header->numPat-1;
	}
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK * sizeof(FXSets), 1);
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	for (i = 0; i < MAXTRACK; i++) {
		if (i % 2 == 0) theMAD->header->chanPan[i] = MAX_PANNING/4;
		else theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	/***************************/
	/**      SAMPLES INS      **/
	/***************************/
	
	theMAD->fid = (InstrData*) calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) calloc(sizeof(sData*), (long) MAXINSTRU * (long) MAXSAMPLE);
	if (!theMAD->sample) return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for (t = 0 ; t < medInfo->ms->numsamples ; t++)
	{
		medInfo->theMEDRead = theMED + sa[t];
		READMEDFILE(&s, sizeof(InstrHdr));
		byteSwapInstrHdr(&s);
		
		theMAD->fid[t].type = 0;
		
		if (s.length > 0 && s.length < 0x4d4d4430)
		{
			sData	*curData;
			
			theMAD->fid[t].numSamples = 1;
			theMAD->fid[t].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[t*MAXSAMPLE + 0] = (sData*) calloc(sizeof(sData), 1);
			
			curData->size		= s.length;
			curData->loopBeg 	= medInfo->ms->sample[t].rep<<1;
			curData->loopSize 	= curData->loopBeg + (medInfo->ms->sample[t].replen<<1);
			if (curData->loopBeg + curData->loopSize > curData->size) {
				curData->loopSize = curData->size - curData->loopBeg;
			}
			curData->vol		= 64;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->realNote	= 0;
			
			if (curData->size) {
				curData->data 		= (char*)malloc(curData->size);
				if (curData->data == NULL)
					return MADNeedMemory;
				
				READMEDFILE(curData->data, curData->size);
			}
		}
		else theMAD->fid[t].numSamples = 0;
	}
	
	/***************************/
	/**       PATTERNS        **/
	/***************************/
	
	switch (medInfo->mh->id) {
		case 'MMD0':
			iErr = LoadMMD0Patterns(theMAD, theMED, init, medInfo);
			break;
			
		case 'MMD1':
			iErr = LoadMMD1Patterns(theMAD, theMED, init, medInfo);
			break;
	}
	
	if (medInfo->mi) {
		uint32_t smpinfoex = medInfo->mi->iinfo;
		//uint32_2 trackinfo_ofs = medInfo->mi->trackinfo_ofs
		
		/**************************/
		/**   READ Sample Names  **/
		/**************************/

		if (smpinfoex) {
			uint32_t ientries = medInfo->mi->i_ext_entries;
			uint32_t ientrysz = medInfo->mi->i_ext_entrsz;
			
			if ((ientrysz < 256) && (ientries*ientrysz < MEDSize) && (smpinfoex < MEDSize - ientries*ientrysz)) {
				const char *psznames = (const char *)(theMED + smpinfoex);
				for (i=0; i<ientries; i++) if (i < medInfo->ms->numsamples) {
					const char *name = (psznames + i * ientrysz);
					if (strlen(name) == 0) {
						continue;
					}
					//hacky, hacky hack.
					if (theMAD->fid[i].numSamples == 0) {
						theMAD->sample[i*MAXSAMPLE + 0] = calloc(1, sizeof(sData));
						
						theMAD->fid[i].numSamples = 1;
					}
					if (hasHiBit(name, ientrysz)) {
						// todo: iconv
						strncpy(theMAD->sample[i*MAXSAMPLE + 0]->name, name, sizeof(theMAD->sample[i*MAXSAMPLE + 0]->name));
					} else {
						strncpy(theMAD->sample[i*MAXSAMPLE + 0]->name, name, sizeof(theMAD->sample[i*MAXSAMPLE + 0]->name));
					}
				}
			}
		}
	}
	
	return iErr;
}

static MADErr TestMEDFile(char* AlienFile)
{
	if (memcmp(AlienFile, "MMD0", 4) == 0 || memcmp(AlienFile, "MMD1", 4) == 0)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static MADErr ExtractMEDInfo(MADInfoRec *info, UNFILE theMED, struct MEDInfo *medInfo)
{
	/*long	PatternSize;
	 short	i;
	 short	maxInstru;
	 short	tracksNo;
	 long	inOutCount;*/
	short totalPats, partLen;
	
#define READMEDFILE2(dst, size)	{iRead(size, dst, theMED);}
	
	//Use fseek and SEEK_SET as workarounds.
	fseek(theMED, 0, SEEK_SET);
	READMEDFILE2(medInfo->mh, sizeof(MMD0));
	byteSwapMMD0(medInfo->mh);
	
	fseek(theMED, medInfo->mh->MMD0songP, SEEK_SET);
	READMEDFILE2(medInfo->ms, sizeof(MMD0song));
	
	if (medInfo->mh->MMD0expP != 0 && info->fileSize >= (sizeof(MMD0exp) + medInfo->mh->MMD0expP)) {
		medInfo->mi = malloc(sizeof(MMD0exp));
		fseek(theMED, medInfo->mh->MMD0expP, SEEK_SET);
		READMEDFILE2(medInfo->mi, sizeof(MMD0exp));
		byteSwapMMD0exp(medInfo->mi);
		
		if (medInfo->mi->songname != 0 && medInfo->mi->songnamelen != 0 && info->fileSize >= medInfo->mi->songname + medInfo->mi->songnamelen) {
			// +1 for null termination, just in case.
			char *songName = calloc(medInfo->mi->songnamelen + 1, sizeof(char));
			fseek(theMED, medInfo->mi->songname, SEEK_SET);
			READMEDFILE2(songName, medInfo->mi->songnamelen);
			if (hasHiBit(songName, medInfo->mi->songnamelen)) {
				//todo: iconv
				strncpy(info->internalFileName, songName, sizeof(info->formatDescription));
			} else {
				strncpy(info->internalFileName, songName, sizeof(info->formatDescription));
			}
			free(songName);
		} else {
			info->internalFileName[0] = '\0';
		}
	} else {
		info->internalFileName[0] = '\0';
	}
	
	info->signature = medInfo->mh->id;
	
	totalPats = medInfo->ms->numblocks;
	partLen = medInfo->ms->songlen;
	MADBE16(&totalPats);
	MADBE16(&partLen);
	info->totalPatterns = totalPats;
	info->partitionLength = partLen;
	info->totalInstruments = medInfo->ms->numsamples;
	info->totalTracks = 0;
	
	strncpy(info->formatDescription, "MED Plug", sizeof(info->formatDescription));
	
	return MADNoErr;
}

#ifndef _MAC_H

EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strncpy(p->type, 		"MED ", sizeof(p->type));
	strncpy(p->MenuName, 	"MED Files", sizeof(p->type));
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif


#if defined(EMBEDPLUGS) && EMBEDPLUGS
MADErr mainMED(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr = MADNoErr;
	void*	AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	struct MEDInfo medInfo;
	
	if(MED_Init(init, &medInfo) == 0) {
		MED_Cleanup(&medInfo);
		return MADNeedMemory;
	}
	
	switch(order) {
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestMEDFile(AlienFile);
					if (myErr == MADNoErr) {
						myErr = MED_Load(AlienFile, sndSize, MadFile, init, &medInfo);
					}
					
					free(AlienFile); AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024;
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					iRead(sndSize, AlienFile, iFileRefI);
					myErr = TestMEDFile(AlienFile);
					
					free(AlienFile); AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugInfo:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				myErr = ExtractMEDInfo(info, iFileRefI, &medInfo);
				
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	MED_Cleanup(&medInfo);
	
	return myErr;
}
