//
//  SoundOutputController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SoundOutputController.h"
#import "UserDefaultKeys.h"

@interface SoundOutputController ()
@end

@implementation SoundOutputController

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) {
		self.delegate = self;
		self.title = NSLocalizedStringFromTable(@"Sound Output", @"PreferenceNames", @"Sound Output");
	}
	
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	MADDriverSettings drivSet;
	
	drivSet.surround = [defaults boolForKey:PPSurroundToggle];
	drivSet.driverMode = [defaults integerForKey:PPSoundDriver];
	drivSet.outPutBits = [defaults integerForKey:PPSoundOutBits];
	drivSet.outPutRate = (unsigned int)[defaults integerForKey:PPSoundOutRate];
	drivSet.outPutMode = DeluxeStereoOutPut;
	if ([defaults boolForKey:PPOversamplingToggle]) {
		drivSet.oversampling = (int)[defaults integerForKey:PPOversamplingAmount];
	} else {
		drivSet.oversampling = 1;
	}
	drivSet.Reverb = [defaults boolForKey:PPReverbToggle];
	drivSet.ReverbSize = (int)[defaults integerForKey:PPReverbAmount];
	drivSet.ReverbStrength = (int)[defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		drivSet.MicroDelaySize = (int)[defaults integerForKey:PPStereoDelayAmount];
	} else {
		drivSet.MicroDelaySize = 0;
	}
	
	[self settingsFromDriverSettings:&drivSet];
}

- (void)soundOutDriverDidChange:(short)driv
{
	[[NSUserDefaults standardUserDefaults] setInteger:driv forKey:PPSoundDriver];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutBitsDidChange:(short)bits
{
	[[NSUserDefaults standardUserDefaults] setInteger:bits forKey:PPSoundOutBits];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutRateDidChange:(unsigned int)rat
{
	[[NSUserDefaults standardUserDefaults] setInteger:rat forKey:PPSoundOutRate];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutReverbDidChangeActive:(BOOL)isAct
{
	[[NSUserDefaults standardUserDefaults] setBool:isAct forKey:PPReverbToggle];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutOversamplingDidChangeActive:(BOOL)isAct
{
	[[NSUserDefaults standardUserDefaults] setBool:isAct forKey:PPOversamplingToggle];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutStereoDelayDidChangeActive:(BOOL)isAct
{
	[[NSUserDefaults standardUserDefaults] setBool:isAct forKey:PPStereoDelayToggle];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutSurroundDidChangeActive:(BOOL)isAct
{
	[[NSUserDefaults standardUserDefaults] setBool:isAct forKey:PPSurroundToggle];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutReverbStrengthDidChange:(int)rev
{
	[[NSUserDefaults standardUserDefaults] setInteger:rev forKey:PPReverbStrength];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutReverbSizeDidChange:(int)rev
{
	[[NSUserDefaults standardUserDefaults] setInteger:rev forKey:PPReverbAmount];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutOversamplingAmountDidChange:(int)ovs
{
	[[NSUserDefaults standardUserDefaults] setInteger:ovs forKey:PPOversamplingAmount];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutStereoDelayAmountDidChange:(int)std
{
	[[NSUserDefaults standardUserDefaults] setInteger:std forKey:PPStereoDelayAmount];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

@end
