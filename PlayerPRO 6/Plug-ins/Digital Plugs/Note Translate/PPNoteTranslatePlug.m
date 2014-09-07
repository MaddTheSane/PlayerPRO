//
//  PPNoteTranslatePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPNoteTranslatePlug.h"

@implementation PPNoteTranslatePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver
{
	return MADUnknownErr;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentDocument:(NSDocument *)document handler:(void (^)(MADErr))handle
{
	
}

@end
