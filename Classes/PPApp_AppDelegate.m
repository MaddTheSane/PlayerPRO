//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"

@implementation PPApp_AppDelegate

- (IBAction)showMusicList:(id)sender {
    [window makeKeyAndOrderFront:sender];
}

- (BOOL)loadMADMusic:(MADMusic*)musicToLoad
{
	
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
	MADGetBestDriver( &init);

}

-(void)dealloc
{
	MADStopDriver( MADDriver);
	MADDisposeDriver( MADDriver);
	MADDisposeLibrary( MADLib);
	
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

@end
