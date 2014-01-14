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

#pragma mark PlayerPROKit Envelope NSCoding keys
#define PPEnvPos @"PlayerPROKit EnvRec Position"
#define PPEnvVal @"PlayerPROKit EnvRec Value"

#pragma mark PlayerPROKit Sample NSCoding keys
#define PPName @"PlayerPROKit Sample Name"
#define PPLocation @"PlayerPROKit Sample Location"
#define PPSampCount @"PlayerPROKit Sample Count"
#define PPMIDI @"PlayerPROKit Sample MIDI"
#define PPMIDIType @"PlayerPROKit Sample MIDI Type"
#define PPVolSize @"PlayerPROKit Sample Volume Size"
#define PPPannSize @"PlayerPROKit Sample Panning Size"
#define PPPitchSize @"PlayerPROKit Sample Pitch Size"
#define PPVolSus @"PlayerPROKit Sample Volume Sustain"
#define PPVolBeg @"PlayerPROKit Sample Volume Begin"
#define PPVolEnd @"PlayerPROKit Sample Volume End"
#define PPPanSus @"PlayerPROKit Sample Panning Sustain"
#define PPPanBeg @"PlayerPROKit Sample Panning Begin"
#define PPPanEnd @"PlayerPROKit Sample Panning End"
#define PPPitchSus @"PlayerPROKit Sample Pitch Sustain"
#define PPPitchBeg @"PlayerPROKit Sample Pitch Begin"
#define PPPitchEnd @"PlayerPROKit Sample Pitch End"
#define PPVibDepth @"PlayerPROKit Sample Vibrato Depth"
#define PPVibRate @"PlayerPROKit Sample Vibrato Rate"
#define PPVolType @"PlayerPROKit Sample Volume Type"
#define PPPannType @"PlayerPROKit Sample Panning Type"
#define PPVolFade @"PlayerPROKit Sample Volume Fade"
#define PPNotes @"PlayerPROKit Sample Notes"

@interface PPEnvelopeObject ()
@end

@implementation PPEnvelopeObject
@synthesize envelopeRec;

- (short)position
{
	return envelopeRec.pos;
}

- (void)setPosition:(short)position
{
	[self willChangeValueForKey:@"position"];
	[self willChangeValueForKey:@"envelopeRec"];
	envelopeRec.pos = position;
	[self didChangeValueForKey:@"position"];
	[self didChangeValueForKey:@"envelopeRec"];
}

- (short)value
{
	return envelopeRec.val;
}

- (void)setValue:(short)value
{
	[self willChangeValueForKey:@"value"];
	[self willChangeValueForKey:@"envelopeRec"];
	envelopeRec.val = value;
	[self didChangeValueForKey:@"value"];
	[self didChangeValueForKey:@"envelopeRec"];
}

- (void)writeBackToStruct
{
	//Do nothing
}

- (instancetype)initWithEnvRec:(EnvRec)theRec
{
	if (self = [super init]) {
		envelopeRec = theRec;
	}
	
	return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		envelopeRec.pos = [aDecoder decodeIntForKey:PPEnvPos];
		envelopeRec.val = [aDecoder decodeIntForKey:PPEnvVal];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeInt:envelopeRec.pos forKey:PPEnvPos];
	[aCoder encodeInt:envelopeRec.val forKey:PPEnvVal];
}

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] alloc] initWithEnvRec:envelopeRec];
}

@end

@implementation PPInstrumentObject
@synthesize theInstrument;
@synthesize number;
@synthesize name;
@synthesize _pitchEnvelope;
@synthesize _volumeEnvelope;
@synthesize _panningEnvelope;

- (NSArray*)volumeEnvelope
{
	return [NSArray arrayWithArray:_volumeEnvelope];
}

- (NSArray*)panningEnvelope
{
	return [NSArray arrayWithArray:_panningEnvelope];
}

- (NSArray*)pitchEnvelope
{
	return [NSArray arrayWithArray:_pitchEnvelope];
}

