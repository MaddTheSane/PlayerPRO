#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "stdio.h"

			DialogPtr				AHelpDlog;
	extern	EventRecord				theEvent;
	extern	MenuHandle				ViewsMenu;
	extern	short					mainVRefNum;
	extern	long					mainParID;
	extern	Boolean					HelpAvalaible;
	extern	WindowPtr				oldWindow;
	extern	ProcessSerialNumber		playerPROPSN;
	
	static	long					lastwref;
	static	short					lastitem;
	static	Point					savept;
	static	RgnHandle				mBarRgn;
	static	short					oldMBarHeight, MBarHeightInt;

	#define WINDOWHIGH 17

	void AdjustZoomClassic2( Rect	*vRect);
	void AdjustZoomPiano2( Rect *vRect);
	void AdjustZoomOscillo2( Rect	*vRect);
	void DoHelpStaff( short **items, short *lsize);
	
void DoAHelpInfo(void)
{
	short						i, iType, *items, lsize;
	Handle						iHandle;
	Rect						iRect;
	Str255						aStr;
	Point						pt, ptLocal;
	long						wref;
	GrafPtr						savePort;
	GrafPtr						windowManagerPort;
	WindowPtr					aWin;
	RgnHandle					strucRgn;
	ProcessSerialNumber			PSN;
	unsigned long				outFeatures;
	
	if( !HelpAvalaible) return;
	if( AHelpDlog == 0L) return;
	if( MacIsWindowVisible( GetDialogWindow( AHelpDlog)) == false) return;

	GetFrontProcess( &PSN);
	if( PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) return;
	
	GetPort( &savePort);
	
/*	#if MACOS9VERSION
	GetWMgrPort( &windowManagerPort);
	SetPort( windowManagerPort);
	#endif*/
	
	GetMouse( &pt);
	LocalToGlobal( &pt);
	if( pt.h == savept.h && pt.v == savept.v) return;
	
	savept = pt;

	aWin = FrontWindow();
	
	while( aWin != 0L)
	{
		strucRgn = NewRgn();
		
	/*	GetWindowFeatures( aWin, &outFeatures);
		if( outFeatures & kWindowCanGetWindowRegion) aWin = aWin;
		else Debugger();*/
		
		#if MACOS9VERSION
		if( PtInRgn( pt, ((WindowPeek)aWin)->strucRgn))
		#else
		GetWindowRegion( aWin, kWindowStructureRgn, strucRgn);
			
		if( PtInRgn( pt, strucRgn))
		#endif
		{
			wref = GetWRefCon( aWin);
		
			switch( wref)
			{
				case RefMozart:
					DoHelpMozart( &items, &lsize);
				break;
			
				case RefPartition:
					DoHelpEditor( &items, &lsize);
				break;
			
				case RefPlayer:
					DoHelpOscillo( &items, &lsize);
				break;
			
				case RefSpectrum:
					DoHelpSpectrum( &items, &lsize);
				break;
			
				case RefInstruList:
					DoHelpInstruList( &items, &lsize);
				break;
				
				case RefMODList:
					DoHelpMODList( &items, &lsize);
				break;
				
				case RefTools:
					DoHelpTools( &items, &lsize);
				break;
				
				case RefPiano:
					DoHelpPiano( &items, &lsize);
				break;
				
				case RefSample:
					DoHelpSamples( &items, &lsize);
				break;
				
				case RefClassic:
					DoHelpClassic( &items, &lsize);
				break;
				
				case RefParti:
					DoHelpPartition( &items, &lsize);
				break;
				
				case RefAdaptators:
					DoHelpAdap( &items, &lsize);
				break;
				
				case RefPatList:
					DoHelpPatList( &items, &lsize);
				break;
				
				case RefStaff:
					DoHelpStaff( &items, &lsize);
				break;
				
				default:
					lsize = 0;
				break;
			}
			//////
			
			if( lsize > 0)
			{
				SetPortWindowPort( aWin);
				
				GetMouse( &ptLocal);
				for( i = 0; i < lsize; i++)
				{
					GetDialogItem( GetDialogFromWindow( aWin), items[ i], &iType, &iHandle, &iRect);
					if( PtInRect( ptLocal, &iRect))
					{
						if( lastitem != i || lastwref != wref)
						{
							lastitem = i;
							lastwref = wref;
							
							GetIndString( aStr, wref*200, i + 1);
							
							SetPortDialogPort( AHelpDlog);
							TextFont( kFontIDGeneva);	TextSize( 9);	TextFace( bold);
							SetDText( AHelpDlog, 1, aStr);
							SetPortWindowPort( aWin);
						}
						
						SetPort( savePort);
						return;
					}
				}
				
			//	UseResFile( iRefNum);
			}
			/////
			
			SetDText( AHelpDlog, 1, "\p");
			wref = -1;		lastitem = -1;
			SetPort( savePort);
			return;
		}
		
		DisposeRgn( strucRgn);
		
		aWin = MacGetNextWindow( aWin);
	}

	SetDText( AHelpDlog, 1, "\p");
	wref = -1;		lastitem = -1;
	SetPort( savePort);
}

