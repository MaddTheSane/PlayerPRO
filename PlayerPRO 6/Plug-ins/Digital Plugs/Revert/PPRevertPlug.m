//
//  PPRevertPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import "PPRevertPlug.h"

@implementation PPRevertPlug

+ (BOOL)hasUIConfiguration
{
	return NO;
}

- (MADErr)runWithPcmd:(inout Pcmd*)myPcmd pluginInfo:(in PPInfoPlug *)thePPInfoPlug
{
	short	track, row;
	Pcmd	*srcCmd;
	long	memSize;
	
	memSize = myPcmd->structSize;
	
	srcCmd = (Pcmd*)calloc(memSize, 1);
	srcCmd->length = myPcmd->length;
	srcCmd->tracks = myPcmd->tracks;
	
	for (track = 0; track < myPcmd->tracks; track ++) {
		for (row = 0; row < myPcmd->length; row ++) {
			Cmd *myCmd, *myCmdsrc;
			myCmdsrc	= MADGetCmd(row, track, srcCmd);
			myCmd		= MADGetCmd(row, track, myPcmd);
			*myCmdsrc	= *myCmd;
		}
	}
	
	for (track = 0; track < myPcmd->tracks; track ++) {
		for (row = 0; row < myPcmd->length; row ++) {
			Cmd *myCmd, *myCmdsrc;
			myCmdsrc	= MADGetCmd(myPcmd->length - 1 - row, track, srcCmd);
			myCmd		= MADGetCmd(row, track, myPcmd);
			*myCmd		= *myCmdsrc;
		}
	}
	
	free(srcCmd);
	
	return MADNoErr;
}

@end
