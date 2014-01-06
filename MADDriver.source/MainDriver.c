/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifdef MAINPLAYERPRO
#include "Shuddup.h"
#endif
#include "RDriver.h"
#include "RDriverInt.h"
#include "FileUtils.h"
#include <string.h>
#include "PPPrivate.h"
#ifdef _MAC_H
#include <CoreFoundation/CoreFoundation.h>
#else
#ifdef __BLOCKS__
#include <dispatch/dispatch.h>
#endif
#endif

void debugger(Ptr a) DEPRECATED_ATTRIBUTE;

///////////////////////////////

//#define  NO_ASM

enum {
	MADFileType = 1,
	MADPtrType = 3
};

void		CheckVSTEditor(VSTEffect *ce);
void		SendMIDIClock(MADDriverRec *intDriver, Byte MIDIByte);
void		DisposeVSTEffect(VSTEffect *myEffect);
VSTEffect*	CreateVSTEffect(short effectID);
short		ConvertUniqueIDToIndex(UInt32);
void		ApplyVSTSets(VSTEffect* myEffect, FXSets* set);

MADMusic* CreateFreeMADK()
{
	MADMusic	*music = (MADMusic*)calloc(sizeof(MADMusic), 1);
	MADSpec		*MADI;
	short		i, x, z;
	Cmd			*aCmd;
	if (music == NULL)
		return NULL;
	// ******* HEADER *********
	
	MADI = (MADSpec*)calloc(sizeof(MADSpec), 1);
	if (MADI == NULL)
	{
		free(music);
		return NULL;
	}
	
	MADI->MAD					= MADID;
	MADI->numPat				= 1;
	MADI->numPointers			= 1;
	MADI->numInstru				= 0;
	MADI->numChn				= 4;
	MADI->speed					= 6;
	MADI->tempo					= 125;
	
	MADI->EPitch 				= 8000;
	MADI->ESpeed 				= 8000;
	MADI->generalVol			= 64;
	MADI->generalPitch			= 252;
	MADI->generalSpeed			= 252;
	MADI->generalPan			= 64;
	MADI->XMLinear = false;
	
	music->header	= MADI;
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (i % 2 == 0) MADI->chanPan[ i] = MAX_PANNING/4;
		else MADI->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		
		MADI->chanVol[ i] = MAX_VOLUME;
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		MADI->chanBus[ i].copyId = i;
	}
	
	music->fid = (InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if (music->fid == NULL)
	{
		free(MADI);
		free(music);
		return NULL;
	}
	
	music->sample = (sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if (music->sample == NULL)
	{
		free(music->fid);
		free(MADI);
		free(music);
		return NULL;
	}
	// ******* PARTITION *********
	
	for(i=0; i < MADI->numPat; i++)
	{
		music->partition[ i] = (PatData*) calloc( sizeof( PatHeader) + MADI->numChn * 64L * sizeof( Cmd), 1);
		if (music->partition[i] == NULL)
		{
			for(i = 0; i < MADI->numPat; i++)
			{
				if (music->partition[i] != NULL)
					free(music->partition[i]);
			}
			free(music->fid);
			free(music->sample);
			free(MADI);
			free(music);
			return NULL;
		}
		
		
		music->partition[ i]->header.size 		= 64L;
		music->partition[ i]->header.compMode 	= 'NONE';
		music->partition[ i]->header.patBytes 	= 0;
		music->partition[ i]->header.unused2 		= 0;
		
		for (x = 0; x < 64; x++)
		{
			for (z = 0; z < music->header->numChn; z++)
			{
				aCmd = GetMADCommand(  x,  z, music->partition[ i]);
				
				MADKillCmd( aCmd);
			}
		}
	}
	for (i = music->header->numPat; i < MAXPATTERN ; i++) music->partition[ i] = NULL;
	
	music->sets = (FXSets*)calloc(MAXTRACK * sizeof(FXSets), 1);
	
	return music;
}

#if 0
void WriteMADKFile( FSSpec *newFile, MADMusic	*music)
{
	short	i, fRefNum;
	OSErr	iErr;
	long	inOutCount, tt;
	
	iErr = FSpOpenDF( newFile, 0, &fRefNum);
	
	if (iErr != noErr)
	{
		iErr = FSpCreate( newFile, 0, 'SNPL', MADID);
		iErr = FSpOpenDF( newFile, 0, &fRefNum);
	}
	
	inOutCount = sizeof( MADSpec);
	iErr = FSWrite( fRefNum, &inOutCount, music->header);
	
	for (i = 0; i < music->header->numPat ; i++)
	{
		if (thePrefs.MADCompression) music->partition[ i]->header.compMode = 'MAD1';
		
		if (music->partition[ i]->header.compMode == 'MAD1')
		{
			PatData* PatMAD = CompressPartitionMAD1( music, music->partition[ i]);
			inOutCount = PatMAD->header.patBytes + sizeof( struct PatHeader);
			
			iErr = FSWrite( fRefNum, &inOutCount, PatMAD);
			
			Myfree( (Ptr*) &PatMAD);
		}
		else
		{
			inOutCount = sizeof( PatHeader);
			inOutCount += music->header->numChn * music->partition[ i]->header.size * sizeof( Cmd);
			
			iErr = FSWrite( fRefNum, &inOutCount, music->partition[ i]);
		}
	}
	
	GetFPos( fRefNum, &tt);
	SetEOF( fRefNum, tt);
	FSClose( fRefNum);
}
#endif

size_t MADGetMusicSize(MADMusic *music)
{
	int i, x;
	size_t fileSize;
	
	if (music->header == NULL) return 0;
	
	fileSize = sizeof( MADSpec);
	fileSize += (long) music->header->numInstru * sizeof( InstrData);
	for (i = 0; i < music->header->numPat; i++) fileSize += sizeof( PatHeader) + music->header->numChn * music->partition[ i]->header.size * sizeof( Cmd);
	for (i = 0; i < music->header->numInstru ; i++) {
		for (x = 0; x < music->fid[ i].numSamples ; x++) {
			sData	*curData = music->sample[ i * MAXSAMPLE + x];
			
			fileSize += sizeof( sData32);
			fileSize += curData->size;
		}
	}
	
	return fileSize;
}

void ConvertTo64Rows(MADMusic *music)
{
	SInt32	i, x, z;
	Boolean	IsReading;
	SInt32	patID, found;
	
	if (music->header == NULL)
		return;
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = music->header->numPat-1; i >= 0 ; i--) {
		// Resize pattern to 64 rows and put a pattern break
		
		SInt32		newSize;
		PatData		*newPat = NULL;
		
		patID = i;
		
		newSize = sizeof( PatHeader) + music->header->numChn * 64L * sizeof( Cmd);
		
		if (music->partition[ i]->header.size < 64) {
			Cmd		*srccmd = NULL, *dstcmd = NULL;
			SInt32	patsize;
			
			newPat = ( PatData*) calloc( newSize, 1);
			
			newPat->header.size 		= 64L;
			newPat->header.compMode 	= 'NONE';
			newPat->header.patBytes 	= 0;
			newPat->header.unused2 		= 0;
			
			memcpy(newPat->header.name, music->partition[i]->header.name, 32);
			
			// Upgrade length to 64
			
			for (x = 0, patsize = 0; x < 64; x++, patsize++)
			{
				for (z = 0; z < music->header->numChn; z++)
				{
					if (x < music->partition[ i]->header.size)
					{
						// src
						srccmd = GetMADCommand( patsize, z, music->partition[ i]);
						
						// dst
						dstcmd = GetMADCommand( x, z, newPat);
						if (dstcmd && srccmd) {
							*dstcmd = *srccmd;
						}
					}
					else
					{
						dstcmd = GetMADCommand( x, z, newPat);
						
						MADKillCmd( dstcmd);
					}
				}
			}
			
			// dst
			dstcmd = GetMADCommand(music->partition[i]->header.size-1, 0, newPat);		// Pose le pattern break
			
			MADKillCmd(dstcmd);
			
			dstcmd->cmd = skipE;
			dstcmd->arg = 0;
			
			// Replace old pattern
			
			free(music->partition[i]);
			music->partition[ i] = newPat;
		} else if (music->partition[ i]->header.size > 64) {
			SInt32 		patsize = 0;
			PatData*	srcPat = music->partition[ i];
			
			while (patsize < srcPat->header.size) {
				newPat = ( PatData*) calloc( newSize, 1);
				
				newPat->header.size 		= 64L;
				newPat->header.compMode 	= 'NONE';
				newPat->header.patBytes 	= 0;
				newPat->header.unused2 		= 0;
				
				memcpy( newPat->header.name, srcPat->header.name, 32);
				
				for (x = 0; x < 64; x++, patsize++) {
					for (z = 0; z < music->header->numChn; z++) {
						Cmd		*srccmd, *dstcmd;
						
						if (patsize < srcPat->header.size) {
							// src
							srccmd = GetMADCommand( patsize, z, srcPat);
							
							// dst
							dstcmd = GetMADCommand( x, z, newPat);
							
							*dstcmd = *srccmd;
						} else {
							dstcmd = GetMADCommand( x, z, newPat);
							MADKillCmd( dstcmd);
						}
					}
				}
				
				// Update patterns list & partition
				
				if (patID != i) {
					for (x = music->header->numPat; x > patID ; x--)
						music->partition[x] = music->partition[x - 1];
					music->header->numPat++;
				}
				
				for (x = music->header->numPat; x < MAXPATTERN; x++)
					music->partition[x] = NULL;
				
				music->partition[patID] = newPat;
				
				patID++;
			}
			patID--;
			
			
			// Do we need a pattern break ?
			
			if ((srcPat->header.size / 64) * 64 != srcPat->header.size) {
				short	breakpos;
				Cmd		*dstcmd;
				
				breakpos = srcPat->header.size - (srcPat->header.size / 64) * 64;
				
				dstcmd = GetMADCommand( breakpos-1, 0, newPat);
				MADKillCmd( dstcmd);
				if (dstcmd) {
					dstcmd->cmd = skipE;
					dstcmd->arg = 0;
				}
			}
			
			// Update la partition list
			
			for (x = 0; x < music->header->numPointers ; x++) {
				if (music->header->oPointers[x] > i)
					music->header->oPointers[x] += patID - i;
			}
			
			found = 0;
			
			for (x = music->header->numPointers-1; x >= 0 ; x--) {
				if (music->header->oPointers[x] == i) {
					found++;
					
					// Avance le reste de la partition
					
					for (z = 256-1; z >= x ; z--) {
						if (z + patID - i < 256) {
							music->header->oPointers[ z + patID - i] = music->header->oPointers[ z];
						}
					}
					
					for (z = 0; z <= patID - i; z++)
						music->header->oPointers[x + z] = i + z;
				}
			}
			
			music->header->numPointers += found * (patID - i);
			
			free(srcPat);
		}
	}
	
	music->musicUnderModification = IsReading;
}

