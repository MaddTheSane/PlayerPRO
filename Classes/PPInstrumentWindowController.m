//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#import "PPInstrumentImporter.h"

@interface PPInstrumentWindowController ()

@end

@implementation PPInstrumentWindowController

@synthesize importer;
@synthesize curMusic;

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (id)init
{
	return self = [self initWithWindowNibName:@"PPInstrumentWindowController"];
}


- (IBAction)exportInstrument:(id)sender
{
	
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[importer release];
	
	[super dealloc];
}
#endif

@end
