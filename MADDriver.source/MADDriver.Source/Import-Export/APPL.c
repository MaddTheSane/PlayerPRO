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

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#include <CoreServices/CoreServices.h>
#define BlockMoveData(x, y, z) memmove(y, x, z)
#include "MOD.h"

#if 0
static OSErr MADResetInstrument( InstrData		*curIns)
{
	short i;

	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	///
	
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
	
	curIns->volFade		= DEFAULT_VOLFADE;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;

	return noErr;
}

static OSErr MADKillInstrument( MADMusic *music, short ins)
{
	short				i;
	InstrData		*curIns;
	Boolean			IsReading;
	
	if( music == NULL) return MADParametersErr;
	
	curIns = &music->fid[ ins];
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for( i = 0; i < curIns->numSamples; i++)
	{
		if( music->sample[ ins * MAXSAMPLE + i] != NULL)
		{
			if( music->sample[ ins * MAXSAMPLE + i]->data != NULL)
			{
				free( (Ptr) music->sample[ ins * MAXSAMPLE + i]->data);
				music->sample[ ins * MAXSAMPLE + i]->data = NULL;
			}
			free( (Ptr) music->sample[ ins * MAXSAMPLE + i]);
			music->sample[ ins * MAXSAMPLE + i] = NULL;
		}
	}
	
	MADResetInstrument( curIns);
	
	music->musicUnderModification = IsReading;
	
	return noErr;
}
#endif

