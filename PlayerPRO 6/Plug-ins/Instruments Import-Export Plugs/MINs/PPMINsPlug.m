//
//  PPMINsPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/14.
//
//

#import "PPMINsPlug.h"

static inline void ByteswapsData(sData *toswap)
{
	MADBE32(&toswap->size);
	MADBE16(&toswap->c2spd);
	MADBE32(&toswap->loopBeg);
	MADBE32(&toswap->loopSize);
}

static inline void ByteswapInstrument(InstrData *toswap)
{
	MADBE16(&toswap->MIDIType);
	MADBE16(&toswap->MIDI);
	MADBE16(&toswap->firstSample);
	MADBE16(&toswap->numSamples);
	MADBE16(&toswap->volFade);
	
	dispatch_apply(12, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE16(&toswap->pannEnv[i].pos);
		MADBE16(&toswap->pannEnv[i].val);
		MADBE16(&toswap->pitchEnv[i].pos);
		MADBE16(&toswap->pitchEnv[i].val);
		MADBE16(&toswap->volEnv[i].pos);
		MADBE16(&toswap->volEnv[i].val);
	});
}

static inline OSErr TestMINS(const InstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

@implementation PPMINsPlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (BOOL)isInstrument
{
	return YES;
}

- (BOOL)canImportSampleAtURL:(NSURL *)sampleURL
{
	NSFileHandle *readHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!readHandle) {
		return NO;
	}
	size_t inOutCount = sizeof(InstrData);
	//void *theSound = malloc(inOutCount);
	NSMutableData *theSound = [[readHandle readDataOfLength:inOutCount] mutableCopy];
	ByteswapInstrument(theSound.mutableBytes);
	return TestMINS(theSound.bytes) == MADNoErr;
}

- (MADErr)importSampleAtURL:(NSURL *)sampleURL instrument:(inout PPInstrumentObject *)InsHeader sample:(inout PPSampleObject *)sample sampleID:(inout short *)sampleID driver:(PPDriver *)driver
{
	NSFileHandle *readHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!readHandle) {
		return MADReadingErr;
	}
	[InsHeader resetInstrument];
	size_t inOutCount = sizeof(InstrData);
	NSMutableData *aHeader = [[readHandle readDataOfLength:inOutCount] mutableCopy];
	ByteswapInstrument(aHeader.mutableBytes);
	const InstrData *ourData = aHeader.bytes;
	{
		InsHeader.name = [NSString stringWithCString:ourData->name encoding:NSMacOSRomanStringEncoding];
		memcpy(InsHeader.what, ourData->what, 96);
		for (int i = 0; i < 12; i++) {
			[InsHeader replaceObjectInPanningEnvelopeAtIndex:i withObject:[[PPEnvelopeObject alloc] initWithEnvRec:ourData->pannEnv[i]]];
			[InsHeader replaceObjectInVolumeEnvelopeAtIndex:i withObject:[[PPEnvelopeObject alloc] initWithEnvRec:ourData->volEnv[i]]];
			[InsHeader replaceObjectInPitchEnvelopeAtIndex:i withObject:[[PPEnvelopeObject alloc] initWithEnvRec:ourData->pitchEnv[i]]];
		}
		InsHeader.volumeType = ourData->volType;
		InsHeader.panningType = ourData->pannType;
		
		InsHeader.volumeSize = ourData->volSize;
		InsHeader.panningSize = ourData->pannSize;
		InsHeader.pitchSize = ourData->pitchSize;
		
		InsHeader.volumeSustain = ourData->volSus;
		InsHeader.volumeBegin = ourData->volBeg;
		InsHeader.volumeEnd = ourData->volEnd;
		
		InsHeader.panningSustain = ourData->pannSus;
		InsHeader.panningBegin = ourData->pannBeg;
		InsHeader.panningEnd = ourData->pannEnd;
		
		InsHeader.pitchSustain = ourData->pitchSus;
		InsHeader.pitchBegin = ourData->pitchBeg;
		InsHeader.pitchEnd = ourData->pitchEnd;
		
		InsHeader.volumeFadeOut = ourData->volFade;
		
		InsHeader.vibratoDepth = ourData->vibDepth;
		InsHeader.vibratoRate = ourData->vibRate;
		
		InsHeader.MIDI = ourData->MIDI;
		
		switch (ourData->MIDIType) {
			case 0:
			default:
				InsHeader.MIDIOut = NO;
				InsHeader.soundOut = YES;
				break;
				
			case 1:
				InsHeader.MIDIOut = YES;
				InsHeader.soundOut = NO;
				break;
				
			case 2:
				InsHeader.MIDIOut = YES;
				InsHeader.soundOut = YES;
				break;
				
			case 3:
				InsHeader.MIDIOut = NO;
				InsHeader.soundOut = NO;
				break;
		}

	}
	
	for (int x = 0; x < ourData->numSamples; x++) {
		sData *curData = MADCreateSampleRaw();
		inOutCount = sizeof(sData32);
		aHeader = [[readHandle readDataOfLength:inOutCount] mutableCopy];
		ByteswapsData(aHeader.mutableBytes);
		memcpy(curData, aHeader.bytes, inOutCount);
		
		inOutCount = curData->size;
		curData->data = malloc(inOutCount);
		NSData *sampData = [readHandle readDataOfLength:inOutCount];
		memcpy(curData->data, sampData.bytes, inOutCount);
		if (curData->amp == 16) {
			short	*shortPtr = (short*)curData->data;
			dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
				MADBE16(&shortPtr[ll]);
			});
		}
		
		[InsHeader addSamplesObject:[[PPSampleObject alloc] initWithSData:curData]];
		free(curData->data);
		free(curData);
	}
	
	return MADNoErr;
}

- (MADErr)exportSampleToURL:(NSURL *)sampleURL instrument:(PPInstrumentObject *)InsHeader sample:(PPSampleObject *)sample sampleID:(short)sampleID driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

@end
