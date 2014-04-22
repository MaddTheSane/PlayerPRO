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

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

#ifndef WIN32
//Windows Defines
typedef SInt16			WORD;
typedef UInt16			UWORD;
typedef SInt32			HRESULT;
typedef UInt32			ULONG;
typedef void*			LPVOID;
typedef SInt32			LONG;

typedef UInt16			UINT;
typedef Boolean			BOOL;
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef FourCharCode	FOURCC;
typedef SInt8			BYTE;
#endif

typedef UInt8			UBYTE;

#include "MED.h"

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

static MMD0 		*mh;
static MMD0song 	*ms;
static ULONG 		*ba;

static MMD0NOTE 	*mmd0pat;
static MMD1NOTE 	*mmd1pat;

#define d0note(row,col) mmd0pat[(row*(UWORD) theMAD->header->numChn)+col]

#define d1note(row,col) mmd1pat[(row*(UWORD) theMAD->header->numChn)+col]


static Ptr			theMEDRead;
#define READMEDFILE(dst, size)	{memcpy(dst, theMEDRead, size);	theMEDRead += (long) size;}

static BOOL MED_Init(MADDriverSettings *init)
{
	mh=NULL;
	ms=NULL;
	ba=NULL;        // blockarr
	mmd0pat=NULL;
	mmd1pat=NULL;
	
	if(!(mh=(MMD0 *) malloc(sizeof(MMD0)))) return 0;
	if(!(ms=(MMD0song *) malloc(sizeof(MMD0song)))) return 0;
	return 1;
}

static void MED_Cleanup(void)
{
	if(mh != NULL)
	{
		free(mh);
		mh = NULL;
	}
	if(ms != NULL)
	{
		free(ms);
		ms = NULL;
	}
	if(ba != NULL)
	{
		free(ba);
		ba = NULL;
	}
	
	if(mmd0pat != NULL)
	{
		free(mmd0pat);
		mmd0pat = NULL;
	}
	if(mmd1pat != NULL)
	{
		free(mmd1pat);
		mmd1pat = NULL;
	}
}

static void EffectCvt(UBYTE eff, UBYTE dat, Cmd *aCmd)
{
	switch(eff){
			
			// 0x0 0x1 0x2 0x3 0x4      // PT effects
			
		case 0x5:       // PT vibrato with speed/depth nibbles swapped
			aCmd->cmd = 4;
			aCmd->arg = (dat>>4) | ((dat&0xf)<<4);
			break;
			
		case 0x6:       // not used
		case 0x7:       // not used
		case 0x8:       // midi hold/decay
			break;
			
		case 0x9:
			if(dat<=0x20)
			{
				aCmd->cmd = 0xf;
				aCmd->arg = dat;
			}
			break;
			
			// 0xa 0xb 0xc all PT effects
			
		case 0xd:       // same as PT volslide
			aCmd->cmd = 0xa;
			aCmd->arg = dat;
			break;
			
		case 0xe:       // synth jmp - midi
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
			
		default:        // all normal PT effects are handled here :)
			aCmd->cmd = eff;
			aCmd->arg = dat;
			break;
	}
}

static void MED_Convert1(short col, short patID, MADMusic *theMAD)
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



static void MED_Convert0(short patID, MADMusic *theMAD)
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

static OSErr LoadMMD0Patterns(MADMusic *theMAD, Ptr theMED, MADDriverSettings *init)
{
	int		t, row /*,col*/;
	UWORD	numtracks, numlines, maxlines=0;
	char	tC;
	int		x;
	
	// first, scan patterns to see how many channels are used
	
	for (t = 0; t < theMAD->header->numPat; t++)
	{
		theMEDRead = theMED + ba[t];
		READMEDFILE(&tC, sizeof(char));	numtracks = tC;
		READMEDFILE(&tC, sizeof(char));	numlines = tC;
		
		if(numtracks > theMAD->header->numChn) theMAD->header->numChn = numtracks;
		if(numlines  > maxlines) maxlines = numlines;
	}
	
	mmd0pat = (MMD0NOTE *) calloc(theMAD->header->numChn * (maxlines+1) * sizeof(MMD0NOTE), 1);
	if (mmd0pat == NULL) //Debugger();
		return MADNeedMemory;
	
	/* second read: no more mr. nice guy,
	 really read and convert patterns */
	
	for (t = 0 ; t < theMAD->header->numPat; t++)
	{
		theMEDRead = theMED + ba[t];
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
			((Ptr)mmd0pat)[x] = 0;
		}
		
		for (row = 0 ; row < numlines; row++) {
			short ztz;
			
			for (ztz = 0; ztz < numtracks; ztz++)
				READMEDFILE(&d0note(row, ztz), 3);		// sizeof(MMD0NOTE) != 3 probleme compilation alignement
		}
		
		MED_Convert0(t, theMAD);
	}
	
	return noErr;
}

