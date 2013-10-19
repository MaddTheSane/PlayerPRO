#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <stdio.h>
#include "VideoToolbox.h"
#include "VA.h"

pascal Boolean AboutFilter(DialogPtr, EventRecord *, short *);
pascal void ScrollProc (ControlHandle theControl, short theCode);
void ScrollTextClose( Boolean RemoveVBL);
void ScrollTextInit( Boolean, Boolean);
void InitBounceBall( short xx, short yy, short TypeB);
void DoInternetMenu( short theItem);

//pascal Boolean AboutFilter(DialogPtr, EventRecord *, short *);

static	TEHandle 		hTE;
static	long			curPosT;
static	Rect			scrollRect, textRect;
static	Point			zeroPt;
static	Boolean			AboutFirstCall;
static	short 			gThumbPrev;
static	GWorldPtr		gGWorldAbout = NULL, gMaskWorld = NULL, gResultWorld = NULL, gFreePict = NULL;
static	PicHandle		backPict;
static	RgnHandle		maskHndl;

Boolean	IsPressed( unsigned short );

extern	Boolean		DebuggingMode;
extern	KeyMap		km;
extern	long		DeviationV;
extern	short		RowBytes;

void MegaPurge(void)
{
	long	lBytesFree, contigSize, i;
	Ptr		aPtr = NULL;

	PurgeMem( FreeMem());
	
	PurgeSpace( &lBytesFree, &contigSize);
	
	contigSize = FreeMem();

	while( aPtr == NULL)
	{
		aPtr = MyNewPtr( contigSize);
		contigSize -= 2;
	}
	
	//for( i = 0; i < contigSize/4; i++) aPtr[ i] = 0x12344321; //FIXME: Overflow in implicit constant conversion
	//XXX: what is the point of this, again?
	for( i = 0; i < contigSize/4; i += 4)
	{
		UInt32 *toconv = (UInt32*)(&aPtr[ i]);
		*toconv = 0x12344321;
	}
	
	
	MyDisposePtr( & aPtr);
}


/*void MyScrollRect( Rect	*rect)
{
	short	larg, haut, i, x;
	long	*src, *dst;

	larg = rect->right - rect->left;
	larg /= 4;
	haut = rect->bottom - rect->top;

	src = (long*) ((long) VA[ 0].quickrow[ zeroPt.v + rect->top + 1] + (long) zeroPt.h + (long) rect->left);
	dst = (long*) ((long) src - (long) VA[ 0].row);

	for( i = 0; i < haut; i++)
	{
		for( x = 0; x < larg; x++) dst[ x] = src[ x];
		dst += VA[ 0].row/4;
		src += VA[ 0].row/4;
	}
}*/

void DoScrollText( DialogPtr aDia)
{
	Str255		tempStr;
	GDHandle	oldGDeviceH;
	CGrafPtr	oldPort;
	PicHandle	tempPict;
	short		itemType;
	Rect		itemRect;
	Handle		itemHandle;
	RGBColor	color;

	GetGWorld( &oldPort, &oldGDeviceH);

	SetGWorld( gGWorldAbout, NULL);

	ScrollRect( &(*GetPortPixMap( gGWorldAbout))->bounds, 0, -1, NULL);

	if( (curPosT % 60) == 0)
	{
		TextSize( 24);
		TextFont( kFontIDHelvetica);
		TextFace( bold);
		BackColor( blackColor);

		color .red = color .green = color .blue = 30583;
		RGBForeColor( &color);
		GetIndString( tempStr, 131, curPosT/60);
	/*	OffsetRect( &textRect, 2, 2);
		TETextBox( tempStr + 1, tempStr[ 0], &textRect, teCenter);
		OffsetRect( &textRect, -2, -2);*/
		SwitchColor( curPosT % 32);
		
		TETextBox( tempStr + 1, tempStr[ 0], &textRect, teCenter);
		
	//	BackColor( whiteColor);
	//	MoveTo(textRect.left, textRect.bottom);
	//	DrawString( tempStr);
		
		ForeColor( blackColor);
		
		if( tempStr[ 0] == 0) curPosT = 1;
	}

	curPosT++;
	

	SetGWorld( oldPort, oldGDeviceH);

	ForeColor( blackColor);
	BackColor( whiteColor);

/*CopyDeepMask(	(BitMap*)	*GetPortPixMap( gGWorldAbout),
				(BitMap*) 	*GetPortPixMap( gMaskWorld),
				(BitMap*) 	*GetPortPixMap( gResultWorld),
							&scrollRect,
							&scrollRect,
							&scrollRect,
							srcCopy,
							NULL);*/

//CopyDeepMask(srcBits, maskBits, dstBits, srcRect, maskRect, dstRect, mode, maskRgn);


	CopyBits(	(BitMap*) 	*GetPortPixMap( gMaskWorld),
				(BitMap*)	*GetPortPixMap( gResultWorld),
							&scrollRect,
							&scrollRect,
							srcCopy,
							NULL);

	CopyBits(	(BitMap*) 	*GetPortPixMap( gGWorldAbout),
				(BitMap*)	*GetPortPixMap( gResultWorld),
							&scrollRect,
							&scrollRect,
							addMax,
							NULL);


	CopyBits(	(BitMap*) 	*GetPortPixMap( gResultWorld),
 						(BitMap*) *GetPortPixMap(GetDialogPort( aDia)),
 						&scrollRect,
 						&scrollRect,
 						srcCopy,
 						NULL);


}