size_t MADMinimize(MADMusic *music)
{
	short 		i, x, z;
	Boolean		remove, IsReading;
	Boolean		inst[MAXINSTRU];
	size_t		before, after;
	
	if (music->header == NULL) return 0;
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	// Check size before
	
	before = MADGetMusicSize( music);
	
	// Check for unused patterns
	
	if (music->header->numPat > 1)
	{
		for (i = 0; i < music->header->numPat; i++)
		{
			remove = true;
			for (x = 0; x < music->header->numPointers; x++)
			{
				if (music->header->oPointers[ x] == i)
				{
					remove = false;
					break;
				}
			}
			
			if (remove)
			{
				music->header->numPat--;
				
				free( music->partition[ i]);
				music->partition[ i] = NULL;
				
				for (x = i; x < music->header->numPat; x++)
				{
					music->partition[ x] = music->partition[ x + 1];
				}
				for (x = music->header->numPat; x < MAXPATTERN; x++) music->partition[ x] = NULL;
				
				/***** Update la partition list ******/
				
				for (x = 0; x < 128; x++)
				{
					if (music->header->oPointers[ x] > i) music->header->oPointers[ x]--;
					else if (music->header->oPointers[ x] == i) music->header->oPointers[ x] = 0;
				}
				
				if (i > 0) i--;
			}
		}
	}
	
	// Check for unused instruments
	for (i = 0; i < 32 ; i++)
		inst[i] = false;
	
	for (i = 0; i < music->header->numPat; i++)
	{
		for (x = 0; x < music->partition[ i]->header.size; x++)
		{
			for (z = 0; z < music->header->numChn; z++)
			{
				Cmd		*cmd;
				
				cmd = GetMADCommand( x, z, music->partition[ i]);
				
				if (cmd->ins > 0) inst[ cmd->ins-1] = true;
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++)
	{
		if (inst[ i] == false) MADKillInstrument( music, i);
	}
	
	// Check for after loop data
	for (i = 0; i < MAXINSTRU; i++)
	{
		for (x = 0; x < music->fid[ i].numSamples; x++)
		{
			sData	*curData = music->sample[ i * MAXSAMPLE + x];
			
			if (curData->loopSize > 2)
			{
				if (curData->size > curData->loopBeg + curData->loopSize)
				{
					curData->data = realloc( curData->data, curData->loopBeg + curData->loopSize);
					
					curData->size = curData->loopBeg + curData->loopSize;
				}
			}
		}
	}
	
	music->musicUnderModification = IsReading;
	
	after = MADGetMusicSize( music);
	
	return before - after;
}

static int driverList = 0;

static void BuildAvailableDriverList()
{
	if (driverList == 0) {
		driverList = MIDISoundDriverBit |
#ifdef _BE_H
		1 << BeOSSoundDriver |
#endif
#ifdef WIN32
		DirectSound95NTBit | Wave95NTBit |
#endif
#ifdef _MAC_H
		CoreAudioDriverBit |
#endif
#ifdef _ESOUND
		ESDDriverBit |
#endif
		0;
	}
}

Boolean MADSoundDriverIsAvalable(short theDriver)
{
	if (theDriver == NoHardwareDriver) {
		return TRUE;
	}
	BuildAvailableDriverList();
	if (driverList & 1 << theDriver)
		return TRUE;
	else
		return FALSE;
}

int MADSoundDriverList()
{
	BuildAvailableDriverList();
	return driverList;
}

void MADGetBestDriver( MADDriverSettings	*Init)
{
	Init->outPutBits		= 16;
	Init->outPutMode		= DeluxeStereoOutPut;
	Init->outPutRate		= 44100;
	Init->numChn			= 4;
	Init->surround			= false;
	Init->repeatMusic		= true;
	Init->MicroDelaySize	= 25;
	Init->Reverb			= false;
	Init->ReverbSize		= 100;
	Init->ReverbStrength	= 20;
	Init->oversampling		= 1;
	Init->TickRemover		= false;
#ifdef _MAC_H
	//Just going to use CoreAudio
	Init->driverMode		= CoreAudioDriver;
#elif defined(WIN32)
	Init->driverMode		= Wave95NT;
#endif
}

OSErr MADCreateMicroDelay( MADDriverRec *intDriver)
{
	intDriver->MDelay = (intDriver->DriverSettings.MicroDelaySize * ( intDriver->DriverSettings.outPutRate)* intDriver->DriverSettings.oversampling) / 1000;
	
	return noErr;
}

void MADDisposeReverb( MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb) free( intDriver->ReverbPtr);
	intDriver->ReverbPtr = NULL;
}

OSErr MADCreateReverb( MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb)
	{
		intDriver->RDelay = (intDriver->DriverSettings.ReverbSize * ( intDriver->DriverSettings.outPutRate)) / 1000;
		
		switch( intDriver->DriverSettings.outPutBits)
		{
			case 8:
				intDriver->ReverbPtr = (Ptr)malloc( intDriver->RDelay * 2L);
				if (intDriver->ReverbPtr == NULL) return MADNeedMemory;
				memset(intDriver->ReverbPtr, 0x80, intDriver->RDelay * 2);
				break;
				
			case 16:
				intDriver->ReverbPtr = (Ptr)calloc( intDriver->RDelay * 4L, 1);
				if (intDriver->ReverbPtr == NULL) return MADNeedMemory;
				break;
		}
	}
	
	return noErr;
}

void MADDisposeDriverBuffer( MADDriverRec *intDriver)
{
	short i;
	
	for (i = 0; i < MAXDRAW; i++)
	{
		if (intDriver->OsciDrawPtr[ i] != NULL)	free( intDriver->OsciDrawPtr[ i]);
		intDriver->OsciDrawPtr[ i] = NULL;
	}
	
	if (intDriver->IntDataPtr != NULL)	free( intDriver->IntDataPtr);
	intDriver->IntDataPtr = NULL;
}

OSErr MADCreateDriverBuffer( MADDriverRec *intDriver)
{
	short	i, x;
	SInt32 BufSize = 0;
	
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	
	BufSize = intDriver->ASCBUFFER;
	
	switch( intDriver->DriverSettings.outPutMode)
	{
		case MonoOutPut:
			BufSize = BufSize;
			break;
		case StereoOutPut:
		case DeluxeStereoOutPut:
			BufSize *= 2;
			break;
		case PolyPhonic:
			BufSize *= 4;
			break;
	}
	
	switch( intDriver->DriverSettings.outPutBits)
	{
		case 16:
			BufSize *= 2;
			break;
		case 20: //This is actually 2.5 more than 8, but I don't want to round
		case 24:
			BufSize *= 3;
			break;
	}
	
	intDriver->curDrawPtr = 0;
	for (i = 0; i < MAXDRAW; i++)
	{
		intDriver->OsciDrawPtr[ i] = NULL;
	}
	
	for (i = 0; i < MAXDRAW; i++)
	{
		intDriver->newData[ i] = false;
		intDriver->OsciDrawPtr[ i] = (Ptr)calloc( BufSize / intDriver->DriverSettings.oversampling, 1);
		if (intDriver->OsciDrawPtr[ i] == NULL)
		{
			for (x = 0; x < MAXDRAW; x++) {
				if (intDriver->OsciDrawPtr[ i] != NULL) {
					free(intDriver->OsciDrawPtr[ i]);
				}
			}
			return MADNeedMemory;
		}
	}
	
	intDriver->IntDataPtr = (Ptr)malloc( BufSize);
	if (intDriver->IntDataPtr == NULL)
	{
		for (x = 0; x < MAXDRAW; x++) {
			if (intDriver->OsciDrawPtr[ i] != NULL) {
				free(intDriver->OsciDrawPtr[ i]);
			}
		}
		return MADNeedMemory;
	}
	
	intDriver->BufSize			= BufSize;
	intDriver->OscilloWavePtr	= intDriver->IntDataPtr;
	intDriver->OscilloWaveSize	= BufSize / intDriver->DriverSettings.oversampling;
	
	return noErr;
}

OSErr MADCreateTiming( MADDriverRec *intDriver)
{
	/*************************/
	/**     Rate table      **/
	/*************************/
	
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	
	intDriver->VSYNC	= ((intDriver->DriverSettings.outPutRate) * 125L * intDriver->DriverSettings.oversampling) / (50);
	intDriver->trackDiv = intDriver->DriverSettings.numChn;
	//	if (intDriver->DriverSettings.outPutMode == StereoOutPut) intDriver->trackDiv /= 2;
	
	return noErr;
}

MADDriverSettings MADGetDriverSettings(MADDriverRec *theDriver)
{
	return theDriver->DriverSettings;
}

OSErr MADChangeDriverSettings(MADDriverSettings *DriverInitParam, MADDriverRec** returnDriver)
{
	MADMusic	*music;
	Boolean		playing;
	OSErr		err;
	long		fullTime, curTime;
	MADLibrary	*lib;
	if (DriverInitParam == NULL || returnDriver == NULL) {
		return MADParametersErr;
	}
	music = (*returnDriver)->curMusic;
	playing = (*returnDriver)->Reading;
	lib = (*returnDriver)->lib;
	
	MADGetMusicStatus(*returnDriver, &fullTime, &curTime);
	
	if ((err = MADStopDriver(*returnDriver)) != noErr)
		return err;
	if ((err = MADDisposeDriver(*returnDriver)) != noErr)
		return err;
	
	if ((err = MADCreateDriver(DriverInitParam, lib, returnDriver)) != noErr)
		return err;
	
	if ((err = MADStartDriver(*returnDriver)) != noErr)
		return err;
	
	if (music) {
		MADAttachDriverToMusic( *returnDriver, music, NULL);
		MADSetMusicStatus( *returnDriver, 0, fullTime, curTime);
		
		if (playing)
			MADPlayMusic( *returnDriver);
	}
	
	return noErr;
}

OSErr MADCreateDriver(MADDriverSettings *DriverInitParam, MADLibrary *lib, MADDriverRec** returnDriver)
{
	OSErr 					theErr;
	SInt32					i;
	MADDriverRec*			MDriver;
	
	if (DriverInitParam == NULL || lib == NULL || returnDriver == NULL)
		return MADParametersErr;
	*returnDriver = NULL;
	
	/*************************/
	/** Paramaters checking **/
	/*************************/
	
	theErr = noErr;
	
	if (DriverInitParam->numChn % 2 != 0) {
		DriverInitParam->numChn /= 2;
		DriverInitParam->numChn *= 2;
		//	DriverInitParam->numChn++;
	}
	if (DriverInitParam->numChn < 2)
		theErr = MADParametersErr;
	if (DriverInitParam->numChn > MAXTRACK)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutBits != 8 && DriverInitParam->outPutBits != 16)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutRate < 5000)
		theErr = MADParametersErr;
	if (DriverInitParam->outPutRate > 48000)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutMode != DeluxeStereoOutPut &&
	   DriverInitParam->outPutMode != PolyPhonic)
		theErr = MADParametersErr;
	
	if (DriverInitParam->driverMode != MIDISoundDriver &&
#ifdef _BE_H
	   DriverInitParam->driverMode != BeOSSoundDriver &&
#endif
#ifdef WIN32
	   DriverInitParam->driverMode != DirectSound95NT &&
	   DriverInitParam->driverMode != Wave95NT &&
#endif
#ifdef _MAC_H
	   DriverInitParam->driverMode != CoreAudioDriver &&
#endif
#ifdef LINUX
	   DriverInitParam->driverMode != ALSADriver &&
#endif
#ifdef _OSSSOUND
	   DriverInitParam->driverMode != OSSDriver &&
#endif
#ifdef _ESOUND
	   DriverInitParam->driverMode != ESDDriver &&
#endif
	   DriverInitParam->driverMode != NoHardwareDriver) {
		if (theErr == noErr) {
			theErr = MADSoundSystemUnavailable;
		}
	}
	
	if (DriverInitParam->MicroDelaySize < 0)
		theErr = MADParametersErr;
	if (DriverInitParam->MicroDelaySize > 1000)
		theErr = MADParametersErr;
	
	if (DriverInitParam->Reverb) {
		if (DriverInitParam->ReverbSize < 25)
			theErr = MADParametersErr;
		if (DriverInitParam->ReverbSize > 1000)
			theErr = MADParametersErr;
		
		if (DriverInitParam->ReverbStrength < 0)
			theErr = MADParametersErr;
		if (DriverInitParam->ReverbStrength > 70)
			theErr = MADParametersErr;
	}
	
	if (DriverInitParam->oversampling < 1)
		theErr = MADParametersErr;
	if (DriverInitParam->oversampling > 30)
		theErr = MADParametersErr;
	
	if (theErr != noErr)
		return theErr;
	
	/*************************/
