#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPlug.h"
#include "PrivateList.h"
	
	/******** HELP MODULE ********/
	enum
	{
		HPiano	= 1,
		HLeft	= 4,
		HPref	= 5
	};
#define	AHELPSIZE	3

	static	short					AHelp[ AHELPSIZE] = { HPiano, HLeft, HPref};
	
	void DoHelpPiano( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}

	/*****************************/

	enum
	{
		eLargePiano = 0,
		eSmallPiano = 1
	};

	struct	LPoint
	{
			long		start;
			long		end;
	};
	typedef		struct LPoint	LPoint;


	extern	EventRecord				theEvent;
	extern	DialogPtr				SampleDlog[], InstruListDlog, EditorDlog;
	extern	LPoint					SelecRect[ 65];
	extern	RGBColor				theColor;
	extern	Boolean					MIDIHardware, DragManagerUse;
	extern	WindowPtr				oldWindow;
	
			DialogPtr				PianoDlog;
			short					LastCanal;
	
			PixMapHandle			PianoPix;
	
	extern	Boolean					PianoRecording;
			
	static	ControlHandle			PianoCtl, prefBut, leftBut, rightBut;
	static 	char					thePressChar[ NUMBER_NOTES];
	static	RGBColor				MyBlack = { 13107, 13107, 13107};
			short					BlackWhite[ 12] ={	0,1,0,1,0,0,1,0,1,0,1,0};
	static	short					PianoL, PianoH, pianoType;
	static	Rect					PianoRect;
	static	Rect					SPianoRect[ NUMBER_NOTES];
	static	short 					gThumbPrev;
	
#define ToucheLarg		17
#define ToucheHaut		29

#define SmallPianoLarg	449
#define SmallPianoHi	58

void NInitSmallPiano( Rect mainRect, Rect*);
Ptr ConvertPcmd2Text( Pcmd *myPcmd);

Boolean DoRecordPiano();
void NPianoRecordProcess( short, short, short, short);

short NewOffscreenPixMap(PixMapHandle *thePixMap, Rect *picRect);
void DoPlaySamplePart( short Note, short Instru, long start, long end);
void TurnRadio( short	item, DialogPtr	dlog, Boolean alors);
short PicToPix (PicHandle thePic, PixMapHandle *offscreenPixMap);
void DoPlayInstruInt( short	Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end);


Boolean DragCommand( RgnHandle myRgn, Cmd myCmd, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	short				mouseDownModifiers, mouseUpModifiers;
	Pcmd				*myPcmd;
	Ptr					myText;
	Rect				dragRegionRect;
	
	if( !DragManagerUse) return false;

	CopyRgn( myRgn, dragRegion = NewRgn());
	SetPt( &theLoc, 0, 0);
	LocalToGlobal( &theLoc);
	OffsetRgn( dragRegion, theLoc.h, theLoc.v);
	
	NewDrag( &theDrag);
	
	myPcmd = (Pcmd*) NewPtrClear( sizeof( Pcmd) + 1 * sizeof( Cmd));
	if( myPcmd == NULL) PPDebugStr( __LINE__, __FILE__, "Memory WARNING");
	
	myPcmd->structSize = sizeof( Pcmd) + 1 * sizeof( Cmd);
	myPcmd->tracks = 1;
	myPcmd->length = 1;
	myPcmd->trackStart = -1;
	myPcmd->posStart = -1;
	myPcmd->myCmd[ 0] = myCmd;
	AddDragItemFlavor( theDrag, 1, 'Pcmd', myPcmd, myPcmd->structSize, 0);
	
	myText = ConvertPcmd2Text( myPcmd);
	AddDragItemFlavor( theDrag, 1, 'TEXT', myText, GetPtrSize( myText), 0);
	MyDisposePtr( & myText);
	
	SetDragItemBounds( theDrag, 1, GetRegionBounds( dragRegion, &dragRegionRect));

	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);

	result = TrackDrag(theDrag, theEvent, dragRegion);
	if (result != noErr && result != userCanceledErr) return(true);

	DisposeDrag(theDrag);
	DisposeRgn(dragRegion);
	MyDisposePtr( (Ptr*) &myPcmd);
	
	return(true);
}

