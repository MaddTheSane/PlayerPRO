//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentObject_PPKPrivate.h"
#import "PPMusicObject.h"
#import "PPMusicObject_PPKPrivate.h"

@implementation PPInstrumentObject
@synthesize theInstrument;
@synthesize number;
@synthesize name;

- (void)writeBackToStruct
{
	//TODO: implement
}

- (short)firstSample
{
	return theInstrument.firstSample;
}

- (short)MIDI
{
	return theInstrument.MIDI;
}

- (void)setMIDI:(short)MIDI
{
	[self willChangeValueForKey:@"MIDI"];
	theInstrument.MIDI = MIDI;
	[self didChangeValueForKey:@"MIDI"];
}

- (BOOL)isSoundOut
{
	switch (theInstrument.MIDIType) {
		case 0:
		case 2:
			return YES;
			break;
			
		default:
			return NO;
			break;
	}
}

- (short)MIDIType
{
	return theInstrument.MIDIType;
}

- (void)setSoundOut:(BOOL)soundOut
{
	[self willChangeValueForKey:@"MIDIType"];
	if (soundOut) {
		if ([self isMIDIOut]) {
			theInstrument.MIDIType = 2;
		} else {
			theInstrument.MIDIType = 0;
		}
	} else {
		if ([self isMIDIOut]) {
			theInstrument.MIDIType = 1;
		} else {
			theInstrument.MIDIType = 3;
		}
	}
	[self didChangeValueForKey:@"MIDIType"];
}

- (BOOL)isMIDIOut
{
	switch (theInstrument.MIDIType) {
		case 1:
		case 2:
			return YES;
			break;
			
		default:
			return NO;
			break;
	}
}

- (void)setMIDIOut:(BOOL)MIDIOut
{
	[self willChangeValueForKey:@"MIDIType"];
	if (MIDIOut) {
		if ([self isSoundOut]) {
			theInstrument.MIDIType = 2;
		} else {
			theInstrument.MIDIType = 1;
		}
	} else {
		if ([self isSoundOut]) {
			theInstrument.MIDIType = 0;
		} else {
			theInstrument.MIDIType = 3;
		}
	}
	[self didChangeValueForKey:@"MIDIType"];
}

- (Byte)volumeSize
{
	return theInstrument.volSize;
}

- (void)setVolumeSize:(Byte)volumeSize
{
	[self willChangeValueForKey:@"volumeSize"];
	theInstrument.volSize = volumeSize;
	[self didChangeValueForKey:@"volumeSize"];
}

- (Byte)panningSize
{
	return theInstrument.pannSize;
}

- (void)setPanningSize:(Byte)panningSize
{
	[self willChangeValueForKey:@"panningSize"];
	theInstrument.pannSize = panningSize;
	[self didChangeValueForKey:@"panningSize"];
}

- (Byte)pitchSize
{
	return theInstrument.pitchSize;
}

- (void)setPitchSize:(Byte)pitchSize
{
	[self willChangeValueForKey:@"pitchSize"];
	theInstrument.pitchSize = pitchSize;
	[self didChangeValueForKey:@"pitchSize"];
}

- (Byte)volumeSustain
{
	return theInstrument.volSus;
}

- (void)setVolumeSustain:(Byte)volumeSustain
{
	[self willChangeValueForKey:@"volumeSustain"];
	theInstrument.volSus = volumeSustain;
	[self didChangeValueForKey:@"volumeSustain"];
}

- (Byte)volumeBegin
{
	return theInstrument.volBeg;
}

- (void)setVolumeBegin:(Byte)volumeBegin
{
	[self willChangeValueForKey:@"volumeBegin"];
	theInstrument.volBeg = volumeBegin;
	[self didChangeValueForKey:@"volumeBegin"];
}

- (Byte)volumeEnd
{
	return theInstrument.volEnd;
}

- (void)setVolumeEnd:(Byte)volumeEnd
{
	[self willChangeValueForKey:@"volumeEnd"];
	theInstrument.volEnd = volumeEnd;
	[self didChangeValueForKey:@"volumeEnd"];
}

- (Byte)panningSustain
{
	return theInstrument.pannSus;
}

- (void)setPanningSustain:(Byte)panningSustain
{
	[self willChangeValueForKey:@"panningSustain"];
	theInstrument.pannSus = panningSustain;
	[self didChangeValueForKey:@"panningSustain"];
}

- (Byte)panningBegin
{
	return theInstrument.pannBeg;
}

- (void)setPanningBegin:(Byte)panningBegin
{
	[self willChangeValueForKey:@"panningBegin"];
	theInstrument.pannBeg = panningBegin;
	[self didChangeValueForKey:@"panningBegin"];
}

