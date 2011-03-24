//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"
#import "PPMusicList.h"
#import "UserDefaultKeys.h"
#import "NDAlias/NSURL+NDCarbonUtilities.h"

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


@implementation PPApp_AppDelegate

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

+(void)initialize {
	NSMutableDictionary *defaultPrefs = [NSMutableDictionary dictionary];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPRememberMusicList];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPLoadMusicAtListLoad];
	[defaultPrefs setObject:[NSNumber numberWithInt:PPStopPlaying] forKey:PPAfterPlayingMusic];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPGotoStartupAfterPlaying];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPSaveModList];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPLoadMusicAtMusicLoad];
	
	[defaultPrefs setObject:[NSNumber numberWithInt:16] forKey:PPSoundOutBits];
	[defaultPrefs setObject:[NSNumber numberWithInt:44] forKey:PPSoundOutRate];
	[defaultPrefs setObject:[NSNumber numberWithInt:SoundManagerDriver] forKey:PPSoundDriver];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPStereoDelayToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPReverbToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPSurroundToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPOversamplingToggle];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPStereoDelayAmount];
	[defaultPrefs setObject:[NSNumber numberWithInt:25] forKey:PPReverbSize];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPReverbStrength];
	[defaultPrefs setObject:[NSNumber numberWithInt:1] forKey:PPOversamplingAmount];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultPrefs];
}

- (IBAction)showMusicList:(id)sender {
    [window makeKeyAndOrderFront:sender];
}


- (BOOL)loadMusicFile:(NSURL*)musicToLoad
{
	MADStopMusic(MADDriver);
	MADCleanDriver(MADDriver);
	MADDisposeMusic(&Music, MADDriver);
	OSType fileType, creatorType;
	UInt16 unused1; 
	[musicToLoad finderInfoFlags:&unused1 type:&fileType creator:&creatorType];
	MADLoadMusicCFURLFile(MADLib, &Music, fileType, (CFURLRef)musicToLoad);
	
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	MADPlayMusic(MADDriver);
}

- (IBAction)showPreferences:(id)sender {
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[preferences showWindow:self];
}


- (IBAction)exportInstrumentAs:(id)sender {
    
}

- (IBAction)showInstrumentsList:(id)sender {
    
}

- (IBAction)showTools:(id)sender {
    
}

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	
	musicList = [[PPMusicList alloc] init];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(preferencesDidChange:) name:PPListPreferencesDidChange object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	MADInitLibraryNew(NULL, &MADLib);
	[self MADDriverWithPreferences];

}

-(void)preferencesDidChange:(NSNotification *)notification {
	
}

-(void)dealloc
{
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	
	[preferences release];
	[musicList release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[super dealloc];
}

- (IBAction)deleteInstrument:(id)sender {
    
}

- (IBAction)showBoxEditor:(id)sender {
    
}

- (IBAction)showClassicEditor:(id)sender {
    
}

- (IBAction)showDigitalEditor:(id)sender {
    
}

- (IBAction)getMusicInfo:(id)sender {
    
}

enum PPMusicToolbarTypes {
	PPToolbarSort = 1001,
	PPToolbarAddMusic,
	PPToolbarRemoveMusic,
	PPToolbarPlayMusic,
	PPToolbarFileInfo
};

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	switch ([theItem tag]) {
		case PPToolbarSort:
		case PPToolbarAddMusic:
			return YES;
			break;
		case PPToolbarRemoveMusic:
		case PPToolbarPlayMusic:
		case PPToolbarFileInfo:
			//[musicTable ]
			return YES;
			break;
		default:
		return NO;
		break;
	}	
}

- (void)soundPreferencesDidChange:(NSNotification *)notification {
	[self MADDriverWithPreferences];
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


@end