void CreatePianoPixMap(void)
{
	GDHandle			oldGDeviceH;
	OSErr				errCode;
	CGrafPtr			oldPort;
	GWorldPtr			theGWorld = NULL;
	Rect				aRect, nRect, tRect;
	short				i;
	Str32				str;
	
	if( PianoPix != NULL) ZapPixMap( &PianoPix);
	
	GetGWorld( &oldPort, &oldGDeviceH);

	switch( pianoType)
	{
		case eLargePiano:
			 SetRect( &aRect, 0, 0, ToucheLarg * NUMBER_NOTES, ToucheHaut);
		break;
		
		case eSmallPiano:
			 SetRect( &aRect, 0, 0, SmallPianoLarg, SmallPianoHi);
		break;
	}

	NewGWorld(	&theGWorld, 8, &aRect, nil, nil, (GWorldFlags) 0);
	
	if( NewOffscreenPixMap( &PianoPix, &aRect) != noErr) PPDebugStr( __LINE__, __FILE__, "Memory Error !!!");

	LockPixels( GetPortPixMap( theGWorld));
	SetGWorld( theGWorld, NULL);

	switch( pianoType)
	{
		case eLargePiano:
			ForeColor( blackColor);	BackColor( whiteColor);
		
			TextFont( 4);	TextSize( 9);
		
			SetRect( &nRect, 0, 0, ToucheLarg, ToucheHaut);
			for( i = 0; i < NUMBER_NOTES; i++)
			{
				/** Touche **/
				
				if( BlackWhite[ i % 12])	RGBForeColor( &MyBlack);
				else
				{
					if( thePrefs.PianoOctaveMarkers) SwitchColor( i/12);
					else RGBForeColor( &theColor);
				}
				
				PaintRect( &nRect);
				
				ForeColor( blackColor);
				
				MoveTo( nRect.left, 0);
				LineTo( nRect.left, ToucheHaut);
				
				nRect.left += ToucheLarg;
				nRect.right += ToucheLarg;
				
				/** Caractre **/
				
				MoveTo( 7 + i * 17, 19);
				
				if( !BlackWhite[ i % 12]) ForeColor( blackColor);
				else ForeColor( whiteColor);
				
				DrawChar( thePressChar[ i]);
			}
			
			TextFont( kFontIDCourier);
			
			for( i = 0; i < NUMBER_NOTES; i++)
			{
				GetNoteString( i, str);
			
				if( !BlackWhite[ i % 12]) ForeColor( blackColor);
				else ForeColor( whiteColor);
		
				MoveTo( 2 + i * 17, 7);
				DrawString( str);
			}
			ForeColor( blackColor);	BackColor (whiteColor);
		break;
		
		case eSmallPiano:
			ForeColor( blackColor);	BackColor( whiteColor);
			{
				PicHandle	aPict;
				
				aPict = GetPicture( 225);
				DrawPicture( aPict, &aRect);		//(*aPict)->picFrame
				ReleaseResource( (Handle) aPict);
				
				TextFont( 4);	TextSize( 9);
				
				for( i = 0; i < NUMBER_NOTES; i++)
				{
					/** Caractre **/
					
					GetToucheRect( &aRect, i);
					
					MoveTo( aRect.left + 1 - PianoRect.left, aRect.bottom -3);
					
					if( !BlackWhite[ i % 12])
					{
						ForeColor( blackColor);
						DrawChar( thePressChar[ i]);
					}
				}
				
				if( thePrefs.PianoOctaveMarkers)
				{
#define OCLARG 56
				
					SetRect( &aRect, 0, PianoRect.bottom - 10, OCLARG, PianoRect.bottom);
				
					for( i = 0 ; i < 8; i++)
					{
						SwitchColorBack( i);
						
						NumToString( i, str);
						TETextBox( str+1, str[ 0], &aRect, teCenter);
						
						aRect.left += OCLARG;
						aRect.right += OCLARG;
					}
					BackColor (whiteColor);
					
					SetRect( &aRect, 0, PianoRect.bottom - 10, OCLARG, PianoRect.bottom);
					for( i = 0 ; i < 8; i++)
					{
						MoveTo( aRect.right, aRect.top);
						LineTo( aRect.right, aRect.bottom);
						aRect.left += OCLARG;
						aRect.right += OCLARG;
					}
				}
			}
			ForeColor( blackColor);	BackColor (whiteColor);
		break;
	}
	
	CopyBits(	(BitMap*) *GetPortPixMap( theGWorld),
 				(BitMap*) *PianoPix,
 				&(*PianoPix)->bounds,
 				&(*PianoPix)->bounds,
 				srcCopy,
 				NULL);

	SetGWorld( oldPort, oldGDeviceH);

 	UnlockPixels( GetPortPixMap( theGWorld));
	DisposeGWorld( theGWorld);
}

void GetToucheRect( Rect *theRect, short Id)
{
short 	itemType;
Handle 	itemHandle;
Rect 	itemRect;

	switch( pianoType)
	{
		case eSmallPiano:
			*theRect = SPianoRect[ Id];
			theRect->right -= GetControlValue( PianoCtl);
			theRect->left -= GetControlValue( PianoCtl);
			
			theRect->top++;
			theRect->bottom--;
			
			theRect->right--;
			theRect->left++;
			
			if( BlackWhite[ Id % 12]) theRect->bottom--;
		break;
		
		case eLargePiano:
			theRect->left = 1 + (ToucheLarg*Id) - GetControlValue( PianoCtl) + PianoRect.left;
			theRect->right = theRect->left + ToucheLarg - 1;
			theRect->top = 10;
			theRect->bottom = ToucheHaut;
		break;
	}
}

