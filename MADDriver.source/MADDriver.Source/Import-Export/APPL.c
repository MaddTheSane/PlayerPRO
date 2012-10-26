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
#include <Carbon/Carbon.h>
#define BlockMoveData(x, y, z) memmove(y, x, z)
#include "MOD.h"

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

static OSErr LoadMADH( Ptr MADPtr, MADMusic *MadFile, MADDriverSettings *init)
{
	//TODO: ugh, yet another byteswap
	short 					i;
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
	
	if( MadHeader->MAD != 'MADK') return MADFileNotSupportedByThisPlug;
	
	//////////////////
	
	MadFile->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if( !MadFile->fid) return MADNeedMemory;
	
	MadFile->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if( !MadFile->sample) return MADNeedMemory;
	
	
	/**** PARTITION ****/
	for( i = MadHeader->numPat; i < MAXPATTERN; i++) MadFile->partition[ i] = NULL;
	
	for( i = 0; i < MadHeader->numPat; i++)
	{
		inOutCount = sizeof( PatHeader);
		BlockMoveData( MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
		
		inOutCount = sizeof( PatHeader) + MadHeader->numChn * tempPatHeader.size * sizeof( Cmd);
		MadFile->partition[ i] = (PatData*) malloc( inOutCount);
		if( MadFile->partition[ i] == NULL) return MADNeedMemory;
		
		BlockMoveData( MADPtr + OffSetToSample, MadFile->partition[ i], inOutCount);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	
	inOutCount = sizeof( InstrData) * (long) MadFile->header->numInstru;
	BlockMoveData( MADPtr + OffSetToSample, MadFile->fid, inOutCount);
	OffSetToSample += inOutCount;
	
	for( i = MadFile->header->numInstru-1; i >= 0 ; i--)
	{
		InstrData	*curIns = &MadFile->fid[ i];
		
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
			if( curData == NULL) return MADNeedMemory;
			
			inOutCount = sizeof( sData);
			
			BlockMoveData( MADPtr + OffSetToSample, curData, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			inOutCount = curData->size;
			
			curData->data = malloc( inOutCount);
			if( curData->data == NULL) return MADNeedMemory;
			
			BlockMoveData( MADPtr + OffSetToSample, curData->data, inOutCount);
			OffSetToSample += inOutCount;
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
				inOutCount = sizeof( FXSets);
				BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
				OffSetToSample += inOutCount;
				alpha++;
			}
		}
		
		for( i = 0; i < MadFile->header->numChn ; i++)	// Channel Effects
		{
			for( x = 0; x < 4; x++)
			{
				if( MadFile->header->chanEffect[ i][ x])
				{
					inOutCount = sizeof( FXSets);
					BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
					OffSetToSample += inOutCount;
					alpha++;
				}
			}
		}
		
	}
	
	return( noErr);
}

static OSErr TESTMADH( MADSpec* MADPtr)
{
	if( MADPtr->MAD == 'MADK') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr INFOMADF( MADSpec* MADPtr, PPInfoRec *info)
{
//	short	i;

	strcpy( info->internalFileName, MADPtr->name);
	
	strcpy( info->formatDescription, "MAD Resource (APPL)");
	
	info->totalPatterns		= MADPtr->numPat;
	info->partitionLength	= MADPtr->numPointers;
	
	info->totalTracks		= MADPtr->numChn;
	info->signature			= MADPtr->MAD;
	
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
	short			i;
	Handle			myRes;
	Boolean			hasToClose = TRUE;
	HFSUniStr255	RFName;
	
	FSPathMakeRef(AlienFileName, &fileRef, NULL);
	
	FSGetResourceForkName(&RFName);
	
	myErr = noErr;
	
	switch( order)
	{
		case 'IMPL':
			FSOpenResourceFile(&fileRef, RFName.length, RFName.unicode, fsRdPerm, &iFileRefI);
			if( iFileRefI == -1) myErr = MADUnknownErr;
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
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'TEST':
			FSOpenResourceFile(&fileRef, RFName.length, RFName.unicode, fsRdPerm, &iFileRefI);
			if( iFileRefI == -1) myErr = MADUnknowErr;
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
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'INFO':
			FSOpenResourceFile(&fileRef, RFName.length, RFName.unicode, fsRdPerm, &iFileRefI);
			if( iFileRefI == -1) myErr = MADUnknowErr;
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
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	return myErr;
}