- (Byte)panningEnd
{
	return theInstrument.pannEnd;
}

- (void)setPanningEnd:(Byte)panningEnd
{
	[self willChangeValueForKey:@"panningEnd"];
	theInstrument.pannEnd = panningEnd;
	[self willChangeValueForKey:@"panningEnd"];
}

- (Byte)pitchSustain
{
	return theInstrument.pitchSus;
}

- (void)setPitchSustain:(Byte)pitchSustain
{
	[self willChangeValueForKey:@"pitchSustain"];
	theInstrument.pitchSus = pitchSustain;
	[self didChangeValueForKey:@"pitchSustain"];
}

- (Byte)pitchBegin
{
	return theInstrument.pitchBeg;
}

- (void)setPitchBegin:(Byte)pitchBegin
{
	[self willChangeValueForKey:@"pitchBegin"];
	theInstrument.pitchBeg = pitchBegin;
	[self didChangeValueForKey:@"pitchBegin"];
}

- (Byte)pitchEnd
{
	return theInstrument.pitchEnd;
}

- (void)setPitchEnd:(Byte)pitchEnd
{
	[self willChangeValueForKey:@"pitchEnd"];
	theInstrument.pitchEnd = pitchEnd;
	[self didChangeValueForKey:@"pitchEnd"];
}

- (Byte)vibratoDepth
{
	return theInstrument.vibDepth;
}

- (void)setVibratoDepth:(Byte)vibratoDepth
{
	[self willChangeValueForKey:@"vibratoDepth"];
	theInstrument.vibDepth = vibratoDepth;
	[self didChangeValueForKey:@"vibratoDepth"];
}

- (Byte)vibratoRate
{
	return theInstrument.vibRate;
}

- (void)setVibratoRate:(Byte)vibratoRate
{
	[self willChangeValueForKey:@"vibratoRate"];
	theInstrument.vibRate = vibratoRate;
	[self didChangeValueForKey:@"vibratoRate"];
}

- (NSArray*)samples
{
	return [NSArray arrayWithArray:samples];
}

- (short)sampleCount
{
	return [samples count];
}

- (void)setNumber:(NSInteger)numberr
{
	[self willChangeValueForKey:@"number"];
	theInstrument.no = number = numberr;
	theInstrument.firstSample = MAXSAMPLE * numberr;
	[self didChangeValueForKey:@"number"];
}

- (NSInteger)number
{
	return number;
}

+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObjectWrapper*)mus
{
	PPInstrumentObject *insObj = [[self alloc] initWithMusic:mus];
	[mus addInstrument:insObj];
	
	return insObj;
}

- (instancetype)initWithMusic:(PPMusicObjectWrapper*)mus;
{
	if (!mus) {
		return nil;
	}
	if (self = [super init]) {
		_theMus = mus;
		samples = [[NSMutableArray alloc] init];
		theInstrument.no = number = -1;
		theInstrument.firstSample = 0;
		MADResetInstrument(&theInstrument);
		name = @"";
	}
	return self;
}

- (instancetype)initWithMusic:(PPMusicObjectWrapper*)mus instrumentIndex:(short)insIdx;
{
	if (self = [self initWithMusic:mus]) {
		theInstrument = mus._currentMusic->fid[insIdx];
		samples = [[NSMutableArray alloc] initWithCapacity:theInstrument.numSamples];
		{
			int sDataCount = theInstrument.numSamples + theInstrument.firstSample;
			
			for (int i = theInstrument.firstSample; i < sDataCount; i++) {
				PPSampleObject *sObj = [[PPSampleObject alloc] initWithsData:mus._currentMusic->sample[i]];
				sObj.sampleIndex = i % MAXSAMPLE;
				sObj.instrumentIndex = insIdx;
				[samples addObject:sObj];
			}
		}
		name = [[NSString alloc] initWithCString:theInstrument.name encoding:NSMacOSRomanStringEncoding];
		theInstrument.no = number = insIdx;
		//In case it's malformed, i.e. from CreateFreeMADK()
		theInstrument.firstSample = MAXSAMPLE * insIdx; /*tempData->firstSample;*/
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: Sample index %d count %d samples: %@", name, self.firstSample, self.sampleCount, [samples description]];
}

#if 0
- (void)writeSampleAtIndexBackToMusic:(short)idx
{
	if (idx >= MAXSAMPLE) {
		return;
	}
	NSInteger firstSample = self.firstSample;
	
	if (theMus->sample[idx + firstSample]) {
		if (theMus->sample[idx + firstSample]->data)
		{
			free(theMus->sample[idx + firstSample]->data);
		}
		free(theMus->sample[idx + firstSample]);
	}
	
	theMus->sample[idx + firstSample] = [samples[idx] createSData];
}

- (void)writeBackToMusic
{
	NSInteger i, ii;
	int firstSample = self.firstSample;
	int totalSamples = self.sampleCount + firstSample;
	int totalPossibleSamples = firstSample + MAXSAMPLE - 1;
	for (i = firstSample; i < totalPossibleSamples; i++) {
		if (theMus->sample[i]) {
			//We can probably call the free of the data without the check
			//but better safe than sorry
			if (theMus->sample[i]->data) {
				free(theMus->sample[i]->data);
			}
			free(theMus->sample[i]);
			theMus->sample[i] = NULL;
		}
	}
	
	for (i = firstSample, ii = 0; i < totalSamples; i++, ii++) {
		PPSampleObject *sampObj = samples[ii];
		theMus->sample[i] = [sampObj createSData];
	}
	InstrData *newData = &theMus->fid[number];
	char tempstr[32] = {0};
	
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(tempstr) - 1) {
		cStrLen = sizeof(tempstr) - 1;
	}
	[tmpCStr getBytes:tempstr length:cStrLen];
	tmpCStr = nil;
	
	memcpy(newData, &theInstrument, sizeof(InstrData));
	strlcpy(newData->name, tempstr, sizeof(newData->name));
		
	newData->numSamples = self.sampleCount;
	
	theMus->hasChanged = TRUE;
}
#endif

