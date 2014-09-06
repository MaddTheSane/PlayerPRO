//
//  MIDI.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <Cocoa/Cocoa.h>
@import PlayerPROCore;
#include <PlayerPROCore/RDriverInt.h>
@import PlayerPROKit;
#import "PPMIDIImporter.h"
#import "MIDI.h"

static MADErr MADReadMAD(MADMusic *MDriver, const void* MADPtr)
{
	short		i, x;
	size_t		inOutCount, OffSetToSample = 0;
	PatHeader	tempPatHeader;
	
	MDriver->musicUnderModification = false;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	MDriver->header = (MADSpec*)calloc(inOutCount, 1);
	if (MDriver->header == NULL) {
		return MADNeedMemory;
	}
	
	memcpy(MDriver->header, MADPtr, inOutCount);
	OffSetToSample += inOutCount;
	
	if (MDriver->header->MAD != 'MADK' || MDriver->header->numInstru > MAXINSTRU) {
		free(MDriver->header);
		return MADIncompatibleFile;
	}
	
	if (MDriver->header->MultiChanNo == 0)
		MDriver->header->MultiChanNo = 48;
	
	/**** PARTITION ****/
	dispatch_apply(MAXPATTERN - MDriver->header->numPat, dispatch_get_global_queue(0, 0), ^(size_t iTmp) {
		size_t i = iTmp + MDriver->header->numPat;
		MDriver->partition[i] = NULL;
	});
	
	for (i = 0; i < MDriver->header->numPat; i++) {
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
		
		inOutCount = sizeof(PatHeader) + MDriver->header->numChn * tempPatHeader.size * sizeof(Cmd);
		
		MDriver->partition[i] = (PatData*)malloc(inOutCount);
		if (MDriver->partition[i] == NULL) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free(MDriver->partition[x]);
			}
			free(MDriver->header);
			
			return MADNeedMemory;
		}
		
		memcpy(MDriver->partition[i], MADPtr + OffSetToSample, inOutCount);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	MDriver->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!MDriver->fid) {
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		
		return MADNeedMemory;
	}
	
	inOutCount = sizeof(InstrData) * MDriver->header->numInstru;
	memcpy(MDriver->fid, MADPtr + OffSetToSample, inOutCount);
	OffSetToSample += inOutCount;
	
	for (i = MDriver->header->numInstru - 1; i >= 0 ; i--) {
		InstrData *curIns = &MDriver->fid[i];
		
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
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < MDriver->fid[i].numSamples ; x++) {
			sData	 *curData;
			
			// ** Read Sample header **
			
			curData = MDriver->sample[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			memcpy(curData, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = (char*)malloc(curData->size);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = curData->size;
			memcpy(curData->data, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
		}
	}
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
		MDriver->fid[ixi].firstSample = ixi * MAXSAMPLE;
	});
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		MDriver->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MDriver->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				alpha++;
			}
		}
		
		for (i = 0; i < MDriver->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MDriver->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					alpha++;
				}
			}
		}
	}
	
	MDriver->header->MAD = 'MADK';
	
	return MADNoErr;
}

PPMusicObject *MIDIReadFromData(NSData *fileData)
{
	MADMusic *theNewMusic = malloc(sizeof(MADMusic));
	MADReadMAD(theNewMusic, [fileData bytes]);
	return [[PPMusicObject alloc] initWithMusicStruct:theNewMusic copy:NO];
}

static unsigned int LongFromFd(Ptr fd, unsigned cb)
{
	unsigned char rgb[4];
	unsigned int longT = 0;
	unsigned ib = 0;
	
	memcpy(rgb, fd, cb);
	for (; ib < cb; ib++)
		longT = (longT << 8) + rgb[ib];
	return longT;
}

NSInteger GetTracksNumber(NSURL *theURL)
{
	NSFileHandle	*wrapper = [NSFileHandle fileHandleForReadingFromURL:theURL error:nil];
	const char		rgbHeader[] = {'M', 'T', 'h', 'd', 0, 0, 0, 6, 0};
	NSData			*rgbNSHeader = [[NSData alloc] initWithBytesNoCopy:(void*)rgbHeader length:sizeof(rgbHeader) freeWhenDone:NO];
	char			tmpHeader[9] = {0};
	short			irfMax;
	
	NSData * myData = [wrapper readDataOfLength:9];
	if (![myData isEqual:rgbNSHeader]) {
		return -1; /** Only process type 0 or type 1 general MIDI files **/
	}
	// Gobble a byte
	[wrapper readDataOfLength:1];
	
	myData = [wrapper readDataOfLength:2];
	[myData getBytes:tmpHeader length:2];
	irfMax = (unsigned) LongFromFd(tmpHeader, 2); /** Get # tracks **/
	irfMax++;
	irfMax /= 2;
	irfMax *= 2;
	
	[wrapper closeFile];
	
	return irfMax;
}
