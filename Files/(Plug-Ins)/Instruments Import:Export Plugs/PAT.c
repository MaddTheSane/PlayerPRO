/*	PAT				*/
/*  IMPORT			*/
/*	v 1.0			*/
/*	1999 by ANR		*/

#include "PPPlug.h"
#include "sound.h"
#include "PAT.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( InstrData*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( sData**)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( short*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( FSSpec*)))
		| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif
/*
sData	* inMADCreateSample()
{
	sData	*curData;

	curData = (sData*) NewPtrClear( sizeof( sData));
	
	curData->size		= 0L;
	curData->loopBeg	= 0L;
	curData->loopSize	= 0L;
	curData->vol		= MAX_VOLUME;
	curData->c2spd		= NOFINETUNE;
	curData->loopType	= 0;
	curData->amp		= 8;
	curData->relNote	= 0;
	curData->data		= 0L;
	
	return curData;		
}*/

unsigned long Tdecode32( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return( (unsigned long) buf[3] << 24) | ( (unsigned long) buf[2] << 16) | ( (unsigned long) buf[ 1] << 8) | ( (unsigned long) buf[0]);
}

short Tdecode16( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return ( (short) buf[1] << 8) | ( (short) buf[0]);
}

OSErr TestPAT( Ptr CC)
{
	char	IDStr[ 50] = "GF1PATCH110";
	short	i;
	
	for( i = 0; i < 12; i++)
	{
		if( CC[ i] != IDStr[ i]) return MADFileNotSupportedByThisPlug;
	}
	
	return noErr;
}

OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
short			i;
Boolean			IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if( sample[ i] != 0L)
		{
			if( sample[ i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ i]->data);
				sample[ i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ i]);
			sample[ i] = 0L;
		}
	}
	
	
	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
	for( i = 0; i < 96; i++) curIns->what[ i]		= 0;
	for( i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
	}
	for( i = 0; i < 12; i++)
	{
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
	}
	for( i = 0; i < 12; i++)
	{
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
	}
	curIns->volSize		= 0;
	curIns->pannSize	= 0;
	
	curIns->volSus		= 0;
	curIns->volBeg		= 0;
	curIns->volEnd		= 0;
	
	curIns->pannSus		= 0;
	curIns->pannBeg		= 0;
	curIns->pannEnd		= 0;

	curIns->volType		= 0;
	curIns->pannType	= 0;
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
}

OSErr PATImport( InstrData *InsHeader, sData **sample, Ptr PATData)
{
	PatchHeader		*PATHeader;
	PatInsHeader	*PATIns;
	LayerHeader		*PATLayer;
	PatSampHeader	*PATSamp;
	long			i, x;
	unsigned long	  scale_table[ 200] = {
	16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
	32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
	65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
	130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
	261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
	523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
	1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
	2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
	4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150};

	
	
	// PATCH HEADER
	
	PATHeader = (PatchHeader*) PATData;
	PATData += 129;
	
	if( PATHeader->InsNo != 1) return MADFileNotSupportedByThisPlug;
	
	InsHeader->numSamples = ((long) PATHeader->LoSamp << 8) + (long) PATHeader->HiSamp;
	
	// INS HEADER -- Read only the first instrument
	
	PATIns = (PatInsHeader*) PATData;
	
	PATIns->size = Tdecode32( &PATIns->size);
	PATData += 63;
	
	for( x = 0; x < 16; x++) InsHeader->name[ x] = PATIns->name[ x];
	
	
	// LAYERS
	
	for( i = 0; i < PATIns->layer; i++) PATData += 47;
	
	
	// SAMPLES
	
	for( x = 0; x < InsHeader->numSamples; x++)
	{
		sData		*curData;
		Boolean		signedData;
		
		
		PATSamp = (PatSampHeader*) PATData;
		
		
		curData = sample[ x] = inMADCreateSample();
		
		for( i = 0; i < 6; i++) curData->name[ i] = PATSamp->name[ i];
		
		PATSamp->size		= Tdecode32( &PATSamp->size);		curData->size		= PATSamp->size;
		
		PATSamp->startLoop	= Tdecode32( &PATSamp->startLoop);	curData->loopBeg 	= PATSamp->startLoop;
		PATSamp->endLoop	= Tdecode32( &PATSamp->endLoop);	curData->loopSize 	= PATSamp->endLoop - PATSamp->startLoop;
		
		PATSamp->rate		= Tdecode16( &PATSamp->rate);		curData->c2spd		= PATSamp->rate;
		
		
		curData->vol		= 64;
		curData->loopType	= 0;
		
		if( PATSamp->Flag & 0x01)	curData->amp = 16;
		else curData->amp = 8;
		
		if( PATSamp->Flag & 0x02)	signedData = true;
		else signedData = false;
		
		if( !(PATSamp->Flag & 0x04))
		{
			curData->loopBeg = 0;
			curData->loopSize = 0;
		}
		
		if( PATSamp->Flag & 0x08) curData->loopType = ePingPongLoop;
		else curData->loopType = eClassicLoop;
		
		///////////////
		
		PATSamp->minFreq	= Tdecode32( &PATSamp->minFreq);
		PATSamp->maxFreq	= Tdecode32( &PATSamp->maxFreq);
		
		PATSamp->originRate		= Tdecode32( &PATSamp->originRate);
		
		for( i = 0; i < 107; i++)
		{
			if( scale_table[ i] >= PATSamp->originRate)
			{
				PATSamp->originRate = i;
				i = 108;
			}
		}
		
		curData->relNote = 60 - (12 + PATSamp->originRate);
		
		for( i = 0; i < 107; i++)
		{
			if( scale_table[ i] >= PATSamp->minFreq)
			{
				PATSamp->minFreq = i;
				i = 108;
			}
		}
		
		for( i = 0; i < 107; i++)
		{
			if( scale_table[ i] >= PATSamp->maxFreq)
			{
				PATSamp->maxFreq = i;
				i = 108;
			}
		}
		
		for( i = PATSamp->minFreq; i < PATSamp->maxFreq; i++)
		{
			if( i < 96 && i >= 0)
			{
				InsHeader->what[ i] = x;
			}
		}
		
		PATData += 96;
		
		
		// DATA
		
		curData->data = NewPtr( curData->size);
		
		if( curData->data != 0L)
		{
			BlockMove( PATData, curData->data, curData->size);

			if( curData->amp == 16)
			{
				short	*tt;
				long	tL;
				
				tt = (short*) curData->data;

				for( tL = 0; tL < curData->size/2; tL++)
				{
					*(tt + tL) = Tdecode16( (Ptr) (tt + tL));
					
					if( signedData) *(tt + tL) += 0x8000;
				}
			}
			else
			{
				for( i = 0; i < curData->size; i++)
				{
					if( signedData) curData->data[ i] = curData->data[i] + 0x80;
				}
			}
		}
		
		PATData += PATSamp->size;
	}
	
	return noErr;
}

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	short	iFileRefI, x;
	long	inOutCount;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutCount);
				
				theSound = NewPtr( inOutCount);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					MAD2KillInstrument( InsHeader, sample);
					
					myErr = PATImport( InsHeader, sample, theSound);
					
					DisposePtr( theSound);
				}
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutCount = 50L;
				theSound = NewPtr( inOutCount);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					myErr = TestPAT( theSound);
				}
				
				DisposePtr( theSound);
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return myErr;
}
