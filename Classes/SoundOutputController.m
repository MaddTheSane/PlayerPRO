//
//  SoundOutputController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SoundOutputController.h"
#import "UserDefaultKeys.h"
#import "RDriver.h"

@implementation SoundOutputController

enum {
	rate11Khz,
	rate22Khz,
	rate44Khz,
	rate48Khz
};

enum {
	bits8,
	bits16,
	bits20,
	bits24
};

typedef struct _tagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;

static const TagCoupling StereoDelayCoupling[] = {{1, 1}, {10, 2}, {25, 3}, {30, 4} ,{35, 5},
	{40, 6}, {45, 7}, {50, 8}, {60, 9}, {70, 10}, {80, 11}, {90, 12}, {100, 13}, {200, 14}};

static const TagCoupling ReverbAmountCoupling[] = {{25, 1}, {30, 2}, {35, 3}, {40, 4}, {45, 5},
	{50, 6}, {60, 7}, {70, 8}, {80, 9}, {90, 10}, {100, 11}, {200, 12}, {300, 13}, {400, 14}};

static const TagCoupling ReverbPercentCoupling[] = {{10, 1}, {20, 2}, {30, 3}, {40, 4},
	{50, 5}, {60, 6}, {70, 7}};

static const TagCoupling OversamplingCoupling[] = {{1, 1}, {2, 2}, {3, 3}, {4, 4},
	{6, 5}, {8, 6}, {10, 7}, {16, 8}, {20, 9}, {25, 10}, {30, 11}};

-(NSInteger)currentRate
{
	id curSelected = [rate selectedCell];
	if ([rate cellAtRow:0 column:0] == curSelected) {
		return rate11Khz;
	} else if ([rate cellAtRow:0 column:1] == curSelected) {
		return rate22Khz;
	} else if ([rate cellAtRow:0 column:2] == curSelected) {
		return rate44Khz;
	} else if ([rate cellAtRow:0 column:3] == curSelected) {
		return rate48Khz;
	} else {
		return -1;
	}
}

-(NSInteger)currentSoundDriver
{
	id curSelected = [soundDriver selectedCell];
	if ([soundDriver cellAtRow:0 column:0] == curSelected) {
		return CoreAudioDriver;
	} else if ([soundDriver cellAtRow:1 column:0] == curSelected) {
		return SoundManagerDriver;
	} else if ([soundDriver cellAtRow:2 column:0] == curSelected) {
		return MIDISoundDriver;
	} else {
		return NoHardwareDriver;
	}
}

-(NSInteger)currentBits
{
	id curSelected = [outputBits selectedCell];
	if ([outputBits cellAtRow:0 column:0] == curSelected) {
		return bits8;
	} else if ([outputBits cellAtRow:0 column:1] == curSelected) {
		return bits16;
	} else if ([outputBits cellAtRow:0 column:2] == curSelected) {
		return bits20;
	} else if ([outputBits cellAtRow:0 column:3] == curSelected) {
		return bits24;
	} else {
		return -1;
	}
}