void FrameRectRelief2( Rect *theRect)
{
RGBColor	theColor;
Rect		aCopy;

#define GrisClair	0xEEEE
#define GrisFonce	0x8888
	
if( (**(**GetMainDevice()).gdPMap).pixelSize < 8)
{
	aCopy.left = theRect->left ;
	aCopy.right = theRect->right;
	aCopy.top = theRect->top ;
	aCopy.bottom = theRect->bottom;
	ForeColor( blackColor);
	FrameRect( &aCopy);
	return;
}
MoveTo( theRect->left, theRect->top);

theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
RGBForeColor( &theColor);
LineTo( theRect->right-1, theRect->top);

theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
RGBForeColor( &theColor);
LineTo( theRect->right-1, theRect->bottom-1);

theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
RGBForeColor( &theColor);
LineTo( theRect->left, theRect->bottom-1);

theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
RGBForeColor( &theColor);
LineTo( theRect->left, theRect->top);


ForeColor( blackColor);
}

void  UpdateAHelpWindow(DialogPtr GetSelection)
{ 
		Rect   		tempRect, itemRect;
 		GrafPtr		SavePort;
 		Handle		itemHandle;
 		Rect		caRect;
 		RgnHandle	visibleRegion;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( AHelpDlog);

		BeginUpdate( GetDialogWindow( AHelpDlog));
		
			TextFont( kFontIDGeneva);	TextSize( 9);	TextFace( bold);
			
			visibleRegion = NewRgn();
			
			GetPortVisibleRegion( GetDialogPort( AHelpDlog), visibleRegion);
			
 			UpdtDialog( AHelpDlog, visibleRegion);
 			
 			DisposeRgn( visibleRegion);
 			
 			GetPortBounds( GetDialogPort( AHelpDlog), &caRect);
 			
 			itemRect = caRect;
			FrameRectRelief2( &itemRect);
		EndUpdate( GetDialogWindow( AHelpDlog));

		SetPort( SavePort);
}

/*void MyFindControl( Point localPt, WindowPtr wind, ControlHandle *myCtl)
{
//	WindowPeek		wPeek = (WindowPeek) wind;
//	ControlHandle	aCtl = (ControlHandle) wPeek->controlList;

	*myCtl = 0;
	
	while( aCtl != 0L)
	{
		if( PtInRect( localPt, &(*aCtl)->contrlRect))
		{
			if( (*aCtl)->contrlRect.right - (*aCtl)->contrlRect.left == 2)
			{
				*myCtl = aCtl;
				return;
			}
		}
		
		aCtl = (*aCtl)->nextControl;
	}
}*/

