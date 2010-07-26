//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"

extern void MyDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error",@"Error"), [NSString stringWithFormat:NSLocalizedString(@"MyDebugStr_MainText", @"The program has encountered the following error:\n %s\nIt is reccomended to quit the program and send the debug report."), text], NSLocalizedString(@"MyDebugStr_Quit",@"Quit"), NSLocalizedString(@"MyDebugStr_Continue",@"Continue"), NSLocalizedString(@"MyDebugStr_Debug",@"Debug"));
	switch (alert) {
		case NSAlertDefaultReturn:
			exit(EXIT_FAILURE);
			break;
		case NSAlertAlternateReturn:
			break;
		case NSAlertOtherReturn:
			Debugger();
			break;
	}
}



@implementation PPApp_AppDelegate
- (IBAction)showMusicList:(id)sender {
    
}

- (IBAction)showPreferences:(id)sender {
    if (!preferences) {
		[NSBundle loadNibNamed:@"preferences" owner:self];
	}
	[preferences makeKeyAndOrderFront:self];

}


- (IBAction)exportInstrumentAs:(id)sender {
    
}

- (IBAction)showInstrumentsList:(id)sender {
    
}

- (IBAction)showTools:(id)sender {
    
}

@synthesize window;
@synthesize preferences;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	MADInitLibraryNew(NULL, &MADLib);
}

-(void)dealloc
{
	
	[super dealloc];
}

@end
