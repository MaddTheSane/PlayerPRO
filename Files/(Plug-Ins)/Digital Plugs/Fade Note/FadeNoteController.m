//
//  FadeNoteController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeNoteController.h"

@interface FadeNoteController ()

@end

@implementation FadeNoteController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
		isMultipleIstanceSafe = YES;
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end
