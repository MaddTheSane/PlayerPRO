//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPSampleObject_PPKPrivate.h"
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

#pragma mark PlayerPROKit KVO/KVC keys
#define kPPVolumeType @"volumeType"
#define kPPPanningType @"panningType"


@interface PPEnvelopeObjectImmutable : PPEnvelopeObject

@end

@implementation PPEnvelopeObject
@synthesize envelopeRec;

- (short)position
{
	return envelopeRec.pos;
}

- (void)setPosition:(short)position
{
	envelopeRec.pos = position;
}

- (short)value
{
	return envelopeRec.val;
}

- (void)setValue:(short)value
{
	envelopeRec.val = value;
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

- (short)MIDIType
{
	return theInstrument.MIDIType;
}

- (void)setSoundOut:(BOOL)soundOut
{
	[self willChangeValueForKey:@"MIDIType"];
	if (soundOut) {
		if (self.MIDIOut) {
			theInstrument.MIDIType = 2;
		} else {
			theInstrument.MIDIType = 0;
		}
	} else {
		if (self.MIDIOut) {
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
		if (self.soundOut) {
			theInstrument.MIDIType = 2;
		} else {
			theInstrument.MIDIType = 1;
		}
	} else {
		if (self.soundOut) {
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

- (void)setNumber:(NSInteger)numberr
{
	theInstrument.no = number = numberr;
	theInstrument.firstSample = MAXSAMPLE * numberr;
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

#if 0
- (void)setUpKVO
{
	[self addObserver:self forKeyPath:kPPVolumeTypeLoop options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPVolumeTypeNote options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPVolumeTypeOn options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPVolumeTypeSustain options:NSKeyValueObservingOptionNew context:NULL];
	
	[self addObserver:self forKeyPath:kPPPanningTypeLoop options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPPanningTypeNote options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPPanningTypeOn options:NSKeyValueObservingOptionNew context:NULL];
	[self addObserver:self forKeyPath:kPPPanningTypeSustain options:NSKeyValueObservingOptionNew context:NULL];
	
}

- (void)shutDownKVO
{
	[self removeObserver:self forKeyPath:kPPVolumeTypeLoop];
	[self removeObserver:self forKeyPath:kPPVolumeTypeNote];
	[self removeObserver:self forKeyPath:kPPVolumeTypeOn];
	[self removeObserver:self forKeyPath:kPPVolumeTypeSustain];
	
	[self removeObserver:self forKeyPath:kPPPanningTypeLoop];
	[self removeObserver:self forKeyPath:kPPPanningTypeNote];
	[self removeObserver:self forKeyPath:kPPPanningTypeOn];
	[self removeObserver:self forKeyPath:kPPPanningTypeSustain];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	//FIXME: is this right?
	if ([keyPath isEqualToString:kPPVolumeTypeLoop] || [keyPath isEqualToString:kPPVolumeTypeNote] ||
		[keyPath isEqualToString:kPPVolumeTypeOn] || [keyPath isEqualToString:kPPVolumeTypeSustain]) {
		[self willChangeValueForKey:kPPVolumeType];
		[self didChangeValueForKey:kPPVolumeType];
	} else if ([keyPath isEqualToString:kPPPanningTypeLoop] || [keyPath isEqualToString:kPPPanningTypeNote] ||
			   [keyPath isEqualToString:kPPPanningTypeOn] || [keyPath isEqualToString:kPPPanningTypeSustain]) {
		[self willChangeValueForKey:kPPPanningType];
		[self didChangeValueForKey:kPPPanningType];
	}
}
#endif

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
			[_panningEnvelope addObject:[PPEnvelopeObject new]];
			[_volumeEnvelope addObject:[PPEnvelopeObject new]];
			[_pitchEnvelope addObject:[PPEnvelopeObject new]];
		}

	}
	return self;
}

- (instancetype)initWithMusicStruct:(MADMusic*)theMus atIndex:(NSInteger)ind
{
	if (self = [super init]) {
		theInstrument = theMus->fid[ind];
		samples = [[NSMutableArray alloc] initWithCapacity:theInstrument.numSamples];
		{
			int sDataCount = theInstrument.numSamples + theInstrument.firstSample;
			
			for (int i = theInstrument.firstSample; i < sDataCount; i++) {
				PPSampleObjectImmutable *sObj = [[PPSampleObjectImmutable alloc] initWithsData:theMus->sample[i]];
				sObj.sampleIndex = i % MAXSAMPLE;
				sObj.instrumentIndex = ind;
				[samples addObject:sObj];
			}
		}
		name = [[NSString alloc] initWithCString:theInstrument.name encoding:NSMacOSRomanStringEncoding];
		theInstrument.no = number = ind;
		//In case it's malformed, i.e. from CreateFreeMADK()
		theInstrument.firstSample = MAXSAMPLE * ind; /*tempData->firstSample;*/
		_panningEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_volumeEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		_pitchEnvelope = [[NSMutableArray alloc] initWithCapacity:12];
		for (int i = 0; i < 12; i++) {
			[_panningEnvelope addObject:[[PPEnvelopeObjectImmutable alloc] initWithEnvRec:theInstrument.pannEnv[i]]];
			[_volumeEnvelope addObject:[[PPEnvelopeObjectImmutable alloc] initWithEnvRec:theInstrument.volEnv[i]]];
			[_pitchEnvelope addObject:[[PPEnvelopeObjectImmutable alloc] initWithEnvRec:theInstrument.pitchEnv[i]]];
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
	[self willChangeValueForKey:kPPVolumeType];
	if (typeOn) {
		theInstrument.volType |= EFON;
	} else {
		theInstrument.volType &= ~(Byte)EFON;
	}
	[self didChangeValueForKey:kPPVolumeType];
}

- (void)setVolumeTypeSustain:(BOOL)typeSus
{
	[self willChangeValueForKey:kPPVolumeType];
	if (typeSus) {
		theInstrument.volType |= EFSUSTAIN;
	} else {
		theInstrument.volType &= ~(Byte)EFSUSTAIN;
	}
	[self didChangeValueForKey:kPPVolumeType];
}

- (void)setVolumeTypeLoop:(BOOL)typeLoop
{
	[self willChangeValueForKey:kPPVolumeType];
	if (typeLoop) {
		theInstrument.volType |= EFLOOP;
	} else {
		theInstrument.volType &= ~(Byte)EFLOOP;
	}
	[self didChangeValueForKey:kPPVolumeType];
}

- (void)setVolumeTypeNote:(BOOL)theLoop
{
	[self willChangeValueForKey:kPPVolumeType];
	if (theLoop) {
		theInstrument.volType |= EFNOTE;
	} else {
		theInstrument.volType &= ~(Byte)EFNOTE;
	}
	[self didChangeValueForKey:kPPVolumeType];
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
	[self willChangeValueForKey:kPPPanningType];
	if (typeOn) {
		theInstrument.pannType |= EFON;
	} else {
		theInstrument.pannType &= ~(Byte)EFON;
	}
	[self didChangeValueForKey:kPPPanningType];
}

- (void)setPanningTypeSustain:(BOOL)typeSus
{
	[self willChangeValueForKey:kPPPanningType];
	if (typeSus) {
		theInstrument.pannType |= EFSUSTAIN;
	} else {
		theInstrument.pannType &= ~(Byte)EFSUSTAIN;
	}
	[self didChangeValueForKey:kPPPanningType];
}

- (void)setPanningTypeLoop:(BOOL)theLoop
{
	[self willChangeValueForKey:kPPPanningType];
	if (theLoop) {
		theInstrument.pannType |= EFLOOP;
	} else {
		theInstrument.pannType &= ~(Byte)EFLOOP;
	}
	[self didChangeValueForKey:kPPPanningType];
}

- (void)setPanningTypeNote:(BOOL)theLoop
{
	[self willChangeValueForKey:kPPPanningType];
	if (theLoop) {
		theInstrument.pannType |= EFNOTE;
	} else {
		theInstrument.pannType &= ~(Byte)EFNOTE;
	}
	[self didChangeValueForKey:kPPPanningType];
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

- (PPSampleObject*)samplesObjectAtIndex:(NSUInteger)idx;
{
	return samples[idx];
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
		
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:name forKey:PPName];
	[aCoder encodeInteger:number forKey:PPLocation];
	[aCoder encodeInteger:self.firstSample forKey:PPSampCount];
	[aCoder encodeInt:theInstrument.MIDI forKey:PPMIDI];
	[aCoder encodeInt:theInstrument.MIDIType forKey:PPMIDIType];
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

@implementation PPEnvelopeObjectImmutable

- (void)setPosition:(short)position
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setValue:(short)value
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

@end