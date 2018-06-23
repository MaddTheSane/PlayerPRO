//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#ifndef __PLAYERPROKIT_PPINSTRUMENTOBJECT_H__
#define __PLAYERPROKIT_PPINSTRUMENTOBJECT_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MAD.h>
#import <PlayerPROKit/PPObjectProtocol.h>

NS_ASSUME_NONNULL_BEGIN

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
+ (nullable instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObject *)mus;
- (nullable instancetype)initWithMusic:(PPMusicObject *)mus instrumentIndex:(short)insIdx;
- (instancetype)init;

@property (nonatomic, readonly, copy) NSArray<PPEnvelopeObject*> *volumeEnvelope;
@property (nonatomic, readonly, copy) NSArray<PPEnvelopeObject*> *panningEnvelope;
@property (nonatomic, readonly, copy) NSArray<PPEnvelopeObject*> *pitchEnvelope;
- (void)replaceObjectInPanningEnvelopeAtIndex:(NSInteger)index withObject:(PPEnvelopeObject *)object;
- (void)replaceObjectInVolumeEnvelopeAtIndex:(NSInteger)index withObject:(PPEnvelopeObject *)object;
- (void)replaceObjectInPitchEnvelopeAtIndex:(NSInteger)index withObject:(PPEnvelopeObject *)object;
- (PPEnvelopeObject *)panningEnvelopeObjectAtIndex:(NSInteger)index;
- (PPEnvelopeObject *)volumeEnvelopeObjectAtIndex:(NSInteger)index;
- (PPEnvelopeObject *)pitchEnvelopeObjectAtIndex:(NSInteger)index;
@property (readonly) NSInteger countOfVolumeEnvelope;
@property (readonly) NSInteger countOfPanningEnvelope;
@property (readonly) NSInteger countOfPitchEnvelope;

@property (readonly) MADByte* what NS_RETURNS_INNER_POINTER;

- (PPSampleObject *)objectAtIndexedSubscript:(NSInteger)index;
- (void)setObject:(PPSampleObject *)obj atIndexedSubscript:(NSInteger)index;
- (void)resetInstrument;

@property unsigned short volumeFadeOut;

@property (weak, nullable) PPMusicObject *theMus;
@property (copy, null_resettable) NSString *name;
@property (readonly) NSInteger number;
@property (nonatomic, readonly) short firstSample;
@property (nonatomic, copy, readonly) NSArray<PPSampleObject*> *samples;

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

//! Volume type.
@property (readwrite) EFType volumeType;
@property (getter = isVolumeTypeOn) BOOL volumeTypeOn;
@property (getter = isVolumeTypeSustain) BOOL volumeTypeSustain;
@property (getter = isVolumeTypeLoop) BOOL volumeTypeLoop;
@property (getter = isVolumeTypeNote) BOOL volumeTypeNote;

//! Panning type.
@property (readwrite) EFType panningType;
@property (getter = isPanningTypeOn) BOOL panningTypeOn;
@property (getter = isPanningTypeSustain) BOOL panningTypeSustain;
@property (getter = isPanningTypeLoop) BOOL panningTypeLoop;
@property (getter = isPanningTypeNote) BOOL panningTypeNote;

//! An instrument is blank if the name is empty and any samples present are considered blank.
@property (readonly, getter=isBlankInstrument) BOOL blankInstrument;

- (void)addSampleObject:(PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(PPSampleObject *)object;
@property (readonly) NSInteger countOfSamples;
- (PPSampleObject *)samplesObjectAtIndex:(NSInteger)idx;
- (void)removeSamplesAtIndexes:(NSIndexSet *)indexes;

@end

NS_ASSUME_NONNULL_END

#endif
