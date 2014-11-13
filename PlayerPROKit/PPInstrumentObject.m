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
#include <PlayerPROCore/RDriverInt.h>
#if !TARGET_OS_IPHONE
#import "PPPasteboardHandling.h"
#endif

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

@interface PPEnvelopeObject ()
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
@end

@implementation PPEnvelopeObject
{
@private
	EnvRec envelopeRec;
	EnvRec *writeBackVal;
}

+ (instancetype)envelopeWithEnvRec:(EnvRec)theRec
{
	return [[self alloc] initWithEnvRec:theRec];
}

- (instancetype)initWithEnvRecPointer:(EnvRec*)theEnv
{
	if (self = [self initWithEnvRec:*theEnv]) {
		writeBackVal = theEnv;
	}
	return self;
}

- (EnvRec)envelopeRec
{
	return *writeBackVal;
}

+ (NSSet*)keyPathsForValuesAffectingEnvelopeRec
{
	return [NSSet setWithObjects:@"position", @"envelopeValue", nil];
}

#if !TARGET_OS_IPHONE
NSString * const kPPKEnvelopePasteboardUTI = @"net.sourceforge.playerpro.envelope";
NSString * const kPPKInstrumentPasteboardUTI = @"net.sourceforge.playerpro.instrument";

static NSArray *UTIArray;
static NSArray *SampleUTIArray;
static dispatch_once_t initUTIOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[kPPKEnvelopePasteboardUTI];
	SampleUTIArray = @[kPPKInstrumentPasteboardUTI];
};

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}
- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:kPPKEnvelopePasteboardUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKEnvelopePasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif

- (short)position
{
	return writeBackVal->pos;
}

- (void)setPosition:(short)position
{
	writeBackVal->pos = position;
}

- (short)envelopeValue
{
	return writeBackVal->val;
}

- (void)setEnvelopeValue:(short)value
{
	writeBackVal->val = value;
}

- (instancetype)init
{
	if (self = [super init]) {
		writeBackVal = &envelopeRec;
	}
	return self;
}

- (instancetype)initWithEnvRec:(EnvRec)theRec
{
	if (self = [super init]) {
		envelopeRec = theRec;
		writeBackVal = &envelopeRec;
	}
	
	return self;
}

+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		writeBackVal = &envelopeRec;
		writeBackVal->pos = [aDecoder decodeIntForKey:PPEnvPos];
		writeBackVal->val = [aDecoder decodeIntForKey:PPEnvVal];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeInt:writeBackVal->pos forKey:PPEnvPos];
	[aCoder encodeInt:writeBackVal->val forKey:PPEnvVal];
}

- (instancetype)copyWithZone:(NSZone *)zone
{
	return [[[self class] alloc] initWithEnvRec:envelopeRec];
}

@end

@interface PPInstrumentObject ()
@property (nonatomic, copy) NSMutableArray *internalVolumeEnvelope;
@property (nonatomic, copy) NSMutableArray *internalPanningEnvelope;
@property (nonatomic, copy) NSMutableArray *internalPitchEnvelope;
@end

@implementation PPInstrumentObject
{
	@private
	InstrData theInstrument;
	NSInteger number;			// Instrument number
}
@synthesize theInstrument;
@synthesize number;
@synthesize name;
@synthesize internalVolumeEnvelope = _volumeEnvelope;
@synthesize internalPanningEnvelope = _panningEnvelope;
@synthesize internalPitchEnvelope = _pitchEnvelope;

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

- (PPEnvelopeObject*)panningEnvelopeObjectAtIndex:(NSInteger)index
{
	return _panningEnvelope[index];
}

- (PPEnvelopeObject*)volumeEnvelopeObjectAtIndex:(NSInteger)index
{
	return _volumeEnvelope[index];
}

- (PPEnvelopeObject*)pitchEnvelopeObjectAtIndex:(NSInteger)index
{
	return _pitchEnvelope[index];
}

- (NSInteger)countOfVolumeEnvelope { return 12; }
- (NSInteger)countOfPanningEnvelope { return 12; }
- (NSInteger)countOfPitchEnvelope { return 12; }

- (instancetype)init
{
	if (self = [super init]) {
		MADResetInstrument(&theInstrument);
	}
	return self;
}

- (NSString*)name
{
	if (!name) {
		name = [[NSString alloc] initWithCString:theInstrument.name encoding:NSMacOSRomanStringEncoding];
		if (!name) {
			name = @"";
			memset(theInstrument.name, 0, sizeof(theInstrument.name));
		}
	}
	return name;
}

- (void)setName:(NSString *)name1
{
	char tempstr[32] = {0};
	NSData *tmpCStr = [name1 dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(tempstr) - 1) {
		cStrLen = sizeof(tempstr) - 1;
	}
	[tmpCStr getBytes:tempstr length:cStrLen];
	tmpCStr = nil;
	
	strlcpy(theInstrument.name, tempstr, sizeof(theInstrument.name));
	name = [[NSString alloc] initWithCString:theInstrument.name encoding:NSMacOSRomanStringEncoding];
}

