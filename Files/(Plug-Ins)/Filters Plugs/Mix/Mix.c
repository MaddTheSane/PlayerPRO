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

#include <Carbon/Carbon.h>
#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

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
static void raiseRect (Rect theRect)
{
	ForeColor(whiteColor);
	MoveTo(theRect.left,theRect.bottom);
	LineTo(theRect.left,theRect.top);
	LineTo(theRect.right,theRect.top);
	ForeColor(blackColor);
	LineTo(theRect.right,theRect.bottom);
	LineTo(theRect.left,theRect.bottom);
}

static void groundRect (Rect theRect)
{
	ForeColor(whiteColor);
	MoveTo(theRect.right,theRect.top);
	LineTo(theRect.right,theRect.bottom);
	LineTo(theRect.left,theRect.bottom);
	ForeColor(blackColor);
	LineTo(theRect.left,theRect.top);
	LineTo(theRect.right,theRect.top);
}

static void drawSlider (Rect	theRect,
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

static pascal void xRectProc (DialogPtr	theWindow,
							  short		theItem)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;

	GetDialogItem(theWindow,theItem,&iType,&iHandle,&iRect);
	raiseRect( iRect);
}

static pascal void sliderProc (DialogPtr	theWindow,
							   short		theItem)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	long	v;

	if (theItem == slider1) v = gp1;
	else v = gp2;

	GetDialogItem(theWindow,theItem,&iType,&iHandle,&iRect);
	drawSlider( iRect, v);
}

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


static Boolean getParams ( short dlgID, Str255 s1, Str255 s2, PPInfoPlug *thePPInfoPlug)
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

static void alertUser ( short errString)
{
	Str255	s;
	GetIndString(s,5010,errString);
	ParamText(s, NULL, NULL, NULL);
	StopAlert(5011, NULL);
}

OSErr mainMix( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long				lCntOrErr;
	Handle				aHandle = NULL;
	OSErr				anErr;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	
	lCntOrErr = 0;
	GetCurrentScrap( &scrap);
	anErr = GetScrapFlavorFlags( scrap, soundListRsrc, &flags);
	if (anErr == noErr) GetScrapFlavorSize( scrap, soundListRsrc, &lCntOrErr);	
	
	if ( lCntOrErr > 0) {
		Str255	s1, s2 = "\pUntitled";

		aHandle = NewHandle(0);
		s1[0] = strlen(theData->name);
		BlockMove(theData->name,&s1[1],s1[0]);
		
		HLock( aHandle);
		GetScrapFlavorData( scrap, 'STR ', &lCntOrErr, *aHandle);
		HUnlock( aHandle);
		
		gp1 = 50;
		gp2 = 50;
		
		if (getParams (5010, s1, s2, thePPInfoPlug)) {
			long	i, length1, length2, resultLength,
					temp1, temp2;
			Ptr		clipPtr, resultPtr;
			
			
			
			HLock( aHandle);
			GetScrapFlavorData( scrap, soundListRsrc, &lCntOrErr, *aHandle);
			HUnlock( aHandle);
			
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
					for (i = 0; i < SelectionStart; i++) // first pass to get max value.
					{
						temp1 = *worgPtr++;
						peak = max( peak, labs( gp1 * temp1));
					}
					for (i = 0; i < (resultLength - SelectionStart); i++)
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
					
					if (peak) {
						peak = ((long)0x80 * 0x10000) / peak;
					}
					
					worgPtr = theData->data; wclipPtr = clipPtr - SelectionStart; wresultPtr = resultPtr;
					for (i = 0; i < resultLength; i++) // one more time, all together, now !!!
					{
						if (i < theData->size) {
							temp1 = *worgPtr;
						} else temp1 = 0;
						if ((i >= SelectionStart) && (i < (length2 + SelectionStart))) {
							temp2 = (*wclipPtr & 0xFF) - 0x80;
						} else temp2 = 0;
						
						temp = (gp1 * temp1) + (gp2 * temp2);
						temp = (peak * temp) / 0x10000;
							
						if (temp >= 127) temp = 127;	// overflow ?
						else if (temp <= -128 ) temp = -128;

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

	return noErr;
}

// AE796F78-C31E-47B2-B86D-42EA6474B674
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xAE, 0x79, 0x6F, 0x78, 0xC3, 0x1E, 0x47, 0xB2, 0xB8, 0x6D, 0x42, 0xEA, 0x64, 0x74, 0xB6, 0x74)

#define PLUGMAIN mainMix
#define PLUGINFACTORY MixFactory

#include "CFPlugin-FilterBridge.c"
