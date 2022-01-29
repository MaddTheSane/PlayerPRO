/*	ToneGenerator	*/
/*	v 1.1			*/
/*	1995 by ANR 	*/


#include "MAD.h"
#include "PPPlug.h"
#include "mixedmode.h"
#include "math.h"

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

enum
{
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
};

#define		PI      		3.1415926535897932384626433
#define		KHZ				22254.54545


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

	GetDialogItemAsControl( dlog, item, &control );
	SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
}

Ptr	CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	Ptr		Audio8Ptr;
	long	i, temp, inter, x, dest;
	Boolean	UpDown;
	
	if( stereo) AudioLength *= 2L;
	
	Audio8Ptr = NewPtr( AudioLength);
	if( Audio8Ptr == 0L) return 0L;
	
	switch( AudioType)
	{
		case wave:
			for( i = 0, x = 0; i < AudioLength; i++, x++)
			{
				temp = 127. * sin( ( ((double) x * (double) AudioFreq * PI * 2.) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
		break;
			
		case silence:
			for( i = 0; i < AudioLength; i++) Audio8Ptr[ i] = 0x00;
		break;
		
		case square:
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					if( inter == -127) inter = 127;
					else inter = -127;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;

				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
		break;
		
		case triangle:
			UpDown = true;
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if( UpDown) temp = (256 * (dest - i)) / inter;
				else temp = (256 * (inter - (dest - i))) / inter;
				
				temp -= 127;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;

				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
		break;
	}
	
	return Audio8Ptr;
}

short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	short	*Audio16Ptr;
	long	i, temp, inter, x, dest;
	Boolean	UpDown;

	if( stereo) AudioLength *= 2L;
	
	Audio16Ptr = (short*) NewPtr( AudioLength*2);
	if( Audio16Ptr == 0L) return 0L;

	switch( AudioType)
	{
		case wave:
			for( i = 0, x = 0; i < AudioLength; i++, x++)
			{
				temp = 32767. * sin( ( ((double) x * (double) AudioFreq * PI * 2.) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
		break;
			
		case silence:
			for( i = 0; i < AudioLength; i++) Audio16Ptr[ i] = 0x00;
		break;
		
		case square:
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					if( inter == -32767L) inter = 32767L;
					else inter = -32767L;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
		break;
		
		case triangle:
			UpDown = true;
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if( UpDown) temp = (65535L * (dest - i)) / inter;
				else temp = (65535L * (inter - (dest - i))) / inter;
				
				temp -= 32767L;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;

				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
		break;
	}

	return Audio16Ptr;
}

OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
long				i, AudioLength, AudioFreq, AudioAmp;
Ptr					Sample8Ptr = theData->data, Audio8Ptr;
short				*Sample16Ptr = (short*) theData->data, *Audio16Ptr;
DialogPtr			myDia;
short				itemHit, itemType, AudioType;
Handle				itemHandle;
Rect				itemRect;
Str255				tStr;
OSErr				iErr;

#ifndef powerc
	long	oldA4 = SetCurrentA4();			//this call is necessary for strings in 68k code resources
#endif

	myDia = GetNewDialog( 128, 0L, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);
	
	// Mono
	switch( theData->stereo)
	{
		case false:
			GetDialogItem( myDia, 22, &itemType, &itemHandle, &itemRect);
			SetControlValue( (ControlHandle) itemHandle, true);
		break;
		
		case true:
			GetDialogItem( myDia, 23, &itemType, &itemHandle, &itemRect);
			SetControlValue( (ControlHandle) itemHandle, true);
		break;
	}
	
	if( theData->size > 0)	// We CANNOT change stereo mode, since data is already here....
	{
		GetDialogItem( myDia, 22, &itemType, &itemHandle, &itemRect);		HiliteControl( (ControlHandle) itemHandle, 255);
		GetDialogItem( myDia, 23, &itemType, &itemHandle, &itemRect);		HiliteControl( (ControlHandle) itemHandle, 255);
	}
	
	AudioType = silence;
	GetDialogItem( myDia, AudioType, &itemType, &itemHandle, &itemRect);
	SetControlValue( (ControlHandle) itemHandle, true);
	
	AudioLength = SelectionEnd - SelectionStart;
	if( theData->amp == 16) AudioLength /= 2;
	if( theData->stereo) AudioLength /= 2;
	if( AudioLength <= 0) AudioLength = 2000;
	NumToString( AudioLength, tStr);	SetDText( myDia, 8, tStr);
	
	AudioFreq	= 440;								NumToString( AudioFreq, tStr);		SetDText( myDia, 9, tStr);
	AudioAmp	= 100;								NumToString( AudioAmp, tStr);		SetDText( myDia, 10, tStr);
	
	SelectDialogItemText( myDia, 8, 0, 10000);
	/********************/
	
	Audio8Ptr	= 0L;	Audio16Ptr	= 0L;

	do
	{
		ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
		
		switch( itemHit)
		{
			case 22:	// Mono
				GetDialogItem( myDia, itemHit, &itemType, &itemHandle, &itemRect);
				SetControlValue( (ControlHandle) itemHandle, true);
				theData->stereo = false;
				
				GetDialogItem( myDia, 23, &itemType, &itemHandle, &itemRect);	SetControlValue( (ControlHandle) itemHandle, false);
			break;
			
			case 23:	// Stereo
				GetDialogItem( myDia, itemHit, &itemType, &itemHandle, &itemRect);
				SetControlValue( (ControlHandle) itemHandle, true);
				theData->stereo = true;
				
				GetDialogItem( myDia, 22, &itemType, &itemHandle, &itemRect);	SetControlValue( (ControlHandle) itemHandle, false);
			break;
		
			case 7:
			{
				static OSErr			(*mPlay)		( Ptr, long, long, long, long, long, long, unsigned long, Boolean);
				
				mPlay = thePPInfoPlug->RPlaySoundUPP;
				
				GetDText( myDia, 8, tStr);		StringToNum( tStr, &AudioLength);
				GetDText( myDia, 9, tStr);		StringToNum( tStr, &AudioFreq);
				GetDText( myDia, 10, tStr);		StringToNum( tStr, &AudioAmp);
				
				switch( theData->amp)
				{
					case 8:
						if( Audio8Ptr != 0L) { DisposePtr( Audio8Ptr);			Audio8Ptr = 0L;}
						Audio8Ptr	= CreateAudio8Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);
						if( Audio8Ptr != 0L)
						{
						//	iErr = mPlay( Audio8Ptr, AudioLength, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
						}
					break;
					
					case 16:
						if( Audio16Ptr != 0L) { DisposePtr( (Ptr) Audio16Ptr);	Audio16Ptr = 0L;}
						Audio16Ptr	= CreateAudio16Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);
						if( Audio16Ptr != 0L)
						{
						//	iErr = mPlay( (Ptr) Audio16Ptr, AudioLength*2, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
						}
					break;
				}
			}
			break;
		
			case 20:
			case 19:
			case 17:
			case 18:
			case 16:
			case 3:
			case 4:
			case 5:
			case 6:
			
				switch( itemHit)
				{
					case 19:
					case 20:	itemHit = 3;	break;
					case 18:	itemHit = 5;	break;
					case 17:	itemHit = 4;	break;
					case 16:	itemHit = 6;	break;
				}
			
				for( i = 3; i <= 6;i ++)
				{
					GetDialogItem( myDia, i, &itemType, &itemHandle, &itemRect);
					SetControlValue( (ControlHandle) itemHandle, 0);
				}
				
				AudioType = itemHit;
				GetDialogItem( myDia, AudioType, &itemType, &itemHandle, &itemRect);
				SetControlValue( (ControlHandle) itemHandle, true);
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		Ptr		resultPtr;
		
		GetDText( myDia, 8, tStr);		StringToNum( tStr, &AudioLength);
		GetDText( myDia, 9, tStr);		StringToNum( tStr, &AudioFreq);
		GetDText( myDia, 10, tStr);		StringToNum( tStr, &AudioAmp);
		
		if( Audio16Ptr != 0L)	{	DisposePtr( (Ptr) Audio16Ptr);			Audio16Ptr = 0L;}
		if( Audio8Ptr != 0L)	{	DisposePtr( (Ptr) Audio8Ptr);			Audio8Ptr = 0L;}
		
		switch( theData->amp)
		{
			case 8:		Audio8Ptr	= CreateAudio8Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);	break;
			case 16:
				Audio16Ptr	= CreateAudio16Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);
				AudioLength *= 2;
			break;
		}
		
		if( theData->stereo) AudioLength *= 2;
		
		resultPtr = NewPtr( theData->size - (SelectionEnd - SelectionStart) + AudioLength);
		
		BlockMoveData( theData->data, resultPtr, SelectionStart);
		
		if( theData->amp == 8) BlockMoveData( Audio8Ptr, resultPtr + SelectionStart, AudioLength);
		else BlockMoveData( Audio16Ptr, resultPtr + SelectionStart, AudioLength);
		
		BlockMoveData( theData->data + SelectionEnd, resultPtr + SelectionStart + AudioLength, theData->size - SelectionEnd);
		
		DisposePtr( theData->data);		DisposePtr( Audio8Ptr);
		theData->data = resultPtr;
		theData->size = theData->size  - (SelectionEnd - SelectionStart) + AudioLength;
	}
	
	DisposeDialog( myDia);

	#ifndef powerc
		SetA4( oldA4);
	#endif

	return noErr;
}