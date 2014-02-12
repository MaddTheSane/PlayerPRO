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
#include <PlayerPROCore/RDriverInt.h>
#include <CoreServices/CoreServices.h>
#include "APPL.h"

static OSErr LoadMADH(char *MADPtr, MADMusic *MadFile, MADDriverSettings *init)
{
	short 		i = 0;
	int			x = 0;
	long 		inOutCount, OffSetToSample;
	PatHeader	tempPatHeader;
	MADSpec		*MadHeader;
	
	/**** HEADER ****/
	MadFile->header = (MADSpec*)malloc(sizeof(MADSpec));
	if (MadFile->header == NULL)
		return MADNeedMemory;
	
	OffSetToSample = 0;
	memcpy(MadFile->header, MADPtr, sizeof(MADSpec));
	OffSetToSample += sizeof(MADSpec);
	
	MadHeader = MadFile->header;
	PPBE32(&MadHeader->MAD);
	
	if (MadHeader->MAD != 'MADK') {
		free(MadFile->header);
		return MADFileNotSupportedByThisPlug;
	}
	
	PPBE16(&MadHeader->speed);
	PPBE16(&MadHeader->tempo);
	PPBE32(&MadHeader->ESpeed);
	PPBE32(&MadHeader->EPitch);
	
	//////////////////
	
	MadFile->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!MadFile->fid) {
		free(MadFile->header);
		return MADNeedMemory;
	}
	
	MadFile->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!MadFile->sample) {
		free(MadFile->header);
		free(MadFile->fid);
		return MADNeedMemory;
	}
	
	
	/**** PARTITION ****/
	for (i = MadHeader->numPat; i < MAXPATTERN; i++)
		MadFile->partition[i] = NULL;
	
	for (i = 0; i < MadHeader->numPat; i++) {
		inOutCount = sizeof(PatHeader);
		memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
		
		PPBE32(&tempPatHeader.size);
		PPBE32(&tempPatHeader.compMode);
		PPBE32(&tempPatHeader.patBytes);
		PPBE32(&tempPatHeader.unused2);
		
		inOutCount = sizeof(PatHeader) + MadHeader->numChn * tempPatHeader.size * sizeof(Cmd);
		MadFile->partition[i] = (PatData*)malloc(inOutCount);
		if (MadFile->partition[i] == NULL) {
			for (x = 0; x < i; x++) {
				if (MadFile->partition[x] != NULL)
					free(MadFile->partition[x]);
			}
			free(MadFile->header);
			free(MadFile->fid);
			free(MadFile->sample);
			
			return MADNeedMemory;
		}
		
		memcpy(MadFile->partition[i], MADPtr + OffSetToSample, inOutCount);
		PPBE32(&MadFile->partition[i]->header.size);
		PPBE32(&MadFile->partition[i]->header.compMode);
		PPBE32(&MadFile->partition[i]->header.patBytes);
		PPBE32(&MadFile->partition[i]->header.unused2);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	
	inOutCount = sizeof(InstrData) * MadFile->header->numInstru;
	memcpy(MadFile->fid, MADPtr + OffSetToSample, inOutCount);
	OffSetToSample += inOutCount;
	
	for (i = MadFile->header->numInstru-1; i >= 0 ; i--) {
		InstrData *curIns = &MadFile->fid[i];
		PPBE16(&curIns->numSamples);
		PPBE16(&curIns->firstSample);
		PPBE16(&curIns->volFade);
		
		dispatch_apply(12, dispatch_get_global_queue(0, 0), ^(size_t x) {
			PPBE16(&curIns->volEnv[x].pos);
			PPBE16(&curIns->volEnv[x].val);
			
			PPBE16(&curIns->pitchEnv[x].pos);
			PPBE16(&curIns->pitchEnv[x].val);
			
			PPBE16(&curIns->pannEnv[x].pos);
			PPBE16(&curIns->pannEnv[x].val);
		});
		
		if (i != curIns->no) {
			MadFile->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	MadFile->header->numInstru = MAXINSTRU;
	
	for (i = 0; i < MAXINSTRU; i++) {
		short x;
		
		for (x = 0; x < MadFile->fid[i].numSamples ; x++) {
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = MadFile->sample[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MadFile, x);
				
				for (x = 0; x < MadFile->header->numPat; x++) {
					if (MadFile->partition[x] != NULL)
						free( MadFile->partition[x]);
				}
				free(MadFile->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			memcpy(curData, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			PPBE32(&curData->size);
			PPBE32(&curData->loopBeg);
			PPBE32(&curData->loopSize);
			PPBE16(&curData->c2spd);
			
			inOutCount = curData->size;
			
			curData->data = malloc(inOutCount);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MadFile, x);
				
				for (x = 0; x < MadFile->header->numPat; x++) {
					if (MadFile->partition[x] != NULL)
						free(MadFile->partition[x]);
				}
				free(MadFile->header);
				
				return MADNeedMemory;
			}
			
			memcpy(curData->data, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			if (curData->amp == 16) {
				short *shortPtr = (short*)curData->data;
				
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
					PPBE16(&shortPtr[ll]);
				});
			}
			
		}
	}
	
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MadFile->fid[i].firstSample = i * MAXSAMPLE;
	});
		
	/*********************/
	
	{
		short	alpha = 0, x;
		
		MadFile->sets = (FXSets*)calloc(MAXTRACK, sizeof(FXSets));
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MadFile->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				memcpy(&MadFile->sets[alpha], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				PPBE16(&MadFile->sets[alpha].id);
				PPBE16(&MadFile->sets[alpha].noArg);
				PPBE16(&MadFile->sets[alpha].track);
				PPBE32(&MadFile->sets[alpha].FXID);
				dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t y) {
					PPBE32(&MadFile->sets[alpha].values[y]);
				});
				alpha++;
			}
		}
		
		for (i = 0; i < MadFile->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MadFile->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					memcpy(&MadFile->sets[alpha], MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					PPBE16(&MadFile->sets[alpha].id);
					PPBE16(&MadFile->sets[alpha].noArg);
					PPBE16(&MadFile->sets[alpha].track);
					PPBE32(&MadFile->sets[alpha].FXID);
					dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t y) {
						PPBE32(&MadFile->sets[alpha].values[y]);
					});
					alpha++;
				}
			}
		}
	}
	
	return noErr;
}

