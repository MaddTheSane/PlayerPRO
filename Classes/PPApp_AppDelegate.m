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

@implementation PPApp_AppDelegate

- (IBAction)showMusicList:(id)sender {
    [window makeKeyAndOrderFront:sender];
}

- (BOOL)loadMusicFile:(NSURL*)musicToLoad
{
	MADStopMusic(MADDriver);
	MADCleanDriver(MADDriver);
	MADDisposeMusic(&Music, MADDriver);
	MADLoadMusicCFURLFile(MADLib, &Music, '\?\?\?\?', (CFURLRef)musicToLoad);
	
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
			//TODO: selected?
			return YES;
			break;
		default:
		return NO;
		break;
	}	
}

@end