- (PPSampleObject *)objectAtIndexedSubscript:(NSInteger)index
{
	return samples[index];
}

#if !TARGET_OS_IPHONE
+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return SampleUTIArray;
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return SampleUTIArray;
}
- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:kPPKInstrumentPasteboardUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKInstrumentPasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif

- (MADByte*)what
{
	return theInstrument.what;
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

- (void)replaceObjectInVolumeEnvelopeAtIndex:(NSInteger)index withObject:(id)object;
{
	_volumeEnvelope[index] = object;
	theInstrument.volEnv[index] = [_volumeEnvelope[index] envelopeRec];
}

- (void)replaceObjectInPanningEnvelopeAtIndex:(NSInteger)index withObject:(id)object
{
	_panningEnvelope[index] = object;
	theInstrument.pannEnv[index] = [_panningEnvelope[index] envelopeRec];
}

- (void)replaceObjectInPitchEnvelopeAtIndex:(NSInteger)index withObject:(id)object
{
	_pitchEnvelope[index] = object;
	theInstrument.pitchEnv[index] = [_pitchEnvelope[index] envelopeRec];
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

- (void)setSamples:(NSArray *)newSamples
{
	samples = [newSamples mutableCopy];
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

+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObject*)mus
{
	PPInstrumentObject *insObj = [[self alloc] initWithMusic:mus];
	[mus addInstrument:insObj];
	
	return insObj;
}

- (instancetype)initWithMusic:(PPMusicObject*)mus;
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
		self.name = @"";
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
				PPSampleObject *sObj = [[PPSampleObject alloc] initWithSData:theMus->sample[i]];
				sObj.sampleIndex = i % MAXSAMPLE;
				sObj.instrumentIndex = ind;
				[samples addObject:sObj];
			}
		}
		name = [[NSString alloc] initWithCString:theInstrument.name encoding:NSMacOSRomanStringEncoding];
		theInstrument.no = number = ind;
		// In case it's malformed, i.e. from CreateFreeMADK()
		theInstrument.firstSample = MAXSAMPLE * ind; /*tempData->firstSample;*/
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