static OSErr LoadMMD1Patterns(MADMusic *theMAD, Ptr theMED, MADDriverSettings *init)
{
	int		t,row,col;
	UWORD	numtracks,numlines,maxlines=0,track=0;
	int		x;
	
	// first, scan patterns to see how many channels are used
	
	for (t = 0 ; t < theMAD->header->numPat; t++)
	{
		theMEDRead = theMED + ba[t];
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
	
	mmd1pat = (MMD1NOTE *) calloc(theMAD->header->numChn * (maxlines+1) * sizeof(MMD1NOTE), 1);
	if (mmd1pat == NULL) //Debugger();
		return MADNeedMemory;
	
	/* second read: no more mr. nice guy,
	 really read and convert patterns */
	
	for(t=0;t<theMAD->header->numPat;t++){
		
		theMEDRead = theMED + ba[t];
		READMEDFILE(&numtracks, sizeof(short));
		READMEDFILE(&numlines, sizeof(short));
		
		theMEDRead += 4L;
		
		numlines++;
		
		theMAD->partition[t] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * numlines * sizeof(Cmd), 1);
		if (theMAD->partition[t] == NULL) return MADNeedMemory;
		
		theMAD->partition[t]->header.size 		= numlines;
		theMAD->partition[t]->header.compMode 	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[t]->header.name[x] = 0;
		
		theMAD->partition[t]->header.patBytes = 0;		theMAD->partition[t]->header.unused2 = 0;
		
		
		
		for (x = 0; x < theMAD->header->numChn * maxlines * sizeof(MMD0NOTE); x++)
		{
			((Ptr) mmd1pat)[x] = 0;
		}
		
		for (row = 0 ; row < numlines ; row++)
		{
			READMEDFILE(&d1note(row, 0), sizeof(MMD1NOTE) * numtracks);
		}
		
		for(col=0;col< theMAD->header->numChn;col++)
		{
			MED_Convert1(col, t, theMAD);
			track++;
		}
	}
	
	return noErr;
}

