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
@class PPMusicObject;

@interface PPEnvelopeObject : NSObject <PPObject>
@property (readonly) EnvRec envelopeRec;
@property short position;
@property short value;
- (instancetype)initWithEnvRec:(EnvRec)theRec;
@end

@interface PPInstrumentObject : NSObject <NSFastEnumeration, PPObject>
+ (instancetype)newInstrumentObjectByAddingToMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusic:(PPMusicObject *)mus instrumentIndex:(short)insIdx;

- (NSArray*)volumeEnvelope;
- (NSArray*)panningEnvelope;
- (NSArray*)pitchEnvelope;

@property (weak) PPMusicObject *theMus;
@property (copy) NSString *name;
@property (readonly) NSInteger number;
- (short)firstSample;
- (NSArray*)samples;

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

@property (readonly) EFType volumeType;
@property (getter = isVolumeTypeOn) BOOL volumeTypeOn;
@property (getter = isVolumeTypeSustain) BOOL volumeTypeSustain;
@property (getter = isVolumeTypeLoop) BOOL volumeTypeLoop;
@property (getter = isVolumeTypeNote) BOOL volumeTypeNote;

@property (readonly) EFType panningType;
@property (getter = isPanningTypeOn) BOOL panningTypeOn;
@property (getter = isPanningTypeSustain) BOOL panningTypeSustain;
@property (getter = isPanningTypeLoop) BOOL panningTypeLoop;
@property (getter = isPanningTypeNote) BOOL panningTypeNote;

- (void)addSamplesObject:(PPSampleObject *)object;
- (void)replaceObjectInSamplesAtIndex:(NSInteger)index withObject:(PPSampleObject *)object;
- (NSUInteger)countOfSamples;
- (PPSampleObject*)samplesObjectAtIndex:(NSUInteger)idx;

@end
