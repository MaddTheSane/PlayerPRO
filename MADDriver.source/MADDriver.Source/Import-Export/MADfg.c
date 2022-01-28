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

#include "MOD.h"
#include "MAD.h"
#include "MADFG.h"
#include "RDriver.h"

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

Ptr MADPlugNewPtr( long size, MADDriverSettings* init)
{
	return NewPtr( size);
}

Ptr MADPlugNewPtrClear( long size, MADDriverSettings* init)
{
	return NewPtrClear( size);
}

struct MusicPattern* oldDecompressPartitionMAD1( struct MusicPattern* myPat, short Tracks, MADDriverSettings *init)
{
	struct MusicPattern*	finalPtr;
	Byte 					*srcPtr;
	struct Command			*myCmd;
	short					maxCmd;
	
	finalPtr = ( struct MusicPattern*) MADPlugNewPtr( sizeof( struct oldPatHeader) + myPat->header.PatternSize * Tracks * sizeof( struct Command), init);
	if( finalPtr == 0L) DebugStr("\pDecompressPartitionMAD1");
	
	BlockMoveData( myPat, finalPtr, sizeof( struct oldPatHeader));

	srcPtr = (Byte*) myPat->Commands;
	myCmd = (struct Command*) finalPtr->Commands;
	maxCmd = finalPtr->header.PatternSize * Tracks;
	
	/*** Decompression Routine ***/

	/*
	First Byte = 0x03 -> Instrument + AmigaPeriod + Effect + Cmd
	First Byte = 0x02 -> Instrument + AmigaPeriod
	First Byte = 0x01 -> Effect + Cmd
	First Byte = 0x00 -> nothing
	*/
	
	while( maxCmd != 0)
	{
		maxCmd--;
		
		switch( *srcPtr)
		{
			case 0x03:
				srcPtr++;
				
				((short*)myCmd)[ 0] = *((short*) srcPtr);
				((short*)myCmd)[ 1] = ((short*) srcPtr)[ 1];
				
				srcPtr += 4;
			break;
			
			case 0x02:
				srcPtr++;
								
				((short*)myCmd)[ 0] = *((short*) srcPtr);
				((short*)myCmd)[ 1] = 0L;
				
				srcPtr += 2;
			break;
			
			case 0x01:
				srcPtr++;
				
				((short*)myCmd)[ 0] = 0L;
				((short*)myCmd)[ 1] = *((short*) srcPtr);
				
				srcPtr += 2;
			break;
			
			case 0x00:
				srcPtr ++;
				
				*((long*)myCmd) = 0L;
			break;
			
			default:
				DebugStr("\pDecompress MAD1 failed.");
			break;
		}
		myCmd++;
	}
	
	return finalPtr;
}

struct Command* GetOldCommand( short PosX, short	TrackIdX, struct MusicPattern*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.PatternSize) PosX = tempMusicPat->header.PatternSize -1;
		
	return( & (tempMusicPat->Commands[ (tempMusicPat->header.PatternSize * TrackIdX) + PosX]));
}

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}


void mystrcpy( Ptr a, Ptr b)
{
	BlockMoveData( b + 1, a, b[ 0]);
}

OSErr MADFG2Mad( Ptr MADPtr, long size, MADMusic *theMAD, MADDriverSettings *init)
{
short 					i, x;
long 					inOutCount, OffSetToSample = 0L, z;
OSErr					theErr = noErr;
Boolean					MADConvert = false;
Ptr						tempPtr;
long 			finetune[16] = 
				{
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
				};


/**** Old MADFG variables ****/

oldMADSpec				*oldMAD;

oldMAD = (oldMADSpec*) MADPtr;


/**** HEADER ****/
if( oldMAD->MADIdentification == 'MADF') MADConvert = true;
else if( oldMAD->MADIdentification == 'MADG') MADConvert = false;
else return MADFileNotSupportedByThisPlug;
OffSetToSample += sizeof( oldMADSpec);

// Conversion
inOutCount = sizeof( MADSpec);
theMAD->header = (MADSpec*) MADPlugNewPtrClear( inOutCount, init);	
if( theMAD->header == 0L) return MADNeedMemory;

theMAD->header->MAD = 'MADK';

BlockMoveData( oldMAD->NameSignature, theMAD->header->name, 32);
theMAD->header->numPat			= oldMAD->PatMax;
theMAD->header->numChn			= oldMAD->Tracks;
theMAD->header->numPointers		= oldMAD->numPointers;
BlockMoveData( oldMAD->oPointers, theMAD->header->oPointers, 128);
theMAD->header->speed			= 6;
theMAD->header->tempo			= 125;

mystrcpy( theMAD->header->infos, (Ptr) "\pConverted by PlayerPRO MAD-F-G Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");


theMAD->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));
for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;

