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
#import "NSColor+PPPreferences.h"

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
	init.outPutRate = [defaults integerForKey:PPSoundOutRate];
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
	[defaultPrefs setObject:[NSNumber numberWithInt:44100] forKey:PPSoundOutRate];
	[defaultPrefs setObject:[NSNumber numberWithInt:CoreAudioDriver] forKey:PPSoundDriver];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPStereoDelayToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPReverbToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPSurroundToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPOversamplingToggle];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPStereoDelayAmount];
	[defaultPrefs setObject:[NSNumber numberWithInt:25] forKey:PPReverbSize];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPReverbStrength];
	[defaultPrefs setObject:[NSNumber numberWithInt:1] forKey:PPOversamplingAmount];
	
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowInstrument];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowNote];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowEffect];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowArgument];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowVolume];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEShowMarkers];
	[defaultPrefs setObject:[NSNumber numberWithInt:0] forKey:PPDEMarkerOffsetPref];
	[defaultPrefs setObject:[NSNumber numberWithInt:3] forKey:PPDEMarkerLoopPref];
	[defaultPrefs setObject:[[NSColor yellowColor] PPencodeColor] forKey:PPDEMarkerColorPref];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEMouseClickControlPref];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPDEMouseClickShiftPref];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPDEMouseClickCommandPref];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPDEMouseClickOptionPref];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDELineHeightNormal];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEMusicTraceOn];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEPatternWrappingPartition];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPDEDragAsPcmd];

	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPBEMarkersEnabled];
	[defaultPrefs setObject:[NSNumber numberWithInt:0] forKey:PPBEMarkersOffset];
	[defaultPrefs setObject:[NSNumber numberWithInt:3] forKey:PPBEMarkersLoop];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPBEOctaveMarkers];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPBENotesProjection];

	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPMAddExtension];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPMMadCompression];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPMNoLoadMixerFromFiles];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPMOscilloscopeDrawLines];
	
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

	OSType fileType = 0;
	MADMusicIdentifyCFURL(MADLib, &fileType, (CFURLRef)musicToLoad);
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
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(preferencesDidChange:) name:PPListPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(digitalEditorPreferencesDidChange:) name:PPDigitalEditorPrefrencesDidChange object:nil];
	
	MADInitLibrary(NULL, false, &MADLib);
	[tableView setDataSource:musicList];
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
		case PPToolbarFileInfo:
			return YES;
			break;
			
		case PPToolbarPlayMusic:
			if([[tableView selectedRowIndexes] count] == 1) {
				return YES;
			} else {
				return NO;
			}
			break;
			
		case PPToolbarRemoveMusic:
			if([[tableView selectedRowIndexes] count] > 0) {
				return YES;
			} else {
				return NO;
			}
			break;

		default:
		return NO;
		break;
	}	
}

- (void)soundPreferencesDidChange:(NSNotification *)notification {
	[self MADDriverWithPreferences];
}

- (void)digitalEditorPreferencesDidChange:(NSNotification *)notification
{
	
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
