//
//  FadeVolumeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeVolumeController.h"

@implementation FadeVolumeController

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		isMultipleIstanceSafe = YES;
		// Initialization code here.
		dispatch_block_t tmpBlock = ^{
			short	track, row;
			long	from, to;
			Cmd		*myCmd;
			
			// Re-adjust val % -> 0...64
			to 		= (self.fadeTo * 64);
			from 	= (self.fadeFrom * 64);
			
			for (track = 0; track < thePcmd->tracks; track++) {
				for (row = 0; row < thePcmd->length; row++) {
					myCmd = GetCmd(row, track, thePcmd);
					
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
		};
		plugBlock = tmpBlock;
	}
	
	return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end


static OSErr mainFadeVol(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	FadeVolumeController *controller = [[FadeVolumeController alloc] initWithWindowNibName:@"FadeVolumeController" infoPlug:thePPInfoPlug];
	controller.thePcmd = myPcmd;
	controller.fadeFrom = 0.0;
	controller.fadeTo = 1.0;
	
	return [controller runAsSheet];
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x02, 0xB1, 0x55, 0x4B, 0xDE, 0x52, 0x47, 0x45, 0x93, 0x2C, 0x29, 0x87, 0xAA, 0x19, 0xD4, 0xEF)
//02B1554B-DE52-4745-932C-2987AA19D4EF
#define PLUGINFACTORY FadeVolFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeVol //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"

