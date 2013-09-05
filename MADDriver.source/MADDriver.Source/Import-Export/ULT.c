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

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif
#include "ULT.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

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

static OSErr ConvertULT2Mad( Ptr theULT, size_t MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	UInt32 				i, PatMax, x, z, channel, Row;
	UInt32 				sndSize, starting, RES;
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
		theInstrument[ i] = NULL;
	}
	
	/**** Header principal *****/
	theULTCopy = (Byte*) theULT;
	
	memcpy( &ULTinfo, theULTCopy, sizeof( ULTinfo));
	
	//if( ULTinfo.reserved != 0) return MADFileNotSupportedByThisPlug;	// RES in v.1.4 see doc
	
	ULTSuite.NOS = *(theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L);
	
	
	/**** Ins Num *****/
	if( sizeof( ULTIns) != 64) //DebugStr("\pULTIns != 64");
		return MADIncompatibleFile;
	ULTSuite.ins = (ULTIns*) calloc( ULTSuite.NOS * sizeof( ULTIns), 1);
	memcpy( ULTSuite.ins, theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L + 1, ULTSuite.NOS * sizeof( ULTIns));
	
	/**** Copy last infos *****/
	memcpy( &ULTSuite.pattSeq, theULTCopy + sizeof( ULTinfo) + ULTinfo.reserved * 32L + 1 + (ULTSuite.NOS * sizeof( ULTIns)), 256 + 2);
	
	// ******** Le ULT a été lu et analysé ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*) calloc( sizeof( MADSpec), 1);
	if( theMAD->header == NULL) return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<32; i++) theMAD->header->name[i] = ULTinfo.name[i];
	
	strlcpy( theMAD->header->infos, "Converted by PlayerPRO ULT Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->numPat			= ULTSuite.NOP;
	theMAD->header->numPointers	= 1;					// CHANGE
	theMAD->header->speed				= 6;
	theMAD->header->tempo				= 125;
	
	theMAD->sets = (FXSets*) calloc( MAXTRACK * sizeof(FXSets), 1);
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
	
	theMAD->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if( !theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if( !theMAD->sample) return MADNeedMemory;
	
	for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	for(i  = 0 ; i < MAXINSTRU; i++)
	{
		for( x = 0; x < MAXSAMPLE; x++) theMAD->sample[ i*MAXSAMPLE + x] = NULL;
		
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
			
			curData = theMAD->sample[ i*MAXSAMPLE +  0] = (sData*) calloc( sizeof( sData), 1);
			if( curData == NULL) return MADNeedMemory;
			
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
			
			curData->data 		= malloc( curData->size);
			if( curData->data == NULL) return MADNeedMemory;
			
			else
			{
				memcpy( curData->data, theULT + ULTSuite.ins[i].sizeStart, curData->size);
			}
		}
		//else curIns->numSamples = 0;
	}
	
	theMAD->header->numChn = ULTSuite.NOC;
	
	for( i = 0; i < MAXPATTERN; i++) theMAD->partition[ i] = NULL;
	for( i = 0; i < theMAD->header->numPat ; i++)
	{
		theMAD->partition[ i] = (PatData*) calloc( sizeof( PatHeader) + theMAD->header->numChn * 64 * sizeof( Cmd), 1);
		if( theMAD->partition[ i] == NULL) return MADNeedMemory;
		
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
	
	free( ULTSuite.ins);
	
	return noErr;
}

static OSErr ExtractULTInfo( PPInfoRec *info, Ptr AlienFile)
{
	short		i, maxInstru, tracksNo;
	ULTForm		ULTinfo;
	/********************************/
	
	/**** Header principal *****/
	memcpy( &ULTinfo, AlienFile, 49);
	
	/*** Signature ***/
	
	info->signature = 'ULT ';
	
	/*** Internal name ***/
	
	ULTinfo.name[ 31] = '\0';
	//pStrcpy( (unsigned char*) info->internalFileName, MYC2PStr( ULTinfo.name));
	strlcpy(info->internalFileName, ULTinfo.name, sizeof(ULTinfo.name));
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;	//Tdecode16(  &ITinfo.patNum);
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;	//Tdecode16( &ITinfo.orderNum);
	
	/*** Total Instruments ***/
	
	info->totalInstruments = 0;
	
	/*** Tracks ***/
	
	info->totalTracks	 = 0;
	
	strlcpy( info->formatDescription, "ULT Plug", sizeof(info->formatDescription));
	
	return noErr;
}

static OSErr TestULTFile( Ptr AlienFile)
{
	ULTForm	*myULT = ( ULTForm*) AlienFile;
	OSType ultID = *((OSType *) myULT->ID);
	PPBE32(&ultID);
	
	if( ultID == 'MAS_') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP OSErr FillPlug( PlugInfo *p);
EXP OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy( p->type, 		"ULT ", sizeof(p->type));
	strlcpy( p->MenuName, 	"ULT Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif


#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr mainULT( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch( order)
	{
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if( iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc( sndSize * 2);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free( AlienFile);
					
					AlienFile = (Ptr)malloc( sndSize);
					if( AlienFile == NULL) myErr = MADNeedMemory;
					else
					{
						myErr = iRead(sndSize, AlienFile, iFileRefI);
						if( myErr == noErr)
						{
							myErr = TestULTFile( AlienFile);
							if( myErr == noErr)
							{
								myErr = ConvertULT2Mad( AlienFile,  sndSize, MadFile, init);
							}
						}
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			} else myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if( iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = (Ptr)malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr)
						myErr = TestULTFile( AlienFile);
					
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			} else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead(AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = ExtractULTInfo( info, AlienFile);
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			} else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
