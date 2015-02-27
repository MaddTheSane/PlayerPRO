//
//  PPAmplitudePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPAmplitudePlug.h"
#import "AmplitudeController.h"

@implementation PPAmplitudePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (MADErr)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithData:(PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle
{
	AmplitudeController *controller = [[AmplitudeController alloc] initWithWindowNibName:@"AmplitudeController"];
	controller.theData = theData;
	controller.selectionRange = selRange;
	controller.stereoMode = StereoMode;
	controller.currentBlock = handle;
	controller.amplitudeAmount = 120;
	controller.parentWindow = document;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
