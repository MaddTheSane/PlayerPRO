//
//  SampleCellView.m
//  PPMacho
//
//  Created by C.W. Betts on 2/15/13.
//
//

#import "SampleCellView.h"

@implementation SampleCellView
@synthesize ampButton;
@synthesize modeButton;
@synthesize rateBox;
@synthesize volumeField;
@synthesize nameField;
@synthesize loopStartField;
@synthesize loopSizeField;
@synthesize loopTypeButton;
@synthesize relNoteField;
@synthesize relNoteLabel;
@synthesize instrumentIndexLabel;

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
	// Drawing code here.
}

- (IBAction)modeChanged:(id)sender
{
	
}

- (IBAction)amplitudeChanged:(id)sender
{
	
}

- (IBAction)rateChanged:(id)sender
{
	
}

- (IBAction)loopTypeChanged:(id)sender
{
	
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox
{
	return 22;
}

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index
{
	switch (index) {
		case 0:
			return @"11025 Hz";
			break;
			
		case 1:
			return @"11127 Hz";
			break;
			
		case 5:
			return @"--";
			break;
			
		default:
			return @"";
			break;
	}
}

@end