void SetControlPiano(void)
{
long	maxValue, beforemax;
long	beforeValue, aftervalue;
short	itemType;
Handle	itemHandle;
Rect	caRect, itemRect;
	
	// X Scroll
	
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);
	
	MyMoveControl( PianoCtl, PianoRect.left, caRect.bottom-15);
	MySizeControl( PianoCtl, caRect.right -14 - PianoRect.left, 16);
	
	if( caRect.right - PianoRect.left < PianoL) SetControlMaximum( PianoCtl, PianoL - (caRect.right - PianoRect.left));
	else SetControlMaximum( PianoCtl, 0);
	
	SetControlMinimum( PianoCtl, 0);
	
	if( gUseControlSize) SetControlViewSize( PianoCtl, (caRect.right - PianoRect.left));
}

void DoGrowPiano(void)
{
long		lSizeVH;
GrafPtr		SavePort;
Handle		itemHandle;
Rect		caRect, itemRect, cellRect, temp;
Point		theCell = { 0, 0}, aPt = { 0, 0};
short		tempA, tempB, itemType;

	GetPort( &SavePort);
 	SetPortDialogPort( PianoDlog);
	
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);
	
	temp.left = 100;
	temp.right = PianoL + PianoRect.left + 1;
	temp.top = caRect.bottom;
	temp.bottom = caRect.bottom;
	
	LocalToGlobal( &aPt);
	
#if MACOS9VERSION
	temp.top++;
	temp.bottom++;
#endif
	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( PianoDlog), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( PianoDlog), &caRect);
		
		tempA = caRect.right;
		if( tempA > temp.right) tempA = temp.right-1;
		
		tempB = caRect.bottom;
	}
	
	MySizeWindow( PianoDlog, tempA, PianoH + 15 , true);
	
	SetControlPiano();
	
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);
	
	InvalWindowRect( GetDialogWindow( PianoDlog), &caRect);

	SetPort( SavePort);
}

void UpdatePianoInfo(void)
{
	GrafPtr		SavePort;
	long		val;
	Rect		caRect;
	
	if( PianoDlog == NULL) return;

 	GetPort( &SavePort);
 	SetPortDialogPort( PianoDlog);
	
	val = GetControlValue( PianoCtl);
	
	if( thePrefs.SmallPiano) pianoType = eSmallPiano;
	else pianoType = eLargePiano;
	
	switch( pianoType)
	{
		case eLargePiano:
			PianoL = ToucheLarg * NUMBER_NOTES;
			PianoH = ToucheHaut;
			PianoRect.left = 26;	PianoRect.right = PianoRect.left + PianoL;
			PianoRect.top = 0;		PianoRect.bottom = PianoH;
		break;
		
		case eSmallPiano:
			PianoL = SmallPianoLarg;
			PianoH = SmallPianoHi;
			PianoRect.left = 26;	PianoRect.right = PianoRect.left + PianoL;
			PianoRect.top = 0;		PianoRect.bottom = PianoH;
			
			NInitSmallPiano( PianoRect, SPianoRect);
		break;
	}
	
	SetControlValue( PianoCtl, 0);
	
	theEvent.what = 0;
	DoGrowPiano();
	
//	AdjustZoomPiano();
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);

	SetMaxWindow( PianoRect.right, caRect.bottom, PianoDlog);
	
	UpdatePressChar();
	CreatePianoPixMap();
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);
	InvalWindowRect( GetDialogWindow( PianoDlog), &caRect);
	
	SetControlValue( PianoCtl, val);
	
	SetPort( SavePort);
}

static 	short		OldTouches[ MAXTRACK];
static	short		OldPartitionReader;

extern	RGBColor	theColor;
extern	short		theDepth;

void MyTETextBox2( Str255 aStr, Rect *Zone)
{
	PaintRect( Zone);
	
	ForeColor( blackColor);
	
	MoveTo( Zone->left + ((Zone->right - Zone->left) - StringWidth( aStr))/2, Zone->bottom-3);

	DrawString( aStr);
}

void AfficheTouche( short myTT, Rect *Zone, short Instru, short note)
{
Str32		aStr;
RgnHandle	saveClip;
Handle		itemHandle;
Rect 		itemRect, rect2;
short 		itemType;

	Instru++;
	NTStr( 2, Instru, (Ptr) aStr+1);
	aStr[ 0 ] = 2;

	saveClip = NewRgn();
 	GetClip( saveClip);
	PianoRect.left++;
	ClipRect( &PianoRect);
	PianoRect.left--;
	
	switch( pianoType)
	{
		case eLargePiano:
			SwitchColor( myTT);
			MyTETextBox2( aStr, Zone);
			ForeColor( blackColor);
		break;
		
		case eSmallPiano:
			SwitchColor( myTT);
			BackColor( whiteColor);
			
			rect2 = *Zone;
			if( note > 0)
			{
				if( BlackWhite[ (note-1) % 12]) rect2.left += 2;
			}
			if( note < 96)
			{
				if( BlackWhite[ (note+1) % 12]) rect2.right -= 2;
			}
			
			PaintRect( &rect2);
			
			rect2 = *Zone;
			rect2.top = 27;
			PaintRect( &rect2);
			
			ForeColor( blackColor);
		break;
	}
	
	RGBBackColor( &theColor);
	
	SetClip( saveClip);
	DisposeRgn( saveClip);
}

