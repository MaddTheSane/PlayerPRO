/*	Sampling Rate	*/
/*	v 1.0			*/
/*	1996 by ANR 	*/


#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

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

static Ptr ConvertSampleC4SPD( Ptr src, long srcSize, short amp, long srcC4SPD, long dstC4SPD, Boolean stereo)
{
	#define LRVAL	3L

	short						*src16 = (short*) src, *dst16;
	char						*src8 = (char*) src, *dst8;
	Ptr							dst;
	long						newSize, tempL = 0, tempR = 0;
	unsigned  long			x, left, right, pos;
	
	srcC4SPD /= 100;
	dstC4SPD /= 100;
	
	newSize = (srcSize * dstC4SPD) / srcC4SPD;
	
	dst = NewPtr( newSize);
	if (dst == NULL) return NULL;
	
	dst16 = (short*) dst;
	dst8 = (char*) dst;
		
	switch( amp)
	{
	case 8:
		for (x = 0; x < newSize; x++)
		{
			pos			= (x * srcC4SPD << LRVAL) / dstC4SPD;
			right		= pos & ((1 << LRVAL)-1);
			left		= (1 << LRVAL) - right;
			
			if (stereo)
			{
				pos >>= LRVAL;
				pos /= 2;
				pos *= 2;
				
				if (1 + pos >= srcSize) {}
				else tempL = (left * src8[ pos] + right * src8[ 2 + pos]) >> LRVAL;
				
				dst8[ x] = tempL;
				
				x++;
				
				if (3 + pos >= srcSize) {}
				else tempR = (left * src8[ 1 + pos] + right * src8[ 3 + pos]) >> LRVAL;
				
				dst8[ x] = tempR;
			}
			else
			{
				pos >>= LRVAL;
				
				if (pos < 0 ||  1 + pos >= srcSize) {}
				else tempL = (left * src8[ pos] + right * src8[ 1 + pos]) >> LRVAL;
				
				dst8[ x] = tempL;
			}
		}
	break;
	
	case 16:
		for (x = 0; x < newSize/2; x++)
		{
			pos			= (x * srcC4SPD << LRVAL) / dstC4SPD;
			right		= pos & ((1 << LRVAL)-1);
			left		= (1 << LRVAL) - right;
			
			if (stereo)
			{
				pos >>= LRVAL;
				pos /= 2;
				pos *= 2;
				
				if (1 + pos >= srcSize/2) {}
				else tempL = (left * src16[ pos] + right * src16[ 2 + pos]) >> LRVAL;
				
				dst16[ x] = tempL;
				
				x++;
				
				if (3 + pos >= srcSize/2) {}
				else tempL = (left * src16[ 1 + pos] + right * src16[ 3 + pos]) >> LRVAL;
				
				dst16[ x] = tempL;
			}
			else
			{
				pos >>= LRVAL;
				
				if (1 + pos >= srcSize/2) {}
				else tempL = (left * src16[ pos] + right * src16[ 1 + pos]) >> LRVAL;
				
				dst16[ x] = tempL;
			}
		}
	break;
	}
	
	return dst;
}

static OSErr mainSampRate(	sData			*theData,
					long			SelectionStart,
					long			SelectionEnd,
					PPInfoPlug		*thePPInfoPlug,
					short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	DialogPtr		myDia;
	short			itemHit;
	Str255			tStr;

	myDia = GetNewDialog( 128, NULL, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);
	
	/** Default values **/
	NumToString( theData->c2spd, tStr);
	SetDText( myDia, 4, tStr);
	SetDText( myDia, 7, tStr);
	
	SelectDialogItemText( myDia, 7, 0, 10000);
	/********************/
	
	ReGo:
	
	do
	{
		ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
	}
	while( itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		long	newFreq;
		Ptr		newPtr;
		
		GetDText( myDia, 7, tStr);		StringToNum( tStr, &newFreq);
		if (newFreq < 2000L || newFreq > 50000)
		{
			SysBeep( 1);
			SelectDialogItemText( myDia, 7, 0, 10000);
			goto ReGo;
		}
		
		newPtr = ConvertSampleC4SPD(	theData->data,
																	theData->size,
																	theData->amp,
																	theData->c2spd,
																	newFreq,
																	theData->stereo);
		
		theData->loopBeg = (theData->loopBeg * (newFreq/100)) / (long) (theData->c2spd/100);
		theData->loopSize = (theData->loopSize * (newFreq/100)) / (long) (theData->c2spd/100);
		
		if (newPtr != NULL)
		{
			DisposePtr( theData->data);
			theData->data		= newPtr;
			theData->size		= GetPtrSize( newPtr);
			theData->c2spd	= newFreq;
		}
		
		
		if (theData->loopBeg < 0) { theData->loopSize += theData->loopBeg;	theData->loopBeg = 0;}
		if (theData->loopBeg > theData->size) {theData->loopBeg = 0;	theData->loopSize = 0;}
		if (theData->loopSize < 0) theData->loopSize = 0;
		if (theData->loopBeg + theData->loopSize > theData->size) theData->loopSize = theData->size - theData->loopBeg;
	}
	
	DisposeDialog( myDia);

	return noErr;
}

// 81D156A0-6737-4D5F-BE63-5BA48F527276
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x81, 0xD1, 0x56, 0xA0, 0x67, 0x37, 0x4D, 0x5F, 0xBE, 0x63, 0x5B, 0xA4, 0x8F, 0x52, 0x72, 0x76)

#define PLUGMAIN mainSampRate
#define PLUGINFACTORY SampRateFactory

#include "CFPlugin-FilterBridge.c"
