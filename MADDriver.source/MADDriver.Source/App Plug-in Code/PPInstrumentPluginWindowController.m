//
//  PPInstrumentPluginWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/26/13.
//
//

#import "PPInstrumentPluginWindowController.h"

@interface PPInstrumentPluginWindowController ()

@end

@implementation PPInstrumentPluginWindowController
@synthesize alienFileURL;
@synthesize instrumentHeader;
@synthesize order;
@synthesize sample;
@synthesize sampleID;

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
