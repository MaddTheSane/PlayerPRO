#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"

	/******** HELP MODULE ********/
	enum
	{
		HInfo	= 12,
		HPlay	= 14,
		HZoom	= 15,
		HTrack	= 7,
		HIns	= 8,
		HPat	= 1
	};
#define	AHELPSIZE 6


	static	short					AHelp[ AHELPSIZE] = { HInfo, HPlay, HZoom, HTrack, HIns, HPat};

	void DoHelpClassic( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}

	/*****************************/


enum
{
	ePlay,
	eZoom
};


	extern	EventRecord				theEvent;
	extern	MenuHandle				TrackView;
	extern	KeyMap					km;
	extern	Cursor					ZoomInCrsr, ZoomOutCrsr, PlayCrsr;
	extern	RGBColor				theColor;
	extern	WindowPtr				oldWindow;

			DialogPtr				ClassicDlog;
			
	static	Rect					NoteZone;
	static	short					PatCopyXIM, PLCopyXIM, ReaderCopy, curSelecTrack, curSelecInstru;
	static	short					CurrentPat, CurHaut, CurLarg;
	static	short					PatternLength, ZoomLevelPat;
	static	ControlHandle			theCtl, InfoBut, PlayBut, ZoomBut;
	static	short 					InvertN, SelectMode;

Boolean DialogPatternInfo( short thePos);

void GetInvertRect( short Alpha, Rect *newRect)
{
	newRect->left = (Alpha - GetControlValue( theCtl))*CurLarg;
	newRect->right = newRect->left + CurLarg;
	newRect->top = NoteZone.top;
	newRect->bottom = NoteZone.bottom;
}

void WriteLevel(void)
{
Str255	aStr, str;
short	x;
GrafPtr	aPort;

	GetPort( &aPort);
	SetPortDialogPort( ClassicDlog);

	TextFont( 4);	TextSize( 9);

	NumToString( ZoomLevelPat, aStr);
	pStrcat( aStr, "\p x (");
	
	x = GetControlValue( theCtl);
	NumToString( x, str);
	pStrcat( aStr, str);
	pStrcat( aStr, "\p-");
	NumToString( x + (PatternLength/ZoomLevelPat), str);
	pStrcat( aStr, str);
	pStrcat( aStr, "\p)");
	
	SetDText( ClassicDlog, 9, aStr);
	
	SetPort( aPort);
}

void SetControlHClassic(void)
{
Str255	aStr, str;
short	x;
Rect	caRect;
	
	GetPortBounds( GetDialogPort( ClassicDlog), &caRect);

	MyMoveControl( theCtl, -1, caRect.bottom-15);
//	SizeControl( theCtl, ClassicDlog->portRect.right -13, 16);

	if (NoteZone.right < caRect.right -13)
		MySizeControl( theCtl, NoteZone.right + 2, 16);
	else MySizeControl( theCtl, caRect.right -13, 16);
	
	if (ZoomLevelPat != 1)
	{
		SetControlMinimum( theCtl, 0);
		SetControlMaximum( theCtl, PatternLength - PatternLength/ZoomLevelPat);

		if (gUseControlSize) SetControlViewSize( theCtl, PatternLength/ZoomLevelPat);
	}
	else
	{
		SetControlMinimum( theCtl, 0);
		SetControlMaximum( theCtl, 0);
		SetControlValue( theCtl, 0);
	}
	
	WriteLevel();
}

