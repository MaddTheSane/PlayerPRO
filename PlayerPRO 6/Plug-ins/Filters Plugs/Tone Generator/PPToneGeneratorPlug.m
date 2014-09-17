//
//  PPToneGeneratorPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPToneGeneratorPlug.h"
#import "PPToneGeneratorController.h"
@import PlayerPROKit.PPSampleObject;

@implementation PPToneGeneratorPlug

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
	long	AudioLength;
	int		AudioFreq = 440;
	
	AudioLength = selRange.length;
	if (theData.amplitude == 16)
		AudioLength /= 2;
	if (theData.stereo)
		AudioLength /= 2;
	
	/********************/
	
	PPToneGeneratorController *controller = [[PPToneGeneratorController alloc] initWithWindowNibName:@"PPToneGeneratorController"];
	
	controller.theData = theData;
	controller.audioLength = (int)AudioLength;
	controller.audioAmplitude = 1.0;
	controller.audioFrequency = AudioFreq;
	controller.selectionStart = selRange.location;
	controller.selectionEnd = NSMaxRange(selRange);
	controller.stereoMode = StereoMode;
	controller.theDriver = driver;
	controller.currentBlock = handle;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
