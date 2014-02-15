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
#endif

#ifdef _MIDIHARDWARE_
#ifdef __MACH__
#include <CoreMIDI/CoreMIDI.h>
#endif
#endif

#ifdef _MAC_H
#include <Carbon/Carbon.h>
#endif

void debugger(Ptr a);
//long MYstrlen(Ptr cStr);
void MYP2CStr(unsigned char *cStr);
unsigned char* MYC2PStr(Ptr cStr);

///////////////////////////////

//#define  NO_ASM

enum {
	MADFileType = 1,
	MADRsrcType = 2,
	MADPtrType = 3
};

#ifdef _MAC_H
void CheckVSTEditor(VSTEffect *ce);
void SendMIDIClock(MADDriverRec *intDriver, Byte MIDIByte);
void DisposeVSTEffect(VSTEffect	*myEffect);
VSTEffect* CreateVSTEffect(short effectID);
short ConvertUniqueIDToIndex(long);
void ApplyVSTSets(VSTEffect* myEffect, FXSets* set);
#endif

MADMusic* CreateFreeMADK(void)
{
	MADMusic	*music = (MADMusic*) NewPtrClear(sizeof(MADMusic));
	MADSpec		*MADI;
	short		i, x, z;
	Cmd			*aCmd;
	if (music == NULL)
		return NULL;
	// ******* HEADER *********
	
	MADI = (MADSpec*) NewPtrClear(sizeof(MADSpec));
	if (MADI == NULL)
	{
		DisposePtr((Ptr)music);
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
		if (i % 2 == 0) MADI->chanPan[i] = MAX_PANNING/4;
		else MADI->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		
		MADI->chanVol[i] = MAX_VOLUME;
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		MADI->chanBus[i].copyId = i;
	}
	
	music->fid = (InstrData*) NewPtrClear(sizeof(InstrData) * (long) MAXINSTRU);
	if (music->fid == NULL)
	{
		DisposePtr((Ptr)MADI);
		DisposePtr((Ptr)music);
		return NULL;
	}
	
	music->sample = (sData**) NewPtrClear(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE);
	if (music->sample == NULL)
	{
		DisposePtr((Ptr)music->fid);
		DisposePtr((Ptr)MADI);
		DisposePtr((Ptr)music);
		return NULL;
	}
	// ******* PARTITION *********
	
	for(i=0; i < MADI->numPat; i++)
	{
		music->partition[i] = (PatData*) NewPtrClear(sizeof(PatHeader) + MADI->numChn * 64L * sizeof(Cmd));
		if (music->partition[i] == NULL)
		{
			for(i = 0; i < MADI->numPat; i++)
			{
				if (music->partition[i] != NULL)
					DisposePtr((Ptr)music->partition[i]);
			}
			DisposePtr((Ptr)music->fid);
			DisposePtr((Ptr)music->sample);
			DisposePtr((Ptr)MADI);
			DisposePtr((Ptr)music);
			return NULL;
		}

		music->partition[i]->header.size 		= 64L;
		music->partition[i]->header.compMode 	= 'NONE';
		music->partition[i]->header.patBytes 	= 0;
		music->partition[i]->header.unused2 		= 0;
		
		for (x = 0; x < 64; x++)
		{
			for (z = 0; z < music->header->numChn; z++)
			{
				aCmd = GetMADCommand(x,  z, music->partition[i]);
				
				MADKillCmd(aCmd);
			}
		}
	}
	for (i = music->header->numPat; i < MAXPATTERN ; i++) music->partition[i] = NULL;
	
	music->sets = (FXSets*)NewPtrClear(MAXTRACK * sizeof(FXSets));
	
	return music;
}

/*
void WriteMADKFile(FSSpec *newFile, MADMusic	*music)
{
	short	i, fRefNum;
	OSErr	iErr;
	long	inOutCount, tt;

	iErr = FSpOpenDF(newFile, 0, &fRefNum);
	
	if (iErr != noErr)
	{
		iErr = FSpCreate(newFile, 0, 'SNPL', MADID);
		iErr = FSpOpenDF(newFile, 0, &fRefNum);
	}

	inOutCount = sizeof(MADSpec);
	iErr = FSWrite(fRefNum, &inOutCount, music->header);
	
	for (i = 0; i < music->header->numPat ; i++)
	{
		if (thePrefs.MADCompression) music->partition[i]->header.compMode = 'MAD1';
		
		if (music->partition[i]->header.compMode == 'MAD1')
		{
			PatData* PatMAD = CompressPartitionMAD1(music, music->partition[i]);
			inOutCount = PatMAD->header.patBytes + sizeof(struct PatHeader);
			
			iErr = FSWrite(fRefNum, &inOutCount, PatMAD);
			
			Myfree((Ptr*) &PatMAD);
		}
		else
		{
			inOutCount = sizeof(PatHeader);
			inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);

			iErr = FSWrite(fRefNum, &inOutCount, music->partition[i]);
		}
	}
	
	GetFPos(fRefNum, &tt);
	SetEOF(fRefNum, tt);
	FSClose(fRefNum);
}*/

long MADGetMusicSize(MADMusic	*music)
{
	short i, x;
	long fileSize;
	
	if (music->header == NULL) return 0;
	
	fileSize = sizeof(MADSpec);
	fileSize += (long) music->header->numInstru * sizeof(InstrData);
	for (i = 0; i < music->header->numPat; i++) fileSize += sizeof(PatHeader) + music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
	for (i = 0; i < music->header->numInstru ; i++)
	{
		for (x = 0; x < music->fid[i].numSamples ; x++)
		{
			sData	*curData = music->sample[i * MAXSAMPLE + x];
			
			fileSize += sizeof(sData);
			fileSize += curData->size;
		}
	}
	
	return fileSize;
}

#ifdef _MAC_H
void ConvertTo64Rows(MADMusic *music)
{
	long		i, x, z;
	Boolean		IsReading;
	long		patID, found;
	
	if (music->header == NULL) return;
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = music->header->numPat-1; i >= 0 ; i--)
	{
		// Resize pattern to 64 rows and put a pattern break
		
		long		newSize;
		PatData		*newPat = NULL;
		
		patID = i;
		
		newSize = sizeof(PatHeader) + music->header->numChn * 64L * sizeof(Cmd);
		
		if (music->partition[i]->header.size < 64)
		{
			Cmd		*srccmd, *dstcmd;
			long	patsize;
			
			newPat = (PatData*) NewPtrClear(newSize);
			
			newPat->header.size 		= 64L;
			newPat->header.compMode 	= 'NONE';
			newPat->header.patBytes 	= 0;
			newPat->header.unused2 		= 0;
			
			BlockMoveData(music->partition[i]->header.name, newPat->header.name, 32);
			
			// Upgrade length to 64
			
			for (x = 0, patsize = 0; x < 64; x++, patsize++)
			{
				for (z = 0; z < music->header->numChn; z++)
				{
					if (x < music->partition[i]->header.size)
					{
						// src
						srccmd = GetMADCommand(patsize, z, music->partition[i]);
						
						// dst
						dstcmd = GetMADCommand(x, z, newPat);
						if (dstcmd && srccmd) {
							*dstcmd = *srccmd;
						}
					}
					else
					{
						dstcmd = GetMADCommand(x, z, newPat);
						
						MADKillCmd(dstcmd);
					}
				}
			}
			
			// dst
			dstcmd = GetMADCommand(music->partition[i]->header.size-1, 0, newPat);		// Pose le pattern break
			
			MADKillCmd(dstcmd);
			
			dstcmd->cmd = skipE;
			dstcmd->arg = 0;
			
			// Replace old pattern
			
			DisposePtr((Ptr) music->partition[i]);
			music->partition[i] = NULL;
			
			music->partition[i] = newPat;
		}
		else if (music->partition[i]->header.size > 64)
		{
			long 		patsize = 0;
			PatData*	srcPat = music->partition[i];
			
			while (patsize < srcPat->header.size)
			{
				newPat = (PatData*) NewPtrClear(newSize);
				
				newPat->header.size 		= 64L;
				newPat->header.compMode 	= 'NONE';
				newPat->header.patBytes 	= 0;
				newPat->header.unused2 		= 0;
				
				BlockMoveData(srcPat->header.name, newPat->header.name, 32);
				
				for (x = 0; x < 64; x++, patsize++)
				{
					for (z = 0; z < music->header->numChn; z++)
					{
						Cmd		*srccmd, *dstcmd;
						
						if (patsize < srcPat->header.size)
						{
							// src
							srccmd = GetMADCommand(patsize, z, srcPat);
							
							// dst
							dstcmd = GetMADCommand(x, z, newPat);
							
							*dstcmd = *srccmd;
						}
						else
						{
							dstcmd = GetMADCommand(x, z, newPat);
							MADKillCmd(dstcmd);
						}
					}
				}
				
				// Update patterns list & partition
				
				if (patID != i)
				{
					for (x = music->header->numPat; x > patID ; x--)
					{
						music->partition[x] = music->partition[x - 1];
					}
					music->header->numPat++;
				}
				
				for (x = music->header->numPat; x < MAXPATTERN; x++) music->partition[x] = NULL;
				
				music->partition[patID] = newPat;
				
				patID++;
			}
			patID--;
			
			
			// Do we need a pattern break ?
			
			if ((srcPat->header.size / 64) * 64 != srcPat->header.size)
			{
				short	breakpos;
				Cmd		*dstcmd;
				
				breakpos = srcPat->header.size - (srcPat->header.size / 64) * 64;
				
				dstcmd = GetMADCommand(breakpos-1, 0, newPat);
				MADKillCmd(dstcmd);
				if (dstcmd) {
					dstcmd->cmd = skipE;
					dstcmd->arg = 0;
				}
			}
			
			// Update la partition list
			
			for (x = 0; x < music->header->numPointers ; x++)
			{
				if (music->header->oPointers[x] > i) music->header->oPointers[x] += patID - i;
			}
			
			found = 0;
			
			for (x = music->header->numPointers-1; x >= 0 ; x--)
			{
				if (music->header->oPointers[x] == i)
				{
					found++;
				
					// Avance le reste de la partition
					
					for (z = 256-1; z >= x ; z--)
					{
						if (z + patID - i< 256)
						{
							music->header->oPointers[z + patID - i] = music->header->oPointers[z];
						}
					}
					
					for (z = 0; z <= patID - i; z++)
					{
						music->header->oPointers[x + z] = i + z;
					}
				}
			}
			
			music->header->numPointers += found * (patID - i);
			
			DisposePtr((Ptr) srcPat);
		}
	}
	
	music->musicUnderModification = IsReading;
}