void CloseScrollText(void)
{

}

void ShowPerformance(void)
{
	Str255			aStr;
	DialogPtr		aDialog;
	short			itemHit;
	long			oldTicks, i, normal, proto = 1;
	Ptr				tempPtr;
	Boolean			MusiqueOn;
	
	aDialog = GetNewDialog( 158, NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
#define CSTTEST	4000
	
	do
	{
		ModalDialog( MyDlgFilterDesc, &itemHit);
		if( itemHit == 7) DebuggingMode = true;
		else if(itemHit == 8) DebuggingMode = false;
		else if( itemHit == 1)
		{
			SetCursor( &watchCrsr);
		
		//	MusiqueOn = MADDriver->Reading;
			MADReset( MADDriver);
			MADStopDriver( MADDriver);
			
			MADDriver->Reading = true;

			tempPtr = MyNewPtr( 8L*MADDriver->ASCBUFFERReal);
			
			oldTicks = TickCount();
			for( i = 0 ; i < CSTTEST; i++)
			{
				proto *= 123;
				proto -= 255;
				proto /= 791;
			}
			normal = TickCount() - oldTicks;
			
			oldTicks = TickCount();
			
			for( i = 0 ; i < CSTTEST; i++)
			{
				proto *= 123;
				proto -= 255;
				proto /= 791;

				
				/** TEST ROUTINES **/
			
				NoteAnalyse( MADDriver);
			//	Play16Stereo( MADDriver);
			
				/*******************/
			}
			
			i = TickCount() - oldTicks - normal;
			
			NumToString( i, aStr);
			SetDText( aDialog, 4, aStr);
			
			i *= 100;
			sprintf( (Ptr) aStr, "%.2f", (double) i / (double) CSTTEST);
			MyC2PStr( (Ptr) aStr);
			pStrcat(aStr, "\p %");
			SetDText( aDialog, 6, aStr);

			MyDisposePtr( & tempPtr);
			
			MADReset( MADDriver);
			MADStartDriver( MADDriver);
		//	MADDriver->Reading = MusiqueOn;
			
			SetCursor( GetQDGlobalsArrow( &qdarrow));
		}
		else if( itemHit == 9) MegaPurge();
	
	}while( itemHit != 2);
	
	FlushEvents( everyEvent, 0);
	DisposeDialog( aDialog);
}

void DoAbout(void)
{
	GrafPtr			myPort;
	DialogPtr		aDialog;
	Handle			itemHandle;
	short			itemType;
	long			oldTicks;
	Point			ssPt;
	Rect			destRect, itemRect;
	GDHandle		oldGDeviceH;
	CGrafPtr		oldPort;
	PicHandle		tempPict;
	
	GetPort( &myPort);
	
	GetKeys( km);
	if( IsPressed( 0x31) == true)
	{
		ShowPerformance();
		SetPort( myPort);
		return; 
	}
	
	aDialog = GetNewDialog( 131, NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
	{
	short				i;
	Str255				text;
	
	GetDialogItem( aDialog, 8, &itemType, &itemHandle, &itemRect);
	OffsetRect( &itemRect, 0, -300);
	SetDialogItem( aDialog, 8, itemType, itemHandle, &itemRect);
	}
	
	DrawDialog( aDialog);
	
	GetDialogItem ( aDialog, 5, &itemType, &itemHandle, &scrollRect);
	
	ssPt.v = scrollRect.top;	ssPt.h = scrollRect.left;
	LocalToGlobal( &ssPt);
	scrollRect.left = 4*(scrollRect.left/4);		scrollRect.right = 4*(scrollRect.right/4);

	textRect = scrollRect;
	

	GetGWorld( &oldPort, &oldGDeviceH);
	gGWorldAbout = NULL;

	scrollRect.bottom += 30;
	NewGWorld( &gGWorldAbout, 32, &scrollRect, nil, nil, (GWorldFlags) 0);		LockPixels( GetPortPixMap( gGWorldAbout));
	NewGWorld( &gMaskWorld, 32, &scrollRect, nil, nil, (GWorldFlags) 0);			LockPixels( GetPortPixMap( gMaskWorld));
	NewGWorld( &gResultWorld, 32, &scrollRect, nil, nil, (GWorldFlags) 0);		LockPixels( GetPortPixMap( gResultWorld));
	scrollRect.bottom -= 30;

	GetDialogItem( aDialog, 8, &itemType, &itemHandle, &itemRect);
	NewGWorld( &gFreePict, 32, &itemRect, nil, nil, (GWorldFlags) 0);			LockPixels( GetPortPixMap( gFreePict));

	textRect.top = scrollRect.bottom;
	textRect.bottom = textRect.top + 30;
	
//	HideCursor();
	
	backPict = GetPicture( 133);
	
	SetRect( &destRect	,	(scrollRect.right - scrollRect.left)/2 - (*backPict)->picFrame.right/2 ,
							(scrollRect.bottom - scrollRect.top)/2 - (*backPict)->picFrame.bottom/2,
							(scrollRect.right - scrollRect.left)/2 + (*backPict)->picFrame.right/2,
							(scrollRect.bottom - scrollRect.top)/2 + (*backPict)->picFrame.bottom/2);
	destRect.left += scrollRect.left;
	destRect.right += scrollRect.left;

	destRect.top += scrollRect.top;
	destRect.bottom += scrollRect.top;

	
	////////////////////////////////
	
	SetGWorld( gGWorldAbout, NULL);
	
	BackColor( blackColor);
	ForeColor( whiteColor);
	EraseRect( &(*GetPortPixMap( gGWorldAbout))->bounds);
	//DrawPicture( backPict, &destRect);
	
	SetGWorld( oldPort, oldGDeviceH);
	
	////////////////////////////////

	tempPict = GetPicture( 128);

	SetGWorld( gMaskWorld, NULL);
	
	BackColor( blackColor);	ForeColor( whiteColor);
	EraseRect( &(*GetPortPixMap( gMaskWorld))->bounds);
	
	GetDialogItem( aDialog, 1, &itemType, &itemHandle, &itemRect);
	DrawPicture( tempPict, &itemRect);

	SetGWorld( oldPort, oldGDeviceH);

	ReleaseResource( (Handle) tempPict);

	////////////////////////////////
	
	SetGWorld( gResultWorld, NULL);
	
	BackColor( blackColor);	ForeColor( whiteColor);
	EraseRect( &(*GetPortPixMap( gResultWorld))->bounds);
	
	tempPict = GetPicture( 128);
	GetDialogItem( aDialog, 1, &itemType, &itemHandle, &itemRect);
	DrawPicture( tempPict, &itemRect);
	ReleaseResource( (Handle) tempPict);
	
	////////////////////////////////

	SetGWorld( gFreePict, NULL);

	tempPict = GetPicture( 156);
	GetDialogItem( aDialog, 8, &itemType, &itemHandle, &itemRect);
	DrawPicture( tempPict, &itemRect);
	ReleaseResource( (Handle) tempPict);
	
	SetGWorld( oldPort, oldGDeviceH);

	////////////////////////////////	

	ForeColor( blackColor);
	BackColor( whiteColor);

	zeroPt.h = 0; zeroPt.v = 0;
	LocalToGlobal( &zeroPt);
	
	oldTicks = TickCount() + 10;
	
	curPosT = 50;
	
//	if( theDepth == 8) InitBounceBall( zeroPt.h, zeroPt.v, 0);

	while( !Button())
	{
		while( oldTicks >= TickCount()) DoGlobalNull();
		oldTicks = TickCount() + 1;
		
		DoScrollText( aDialog);
		
		WaitNextEvent( everyEvent, &theEvent, 1, NULL);
	
		if (QDIsPortBuffered( GetDialogPort( aDialog)))
    					QDFlushPortBuffer( GetDialogPort( aDialog), NULL);

	}
//	if( theDepth == 8) CloseBounceBall();
	
	ReleaseResource( (Handle) backPict);

	CloseScrollText();
	
//	ShowCursor();
	
	UnlockPixels( GetPortPixMap( gGWorldAbout));		DisposeGWorld( gGWorldAbout);
	UnlockPixels( GetPortPixMap( gResultWorld));		DisposeGWorld( gResultWorld);
	UnlockPixels( GetPortPixMap( gMaskWorld));			DisposeGWorld( gMaskWorld);
	UnlockPixels( GetPortPixMap( gFreePict));			DisposeGWorld( gFreePict);
	
	while( Button()) {};
	
	WaitNextEvent( everyEvent, &theEvent, 1, NULL);
	
	FlushEvents( everyEvent, 0);
	DisposeDialog( aDialog);
}

static	RgnHandle	mBarRgn;

void HideMenuBar2(void)
{
	Rect		mBarRect;
	Point		xy = {0,0};
	BitMap		screenBits;

	GetQDGlobalsScreenBits( &screenBits);

	SetRect(	&mBarRect,
				screenBits.bounds.left,
				screenBits.bounds.bottom - 32,
				screenBits.bounds.right,
				screenBits.bounds.bottom);
	
	PaintRect( &mBarRect);
	
	mBarRgn = NewRgn();
	RectRgn(mBarRgn, &mBarRect);
	
	DiffRgn( GetGrayRgn(), mBarRgn, GetGrayRgn());
	
	(**GetGDevice()).gdRect.bottom -= 32;
	SetDepth( GetGDevice(), 8, 0001, 0001);
}

void ShowMenuBar2(void)
{
	(**GetGDevice()).gdRect.bottom += 32;
	
	UnionRgn( GetGrayRgn(), mBarRgn, GetGrayRgn());
	DisposeRgn(mBarRgn);
	
	SetDepth( GetGDevice(), 8, 0001, 0001);
}

Boolean DoHelp(void)
{
	short			itemType,itemHit;
	DialogPtr		theDialog;
	GrafPtr			myPort;
	Handle			itemHandle,Text;
	Rect			itemRect;
	StScrpHandle	theStyle;
	ControlHandle	vScroll;
	//Boolean			ScrollBasActive;
	long			GestaltResponse, StartupWait;
	ModalFilterUPP	AboutFilterDesc = NewModalFilterUPP( AboutFilter);
	Str255			str, str2;
	
	GetPort( &myPort);
	
	Gestalt( gestaltProcessorType, &GestaltResponse);
	
	theDialog = GetNewDialog( 130, NULL, (WindowPtr) -1L);
	SetPortDialogPort( theDialog);
	DrawDialog( theDialog);
	
	GetDialogItem ( theDialog, 2, &itemType, &itemHandle, &itemRect);
	itemRect.left++;  itemRect.right--; itemRect.top++; itemRect.bottom--;
	hTE = TEStyleNew( &itemRect, &itemRect);
	
	GetDialogItem ( theDialog, 2, &itemType, &itemHandle, &itemRect);
	itemRect.left = itemRect.right;
	itemRect.right = itemRect.left + 16;
	
	vScroll = NewControl( GetDialogWindow( theDialog), &itemRect, "\p", true, 0, 0, 0, gScrollBarID, 0);
	SetControlReference( vScroll, 1);
	
	Text = GetResource( 'TEXT', 5632);
	DetachResource( Text);
	HLock( Text);
	
	theStyle = (StScrpHandle) GetResource( 'styl', 5632);
	DetachResource( (Handle) theStyle);
	
	TESetAlignment( teCenter, hTE);
	TEStyleInsert( *Text, GetHandleSize( Text), theStyle, hTE);
	
	//TECalText( hTE);
	
	SetScroll( vScroll, hTE);
	
	AboutFirstCall = true;
	
	GetDialogItem ( theDialog, 2, &itemType, &itemHandle, &itemRect);
	itemRect.right++;
	FrameRect( &itemRect);
	
	SetDialogDefaultItem( theDialog, 1);
	
	SetDText( theDialog, 4, "\p");
	StartupWait = 0;
	//ControlSwitch( 1, theDialog, 255);
	//ControlSwitch( 5, theDialog, 255);
	ControlSwitch( 7, theDialog, 255);
	do {
		ModalDialog( AboutFilterDesc, &itemHit);
		
		switch( itemHit)
		{
			case 1:
				if( StartupWait != 0) itemHit = 0;
				break;
				
			case -5:
				DoGlobalNull();
				
				if( StartupWait != 0)
				{
					if( TickCount() >= StartupWait)
					{
						ControlSwitch( 7, theDialog, 0);
						
						StartupWait = 0;
						
						SetDText( theDialog, 4, "\p");
					}
					else
					{
						NumToString( 1 + ((StartupWait - TickCount())/60), str);
						
						pStrcpy( str2, "\pWait ");
						pStrcat( str2, str);
						pStrcat( str2, "\p s");
						SetDText( theDialog, 4, str2);
					}
				}
				break;
		}
		
	}while( itemHit != 1 && itemHit != 6 && itemHit != 7);
	
	DisposeDialog( theDialog);
	TEDispose( hTE);
	HUnlock( Text);
	MyDisposHandle( & Text);
	MyDisposHandle( (Handle*) &theStyle);
	
	UpdateALLWindow();
	
	DisposeModalFilterUPP( AboutFilterDesc);
	
	switch( itemHit)
	{
		case 6:
			DoInternetMenu( 1);
			break;
			
		case 7:
			break;
	}
	return false;
}

void MyAdjustText ( ControlHandle vScroll)
{
	short			oldScroll, newScroll, delta;
	double			height;
	TEHandle		CurrentTE;
	TEStyleHandle	hStyle;
	
	CurrentTE = hTE;
	
	oldScroll = (**CurrentTE).viewRect.top - (**CurrentTE).destRect.top;
	hStyle = TEGetStyleHandle( hTE);
	
	height = TEGetHeight( 0, (*CurrentTE)->nLines, CurrentTE);
	height /= (*CurrentTE)->nLines;

	newScroll = GetControlValue(vScroll) * height;
	delta = oldScroll - newScroll;
	if (delta != 0) TEPinScroll( 0, delta, CurrentTE);
}

void DoContent(WindowPtr theWindow, EventRecord *theEventI)
{
	short				cntlCode;
	ControlHandle 		theControl;
	GrafPtr				savePort;
	ControlActionUPP	MyControlUPP;
	
	GetPort(&savePort);
	SetPortWindowPort(theWindow);

	GlobalToLocal(&theEventI->where);
	cntlCode = FindControl(theEventI->where, theWindow, &theControl);
	switch( cntlCode)
	{
	case kControlIndicatorPart:
		if( gUseControlSize) goto LiveScroll;
		else TrackControl(theControl, theEventI->where, NULL);
		MyAdjustText( theControl);
	break;
	
	case kControlUpButtonPart:
    case kControlDownButtonPart:
    case kControlPageUpPart:
    case kControlPageDownPart:

		LiveScroll:		

    	MyControlUPP = NewControlActionUPP( ScrollProc);
		TrackControl(theControl, theEventI->where, MyControlUPP);
		DisposeControlActionUPP( MyControlUPP);
	break;
	}
	
	LocalToGlobal(&theEventI->where);
	SetPort(savePort);
}

pascal void ScrollProc (ControlHandle theControl, short theCode)
{
	short		pageSize;
	short		scrollAmt = 0;
	short 		oldCtl;
	TEHandle	CurrentTE;
	double		height;
	
	if (theCode == 0)
		return ;
	
	CurrentTE = hTE;
	
	height = TEGetHeight( 0, (*CurrentTE)->nLines, CurrentTE);
	height /= (*CurrentTE)->nLines;
	
	pageSize = ((**CurrentTE).viewRect.bottom-(**CurrentTE).viewRect.top) /  height - 1;
	
	oldCtl = GetControlValue(theControl);

	switch (theCode) {
		case kControlUpButtonPart: 
			scrollAmt = -1;
			break;
		case kControlDownButtonPart: 
			scrollAmt = 1;
			break;
		case kControlPageUpPart: 
			scrollAmt = -pageSize;
			break;
		case kControlPageDownPart: 
			scrollAmt = pageSize;
			break;

		case kControlIndicatorPart:
			scrollAmt = gThumbPrev;
			gThumbPrev = oldCtl;
		break;
	}
	
	if( theCode != kControlIndicatorPart) SetControlValue(theControl, oldCtl+scrollAmt);

	MyAdjustText( theControl);
}

pascal Boolean AboutFilter (DialogPtr theDialog, EventRecord *theEventI, short *itemHit)
{
		char		xMyFilter;
		WindowPtr	whichWindow;
		short			thePart;
		GrafPtr		CurrentPort;
		short		itemType;
		Rect		itemRect;
		Handle		itemHandle;
 
		xMyFilter = false;
		switch( theEventI->what)
		{		
		case updateEvt:
			GetPort(&CurrentPort);
			
			if( (WindowPtr) theEventI->message == GetDialogWindow( theDialog))
			{
				whichWindow = (WindowPtr) theEventI->message;
				
				SetPortWindowPort( whichWindow);
				BeginUpdate( whichWindow);
					DrawDialog( GetDialogFromWindow( whichWindow));
					
					if( !AboutFirstCall)
					{
						Rect	caRect;
						
						TEUpdate( GetPortBounds( GetDialogPort( theDialog), &caRect), hTE);
					}
					else AboutFirstCall = false;
					
					GetDialogItem ( theDialog, 2, &itemType, &itemHandle, &itemRect);
					itemRect.right++;
					FrameRect( &itemRect);
					
				EndUpdate( whichWindow);
				*itemHit = -updateEvt;
				xMyFilter = true;
			}
			else DoUpdateEvent( theEventI);
		
			SetPort( CurrentPort);
				
			return( true);
		break;
		
		case nullEvent:
			DoGlobalNull();
		
			*itemHit = -5;
			
			return( true);
		break;
		
		case mouseDown:
			thePart = FindWindow(theEventI->where, &whichWindow);
			
			if( whichWindow != GetDialogWindow( theDialog)) break;
			
			if(GetWRefCon( whichWindow) == GetWRefCon( FrontWindow()))
			{
			switch(thePart)
			{
			case inDrag:
			{
				BitMap	screenBits;
				
				GetQDGlobalsScreenBits( &screenBits);
				
				DragWindow(whichWindow,theEventI->where, &screenBits.bounds);
				
				xMyFilter				=	true;
			}
			break;
			
			case inGoAway:
				if (TrackGoAway( whichWindow, theEventI->where) ) xMyFilter =	true;
			break;
			
			case inContent:
				DoContent( whichWindow, theEventI);
			break;
			}
			}
		break;
		
		case keyDown:
		case autoKey:
			if((theEventI->message & charCodeMask)==0x0D || (theEventI->message & charCodeMask)==0x03) 
			{
				xMyFilter				=	true;
				*itemHit				=	1;
			}
		break;
		}
		
		return(xMyFilter);
}