- (instancetype)initWithMusic:(PPMusicObject*)mus instrumentIndex:(short)insIdx;
{
	if (self = [self initWithMusic:mus]) {
		theInstrument = mus._currentMusic->fid[insIdx];
		samples = [[NSMutableArray alloc] initWithCapacity:theInstrument.numSamples];
		{
			int sDataCount = theInstrument.numSamples + theInstrument.firstSample;
			
			for (int i = theInstrument.firstSample; i < sDataCount; i++) {
				PPSampleObject *sObj = [[PPSampleObject alloc] initWithSData:mus._currentMusic->sample[i]];
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

- (void)addSampleObject:(PPSampleObject *)object
{
	[self addSamplesObject:object];
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

- (NSInteger)countOfSamples
{
	return [samples count];
}

- (EFType)volumeType
{
	return theInstrument.volType;
}

- (void)setVolumeType:(EFType)volumeType
{
	theInstrument.volType = volumeType;
}

- (void)setVolumeTypeOn:(BOOL)typeOn
{
	if (typeOn) {
		theInstrument.volType |= EFTypeOn;
	} else {
		theInstrument.volType &= ~EFTypeOn;
	}
}

- (void)setVolumeTypeSustain:(BOOL)typeSus
{
	if (typeSus) {
		theInstrument.volType |= EFTypeSustain;
	} else {
		theInstrument.volType &= ~EFTypeSustain;
	}
}

- (void)setVolumeTypeLoop:(BOOL)typeLoop
{
	if (typeLoop) {
		theInstrument.volType |= EFTypeLoop;
	} else {
		theInstrument.volType &= ~EFTypeLoop;
	}
}

- (void)setVolumeTypeNote:(BOOL)theLoop
{
	if (theLoop) {
		theInstrument.volType |= EFTypeNote;
	} else {
		theInstrument.volType &= ~EFTypeNote;
	}
}

- (BOOL)isVolumeTypeOn
{
	return theInstrument.volType & EFTypeOn;
}

- (BOOL)isVolumeTypeSustain
{
	return theInstrument.volType & EFTypeSustain;
}

- (BOOL)isVolumeTypeLoop
{
	return theInstrument.volType & EFTypeLoop;
}

- (BOOL)isVolumeTypeNote
{
	return theInstrument.volType & EFTypeNote;
}

- (EFType)panningType
{
	return theInstrument.pannType;
}

- (void)setPanningType:(EFType)panningType
{
	theInstrument.pannType = panningType;
}

- (void)setPanningTypeOn:(BOOL)typeOn
{
	if (typeOn) {
		theInstrument.pannType |= EFTypeOn;
	} else {
		theInstrument.pannType &= ~EFTypeOn;
	}
}

- (void)setPanningTypeSustain:(BOOL)typeSus
{
	if (typeSus) {
		theInstrument.pannType |= EFTypeSustain;
	} else {
		theInstrument.pannType &= ~EFTypeSustain;
	}
}

- (void)setPanningTypeLoop:(BOOL)theLoop
{
	if (theLoop) {
		theInstrument.pannType |= EFTypeLoop;
	} else {
		theInstrument.pannType &= ~EFTypeLoop;
	}
}

- (void)setPanningTypeNote:(BOOL)theLoop
{
	if (theLoop) {
		theInstrument.pannType |= EFTypeNote;
	} else {
		theInstrument.pannType &= ~EFTypeNote;
	}
}

- (BOOL)isPanningTypeOn
{
	return theInstrument.pannType & EFTypeOn;
}

- (BOOL)isPanningTypeSustain
{
	return theInstrument.pannType & EFTypeSustain;
}

- (BOOL)isPanningTypeLoop
{
	return theInstrument.pannType & EFTypeLoop;
}

- (BOOL)isPanningTypeNote
{
	return theInstrument.pannType & EFTypeNote;
}

- (PPSampleObject*)samplesObjectAtIndex:(NSInteger)idx
{
	return samples[idx];
}

- (unsigned short)volumeFadeOut
{
	return theInstrument.volFade;
}

- (void)setVolumeFadeOut:(unsigned short)volumeFadeOut
{
	theInstrument.volFade = volumeFadeOut;
}

- (void)removeSamplesAtIndexes:(NSIndexSet *)indexes
{
	[samples removeObjectsAtIndexes:indexes];
}

- (void)resetInstrument
{
	[self removeSamplesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, samples.count)]];
	theInstrument.no = number = -1;
	theInstrument.firstSample = 0;
	MADResetInstrument(&theInstrument);
	self.name = @"";
	for (int i = 0; i < 12; i++) {
		[self replaceObjectInPanningEnvelopeAtIndex:i withObject:[PPEnvelopeObject new]];
		[self replaceObjectInVolumeEnvelopeAtIndex:i withObject:[PPEnvelopeObject new]];
		[self replaceObjectInPitchEnvelopeAtIndex:i withObject:[PPEnvelopeObject new]];
	}
}

- (BOOL)isBlankInstrument
{
	if (self.name && ![self.name isEqualToString:@""]) {
		return NO;
	}
	
	for (PPSampleObject *samp in samples) {
		if (!samp.isBlankSample) {
			return NO;
		}
	}
	
	return YES;
}

#pragma mark KVO/KVC helpers
+ (NSSet*)keyPathsForValuesAffectingBlankInstrument
{
	return [NSSet setWithObjects:@"samples", @"name", nil];
}

+ (NSSet*)keyPathsForValuesAffectingPanningType
{
	return [NSSet setWithObjects:@"panningTypeOn", @"panningTypeSustain", @"panningTypeLoop", @"panningTypeNote", nil];
}

#define affectPannType(theType) \
+ (NSSet*)keyPathsForValuesAffectingPanningType ## theType \
{ \
return [NSSet setWithObject:@"panningType"];\
}

affectPannType(On)
affectPannType(Sustain)
affectPannType(Loop)
affectPannType(Note)
#undef affectPannType

+ (NSSet*)keyPathsForValuesAffectingVolumeType
{
	return [NSSet setWithObjects:@"volumeTypeOn", @"volumeTypeSustain", @"volumeTypeLoop", @"volumeTypeNote", nil];
}

#define affectVolType(theType) \
+ (NSSet*)keyPathsForValuesAffectingVolumeType ## theType \
{ \
	return [NSSet setWithObject:@"volumeType"];\
}

affectVolType(On)
affectVolType(Sustain)
affectVolType(Loop)
affectVolType(Note)
#undef affectVolType

+ (NSSet*)keyPathsForValuesAffectingMIDIType
{
	return [NSSet setWithObjects:@"soundOut", @"MIDIOut", nil];
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	//[self writeBackToStruct];
	PPInstrumentObject *newObj = [[[self class] allocWithZone:zone] initWithMusic:_theMus instrumentIndex:theInstrument.no];
	newObj.name = self.name;
	
	return newObj;
}

#pragma mark NSFastEnumeration protocol

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [samples countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark NSCoding protocol

+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:self.name forKey:PPName];
	[aCoder encodeInteger:number forKey:PPLocation];
	[aCoder encodeInteger:self.firstSample forKey:PPSampCount];
	[aCoder encodeInt:theInstrument.MIDI forKey:PPMIDI];
	[aCoder encodeInt:theInstrument.MIDIType forKey:PPMIDIType];
	[aCoder encodeBytes:&theInstrument.volSize length:1 forKey:PPVolSize];
	[aCoder encodeBytes:&theInstrument.pannSize length:1 forKey:PPPannSize];
	[aCoder encodeBytes:&theInstrument.pitchSize length:1 forKey:PPPitchSize];
	[aCoder encodeBytes:self.what length:sizeof(theInstrument.what) forKey:PPNotes];
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