static OSErr LoadMADH( Ptr MADPtr, MADMusic *MadFile, MADDriverSettings *init)
{
	short 					i = 0;
	int						x = 0;
	long 					inOutCount, OffSetToSample;
	struct PatHeader		tempPatHeader;
	MADSpec					*MadHeader;
	
	/**** HEADER ****/
	MadFile->header = (MADSpec*) malloc( sizeof( MADSpec));
	if( MadFile->header == NULL) return MADNeedMemory;
	
	OffSetToSample = 0;
	memmove( MadFile->header, MADPtr, sizeof( MADSpec));
	OffSetToSample += sizeof( MADSpec);
	
	MadHeader = MadFile->header;
	PPBE32(&MadHeader->MAD);
	
	if( MadHeader->MAD != 'MADK')
	{
		free(MadFile->header);
		return MADFileNotSupportedByThisPlug;
	}
	
	PPBE16( &MadHeader->speed);
	PPBE16( &MadHeader->tempo);
	PPBE32( &MadHeader->ESpeed);
	PPBE32( &MadHeader->EPitch);
	
	//////////////////
	
	MadFile->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if( !MadFile->fid)
	{
		free(MadFile->header);
		return MADNeedMemory;
	}
	
	MadFile->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if( !MadFile->sample)
	{
		free(MadFile->header);
		free(MadFile->fid);
		return MADNeedMemory;
	}
	
	
	/**** PARTITION ****/
	for( i = MadHeader->numPat; i < MAXPATTERN; i++) MadFile->partition[ i] = NULL;
	
	for( i = 0; i < MadHeader->numPat; i++)
	{
		inOutCount = sizeof( PatHeader);
		BlockMoveData( MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
		
		PPBE32( &tempPatHeader.size);
		PPBE32( &tempPatHeader.compMode);
		PPBE32( &tempPatHeader.patBytes);
		PPBE32( &tempPatHeader.unused2);
		
		inOutCount = sizeof( PatHeader) + MadHeader->numChn * tempPatHeader.size * sizeof( Cmd);
		MadFile->partition[ i] = (PatData*) malloc( inOutCount);
		if( MadFile->partition[ i] == NULL)
		{
			for( x = 0; x < i; x++)
			{
				if( MadFile->partition[ x] != NULL)	free( MadFile->partition[ x]);
			}
			free( MadFile->header);
			free(MadFile->fid);
			free(MadFile->sample);
			
			return MADNeedMemory;
		}
		
		BlockMoveData( MADPtr + OffSetToSample, MadFile->partition[ i], inOutCount);
		PPBE32( &MadFile->partition[ i]->header.size);
		PPBE32( &MadFile->partition[ i]->header.compMode);
		PPBE32( &MadFile->partition[ i]->header.patBytes);
		PPBE32( &MadFile->partition[ i]->header.unused2);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	
	inOutCount = sizeof( InstrData) * MadFile->header->numInstru;
	BlockMoveData( MADPtr + OffSetToSample, MadFile->fid, inOutCount);
	OffSetToSample += inOutCount;
	
	for( i = MadFile->header->numInstru-1; i >= 0 ; i--)
	{
		InstrData	*curIns = &MadFile->fid[ i];
		int x = 0;
		PPBE16( &curIns->numSamples);
		PPBE16( &curIns->firstSample);
		PPBE16( &curIns->volFade);
		
		for( x = 0; x < 12; x++)
		{
			PPBE16( &curIns->volEnv[ x].pos);
			PPBE16( &curIns->volEnv[ x].val);
			
			PPBE16( &curIns->pitchEnv[ x].pos);
			PPBE16( &curIns->pitchEnv[ x].val);
			
			PPBE16( &curIns->pannEnv[ x].pos);
			PPBE16( &curIns->pannEnv[ x].val);
		}
		
#if 0
		for( x = 0; x < 12; x++)
		{
			PPBE16( &curIns->pannEnv[ x].pos);
			PPBE16( &curIns->pannEnv[ x].val);
		}
#endif
		
		if( i != curIns->no)
		{
			MadFile->fid[ curIns->no] = *curIns;
			MADResetInstrument( curIns);
		}
	}
	MadFile->header->numInstru = MAXINSTRU;
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		short x;
		
		for( x = 0; x < MadFile->fid[ i].numSamples ; x++)
		{
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = MadFile->sample[ i*MAXSAMPLE + x] = (sData*) malloc( sizeof( sData));
			if( curData == NULL)
			{
				for( x = 0; x < MAXINSTRU ; x++) MADKillInstrument( MadFile, x);
				
				for( x = 0; x < MadFile->header->numPat; x++)
				{
					if( MadFile->partition[ x] != NULL)	free( MadFile->partition[ x]);
				}
				free( MadFile->header);
				
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof( sData);
			
			BlockMoveData( MADPtr + OffSetToSample, curData, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			PPBE32( &curData->size);
			PPBE32( &curData->loopBeg);
			PPBE32( &curData->loopSize);
			PPBE16( &curData->c2spd);

			inOutCount = curData->size;
			
			curData->data = malloc( inOutCount);
			if( curData->data == NULL)
			{
				for( x = 0; x < MAXINSTRU ; x++) MADKillInstrument( MadFile, x);
				
				for( x = 0; x < MadFile->header->numPat; x++)
				{
					if( MadFile->partition[ x] != NULL)	free( MadFile->partition[ x]);
				}
				free( MadFile->header);
				
				return MADNeedMemory;
			}
			
			BlockMoveData( MADPtr + OffSetToSample, curData->data, inOutCount);
			OffSetToSample += inOutCount;
			if( curData->amp == 16)
			{
				SInt32 	ll;
				short	*shortPtr = (short*) curData->data;
				
				for( ll = 0; ll < curData->size/2; ll++) PPBE16( &shortPtr[ ll]);
			}

		}
	}
	
	for( i = 0; i < MAXINSTRU; i++) MadFile->fid[ i].firstSample = i * MAXSAMPLE;
	
	/*********************/
	
	{
		short	alpha, x;
		
		MadFile->sets = (FXSets*) calloc( MAXTRACK * sizeof(FXSets), 1);
		
		alpha = 0;
		
		for( i = 0; i < 10 ; i++)	// Global Effects
		{
			if( MadFile->header->globalEffect[ i])
			{
				int x = 0;
				inOutCount = sizeof( FXSets);
				BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
				OffSetToSample += inOutCount;
				PPBE16(&MadFile->sets[ alpha].id);
				PPBE16(&MadFile->sets[ alpha].noArg);
				PPBE16(&MadFile->sets[ alpha].track);
				PPBE32(&MadFile->sets[ alpha].FXID);
				for (x = 0; x < 100; x++) {
					PPBE32(&MadFile->sets[ alpha].values[x]);
				}
				alpha++;
			}
		}
		
		for( i = 0; i < MadFile->header->numChn ; i++)	// Channel Effects
		{
			for( x = 0; x < 4; x++)
			{
				if( MadFile->header->chanEffect[ i][ x])
				{
					int y = 0;
					inOutCount = sizeof( FXSets);
					BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
					OffSetToSample += inOutCount;
					PPBE16(&MadFile->sets[ alpha].id);
					PPBE16(&MadFile->sets[ alpha].noArg);
					PPBE16(&MadFile->sets[ alpha].track);
					PPBE32(&MadFile->sets[ alpha].FXID);
					for (y = 0; y < 100; y++) {
						PPBE32(&MadFile->sets[ alpha].values[y]);
					}
					alpha++;
				}
			}
		}
		
	}
	
	return( noErr);
}

static OSErr TESTMADH( MADSpec* MADPtr)
{
	OSType madType = MADPtr->MAD;
	PPBE32(&madType);
	if( madType == 'MADK') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr INFOMADF( MADSpec* MADPtr, PPInfoRec *info)
{
//	short	i;

	strcpy( info->internalFileName, MADPtr->name);
	
	strcpy( info->formatDescription, "MADK Resource (APPL)");
	
	info->totalPatterns		= MADPtr->numPat;
	info->partitionLength	= MADPtr->numPointers;
	
	info->totalTracks		= MADPtr->numChn;
	info->signature			= MADPtr->MAD;
	PPBE32(&info->signature);
	
	info->totalInstruments	= MADPtr->numInstru;
	
	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

extern OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	//TODO: learn how to get resource forks using POSIX commands or CoreFoundation, but not Carbon
	OSErr			myErr;
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	Handle			myRes;
	Boolean			hasToClose = TRUE;
	
	if (FSPathMakeRef((UInt8*)AlienFileName, &fileRef, NULL) != noErr) return MADReadingErr;
	
	myErr = noErr;
	
	switch( order)
	{
		case 'IMPL':
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if( iFileRefI == -1) myErr = MADReadingErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					DetachResource( myRes);
					HLock( myRes);
					
					myErr = LoadMADH( *myRes, MadFile, init);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADReadingErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'TEST':
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if( iFileRefI == -1) myErr = MADReadingErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					DetachResource( myRes);
					
					HLock( myRes);
					
					myErr = TESTMADH( (MADSpec*) *myRes);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADReadingErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'INFO':
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if( iFileRefI == -1) myErr = MADReadingErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					info->fileSize = GetResourceSizeOnDisk( myRes);
					
					DetachResource( myRes);
					HLock( myRes);
					
					myErr = INFOMADF( (MADSpec*) *myRes, info);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADReadingErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	return myErr;
}

