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

#ifndef WIN32
//Windows Defines
typedef int16_t		WORD;
typedef uint16_t	UWORD;
#if !defined(__COREFOUNDATION_CFPLUGINCOM__)
typedef int32_t		HRESULT;
typedef uint32_t	ULONG;
#endif
typedef void*		LPVOID;
typedef int32_t		LONG;

typedef uint16_t	UINT;
#ifndef __OBJC__
typedef bool		BOOL;
#endif
typedef uint32_t	DWORD;
typedef uint16_t	USHORT;
typedef int16_t		SHORT;
typedef MADFourChar	FOURCC;
typedef int8_t		BYTE;
#endif

typedef uint8_t		UBYTE;

#include "MED.h"

struct MEDInfo {
	MMD0 		*mh;
	MMD0song 	*ms;
	ULONG 		*ba;
	
	MMD0NOTE 	*mmd0pat;
	MMD1NOTE 	*mmd1pat;
	char		*theMEDRead;
};


#define d0note(row,col) medInfo->mmd0pat[(row*(UWORD) theMAD->header->numChn)+col]
#define d1note(row,col) medInfo->mmd1pat[(row*(UWORD) theMAD->header->numChn)+col]

#define READMEDFILE(dst, size)	{memcpy(dst, medInfo->theMEDRead, size);	medInfo->theMEDRead += (long) size;}

static BOOL MED_Init(MADDriverSettings *init, struct MEDInfo *medInfo)
{
	medInfo->mh = NULL;
	medInfo->ms = NULL;
	medInfo->ba = NULL;		// blockarr
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
	
	if (medInfo->mmd0pat != NULL) {
		free(medInfo->mmd0pat);
		medInfo->mmd0pat = NULL;
	}
	if (medInfo->mmd1pat != NULL) {
		free(medInfo->mmd1pat);
		medInfo->mmd1pat = NULL;
	}
}

static void EffectCvt(UBYTE eff, UBYTE dat, Cmd *aCmd)
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
	UBYTE a,b,c,d,inst,note,eff,dat;
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
	UBYTE 		a,b,c,inst,note,eff,dat/*, temp*/;
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
	int		t, row /*,col*/;
	UWORD	numtracks, numlines, maxlines=0;
	char	tC;
	int		x;
	
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
	
	for (t = 0 ; t < theMAD->header->numPat; t++)
	{
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
	int		t,row,col;
	UWORD	numtracks,numlines,maxlines=0,track=0;
	int		x;
	
	// first, scan patterns to see how many channels are used
	
	for (t = 0 ; t < theMAD->header->numPat; t++)
	{
		medInfo->theMEDRead = theMED + medInfo->ba[t];
		READMEDFILE(&numtracks, sizeof(short));
		READMEDFILE(&numlines, sizeof(short));
		
		if (numtracks > theMAD->header->numChn) theMAD->header->numChn = numtracks;
		if (numlines  > maxlines) maxlines = numlines;
		
		if (numlines > 999)
		{
			//DebugStr("\pCan't load patterns > 999 rows");
			return -1;
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
	int			t, i;
	ULONG		sa[64];
	InstrHdr	s;
	int			inOutCount;
	
	medInfo->theMEDRead = theMED;
	
	/*********************/
	/** READ MED HEADER **/
	/*********************/
	
	READMEDFILE(medInfo->mh, sizeof(MMD0));
	
	/**************************/
	/** READ MMD0song struct **/
	/**************************/
	
	medInfo->theMEDRead = theMED + medInfo->mh->MMD0songP;
	if (MEDSize < (sizeof(MMD0) + medInfo->mh->MMD0songP)) {
		return MADIncompatibleFile;
	}
	READMEDFILE(medInfo->ms, sizeof(MMD0song));
	
	/***************************/
	/** READ SamplePtr struct **/
	/***************************/
	
	medInfo->theMEDRead = theMED + medInfo->mh->InstrHdrPP;
	READMEDFILE(sa, sizeof(ULONG)*medInfo->ms->numsamples);
	
	/***************************/
	/**    BLOCK PTR ARRAY    **/
	/***************************/
	
	medInfo->ba = (ULONG*) calloc(medInfo->ms->numblocks * sizeof(ULONG), 1);
	if (medInfo->ba == NULL) return MADNeedMemory;
	
	medInfo->theMEDRead = theMED + medInfo->mh->MMD0BlockPP;
	READMEDFILE(medInfo->ba, sizeof(ULONG)*medInfo->ms->numblocks);
	
	
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
	
	strncpy(theMAD->header->infos, "Converted by PlayerPRO MED Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
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
	
	theMAD->fid = (InstrData*) calloc(sizeof(InstrData) * (long) MAXINSTRU, 1);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) calloc(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if (!theMAD->sample) return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for (t = 0 ; t < medInfo->ms->numsamples ; t++)
	{
		medInfo->theMEDRead = theMED + sa[t];
		READMEDFILE(&s, sizeof(InstrHdr));
		
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
			if (curData->loopBeg + curData->loopSize > curData->size)
			{
				curData->loopSize = curData->size - curData->loopBeg;
			}
			curData->vol		= 64;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			
			curData->data 		= (char*)malloc(curData->size);
			if (curData->data == NULL) return MADNeedMemory;
			
			READMEDFILE(curData->data, curData->size);
		}
		else theMAD->fid[t].numSamples = 0;
	}
	
	/***************************/
	/**       PATTERNS        **/
	/***************************/
	
	switch(medInfo->mh->id)
	{
		case 'MMD0':
			if (LoadMMD0Patterns(theMAD, theMED, init, medInfo) != MADNoErr)
				return MADUnknownErr;
			break;
			
		case 'MMD1':
			if (LoadMMD1Patterns(theMAD, theMED, init, medInfo) != MADNoErr)
				return MADUnknownErr;
			break;
	}
	
	return MADNoErr;
}

static MADErr TestMEDFile(char* AlienFile)
{
	if (memcmp(AlienFile, "MMD0", 4) == 0 && memcmp(AlienFile, "MMD1", 4) == 0)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static MADErr ExtractMEDInfo(MADInfoRec *info, char* theMED, struct MEDInfo *medInfo)
{
	/*long	PatternSize;
	 short	i;
	 short	maxInstru;
	 short	tracksNo;
	 long	inOutCount;*/
	
	medInfo->theMEDRead = theMED;
	
	READMEDFILE(medInfo->mh, sizeof(MMD0));
	
	medInfo->theMEDRead = theMED + medInfo->mh->MMD0songP;
	READMEDFILE(medInfo->ms, sizeof(MMD0song));
	
	info->signature = medInfo->mh->id;
	
	//strcpy(info->internalFileName, "");
	info->internalFileName[0] = '\0';
	
	info->totalPatterns = medInfo->ms->numblocks;
	info->partitionLength = medInfo->ms->songlen;
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
				
				// ** MEMORY Test Start
				AlienFile = malloc(sndSize * 2L);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					free(AlienFile);
					
					AlienFile = malloc(sndSize);
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestMEDFile(AlienFile);
					if (myErr == MADNoErr) {
						myErr = MED_Load(AlienFile,  sndSize, MadFile, init, &medInfo);
					}
					
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else
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
					
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000;	// Read only 5000 first bytes for optimisation
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else {
					iRead(sndSize, AlienFile, iFileRefI);
					myErr = ExtractMEDInfo(info, AlienFile, &medInfo);
					free(AlienFile);
					AlienFile = NULL;
				}
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
