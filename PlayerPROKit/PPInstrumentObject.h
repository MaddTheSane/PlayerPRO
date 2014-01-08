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
@class PPMusicObjectWrapper;

@interface PPInstrumentObject : NSObject <NSCopying>
{
	NSMutableArray	*samples;
	NSString		*name;				// instrument name
	NSInteger		number;				// Instrument number
}

- (id)initWithMusic:(PPMusicObjectWrapper*)mus instrumentIndex:(short)insIdx;

@property (weak) PPMusicObjectWrapper *theMus;
@property (copy) NSString *name;
@property (readonly) NSArray *samples;
@property (readonly) NSInteger number;
@property (readonly) short sampleCount;
@property (readonly) short firstSample;

@property short MIDI;
@property (getter = isSoundOut) BOOL soundOut;
@property (getter = isMIDIOut) BOOL MIDIOut;

@property Byte volumeSize;
@property Byte panningSize;
@property Byte pitchSize;

@property Byte volumeSustain;
@property Byte volumeBegin;
@property Byte volumeEnd;

@property Byte panningSustain;
@property Byte panningBegin;
@property Byte panningEnd;

@property Byte pitchSustain;
@property Byte pitchBegin;
@property Byte pitchEnd;

@property Byte vibratoDepth;
@property Byte vibratoRate;

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
