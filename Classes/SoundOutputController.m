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
	rate11Khz = 0,
	rate22Khz,
	rate44Khz,
	rate48Khz
};

enum {
	bits8 = 0,
	bits16,
	bits24
};

-(NSInteger)currentRate {
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

-(NSInteger)currentSoundDriver {
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

-(NSInteger)currentBits {
	id curSelected = [outputBits selectedCell];
	if ([outputBits cellAtRow:0 column:0] == curSelected) {
		return bits8;
	} else if ([outputBits cellAtRow:0 column:1] == curSelected) {
		return bits16;
	} else if ([outputBits cellAtRow:0 column:2] == curSelected) {
		return bits24;
	} else {
		return -1;
	}
}

- (IBAction)changeBits:(id)sender {
    short currBits = [self currentBits];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	switch (currBits) {
		case bits8:
			[defaults setInteger:8 forKey:PPSoundOutBits];
			break;
		case bits16:
			[defaults setInteger:16 forKey:PPSoundOutBits];
			break;
		case bits24:
			[defaults setInteger:24 forKey:PPSoundOutBits];
			break;

		default:
			return;
			break;
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeDriver:(id)sender {
	[[NSUserDefaults standardUserDefaults] setInteger:[self currentSoundDriver] forKey:PPSoundDriver];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeRate:(id)sender {
    short curBits = [self currentRate];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	switch (curBits) {
		case rate11Khz:
			[defaults setInteger:11 forKey:PPSoundOutRate];
			break;
		case rate22Khz:
			[defaults setInteger:22 forKey:PPSoundOutRate];
			break;
		case rate44Khz:
			[defaults setInteger:44 forKey:PPSoundOutRate];
			break;
		case rate48Khz:
			[defaults setInteger:48 forKey:PPSoundOutRate];
			break;
			
		default:
			return;
			break;
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeChecked:(id)sender {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[surround state] forKey:PPSurroundToggle];
	[defaults setBool:[reverb state] forKey:PPReverbToggle];
	[defaults setBool:[stereoDelay state] forKey:PPStereoDelayToggle];
	[defaults setBool:[oversampling state] forKey:PPOversamplingToggle];
	if (![defaults boolForKey:PPOversamplingToggle]) {
		[defaults setInteger:1 forKey:PPOversamplingAmount];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeOversampling:(id)sender {
    
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeReverbAmount:(id)sender {
    
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeReverbPercent:(id)sender {
    
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (IBAction)changeStereoDelay:(id)sender {
    
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

-(id)init {
	if (![super initWithNibName:@"SoundOutput" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Sound Output"];
	return self;
}

-(void)setCurrentSoundDriver:(NSInteger)theDriver {
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

-(void)setCurrentBits:(NSInteger)bits {
	switch (bits) {
		case bits8:
			[outputBits selectCellAtRow:0 column:0];
			break;
		case bits16:
			[outputBits selectCellAtRow:0 column:1];
			break;
		case bits24:
			[outputBits selectCellAtRow:0 column:2];
			break;

	   default:
			break;
	}
}

-(void)setCurrentRate:(NSInteger)currRate {
	switch (currRate) {
		case rate11Khz:
			[rate selectCellAtRow:0 column:0];
			break;
		case rate22Khz:
			[rate selectCellAtRow:0 column:1];
			break;
		case rate44Khz:
			[rate selectCellAtRow:0 column:2];
			break;
		case rate48Khz:
			[rate selectCellAtRow:0 column:3];
			break;

		default:
			break;
	}
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[oversampling setState:[defaults boolForKey:PPOversamplingToggle]];
	[reverb setState:[defaults boolForKey:PPReverbToggle]];
	[stereoDelay setState:[defaults boolForKey:PPStereoDelayToggle]];
	[surround setState:[defaults boolForKey:PPSurroundToggle]];
	[self setCurrentSoundDriver:[defaults integerForKey:PPSoundDriver]];
	
	{
		NSInteger unConvBits = [defaults integerForKey:PPSoundOutBits], ConvBits;
		if (unConvBits == 8) {
			ConvBits = bits8;
		} else if (unConvBits == 16) {
			ConvBits = bits16;
		} else if (unConvBits == 24) {
			ConvBits = bits24;
		} else {
			ConvBits = bits16;
		}
		[self setCurrentBits:ConvBits];
	}
	{
		NSInteger unConvRate = [defaults integerForKey:PPSoundOutRate], convRate;
		if (unConvRate == 11) {
			convRate = rate11Khz;
		} else if (unConvRate == 22) {
			convRate = rate22Khz;
		} else if (unConvRate == 44) {
			convRate = rate44Khz;
		} else if (unConvRate == 48) {
			convRate = rate48Khz;
		} else {
			convRate = rate44Khz;
		}
		[self setCurrentRate:convRate];
	}
}

@end
