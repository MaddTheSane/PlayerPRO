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

#include "MAD.h"
#include "AMF.h"
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

static Ptr			theAMFRead;

#define READAMFFILE(dst, size)	{BlockMoveData( theAMFRead, dst, size);	theAMFRead += (long) size;}


Ptr MADPlugNewPtr( long size, MADDriverSettings* init)
{
	if( init->sysMemory) return NewPtrSys( size);
	else return NewPtr( size);
}

Ptr MADPlugNewPtrClear( long size, MADDriverSettings* init)
{
	if( init->sysMemory) return NewPtrSysClear( size);
	else return NewPtrClear( size);
}

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

void pStrcpy(register unsigned char *s1, register unsigned char *s2)
{
	register short len, i;
	
	len = *s2;
	for ( i = 0; i <= len; i++) s1[ i] = s2[ i];
}

short Tdecode16( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return ( (short) buf[1] << 8) | ( (short) buf[0]);
}

unsigned long Tdecode32( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return( (unsigned long) buf[3] << 24) | ( (unsigned long) buf[2] << 16) | ( (unsigned long) buf[ 1] << 8) | ( (unsigned long) buf[0]);
}

OSErr AMF2Mad( Ptr AMFCopyPtr, long size, MADMusic *theMAD, MADDriverSettings *init)
{
Byte					tempByte;
short 					i, x, noIns, tempShort, trackCount, trckPtr, t;
long 					inOutCount, OffSetToSample = 0L, z;
OSErr					theErr = noErr;
Ptr						tempPtr;
OSType					AMFType;
long		 			finetune[16] = 
						{
							8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
							7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
						};

short					pan, uusize, oldIns = 1;

theAMFRead = AMFCopyPtr;

READAMFFILE( &AMFType, 4);		// AMF Type
if( AMFType >= 0x414D460C ) pan = 32;
else pan = 16;

if( AMFType == 0x414D4601 ) uusize = 3;
else if( AMFType >= 0x414D460A ) oldIns = 0;
else if( AMFType!= 0x414D4608 && AMFType != 0x414D4609) return MADFileNotSupportedByThisPlug;

// Conversion
theMAD->header = (MADSpec*) MADPlugNewPtrClear( sizeof( MADSpec), init);	
if( theMAD->header == 0L) return MADNeedMemory;

mystrcpy( theMAD->header->infos, (Ptr) "\pConverted by PlayerPRO AMF Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");

theMAD->header->MAD = 'MADK';

READAMFFILE( theMAD->header->name, 32);
READAMFFILE( &tempByte, 1);		noIns = tempByte;
READAMFFILE( &tempByte, 1);		theMAD->header->numPat = tempByte;
READAMFFILE( &tempShort, 2);	trackCount = Tdecode16( &tempShort);

if( AMFType >= 0x414D4609 )
{
    READAMFFILE( &tempByte, 1);		theMAD->header->numChn = tempByte;
	
    READAMFFILE( &tempByte, pan);
    if( AMFType < 0x414D460B )
    {
        //memcpy(&module->channelPanning,order16,16);
    }
}
else theMAD->header->numChn = 4;
if( AMFType >= 0x414D460D )
{
	READAMFFILE( &tempByte, 1);		theMAD->header->tempo = tempByte;
	READAMFFILE( &tempByte, 1);		theMAD->header->speed = tempByte;
}
else
{
	theMAD->header->speed			= 6;
	theMAD->header->tempo			= 125;
}

//theMAD->header->numPointers		= oldMAD->numPointers;
//BlockMoveData( oldMAD->oPointers, theMAD->header->oPointers, 128);

theMAD->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));
for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;
/**** Patterns *******/