void AdjustZoomIn( WindowPtr	wind)
{
//	WindowPeek		wPeek;
//	WStateData		*wspd;
	GDHandle		aH;
	Point			mouse;
	Rect			stdRect, ViewRect, ar;
	short			h,v;
	GrafPtr			savedPtr;
	Rect			ttt;
//	ControlHandle	aCtl;
	Point			localPt;
	
	#define BORD	4
	
	GetPort( &savedPtr);
	SetPortWindowPort( wind);
	
	localPt.v = localPt.h = 1;
//	FindControl( localPt, wind, &aCtl);
	
	mouse = theEvent.where;
	
	aH = GetDeviceList();
	ar = (*(*aH)->gdPMap)->bounds;
	SetRect( &ViewRect, ar.left + BORD, ar.top + BORD + 7, ar.right - BORD, ar.bottom - BORD);
	if( AHelpDlog != 0L) ViewRect.top += WINDOWHIGH + 33;
	else ViewRect.top += 35;
	
	do
	{
		aH = GetNextDevice( aH);
		if( aH != 0L)
		{
			if( PtInRect( mouse, &(*(*aH)->gdPMap)->bounds))
			{
				ar = (*(*aH)->gdPMap)->bounds;
				
				SetRect( &ViewRect, ar.left + BORD, ar.top + 24,
									ar.right - BORD, ar.bottom - BORD);
			}
		}
	}
	while( aH != 0L);
	
//	wPeek = (WindowPeek) wind;
//	wspd = (WStateData*) *(wPeek->dataHandle);
	
	GetWindowStandardState( wind, &stdRect);
	
/*	if( aCtl != 0L)
	{
		long lRefCon = GetCRefCon( aCtl);
		
		h = lRefCon>>16L;
		v = lRefCon & 0x0000FFFF;
	}
	else*/
	{
		h = stdRect.right - stdRect.left;
		v = stdRect.bottom - stdRect.top;
	}
	
	if( v == 0) v = ViewRect.bottom - ViewRect.top;
	if( h == 0) h = ViewRect.right - ViewRect.left;
	
	stdRect.top = ViewRect.top;
	stdRect.left = ViewRect.left;
	
	stdRect.bottom = stdRect.top + v;
	stdRect.right = stdRect.left + h;
	
	SetWindowStandardState( wind, &stdRect);
	
	switch( GetWRefCon( wind))
	{
		case RefClassic:
			AdjustZoomClassic2( &ViewRect);
		break;
		
		case RefPiano:
			AdjustZoomPiano2( &ViewRect);
		break;
		
		case RefPlayer:
			AdjustZoomOscillo2( &ViewRect);
		break;
	}
	
	GetWindowStandardState( wind, &stdRect);
	
	if( stdRect.bottom > ViewRect.bottom) stdRect.bottom = ViewRect.bottom;
	if( stdRect.right > ViewRect.right) stdRect.right = ViewRect.right;
	
	SetWindowStandardState( wind, &stdRect);
	
	SetPort( savedPtr);
}

void InitWindowBar(void)
{
	Rect	mBarRect;
	BitMap	screenBits;

	oldMBarHeight = GetMBarHeight();
	MBarHeightInt = oldMBarHeight + WINDOWHIGH;
	
	GetQDGlobalsScreenBits( &screenBits);
	
	SetRect(	&mBarRect, screenBits.bounds.left, oldMBarHeight,
				screenBits.bounds.right, MBarHeightInt);
			
	mBarRgn = NewRgn();
	RectRgn( mBarRgn, &mBarRect);
}

void AddWindowBar(void)
{
	DiffRgn( GetGrayRgn(), mBarRgn, GetGrayRgn());
}

void RemoveWindowBar(void)
{
	UnionRgn( GetGrayRgn(), mBarRgn, GetGrayRgn());
}

void CreateAHelpWindow(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i, cVRef;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;
	long		cParID;
	Rect	caRect;
	
	

	SetItemMark( ViewsMenu, 1, checkMark);

	if( !HelpAvalaible) return;
	if( AHelpDlog != 0L) return;
	
	GetPort( &savePort);
	
	AHelpDlog = GetNewDialog( 173, 0L, GetDialogWindow( ToolsDlog));
	SetPortDialogPort( AHelpDlog);
	
	GetPortBounds( GetDialogPort( AHelpDlog), &caRect);
	
	MySizeWindow( AHelpDlog, (**GetMainDevice()).gdRect.right, caRect.bottom, true);
	
	TextFont( kFontIDGeneva);	TextSize( 9);	TextFace( bold);
	
	ShowWindow( GetDialogWindow( AHelpDlog));
	SelectWindow2( GetDialogWindow( AHelpDlog));
	HiliteWindow( GetDialogWindow( AHelpDlog), true);
	
	SetItem( ViewsMenu, 1, "\pHide Online Help");
	
	InitWindowBar();
	
	SetPort( savePort);
}

void CloseAHelp(void)
{
	if( AHelpDlog != 0L)
	{
		SetItem( ViewsMenu, 1, "\pShow Online Help");
		
		RemoveWindowBar();
		DisposDialog( AHelpDlog);
	}
	AHelpDlog = 0L;
	
	SetItemMark( ViewsMenu, 1, noMark);
}