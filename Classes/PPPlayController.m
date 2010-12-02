//
//  PPPlayController.m
//
//  Created by C.W. Betts on 11/23/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPPlayController.h"
#import "UserDefaultKeys.h"
//#include "RDriverInt.h"

static inline UnsignedFixed GetFixedRate(int Rate)
{
	switch (Rate) {
		case 11:
			return rate11025hz;
			break;
		case 22:
			return rate22050hz;
			break;
		case 44:
			return rate44khz;
			break;
		case 48:
			return rate48khz;
			break;
			
		default:
			return rate44khz;
			break;
	}
}

@implementation PPPlayController

- (void)MADDriverWithPreferences {
	Boolean madWasReading = NO;
	long fullTime = 0, curTime = 0;
	if (MADDriver) {
		madWasReading = MADWasReading(MADDriver);
		MADSetReading( MADDriver, FALSE);
		MADStopDriver(MADDriver);
		MADDisposeDriver(MADDriver);
		if (madWasReading) {
			MADGetMusicStatus(MADDriver, &fullTime, &curTime);
		}
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = GetFixedRate([defaults integerForKey:PPSoundOutRate]);
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	init.oversampling = [defaults integerForKey:PPOversamplingAmount];
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = [defaults integerForKey:PPReverbSize];
	init.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	init.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	
	MADCreateDriver(&init, MADLib, &MADDriver);
	MADStartDriver(MADDriver);
	if (madWasReading) {
		MADAttachDriverToMusic(MADDriver, Music, NULL);
		MADSetMusicStatus(MADDriver, 0, fullTime, curTime);
		MADSetReading(MADDriver, true);
	}
}

- (id)init {
	[super init];
	{
		MADInitLibraryNew(NULL, &MADLib);
		[self MADDriverWithPreferences];
	}
	
	return self;
}

- (IBAction)fastForwardButtonPressed:(id)sender {
    
}

- (IBAction)loopButtonPressed:(id)sender {
    
}

- (IBAction)nextButtonPressed:(id)sender {
    
}

- (IBAction)playButtonPressed:(id)sender {
    
}

- (IBAction)prevButtonPressed:(id)sender {
    
}

- (IBAction)recordButtonPressed:(id)sender {
    
}

- (IBAction)rewindButtonPressed:(id)sender {
    
}

- (IBAction)sliderChanged:(id)sender {
    
}

- (IBAction)stopButtonPressed:(id)sender {
    
}

-(void)dealloc {
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);

	[super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	
}

- (void)soundPreferencesDidChange:(NSNotification *)notification {
	[self MADDriverWithPreferences];
}

@end