#if !defined(HAS_LONG_LONG) || !defined(HAS_LONG_DOUBLE)
	DriverInitParam->oversampling = 1;// We do NOT support oversampling on compilers that don't have long longs and long doubles
#endif
	
	MDriver = (MADDriverRec*)calloc(sizeof(MADDriverRec), 1);
	
	MDriver->lib = lib;
	MDriver->curMusic = NULL;
	
	MDriver->Reading = false;
	MDriver->wasReading = false;
	
	theErr = MADStopDriver(MDriver);
	if (theErr != noErr) {
		free(MDriver);
		return theErr;
	}
	
	//theErr = MADDisposeDriver( MDriver);
	//if (theErr != noErr) return theErr;
	
	theErr = MADCreateVibrato( MDriver);
	if (theErr != noErr) {
		free(MDriver);
		return theErr;
	}
	
	for (i = 0; i < MAXTRACK; i++)
		MDriver->Active[i] = true;
	
	MDriver->DriverSettings		= *DriverInitParam;
	MDriver->musicEnd			= false;
	MDriver->Reading			= false;
	MDriver->JumpToNextPattern	= true;
	MDriver->smallcounter		= 128;			// Start immediately
	MDriver->BufCounter			= 0;
	MDriver->BytesToGenerate	= 0;
	MDriver->speed				= 6;
	MDriver->finespeed			= 125;
	MDriver->VExt				= 8000;
	MDriver->FreqExt			= 8000;
	MDriver->VolGlobal			= 64;
	MDriver->SendMIDIClockData	= false;
	MDriver->MODMode			= false;
	MDriver->XMLinear			= false;
	MDriver->Equalizer			= false;
	MDriver->MultiChanNo		= 48;
	MDriver->globPan			= 64;
	MDriver->currentlyExporting	= false;
	MDriver->thisExport			= false;
	MDriver->OneMoreBeforeEnd	= false;
	MDriver->useOsciBuffers		= false;
	MDriver->MIN_PITCH			= GetOldPeriod(NUMBER_NOTES-1, NOFINETUNE, MDriver);
	MDriver->MAX_PITCH			= GetOldPeriod(0, NOFINETUNE, MDriver);
	
	MDriver->MOD_MIN_PITCH		= GetOldPeriod(24 + 4*12 -1, NOFINETUNE, MDriver);
	MDriver->MOD_MAX_PITCH		= GetOldPeriod(24, NOFINETUNE, MDriver);
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;
	
	MDriver->levelL = 0;
	MDriver->levelR = 0;
	
	
	MADCleanDriver( MDriver);
	
	/*************************/
	/** 	Driver MODE	    **/
	/*************************/
	
	switch(MDriver->DriverSettings.driverMode)
	{
			
#ifdef _MAC_H
			
		case CoreAudioDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef LINUX
		case ALSADriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef _OSSSOUND
		case OSSDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
		case BeOSSoundDriver:
			MDriver->ASCBUFFER = 1024L;
			if (MDriver->DriverSettings.outPutBits == 8) MDriver->ASCBUFFER *= 2L;
			break;
			
		case NoHardwareDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
			
#ifdef WIN32
		case DirectSound95NT:
			MDriver->ASCBUFFER = 7500 * MDriver->DriverSettings.oversampling;
			break;
			
		case Wave95NT:
			MDriver->ASCBUFFER = 7500 * MDriver->DriverSettings.oversampling;
			break;
#endif
			
		default:
			MDriver->DriverSettings.driverMode = NoHardwareDriver;
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			//			return MADParametersErr;
			break;
	}
	
	MDriver->ASCBUFFERReal = MDriver->ASCBUFFER / MDriver->DriverSettings.oversampling;
	
	theErr = MADCreateDriverBuffer(MDriver);
	if (theErr != noErr) {
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateTiming(MDriver);
	if (theErr != noErr) {
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateReverb(MDriver);
	if (theErr != noErr) {
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateVolumeTable(MDriver);
	if (theErr != noErr) {
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADInitEqualizer(MDriver);
	if (theErr != noErr) {
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	
	/**********************************************/
	/**    Interruption - Hardware Support       **/
	/**********************************************/
	
	switch(MDriver->DriverSettings.driverMode)
	{
			//#ifdef _MIDIHARDWARE_
		case MIDISoundDriver:
			
			OpenMIDIHardware(MDriver);
			
			/*	MDriver->gOutNodeRefNum = -1;
			 OMSAddPort( 'SNPL', 'out ', omsPortTypeOutput, NULL, NULL, &MDriver->MIDIPortRefNum);*/
			
#if 0
			theErr = InitDBSoundManager( MDriver, initStereo);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
#endif
			break;
			//#endif
			
#ifdef _MAC_H
		case CoreAudioDriver:
			theErr = initCoreAudio(MDriver);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
			
		case BeOSSoundDriver:
			
			break;
			
#ifdef WIN32
		case DirectSound95NT:
			if (!DirectSoundInit( MDriver)) theErr = MADUnknowErr;
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
			
		case Wave95NT:
			if (!W95_Init( MDriver)) theErr = MADUnknowErr;
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			theErr = initESD(MDriver);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef LINUX
		case ALSADriver:
			theErr = initALSA(MDriver);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef _OSSSOUND
		case OSSDriver:
			theErr = initOSS(MDriver);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
		case NoHardwareDriver:
			
			break;
	}
	
	*returnDriver = MDriver;
	
	return noErr;
}

OSErr MADDisposeDriver(MADDriverRec* MDriver)
{
	if (MDriver->IntDataPtr == NULL)
		return noErr;
	
	MDriver->Reading = false;
	
	MADCleanDriver(MDriver);
	
	switch(MDriver->DriverSettings.driverMode)
	{
		case MIDISoundDriver:
			CloseMIDIHarware();
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			closeCoreAudio(MDriver);
			break;
#endif
			
#ifdef WIN32
		case DirectSound95NT:
			DirectSoundClose(MDriver);
			break;
			
		case Wave95NT:
			W95_Exit(MDriver);
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			closeESD(MDriver);
			break;
#endif
			
#ifdef LINUX
		case ALSADriver:
			closeALSA(MDriver);
			break;
#endif
			
#ifdef _OSSSOUND
		case OSSDriver:
			closeOSS(MDriver);
			break;
#endif
			
	}
	
	MADDisposeDriverBuffer(MDriver);
	MADDisposeVolumeTable(MDriver);
	MADDisposeReverb(MDriver);
	
	free(MDriver);
	
	return noErr;
}

OSErr MADInitLibrary(const char *PlugsFolderName, MADLibrary **lib)
{
	UInt32 i, mytab[12] =
	{
		1712*16, 1616*16, 1524*16, 1440*16, 1356*16, 1280*16,
		1208*16, 1140*16, 1076*16, 1016*16, 960*16, 907*16
	};
	if (lib == NULL)
		return MADParametersErr;
	
	*lib = (MADLibrary*)calloc(sizeof(MADLibrary), 1);
	
	if (*lib == NULL)
		return MADNeedMemory;
	
	(*lib)->IDType = 'MADD';
	
	for (i = 0; i < 12; i++)
		(*lib)->mytab[i] = mytab[i];
	
	MInitImportPlug(*lib, PlugsFolderName);
	return noErr;
}

OSErr MADDisposeLibrary(MADLibrary *MLibrary)
{
	if (MLibrary != NULL)
	{
		CloseImportPlug(MLibrary);
		MLibrary->IDType = 'XXXX';
		
		free(MLibrary);
		MLibrary = NULL;
	}
	return noErr;
}

OSErr MADAttachDriverToMusic(MADDriverRec *driver, MADMusic *music, char *MissingPlugs)
{
#if MAINPLAYERPRO
	short		alpha, x, i, index;
#endif
	Boolean		needToReset;
	
	if (!driver)
		return MADParametersErr;
	if (!music)
		return MADParametersErr;
	
	driver->musicEnd = false;
	
	if (music != driver->curMusic)
		needToReset = true;
	else
		needToReset = false;
	
	// Save old preferences
	
	if (driver->curMusic) {
		if (driver->curMusic->header) {
			driver->curMusic->header->generalVol = driver->VolGlobal;
			driver->curMusic->header->ESpeed = driver->VExt;
			driver->curMusic->header->EPitch = driver->FreqExt;
			
			driver->curMusic->header->generalSpeed = driver->VExt/100;
			driver->curMusic->header->generalPitch = driver->FreqExt/100;
			driver->curMusic->header->generalPan = driver->globPan;
		}
	}
#if MAINPLAYERPRO
	//TODO: Check VST editor
	CheckVSTEditor( NULL);
#endif
	
	/////////////////////////
	
	driver->curMusic = music;
	driver->VolGlobal = music->header->generalVol;
	if (driver->VolGlobal <= 0)
		driver->VolGlobal = 64;
	
	if (music->header->generalSpeed == 252) {
		driver->VExt	= music->header->ESpeed;
		driver->FreqExt	= music->header->EPitch;
	} else {
		driver->VExt = music->header->generalSpeed;
		if (driver->VExt <= 160)
			driver->VExt *= 100;
		if (driver->VExt <= 160)
			driver->VExt = 8000;
		
		
		driver->FreqExt = music->header->generalPitch;
		if (driver->FreqExt <= 160)
			driver->FreqExt *= 100;
		if (driver->FreqExt <= 160)
			driver->FreqExt = 8000;
	}
	
	if (music->header->generalPan == 0)
		music->header->generalPan = 64;
	driver->globPan = music->header->generalPan;
	
	if (music->header->MultiChanNo < 4)
		music->header->MultiChanNo = 48;
	if (music->header->MultiChanNo > 99)
		music->header->MultiChanNo = 99;
	
	if (music->header->MultiChanNo <= 0)
		driver->MultiChanNo = 48;
	else
		driver->MultiChanNo = music->header->MultiChanNo;
	
	if (music->header->MODMode)
		driver->MODMode = true;
	else
		driver->MODMode = false;
	
	if (music->header->XMLinear)
		driver->XMLinear = true;
	else
		driver->XMLinear = false;
	
	
#if MAINPLAYERPRO
	if (MissingPlugs) strcpy( (Ptr) MissingPlugs, "");
#endif
	
	//INSTALL ALL VST EFFECTS !!!!!!
#if MAINPLAYERPRO
	//TODO: VST Effects
	// Purge previous Effects !
	for (i = 0; i < 10 ; i++)
	{
		if (driver->masterVST[ i])
		{
			DisposeVSTEffect( driver->masterVST[ i]);
			driver->masterVST[ i] = NULL;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++)	// Channel Effects
	{
		for (x = 0; x < 4; x++)
		{
			if (driver->chanVST[ i][ x])
			{
				DisposeVSTEffect( driver->chanVST[ i][ x]);
				driver->chanVST[ i][ x] = NULL;
			}
		}
	}
	
	// Install new effects
	alpha = 0;
	
	for (i = 0; i < 10 ; i++)	// Global Effects
	{
		if (music->header->globalEffect[ i])
		{
			index = ConvertUniqueIDToIndex( music->sets[ alpha].FXID);
			
			if (index >= 0)
			{
				driver->masterVST[ i] = CreateVSTEffect( index);
				ApplyVSTSets( driver->masterVST[ i], &music->sets[ alpha]);
			}
			else if (MissingPlugs)
			{
				if (strlen( (Ptr) MissingPlugs) > 0) strcat(  (Ptr) MissingPlugs, ", ");
				MYP2CStr( music->sets[ alpha].name);
				strcpy(  (Ptr) MissingPlugs,  (Ptr) &music->sets[ alpha].name);
				MYC2PStr( (Ptr) music->sets[ alpha].name);
			}
			alpha++;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++)	// Channel Effects
	{
		for (x = 0; x < 4; x++)
		{
			if (music->header->chanEffect[ i][ x])
			{
				index = ConvertUniqueIDToIndex( music->sets[ alpha].FXID);
				
				if (index >= 0)
				{
					driver->chanVST[ i][ x] = CreateVSTEffect( index);
					ApplyVSTSets( driver->chanVST[ i][ x], &music->sets[ alpha]);
				}
				else if (MissingPlugs)
				{
					strcat(  (Ptr) MissingPlugs, ", ");
					MYP2CStr( music->sets[ alpha].name);
					strcpy(  (Ptr) MissingPlugs,  (Ptr) &music->sets[ alpha].name);
					MYC2PStr( (Ptr) music->sets[ alpha].name);
				}
				alpha++;
			}
		}
	}
	
	if (MissingPlugs) MYC2PStr( (Ptr) MissingPlugs);
#endif
	
	if (needToReset)
		MADReset(driver);
	UpdateTracksNumber(driver);
	
	return noErr;
}

OSErr MADLoadMusicPtr(MADMusic **music, Ptr myPtr)
{
	OSErr	theErr;
	
	//	MADDisposeMusic( music);
	
	theErr = MADReadMAD(music, 0, MADPtrType, NULL, myPtr);
	if (theErr != noErr)
		return theErr;
	
	return noErr;
}

OSErr MADLoadMADFileCString(MADMusic **music, const char *fName)
{
	OSErr	theErr;
	UNFILE	srcFile;
	
	if (fName == NULL || music == NULL)
		return MADParametersErr;
	
	//MADDisposeMusic(music);
	
	srcFile = iFileOpenRead(fName);
	if (srcFile == 0) return MADReadingErr;
	
	theErr = MADReadMAD(music, srcFile, MADFileType, NULL, NULL);
	if (theErr != noErr)
	{
		iClose(srcFile);
		//MADDisposeMusic( music, MADDriver);
		return theErr;
	}
	
	iClose(srcFile);
	
	return noErr;
}

#ifdef _MAC_H
//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static inline CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString;
	CFIndex strLength;
	fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	
	return strLength;
}

static OSErr getCStringFromCFURL(CFURLRef theRef, char **cStrOut)
{
	char *URLcString = NULL, *trimURLcString = NULL;
	CFIndex pathLen;
	Boolean pathOK;
	if (cStrOut == NULL) {
		return MADParametersErr;
	}
	if (theRef == NULL) {
		*cStrOut = NULL;
		return MADParametersErr;
	}
	pathLen = getCFURLFilePathRepresentationLength(theRef, true);
	URLcString = malloc(pathLen);
	if (URLcString == NULL) {
		*cStrOut = NULL;
		return MADNeedMemory;
	}
	pathOK = CFURLGetFileSystemRepresentation(theRef, true, (unsigned char*)URLcString, pathLen);
	if (pathOK == false) {
		free(URLcString);
		*cStrOut = NULL;
		return MADReadingErr;
	}
	trimURLcString = realloc(URLcString, strlen(URLcString) + 1);
	if (!trimURLcString) {
		*cStrOut = URLcString;
		return noErr;
	}
	
	*cStrOut = trimURLcString;
	return noErr;
}

OSErr MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef)
{
	char *URLcString = NULL;
	OSErr theErr = getCStringFromCFURL(theRef, &URLcString);
	
	if (theErr != noErr)
		return theErr;
	
	theErr = MADLoadMusicFileCString(lib, music, type, URLcString);
	free(URLcString);
	return theErr;
}

#if 0
OSErr MADCopyCurrentPartition(MADMusic *aPartition)
{
	long	i, x;
	
	if (MDriver->header == NULL) return -1;
	if (aPartition == NULL) return -1;
	
	aPartition->header = (MADSpec*) MADNewPtr( sizeof( MADSpec), &MDriver->DriverSettings);
	if (aPartition->header == NULL) return -1;
	
	BlockMoveData( MDriver->header, aPartition->header, sizeof( MADSpec));
	
	for (i = 0; i < MDriver->header->numPat ; i++)
	{
		aPartition->partition[ i] = (PatData*) MADNewPtr( GetPtrSize( (Ptr) MDriver->partition[ i]), &MDriver->DriverSettings);
		if (aPartition->partition[ i] == NULL) return -1;
		
		BlockMoveData( MDriver->partition[ i], aPartition->partition[ i], GetPtrSize( (Ptr) MDriver->partition[ i]));
	}
	for (i = MDriver->header->numPat; i < MAXPATTERN ; i++) aPartition->partition[ i] = NULL;
	
	for (i = 0; i < MAXINSTRU ; i++)
	{
		for (x = 0; x < MDriver->header->fid[i].numSamples; x++)
		{
			sData	*dstData, *srcData = MDriver->sample[ i][ x];
			
			aPartition->sample[ i][ x] = dstData = (sData*) MADNewPtr( sizeof( sData), &MDriver->DriverSettings);
			if (dstData == NULL) return -1;
			
			BlockMoveData( srcData, dstData, sizeof( sData));
			
			dstData->data = MADNewPtr( srcData->size, &MDriver->DriverSettings);
			if (dstData->data == NULL) return -1;
			
			BlockMoveData( srcData->data, dstData->data, srcData->size);
		}
	}
	
	return noErr;
}
#endif


OSErr MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef)
{
	char *URLcString = NULL;
	OSErr theErr = getCStringFromCFURL(URLRef, &URLcString);
	
	if (theErr != noErr)
		return theErr;
	
	theErr = MADMusicIdentifyCString(lib, type, URLcString);
	free(URLcString);
	return theErr;
}

OSErr MADMusicInfoCFURL(MADLibrary *lib, char *type, CFURLRef theRef, PPInfoRec *InfoRec)
{
	char *URLcString = NULL;
	OSErr theErr = getCStringFromCFURL(theRef, &URLcString);
	
	if (theErr != noErr)
		return theErr;
	
	theErr = MADMusicInfoCString(lib, type, URLcString, InfoRec);
	free(URLcString);
	return theErr;
}

OSErr MADMusicExportCFURL(MADLibrary *lib, MADMusic *music, char *type, CFURLRef fileURL)
{
	char *URLcString = NULL;
	OSErr theErr = getCStringFromCFURL(fileURL, &URLcString);
	
	if (theErr)
		return theErr;
	
	theErr = MADMusicExportCString(lib, music, type, URLcString);
	free(URLcString);
	return theErr;
}

OSErr MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, Boolean compressMAD)
{
	char *URLcString = NULL;
	OSErr theErr = getCStringFromCFURL(urlRef, &URLcString);
	
	if (theErr)
		return theErr;
	
	theErr = MADMusicSaveCString(music, URLcString, compressMAD);
	free(URLcString);
	return theErr;
}
#endif

OSErr MADMusicExportCString(MADLibrary *lib, MADMusic *music, char *type, char* cName)
{
	if (!lib || !music || !type || !cName) {
		return MADParametersErr;
	}
	return PPExportFile(lib, type, cName, music);
}

OSErr MADMusicInfoCString(MADLibrary *lib, char *type, char* cName, PPInfoRec *InfoRec)
{
	if (lib == NULL || cName == NULL || InfoRec == NULL || type == NULL) {
		return MADParametersErr;
	}
	memset(InfoRec, 0, sizeof(PPInfoRec));
	return PPInfoFile(lib, type, cName, InfoRec);
}

OSErr MADMusicIdentifyCString(MADLibrary *lib, char *type, char *fName)
{
	if (lib == NULL || type == NULL || fName == NULL) {
		return MADParametersErr;
	}
	return PPIdentifyFile(lib, type, fName);
}

OSErr MADLoadMusicFileCString(MADLibrary *lib, MADMusic **music, char *plugType, char *fName)
{
	OSErr iErr = noErr;
	
	if (lib == NULL || music == NULL || plugType == NULL || fName == NULL) {
		return MADParametersErr;
	}
	
	if (!strcmp("MADK", plugType))
		iErr = MADLoadMADFileCString(music, fName);
	else {
		iErr = PPImportFile(lib, plugType, fName, music);
	}
	
	return iErr;
}

OSErr MADSetMusicStatus(MADDriverRec *MDriver, long minV, long maxV, long curV)
{
	short			i, x, y;
	Cmd				*aCmd = NULL;
	
	float			timeResult;
	long			time;
	long			speed, finespeed, fullTime, curTime;
	long			dstTime;
	
	if (MDriver == NULL)
		return MADParametersErr;
	
	if (MDriver->curMusic == NULL)
		return MADDriverHasNoMusic;
	
	if (MDriver->curMusic->header == NULL)
		return MADDriverHasNoMusic;
	
	if (maxV > curV)
		MDriver->musicEnd = false;
	
	MADGetMusicStatus(MDriver, &fullTime, &curTime);
	
	curV -= minV;
	maxV -= minV;
	
	if (maxV)
		dstTime = (curV * fullTime)/maxV;
	else
		dstTime = 0;
	timeResult		= 0;
	time			= 0;
	speed			= MDriver->curMusic->header->speed;
	finespeed		= MDriver->curMusic->header->tempo;
	
	for (i = 0; i < MDriver->curMusic->header->numPointers; i++) {
		for (x = 0; x < MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]->header.size; x++) {
			time ++;
			
			if (dstTime <= timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed))) {
				MDriver->PL = i;
				MDriver->PartitionReader = x;
				MDriver->Pat = MDriver->curMusic->header->oPointers[ i];
				
				MADCheckSpeed( MDriver->curMusic, MDriver);
				
				MADPurgeTrack( MDriver);
				
				return noErr;
			}
			
			for (y = 0; y <  MDriver->curMusic->header->numChn; y++) {
				aCmd = GetMADCommand( x, y, MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]);
				
				/** SpeedE **/
				if (aCmd) {
					if (aCmd->cmd == speedE) {
						/** Compute time for this interval **/
						
						timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
						time = 0;
						
						/************************************/
						
						if (aCmd->arg < 32) {
							if (aCmd->arg != 0) speed = aCmd->arg;
						} else {
							if (aCmd->arg != 0) finespeed = aCmd->arg;
						}
					}
					
					/** SkipE **/
					
					if (aCmd->cmd == skipE) {
						for (; x < MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]->header.size; x++) {
						}
					}
				}
			}
		}
	}
	
	return MADUnknownErr;
}

