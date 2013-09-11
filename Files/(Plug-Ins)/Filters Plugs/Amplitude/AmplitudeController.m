//
//  AmplitudeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "AmplitudeController.h"
#import <PlayerPROCore/PPPlug.h>
#include "Amplitude.h"


@interface AmplitudeController ()

@end

Boolean getParams ( SInt32 *p1, PPInfoPlug *thePPInfoPlug)
{
	AmplitudeController *controller = [[AmplitudeController alloc] initWithWindowNibName:@"AmplitudeController"];
	controller.amplitudeAmount = *p1;
	
	NSInteger retVal = [NSApp runModalForWindow:controller.window];
	[controller close];

	if (retVal == NSOffState) {
		return FALSE;
	} else {
		*p1 = controller.amplitudeAmount;
		return TRUE;
	}
}

@implementation AmplitudeController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)okOrCancel:(id)sender {
	[NSApp stopModalWithCode:([sender tag] == 1) ? NSOffState : NSOnState];

}
@end
