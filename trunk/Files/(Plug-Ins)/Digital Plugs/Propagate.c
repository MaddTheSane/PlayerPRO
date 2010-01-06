/*	Complex Fade	*/
/*	v 1.0			*/
/*	1997 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include <PlayerPROCore/PlayerPROCore.h>

Cmd* GetCmd( short row, short	track, Pcmd*	myPcmd)
{
	if( row < 0) row = 0;
	else if( row >= myPcmd->length) row = myPcmd->length -1;

	if( track < 0) track = 0;
	else if( track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return( &(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

OSErr mainPropagate( Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit, mode, track, row;
	Str255				tStr;
		
	for( track = 0; track < myPcmd->tracks; track ++)
	{
		for( row = 0; row < myPcmd->length; row ++)
		{
			Cmd		*myCmd, *myCmdsrc;
			
			myCmdsrc = GetCmd( 0, 0, myPcmd);
			
			myCmd	 = GetCmd( row, track, myPcmd);
			
			*myCmd = *myCmdsrc;
		}
	}
			
	return noErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x7E, 0xD0, 0x0D, 0xF2, 0xDA, 0x2B, 0x4A, 0xCB, 0xAF, 0x6E, 0x54, 0x30, 0xE9, 0x79, 0xD5, 0x5F)
//7ED00DF2-DA2B-4ACB-AF6E-5430E979D55F
#define PLUGINFACTORY PropagateFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainPropagate //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"

