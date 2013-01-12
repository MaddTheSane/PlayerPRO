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

typedef struct
{
	unsigned short on:1;
	unsigned short sustain:1;
	unsigned short loop:1;
	unsigned short reserved:5;
}insType;

@interface PPInstrumentObject : NSObject
{
	MADMusic		*theMus;
	
	NSMutableArray	*samples;
	NSString		*name;				// instrument name
	Byte			type;				// Instrument type = 0
	NSInteger		number;				// Instrument number
		
	short			MIDI;				// MIDI Channel Value
	
	BOOL			soundOut;
	BOOL			MIDIOut;
	
	/**/
	
	short			firstSample;		// First sample ID in sample list
	//short			sampleCount;		// Number of samples in instrument
	
	Byte			what[ 96];			// Sample number for all notes
	EnvRec			volEnv[ 12];		// Points for volume envelope
	EnvRec			pannEnv[ 12];		// Points for panning envelope
	EnvRec			pitchEnv[ 12];		// Points for panning envelope
	
	NSInteger		volumeSize;			// Number of volume points
	NSInteger		panningSize;		// Number of panning points
	NSInteger		pitchSize;			// Number of panning points
	
	NSInteger		volumeSustain;		// Volume sustain point
	NSInteger		volumeBegin;		// Volume loop start point
	NSInteger		volumeEnd;			// Volume loop end point
	
	NSInteger		panningSustain;		// Panning sustain point
	NSInteger		panningBegin;		// Panning loop start point
	NSInteger		panningEnd;			// Panning loop end point
	
	NSInteger		pitchSustain;		// Pitch sustain point
	NSInteger		pitchBegin;			// Pitch loop start point
	NSInteger		pitchEnd;			// Pitch loop end point
	
	insType			volumeType;			// Volume type: bit 0: On; 1: Sustain; 2: Loop
	insType			panningType;		// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short	volumeFade;			// Volume fadeout
	
	NSInteger		vibratoDepth;
	NSInteger		vibratoRate;
}

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
- (void)writeBackToMusic;

@property (readwrite, retain) NSString *name;
@property (readonly) NSArray *samples;
@property (readwrite) Byte type;

@property (readwrite, getter = isSoundOut) BOOL soundOut;
@property (readwrite, getter = isMIDIOut) BOOL MIDIOut;
@property (readonly) short sampleCount;
@property (readonly) short firstSample;

@property (readwrite) NSInteger volumeSize;
@property (readwrite) NSInteger panningSize;
@property (readwrite) NSInteger pitchSize;

@property (readwrite) NSInteger volumeSustain;
@property (readwrite) NSInteger volumeBegin;
@property (readwrite) NSInteger volumeEnd;

@property (readwrite) NSInteger panningSustain;
@property (readwrite) NSInteger panningBegin;
@property (readwrite) NSInteger panningEnd;

@property (readwrite) NSInteger pitchSustain;
@property (readwrite) NSInteger pitchBegin;
@property (readwrite) NSInteger pitchEnd;

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