OSErr MADGetMusicStatus(MADDriverRec *MDriver, long *fullTime, long *curTime)
{
	short			i, x, y;
	Cmd				*aCmd;
	
	float			timeResult;
	long			time;
	long			speed, finespeed;
	
	if (MDriver == NULL) {
		*curTime = 0;
		*fullTime = 1;
		
		return MADParametersErr;
	}
	
	if (MDriver->curMusic == NULL) {
		*curTime = 0;
		*fullTime = 1;
		
		return MADDriverHasNoMusic;
	}
	
	if (MDriver->curMusic->header == NULL) {
		*curTime = 0;
		*fullTime = 1;
		
		return MADDriverHasNoMusic;
	}
	
	timeResult	= 0;
	time		= 0;
	speed		= MDriver->curMusic->header->speed;
	finespeed	= MDriver->curMusic->header->tempo;
	
	*curTime	= -1;
	
	for (i = 0; i < MDriver->curMusic->header->numPointers; i++) {
		for (x = 0; x < MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]->header.size; x++) {
			time ++;
			
			if (i == MDriver->PL && x == MDriver->PartitionReader) {
				*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
			}
			
			for (y = 0; y <  MDriver->curMusic->header->numChn; y++) {
				aCmd = GetMADCommand(x, y, MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]);
				
				if (aCmd) {
					/** SpeedE **/
					if (aCmd->cmd == speedE) {
						/** Compute time for this interval **/
						
						timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
						time = 0;
						
						/************************************/
						
						if (aCmd->arg < 32) {
							if (aCmd->arg != 0) speed = aCmd->arg;
						} else {
							if (aCmd->arg != 0) finespeed = aCmd->arg;
						}
					} else
						
					/** SkipE **/
						
						if (aCmd->cmd == skipE) {
							for (; x < MDriver->curMusic->partition[ MDriver->curMusic->header->oPointers[ i]]->header.size; x++) {
								if (i == MDriver->PL && x == MDriver->PartitionReader) {
									*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
								}
							}
						}
				}
			}
		}
	}
	
	timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
	
	*fullTime = timeResult;
	
	if (*curTime == -1)
		*curTime = *fullTime;
	
	return noErr;
}

