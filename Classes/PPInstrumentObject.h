//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPSampleObject;

@interface PPInstrumentObject : NSObject
{
	MADMusic		*theMus;
	InstrData		theInstrument;
	
	NSMutableArray	*samples;
	NSString		*name;				// instrument name
	NSInteger		number;				// Instrument number
}

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
- (void)writeBackToMusic;
- (void)writeSampleAtIndexBackToMusic:(short)idx;

@property (readwrite, copy) NSString *name;
@property (readonly) NSArray *samples;
@property (readonly) NSInteger number;
@property (readonly) short sampleCount;
@property (readonly) short firstSample;

@property (readwrite) short MIDI;
@property (readwrite, getter = isSoundOut) BOOL soundOut;
@property (readwrite, getter = isMIDIOut) BOOL MIDIOut;

@property (readwrite) Byte volumeSize;
@property (readwrite) Byte panningSize;
@property (readwrite) Byte pitchSize;

@property (readwrite) Byte volumeSustain;
@property (readwrite) Byte volumeBegin;
@property (readwrite) Byte volumeEnd;

@property (readwrite) Byte panningSustain;
@property (readwrite) Byte panningBegin;
@property (readwrite) Byte panningEnd;

@property (readwrite) Byte pitchSustain;
@property (readwrite) Byte pitchBegin;
@property (readwrite) Byte pitchEnd;

@property (readwrite) Byte vibratoDepth;
@property (readwrite) Byte vibratoRate;

- (BOOL)isVolumeTypeOn;
- (void)setVolumeTypeOn;
- (BOOL)isVolumeTypeSustain;
- (void)setVolumeTypeSustain;
- (BOOL)isVolumeTypeLoop;
- (void)setVolumeTypeLoop;

- (BOOL)isPanningTypeOn;
- (void)setPanningTypeOn;
- (BOOL)isPanningTypeSustain;
- (void)setPanningTypeSustain;
- (BOOL)isPanningTypeLoop;
- (void)setPanningTypeLoop;

- (void)addSamplesObject:(PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(short)index withObject:(PPSampleObject *)object;
- (NSUInteger)countOfSamples;

- (NSArray *)children;
- (PPSampleObject*)childAtIndex:(NSUInteger)idx;
- (NSUInteger)countOfChildren;

@end
