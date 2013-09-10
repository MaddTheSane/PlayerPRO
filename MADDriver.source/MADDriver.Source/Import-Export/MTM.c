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
#include "MTM.h"

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

static inline void mystrcpy( Ptr a, BytePtr b)
{
	BlockMoveData( b + 1, a, b[ 0]);
}

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	INT16(&toswap);
	return toswap;
}

static inline UInt32 Tdecode32( void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	INT32(&toswap);
	return toswap;
}

#endif
#endif


static struct MTMTrack* GetMTMCommand( short position, short whichTracks, Ptr PatPtr)
{
	Ptr					aPtr;
	
	aPtr =	 (PatPtr +
			  whichTracks * 192L +
			  position * 3L);
	
	return (struct MTMTrack*) aPtr;
}

static OSErr ConvertMTM2Mad( MTMDef *MTMFile, long MTMSize, MADMusic *theMAD, MADDriverSettings *init)
{
	short 			i, x, z;
	long 			sndSize, OffSetToSample, MPatSize, temp, inOutCount;
	Ptr				MaxPtr;
	//OSErr			theErr;
	Ptr				theInstrument[ 64], destPtr;
	long 			finetune[16] = 
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;

	/**** Variables pour le MTM ****/
	
	struct Instru		*instru[ 64];
	struct MTMTrack		*theCom;
	Ptr					samplePtr, patPtr, positionPtr;
	short				*patTracks; 
	/********************************/

	/**** Calcul de divers offsets *****/

	MTMFile->tracks = Tdecode16( &MTMFile->tracks);
	MTMFile->comments = Tdecode16( &MTMFile->comments);
	
	MaxPtr 		= (Ptr) ( (long)	MTMFile + MTMFile);
	positionPtr	= (Ptr) ( (long)	MTMFile + 66L + MTMFile->NOS*37L);
	patPtr 		= (Ptr) ( (long)	MTMFile + 194L + MTMFile->NOS*37L);
	destPtr		= (Ptr) ( (long)	MTMFile + 194L + MTMFile->NOS*37L + MTMFile->tracks*192L);
	patTracks	= (short*) destPtr;
	samplePtr 	= (Ptr) ( (long)	MTMFile + 194L + MTMFile->NOS*37L + MTMFile->tracks*192L +
									(MTMFile->patNo + 1L)*32L*2L + MTMFile->comments);
	
	/**** Analyse des instruments ****/
	if( MTMFile->NOS > 64) return MADUnknowErr;
	
	for( i = 0, OffSetToSample = 0; i < MTMFile->NOS ; i++)
	{
		theInstrument[ i] = samplePtr + OffSetToSample;

		instru[ i] = (struct Instru*) ((long) MTMFile + 66L + i*37L);

		instru[ i]->size = Tdecode32( &instru[ i]->size);
		instru[ i]->loopBegin = Tdecode32( &instru[ i]->loopBegin);
		instru[ i]->loopEnd = Tdecode32( &instru[ i]->loopEnd);


		sndSize = instru[ i]->size;
		if( theInstrument[i] + sndSize > MaxPtr)
		{
			// sndSize = instru[ i]->size = MaxPtr - theInstrument[i];
			return MADUnknowErr;
		}
		OffSetToSample += sndSize;
	}


	/***********************************************/
	/******** Le MTM a été lu et analysé ***********/
	/***** Copie des informations dans le MAD ******/
	/***********************************************/
	
	
	
	/**************************/
	/*** MAD Initialisation ***/
	/**************************/
	
	inOutCount = sizeof( MADSpec);
	theMAD->header = (MADSpec*) MADPlugNewPtrClear( inOutCount, init);	
	if( theMAD->header == NULL) return MADNeedMemory;
	theMAD->header->MAD = 'MADK';
	
	/*****************************************/
	/*** Copie des informations MTM -> MAD ***/
	/*****************************************/
	
	for(i=0; i<22; i++)
	{
		theMAD->header->name[i] = MTMFile->songname[i];
	}
	
	mystrcpy( theMAD->header->infos, "\pConverted by PlayerPRO MTM Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->tempo = 125;
	theMAD->header->speed = 6;
	theMAD->header->numPat 				= MTMFile->patNo + 1;
	theMAD->header->numPointers			= MTMFile->positionNo;
	for(i=0; i<128; i++)
	{
		theMAD->header->oPointers[ i] 	= positionPtr[ i];
	}
	theMAD->header->numChn 				= MTMFile->trackback;

	theMAD->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));
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

	
	// INSTRUMENTS
	
	theMAD->fid = ( InstrData*) MADPlugNewPtrClear( sizeof( InstrData) * (long) MAXINSTRU, init);
	if( !theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = ( sData**) MADPlugNewPtrClear( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
	if( !theMAD->sample) return MADNeedMemory;
	
	for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	for(i = 0; i < MTMFile->NOS; i++)
	{
		for( x = 0; x < 22; x++) theMAD->fid[i].name[x] = instru[i]->name[x];
		theMAD->fid[i].type = 0;
		
		if( instru[ i]->size > 0)
		{
			sData	*curData;
			
			theMAD->fid[i].numSamples = 1;
			theMAD->fid[i].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
			
			curData->size		= instru[ i]->size;
			curData->loopBeg 	= instru[ i]->loopBegin;
			curData->loopSize 	= instru[ i]->loopEnd - instru[i]->loopBegin;
			curData->vol		= instru[ i]->volume;
			curData->c2spd		= finetune[ instru[ i]->fineTune];
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
		//	for( x = 0; x < 22; x++) curData->name[x] = instru[i]->name[x];
			
			curData->data 		= MADPlugNewPtr( curData->size, init);
			if( curData->data == NULL) DebugStr("\pInstruments: I NEED MEMORY !!! NOW !");
			
			BlockMoveData( theInstrument[i], curData->data, curData->size);
			
			destPtr = curData->data;
			for( temp = 0; temp < curData->size; temp++) *(destPtr + temp) -= 0x80;
		}
		else theMAD->fid[ i].numSamples = 0;
	}
	
	for(i=0; i<theMAD->header->numPat; i++)
	{
		
		theMAD->partition[ i] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd), init);
		if( theMAD->partition[ i] == NULL) return MADNeedMemory;

		theMAD->partition[ i]->header.size = 64L;
		theMAD->partition[ i]->header.compMode = 'NONE';

		for( x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;

		theMAD->partition[ i]->header.patBytes = 0;
		theMAD->partition[ i]->header.unused2 = 0;

		MaxPtr = (Ptr) theMAD->partition[ i];
		MaxPtr += sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd);

		for( z = 0; z < 32; z++) patTracks[ z] = Tdecode16( &patTracks[ z]);

		for(x=0; x<64; x++)
		{
			for(z=0; z<theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand(  x,  z, theMAD->partition[ i]);
				if( (Ptr) aCmd + sizeof( Cmd) > MaxPtr) return MADUnknowErr;
				
				if( patTracks[ z] == 0)
				{
					aCmd->ins		= 0;
					aCmd->note		= 0xFF;
					aCmd->cmd		= 0;
					aCmd->arg		= 0;
					aCmd->vol		= 0xFF;
				}
				else
				{
					theCom 		= GetMTMCommand(	x,
												patTracks[ z],
												(Ptr) patPtr);
					
					aCmd->ins 	= theCom->instru;
					
					if( theCom->pitch)
					{
						aCmd->note 	= theCom->pitch + 22;
					}
					else aCmd->note = 0xFF;
					
					aCmd->cmd 	= theCom->EffectCmd;
					aCmd->arg 	= theCom->EffectArg;
					aCmd->vol	= 0xFF;
				}
			}
		}
		
		/*** Avance dans les tracks-patterns suivants ***/
		patTracks += 32;
	}
	
	return noErr;
}

