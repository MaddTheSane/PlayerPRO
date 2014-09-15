//
//  PPFadeVolumePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPFadeVolumePlug.h"
#import "FadeVolumeController.h"

@implementation PPFadeVolumePlug 

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle
{
	FadeVolumeController *controller = [[FadeVolumeController alloc] initWithWindowNibName:@"FadeVolumeController"];
	controller.thePcmd = aPcmd;
	controller.fadeFrom = 0.0;
	controller.fadeTo = 1.0;
	controller.currentBlock = handle;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