void EffaceTouche( short Type, Rect *Zone)
{
short		i;
RgnHandle	saveClip;
Handle		itemHandle;
Rect 		itemRect, tempRect;
short 		itemType;

	if( Type < 0) return;

	if( Zone->left <= PianoRect.left)
	{
		Zone->left = PianoRect.left+1;
		if( Zone->right <= PianoRect.left) return;
	}

	tempRect.left = GetControlValue( PianoCtl) + Zone->left - PianoRect.left;
	tempRect.right = GetControlValue( PianoCtl) + Zone->right - PianoRect.left;
	tempRect.top = Zone->top;
	tempRect.bottom = Zone->bottom;
	
	CopyBits(	(BitMap*) *PianoPix,
 				(BitMap*) *GetPortPixMap(GetDialogPort( PianoDlog)),
 				&tempRect,
 				Zone,
 				srcCopy,
 				NULL);
}

void SelectTouche( short	i, short instru)
{
Rect		cellRect, itemRect;
GrafPtr		SavePort;
Point 		theCell;
short		itemType;
Handle		itemHandle;
RgnHandle	saveClip;

if( i < 0) return;
if( PianoDlog == NULL) return;

GetPort( &SavePort);
SetPortDialogPort( PianoDlog);
TextFont( 4);	TextSize( 9);

saveClip = NewRgn();
GetClip( saveClip);
PianoRect.left++;
ClipRect( &PianoRect);
PianoRect.left--;

GetToucheRect( &cellRect, i);

if( instru == -1)
{
	instru = 0;
	if( !GetIns( &instru, NULL)) goto Away;
}

AfficheTouche( LastCanal, &cellRect, instru, i);

Away:

SetClip( saveClip);
DisposeRgn( saveClip);
	
	SetPort( SavePort);
}

void ResetPiano(void)
{
short			i;
Rect		tempRect;
GrafPtr		SavePort;

if( PianoDlog == NULL) return;


GetPort( &SavePort);
SetPortDialogPort( PianoDlog);

for(i=0; i<MAXTRACK;i++)
{
	if( OldTouches[i] != -1)
	{
		GetToucheRect( &tempRect, OldTouches[i]);
		EffaceTouche( OldTouches[i], &tempRect);
	}
}
SetPort( SavePort);
}

void DoNullPiano(void)
{
	Str255				String;
	GrafPtr				SavePort;
	short				temp, val,i, tag, InstruNo;
 	Rect				tempRect;
 	Cmd					*theCommand;
 	Boolean				memeTouche;
 	
 	if( PianoDlog == NULL) return;
 	if( OldPartitionReader == MADDriver->PartitionReader) return;
 	OldPartitionReader = MADDriver->PartitionReader;
 	
 	GetPort( &SavePort);
 	SetPortDialogPort( PianoDlog);

	TextFont( 4);
	TextSize( 9);

	if( MusicPlayActive)
	{
		for(i=0; i<curMusic->header->numChn; i++)
		{
			theCommand = GetMADCommand( MADDriver->PartitionReader, i, curMusic->partition[ MADDriver->Pat ]);
			val = theCommand->note;
			
			if( val != 0xFF)
			{
				// EFFACE LES VIEILLES TOUCHES
				
				if( OldTouches[i] != -1)
				{
					GetToucheRect( &tempRect, OldTouches[i]);
					EffaceTouche( OldTouches[i], &tempRect);
				}
			
				// NOUVELLES TOUCHES
				
				OldTouches[ i] = val;
				GetToucheRect( &tempRect, OldTouches[ i]);
				AfficheTouche( i, &tempRect, MADDriver->chan[i].ins, OldTouches[ i]);
			}
		}
	}
	TextFont( 0);
	TextSize( 0);
	
	SetPort( SavePort);
}

void  UpdatePianoWindow( DialogPtr  GetSelection)  	/* Pointer to this dialog */
{ 
		Rect   		caRect, tempRect, itemRect, srcRect;
 		GrafPtr		SavePort;
 		Point		theCell;
 		Ptr			theStr;
 		Str255		tempStr;
 		short		i,itemType;
 		Handle		itemHandle;
 		PicHandle	aPict;
 		RgnHandle	saveClip;
 		RgnHandle	visibleRegion;
 		
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( PianoDlog);

		BeginUpdate( GetDialogWindow( PianoDlog));

		GetDialogItem ( GetSelection, 1, &itemType, &itemHandle, &itemRect);
		
		GetPortBounds( GetDialogPort( PianoDlog), &caRect);
		
		itemRect.bottom = PianoRect.top + (*PianoPix)->bounds.bottom;
		itemRect.right = caRect.right;

		tempRect.left = GetControlValue( PianoCtl);
		tempRect.right = GetControlValue( PianoCtl) + itemRect.right - itemRect.left;
		tempRect.top = 0;
		tempRect.bottom = (*PianoPix)->bounds.bottom;
		
		CopyBits(	(BitMap*) *PianoPix,
 					(BitMap*) *GetPortPixMap(GetDialogPort( GetSelection)),
 					&tempRect,
 					&itemRect,
 					srcCopy,
 					NULL);
		
		GetPortBounds( GetDialogPort( PianoDlog), &caRect);
		
		MoveTo( itemRect.left, 0);
		LineTo( itemRect.left, caRect.bottom);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( PianoDlog), visibleRegion);
		
		UpdateDialog( PianoDlog, visibleRegion);
		
		DisposeRgn( visibleRegion);
		
		DrawGrowIconP( PianoDlog);
		
		EndUpdate( GetDialogWindow( PianoDlog));

		SetPort( SavePort);
}

