//
//  PPAPPLHelpers.m
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

#import "PPAPPLHelpers.h"
#include <Carbon/Carbon.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>

MADErr TESTMADK(const void* MADPtr)
{
	if (memcmp("MADK", MADPtr, 4) == 0) {
		return MADNoErr;
	} else {
		return MADFileNotSupportedByThisPlug;
	}
}

MADErr LoadMADK(const char *MADPtr, size_t aSize, MADMusic *MadFile, MADDriverSettings *init)
{
	short 		i = 0;
	int			x = 0;
	size_t 		inOutCount = 0, OffSetToSample = 0;
	PatHeader	tempPatHeader;
	MADSpec		*MadHeader;
	
	/**** HEADER ****/
	MadFile->header = (MADSpec*)malloc(sizeof(MADSpec));
	if (MadFile->header == NULL)
		return MADNeedMemory;
	
	memcpy(MadFile->header, MADPtr, sizeof(MADSpec));
	OffSetToSample += sizeof(MADSpec);
	
	MadHeader = MadFile->header;
	MADBE32(&MadHeader->MAD);
	
	if (MadHeader->MAD != 'MADK') {
		free(MadFile->header);
		return MADFileNotSupportedByThisPlug;
	}
	
	MADBE16(&MadHeader->speed);
	MADBE16(&MadHeader->tempo);
	MADBE32(&MadHeader->ESpeed);
	MADBE32(&MadHeader->EPitch);
	
	//////////////////
	
	MadFile->fid = (InstrData*)calloc(MAXINSTRU, sizeof(InstrData));
	if (!MadFile->fid) {
		free(MadFile->header);
		return MADNeedMemory;
	}
	
	MadFile->sample = (sData**)calloc(MAXINSTRU * MAXSAMPLE, sizeof(sData*));
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
		
		MADBE32(&tempPatHeader.size);
		MADBE32(&tempPatHeader.compMode);
		MADBE32(&tempPatHeader.patBytes);
		MADBE32(&tempPatHeader.unused2);
		
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
		MADBE32(&MadFile->partition[i]->header.size);
		MADBE32(&MadFile->partition[i]->header.compMode);
		MADBE32(&MadFile->partition[i]->header.patBytes);
		MADBE32(&MadFile->partition[i]->header.unused2);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	
	inOutCount = sizeof(InstrData) * MadFile->header->numInstru;
	memcpy(MadFile->fid, MADPtr + OffSetToSample, inOutCount);
	OffSetToSample += inOutCount;
	
	for (i = MadFile->header->numInstru-1; i >= 0 ; i--) {
		InstrData *curIns = &MadFile->fid[i];
		MADBE16(&curIns->numSamples);
		MADBE16(&curIns->firstSample);
		MADBE16(&curIns->volFade);
		
		for (int x = 0; x < 12; x++) {
			MADBE16(&curIns->volEnv[x].pos);
			MADBE16(&curIns->volEnv[x].val);
			
			MADBE16(&curIns->pitchEnv[x].pos);
			MADBE16(&curIns->pitchEnv[x].val);
			
			MADBE16(&curIns->pannEnv[x].pos);
			MADBE16(&curIns->pannEnv[x].val);
		}
		
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
						free(MadFile->partition[x]);
				}
				free(MadFile->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			memcpy(curData, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			MADBE32(&curData->size);
			MADBE32(&curData->loopBeg);
			MADBE32(&curData->loopSize);
			MADBE16(&curData->c2spd);
			
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
				
				for (int ll = 0; ll < curData->size / 2; ll++) {
					MADBE16(&shortPtr[ll]);
				}
			}
		}
	}
	
	for (int i = 0; i < MAXINSTRU; i++) {
		MadFile->fid[i].firstSample = i * MAXSAMPLE;
	}
	
	/*********************/
	
	{
		short	alpha = 0, x;
		
		MadFile->sets = (FXSets*)calloc(MAXTRACK, sizeof(FXSets));
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MadFile->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				memcpy(&MadFile->sets[alpha], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				MADBE16(&MadFile->sets[alpha].id);
				MADBE16(&MadFile->sets[alpha].noArg);
				MADBE16(&MadFile->sets[alpha].track);
				MADBE32(&MadFile->sets[alpha].FXID);
				for (int y = 0; y < 100; y++) {
					MADBE32(&MadFile->sets[alpha].values[y]);
				}
				alpha++;
			}
		}
		
		for (i = 0; i < MadFile->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MadFile->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					memcpy(&MadFile->sets[alpha], MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					MADBE16(&MadFile->sets[alpha].id);
					MADBE16(&MadFile->sets[alpha].noArg);
					MADBE16(&MadFile->sets[alpha].track);
					MADBE32(&MadFile->sets[alpha].FXID);
					for (int y = 0; y < 100; y++) {
						MADBE32(&MadFile->sets[alpha].values[y]);
					}
					alpha++;
				}
			}
		}
	}
	
	return MADNoErr;
}

id GetValueUsingKVO(id object)
{
	return [object value];
}

