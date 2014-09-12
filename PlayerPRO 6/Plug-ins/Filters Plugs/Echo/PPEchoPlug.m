//
//  PPEchoPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPEchoPlug.h"
#import "EchoWindowController.h"
@import PlayerPROKit;
@import Cocoa;

@implementation PPEchoPlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithData:(PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver parentDocument:(NSDocument *)document handler:(PPPlugErrorBlock)handle
{
	EchoWindowController *controller = [[EchoWindowController alloc] initWithWindowNibName:@"EchoWindowController"];
	controller.echoStrength = 0.50;
	controller.echoDelay = 250;
	controller.theData = theData;
	controller.selectionRange = selRange;
	controller.currentBlock = handle;

	[controller.window beginSheet:[document windowForSheet] completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
