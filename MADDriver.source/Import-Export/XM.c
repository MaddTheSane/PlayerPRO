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

//Needed to quiet a few warnings on Windows.
#define _CRT_SECURE_NO_WARNINGS 1

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

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

#include "XM.h"

/**************************************************************************
 **************************************************************************/

#define HEADERSIZE 	276
#define PHSIZE		9
#define IHSIZE		263
#define IHSIZESMALL	33
#define IHSSIZE		40

static		Byte		LastAEffect[MAXTRACK];
static		XMHEADER	*mh;
static		char		*theXMRead, *theXMMax;

#define READXMFILE(dst, size)	{memcpy(dst, theXMRead, size);	theXMRead += (long) size;}
#define WRITEXMFILE(src, size)	{memcpy(theXMRead, src, size);	theXMRead += (long) size;}

static Boolean XM_Init( MADDriverSettings *init)
{
	mh = (XMHEADER*)malloc(sizeof(XMHEADER));
	if (mh == NULL)
		return false;
	else
		return true;
}


static void XM_Cleanup(void)
{
	if (mh != NULL) {
		free(mh);
		mh = NULL;
	}
}

static void XM_WriteNote( Cmd *cmd)
{
	UBYTE	cmp = 0;
	SInt32	r;
	
	if (cmd->note)
		cmp |= 1;
	if (cmd->ins)
		cmp |= 2;
	if (cmd->vol)
		cmp |= 4;
	if (cmd->cmd)
		cmp |= 8;
	if (cmd->arg)
		cmp |= 16;
	
	if (cmp == 31)		// all bytes used -> no compression
	{
		WRITEXMFILE( &cmd->note, 1);
		WRITEXMFILE( &cmd->ins, 1);
		WRITEXMFILE( &cmd->vol, 1);
		WRITEXMFILE( &cmd->cmd, 1);
		WRITEXMFILE( &cmd->arg, 1);
	} else {
		UBYTE ccmp = cmp + 0x80;
		
		WRITEXMFILE(&ccmp, 1);
		
		r = cmp & 1;
		if (r) {
			WRITEXMFILE(&cmd->note, 1);
		}
		r = cmp & 2;
		if (r) {
			WRITEXMFILE(&cmd->ins, 1);
		}
		r = cmp & 4;
		if (r) {
			WRITEXMFILE(&cmd->vol, 1);
		}
		r = cmp & 8;
		if (r) {
			WRITEXMFILE(&cmd->cmd, 1);
		}
		r = cmp & 16;
		if (r) {
			WRITEXMFILE(&cmd->arg, 1);
		}
	}
}

static void XM_ReadNote(XMNOTE *n)
{
	UBYTE	cmp;
	SInt32	r;
	
	READXMFILE(&cmp, 1);
	
	r = cmp & 0x80;
	if (r) {
		r = cmp & 1L;
		if (r) {
			READXMFILE(&n->note, 1)
		} else
			n->note = 0xFF;
		r = cmp & 2L;
		if (r) {
			READXMFILE( &n->ins, 1)
		} else
			n->ins = 0;
		r = cmp & 4L;
		if (r) {
			READXMFILE( &n->vol, 1)
		} else
			n->vol = 0xFF;
		r = cmp & 8L;
		if (r) {
			READXMFILE( &n->eff, 1)
		} else
			n->eff = 0xFF;
		r = cmp & 16L;
		if (r) {
			READXMFILE( &n->dat, 1)
		} else
			n->dat = 0xFF;
	} else {
		n->note	=	cmp;
		READXMFILE( &n->ins, 1);
		READXMFILE( &n->vol, 1);
		READXMFILE( &n->eff, 1);
		READXMFILE( &n->dat, 1);
	}
}

