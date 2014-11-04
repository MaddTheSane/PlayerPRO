//
//  PPLengthPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 11/3/14.
//
//

#import "PPLengthPlug.h"
#import "LengthWindowController.h"

@implementation PPLengthPlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler
{
	
}

@end