/*void TrackRect( Rect	*itemRect)
{
InvertRect( itemRect);

while( Button())
{
	DoGlobalNull();
	WaitNextEvent( everyEvent, &theEvent, 1, NULL);
}
InvertRect( itemRect);
}*/

extern	DialogPtr	AdapDlog;

pascal void actionPiano(  ControlHandle	theControl,  short			ctlPart)
{
long			lRefCon;
Handle			itemHandle;
short			itemType, maxValue, minValue, curVal, copyval;
RgnHandle		aRgn;
Rect			caRect, itemRect;

if( ctlPart <= 0) return;

maxValue = GetControlMaximum( theControl);
minValue = GetControlMinimum( theControl);
copyval = curVal = GetControlValue( theControl);

GetPortBounds( GetDialogPort( PianoDlog), &caRect);

	switch( ctlPart)
	{
		case kControlUpButtonPart:
			curVal -= ToucheLarg;
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlDownButtonPart:
			curVal += ToucheLarg;
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlPageUpPart:
			curVal -= caRect.right;
			
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			curVal += caRect.right;
			
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlIndicatorPart:
			copyval = gThumbPrev;
			gThumbPrev = curVal;
		break;
	}
		
	if( copyval != curVal)
	{
		RgnHandle	aRgn;
	
		SetControlValue( theControl, curVal);
		
		itemRect = PianoRect;
		itemRect.left++;
	//	itemRect.bottom--;
		itemRect.right = caRect.right;
		
		aRgn = NewRgn();
		
		ScrollRect( &itemRect, copyval - curVal, 0, aRgn);
		
		InvalWindowRgn( GetDialogWindow( PianoDlog), aRgn);
		UpdatePianoWindow( PianoDlog);
		DisposeRgn( aRgn);
	}
}

