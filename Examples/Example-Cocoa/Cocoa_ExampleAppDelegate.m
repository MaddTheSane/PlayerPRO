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
	MADInitLibrary(NULL, false, &MADLib);
	MADDriverSettings init;
	MADGetBestDriver(&init);
	MADCreateDriver(&init, MADLib, &MADDriver);
	MADStartDriver(MADDriver);
}

- (void)dealloc
{
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	
	[super dealloc];
}

@end
