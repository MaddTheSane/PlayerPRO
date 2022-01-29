void 					DoContentHelp(WindowPtr theWindow, EventRecord *theEventI);
 pascal void 		ScrollProcHelp (ControlHandle theControl, short theCode);
void 					AdjustTextHelp ( ControlHandle vScroll);
Boolean				IsPressed( unsigned short );
void 					CreateHelpOnline( short whichSection);
void					PrintTEHandle( TEHandle hTE);

#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "stdio.h"

extern	short			mainVRefNum;
extern	long				mainParID;
extern	KeyMap			km;
extern	Boolean			HelpAvalaible;

static	TEHandle 			hTE;
static	long				curPosT;
static	Rect				scrollRect, textRect;
static	Point				zeroPt;
		DialogPtr			HelpDlog;
static	StScrpHandle		theStyle;
static	Ptr				Text;
static 	ControlHandle		vScroll;
static	MenuHandle		sectMenu;
static	long				*lineList, curSect;

static	ControlActionUPP		MyControlUPP;



void ShowSection( short selectionID)
{
short		i;
double		height;

	if( HelpDlog == 0L)
	{
		if( HelpAvalaible) CreateHelpOnline( selectionID);
	}
	else
	{
		curSect = selectionID;

		for( i=0; i < (*hTE)->nLines; i++)
		{
			if( lineList[ curSect] <= (*hTE)->lineStarts[ i]) break;
		}
		
		height = TEGetHeight( 0, (*hTE)->nLines, hTE);
		height /= (double) (*hTE)->nLines;
		
		SetCtlValue( vScroll, i );
		AdjustTextHelp( vScroll);
	}
}

void ChangeMenuName( Str255 curN)
{
	long		Result;
	short	PourCent, itemHit,i;
	DialogPtr	TheDia;
	Str255	theStr, aStr, aStr2;
	GrafPtr	myPort;
	Point		theCell;
	
	GetPort( &myPort);

	TheDia = GetNewDialog( 168,0L, (WindowPtr) -1L);
	SetPortDialogPort( TheDia);
	AutoPosition( TheDia);
	
	SetDText( TheDia, 4, curN);
	SelIText( TheDia, 4, 0, 32767);
	
	do
	{
		ModalDialog( MyDlgFilterDesc, &itemHit);
	
	}while( itemHit != 1 && itemHit != 2);

	if( itemHit == 1) GetDText( TheDia, 4, curN);
	
	DisposDialog( TheDia);
}

void WriteLineList(void)
{
/*	Handle	myRes;
	long		cParID;
	short	i = 128, rID, iFileRef, cVRef;
	OSType	rType;
	Str255	rName;

	HGetVol( 0L, &cVRef, &cParID);
	HSetVol( 0L, mainVRefNum, mainParID);

	if( iHelpPP != -1)
	{
		UseResFile( iHelpPP);
		for( i = 0; i < CountMenuItems( sectMenu); i++)
		{
			myRes = GetResource( 'SECT', i + 128);
			if( myRes != 0L)
			{
				RemoveResource( myRes);
				MyDisposHandle( & myRes);
			}
			
			myRes = MyNewHandle( 4L);
			*((long*) (*myRes)) = lineList[ i];
			GetItem( sectMenu,  i + 1, rName);
			
			UseResFile( iHelpPP);
			AddResource( myRes, 'SECT', i + 128, rName);
			ReleaseResource( myRes);
		}
	}
	
	HSetVol( 0L, cVRef, cParID);*/
}

void CreateMenuHelp(void)
{
	Handle	myRes;
	short	i = 128, rID;
	OSType	rType;
	Str255	rName;
	
	sectMenu = NewMenu( 945, "\pSections:");
	
	do
	{
		myRes = GetResource( 'SECT', i);
		if( myRes != 0L)
		{
			GetResInfo( myRes, &rID, &rType, rName);
			AppendMenu( sectMenu, rName);

			lineList[ i - 128] = *((long*) (*myRes));
		}
		i++;
	}
	while( myRes != 0L);
}

