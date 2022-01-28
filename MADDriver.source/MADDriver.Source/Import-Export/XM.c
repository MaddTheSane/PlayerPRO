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


#include "RDriver.h"
#include "XM.h"
#include "FileUtils.h"

#ifdef _SRC
#elif defined(_MAC_H)
#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( Ptr)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( MADMusic*)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( PPInfoRec*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( MADDriverSettings*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif
#endif

Ptr MADPlugNewPtr( long size, MADDriverSettings* init);
Ptr MADPlugNewPtrClear( long size, MADDriverSettings* init);

/**************************************************************************
**************************************************************************/

#define HEADERSIZE 	276
#define PHSIZE		9
#define IHSIZE		263
#define IHSIZESMALL	33
#define IHSSIZE		40

static	Byte		LastAEffect[ MAXTRACK];
static XMHEADER 	*mh;
static Ptr			theXMRead, theXMMax;

#define READXMFILE(dst, size)	{BlockMoveData( theXMRead, dst, size);	theXMRead += (long) size;}
#define WRITEXMFILE(src, size)	{BlockMoveData( src, theXMRead, size);	theXMRead += (long) size;}

#ifndef _SRC

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}



Ptr MADPlugNewPtr( long size, MADDriverSettings* init)
{
	return NewPtr( size);
}

Ptr MADPlugNewPtrClear( long size, MADDriverSettings* init)
{
	return NewPtrClear( size);
}

#endif

Boolean XM_Init( MADDriverSettings *init)
{
	mh = (XMHEADER*) MADPlugNewPtr( sizeof(XMHEADER), init);
	if( mh == 0L) return false;
	else return true;
}


void XM_Cleanup(void)
{
	if( mh != 0L) DisposePtr( (Ptr) mh);
}

void XM_WriteNote( Cmd *cmd)
{
	UBYTE	cmp;
	long	r;
	
	cmp = 0;
	if( cmd->note) 	cmp += 1;
	if( cmd->ins) 	cmp += 2;
	if( cmd->vol) 	cmp += 4;
	if( cmd->cmd) 	cmp += 8;
	if( cmd->arg) 	cmp += 16;
	
	if( cmp == 31)		// all bytes used -> no compression
	{
		WRITEXMFILE( &cmd->note, 1);
		WRITEXMFILE( &cmd->ins, 1);
		WRITEXMFILE( &cmd->vol, 1);
		WRITEXMFILE( &cmd->cmd, 1);
		WRITEXMFILE( &cmd->arg, 1);
	}
	else
	{
		UBYTE	ccmp = cmp + 0x80;
	
		WRITEXMFILE( &ccmp, 1);
		
		r = cmp & 1L;	if( r) 	WRITEXMFILE( &cmd->note, 1);
		r = cmp & 2L;	if( r) 	WRITEXMFILE( &cmd->ins, 1);
		r = cmp & 4L; 	if( r) 	WRITEXMFILE( &cmd->vol, 1);
		r = cmp & 8L;	if( r) 	WRITEXMFILE( &cmd->cmd, 1);
		r = cmp & 16L;	if( r) 	WRITEXMFILE( &cmd->arg, 1);
		
		
	}
}

void XM_ReadNote(XMNOTE *n)
{
	UBYTE	cmp;
	long	r;

	READXMFILE( &cmp, 1);

	r = cmp & 0x80;
	if( r)
	{
		r = cmp & 1L;	if( r) 	READXMFILE( &n->note, 1)	else n->note 	= 0xFF;
		r = cmp & 2L;	if( r) 	READXMFILE( &n->ins, 1)		else n->ins 	= 0;
		r = cmp & 4L; 	if( r) 	READXMFILE( &n->vol, 1)		else n->vol 	= 0xFF;
		r = cmp & 8L;	if( r) 	READXMFILE( &n->eff, 1)		else n->eff 	= 0xFF;
		r = cmp & 16L;	if( r) 	READXMFILE( &n->dat, 1)		else n->dat 	= 0xFF;
	}
	else
	{
		n->note	=	cmp;
		READXMFILE( &n->ins, 1);
		READXMFILE( &n->vol, 1);
		READXMFILE( &n->eff, 1);
		READXMFILE( &n->dat, 1);
	}
}

