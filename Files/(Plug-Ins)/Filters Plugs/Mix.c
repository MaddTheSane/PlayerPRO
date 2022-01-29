/*	Mix				*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Digital mixing of two sounds.
//	The sound in the clipboard (get it by Copy) is
//	mixed with the one opened, beginning at the
//	start of the selection.
//	The sound is extended as needed, and Normalize
//	is applied to avoid clipping.
//
//	WARNING
//	This plug does work ONLY with 8 bits data

#include <A4Stuff.h>
#include <SetUpA4.h>
#include <Sound.h>
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


typedef short	*shortPtr;
typedef long	*longPtr;

#define enclosure	9
#define sname1		3
#define sname2		4
#define slider1		5
#define slider2		6

static long gp1;
static long gp2;

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/*
pascal OSErr SetDialogDefaultItem (	DialogPtr	theDialog,
									short		newItem)
	= {0x303c,0x0304,0xAA68};
*/
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

void groundRect (Rect theRect)
{
	ForeColor(whiteColor);
	MoveTo(theRect.right,theRect.top);
	LineTo(theRect.right,theRect.bottom);
	LineTo(theRect.left,theRect.bottom);
	ForeColor(blackColor);
	LineTo(theRect.left,theRect.top);
	LineTo(theRect.right,theRect.top);
}

void drawSlider (	Rect	theRect,
					long	theValue)
{
short	iWidth;
Str255	s;

	groundRect(theRect);
	theRect.left += 1;
	theRect.top += 1;
	{
	Pattern	myPat = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	iWidth = theRect.left; // save left
	theRect.left = theRect.left + (((theRect.right - theRect.left) * theValue) / 100);
	EraseRect(&theRect);
	theRect.right = theRect.left;
	theRect.left = iWidth;
	ForeColor(greenColor);
	FillRect(&theRect,&myPat);
	}
	
	ForeColor(blackColor);
	NumToString(theValue, s);
	TextFont( kFontIDGeneva);
	TextSize( 9);
	SetRect(&theRect,theRect.left - 20,theRect.top,theRect.left - 1,theRect.bottom - 4);
	EraseRect(&theRect);
	iWidth = TextWidth(&s[1],0,s[0]);
	MoveTo(theRect.right - iWidth - 2,theRect.bottom);
	DrawString(s);
	TextFont(systemFont);
	TextSize(12);
}

pascal void xRectProc (DialogPtr	theWindow,
						short		theItem)
{
short	iType;
Handle	iHandle;
Rect	iRect;

	GetDialogItem(theWindow,theItem,&iType,&iHandle,&iRect);
	raiseRect( iRect);
}

pascal void sliderProc (DialogPtr	theWindow,
						short		theItem)
{
short	iType;
Handle	iHandle;
Rect	iRect;
long	v;

	long oldA4 = SetUpA4 ();

	if (theItem == slider1) v = gp1;
	else v = gp2;

	GetDialogItem(theWindow,theItem,&iType,&iHandle,&iRect);
	drawSlider( iRect, v);

	RestoreA4(oldA4);
}

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


Boolean getParams ( short dlgID, Str255 s1, Str255 s2, PPInfoPlug *thePPInfoPlug)
{
DialogPtr	theDialog;
Boolean		theResult = false;

	theDialog = GetNewDialog(dlgID,nil,(WindowPtr)-1);
	if (theDialog) {
		short	iType, itemHit;
		Handle	iHandle;
		Rect	iRect;
		
		SetPortDialogPort( theDialog);
		AutoPosition( theDialog);
		SetDText( theDialog, sname1, s1);
		
		SetDText( theDialog, sname2, s2);
		
		GetDialogItem(theDialog,slider1,&iType,&iHandle,&iRect);
		SetDialogItem(theDialog,slider1,iType,(Handle) NewUserItemUPP( sliderProc),&iRect);
		
		GetDialogItem(theDialog,slider2,&iType,&iHandle,&iRect);
		SetDialogItem(theDialog,slider2,iType,(Handle) NewUserItemUPP( sliderProc),&iRect);
		
		GetDialogItem(theDialog,enclosure,&iType,&iHandle,&iRect);
		SetDialogItem(theDialog,enclosure,iType,(Handle) NewUserItemUPP(xRectProc),&iRect);

		do {
			ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
			if ((itemHit == slider1) || (itemHit == slider2))
			{	//track slider
				GrafPtr	savePort;
				Point	newp, oldp = { 0, 0};
				GetPort(&savePort);
				SetPortDialogPort(theDialog);
				GetDialogItem(theDialog,itemHit,&iType,&iHandle,&iRect);
				while(WaitMouseUp()) {
					GetMouse(&newp);
					if (DeltaPoint(oldp,newp) & 0xFFFF) {
						long newVal = min( 100, max( 0, ((newp.h - iRect.left) * 100) / (iRect.right - iRect.left)));
						if (itemHit == slider1) gp1 = newVal;
						else gp2 = newVal;
						drawSlider( iRect, newVal);
						oldp = newp;
					}
				}
				SetPort(savePort);
			}
		} while ((itemHit != ok) && (itemHit != cancel));
		if (itemHit == ok) {
			theResult = true;
		}
		DisposeDialog(theDialog);
	}
	return theResult;
}

