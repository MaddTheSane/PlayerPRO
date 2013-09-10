/*	ToneGenerator	*/
/*	v 1.1			*/
/*	1995 by ANR 	*/


#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include <Carbon/Carbon.h>
#include <math.h>

enum
{
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
};

#define		PI      		M_PI
#define		KHZ				22254.54545

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

	GetDialogItemAsControl( dlog, item, &control );
	SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl( control);
}

static Ptr CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	Ptr		Audio8Ptr;
	long	i, temp, inter, x, dest;
	Boolean	UpDown;
	
	if( stereo) AudioLength *= 2L;
	
	Audio8Ptr = NewPtr( AudioLength);
	if( Audio8Ptr == NULL) return NULL;
	
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

static short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	short	*Audio16Ptr;
	long	i, temp, inter, x, dest;
	Boolean	UpDown;
	
	if( stereo) AudioLength *= 2L;
	
	Audio16Ptr = (short*) NewPtr( AudioLength*2);
	if( Audio16Ptr == NULL) return NULL;
	
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

static OSErr mainToneGenerator(sData			*theData,
						long			SelectionStart,
						long			SelectionEnd,
						PPInfoPlug		*thePPInfoPlug,
						short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long				i, AudioLength, AudioFreq, AudioAmp;
	Ptr					/*Sample8Ptr = theData->data,*/ Audio8Ptr;
	short				/* *Sample16Ptr = (short*) theData->data,*/ *Audio16Ptr;
	DialogPtr			myDia;
	short				itemHit, itemType, AudioType;
	Handle				itemHandle;
	Rect				itemRect;
	Str255				tStr;

	myDia = GetNewDialog( 128, NULL, (WindowPtr) -1L);
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
	
	Audio8Ptr	= NULL;	Audio16Ptr	= NULL;

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
						if( Audio8Ptr != NULL) { DisposePtr( Audio8Ptr);			Audio8Ptr = NULL;}
						Audio8Ptr	= CreateAudio8Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);
						if( Audio8Ptr != NULL)
						{
						//	iErr = mPlay( Audio8Ptr, AudioLength, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
						}
					break;
					
					case 16:
						if( Audio16Ptr != NULL) { DisposePtr( (Ptr) Audio16Ptr);	Audio16Ptr = NULL;}
						Audio16Ptr	= CreateAudio16Ptr( AudioLength, AudioFreq, AudioAmp, AudioType, theData->stereo);
						if( Audio16Ptr != NULL)
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
		
		if( Audio16Ptr != NULL)	{	DisposePtr( (Ptr) Audio16Ptr);			Audio16Ptr = NULL;}
		if( Audio8Ptr != NULL)	{	DisposePtr( (Ptr) Audio8Ptr);			Audio8Ptr = NULL;}
		
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

	return noErr;
}

// 25FA16EC-75FF-4514-9C84-7202360044B9
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x25, 0xFA, 0x16, 0xEC, 0x75, 0xFF, 0x45, 0x14, 0x9C, 0x84, 0x72, 0x02, 0x36, 0x00, 0x44, 0xB9)

#define PLUGMAIN mainToneGenerator
#define PLUGINFACTORY ToneGeneratorFactory

#include "CFPlugin-FilterBridge.c"