- (void)addSamplesObject:(PPSampleObject *)object
{
	NSAssert(number != -1, @"The instrument should be in a Music Object wrapper BEFORE adding samples");
	if (self.sampleCount >= MAXSAMPLE) {
		return;
	}
	object = [object copy];
	object.sampleIndex = self.sampleCount;
	object.instrumentIndex = number;
	
	[samples addObject:object];
	theInstrument.numSamples++;
}

- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(PPSampleObject *)object
{
	NSAssert(number != -1, @"The instrument should be in a Music Object wrapper BEFORE adding samples");
	if (index >= MAXSAMPLE || index < 0) {
		return;
	}
	object = [object copy];
	object.sampleIndex = index;
	object.instrumentIndex = number;
	
	samples[index] = object;
}

- (NSUInteger)countOfSamples
{
	return [samples count];
}

- (Byte)volumeType
{
	return theInstrument.volType;
}

- (void)setVolumeTypeOn
{
	[self willChangeValueForKey:@"volumeType"];
	theInstrument.volType = 0;
	[self didChangeValueForKey:@"volumeType"];
}

- (void)setVolumeTypeSustain
{
	[self willChangeValueForKey:@"volumeType"];
	theInstrument.volType = 1;
	[self didChangeValueForKey:@"volumeType"];
}

- (void)setVolumeTypeLoop
{
	[self willChangeValueForKey:@"volumeType"];
	theInstrument.volType = 2;
	[self didChangeValueForKey:@"volumeType"];
}

- (BOOL)isVolumeTypeOn
{
	return theInstrument.volType == 0;
}

- (BOOL)isVolumeTypeSustain
{
	return theInstrument.volType == 1;
}

- (BOOL)isVolumeTypeLoop
{
	return theInstrument.volType == 2;
}

- (Byte)panningType
{
	return theInstrument.pannType;
}

- (void)setPanningTypeOn
{
	[self willChangeValueForKey:@"panningType"];
	theInstrument.pannType = 0;
	[self didChangeValueForKey:@"panningType"];
}

- (void)setPanningTypeSustain
{
	[self willChangeValueForKey:@"panningType"];
	theInstrument.pannType = 1;
	[self didChangeValueForKey:@"panningType"];
}

- (void)setPanningTypeLoop
{
	[self willChangeValueForKey:@"panningType"];
	theInstrument.pannType = 2;
	[self didChangeValueForKey:@"panningType"];
}

- (BOOL)isPanningTypeOn
{
	return theInstrument.pannType == 0;
}

- (BOOL)isPanningTypeSustain
{
	return theInstrument.pannType == 1;
}

- (BOOL)isPanningTypeLoop
{
	return theInstrument.pannType == 2;
}

- (NSArray *)children;
{
	return [NSArray arrayWithArray:samples];
}

- (PPSampleObject*)childAtIndex:(NSUInteger)idx
{
	return samples[idx];
}

- (NSUInteger)countOfChildren
{
	return [samples count];
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	[self writeBackToStruct];
	return [[[self class] allocWithZone:zone] initWithMusic:_theMus instrumentIndex:theInstrument.no];
}

#pragma mark NSFastEnumeration protocol

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [samples countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark NSCoding protocol

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	
}

@end