static inline void ByteSwapsData(sData *toSwap)
{
	PPBE32(&toSwap->size);
	PPBE32(&toSwap->loopBeg);
	PPBE32(&toSwap->loopSize);
	PPBE16(&toSwap->c2spd);
}

static inline void SwapFXSets(FXSets *set)
{
#ifndef __BLOCKS__
	int y;
#endif
	PPBE16(&set->id);
	PPBE16(&set->noArg);
	PPBE16(&set->track);
	PPBE32(&set->FXID);
#ifdef __BLOCKS__
	dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t y) {
		PPBE32(&set->values[y]);
	});
#else
	for (y = 0; y < 100; y++) {
		PPBE32(&set->values[y]);
	}
#endif
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
#ifndef __BLOCKS__
	int x;
#endif
	PPBE16( &toSwap->numSamples);
	PPBE16( &toSwap->firstSample);
	PPBE16( &toSwap->volFade);
	
	PPBE16( &toSwap->MIDI);
	PPBE16( &toSwap->MIDIType);
#ifdef __BLOCKS__
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		PPBE16( &toSwap->volEnv[ x].pos);
		PPBE16( &toSwap->volEnv[ x].val);
		
		PPBE16( &toSwap->pannEnv[ x].pos);
		PPBE16( &toSwap->pannEnv[ x].val);
		
		PPBE16( &toSwap->pitchEnv[ x].pos);
		PPBE16( &toSwap->pitchEnv[ x].val);
	});
#else
	for (x = 0; x < 12; x++) {
		PPBE16( &toSwap->volEnv[ x].pos);
		PPBE16( &toSwap->volEnv[ x].val);
		
		PPBE16( &toSwap->pannEnv[ x].pos);
		PPBE16( &toSwap->pannEnv[ x].val);
		
		PPBE16( &toSwap->pitchEnv[ x].pos);
		PPBE16( &toSwap->pitchEnv[ x].val);
	}
#endif
}

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	PPBE32(&toSwap->MAD);
	PPBE16(&toSwap->speed);
	PPBE16(&toSwap->tempo);
	PPBE32(&toSwap->EPitch);
	PPBE32(&toSwap->ESpeed);
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	PPBE32(&toSwap->size);
	PPBE32(&toSwap->compMode);
	PPBE32(&toSwap->patBytes);
	PPBE32(&toSwap->unused2);
}

OSErr MADReadMAD(MADMusic **music, UNFILE srcFile, short InPutType, Handle MADRsrc, Ptr MADPtr)
{
	short 					i, x;
	size_t 					inOutCount, OffSetToSample = 0;
	OSErr					theErr = noErr;
	PatHeader				tempPatHeader;
	PatData*				tempPat;
	MADMusic 				*MDriver;
	
	*music = NULL;
	
	MDriver = (MADMusic*) calloc(sizeof(MADMusic), 1);
	if (!MDriver)
		return MADNeedMemory;
	
	MDriver->musicUnderModification = false;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	MDriver->header = (MADSpec*)calloc(inOutCount, 1);
	if (MDriver->header == NULL) {
		free(MDriver);
		return MADNeedMemory;
	}
	
	switch(InPutType)
	{
		case MADFileType:
			if (iRead(inOutCount, MDriver->header, srcFile))
				theErr = MADIncompatibleFile;
			break;
			
		case MADPtrType:
			OffSetToSample = 0;
			memcpy(MDriver->header, MADPtr, inOutCount);
			OffSetToSample += inOutCount;
			break;
			
		default:
			free(MDriver->header);
			free(MDriver);
			return MADParametersErr;
			break;
	}
	
	PPBE32(&MDriver->header->MAD);
	PPBE16(&MDriver->header->speed);
	PPBE16(&MDriver->header->tempo);
	PPBE32(&MDriver->header->EPitch);
	PPBE32(&MDriver->header->ESpeed);
	
	if (MDriver->header->MAD != 'MADK' || MDriver->header->numInstru > MAXINSTRU) {
		free(MDriver->header);
		free(MDriver);
		return MADIncompatibleFile;
	}
	
	if (MDriver->header->MultiChanNo == 0)
		MDriver->header->MultiChanNo = 48;
	
	
	/**** PARTITION ****/
	for (i = MDriver->header->numPat; i < MAXPATTERN; i++)
		MDriver->partition[ i] = NULL;
	
	for (i = 0; i < MDriver->header->numPat; i++) {
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		switch(InPutType)
		{
			case MADFileType:
				if (iRead(inOutCount, &tempPatHeader, srcFile))
					theErr = MADIncompatibleFile;
				iSeekCur(-(long)sizeof(PatHeader), srcFile);
				break;
				
			case MADPtrType:
				memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
				break;
		}
		
		PPBE32(&tempPatHeader.size);
		PPBE32(&tempPatHeader.compMode);
		PPBE32(&tempPatHeader.patBytes);
		PPBE32(&tempPatHeader.unused2);
		
		/*************************************************/
		/** Lecture du header + contenu de la partition **/
		/*************************************************/
		
		switch( tempPatHeader.compMode)
		{
			case 'MAD1':
				inOutCount = sizeof(PatHeader) + tempPatHeader.patBytes;
				break;
				
			default:
			case 'NONE':
				inOutCount = sizeof(PatHeader) + MDriver->header->numChn * tempPatHeader.size * sizeof(Cmd);
				break;
		}
		
		MDriver->partition[i] = (PatData*)malloc(inOutCount);
		if (MDriver->partition[ i] == NULL) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free( MDriver->partition[x]);
			}
			free(MDriver->header);
			free(MDriver);
			
			return MADNeedMemory;
		}
		switch(InPutType)
		{
			case MADFileType:
				if (iRead(inOutCount, MDriver->partition[i], srcFile))
					theErr = MADIncompatibleFile;
				break;
				
			case MADPtrType:
				memcpy(MDriver->partition[i], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				break;
		}
		
		ByteSwapPatHeader(&MDriver->partition[i]->header);
		
		if (MDriver->partition[i]->header.compMode == 'MAD1') {
			tempPat = DecompressPartitionMAD1(MDriver, MDriver->partition[ i]);
			
			if (tempPat == NULL) {
				for (x = 0; x < i; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			free(MDriver->partition[i]);
			MDriver->partition[i] = tempPat;
		}
	}
	
	/**** INSTRUMENTS ****/
	MDriver->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!MDriver->fid) {
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		free(MDriver);
		
		return MADNeedMemory;
	}
	
	inOutCount = sizeof(InstrData) * MDriver->header->numInstru;
	switch(InPutType)
	{
		case MADFileType:
			if (iRead(inOutCount, MDriver->fid, srcFile))
				theErr = MADIncompatibleFile;
			break;
			
		case MADPtrType:
			memcpy(MDriver->fid, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			break;
	}
	
	for (i = MDriver->header->numInstru-1; i >= 0 ; i--) {
		InstrData *curIns = &MDriver->fid[i];
		
		ByteSwapInstrData(curIns);
						
		if (i != curIns->no) {
			MDriver->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	MDriver->header->numInstru = MAXINSTRU;
	
	/**** SAMPLES ****/
	MDriver->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!MDriver->sample) {
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(MDriver, x);
		
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		free(MDriver);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < MDriver->fid[ i].numSamples ; x++) {
			sData	 *curData;
			
			// ** Read Sample header **
			
			curData = MDriver->sample[i*MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			switch(InPutType)
			{
				case MADFileType:
					if (iRead(inOutCount, curData, srcFile))
						theErr = MADIncompatibleFile;
					break;
					
				case MADPtrType:
					memcpy(curData, MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
			ByteSwapsData(curData);
			
			// ** Read Sample DATA
			
			curData->data = (Ptr)malloc(curData->size);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument( MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = curData->size;
			switch(InPutType)
			{
				case MADFileType:
					if (iRead(inOutCount, curData->data, srcFile))
						theErr = MADIncompatibleFile;
					break;
					
				case MADPtrType:
					memcpy(curData->data, MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
			//TODO: block function here.
			if (curData->amp == 16) {
				short	*shortPtr = (short*) curData->data;
#ifdef __BLOCKS__
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
					PPBE16(&shortPtr[ll]);
				});
#else
				size_t 	ll;
				for (ll = 0; ll < curData->size / 2; ll++)
					PPBE16(&shortPtr[ll]);
#endif
			}
		}
	}
#ifdef __BLOCKS__
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
		MDriver->fid[ixi].firstSample = ixi * MAXSAMPLE;
	});
#else
	for (i = 0; i < MAXINSTRU; i++)
		MDriver->fid[i].firstSample = i * MAXSAMPLE;
#endif
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		MDriver->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MDriver->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				switch(InPutType)
				{
					case MADFileType:
						if (iRead(inOutCount, &MDriver->sets[ alpha], srcFile))
							theErr = MADIncompatibleFile;
						break;
						
					case MADPtrType:
						memcpy(&MDriver->sets[ alpha], MADPtr + OffSetToSample, inOutCount);
						OffSetToSample += inOutCount;
						break;
				}
				SwapFXSets(&MDriver->sets[alpha]);
				alpha++;
			}
		}
		
		for (i = 0; i < MDriver->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MDriver->header->chanEffect[i][x]) {
					inOutCount = sizeof( FXSets);
					switch(InPutType)
					{
						case MADFileType:
							if (iRead(inOutCount, &MDriver->sets[ alpha], srcFile))
								theErr = MADIncompatibleFile;
							break;
							
						case MADPtrType:
							memcpy(&MDriver->sets[ alpha], MADPtr + OffSetToSample, inOutCount);
							OffSetToSample += inOutCount;
							break;
					}
					SwapFXSets(&MDriver->sets[alpha]);
					alpha++;
				}
			}
		}
		
	}
	// *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	
	MDriver->header->MAD = 'MADK';
	
	//MADCleanDriver( MDriver);
	
	*music = MDriver;
	
	return noErr;
}

