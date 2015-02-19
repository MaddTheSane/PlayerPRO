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

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@class PPSampleObject;
@class PPMusicObject;

@interface PPEnvelopeObject : NSObject <PPObject>
@property (readonly) EnvRec envelopeRec;
@property short position;
@property short envelopeValue;
- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithEnvRec:(EnvRec)theRec NS_DESIGNATED_INITIALIZER;
+ (instancetype)envelopeWithEnvRec:(EnvRec)theRec;
@end

@interface PPInstrumentObject : NSObject <NSFastEnumeration, PPObject>
+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusic:(PPMusicObject *)mus instrumentIndex:(short)insIdx;
- (instancetype)init;

@property (nonatomic, readonly, copy, nonnull) NSArray *volumeEnvelope;
@property (nonatomic, readonly, copy, nonnull) NSArray *panningEnvelope;
@property (nonatomic, readonly, copy, nonnull) NSArray *pitchEnvelope;
- (void)replaceObjectInPanningEnvelopeAtIndex:(NSInteger)index withObject:(__nonnull PPEnvelopeObject*)object;
- (void)replaceObjectInVolumeEnvelopeAtIndex:(NSInteger)index withObject:(__nonnull PPEnvelopeObject*)object;
- (void)replaceObjectInPitchEnvelopeAtIndex:(NSInteger)index withObject:(__nonnull PPEnvelopeObject*)object;
- (__nonnull PPEnvelopeObject*)panningEnvelopeObjectAtIndex:(NSInteger)index;
- (__nonnull PPEnvelopeObject*)volumeEnvelopeObjectAtIndex:(NSInteger)index;
- (__nonnull PPEnvelopeObject*)pitchEnvelopeObjectAtIndex:(NSInteger)index;
@property (readonly) NSInteger countOfVolumeEnvelope;
@property (readonly) NSInteger countOfPanningEnvelope;
@property (readonly) NSInteger countOfPitchEnvelope;

@property (readonly, nonnull) MADByte* what NS_RETURNS_INNER_POINTER;

- (__nonnull PPSampleObject *)objectAtIndexedSubscript:(NSInteger)index;
- (void)setObject:(__nonnull PPSampleObject *)obj atIndexedSubscript:(NSInteger)index;
- (void)resetInstrument;

@property unsigned short volumeFadeOut;

@property (weak, nullable) PPMusicObject *theMus;
@property (copy, null_resettable) NSString *name;
@property (readonly) NSInteger number;
@property (nonatomic, readonly) short firstSample;
@property (nonatomic, copy, readonly, nonnull) NSArray *samples;

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

@property (readwrite) EFType volumeType;
@property (getter = isVolumeTypeOn) BOOL volumeTypeOn;
@property (getter = isVolumeTypeSustain) BOOL volumeTypeSustain;
@property (getter = isVolumeTypeLoop) BOOL volumeTypeLoop;
@property (getter = isVolumeTypeNote) BOOL volumeTypeNote;

@property (readwrite) EFType panningType;
@property (getter = isPanningTypeOn) BOOL panningTypeOn;
@property (getter = isPanningTypeSustain) BOOL panningTypeSustain;
@property (getter = isPanningTypeLoop) BOOL panningTypeLoop;
@property (getter = isPanningTypeNote) BOOL panningTypeNote;

@property (readonly, getter=isBlankInstrument) BOOL blankInstrument;

- (void)addSampleObject:(__nonnull PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(__nonnull PPSampleObject *)object;
@property (readonly) NSInteger countOfSamples;
- (__nonnull PPSampleObject*)samplesObjectAtIndex:(NSInteger)idx;
- (void)removeSamplesAtIndexes:(__nonnull NSIndexSet *)indexes;

@end
