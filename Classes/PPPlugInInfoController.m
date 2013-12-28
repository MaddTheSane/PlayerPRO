//
//  PPPlugInInfoController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfoController.h"
#import "PPPlugInInfo.h"

@implementation PPPlugInInfoController

- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;
{
	if(self = [self initWithWindowNibName:@"PPPlugInInfoController"]) {
		info = plugInfo;
	}
	return self;
}

- (IBAction)closeWindow:(id)sender {
	[NSApp stopModal];
	[[self window] close];
}

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
	[nameField setStringValue:info.plugName];
	[authorField setStringValue:info.authorName];
	[typeField setStringValue:info.plugType];
	NSImage *plugImage = [[NSWorkspace sharedWorkspace] iconForFile:[info.plugURL path]];
	[plugImage setSize:NSMakeSize(64, 64)];
	[pluginImage setImage:plugImage];
}

@end