/**** Patterns *******/

for( i = 0; i < oldMAD->PatMax; i++)
{
	struct MusicPattern		*tempPat, *tempPat2;
	struct oldPatHeader		tempPatHeader;
	
	/** Lecture du header de la partition **/
	if( !MADConvert)
	{
		inOutCount = sizeof( struct oldPatHeader);
		
		BlockMoveData( MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
	}
	else tempPatHeader.PatternSize = 64L;
	
	/*************************************************/
	/** Lecture du header + contenu de la partition **/
	/*************************************************/
	
	if( tempPatHeader.CompressionMode == 'MAD1')
	{
		inOutCount = sizeof( struct MusicPattern) + tempPatHeader.PatBytes;
	}
	else
	{
		inOutCount = sizeof( struct MusicPattern) + oldMAD->Tracks * tempPatHeader.PatternSize * sizeof( struct Command);
	}
	
	tempPat = (struct MusicPattern*) MADPlugNewPtr( inOutCount, init);
	if( tempPat == 0L) DebugStr("\pMemory Prob1");
	
	if( MADConvert)
	{
		tempPat = (struct MusicPattern*) ((Ptr) tempPat + sizeof( struct oldPatHeader));
		inOutCount -= sizeof( struct oldPatHeader);
	}
	
	BlockMoveData( MADPtr + OffSetToSample, tempPat, inOutCount);
	OffSetToSample += inOutCount;
	
	if( MADConvert)
	{
		tempPat = (struct MusicPattern*) ((Ptr) tempPat - sizeof( struct oldPatHeader));
		tempPat->header.PatternSize = 64L;
		tempPat->header.CompressionMode = 'NONE';
		
		for( x = 0; x < 20; x++) tempPat->header.PatternName[ x] = 0;
		
		tempPat->header.unused2 = 0L;
	}
	
	if( tempPat->header.CompressionMode == 'MAD1')
	{
		tempPat2 = oldDecompressPartitionMAD1( tempPat, oldMAD->Tracks, init);
		
		DisposePtr( (Ptr) tempPat);
		
		tempPat = tempPat2;
	}
	
	/**************/
	/* CONVERSION */
	/**************/
	
	theMAD->partition[ i] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * tempPat->header.PatternSize * sizeof( Cmd), init);
	if( theMAD->partition[ i] == 0L) return MADNeedMemory;
	
	theMAD->partition[ i]->header.size 		= tempPat->header.PatternSize;
	theMAD->partition[ i]->header.compMode 	= 'NONE';
	
	BlockMoveData( tempPat->header.PatternName, theMAD->partition[ i]->header.name, 20);
	
	theMAD->partition[ i]->header.patBytes = 0L;		theMAD->partition[ i]->header.unused2 = 0L;
	
	for( x = 0; x < theMAD->partition[ i]->header.size; x++)
	{
		for( z = 0; z < theMAD->header->numChn; z++)
		{
			struct Command *oldCmd;
			Cmd	*aCmd;
			
			aCmd = GetMADCommand(  x,  z, theMAD->partition[ i]);
			
			oldCmd 	= GetOldCommand(	x,
										z,
										tempPat);
			
			aCmd->ins 	= oldCmd->InstrumentNo;
			if( oldCmd->AmigaPeriod == 0L) aCmd->note = 0xFF;
			else aCmd->note 	= oldCmd->AmigaPeriod + 22;
			aCmd->cmd 	= oldCmd->EffectCmd;
			aCmd->arg 	= oldCmd->EffectArg;
			aCmd->vol	= 0xFF;
			
			if( aCmd->cmd == 0x0C)
			{
				aCmd->vol	= 0x10 + (aCmd->arg & 0x00FF);
				aCmd->cmd 	= 0;
				aCmd->arg 	= 0;
			}
		}
	}
}
for( i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[ i] = 0L;

for( i = 0; i < MAXTRACK; i++)
{
	if( i % 2 == 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
	else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
	
	theMAD->header->chanVol[ i] = MAX_VOLUME;
}

	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;

/**** Instruments header *****/

theMAD->fid = ( InstrData*) MADPlugNewPtrClear( sizeof( InstrData) * (long) MAXINSTRU, init);
if( !theMAD->fid) return MADNeedMemory;

theMAD->sample = ( sData**) MADPlugNewPtrClear( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
if( !theMAD->sample) return MADNeedMemory;

for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;

for( i = 0; i < 64; i++)
{
	InstrData	*curIns = &theMAD->fid[ i];
	
	BlockMoveData( oldMAD->fid[ i].Filename, curIns->name, 32);
	curIns->type = 0;
	
	if( oldMAD->fid[ i].insSize > 0)
	{
		sData	*curData;
		
		curIns->numSamples = 1;
		curIns->volFade = DEFAULT_VOLFADE;
		
		curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
		
		curData->size		= oldMAD->fid[ i].insSize;
		curData->loopBeg 	= oldMAD->fid[ i].loopStart;
		curData->loopSize 	= oldMAD->fid[ i].loopLenght;
		curData->vol		= oldMAD->fid[ i].volume;
		curData->c2spd		= finetune[ oldMAD->fid[ i].fineTune];
		curData->loopType	= 0;
		curData->amp		= oldMAD->fid[ i].amplitude;
		
		curData->relNote	= 0;
		
		curData->data 		= MADPlugNewPtr( curData->size, init);
		if( curData->data == 0L) return MADNeedMemory;
		
		BlockMoveData( MADPtr + OffSetToSample, curData->data, curData->size);
		OffSetToSample += curData->size;
	}
	else curIns->numSamples = 0;
}

return noErr;
}

OSErr TestoldMADFile( Ptr AlienFile)
{
OSType	*myMADSign = (OSType*) AlienFile;

if(	*myMADSign == 'MADF' || *myMADSign == 'MADG') return   noErr;
else return  MADFileNotSupportedByThisPlug;
}

OSErr ExtractoldMADInfo( PPInfoRec *info, Ptr AlienFile)
{
	oldMADSpec	*myMOD = ( oldMADSpec*) AlienFile;
	long		PatternSize;
	short		i;
	short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = myMOD->MADIdentification;
	
	/*** Internal name ***/
	
	myMOD->NameSignature[ 31] = '\0';
	MADstrcpy( info->internalFileName, myMOD->NameSignature);

	/*** Tracks ***/
	
	info->totalTracks = myMOD->Tracks;
		
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	for( i = 0; i < 128; i++)
	{
		if( myMOD->oPointers[ i] >= info->totalPatterns)	info->totalPatterns = myMOD->oPointers[ i];
	}
	info->totalPatterns++;
	
	/*** Partition Length ***/
	
	info->partitionLength = myMOD->numPointers;
	
	/*** Total Instruments ***/
	
	for( i = 0, info->totalInstruments = 0; i < MAXINSTRU ; i++)
	{
		if( myMOD->fid[ i].insSize > 5) info->totalInstruments++;
	}
	
	MADstrcpy( info->formatDescription, "MAD-FG Plug");

	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

#ifdef _SRC
OSErr mainMADfg( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
EXP OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif


//OSErr main( OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	short	vRefNum, iFileRefI;
	long	dirID, sndSize;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif

//	MYC2PStr( AlienFileName);
	
	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				GetEOF( iFileRefI, &sndSize);
			
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr( sndSize * 2L, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					DisposePtr( AlienFile);
					
					AlienFile = MADPlugNewPtr( sndSize, init);
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					if( myErr == noErr)
					{
						myErr = TestoldMADFile( AlienFile);
						if( myErr == noErr)
						{
							myErr = MADFG2Mad( AlienFile, GetPtrSize( AlienFile), MadFile, init);
						}
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		case 'TEST':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					myErr = TestoldMADFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;

		case 'INFO':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				GetEOF( iFileRefI, &info->fileSize);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					if( myErr == noErr)
					{
						myErr = ExtractoldMADInfo( info, AlienFile);
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}

//	MYP2CStr( (unsigned char*) AlienFileName);

	#ifndef powerc
		SetA4( oldA4);
	#endif
	return myErr;
}