void UpdateSection(void)
{

}

void CreateHelpOnline( short whichSection)
{
short			itemType,itemHit, iFileRefI, iFileRef, cVRef;
GrafPtr			myPort;
Handle			itemHandle;
Rect				itemRect;
Boolean			ScrollBasActive;
long				GestaltResponse, inOutBytes, cParID;
OSErr			iErr;
FSSpec			spec;

if( HelpDlog != 0L)
{
	SelectWindow2( GetDialogWindow( HelpDlog));
	SetPortDialogPort( HelpDlog);
	if( curSect != whichSection)
	{
		curSect = whichSection;
		UpdateSection();
	}
	return;
}

SetItemMark( ViewsMenu, 1, checkMark);

curSect = whichSection;

lineList = (long*) MyNewPtr( 8000L);

/** Lecture du fichier Help **/

//HGetVol( 0L, &cVRef, &cParID);
//HSetVol( 0L, mainVRefNum, mainParID);

pStrcpy( spec.name, "\pHelp PP");
spec.vRefNum = mainVRefNum;
spec.parID = mainParID;

iErr = FSpOpenDF( &spec, fsCurPerm, &iFileRefI);
if( iErr)
{
	Erreur(56 , iErr);
	return;
}
GetEOF( iFileRefI, &inOutBytes);
Text = MyNewPtr( inOutBytes);
FSRead( iFileRefI, &inOutBytes, Text);			
FSClose( iFileRefI);

/*******************/

/*
if( iHelpPP != -1)
{
	theStyle = (StScrpHandle) Get1Resource( 'styl', 128);
	if( theStyle != 0L) DetachResource( (Handle) theStyle);
	
	CreateMenuHelp();
}*/

//HSetVol( 0L, cVRef, cParID);

GetPort( &myPort);

HelpDlog = GetNewDialog( 169,0L, (WindowPtr) -1L);
SetPortDialogPort( HelpDlog);
TextFont( 4);		TextSize( 9);

SetWindEtat( GetDialogWindow(HelpDlog));

/*** Zone Texte ***/

GetPortBounds( GetDialogPort( HelpDlog), &itemRect);

itemRect.bottom -=15;
itemRect.right -=16;
itemRect.left ++;

hTE = TEStyleNew( &itemRect, &itemRect);

/*** Zone Scroll ***/

GetPortBounds( GetDialogPort( HelpDlog), &itemRect);

itemRect.right++;
itemRect.top--;
itemRect.left = itemRect.right - 16;
itemRect.bottom -= 14;

vScroll = NewControl( GetDialogWindow( HelpDlog), &itemRect, "\p", true, 0, 0, 0, scrollBarProc, 0L);
SetCRefCon( vScroll, 1);

TESetAlignment( teCenter, hTE);
if( theStyle != 0L) TEStyleInsert( Text, GetPtrSize( Text), theStyle, hTE);
TEAutoView( true, hTE);
TECalText( hTE);
SetScroll( vScroll, hTE);

SelectWindow2( GetDialogWindow( HelpDlog));
ShowWindow( GetDialogWindow( HelpDlog));
}

void CloseHelpOnline(void)
{
	if( HelpDlog != 0L)
	{
		DisposDialog( HelpDlog);		HelpDlog = 0L;
		TEDispose( hTE);
		MyDisposePtr( & Text);
		if( theStyle != 0L) MyDisposHandle(  (Handle*) & theStyle);
		theStyle = 0L;
		MyDisposePtr( (Ptr*) &lineList);
		DisposeMenu(sectMenu);
	
		SetItemMark( ViewsMenu, 1, noMark);
	}
}