static inline OSErr TESTMADk(MADSpec* MADPtr)
{
	OSType madType = MADPtr->MAD;
	PPBE32(&madType);
	if (madType == 'MADK')
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr INFOMADkApp(MADSpec* MADPtr, PPInfoRec *info)
{
	//short	i;
	
	strlcpy(info->internalFileName, MADPtr->name, sizeof(MADPtr->name));
	
	strcpy(info->formatDescription, "MADK Resource (APPL)");
	
	info->totalPatterns		= MADPtr->numPat;
	info->partitionLength	= MADPtr->numPointers;
	
	info->totalTracks		= MADPtr->numChn;
	info->signature			= MADPtr->MAD;
	PPBE32(&info->signature);
	
	info->totalInstruments	= MADPtr->numInstru;
	
	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

extern OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	//TODO: learn how to get and parse resource forks using POSIX commands or CoreFoundation, but not Carbon
	OSErr			myErr = noErr;
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	Handle			myRes;
	
	if (FSPathMakeRef((UInt8*)AlienFileName, &fileRef, NULL) != noErr)
		return MADReadingErr;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = LoadMADH(*myRes, MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADI2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADH2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADFG2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADFG2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		case MADPlugTest:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADk((MADSpec*)*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADI(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MADI Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADH(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MADH Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TestMADFGFile(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MAD-FG Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TestMADFGFile(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MAD-FG Application", sizeof(info->formatDescription));
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		case MADPlugInfo:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = INFOMADkApp((MADSpec*)*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADIInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADHInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADFGInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADFGInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
