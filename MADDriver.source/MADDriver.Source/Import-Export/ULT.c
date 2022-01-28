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

#include "ULT.h"
#include "MAD.h"
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

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

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

Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

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

void pStrcpy(register unsigned char *s1, register unsigned char *s2)
{
	register short len, i;
	
	len = *s2;
	for ( i = 0; i <= len; i++) s1[ i] = s2[ i];
}

void strncpy( Ptr dst, Ptr str, long size)
{
	BlockMoveData( str, dst, size);
}


void mystrcpy( Ptr a, Ptr b)
{
	BlockMoveData( b + 1, a, b[ 0]);
}

OSErr ConvertULT2Mad( Ptr theULT, long MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	long 				i, PatMax, x, z, channel, Row;
	long 				sndSize, starting, RES;
	Ptr					MaxPtr;
	OSErr				theErr;
	Ptr					theInstrument[ 64], destPtr;
	Byte				tempChar, *theULTCopy;
	short				Note, Octave, maxTrack;
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;

	/**** Variables pour le ULT ****/
	
	ULTForm			ULTinfo;
	ULTSuite		ULTSuite;
	/********************************/

	for( i = 0 ; i < 64; i ++)
	{
		theInstrument[ i] = 0L;
	}

	/**** Header principal *****/
	theULTCopy = (Byte*) theULT;
	
	BlockMoveData( theULTCopy, &ULTinfo, sizeof( ULTinfo));
	
//	if( ULTinfo.reserved != 0) return MADFileNotSupportedByThisPlug;	// RES in v.1.4 see doc
	
	ULTSuite.NOS = *(theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L);
	
	
	/**** Ins Num *****/
	if( sizeof( ULTIns) != 64) DebugStr("\pULTIns != 64");
	ULTSuite.ins = (ULTIns*) NewPtrClear( ULTSuite.NOS * sizeof( ULTIns));
	BlockMoveData( theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L + 1, ULTSuite.ins, ULTSuite.NOS * sizeof( ULTIns));
	
	/**** Copy last infos *****/
	BlockMoveData( theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L + 1 + (ULTSuite.NOS * sizeof( ULTIns)), &ULTSuite.pattSeq, 256 + 2);
	
	// ******** Le ULT a ŽtŽ lu et analysŽ ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*) MADPlugNewPtrClear( sizeof( MADSpec), init);
	if( theMAD->header == 0L) return MADNeedMemory;
		
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<32; i++) theMAD->header->name[i] = ULTinfo.name[i];
	
	mystrcpy( theMAD->header->infos, (Ptr) "\pConverted by PlayerPRO ULT Plug (©Antoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->numPat			= ULTSuite.NOP;
	theMAD->header->numPointers	= 1;					// CHANGE
	theMAD->header->speed				= 6;
	theMAD->header->tempo				= 125;

	theMAD->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;

	for(i=0; i<128; i++) theMAD->header->oPointers[ i] = 0;
	for(i=0; i<128; i++)
	{
		theMAD->header->oPointers[ i] = ULTSuite.pattSeq[i];
		
		if( theMAD->header->oPointers[ i] < 0 || theMAD->header->oPointers[ i] >= 128) theMAD->header->oPointers[ i] = 0;
	}

for( i = 0; i < MAXTRACK; i++)
{
	if( i % 2 == 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
	else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
	
	theMAD->header->chanVol[ i] = MAX_VOLUME;
}

	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;

	// ********************
	// ***** INSTRUMENTS *****
	// ********************
	
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
	
	for(i=0; i<ULTSuite.NOS; i++)
	{
		InstrData		*curIns = &theMAD->fid[ i];
		
		curIns->type	= 0;
		
		{
			sData	*curData;
			
			curIns->numSamples	= 1;
			curIns->volFade			= DEFAULT_VOLFADE;
			
			curData = theMAD->sample[ i*MAXSAMPLE +  0] = (sData*) MADPlugNewPtrClear( sizeof( sData), init);
			if( curData == 0L) return MADNeedMemory;
			
			ULTSuite.ins[i].loopStart	= Tdecode32( &ULTSuite.ins[i].loopStart);
			ULTSuite.ins[i].loopEnd		= Tdecode32( &ULTSuite.ins[i].loopEnd);
			ULTSuite.ins[i].sizeStart	= Tdecode32( &ULTSuite.ins[i].sizeStart);
			ULTSuite.ins[i].sizeEnd		= Tdecode32( &ULTSuite.ins[i].sizeEnd);
			ULTSuite.ins[i].finetune	= Tdecode16( &ULTSuite.ins[i].finetune);
			
			curData->size			= ULTSuite.ins[i].sizeEnd - ULTSuite.ins[i].sizeStart;		// * 2 ???
			curData->loopBeg 	= ULTSuite.ins[i].loopStart;
			curData->loopSize	= ULTSuite.ins[i].loopEnd - ULTSuite.ins[i].loopStart;
			curData->vol			= ULTSuite.ins[i].volume;
			curData->c2spd		= ULTSuite.ins[i].finetune;
			curData->loopType	= 0;
			
			switch( ULTSuite.ins[i].Bidi)
			{
				case 4:
				case 12:
				case 28:
					curData->amp = 16;
				break;
				
				default:
					curData->amp			= 8;
				break;
				
			}
			
			
			curData->relNote	= 0;
			for( x = 0; x < 28; x++) theMAD->fid[i].name[x] = ULTSuite.ins[i].name[x];
			
			curData->data 		= MADPlugNewPtr( curData->size, init);
			if( curData->data == 0L) return MADNeedMemory;
			
			if( curData->data == 0L) DebugStr("\pInstruments: I NEED MEMORY !!! NOW !");
			
			if( curData->data != 0L)
			{
				BlockMoveData( theULT + ULTSuite.ins[i].sizeStart, curData->data, curData->size);
			}
		}
	//	else curIns->numSamples = 0;
	}
	
	theMAD->header->numChn = ULTSuite.NOC;
	
	for( i = 0; i < MAXPATTERN; i++) theMAD->partition[ i] = 0L;
	for( i = 0; i < theMAD->header->numPat ; i++)
	{
		theMAD->partition[ i] = (PatData*) MADPlugNewPtrClear( sizeof( PatHeader) + theMAD->header->numChn * 64 * sizeof( Cmd), init);
		if( theMAD->partition[ i] == 0L) return MADNeedMemory;
		
		theMAD->partition[ i]->header.size 			= 64;
		theMAD->partition[ i]->header.compMode 	= 'NONE';
		
		for( x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		
		MaxPtr = (Ptr) theMAD->partition[ i];
		MaxPtr += sizeof( PatHeader) + theMAD->header->numChn * 64 * sizeof( Cmd);
		
		for( Row = 0; Row < 64; Row++)
		{
			for(z = 0; z < theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand( Row, z, theMAD->partition[ i]);
				
				aCmd->note		= 0xFF;
				aCmd->ins			= 0;
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				aCmd->vol		= 0xFF;
			}
		}
	}
	
	DisposePtr( (Ptr) ULTSuite.ins);
	
	return noErr;
}

OSErr ExtractULTInfo( PPInfoRec *info, Ptr AlienFile)
{
	short		i, maxInstru, tracksNo;
	ULTForm		ULTinfo;
	/********************************/

	/**** Header principal *****/
	BlockMoveData( AlienFile, &ULTinfo, 49);
	
	/*** Signature ***/
	
	info->signature = 'ULT ';
	
	/*** Internal name ***/
	
	ULTinfo.name[ 31] = '\0';
	pStrcpy( (unsigned char*) info->internalFileName, MYC2PStr( ULTinfo.name));
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;	//Tdecode16(  &ITinfo.patNum);
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;	//Tdecode16( &ITinfo.orderNum);
	
	/*** Total Instruments ***/
	
	info->totalInstruments = 0;
	
	/*** Tracks ***/
	
	info->totalTracks	 = 0;
	
	pStrcpy( info->formatDescription, "ULT Plug");

	return noErr;
}

OSErr TestULTFile( Ptr AlienFile)
{
	ULTForm	*myULT = ( ULTForm*) AlienFile;

	if( *((long *) myULT->ID) == 'MAS_') return   noErr;
	else return  MADFileNotSupportedByThisPlug;
}

#ifdef _SRC
OSErr mainULT( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
EXP OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif


//OSErr TEST2main( OSType order, char *AlienFileFSSpec, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	short	vRefNum, iFileRefI;
	long		dirID, sndSize;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif

	HGetVol( 0L, &vRefNum, &dirID);
	HSetVol( 0L, AlienFileFSSpec->vRefNum, AlienFileFSSpec->parID);

	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			myErr = FSOpen( AlienFileFSSpec->name, 0, &iFileRefI);
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
					if( AlienFile == 0L) myErr = MADNeedMemory;
					else
					{
						myErr = FSRead( iFileRefI, &sndSize, AlienFile);
						if( myErr == noErr)
						{
							myErr = TestULTFile( AlienFile);
							if( myErr == noErr)
							{
								myErr = ConvertULT2Mad( AlienFile,  GetPtrSize( AlienFile), MadFile, init);
							}
						}
					}
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
		break;
		
		case 'TEST':
			myErr = FSOpen( AlienFileFSSpec->name, 0, &iFileRefI);
			if( myErr == noErr)
			{
				sndSize = 1024L;
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					myErr = TestULTFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
		break;

		case 'INFO':
			myErr = FSOpen( AlienFileFSSpec->name, 0, &iFileRefI);
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
						myErr = ExtractULTInfo( info, AlienFile);
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

	HSetVol( 0L, vRefNum, dirID);

	#ifndef powerc
		SetA4( oldA4);
	#endif
	return myErr;
}