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
	for (short track = 0; track < myPcmd->tracks; track ++) {
		for (short row = 0; row < myPcmd->length; row ++) {
			if (row == 0 && track == 0) {
				continue;
			}
			Cmd *myCmd, *myCmdsrc;
			myCmdsrc	= MADGetCmd(0, 0, myPcmd);
			myCmd		= MADGetCmd(row, track, myPcmd);
			*myCmd		= *myCmdsrc;
		}
	}
	
	return MADNoErr;
}

@end
