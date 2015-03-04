//
//  NoteTranslateController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "NoteTranslateController.h"

@implementation NoteTranslateController
@synthesize thePcmd;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
	}
	
	return self;
}

#if 0
- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

- (IBAction)cancel:(id)sender
{
	[_parentWindow endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

- (IBAction)okay:(id)sender
{
	short		track, row;
	NSInteger	trans = self.transAmount;
	Cmd			*myCmd;
	
	for (track = 0; track < thePcmd->tracks; track++) {
		for (row = 0; row < thePcmd->length; row++) {
			myCmd = MADGetCmd(row, track, thePcmd);
			
			if (myCmd->note != 0xFF && myCmd->note != 0xFE) {	// no notes = 0xFF
				if (myCmd->note + trans < 0)
					myCmd->note = 0;
				else if (myCmd->note + trans >= 96)
					myCmd->note = 96-1;
				else
					myCmd->note += trans;
			}
		}
	}
	[_parentWindow endSheet:self.window];
	_currentBlock(MADNoErr);
}

@end
