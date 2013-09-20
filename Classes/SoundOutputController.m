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
@property (retain) PPSoundSettingsViewController *soundSettingsView;
@end

@implementation SoundOutputController
@synthesize soundSettingsView;
@synthesize subView;

-(id)init
{
	if (self = [super initWithNibName:@"SoundOutput" bundle:nil]) {
		[self setTitle:@"Sound Output"];
	}
	return self;
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	self.soundSettingsView = [[PPSoundSettingsViewController alloc] init];
	soundSettingsView.delegate = self;
	
	[subView setContentView:[soundSettingsView view]];

	MADDriverSettings drivSet;
	
	drivSet.surround = [defaults boolForKey:PPSurroundToggle];
	drivSet.driverMode = [defaults integerForKey:PPSoundDriver];
	drivSet.outPutBits = [defaults integerForKey:PPSoundOutBits];
	drivSet.outPutRate = [defaults integerForKey:PPSoundOutRate];
	drivSet.outPutMode = DeluxeStereoOutPut;
	if ([defaults boolForKey:PPOversamplingToggle]) {
		drivSet.oversampling = [defaults integerForKey:PPOversamplingAmount];
	} else {
		drivSet.oversampling = 1;
	}
	drivSet.Reverb = [defaults boolForKey:PPReverbToggle];
	drivSet.ReverbSize = [defaults integerForKey:PPReverbAmount];
	drivSet.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		drivSet.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	} else {
		drivSet.MicroDelaySize = 0;
	}
	
	[soundSettingsView settingsFromDriverSettings:&drivSet];
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

- (void)soundOutReverbStrengthDidChange:(short)rev
{
	[[NSUserDefaults standardUserDefaults] setInteger:rev forKey:PPReverbStrength];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutReverbSizeDidChange:(short)rev
{
	[[NSUserDefaults standardUserDefaults] setInteger:rev forKey:PPReverbAmount];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutOversamplingAmountDidChange:(short)ovs
{
	[[NSUserDefaults standardUserDefaults] setInteger:ovs forKey:PPOversamplingAmount];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

- (void)soundOutStereoDelayAmountDidChange:(short)std
{
	[[NSUserDefaults standardUserDefaults] setInteger:std forKey:PPStereoDelayAmount];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPSoundPreferencesDidChange object:self];
}

@end
