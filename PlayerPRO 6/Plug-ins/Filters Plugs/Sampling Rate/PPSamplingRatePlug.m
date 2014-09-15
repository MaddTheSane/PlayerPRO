//
//  PPSamplingRatePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPSamplingRatePlug.h"
#import "SamplingRateWindowController.h"

@implementation PPSamplingRatePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver;
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle;
{
	SamplingRateWindowController *controller = [[SamplingRateWindowController alloc] initWithWindowNibName:@"SamplingRateWindowController"];
	controller.currentRate = controller.changedRate = theData.c2spd;
	controller.theData = theData;
	controller.currentBlock = handle;
	controller.selectionRange = selRange;
	controller.stereoMode = StereoMode;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
