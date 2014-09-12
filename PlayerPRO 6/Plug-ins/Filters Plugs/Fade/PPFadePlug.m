//
//  PPFadePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPFadePlug.h"
#import "FadeWindowController.h"
//@import PlayerPROKit.PPSampleObject;

@implementation PPFadePlug

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
	FadeWindowController *controller = [[FadeWindowController alloc] initWithWindowNibName:@"FadeWindowController"];
	controller.theData = theData;
	controller.selectionRange = selRange;
	controller.currentBlock = handle;
	controller.fadeTo = 1.0;
	controller.fadeFrom = .70;
	controller.stereoMode = StereoMode;
	
	[controller.window beginSheet:[document windowForSheet] completionHandler:^(NSModalResponse returnCode) {
		
	}];

}

@end