void DoItemPressPiano( short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
		Cell				theCell;
		long				tempLong;
 		short				temp, ctlPart, oldtemp, itemType, bogus, i;
 		Rect				caRect, cellRect, tempRect, aRect;
 		GrafPtr				SavePort;
 		ControlHandle		theControl;
 		Handle				itemHandle;
		RgnHandle			myRgn;
 		Str255				sTemp;
 		Point				myPt;
		Cmd					myCmd;
 		ControlActionUPP	MyControlUPP;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( PianoDlog);
 		
		if (theEvent.what == mouseDown) /* See if a mouse click */
		{
			myPt = theEvent.where;
			GlobalToLocal(&myPt);
			
			GetDialogItem( whichDialog, 1, &itemType, &itemHandle, &tempRect);
			if( PtInRect( myPt, &tempRect))
			{
				theControl = NULL;
				if( TestControl(  PianoCtl, myPt)) theControl = PianoCtl;
				
				GetPortBounds( GetDialogPort( whichDialog), &caRect);
				
				if( theControl)
				{
					MyControlUPP = NewControlActionUPP( actionPiano);
					gThumbPrev = GetControlValue( theControl);
					TrackControl(theControl, myPt, MyControlUPP);
					DisposeControlActionUPP( MyControlUPP);
				}
				
				if( myPt.v > 1 && myPt.v < PianoH && myPt.h > PianoRect.left)
				{
					if( !GetIns( &bogus, NULL)) Erreur( 13, 0);
					else
					{
						Point tPt;
					
						oldtemp = -1;
						SetRect( &cellRect,0,0,0,0);
					
						do
						{
							WaitNextEvent( everyEvent, &theEvent, 1, NULL);
							DoGlobalNull();
							DoNullInstrument();
							GetMouse( &myPt);
							
							switch( pianoType)
							{
								case eLargePiano:
									temp = (myPt.h + GetControlValue( PianoCtl) - PianoRect.left) / ToucheLarg;
									
								break;
								
								case eSmallPiano:
									temp = -1;
									tPt = myPt;
									tPt.h += GetControlValue( PianoCtl);
									
									for( i = 0; i < NUMBER_NOTES; i++)
									{
										if( PtInRect( tPt, &SPianoRect[ i]))
										{
											temp = i;
											if( SPianoRect[ i].right - SPianoRect[ i].left == 5) break;
										}
									}
								break;
							}
							
							if( temp >= 0 && temp < NUMBER_NOTES)
							{
								if( oldtemp != temp)
								{
									GetToucheRect( &tempRect,  oldtemp);
									EffaceTouche( oldtemp, &tempRect);
									
									switch( thePrefs.KeyUpMode)
									{
										case eStop:
											MADDriver->chan[ LastCanal].loopBeg = 0;
											MADDriver->chan[ LastCanal].loopSize = 0;
											MADDriver->chan[ LastCanal].maxPtr = MADDriver->chan[ LastCanal].curPtr;
										break;
										
										case eNoteOFF:
											MADDriver->chan[ LastCanal].KeyOn = false;
										break;
									}
								}
								
								GetPortBounds( GetDialogPort( PianoDlog), &caRect);
								
								if( myPt.h < PianoRect.left)
								{
									if( GetControlValue( PianoCtl) - ToucheLarg >= 0)
									{
										SetControlValue( PianoCtl, GetControlValue( PianoCtl) - ToucheLarg);
										InvalWindowRect( GetDialogWindow( PianoDlog), &PianoRect);
										UpdatePianoWindow( PianoDlog);
										
										myPt.h += ToucheLarg;
										
										oldtemp = -1;
									}
									else if( GetControlValue( PianoCtl) - 1 >= 0)
									{
										SetControlValue( PianoCtl, GetControlMinimum( PianoCtl));
										InvalWindowRect( GetDialogWindow( PianoDlog), &PianoRect);
										UpdatePianoWindow( PianoDlog);
										
										myPt.h += ToucheLarg;
										
										oldtemp = -1;
									}
								}
								else if( myPt.h > caRect.right)
								{
									if( GetControlValue( PianoCtl) + ToucheLarg + caRect.right - PianoRect.left <= PianoL)
									{
										SetControlValue( PianoCtl, GetControlValue( PianoCtl) + ToucheLarg);
										InvalWindowRect( GetDialogWindow( PianoDlog), &PianoRect);
										UpdatePianoWindow( PianoDlog);
										
										myPt.h -= ToucheLarg;
										
										oldtemp = -1;
									}
									else if( GetControlValue( PianoCtl) + 1 + caRect.right - PianoRect.left <= PianoL)
									{
										SetControlValue( PianoCtl, GetControlMaximum( PianoCtl));
										InvalWindowRect( GetDialogWindow( PianoDlog), &PianoRect);
										UpdatePianoWindow( PianoDlog);
										
										myPt.h -= ToucheLarg;
										
										oldtemp = -1;
									}
								}
								
								if( oldtemp != temp)
								{
									oldtemp = temp;
									
									SelectTouche( temp, -1);
									SelectToucheMozart( temp + 1, 0);
									
									if( GetIns( &bogus, NULL))
									{
										short	track = GetWhichTrackPlay();
										short	eff = 0, arg = 0, volCmd = 0xFF;
										
										if( PianoRecording) NPianoRecordProcess( temp, -1, 0xFF, track);
										else if( oldWindow == GetDialogWindow( EditorDlog)) DigitalEditorProcess( temp  + 2, &eff, &arg, &volCmd);
										
										
										if(	SampleDlog[ bogus] != NULL &&
											SelecRect[ bogus].end != SelecRect[ bogus].start)
										{
											DoPlayInstruInt( temp, bogus, eff, arg, volCmd, &MADDriver->chan[ track], SelecRect[ bogus].start, SelecRect[ bogus].end);
										}
										else
										{
											DoPlayInstruInt( temp , bogus, eff, arg, volCmd, &MADDriver->chan[ track], 0, 0);
										}
										
										if( pianoType == eSmallPiano)
										{
											Rect	 itemRect;
										
											GetDialogItem( whichDialog, 6, &itemType, &itemHandle, &itemRect);
											TextFace( bold);
											GetNoteString( oldtemp, sTemp);
											TETextBox( sTemp+1, sTemp[ 0], &itemRect, teCenter);
											TextFace( 0);
										}
									}
								}
								
								if( myPt.v < 0 && DragManagerUse == true)
								{
									myRgn = NewRgn();
									
									GetToucheRect( &aRect,  temp);
									aRect.top = 0;
									
									RectRgn( myRgn, &aRect);
									
									theEvent.where.v = aRect.top + (aRect.bottom - aRect.top)/2;
									theEvent.where.h = aRect.left + (aRect.right - aRect.left)/2;
									LocalToGlobal( &theEvent.where);
									
									if( GetIns( &bogus, NULL))
									{
										myCmd.ins = bogus + 1;
										myCmd.note = temp;
										myCmd.cmd = 0;
										myCmd.arg = 0;
										myCmd.vol = 0xFF;
									}
									
									DragCommand( myRgn, myCmd, &theEvent);
									DisposeRgn( myRgn);
								}
							}
						}
						while( Button());
						
						GetToucheRect( &tempRect,  oldtemp);
						EffaceTouche( oldtemp, &tempRect);
						SelectToucheMozart( -1, 0);
						
						if( pianoType == eSmallPiano)
						{
							Rect	 itemRect;
						
							GetDialogItem( whichDialog, 6, &itemType, &itemHandle, &itemRect);
							EraseRect( &itemRect);
						}
						
						switch( thePrefs.KeyUpMode)
						{
							case eStop:
								MADDriver->chan[ LastCanal].loopBeg = 0;
								MADDriver->chan[ LastCanal].loopSize = 0;
								MADDriver->chan[ LastCanal].maxPtr = MADDriver->chan[ LastCanal].curPtr;
							break;
							
							case eNoteOFF:
								MADDriver->chan[ LastCanal].KeyOn = false;
							break;
						}
						
						if( MADDriver->DriverSettings.driverMode == MIDISoundDriver)
						{
							if( MADDriver->NoteOld[ LastCanal] != -1)
							{
								NoteOff( MADDriver->InstuNoOld[ LastCanal], MADDriver->NoteOld[ LastCanal], MADDriver->VelocityOld[ LastCanal], MADDriver);
								MADDriver->NoteOld[ LastCanal] = -1;
							}
						}
					}
				}
			}   		
		
		switch( whichItem)
		{
			case 5:
				if( GetControlHilite( prefBut) == 0 && MyTrackControl( prefBut, theEvent.where, NULL))
				{
#include "Help.h"
			
					ShowPrefs( EDITOR);
				}
			break;
			
			case 7:	// Right
				if( GetControlHilite( rightBut) == 0 && MyTrackControl( rightBut, theEvent.where, NULL))
				{
					if( thePrefs.pianoOffset < 7)
					{
						thePrefs.pianoOffset++;
						UpdatePianoInfo();
					}
				}
			break;
			
			case 4:	// Left
				if( GetControlHilite( leftBut) == 0 && MyTrackControl( leftBut, theEvent.where, NULL))
				{
					if( thePrefs.pianoOffset > -7)
					{
						thePrefs.pianoOffset--;
						UpdatePianoInfo();
					}
				}
			break;
	
		SetPort( SavePort);
		}
		}
}

