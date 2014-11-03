//
//  PPMINsPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/14.
//
//

#import "PPMINsPlug.h"
#include <PlayerPROCore/PlayerPROCore.h>
@import PlayerPROKit.PPInstrumentObject;
@import PlayerPROKit.PPSampleObject;

#pragma pack(push, 2)
typedef struct oldInstrData
{
	char 	name[32];
	Byte 	type;
	Byte	no;
	
	short	firstSample;
	short	numSamples;
	
	Byte	what[96];
	EnvRec 	volEnv[12];
	EnvRec	pannEnv[12];
	
	Byte	volSize;
	Byte	pannSize;
	
	Byte	volSus;
	Byte	volBeg;
	Byte	volEnd;
	
	Byte	pannSus;
	Byte	pannBeg;
	Byte	pannEnd;
	
	Byte	volType;
	Byte	pannType;
	
	unsigned short	volFade;
	
	Byte	vibDepth;
	Byte	vibRate;
} oldInstrData;
#pragma pack(pop)

@interface PPInstrumentObject (private)
- (InstrData)theInstrument;
@end

static inline void ByteswapsData(sData *toswap)
{
	MADBE32(&toswap->size);
	MADBE16(&toswap->c2spd);
	MADBE32(&toswap->loopBeg);
	MADBE32(&toswap->loopSize);
}

static inline void ByteswapsData32(sData32 *toswap)
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

static inline void ByteswapOldInstrument(oldInstrData *toswap)
{
	MADBE16(&toswap->firstSample);
	MADBE16(&toswap->numSamples);
	MADBE16(&toswap->volFade);
	
	dispatch_apply(12, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE16(&toswap->pannEnv[i].pos);
		MADBE16(&toswap->pannEnv[i].val);
		MADBE16(&toswap->volEnv[i].pos);
		MADBE16(&toswap->volEnv[i].val);
	});
}

static inline MADErr TestMINS(const InstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static inline MADErr TestOldMINs(const oldInstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE) {
		return MADNoErr;
	} else {
		return MADFileNotSupportedByThisPlug;
	}
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

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)canImportSampleAtURL:(NSURL *)sampleURL
{
	NSFileHandle *readHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!readHandle) {
		return NO;
	}
	size_t inOutCount = sizeof(InstrData);
	//void *theSound = malloc(inOutCount);
	NSData *fileData = [readHandle readDataOfLength:inOutCount];
	NSMutableData *theSound = [fileData mutableCopy];
	ByteswapInstrument(theSound.mutableBytes);
	BOOL newMins = TestMINS(theSound.bytes) == MADNoErr;
	if (!newMins) {
		theSound = [fileData mutableCopy];
		ByteswapOldInstrument(theSound.mutableBytes);
		return TestOldMINs(theSound.bytes) == MADNoErr;
	} else {
		return YES;
	}
}