static void XM_Convert2MAD(XMNOTE *xmtrack, Cmd *aCmd, short channel)
{
	UBYTE 	note,ins,vol,eff,dat;
	//Str32	str;
	
	note	=	xmtrack->note;
	if (note == 0 || note == 0xFF)
		note = 0xFF;
	else
		note--;
	
	ins		=	xmtrack->ins;
	vol		=	xmtrack->vol;
	eff		=	xmtrack->eff;
	dat		=	xmtrack->dat;
	
	aCmd->ins		= ins;
	aCmd->note		= note;
	aCmd->vol		= vol;
	if (note == 96) {						// KEY OFF
		aCmd->note		= 0xFE;
	}
	
	
	//	if (eff == 0 && dat != 0) Debugger();
	
	if (eff <= 0x0F) {
		aCmd->cmd		= eff;
		aCmd->arg		= dat;
		
		if (aCmd->arg == 0xFF) {
			switch( aCmd->cmd)
			{
				case portamentoE:
					break;
					
				default:
					aCmd->arg = 0;
					break;
			}
		}
		
		if (aCmd->cmd == slidevolE) {
#if 0
			if (aCmd->arg == 0)
				aCmd->arg = LastAEffect[channel];
			else
				LastAEffect[channel] = aCmd->arg;
#endif
		}
	} else {
		aCmd->cmd		= 0;
		aCmd->arg		= dat;
		if (aCmd->arg == 0xFF)
			aCmd->arg = 0;
		
		switch(eff) {
			case 255:
				aCmd->cmd		= 0;
				break;
				
			case 'G'-55:                    // G - set global volume
				aCmd->arg = 0;
				break;
				
			case 'H'-55:                    // H - global volume slide
				aCmd->arg = 0;
				break;
				
			case 'K'-55:                    // K - keyoff
				aCmd->ins		= 00;
				aCmd->note		= 0xFE;
				break;
				
			case 'L'-55:                    // L - set envelope position
				aCmd->arg = 0;
				break;
				
			case 'P'-55:                    // P - panning slide
				//	Convert en slide panning (volume cmd !)
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				
			{
				Byte	lo, hi;
				
				lo = dat & 0xf;
				hi = dat >> 4;
				
				/* slide right has absolute priority */
				if (hi)
					lo = 0;
				
				if (hi)
					aCmd->vol = 0xE0 + hi;
				else
					aCmd->vol = 0xD0 + lo;
			}
				break;
				
			case 'R'-55:                    // R - multi retrig note
				aCmd->arg = 0;
				break;
				
			case 'T'-55:
				aCmd->arg = 0;
				break;
				
			case 'X'-55:
				if ((dat >> 4) == 1) {	// X1 extra fine porta up
					aCmd->arg = 0;
				} else {			// X2 extra fine porta down
					aCmd->arg = 0;
				}
				break;
				
			default:
				/*	NumToString( eff, str);
				 DebugStr( str);*/
				aCmd->arg = 0;
				break;
		}
	}
}

static void XM_Convert2XM(Cmd *aCmd)
{
	if (aCmd->note == 0xFE)
		aCmd->note = 96 + 1;	// KEY OFF
	else if (aCmd->note == 0xFF)
		aCmd->note = 0;
	else
		aCmd->note++;
	
	if (aCmd->vol == 0xFF)
		aCmd->vol = 0;
	if (aCmd->cmd == 0xFF)
		aCmd->cmd = 0;
}

static OSErr XMReadPattern(MADMusic *theMAD, MADDriverSettings *init)
{
	int t, u, v, i;
	
	/*****************/
	/** PATTERNS **/
	/*****************/
	
	for (i = 0; i < MAXPATTERN; i++)
		theMAD->partition[i] = NULL;
	for (i = 0; i < MAXTRACK; i++)
		LastAEffect[i] = 0;
	
	for (t = 0; t < mh->numpat; t++) {
		short				PatternSize;
		XMPATHEADER 		ph;
		XMNOTE				xmpat;
		Cmd					*aCmd;
		Ptr					theXMReadCopy;
		
		theXMReadCopy = theXMRead;
		READXMFILE(&ph.size, 		4);
		PPLE32(&ph.size);
		READXMFILE(&ph.packing, 	1);
		READXMFILE(&ph.numrows, 	2);
		PPLE16(&ph.numrows);
		READXMFILE(&ph.packsize, 	2);
		PPLE16(&ph.packsize);
		theXMRead = theXMReadCopy + ph.size;
		
		/*
		 Gr8.. when packsize is 0, don't try to load a pattern.. it's empty.
		 This bug was discovered thanks to Khyron's module..
		 */
		
		if (ph.packsize > 0) {
			PatternSize = ph.numrows;
			theMAD->partition[t] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * PatternSize * sizeof(Cmd), 1);
			if (theMAD->partition[t] == NULL)
				return MADNeedMemory;
			
			theMAD->partition[ t]->header.size = PatternSize;
			theMAD->partition[ t]->header.compMode = 'NONE';
			
			for (u = 0 ; u < PatternSize ; u++) {
				for (v = 0 ; v < theMAD->header->numChn ; v++) {
					XM_ReadNote( &xmpat);
					
					aCmd = GetMADCommand(u, v, theMAD->partition[t]);
					
					XM_Convert2MAD(&xmpat, aCmd, v);
				}
			}
		} else {
			Cmd			nullCmd;
			
			nullCmd.ins		= 0;
			nullCmd.note	= 0xFF;
			nullCmd.cmd		= 0;
			nullCmd.arg		= 0;
			nullCmd.vol		= 0xFF;
			nullCmd.unused	= 0;
			
			PatternSize = 1;
			theMAD->partition[t] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * PatternSize * sizeof(Cmd), 1);
			if (theMAD->partition[t] == NULL)
				return MADNeedMemory;
			
			theMAD->partition[t]->header.size = PatternSize;
			
			for (u = 0 ; u < PatternSize ; u++) {
				for (v = 0 ; v < theMAD->header->numChn ; v++) {
					aCmd = GetMADCommand(u, v, theMAD->partition[t]);
					*aCmd = nullCmd;
				}
			}
		}
	}
	
	return noErr;
}

