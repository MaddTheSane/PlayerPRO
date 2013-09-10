//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentObject.h"
#import "PPSampleObject.h"

@implementation PPInstrumentObject

@synthesize number;
@synthesize name;

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
	theInstrument.MIDI = MIDI;
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

- (void)setSoundOut:(BOOL)soundOut
{
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
}

- (Byte)volumeSize
{
	return theInstrument.volSize;
}

- (void)setVolumeSize:(Byte)volumeSize
{
	theInstrument.volSize = volumeSize;
}

- (Byte)panningSize
{
	return theInstrument.pannSize;
}

- (void)setPanningSize:(Byte)panningSize
{
	theInstrument.pannSize = panningSize;
}

- (Byte)pitchSize
{
	return theInstrument.pitchSize;
}

- (void)setPitchSize:(Byte)pitchSize
{
	theInstrument.pitchSize = pitchSize;
}

- (Byte)volumeSustain
{
	return theInstrument.volSus;
}

- (void)setVolumeSustain:(Byte)volumeSustain
{
	theInstrument.volSus = volumeSustain;
}

- (Byte)volumeBegin
{
	return theInstrument.volBeg;
}

- (void)setVolumeBegin:(Byte)volumeBegin
{
	theInstrument.volBeg = volumeBegin;
}

- (Byte)volumeEnd
{
	return theInstrument.volEnd;
}

- (void)setVolumeEnd:(Byte)volumeEnd
{
	theInstrument.volEnd = volumeEnd;
}

- (Byte)panningSustain
{
	return theInstrument.pannSus;
}

- (void)setPanningSustain:(Byte)panningSustain
{
	theInstrument.pannSus = panningSustain;
}

- (Byte)panningBegin
{
	return theInstrument.pannBeg;
}

- (void)setPanningBegin:(Byte)panningBegin
{
	theInstrument.pannBeg = panningBegin;
}

- (Byte)panningEnd
{
	return theInstrument.pannEnd;
}

- (void)setPanningEnd:(Byte)panningEnd
{
	theInstrument.pannEnd = panningEnd;
}

- (Byte)pitchSustain
{
	return theInstrument.pitchSus;
}

- (void)setPitchSustain:(Byte)pitchSustain
{
	theInstrument.pitchSus = pitchSustain;
}

- (Byte)pitchBegin
{
	return theInstrument.pitchBeg;
}

- (void)setPitchBegin:(Byte)pitchBegin
{
	theInstrument.pitchBeg = pitchBegin;
}

- (Byte)pitchEnd
{
	return theInstrument.pitchEnd;
}

- (void)setPitchEnd:(Byte)pitchEnd
{
	theInstrument.pitchEnd = pitchEnd;
}

- (Byte)vibratoDepth
{
	return theInstrument.vibDepth;
}

- (void)setVibratoDepth:(Byte)vibratoDepth
{
	theInstrument.vibDepth = vibratoDepth;
}

- (Byte)vibratoRate
{
	return theInstrument.vibRate;
}

- (void)setVibratoRate:(Byte)vibratoRate
{
	theInstrument.vibRate = vibratoRate;
}

- (NSArray*)samples
{
	return [NSArray arrayWithArray:samples];
}

- (short)sampleCount
{
	return [samples count];
}

- (id)copyWithZone:(NSZone *)zone
{
	return [[PPInstrumentObject allocWithZone:zone] initWithMusic:theMus instrumentIndex:theInstrument.no];
}

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
{
	if (self = [super init]) {
		if (!mus) {
			return nil;
		}
		theMus = mus;
		
		InstrData *tempData = &mus->fid[insIdx];
		memcpy(&theInstrument, tempData, sizeof(InstrData));
		samples = [[NSMutableArray alloc] initWithCapacity:tempData->numSamples];
		{
			int sDataCount = tempData->numSamples + tempData->firstSample;
			
			int i = 0;
			for (i = tempData->firstSample; i < sDataCount; i++) {
				PPSampleObject *sObj = [[PPSampleObject alloc] initWithsData:mus->sample[i]];
				sObj.sampleIndex = i % MAXSAMPLE;
				sObj.instrumentIndex = insIdx;
				[samples addObject:sObj];
			}
		}
		self.name = [[NSString alloc] initWithCString:tempData->name encoding:NSMacOSRomanStringEncoding];
		theInstrument.no = number = insIdx;/*tempData->no;*/
		//In case it's malformed, i.e. from CreateFreeMADK()
		theInstrument.firstSample = MAXSAMPLE * insIdx; /*tempData->firstSample;*/
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: Sample index %d count %d samples: %@", name, self.firstSample, self.sampleCount, [samples description]];
}

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
	NSInteger firstSample = self.firstSample;
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

- (void)addSamplesObject:(PPSampleObject *)object
{
	if (self.sampleCount >= MAXSAMPLE) {
		return;
	}
	[samples addObject:object];
	theInstrument.numSamples++;
	theMus->hasChanged = TRUE;
}

- (void)replaceObjectInSamplesAtIndex:(short)index withObject:(PPSampleObject *)object
{
	if (index >= MAXSAMPLE || index < 0) {
		return;
	}
	samples[index] = object;
	theMus->hasChanged = TRUE;
}

- (NSUInteger)countOfSamples
{
	return [samples count];
}

- (void)setVolumeTypeOn
{
	theInstrument.volType = 0;
}

- (void)setVolumeTypeSustain
{
	theInstrument.volType = 1;
}

- (void)setVolumeTypeLoop
{
	theInstrument.volType = 2;
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

- (void)setPanningTypeOn
{
	theInstrument.pannType = 0;
}

- (void)setPanningTypeSustain
{
	theInstrument.pannType = 1;
}

- (void)setPanningTypeLoop
{
	theInstrument.pannType = 2;
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

@end
