/*	Complex Fade	*/
/*	v 1.0			*/
/*	1997 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include <PlayerPROCore/PlayerPROCore.h>

static OSErr mainRevert(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
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

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x33, 0x7A, 0xC1, 0x07, 0x22, 0xD8, 0x44, 0x1F, 0x86, 0x39, 0xE2, 0xEE, 0xE8, 0xE3, 0x45, 0x92)
//337AC107-22D8-441F-8639-E2EEE8E34592
#define PLUGINFACTORY RevertFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainRevert //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
