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

@implementation PPApp_AppDelegate

+(void)initialize {
	NSMutableDictionary *defaultPrefs = [NSMutableDictionary dictionary];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPRemberMusicList];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPLoadMusicAtListLoad];
	[defaultPrefs setObject:[NSNumber numberWithInt:1] forKey:PPAfterPlayingMusic];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPGotoStartupAfterPlaying];
	[defaultPrefs setObject:[NSNumber numberWithBool:YES] forKey:PPSaveModList];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPLoadMusicAtMusicLoad];
	
	[defaultPrefs setObject:[NSNumber numberWithInt:16] forKey:PPSoundOutBits];
	[defaultPrefs setObject:[NSNumber numberWithInt:44] forKey:PPSoundOutRate];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPStereoDelayToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPReverbToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPSurroundToggle];
	[defaultPrefs setObject:[NSNumber numberWithBool:NO] forKey:PPOversamplingToggle];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPStereoDelayAmount];
	[defaultPrefs setObject:[NSNumber numberWithInt:25] forKey:PPReverbAmount];
	[defaultPrefs setObject:[NSNumber numberWithInt:30] forKey:PPReverbPercent];
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
	MADInitLibraryNew(NULL, &MADLib);
	MADDriverSettings init;
	MADGetBestDriver(&init);
	MADCreateDriver(&init, MADLib, &MADDriver);
	MADStartDriver(MADDriver);

	musicList = [[PPMusicList alloc] init];
}

-(void)dealloc
{
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	[preferences release];
	[musicList release];
	
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

@end
