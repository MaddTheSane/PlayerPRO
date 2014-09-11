//
//  FadeVolumeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeVolumeController.h"

@implementation FadeVolumeController
@synthesize thePcmd;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
	}
	
	return self;
}

- (IBAction)okay:(id)sender
{
	short	track, row;
	long	from, to;
	Cmd		*myCmd;
	
	// Re-adjust val % -> 0...64
	to 		= (self.fadeTo * 64);
	from 	= (self.fadeFrom * 64);
	
	for (track = 0; track < thePcmd->tracks; track++) {
		for (row = 0; row < thePcmd->length; row++) {
			myCmd = MADGetCmd(row, track, thePcmd);
			
			myCmd->ins 	= myCmd->ins;	// is this very usefull?
			myCmd->note	= myCmd->note;	// is this very usefull?
			myCmd->cmd	= myCmd->cmd;	// is this very usefull?
			myCmd->arg	= myCmd->arg;	// is this very usefull?
			
			if (thePcmd->length > 1)		// no zero div !!
				myCmd->vol	= 0x10 + from + ((to-from) * row) / (thePcmd->length-1);
			
			// my fade command : 0x10 min vol, 0x50 : max vol
			// Refer to MAD description for more informations
		}
	}

	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADNoErr);
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

#if 0
- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

@end