static OSErr ExtractInfo( PPInfoRec *info, MTMDef *myFile)
{
	//long	PatternSize;
	short	i;
	//short	maxInstru;
	//short	tracksNo;
	
	for( i = 0; i < sizeof( myFile->songname); i++)
	{
		info->internalFileName[ i] = myFile->songname[ i];
	}
	info->internalFileName[ 21] = 0;
	MYC2PStr( (Ptr) info->internalFileName);

	strcpy( info->formatDescription, "MTM Plug");

	info->totalPatterns		= myFile->patNo;
	info->partitionLength	= myFile->positionNo;
	info->totalTracks		= myFile->tracks;
	info->totalInstruments	= myFile->NOS;
	info->signature			= 'MTM ';

	return noErr;
}

static OSErr TestFile( MTMDef *myFile)
{
	if(myFile->Id[ 0] == 'M' &&
	   myFile->Id[ 1] == 'T' &&
	   myFile->Id[ 2] == 'M') return noErr;
	
	else return MADFileNotSupportedByThisPlug;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

static OSErr mainMTM( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI);
			
				// ** TEST MEMOIRE :  Environ 2 fois la taille du fichier**
				AlienFile = MADPlugNewPtr( sndSize * 2L, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** 
				
				else
				{
					DisposePtr( AlienFile);
					
					AlienFile = MADPlugNewPtr( sndSize, init);
					iRead( sndSize, AlienFile, iFileRefI);
					
					myErr = TestFile( (MTMDef*) AlienFile);
					if( myErr == noErr)
					{
						myErr = ConvertMTM2Mad( (MTMDef*) AlienFile, GetPtrSize( AlienFile), MadFile, init);
					}
						
					DisposePtr( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		case 'TEST':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead( sndSize, AlienFile, iFileRefI);
					
					myErr = TestFile( (MTMDef*) AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		case 'EXPL':
			myErr = MADOrderNotImplemented;
		break;

		case 'INFO':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead( sndSize, AlienFile, iFileRefI);
					
					myErr = ExtractInfo( info, (MTMDef*) AlienFile);
					
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
#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x73, 0xCB, 0x5A, 0x1C, 0x87, 0xA8, 0x47, 0xBE, 0x91, 0xC8, 0x78, 0xD3, 0xD2, 0xD5, 0x2B, 0x66))
//73CB5A1C-87A8-47BE-91C8-78D3D2D52B66

#define PLUGMAIN mainMTM
#define PLUGINFACTORY MTMFactory
#include "CFPlugin-bridge.c"
#else
OSErr mainPLUG( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	return mainMTM(order, AlienFileName, MadFile, info, init);
}
#endif
