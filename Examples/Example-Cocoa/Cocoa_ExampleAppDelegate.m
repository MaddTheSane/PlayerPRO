//
//  Cocoa_ExampleAppDelegate.m
//  Cocoa Example
//
//  Created by C.W. Betts on 11/29/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Cocoa_ExampleAppDelegate.h"

@implementation Cocoa_ExampleAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	MADInitLibrary(NULL, &MADLib);
	MADDriverSettings init;
	MADGetBestDriver(&init);
	MADCreateDriver(&init, MADLib, &MADDriver);
	MADStartDriver(MADDriver);
}

- (void)dealloc
{
	if (Music != NULL) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	
	[super dealloc];
}

- (void)loadMusicFromNSURL:(NSURL*)theURL
{
	if (Music) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}
	
	char type[5] = {0};
	
	if(MADMusicIdentifyCFURL(MADLib, type, (CFURLRef)theURL) !=noErr)
	{
		return;
	}
	
	MADLoadMusicCFURLFile(MADLib, &Music, type, (CFURLRef)theURL);
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	MADPlayMusic(MADDriver);
}

@end
