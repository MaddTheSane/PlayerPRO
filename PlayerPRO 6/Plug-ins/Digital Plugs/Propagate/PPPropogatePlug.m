//
//  PPPropogatePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import "PPPropogatePlug.h"

@implementation PPPropogatePlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (MADErr)runWithPcmd:(inout Pcmd*)myPcmd driver:(PPDriver *)driver
{
	short track, row;
	
	for (track = 0; track < myPcmd->tracks; track ++) {
		for (row = 0; row < myPcmd->length; row ++) {
			Cmd *myCmd, *myCmdsrc;
			myCmdsrc	= MADGetCmd(0, 0, myPcmd);
			myCmd		= MADGetCmd(row, track, myPcmd);
			*myCmd		= *myCmdsrc;
		}
	}
	
	return MADNoErr;
}

@end
