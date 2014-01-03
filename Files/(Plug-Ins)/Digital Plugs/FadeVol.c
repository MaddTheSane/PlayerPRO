/*	Fade In Vol		*/
/*	v 1.0			*/
/*	1995 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include <PlayerPROCore/PlayerPROCore.h>

static void GetDText (DialogPtr dlog, short item, StringPtr str)
{
	Handle	itemHandle;
	short	itemType;
	Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	GetDialogItemText (itemHandle, str);
}

static void SetDText (DialogPtr dlog, short item, Str255 str)
{
	//Handle			itemHandle;
	//short			itemType;
	//Rect			itemRect;
	//Str255			myStr;
	ControlHandle	control;
	OSErr			err;

	GetDialogItemAsControl(dlog, item, &control );
	err = SetControlData(control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl(control);
}

static void AutoPosition(DialogPtr aDia)
{
	Point		Position, mouse;
	Rect		ViewRect, caRect;
	short		XSize, YSize;
	GDHandle	aH;
	BitMap		screenBits;
	
	GetMouse(&mouse);
	LocalToGlobal(&mouse);
	
	GetPortBounds(GetDialogPort(aDia), &caRect);
	
	XSize = (caRect.right - caRect.left);
	YSize = (caRect.bottom - caRect.top);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	SetRect(&ViewRect, screenBits.bounds.left + 8, screenBits.bounds.top + 43,
						screenBits.bounds.right - 8, screenBits.bounds.bottom - 8);
	
	aH = GetDeviceList();
	do
	{
		aH = GetNextDevice(aH);
		if (aH != NULL)
		{
			if (PtInRect(mouse, &(*(*aH)->gdPMap)->bounds))
			{
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect(&ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	}
	while (aH != NULL);
	
	Position.h = mouse.h - XSize/2;
	if (Position.h + XSize >= ViewRect.right) Position.h = ViewRect.right - XSize;
	else if (Position.h <= ViewRect.left) Position.h = ViewRect.left;

	Position.v = mouse.v - YSize/2;
	if (Position.v + YSize >= ViewRect.bottom) Position.v = ViewRect.bottom - YSize;
	else if (Position.v <= ViewRect.top) Position.v = ViewRect.top;

	SetDialogDefaultItem(aDia, 1 );
	SetDialogCancelItem(aDia, 2 );

	MoveWindow(GetDialogWindow(aDia), Position.h, Position.v, false);

	ShowWindow(GetDialogWindow(aDia));
}

static Cmd* GetCmd(short row, short	track, Pcmd*	myPcmd)
{
	if (row < 0) row = 0;
	else if (row >= myPcmd->length) row = myPcmd->length -1;

	if (track < 0) track = 0;
	else if (track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return(&(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

static OSErr mainFadeVol(Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit;
	Str255				tStr;
	
	myDia = GetNewDialog(128, NULL, (WindowPtr) -1L);
	SetPortDialogPort(myDia);
	AutoPosition(myDia);

	SetDText(myDia, 3, "\p0");
	SetDText(myDia, 4, "\p100");
	SelectDialogItemText(myDia, 3, 0, 200);
	
	do
	{
		RESTART:
	
		ModalDialog(thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		
	}while (itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		short	track, row;
		long	from, to;
		Cmd		*myCmd;
	
		GetDText(myDia, 3, tStr);		StringToNum(tStr, &from);
		GetDText(myDia, 4, tStr);		StringToNum(tStr, &to);
		
		// Check values
		
		if (from < 0 || from > 100)
		{
			SelectDialogItemText(myDia, 3, 0, 200);
			SysBeep(1);
			goto RESTART;
		}
		
		if (to < 0 || to > 100)
		{
			SelectDialogItemText(myDia, 4, 0, 200);
			SysBeep(1);
			goto RESTART;
		}
		
		// Re-adjust val % -> 0...64
		
		to 		= (to * 64) / 100;
		from 	= (from * 64) / 100;
		
		for (track = 0; track < myPcmd->tracks; track++)
		{
			for (row = 0; row < myPcmd->length; row++)
			{
				myCmd = GetCmd(row, track, myPcmd);
			
				myCmd->ins 	= myCmd->ins;		// is this very usefull ?
				myCmd->note	= myCmd->note;		// is this very usefull ?
				myCmd->cmd	= myCmd->cmd;		// is this very usefull ?
				myCmd->arg	= myCmd->arg;		// is this very usefull ?
				
				if (myPcmd->length > 1)			// no zero div !!
					myCmd->vol	= 0x10 + from + ((to-from) * row) / (myPcmd->length-1);
				
				// my fade command : 0x10 min vol, 0x50 : max vol
				// Refer to MAD description for more informations			
			}
		}
	}
	
	DisposeDialog(myDia);
	
	return noErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x02, 0xB1, 0x55, 0x4B, 0xDE, 0x52, 0x47, 0x45, 0x93, 0x2C, 0x29, 0x87, 0xAA, 0x19, 0xD4, 0xEF)
//02B1554B-DE52-4745-932C-2987AA19D4EF
#define PLUGINFACTORY FadeVolFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeVol //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"

