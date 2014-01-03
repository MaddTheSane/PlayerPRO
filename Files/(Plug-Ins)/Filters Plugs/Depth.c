/*
	Player PRO 5.x

	Antoine ROSSET
	16 Tranchees
	1206 GENEVA
	SWITZERLAND
*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

static void SetDText (DialogPtr dlog, short item, Str255 str)
{
	ControlHandle	control;

	GetDialogItemAsControl( dlog, item, &control );
	SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
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


static Boolean getParams ( long *p1, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr	theDialog;
	Boolean		theResult = false;

	theDialog = GetNewDialog( 128,nil,(WindowPtr)-1);
	if (theDialog) {
		short	iType, itemHit;
		Handle	iHandle;
		Rect	iRect;
		Str255	textStr;
		
		SetPortDialogPort( theDialog);
		AutoPosition( theDialog);
		NumToString( *p1, textStr);
		SetDText( theDialog, 3, textStr);
		SelectDialogItemText( theDialog, 3, 0, 32767);

		do
		{
			RESTART:
			
//			#if defined(powerc) || defined(__powerc)
			ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//			#else
//			ModalDialog( (ModalFilterProcPtr) thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//			#endif
		}
		while ((itemHit != ok) && (itemHit != cancel));
		
		if (itemHit == ok)
		{
			theResult = true;
			GetDialogItem( theDialog, 3,&iType,&iHandle,&iRect);
			GetDialogItemText( iHandle, textStr);
			StringToNum( textStr, p1);
			
			if (*p1 < 1 || *p1 > 16)
			{
				SysBeep( 1);
				
				SelectDialogItemText( theDialog, 3, 0, 150);
				
				goto RESTART;
			}
			
			*p1 = 1L << *p1;
		}
		DisposeDialog(theDialog);
	}
	return theResult;
}

static OSErr mainDepth( 	sData					*theData,
					long					SelectionStart,
					long					SelectionEnd,
					PPInfoPlug				*thePPInfoPlug,
					short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long			i, temp, Inc;
	Byte			*Sample8Ptr = (Byte*) theData->data;
	short			*Sample16Ptr = ( short*) theData->data;

	Inc = 8;
	if (getParams( &Inc, thePPInfoPlug))
	{
		if (Inc == 0) Inc = 1;
		switch( theData->amp)
		{
			case 8:
				Sample8Ptr += SelectionStart;
				
				for( i = 0; i < SelectionEnd - SelectionStart; i++)
				{
					temp = *Sample8Ptr;
					
					temp *= Inc;
					temp /= 256;
					
					temp *= 256;
					temp /= Inc;
					
					*Sample8Ptr = temp;
					
					if (StereoMode)
					{
						Sample8Ptr++;
						i++;
					}
					
					Sample8Ptr++;
				}
			break;
			
			case 16:
				Sample16Ptr += SelectionStart/2;						// Div 2, because it's in bytes !!!
			
				for( i = 0; i < (SelectionEnd - SelectionStart)/2; i++)	// Div 2, because it's in bytes !!!
				{
					temp = *Sample16Ptr;
					
					temp *= Inc;
					temp /= 0x0000FFFF;
					
					temp *= 0x0000FFFF;
					temp /= Inc;
					
					*Sample16Ptr = temp;
					
					if (StereoMode)
					{
						Sample16Ptr++;
						i++;
					}
					
					Sample16Ptr++;
				}
			break;
		}
	}
	return noErr;
}

// D7F6D8C0-FC86-48E4-A2B8-61BB681DE071
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xD7, 0xF6, 0xD8, 0xC0, 0xFC, 0x86, 0x48, 0xE4, 0xA2, 0xB8, 0x61, 0xBB, 0x68, 0x1D, 0xE0, 0x71)

#define PLUGMAIN mainDepth
#define PLUGINFACTORY DepthFactory

#include "CFPlugin-FilterBridge.c"