static OSErr XMReadInstruments(MADMusic *theMAD, MADDriverSettings *init)
{
	int			t, u, i, x;
	//long		inOutCount = 0;
	
	/*****************/
	/** INSTRUMENTS **/
	/*****************/
	
	theMAD->fid = (InstrData*)calloc(sizeof(InstrData) ,MAXINSTRU);
	if (!theMAD->fid)
		return MADNeedMemory;
	
	theMAD->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample)
		return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++)
		theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	for (i  = 0 ; i < MAXINSTRU; i++) {
		for (x = 0; x < MAXSAMPLE; x++)
			theMAD->sample[i * MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples = 0;
	}
	
	for (t = 0; t < mh->numins; t++) {
		XMINSTHEADER 	ih;
		InstrData		*curIns = &theMAD->fid[t];
		char			*theXMReadCopy;
		
		theXMReadCopy = theXMRead;
		READXMFILE(&ih.size,	4);
		PPLE32( &ih.size);
		READXMFILE(&ih.name,	22);
		READXMFILE(&ih.type,	1);
		READXMFILE(&ih.numsmp,	2);
		PPLE16( &ih.numsmp);
		//	READXMFILE( &ih.ssize,	4);		PPLE32( &ih.ssize);
		
		
		for (x = 0; x < 22; x++)
			curIns->name[x] = ih.name[x];
		curIns->numSamples	= ih.numsmp;
		
		if (ih.numsmp > 0) {
			XMPATCHHEADER pth;
			
			READXMFILE(&ih.ssize,		4);
			PPLE32(&ih.ssize);
			READXMFILE(&pth.what,		96);
			READXMFILE(&pth.volenv,		48);
			READXMFILE(&pth.panenv,		48);
			READXMFILE(&pth.volpts,		1);
			READXMFILE(&pth.panpts,		1);
			READXMFILE(&pth.volsus,		1);
			READXMFILE(&pth.volbeg,		1);
			READXMFILE(&pth.volend,		1);
			READXMFILE(&pth.pansus,		1);
			READXMFILE(&pth.panbeg,		1);
			READXMFILE(&pth.panend,		1);
			READXMFILE(&pth.volflg,		1);
			READXMFILE(&pth.panflg,		1);
			READXMFILE(&pth.vibflg,		1);
			READXMFILE(&pth.vibsweep,	1);
			READXMFILE(&pth.vibdepth,	1);
			READXMFILE(&pth.vibrate,	1);
			READXMFILE(&pth.volfade,	2);
			PPLE16(&pth.volfade);
			READXMFILE(&pth.reserved,	22);
			
			
			
			memcpy(curIns->what, pth.what, 96);
			
			// Volume Env
			
			memcpy( curIns->volEnv, pth.volenv, 48);
			for (x = 0; x < 12; x++) {
				PPLE16(&curIns->volEnv[x].pos);	//
				PPLE16(&curIns->volEnv[x].val);	// 00...64
			}
			
			curIns->volSize	= pth.volpts;
			curIns->volType	= pth.volflg;
			curIns->volSus	= pth.volsus;
			curIns->volBeg	= pth.volbeg;
			curIns->volEnd	= pth.volend;
			
			if (curIns->volBeg >= curIns->volSize)
				curIns->volBeg = curIns->volSize-1;
			if (curIns->volEnd >= curIns->volSize)
				curIns->volEnd = curIns->volSize-1;
			
			curIns->volFade	= pth.volfade;
			
			// Panning Env
			
			memcpy(curIns->pannEnv, pth.panenv, 48);
			
			for (x = 0; x < 12; x++) {
				PPLE16(&curIns->pannEnv[x].pos);	//
				PPLE16(&curIns->pannEnv[x].val);	// 00...64
			}
			
			curIns->pannSize	= pth.panpts;
			curIns->pannType	= pth.panflg;
			curIns->pannSus		= pth.pansus;
			curIns->pannBeg		= pth.panbeg;
			curIns->pannEnd		= pth.panend;
			
			if (curIns->pannBeg >= curIns->pannSize)
				curIns->pannBeg = curIns->pannSize-1;
			if (curIns->pannEnd >= curIns->pannSize)
				curIns->pannEnd = curIns->pannSize-1;
			
			//	curIns->panFade	= pth.panfade;
		}
		theXMRead = theXMReadCopy + ih.size;
		
		/** Read samples */
		
		if (ih.numsmp < 0 || ih.numsmp > 30)
			return -1;
		
		for (u = 0 ; u < ih.numsmp ; u++) {
			XMWAVHEADER		wh;
			
			theXMReadCopy = theXMRead;
			READXMFILE(&wh.length,		4);
			PPLE32( &wh.length);
			READXMFILE(&wh.loopstart,	4);
			PPLE32( &wh.loopstart);
			READXMFILE(&wh.looplength,	4);
			PPLE32( &wh.looplength);
			READXMFILE(&wh.volume,		1);
			READXMFILE(&wh.finetune,	1);
			READXMFILE(&wh.type,		1);
			READXMFILE(&wh.panning,		1);
			READXMFILE(&wh.relnote,		1);
			READXMFILE(&wh.reserved,	1);
			READXMFILE(&wh.samplename,	22);
			
			theXMRead = theXMReadCopy + ih.ssize;
			
			{
				sData	*curData;
				SInt32 	finetune[ 16] =
				{
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				curData = theMAD->sample[t * MAXSAMPLE + u] = (sData*)calloc(sizeof(sData), 1);
				
				curData->size		= wh.length;
				curData->loopBeg 	= wh.loopstart;
				curData->loopSize 	= wh.looplength;
				
				curData->vol		= wh.volume;
				curData->c2spd		= finetune[ (wh.finetune + 128)/16];
				curData->loopType	= 0;
				curData->amp		= 8;
				
				
				
				if (wh.type & 0x10) {		// 16 Bits
					curData->amp = 16;
				}
				
				if ((wh.type & 0x3) == 0) {	// NO LOOP
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if ((wh.type & 0x3) == 1) {	// Forward Loop
					curData->loopType = eClassicLoop;
				}
				
				if ((wh.type & 0x3) == 2) {	// Ping-pong Loop
					curData->loopType = ePingPongLoop;
				}
				
				//	curData->panning	= wh.panning;
				curData->relNote	= wh.relnote;
				for (x = 0; x < 22; x++)
					curData->name[x] = wh.samplename[x];
			}
		}
		
		/** Read samples data **/
		
		for (u = 0 ; u < curIns->numSamples ; u++) {
			sData	*curData = theMAD->sample[ t*MAXSAMPLE + u];
			
			curData->data 		= (char*)malloc( curData->size);
			if (curData->data == NULL)
				return MADNeedMemory;
			else {
				READXMFILE(curData->data, curData->size);
				
				if (curData->amp == 16) {
					short	*tt;
					SInt32	tL;
					
					tt = (short*)curData->data;
					
					for (tL = 0; tL < curData->size/2; tL++) {
						PPLE16((void*)(tt + tL));
					}
					
					{
						/* Delta to Real */
						SInt32	oldV, newV;
						SInt32	xL;
						
						oldV = 0;
						
						for (xL = 0; xL < curData->size/2; xL++) {
							newV = tt[xL] + oldV;
							tt[xL] = newV;
							oldV = newV;
						}
					}
				} else {
					/* Delta to Real */
					SInt32	oldV, newV;
					SInt32	xL;
					
					oldV = 0;
					
					for (xL = 0; xL < curData->size; xL++) {
						newV = curData->data[ xL] + oldV;
						curData->data[ xL] = newV;
						oldV = newV;
					}
				}
			}
		}
	}
	
	return noErr;
}

static OSErr XM_Load(Ptr theXM, size_t XMSize, MADMusic *theMAD, MADDriverSettings *init)
{
	int		i;
	SInt32	inOutCount;
	OSErr	iErr = noErr;
	
	theXMRead = theXM;
	
	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE(mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */
	
	PPLE16(&mh->version);
	PPLE32(&mh->headersize);
	PPLE16(&mh->songlength);
	PPLE16(&mh->restart);
	PPLE16(&mh->numchn);
	PPLE16(&mh->numpat);
	PPLE16(&mh->numins);
	if (mh->numins > MAXINSTRU)
		mh->numins = MAXINSTRU;
	
	PPLE16(&mh->flags);
	PPLE16(&mh->bpm);
	PPLE16(&mh->tempo);
	
	/********************/
	/** MAD ALLOCATION **/
	/********************/
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*)calloc(inOutCount, 1);
	if (theMAD->header == NULL)
		return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	for (i = 0; i < 20; i++) {
		theMAD->header->name[i] = mh->songname[i];
		if (theMAD->header->name[i] == 0)
			i = 21;
	}
	
	theMAD->header->speed		= 	mh->tempo;
	theMAD->header->tempo		=	mh->bpm;
	theMAD->header->numChn		=	mh->numchn;
	theMAD->header->numPat		=	mh->numpat;
	theMAD->header->numPointers	=	mh->songlength;
	
	if (theMAD->header->numPointers > 128)
		theMAD->header->numPointers = 128;
	
	strlcpy(theMAD->header->infos, "Converted by PlayerPRO XM Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	for (i = 0; i < mh->songlength; i++) {
		theMAD->header->oPointers[i] = mh->orders[i];
		if (theMAD->header->oPointers[i] >= theMAD->header->numPat)
			theMAD->header->oPointers[i] = theMAD->header->numPat - 1;
	}
	
	if (mh->flags & 1)
		theMAD->header->XMLinear = true;
	
	for (i = 0; i < MAXTRACK; i++) {
		/*	if (x > 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
		 else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		 x--;
		 
		 if (x == -2) x = 2;*/
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
		theMAD->header->chanPan[ i] = MAX_PANNING / 2;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*)calloc(MAXTRACK, sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++)
		theMAD->header->chanBus[i].copyId = i;
	
	switch (mh->version) {
		case 0x104:
			XMReadPattern(theMAD, init);
			iErr = XMReadInstruments(theMAD, init);
			break;
			
			/*	case 0x103:
			 XMReadInstruments( theMAD);
			 XMReadPattern( theMAD);
			 break;*/
			
		default:
			return MADFileNotSupportedByThisPlug;
			break;
	}
	
	return iErr;
}

//This Function isn't used right now. #if 0'ing out
#if 0
static SInt32 ConvertSampleC4SPDXM( Ptr src, size_t srcSize, short amp, SInt32 srcC4SPD, Ptr dst, SInt32 dstC4SPD)
{
	short	*src16 = (short*) src, *dst16 = (short*) dst;
	Ptr		src8 = src, dst8 = dst;
	SInt32	x;
	
	if (amp == 8)
	{
		for( x = 0; x < srcSize; x++)
		{
			dst8[ (x * dstC4SPD) / srcC4SPD] = src8[ x];
		}
	}
	else
	{
		for( x = 0; x < srcSize/2; x++)
		{
			dst16[ (x * dstC4SPD) / srcC4SPD] = src16[ x];
		}
	}
	
	return (srcSize * dstC4SPD) / srcC4SPD;
}
#endif

static long XMGetPeriod( short note, SInt32 c2spd)
{
	UInt32 	period, n,o, mytab[12] = {1712 * 16, 1616 * 16, 1524 * 16, 1440 * 16, 1356 * 16, 1280 * 16,
		1208 * 16, 1140 * 16, 1076 * 16, 1016 * 16, 960 * 16, 907 * 16 };
	
	n = note % 12;
	o = note / 12;
	
	period = ((8363U * (mytab[n])) >> o ) / c2spd;
	
	if (period == 0)
		period = 7242;
	
	return period;
}


static Ptr	ConvertMad2XM( MADMusic *theMAD, MADDriverSettings *init, long *sndSize)
{
	int			u, i;
	size_t		PatternSize, InstruSize;
	SInt32		NumberInstru;
	Ptr			finalXMPtr;
	
	/********************************/
	/* 			MAD INFORMATIONS    */
	/********************************/
	InstruSize 		= 0;
	PatternSize 	= 0;
	
	NumberInstru = MAXINSTRU;
	
	for (i = 0; i < MAXINSTRU ; i++) {
		InstruSize += sizeof( XMINSTHEADER) + sizeof( XMPATCHHEADER);
		
		for (u = 0; u < theMAD->fid[i].numSamples; u++) {
			InstruSize += sizeof( XMWAVHEADER) + theMAD->sample[ i*MAXSAMPLE + u]->size;
		}
		
		if (theMAD->fid[ i].numSamples > 0 || theMAD->fid[ i].name[ 0] != 0) {
			NumberInstru = i + 1;
		}
	}
	
	for (i = 0; i < theMAD->header->numPat;i++) {
		PatternSize += sizeof(XMNOTE) * theMAD->header->numChn * theMAD->partition[i]->header.size;
		PatternSize += sizeof( XMPATHEADER);
	}
	/********************************/
	
	*sndSize = 						sizeof(XMHEADER) +
	InstruSize +
	PatternSize;
	
	theXMRead = finalXMPtr = (Ptr)malloc(*sndSize);
	theXMMax = theXMRead + *sndSize;
	if (theXMRead == NULL)
		return NULL;
	
	/********************/
	/** WRITE XM HEADER */
	/********************/
	
	
	
	strncpy(mh->id, "Extended Module: ", sizeof(mh->id));
	strncpy(mh->trackername, "FastTracker v2.00   ", sizeof(mh->trackername));
	mh->version			= 0x104;
	PPLE16(&mh->version);
	mh->headersize		= HEADERSIZE;
	PPLE32(&mh->headersize);
	mh->songlength 		= theMAD->header->numPointers;
	PPLE16(&mh->songlength);
	mh->restart 		= 0;
	PPLE16(&mh->restart);
	mh->numchn 			= theMAD->header->numChn;
	PPLE16(&mh->numchn);
	mh->numpat 			= theMAD->header->numPat;
	PPLE16(&mh->numpat);
	mh->numins 			= NumberInstru;
	PPLE16(&mh->numins);
	mh->flags 			= 0;
	if (theMAD->header->XMLinear)
		mh->flags = 1;
	else
		mh->flags = 0;
	PPLE16(&mh->flags);
	mh->bpm 			= theMAD->header->tempo;
	PPLE16(&mh->bpm);
	mh->tempo 			= theMAD->header->speed;
	PPLE16(&mh->tempo);
	
	memset(mh->songname, ' ', sizeof(mh->songname));
	for (i = 0; i < 21; i++) {
		mh->songname[i] = theMAD->header->name[i];
		if (theMAD->header->name[i] == 0)
			i = 21;
	}
	mh->songname[20] = 0x1a;
	
	memset(mh->orders, 0, sizeof(mh->orders));
	//TODO: dispatch this
	for(i = 0; i < theMAD->header->numPointers; i++) mh->orders[i] = theMAD->header->oPointers[i];
	
	WRITEXMFILE(mh, sizeof(XMHEADER));
	
	/*****************/
	/** PATTERNS    **/
	/*****************/
	
	{
		int			t, u, v;
		
		for (t = 0; t < theMAD->header->numPat; t++) {
			short				PatternSize;
			XMPATHEADER 		ph;
			Ptr					packingCopy, cc;
			
			ph.size = PHSIZE;
			PPLE32(&ph.size);
			WRITEXMFILE(&ph.size, 4);
			ph.packing = 0;
			WRITEXMFILE(&ph.packing, 1);
			ph.numrows = theMAD->partition[t]->header.size;
			PPLE16(&ph.numrows);
			WRITEXMFILE(&ph.numrows, 2);
			
			packingCopy = theXMRead;
			ph.packsize = 1;
			PPLE16(&ph.packsize);
			WRITEXMFILE(&ph.packsize, 2);
			
			cc = theXMRead;
			for (u = 0 ; u < theMAD->partition[ t]->header.size ; u++) {
				for (v = 0 ; v < theMAD->header->numChn ; v++) {
					Cmd		*aCmd;
					Cmd		bCmd;
					
					aCmd = GetMADCommand(u, v, theMAD->partition[t]);
					
					bCmd = *aCmd;
					
					XM_Convert2XM(&bCmd);
					
					XM_WriteNote(&bCmd);
				}
			}
			
			PatternSize = theXMRead - cc;
			
			cc = theXMRead;
			theXMRead = packingCopy;
			ph.packsize = PatternSize;
			PPLE16(&ph.packsize);
			WRITEXMFILE(&ph.packsize, 2);
			theXMRead = cc;
		}
	}
	
	/*****************/
	/** INSTRUMENTS **/
	/*****************/
	
	{
		int			t, u, x;
		//long		inOutCount = 0;
		
		for (t = 0; t < NumberInstru; t++) {
			XMINSTHEADER 	ih;
			size_t			ihsizecopy, ihssizecopy;
			
			InstrData		*curIns = &theMAD->fid[t];
			Ptr				theXMReadCopy;
			
			theXMReadCopy = theXMRead;
			
			//************************//
			// Instrument header size //
			
			if (curIns->numSamples > 0)
				ih.size = IHSIZE;
			else
				ih.size = IHSIZESMALL;
			
			ihsizecopy = ih.size;
			PPLE32(&ih.size);
			WRITEXMFILE(&ih.size,	4);
			
			//************************//
			
			for (x = 0; x < 22; x++)
				ih.name[x] = curIns->name[x];
			WRITEXMFILE(&ih.name, 22);
			ih.type = 0;
			WRITEXMFILE(&ih.type, 1);
			ih.numsmp = curIns->numSamples;
			PPLE16(&ih.numsmp);
			WRITEXMFILE(&ih.numsmp, 2);
			
			ih.ssize = IHSSIZE;
			ihssizecopy = ih.ssize;
			PPLE32(&ih.ssize);
			
			if (curIns->numSamples > 0) {
				XMPATCHHEADER	pth;
				
				memset(&pth, 0, sizeof(pth));
				
				memcpy(pth.what, curIns->what, 96);
				
				memcpy(pth.volenv, curIns->volEnv, 48);
				for (x = 0; x < 24; x++) PPLE16(&pth.volenv[x]);
				
				pth.volpts = curIns->volSize;
				pth.volflg = curIns->volType;
				pth.volsus = curIns->volSus;
				pth.volbeg = curIns->volBeg;
				pth.volend = curIns->volEnd;
				pth.volfade = curIns->volFade;
				
				
				
				memcpy(pth.panenv, curIns->pannEnv, 48);
				for (x = 0; x < 24; x++) PPLE16(&pth.panenv[x]);
				
				pth.panpts = curIns->pannSize;
				pth.panflg = curIns->pannType;
				pth.pansus = curIns->pannSus;
				pth.panbeg = curIns->pannBeg;
				pth.panend = curIns->pannEnd;
				
				
				
				WRITEXMFILE(&ih.ssize,		4);
				WRITEXMFILE(&pth.what,		96);
				WRITEXMFILE(&pth.volenv,	48);
				WRITEXMFILE(&pth.panenv,	48);
				WRITEXMFILE(&pth.volpts,	1);
				WRITEXMFILE(&pth.panpts,	1);
				WRITEXMFILE(&pth.volsus,	1);
				WRITEXMFILE(&pth.volbeg,	1);
				WRITEXMFILE(&pth.volend,	1);
				WRITEXMFILE(&pth.pansus,	1);
				WRITEXMFILE(&pth.panbeg,	1);
				WRITEXMFILE(&pth.panend,	1);
				WRITEXMFILE(&pth.volflg,	1);
				WRITEXMFILE(&pth.panflg,	1);
				WRITEXMFILE(&pth.vibflg,	1);
				WRITEXMFILE(&pth.vibsweep,	1);
				WRITEXMFILE(&pth.vibdepth,	1);
				WRITEXMFILE(&pth.vibrate,	1);
				PPLE16(&pth.volfade);
				WRITEXMFILE(&pth.volfade,	2);
				WRITEXMFILE(&pth.reserved,	22);
			}
			theXMRead = theXMReadCopy + ihsizecopy;
			
			/** WRITE samples */
			
			for (u = 0 ; u < curIns->numSamples ; u++) {
				XMWAVHEADER		wh;
				sData			*curData;
				short			modifc2spd = 0;
				SInt32			copyc2spd;
				
				SInt32 	finetune[16] = {
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				curData = theMAD->sample[ t*MAXSAMPLE + u];
				
				wh.volume		= curData->vol;
				
				copyc2spd = curData->c2spd;
				
				if (curData->c2spd > 8757 || curData->c2spd < 7895) {
#define BASECALC 45
					
					wh.finetune = 0;			// <- 8363 Hz
					
					while (XMGetPeriod(BASECALC, curData->c2spd) < XMGetPeriod(BASECALC + modifc2spd, 8363)) {
						modifc2spd++;
					}
					
					curData->c2spd = 8363;
				}
				
				
				{
					wh.length 		= curData->size;
					wh.loopstart	= curData->loopBeg;
					wh.looplength	= curData->loopSize;
					
					if (curData->stereo) {
						wh.length /= 2;
						wh.loopstart /= 2;
						wh.looplength /= 2;
					}
					
					wh.finetune = -128;
					if (curData->c2spd > 8757)
						wh.finetune = 127;
					else {
						while (finetune[(wh.finetune + 128)/16] < curData->c2spd) {
							wh.finetune += 16;
						}
					}
				}
				
				curData->c2spd = copyc2spd;
				
				wh.type = 0;
				if (curData->amp == 16)
					wh.type |= 0x10;
				if (curData->loopSize > 0) {
					if (curData->loopType == ePingPongLoop)
						wh.type |= 0x2;
					else
						wh.type |= 0x1;
				}
				
				wh.panning = 128;	//curData->panning;
				wh.relnote = curData->relNote + modifc2spd;
				strlcpy(wh.samplename, curData->name, sizeof(wh.samplename));
				
				theXMReadCopy = theXMRead;
				PPLE32(&wh.length);
				WRITEXMFILE(&wh.length,		4);
				PPLE32(&wh.loopstart);
				WRITEXMFILE(&wh.loopstart,	4);
				PPLE32(&wh.looplength);
				WRITEXMFILE(&wh.looplength,	4);
				WRITEXMFILE(&wh.volume,		1);
				WRITEXMFILE(&wh.finetune,	1);
				WRITEXMFILE(&wh.type,		1);
				wh.panning		= 128;
				WRITEXMFILE(&wh.panning,	1);
				WRITEXMFILE(&wh.relnote,	1);
				WRITEXMFILE(&wh.reserved,	1);
				WRITEXMFILE(&wh.samplename,	22);
				theXMRead = theXMReadCopy + ihssizecopy;
			}
			
			/** WRITE samples data **/
			
			for (u = 0 ; u < curIns->numSamples ; u++) {
				sData	*curData = theMAD->sample[t * MAXSAMPLE + u];
				Ptr		theXMReadCopy = theXMRead;
				SInt32	curSize;
				
				WRITEXMFILE(curData->data, curData->size);
				curSize = curData->size;
				
				if (curData->stereo) {
					if (curData->amp == 8) {
						for (x = 0 ; x < curSize; x += 2) {
							theXMReadCopy[x / 2] = ((SInt32)theXMReadCopy[x] + (SInt32) theXMReadCopy[x + 1]) / 2L;
						}
					} else {
						short *short16out = (short*)theXMReadCopy, *short16in = (short*)theXMReadCopy;
						
						for (x = 0 ; x < curSize/2; x += 2) {
							short16out[x / 2] = ((SInt32)short16in[x] + (SInt32)short16in[x + 1]) / 2;
						}
					}
					
					curSize /= 2;
					theXMRead -= curSize;
				}
				
				if (curData->amp == 16) {
					short	*tt = (short*) theXMReadCopy;
					SInt32	tL;
					
					/* Real to Delta */
					SInt32	oldV, newV;
					SInt32	xL;
					
					oldV = 0;
					
					for (xL = 0; xL < curSize / 2; xL++) {
						newV = tt[ xL];
						tt[ xL] -= oldV;
						oldV = newV;
					}
					
					for (tL = 0; tL < curSize / 2; tL++) {
						PPLE16((Ptr)(tt + tL));
					}
				} else {
					/* Real to Delta */
					SInt32	oldV, newV;
					SInt32	xL;
					//	Byte	*tempByte = (Byte*) theXMReadCopy;
					
					oldV = 0;
					
					for (xL = 0; xL < curSize; xL++) {
						newV = theXMReadCopy[xL];
						theXMReadCopy[xL] -= oldV;
						oldV = newV;
					}
				}
			}
		}
	}
	
	*sndSize = theXMRead - finalXMPtr;
	
	return finalXMPtr;
}

static inline Boolean compMem(const void *a, const void *b, size_t s)
{
	if (memcmp(a, b, s) == 0) {
		return true;
	} else {
		return false;
	}
}

static OSErr TestXMFile(Ptr AlienFile)
{
	if (compMem(AlienFile, "Extended Module: ", 17)) {
		theXMRead = AlienFile;
		
		/********************/
		/** READ XM HEADER **/
		/********************/
		
		READXMFILE(mh, sizeof(XMHEADER));
		/* BigEndian <-> LittleEndian */
		
		PPLE16(&mh->version);
		PPLE16(&mh->songlength);
		PPLE16(&mh->numchn);
		PPLE16(&mh->numpat);
		PPLE16(&mh->numins);
		
		switch (mh->version)
		{
			case 0x104:
				break;
				
			default:
				return MADFileNotSupportedByThisPlug;
				break;
		}
		return noErr;
	}
	return MADFileNotSupportedByThisPlug;
}

static OSErr ExtractXMInfo(PPInfoRec *info, void *AlienFile)
{
	short	i;
	
	theXMRead = AlienFile;
	
	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE(mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */
	
	PPLE16(&mh->version);
	PPLE16(&mh->songlength);
	PPLE16(&mh->numchn);
	PPLE16(&mh->numpat);
	PPLE16(&mh->numins);
	PPLE16(&mh->flags);
	
	switch( mh->version)
	{
		case 0x104:
			break;
			
		default:
			return MADFileNotSupportedByThisPlug;
			break;
	}
	
	/*** Signature ***/
	
	info->signature = 'XM  ';
	
	/*** Internal name ***/
	
	for (i = 0; i < 21; i++) {
		info->internalFileName[i] = mh->songname[i];
		if (info->internalFileName[i] == 0 || info->internalFileName[i] == 0x1a) {
			info->internalFileName[i] = 0;
			i = 21;
		}
	}
	
	/*** Total Patterns ***/
	
	info->totalPatterns = mh->numpat;
	
	/*** Partition Length ***/
	
	info->partitionLength = mh->songlength;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = mh->numins;
	
	/*** Total Tracks ***/
	
	info->totalTracks = mh->numchn;
	
	if (mh->flags & 1)
		strlcpy(info->formatDescription, "XM Linear Plug", sizeof(info->formatDescription));
	else
		strlcpy(info->formatDescription, "XM Log Plug", sizeof(info->formatDescription));
	
	return noErr;
}

#ifndef _MAC_H

EXP OSErr FillPlug( PlugInfo *p);
EXP OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy(p->type, 		"XM  ", sizeof(p->type));
	strlcpy(p->MenuName, 	"XM Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImportExport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr mainXM(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	OSErr		myErr;
	Ptr			AlienFile;
	long		sndSize;
	UNFILE		iFileRefI;
	
	myErr = noErr;
	
	if (!XM_Init(init))
		return MADNeedMemory;
	
	switch (order) {
		case MADPlugExport:
			AlienFile = ConvertMad2XM(MadFile, init, &sndSize);
			
			if (AlienFile != NULL) {
				iFileCreate(AlienFileName, 'XM  ');
				iFileRefI = iFileOpenWrite(AlienFileName);
				if (iFileRefI) {
					iWrite(sndSize, AlienFile, iFileRefI);
					iClose(iFileRefI);
				} else {
					myErr = MADWritingErr;
				}
				free(AlienFile);	AlienFile = NULL;
			} else
				myErr = MADNeedMemory;
			break;
			
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc(sndSize * 2);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					free(AlienFile);
					
					AlienFile = (Ptr)malloc(sndSize);
					if (AlienFile == NULL)
						myErr = MADNeedMemory;
					else {
						iRead(sndSize, AlienFile, iFileRefI);
						
						myErr = TestXMFile(AlienFile);
						if (myErr == noErr) {
							myErr = XM_Load(AlienFile, sndSize, MadFile, init);
						}
					}
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024;
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					
					if (myErr == noErr)
						myErr = TestXMFile(AlienFile);
					
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
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr) {
						myErr = TestXMFile(AlienFile);
						if (!myErr)
							myErr = ExtractXMInfo(info, AlienFile);
					}
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
	
	XM_Cleanup();
	
	return myErr;
}
