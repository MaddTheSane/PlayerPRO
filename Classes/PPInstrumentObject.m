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

@synthesize soundOut;
@synthesize MIDIOut;

//@synthesize sampleCount;
- (short)sampleCount
{
	return [samples count];
}

@synthesize panningBegin;
@synthesize panningEnd;
@synthesize panningSize;
@synthesize panningSustain;
@synthesize pitchBegin;
@synthesize pitchEnd;
@synthesize pitchSize;
@synthesize pitchSustain;
@synthesize volumeBegin;
@synthesize volumeEnd;
@synthesize volumeSize;
@synthesize volumeSustain;
@synthesize number;

- (id)initWithMusic:(MADMusic*)mus instrumentIndex:(short)insIdx;
{
	if (self = [super init]) {
		if (!mus) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		theMus = mus;
		
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
		number = insIdx;/*tempData->no;*/
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
		//In case it's malformed, i.e. from CreateFreeMADK()
		firstSample = MAXSAMPLE * insIdx; /*tempData->firstSample;*/
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
		
		volumeType = tempData->volType;
		
		panningType = tempData->pannType;
				
		volumeFade = tempData->volFade;
		
		vibratoDepth = tempData->vibDepth;
		vibratoRate = tempData->vibRate;
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

- (void)writeSampleAtIndexBackToMusic:(short)idx
{
	if (idx >= MAXSAMPLE) {
		return;
	}
	
	if (theMus->sample[idx + firstSample]) {
		if (theMus->sample[idx + firstSample]->data)
		{
			free(theMus->sample[idx + firstSample]->data);
		}
		free(theMus->sample[idx + firstSample]);
	}
	
	theMus->sample[idx + firstSample] = [[samples objectAtIndex:idx] createSData];
}

- (void)writeBackToMusic
{
	int i, ii;
	int totalSamples = self.sampleCount + firstSample;
	int totalPossibleSamples = firstSample + MAXSAMPLE - 1;
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
	
	for (i = firstSample, ii = 0; i < totalSamples; i++, ii++) {
		PPSampleObject *sampObj = [samples objectAtIndex:ii];
		theMus->sample[i] = [sampObj createSData];
	}
	InstrData *newData = &theMus->fid[number];
	char tempstr[32] = {0};
	
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(tempstr) - 1) {
		cStrLen = sizeof(tempstr) - 1;
	}
	[tmpCStr getBytes:tempstr length:cStrLen];
	tmpCStr = nil;
	
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
	newData->volType = volumeType;
	
	newData->pannType = panningType;
	
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
	theMus->hasChanged = TRUE;
}

- (void)replaceObjectInSamplesAtIndex:(short)index withObject:(PPSampleObject *)object
{
	if (index >= MAXSAMPLE || index < 0) {
		return;
	}
	[samples replaceObjectAtIndex:index withObject:object];
	theMus->hasChanged = TRUE;
}

- (NSUInteger)countOfSamples
{
	return [samples count];
}

- (void)setVolumeTypeOn
{
	volumeType = 0;
}

- (void)setVolumeTypeSustain
{
	volumeType = 1;
}

- (void)setVolumeTypeLoop
{
	volumeType = 2;
}

- (BOOL)isVolumeTypeOn
{
	return volumeType == 0;
}

- (BOOL)isVolumeTypeSustain
{
	return volumeType == 1;
}

- (BOOL)isVolumeTypeLoop
{
	return volumeType == 2;
}

- (void)setPanningTypeOn
{
	panningType = 0;
}

- (void)setPanningTypeSustain
{
	panningType = 1;
}

- (void)setPanningTypeLoop
{
	panningType = 2;
}

- (BOOL)isPanningTypeOn
{
	return panningType == 0;
}

- (BOOL)isPanningTypeSustain
{
	return panningType == 1;
}

- (BOOL)isPanningTypeLoop
{
	return panningType == 2;
}

- (NSArray *)children;
{
	return [NSArray arrayWithArray:samples];
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
