//
//  PPFilterPluginWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/24/13.
//
//

#import "PPFilterPluginWindowController.h"

@interface PPFilterPluginWindowController ()

@end

@implementation PPFilterPluginWindowController
@synthesize theData;
@synthesize selectionEnd;
@synthesize selectionStart;
@synthesize stereoMode;

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

@end
