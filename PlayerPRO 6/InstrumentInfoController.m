//
//  InstrumentInfoController.m
//  PPMacho
//
//  Created by C.W. Betts on 2/9/13.
//
//

#import <PlayerPROKit/PlayerPROKit.h>
#import "InstrumentInfoController.h"
#import "PPInstrumentViewController.h"
#import "PPSmallPianoView.h"

@interface InstrumentInfoController ()

@end

@implementation InstrumentInfoController
@synthesize instrument;
@synthesize delegate;
@synthesize pianoView;
@synthesize instrumentName;
@synthesize instrumentNumber;
@synthesize panningFixedSpeedButton;
@synthesize panningLoopButton;
@synthesize panningMatrix;
@synthesize panningSustainButton;
@synthesize sampleTable;
@synthesize volumeFixedSpeedButton;
@synthesize volumeLoopButton;
@synthesize volumeMatrix;
@synthesize volumeSustainButton;

- (void)updateInstrumentInfo
{
	[sampleTable reloadData];
	[instrumentName setStringValue:instrument.name];
	[instrumentNumber setStringValue:[NSString stringWithFormat:@"Instrument %ld", (long)instrument.number + 1]];
}


- (void)setInstrument:(PPInstrumentObject *)ainstrument
{
	instrument = [ainstrument copy];
	if ([self isWindowLoaded]) {
		[self updateInstrumentInfo];
	}
}


- (instancetype)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (void)instrumentSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (sheet == [self window]) {
		if (returnCode == NSOKButton) {
			[delegate replaceObjectInInstrumentsAtIndex:[instrument number] withObject:instrument];
		}
	}
	[sheet orderOut:nil];
}

- (instancetype)init
{
	return self = [self initWithWindowNibName:@"InstrumentInfoView"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	
	if (instrument) {
		[self updateInstrumentInfo];
	}
}

- (IBAction)okayButton:(id)sender
{
	[NSApp endSheet:[self window] returnCode:NSOKButton];
}

- (IBAction)cancelButton:(id)sender
{
	[NSApp endSheet:[self window] returnCode:NSCancelButton];
}

- (IBAction)volumeEditButtonPressed:(id)sender
{
	
}

- (IBAction)volumeCheckClicked:(id)sender
{
	
}

- (IBAction)panningCheckClicked:(id)sender
{
	
}

- (IBAction)panningEditButtonPressed:(id)sender
{
	
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [instrument countOfSamples];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	return [[instrument samples][row] name];
}

@end