OSErr MADMusicSaveCString(MADMusic *music, const char *cName, Boolean compressMAD)
{
	int		alpha = 0;
	int		i, x;
	size_t	inOutCount;
	UNFILE	curFile = NULL;
	OSErr	theErr = noErr;
	MADSpec aHeader;

	if (music->musicUnderModification)
		return MADWritingErr;
	
	iFileCreate(cName, 'MADK');
	curFile = iFileOpenWrite(cName);
	if (!curFile) {
		return MADWritingErr;
	}
	
	
	//TODO: error-checking
	
	music->musicUnderModification = TRUE;
	
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		music->fid[i].no = i;
		
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			x++;
		}
	}
	
	music->header->numInstru = x;
	
	aHeader = *music->header;
	ByteSwapMADSpec(&aHeader);
	iWrite(sizeof(MADSpec), &aHeader, curFile);
	
	if (compressMAD) {
		for (i = 0; i < music->header->numPat ; i++) {
			if (music->partition[i]) {
				PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
				inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
				tmpPat->header.compMode = 'MAD1';
				ByteSwapPatHeader(&tmpPat->header);
				iWrite(inOutCount, tmpPat, curFile);
				free(tmpPat);
			}
		}
	} else {
		for (i = 0; i < music->header->numPat; i++) {
			if (music->partition[i]) {
				PatData *tmpPat;
				inOutCount = sizeof(PatHeader);
				inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
				tmpPat = calloc(inOutCount, 1);
				memcpy(tmpPat, music->partition[i], inOutCount);
				tmpPat->header.compMode = 'NONE';
				ByteSwapPatHeader(&tmpPat->header);
				iWrite(inOutCount, tmpPat, curFile);
				free(tmpPat);
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			InstrData instData;
			music->fid[i].no = i;
			instData = music->fid[i];
			ByteSwapInstrData(&instData);
			iWrite(sizeof(InstrData), &instData, curFile);
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			void	*dataCopy = NULL;
			curData = *music->sample[music->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			iWrite(inOutCount, &copyData, curFile);
			
			inOutCount = music->sample[music->fid[i].firstSample + x]->size;
			dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if (curData.amp == 16) {
				short *shortPtr = (short*)dataCopy;
#ifdef __BLOCKS__
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t ll) {
					PPBE16(&shortPtr[ll]);
				});
#else
				size_t ll;
				for (ll = 0; ll < inOutCount / 2 ; ll++) {
					PPBE16(&shortPtr[ll]);
				}
#endif
			}
			iWrite(inOutCount, dataCopy, curFile);
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	for (i = 0; i < 10 ; i++) {	// Global Effects
		if (music->header->globalEffect[i]) {
			FXSets aSet;
			inOutCount = sizeof(FXSets);
			aSet = music->sets[alpha];
			SwapFXSets(&aSet);
			iWrite(inOutCount, &aSet, curFile);
			alpha++;
		}
	}
	
	for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				FXSets aSet;
				inOutCount = sizeof(FXSets);
				aSet = music->sets[alpha];
				SwapFXSets(&aSet);
				iWrite(inOutCount, &aSet, curFile);
				alpha++;
			}
		}
	}
	
	iClose(curFile);
	music->header->numInstru = MAXINSTRU;

	return theErr;
}

OSErr MADMusicSavePointer(MADMusic *music, void **outPtr, size_t *outPtrSize, Boolean compressMAD)
{
	return MADOrderNotImplemented;
}

OSErr MADKillSample(MADMusic *MDriver, short ins, short sample)
{
	short		i;
	Boolean		IsReading;
	
	if (MDriver->header == NULL)
		return MADDriverHasNoMusic;
	
	IsReading = MDriver->musicUnderModification;
	MDriver->musicUnderModification = true;
	
	if (MDriver->sample[ ins * MAXSAMPLE + sample] == NULL)
		return MADParametersErr;
	if (MDriver->fid[ ins].numSamples <= sample)
		return MADParametersErr;
	
	if (MDriver->sample[ins * MAXSAMPLE + sample]->data)
		free(MDriver->sample[ins * MAXSAMPLE + sample]->data);
	free(MDriver->sample[ins * MAXSAMPLE + sample]);
	MDriver->sample[ins * MAXSAMPLE + sample] = NULL;
	
	MDriver->fid[ins].numSamples--;
	
	for (i = sample ; i < MDriver->fid[ ins].numSamples; i++) {
		MDriver->sample[ins * MAXSAMPLE + i] = MDriver->sample[ins * MAXSAMPLE + i + 1];
	}
	
	for (i = 0; i < NUMBER_NOTES; i++) {
		if (MDriver->fid[ins].what[i] >= sample) {
			if (MDriver->fid[ins].what[i] > 0)
				MDriver->fid[ins].what[i]--;
		}
	}
	
	MDriver->musicUnderModification = IsReading;
	
	return noErr;
}

sData *MADCreateSample(MADMusic *MDriver, short ins, short sample)
{
	sData	*curData;
	
	curData = (sData*)calloc(sizeof(sData), 1);
	if (curData) {
		curData->size		= 0;
		curData->loopBeg	= 0;
		curData->loopSize	= 0;
		curData->vol		= MAX_VOLUME;
		curData->c2spd		= NOFINETUNE;
		curData->loopType	= eClassicLoop;
		curData->amp		= 8;
		curData->relNote	= 0;
		//curData->name
		curData->data		= NULL;
		
		// Install it
		
		if (sample < MDriver->fid[ ins].numSamples) PPDebugStr( __LINE__, __FILE__, "MADCreateSample");
		
		MDriver->sample[ins * MAXSAMPLE + sample] = curData;
		MDriver->fid[ins].numSamples++;
	}
	
	return curData;
}

void MADPurgeTrack(MADDriverRec *intDriver)
{
	int i;
	
	for (i = 0; i<intDriver->DriverSettings.numChn; i++) {
		intDriver->chan[i].prevPtr		= 0;
		
		intDriver->chan[i].curPtr = intDriver->chan[i].maxPtr;
		intDriver->chan[i].samplePtr = NULL;
		intDriver->chan[i].lAC		= 0;
		intDriver->chan[i].loopBeg = 0;
		intDriver->chan[i].loopSize = 0;
		intDriver->chan[i].RemoverWorking = false;
		intDriver->chan[i].TICKREMOVESIZE = 1;
		
		intDriver->chan[i].lastWordR		= 0;
		intDriver->chan[i].lastWordL		= 0;
		intDriver->chan[i].curLevelL		= 0;
		intDriver->chan[i].curLevelR		= 0;
		intDriver->chan[i].prevPtr		= 0;
		intDriver->chan[i].prevVol0		= 1;
		intDriver->chan[i].prevVol1		= 1;
		intDriver->chan[i].preOff			= 0xFFFFFFFF;
		intDriver->chan[i].preVal2		= 0;
		intDriver->chan[i].preVal2R		= 0;
		
		intDriver->chan[i].spreVal2		= 0;
		intDriver->chan[i].spreVal2R	= 0;
		
		intDriver->chan[i].preVal			= 0;
		intDriver->chan[i].spreVal		= 0;
		intDriver->chan[i].LevelDirectionR = true;
		intDriver->chan[i].LevelDirectionL = true;
	}
}

void MADPurgeTrackIfInstru(MADDriverRec *intDriver, short instru)
{
	int	i;
	
	for (i = 0; i < intDriver->DriverSettings.numChn; i++) {
		if (intDriver->chan[i].ins == instru && intDriver->chan[i].curPtr != intDriver->chan[i].maxPtr) {
			//intDriver->chan[i].lastWord		= 0;
			//intDriver->chan[i].curLevel		= 0;
			intDriver->chan[i].prevPtr			= 0;
			
			intDriver->chan[i].curPtr			= intDriver->chan[i].maxPtr;
			intDriver->chan[i].samplePtr		= NULL;
			intDriver->chan[i].lAC				= 0;
			intDriver->chan[i].loopBeg			= 0;
			intDriver->chan[i].loopSize			= 0;
			intDriver->chan[i].RemoverWorking	= false;
			intDriver->chan[i].TICKREMOVESIZE	= 1;
			
			intDriver->chan[i].lastWordR		= 0;
			intDriver->chan[i].lastWordL		= 0;
			intDriver->chan[i].curLevelL		= 0;
			intDriver->chan[i].curLevelR		= 0;
			intDriver->chan[i].prevPtr			= 0;
			intDriver->chan[i].prevVol0			= 1;
			intDriver->chan[i].prevVol1			= 1;
			intDriver->chan[i].preOff			= 0xFFFFFFFF;
			intDriver->chan[i].preVal2			= 0;
			intDriver->chan[i].preVal2R			= 0;
			
			intDriver->chan[i].spreVal2			= 0;
			intDriver->chan[i].spreVal2R		= 0;
			
			intDriver->chan[i].preVal			= 0;
			intDriver->chan[i].spreVal			= 0;
			intDriver->chan[i].LevelDirectionR	= true;
			intDriver->chan[i].LevelDirectionL	= true;
		}
	}
}

OSErr MADCleanCurrentMusic(MADMusic *MDriver, MADDriverRec *intDriver)
{
	if (MDriver->header != NULL) {
		short i, x;
		
		if (intDriver) {
			if (intDriver->Pat > MDriver->header->numPat)
				MADReset(intDriver);
			if (intDriver->PartitionReader > MDriver->partition[intDriver->Pat]->header.size)
				MADReset( intDriver);
		}
		
		for (i = 0; i < MAXINSTRU; i++) {
			for (x = 0; x < 96; x++) {
				if (MDriver->fid[ i].what[ x]) {
					if (MDriver->fid[ i].what[ x] >= MDriver->fid[ i].numSamples)
						MDriver->fid[ i].what[ x] = 0;
				}
			}
			
			for (x = 0; x < MDriver->fid[ i].numSamples; x++) {
				sData	*curData = MDriver->sample[ i * MAXSAMPLE + x];
				
				if (curData == NULL)
					curData = MDriver->sample[i * MAXSAMPLE + x] = MADCreateSample(MDriver, i, x);
				
				if (curData->data == NULL) {
					curData->data = malloc( 0); //FIXME: I don't think this is valid...
					curData->size = 0;
				}
				
				if (curData->size <= 0) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopSize < 0)
					curData->loopSize = 0;
				
				if (curData->loopBeg > curData->size) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopBeg + curData->loopSize > curData->size) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
			}
		}
	}
	
	return noErr;
}

OSErr MADResetInstrument(InstrData *curIns)
{
	if (curIns == NULL)
		return MADParametersErr;
	
	memset(curIns, 0, sizeof(InstrData));
	curIns->volFade = DEFAULT_VOLFADE;
	return noErr;
}

OSErr MADKillInstrument(MADMusic *music, short ins)
{
	short		i;
	InstrData	*curIns;
	Boolean		IsReading;
	
	if (music == NULL)
		return MADParametersErr;
	
	curIns = &music->fid[ins];
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = 0; i < curIns->numSamples; i++) {
		if (music->sample[ins * MAXSAMPLE + i] != NULL) {
			if (music->sample[ins * MAXSAMPLE + i]->data != NULL) {
				free(music->sample[ins * MAXSAMPLE + i]->data);
				music->sample[ins * MAXSAMPLE + i]->data = NULL;
			}
			free(music->sample[ins * MAXSAMPLE + i]);
			music->sample[ins * MAXSAMPLE + i] = NULL;
		}
	}
	
	MADResetInstrument( curIns);
	
	music->musicUnderModification = IsReading;
	
	return noErr;
}

OSErr MADKillCmd(Cmd *aCmd)
{
	if (aCmd == NULL)
		return MADParametersErr;
	
	aCmd->cmd 	= 0;
	aCmd->note 	= 0xFF;
	aCmd->arg 	= 0;
	aCmd->ins 	= 0;
	aCmd->vol 	= 0xFF;
	
	return noErr;
}

void MADCheckSpeed( MADMusic *MDriver, MADDriverRec *intDriver)
{
	short		i, x, y;
	short		curPartitionReader;
	Cmd			*aCmd;
	Boolean		CmdSpeed = false, FineFound = false, ArgFound[MAXTRACK], NoteFound[MAXTRACK], InsFound[MAXTRACK];
	
	if (!MDriver)
		return;
	if (MDriver->header == NULL)
		return;
	if (!intDriver)
		return;
	
	for (i = 0; i < MAXTRACK; i++) {
		ArgFound[i] = false;
		NoteFound[i] = false;
		InsFound[i] = false;
	}
	
	for (i = intDriver->PL; i >= 0; i--) {
		if (i == intDriver->PL) {
			curPartitionReader = intDriver->PartitionReader;
			if (curPartitionReader >= MDriver->partition[MDriver->header->oPointers[i]]->header.size)
				curPartitionReader--;
		} else {
			curPartitionReader = MDriver->partition[MDriver->header->oPointers[i]]->header.size-1;
		}
		
		for (x = curPartitionReader; x >= 0; x--) {
			for (y = MDriver->header->numChn-1; y >= 0 ; y--) {
				Channel	*ch = &intDriver->chan[y];
				
				aCmd = GetMADCommand(x, y, MDriver->partition[MDriver->header->oPointers[i]]);
				
				if (aCmd->cmd == speedE)
				{
					if (aCmd->arg < 32)
					{
						if (aCmd->arg != 0)
						{
							if (CmdSpeed == false) intDriver->speed = aCmd->arg;
							CmdSpeed = true;
						}
					}
					else
					{
						if (aCmd->arg != 0)
						{
							if (FineFound == false) intDriver->finespeed = aCmd->arg;
							FineFound = true;
						}
					}
				}
				
				if (aCmd->arg != 0)
				{
					if (!ArgFound[ y]) ch->oldArg[ aCmd->cmd] = aCmd->arg;
					ArgFound[ y] = true;
				}
				
				if (aCmd->ins != 0)
				{
					if (!InsFound[ y]) ch->insOld = aCmd->ins;
					InsFound[ y] = true;
				}
				
				if (aCmd->note != 0xFF)
				{
					if (!NoteFound[ y]) ch->noteOld = aCmd->note;
					NoteFound[ y] = true;
				}
			}
		}
	}
	
	
	if (!CmdSpeed)
		intDriver->speed 		= MDriver->header->speed;
	if (!FineFound)
		intDriver->finespeed 	= MDriver->header->tempo;
}

