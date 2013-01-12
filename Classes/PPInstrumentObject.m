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

- (short)sampleCount
{
	return [samples count];
}

@synthesize soundOut;

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
{
	if (self = [super init]) {
		if (!mus) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		theMus = mus;
		if (insIdx >= mus->header->numChn) {
			//Create a blank Instrument info
			samples = [[NSMutableArray alloc] init];
			name = @"";
			firstSample = insIdx * MAXSAMPLE;
			//sampleCount = 0;
			soundOut = YES;
			number = insIdx;
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
			//sampleCount = tempData->numSamples;
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

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: Sample index %d count %d samples: %@", name, firstSample, self.sampleCount, [samples description]];
}

typedef enum {
	PPInstruCanDoNothing = 0,
	PPInstruCanDoSoundOut = 1,
	PPInstruCanDoMidiOut = 2,
	PPInstruCanDoBoth = PPInstruCanDoSoundOut | PPInstruCanDoMidiOut
}PPInstrumentOut;

- (void)writeBackToMusic
{
	int i;
	int totalSamples = self.sampleCount + firstSample;
	int totalPossibleSamples = firstSample + MAXSAMPLE;
	for (i = firstSample; i < totalPossibleSamples; i++) {
		if (theMus->sample[i]) {
			//We can probably call the free of the data without the check
			//but better safe than sorry
			if (theMus->sample[i]->data) {
				free(theMus->sample[i]->data);
			}
			free(theMus->sample[i]);
			theMus->sample[i] = NULL;
		}
	}
	
	for (i = firstSample; i < totalSamples; i++) {
		PPSampleObject *sampObj = [samples objectAtIndex:i];
		theMus->sample[i] = [sampObj createSData];
	}
	InstrData *newData = &theMus->fid[number];
	char tempstr[32] = {0};
	
	strlcpy(tempstr, [name cStringUsingEncoding:NSMacOSRomanStringEncoding], sizeof(tempstr));
	
	memcpy(newData->name, tempstr, sizeof(newData->name));
		
	newData->numSamples = self.sampleCount;
	newData->MIDI = MIDI;
	PPInstrumentOut insOut = PPInstruCanDoNothing;
	if (soundOut) {
		insOut = PPInstruCanDoSoundOut;
	}
	if (MIDIOut) {
		insOut |= PPInstruCanDoMidiOut;
	}
	newData->MIDIType = insOut - 1;
	
	newData->pannBeg = panningBegin;
	newData->pannEnd = panningEnd;
	newData->pannSize = panningSize;
	
	newData->pitchBeg = pitchBegin;
	newData->pitchSize = pitchSize;
	newData->pitchEnd = pitchEnd;
	
	newData->volBeg = volumeBegin;
	newData->volEnd = volumeEnd;
	newData->volSize = volumeSize;
	newData->volFade = volumeFade;
	
	memcpy(newData->what, what,sizeof(what));
	if (volumeType.on) {
		newData->volType = 0;
	} else if (volumeType.loop) {
		newData->volType = 2;
	} else {
		newData->volType = 1;
	}
	//newData->volType = volumeType;
	
	if (panningType.on) {
		newData->pannType = 0;
	} else if(panningType.loop) {
		newData->pannType = 2;
	} else {
		newData->pannType = 1;
	}
	
	memcpy(newData->pannEnv, pannEnv, sizeof(pannEnv));
	memcpy(newData->pitchEnv, pitchEnv, sizeof(pitchEnv));
	memcpy(newData->volEnv, volEnv, sizeof(volEnv));
	
	theMus->hasChanged = TRUE;
}

- (void)addSamplesObject:(PPSampleObject *)object
{
	if (self.sampleCount >= MAXSAMPLE) {
		return;
	}
	[samples addObject:object];
	//sampleCount++;
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