- (void)writeBackToStruct
{
	char tempstr[32] = {0};
	
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(tempstr) - 1) {
		cStrLen = sizeof(tempstr) - 1;
	}
	[tmpCStr getBytes:tempstr length:cStrLen];
	tmpCStr = nil;
	
	//memcpy(newData, &theInstrument, sizeof(InstrData));
	strlcpy(theInstrument.name, tempstr, sizeof(theInstrument.name));
	
	theInstrument.numSamples = [samples count];
	
	for (char i = 0; i < 12; i++) {
		theInstrument.pitchEnv[i] = [_pitchEnvelope[i] envelopeRec];
		theInstrument.volEnv[i] = [_volumeEnvelope[i] envelopeRec];
		theInstrument.pannEnv[i] = [_panningEnvelope[i] envelopeRec];
	}
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
		_panningEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_volumeEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_pitchEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		for (int i = 0; i < 12; i++) {
			[_panningEnvelope addObject:[[PPEnvelopeObject alloc] init]];
			[_volumeEnvelope addObject:[[PPEnvelopeObject alloc] init]];
			[_pitchEnvelope addObject:[[PPEnvelopeObject alloc] init]];
		}

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
		_panningEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_volumeEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_pitchEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		for (int i = 0; i < 12; i++) {
			[_panningEnvelope addObject:[[PPEnvelopeObject alloc] initWithEnvRec:theInstrument.pannEnv[i]]];
			[_volumeEnvelope addObject:[[PPEnvelopeObject alloc] initWithEnvRec:theInstrument.volEnv[i]]];
			[_pitchEnvelope addObject:[[PPEnvelopeObject alloc] initWithEnvRec:theInstrument.pitchEnv[i]]];

		}
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: Sample index %d count %lu samples: %@", name, self.firstSample, (unsigned long)[samples count], [samples description]];
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
	if ([samples count] >= MAXSAMPLE) {
		return;
	}
	object = [object copy];
	object.sampleIndex = [samples count];
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

- (void)setVolumeTypeOn:(BOOL)typeOn
{
	[self willChangeValueForKey:@"volumeType"];
	if (typeOn) {
		theInstrument.volType |= (Byte)EFON;
	} else {
		theInstrument.volType &= ~(Byte)EFON;
	}
	[self didChangeValueForKey:@"volumeType"];
}

- (void)setVolumeTypeSustain:(BOOL)typeSus
{
	[self willChangeValueForKey:@"volumeType"];
	if (typeSus) {
		theInstrument.volType |= (Byte)EFSUSTAIN;
	} else {
		theInstrument.volType &= ~(Byte)EFSUSTAIN;
	}
	[self didChangeValueForKey:@"volumeType"];
}

- (void)setVolumeTypeLoop:(BOOL)typeLoop
{
	[self willChangeValueForKey:@"volumeType"];
	if (typeLoop) {
		theInstrument.volType |= (Byte)EFLOOP;
	} else {
		theInstrument.volType &= ~(Byte)EFLOOP;
	}
	[self didChangeValueForKey:@"volumeType"];
}

- (void)setVolumeTypeNote:(BOOL)theLoop
{
	[self willChangeValueForKey:@"panningType"];
	if (theLoop) {
		theInstrument.volType |= (Byte)EFNOTE;
	} else {
		theInstrument.volType &= ~(Byte)EFNOTE;
	}
	[self didChangeValueForKey:@"panningType"];
}

- (BOOL)isVolumeTypeOn
{
	return theInstrument.volType & EFON;
}

- (BOOL)isVolumeTypeSustain
{
	return theInstrument.volType & EFSUSTAIN;
}

- (BOOL)isVolumeTypeLoop
{
	return theInstrument.volType & EFLOOP;
}

- (BOOL)isVolumeTypeNote
{
	return theInstrument.volType & EFNOTE;
}

- (Byte)panningType
{
	return theInstrument.pannType;
}

- (void)setPanningTypeOn:(BOOL)typeOn
{
	[self willChangeValueForKey:@"panningType"];
	if (typeOn) {
		theInstrument.pannType |= (Byte)EFON;
	} else {
		theInstrument.pannType &= ~(Byte)EFON;
	}
	[self didChangeValueForKey:@"panningType"];
}

