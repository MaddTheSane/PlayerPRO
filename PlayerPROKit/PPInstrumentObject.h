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
@property short value;
- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithEnvRec:(EnvRec)theRec NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
@end

@interface PPInstrumentObject : NSObject <NSFastEnumeration, PPObject>
+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusic:(PPMusicObject *)mus instrumentIndex:(short)insIdx;

@property (nonatomic, copy) NSMutableArray *volumeEnvelope;
@property (nonatomic, copy) NSMutableArray *panningEnvelope;
@property (nonatomic, copy) NSMutableArray *pitchEnvelope;

- (MADByte*)what NS_RETURNS_INNER_POINTER;

- (PPEnvelopeObject *)objectAtIndexedSubscript:(NSInteger)index;
- (void)resetInstrument;

@property unsigned short volumeFadeOut;

@property (weak) PPMusicObject *theMus;
@property (copy) NSString *name;
@property (readonly) NSInteger number;
@property (nonatomic, readonly) short firstSample;
@property (nonatomic, readonly, copy) NSArray *samples;

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

- (void)addSamplesObject:(PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(PPSampleObject *)object;
@property (readonly) NSInteger countOfSamples;
- (PPSampleObject*)samplesObjectAtIndex:(NSInteger)idx;

@end
