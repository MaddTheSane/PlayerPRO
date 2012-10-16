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
#include "669.h"

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*msg_buf)
#else
static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	INT16(&toswap);
	return toswap;
}
#endif

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

static inline void mystrcpy( Ptr a, BytePtr b)
{
	memmove( a, b + 1, b[ 0]);
}

static OSErr Convert6692Mad( Ptr	AlienFile, long MODSize, MADMusic	*theMAD, MADDriverSettings *init)
{
	SixSixNine			*the669;
	short				i, PatMax, x, z;
	SInt32				sndSize, OffSetToSample, OldTicks, temp;
	Ptr					MaxPtr;
	//OSErr				theErr;
	Ptr					theInstrument[ 64], destPtr;
	unsigned short		tempS;
	short				Note, Octave;
	Byte				*thePasByte;
	
	/**** Variables pour le MAD ****/
	Cmd					*aCmd;
	
	/**** Variables pour le MOD ****/
	
	struct PatSix		*PatInt;
	struct PatCmd		*theCommand;
	struct SampleInfo	*SInfo;
	/********************************/
	
	the669 = (SixSixNine*) AlienFile;
	
	theMAD->header = (MADSpec*) calloc( sizeof( MADSpec), 1);
	
	MaxPtr = (Ptr)((long) the669 + MODSize);
	
	OffSetToSample = 0x1f1L + (long)  the669->NOS * 25L + (long) the669->NOP * 0x600L;
	
	for( i = 0; i < the669->NOS ; i++)
	{
		temp = (long) the669;
		temp += 0x1f1L + i*25L + 13L;
		
		SInfo = (SampleInfo*) temp;
		
		SInfo->length =	Tdecode16( &SInfo->length);
		SInfo->loopStart = Tdecode16( &SInfo->loopStart);
		SInfo->loopEnd = Tdecode16( &SInfo->loopEnd);
		
		theInstrument[i] = (Ptr) ((long) the669 + (long) OffSetToSample);
		OffSetToSample += SInfo->length;
	}
	
	/******** Le 669 a été lu et analysé ***********/
	/******** Copie des informations dans le MAD ***/
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = the669->message[i];
	
	mystrcpy( theMAD->header->infos, "\pConverted by PlayerPRO 669 Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->numPointers = 128;	//the669->loopOrder;
	theMAD->header->tempo = 125;
	theMAD->header->speed = 6;
	theMAD->header->numPat = the669->NOP;
	for(i=0; i<128; i++)
	{
		theMAD->header->oPointers[ i] = the669->orderList[ i];
		if( theMAD->header->oPointers[ i] >= theMAD->header->numPat) theMAD->header->oPointers[ i] = theMAD->header->numPat-1;
	}
	theMAD->header->numChn = 8;
	
	theMAD->sets = (FXSets*) calloc( MAXTRACK * sizeof(FXSets), 1);
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;
	
	
	for( i = 0; i < MAXTRACK; i++)
	{
		if( i % 2 == 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
		else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if( !theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if( !theMAD->sample) return MADNeedMemory;
	
	for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	for(i=0; i<the669->NOS; i++)
	{
		temp = (long) the669;
		temp += 0x1f1L + i*25L + 13L;
		
		SInfo = (SampleInfo*) temp;
		
		if( SInfo->length > 0)
		{
			sData	*curData;
			
			theMAD->fid[ i].numSamples = 1;
			theMAD->fid[i].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) calloc( sizeof( sData), 1);
			
			curData->size		= SInfo->length;
			curData->loopBeg 	= 0;
			curData->loopSize 	= 0;
			curData->vol		= 64;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			//	for( x = 0; x < 22; x++) curData->name[x] = instru[i]->name[x];
			
			curData->data 		= malloc( curData->size);
			if( curData->data == NULL) //DebugStr("\pInstruments: I NEED MEMORY !!! NOW !");
				return MADNeedMemory;
			
			memmove( curData->data, theInstrument[i], curData->size);
			
			destPtr = curData->data;
			for( temp = 0; temp < curData->size; temp++) *(destPtr + temp) -= 0x80;
		}
		else theMAD->fid[ i].numSamples = 0;
	}
	
	PatInt = ( struct PatSix*) the669 + 0x1f1 + the669->NOS * 0x19;
	
	temp = (long) the669;
	temp += 0x1f1L + (long) the669->NOS * 0x19L;
	
	PatInt = ( struct PatSix*) temp;
	
	/***** TEMPORAIRE ********/
	
	theMAD->header->numChn = 8;
	//	theMAD->header->PatMax = 1;
	
	for( i = 0; i < theMAD->header->numPat; i++)
	{
		theMAD->partition[ i] = (PatData*) calloc( sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd), 1);
		theMAD->partition[ i]->header.size = 64L;
		theMAD->partition[ i]->header.compMode = 'NONE';
		for( x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		theMAD->partition[ i]->header.patBytes = 0;
		theMAD->partition[ i]->header.unused2 = 0;
		
		for( x = 0 ; x < 64; x++)
		{
			for( z = 0; z<theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand( x, z, theMAD->partition[ i]);
				
				theCommand = &PatInt[ i].Cmds[ x][ z];
				if( (Ptr) theCommand >= MaxPtr) //Debugger();
					return MADIncompatibleFile;
					
				thePasByte = ( Byte*) theCommand;
				
				if( thePasByte[0] == 0xFF)
				{
					aCmd->cmd 	= 0;
					aCmd->arg 	= 0;
					aCmd->ins 	= 0;
					aCmd->note	= 0xFF;
					aCmd->vol	= 0xFF;
				}
				else if( thePasByte[0] == 0xFE)
				{
					aCmd->ins 	= 0;
					aCmd->note 	= 0xFF;
					aCmd->cmd 	= 0;
					aCmd->arg 	= 0;
					
					aCmd->vol	= theCommand->Volume;
					aCmd->vol	= (aCmd->vol * 64) / 15;
				}
				else
				{
					aCmd->ins = theCommand->Instru + 1;
					aCmd->note = theCommand->AmigaPeriod;
					
					Note = (aCmd->note & 0xF0) >> 4;
					Octave = (aCmd->note & 0x0F);
					
					aCmd->note = Note*12 + Octave;
					
					if( aCmd->note == 0) aCmd->note = 0xFF;
					else aCmd->note += 24;
					
					aCmd->vol	= theCommand->Volume;
					aCmd->vol	= (aCmd->vol * 64) / 15;
				}
				
				if( thePasByte[2] == 0xFF)
				{
					aCmd->cmd = 0;
					aCmd->arg = 0;
				}
				else
				{
					aCmd->cmd = 0;
					aCmd->arg = 0;
				}
			}
		}
	}
	
	return noErr;
}

static OSErr Extract669Info( PPInfoRec *info, Ptr AlienFile)
{
	SixSixNine	*the669 = (SixSixNine*) AlienFile;
//	long			PatternSize;
//	short		i;
//	short		maxInstru;
//	short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = '669 ';
	
	/*** Internal name ***/
	
	the669->message[ 30] = '\0';
	strcpy( info->internalFileName, ( the669->message));

	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = 0;
	
	strcpy( info->formatDescription, "669 Plug");

	return noErr;
}

static OSErr Test669File( Ptr AlienFile)
{
	SixSixNine	*the669 = (SixSixNine*) AlienFile;

	//This seems to be endian-safe...
	if( the669->marker == 0x6669 || the669->marker == 0x6966) return   noErr;
	else return  MADFileNotSupportedByThisPlug;
}

extern OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI );
			
				// ** MEMORY Test Start
				AlienFile = malloc( sndSize * 2L);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free( AlienFile);
					
					AlienFile = malloc( sndSize);
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = Test669File( AlienFile);
						if( myErr == noErr)
						{
							myErr = Convert6692Mad( AlienFile,  sndSize, MadFile, init);
						}
					}
					free( AlienFile);	AlienFile = NULL;
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
				
				AlienFile = malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI );
					
					myErr = Test669File( AlienFile);
					
					free( AlienFile);	AlienFile = NULL;
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
				
				AlienFile = malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI );
					if( myErr == noErr)
					{
						myErr = Extract669Info( info, AlienFile);
					}
					free( AlienFile);	AlienFile = NULL;
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
