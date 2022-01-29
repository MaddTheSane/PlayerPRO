/*	Echo			*/
/*	v 0.3			*/
/*	1995 by Liane	*/

//	Usage:
//	Simulates an echo.
//	You can set the timing (ms), which is computed
//	for a F#5.
//	Works on the selected part or all the waveform
//	if there is no selection.

#include <Dialogs.h>
#include "MAD.h"
#include "PPPlug.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( sData*)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( PPInfoPlug*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( long)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif


#define tdelay		3
#define tstrength	4
#define enclosure	9

GDHandle	TheGDevice:0xCC8;

void SetDText (DialogPtr dlog, short item, Str255 str)
{
ControlHandle	control;

	GetDialogItemAsControl( dlog, item, &control );
	SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
}

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

void raiseRect (Rect theRect)
{
	ForeColor(whiteColor);
	MoveTo(theRect.left,theRect.bottom);
	LineTo(theRect.left,theRect.top);
	LineTo(theRect.right,theRect.top);
	ForeColor(blackColor);
	LineTo(theRect.right,theRect.bottom);
	LineTo(theRect.left,theRect.bottom);
}

#define timeConvert		22254 //�22KHZ

pascal void xRectProc (DialogPtr	theWindow,
						short		theItem)
{
short	iType;
Handle	iHandle;
Rect	iRect;

	GetDialogItem(theWindow,theItem,&iType,&iHandle,&iRect);
	raiseRect( iRect);
}

Boolean getParams ( short dlgID, long *p1, long *p2, PPInfoPlug *thePPInfoPlug)
{
DialogPtr	theDialog;
Boolean		theResult = false;

	theDialog = GetNewDialog(dlgID,nil,(WindowPtr)-1);
	if (theDialog) {
		short	iType, itemHit;
		Handle	iHandle;
		Rect	iRect;
		Str255	textStr;
		
		SetPortDialogPort( theDialog);
		AutoPosition( theDialog);
		GetDialogItem(theDialog,enclosure,&iType,&iHandle,&iRect);
		SetDialogItem(theDialog,enclosure,iType,(Handle) NewUserItemUPP(xRectProc),&iRect);
		
		NumToString(*p1,textStr);
		SetDText( theDialog, tdelay, textStr);
		
		NumToString(*p2,textStr);
		SetDText( theDialog, tstrength, textStr);
		SelectDialogItemText(theDialog,tdelay,0,32767);

	//	SetDialogDefaultItem(theDialog,1);

		do
		{
			ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		}
		while ((itemHit != ok) && (itemHit != cancel));
		
		if (itemHit == ok)
		{
			theResult = true;
			GetDialogItem(theDialog,tdelay,&iType,&iHandle,&iRect);
			GetDialogItemText(iHandle,textStr);
			StringToNum(textStr,p1);
			GetDialogItem(theDialog,tstrength,&iType,&iHandle,&iRect);
			GetDialogItemText(iHandle,textStr);
			StringToNum(textStr,p2);
		}
		DisposeDialog(theDialog);
	}
	return theResult;
}

int checkMax (int v)
{
	if( v >= 127) return 127;
	else if( v <= -127 ) return -127;
	else return v;
}

OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
long	i, length,
		temp1, temp2,
		pDelay = 250, pStrength = 50;

	if (getParams (5000, &pDelay, &pStrength, thePPInfoPlug))
	{
		length = SelectionEnd - SelectionStart - 1;

		pDelay = (pDelay * timeConvert) / 1000;	//convert ms to samples

		switch( theData->amp)
		{
			case 8:
			{
				Ptr	orgPtr = (theData->data) + SelectionStart, destPtr = orgPtr + pDelay;
				
				for( i = 0; i < (length - pDelay); i++)
				{
					temp1 = *orgPtr++;
					temp1 = (pStrength * temp1) / 100;
					
					temp2 = *destPtr;
					
					temp1 += temp2;
					
					if( temp1 >= 127) temp1 = 127;	// overflow ?
					else if( temp1 <= -128 ) temp1 = -128;
					
					*destPtr++ = temp1;
				}
			}
			break;

			case 16:
			{
				short	*orgPtr = (short*) theData->data + (SelectionStart / 2),
				*destPtr = orgPtr + pDelay;
				
				for( i = 0; i < length / 2 - pDelay; i++)
				{
					temp1 = *orgPtr++;
					temp1 = (pStrength * temp1) / 100;
					
					temp2 = *destPtr;
					
					temp1 += temp2;
					
					if( temp1 >= (short)0x7FFF) temp1 = 0x7FFF;	// overflow ?
					else if( temp1 <= (short)0x8000 ) temp1 = (short)0x8000;
					
					*destPtr++ = temp1;
				}
			}
			break;
		}
	}
	
	return noErr;
}