void alertUser ( short errString)
{
Str255	s;
	GetIndString(s,5010,errString);
	ParamText(s,nil,nil,nil);
	StopAlert(5011,nil);
}

OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long				oldA4, scrapOffset, lCntOrErr;
	Handle				aHandle;
	OSErr				anErr;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	
	
	oldA4 = SetCurrentA4();
	RememberA4();
	aHandle = nil;
	
	#if TARGET_API_MAC_CARBON
	
	lCntOrErr = 0;
	GetCurrentScrap( &scrap);
	anErr = GetScrapFlavorFlags( scrap, soundListRsrc, &flags);
	if( anErr == noErr) GetScrapFlavorSize( scrap, soundListRsrc, &lCntOrErr);	
	
	#else
	
	lCntOrErr = GetScrap( 0L, soundListRsrc, &scrapOffset);
	
	#endif
	
	if ( lCntOrErr > 0) {
		Str255	s1, s2 = "\pUntitled";

		aHandle = NewHandle(0);
		s1[0] = strlen(theData->name);
		BlockMove(theData->name,&s1[1],s1[0]);
		
		#if TARGET_API_MAC_CARBON
		HLock( aHandle);
		GetScrapFlavorData( scrap, 'STR ', &lCntOrErr, *aHandle);
		HUnlock( aHandle);
		#else
		if (GetScrap(aHandle,'STR ',&scrapOffset) > 0)
			BlockMove(*aHandle,&s2,**aHandle + 1);
		#endif
		
		gp1 = 50;
		gp2 = 50;
		
		if (getParams (5010, s1, s2, thePPInfoPlug)) {
			long	i, length1, length2, resultLength,
					temp1, temp2;
			Ptr		clipPtr, resultPtr;
			
			
			
			#if TARGET_API_MAC_CARBON
			HLock( aHandle);
			GetScrapFlavorData( scrap, soundListRsrc, &lCntOrErr, *aHandle);
			HUnlock( aHandle);
			#else
			GetScrap(aHandle,soundListRsrc,&scrapOffset);	//get the sound
			#endif
			
			HLockHi(aHandle);
			
			clipPtr = *aHandle;
			i = *(shortPtr)clipPtr;
			clipPtr += 4;
			if ((i == 1) || (i == 2)) {
				if ((i == 2) || (*(shortPtr)clipPtr == sampledSynth)) {
					Ptr		worgPtr, wclipPtr, wresultPtr;
					long	temp, peak = 0;
					
					if (i == 1) {
						clipPtr += 20;
					} else  clipPtr += 18;
					length2 = *(longPtr)clipPtr;	// sample length
					clipPtr += 18;					// points to sample data
	
					length1 = theData->size - SelectionStart;
					
					resultLength = SelectionStart + max( length1, length2);
					resultPtr = NewPtr(resultLength);

					worgPtr = theData->data; wclipPtr = clipPtr;
					for( i = 0; i < SelectionStart; i++) // first pass to get max value.
					{
						temp1 = *worgPtr++;
						peak = max( peak, labs( gp1 * temp1));
					}
					for( i = 0; i < (resultLength - SelectionStart); i++)
					{
						if (i < length1) {
							temp1 = *worgPtr;
						} else temp1 = 0;
						if (i < length2) {
							temp2 = (*wclipPtr & 0xFF) - 0x80;
						} else temp2 = 0;
						
						peak = max( peak, labs((gp1 * temp1) + (gp2 * temp2)));
							
						worgPtr++;
						wclipPtr++;
					}
					
					peak = ((long)0x80 * 0x10000) / peak;
					
					worgPtr = theData->data; wclipPtr = clipPtr - SelectionStart; wresultPtr = resultPtr;
					for( i = 0; i < resultLength; i++) // one more time, all together, now !!!
					{
						if (i < theData->size) {
							temp1 = *worgPtr;
						} else temp1 = 0;
						if ((i >= SelectionStart) && (i < (length2 + SelectionStart))) {
							temp2 = (*wclipPtr & 0xFF) - 0x80;
						} else temp2 = 0;
						
						temp = (gp1 * temp1) + (gp2 * temp2);
						temp = (peak * temp) / 0x10000;
							
						if( temp >= 127) temp = 127;	// overflow ?
						else if( temp <= -128 ) temp = -128;

						*wresultPtr = temp;
						worgPtr++;
						wclipPtr++;
						wresultPtr++;
					}
					
					DisposePtr(theData->data);			//all done
					theData->data = resultPtr;			//now switch
					theData->size = resultLength;		//Instrument Ptr
				
				} else alertUser (3);
			} else alertUser (2);
			
			HUnlock(aHandle);
		
		}
		
		DisposeHandle(aHandle);
		
	} else alertUser (1);

	SetA4(oldA4);
	return noErr;
}