void SetWClassic( short No)
{
	Str255		String, aStr, theStr;
	short		x;

	if (ClassicDlog == NULL) return;

	No = PatCopyXIM;

	NumToString( (long) No, String);
	pStrcpy( aStr, "\p");
	if (No< 10) pStrcat( aStr, "\p0");
	if (No< 100) pStrcat( aStr, "\p0");
	pStrcat( aStr, String);
	
	pStrcpy( String, "\pPattern: ");
	pStrcat( String, aStr);
	
	theStr[ 0] = 20;
	for (x = 0; x < 20; x++) theStr[ x + 1] = curMusic->partition[ PatCopyXIM]->header.name[ x];
	for (x = 1; x < 20; x++) if (theStr[ x] == 0) { theStr[ 0] = x - 1; break;}
	pStrcat( String, "\p ");
	pStrcat( String, theStr);
	
	SetWTitle( GetDialogWindow( ClassicDlog), String);
	
	NumToString( PatternLength, aStr);
	pStrcat( aStr, "\p x ");
	NumToString( curMusic->header->numChn, String);
	pStrcat( aStr, String);
	SetDText( ClassicDlog, 10, aStr);
}

/*
short FindVal( short myFrq)
{
short	NCount = 1;

    while (NCount <= 84)
    {
    	if (myFrq >= GetOldPeriod( NCount, NOFINETUNE)) return NCount;	//if (myFrq >= MADDriver->pitchTable[ NCount][ 0] ) return NCount;
    	NCount++;
  	}
  	
  	return 0;
}*/

void AdjustZoomClassic2( Rect	*vRect)
{
	short		tempA, tempB;
	Rect		stdRect;

	GetWindowStandardState( GetDialogWindow( ClassicDlog), &stdRect);

	tempA = vRect->right - vRect->left;
	tempB = vRect->bottom - NoteZone.top - vRect->top;
	
	tempA /= PatternLength;
	tempA *= PatternLength;
	
	tempB /= NUMBER_NOTES;
	tempB *= NUMBER_NOTES;
	
	stdRect.right = stdRect.left + tempA;
	stdRect.bottom = stdRect.top + tempB + NoteZone.top + 15;

	SetWindowStandardState( GetDialogWindow( ClassicDlog), &stdRect);
}

void DoGrowClassic(void)
{
long		lSizeVH;
GrafPtr		SavePort;
Rect		temp;
long		tempA, tempB;
Rect	caRect;
	
	


	GetPort( &SavePort);
 	SetPortDialogPort( ClassicDlog);

	temp.left = PatternLength;
	temp.right = PatternLength * 100;
	
	temp.top = NUMBER_NOTES + NoteZone.top + 16;
	temp.bottom = NUMBER_NOTES*16 + NoteZone.top + 16;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( ClassicDlog), theEvent.where, &temp);
	
	GetPortBounds( GetDialogPort( ClassicDlog), &caRect);

	if (lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH) - NoteZone.top;
	}
	else
	{	
		tempA = caRect.right;
		tempB = caRect.bottom - NoteZone.top;
		
		if (tempA >= temp.right) tempA = temp.right-1;
		if (tempB >= temp.bottom) tempB = temp.bottom-1;
	}
	
	if (tempA < PatternLength) tempA = PatternLength;

	if ((theEvent.modifiers & optionKey) != 0)
	{
		tempA = tempA / PatternLength;
		tempA = tempA * PatternLength;
	}
	
	tempB /= NUMBER_NOTES;
	tempB *= NUMBER_NOTES;

	MySizeWindow( ClassicDlog, tempA, tempB + NoteZone.top + 15, true);

	GetPortBounds( GetDialogPort( ClassicDlog), &caRect);

	NoteZone.bottom = caRect.bottom - 15;
	NoteZone.right = caRect.right;
	NoteZone.right /= PatternLength;
	NoteZone.right *= PatternLength;


	CurHaut = (NoteZone.bottom - NoteZone.top) / NUMBER_NOTES;
	CurLarg = caRect.right / PatternLength;
	CurLarg	*= ZoomLevelPat;

	InvalWindowRect( GetDialogWindow( ClassicDlog), &caRect);
	SetControlHClassic();
	
	SetPort( SavePort);
}