- (void)setPanningTypeSustain:(BOOL)typeSus
{
	[self willChangeValueForKey:@"panningType"];
	if (typeSus) {
		theInstrument.pannType |= (Byte)EFSUSTAIN;
	} else {
		theInstrument.pannType &= ~(Byte)EFSUSTAIN;
	}
	[self didChangeValueForKey:@"panningType"];
}

- (void)setPanningTypeLoop:(BOOL)theLoop
{
	[self willChangeValueForKey:@"panningType"];
	if (theLoop) {
		theInstrument.pannType |= (Byte)EFLOOP;
	} else {
		theInstrument.pannType &= ~(Byte)EFLOOP;
	}
	[self didChangeValueForKey:@"panningType"];
}

- (void)setPanningTypeNote:(BOOL)theLoop
{
	[self willChangeValueForKey:@"panningType"];
	if (theLoop) {
		theInstrument.pannType |= (Byte)EFNOTE;
	} else {
		theInstrument.pannType &= ~(Byte)EFNOTE;
	}
	[self didChangeValueForKey:@"panningType"];
}

- (BOOL)isPanningTypeOn
{
	return theInstrument.pannType & EFON;
}

- (BOOL)isPanningTypeSustain
{
	return theInstrument.pannType & EFSUSTAIN;
}

- (BOOL)isPanningTypeLoop
{
	return theInstrument.pannType & EFLOOP;
}

- (BOOL)isPanningTypeNote
{
	return theInstrument.pannType & EFNOTE;
}

- (NSArray *)children;
{
	return [self samples];
}

- (PPSampleObject*)childAtIndex:(NSUInteger)idx
{
	return samples[idx];
}

- (NSUInteger)countOfChildren
{
	return [self countOfSamples];
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	[self writeBackToStruct];
	PPInstrumentObject *newObj = [[[self class] allocWithZone:zone] initWithMusic:_theMus instrumentIndex:theInstrument.no];
	
	
	return newObj;
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
		
		
		
		[self setUpKVO];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:name forKey:PPName];
	[aCoder encodeInteger:number forKey:PPLocation];
	[aCoder encodeInteger:self.firstSample forKey:PPSampCount];
	[aCoder encodeObject:@(theInstrument.MIDI) forKey:PPMIDI];
	[aCoder encodeObject:@(theInstrument.MIDIType) forKey:PPMIDIType];
	[aCoder encodeBytes:&theInstrument.volSize length:1 forKey:PPVolSize];
	[aCoder encodeBytes:&theInstrument.pannSize length:1 forKey:PPPannSize];
	[aCoder encodeBytes:&theInstrument.pitchSize length:1 forKey:PPPitchSize];
	[aCoder encodeBytes:theInstrument.what length:sizeof(theInstrument.what) forKey:PPNotes];
	[aCoder encodeInt:theInstrument.volFade forKey:PPVolFade];
	
	[aCoder encodeBytes:&theInstrument.volSus length:1 forKey:PPVolSus];
	[aCoder encodeBytes:&theInstrument.volBeg length:1 forKey:PPVolBeg];
	[aCoder encodeBytes:&theInstrument.volEnd length:1 forKey:PPVolEnd];
	
	[aCoder encodeBytes:&theInstrument.pannSus length:1 forKey:PPPanSus];
	[aCoder encodeBytes:&theInstrument.pannBeg length:1 forKey:PPPanBeg];
	[aCoder encodeBytes:&theInstrument.pannEnd length:1 forKey:PPPanEnd];
	
	[aCoder encodeBytes:&theInstrument.pitchSus length:1 forKey:PPPitchSus];
	[aCoder encodeBytes:&theInstrument.pitchBeg length:1 forKey:PPPitchBeg];
	[aCoder encodeBytes:&theInstrument.pitchEnd length:1 forKey:PPPitchEnd];
	
	[aCoder encodeBytes:&theInstrument.volType length:1 forKey:PPVolType];
	[aCoder encodeBytes:&theInstrument.pannType length:1 forKey:PPPannType];
	
	[aCoder encodeBytes:&theInstrument.vibDepth length:1 forKey:PPVibDepth];
	[aCoder encodeBytes:&theInstrument.vibRate length:1 forKey:PPVibRate];
}

@end