void UpdatePressChar(void)
{
short	i, x;
Str255	str, bstr;

	for( x = 0; x < NUMBER_NOTES; x++) thePressChar[ x] = 0;
	
	for( x = 0; x < NUMBER_NOTES; x++)
	{
		for( i = 0; i < 300; i++)
		{
			if( thePrefs.PianoKey[ (short) i] == x)
			{
				if( x + thePrefs.pianoOffset*12 >= 0 && x + thePrefs.pianoOffset*12 < NUMBER_NOTES)
				{
					thePressChar[ x + thePrefs.pianoOffset*12] = i;
				}
			}
		}
	}
	
	if( PianoDlog == NULL) return;
	
	pStrcpy( str, "\pPiano ");
	
	if( thePrefs.pianoOffset >= 0) pStrcat( str, "\p+");
	
	NumToString( thePrefs.pianoOffset, bstr);
	pStrcat( str, bstr);
	
	pStrcat( str, "\p Octave(s)");
	
	SetWTitle( GetDialogWindow( PianoDlog), str);
}

void AdjustZoomPiano2( Rect *vRect)
{
//	WindowPeek	wPeek;
//	WStateData	*wspd;
	short		tempA, tempB;
	Rect		stdRect, caRect;

//	wPeek = ( WindowPeek) PianoDlog;
	
//	wspd = (WStateData*) *(wPeek->dataHandle);

	GetWindowStandardState( GetDialogWindow( PianoDlog), &stdRect);
	
	GetPortBounds( GetDialogPort( PianoDlog), &caRect);
	
	stdRect.bottom = vRect->bottom;
	stdRect.top = stdRect.bottom - caRect.bottom;
	
	SetWindowStandardState( GetDialogWindow( PianoDlog), &stdRect);
}