void AfficheClassicRect( short tt, short i)
{
Boolean				Note;
Cmd					*theCommand;
short				val, Pos;
Rect				destRect;

	Note = false;
	
	if (tt < 0) return;
	if (tt >= curMusic->partition[ PatCopyXIM]->header.size) return;
	
	theCommand = GetMADCommand( tt, i, curMusic->partition[ PatCopyXIM]);
	
	val = theCommand->ins;
	if (val != 0)
	{
		if (curSelecInstru == -1) Note = true;
		else if (curSelecInstru == val - 1) Note = true;
	}
	
	val = theCommand->note;
	if (val == 0xFF) Note = false;
	if (val == 0xFE) Note = false;
	
	Pos = NUMBER_NOTES - val;
	
	if (Note == true)
	{
		destRect.left = (tt - GetControlValue( theCtl)) * CurLarg;
		destRect.right = destRect.left + CurLarg;
		destRect.top = Pos * CurHaut + NoteZone.top;
		if (destRect.top < NoteZone.top) destRect.top = NoteZone.top;
		
		destRect.bottom = destRect.top + CurHaut;
		if (destRect.bottom > NoteZone.bottom) destRect.bottom = NoteZone.bottom;
		
 		SwitchColor( i);
 		PaintRect( &destRect);
	}
}

void DrawCurrentNote( short tt)
{
 	short				i;
	Rect				destRect;
	GrafPtr				savePort;

	if (ClassicDlog == NULL) return;
	
	GetPort( &savePort);
	SetPortDialogPort( ClassicDlog);
	
	destRect.left = (tt - GetControlValue( theCtl))*CurLarg;
	destRect.right = destRect.left + CurLarg;
				
	destRect.top = NoteZone.top;
	destRect.bottom = NoteZone.bottom;

	ForeColor( blackColor);
	PaintRect( &destRect);
	
	if (curSelecTrack != -1) AfficheClassicRect( tt, curSelecTrack);
	else
	{
		for(i=0; i< curMusic->header->numChn; i++) AfficheClassicRect( tt, i);
	}
	
	if (tt == InvertN)
	{
		GetInvertRect( InvertN, &destRect);
		InvertRect( &destRect);
	}
	
	ForeColor( blackColor);
	
	SetPort( savePort);
}

void DrawCurrentClassic(void)
{
	Point   			cSize;
 	short				i, tt;
	GrafPtr				SavePort;
	Rect				tRect;
	RgnHandle			saveClipRgn;
	Rect				caRect;
	
	if (ClassicDlog == NULL) return;

	GetPort( &SavePort);
	SetPortDialogPort( ClassicDlog);
	
	GetPortBounds( GetDialogPort( ClassicDlog), &caRect);

	MoveTo( 0, NoteZone.top - 1);
	LineTo( caRect.right, NoteZone.top - 1);
	PaintRect( &NoteZone);
	
	tRect = NoteZone;
	tRect.left = NoteZone.right;
	tRect.right = caRect.right;
	tRect.bottom = caRect.bottom-15;
	RGBForeColor( &theColor);
	PaintRect( &tRect);
	ForeColor( blackColor);
	
	MoveTo( tRect.left, tRect.top);
	LineTo( tRect.left, tRect.bottom);
	
	saveClipRgn = NewRgn();								/* get an empty region */
	GetClip( saveClipRgn );								/* save current */
	
	ClipRect( &NoteZone);

	if (curMusic != NULL)
	{
		for(tt=0; tt<PatternLength; tt++)
		{	
			cSize.v = tt;
			
			if (curSelecTrack != -1) AfficheClassicRect( tt, curSelecTrack);
			else
			{
				for(i=0; i< curMusic->header->numChn; i++) AfficheClassicRect( tt, i);
			}
		}
	}
	ForeColor( blackColor);
	
	SetClip( saveClipRgn );			/* restore previous value */
	DisposeRgn( saveClipRgn );		/* not needed any more */
	
	SetPort( SavePort);
}

void UpdatePartitionWindow( DialogPtr);

