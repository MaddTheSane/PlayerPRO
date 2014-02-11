/*	Complex Fade	*/
/*	v 1.0			*/
/*	1997 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include <PlayerPROCore/PlayerPROCore.h>

static Cmd* GetCmd(short row, short track, Pcmd* myPcmd)
{
	if (row < 0)
		row = 0;
	else if (row >= myPcmd->length)
		row = myPcmd->length -1;
	
	if (track < 0)
		track = 0;
	else if (track >= myPcmd->tracks)
		track = myPcmd->tracks -1;
	
	return &(myPcmd->myCmd[(myPcmd->length * track) + row]);
}

static OSErr mainRevert(Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	short	track, row;
	Pcmd	*srcCmd;
	long	memSize;
	
	memSize = GetPtrSize((Ptr)myPcmd);
	
	srcCmd = (Pcmd*)NewPtrClear(memSize);
	
	srcCmd->length = myPcmd->length;
	srcCmd->tracks = myPcmd->tracks;
	
	for (track = 0; track < myPcmd->tracks; track++) {
		for (row = 0; row < myPcmd->length; row++) {
			Cmd		*myCmd, *myCmdsrc;
			
			myCmdsrc = GetCmd(row, track, srcCmd);
			myCmd	 = GetCmd(row, track, myPcmd);
			*myCmdsrc = *myCmd;
		}
	}
	
	for (track = 0; track < myPcmd->tracks; track ++) {
		for (row = 0; row < myPcmd->length; row ++) {
			Cmd		*myCmd, *myCmdsrc;
			myCmdsrc = GetCmd(myPcmd->length - 1 - row, track, srcCmd);
			myCmd	 = GetCmd(row, track, myPcmd);
			*myCmd = *myCmdsrc;
		}
	}
	
	DisposePtr((Ptr)srcCmd);
	
	return noErr;
}

//337AC107-22D8-441F-8639-E2EEE8E34592
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x33, 0x7A, 0xC1, 0x07, 0x22, 0xD8, 0x44, 0x1F, 0x86, 0x39, 0xE2, 0xEE, 0xE8, 0xE3, 0x45, 0x92)
#define PLUGINFACTORY RevertFactory 
#define PLUGMAIN mainRevert 

#include "CFPlugin-DigitalBridge.c"