for( i = 0; i < theMAD->header->numPat; i++ )
{
	long patSize;
	
	if( AMFType >= 0x414D460E )
	{
		READAMFFILE( &tempShort, 2);	patSize = Tdecode16( &tempShort);
	}
    else patSize = 64;

	theMAD->partition[ i] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * patSize * sizeof( Cmd), init);
	if( theMAD->partition[ i] == 0L) return MADNeedMemory;
	
	theMAD->partition[ i]->header.size 		= patSize;
	theMAD->partition[ i]->header.compMode 	= 'NONE';
	
	for( x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
	
	theMAD->partition[ i]->header.patBytes = 0L;		theMAD->partition[ i]->header.unused2 = 0L;
	
    for( x = 0; x < theMAD->header->numChn; x++ )
    {
    //	fread(&(module->patterns[t].track[i]),2,1,file);
    	READAMFFILE( &tempShort, 2);
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

for( i = 0; i < noIns; i++)
{
	InstrData		*curIns = &theMAD->fid[ i];
	
	if( oldIns )
	{
		OLDINSTRUMENT	oi;
		
		READAMFFILE( &oi, sizeof( OLDINSTRUMENT));
		
		
		BlockMoveData( oi.name, curIns->name, 32);
		curIns->type = 0;
		
		if( oi.size > 0)
		{
			sData	*curData;
			
			curIns->numSamples = 1;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
			
			curData->size		= Tdecode32( &oi.size);
			curData->loopBeg 	= oi.loopstart;
			curData->loopSize 	= oi.loopend - oi.loopstart;
			if( oi.loopend == 65535)
			{
				curData->loopSize = curData->loopBeg = 0;
			}
			curData->vol		= oi.volume;
			curData->c2spd		= oi.rate;	//finetune[ oldMAD->fid[ i].fineTune];
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			
			curData->data 		= MADPlugNewPtr( curData->size, init);
			if( curData->data == 0L) return MADNeedMemory;
		}
		else curIns->numSamples = 0;
	}
	else
	{
		INSTRUMENT		oi;
		
		READAMFFILE( &oi, sizeof( INSTRUMENT));
		theAMFRead--;
		
		BlockMoveData( oi.name, curIns->name, 32);
		curIns->type = 0;
		
		if( oi.size > 0)
		{
			sData	*curData;
			
			curIns->numSamples = 1;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
			
			curData->size		= Tdecode32( &oi.size);
			curData->loopBeg 	= Tdecode32( &oi.loopstart);
			curData->loopSize 	= Tdecode32( &oi.loopend) - Tdecode32( &oi.loopstart);
			if( oi.loopend == 65535)
			{
				curData->loopSize = curData->loopBeg = 0;
			}
			curData->vol		= oi.volume;
			curData->c2spd		= NOFINETUNE;	//oi.rate;	//finetune[ oldMAD->fid[ i].fineTune];
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			
			curData->data 		= MADPlugNewPtr( curData->size, init);
			if( curData->data == 0L) return MADNeedMemory;
		}
		else curIns->numSamples = 0;
	}
}

trckPtr = 0;
for( t = 0; t < trackCount; t++ )
{
	READAMFFILE( &tempShort, 2);	tempShort = Tdecode16( &tempShort);
	if( tempShort > trckPtr) trckPtr = tempShort;
}

for( t = 0; t < trckPtr; t++)
{
	READAMFFILE( &tempShort, 2);		tempShort = Tdecode16( &tempShort);
	READAMFFILE( &tempByte, 1);
	
    if( tempShort == 0 ) t=t;
    else
    {
    	Ptr tPtr = NewPtr( tempShort * 3 + size);
        READAMFFILE( tPtr,tempShort * 3 + size);
        DisposePtr( tPtr);
    }
}

for( i = 0; i < noIns; i++)
{
	InstrData		*curIns = &theMAD->fid[ i];
	
	if( curIns->numSamples > 0)
	{
		sData	*curData;
		
		curData = theMAD->sample[ i*MAXSAMPLE + 0];
		
		READAMFFILE( curData->data, curData->size);
	}
}

return noErr;
}

OSErr TestAMFFile( Ptr AlienFile)
{
	unsigned long	*myMADSign = (unsigned long*) AlienFile;
	
	if( (*myMADSign & 0xFFFFFF00) == 0x414D4600) return noErr;
	else return MADFileNotSupportedByThisPlug;
	
	return noErr;
}

OSErr ExtractAMFInfo( PPInfoRec *info, Ptr AlienFile)
{
	long		PatternSize;
	short		i;
	short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = 'AMF ';
	
	/*** Internal name ***/
	
	info->internalFileName[ 31] = '\0';

	/*** Tracks ***/
	
	info->totalTracks = 0;
		
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;
	
	/*** Total Instruments ***/
	info->totalInstruments = 0;
	
	pStrcpy( (unsigned char*) info->formatDescription, "\pAMF Plug");

	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

#ifdef _SRC
OSErr mainAMF( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
EXP OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif

//OSErr main( OSType order, char *AlienFileFSSpec, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	short	iFileRefI;
	long	sndSize;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif
	
	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
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
						myErr = TestAMFFile( AlienFile);
						if( myErr == noErr)
						{
							myErr = AMF2Mad( AlienFile, GetPtrSize( AlienFile), MadFile, init);
						}
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
		break;
		
		case 'TEST':
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					myErr = TestAMFFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
		break;

		case 'INFO':
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
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
						myErr = ExtractAMFInfo( info, AlienFile);
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
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