void CreatePianoWindow(void)
{
	Rect		caRect, itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	
	if( PianoDlog != NULL)
	{
		SetWindEtat( GetDialogWindow(PianoDlog));
		return;
	}
	
	SetItemMark( ViewsMenu, mPiano, checkMark);
	
	if( thePrefs.SmallPiano) pianoType = eSmallPiano;
	else pianoType = eLargePiano;
	
	switch( pianoType)
	{
		case eLargePiano:
			PianoL = ToucheLarg * NUMBER_NOTES;
			PianoH = ToucheHaut;
			PianoRect.left = 26;	PianoRect.right = PianoRect.left + PianoL;
			PianoRect.top = 0;		PianoRect.bottom = PianoH;
		break;
		
		case eSmallPiano:
			PianoL = SmallPianoLarg;
			PianoH = SmallPianoHi;
			PianoRect.left = 26;	PianoRect.right = PianoRect.left + PianoL;
			PianoRect.top = 0;		PianoRect.bottom = PianoH;
			
			NInitSmallPiano( PianoRect, SPianoRect);
		break;
	}
	
	PianoDlog = GetNewDialog( 145, NULL, GetDialogWindow( ToolsDlog));
	SetWindEtat( GetDialogWindow(PianoDlog));
	
//	GetDialogItem ( PianoDlog, 2, &itemType, (Handle*) &PianoCtl, &itemRect);

	SetRect( &itemRect, 0, 0, 30, 16);
	PianoCtl = NewControl(			GetDialogWindow( PianoDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									xScrollNum);
	SetControlPiano();
	SetControlValue( PianoCtl, thePrefs.PianoPos);
	
//	AdjustZoomPiano();

	GetPortBounds( GetDialogPort( PianoDlog), &caRect);

	SetMaxWindow( PianoRect.right, caRect.bottom, PianoDlog);
	
	ShowWindow( GetDialogWindow( PianoDlog));
	SetPortDialogPort( PianoDlog);
	SelectWindow2( GetDialogWindow( PianoDlog));
	
	for(i=0; i<MAXTRACK; i++)
	{
		OldTouches[i] = -1;
	}
	
	UpdatePressChar();
	CreatePianoPixMap();
	
	GetDialogItem( PianoDlog , 5, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	prefBut = NewControl( 	GetDialogWindow( PianoDlog),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							174,
							kControlBevelButtonNormalBevelProc,
							0);
								
	GetDialogItem( PianoDlog , 4, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	leftBut = NewControl( 	GetDialogWindow( PianoDlog),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							207,
							kControlBevelButtonNormalBevelProc,
							0);
								
	GetDialogItem( PianoDlog , 7, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	rightBut = NewControl( 	GetDialogWindow( PianoDlog),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							208,
							kControlBevelButtonNormalBevelProc,
							0);

//	PianoRecording = false;
}

void ClosePiano(void)
{
	thePrefs.PianoPos = GetControlValue( PianoCtl);
	
	if( PianoDlog != NULL) DisposeDialog( PianoDlog);
	PianoDlog = NULL;
//	PianoRecording = false;
	
	SetItemMark( ViewsMenu, mPiano, noMark);
}

extern MenuHandle				TrackView;

static short lastReaderTrack[ MAXTRACK];

void InitRecording( void)
{
	short	i;
	
	for( i = 0; i < 32; i++)
	{
		lastReaderTrack[ i] = -1;
	}
}

void NPianoRecordProcess( short i, short curSelecInstru, short volume, short track)
{
	long				mresult;
	short				itemHit, itemType, xx, MyRea, tSelec, curPat;
	Handle				itemHandle;
	Str255				theStr, str1;
	Ptr					tempPtr;
	Rect				itemRect;
	Point				myPt;
	GrafPtr				myPort;
	Cmd					*theCommand;

//	if( PianoDlog == NULL) return;

	if( curSelecInstru == -1)
	{
		if( !GetIns( &curSelecInstru, NULL)) return;
		if( i < 0) curSelecInstru = -1;
	}
	
	/**/
	
	if( curSelecInstru == 0xFF) curSelecInstru = -1;
	
	if( MusicPlayActive) MyRea = MADDriver->PartitionReader-1;
	else MyRea = MADDriver->PartitionReader;
	
	if( MyRea < 0) MyRea = 0;
	if( MyRea >= curMusic->partition[ MADDriver->Pat]->header.size) MyRea = curMusic->partition[ MADDriver->Pat]->header.size - 1;
	
	/**/
	
	curPat = MADDriver->Pat;
	if( lastReaderTrack[ track] == MyRea)
	{
		MyRea = MADGetNextReader( curMusic, MADDriver, MyRea, &curPat);
	}
	lastReaderTrack[ track] = MyRea;
	
	/**/
	
	MADDriver->TrackLineReading[ track] = false;
	
	theCommand = GetMADCommand( MyRea, track, curMusic->partition[ curPat]);
	
	theCommand->ins = curSelecInstru + 1;		theCommand->note = i;
	
	if( thePrefs.FinePositioning && MusicPlayActive && theCommand->note != 0xFE)					// EXTRA SMALL POSITION
	{
		if( MADDriver->smallcounter != 0)
		{
			theCommand->cmd = 0x0E;					theCommand->arg = 0xD0 + MADDriver->smallcounter;
		}
		else
		{
			theCommand->cmd = 0;					theCommand->arg = 0;
		}
	}
	else
	{
		theCommand->cmd = 0;						theCommand->arg = 0;
	}
	
	theCommand->vol = volume;
	
	UPDATE_Note( MyRea, track);
	
	if( !MusicPlayActive)
	{
		MADDriver->PartitionReader++;
		if( MADDriver->PartitionReader == curMusic->partition[ MADDriver->Pat]->header.size)
		{
			MADDriver->PartitionReader = 0;
			MADDriver->PL++;
			if( MADDriver->PL >= (curMusic->header)->numPointers) MADDriver->PL--;
			MADDriver->Pat = (curMusic->header)->oPointers[ MADDriver->PL];
		}
	}
}
