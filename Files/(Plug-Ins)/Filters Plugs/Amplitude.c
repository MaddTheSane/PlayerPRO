#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include <Carbon/Carbon.h>

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

static void SetDText (DialogPtr dlog, short item, Str255 str)
{
	ControlHandle	control;

	GetDialogItemAsControl( dlog, item, &control );
	SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
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
			ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		}
		while ((itemHit != ok) && (itemHit != cancel));
		
		if (itemHit == ok)
		{
			theResult = true;
			GetDialogItem( theDialog, 3,&iType,&iHandle,&iRect);
			GetDialogItemText( iHandle, textStr);
			StringToNum( textStr, p1);
		}
		DisposeDialog(theDialog);
	}
	return theResult;
}

static OSErr mainAmplitude(	sData					*theData,
						long					SelectionStart,
						long					SelectionEnd,
						PPInfoPlug				*thePPInfoPlug,
						short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i, temp, Inc;
	Ptr		Sample8Ptr = theData->data;
	short	*Sample16Ptr = (short*) theData->data;

	Inc = 120;
	if( getParams( &Inc, thePPInfoPlug))
	{
		switch( theData->amp)
		{
			case 8:
				Sample8Ptr += SelectionStart;
				
				for( i = 0; i < SelectionEnd - SelectionStart; i++)
				{
					temp = *Sample8Ptr;
					if( temp >= 0x80) temp -= 0xFF;
						
					temp *= Inc;
					temp /= 100L;
					if( temp >= 127) temp = 127;
					else if( temp <= -127 ) temp = -127;
						
					*Sample8Ptr = temp;
					
					if( StereoMode)
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
					temp /= 100L;
					
					if( temp >= (short) 0x7FFF) temp = 0x7FFF;	// overflow ?
					else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
					
					*Sample16Ptr = temp;
					
					if( StereoMode)
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

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0xF1, 0xE5, 0xEA, 0x0B, 0x1A, 0xBC, 0x4D, 0x5F, 0x89, 0xDF, 0x3C, 0x89, 0x75, 0x8E, 0xAC, 0x0E))
//F1E5EA0B-1ABC-4D5F-89DF-3C89758EAC0E

#define PLUGMAIN mainAmplitude
#define PLUGINFACTORY AmplitudeFactory

#include "CFPlugin-FilterBridge.c"
