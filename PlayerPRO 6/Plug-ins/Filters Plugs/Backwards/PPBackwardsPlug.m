//
//  PPBackwardsPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPBackwardsPlug.h"

@implementation PPBackwardsPlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (MADErr)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

@end
