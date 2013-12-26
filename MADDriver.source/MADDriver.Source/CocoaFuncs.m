//
//  CocoaFuncs.m
//  PPMacho
//
//  Created by C.W. Betts on 10/24/13.
//
//

#import <Foundation/Foundation.h>
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

#if !TARGET_OS_IPHONE
void SetOSType(CFURLRef theURL, OSType theType)
{
	@autoreleasepool {
		NSURL *theNSURL = (__bridge NSURL*)theURL;
		if (theType != 0) {
			[theNSURL setResourceValue:@(theType) forKey:NSFileHFSTypeCode error:NULL];
		}
	}
}
#endif

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

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	PPBE16(&toSwap->numSamples);
	PPBE16(&toSwap->firstSample);
	PPBE16(&toSwap->volFade);
	PPBE16(&toSwap->MIDI);
	PPBE16(&toSwap->MIDIType);
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
		PPBE16(&toSwap->volEnv[x].pos);
		PPBE16(&toSwap->volEnv[x].val);
		PPBE16(&toSwap->pannEnv[x].pos);
		PPBE16(&toSwap->pannEnv[x].val);
		PPBE16(&toSwap->pitchEnv[x].pos);
		PPBE16(&toSwap->pitchEnv[x].val);
	});
}

static inline void ByteSwapsData(sData *toSwap)
{
	PPBE32(&toSwap->size);
	PPBE32(&toSwap->loopBeg);
	PPBE32(&toSwap->loopSize);
	PPBE16(&toSwap->c2spd);
}

OSErr MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, Boolean compressMAD)
{
	@autoreleasepool {
		NSURL *saveURL = (__bridge NSURL*)urlRef;
		OSErr theErr = noErr;
		NSMutableData *saveData = nil;
				
		//TODO: error-checking
		int i, x;
		size_t inOutCount;
		saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(music)];
		if (!saveData) {
			return MADNeedMemory;
		}
		
		for (i = 0, x = 0; i < MAXINSTRU; i++) {
			music->fid[i].no = i;
			
			if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) 	// Is there something in this instrument?
				x++;
		}
		
		music->header->numInstru = x;
		
		MADSpec aHeader;
		aHeader = *music->header;
		ByteSwapMADSpec(&aHeader);
		[saveData appendBytes:&aHeader length:sizeof(MADSpec)];
		
		if (compressMAD) {
			for (short i = 0; i < music->header->numPat ; i++)
			{
				if (music->partition[i]) {
					PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
					inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
					tmpPat->header.compMode = 'MAD1';
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		} else {
			for (short i = 0; i < music->header->numPat; i++) {
				if (music->partition[i]) {
					inOutCount = sizeof(PatHeader);
					inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
					PatData *tmpPat = calloc(inOutCount, 1);
					memcpy(tmpPat, music->partition[i], inOutCount);
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		}
		
		for (short i = 0; i < MAXINSTRU; i++) {
			if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
				music->fid[i].no = i;
				InstrData instData = music->fid[i];
				ByteSwapInstrData(&instData);
				[saveData appendBytes:&instData length:sizeof(InstrData)];
			}
		}
		
		for (short i = 0; i < MAXINSTRU; i++) {
			for (short x = 0; x < music->fid[i].numSamples; x++) {
				sData	curData;
				sData32	copyData;
				curData = *music->sample[music->fid[i].firstSample + x];
				
				inOutCount = sizeof(sData32);
				ByteSwapsData(&curData);
				memcpy(&copyData, &curData, inOutCount);
				copyData.data = 0;
				[saveData appendBytes:&copyData length:inOutCount];
				
				inOutCount = music->sample[music->fid[i].firstSample + x]->size;
				void *dataCopy = malloc(inOutCount);
				memcpy(dataCopy, curData.data, inOutCount);
				if (curData.amp == 16) {
					__block short *shortPtr = (short*)dataCopy;
					
					dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
						PPBE16(&shortPtr[y]);
					});
				}
				
				[saveData appendBytes:dataCopy length:inOutCount];
				free(dataCopy);
			}
		}
		
		// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
		
		int alpha = 0;
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (music->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				__block FXSets aSet = music->sets[alpha];
				PPBE16(&aSet.id);
				PPBE16(&aSet.noArg);
				PPBE16(&aSet.track);
				PPBE32(&aSet.FXID);
				dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
					PPBE32(&aSet.values[y]);
				});
				
				[saveData appendBytes:&aSet length:inOutCount];
				alpha++;
			}
		}
		
		for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (music->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					__block FXSets aSet = music->sets[alpha];
					PPBE16(&aSet.id);
					PPBE16(&aSet.noArg);
					PPBE16(&aSet.track);
					PPBE32(&aSet.FXID);
					dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
						PPBE32(&aSet.values[y]);
					});
					
					[saveData appendBytes:&aSet length:inOutCount];
					alpha++;
				}
			}
		}
		
		if (![saveData writeToURL:saveURL atomically:NO])
			theErr = MADWritingErr;
		
#if !__has_feature(objc_arc)
		[saveData release];
#endif
		
		music->header->numInstru = MAXINSTRU;

		return theErr;
	}
}