- (IBAction)changeBits:(id)sender
{
    short currBits = [self currentBits];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	switch (currBits) {
		case bits8:
			[defaults setInteger:8 forKey:PPSoundOutBits];
			break;
		case bits16:
		default:
			[defaults setInteger:16 forKey:PPSoundOutBits];
			break;
		case bits20:
			[defaults setInteger:20 forKey:PPSoundOutBits];
			break;
		case bits24:
			[defaults setInteger:24 forKey:PPSoundOutBits];
			break;
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeDriver:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setInteger:[self currentSoundDriver] forKey:PPSoundDriver];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeRate:(id)sender
{
    short curBits = [self currentRate];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	switch (curBits) {
		case rate11Khz:
			[defaults setInteger:11025 forKey:PPSoundOutRate];
			break;
		case rate22Khz:
			[defaults setInteger:22050 forKey:PPSoundOutRate];
			break;
		case rate44Khz:
		default:
			[defaults setInteger:44100 forKey:PPSoundOutRate];
			break;
		case rate48Khz:
			[defaults setInteger:48000 forKey:PPSoundOutRate];
			break;
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeChecked:(id)sender
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSInteger reverbState = [reverb state];
	NSInteger stereoDelayState = [stereoDelay state];
	NSInteger oversamplingState = [oversampling state];
	
	[defaults setBool:[surround state] forKey:PPSurroundToggle];
	[defaults setBool:reverbState forKey:PPReverbToggle];
	[defaults setBool:stereoDelayState forKey:PPStereoDelayToggle];
	[defaults setBool:oversamplingState forKey:PPOversamplingToggle];
	if (![defaults boolForKey:PPOversamplingToggle]) {
		//[defaults setInteger:1 forKey:PPOversamplingAmount];
	}
	
	[oversamplingNum setEnabled:oversamplingState];
	[reverbNum setEnabled:reverbState];
	[reverbPercent setEnabled:reverbState];
	[stereoDelayNum setEnabled:stereoDelayState];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeOversampling:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(OversamplingCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (OversamplingCoupling[i].tag == tag) {
			toSet = OversamplingCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 1;
	}
	[[NSUserDefaults standardUserDefaults] setInteger:toSet forKey:PPOversamplingAmount];

	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeReverbAmount:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(ReverbAmountCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (ReverbAmountCoupling[i].tag == tag) {
			toSet = ReverbAmountCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 25;
	}
	[[NSUserDefaults standardUserDefaults] setInteger:toSet forKey:PPReverbAmount];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeReverbPercent:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(ReverbPercentCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (ReverbPercentCoupling[i].tag == tag) {
			toSet = ReverbPercentCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 30;
	}
	[[NSUserDefaults standardUserDefaults] setInteger:toSet forKey:PPReverbStrength];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeStereoDelay:(id)sender
{
    int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(StereoDelayCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (StereoDelayCoupling[i].tag == tag) {
			toSet = StereoDelayCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 30;
	}
	[[NSUserDefaults standardUserDefaults] setInteger:toSet forKey:PPStereoDelayAmount];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

-(id)init
{
	if (self = [super initWithNibName:@"SoundOutput" bundle:nil]) {
		[self setTitle:@"Sound Output"];
	}
	return self;
}

-(void)setCurrentSoundDriver:(NSInteger)theDriver
{
	switch (theDriver) {
		case CoreAudioDriver:
			[soundDriver selectCellAtRow:0 column:0];
			break;
			
		case SoundManagerDriver:
			[soundDriver selectCellAtRow:1 column:0];
			break;
			
		case MIDISoundDriver:
			[soundDriver selectCellAtRow:2 column:0];
			break;

		default:
			[soundDriver selectCellAtRow:3 column:0];
			break;
	}
}

-(void)setCurrentBits:(NSInteger)bits
{
	switch (bits) {
		case bits8:
			[outputBits selectCellAtRow:0 column:0];
			break;
			
		case bits16:
		default:
			[outputBits selectCellAtRow:0 column:1];
			break;
			
		case bits20:
			[outputBits selectCellAtRow:0 column:2];
			break;
			
		case bits24:
			[outputBits selectCellAtRow:0 column:3];
			break;
	}
}

-(void)setCurrentRate:(NSInteger)currRate
{
	switch (currRate) {
		case rate11Khz:
			[rate selectCellAtRow:0 column:0];
			break;
			
		case rate22Khz:
			[rate selectCellAtRow:0 column:1];
			break;
			
		case rate44Khz:
		default:
			[rate selectCellAtRow:0 column:2];
			break;
			
		case rate48Khz:
			[rate selectCellAtRow:0 column:3];
			break;
	}
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	BOOL oversamplingState = [defaults boolForKey:PPOversamplingToggle];
	BOOL reverbState = [defaults boolForKey:PPReverbToggle];
	BOOL stereoDelayState = [defaults boolForKey:PPStereoDelayToggle];
	[oversampling setState:oversamplingState];
	[reverb setState:reverbState];
	[stereoDelay setState:stereoDelayState];
	[surround setState:[defaults boolForKey:PPSurroundToggle]];
	
	[oversamplingNum setEnabled:oversamplingState];
	[reverbNum setEnabled:reverbState];
	[reverbPercent setEnabled:reverbState];
	[stereoDelayNum setEnabled:stereoDelayState];

	
	[self setCurrentSoundDriver:[defaults integerForKey:PPSoundDriver]];
	
	{
		NSInteger unConvBits = [defaults integerForKey:PPSoundOutBits], ConvBits;
		switch (unConvBits) {
			case 8:
				ConvBits = bits8;
				break;
				
			default:
			case 16:
				ConvBits = bits16;
				break;
				
			case 20:
				ConvBits = bits20;
				break;
				
			case 24:
				ConvBits = bits24;
				break;
				
		}
		[self setCurrentBits:ConvBits];
	}
	{
		NSInteger unConvRate = [defaults integerForKey:PPSoundOutRate], convRate;
		switch (unConvRate) {
			case 11025:
				convRate = rate11Khz;
				break;
				
			case 22050:
				convRate = rate22Khz;
				break;
				
			default:
			case 44100:
				convRate = rate44Khz;
				break;
				
			case 48000:
				convRate = rate48Khz;
				break;
		}
		[self setCurrentRate:convRate];
	}
	{
		NSInteger theRate = [defaults integerForKey:PPStereoDelayAmount];
		NSInteger toSet = 0;
		int i = 0;
		size_t sizeofCoupling = sizeof(StereoDelayCoupling) / sizeof(TagCoupling);
		for (i = 0; i < sizeofCoupling; i++) {
			if (StereoDelayCoupling[i].amount == theRate) {
				toSet = StereoDelayCoupling[i].tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 4;
		}
		[stereoDelayNum selectItemAtIndex:toSet - 1];
	}
	{
		NSInteger reverbAmount = [defaults integerForKey:PPReverbAmount];
		NSInteger toSet = 0;
		int i = 0;
		size_t sizeofCoupling = sizeof(ReverbAmountCoupling) / sizeof(TagCoupling);
		for (i = 0; i < sizeofCoupling; i++) {
			if (ReverbAmountCoupling[i].amount == reverbAmount) {
				toSet = ReverbAmountCoupling[i].tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 4;
		}
		[reverbNum selectItemAtIndex:toSet - 1];
	}
	{
		NSInteger reverbPercentage = [defaults integerForKey:PPReverbStrength];
		NSInteger toSet = 0;
		int i = 0;
		size_t sizeofCoupling = sizeof(ReverbPercentCoupling) / sizeof(TagCoupling);
		for (i = 0; i < sizeofCoupling; i++) {
			if (ReverbPercentCoupling[i].amount == reverbPercentage) {
				toSet = ReverbPercentCoupling[i].tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 3;
		}
		[reverbPercent selectItemAtIndex:toSet - 1];
	}
	{
		NSInteger oversamplingAmount = [defaults integerForKey:PPOversamplingAmount];
		NSInteger toSet = 0;
		int i = 0;
		size_t sizeofCoupling = sizeof(OversamplingCoupling) / sizeof(TagCoupling);
		for (i = 0; i < sizeofCoupling; i++) {
			if (OversamplingCoupling[i].amount == oversamplingAmount) {
				toSet = OversamplingCoupling[i].tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 1;
		}
		[oversamplingNum selectItemAtIndex:toSet - 1];
	}

}

@end