void XM_Convert2MAD(XMNOTE *xmtrack, Cmd		*aCmd, short channel)
{
	UBYTE 	note,ins,vol,eff,dat;
	Str32	str;

		note	=	xmtrack->note;
		if( note == 0 || note == 0xFF) note = 0xFF;
		else note--;
		
		ins		=	xmtrack->ins;
		vol		=	xmtrack->vol;
		eff		=	xmtrack->eff;
		dat		=	xmtrack->dat;

		aCmd->ins		= ins;
		aCmd->note		= note;
		aCmd->vol		= vol;
		if( note == 96)						// KEY OFF
		{
			aCmd->note		= 0xFE;
		}
		
		
	//	if( eff == 0 && dat != 0) Debugger();
		
		if( eff <= 0x0F)
		{
			aCmd->cmd		= eff;	
			aCmd->arg		= dat;
			
			if( aCmd->arg == 0xFF)
			{
				switch( aCmd->cmd)
				{
					case portamentoE:	break;
				
					default:	aCmd->arg = 0;	break;
				}
			}
			
			if( aCmd->cmd == slidevolE)
			{
			/*	if( aCmd->arg == 0) aCmd->arg = LastAEffect[ channel];
				else LastAEffect[ channel] = aCmd->arg;*/
			}
		}
		else
		{
			aCmd->cmd		= 0;
			aCmd->arg		= dat;	if( aCmd->arg == 0xFF) aCmd->arg = 0;
			
			switch(eff)
			{
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
					if(hi) lo = 0;
					
					if( hi) aCmd->vol = 0xE0 + hi;
					else aCmd->vol = 0xD0 + lo;
					}
				break;
				
				case 'R'-55:                    // R - multi retrig note
					aCmd->arg = 0;
				break;
	
				case 'T'-55:
					aCmd->arg = 0;
				break;
	
				case 'X'-55:
					if((dat>>4)==1)
					{    						// X1 extra fine porta up
						aCmd->arg = 0;
					}
					else
					{							// X2 extra fine porta down
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

void XM_Convert2XM( Cmd		*aCmd)
{
	if( aCmd->note == 0xFE) aCmd->note		= 96 + 1;	// KEY OFF
	else if( aCmd->note == 0xFF) aCmd->note		= 0;
	else aCmd->note++;
	
	if( aCmd->vol == 0xFF)	aCmd->vol = 0;
	if( aCmd->cmd == 0xFF)	aCmd->cmd = 0;
}

OSErr XMReadPattern( MADMusic *theMAD, MADDriverSettings *init)
{
	int			t, u, v, i;

	/*****************/
	/** PATTERNS **/
	/*****************/

	for( i = 0; i < MAXPATTERN; i++) theMAD->partition[ i] = 0L;
	for( i = 0; i < MAXTRACK; i++) LastAEffect[ i] = 0;
	
	for(t=0;t<mh->numpat;t++)
	{
		short				PatternSize;
		XMPATHEADER 		ph;
		XMNOTE				xmpat;
		Cmd					*aCmd;
		Ptr					theXMReadCopy;
		
		theXMReadCopy = theXMRead;
		READXMFILE( &ph.size, 		4);			INT32( &ph.size);
		READXMFILE( &ph.packing, 	1);
		READXMFILE( &ph.numrows, 	2);			INT16( &ph.numrows);
		READXMFILE( &ph.packsize, 	2);			INT16( &ph.packsize);
		theXMRead = theXMReadCopy + ph.size;
		
		/*
			Gr8.. when packsize is 0, don't try to load a pattern.. it's empty.
			This bug was discovered thanks to Khyron's module..
		*/

		if( ph.packsize > 0)
		{
			PatternSize = ph.numrows;
			theMAD->partition[ t] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * PatternSize * sizeof( Cmd), init);
			if( theMAD->partition[ t] == 0L) return MADNeedMemory;
			
			theMAD->partition[ t]->header.size = PatternSize;
			theMAD->partition[ t]->header.compMode = 'NONE';
			
			for( u = 0 ; u < PatternSize ; u++)
			{
				for( v = 0 ; v < theMAD->header->numChn ; v++)
				{
					XM_ReadNote( &xmpat);

					aCmd = GetMADCommand(  u,  v, theMAD->partition[ t]);
					
					XM_Convert2MAD( &xmpat, aCmd, v);
				}
			}
		}
		else
		{
			Cmd			nullCmd;
			
			nullCmd.ins		= 0;
			nullCmd.note	= 0xFF;
			nullCmd.cmd		= 0;
			nullCmd.arg		= 0;
			nullCmd.vol		= 0xFF;
			nullCmd.unused	= 0;
			
			PatternSize = 1;
			theMAD->partition[ t] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * PatternSize * sizeof( Cmd), init);
			if( theMAD->partition[ t] == 0L) return MADNeedMemory;
			
			theMAD->partition[ t]->header.size = PatternSize;
			
			for( u = 0 ; u < PatternSize ; u++)
			{
				for( v = 0 ; v < theMAD->header->numChn ; v++)
				{
					aCmd = GetMADCommand(  u,  v, theMAD->partition[ t]);
					*aCmd = nullCmd;
				}
			}
		}
	}
	
	return noErr;
}

OSErr XMReadInstruments( MADMusic *theMAD, MADDriverSettings *init)
{
	int			t, u, i, x;
	long		inOutCount = 0;

	/*****************/
	/** INSTRUMENTS **/
	/*****************/
	
	theMAD->fid = ( InstrData*) MADPlugNewPtrClear( sizeof( InstrData) * (long) MAXINSTRU, init);
	if( !theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = ( sData**) MADPlugNewPtrClear( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
	if( !theMAD->sample) return MADNeedMemory;
	
	for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	for(i  = 0 ; i < MAXINSTRU; i++)
	{
		for( x = 0; x < MAXSAMPLE; x++) theMAD->sample[ i*MAXSAMPLE + x] = 0L;

		theMAD->fid[i].numSamples	= 0;
	}
	
	for( t = 0; t < mh->numins; t++)	//
	{
		XMINSTHEADER 	ih;
		InstrData		*curIns = &theMAD->fid[ t];
		Ptr				theXMReadCopy;

		theXMReadCopy = theXMRead;
		READXMFILE( &ih.size,	4);		INT32( &ih.size);
		READXMFILE( &ih.name,	22);
		READXMFILE( &ih.type,	1);
		READXMFILE( &ih.numsmp,	2);	INT16( &ih.numsmp);
	//	READXMFILE( &ih.ssize,	4);		INT32( &ih.ssize);
		

		for( x = 0; x < 22; x++) curIns->name[ x] = ih.name[x];
		curIns->numSamples	= ih.numsmp;

		if(ih.numsmp > 0)
		{
			XMPATCHHEADER	pth;

			READXMFILE( &ih.ssize,	4);			INT32( &ih.ssize);
			READXMFILE( &pth.what,		96);
			READXMFILE( &pth.volenv,	48);
			READXMFILE( &pth.panenv,	48);
			READXMFILE( &pth.volpts,	1);
			READXMFILE( &pth.panpts,	1);
			READXMFILE( &pth.volsus,	1);
			READXMFILE( &pth.volbeg,	1);
			READXMFILE( &pth.volend,	1);
			READXMFILE( &pth.pansus,	1);
			READXMFILE( &pth.panbeg,	1);
			READXMFILE( &pth.panend,	1);
			READXMFILE( &pth.volflg,	1);
			READXMFILE( &pth.panflg,	1);
			READXMFILE( &pth.vibflg,	1);
			READXMFILE( &pth.vibsweep,	1);
			READXMFILE( &pth.vibdepth,	1);
			READXMFILE( &pth.vibrate,	1);
			READXMFILE( &pth.volfade,	2);		INT16( &pth.volfade);
			READXMFILE( &pth.reserved,	22);
			
			
			
			BlockMoveData( pth.what, 	curIns->what, 		96);
			
			// Volume Env
			
			BlockMoveData( pth.volenv, 	curIns->volEnv, 	48);
			for( x = 0; x < 12; x++)
			{
				INT16( &curIns->volEnv[ x].pos);	// 
				INT16( &curIns->volEnv[ x].val);	// 00...64
			}
			
			curIns->volSize	= pth.volpts;
			curIns->volType	= pth.volflg;
			curIns->volSus	= pth.volsus;
			curIns->volBeg	= pth.volbeg;
			curIns->volEnd	= pth.volend;
			
			if( curIns->volBeg >= curIns->volSize) curIns->volBeg = curIns->volSize-1;
			if( curIns->volEnd >= curIns->volSize) curIns->volEnd = curIns->volSize-1;
			
			curIns->volFade	= pth.volfade;
			
			// Panning Env
			
			BlockMoveData( pth.panenv, 	curIns->pannEnv, 	48);
			
			for( x = 0; x < 12; x++)
			{
				INT16( &curIns->pannEnv[ x].pos);	// 
				INT16( &curIns->pannEnv[ x].val);	// 00...64
			}
			
			curIns->pannSize	= pth.panpts;
			curIns->pannType	= pth.panflg;
			curIns->pannSus	= pth.pansus;
			curIns->pannBeg	= pth.panbeg;
			curIns->pannEnd	= pth.panend;
			
			if( curIns->pannBeg >= curIns->pannSize) curIns->pannBeg = curIns->pannSize-1;
			if( curIns->pannEnd >= curIns->pannSize) curIns->pannEnd = curIns->pannSize-1;
			
		//	curIns->panFade	= pth.panfade;
		}
		theXMRead = theXMReadCopy + ih.size;
		
		/** Read samples */
		
		if( ih.numsmp < 0 || ih.numsmp > 30) return -1;
		
		for( u = 0 ; u < ih.numsmp ; u++)
		{
			XMWAVHEADER		wh;
			
			theXMReadCopy = theXMRead;
			READXMFILE( &wh.length,			4);			INT32( &wh.length);
			READXMFILE( &wh.loopstart,		4);		INT32( &wh.loopstart);
			READXMFILE( &wh.looplength,		4);		INT32( &wh.looplength);
			READXMFILE( &wh.volume,			1);
			READXMFILE( &wh.finetune,		1);
			READXMFILE( &wh.type,			1);
			READXMFILE( &wh.panning,		1);
			READXMFILE( &wh.relnote,		1);
			READXMFILE( &wh.reserved,		1);
			READXMFILE( &wh.samplename,		22);
			
			theXMRead = theXMReadCopy + ih.ssize;
			
			{
				sData	*curData;
				long 	finetune[ 16] = 
				{
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				curData = theMAD->sample[ t*MAXSAMPLE + u] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
				
				curData->size		= wh.length;
				curData->loopBeg 	= wh.loopstart;
				curData->loopSize 	= wh.looplength;
				
				curData->vol		= wh.volume;
				curData->c2spd		= finetune[ (wh.finetune + 128)/16];
				curData->loopType	= 0;
				curData->amp		= 8;
				
				
				
				if( wh.type & 0x10)		// 16 Bits
				{
					curData->amp = 16;
				}
				
				if( (wh.type & 0x3) == 0)	// NO LOOP
				{
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if( (wh.type & 0x3) == 1)	// Forward Loop
				{
					curData->loopType = eClassicLoop;
				}
					
				if( (wh.type & 0x3) == 2)	// Ping-pong Loop
				{
					curData->loopType = ePingPongLoop;
				}
				
			//	curData->panning	= wh.panning;
				curData->relNote	= wh.relnote;
				for( x = 0; x < 22; x++) curData->name[ x] = wh.samplename[ x];
			}
		}

		/** Read samples data **/
		
		for( u = 0 ; u < curIns->numSamples ; u++)
		{
			sData	*curData = theMAD->sample[ t*MAXSAMPLE + u];
			
			curData->data 		= MADPlugNewPtr( curData->size, init);
			if( curData->data == 0L) return MADNeedMemory;
			
			if( curData->data != 0L)
			{
				READXMFILE( curData->data, curData->size);
				
				if( curData->amp == 16)
				{
					short	*tt;
					long	tL;
					
					tt = (short*) curData->data;

					for( tL = 0; tL < curData->size/2; tL++)
					{
						INT16( (Ptr) (tt + tL));
					}
					
					{
					/* Delta to Real */
					long	oldV, newV;
					long	xL;
					
					oldV = 0;
					
					for( xL = 0; xL < curData->size/2; xL++)
					{
						newV = tt[ xL] + oldV;
						tt[ xL] = newV;
						oldV = newV;
					}
					}
				}
				else
				{
					/* Delta to Real */
					long	oldV, newV;
					long	xL;
					
					oldV = 0;

					for( xL = 0; xL < curData->size; xL++)
					{
						newV = curData->data[ xL] + oldV;
						curData->data[ xL] = newV;
						oldV = newV;
					}
				}
			}
			else return MADNeedMemory;
		}
	}
	
	
	return noErr;
}

OSErr XM_Load( Ptr	theXM, long XMSize, MADMusic *theMAD, MADDriverSettings *init)
{
	int			i;
	long		inOutCount, x;
	OSErr		iErr = noErr;

	theXMRead = theXM;

	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE( mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */

	INT16( &mh->version);
	INT32( &mh->headersize);
	INT16( &mh->songlength);
	INT16( &mh->restart);
	INT16( &mh->numchn);
	INT16( &mh->numpat);
	INT16( &mh->numins);		if( mh->numins > MAXINSTRU) { mh->numins = MAXINSTRU;	}
	INT16( &mh->flags);
	INT16( &mh->bpm);
	INT16( &mh->tempo);

	/********************/
	/** MAD ALLOCATION **/
	/********************/
	
	inOutCount = sizeof( MADSpec);
	theMAD->header = (MADSpec*) MADPlugNewPtrClear( inOutCount, init);	
	if( theMAD->header == 0L) return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	for(i=0; i<20; i++)
	{
		theMAD->header->name[ i] = mh->songname[ i];
		if( theMAD->header->name[ i] == 0) i = 21;
	}
	
	theMAD->header->speed			= 	mh->tempo;
	theMAD->header->tempo			=	mh->bpm;
	theMAD->header->numChn			=	mh->numchn;
	theMAD->header->numPat			=	mh->numpat;
	theMAD->header->numPointers		=	mh->songlength;
	
	if( theMAD->header->numPointers > 128) theMAD->header->numPointers = 128;
	
	MADstrcpy( theMAD->header->infos, (Ptr) "Converted by PlayerPRO XM Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");
	
	for( i = 0; i < mh->songlength; i++)
	{
		theMAD->header->oPointers[ i] = mh->orders[ i];
		if( theMAD->header->oPointers[ i] >= theMAD->header->numPat)
			theMAD->header->oPointers[ i] = theMAD->header->numPat-1;
	}
	
	if(mh->flags&1) theMAD->header->XMLinear = true;
	
	x = 1;
	for( i = 0; i < MAXTRACK; i++)
	{
	/*	if( x > 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
		else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		x--;
		
		if( x == -2) x = 2;*/
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
		theMAD->header->chanPan[ i] = MAX_PANNING/2;
	}

	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;

	theMAD->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;

	switch( mh->version)
	{
		case 0x104:
			XMReadPattern( theMAD, init);
			iErr = XMReadInstruments( theMAD, init);
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

long ConvertSampleC4SPDXM( Ptr src, long srcSize, short amp, long srcC4SPD, Ptr dst, long dstC4SPD)
{
	short	*src16 = (short*) src, *dst16 = (short*) dst;
	Ptr		src8 = src, dst8 = dst;
	long	x;
	
	if( amp == 8)
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

long XMGetPeriod( short note, long c2spd)
{
	unsigned long 	period, n,o, mytab[ 12] = {		1712L*16L,1616L*16L,1524L*16L,1440L*16L,1356L*16L,1280L*16L,
													1208L*16L,1140L*16L,1076L*16L,1016L*16L,960L*16L,907L*16L };
	
	n = note%12;		o = note/12;
	
	period = (( 8363UL * ( mytab[ n]) ) >> o ) / c2spd;
	
	if( period == 0L) period = 7242;
	
	return period;
}


Ptr	ConvertMad2XM( MADMusic *theMAD, MADDriverSettings *init, long *sndSize)
{
	int			u, i;
	long		PatternSize, InstruSize, NumberInstru;
	Ptr			finalXMPtr;
	
	/********************************/
	/* 			MAD INFORMATIONS    */
	/********************************/
	InstruSize 		= 0;
	PatternSize 	= 0;
	
	NumberInstru = MAXINSTRU;
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		InstruSize += sizeof( XMINSTHEADER) + sizeof( XMPATCHHEADER);
		
		for( u = 0; u < theMAD->fid[ i].numSamples; u++)
		{
			InstruSize			+= sizeof( XMWAVHEADER) + theMAD->sample[ i*MAXSAMPLE + u]->size;
		}
		
		if( theMAD->fid[ i].numSamples > 0 || theMAD->fid[ i].name[ 0] != 0)
		{
			NumberInstru = i + 1;
		}
	}
	
	for( i = 0; i < theMAD->header->numPat;i++)
	{
		PatternSize += sizeof( XMNOTE) * theMAD->header->numChn * theMAD->partition[ i]->header.size;
		PatternSize += sizeof( XMPATHEADER);
	}
	/********************************/
	
	*sndSize = 						sizeof(XMHEADER) +
												InstruSize +
												PatternSize;
	
	theXMRead = finalXMPtr = MADPlugNewPtr( *sndSize, init);
	theXMMax = theXMRead + *sndSize;
	if( theXMRead == 0L) return 0L;
	
	/********************/
	/** WRITE XM HEADER */
	/********************/
	
	
	
	MADstrcpy( mh->id, (Ptr) "Extended Module: ");
	MADstrcpy( mh->trackername, (Ptr) "FastTracker v2.00   ");
	mh->version			= 0x104;													INT16( &mh->version);
	mh->headersize		= HEADERSIZE;												INT32( &mh->headersize);
	mh->songlength 		= theMAD->header->numPointers;	INT16( &mh->songlength);
	mh->restart 		= 0;															INT16( &mh->restart);
	mh->numchn 			= theMAD->header->numChn;					INT16( &mh->numchn);
	mh->numpat 			= theMAD->header->numPat;					INT16( &mh->numpat);
	mh->numins 			= NumberInstru;								INT16( &mh->numins);
	mh->flags 			= 0;
	if( theMAD->header->XMLinear) mh->flags = 1;
	else mh->flags = 0;
	INT16( &mh->flags);
	mh->bpm 			= theMAD->header->tempo;					INT16( &mh->bpm);
	mh->tempo 			= theMAD->header->speed;					INT16( &mh->tempo);
	
	for(i=0; i<21; i++) mh->songname[ i] = ' ';
	for(i=0; i<21; i++)
	{
		mh->songname[ i] = theMAD->header->name[ i];
		if( theMAD->header->name[ i] == 0) i = 21;
	}
	mh->songname[ 20] = 0x1a;
	
	for( i = 0; i < 256; i++) mh->orders[ i] = 0;
	for( i = 0; i < theMAD->header->numPointers; i++) mh->orders[ i] = theMAD->header->oPointers[ i];
	
	WRITEXMFILE( mh, sizeof(XMHEADER));
	
	/*****************/
	/** PATTERNS    **/
	/*****************/
	
	{
	int			t, u, v;
	
	for(t=0;t<theMAD->header->numPat;t++)
	{
		short				PatternSize;
		XMPATHEADER 		ph;
		Ptr					packingCopy, cc;
		
		ph.size = PHSIZE;						INT32( &ph.size);			WRITEXMFILE( &ph.size,	4);
		ph.packing = 0;							WRITEXMFILE( &ph.packing,	1);
		ph.numrows = theMAD->partition[ t]->header.size;	INT16( &ph.numrows);		WRITEXMFILE( &ph.numrows,	2);
		
		packingCopy = theXMRead;
		ph.packsize = 1;									INT16( &ph.packsize);		WRITEXMFILE( &ph.packsize,	2);
		
		cc = theXMRead;
		for( u = 0 ; u < theMAD->partition[ t]->header.size ; u++)
		{
			for( v = 0 ; v < theMAD->header->numChn ; v++)
			{
				Cmd		*aCmd;
				Cmd		bCmd;
				
				aCmd = GetMADCommand(  u,  v, theMAD->partition[ t]);
				
				bCmd = *aCmd;
				
				XM_Convert2XM( &bCmd);
				
				XM_WriteNote( &bCmd);
			}
		}
		
		PatternSize = theXMRead - cc;
		
		cc = theXMRead;
		theXMRead = packingCopy;
		ph.packsize = PatternSize;						INT16( &ph.packsize);		WRITEXMFILE( &ph.packsize,	2);
		theXMRead = cc;
	}
	}
	
	/*****************/
	/** INSTRUMENTS **/
	/*****************/

	{
	int			t, u, x;
	long		inOutCount = 0;
	
	for( t = 0; t < NumberInstru; t++)
	{
		XMINSTHEADER 	ih;
		long			ihsizecopy, ihssizecopy;
		
		InstrData		*curIns = &theMAD->fid[ t];
		Ptr				theXMReadCopy;

		theXMReadCopy = theXMRead;
		
		//************************//
		// Instrument header size //
		
		if( curIns->numSamples > 0) ih.size = IHSIZE;
		else ih.size = IHSIZESMALL;
		
		ihsizecopy = ih.size;
		INT32( &ih.size);
		WRITEXMFILE( &ih.size,	4);
		
		//************************//
		
		for( x = 0; x < 22; x++) 			ih.name[x] = curIns->name[ x];			WRITEXMFILE( &ih.name,	22);
		ih.type = 0;																WRITEXMFILE( &ih.type,	1);
		ih.numsmp = curIns->numSamples;			INT16( &ih.numsmp);					WRITEXMFILE( &ih.numsmp,	2);
		
		ih.ssize = IHSSIZE;
		ihssizecopy = ih.ssize;
		INT32( &ih.ssize);
		
		if( curIns->numSamples > 0)
		{
			XMPATCHHEADER	pth;
			
			for( x = 0; x < sizeof( pth); x++) ((char*) &pth)[ x] = 0;
			
			BlockMoveData( curIns->what, pth.what, 96);
			
			BlockMoveData( curIns->volEnv, pth.volenv, 48);
			for( x = 0; x < 24; x++) INT16( &pth.volenv[ x]);
			
			pth.volpts = curIns->volSize;
			pth.volflg = curIns->volType;
			pth.volsus = curIns->volSus;
			pth.volbeg = curIns->volBeg;
			pth.volend = curIns->volEnd;
			pth.volfade = curIns->volFade;
			
			
			
			BlockMoveData( curIns->pannEnv, pth.panenv, 48);
			for( x = 0; x < 24; x++) INT16( &pth.panenv[ x]);
			
			pth.panpts = curIns->pannSize;
			pth.panflg = curIns->pannType;
			pth.pansus = curIns->pannSus;
			pth.panbeg = curIns->pannBeg;
			pth.panend = curIns->pannEnd;
			
			
			
			WRITEXMFILE( &ih.ssize,		4);
			WRITEXMFILE( &pth.what,		96);
			WRITEXMFILE( &pth.volenv,	48);
			WRITEXMFILE( &pth.panenv,	48);
			WRITEXMFILE( &pth.volpts,	1);
			WRITEXMFILE( &pth.panpts,	1);
			WRITEXMFILE( &pth.volsus,	1);
			WRITEXMFILE( &pth.volbeg,	1);
			WRITEXMFILE( &pth.volend,	1);
			WRITEXMFILE( &pth.pansus,	1);
			WRITEXMFILE( &pth.panbeg,	1);
			WRITEXMFILE( &pth.panend,	1);
			WRITEXMFILE( &pth.volflg,	1);
			WRITEXMFILE( &pth.panflg,	1);
			WRITEXMFILE( &pth.vibflg,	1);
			WRITEXMFILE( &pth.vibsweep,	1);
			WRITEXMFILE( &pth.vibdepth,	1);
			WRITEXMFILE( &pth.vibrate,	1);
			INT16( &pth.volfade);		WRITEXMFILE( &pth.volfade,	2);
			WRITEXMFILE( &pth.reserved,	22);
		}
		theXMRead = theXMReadCopy + ihsizecopy;
		
		/** WRITE samples */
		
		for( u = 0 ; u < curIns->numSamples ; u++)
		{
			XMWAVHEADER		wh;
			sData			*curData;
			short			modifc2spd = 0;
			long			copyc2spd;
			
			long 	finetune[ 16] = 
			{
				7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
				8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
			};
			
			curData = theMAD->sample[ t*MAXSAMPLE + u];
			
			wh.volume		= curData->vol;
			
			copyc2spd = curData->c2spd;
			
			if( curData->c2spd > 8757 || curData->c2spd < 7895)
			{
				#define BASECALC	45
				
				wh.finetune = 0;			// <- 8363 Hz
				
				while( XMGetPeriod( BASECALC, curData->c2spd) < XMGetPeriod( BASECALC + modifc2spd, 8363))
				{
					modifc2spd++;
				}
				
				curData->c2spd = 8363;
			}
			
			
			{
				wh.length 		= curData->size;
				wh.loopstart	= curData->loopBeg;
				wh.looplength	= curData->loopSize;
				
				if( curData->stereo)
				{
					wh.length /= 2;
					wh.loopstart /= 2;
					wh.looplength /= 2;
				}
				
				wh.finetune = -128;
				if( curData->c2spd > 8757) wh.finetune = 127;
				else
				{
					while( finetune[ (wh.finetune + 128)/16] < curData->c2spd)
					{
						wh.finetune += 16;
					}
				}
			}
			
			curData->c2spd = copyc2spd;
			
			wh.type = 0;
			if( curData->amp == 16) wh.type += 0x10;
			if( curData->loopSize > 0)
			{
				if( curData->loopType == ePingPongLoop) wh.type += 0x2;
				else wh.type += 0x1;
			}
			
			wh.panning = 128;	//curData->panning;
			wh.relnote = curData->relNote + modifc2spd;
			for( x = 0; x < 22; x++) wh.samplename[ x] = curData->name[ x];
			
			theXMReadCopy = theXMRead;
			INT32( &wh.length);		WRITEXMFILE( &wh.length,		4);
			INT32( &wh.loopstart);	WRITEXMFILE( &wh.loopstart,		4);
			INT32( &wh.looplength);	WRITEXMFILE( &wh.looplength,	4);
			WRITEXMFILE( &wh.volume,		1);
			WRITEXMFILE( &wh.finetune,		1);
			WRITEXMFILE( &wh.type,			1);
			wh.panning		= 128;							WRITEXMFILE( &wh.panning,		1);
			WRITEXMFILE( &wh.relnote,		1);
			WRITEXMFILE( &wh.reserved,		1);
			WRITEXMFILE( &wh.samplename,		22);
			theXMRead = theXMReadCopy + ihssizecopy;
		}
		
		/** WRITE samples data **/
		
		for( u = 0 ; u < curIns->numSamples ; u++)
		{
			sData	*curData = theMAD->sample[ t*MAXSAMPLE + u];
			Ptr		theXMReadCopy = theXMRead;
			long	curSize;
			
			WRITEXMFILE( curData->data, curData->size);
			curSize = curData->size;
			
			if( curData->stereo)
			{
				if( curData->amp == 8)
				{
					for( x = 0 ; x < curSize; x+=2)
					{
						theXMReadCopy[ x / 2] = ((long) theXMReadCopy[ x] + (long) theXMReadCopy[ x + 1]) / 2L;
					}
				}
				else
				{
					short *short16out = (short*) theXMReadCopy, *short16in = (short*) theXMReadCopy;
					
					for( x = 0 ; x < curSize/2; x+=2)
					{
						short16out[ x / 2] = ((long) short16in[ x] + (long) short16in[ x + 1]) / 2L;
					}
				}
				
				curSize /= 2;
				theXMRead -= curSize;
			}
			
			if( curData->amp == 16)
			{
				short	*tt = (short*) theXMReadCopy;
				long	tL;
				
				/* Real to Delta */
				long	oldV, newV;
				long	xL;
				
				oldV = 0;
				
				for( xL = 0; xL < curSize/2; xL++)
				{
					newV = tt[ xL];
					tt[ xL] -= oldV;
					oldV = newV;
				}
				
				for( tL = 0; tL < curSize/2; tL++)
				{
					INT16( (Ptr) (tt + tL));
				}
			}
			else
			{
				/* Real to Delta */
				long	oldV, newV;
				long	xL;
			//	Byte	*tempByte = (Byte*) theXMReadCopy;
				
				oldV = 0;
				
				for( xL = 0; xL < curSize; xL++)
				{
					newV = theXMReadCopy[ xL];
					theXMReadCopy[ xL] -= oldV;
					oldV = newV;
				}
			}
		}
	}
	}
	
	*sndSize = theXMRead - finalXMPtr;
	
	return finalXMPtr;
}

Boolean compMem( Ptr a, Ptr b, long s)
{
long 	i;

	for( i = 0; i < s; i++)
	{
		if( a[ i] != b[ i]) return false;
	}
	
	return true;
}

OSErr TestXMFile( Ptr AlienFile)
{
	if( compMem( AlienFile, "Extended Module: ", 17))
	{
		theXMRead = AlienFile;
	
		/********************/
		/** READ XM HEADER **/
		/********************/
		
		READXMFILE( mh, sizeof(XMHEADER));
		/* BigEndian <-> LittleEndian */
	
		INT16( &mh->version);
		INT16( &mh->songlength);
		INT16( &mh->numchn);
		INT16( &mh->numpat);
		INT16( &mh->numins);
	
		switch( mh->version)
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

OSErr ExtractXMInfo( PPInfoRec *info, Ptr AlienFile)
{
	short	i;
	

	theXMRead = AlienFile;

	/********************/
	/** READ XM HEADER **/
	/********************/
	
	READXMFILE( mh, sizeof(XMHEADER));
	/* BigEndian <-> LittleEndian */

	INT16( &mh->version);
	INT16( &mh->songlength);
	INT16( &mh->numchn);
	INT16( &mh->numpat);
	INT16( &mh->numins);
	INT16( &mh->flags);

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
	
	for(i=0; i<21; i++)
	{
		info->internalFileName[ i] = mh->songname[ i];
		if( info->internalFileName[ i] == 0 || info->internalFileName[ i] == 0x1a)
		{
			info->internalFileName[ i] = 0;
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
	
	if( mh->flags&1) MADstrcpy( info->formatDescription, "XM Linear Plug");
	else MADstrcpy( info->formatDescription, "XM Log Plug");

	return noErr;
}

#ifndef _MAC_H

extern "C" EXP OSErr FillPlug( PlugInfo *p);
extern "C" EXP OSErr mainPLUG( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	MADstrcpy( p->type, 		".XM");
	MADstrcpy( p->MenuName, 	"XM Files");
	p->mode	=	'EXIM';
	
	return noErr;
}
#endif

#ifdef _SRC
OSErr mainXM( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
EXP OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif

{
	OSErr			myErr;
	Ptr				AlienFile;
	long			sndSize;
	UNFILE		iFileRefI;
	
	myErr = noErr;
	
	if( !XM_Init( init)) return MADNeedMemory;
	
	switch( order)
	{
		case 'EXPL':
			AlienFile = ConvertMad2XM( MadFile, init, &sndSize);
			
			if( AlienFile != 0L)
			{
				iFileCreate( AlienFileName, 'XM  ');
				iFileRefI = iFileOpen( AlienFileName);
				if( iFileRefI)
				{
					iWrite( sndSize, AlienFile, iFileRefI);
					iClose( iFileRefI);
				}
				DisposePtr( AlienFile);	AlienFile = 0L;
			}
			else myErr = MADNeedMemory;
		break;
		
		case 'IMPL':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr( sndSize * 2L, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					DisposePtr( AlienFile);
					
					AlienFile = MADPlugNewPtr( sndSize, init);
					if( AlienFile == 0L) myErr = MADNeedMemory;
					else
					{
						iRead( sndSize, AlienFile, iFileRefI);
						
						myErr = TestXMFile( AlienFile);
						if( myErr == noErr)
						{
							myErr = XM_Load( AlienFile,  sndSize, MadFile, init);
						}
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		case 'TEST':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					
					myErr = TestXMFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;

		case 'INFO':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = TestXMFile( AlienFile);
						if( !myErr) myErr = ExtractXMInfo( info, AlienFile);
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}

	XM_Cleanup();
	
	return myErr;
}