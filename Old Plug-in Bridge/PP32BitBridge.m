//
//  PP32BitBridge.m
//  PPMacho
//
//  Created by C.W. Betts on 12/21/13.
//
//

#import "PP32BitBridge.h"
#include <PlayerPROCoreTemp/FileUtils.h>
#include <PlayerPROCoreTemp/RDriverInt.h>

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
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
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

@implementation PP32BitBridge

- (id)init
{
	if (self = [super init]) {
		const SInt32 mytab[12] =
		{
			1712*16, 1616*16, 1524*16, 1440*16, 1356*16, 1280*16,
			1208*16, 1140*16, 1076*16, 1016*16, 960*16, 907*16
		};
		the32BitLibrary = (MADLibrary*)calloc(sizeof(MADLibrary), 1);
		
		if (the32BitLibrary == NULL) {
			[self autorelease];
			return nil;
		}
		
		the32BitLibrary->IDType = 'MADD';
		
		//for (int i = 0; i < 12; i++)
		//	the32BitLibrary->mytab[i] = mytab[i];
		memcpy(the32BitLibrary->mytab, mytab, sizeof(mytab));
		
		the32BitLibrary->ThePlug = calloc(sizeof(PlugInfo), MAXPLUG);
		the32BitLibrary->TotalPlug = 0;
	}
	return self;
}

- (BOOL)addPlugInAtPath:(NSURL*)plugPath
{
	return MakeMADPlugFromNSURL(the32BitLibrary, plugPath);
}

- (void)dealloc
{
	if (the32BitLibrary) {
		MADDisposeLibrary(the32BitLibrary);
	}
	
	[super dealloc];
}

- (OSErr)getMusicData:(out NSData **)theOutData fromMusicAtFileURL:(NSURL*)theURL withPlugInType:(char*)theType
{
	if (!theOutData)
		return MADParametersErr;
	
	MADMusic *tempMusic = NULL;
	OSErr theErr =  MADLoadMusicCFURLFile(the32BitLibrary, &tempMusic, theType, (CFURLRef)theURL);
	NSMutableData *saveData = nil;
	
	if (theErr)
		return theErr;
	
	//TODO: error-checking
	int i, x;
	size_t inOutCount;
	saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(tempMusic)];
	if (!saveData) {
		MADDisposeMusic(&tempMusic, NULL);
		return MADNeedMemory;
	}
	
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		tempMusic->fid[i].no = i;
		
		if (tempMusic->fid[i].numSamples > 0 || tempMusic->fid[i].name[0] != 0) {	// Is there something in this instrument?
			x++;
		}
	}
	
	tempMusic->header->numInstru = x;
	{
		MADSpec aHeader;
		aHeader = *tempMusic->header;
		ByteSwapMADSpec(&aHeader);
		[saveData appendBytes:&aHeader length:sizeof(MADSpec)];
		
		for (i = 0; i < tempMusic->header->numPat ; i++) {
			if (tempMusic->partition[i]) {
				PatData *tmpPat = CompressPartitionMAD1(tempMusic, tempMusic->partition[i]);
				inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
				tmpPat->header.compMode = 'MAD1';
				ByteSwapPatHeader(&tmpPat->header);
				[saveData appendBytes:tmpPat length:inOutCount];
				free(tmpPat);
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (tempMusic->fid[i].numSamples > 0 || tempMusic->fid[i].name[0] != 0) {	// Is there something in this instrument?
			tempMusic->fid[i].no = i;
			InstrData instData = tempMusic->fid[i];
			ByteSwapInstrData(&instData);
			[saveData appendBytes:&instData length:sizeof(InstrData)];
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < tempMusic->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			curData = *tempMusic->sample[tempMusic->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[saveData appendBytes:&copyData length:inOutCount];
			
			inOutCount = tempMusic->sample[tempMusic->fid[i].firstSample + x]->size;
			Ptr dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if (curData.amp == 16) {
				__block short *shortPtr = (short*)dataCopy;
				
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
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
		if (tempMusic->header->globalEffect[i]) {
			inOutCount = sizeof(FXSets);
			__block FXSets aSet = tempMusic->sets[alpha];
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
	
	for (i = 0; i < tempMusic->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (tempMusic->header->chanEffect[i][x]) {
				inOutCount = sizeof(FXSets);
				__block FXSets aSet = tempMusic->sets[alpha];
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
	
	*theOutData = [NSData dataWithData:saveData];
	[saveData release];
	MADDisposeMusic(&tempMusic, NULL);
	if (!(*theOutData))
		return MADNeedMemory;
	else
		 return noErr;
}

- (BOOL)canLoadFileAtURL:(NSURL*)theURL withPlugInType:(char*)theType
{
	char theTempType[5] = {0};
	OSErr theErr = MADMusicIdentifyCFURL(the32BitLibrary, theTempType, (CFURLRef)theURL);
	if (theErr) {
		return NO;
	}
	if (strcmp(theTempType, theType) == 0) {
		return YES;
	}
	return NO;
}

- (OSErr)getMusicInformation:(out NSData **)theOutData fromMusicAtFileURL:(NSURL*)theURL withPlugInType:(char*)theType
{
	if (!theOutData) {
		return MADParametersErr;
	}
	PPInfoRec	theInfo = {0};
	OSErr		theErr = noErr;
	
	
	theErr = MADMusicInfoCFURL(the32BitLibrary, theType, (CFURLRef)theURL, &theInfo);
	
	NSData *retData = [NSData dataWithBytes:&theInfo length:sizeof(theInfo)];
	if (!retData) {
		return MADNeedMemory;
	}
	
	*theOutData = retData;
	
	return theErr;
}

@end
