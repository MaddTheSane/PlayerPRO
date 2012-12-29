/*	PAT				*/
/*  IMPORT			*/
/*	v 1.0			*/
/*	1999 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
//#include <Carbon/Carbon.h>
#include "PAT.h"

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt32 Tdecode32( void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	PPLE32(&toswap);
	return toswap;
}

static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	PPLE16(&toswap);
	return toswap;
}

#endif
#endif

static OSErr TestPAT( Ptr CC)
{
	char	IDStr[ 50] = "GF1PATCH110";
	short	i;
	
	for( i = 0; i < 12; i++)
	{
		if( CC[ i] != IDStr[ i]) return MADFileNotSupportedByThisPlug;
	}
	
	return noErr;
}

static OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
	short			i;
//	Boolean			IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if( sample[ i] != NULL)
		{
			if( sample[ i]->data != NULL)
			{
				free ( sample[ i]->data);
				sample[ i]->data = NULL;
			}
			free( sample[ i]);
			sample[ i] = NULL;
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
		
		curIns->pannEnv[ i].pos		= 0;
		curIns->pannEnv[ i].val		= 0;

		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
	}
#if 0
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
#endif
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

static OSErr PATImport( InstrData *InsHeader, sData **sample, Ptr PATData)
{
	PatchHeader		*PATHeader;
	PatInsHeader	*PATIns;
//	LayerHeader		*PATLayer;
	PatSampHeader	*PATSamp;
	long			i, x;
	unsigned int	  scale_table[ 200] = {
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
		
		curData->data = malloc( curData->size);
		
		if( curData->data != NULL)
		{
			memcpy(curData->data, PATData, curData->size);

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

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFURLRef toDeref = theRef;
	if (resolveAgainstBase) {
		toDeref = CFURLCopyAbsoluteURL(theRef);
	}
	CFStringRef fileString = CFURLCopyFileSystemPath(toDeref, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	if (resolveAgainstBase) {
		CFRelease(toDeref);
	}
	return strLength;
}


OSErr mainPAT(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				CFURLRef					AlienFileCFURL,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	UNFILE	iFileRefI;
//	short	x;
	long	inOutCount;
	char *file = NULL;
	{
		CFIndex pathLen = getCFURLFilePathRepresentationLength(AlienFileCFURL, TRUE);
		file = malloc(pathLen);
		if (!file) {
			return MADNeedMemory;
		}
		Boolean pathOK = CFURLGetFileSystemRepresentation(AlienFileCFURL, true, (unsigned char*)file, pathLen);
		if (!pathOK) {
			free(file);
			return MADReadingErr;
		}

	}
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			
			iFileRefI = iFileOpenRead(file);
			if( iFileRefI != NULL)
			{
				inOutCount = iGetEOF( iFileRefI);
				
				theSound = malloc( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(inOutCount, theSound, iFileRefI);
					
					MAD2KillInstrument( InsHeader, sample);
					
					myErr = PATImport( InsHeader, sample, theSound);
					
					free( theSound);
				}
				
				iClose(iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			iFileRefI = iFileOpenRead(file);
			if( myErr == noErr)
			{
				inOutCount = 50L;
				theSound = malloc( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(inOutCount, theSound, iFileRefI);
					
					myErr = TestPAT( theSound);
				}
				
				free( theSound);
				
				iClose( iFileRefI);
			}
		}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	if (file) {
		free(file);
	}
	
	return myErr;
}

// D54EE3CC-B94C-4245-9E82-2F1D65C0009D
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0xD5, 0x4E, 0xE3, 0xCC, 0xB9, 0x4C, 0x42, 0x45, 0x9E, 0x82, 0x2F, 0x1D, 0x65, 0xC0, 0x00, 0x9D)
#define PLUGINFACTORY PATFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainPAT //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