void MADCheckSpeedPattern(MADMusic *MDriver, MADDriverRec *intDriver)
{
	short		x, y;
	short		curPartitionReader;
	Cmd			*aCmd;
	Boolean		CmdSpeed = false, FineFound = false;
	
	if (!MDriver)
		return;
	if (MDriver->header == NULL)
		return;
	if (!intDriver)
		return;
	
	
	curPartitionReader = intDriver->PartitionReader;
	if (curPartitionReader >= MDriver->partition[ intDriver->Pat]->header.size) curPartitionReader--;
	
	for (x = curPartitionReader; x >= 0; x--) {
		for (y = MDriver->header->numChn-1; y >= 0 ; y--) {
			//Channel	*ch = &intDriver->chan[ y];
			
			aCmd = GetMADCommand( x, y, MDriver->partition[ intDriver->Pat]);
			
			if (aCmd) {
				if (aCmd->cmd == speedE) {
					if (aCmd->arg < 32) {
						if (aCmd->arg != 0) {
							if (CmdSpeed == false)
								intDriver->speed = aCmd->arg;
							CmdSpeed = true;
						}
					} else {
						if (aCmd->arg != 0) {
							if (FineFound == false)
								intDriver->finespeed = aCmd->arg;
							FineFound = true;
						}
					}
				}
			}
		}
	}
	
	if (!CmdSpeed)
		intDriver->speed 		= MDriver->header->speed;
	if (!FineFound)
		intDriver->finespeed 	= MDriver->header->tempo;
}

OSErr MADPlayMusic(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	if (!MDriver->curMusic)
		return MADDriverHasNoMusic;
	
	MDriver->Reading = true;
	
	return noErr;
}

Boolean MADIsPlayingMusic(MADDriverRec *driver)
{
	if (driver == NULL)
		return false;
	
	return driver->Reading;
}

OSErr MADStopMusic(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	if (!MDriver->curMusic)
		return MADDriverHasNoMusic;
	
	MDriver->Reading = false;
	//#ifdef _MIDIHARDWARE_
	if (MDriver->SendMIDIClockData)
		SendMIDIClock( MDriver, 0xFC);
	//#endif
	
	return noErr;
}

OSErr MADStartDriver(MADDriverRec *MDriver)
{
	if (MDriver == NULL)
		return MADParametersErr;
	
	MDriver->MADPlay = true;
	MDriver->Reading = false;
	MDriver->musicEnd = false;
	
	MADCleanDriver(MDriver);
	
	MADCheckSpeed(MDriver->curMusic, MDriver);
	
	switch(MDriver->DriverSettings.driverMode)
	{
		case MIDISoundDriver:
			//PlayChannel( MDriver);
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			break;
			
#endif
#ifdef _ESOUND
		case ESDDriver:
			PlayChannelESD(MDriver);
			break;
#endif
			
#ifdef LINUX
		case ALSADriver:
			PlayChannelALSA(MDriver);
			break;
#endif
			
#ifdef _OSSSOUND
		case OSSDriver:
			PlayChannelOSS(MDriver);
			break;
#endif
	}
	
	return noErr;
}

OSErr MADStopDriver(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	
	MDriver->MADPlay = false;
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;
	
	MDriver->levelL = 0;
	MDriver->levelR = 0;
	
	switch(MDriver->DriverSettings.driverMode)
	{
		case MIDISoundDriver:
			//AllNoteOff( MDriver);
			//StopChannel( MDriver);
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			StopChannelESD(MDriver);
			break;
#endif
			
#ifdef LINUX
		case ALSADriver:
			StopChannelALSA(MDriver);
			break;
#endif
			
#ifdef _OSSSOUND
		case OSSDriver:
			StopChannelOSS(MDriver);
			break;
#endif
		default:
			break;
	}
	
	MADCleanDriver(MDriver);
	
	//#ifdef _MIDIHARDWARE_
	if (MDriver->SendMIDIClockData)
		SendMIDIClock(MDriver, 0xFC);
	//#endif
	
	return noErr;
}

OSErr MADReset(MADDriverRec *MDriver)
{
	MADCleanDriver(MDriver);
	
	MDriver->BufCounter = 0;
	MDriver->BytesToGenerate = 0;
	MDriver->smallcounter = 128;
	
	MDriver->PL = 0;
	MDriver->PartitionReader = 0;
	
	if (MDriver->curMusic != NULL) {
		MDriver->Pat = MDriver->curMusic->header->oPointers[MDriver->PL];
		MDriver->speed = MDriver->curMusic->header->speed;
		MDriver->finespeed = MDriver->curMusic->header->tempo;
	}
	
	return noErr;
}

short MADGetNextReader(MADMusic *music, MADDriverRec *intDriver, short cur, short *pat)
{
	if (music == NULL)
		return 0;
	if (intDriver == NULL)
		return 0;
	if (music->header == NULL)
		return 0;
	
	cur++;
	if (cur >= music->partition[intDriver->Pat]->header.size) {
		cur = 0;
		
		if (intDriver->JumpToNextPattern) {
			*pat = music->header->oPointers[intDriver->PL + 1];
			
			if (intDriver->speed == 1 && intDriver->PL + 1 >= music->header->numPointers)
				*pat = music->header->oPointers[0];
		}
	}
	
	return cur;
}

OSErr MADDisposeMusic(MADMusic **music, MADDriverRec *MDriver)
{
	short		x;
	
	if (!music)
		return noErr;
	if (!*music)
		return noErr;
	
	(*music)->musicUnderModification = true;
	
	if (MDriver) {
		if (MDriver->curMusic == *music)
			MDriver->curMusic = NULL;
	}
	
	if ((*music)->header != NULL) {
		for (x = 0; x < (*music)->header->numPat ; x++) {
			if ( (*music)->partition[ x] != NULL)
				free((*music)->partition[x]);
			(*music)->partition[x] = NULL;
		}
		
		for (x = (*music)->header->numPat; x < MAXPATTERN ; x++) {
			if ((*music)->partition[ x] != NULL)
				return MADUnknownErr;
		}
		
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(*music, x);
		
		free( (*music)->header);
		
		if ((*music)->fid == NULL)
			return MADUnknownErr;
		free((*music)->fid);
		
		if ((*music)->sample == NULL)
			return MADUnknownErr;
		free((*music)->sample);
		
		if ((*music)->sets == NULL)
			return MADUnknownErr;
		free((*music)->sets);
	}
	
	free(*music);
	*music = NULL;
	
	return noErr;
}

#pragma pack(push, 2)

Boolean	MADIsPressed(unsigned char* km2, unsigned short k)
{
	return (Boolean)((km2[k >> 3] >> (k & 7)) & 1);
}

#if 0
OSErr MADPlaySoundDataSYNC( MADDriverRec *MDriver, char *soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, double rate, Boolean stereo)
{
	OSErr		iErr;
	Boolean		continueLoop;
	KeyMap		km;
	
	iErr = MADPlaySoundData( MDriver, soundPtr, size, channel, note, amplitude, loopBeg, loopSize, rate, stereo);
	if (iErr == noErr)
	{
		continueLoop = true;
		while( continueLoop)
		{
			GetKeys( km);
			
			if (MDriver->chan[ channel].samplePtr == NULL) continueLoop = false;
			if (MADIsPressed( (unsigned char*) km, 0x37) && MADIsPressed( (unsigned char*) km, 0x2F)) continueLoop = false;
			if (Button()) continueLoop = false;
			
#if MAINPLAYERPRO
			DoGlobalNull();
			WaitNextEvent( everyEvent, &theEvent, 1, NULL);
#endif
		}
		
		if (MDriver->chan[ channel].samplePtr != NULL)
		{
			MDriver->chan[ channel].curPtr 		= MDriver->chan[ channel].maxPtr;
			MDriver->chan[ channel].samplePtr	= NULL;
			MDriver->chan[ channel].lAC			= 0;
			MDriver->chan[ channel].loopBeg 	= 0;
			MDriver->chan[ channel].loopSize 	= 0;
		}
	}
	
	return iErr;
}
#endif

