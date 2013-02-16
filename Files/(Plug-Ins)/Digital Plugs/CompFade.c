/*	Complex Fade	*/
/*	v 1.0			*/
/*	1997 by ANR		*/

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
	ControlHandle	control;
	OSErr			err;

	GetDialogItemAsControl( dlog, item, &control );
	err = SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
}


static void TurnRadio( short item, DialogPtr dlog, Boolean alors)
{
	Handle		itemHandle;
	short		itemType;
	Rect		itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);

	if( alors) SetControlValue( (ControlHandle) itemHandle, 255);
	
	else SetControlValue( (ControlHandle) itemHandle, 0);
	
}

static void AutoPosition( DialogPtr aDia)
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
		if( aH != NULL)
		{
			if( PtInRect( mouse, &(*(*aH)->gdPMap)->bounds))
			{
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect( &ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	}
	while( aH != NULL);
	
	Position.h = mouse.h - XSize/2;
	if( Position.h + XSize >= ViewRect.right) Position.h = ViewRect.right - XSize;
	else if( Position.h <= ViewRect.left) Position.h = ViewRect.left;

	Position.v = mouse.v - YSize/2;
	if( Position.v + YSize >= ViewRect.bottom) Position.v = ViewRect.bottom - YSize;
	else if( Position.v <= ViewRect.top) Position.v = ViewRect.top;

	SetDialogDefaultItem( aDia, 1 );
	SetDialogCancelItem( aDia, 2 );

	MoveWindow( GetDialogWindow( aDia), Position.h, Position.v, false);

	ShowWindow( GetDialogWindow( aDia));
}

static Cmd* GetCmd( short row, short	track, Pcmd*	myPcmd)
{
	if( row < 0) row = 0;
	else if( row >= myPcmd->length) row = myPcmd->length -1;

	if( track < 0) track = 0;
	else if( track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return( &(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

static short Text2Note( Str255 myTT)
{
	short		Oct;
	
	if( myTT[ 0] > 2) Oct = myTT[ 3] - 48;
	else Oct = myTT[ 2] - 48;
	
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	switch( myTT[1])
	{
		case 'C':	case'c':	Oct += 0;	break;
		case 'D':	case'd':	Oct += 2;	break;
		case 'E':	case'e':	Oct += 4;	break;
		case 'F':	case'f':	Oct += 5;	break;
		case 'G':	case'g':	Oct += 7;	break;
		case 'A':	case'a':	Oct += 9;	break;
		case 'B':	case'b':	Oct += 11;	break;
		
		default:	Oct = 0xFF;		break;
	}
	
	if( Oct != 0xFF)
	{
		if( myTT[ 2] == '#') Oct++;
		if( Oct >= 96) Oct = 0xFF;
		if( Oct < 0) Oct = 0xFF;
	}
	
	return( Oct);
}

static void StringToHex( Str255 str, long *oct)
{
	if( str[ 2] >= 'A' && str[ 2] <= 'F') *oct = 10 + str[ 2] - 'A';
	if( str[ 2] >= '0' && str[ 2] <= '9') *oct = str[ 2] - '0';
	
	if( str[ 1] >= 'A' && str[ 1] <= 'F') *oct += (10 + str[ 1] - 'A')<<4;
	if( str[ 1] >= '0' && str[ 1] <= '9') *oct += (str[ 1] - '0')<<4;
}

static OSErr mainCompFade( Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit, mode;
	Str255				tStr;
	
	myDia = GetNewDialog( 128, NULL, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);
	
	TurnRadio( 7, myDia, true);
	mode = 7;
	
	SetDText( myDia, 3, "\p1");	SetDText( myDia, 4, "\p64");
	SelectDialogItemText( myDia, 3, 0, 200);
	
	do
	{
	RESTART:
	
		ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		
		switch( itemHit)
		{
			case 7:
			case 8:
			case 9:
			case 10:
				
				TurnRadio( 7, myDia, false);
				TurnRadio( 8, myDia, false);
				TurnRadio( 9, myDia, false);
				TurnRadio( 10, myDia, false);
				
				TurnRadio( itemHit, myDia, true);
				mode = itemHit;
				
				switch( itemHit)
				{
					case 7:		SetDText( myDia, 15, "\pFrom 1 to 64");		SetDText( myDia, 3, "\p1");		SetDText( myDia, 4, "\p64");	break;
					case 8:		SetDText( myDia, 15, "\pFrom C0 to B7");	SetDText( myDia, 3, "\pC0");	SetDText( myDia, 4, "\pB7");	break;
					case 9:		SetDText( myDia, 15, "\pFrom 00 to FF");	SetDText( myDia, 3, "\p00");	SetDText( myDia, 4, "\pFF");	break;
					case 10:	SetDText( myDia, 15, "\pFrom 00 to FF");	SetDText( myDia, 3, "\p00");	SetDText( myDia, 4, "\pFF");	break;
				}
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		short	track, row;
		long	from, to, step;
		Cmd		*myCmd;
		
		// Check values
		
		switch( mode)
		{
			case 7:
				GetDText( myDia, 3, tStr);		StringToNum( tStr, &from);
				GetDText( myDia, 4, tStr);		StringToNum( tStr, &to);
				
				if( from < 1 || from > 64)	{	SelectDialogItemText( myDia, 3, 0, 200);	SysBeep( 1);	goto RESTART;}
				if( to < 1 || to > 64)		{	SelectDialogItemText( myDia, 4, 0, 200);	SysBeep( 1);	goto RESTART;}
			break;
			
			case 8:
				GetDText( myDia, 3, tStr);		from 	= Text2Note( tStr);
				GetDText( myDia, 4, tStr);		to 		= Text2Note( tStr);
				
				if( from < 0 || from >= 96)	{	SelectDialogItemText( myDia, 3, 0, 200);	SysBeep( 1);	goto RESTART;}
				if( to < 0 || to >= 96)		{	SelectDialogItemText( myDia, 4, 0, 200);	SysBeep( 1);	goto RESTART;}
			break;
			
			case 9:
				GetDText( myDia, 3, tStr);		StringToHex( tStr, &from);
				GetDText( myDia, 4, tStr);		StringToHex( tStr, &to);
				
				if( from < 0 || from > 0xFF)	{	SelectDialogItemText( myDia, 3, 0, 200);	SysBeep( 1);	goto RESTART;}
				if( to < 0 || to > 0xFF)		{	SelectDialogItemText( myDia, 4, 0, 200);	SysBeep( 1);	goto RESTART;}
			break;
			
			case 10:
				GetDText( myDia, 3, tStr);		StringToHex( tStr, &from);
				GetDText( myDia, 4, tStr);		StringToHex( tStr, &to);
				
				if( from < 0 || from > 0xFF)	{	SelectDialogItemText( myDia, 3, 0, 200);	SysBeep( 1);	goto RESTART;}
				if( to < 0 || to > 0xFF)		{	SelectDialogItemText( myDia, 4, 0, 200);	SysBeep( 1);	goto RESTART;}
			break;
		}
		
		GetDText( myDia, 11, tStr);		StringToNum( tStr, &step);
		if( step < 1 || step > 64)	{	SelectDialogItemText( myDia, 11, 0, 200);	SysBeep( 1);	goto RESTART;}
		
		for( track = 0; track < myPcmd->tracks; track ++)
		{
			for( row = 0; row < myPcmd->length; row += step)
			{
				myCmd = GetCmd( row, track, myPcmd);
				
				if( myPcmd->length > 1)			// no zero div !!
				{
					switch( mode)
					{
						case 7:		myCmd->ins	= from + ((to-from) * row) / (myPcmd->length-1);	break;
						case 8:		myCmd->note = from + ((to-from) * row) / (myPcmd->length-1);	break;
						case 9:		myCmd->arg	= from + ((to-from) * row) / (myPcmd->length-1);	break;
						case 10:	myCmd->vol	= from + ((to-from) * row) / (myPcmd->length-1);	break;
					}
				}
			}
		}
	}
	
	DisposeDialog( myDia);
		
	return noErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x2C, 0xE9, 0x02, 0x81, 0xE2, 0xC2, 0x47, 0x5A, 0xA0, 0xF0, 0xB9, 0x0C, 0x64, 0x1E, 0xAE, 0xB1)
//2CE90281-E2C2-475A-A0F0-B90C641EAEB1
#define PLUGINFACTORY CompFadeFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainCompFade //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
