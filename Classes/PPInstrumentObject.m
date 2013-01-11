//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "ARCBridge.h"

@implementation PPInstrumentObject

@synthesize samples;

- (NSArray*)samples
{
	return [NSArray arrayWithArray:samples];
}

@synthesize name;
@synthesize type;

@synthesize firstSample;
@synthesize MIDIOut;
@synthesize sampleCount;
@synthesize soundOut;

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
{
	if (self = [super init]) {
		if (!mus) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		theMus = mus;
		if (insIdx > mus->header->numInstru) {
			//Create a blank Instrument info
			samples = [[NSMutableArray alloc] init];
			name = @"";
			firstSample = insIdx * MAXSAMPLE;
			sampleCount = 0;
		}else {
			InstrData *tempData = &mus->fid[insIdx];
			samples = [[NSMutableArray alloc] initWithCapacity:tempData->numSamples];
			{
				int sDataCount = tempData->numSamples + tempData->firstSample;

				int i = 0;
				for (i = tempData->firstSample; i < sDataCount; i++) {
					PPSampleObject *sObj = [[PPSampleObject alloc] initWithsData:mus->sample[i]];
					[samples addObject:sObj];
					RELEASEOBJ(sObj);
				}
			}
			name = [[NSString alloc] initWithCString:tempData->name encoding:NSMacOSRomanStringEncoding];
			type = tempData->type;
			number = tempData->no;
			MIDI = tempData->MIDI;
			switch (tempData->MIDIType) {
				case 0:
				default:
					soundOut = YES;
					MIDIOut = NO;
					break;
					
				case 1:
					soundOut = NO;
					MIDIOut = YES;
					break;
					
				case 2:
					soundOut = MIDIOut = YES;
					break;
			}
			firstSample = tempData->firstSample;
			sampleCount = tempData->numSamples;
			memcpy(what, tempData->what, sizeof(what));
			memcpy(volEnv, tempData->volEnv, sizeof(volEnv));
			memcpy(pannEnv, tempData->pannEnv, sizeof(pannEnv));
			memcpy(pitchEnv, tempData->pitchEnv, sizeof(pitchEnv));
			volumeSize = tempData->volSize;
			panningSize = tempData->pannSize;
			pitchSize = tempData->pitchSize;
			
			volumeSustain = tempData->volSus;
			volumeBegin = tempData->volBeg;
			volumeEnd = tempData->volEnd;
			
			panningSustain = tempData->pannSus;
			panningBegin = tempData->pannBeg;
			panningEnd = tempData->pannEnd;
			
			pitchSustain = tempData->pitchSus;
			pitchBegin = tempData->pitchBeg;
			pitchEnd = tempData->pitchEnd;
			
			switch (tempData->volType) {
				case 0:
				default:
					volumeType.on = 1;
					break;
					
				case 1:
					volumeType.sustain = 1;
					break;
					
				case 2:
					volumeType.loop = 1;
					break;
			}
			
			switch (tempData->pannType) {
				default:
				case 0:
					panningType.on = 1;
					break;
					
				case 1:
					panningType.sustain = 1;
					break;
					
				case 2:
					panningType.loop = 1;
					break;
					
			}
			
			
			volumeFade = tempData->volFade;
			
			vibratoDepth = tempData->vibDepth;
			vibratoRate = tempData->vibRate;
		}
	}
	return self;
}

- (void)addSamplesObject:(PPSampleObject *)object
{
	if (sampleCount >= MAXSAMPLE) {
		return;
	}
	[samples addObject:object];
	sampleCount++;
}

- (void)replaceObjectInSamplesAtIndex:(short)index withObject:(PPSampleObject *)object
{
	if (index >= MAXSAMPLE || index < 0) {
		return;
	}
	[samples replaceObjectAtIndex:index withObject:object];
}
- (NSUInteger)countOfSamples
{
	return [samples count];
}

- (BOOL)isVolumeTypeOn
{
	return volumeType.on;
}

- (BOOL)isVolumeTypeSustain
{
	return volumeType.sustain;
}

- (BOOL)isVolumeTypeLoop
{
	return volumeType.loop;
}

- (BOOL)isPanningTypeOn
{
	return panningType.on;
}

- (BOOL)isPanningTypeSustain
{
	return panningType.sustain;
}

- (BOOL)isPanningTypeLoop
{
	return panningType.loop;
}

- (NSArray *)children;
{
	return samples;
}

- (PPSampleObject*)childAtIndex:(NSUInteger)idx
{
	return [samples objectAtIndex:idx];
}

- (NSUInteger)countOfChildren
{
	return [samples count];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[samples release];
	[name release];
	
	[super dealloc];
}
#endif

@end