void DoNullClassic(void)
{
	GrafPtr		SavePort;
 	Rect		tempRect, resulRect;
 	Point		pt;
 	short		zz;
	Rect		caRect;
	RgnHandle	visibleRegion;


 	if (ClassicDlog == NULL) return;
 
 	GetPort( &SavePort);
 	SetPortDialogPort( ClassicDlog);

	TextFont( 4);
	TextSize( 9);

	if (PatCopyXIM != MADDriver->Pat || PLCopyXIM != MADDriver->PL || PatternLength != curMusic->partition[ PatCopyXIM]->header.size)
	{
		PatCopyXIM = MADDriver->Pat;
		PLCopyXIM = MADDriver->PL;
		if (PatternLength != curMusic->partition[ PatCopyXIM]->header.size)
		{
			PatternLength = curMusic->partition[ PatCopyXIM]->header.size;
			theEvent.what = 0;
			DoGrowClassic();
		}

		GetPortBounds( GetDialogPort( ClassicDlog), &caRect);		

		PatternLength = curMusic->partition[ PatCopyXIM]->header.size;
		CurLarg = caRect.right / PatternLength;
		CurLarg *= ZoomLevelPat;
		
		SetControlValue( theCtl, 0);
		
		DrawCurrentClassic();
		SetWClassic( PatCopyXIM);
	//	AdjustZoomClassic();
	//	SetMaxWindow( OsciL + 15, OsciVStart + 15 + OsciNo * (InterText + OsciH), ClassicDlog);
		WriteLevel();
		
		InvertN = -1;
	}

	if (InvertN != MADDriver->PartitionReader)
	{
		zz = InvertN;
		InvertN = MADDriver->PartitionReader;
		if (zz != -1) DrawCurrentNote( zz);
		
		GetInvertRect( InvertN, &tempRect);
		if (!SectRect( &tempRect, &NoteZone, &resulRect))
		{
			SetControlValue( theCtl, InvertN);
			DrawCurrentClassic();
			WriteLevel();
			
			GetInvertRect( InvertN, &tempRect);
		}
		InvertRect( &tempRect);
	}
		
	if (GetDialogWindow( ClassicDlog) == oldWindow)
	{
		pt = theEvent.where;
		GlobalToLocal( &pt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetWindowPort( oldWindow), visibleRegion);
		
		if (PtInRgn( pt, visibleRegion))
		{
			if (PtInRect( pt, &NoteZone))
			{
				GetKeys( km);
				if (IsPressed( 0x003A) && ZoomLevelPat != 1)
				{
					SetCursor( &ZoomOutCrsr);
				}
				else if (IsPressed( 0x0037) && ZoomLevelPat < 32)
				{
					SetCursor( &ZoomInCrsr);
				}
				else
				{
					if (SelectMode == ePlay) SetCursor( &PlayCrsr);
					else if (SelectMode == eZoom) SetCursor( &ZoomInCrsr);
				}
			}
			else SetCursor( GetQDGlobalsArrow( &qdarrow));
		}
		else SetCursor( GetQDGlobalsArrow( &qdarrow));

		DisposeRgn( visibleRegion);
	}
	SetPort( SavePort);
}

extern	RGBColor	theColor;

void UpdateClassicInfo(void)
{
	GrafPtr		SavePort;
	
	if (ClassicDlog == NULL) return;
	
 	GetPort( &SavePort);
 	SetPortDialogPort( ClassicDlog);
 	
	PatCopyXIM = -1;
	DoNullClassic();

	SetPort( SavePort);
}

void  UpdateClassicWindow( DialogPtr GetSelection)  	/* Pointer to this dialog */
{ 
		Rect   		tempRect, itemRect;   			/* Temporary rectangle */
 		GrafPtr		SavePort;
 		short		itemType;
 		Handle		itemHandle;
 		RgnHandle	visibleRegion;


 		GetPort( &SavePort);
 		SetPortDialogPort( ClassicDlog);

 		TextFont( 4);	TextSize( 9);

		BeginUpdate( GetDialogWindow( ClassicDlog));
		
		DrawCurrentClassic();
		
		GetInvertRect( InvertN, &tempRect);
		InvertRect( &tempRect);

		DrawGrowIconP( ClassicDlog);

		GetDialogItem( ClassicDlog, 13, &itemType, &itemHandle, &itemRect);
		
		if (curSelecTrack >= 0)
		{
			SwitchColor( curSelecTrack);
			PaintRect( &itemRect);
			itemRect.right--;
			itemRect.bottom--;
			FrameRectRelief( &itemRect);
		}
		else EraseRect( &itemRect);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( ClassicDlog), visibleRegion);

		UpdateDialog( ClassicDlog, visibleRegion);
		
		DisposeRgn( visibleRegion);		

		EndUpdate( GetDialogWindow( ClassicDlog));

		SetPort( SavePort);
} 