- (MADErr)importSampleAtURL:(NSURL *)sampleURL instrument:(inout PPInstrumentObject *)InsHeader sample:(inout PPSampleObject *)sample sampleID:(inout short *)sampleID driver:(PPDriver *)driver
{
	NSFileHandle *readHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!readHandle) {
		return MADReadingErr;
	}
	size_t inOutCount = sizeof(InstrData);
	NSMutableData *aHeader = [[readHandle readDataOfLength:inOutCount] mutableCopy];
	ByteswapInstrument(aHeader.mutableBytes);
	const InstrData *ourData = aHeader.bytes;
	if (TestMINS(ourData) == MADNoErr) {
		[InsHeader resetInstrument];
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
				short *shortPtr = (short*)curData->data;
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
					MADBE16(&shortPtr[ll]);
				});
			}
			
			[InsHeader addSampleObject:[[PPSampleObject alloc] initWithSData:curData]];
			free(curData->data);
			free(curData);
		}
	} else {
		// re-open the file handle, because we can't go backwards in a file handle
		[readHandle closeFile];
		readHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
		inOutCount = sizeof(oldInstrData);
		aHeader = [[readHandle readDataOfLength:inOutCount] mutableCopy];
		ByteswapOldInstrument(aHeader.mutableBytes);
		const oldInstrData *oldOurData = aHeader.bytes;
		if (TestOldMINs(oldOurData) != MADNoErr) {
			return MADFileNotSupportedByThisPlug;
		}
		[InsHeader resetInstrument];
		InsHeader.name = [NSString stringWithCString:oldOurData->name encoding:NSMacOSRomanStringEncoding];
		memcpy(InsHeader.what, oldOurData->what, 96);
		for (int i = 0; i < 12; i++) {
			[InsHeader replaceObjectInPanningEnvelopeAtIndex:i withObject:[[PPEnvelopeObject alloc] initWithEnvRec:oldOurData->pannEnv[i]]];
			[InsHeader replaceObjectInVolumeEnvelopeAtIndex:i withObject:[[PPEnvelopeObject alloc] initWithEnvRec:oldOurData->volEnv[i]]];
		}
		InsHeader.volumeType = oldOurData->volType;
		InsHeader.panningType = oldOurData->pannType;
		
		InsHeader.volumeSize = oldOurData->volSize;
		InsHeader.panningSize = oldOurData->pannSize;
		
		InsHeader.volumeSustain = oldOurData->volSus;
		InsHeader.volumeBegin = oldOurData->volBeg;
		InsHeader.volumeEnd = oldOurData->volEnd;
		
		InsHeader.panningSustain = oldOurData->pannSus;
		InsHeader.panningBegin = oldOurData->pannBeg;
		InsHeader.panningEnd = oldOurData->pannEnd;
		
		InsHeader.volumeFadeOut = oldOurData->volFade;
		
		InsHeader.vibratoDepth = oldOurData->vibDepth;
		InsHeader.vibratoRate = oldOurData->vibRate;
		
		for (int x = 0; x < oldOurData->numSamples; x++) {
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
				short *shortPtr = (short*)curData->data;
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
					MADBE16(&shortPtr[ll]);
				});
			}
			
			[InsHeader addSampleObject:[[PPSampleObject alloc] initWithSData:curData]];
			free(curData->data);
			free(curData);
		}
	}
	
	return MADNoErr;
}

- (MADErr)exportSampleToURL:(NSURL *)sampleURL instrument:(PPInstrumentObject *)InsHeader sample:(PPSampleObject *)sample sampleID:(short)sampleID driver:(PPDriver *)driver
{
	NSFileHandle *fileHand = [NSFileHandle fileHandleForWritingToURL:sampleURL error:NULL];
	if (fileHand == nil) {
		return MADWritingErr;
	}
	size_t inOutCount = sizeof(InstrData);
	InstrData *tempIns = malloc(inOutCount);
	InstrData headerData = [InsHeader theInstrument];
	memcpy(tempIns, &headerData, sizeof(InstrData));
	ByteswapInstrument(tempIns);
	[fileHand writeData:[NSData dataWithBytesNoCopy:tempIns length:inOutCount]];
	for (PPSampleObject *samp in [InsHeader samples]) {
		sData32 toWrite = {0};
		toWrite.amp = samp.amplitude;
		toWrite.c2spd = samp.c2spd;
		toWrite.loopBeg = samp.loopBegin;
		toWrite.loopSize = samp.loopSize;
		toWrite.loopType = samp.loopType;
		toWrite.relNote = samp.relativeNote;
		toWrite.stereo = samp.stereo;
		toWrite.vol = samp.volume;
		toWrite.size = (int)[samp.data length];
		strlcpy(toWrite.name, [samp.name cStringUsingEncoding:NSMacOSRomanStringEncoding], sizeof(toWrite.name));
		ByteswapsData32(&toWrite);
		
		[fileHand writeData:[NSData dataWithBytes:&toWrite length:sizeof(sData32)]];
		
		NSMutableData *sDataData = [samp.data mutableCopy];
		if (samp.amplitude == 16) {
			short	*shortPtr = (short*)sDataData.mutableBytes;
			dispatch_apply(sDataData.length / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
				MADBE16(&shortPtr[ll]);
			});
		}
		[fileHand writeData:sDataData];
	}
	[fileHand closeFile];
	
	return MADNoErr;
}

@end