void DoGrowHelpOnline(void)
{
long		lSizeVH;
GrafPtr	SavePort;
Rect		caRect, temp, cellRect, tempRect;
short	cur, tempB, tempA, itemType, avant;
Handle	itemHandle;

	GetPort( &SavePort);
 	SetPortDialogPort( HelpDlog);

	temp.left = 300;
	temp.right = 5000;
	temp.top = 100;
	temp.bottom = 5000;
	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( HelpDlog), theEvent.where, &temp);

	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( HelpDlog), &caRect);

		tempA = caRect.right;
		tempB = caRect.bottom;
	}


	MySizeControl( vScroll, 16, tempB - 13);
	MyMoveControl( vScroll, tempA - 15, -1);
	
	GetPortBounds( GetDialogPort( HelpDlog), &caRect);
	
	InvalWindowRect( GetDialogWindow( HelpDlog), &caRect);
	EraseRect( &caRect);
	
	MySizeWindow( HelpDlog, tempA, tempB, true);
	
	GetPortBounds( GetDialogPort( HelpDlog), &caRect);
	
	(*hTE)->viewRect = caRect;
	(*hTE)->viewRect.bottom -= 15;
	(*hTE)->viewRect.right -= 16;
	(*hTE)->viewRect.left ++;
	
	(*hTE)->destRect.right = (*hTE)->viewRect.right;
	
	TECalText( hTE);
	SetScroll( vScroll, hTE);
	AdjustTextHelp( vScroll);

	SetPort( SavePort);
}

void DoItemPressHelpOnline( short whichItem, DialogPtr whichDialog)
{
 		short		bogus,itemType, ctlPart, i;
		Point			Location, myPt;
		Handle		itemHandle;
		Rect			caRect, itemRect;
		ControlHandle	theControl;
		RgnHandle		saveClip;
		GrafPtr		savePort;
		long			mresult;
		Str255		rName;
	
	GetPort( &savePort);
	SetPortDialogPort( whichDialog);
	
	TextFont( 4);	TextSize( 9);
	
	if (theEvent.what == mouseDown) /* See if a mouse click */
	{
		DoContentHelp( GetDialogWindow( HelpDlog), &theEvent);
		
		GetPortBounds( GetDialogPort( HelpDlog), &caRect);
		
		itemRect.top = caRect.bottom -16;
		itemRect.bottom = caRect.bottom;
		itemRect.left = 170;
		itemRect.right = 190;

		myPt = theEvent.where;
		GlobalToLocal( &myPt);
		
		if( PtInRect( myPt, &itemRect))
		{
			InsertMenu( sectMenu, hierMenu );

			myPt.v = itemRect.top + 4;	myPt.h = itemRect.left;
			LocalToGlobal( &myPt);
			
			SetItemMark( sectMenu, curSect + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	sectMenu,
									myPt.v,
									myPt.h,
									curSect + 1);
										
			SetItemMark( sectMenu, curSect + 1, 0);
		
			if ( HiWord(mresult ) != 0 )
			{
				curSect = (Byte) LoWord( mresult) - 1;
				
				itemRect.left = 0;		itemRect.right = 170;
				itemRect.top += 2;
				EraseRect( &itemRect);
				InvalWindowRect( GetDialogWindow( HelpDlog), &itemRect);
				
				GetKeys( km);
				if( IsPressed( 0x37) == true)		// Change the current item
				{
					myPt.v = 2;	myPt.h = 2;
					lineList[ curSect] = TEGetOffset( myPt, hTE);
					
					GetItem( sectMenu,  curSect + 1, rName);
					ChangeMenuName( rName);
					SetItem( sectMenu, curSect + 1, rName);
					
					WriteLineList();
				}
				else if( IsPressed( 0x31) == true)	// Add a new item
				{
					myPt.v = 2;	myPt.h = 2;
					lineList[ CountMenuItems( sectMenu)] = TEGetOffset( myPt, hTE);
					
					pStrcpy( rName, "\pUntitled");
					ChangeMenuName( rName);
					AppendMenu( sectMenu, rName);
					
					WriteLineList();
				}
				else
				{
					ShowSection( curSect);
				}
			}
			
			DeleteMenu( GetMenuID( sectMenu));
		}
	}
	
	switch( whichItem)
	{
	}
	
	SetPort( savePort);
}

