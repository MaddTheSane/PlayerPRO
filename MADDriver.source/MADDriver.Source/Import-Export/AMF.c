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
#include "AMF.h"

static inline void mystrcpy( Ptr a, BytePtr b)
{
	BlockMoveData( b + 1, a, b[ 0]);
}

static Ptr			theAMFRead;

#define READAMFFILE(dst, size)	{BlockMoveData( theAMFRead, dst, size);	theAMFRead += (long) size;}

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

#ifdef _MAC_H
#define Tdecode16(msg_buf) EndianU16_LtoN(*(UInt16*)msg_buf)
#else
static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	INT16(&toswap);
	return toswap;
}
#endif

#ifdef _MAC_H
#define Tdecode32(msg_buf)  EndianU32_LtoN(*(UInt32*)msg_buf)
#else
static inline UInt32 Tdecode32( void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	INT32(&toswap);
	return toswap;
}
#endif

static OSErr AMF2Mad( Ptr AMFCopyPtr, long size, MADMusic *theMAD, MADDriverSettings *init)
{
	Byte			tempByte;
	short			i, x, noIns, tempShort, trackCount, trckPtr, t;
	long			inOutCount, OffSetToSample = 0, z;
	OSErr			theErr = noErr;
	Ptr				tempPtr;
	OSType			AMFType;
	/*SInt32			finetune[16] =
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};*/
	
	short			pan, uusize, oldIns = 1;
	
	theAMFRead = AMFCopyPtr;
	
	READAMFFILE( &AMFType, 4);		// AMF Type
	MOT32(&AMFType);
	//XXXX: Byte-swapping!
	if( AMFType >= 0x414D460C ) pan = 32;
	else pan = 16;
	
	if( AMFType == 0x414D4601 ) uusize = 3;
	else if( AMFType >= 0x414D460A ) oldIns = 0;
	else if( AMFType!= 0x414D4608 && AMFType != 0x414D4609) return MADFileNotSupportedByThisPlug;
	
	// Conversion
	theMAD->header = (MADSpec*) MADPlugNewPtrClear( sizeof( MADSpec), init);	
	if( theMAD->header == NULL) return MADNeedMemory;
	
	mystrcpy( theMAD->header->infos, "\pConverted by PlayerPRO AMF Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");
	
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
		if( theMAD->partition[ i] == NULL) return MADNeedMemory;
		
		theMAD->partition[ i]->header.size 		= patSize;
		theMAD->partition[ i]->header.compMode 	= 'NONE';
		
		for( x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		
		theMAD->partition[ i]->header.patBytes = 0;		theMAD->partition[ i]->header.unused2 = 0;
		
		for( x = 0; x < theMAD->header->numChn; x++ )
		{
			//	fread(&(module->patterns[t].track[i]),2,1,file);
			READAMFFILE( &tempShort, 2);
		}
	}
	for( i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[ i] = NULL;
	
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
				ushort oiloopstart = 0;
				ushort oiloopend = 0;
				
				curIns->numSamples = 1;
				
				curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
				
				curData->size		= Tdecode32( &oi.size);
				//FIXME: were loopstart and loopend supposed to be byteswapped on PowerPC?
				oiloopstart = Tdecode16( &oi.loopstart);
				oiloopend = Tdecode16( &oi.loopend);
				curData->loopBeg 	= oiloopstart; 
				curData->loopSize 	= oiloopend - oiloopstart;
				if( oiloopend == 65535)
				{
					curData->loopSize = curData->loopBeg = 0;
				}
				curData->vol		= oi.volume;
				curData->c2spd		= Tdecode16( &oi.rate);	//finetune[ oldMAD->fid[ i].fineTune];
				curData->loopType	= 0;
				curData->amp		= 8;
				
				curData->relNote	= 0;
				
				curData->data 		= MADPlugNewPtr( curData->size, init);
				if( curData->data == NULL) return MADNeedMemory;
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
				if( curData->data == NULL) return MADNeedMemory;
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

static OSErr TestAMFFile( Ptr AlienFile)
{
	OSType	myMADSign = *((unsigned long*) AlienFile);
	MOT32(&myMADSign);
	
	if( (myMADSign & 0xFFFFFF00) == 0x414D4600) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr ExtractAMFInfo( PPInfoRec *info, Ptr AlienFile)
{
	//long		PatternSize;
	//short		i;
	//short		tracksNo;
	
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
	
	strcpy( info->formatDescription, "AMF Plug");

	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

OSErr mainAMF( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
		
	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen(AlienFileName);
			if( iFileRefI )
			{
				sndSize = iGetEOF( iFileRefI);
			
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr( sndSize * 2L, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					DisposePtr( AlienFile);
					
					AlienFile = MADPlugNewPtr( sndSize, init);
					myErr = iRead( sndSize, AlienFile,iFileRefI);
					if( myErr == noErr)
					{
						myErr = TestAMFFile( AlienFile);
						if( myErr == noErr)
						{
							myErr = AMF2Mad( AlienFile, GetPtrSize( AlienFile), MadFile, init);
						}
					}
					DisposePtr( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		case 'TEST':
			iFileRefI = iFileOpen(AlienFileName);
			if( iFileRefI)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					myErr = TestAMFFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;

		break;

		case 'INFO':
			iFileRefI = iFileOpen(AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = ExtractAMFInfo( info, AlienFile);
					}
					DisposePtr( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;

		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}

	return myErr;
}

#ifdef _MAC_H
#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0xE9, 0x93, 0xEF, 0xD8, 0x0C, 0x3F, 0x46, 0xF6, 0x90, 0x63, 0x6E, 0x34, 0x0C, 0x17, 0x9D, 0x88))
//E993EFD8-0C3F-46F6-9063-6E340C179D88

#define PLUGMAIN mainAMF
#define PLUGINFACTORY AMFFactory
#include "CFPlugin-bridge.c"
#else
OSErr mainPLUG( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	return mainAMF(order, AlienFileName, MadFile, info, init);
}
#endif

