/*	Note Translate	*/
/*	v 1.0			*/
/*	1995 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include "OldCarbHeaders.h"

/** Utils Functions **/

void GetDText (DialogPtr dlog, short item, StringPtr str)
{
	Handle	itemHandle;
	short	itemType;
	Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	GetDialogItemText (itemHandle, str);
}

void SetDText (DialogPtr dlog, short item, Str255 str)
{
	ControlHandle	control;
	OSErr			err;

	GetDialogItemAsControl( dlog, item, &control );
	err = SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
}

GDHandle	TheGDevice/*:0xCC8*/;

void AutoPosition( DialogPtr aDia)
{
	Point		Position, mouse;
	Rect		ViewRect, caRect;
	short		XSize, YSize;
	GDHandle	aH;
	BitMap		screenBits;
	
	GetMouse( &mouse);
	LocalToGlobal( &mouse);
	
	GetPortBounds( GetDialogPort( aDia), &caRect);
	
	XSize = (caRect.right - caRect.left);
	YSize = (caRect.bottom - caRect.top);
	
	GetQDGlobalsScreenBits( &screenBits);
	
	SetRect( &ViewRect, screenBits.bounds.left + 8, screenBits.bounds.top + 43,
						screenBits.bounds.right - 8, screenBits.bounds.bottom - 8);
	
	aH = GetDeviceList();
	do
	{
		aH = GetNextDevice( aH);
		if (aH != NULL)
		{
			if (PtInRect( mouse, &(*(*aH)->gdPMap)->bounds))
			{
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect( &ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	}
	while( aH != NULL);
	
	Position.h = mouse.h - XSize/2;
	if (Position.h + XSize >= ViewRect.right) Position.h = ViewRect.right - XSize;
	else if (Position.h <= ViewRect.left) Position.h = ViewRect.left;

	Position.v = mouse.v - YSize/2;
	if (Position.v + YSize >= ViewRect.bottom) Position.v = ViewRect.bottom - YSize;
	else if (Position.v <= ViewRect.top) Position.v = ViewRect.top;

	SetDialogDefaultItem( aDia, 1 );
	SetDialogCancelItem( aDia, 2 );

	MoveWindow( GetDialogWindow( aDia), Position.h, Position.v, false);

	ShowWindow( GetDialogWindow( aDia));
}

static Boolean PPModalDialogRep(DialogRef theDialog, EventRecord *theEvent, DialogItemIndex *itemHit)
{
	short			thePart;
	WindowPtr		whichWindow;
	short			LoopSet;
	GrafPtr			savePort;
	
	short dialogModifiers = theEvent->modifiers;
	*itemHit = 0;
	
	if (theEvent->what == updateEvt)
	{
		if ((WindowPtr) theEvent->message == GetDialogWindow( theDialog))
		{
			switch( GetWRefCon( GetDialogWindow( theDialog)))
			{
					//case 0:		oldFrameButton( theDialog);					break;
					//case 96:	oldFrameButton( theDialog);					break;
					//case 9996:	oldFrameButton( theDialog);					break;
					//case 9987:	DrawChooseColorWindow();					break;
					//case 9467:	UpdateEditInstruWindow( theDialog);			break;
					//case 7311:	UpdateSoundQualityExportSndWindow( theDialog);	break;
					//case 966:	UpdateFileInformations( theDialog);			break;
					//case 99802:	UpdatePlugsAbout( theDialog);					break;
			}
			
			return false;
		}
		//DoUpdateEvent( theEvt);
		
		*itemHit = -updateEvt;
		
		return( true);
	}
	else if(theEvent->what == mouseDown)
	{
		thePart = FindWindow( theEvent->where, &whichWindow);
		
		if (thePart == inDrag)
		{
			BitMap		screenBits;
			
			if (whichWindow != GetDialogWindow( theDialog)) return( false);
			
			GetQDGlobalsScreenBits( &screenBits);
			
			DragWindow( whichWindow, theEvent->where, &screenBits.bounds);
			return( true);
		}
		else return( false);
	}
	else if (theEvent->what == keyDown)
	{
		switch ( (theEvent->message) & charCodeMask )
		{
			case 0x0d:
			case 0x03:
				*itemHit = 1;
				return( true );
			case 0x1b:
				*itemHit = 2;
				return( true );
			default:
				return( false );
		}
	}
	else if (theEvent->what == nullEvent)
	{
		//ProcessSerialNumber	PSN;
		
		//LoopSet = thePrefs.LoopType;
		//thePrefs.LoopType = 4;
		
		//if (GetWRefCon( GetDialogWindow( theDlg)) != 9996) DoGlobalNull();
		
		//thePrefs.LoopType = LoopSet;
		
		/*GetCurrentProcess( &PSN);
		 if(	PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		 PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN)
		 {
		 SetFrontProcess( &playerPROPSN);
		 }*/
		
		*itemHit = -5;
		return( true );
	}
	/*	else if (theEvt->what == activateEvt)
	 {
	 if (AHelpDlog != NULL)
	 {
	 if (MacIsWindowVisible( GetDialogWindow( AHelpDlog))) ActivateProcedure( true);
	 }
	 }	*/
	
	return( false);
}


/** Main function **/

static OSErr mainNoteTrans(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit;
	Str255				tStr;
	
	myDia = GetNewDialog( 128, NULL, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);
	
	SetDText( myDia, 3, "\p0");
	SelectDialogItemText( myDia, 3, 0, 200);
	
	ModalFilterUPP PPModalFilter = NewModalFilterUPP(PPModalDialogRep);
	
	do {
	RESTART:
		
		ModalDialog( PPModalFilter, &itemHit);
		
	} while( itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		short	track, row;
		long	trans;
		Cmd		*myCmd;
		
		GetDText( myDia, 3, tStr);		StringToNum( tStr, &trans);
		
		// Check values
		
		if (trans < -96 || trans > 96)
		{
			SelectDialogItemText( myDia, 3, 0, 200);
			SysBeep( 1);
			goto RESTART;
		}
		
		for( track = 0; track < myPcmd->tracks; track++)
		{
			for( row = 0; row < myPcmd->length; row++)
			{
				myCmd = GetCmd( row, track, myPcmd);
				
				if (myCmd->note != 0xFF && myCmd->note != 0xFE)		// no notes = 0xFF
				{
					if ((long) myCmd->note + trans < 0) myCmd->note = 0;
					else if ((long) myCmd->note + trans >= 96) myCmd->note = 96-1;
					else myCmd->note += trans;
				}
			}
		}
	}
	
	DisposeDialog( myDia);
	
	return noErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xF9, 0x97, 0xDE, 0x0A, 0xC9, 0x56, 0x4A, 0x71, 0x93, 0x78, 0x14, 0x20, 0x50, 0x83, 0x94, 0xCC)
//F997DE0A-C956-4A71-9378-1420508394CC
#define PLUGINFACTORY NoteTransFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainNoteTrans //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
