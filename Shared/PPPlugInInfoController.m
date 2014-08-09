//
//  PPPlugInInfoController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfoController.h"
#ifdef PLAYERPRO6
#import "PlayerPRO_6-Swift.h"
#else
#import "PlayerPRO_Player-Swift.h"
#endif

@interface PPPlugInInfoController ()
@property (strong) PPPlugInInfo *info;
@end

@implementation PPPlugInInfoController
@synthesize authorField;
@synthesize info;
@synthesize nameField;
@synthesize pluginImage;
@synthesize theCopyrightField;
@synthesize typeField;

- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;
{
	if (self = [self initWithWindowNibName:@"PPPlugInInfoController"]) {
		self.info = plugInfo;
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
	[nameField setStringValue:self.info.plugName];
	[authorField setStringValue:self.info.authorName];
	[typeField setStringValue:self.info.plugType];
	[theCopyrightField setStringValue:self.info.plugCopyright];
	NSImage *plugImage = [[NSWorkspace sharedWorkspace] iconForFile:self.info.plugURL.path];
	[plugImage setSize:NSMakeSize(64, 64)];
	[pluginImage setImage:plugImage];
}

@end