OSErr MADPlaySoundData( MADDriverRec *MDriver, char *soundPtr, size_t size, SInt32 channel, SInt32 note, SInt32 amplitude, size_t loopBeg, size_t loopSize, unsigned int rate, Boolean stereo)
{
	Channel *curVoice;
	
	if (channel < 0 || channel >= MDriver->DriverSettings.numChn)
		return MADParametersErr;
	
	if (MDriver->curMusic != NULL)
		MDriver->curMusic->musicUnderModification = true;
	
	curVoice = &MDriver->chan[channel];
	
	curVoice->prevPtr		= NULL;
	curVoice->samplePtr		= soundPtr;
	curVoice->stereo		= stereo;
	
	curVoice->maxPtr = curVoice->curPtr = curVoice->begPtr = soundPtr;
	curVoice->maxPtr		+= size;
	curVoice->sizePtr		= size;
	curVoice->lAC			= 0;
	curVoice->pingpong		= false;
	curVoice->PanningE8		= false;
	curVoice->trig			= 0;
	curVoice->preOff		= 0xFFFFFFFF;
	curVoice->preVal		= 0;
	curVoice->spreVal		= 0;
	curVoice->preVal2		= *curVoice->curPtr;
	if (curVoice->amp == 8)
		curVoice->preVal2R	= *(curVoice->curPtr+1);
	else
		curVoice->preVal2R	= *(curVoice->curPtr+2);
	curVoice->spreVal2	= *(short*)curVoice->curPtr;
	curVoice->spreVal2R	= *(short*)(curVoice->curPtr+2);
	
	if (note == 0xFF)
		note = 48;
	curVoice->note		= note;
	curVoice->ins		= 0;
	curVoice->viboffset	= 0;
	curVoice->amp		= amplitude;
	curVoice->fineTune	= rate;
	
	curVoice->period	= GetOldPeriod(curVoice->note, curVoice->fineTune, MDriver);
	
	if (loopBeg > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	if (loopBeg + loopSize > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	curVoice->loopBeg 		= loopBeg;
	curVoice->loopSize	 	= loopSize;
	
	if (loopSize > 0)
		curVoice->maxPtr 	= (Ptr)((size_t)curVoice->begPtr + loopBeg + loopSize);
	
	curVoice->pann			= 32;
	
	curVoice->vol 			= 64;
	curVoice->volFade		= 32767;
	curVoice->nextvolFade	= 32767;
	curVoice->volEnv		= 64;
	curVoice->KeyOn			= true;
	
	if (MDriver->curMusic != NULL)
		MDriver->curMusic->musicUnderModification = false;
	
	return noErr;
}

void MADKeyOFF(MADDriverRec *MDriver, short track)
{
	if (track == -1) {
		short i;
		
		for (i = 0; i < MAXTRACK; i++)
			MDriver->chan[i].KeyOn = false;
	} else
		MDriver->chan[track].KeyOn = false;
}

#if 0
OSErr MADPlaySndHandle( MADDriverRec *MDriver, Handle sound, long channel, long note)
{
	Ptr 			soundPtr;
	short 			soundFormat;
	short 			numSynths, numCmds;
	long 			offset;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader = NULL;
	ExtSoundHeader	*ExtHeader = NULL;
	long			i;
	Boolean			stereo = false;
	Channel			*curVoice;
	OSType			Scomp;
	unsigned long	Ssize, Samp, SbaseFreq, Sc2spd, Sstart, Send;
	Ptr				rPtr;
	
	/* make the sound safe to use at interrupt time. */
	HLock( sound);
	
	soundPtr = *sound;
	
	/* determine what format sound we have. */
	soundFormat = *(short*)soundPtr;
	
	switch(soundFormat)
	{
		case 1:						/* format 1 sound. */
			/* look inside the format 1 resource and deduce offsets. */
			numSynths = ((short*)soundPtr)[1];					/* get # synths. */
			numCmds = *(short*)(soundPtr+4+numSynths*6);		/* get # commands. */
			break;
			
		case 2:						/* format 2 sound. */
			numSynths = 0;			/* format 2 sounds have no synth's. */
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:					/* jack says, what about 12? or 6? */
			HUnlock( sound);
			return MADUnknowErr;
			break;
	}
	
	/* compute address of sound header. */
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) ((*sound) + offset);
	
	switch( header->encode)
	{
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			if (CmpHeader->numChannels > 2)
			{
				HUnlock( sound);
				return MADUnknowErr;
			}
			
			if (CmpHeader->numChannels == 2) stereo = true;
			else stereo = false;
			
			Sstart		= CmpHeader->loopStart;
			Send		= CmpHeader->loopEnd;
			Samp		= CmpHeader->sampleSize;
			Sc2spd		= CmpHeader->sampleRate;
			SbaseFreq 	= CmpHeader->baseFrequency;
			Ssize 		= CmpHeader->numFrames;
			rPtr 		= (Ptr) CmpHeader->sampleArea;
			Scomp		= CmpHeader->compressionID;
			break;
			
		case extSH:
			ExtHeader	= (ExtSoundHeader*) header;
			if (ExtHeader->numChannels > 2)
			{
				HUnlock( sound);
				return MADUnknowErr;
			}
			
			if (ExtHeader->numChannels == 2) stereo = true;
			else stereo = false;
			
			Sstart		= ExtHeader->loopStart;
			Send		= ExtHeader->loopEnd;
			Samp		= ExtHeader->sampleSize;
			Sc2spd		= ExtHeader->sampleRate;
			SbaseFreq 	= ExtHeader->baseFrequency;
			Ssize 		= ExtHeader->numFrames;
			rPtr 		= (Ptr) ExtHeader->sampleArea;
			Scomp		= 'NONE';
			break;
			
		default:
		case stdSH:
			Sstart		= header->loopStart;
			Send		= header->loopEnd;
			if (header->encode == 0x40) Samp		= 0;
			else Samp		= 8;
			Sc2spd		= header->sampleRate;
			SbaseFreq 	= header->baseFrequency;
			Ssize 		= header->length;
			rPtr 		= (Ptr) header->sampleArea;
			Scomp		= 'NONE';
			break;
	}
	
	if (Samp == 16)
	{
		Ssize 	*= 2L;
		Sstart 	*= 2L;
		Send 	*= 2L;
		
		if (stereo)
		{
			Ssize 	*= 2L;
			Sstart 	*= 2L;
			Send 	*= 2L;
		}
	}
	else
	{
		if (Samp == 8)
		{
			if (stereo)
			{
				Ssize 	*= 2L;
				Sstart 	*= 2L;
				Send 	*= 2L;
			}
			
			for (i = 0; i < Ssize; i++) rPtr[ i] = rPtr[ i] - 0x80;
			
			switch( header->encode)
			{
				case extSH:
					ExtHeader->sampleSize = 0;
					break;
					
				case cmpSH:
					CmpHeader->sampleSize = 0;
					break;
					
				default:
				case stdSH:
					header->encode = 0x40;
					break;
			}
		}
		Samp = 8;
	}
	
	/*********************/
	
	if (note == 0xFF) note = 48;
	
	return MADPlaySoundData( MDriver, rPtr, Ssize, channel, note + (60 - SbaseFreq), Samp, Sstart, Send - Sstart, Sc2spd, stereo);
}

#endif

#pragma pack(pop)

Cmd* GetMADCommand( short PosX, short	TrackIdX, PatData*	tempMusicPat)
{
	if (tempMusicPat == NULL) {
		return NULL;
	}
	
	if (PosX < 0) PosX = 0;
	else if (PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
	
	return( &(tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

void MADDisposeVolumeTable( MADDriverRec *intDriver)
{
	if (intDriver == NULL) {
		return;
	}
	if (intDriver->DriverSettings.outPutMode == DeluxeStereoOutPut) MADKillOverShoot( intDriver);
}

OSErr MADCreateVolumeTable( MADDriverRec *intDriver)
{
	SInt32 Tracks = 0;
	OSErr theErr;
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	Tracks = intDriver->DriverSettings.numChn;
	
	
	theErr = MADCreateMicroDelay( intDriver);			if (theErr != noErr) return theErr;
	
	switch( intDriver->DriverSettings.outPutMode)
	{
		case DeluxeStereoOutPut:			Tracks	= 1;		MADCreateOverShoot( intDriver);	break;
		case PolyPhonic:					Tracks 	= 1;		break;
	}
	
	return noErr;
}

void MADChangeTracks( MADDriverRec *MDriver, short newVal)
{
	Boolean	play = MDriver->MADPlay, reading = MDriver->Reading;
	
	MADStopDriver( MDriver);
	
	MDriver->DriverSettings.numChn = newVal;
	
	MADDisposeVolumeTable( MDriver);
	MADCreateVolumeTable( MDriver);
	
	MDriver->trackDiv = MDriver->DriverSettings.numChn;
	
	if (play) MADStartDriver( MDriver);
	if (reading) MADPlayMusic( MDriver);
}

void UpdateTracksNumber(MADDriverRec *MDriver)
{
	if (MDriver->curMusic != NULL) {
		if (MDriver->curMusic->header != NULL) {
			if (MDriver->curMusic->header->numChn != MDriver->DriverSettings.numChn)
				MADChangeTracks(MDriver, MDriver->curMusic->header->numChn);
		}
	}
}

OSErr MADCreateVibrato(MADDriverRec *MDriver)
{
	short vibrato_table[64] =
	{
		0,24,49,74,97,120,141,161,
		180,197,212,224,235,244,250,253,
		255,253,250,244,235,224,212,197,
		180,161,141,120,97,74,49,24
	};
	
	/*	{
	 0,50,100,149,196,241,284,325,362,396,426,452,473,490,502,510,512,
	 510,502,490,473,452,426,396,362,325,284,241,196,149,100,50,0,-49,
	 -99,-148,-195,-240,-283,-324,-361,-395,-425,-451,-472,-489,-501,
	 -509,-511,-509,-501,-489,-472,-451,-425,-395,-361,-324,-283,-240,
	 -195,-148,-99,-49
	 };*/
	
	
	memcpy(MDriver->vibrato_table, vibrato_table, sizeof(vibrato_table));
	
	return noErr;
}

enum
{
	ins 	= 1 << 0,
	note	= 1 << 1,
	cmd		= 1 << 2,
	argu	= 1 << 3,
	vol		= 1 << 4
};

PatData* DecompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData*				finalPtr;
	Byte 					*srcPtr;
	Cmd						*myCmd;
	short					maxCmd;
	Byte					set;
	
	finalPtr = (PatData*)malloc(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * sizeof(Cmd));
	if (finalPtr == NULL)
		return NULL;
	
	memcpy(finalPtr, myPat, sizeof( PatHeader));
	
	srcPtr = (Byte*)myPat->Cmds;
	myCmd = (Cmd*)finalPtr->Cmds;
	maxCmd = finalPtr->header.size * MDriver->header->numChn;
	
	/*** Decompression Routine ***/
	
	while (maxCmd != 0) {
		maxCmd--;
		
		MADKillCmd( myCmd);
		
		set = *srcPtr++;
		
		if (set & ins)
			myCmd->ins = *srcPtr++;
		
		if (set & note)
			myCmd->note = *srcPtr++;
		
		if (set & cmd)
			myCmd->cmd = *srcPtr++;
		
		if (set & argu)
			myCmd->arg = *srcPtr++;
		
		if (set & vol)
			myCmd->vol = *srcPtr++;
		
		myCmd++;
	}
	
	return finalPtr;
}

PatData* CompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData		*finalPtr;
	UInt8 		*dstPtr, *setByte;
	Cmd			*myCmd;
	SInt32		maxCmd;
	size_t		NewPtrSize = 0;
	UInt8		set;
	
	finalPtr = (PatData*)malloc(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * 6L);
	if (finalPtr == NULL)
		return NULL;
	
	memcpy(finalPtr, myPat, sizeof( PatHeader));
	
	dstPtr = (UInt8*)finalPtr->Cmds;
	myCmd = (Cmd*)myPat->Cmds;
	maxCmd = myPat->header.size * MDriver->header->numChn;
	
	/*** Compression Routine ***/
		
	while (maxCmd != 0) {
		maxCmd--;
		
		// Set byte
		set			= 0;
		setByte 	= dstPtr;
		*dstPtr++ 	= 0x00;
		NewPtrSize++;
		////////
		
		if (myCmd->ins > 0) {
			set |= ins;
			*dstPtr++ = myCmd->ins;
			NewPtrSize++;
		}
		
		if (myCmd->note != 0xFF) {
			set |= note;
			*dstPtr++ = myCmd->note;
			NewPtrSize++;
		}
		
		if (myCmd->cmd > 0) {
			set |= cmd;
			*dstPtr++ = myCmd->cmd;
			NewPtrSize++;
		}
		
		if (myCmd->arg > 0) {
			set |= argu;
			*dstPtr++ = myCmd->arg;
			NewPtrSize++;
		}
		
		if (myCmd->vol != 0xFF) {
			set |= vol;
			*dstPtr++ = myCmd->vol;
			NewPtrSize++;
		}
		// Set byte
		*setByte = set;
		////////
		
		myCmd++;
	}
	
	finalPtr->header.patBytes = (SInt32)NewPtrSize;
	
	finalPtr = realloc(finalPtr, NewPtrSize + sizeof(PatHeader));
	
	return finalPtr;
}

Boolean MADWasReading(MADDriverRec *driver)
{
	return MADIsPlayingMusic(driver);
}

void MADSetReading(MADDriverRec *driver, Boolean toSet)
{
	if (toSet == true) {
		MADPlayMusic(driver);
	} else {
		MADStopMusic(driver);
	}
}

Boolean MADIsDonePlaying(MADDriverRec *MDriver)
{
	if (!MDriver) {
		return false;
	}
	return MDriver->musicEnd;
}

void MADBeginExport(MADDriverRec *driver)
{
	if (driver) {
		driver->currentlyExporting = true;
	}
}

void MADEndExport(MADDriverRec *driver)
{
	if (driver) {
		driver->currentlyExporting = false;
	}
}

Boolean MADIsExporting(MADDriverRec *driver)
{
	if (!driver) {
		return false;
	} else {
		return driver->currentlyExporting;
	}
}

SInt32 MADAudioLength(MADDriverRec *theRec)
{
	if (!theRec)
		return 0;
	return (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd);
}
