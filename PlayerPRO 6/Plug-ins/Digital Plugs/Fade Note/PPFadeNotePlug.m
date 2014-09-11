//
//  PPFadeNotePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPFadeNotePlug.h"
#import "FadeNoteController.h"

@implementation PPFadeNotePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentDocument:(NSDocument *)document handler:(PPPlugErrorBlock)handle
{
	FadeNoteController *controller = [[FadeNoteController alloc] initWithWindowNibName:@"FadeNoteController"];
	controller.thePcmd = aPcmd;
	controller.initialFrom = @"C 3";
	controller.initialTo = @"C 7";
	controller.currentBlock = handle;
	
	[[controller window] beginSheet:[document windowForSheet] completionHandler:^(NSModalResponse returnCode) {
		;
	}];
}

@end
