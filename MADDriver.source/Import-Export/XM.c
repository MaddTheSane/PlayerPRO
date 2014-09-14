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
typedef int16_t		WORD;
typedef uint16_t	UWORD;
#if !defined(__COREFOUNDATION_CFPLUGINCOM__)
typedef int32_t		HRESULT;
typedef uint32_t	ULONG;
#endif
typedef void*		LPVOID;
typedef int32_t		LONG;

typedef uint16_t	UINT;
typedef bool		BOOL;
typedef uint32_t	DWORD;
typedef uint16_t	USHORT;
typedef int16_t		SHORT;
typedef MADFourChar	FOURCC;
typedef int8_t		BYTE;
#endif

typedef uint8_t		UBYTE;

#include "XM.h"

/**************************************************************************
 **************************************************************************/

#define HEADERSIZE 	276
#define PHSIZE		9
#define IHSIZE		263
#define IHSIZESMALL	33
#define IHSSIZE		40

struct staticXMData {
	MADByte		LastAEffect[MAXTRACK];
	XMHEADER	*mh;
	char		*theXMRead, *theXMMax;
};

#define READXMFILE(dst, size)	{memcpy(dst, xmData->theXMRead, size);	xmData->theXMRead += (long) size;}
#define WRITEXMFILE(src, size)	{memcpy(xmData->theXMRead, src, size);	xmData->theXMRead += (long) size;}

static bool XM_Init(MADDriverSettings *init, struct staticXMData *xmData)
{
	xmData->mh = (XMHEADER*)malloc(sizeof(XMHEADER));
	if (xmData->mh == NULL)
		return false;
	else
		return true;
}


static void XM_Cleanup(struct staticXMData *xmData)
{
	if (xmData->mh != NULL) {
		free(xmData->mh);
		xmData->mh = NULL;
	}
}

static void XM_WriteNote(Cmd *cmd, struct staticXMData *xmData)
{
	UBYTE	cmp = 0;
	int		r;
	
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
		WRITEXMFILE(&cmd->note, 1);
		WRITEXMFILE(&cmd->ins, 1);
		WRITEXMFILE(&cmd->vol, 1);
		WRITEXMFILE(&cmd->cmd, 1);
		WRITEXMFILE(&cmd->arg, 1);
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

static void XM_ReadNote(XMNOTE *n, struct staticXMData *xmData)
{
	UBYTE	cmp;
	int		r;
	
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
			READXMFILE(&n->ins, 1)
		} else
			n->ins = 0;
		r = cmp & 4L;
		if (r) {
			READXMFILE(&n->vol, 1)
		} else
			n->vol = 0xFF;
		r = cmp & 8L;
		if (r) {
			READXMFILE(&n->eff, 1)
		} else
			n->eff = 0xFF;
		r = cmp & 16L;
		if (r) {
			READXMFILE(&n->dat, 1)
		} else
			n->dat = 0xFF;
	} else {
		n->note	=	cmp;
		READXMFILE(&n->ins, 1);
		READXMFILE(&n->vol, 1);
		READXMFILE(&n->eff, 1);
		READXMFILE(&n->dat, 1);
	}
}

