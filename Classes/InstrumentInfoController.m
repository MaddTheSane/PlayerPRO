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

@interface InstrumentInfoController ()

@end

@implementation InstrumentInfoController

- (void)updateInstrumentInfo
{
	[sampleTable reloadData];
	[[instrumentName cell] setTitle:instrument.name];
	[[instrumentNumber cell] setTitle:[NSString stringWithFormat:@"Instrument %ld", (long)instrument.number + 1]];
}

@synthesize instrument;

- (void)setInstrument:(PPInstrumentObject *)ainstrument
{
	instrument = [ainstrument copy];
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
	if (sheet == [self window]) {
		if (returnCode == NSOKButton) {
			[delegate replaceObjectInInstrumentsAtIndex:[instrument number] withObject:instrument];
		}
	}
	[sheet orderOut:nil];
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
	[NSApp endSheet:[self window] returnCode:NSOKButton];
}

- (IBAction)cancelButton:(id)sender {
	[NSApp endSheet:[self window] returnCode:NSCancelButton];
}

- (IBAction)volumeEditButtonPressed:(id)sender {
	
}

- (IBAction)volumeCheckClicked:(id)sender {
	
}

- (IBAction)panningCheckClicked:(id)sender {
	
}

- (IBAction)panningEditButtonPressed:(id)sender {
	
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [instrument sampleCount];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	return [[instrument samples][row] name];
}

@end