pascal void actionProcClassic(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			maxValue, minValue, curVal, XX, sVal;
RgnHandle		aRgn;

if (ctlPart <= 0) return;

lRefCon = GetControlReference( theControl);
maxValue = GetControlMaximum( theControl);
minValue = GetControlMinimum( theControl);
curVal = sVal = GetControlValue( theControl);

	switch( ctlPart)
	{
		case kControlUpButtonPart:
			curVal -= 1;
			if (curVal < minValue) curVal = minValue;
		break;
		
		case kControlDownButtonPart:
			curVal += 1;
			if (curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlPageUpPart:
			curVal -= PatternLength;
			if (curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			curVal += PatternLength;
			if (curVal > maxValue) curVal = maxValue;
		break;
	}
	
	SetControlValue( theControl, curVal);
	
	if (sVal != curVal)
	{
		aRgn = NewRgn();
		ScrollRect( &NoteZone, (sVal - curVal)*CurLarg, 0, aRgn);
		InvalWindowRgn( GetDialogWindow( ClassicDlog), aRgn);
		UpdateClassicWindow( ClassicDlog);
		DisposeRgn( aRgn);
		
		WriteLevel();
	}
}

static	long lastWhen = 0;

void DoItemPressClassic( short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
		long				mresult;
 		short				itemType, oldPosM, bogus, ctlPart;
 		Handle 				itemHandle;
 		Rect				itemRect;
 		GrafPtr				SavePort;
 		Boolean				IsPlay;
 		ControlHandle		theControl;
 		Str255				theStr, theStr2;
 		Point				myPt;
 		ControlActionUPP	MyControlUPP;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( ClassicDlog);
 		
 		TextFont(4);
 		TextSize(9);
 		
		if (theEvent.what == mouseDown) /* See if a mouse click */
		{
			myPt = theEvent.where;
			GlobalToLocal(&myPt);
			
			if(PtInRect( myPt, &NoteZone))
			{
				GetKeys( km);
				if (IsPressed( 0x003A))
				{
					while (Button()) {};
					if (ZoomLevelPat > 1)
					{
						ZoomLevelPat /= 2;
						CurLarg	/= 2;
						
						SetControlValue( theCtl, GetControlValue( theCtl) / 2);
						SetControlHClassic();
						
						DrawCurrentClassic();
						InvertN = -1;
					}
				}
				else if (IsPressed( 0x0037) || SelectMode == eZoom)
				{
					while (Button()) {};
					if (ZoomLevelPat < 32)
					{
						ZoomLevelPat *= 2;
						CurLarg	*= 2;
						SetControlHClassic();
						
						bogus = (myPt.h * PatternLength) / (NoteZone.right - NoteZone.left);
						bogus *= 2;
						bogus -= PatternLength/2;
						
						SetControlValue( theCtl, GetControlValue( theCtl)*2 + bogus);
						
						DrawCurrentClassic();
						InvertN = -1;
					}
				}
				else if (SelectMode == ePlay)
				{
					IsPlay = MusicPlayActive;
					
					GetMouse( &myPt);
					myPt.h /= CurLarg;
					myPt.h += GetControlValue( theCtl);
					MADDriver->PartitionReader = myPt.h;
					if (MADDriver->PartitionReader < 0) MADDriver->PartitionReader = 0;
					else if (MADDriver->PartitionReader >= curMusic->partition[ PatCopyXIM]->header.size) MADDriver->PartitionReader = curMusic->partition[ PatCopyXIM]->header.size - 1;
					
					if (IsPlay == false) DoPlay();
					
					while (Button())
					{
						DoGlobalNull();
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
					}
					
					if (IsPlay == false) DoPause();
				}
			}
			else	// *** Press in scrollbars !!! ***
			{
				ctlPart = FindControl( myPt, GetDialogWindow( whichDialog), &theControl);
				if (theControl == theCtl)
				{
					if (ctlPart == kControlIndicatorPart)
					{
						bogus = TrackControl( theControl, myPt, NULL);
						if (bogus != 0)
						{
							DrawCurrentClassic();
						}
					}
					else if (ctlPart > 0)
					{
						MyControlUPP = NewControlActionUPP( actionProcClassic);
						TrackControl(theControl, myPt, MyControlUPP);
						DisposeControlActionUPP( MyControlUPP);
					}
					WriteLevel();
				}
			}
		}   						/* End of mouseDown */
		
		switch( whichItem)
		{
			case 12:
				if (GetControlHilite( InfoBut) == 0  && MyTrackControl( InfoBut, theEvent.where, NULL))
				{
					DialogPatternInfo( PatCopyXIM);
				}
			break;

			case 14:
				if (GetControlHilite( PlayBut)	 == 0 && MyTrackControl( PlayBut, theEvent.where, NULL))
				{
					SelectMode = ePlay;
					HiliteControl( PlayBut, kControlButtonPart);
					HiliteControl( ZoomBut, 0);
				}
			break;

			case 15:
				if (GetControlHilite( ZoomBut) == 0 && MyTrackControl( ZoomBut, theEvent.where, NULL))
				{
					SelectMode = eZoom;
					HiliteControl( PlayBut, 0);
					HiliteControl( ZoomBut, kControlButtonPart);
				}
			break;

			
			case 7:
				SetUpTracksMenu();
			
				InsertMenuItem( TrackView, "\pAll tracks", 0);
			
				InsertMenu( TrackView, hierMenu );
				GetDialogItem( whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( TrackView, curSelecTrack + 2, 0xa5);
				
				mresult = PopUpMenuSelect(	TrackView,
											myPt.v,
											myPt.h,
											curSelecTrack + 2);
											
				SetItemMark( TrackView, curSelecTrack + 2, 0);
			
				if ( HiWord(mresult ) != 0 )
				{
					curSelecTrack = (Byte) LoWord( mresult) - 2;

					if (curSelecTrack == -1)
					{
						SetDText( whichDialog, 5, "\pAll Channels");
					}
					else
					{
						NumToString( curSelecTrack + 1, theStr);						
						SetDText( whichDialog, 5, theStr);
					}
					GetDialogItem( whichDialog, 13, &itemType, &itemHandle, &itemRect);
					if (curSelecTrack >= 0)
					{
						SwitchColor( curSelecTrack);
						PaintRect( &itemRect);
						itemRect.right--;
						itemRect.bottom--;
						FrameRectRelief( &itemRect);
					}
					else EraseRect( &itemRect);

					InvalWindowRect( GetDialogWindow( whichDialog), &NoteZone);
				}
				DeleteMenu( GetMenuID( TrackView));
				
				DeleteMenuItem( TrackView, 1);
			break;
			
			case 8:
				InsertMenuItem( InstruMenu, "\pAll instruments", 0);
			
				InsertMenu( InstruMenu, hierMenu );
				GetDialogItem( whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( InstruMenu, curSelecInstru + 2, 0xa5);
				
				mresult = PopUpMenuSelect(	InstruMenu,
											myPt.v,
											myPt.h,
											curSelecInstru + 2);
				
				SetItemMark( InstruMenu, curSelecInstru + 2, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					curSelecInstru = (Byte) LoWord( mresult) - 2;
					
					if (curSelecInstru == -1)
					{
						SetDText( whichDialog, 6, "\pAll instruments");
					}
					else
					{
						NumToString( curSelecInstru+1, theStr2);
						pStrcat( theStr2, "\p ");
						strcpy( (Ptr) theStr, curMusic->fid[ curSelecInstru].name);
						MyC2PStr( (Ptr) theStr);
						pStrcat( theStr2, theStr);
						SetDText( whichDialog, 6, theStr2);
					}
					InvalWindowRect( GetDialogWindow( whichDialog), &NoteZone);
				}
				DeleteMenu( GetMenuID( InstruMenu));
				
				DeleteMenuItem( InstruMenu, 1);
			break;
		}
		
		SetPort( SavePort);
}

void CreateClassicWindow(void)
{
	Rect		itemRect, aRect, caRect;
	Handle		itemHandle;
	short		itemType;
	Str255		aStr;

	if (ClassicDlog != NULL)
	{
		SetWindEtat( GetDialogWindow( ClassicDlog));
		return;
	}

	SetItemMark( ViewsMenu, mPatternV, checkMark);

	ClassicDlog = GetNewDialog( 147, NULL, GetDialogWindow( ToolsDlog));
	SetWindEtat( GetDialogWindow( ClassicDlog));
	SetPortDialogPort( ClassicDlog);
	SelectWindow2( GetDialogWindow( ClassicDlog));
	
	TextFont( 4);	TextSize( 9);
	
	SelectMode = ePlay;
	ZoomLevelPat = 1;
	
	SetRect( &itemRect, 0, 0, 30, 16);
	theCtl = NewControl( 	GetDialogWindow( ClassicDlog),
							&itemRect,
							"\p.",
							true,
							0,
							0,
							0,
							16,
							0);
							
	GetDialogItem ( ClassicDlog, 12, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	InfoBut = NewControl( 	GetDialogWindow( ClassicDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							149,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem ( ClassicDlog, 14, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	PlayBut = NewControl( 	GetDialogWindow( ClassicDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							160,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem ( ClassicDlog, 15, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ZoomBut = NewControl( 	GetDialogWindow( ClassicDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							177,
							kControlBevelButtonNormalBevelProc,
							0);
	
	HiliteControl( PlayBut, kControlButtonPart);
	
	SetControlHClassic();

	ReaderCopy = MADDriver->PartitionReader;
	PatCopyXIM = MADDriver->Pat;
	InvertN = -1;
	PatternLength = curMusic->partition[ PatCopyXIM]->header.size;
	SetWClassic( MADDriver->Pat);
	
	GetPortBounds( GetDialogPort( ClassicDlog), &caRect);

	GetDialogItem ( ClassicDlog, 1, &itemType, &itemHandle, &NoteZone);
	NoteZone.bottom = caRect.bottom - 15;
	NoteZone.right = caRect.right;
	
	CurHaut = (NoteZone.bottom - NoteZone.top) / NUMBER_NOTES;
	CurLarg = caRect.right / PatternLength;
	CurLarg	*= ZoomLevelPat;

	TextFont(4);
	TextSize(9);
	
	curSelecTrack = -1;
	SetDText( ClassicDlog, 5, "\pAll Channels");
	curSelecInstru = -1;
	SetDText( ClassicDlog, 6, "\pAll instruments");
	
	theEvent.what = 0;
	DoGrowClassic();
	
	ShowWindow( GetDialogWindow( ClassicDlog));
	
//	AdjustZoomClassic();
//	SetMaxWindow( OsciL + 15, OsciVStart + 15 + OsciNo * (InterText + OsciH), ClassicDlog);
}

void CloseClassicWindow(void)
{
	if (ClassicDlog != NULL)
	{
		DisposeDialog( ClassicDlog);
		SetItemMark( ViewsMenu, mPatternV, noMark);
	}
	ClassicDlog = NULL;
}

void DoKeyPressClassic( short theChar)
{
	if (theChar == getinfo) DialogPatternInfo( PatCopyXIM);
}
