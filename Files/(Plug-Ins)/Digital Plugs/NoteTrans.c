/*	Note Translate	*/
/*	v 1.0			*/
/*	1995 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include "MAD.h"
#include "PPPlug.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( Pcmd*)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

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

GDHandle	TheGDevice:0xCC8;

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
		if( aH != 0L)
		{
			if( PtInRect( mouse, &(*(*aH)->gdPMap)->bounds))
			{
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect( &ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	}
	while( aH != 0L);
	
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

Cmd* GetCmd( short row, short	track, Pcmd*	myPcmd)
{
	if( row < 0) row = 0;
	else if( row >= myPcmd->length) row = myPcmd->length -1;

	if( track < 0) track = 0;
	else if( track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return( &(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

/** Main function **/

OSErr main( 	Pcmd					*myPcmd,
				PPInfoPlug				*thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit;
	Str255				tStr;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif

	myDia = GetNewDialog( 128, 0L, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);

	SetDText( myDia, 3, "\p0");
	SelectDialogItemText( myDia, 3, 0, 200);
	
	do
	{
		RESTART:
	
		#if defined(powerc) || defined(__powerc)
		ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		#else
		ModalDialog( (ModalFilterProcPtr) thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		#endif
		
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		short	track, row;
		long	trans;
		Cmd		*myCmd;
	
		GetDText( myDia, 3, tStr);		StringToNum( tStr, &trans);
		
		// Check values
		
		if( trans < -96 || trans > 96)
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
				
				if( myCmd->note != 0xFF && myCmd->note != 0xFE)		// no notes = 0xFF
				{
					if( (long) myCmd->note + trans < 0) myCmd->note = 0;
					else if( (long) myCmd->note + trans >= 96) myCmd->note = 96-1;
					else myCmd->note += trans;
				}
			}
		}
	}
	
	DisposeDialog( myDia);
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return noErr;
}