static void XM_Convert2MAD(XMNOTE *xmtrack, Cmd *aCmd, short channel)
{
	UBYTE note,ins,vol,eff,dat;
	
	note = xmtrack->note;
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
			switch(aCmd->cmd)
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
				
			case 'G'-55:					// G - set global volume
				aCmd->arg = 0;
				break;
				
			case 'H'-55:					// H - global volume slide
				aCmd->arg = 0;
				break;
				
			case 'K'-55:					// K - keyoff
				aCmd->ins		= 00;
				aCmd->note		= 0xFE;
				break;
				
			case 'L'-55:					// L - set envelope position
				aCmd->arg = 0;
				break;
				
			case 'P'-55:					// P - panning slide
				//	Convert en slide panning (volume cmd !)
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				
			{
				MADByte	lo, hi;
				
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
				
			case 'R'-55:					// R - multi retrig note
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
				/*	NumToString(eff, str);
				 DebugStr(str);*/
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

static MADErr XMReadPattern(MADMusic *theMAD, MADDriverSettings *init, struct staticXMData *xmData)
{
	int t, u, v, i;
	
	/*****************/
	/** PATTERNS **/
	/*****************/
	
	for (i = 0; i < MAXPATTERN; i++)
		theMAD->partition[i] = NULL;
	for (i = 0; i < MAXTRACK; i++)
		xmData->LastAEffect[i] = 0;
	
	for (t = 0; t < xmData->mh->numpat; t++) {
		short		PatternSize;
		XMPATHEADER	ph;
		XMNOTE		xmpat;
		Cmd			*aCmd;
		char		*theXMReadCopy;
		
		theXMReadCopy = xmData->theXMRead;
		READXMFILE(&ph.size, 		4);
		MADLE32(&ph.size);
		READXMFILE(&ph.packing, 	1);
		READXMFILE(&ph.numrows, 	2);
		MADLE16(&ph.numrows);
		READXMFILE(&ph.packsize, 	2);
		MADLE16(&ph.packsize);
		xmData->theXMRead = theXMReadCopy + ph.size;
		
		/*
		 Gr8.. when packsize is 0, don't try to load a pattern.. it's empty.
		 This bug was discovered thanks to Khyron's module..
		 */
		
		if (ph.packsize > 0) {
			PatternSize = ph.numrows;
			theMAD->partition[t] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * PatternSize * sizeof(Cmd), 1);
			if (theMAD->partition[t] == NULL)
				return MADNeedMemory;
			
			theMAD->partition[t]->header.size = PatternSize;
			theMAD->partition[t]->header.compMode = 'NONE';
			
			for (u = 0 ; u < PatternSize ; u++) {
				for (v = 0 ; v < theMAD->header->numChn ; v++) {
					XM_ReadNote(&xmpat, xmData);
					
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
	
	return MADNoErr;
}

static MADErr XMReadInstruments(MADMusic *theMAD, MADDriverSettings *init, struct staticXMData *xmData)
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
		theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for (i  = 0 ; i < MAXINSTRU; i++) {
		for (x = 0; x < MAXSAMPLE; x++)
			theMAD->sample[i * MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples = 0;
	}
	
	for (t = 0; t < xmData->mh->numins; t++) {
		XMINSTHEADER 	ih;
		InstrData		*curIns = &theMAD->fid[t];
		char			*theXMReadCopy;
		
		theXMReadCopy = xmData->theXMRead;
		READXMFILE(&ih.size,	4);
		MADLE32(&ih.size);
		READXMFILE(&ih.name,	22);
		READXMFILE(&ih.type,	1);
		READXMFILE(&ih.numsmp,	2);
		MADLE16(&ih.numsmp);
		//	READXMFILE(&ih.ssize,	4);		MADLE32(&ih.ssize);
		
		
		for (x = 0; x < 22; x++)
			curIns->name[x] = ih.name[x];
		curIns->numSamples	= ih.numsmp;
		
		if (ih.numsmp > 0) {
			XMPATCHHEADER pth;
			
			READXMFILE(&ih.ssize,		4);
			MADLE32(&ih.ssize);
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
			MADLE16(&pth.volfade);
			READXMFILE(&pth.reserved,	22);
			
			
			
			memcpy(curIns->what, pth.what, 96);
			
			// Volume Env
			
			memcpy(curIns->volEnv, pth.volenv, 48);
			for (x = 0; x < 12; x++) {
				MADLE16(&curIns->volEnv[x].pos);	//
				MADLE16(&curIns->volEnv[x].val);	// 00...64
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
				MADLE16(&curIns->pannEnv[x].pos);	//
				MADLE16(&curIns->pannEnv[x].val);	// 00...64
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
		xmData->theXMRead = theXMReadCopy + ih.size;
		
		/** Read samples */
		
		if (ih.numsmp < 0 || ih.numsmp > 30)
			return -1;
		
		for (u = 0 ; u < ih.numsmp ; u++) {
			XMWAVHEADER		wh;
			
			theXMReadCopy = xmData->theXMRead;
			READXMFILE(&wh.length,		4);
			MADLE32(&wh.length);
			READXMFILE(&wh.loopstart,	4);
			MADLE32(&wh.loopstart);
			READXMFILE(&wh.looplength,	4);
			MADLE32(&wh.looplength);
			READXMFILE(&wh.volume,		1);
			READXMFILE(&wh.finetune,	1);
			READXMFILE(&wh.type,		1);
			READXMFILE(&wh.panning,		1);
			READXMFILE(&wh.relnote,		1);
			READXMFILE(&wh.reserved,	1);
			READXMFILE(&wh.samplename,	22);
			
			xmData->theXMRead = theXMReadCopy + ih.ssize;
			
			{
				sData	*curData;
				int		finetune[16] =
				{
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				curData = theMAD->sample[t * MAXSAMPLE + u] = (sData*)calloc(sizeof(sData), 1);
				
				curData->size		= wh.length;
				curData->loopBeg 	= wh.loopstart;
				curData->loopSize 	= wh.looplength;
				
				curData->vol		= wh.volume;
				curData->c2spd		= finetune[(wh.finetune + 128)/16];
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
					curData->loopType = MADLoopTypeClassic;
				}
				
				if ((wh.type & 0x3) == 2) {	// Ping-pong Loop
					curData->loopType = MADLoopTypePingPong;
				}
				
				//	curData->panning	= wh.panning;
				curData->relNote	= wh.relnote;
				for (x = 0; x < 22; x++)
					curData->name[x] = wh.samplename[x];
			}
		}
		
		/** Read samples data **/
		
		for (u = 0 ; u < curIns->numSamples ; u++) {
			sData	*curData = theMAD->sample[t*MAXSAMPLE + u];
			
			curData->data 		= (char*)malloc(curData->size);
			if (curData->data == NULL)
				return MADNeedMemory;
			else {
				READXMFILE(curData->data, curData->size);
				
				if (curData->amp == 16) {
					short	*tt;
					int	tL;
					
					tt = (short*)curData->data;
					
					for (tL = 0; tL < curData->size/2; tL++) {
						MADLE16((void*)(tt + tL));
					}
					
					{
						/* Delta to Real */
						int oldV = 0, newV;
						int xL;
						
						for (xL = 0; xL < curData->size/2; xL++) {
							newV = tt[xL] + oldV;
							tt[xL] = newV;
							oldV = newV;
						}
					}
				} else {
					/* Delta to Real */
					int oldV = 0, newV;
					int xL;
					
					for (xL = 0; xL < curData->size; xL++) {
						newV = curData->data[xL] + oldV;
						curData->data[xL] = newV;
						oldV = newV;
					}
				}
			}
		}
	}
	
	return MADNoErr;
}

static MADErr XM_Load(char* theXM, size_t XMSize, MADMusic *theMAD, MADDriverSettings *init, struct staticXMData *xmData)
{
	int		i;
	int		inOutCount;
	MADErr	iErr = MADNoErr;
	
	xmData->theXMRead = theXM;
	
	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE(xmData->mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */
	
	MADLE16(&xmData->mh->version);
	MADLE32(&xmData->mh->headersize);
	MADLE16(&xmData->mh->songlength);
	MADLE16(&xmData->mh->restart);
	MADLE16(&xmData->mh->numchn);
	MADLE16(&xmData->mh->numpat);
	MADLE16(&xmData->mh->numins);
	if (xmData->mh->numins > MAXINSTRU)
		xmData->mh->numins = MAXINSTRU;
	
	MADLE16(&xmData->mh->flags);
	MADLE16(&xmData->mh->bpm);
	MADLE16(&xmData->mh->tempo);
	
	/********************/
	/** MAD ALLOCATION **/
	/********************/
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*)calloc(inOutCount, 1);
	if (theMAD->header == NULL)
		return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	for (i = 0; i < 20; i++) {
		theMAD->header->name[i] = xmData->mh->songname[i];
		if (theMAD->header->name[i] == 0)
			i = 21;
	}
	
	theMAD->header->speed		= 	xmData->mh->tempo;
	theMAD->header->tempo		=	xmData->mh->bpm;
	theMAD->header->numChn		=	xmData->mh->numchn;
	theMAD->header->numPat		=	xmData->mh->numpat;
	theMAD->header->numPointers	=	xmData->mh->songlength;
	
	if (theMAD->header->numPointers > 128)
		theMAD->header->numPointers = 128;
	
	strncpy(theMAD->header->infos, "Converted by PlayerPRO XM Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	for (i = 0; i < xmData->mh->songlength; i++) {
		theMAD->header->oPointers[i] = xmData->mh->orders[i];
		if (theMAD->header->oPointers[i] >= theMAD->header->numPat)
			theMAD->header->oPointers[i] = theMAD->header->numPat - 1;
	}
	
	if (xmData->mh->flags & 1)
		theMAD->header->XMLinear = true;
	
	for (i = 0; i < MAXTRACK; i++) {
		/*	if (x > 0) theMAD->header->chanPan[i] = MAX_PANNING/4;
		 else theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		 x--;
		 
		 if (x == -2) x = 2;*/
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
		theMAD->header->chanPan[i] = MAX_PANNING / 2;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*)calloc(MAXTRACK, sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++)
		theMAD->header->chanBus[i].copyId = i;
	
	switch (xmData->mh->version) {
		case 0x104:
			XMReadPattern(theMAD, init, xmData);
			iErr = XMReadInstruments(theMAD, init, xmData);
			break;
			
			/*	case 0x103:
			 XMReadInstruments(theMAD);
			 XMReadPattern(theMAD);
			 break;*/
			
		default:
			return MADFileNotSupportedByThisPlug;
			break;
	}
	
	return iErr;
}

//This Function isn't used right now. #if 0'ing out
#if 0
static int ConvertSampleC4SPDXM(char* src, size_t srcSize, short amp, int srcC4SPD, char* dst, int dstC4SPD)
{
	short	*src16 = (short*) src, *dst16 = (short*) dst;
	char*		src8 = src, dst8 = dst;
	int		x;
	
	if (amp == 8)
	{
		for(x = 0; x < srcSize; x++)
		{
			dst8[(x * dstC4SPD) / srcC4SPD] = src8[x];
		}
	}
	else
	{
		for(x = 0; x < srcSize/2; x++)
		{
			dst16[(x * dstC4SPD) / srcC4SPD] = src16[x];
		}
	}
	
	return (srcSize * dstC4SPD) / srcC4SPD;
}
#endif

static long XMGetPeriod(short note, int c2spd)
{
	uint32_t 	period, n,o, mytab[12] = {1712 * 16, 1616 * 16, 1524 * 16, 1440 * 16, 1356 * 16, 1280 * 16,
		1208 * 16, 1140 * 16, 1076 * 16, 1016 * 16, 960 * 16, 907 * 16 };
	
	n = note % 12;
	o = note / 12;
	
	period = ((8363U * (mytab[n])) >> o ) / c2spd;
	
	if (period == 0)
		period = 7242;
	
	return period;
}


static char* ConvertMad2XM(MADMusic *theMAD, MADDriverSettings *init, long *sndSize, struct staticXMData *xmData)
{
	int		u, i;
	size_t	PatternSize, InstruSize;
	int		NumberInstru;
	char*		finalXMPtr;
	
	/********************************/
	/* 			MAD INFORMATIONS    */
	/********************************/
	InstruSize 		= 0;
	PatternSize 	= 0;
	
	NumberInstru = MAXINSTRU;
	
	for (i = 0; i < MAXINSTRU ; i++) {
		InstruSize += sizeof(XMINSTHEADER) + sizeof(XMPATCHHEADER);
		
		for (u = 0; u < theMAD->fid[i].numSamples; u++) {
			InstruSize += sizeof(XMWAVHEADER) + theMAD->sample[i*MAXSAMPLE + u]->size;
		}
		
		if (theMAD->fid[i].numSamples > 0 || theMAD->fid[i].name[0] != 0) {
			NumberInstru = i + 1;
		}
	}
	
	for (i = 0; i < theMAD->header->numPat;i++) {
		PatternSize += sizeof(XMNOTE) * theMAD->header->numChn * theMAD->partition[i]->header.size;
		PatternSize += sizeof(XMPATHEADER);
	}
	/********************************/
	
	*sndSize = sizeof(XMHEADER) + InstruSize + PatternSize;
	
	xmData->theXMRead = finalXMPtr = (char*)malloc(*sndSize);
	xmData->theXMMax = xmData->theXMRead + *sndSize;
	if (xmData->theXMRead == NULL)
		return NULL;
	
	/********************/
	/** WRITE XM HEADER */
	/********************/
	
	memcpy(xmData->mh->id, "Extended Module: ", sizeof(xmData->mh->id));
	memcpy(xmData->mh->trackername, "FastTracker v2.00   ", sizeof(xmData->mh->trackername));
	xmData->mh->version			= 0x104;
	MADLE16(&xmData->mh->version);
	xmData->mh->headersize		= HEADERSIZE;
	MADLE32(&xmData->mh->headersize);
	xmData->mh->songlength 		= theMAD->header->numPointers;
	MADLE16(&xmData->mh->songlength);
	xmData->mh->restart 		= 0;
	MADLE16(&xmData->mh->restart);
	xmData->mh->numchn 			= theMAD->header->numChn;
	MADLE16(&xmData->mh->numchn);
	xmData->mh->numpat 			= theMAD->header->numPat;
	MADLE16(&xmData->mh->numpat);
	xmData->mh->numins 			= NumberInstru;
	MADLE16(&xmData->mh->numins);
	xmData->mh->flags 			= 0;
	if (theMAD->header->XMLinear)
		xmData->mh->flags = 1;
	else
		xmData->mh->flags = 0;
	MADLE16(&xmData->mh->flags);
	xmData->mh->bpm 			= theMAD->header->tempo;
	MADLE16(&xmData->mh->bpm);
	xmData->mh->tempo 			= theMAD->header->speed;
	MADLE16(&xmData->mh->tempo);
	
	memset(xmData->mh->songname, ' ', sizeof(xmData->mh->songname));
	for (i = 0; i < 21; i++) {
		xmData->mh->songname[i] = theMAD->header->name[i];
		if (theMAD->header->name[i] == 0)
			i = 21;
	}
	xmData->mh->songname[20] = 0x1a;
	
	memset(xmData->mh->orders, 0, sizeof(xmData->mh->orders));
	//TODO: dispatch this
	for(i = 0; i < theMAD->header->numPointers; i++) xmData->mh->orders[i] = theMAD->header->oPointers[i];
	
	WRITEXMFILE(xmData->mh, sizeof(XMHEADER));
	
	/*****************/
	/** PATTERNS    **/
	/*****************/
	
	{
		int t, u, v;
		
		for (t = 0; t < theMAD->header->numPat; t++) {
			short		PatternSize;
			XMPATHEADER	ph;
			char		*packingCopy, *cc;
			
			ph.size = PHSIZE;
			MADLE32(&ph.size);
			WRITEXMFILE(&ph.size, 4);
			ph.packing = 0;
			WRITEXMFILE(&ph.packing, 1);
			ph.numrows = theMAD->partition[t]->header.size;
			MADLE16(&ph.numrows);
			WRITEXMFILE(&ph.numrows, 2);
			
			packingCopy = xmData->theXMRead;
			ph.packsize = 1;
			MADLE16(&ph.packsize);
			WRITEXMFILE(&ph.packsize, 2);
			
			cc = xmData->theXMRead;
			for (u = 0 ; u < theMAD->partition[t]->header.size ; u++) {
				for (v = 0 ; v < theMAD->header->numChn ; v++) {
					Cmd		*aCmd;
					Cmd		bCmd;
					
					aCmd = GetMADCommand(u, v, theMAD->partition[t]);
					
					bCmd = *aCmd;
					
					XM_Convert2XM(&bCmd);
					
					XM_WriteNote(&bCmd, xmData);
				}
			}
			
			PatternSize = xmData->theXMRead - cc;
			
			cc = xmData->theXMRead;
			xmData->theXMRead = packingCopy;
			ph.packsize = PatternSize;
			MADLE16(&ph.packsize);
			WRITEXMFILE(&ph.packsize, 2);
			xmData->theXMRead = cc;
		}
	}
	
	/*****************/
	/** INSTRUMENTS **/
	/*****************/
	
	{
		int t, u, x;
		
		for (t = 0; t < NumberInstru; t++) {
			XMINSTHEADER 	ih;
			size_t			ihsizecopy, ihssizecopy;
			InstrData		*curIns = &theMAD->fid[t];
			char			*theXMReadCopy = xmData->theXMRead;
			
			//************************//
			// Instrument header size //
			
			if (curIns->numSamples > 0)
				ih.size = IHSIZE;
			else
				ih.size = IHSIZESMALL;
			
			ihsizecopy = ih.size;
			MADLE32(&ih.size);
			WRITEXMFILE(&ih.size, 4);
			
			//************************//
			
			for (x = 0; x < 22; x++)
				ih.name[x] = curIns->name[x];
			WRITEXMFILE(&ih.name, 22);
			ih.type = 0;
			WRITEXMFILE(&ih.type, 1);
			ih.numsmp = curIns->numSamples;
			MADLE16(&ih.numsmp);
			WRITEXMFILE(&ih.numsmp, 2);
			
			ih.ssize = IHSSIZE;
			ihssizecopy = ih.ssize;
			MADLE32(&ih.ssize);
			
			if (curIns->numSamples > 0) {
				XMPATCHHEADER	pth;
				
				memset(&pth, 0, sizeof(pth));
				
				memcpy(pth.what, curIns->what, 96);
				
				memcpy(pth.volenv, curIns->volEnv, 48);
				for (x = 0; x < 24; x++) MADLE16(&pth.volenv[x]);
				
				pth.volpts = curIns->volSize;
				pth.volflg = curIns->volType;
				pth.volsus = curIns->volSus;
				pth.volbeg = curIns->volBeg;
				pth.volend = curIns->volEnd;
				pth.volfade = curIns->volFade;
				
				
				
				memcpy(pth.panenv, curIns->pannEnv, 48);
				for (x = 0; x < 24; x++) MADLE16(&pth.panenv[x]);
				
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
				MADLE16(&pth.volfade);
				WRITEXMFILE(&pth.volfade,	2);
				WRITEXMFILE(&pth.reserved,	22);
			}
			xmData->theXMRead = theXMReadCopy + ihsizecopy;
			
			/** WRITE samples */
			
			for (u = 0 ; u < curIns->numSamples ; u++) {
				XMWAVHEADER	wh;
				sData		*curData;
				short		modifc2spd = 0;
				int			copyc2spd;
				int			finetune[16] = {
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				curData = theMAD->sample[t*MAXSAMPLE + u];
				
				wh.volume = curData->vol;
				
				copyc2spd = curData->c2spd;
				
				if (curData->c2spd > 8757 || curData->c2spd < 7895) {
#define BASECALC 45
					
					wh.finetune = 0;			// <- 8363 Hz
					
					while (XMGetPeriod(BASECALC, curData->c2spd) < XMGetPeriod(BASECALC + modifc2spd, 8363)) {
						modifc2spd++;
					}
					
					curData->c2spd = 8363;
					
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
					if (curData->loopType == MADLoopTypePingPong)
						wh.type |= 0x2;
					else
						wh.type |= 0x1;
				}
				
				wh.panning = 128;	//curData->panning;
				wh.relnote = curData->relNote + modifc2spd;
				strncpy(wh.samplename, curData->name, sizeof(wh.samplename));
				
				theXMReadCopy = xmData->theXMRead;
				MADLE32(&wh.length);
				WRITEXMFILE(&wh.length,		4);
				MADLE32(&wh.loopstart);
				WRITEXMFILE(&wh.loopstart,	4);
				MADLE32(&wh.looplength);
				WRITEXMFILE(&wh.looplength,	4);
				WRITEXMFILE(&wh.volume,		1);
				WRITEXMFILE(&wh.finetune,	1);
				WRITEXMFILE(&wh.type,		1);
				wh.panning		= 128;
				WRITEXMFILE(&wh.panning,	1);
				WRITEXMFILE(&wh.relnote,	1);
				WRITEXMFILE(&wh.reserved,	1);
				WRITEXMFILE(&wh.samplename,	22);
				xmData->theXMRead = theXMReadCopy + ihssizecopy;
			}
			
			/** WRITE samples data **/
			
			for (u = 0 ; u < curIns->numSamples ; u++) {
				sData	*curData = theMAD->sample[t * MAXSAMPLE + u];
				char	*theXMReadCopy = xmData->theXMRead;
				int		curSize;
				
				WRITEXMFILE(curData->data, curData->size);
				curSize = curData->size;
				
				if (curData->stereo) {
					if (curData->amp == 8) {
						for (x = 0 ; x < curSize; x += 2) {
							theXMReadCopy[x / 2] = ((int)theXMReadCopy[x] + (int) theXMReadCopy[x + 1]) / 2L;
						}
					} else {
						short *short16out = (short*)theXMReadCopy, *short16in = (short*)theXMReadCopy;
						
						for (x = 0 ; x < curSize/2; x += 2) {
							short16out[x / 2] = ((int)short16in[x] + (int)short16in[x + 1]) / 2;
						}
					}
					
					curSize /= 2;
					xmData->theXMRead -= curSize;
				}
				
				if (curData->amp == 16) {
					short	*tt = (short*) theXMReadCopy;
					int		tL;
					
					/* Real to Delta */
					int	oldV = 0, newV;
					int	xL;
					
					for (xL = 0; xL < curSize / 2; xL++) {
						newV = tt[xL];
						tt[xL] -= oldV;
						oldV = newV;
					}
					
					for (tL = 0; tL < curSize / 2; tL++) {
						MADLE16((char*)(tt + tL));
					}
				} else {
					/* Real to Delta */
					int	oldV = 0, newV;
					int	xL;
					
					for (xL = 0; xL < curSize; xL++) {
						newV = theXMReadCopy[xL];
						theXMReadCopy[xL] -= oldV;
						oldV = newV;
					}
				}
			}
		}
	}
	
	*sndSize = xmData->theXMRead - finalXMPtr;
	
	return finalXMPtr;
}

static inline bool compMem(const void *a, const void *b, size_t s)
{
	return memcmp(a, b, s) == 0;
}

static MADErr TestXMFile(char* AlienFile, struct staticXMData *xmData)
{
	if (compMem(AlienFile, "Extended Module: ", 17)) {
		xmData->theXMRead = AlienFile;
		
		/********************/
		/** READ XM HEADER **/
		/********************/
		
		READXMFILE(xmData->mh, sizeof(XMHEADER));
		/* BigEndian <-> LittleEndian */
		
		MADLE16(&xmData->mh->version);
		MADLE16(&xmData->mh->songlength);
		MADLE16(&xmData->mh->numchn);
		MADLE16(&xmData->mh->numpat);
		MADLE16(&xmData->mh->numins);
		
		switch (xmData->mh->version) {
			case 0x104:
				break;
				
			default:
				return MADFileNotSupportedByThisPlug;
				break;
		}
		return MADNoErr;
	}
	return MADFileNotSupportedByThisPlug;
}

static MADErr ExtractXMInfo(MADInfoRec *info, void *AlienFile, struct staticXMData *xmData)
{
	short i;
	
	xmData->theXMRead = AlienFile;
	
	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE(xmData->mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */
	
	MADLE16(&xmData->mh->version);
	MADLE16(&xmData->mh->songlength);
	MADLE16(&xmData->mh->numchn);
	MADLE16(&xmData->mh->numpat);
	MADLE16(&xmData->mh->numins);
	MADLE16(&xmData->mh->flags);
	
	switch(xmData->mh->version) {
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
		info->internalFileName[i] = xmData->mh->songname[i];
		if (info->internalFileName[i] == 0 || info->internalFileName[i] == 0x1a) {
			info->internalFileName[i] = 0;
			i = 21;
		}
	}
	
	/*** Total Patterns ***/
	
	info->totalPatterns = xmData->mh->numpat;
	
	/*** Partition Length ***/
	
	info->partitionLength = xmData->mh->songlength;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = xmData->mh->numins;
	
	/*** Total Tracks ***/
	
	info->totalTracks = xmData->mh->numchn;
	
	if (xmData->mh->flags & 1)
		strncpy(info->formatDescription, "XM Linear Plug", sizeof(info->formatDescription));
	else
		strncpy(info->formatDescription, "XM Log Plug", sizeof(info->formatDescription));
	
	return MADNoErr;
}

#ifndef _MAC_H

EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strncpy(p->type, 		"XM  ", sizeof(p->type));
	strncpy(p->MenuName, 	"XM Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImportExport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
MADErr mainXM(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr = MADNoErr;
	void*	AlienFile;
	long	sndSize;
	UNFILE	iFileRefI;
	struct staticXMData xmData = {0};
	
	if (!XM_Init(init, &xmData))
		return MADNeedMemory;
	
	switch (order) {
		case MADPlugExport:
			AlienFile = ConvertMad2XM(MadFile, init, &sndSize, &xmData);
			
			if (AlienFile != NULL) {
				iFileCreate(AlienFileName, 'XM  ');
				iFileRefI = iFileOpenWrite(AlienFileName);
				if (iFileRefI) {
					iWrite(sndSize, AlienFile, iFileRefI);
					iClose(iFileRefI);
				} else {
					myErr = MADWritingErr;
				}
				free(AlienFile);
				AlienFile = NULL;
			} else
				myErr = MADNeedMemory;
			break;
			
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = malloc(sndSize * 2);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					free(AlienFile);
					
					AlienFile = malloc(sndSize);
					if (AlienFile == NULL)
						myErr = MADNeedMemory;
					else {
						iRead(sndSize, AlienFile, iFileRefI);
						
						myErr = TestXMFile(AlienFile, &xmData);
						if (myErr == MADNoErr) {
							myErr = XM_Load(AlienFile, sndSize, MadFile, init, &xmData);
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
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					
					if (myErr == MADNoErr)
						myErr = TestXMFile(AlienFile, &xmData);
					
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
				
				sndSize = 5000; // Read only 5000 first bytes for optimisation
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == MADNoErr) {
						myErr = TestXMFile(AlienFile, &xmData);
						if (!myErr)
							myErr = ExtractXMInfo(info, AlienFile, &xmData);
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
	
	XM_Cleanup(&xmData);
	
	return myErr;
}