static OSErr MED_Load(Ptr	theMED, long MEDSize, MADMusic *theMAD, MADDriverSettings *init)
{
	int			t, i;
	ULONG		sa[64];
	InstrHdr	s;
	int			inOutCount;
	
	theMEDRead = theMED;
	
	/*********************/
	/** READ MED HEADER **/
	/*********************/
	
	READMEDFILE(mh, sizeof(MMD0));
	
	/**************************/
	/** READ MMD0song struct **/
	/**************************/
	
	theMEDRead = theMED + mh->MMD0songP;
	if (MEDSize < (sizeof(MMD0) + mh->MMD0songP)) {
		return MADIncompatibleFile;
	}
	READMEDFILE(ms, sizeof(MMD0song));
	
	/***************************/
	/** READ SamplePtr struct **/
	/***************************/
	
	theMEDRead = theMED + mh->InstrHdrPP;
	READMEDFILE(sa, sizeof(ULONG)*ms->numsamples);
	
	/***************************/
	/**    BLOCK PTR ARRAY    **/
	/***************************/
	
	ba = (ULONG*) calloc(ms->numblocks * sizeof(ULONG), 1);
	if (ba == NULL) return MADNeedMemory;
	
	theMEDRead = theMED + mh->MMD0BlockPP;
	READMEDFILE(ba, sizeof(ULONG)*ms->numblocks);
	
	
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
	
	strlcpy(theMAD->header->infos, "Converted by PlayerPRO MED Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->speed			= 	ms->tempo2;
	theMAD->header->tempo			=	(ms->deftempo * 125) / 33;
	theMAD->header->numChn			=	0;		// will be counted later
	theMAD->header->numPat			=	ms->numblocks;
	theMAD->header->numPointers		=	ms->songlen;
	if (ms->numsamples >= MAXINSTRU) ms->numsamples = MAXINSTRU-1;
	
	if (theMAD->header->numPointers > 128) theMAD->header->numPointers = 128;
	
	for (i = 0; i < ms->songlen; i++)
	{
		theMAD->header->oPointers[i] = ms->playseq[i];
		if (theMAD->header->oPointers[i] >= theMAD->header->numPat)
			theMAD->header->oPointers[i] = theMAD->header->numPat-1;
	}
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK * sizeof(FXSets), 1);
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	for (i = 0; i < MAXTRACK; i++)
	{
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
	
	for (t = 0 ; t < ms->numsamples ; t++)
	{
		theMEDRead = theMED + sa[t];
		READMEDFILE(&s, sizeof(InstrHdr));
		
		theMAD->fid[t].type = 0;
		
		if (s.length > 0 && s.length < 0x4d4d4430)
		{
			sData	*curData;
			
			theMAD->fid[t].numSamples = 1;
			theMAD->fid[t].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[t*MAXSAMPLE + 0] = (sData*) calloc(sizeof(sData), 1);
			
			curData->size		= s.length;
			curData->loopBeg 	= ms->sample[t].rep<<1;
			curData->loopSize 	= curData->loopBeg + (ms->sample[t].replen<<1);
			if (curData->loopBeg + curData->loopSize > curData->size)
			{
				curData->loopSize = curData->size - curData->loopBeg;
			}
			curData->vol		= 64;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			
			curData->data 		= (Ptr)malloc(curData->size);
			if (curData->data == NULL) return MADNeedMemory;
			
			READMEDFILE(curData->data, curData->size);
		}
		else theMAD->fid[t].numSamples = 0;
	}
	
	/***************************/
	/**       PATTERNS        **/
	/***************************/
	
	switch(mh->id)
	{
		case 'MMD0':
			if (LoadMMD0Patterns(theMAD, theMED, init) != noErr) return MADUnknowErr;
			break;
			
		case 'MMD1':
			if (LoadMMD1Patterns(theMAD, theMED, init) != noErr) return MADUnknowErr;
			break;
	}
	
	return noErr;
}

static Boolean compMem(Ptr a, Ptr b, long s)
{
	long 	i;
	
	for (i = 0; i < s; i++)
	{
		if (a[i] != b[i]) return false;
	}
	
	return true;
}

static OSErr TestMEDFile(Ptr AlienFile)
{
	if (compMem(AlienFile, "MMD0", 4) == false && compMem(AlienFile, "MMD1", 4) == false) return MADFileNotSupportedByThisPlug;
	else return noErr;
}

static OSErr ExtractMEDInfo(PPInfoRec *info, Ptr theMED)
{
	/*long	PatternSize;
	 short	i;
	 short	maxInstru;
	 short	tracksNo;
	 long	inOutCount;*/
	
	theMEDRead = theMED;
	
	READMEDFILE(mh, sizeof(MMD0));
	
	theMEDRead = theMED + mh->MMD0songP;
	READMEDFILE(ms, sizeof(MMD0song));
	
	info->signature = mh->id;
	
	//strcpy(info->internalFileName, "");
	info->internalFileName[0] = '\0';
	
	info->totalPatterns = ms->numblocks;
	info->partitionLength = ms->songlen;
	info->totalInstruments = ms->numsamples;
	info->totalTracks = 0;
	
	strlcpy(info->formatDescription, "MED Plug", sizeof(info->formatDescription));
	
	return noErr;
}

#ifndef _MAC_H

EXP OSErr FillPlug(PlugInfo *p);
EXP OSErr PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy(p->type, 		"MED ", sizeof(p->type));
	strlcpy(p->MenuName, 	"MED Files", sizeof(p->type));
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif


#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr mainMED(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	if(MED_Init(init) == 0)
	{
		MED_Cleanup();
		return MADNeedMemory;
	}
	
	switch(order)
	{
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc(sndSize * 2L);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free(AlienFile);
					
					AlienFile = (Ptr)malloc(sndSize);
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestMEDFile(AlienFile);
					if (myErr == noErr)
					{
						myErr = MED_Load(AlienFile,  sndSize, MadFile, init);
					}
					
					free(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(sndSize, AlienFile, iFileRefI);
					myErr = TestMEDFile(AlienFile);
					
					free(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = ExtractMEDInfo(info, AlienFile);
					
					free(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	MED_Cleanup();
	
	return myErr;
}
