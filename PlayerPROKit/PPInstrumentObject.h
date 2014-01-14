//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@class PPSampleObject;
@class PPMusicObjectWrapper;

@interface PPEnvelopeObject : NSObject <PPObject>
@property (readonly) EnvRec envelopeRec;
@property short position;
@property short value;
- (instancetype)initWithEnvRec:(EnvRec)theRec;
@end

@interface PPInstrumentObject : NSObject <NSFastEnumeration, PPObject>
{
	NSInteger		number;				// Instrument number
}
+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObjectWrapper*)mus;
- (instancetype)initWithMusic:(PPMusicObjectWrapper*)mus instrumentIndex:(short)insIdx;

@property (weak) PPMusicObjectWrapper *theMus;
@property (copy) NSString *name;
@property (readonly) NSArray *samples;
@property (readonly) NSInteger number;
@property (readonly) short sampleCount;
@property (readonly) short firstSample;

@property short MIDI;
@property (readonly) short MIDIType;
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

@property (readonly) Byte volumeType;
@property (getter = isVolumeTypeOn) BOOL volumeTypeOn;
@property (getter = isVolumeTypeSustain) BOOL volumeTypeSustain;
@property (getter = isVolumeTypeLoop) BOOL volumeTypeLoop;
@property (getter = isVolumeTypeNote) BOOL volumeTypeNote;

@property (readonly) Byte panningType;
@property (getter = isPanningTypeOn) BOOL panningTypeOn;
@property (getter = isPanningTypeSustain) BOOL panningTypeSustain;
@property (getter = isPanningTypeLoop) BOOL panningTypeLoop;
@property (getter = isPanningTypeNote) BOOL panningTypeNote;

- (void)addSamplesObject:(PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(PPSampleObject *)object;
- (NSUInteger)countOfSamples;

- (NSArray *)children DEPRECATED_ATTRIBUTE;
- (PPSampleObject*)childAtIndex:(NSUInteger)idx;
- (NSUInteger)countOfChildren DEPRECATED_ATTRIBUTE;

@end
