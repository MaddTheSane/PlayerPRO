//
//  InstrumentInfoController.m
//  PPMacho
//
//  Created by C.W. Betts on 2/9/13.
//
//

#import "InstrumentInfoController.h"
#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentWindowController.h"
#import "ARCBridge.h"

@interface InstrumentInfoController ()

@end

@implementation InstrumentInfoController

- (void)updateInstrumentInfo
{
	[sampleTable reloadData];
}

@synthesize instrument;

- (void)setInstrument:(PPInstrumentObject *)ainstrument
{
#if __has_feature(objc_arc)
	instrument = [ainstrument copy];
#else
	PPInstrumentObject *tmpObj = instrument;
	instrument = [aistrument copy];
	[tmpObj release];
#endif
	if ([self isWindowLoaded]) {
		[self updateInstrumentInfo];
	}
}

@synthesize delegate;

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton) {
		[delegate replaceObjectInInstrumentsAtIndex:[instrument number] withObject:instrument];
	}
}

- (id)init
{
	return self = [self initWithWindowNibName:@"InstrumentInfoController"];
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
	if (instrument) {
		[self updateInstrumentInfo];
	}
	
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)okayButton:(id)sender {
	[NSApp stopModalWithCode:NSOKButton];
	[[self window] close];
}

- (IBAction)cancelButton:(id)sender {
	[NSApp stopModalWithCode:NSCancelButton];
	[[self window] close];
}

- (IBAction)volumeEditButtonPressed:(id)sender {
	
}

- (IBAction)volumeCheckClicked:(id)sender {
	
}

- (IBAction)panningCheckClicked:(id)sender {
	
}

- (IBAction)panningEditButtonPressed:(id)sender {
	
}
@end
