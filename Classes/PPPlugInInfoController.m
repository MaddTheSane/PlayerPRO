//
//  PPPlugInInfoController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfoController.h"
#import "PPPlugInInfo.h"

@interface PPPlugInInfoController ()

@end

@implementation PPPlugInInfoController

- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;
{
	if(self = [self initWithWindowNibName:@"PPPlugInInfoController"])
	{
		info = RETAINOBJ(plugInfo);
	}
	return self;
}

- (IBAction)closeWindow:(id)sender {
	[[NSApplication sharedApplication] stopModal];
	[self closeWindow:sender];
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
    [nameField setTitleWithMnemonic:info.plugName];
	[authorField setTitleWithMnemonic:info.authorName];
	[typeField setTitleWithMnemonic:info.plugType];
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[info release];
	
	[super dealloc];
}
#endif


@end