long MADMinimize(MADMusic *music)
{
	short 		i, x, z;
	Boolean		remove, IsReading;
	Boolean		inst[MAXINSTRU];
 	long		before, after;
 	
 	if (music->header == NULL) return 0;
 	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
 	
	// Check size before
	
	before = MADGetMusicSize(music);
	
	// Check for unused patterns
	
	if (music->header->numPat > 1)
	{
		for (i = 0; i < music->header->numPat; i++)
		{
			remove = true;
			for (x = 0; x < music->header->numPointers; x++)
			{
				if (music->header->oPointers[x] == i)
				{
					remove = false;
					break;
				}
			}
			
			if (remove)
			{
				music->header->numPat--;
				
				DisposePtr((Ptr) music->partition[i]);
				music->partition[i] = NULL;
				
				for (x = i; x < music->header->numPat; x++)
				{
					music->partition[x] = music->partition[x + 1];
				}
				for (x = music->header->numPat; x < MAXPATTERN; x++) music->partition[x] = NULL;
				
				/***** Update la partition list ******/
				
				for (x = 0; x < 128; x++)
				{
					if (music->header->oPointers[x] > i) music->header->oPointers[x]--;
					else if (music->header->oPointers[x] == i) music->header->oPointers[x] = 0;
				}
				
				if (i > 0) i--;
			}
		}
	}
	
	// Check for unused instruments
	for (i = 0; i < 32 ; i++) inst[i] = false;
	
	for (i = 0; i < music->header->numPat; i++)
	{
		for (x = 0; x < music->partition[i]->header.size; x++)
		{
			for (z = 0; z < music->header->numChn; z++)
			{
				Cmd		*cmd;
				
				cmd = GetMADCommand(x, z, music->partition[i]);
				
				if (cmd->ins > 0) inst[cmd->ins-1] = true;
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++)
	{
		if (inst[i] == false) MADKillInstrument(music, i);
	}
	
	// Check for after loop data
	for (i = 0; i < MAXINSTRU; i++)
	{
		for (x = 0; x < music->fid[i].numSamples; x++)
		{
			sData	*curData = music->sample[i * MAXSAMPLE + x];
			
			if (curData->loopSize > 2)
			{
				if (curData->size > curData->loopBeg + curData->loopSize)
				{
					SetPtrSize(curData->data, curData->loopBeg + curData->loopSize);
					
					curData->size = curData->loopBeg + curData->loopSize;
				}
			}
		}
	}
	
	music->musicUnderModification = IsReading;
	
	after = MADGetMusicSize(music);
	
	return before - after;
}
#endif

static int driverList = 0;

static void BuildAvailableDriverList()
{
	if (driverList == 0) {
		driverList =   1 << MIDISoundDriver |
#ifdef _BE_H
		1 << BeOSSoundDriver |
#endif
#ifdef WIN32
		1 << DirectSound95NT |
		1 << Wave95NT |
#endif
#ifdef _MAC_H
		1 << CoreAudioDriver |
		1 << SoundManagerDriver |
#endif
#ifdef LINUX
		1 << ALSADriver |
#endif
#ifdef _OSSSOUND
		1 << OSSDriver |
#endif
#ifdef _ESOUND
		1 << ESDDriver |
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

void MADGetBestDriver(MADDriverSettings	*Init)
{
#ifdef _MAC_H
	SInt32			gestaltAnswer;
	short			myBit;
	NumVersion		nVers;
	Boolean			Stereo, StereoMixing, NewSoundManager, NewSoundManager31, hasASC, Audio16;
	
	/***************					****************/
	/****** HARDWARE IDENTIFICATION AND CHECK **********/
	/***************					****************/
	
	/****************/
	/** ASC CHIP ? **/
	/****************/
	
	Gestalt(kSoundOutputDeviceType, &gestaltAnswer);
	gestaltAnswer = EndianS32_BtoN(gestaltAnswer);
	if (gestaltAnswer == kASCSubType) hasASC = true;
	else hasASC = false;
	
	/**************/
	/** STEREO ? **/
	/**************/
	
	Gestalt(gestaltSoundAttr, &gestaltAnswer);
	gestaltAnswer = EndianS32_BtoN(gestaltAnswer);
	myBit = gestaltStereoCapability;
	Stereo = BitTst(&gestaltAnswer, 31-myBit);
	
	/******************************/
	/** INTERNAL STEREO MIXING ? **/
	/******************************/
	
	myBit = gestaltStereoMixing;
	StereoMixing = BitTst(&gestaltAnswer, 31-myBit);
	
	/***********************/
	/** 16 BITS SUPPORT ? **/
	/***********************/
	
	myBit = gestalt16BitSoundIO;
	Audio16 = BitTst(&gestaltAnswer, 31-myBit);
	
	/****************************/
	/** SOUND MANAGER >= 3.0 ? **/
	/****************************/
	
	nVers = SndSoundManagerVersion();
	NewSoundManager31 = false;	NewSoundManager = false;
	if (nVers.majorRev >= 3 && nVers.minorAndBugRev >= 0x10)
	{
		NewSoundManager31 = true;
		NewSoundManager = true;
	}
	else if (nVers.majorRev >= 3) NewSoundManager = true;
	
	Init->numChn			= 4;
	Init->surround			= false;
	Init->repeatMusic		= true;
	Init->sysMemory			= false;
	Init->MicroDelaySize	= 25;
	Init->Reverb			= false;
	Init->ReverbSize		= 100;
	Init->ReverbStrength	= 20;
	Init->oversampling		= 1;
	
	// STEREO OR MONO ?
	
	Init->outPutMode = DeluxeStereoOutPut;
	
	// RATE AND SIZE
	
	if (NewSoundManager == true)
	{
		Init->driverMode = SoundManagerDriver;
		
		////////
		
#if !(defined(TARGET_RT_MAC_CFM) || defined(TARGET_RT_MAC_MACHO))
		if (NewSoundManager31)
		{
			OSErr	iErr;
			
			iErr = GetSoundOutputInfo(NULL, siSampleRate, &Init->outPutRate);			if(iErr) goto oldWay;
			iErr = GetSoundOutputInfo(NULL, siSampleSize, (void*) &Init->outPutBits);	if(iErr) goto oldWay;
		}
		else
#endif
		{
		oldWay:;
			
			if (Audio16)			{	Init->outPutBits = 16;	Init->outPutRate = rate44khz;}
			else					{	Init->outPutBits = 8;	Init->outPutRate = rate22khz;}
		}
	}
	else
	{
		Init->driverMode = SoundManagerDriver;
		
		Init->outPutBits = 8;
		Init->outPutRate = rate22khz;
	}
#else
	
#endif
}

OSErr MADCreateMicroDelay(MADDriverRec *intDriver)
{
	intDriver->MDelay = (intDriver->DriverSettings.MicroDelaySize * (intDriver->DriverSettings.outPutRate >> 16)* intDriver->DriverSettings.oversampling) / 1000;
	
	return noErr;
}

void MADDisposeReverb(MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb) DisposePtr(intDriver->ReverbPtr);
	intDriver->ReverbPtr = NULL;
}

OSErr MADCreateReverb(MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb) {
		intDriver->RDelay = (intDriver->DriverSettings.ReverbSize * (intDriver->DriverSettings.outPutRate >> 16L)) / 1000;
		
		switch (intDriver->DriverSettings.outPutBits) {
			case 8:
				intDriver->ReverbPtr = NewPtr(intDriver->RDelay * 2L);
				if (intDriver->ReverbPtr == NULL) return MADNeedMemory;
				memset(intDriver->ReverbPtr, 0x80, intDriver->RDelay * 2);
					break;
				
			case 16:
				intDriver->ReverbPtr = NewPtrClear(intDriver->RDelay * 4L);
				if (intDriver->ReverbPtr == NULL) return MADNeedMemory;
					break;
		}
	}
	
	return noErr;
}

void MADDisposeDriverBuffer(MADDriverRec *intDriver)
{
	short i;
	
	for (i = 0; i < MAXDRAW; i++) {
		if (intDriver->OsciDrawPtr[i] != NULL)
			DisposePtr(intDriver->OsciDrawPtr[i]);
		intDriver->OsciDrawPtr[i] = NULL;
	}
	
	if (intDriver->IntDataPtr != NULL)
		DisposePtr(intDriver->IntDataPtr);
	intDriver->IntDataPtr = NULL;
}

OSErr MADCreateDriverBuffer(MADDriverRec *intDriver)
{
	short	i, x;
	long BufSize = 0;
	
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	
	BufSize = intDriver->ASCBUFFER;
	
	switch (intDriver->DriverSettings.outPutMode) {
		case MonoOutPut:
			break;
			
		case StereoOutPut:
		case DeluxeStereoOutPut:
			BufSize *= 2;
			break;
	}
	
	switch (intDriver->DriverSettings.outPutBits) {
		case 16:
			BufSize *= 2;
			break;
	}
	
	intDriver->curDrawPtr = 0;
	for (i = 0; i < MAXDRAW; i++) {
		intDriver->newData[i] = false;
		intDriver->OsciDrawPtr[i] = (Ptr)MADNewPtrClear(BufSize / intDriver->DriverSettings.oversampling, intDriver->lib);
		if (intDriver->OsciDrawPtr[i] == NULL) {
			for (x = 0; x < MAXDRAW; x++) {
				if (intDriver->OsciDrawPtr[i] != NULL) {
					DisposePtr(intDriver->OsciDrawPtr[i]);
				}
			}
			return MADNeedMemory;
		}
	}
	
	intDriver->IntDataPtr = (Ptr)MADNewPtr(BufSize, intDriver->lib);
	if (intDriver->IntDataPtr == NULL) {
		for (x = 0; x < MAXDRAW; x++) {
			if (intDriver->OsciDrawPtr[i] != NULL) {
				DisposePtr(intDriver->OsciDrawPtr[i]);
			}
		}
		return MADNeedMemory;
	}
	
	intDriver->BufSize			= BufSize;
	intDriver->OscilloWavePtr	= intDriver->IntDataPtr;
	intDriver->OscilloWaveSize	= BufSize / intDriver->DriverSettings.oversampling;
	
	return noErr;
}

OSErr MADCreateTiming(MADDriverRec *intDriver)
{
	/*************************/
	/**     Rate table      **/
	/*************************/
	
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	
	intDriver->VSYNC	= ((intDriver->DriverSettings.outPutRate>>16) * 125L * intDriver->DriverSettings.oversampling) / (50);
	intDriver->trackDiv = intDriver->DriverSettings.numChn;
//	if (intDriver->DriverSettings.outPutMode == StereoOutPut) intDriver->trackDiv /= 2;

	return noErr;
}


OSErr MADChangeDriverSettings(MADDriverSettings	*DriverInitParam, MADDriverRec** returnDriver)
{
	MADMusic	*music;
	Boolean		playing;
	OSErr		err;
	long		fullTime, curTime;
	if(DriverInitParam == NULL || returnDriver == NULL)
	{
		return MADParametersErr;
	}
	music = (*returnDriver)->curMusic;
	playing = (*returnDriver)->Reading;
	
	MADGetMusicStatus(*returnDriver, &fullTime, &curTime);
	
	if ((err = MADStopDriver(*returnDriver)) != noErr) return err;
	if ((err = MADDisposeDriver(*returnDriver)) != noErr) return err;
	
	if ((err = MADCreateDriver(DriverInitParam, (*returnDriver)->lib, returnDriver)) != noErr) return err;
	
	if ((err = MADStartDriver(*returnDriver)) != noErr) return err;
	
	if (music)
	{
		MADAttachDriverToMusic(*returnDriver, music, NULL);
		
		MADSetMusicStatus(*returnDriver, 0, fullTime, curTime);
		
		if (playing) MADPlayMusic(*returnDriver);
	}
	
	return noErr;
}

void OpenMIDIHardware();

OSErr MADCreateDriver(MADDriverSettings	*DriverInitParam, MADLibrary *lib, MADDriverRec** returnDriver)
{
	OSErr 					theErr;
	long					i;
	MADDriverRec*			MDriver;

	if(DriverInitParam == NULL || lib == NULL || returnDriver == NULL)
	{
		return MADParametersErr;
	}
	*returnDriver = NULL;
	
	/*************************/
	/** Paramaters checking **/
	/*************************/
	
	theErr = noErr;
	
	if (DriverInitParam->numChn % 2 != 0)
	{
		DriverInitParam->numChn /= 2;
		DriverInitParam->numChn *= 2;
	//	DriverInitParam->numChn++;
	}
	if (DriverInitParam->numChn < 2) theErr = MADParametersErr;
	if (DriverInitParam->numChn > MAXTRACK) theErr = MADParametersErr;
	
	if (DriverInitParam->outPutBits != 8 && DriverInitParam->outPutBits != 16) theErr = MADParametersErr;
	
	if (DriverInitParam->outPutRate < rate5khz) theErr = MADParametersErr;
	if (DriverInitParam->outPutRate > rate48khz) theErr = MADParametersErr;
	
	if (DriverInitParam->outPutMode != DeluxeStereoOutPut &&
		DriverInitParam->outPutMode != PolyPhonic) theErr = MADParametersErr;
	
	if (DriverInitParam->driverMode != SoundManagerDriver &&
	   DriverInitParam->driverMode != MIDISoundDriver &&
	   DriverInitParam->driverMode != BeOSSoundDriver &&
	   DriverInitParam->driverMode != DirectSound95NT &&
	   DriverInitParam->driverMode != Wave95NT &&
	   DriverInitParam->driverMode != ASIOSoundManager &&
	   DriverInitParam->driverMode != CoreAudioDriver &&
   	   DriverInitParam->driverMode != ALSADriver &&
	   DriverInitParam->driverMode != OSSDriver &&
	   DriverInitParam->driverMode != ESDDriver &&
	   DriverInitParam->driverMode != NoHardwareDriver) theErr = MADParametersErr;
	
	if (DriverInitParam->MicroDelaySize < 0) 		theErr = MADParametersErr;
	if (DriverInitParam->MicroDelaySize > 1000) 	theErr = MADParametersErr;
	
	if (DriverInitParam->Reverb)
	{
		if (DriverInitParam->ReverbSize < 25) 			theErr = MADParametersErr;
		if (DriverInitParam->ReverbSize > 1000) 		theErr = MADParametersErr;
		
		if (DriverInitParam->ReverbStrength < 0) 		theErr = MADParametersErr;
		if (DriverInitParam->ReverbStrength > 70) 		theErr = MADParametersErr;
	}
	
	if (DriverInitParam->oversampling < 1)	theErr = MADParametersErr;
	if (DriverInitParam->oversampling > 30)	theErr = MADParametersErr;
	
	if (theErr != noErr)
	{
		return theErr;
	}
	
	/*************************/
#if !defined(HAS_LONG_LONG) || !defined(HAS_LONG_DOUBLE)
	DriverInitParam->oversampling = 1;// We do NOT support oversampling on compilers that don't have long longs and long doubles
#endif
	
	MDriver = (MADDriverRec*) NewPtrClear(sizeof(MADDriverRec));
	
	MDriver->lib = lib;
	MDriver->curMusic = NULL;
	
	MDriver->Reading = false;
	MDriver->wasReading = false;
	
	theErr = MADStopDriver(MDriver);
	if (theErr != noErr) 
	{
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	
	//theErr = MADDisposeDriver(MDriver);
	//if (theErr != noErr) return theErr;
	
	theErr = MADCreateVibrato(MDriver);
	if (theErr != noErr) 
	{
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	
	for (i = 0; i < MAXTRACK; i++) MDriver->Active[i] = true;
	
	MDriver->DriverSettings		= *DriverInitParam;
	MDriver->musicEnd			= false;
	MDriver->Reading			= false;
	MDriver->JumpToNextPattern	= true;
	MDriver->smallcounter		= 128;			// Start immediately
	MDriver->BufCounter			= 0;
	MDriver->BytesToGenerate		= 0;
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
	MDriver->MIN_PITCH			= GetOldPeriod(NUMBER_NOTES-1, NOFINETUNE, MDriver);	//
	MDriver->MAX_PITCH			= GetOldPeriod(0, NOFINETUNE, MDriver);

	MDriver->MOD_MIN_PITCH		= GetOldPeriod(24 + 4*12 -1, NOFINETUNE, MDriver);
	MDriver->MOD_MAX_PITCH		= GetOldPeriod(24, NOFINETUNE, MDriver);
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;
	
	MDriver->levelL = 0;
	MDriver->levelR = 0;

	
	MADCleanDriver(MDriver);
	
	/*************************/
	/** 	Driver MODE	    **/
	/*************************/
	
	switch (MDriver->DriverSettings.driverMode)
	{
		case ASIOSoundManager:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
			
#ifdef _MAC_H
		case MIDISoundDriver:
			MDriver->ASCBUFFER = 370;
			MDriver->ASCBUFFER = (MDriver->ASCBUFFER * (MDriver->DriverSettings.outPutRate>>16L) * MDriver->DriverSettings.oversampling) / 22254L;
			MDriver->ASCBUFFER /= 2;
			MDriver->ASCBUFFER *= 2;
			if (MDriver->ASCBUFFER < 370) MDriver->ASCBUFFER = 370;
			break;
			
		case CoreAudioDriver:
		case SoundManagerDriver:
			/*	MDriver->ASCBUFFER = 512;
			 MDriver->ASCBUFFER = (MDriver->ASCBUFFER * (MDriver->DriverSettings.outPutRate>>16L) * MDriver->DriverSettings.oversampling) / 22254L;
			 MDriver->ASCBUFFER /= 2;
			 MDriver->ASCBUFFER *= 2;
			 if (MDriver->ASCBUFFER < 370) MDriver->ASCBUFFER = 512;*/
			
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
		case NoHardwareDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
			
		case BeOSSoundDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			
			if (MDriver->DriverSettings.outPutBits == 8) MDriver->ASCBUFFER *= 2L;
			break;
			
		case DirectSound95NT:
			MDriver->ASCBUFFER = 7500 * MDriver->DriverSettings.oversampling;
			break;
			
		case Wave95NT:
			MDriver->ASCBUFFER = 7500 * MDriver->DriverSettings.oversampling;
			break;
			
		default:
			MDriver->DriverSettings.driverMode = NoHardwareDriver;
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			//			return MADParametersErr;
			break;
	}
	
	MDriver->ASCBUFFERReal = MDriver->ASCBUFFER / MDriver->DriverSettings.oversampling;
	
	theErr = MADCreateDriverBuffer(MDriver);		
	if (theErr != noErr)
	{
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	theErr = MADCreateTiming(MDriver);				
	if (theErr != noErr) 
	{
		MADDisposeDriverBuffer(MDriver);
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	theErr = MADCreateReverb(MDriver);				
	if (theErr != noErr) 
	{
		MADDisposeDriverBuffer(MDriver);
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	theErr = MADCreateVolumeTable(	MDriver);		
	if (theErr != noErr) 
	{
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	theErr = MADInitEqualizer(MDriver);			
	if (theErr != noErr) 
	{
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		DisposePtr((Ptr)MDriver);
		return theErr;
	}
	
	/**********************************************/
	/**    Interruption - Hardware Support       **/
	/**********************************************/
	
	switch (MDriver->DriverSettings.driverMode)
	{
		case ASIOSoundManager:
			break;
			
#ifdef _MIDIHARDWARE_
		case MIDISoundDriver:
			
			OpenMIDIHardware();
			
			/*	MDriver->gOutNodeRefNum = -1;
			 OMSAddPort('SNPL', 'out ', omsPortTypeOutput, NULL, NULL, &MDriver->MIDIPortRefNum);*/
			
			theErr = InitDBSoundManager(MDriver, initStereo);
			if (theErr != noErr)
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				DisposePtr((Ptr)MDriver);
				return theErr;
			}
#endif
			
#ifdef _MAC_H
		case SoundManagerDriver:
			theErr = InitDBSoundManager(MDriver, initStereo);
			if (theErr != noErr) 
{
return theErr;
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				DisposePtr((Ptr)MDriver);
				return theErr;
}
			break;
			
		case CoreAudioDriver:
			theErr = initCoreAudio(MDriver, initStereo);
			if (theErr != noErr) 
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free((Ptr)MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef WIN32
		case DirectSound95NT:
			if (!DirectSoundInit(MDriver)) theErr = MADUnknowErr;
			if (theErr != noErr) 
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				DisposePtr(MDriver);
				return theErr;
			}
			break;
		
		case Wave95NT:
			if (!W95_Init(MDriver)) theErr = MADUnknowErr;
			if (theErr != noErr) 
			{
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				DisposePtr(MDriver);
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
				DisposePtr(MDriver);
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
				DisposePtr(MDriver);
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
				DisposePtr(MDriver);
				return theErr;
			}
			break;			
#endif
			
		case BeOSSoundDriver:
			
			break;
			
		case NoHardwareDriver:
		
			break;
	}
	
	*returnDriver = MDriver;
	
	return noErr;
}

OSErr MADDisposeDriver(MADDriverRec* MDriver)
{
	if (MDriver->IntDataPtr == NULL) return noErr;
	
	MDriver->Reading = false;
	
	MADCleanDriver(MDriver);
	
	switch (MDriver->DriverSettings.driverMode)
	{
#ifdef _MAC_H
		case MIDISoundDriver:
			AllNoteOff(MDriver);
			DBSndClose(MDriver);
			break;
			
		case ASIOSoundManager:
			break;
			
		case SoundManagerDriver:
			DBSndClose(MDriver);
			break;
			
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
	
	MADDisposeDriverBuffer(MDriver);			if (MemError()) return MADUnknowErr;
	MADDisposeVolumeTable(MDriver);			if (MemError()) return MADUnknowErr;
	MADDisposeReverb(MDriver);					if (MemError()) return MADUnknowErr;
	
	DisposePtr((Ptr) MDriver);
	
	return noErr;
}

#ifdef _MAC_H
OSErr MADInitLibraryNew(FSRefPtr PlugsFolder, MADLibrary **lib)
{
	long 	i, mytab[12] = {
		1712L*16L,1616L*16L,1524L*16L,1440L*16L,1356L*16L,1280L*16L,
		1208L*16L,1140L*16L,1076L*16L,1016L*16L,960L*16L,907L*16L
	};
	
	if (PlugsFolder != NULL) {
		Boolean UnusedBool1, isFolder;
		OSErr aliasErr = noErr;
		aliasErr = FSResolveAliasFile(PlugsFolder, TRUE, &isFolder, &UnusedBool1);
		if (isFolder == FALSE) {
			return MADParametersErr;
		}
	}
	
	*lib = (MADLibrary*) NewPtrClear(sizeof(MADLibrary));
	
	if (*lib == NULL)
		return MADNeedMemory;
	
	(*lib)->IDType = 'MADD';
	(*lib)->sysMemory = FALSE;
	
	for (i = 0; i < 12; i++) {
		(*lib)->mytab[i] = mytab[i];
	}
	
	MADInitImportPlug(*lib, PlugsFolder);
	return noErr;
}

OSErr MADInitLibrary(FSSpec *PlugsFolderName, Boolean sysMemory, MADLibrary **lib)
{
	if(sysMemory == TRUE)
		fprintf(stderr, "PlayerPROCore: sysMemory Definition in MADInitLibrary is ignored\n");
	
	if(PlugsFolderName != NULL) {
		FSRef TempRef;
		OSErr iErr = noErr;
		iErr = FSpMakeFSRef(PlugsFolderName, &TempRef);
		if (iErr != noErr) {
			return MADInitLibraryNew(NULL, lib);
		} else
			return MADInitLibraryNew(&TempRef, lib);
	}
	return MADInitLibraryNew(NULL, lib);
}
#else

OSErr MADInitLibrary(FSSpec *PlugsFolderName, Boolean sysMemory, MADLibrary **lib)
{
	OSErr errmess = noErr;
	long 	i, mytab[12] =
	{
		1712L*16L,1616L*16L,1524L*16L,1440L*16L,1356L*16L,1280L*16L,
		1208L*16L,1140L*16L,1076L*16L,1016L*16L,960L*16L,907L*16L
	};
	
	*lib = (MADLibrary*) NewPtrClear(sizeof(MADLibrary));
	
	if (*lib == NULL) return MADNeedMemory;
	
	(*lib)->IDType = 'MADD';
	(*lib)->sysMemory = FALSE;
	
	for (i = 0; i < 12; i++)
	{
		(*lib)->mytab[i] = mytab[i];
	}
	
	MInitImportPlug(*lib, PlugsFolderName);
	return noErr;
}

#endif

OSErr MADDisposeLibrary(MADLibrary *MLibrary)
{
	if (MLibrary != NULL)
	{
		CloseImportPlug(MLibrary);
		MLibrary->IDType = 'XXXX';
		
		DisposePtr((Ptr) MLibrary);
		MLibrary = NULL;
	}
	return noErr;
}

OSErr MADAttachDriverToMusic(MADDriverRec *driver, MADMusic *music, unsigned char *MissingPlugs)
{
#if MAINPLAYERPRO
	short		alpha, x, i, index;
#endif
	Boolean		needToReset;

	if (!driver) return MADParametersErr;
	if (!music) return MADParametersErr;
	
	driver->musicEnd = false;
	
	if (music != driver->curMusic) needToReset = true;
	else needToReset = false;
	
	// Save old preferences
	
	if (driver->curMusic)
	{
		if (driver->curMusic->header)
		{
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
	CheckVSTEditor(NULL);
#endif
	
	/////////////////////////
	
	driver->curMusic = music;
	driver->VolGlobal	= music->header->generalVol;		if (driver->VolGlobal <= 0)	driver->VolGlobal = 64;
	
	if (music->header->generalSpeed == 252)
	{
		driver->VExt		= music->header->ESpeed;
		driver->FreqExt		= music->header->EPitch;	
	}
	else
	{
		driver->VExt		= music->header->generalSpeed;		if (driver->VExt <= 160) 	driver->VExt *= 100;		if (driver->VExt <= 160) driver->VExt = 8000;
		driver->FreqExt		= music->header->generalPitch;		if (driver->FreqExt <= 160)	driver->FreqExt *= 100;		if (driver->FreqExt <= 160) driver->FreqExt = 8000;
	}
	
	if (music->header->generalPan == 0) music->header->generalPan = 64;
	driver->globPan			= music->header->generalPan;
	
	if (music->header->MultiChanNo < 4) music->header->MultiChanNo = 48;
	if (music->header->MultiChanNo > 99) music->header->MultiChanNo = 99;
	
	if (music->header->MultiChanNo <= 0) driver->MultiChanNo = 48;
	else driver->MultiChanNo = music->header->MultiChanNo;
	
	if (music->header->MODMode) driver->MODMode = true;
	else driver->MODMode = false;
	
	if (music->header->XMLinear) driver->XMLinear = true;
	else driver->XMLinear = false;
	
	
#if MAINPLAYERPRO
	if (MissingPlugs) strcpy((Ptr) MissingPlugs, "");
#endif
	
	//INSTALL ALL VST EFFECTS !!!!!!
#if MAINPLAYERPRO
//TODO: VST Effects
	// Purge previous Effects !
	for (i = 0; i < 10 ; i++)
	{
		if (driver->masterVST[i])
		{
			DisposeVSTEffect(driver->masterVST[i]);
			driver->masterVST[i] = NULL;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++)	// Channel Effects
	{
		for (x = 0; x < 4; x++)
		{
			if (driver->chanVST[i][x])
			{
				DisposeVSTEffect(driver->chanVST[i][x]);
				driver->chanVST[i][x] = NULL;
			}
		}
	}
	
	// Install new effects
	alpha = 0;
	
	for (i = 0; i < 10 ; i++)	// Global Effects
	{
		if (music->header->globalEffect[i])
		{
			index = ConvertUniqueIDToIndex(music->sets[alpha].FXID);
			
			if (index >= 0)
			{
				driver->masterVST[i] = CreateVSTEffect(index);
				ApplyVSTSets(driver->masterVST[i], &music->sets[alpha]);
			}
			else if (MissingPlugs)
			{
				if (strlen((Ptr) MissingPlugs) > 0) strcat((Ptr) MissingPlugs, ", ");
				MYP2CStr(music->sets[alpha].name);
				strcpy((Ptr) MissingPlugs,  (Ptr) &music->sets[alpha].name);
				MYC2PStr((Ptr) music->sets[alpha].name);
			}
			alpha++;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++)	// Channel Effects
	{
		for (x = 0; x < 4; x++)
		{
			if (music->header->chanEffect[i][x])
			{
				index = ConvertUniqueIDToIndex(music->sets[alpha].FXID);
			
				if (index >= 0)
				{
					driver->chanVST[i][x] = CreateVSTEffect(index);
					ApplyVSTSets(driver->chanVST[i][x], &music->sets[alpha]);
				}
				else if (MissingPlugs)
				{
					strcat((Ptr) MissingPlugs, ", ");
					MYP2CStr(music->sets[alpha].name);
					strcpy((Ptr) MissingPlugs,  (Ptr) &music->sets[alpha].name);
					MYC2PStr((Ptr) music->sets[alpha].name);
				}
				alpha++;
			}
		}
	}
	
	if (MissingPlugs) MYC2PStr((Ptr) MissingPlugs);
#endif
	
	if (needToReset) MADReset(driver);
	UpdateTracksNumber(driver);
	
	return noErr;
}

#ifdef _MAC_H
OSErr MADLoadMusicRsrc(MADMusic **music, OSType IDName, short	IDNo)
{
	Handle	theRsrc;
	OSErr	theErr;
	
	SetResLoad(false);
	theRsrc = GetResource(IDName, IDNo);
	if (theRsrc == NULL) return MADReadingErr;
	
	theErr = MADReadMAD(music, 0, MADRsrcType, theRsrc, NULL);
	if (theErr != noErr)
	{
		ReleaseResource(theRsrc);
		SetResLoad(true);
		
		//MADDisposeMusic(music, MADDriver);
		return theErr;
	}
	
	ReleaseResource(theRsrc);
	SetResLoad(true);
	
	return noErr;
}
#endif

OSErr MADLoadMusicPtr(MADMusic **music, Ptr myPtr)
{
	OSErr	theErr;
	
//	MADDisposeMusic(music);
	
	theErr = MADReadMAD(music, 0, MADPtrType, NULL, myPtr);
	if (theErr != noErr) return theErr;
	
	return noErr;
}

OSErr MADLoadMADFileCString(MADMusic **music, Ptr fName)
{
	OSErr	theErr;
	UNFILE	srcFile;
	
	if(fName == NULL || music == NULL)
		return MADParametersErr;

	//MADDisposeMusic(music);
	
	srcFile = iFileOpen(fName);
	if (srcFile == 0)
		return MADReadingErr;
	
	theErr = MADReadMAD(music, srcFile, MADFileType, NULL, NULL);
	if (theErr != noErr) {
		iClose(srcFile);
		//MADDisposeMusic(music, MADDriver);
		return theErr;
	}
	
	iClose(srcFile);
	
	return noErr;
}

#ifdef _MAC_H
OSErr MADSetHardwareVolume(long vol)
{
	Point	tempL;
	long	*tL;
	
	if (vol >= 0) {
		tempL.v = vol;
		tempL.h = vol;
		
		tL = (long*) &tempL;
		
		SetDefaultOutputVolume(*tL);
	}
	
	return noErr;
}

long MADGetHardwareVolume()
{
	Point	tempL;
	long	vol = 0;
	
	GetDefaultOutputVolume((long*) &tempL);
	
	vol = tempL.h;
	
	return vol;
}

OSErr MADLoadMusicFSRefFile(MADLibrary *lib, MADMusic **music, char *type, FSRefPtr theRef)
{
	OSErr	iErr;
	FSSpec	oldspec;
	Boolean UnusedBool1, UnusedBool2;
	iErr = FSResolveAliasFile(theRef, TRUE, &UnusedBool1, &UnusedBool2);
	if(iErr == fnfErr)
		return MADReadingErr;
	else
		iErr = noErr;
	
	iErr = FSGetCatalogInfo(theRef, kFSCatInfoNone, NULL, NULL, &oldspec, NULL);
	if (iErr)
		return iErr;
	
	return MADLoadMusicFSpFile(lib, music, type, &oldspec);
}

OSErr MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef)
{
	FSRef tempRef;
	CFURLGetFSRef(theRef, &tempRef);
	return MADLoadMusicFSRefFile(lib, music, type, &tempRef);
}

OSErr MADLoadMusicFSpFile(MADLibrary *lib, MADMusic **music, char *plugType, FSSpec *theSpec)
{
	OSErr			iErr;
	FSSpec			saved;
	
	HGetVol(NULL, &saved.vRefNum, &saved.parID);
	HSetVol(NULL, theSpec->vRefNum , theSpec->parID);
	MYP2CStr(theSpec->name);
	
	if (!strcmp("MADK", plugType))
		iErr = MADLoadMADFileCString(music, (Ptr) theSpec->name);
	else
		iErr = MADLoadMusicFileCString(lib, music, plugType, (Ptr) theSpec->name);
	
	MYC2PStr((Ptr) theSpec->name);
	HSetVol(NULL, saved.vRefNum , saved.parID);
	
	return iErr;
}

OSErr MADLoadMusicFilePString(MADLibrary *lib, MADMusic **music, char *plugType, Str255 fName)
{
	OSErr				iErr;
	
	MYP2CStr(fName);
	
	if (!strcmp("MADK", plugType)) 	iErr = MADLoadMADFileCString(music, (Ptr) fName);
	else								iErr = MADLoadMusicFileCString(lib, music, plugType, (Ptr) fName);
	
	MYC2PStr((Ptr) fName);
	
	return iErr;
}

/*
 OSErr MADCopyCurrentPartition(MADMusic *aPartition)
 {
 long	i, x;
 
 if (MDriver->header == NULL) return -1;
 if (aPartition == NULL) return -1;
 
 aPartition->header = (MADSpec*) MADNewPtr(sizeof(MADSpec), &MDriver->DriverSettings);
 if (aPartition->header == NULL) return -1;
 
 BlockMoveData(MDriver->header, aPartition->header, sizeof(MADSpec));
 
 for (i = 0; i < MDriver->header->numPat ; i++)
 {
 aPartition->partition[i] = (PatData*) MADNewPtr(GetPtrSize((Ptr) MDriver->partition[i]), &MDriver->DriverSettings);
 if (aPartition->partition[i] == NULL) return -1;
 
 BlockMoveData(MDriver->partition[i], aPartition->partition[i], GetPtrSize((Ptr) MDriver->partition[i]));
 }
 for (i = MDriver->header->numPat; i < MAXPATTERN ; i++) aPartition->partition[i] = NULL;
 
 for (i = 0; i < MAXINSTRU ; i++)
 {
 for (x = 0; x < MDriver->header->fid[i].numSamples; x++)
 {
 sData	*dstData, *srcData = MDriver->sample[i][x];
 
 aPartition->sample[i][x] = dstData = (sData*) MADNewPtr(sizeof(sData), &MDriver->DriverSettings);
 if (dstData == NULL) return -1;
 
 BlockMoveData(srcData, dstData, sizeof(sData));
 
 dstData->data = MADNewPtr(srcData->size, &MDriver->DriverSettings);
 if (dstData->data == NULL) return -1;
 
 BlockMoveData(srcData->data, dstData->data, srcData->size);
 }
 }
 
 return noErr;
 }*/

OSErr MADMusicIdentifyFSRef(MADLibrary *lib, char *type, FSRefPtr theRef)
{
	FSSpec	oldspec;
	OSErr	iErr = noErr;
	Boolean UnusedBool1, UnusedBool2;
	iErr = FSResolveAliasFile(theRef, TRUE, &UnusedBool1, &UnusedBool2);
	if(iErr == fnfErr)
		return MADReadingErr;
	else
		iErr = noErr;
	
	iErr = FSGetCatalogInfo(theRef, kFSCatInfoNone, NULL, NULL, &oldspec, NULL);
	if (iErr)
		return iErr;

	iErr = MADMusicIdentifyFSp(lib, type, &oldspec);
	
	return iErr;
}

OSErr MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef)
{
	FSRef tempRef;
	CFURLGetFSRef(URLRef, &tempRef);
	return MADMusicIdentifyFSRef(lib, type, &tempRef);
}

OSErr MADMusicIdentifyFSp(MADLibrary *lib, char *type, FSSpec *theSpec)
{
	FSSpec	saved;
	OSErr	err;
	
	HGetVol(NULL, &saved.vRefNum, &saved.parID);
	HSetVol(NULL, theSpec->vRefNum, theSpec->parID);
	MYP2CStr(theSpec->name);
	
	err =  PPIdentifyFile(lib, type, (Ptr) theSpec->name);
	
	MYC2PStr((Ptr) theSpec->name);
	HSetVol(NULL, saved.vRefNum, saved.parID);
	
	return err;
}

OSErr	MADMusicIdentifyPString(MADLibrary *lib, char *type, Str255 fName)
{
	OSErr	err;
	
	MYP2CStr(fName);
	err = PPIdentifyFile(lib, type, (Ptr) fName);
	MYC2PStr((Ptr) fName);
	
	return err;
}
#endif

OSErr MADMusicIdentifyCString(MADLibrary *lib, char *type, Ptr fName)
{
	if (lib == NULL || type == NULL || fName == NULL) {
		return MADParametersErr;
	}
	return PPIdentifyFile(lib, type, fName);
}

OSErr MADLoadMusicFileCString(MADLibrary *lib, MADMusic **music, char *plugType, Ptr fName)
{
	OSErr iErr = noErr;
	
	if (lib == NULL || music == NULL || plugType == NULL || fName == NULL)
		return MADParametersErr;

	if (!strcmp("MADK", plugType))
		iErr = MADLoadMADFileCString(music, fName);
	else
		iErr = PPImportFile(lib, plugType, fName, music);
	
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
	{
		return MADParametersErr;
	}
	
	if (MDriver->curMusic == NULL)
	{
		return MADDriverHasNoMusic;
	}
	
	if (MDriver->curMusic->header == NULL)
	{
		return MADDriverHasNoMusic;
	}
	
	if (maxV > curV) {
		MDriver->musicEnd = false;
	}
	
	MADGetMusicStatus(MDriver, &fullTime, &curTime);
	
	curV -= minV;
	maxV -= minV;
	
	if (maxV) {
		dstTime		= (curV * fullTime)/maxV;
	}
	else
	dstTime			= 0;
	timeResult		= 0;
	time			= 0;
	speed			= MDriver->curMusic->header->speed;
	finespeed		= MDriver->curMusic->header->tempo;
	
	for (i = 0; i < MDriver->curMusic->header->numPointers; i++)
	{
		for (x = 0; x < MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]->header.size; x++)
		{
			time ++;
			
			if (dstTime <= timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed)))
			{
				MDriver->PL = i;
				MDriver->PartitionReader = x;
				MDriver->Pat = MDriver->curMusic->header->oPointers[i];
				
				MADCheckSpeed(MDriver->curMusic, MDriver);
				
				MADPurgeTrack(MDriver);
				
				return noErr;
			}
			
			for (y = 0; y <  MDriver->curMusic->header->numChn; y++)
			{
				aCmd = GetMADCommand(x, y, MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]);
								
				/** SpeedE **/
				
				if (aCmd->cmd == speedE)
				{
					/** Compute time for this interval **/
					
					timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
					time = 0;
					
					/************************************/
					
					if (aCmd->arg < 32)
					{
						if (aCmd->arg != 0) speed = aCmd->arg;
					}
					else
					{
						if (aCmd->arg != 0) finespeed = aCmd->arg;
					}
				}
				
				/** SkipE **/
				
				if (aCmd->cmd == skipE)
				{
					for (; x < MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]->header.size; x++)
					{
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
	
	if (MDriver == NULL)
	{
		*curTime = 0;
		*fullTime = 1;
		
		return MADParametersErr;
	}
	
	if (MDriver->curMusic == NULL)
	{
		*curTime = 0;
		*fullTime = 1;
		
		return MADDriverHasNoMusic;
	}
	
	if (MDriver->curMusic->header == NULL)
	{
		*curTime = 0;
		*fullTime = 1;
		
		return MADDriverHasNoMusic;
	}
	
	timeResult	= 0;
	time				= 0;
	speed				= MDriver->curMusic->header->speed;
	finespeed			= MDriver->curMusic->header->tempo;
	
	*curTime		= -1;
	
	for (i = 0; i < MDriver->curMusic->header->numPointers; i++)
	{
		for (x = 0; x < MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]->header.size; x++)
		{
			time ++;
			
			if (i == MDriver->PL	&&
			   x == MDriver->PartitionReader)
			{
				*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
			}
			
			for (y = 0; y <  MDriver->curMusic->header->numChn; y++)
			{
				aCmd = GetMADCommand(x, y, MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]);
				
				/** SpeedE **/
				
				if (aCmd->cmd == speedE)
				{
					/** Compute time for this interval **/
					
					timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
					time = 0;
					
					/************************************/
					
					if (aCmd->arg < 32)
					{
						if (aCmd->arg != 0) speed = aCmd->arg;
					}
					else
					{
						if (aCmd->arg != 0) finespeed = aCmd->arg;
					}
				}
				
				/** SkipE **/
				
				if (aCmd->cmd == skipE)
				{
					for (; x < MDriver->curMusic->partition[MDriver->curMusic->header->oPointers[i]]->header.size; x++)
					{
						if (i == MDriver->PL	&&
						   x == MDriver->PartitionReader)
						{
							*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
						}
					}
				}
			}
		}
	}
	
	timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
	
	*fullTime = timeResult;
	
	if (*curTime == -1) *curTime = *fullTime;
	
	return noErr;
}

OSErr MADReadMAD(MADMusic **music, UNFILE srcFile, short InPutType, Handle MADRsrc, Ptr MADPtr)
{
	short 					i, x;
	long 					inOutCount, OffSetToSample = 0;
	OSErr					theErr = noErr;
	PatHeader				tempPatHeader;
	PatData*				tempPat;
	MADMusic 				*MDriver;
	
	*music = NULL;
	
	MDriver = (MADMusic*) NewPtrClear(sizeof(MADMusic));
	if (!MDriver) return MADNeedMemory;
	
	MDriver->musicUnderModification = false;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	MDriver->header = (MADSpec*) NewPtrClear(inOutCount);
	if (MDriver->header == NULL)
	{
		DisposePtr((Ptr) MDriver);
		return MADNeedMemory;
	}
	
	switch (InPutType)
	{
#ifdef _MAC_H
		case MADRsrcType:
			OffSetToSample = 0;
			ReadPartialResource(MADRsrc, 0, MDriver->header, inOutCount);
			OffSetToSample += inOutCount;
			break;
#endif
			
		case MADFileType:
			if (iRead(inOutCount, (Ptr) MDriver->header, srcFile)) theErr = MADIncompatibleFile;
			break;
			
		case MADPtrType:
			OffSetToSample = 0;
			BlockMoveData(MADPtr, MDriver->header, inOutCount);
			OffSetToSample += inOutCount;
			break;
			
		default:
			DisposePtr((Ptr) MDriver->header);
			DisposePtr((Ptr) MDriver);
			return MADParametersErr;
			break;
	}
	
	MOT32(&MDriver->header->MAD);
	MOT16(&MDriver->header->speed);
	MOT16(&MDriver->header->tempo);
	MOT32(&MDriver->header->EPitch);
	MOT32(&MDriver->header->ESpeed);
	
	if (MDriver->header->MAD != 'MADK' || MDriver->header->numInstru > MAXINSTRU)
	{
		DisposePtr((Ptr) MDriver->header);
		DisposePtr((Ptr) MDriver);
		return MADIncompatibleFile;
	}
	
	if (MDriver->header->MultiChanNo == 0) MDriver->header->MultiChanNo = 48;
	
	
	/**** PARTITION ****/
	for (i = MDriver->header->numPat; i < MAXPATTERN; i++) MDriver->partition[i] = NULL;
	
	for (i = 0; i < MDriver->header->numPat; i++)
	{
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		switch (InPutType)
		{
#ifdef _MAC_H
			case MADRsrcType:
				ReadPartialResource(MADRsrc, OffSetToSample, &tempPatHeader, inOutCount);
				//	OffSetToSample += inOutCount; ON LE RELIT APRES !!!!!!
				break;
#endif
				
			case MADFileType:
				if (iRead(inOutCount, (Ptr) &tempPatHeader, srcFile)) theErr = MADIncompatibleFile;
				iSeekCur(-(long)sizeof(PatHeader), srcFile);
				break;
				
			case MADPtrType:
				BlockMoveData(MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
				break;
		}
		
		MOT32(&tempPatHeader.size);
		MOT32(&tempPatHeader.compMode);
		MOT32(&tempPatHeader.patBytes);
		MOT32(&tempPatHeader.unused2);
		
		/*************************************************/
		/** Lecture du header + contenu de la partition **/
		/*************************************************/
		
		switch (tempPatHeader.compMode)
		{
			case 'MAD1':
				inOutCount = sizeof(PatHeader) + tempPatHeader.patBytes;
				break;
				
			default:
			case 'NONE':
				inOutCount = sizeof(PatHeader) + MDriver->header->numChn * tempPatHeader.size * sizeof(Cmd);
				break;
		}
		
		MDriver->partition[i] = (PatData*) NewPtr(inOutCount);
		if (MDriver->partition[i] == NULL)
		{
			for (x = 0; x < i; x++)
			{
				if (MDriver->partition[x] != NULL)	DisposePtr((Ptr) MDriver->partition[x]);
			}
			DisposePtr((Ptr) MDriver->header);
			DisposePtr((Ptr) MDriver);
			
			return MADNeedMemory;
		}
		switch (InPutType)
		{
#ifdef _MAC_H
			case MADRsrcType:
				ReadPartialResource(MADRsrc, OffSetToSample, MDriver->partition[i], inOutCount);
				OffSetToSample += inOutCount;
				break;
#endif
				
			case MADFileType:
				if (iRead(inOutCount, (Ptr) MDriver->partition[i], srcFile)) theErr = MADIncompatibleFile;
				break;
				
			case MADPtrType:
				BlockMoveData(MADPtr + OffSetToSample, MDriver->partition[i], inOutCount);
				OffSetToSample += inOutCount;
				break;
		}
		
		MOT32(&MDriver->partition[i]->header.size);
		MOT32(&MDriver->partition[i]->header.compMode);
		MOT32(&MDriver->partition[i]->header.patBytes);
		MOT32(&MDriver->partition[i]->header.unused2);
		
		if (MDriver->partition[i]->header.compMode == 'MAD1')
		{
			tempPat = DecompressPartitionMAD1(MDriver, MDriver->partition[i]);
			
			if (tempPat == NULL)
			{
				for (x = 0; x < i; x++)
				{
					if (MDriver->partition[x] != NULL)	DisposePtr((Ptr) MDriver->partition[x]);
				}
				DisposePtr((Ptr) MDriver->header);
				DisposePtr((Ptr) MDriver);
				
				return MADNeedMemory;
			}
			
			DisposePtr((Ptr) MDriver->partition[i]);
			MDriver->partition[i] = tempPat;
		}
	}
	
	/**** INSTRUMENTS ****/
	MDriver->fid = (InstrData*) NewPtrClear(sizeof(InstrData) * (long) MAXINSTRU);
	if (!MDriver->fid)
	{
		for (x = 0; x < MDriver->header->numPat; x++)
		{
			if (MDriver->partition[x] != NULL)	DisposePtr((Ptr) MDriver->partition[x]);
		}
		DisposePtr((Ptr) MDriver->header);
		DisposePtr((Ptr) MDriver);
		
		return MADNeedMemory;
	}
	
	inOutCount = sizeof(InstrData) * (long) MDriver->header->numInstru;
	switch (InPutType)
	{
#ifdef _MAC_H
		case MADRsrcType:
			ReadPartialResource(MADRsrc, OffSetToSample, MDriver->fid, inOutCount);
			OffSetToSample += inOutCount;
			break;
#endif
			
		case MADFileType:
			if (iRead(inOutCount, (Ptr) MDriver->fid, srcFile)) theErr = MADIncompatibleFile;
			break;
			
		case MADPtrType:
			BlockMoveData(MADPtr + OffSetToSample, MDriver->fid, inOutCount);
			OffSetToSample += inOutCount;
			break;
	}
	
	for (i = MDriver->header->numInstru-1; i >= 0 ; i--)
	{
		InstrData	*curIns = &MDriver->fid[i];
		
		MOT16(&curIns->numSamples);
		MOT16(&curIns->firstSample);
		MOT16(&curIns->volFade);
		
		for (x = 0; x < 12; x++)
		{
			MOT16(&curIns->volEnv[x].pos);
			MOT16(&curIns->volEnv[x].val);

			MOT16(&curIns->pannEnv[x].pos);
			MOT16(&curIns->pannEnv[x].val);

			MOT16(&curIns->pitchEnv[x].pos);
			MOT16(&curIns->pitchEnv[x].val);
		}
		
#if 0
		for (x = 0; x < 12; x++)
		{
			PPBE16(&curIns->pannEnv[x].pos);
			PPBE16(&curIns->pannEnv[x].val);
		}
#endif
		
		if (i != curIns->no)
		{
			MDriver->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	MDriver->header->numInstru = MAXINSTRU;
	
	/**** SAMPLES ****/
	MDriver->sample = (sData**) NewPtrClear(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE);
	if (!MDriver->sample)
	{
		for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(MDriver, x);
		
		for (x = 0; x < MDriver->header->numPat; x++)
		{
			if (MDriver->partition[x] != NULL)	DisposePtr((Ptr) MDriver->partition[x]);
		}
		DisposePtr((Ptr) MDriver->header);
		DisposePtr((Ptr) MDriver);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU ; i++)
	{
		for (x = 0; x < MDriver->fid[i].numSamples ; x++)
		{
			sData	 *curData;
			
			// ** Read Sample header **
			
			curData = MDriver->sample[i*MAXSAMPLE + x] = (sData*) NewPtr(sizeof(sData));
			if (curData == NULL)
			{
				for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++)
				{
					if (MDriver->partition[x] != NULL)	DisposePtr((Ptr) MDriver->partition[x]);
				}
				DisposePtr((Ptr) MDriver->header);
				DisposePtr((Ptr) MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData);
			
			switch (InPutType)
			{
#ifdef _MAC_H
				case MADRsrcType:
					ReadPartialResource(MADRsrc, OffSetToSample, curData, inOutCount);
					OffSetToSample += inOutCount;
					break;
#endif
					
				case MADFileType:
					if (iRead(inOutCount, (Ptr) curData, srcFile)) theErr = MADIncompatibleFile;
					break;
					
				case MADPtrType:
					BlockMoveData(MADPtr + OffSetToSample, curData, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
			MOT32(&curData->size);
			MOT32(&curData->loopBeg);
			MOT32(&curData->loopSize);
			MOT16(&curData->c2spd);
			
			// ** Read Sample DATA
			
			curData->data = NewPtr(curData->size);
			if (curData->data == NULL)
			{
				for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						DisposePtr((Ptr) MDriver->partition[x]);
				}
				DisposePtr((Ptr) MDriver->header);
				DisposePtr((Ptr) MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = curData->size;
			switch (InPutType) {
#ifdef _MAC_H
				case MADRsrcType:
					ReadPartialResource(MADRsrc, OffSetToSample, curData->data, inOutCount);
					OffSetToSample += inOutCount;
					break;
#endif
					
				case MADFileType:
					if (iRead(inOutCount, (Ptr) curData->data, srcFile)) theErr = MADIncompatibleFile;
					break;
					
				case MADPtrType:
					BlockMoveData(MADPtr + OffSetToSample, curData->data, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
#ifdef __LITTLE_ENDIAN__
			if (curData->amp == 16) {
				long 	ll;
				short	*shortPtr = (short*) curData->data;
				
				for (ll = 0; ll < curData->size/2; ll++) MOT16(&shortPtr[ll]);
			}
#endif
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) MDriver->fid[i].firstSample = i * MAXSAMPLE;
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		MDriver->sets = (FXSets*) NewPtrClear(MAXTRACK * sizeof(FXSets));
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++)	// Global Effects
		{
			if (MDriver->header->globalEffect[i])
			{
				int y = 0;
				inOutCount = sizeof(FXSets);
				switch (InPutType)
				{
#ifdef _MAC_H
					case MADRsrcType:
						ReadPartialResource(MADRsrc, OffSetToSample, &MDriver->sets[alpha], inOutCount);
						OffSetToSample += inOutCount;
						break;
#endif
						
					case MADFileType:
						if (iRead(inOutCount, (Ptr) &MDriver->sets[alpha], srcFile)) theErr = MADIncompatibleFile;
						break;
						
					case MADPtrType:
						BlockMoveData(MADPtr + OffSetToSample, &MDriver->sets[alpha], inOutCount);
						OffSetToSample += inOutCount;
						break;
				}
				MOT16(&MDriver->sets[alpha].id);
				MOT16(&MDriver->sets[alpha].noArg);
				MOT16(&MDriver->sets[alpha].track);
				MOT32(&MDriver->sets[alpha].FXID);
				for (y = 0; y < 100; y++) {
					MOT32(&MDriver->sets[alpha].values[y]);
				}
				alpha++;
			}
		}
		
		for (i = 0; i < MDriver->header->numChn ; i++)	// Channel Effects
		{
			for (x = 0; x < 4; x++)
			{
				if (MDriver->header->chanEffect[i][x])
				{
					int y = 0;
					inOutCount = sizeof(FXSets);
					switch (InPutType)
					{
#ifdef _MAC_H
						case MADRsrcType:
							ReadPartialResource(MADRsrc, OffSetToSample, &MDriver->sets[alpha], inOutCount);
							OffSetToSample += inOutCount;
							break;
#endif
							
						case MADFileType:
							if (iRead(inOutCount, (Ptr) &MDriver->sets[alpha], srcFile)) theErr = MADIncompatibleFile;
							break;
							
						case MADPtrType:
							BlockMoveData(MADPtr + OffSetToSample, &MDriver->sets[alpha], inOutCount);
							OffSetToSample += inOutCount;
							break;
					}
					MOT16(&MDriver->sets[alpha].id);
					MOT16(&MDriver->sets[alpha].noArg);
					MOT16(&MDriver->sets[alpha].track);
					MOT32(&MDriver->sets[alpha].FXID);
					for (y = 0; y < 100; y++) {
						MOT32(&MDriver->sets[alpha].values[y]);
					}
					alpha++;
				}
			}
		}
		
	}
	// *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	
	MDriver->header->MAD = 'MADK';
	
	//MADCleanDriver(MDriver);
	
	*music = MDriver;
	
	return(noErr);
}

OSErr MADKillSample(MADMusic *MDriver, short ins, short sample)
{
	short	i;
	Boolean	IsReading;
	
	if (MDriver->header == NULL) return MADDriverHasNoMusic;
	
	IsReading = MDriver->musicUnderModification;
	MDriver->musicUnderModification = true;
	
	if (MDriver->sample[ins * MAXSAMPLE + sample] == NULL) return MADParametersErr;
	if (MDriver->fid[ins].numSamples <= sample) return MADParametersErr;
	
	if (MDriver->sample[ins * MAXSAMPLE + sample]->data) DisposePtr(MDriver->sample[ins * MAXSAMPLE + sample]->data);
	DisposePtr((Ptr) MDriver->sample[ins * MAXSAMPLE + sample]);
	MDriver->sample[ins * MAXSAMPLE + sample] = NULL;
	
	MDriver->fid[ins].numSamples--;
	
	for (i = sample ; i < MDriver->fid[ins].numSamples; i++)
	{
		MDriver->sample[ins * MAXSAMPLE + i] = MDriver->sample[ins * MAXSAMPLE + i + 1];
	}
	
	for (i = 0; i < NUMBER_NOTES; i++)
	{
		if (MDriver->fid[ins].what[i] >= sample)
		{
			if (MDriver->fid[ins].what[i] > 0)
				MDriver->fid[ins].what[i]--;
		}
	}
	
	MDriver->musicUnderModification = IsReading;
	
	return noErr;
}

sData	* MADCreateSample(MADMusic *MDriver, short ins, short sample)
{
	sData	*curData;
	
	curData = (sData*) NewPtrClear(sizeof(sData));
	if (curData)
	{
		curData->size		= 0;
		curData->loopBeg	= 0;
		curData->loopSize	= 0;
		curData->vol		= MAX_VOLUME;
		curData->c2spd		= NOFINETUNE;
		curData->loopType	= eClassicLoop;
		curData->amp		= 8;
		curData->relNote	= 0;
		// curData->name
		curData->data		= NULL;
		
		// Install it
		
		if (sample < MDriver->fid[ins].numSamples) MyDebugStr(__LINE__, __FILE__, "MADCreateSample");
		
		MDriver->sample[ins * MAXSAMPLE + sample] = curData;
		MDriver->fid[ins].numSamples++;
	}
	
	return curData;		
}

void MADPurgeTrack(MADDriverRec *intDriver)
{
	short	i;
	
	for (i=0; i<intDriver->DriverSettings.numChn; i++)
	{
	//	intDriver->chan[i].lastWord		= 0;
	//	intDriver->chan[i].curLevel		= 0;
		intDriver->chan[i].prevPtr		= 0;
		
		intDriver->chan[i].curPtr = intDriver->chan[i].maxPtr;
		intDriver->chan[i].samplePtr = NULL;
		intDriver->chan[i].lAC		= 0;
		intDriver->chan[i].loopBeg = 0;
		intDriver->chan[i].loopSize = 0;
		intDriver->chan[i].RemoverWorking = false;
		intDriver->chan[i].TICKREMOVESIZE = 1;
	//	intDriver->chan[i].TrackID = i;
		
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
	short	i;
	
	for (i=0; i<intDriver->DriverSettings.numChn; i++)	//
	{
		if (intDriver->chan[i].ins == instru && intDriver->chan[i].curPtr != intDriver->chan[i].maxPtr)
		{
			//	intDriver->chan[i].lastWord		= 0;
			//	intDriver->chan[i].curLevel		= 0;
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
}

OSErr MADCleanCurrentMusic(MADMusic *MDriver, MADDriverRec *intDriver)
{
	if (MDriver->header != NULL)
	{
		short i, x;
		
		if (intDriver)
		{
			if (intDriver->Pat > MDriver->header->numPat) MADReset(intDriver);
			if (intDriver->PartitionReader > MDriver->partition[intDriver->Pat]->header.size) MADReset(intDriver);
		}
		
		for (i = 0; i < MAXINSTRU; i++)
		{
			for (x = 0; x < 96; x++)
			{
				if (MDriver->fid[i].what[x])
				{
					if (MDriver->fid[i].what[x] >= MDriver->fid[i].numSamples)
					{
						MDriver->fid[i].what[x] = 0;
					}
				}
			}
			
			for (x = 0; x < MDriver->fid[i].numSamples; x++)
			{
				sData	*curData = MDriver->sample[i * MAXSAMPLE + x];
				
				if (curData == NULL)
				{
					curData = MDriver->sample[i * MAXSAMPLE + x] = MADCreateSample(MDriver, i, x);
				}
				
				if (curData->data == NULL)
				{
					curData->data = NewPtr(0);
					curData->size = 0;
				}
				
				if (curData->size <= 0)
				{
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopSize < 0) curData->loopSize = 0;
				
				if (curData->loopBeg > curData->size)
				{
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopBeg + curData->loopSize > curData->size)
				{
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
			}
		}
	}
	
	return noErr;
}

OSErr MADResetInstrument(InstrData		*curIns)
{
	if (curIns == NULL) {
		return MADParametersErr;
	}
	
	//for (i = 0; i < 32; i++) curIns->name[i]	= 0;
	memset(curIns->name, 0, sizeof(curIns->name));
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
	memset(curIns->what, 0, sizeof(curIns->what));
	memset(curIns->volEnv, 0, sizeof(curIns->volEnv));
	memset(curIns->pannEnv, 0, sizeof(curIns->pannEnv));
	memset(curIns->pitchEnv, 0, sizeof(curIns->pitchEnv));
	
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

OSErr MADKillInstrument(MADMusic *music, short ins)
{
	short				i;
	InstrData		*curIns;
	Boolean			IsReading;

	if (music == NULL) return MADParametersErr;
	
	curIns = &music->fid[ins];
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = 0; i < curIns->numSamples; i++)
	{
		if (music->sample[ins * MAXSAMPLE + i] != NULL)
		{
			if (music->sample[ins * MAXSAMPLE + i]->data != NULL)
			{
				DisposePtr((Ptr) music->sample[ins * MAXSAMPLE + i]->data);
				music->sample[ins * MAXSAMPLE + i]->data = NULL;
			}
			DisposePtr((Ptr) music->sample[ins * MAXSAMPLE + i]);
			music->sample[ins * MAXSAMPLE + i] = NULL;
		}
	}
	
	MADResetInstrument(curIns);
	
	music->musicUnderModification = IsReading;
	
	return noErr;
}

OSErr MADKillCmd(Cmd	*aCmd)
{
	if (aCmd == NULL) {
		return MADParametersErr;
	}
	aCmd->cmd 	= 0;
	aCmd->note 	= 0xFF;
	aCmd->arg 	= 0;
	aCmd->ins 	= 0;
	aCmd->vol 	= 0xFF;

	return noErr;
}

void MADCheckSpeed(MADMusic *MDriver, MADDriverRec *intDriver)
{
	short			i, x, y;
	short			curPartitionReader;
	Cmd				*aCmd;
	Boolean			CmdSpeed = false, FineFound = false, ArgFound[MAXTRACK], NoteFound[MAXTRACK], InsFound[MAXTRACK];

	if (!MDriver) return;
	if (MDriver->header == NULL) return;
	if (!intDriver) return;
	
	for (i = 0; i < MAXTRACK; i++)
	{
		ArgFound[i] = false;
		NoteFound[i] = false;
		InsFound[i] = false;
	}
	
	for (i = intDriver->PL; i >= 0; i--)
	{
		if (i == intDriver->PL)
		{
			curPartitionReader = intDriver->PartitionReader;
			if (curPartitionReader >= MDriver->partition[MDriver->header->oPointers[i]]->header.size) curPartitionReader--;
		}
		else
		{
			curPartitionReader = MDriver->partition[MDriver->header->oPointers[i]]->header.size-1;
		}

		for (x = curPartitionReader; x >= 0; x--)
		{
			for (y = MDriver->header->numChn-1; y >= 0 ; y--)
			{
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
					if (!ArgFound[y]) ch->oldArg[aCmd->cmd] = aCmd->arg;
					ArgFound[y] = true;
				}
				
				if (aCmd->ins != 0)
				{
					if (!InsFound[y]) ch->insOld = aCmd->ins;
					InsFound[y] = true;
				}
				
				if (aCmd->note != 0xFF)
				{
					if (!NoteFound[y]) ch->noteOld = aCmd->note;
					NoteFound[y] = true;
				}
			}
		}
	}
		

	if (!CmdSpeed) 	intDriver->speed 		= MDriver->header->speed;
	if (!FineFound) intDriver->finespeed 	= MDriver->header->tempo;
}

void MADCheckSpeedPattern(MADMusic *MDriver, MADDriverRec *intDriver)
{
	short			x, y;
	short			curPartitionReader;
	Cmd				*aCmd;
	Boolean			CmdSpeed = false, FineFound = false;

	if (!MDriver) return;
	if (MDriver->header == NULL) return;
	if (!intDriver) return;
	

	curPartitionReader = intDriver->PartitionReader;
	if (curPartitionReader >= MDriver->partition[intDriver->Pat]->header.size) curPartitionReader--;
	
	for (x = curPartitionReader; x >= 0; x--)
	{
		for (y = MDriver->header->numChn-1; y >= 0 ; y--)
		{
			//Channel	*ch = &intDriver->chan[y];
			
			aCmd = GetMADCommand(x, y, MDriver->partition[intDriver->Pat]);
			
			if (aCmd)
			{
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
			}
		}
	}

	if (!CmdSpeed) 	intDriver->speed 		= MDriver->header->speed;
	if (!FineFound) intDriver->finespeed 	= MDriver->header->tempo;
}

OSErr MADPlayMusic(MADDriverRec *MDriver)
{
	if (!MDriver) return MADParametersErr;
	if (!MDriver->curMusic) return MADDriverHasNoMusic;
	
	MDriver->Reading = true;
	
	return noErr;
}

Boolean MADIsPlayingMusic(MADDriverRec *driver)
{
	if (driver == NULL) {
		return false;
	}
	return driver->Reading;
}

OSErr MADStopMusic(MADDriverRec *MDriver)
{
	if (!MDriver) return MADParametersErr;
	if (!MDriver->curMusic) return MADDriverHasNoMusic;
	
	MDriver->Reading = false;
#ifdef _MIDIHARDWARE_
	if (MDriver->SendMIDIClockData) SendMIDIClock(MDriver, 0xFC);
#endif
	
	return noErr;
}

OSErr MADStartDriver(MADDriverRec *MDriver)
{
	if (MDriver == NULL) {
		return MADParametersErr;
	}
	MDriver->MADPlay = true;
	MDriver->Reading = false;
	MDriver->musicEnd = false;
	
	MADCleanDriver(MDriver);
	
	MADCheckSpeed(MDriver->curMusic, MDriver);
	
	switch (MDriver->DriverSettings.driverMode)
	{
#ifdef _MAC_H
			/*	case AWACSoundDriver:
			 if ((*(unsigned char *)AMICCodecStatus2 & kHeadphonesInserted))
			 {
			 curR1 = 0;
			 curR1 |= kSpeakerMuteBit;
			 WRITE_AWACS_REG_1;
			 }
			 
			 
			 CLEAR_SND_OUT_BUF_INTS;
			 CLEAR_UNDERRUN_INT;
			 ENABLE_BUF_AND_UNDERRUN_INTS;
			 SND_OUT_DMA_ENABLE;
			 break;
			 
			 case ASCSoundDriver:*/
		case MIDISoundDriver:
			PlayChannel(MDriver);
			break;
			
		case SoundManagerDriver:
			PlayChannel(MDriver);
			break;
			
		case ASIOSoundManager:
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
	
	return(noErr);
}

OSErr MADStopDriver(MADDriverRec *MDriver)
{
	if (!MDriver) {
		return MADParametersErr;
	}
	
	MDriver->MADPlay = false;
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;

	MDriver->levelL = 0;
	MDriver->levelR = 0;

	switch (MDriver->DriverSettings.driverMode)
	{
#ifdef _MAC_H
			/*	case AWACSoundDriver:
			 DISABLE_BUF_AND_UNDERRUN_INTS;
			 SND_OUT_DMA_DISABLE;
			 CLEAR_UNDERRUN_INT;
			 break;
			 
			 case ASCSoundDriver:*/
		case MIDISoundDriver:
			AllNoteOff(MDriver);
			StopChannel(MDriver);
			break;
			
		case SoundManagerDriver:
			StopChannel(MDriver);
			break;
			
		case ASIOSoundManager:
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
	}
	
	MADCleanDriver(MDriver);
	
#ifdef _MIDIHARDWARE_
	if (MDriver->SendMIDIClockData) SendMIDIClock(MDriver, 0xFC);
#endif
	
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
	
	if (MDriver->curMusic != NULL)
	{
		MDriver->Pat = MDriver->curMusic->header->oPointers[MDriver->PL];
		MDriver->speed = MDriver->curMusic->header->speed;
		MDriver->finespeed = MDriver->curMusic->header->tempo;
	}
	
	return(noErr);
}

short MADGetNextReader(MADMusic *music, MADDriverRec *intDriver, short cur, short *pat)
{
	if (music == NULL) return 0;
	if (intDriver == NULL) return 0;
	if (music->header == NULL) return 0;
	
	cur++;
	if (cur >= music->partition[intDriver->Pat]->header.size)
	{
		cur = 0;
		
		if (intDriver->JumpToNextPattern)
		{
			*pat = music->header->oPointers[intDriver->PL + 1];
			
			if (intDriver->speed == 1 && intDriver->PL + 1 >= music->header->numPointers)
			{
				*pat = music->header->oPointers[0];
			}
		}
	}
	
	return cur;
}

OSErr MADDisposeMusic(MADMusic **music, MADDriverRec *MDriver)
{
	short		x;
	
	if (!music) return noErr;
	if (!*music) return noErr;
	
//	if ((*music)->currentDriver != NULL)
	{
		(*music)->musicUnderModification = true;
	//	MADPurgeTrack((*music)->currentDriver);
	//	MADCleanDriver((*music)->currentDriver);
		
	//	MADDetachMusicFromDriver(*music);
	}
	
	if (MDriver)
	{
		if (MDriver->curMusic == *music) MDriver->curMusic = NULL;
	}
	
	if ((*music)->header != NULL)
	{
		for (x = 0; x < (*music)->header->numPat ; x++)
		{
			if ((*music)->partition[x] != NULL) DisposePtr((Ptr)  (*music)->partition[x]);
			(*music)->partition[x] = NULL;
		}
		
		for (x = (*music)->header->numPat; x < MAXPATTERN ; x++)
		{
			if ((*music)->partition[x] != NULL) DebugStr("\pMADDispose (*music) !");
		}
		
		for (x = 0; x < MAXINSTRU ; x++)
		{
			MADKillInstrument(*music, x);
		}
		
		DisposePtr((Ptr) (*music)->header);
		
		if ((*music)->fid == NULL) DebugStr("\pMADDispose (*fid) !");
		DisposePtr((Ptr) (*music)->fid);
		
		if ((*music)->sample == NULL) DebugStr("\pMADDispose (*sample) !");
		DisposePtr((Ptr) (*music)->sample);
		
		if ((*music)->sets == NULL) DebugStr("\pMADDispose (*sets) !");
		DisposePtr((Ptr) (*music)->sets);
	}
	
	
	DisposePtr((Ptr) *music);
	*music = NULL;
	
	return noErr;
}

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

Boolean	MADIsPressed(unsigned char* km2, unsigned short k)
{
	return((Boolean) ((km2[k>>3] >> (k & 7)) & 1) );
}

#ifdef _MAC_H
OSErr MADPlaySoundDataSYNC(MADDriverRec *MDriver, Ptr soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, unsigned long rate, Boolean stereo)
{
	OSErr		iErr;
	Boolean		continueLoop;
	KeyMap		km;
	
	iErr = MADPlaySoundData(MDriver, soundPtr, size, channel, note, amplitude, loopBeg, loopSize, rate, stereo);
	if (iErr == noErr) {
		continueLoop = true;
		while (continueLoop) {
			GetKeys(km);
			
			if (MDriver->chan[channel].samplePtr == NULL)
				continueLoop = false;
			if (MADIsPressed((unsigned char*) km, 0x37) && MADIsPressed((unsigned char*) km, 0x2F))
				continueLoop = false;
			if (Button())
				continueLoop = false;
			
#if MAINPLAYERPRO
			DoGlobalNull();
			WaitNextEvent(everyEvent, CurrentEvent(), 1, NULL);
#endif
		}
		
		if (MDriver->chan[channel].samplePtr != NULL) {
			MDriver->chan[channel].curPtr		= MDriver->chan[channel].maxPtr;
			MDriver->chan[channel].samplePtr	= NULL;
			MDriver->chan[channel].lAC			= 0;
			MDriver->chan[channel].loopBeg		= 0;
			MDriver->chan[channel].loopSize		= 0;
		}
	}
	
	return iErr;
}
#endif

OSErr MADPlaySoundData(MADDriverRec *MDriver, Ptr soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, unsigned long rate, Boolean stereo)
{
	Channel *curVoice;
	
	if (channel < 0)
		return MADParametersErr;
	if (channel >= MDriver->DriverSettings.numChn)
		return MADParametersErr;
	
	if (MDriver->curMusic != NULL) {
		MDriver->curMusic->musicUnderModification = true;
	}
	
	curVoice = &MDriver->chan[channel];
	
	curVoice->prevPtr		= NULL;
	curVoice->samplePtr		= soundPtr;
	curVoice->stereo			= stereo;
	
	curVoice->maxPtr	= curVoice->curPtr = curVoice->begPtr = soundPtr;
	curVoice->maxPtr	+= size;
	curVoice->sizePtr	= size;
	curVoice->lAC		= 0;
	curVoice->pingpong	= false;
	curVoice->PanningE8	= false;
	curVoice->trig		= 0;
	curVoice->preOff	= 0xFFFFFFFF;
	curVoice->preVal	= 0;
	curVoice->spreVal	= 0;
	curVoice->preVal2	= *curVoice->curPtr;
	if (curVoice->amp == 8) curVoice->preVal2R	= *(curVoice->curPtr+1);
	else curVoice->preVal2R	= *(curVoice->curPtr+2);
	curVoice->spreVal2	= *(short*) curVoice->curPtr;
	curVoice->spreVal2R	= *(short*) (curVoice->curPtr+2);
	
	if (note == 0xFF)
		note = 48;
	curVoice->note		= note;
	curVoice->ins		= 0;
	curVoice->viboffset	= 0;
	curVoice->amp		= amplitude;
	curVoice->fineTune	= rate >> 16L;
	
	curVoice->period = GetOldPeriod(curVoice->note, curVoice->fineTune, MDriver);
	
	if (loopBeg > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	if (loopBeg + loopSize > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	curVoice->loopBeg	= loopBeg;
	curVoice->loopSize	= loopSize;
	
	if (loopSize > 0) {
		curVoice->maxPtr = (Ptr)((size_t) curVoice->begPtr + loopBeg + loopSize);
	}
	
	curVoice->pann	= 32;
	
	curVoice->vol			= 64;
	curVoice->volFade		= 32767;
	curVoice->nextvolFade	= 32767;
	curVoice->volEnv		= 64;
	curVoice->KeyOn			= true;
	
	if (MDriver->curMusic != NULL) {
		MDriver->curMusic->musicUnderModification = false;
	}
	
	return noErr;
}

void MADKeyOFF(MADDriverRec *MDriver, short track)
{
	if (track == -1) {
		short i;
		
		for (i = 0; i < MAXTRACK; i++) MDriver->chan[i].KeyOn = false;
	} else
		MDriver->chan[track].KeyOn = false;
}

#ifdef _MAC_H
OSErr MADPlaySndHandle(MADDriverRec *MDriver, Handle sound, long channel, long note)
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
	//Channel			*curVoice;
	OSType			Scomp;
	unsigned long	Ssize, Samp, SbaseFreq, Sc2spd, Sstart, Send;
	Ptr				rPtr;
	
	/* make the sound safe to use at interrupt time. */
	HLock(sound);
	
	soundPtr = *sound;
	
	/* determine what format sound we have. */
	soundFormat = *(short*)soundPtr;
	
	switch (soundFormat) {
		case 1:
			/* format 1 sound. */
			/* look inside the format 1 resource and deduce offsets. */
			numSynths = ((short*)soundPtr)[1];					/* get # synths. */
			numCmds = *(short*)(soundPtr+4+numSynths*6);		/* get # commands. */
			break;
			
		case 2:
			/* format 2 sound. */
			numSynths = 0; /* format 2 sounds have no synth's. */
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:
			/* jack says, what about 12? or 6? */
			HUnlock(sound);
			return MADUnknowErr;
			break;
	} 

	/* compute address of sound header. */
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) ((*sound) + offset);
	
	switch (header->encode) {
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			if (CmpHeader->numChannels > 2) {
				HUnlock(sound);
				return MADUnknowErr;
			}
			
			if (CmpHeader->numChannels == 2)
				stereo = true;
			else
				stereo = false;
			
			Sstart		= CmpHeader->loopStart;
			Send		= CmpHeader->loopEnd;
			Samp		= CmpHeader->sampleSize;
			Sc2spd		= CmpHeader->sampleRate;
			SbaseFreq	= CmpHeader->baseFrequency;
			Ssize		= CmpHeader->numFrames;
			rPtr		= (Ptr)CmpHeader->sampleArea;
			Scomp		= CmpHeader->compressionID;
			break;
			
		case extSH:
			ExtHeader	= (ExtSoundHeader*)header;
			if (ExtHeader->numChannels > 2) {
				HUnlock(sound);
				return MADUnknowErr;
			}
			
			if (ExtHeader->numChannels == 2)
				stereo = true;
			else
				stereo = false;
			
			Sstart		= ExtHeader->loopStart;
			Send		= ExtHeader->loopEnd;
			Samp		= ExtHeader->sampleSize;
			Sc2spd		= ExtHeader->sampleRate;
			SbaseFreq	= ExtHeader->baseFrequency;
			Ssize		= ExtHeader->numFrames;
			rPtr		= (Ptr)ExtHeader->sampleArea;
			Scomp		= 'NONE';
			break;
			
		default:
		case stdSH:
			Sstart		= header->loopStart;
			Send		= header->loopEnd;
			if (header->encode == 0x40)
				Samp = 0;
			else
				Samp = 8;
			Sc2spd		= header->sampleRate;
			SbaseFreq	= header->baseFrequency;
			Ssize		= header->length;
			rPtr		= (Ptr)header->sampleArea;
			Scomp		= 'NONE';
			break;
	}
	
	if (Samp == 16) {
		Ssize 	*= 2;
		Sstart 	*= 2;
		Send 	*= 2;
		
		if (stereo) {
			Ssize	*= 2;
			Sstart	*= 2;
			Send	*= 2;
		}
	} else {
		if (Samp == 8) {
			if (stereo) {
				Ssize	*= 2;
				Sstart	*= 2;
				Send	*= 2;
			}
			
			for (i = 0; i < Ssize; i++) rPtr[i] = rPtr[i] - 0x80;
			
			switch (header->encode) {
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
	
	if (note == 0xFF)
		note = 48;
	
	return MADPlaySoundData(MDriver, rPtr, Ssize, channel, note + (60 - SbaseFreq), Samp, Sstart, Send - Sstart, Sc2spd, stereo);
}

#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif

Cmd* GetMADCommand(short PosX, short TrackIdX, PatData* tempMusicPat)
{
	if (tempMusicPat == NULL) {
		return NULL;
	}
	
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= tempMusicPat->header.size)
		PosX = tempMusicPat->header.size -1;
	
	return &(tempMusicPat->Cmds[(tempMusicPat->header.size * TrackIdX) + PosX]);
}

void MADDisposeVolumeTable(MADDriverRec *intDriver)
{
	if (intDriver == NULL) {
		return;
	}
	if (intDriver->DriverSettings.outPutMode == DeluxeStereoOutPut) MADKillOverShoot(intDriver);
}

OSErr MADCreateVolumeTable(MADDriverRec *intDriver)
{
	long Tracks = 0;
	OSErr theErr;
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	Tracks = intDriver->DriverSettings.numChn;


	theErr = MADCreateMicroDelay(intDriver);
	if (theErr != noErr)
		return theErr;

	switch (intDriver->DriverSettings.outPutMode) {
		case DeluxeStereoOutPut:
			Tracks	= 1;
			MADCreateOverShoot(intDriver);
			break;
			
		case PolyPhonic:
			Tracks 	= 1;
			break;
	}
	
	return noErr;
}

void MADChangeTracks(MADDriverRec *MDriver, short newVal)
{
	Boolean	play = MDriver->MADPlay, reading = MDriver->Reading;
	
	MADStopDriver(MDriver);
	
	MDriver->DriverSettings.numChn = newVal;
	
	MADDisposeVolumeTable(MDriver);
	MADCreateVolumeTable(MDriver);
	
	MDriver->trackDiv = MDriver->DriverSettings.numChn;
	
	if (play)
		MADStartDriver(MDriver);
	if (reading)
		MADPlayMusic(MDriver);
}

void UpdateTracksNumber(MADDriverRec *MDriver)
{
	if (MDriver->curMusic != NULL) {
		if (MDriver->curMusic->header != NULL) {
			if (MDriver->curMusic->header->numChn != MDriver->DriverSettings.numChn) {
				MADChangeTracks(MDriver, MDriver->curMusic->header->numChn);
			}
		}
	}
}

OSErr MADCreateVibrato(MADDriverRec *MDriver)
{
	short i, vibrato_table[64] =  {
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
	
	

	for (i = 0; i < 64; i++) MDriver->vibrato_table[i] = vibrato_table[i];

	return noErr;

/*
double			base, pitch;
short			i, j, k;

	for (j = -8; j < 8; j++)
	{
		k = j < 0 ? j + 16 : j;
		base = AMIGA_CLOCKFREQ2 / 440.0 / 4.0 / pow(2.0, j/96.0);
	
		for (i = 0; i < NUMBER_NOTES + 4; i++)
		{
				pitch = base / pow(2.0, (i+1) /12.0);
				pitchTable[i][k] = floor(pitch + 0.5);
		}
	}
*/
}
/*
#ifdef _MAC_H

#define VIA 0x1D4

#if defined(powerc) || defined (__powerc) || defined(NO_ASM)
void VIAOn2()
{
	register	Ptr		VIAPtr = *(Ptr*) VIA, ASCBasePtr = *(Ptr*) ASCBase;

	BitSet((Ptr) VIAPtr, 0);

	if (ASCBasePtr[0x801] == 0)
	{
		ASCBasePtr[0x801] = 0x01;
		ASCBasePtr[0x806] = 0xFF;
	}
	
	BitSet(&ASCBasePtr[0x802], 6);
}

void SndOff()
{
	Ptr		VIAPtr = *(Ptr*) VIA;

	BitSet((Ptr) VIAPtr, 0);
	*((Byte*) SoundActive) = 0x05;
}

void SndOn()
{
	Ptr		VIAPtr = *(Ptr*) VIA;

	if (BitTst((Ptr) VIAPtr, 0))
	{
		BitClr((Ptr) VIAPtr, 0);
	 	*((Byte*) SoundActive) = 0x05;
	}
}
#else

void VIAOn2()
{
asm
{
		MOVEA.L VIA,A0
		BSET    #7,(A0)
		
		MOVE.L	ASCBase, A0
		TST.B	0x801(A0)
		BNE.S   @lcf_2
		MOVE.B	#01, 	0x801(A0)
		MOVE.B	#0xFF, 	0x806(A0)
@lcf_2	BSET	#1,		0x802(A0)
}
}

void SndOff()
{
asm
{
	MOVEA.L	VIA,A0
	BSET    #7,(A0)
	MOVE.B	#0x05, SoundActive
}
}

void SndOn()
{
asm
{		MOVEA.L VIA,A0
	 	BTST    #7,(A0)
	 	BEQ.S   @lcf_1
	 	BCLR    #7,(A0)
	 	MOVE.B	#0x05, SoundActive
@lcf_1	
}
}
#endif
#endif*/

enum
{
	ins 	= 1,
	note	= 2,
	cmd		= 4,
	argu	= 8,
	vol		= 16
};

PatData* DecompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData*				finalPtr;
	Byte 					*srcPtr;
	Cmd						*myCmd;
	short					maxCmd;
	Byte					set;
	
	finalPtr = (PatData*) NewPtr(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * sizeof(Cmd));
	if (finalPtr == NULL) return NULL;
	
	BlockMoveData(myPat, finalPtr, sizeof(PatHeader));
	
	srcPtr = (Byte*) myPat->Cmds;
	myCmd = (Cmd*) finalPtr->Cmds;
	maxCmd = finalPtr->header.size * MDriver->header->numChn;
	
	/*** Decompression Routine ***/
	
	while (maxCmd != 0)
	{
		maxCmd--;
		
		MADKillCmd(myCmd);
		
		set = *srcPtr++;
		
		if (set & ins)	myCmd->ins = *srcPtr++;
		if (set & note)	myCmd->note = *srcPtr++;
		if (set & cmd)	myCmd->cmd = *srcPtr++;
		if (set & argu)	myCmd->arg = *srcPtr++;
		if (set & vol)	myCmd->vol = *srcPtr++;
		
		myCmd++;
	}
	
	return finalPtr;
}

PatData* CompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData*				finalPtr;
	Byte 					*dstPtr, *setByte;
	Cmd						*myCmd;
	long					maxCmd;
	long					NewPtrSize;
	Byte					set;
	
	finalPtr = (PatData*) NewPtr(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * 6L);
	if (finalPtr == NULL) DebugStr("\pCompressPartitionMAD1");
	
	BlockMoveData(myPat, finalPtr, sizeof(PatHeader));
	
	dstPtr = (Byte*) finalPtr->Cmds;
	myCmd = (Cmd*) myPat->Cmds;
	maxCmd = myPat->header.size * MDriver->header->numChn;
	
	/*** Compression Routine ***/
	
	set 		= 0;
	NewPtrSize 	= 0;
	
	while (maxCmd != 0)
	{
		maxCmd--;
		
		// Set byte
		set			= 0;
		setByte 	= dstPtr;
		*dstPtr++ 	= 0x00;
		NewPtrSize++;
		////////
		
		if (myCmd->ins > 0)
		{
			set += ins;
			*dstPtr++ = myCmd->ins;
			NewPtrSize++;
		}
		
		if (myCmd->note != 0xFF)
		{
			set += note;
			*dstPtr++ = myCmd->note;
			NewPtrSize++;
		}

		if (myCmd->cmd > 0)
		{
			set += cmd;
			*dstPtr++ = myCmd->cmd;
			NewPtrSize++;
		}

		if (myCmd->arg > 0)
		{
			set += argu;
			*dstPtr++ = myCmd->arg;
			NewPtrSize++;
		}

		if (myCmd->vol != 0xFF)
		{
			set += vol;
			*dstPtr++ = myCmd->vol;
			NewPtrSize++;
		}
		// Set byte
		*setByte = set;
		////////
		
		myCmd++;
	}
	
	finalPtr->header.patBytes = NewPtrSize;
	
//	SetPtrSize((Ptr) finalPtr, (long)(NewPtrSize + sizeof(PatHeader)));
	
	return finalPtr;
}

/*
#ifdef _MAC_H
void DMAIntInstall(unsigned long refCon, Ptr handlerAddr, unsigned long vectorBit)
{
	Ptr		ExpandPtr = (Ptr) 0x02B6L;
	Ptr		dstPtr;
	
	dstPtr = *((Ptr*) ExpandPtr);
	dstPtr += 0x0210;
	dstPtr = *((Ptr*) dstPtr);
	
	*((long*)(0x04L + (long) dstPtr + vectorBit*8L)) = (long) handlerAddr;
	*((long*)(0x08L + (long) dstPtr + vectorBit*8L)) = refCon;
}

void DMAIntRemove(unsigned long vectorBit)
{
	Ptr		ExpandPtr = (Ptr) 0x02B6L;
	Ptr		dstPtr;
	
	dstPtr = *((Ptr*)ExpandPtr);
	dstPtr += 0x0210;
	dstPtr = *((Ptr*) dstPtr);
	
	*((long*)(0x04L + (long) dstPtr + vectorBit*8L)) = (long) 0x0060;
	*((long*)(0x08L + (long) dstPtr + vectorBit*8L)) = NULL;
}


extern MADDriverRec *MODPlayMADDriver;

void	InstallMODVBL(MADDriverRec *MDriver)
{
//	THz 			savedZone;

	MDriver->VBL.VBL.qLink		= NULL;		// VLBQueue
	MDriver->VBL.VBL.qType		= vType;
		
//	savedZone = GetZone();
//	SetZone(SystemZone());
	MDriver->VBL.VBL.vblAddr		= NewVBLProc(MODPlay);
//	SetZone(savedZone);
	
	MDriver->VBL.VBL.vblCount		= 0;
	MDriver->VBL.VBL.vblPhase		= 0;
	MDriver->VBL.VBLA5					= (long) MDriver;
	
	MODPlayMADDriver = MDriver;
	
	InstallPersistentVBL(MDriver, &MDriver->VBL.VBL);
}

void	RemoveMODVBL(MADDriverRec *MDriver)
{
	OSErr	myErr;
	
	myErr = VRemove((QElemPtr) &MDriver->VBL);
	
	if (MDriver->SysHeapPtr != NULL)
	{
		DisposeRoutineDescriptor(MDriver->VBL.VBL.vblAddr);
		
		free(MDriver->SysHeapPtr);
		MDriver->SysHeapPtr = NULL;
	}
}

void InstallPersistentVBL(MADDriverRec *MDriver, VBLTask *theVBLRec)
{
	#define kJMPInstr 	0x4EF9
	#define kJMPSize 		6
	
	OSErr	myErr;
	Ptr		tempPtr;

	MDriver->SysHeapPtr = NewPtrSys(kJMPSize);
	if (MDriver->SysHeapPtr == NULL) DebugStr("\pNewPtrSys Error");

	*(short*) MDriver->SysHeapPtr = kJMPInstr;
	tempPtr = (Ptr) MDriver->SysHeapPtr + sizeof(short);
	
	*(ProcPtr*)tempPtr = (ProcPtr) theVBLRec->vblAddr;
	
	#if defined(powerc) || defined (__powerc)
	#else
	theVBLRec->vblAddr = (VBLUPP) (MDriver->SysHeapPtr);
	#endif
	
	myErr = VInstall((QElemPtr) theVBLRec);
	if (myErr) DebugStr("\pError in VInstall");
}
#endif*/

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