void AdjustTextHelp ( ControlHandle vScroll)
{
	short			oldScroll, newScroll, delta;
	double			height;
	TEHandle			CurrentTE;
	TEStyleHandle		hStyle;
	
	CurrentTE = hTE;
	
	oldScroll = (**CurrentTE).viewRect.top - (**CurrentTE).destRect.top;
	hStyle = TEGetStyleHandle( hTE);
	
	height = TEGetHeight( 0, (*CurrentTE)->nLines, CurrentTE);
	height /= (*CurrentTE)->nLines;

	newScroll = GetCtlValue(vScroll) * height;
	delta = oldScroll - newScroll;
	if (delta != 0) TEPinScroll( 0, delta, CurrentTE);
}

void DoContentHelp(WindowPtr theWindow, EventRecord *theEventI)
{
	short			cntlCode;
	ControlHandle 		theControl;
	GrafPtr			savePort;
		
	GetPort(&savePort);
	SetPortWindowPort(theWindow);

	GlobalToLocal(&theEventI->where);
	cntlCode = FindControl(theEventI->where, theWindow, &theControl);
	switch( cntlCode)
	{
	case kControlIndicatorPart:
		TrackControl(theControl, theEventI->where, 0L);
		AdjustTextHelp( theControl);
	break;
	
	case kControlUpButtonPart:
	case kControlDownButtonPart:
	case kControlPageUpPart:
	case kControlPageDownPart:
   
		MyControlUPP = NewControlActionUPP( ScrollProcHelp);
		TrackControl(theControl, theEventI->where, MyControlUPP);
		DisposeControlActionUPP( MyControlUPP);
	
	break;
	}
	
	LocalToGlobal(&theEventI->where);
	SetPort(savePort);
}

pascal void ScrollProcHelp (ControlHandle theControl, short theCode)
{
	short		pageSize;
	short		scrollAmt;
	short 		oldCtl;
	TEHandle	CurrentTE;
	double		height;
	
	if (theCode == 0)
		return ;
	
	CurrentTE = hTE;
	
	height = TEGetHeight( 0, (*CurrentTE)->nLines, CurrentTE);
	height /= (*CurrentTE)->nLines;
	
	pageSize = ((**CurrentTE).viewRect.bottom-(**CurrentTE).viewRect.top) /  height - 1;
	
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
	}

	oldCtl = GetCtlValue(theControl);
	SetCtlValue(theControl, oldCtl+scrollAmt);

	AdjustTextHelp( theControl);
}

void UpdateHelpOnline(void)
{
	GrafPtr				CurrentPort;
	PicHandle				myPict;
	Str255				myName;
	Rect					caRect, destRect;

	GetPort(&CurrentPort);
	SetPortDialogPort( HelpDlog);
	
	BeginUpdate( GetDialogWindow( HelpDlog));
		
		DrawDialog( HelpDlog);
		
		GetPortBounds( GetDialogPort( HelpDlog), &caRect);
		TEUpdate( &caRect, hTE);
		DrawGrowIcon( GetDialogWindow( HelpDlog));
		
		MoveTo( 0, caRect.bottom - 15);
		LineTo( caRect.right, caRect.bottom - 15);
		
		MoveTo( 4, caRect.bottom - 4);
		DrawString( "\pSection:   ");
		
		GetItem( sectMenu, curSect + 1, myName);
		DrawString( myName);
		
		myPict = GetPicture( 200);
		destRect = (*myPict)->picFrame;
		destRect.top += caRect.bottom - 13;
		destRect.bottom += caRect.bottom - 13;
		
		destRect.left += 170;
		destRect.right += 170;
		
		DrawPicture( myPict, &destRect);
	//	ReleaseResource( ( Handle) myPict);
		
	EndUpdate( GetDialogWindow( HelpDlog));
	
	SetPort( CurrentPort);
}

void PrintHelpOnline(void)
{
	PrintTEHandle( hTE);
}
