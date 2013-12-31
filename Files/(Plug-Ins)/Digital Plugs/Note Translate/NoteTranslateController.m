//
//  NoteTranslateController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "NoteTranslateController.h"

@interface NoteTranslateController ()

@end

@implementation NoteTranslateController

- (instancetype)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
		isMultipleIstanceSafe = YES;
		dispatch_block_t tmpBlock = ^{
			short		track, row;
			NSInteger	trans = self.transAmount;
			Cmd			*myCmd;

			for (track = 0; track < thePcmd->tracks; track++) {
				for (row = 0; row < thePcmd->length; row++) {
					myCmd = GetCmd(row, track, thePcmd);
					
					if (myCmd->note != 0xFF && myCmd->note != 0xFE) {	// no notes = 0xFF
						if (myCmd->note + trans < 0)
							myCmd->note = 0;
						else if ( myCmd->note + trans >= 96)
							myCmd->note = 96-1;
						else
							myCmd->note += trans;
					}
				}
			}
		};
		self.plugBlock = tmpBlock;
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end

static OSErr mainNoteTrans(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	NoteTranslateController *controller = [[NoteTranslateController alloc] initWithWindowNibName:@"NoteTranslateController" infoPlug:thePPInfoPlug];
	controller.thePcmd = myPcmd;
	controller.transAmount = 0;
	
	return [controller runAsSheet];
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xF9, 0x97, 0xDE, 0x0A, 0xC9, 0x56, 0x4A, 0x71, 0x93, 0x78, 0x14, 0x20, 0x50, 0x83, 0x94, 0xCC)
//F997DE0A-C956-4A71-9378-1420508394CC
#define PLUGINFACTORY NoteTransFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainNoteTrans //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"

