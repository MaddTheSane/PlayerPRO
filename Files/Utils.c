/****************************************************************************/
/*									      									*/
/*	Project  : P L A Y E R     P R O    			      					*/
/*	File     : Utils.c					  		   							*/
/*	Function : Utilities								     	     		*/
/*	Authors  : Antoine Rosset					      						*/
/*											      							*/
/*	History  : 4.1991	version 1				      						*/
/*								   	      									*/
/*	(C) 1990, 1991 ROSSET Antoine							      			*/
/*		       All Rights Reserved				      						*/
/*									      									*/
/****************************************************************************/

#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "QDOffscreen.h"
#include "Navigation.h"

void				PrefFilterPiano( DialogPtr theDialog, EventRecord *theEventI, short *itemHit);
void				ZapBitMap();
void				ZapPixMap();
void				LigneGO();
void 				FrameRectRelief( Rect *theRect);
short				NewOffscreenPixMap();
short				NewOffscreenBitMap();
void				pStrcpy();
short				IclToPix (Handle , PixMapHandle *);
void				CopyBitsFast( PixMapHandle, Rect);
void				ScrollFast();
void				DessineRgn(Rect *, Rect *);
void				TolBoxInit(void);
void				ControlSwitch(short	item, DialogPtr	dlog, short	Switch);
void UpdatePlugsAbout( DialogPtr aDialog);
void				FrameButton(DialogPtr theDia, short whichButton, Boolean drawIt);
short				SetScroll(ControlHandle vScroll, TEHandle TEH);
void AddRemoveBookmarksFilter( DialogPtr theDialog, EventRecord *theEventI, short *itemHit);
void	UpdateBookmarks( DialogPtr);
void VSTFilter( DialogPtr theDialog, EventRecord *theEventI, short *itemHit);

extern	short					theDepth;
extern	WindowPtr				oldWindow;
extern	ProcessSerialNumber		playerPROPSN;
extern	DialogPtr				AHelpDlog;
extern	SndListHandle			InSound, OutSound;

#include "Sound.h"
#include "mixedmode.h"

/************************************************************/
/*static	long	oldMyDisposePtr;

pascal void NewMyDisposePtr( & Ptr);
pascal void NewMyDisposePtr( & Ptr myPtr)
{
	long aSize;

	aSize = GetPtrSize( myPtr);
	if( aSize <= 0 || aSize >= 400000) MyDebugStr( __LINE__, __FILE__, "MyDisposePtrError");

//	CallPascal( myPtr, oldMyDisposePtr);
	
	if( MemError() != noErr) MyDebugStr( __LINE__, __FILE__, "MyDisposePtrError");
}


void PatchMyDisposePtr( &)
{
	MyDebugStr( __LINE__, __FILE__, "");

	oldMyDisposePtr = NGetTrapAddress( 0xA01F, 0);
	NSetTrapAddress( (long) NewMyDisposePtr, 0xA01F, 0);
}

void DePatchMyDisposePtr( &)
{
	NSetTrapAddress( oldMyDisposePtr, 0xA01F, 0);
}
*/
/************************************************************/

unsigned char* MyC2PStr( Ptr cStr)
{
	long size = strlen( cStr);
	BlockMoveData( cStr, cStr + 1, strlen( cStr));
	cStr[ 0] = size;

	return (unsigned char*) cStr;
}

void MyP2CStr( unsigned char *cStr)
{
	long size = cStr[ 0];
	BlockMoveData( cStr + 1, cStr, size);
	cStr[ size] = 0;
}


void NNumToString( short no, Str255 aStr)
{
Str255	tStr;

	NumToString( no, tStr);
	
	pStrcpy( aStr, "\p");
	if( no < 100) pStrcat( aStr, "\p0");
	if( no < 10) pStrcat( aStr, "\p0");
	pStrcat( aStr, tStr);

}

/*OSErr InstallVBL (VBLTask *theVBLTask, ProcPtr myVBLProc, Boolean isPersistent)
{
	OSErr	theError;
	THz 	savedZone;
 
	if (isPersistent)
	{
		savedZone = GetZone();
		SetZone( SystemZone());
	}
	theVBLTask->vblAddr = NewVBLProc( myVBLProc);
	theError = MemError();
	if (isPersistent) SetZone(savedZone);
	
	if (theVBLTask->vblAddr != nil)
	{
		theVBLTask->qType = vType;
		theVBLTask->vblCount = 1;
		theVBLTask->vblPhase = 0;
		theError = VInstall((QElemPtr) theVBLTask);
	}
	return (theError);
}

long	gCounter = 0;
 
pascal void MyVBLProc (VBLTask *theVBLTask)
{
	theVBLTask->vblCount = 1;
	gCounter++;
	return;
}

void RemoveVBL (VBLTask *theVBLTask)
{
	THz	savedZone;
 
	VRemove((QElemPtr) theVBLTask);
	if (theVBLTask->vblAddr)
	{
		savedZone = GetZone();

		#if defined(powerc) || defined (__powerc)

		SetZone(PtrZone((Ptr) theVBLTask->vblAddr));
		#endif		

		DisposeRoutineDescriptor(theVBLTask->vblAddr);
		
		SetZone(savedZone);
	}
	return;
}*/

void oldFrameButton( DialogPtr theDlg)
{
	Rect	iRect;
	Handle	iHndl;
	short	iType;

	GetDialogItem (theDlg, 1, &iType, &iHndl, &iRect );
	
	if( iHndl == 0L) MyDebugStr( __LINE__, __FILE__, "oldFrameButton itemHandle = 0L");
	
/*	PenSize( 3,3 );
	InsetRect( &iRect, -4,-4);
	FrameRoundRect( &iRect, 16,16 );
	PenSize( 1,1 );*/
	
	SetDialogDefaultItem( theDlg, 1);
}

void CenterRect( Rect *main, Rect *dst, Rect *result)
{
	Rect		tempRect;
	short			Larg, Haut;
	
	tempRect.left = main->left;		tempRect.right = main->right;
	tempRect.top = main->top;		tempRect.bottom = main->bottom;
	
	Larg = dst->right - dst->left;
	Haut = dst->bottom - dst->top;
	
	result->left = tempRect.left + - Larg/2 + (tempRect.right - tempRect.left)/2;
	result->right = result->left + Larg;
	
	result->top = tempRect.top + - Haut/2 + (tempRect.bottom - tempRect.top)/2;
	result->bottom = result->top + Haut;
}

pascal Boolean mylClickLoop( void)
{
//	DoGlobalNull();
	
	return( true);
}

short 	dialogModifiers;

pascal Boolean MyDlgFilter( DialogPtr theDlg, EventRecord *theEvt, short *itemHit)
{
short			thePart;
WindowPtr		whichWindow;
short			LoopSet;
GrafPtr			savePort;

	dialogModifiers = theEvt->modifiers;
	*itemHit = 0;

	if( theEvt->what == updateEvt)
	{
		if( (WindowPtr) theEvt->message == GetDialogWindow( theDlg))
		{
			switch( GetWRefCon( GetDialogWindow( theDlg)))
			{
				case 0:		oldFrameButton( theDlg);					break;
				case 96:	oldFrameButton( theDlg);					break;
				case 9996:	oldFrameButton( theDlg);					break;
				case 9987:	DrawChooseColorWindow();					break;
				case 9467:	UpdateEditInstruWindow( theDlg);			break;
				case 7311:	UpdateSoundQualityExportSndWindow( theDlg);	break;
				case 966:	UpdateFileInformations( theDlg);			break;
				case 99802:	UpdatePlugsAbout( theDlg);					break;
			}
			
			return false;
		}
		DoUpdateEvent( theEvt);
		
		*itemHit = -updateEvt;
		
		return( true);
	}
	else if(theEvt->what == mouseDown) 
	{
		thePart = FindWindow( theEvt->where, &whichWindow);
		
		if( thePart == inDrag)
		{
			BitMap		screenBits;
			
			if( whichWindow != GetDialogWindow( theDlg)) return( false);
			
			GetQDGlobalsScreenBits( &screenBits);
			
			DragWindow( whichWindow, theEvt->where, &screenBits.bounds);
			return( true);
		}
		else return( false);
	}
	else if( theEvt->what == keyDown)
	{
		switch ( (theEvt->message) & charCodeMask )
		{
		case 0x0d:
		case 0x03:
			*itemHit = 1;
			return( true );
		case 0x1b:
			*itemHit = 2;
			return( true );
		default:
			return( false );
		}
	}
	else if( theEvt->what == nullEvent)
	{
		//ProcessSerialNumber	PSN;

		LoopSet = thePrefs.LoopType;
		thePrefs.LoopType = 4;
		
		if( GetWRefCon( GetDialogWindow( theDlg)) != 9996) DoGlobalNull();
		
		thePrefs.LoopType = LoopSet;

		/*GetCurrentProcess( &PSN);
		if(	PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
			PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN)
		{
			SetFrontProcess( &playerPROPSN);		
		}*/

		*itemHit = -5;
		return( true );
	}
/*	else if( theEvt->what == activateEvt)
	{
		if( AHelpDlog != 0L)
		{
			if( MacIsWindowVisible( GetDialogWindow( AHelpDlog))) ActivateProcedure( true);
		}
	}	*/
	
	return( false);
}

void MyUpdateDialog( DialogPtr theDia)
{
	GrafPtr		savePort;
	
	GetPort( &savePort);
	SetPortDialogPort( theDia);
	
	BeginUpdate( GetDialogWindow( theDia));
	
	DrawDialog( theDia);
	
	oldFrameButton( theDia);
	
	EndUpdate( GetDialogWindow( theDia));
	SetPort( savePort);
}

Boolean MyIntModalDialog( DialogPtr theDlg, short *itemHit, EventRecord *myIntEvent)
{
short			thePart;
WindowPtr		whichWindow;
short			LoopSet;
GrafPtr			savePort;
DialogPtr		whichDialog;

	WaitNextEvent( everyEvent, myIntEvent, 1, 0L);
	
	mainSystemDrag = false;

	dialogModifiers = myIntEvent->modifiers;
	*itemHit = 0;
	
	if( myIntEvent->what == updateEvt)
	{
		if( (WindowPtr) myIntEvent->message == GetDialogWindow( theDlg))
		{
			switch( GetWRefCon( GetDialogWindow( theDlg)))
			{
				case RefPref:	UpdatePrefWindow( theDlg);				break;
				
				default:
				case 0:		MyUpdateDialog( theDlg);					break;
				case 96:	MyUpdateDialog( theDlg);					break;
				case 9996:	MyUpdateDialog( theDlg);					break;
				case 9987:	DrawChooseColorWindow();					break;
				case 9467:	UpdateEditInstruWindow( theDlg);			break;
				case 7311:	UpdateSoundQualityExportSndWindow( theDlg);	break;
				case 966:	UpdateFileInformations( theDlg);			break;
				case 99802:	UpdatePlugsAbout( theDlg);					break;
				case 376:	UpdateWorkingWindow();						break;
				case 6648:	DeviceFilterUpdate();						break;
				case 3885:	UpdateBookmarks( theDlg);	break;
			}
			
			return false;
		}
		
		DoUpdateEvent( myIntEvent);
		
		*itemHit = -updateEvt;
		
		return( true);
	}
	else if(myIntEvent->what == mouseDown) 
	{
		thePart = FindWindow( myIntEvent->where, &whichWindow);
		
		switch( GetWRefCon( GetDialogWindow( theDlg)))
		{
			case 6648:	DeviceFilterMouseDown( myIntEvent);					break;
			case RefPref: PrefFilterPiano( theDlg, myIntEvent, itemHit);	break;
			case 3885:	AddRemoveBookmarksFilter( theDlg, myIntEvent, itemHit);	break;				

		}
		
		if( thePart == inDrag)
		{
			BitMap		screenBits;
			
			if( whichWindow != GetDialogWindow( theDlg)) return( false);
			
			GetQDGlobalsScreenBits( &screenBits);
			
			DragWindow( whichWindow, myIntEvent->where, &screenBits.bounds);
			
			return( true);
		}
		else if( thePart == inContent)
		{
			if( DialogSelect( myIntEvent, &whichDialog, itemHit) == false ) return false;
			else return true;
		}
	}
	else if( myIntEvent->what == keyDown)
	{
		switch( GetWRefCon( GetDialogWindow( theDlg)))
		{
			case RefPref:	PrefFilterPiano( theDlg, myIntEvent, itemHit);	break;
			case 6969: 		VSTFilter( theDlg, myIntEvent, itemHit);	break;			//VST
		}
		
		switch ( (myIntEvent->message) & charCodeMask )
		{
		case 0x03:
			*itemHit = 1;
			return true;
		break;
		
		case 0x0d:
			if( GetWRefCon( GetDialogWindow( theDlg)) != 8775)
			{
				*itemHit = 1;
				return true;
			}
		break;

		case 0x1b:
			*itemHit = 2;
			return true;
		break;

		default:
		{
			DialogSelect( myIntEvent, &whichDialog, itemHit);
			
			return false;
		}
		break;
		}
	}
	else if( myIntEvent->what == nullEvent)
	{
		TEHandle	textEdit;

		///////
		
	/*	textEdit = GetDialogTextEditHandle( theDlg);
		if( textEdit) TEIdle( textEdit);*/

		//if( aDiaPeek->textH != 0L && aDiaPeek->editField != -1) TEIdle( aDiaPeek->textH);
		
		///////
		
		DialogSelect( myIntEvent, &whichDialog, itemHit);
		
		LoopSet = thePrefs.LoopType;
		thePrefs.LoopType = 4;
		
		if( GetWRefCon( GetDialogWindow( theDlg)) != 9996) DoGlobalNull();
		
		thePrefs.LoopType = LoopSet;
		
/*		GetFrontProcess( &PSN);
	if( PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) SetFrontProcess( &playerPROPSN);*/
		
		*itemHit = -5;
		return( true );
	}
	else if( myIntEvent->what == osEvt)
	{
		DoOSEvent( myIntEvent, true);
	}
	
	return( false);
}

Boolean MyModalDialog( DialogPtr theDlg, short *itemHit)
{
	EventRecord		gModalDialogEvent;

	if( MyIntModalDialog( theDlg, itemHit, &gModalDialogEvent) == false)
	{
		/*if( IsDialogEvent( &gModalDialogEvent))
		{
			short 		whichItem;
			DialogPtr	whichDialog;
			
			if( DialogSelect( &gModalDialogEvent, &whichDialog, &whichItem))
			{
				*itemHit = whichItem;
			}
		}*/
	}
	return true;
}

pascal Boolean MyDlgFilterSF( DialogPtr theDlg, EventRecord *theEvt, short *itemHit, void *data)
{
	return( MyDlgFilter( theDlg, theEvt, itemHit));
}

pascal void MyDlgFilterNav(		NavEventCallbackMessage 	callBackSelector, 
								NavCBRecPtr 				callBackParms, 
								NavCallBackUserData 		callBackUD)
{
	short itemHit;
	Rect	caRect;
	
	switch (callBackSelector)
	{
		case kNavCBEvent:
			switch (callBackParms->eventData.eventDataParms.event->what)
			{
				case updateEvt:
					if( (WindowPtr) callBackParms->eventData.eventDataParms.event->message == callBackParms->window)
					{
						
					}
					else
					{
						EventRecord *event = callBackParms->eventData.eventDataParms.event;
						GrafPtr		savedPort;
						
						GetPort( &savedPort);
						SetPortWindowPort( (WindowPtr) event->message);
						GetPortBounds( GetWindowPort( (WindowPtr) event->message), &caRect);
						InvalWindowRect( (WindowPtr) event->message, &caRect);
						SetPort( savedPort);
					}
				break;
			}
			MyDlgFilter( GetDialogFromWindow( callBackParms->window), callBackParms->eventData.eventDataParms.event, &itemHit);
		break;
	}
}

void	ToolBoxInit(void)
{
	OSErr	iErr;
	
	#if MACOS9VERSION

	InitGraf( &qd.thePort);
	InitFonts();
//	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs( 0L);
	
	#endif

	InitCursor();
	
	#if MACOS9VERSION
	MaxApplZone();
	InitFloatingWindows();
	#endif
	

	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
}

void InverseRadio( short	item, DialogPtr	dlog)
{
Handle		itemHandle;
short		itemType;
Rect		itemRect;

GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);

if( itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "InverseRadio itemHandle = 0L");

if( itemType >= 128) itemType -= 128;
if( itemType != 6 && itemType != 5) MyDebugStr( __LINE__, __FILE__, "Error in InverseRadio");

SetCtlValue( (ControlHandle) itemHandle,!GetCtlValue( (ControlHandle) itemHandle));
}


void TurnRadio( short	item, DialogPtr	dlog, Boolean alors)
{
Handle		itemHandle;
short		itemType;
Rect		itemRect;

GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);

if( itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "TurnRadio itemHandle = 0L");

if( itemType >= 128) itemType -= 128;
if( itemType != 6 && itemType != 5) MyDebugStr( __LINE__, __FILE__, "Error in TurnRadio");

if( alors)
{
	SetCtlValue( (ControlHandle) itemHandle, 1);
}
else
{
	SetCtlValue( (ControlHandle) itemHandle, 0);
}
}

void	ControlSwitch(short	item, DialogPtr	dlog, short	Switch)
{
Handle			itemHandle;
ControlHandle	control;
short			itemType;
Rect			itemRect;

//	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);

//	if( itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "ControlSwitch itemHandle = 0L");

	GetDialogItemAsControl( dlog, item, &control );
	
	HiliteControl( control, Switch);
	
	DrawOneControl( control);
}

void MyMoveControl( ControlHandle ah, short x, short y)
{
	Rect	rect;

	if( ah == 0) MyDebugStr( __LINE__, __FILE__, "MyMoveControl ah == 0");
	
	GetControlBounds( ah, &rect);

	if( rect.top  != y ||
		rect.left != x)
		MoveControl( ah, x, y);
}

void MySizeControl( ControlHandle ah, short x, short y)
{
	Rect	rect;
	
	if( ah == 0) MyDebugStr( __LINE__, __FILE__, "MySizeControl ah == 0");
	
	GetControlBounds( ah, &rect);
	
	if( rect.right - rect.left != x ||
		rect.bottom - rect.top != y)
		SizeControl( ah, x, y);
}

void MySizeWindow( DialogPtr dlg, short right, short bottom, Boolean v)
{
Rect	caRect;

	GetPortBounds( GetDialogPort( dlg), &caRect);
	
	if( bottom != caRect.bottom || right != caRect.right)
	{
		if( right <= 0) return;
		if( bottom <= 0) return;
		
		SizeWindow( GetDialogWindow( dlg), right, bottom, v);
	}
}

static Boolean	FKeyMode;

void SetFKeyMode( Boolean v)
{
	FKeyMode = v;
}

pascal void myTrackAction( ControlHandle theCntl, short ctlPart)
{
//	WaitNextEvent( everyEvent, &theEvent, 1, 0L);
	
//	#if MACOS9VERSION
	DoGlobalNull();
//	#endif
}


Boolean MyTrackControl( ControlHandle	myCtl, Point where, ControlActionUPP function)
{
	Point		myPt = where;
	Boolean		JobOK = true;
	short		hiliteCpy, val;
	Byte		visible;
	Rect		contrlRect;
	
	#if MACOS9VERSION

	if( myCtl == 0L) MyDebugStr( __LINE__, __FILE__, "Ctl == 0L");
	
	hiliteCpy = GetControlHilite( myCtl);
	
	if( FKeyMode)
	{
		unsigned long ttL;
		
		HiliteControl( myCtl, kControlButtonPart);
		
		Delay( 5, &ttL);		
		
		HiliteControl( myCtl, hiliteCpy);
		
		return true;
	}
	
	GlobalToLocal( &myPt);

	visible = IsControlVisible( myCtl); //(*myCtl)->contrlVis;
	SetControlVisibility( myCtl, true, false);	//(*myCtl)->contrlVis = 255;
	HiliteControl( myCtl, kControlButtonPart);
	
//	MADPlaySndHandle( MADDriver, (Handle) InSound, 0, 20);
//	MADPlaySndHandle( MADDriver, (Handle) InSound, 1, 20);
	
	if( thePrefs.clickSound)	SndPlay( 0L, InSound, true);

	do
	{
		GetControlBounds( myCtl, &contrlRect);		

		if( PtInRect( myPt, &contrlRect))
		{
			if( GetControlHilite( myCtl) != kControlButtonPart) HiliteControl( myCtl, kControlButtonPart);
			JobOK = true;
		}
		else
		{
			if( GetControlHilite( myCtl) != 0) HiliteControl( myCtl, 0);
			JobOK = false;
		}
		GetMouse( &myPt);

		if( function != 0L)
		{
			InvokeControlActionUPP( myCtl, GetControlHilite( myCtl), function);
		/*	#if defined(powerc) || defined (__powerc)
			#define 		MyCallMode ( kCStackBased|\
							RESULT_SIZE( SIZE_CODE( sizeof( Byte)))|\
							STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof( ControlHandle)))|\
			 				STACK_ROUTINE_PARAMETER( 2, SIZE_CODE( sizeof( short))))
				
				CallUniversalProc( function, MyCallMode, myCtl, GetControlHilite( myCtl));
			#else
				typedef pascal void (*MyProcPtr) ( ControlHandle, short);

				((MyProcPtr) function) (myCtl, GetControlHilite( myCtl));
			#endif*/
		}
		
		DoGlobalNull();
		
	}while( Button());
	
	if( thePrefs.clickSound)	SndPlay( 0L, OutSound, true);
//	MADPlaySndHandle( MADDriver, (Handle) OutSound, 0, 20);
//	MADPlaySndHandle( MADDriver, (Handle) OutSound, 1, 20);

	HiliteControl( myCtl, hiliteCpy);
//	(*myCtl)->contrlVis = visible;
	SetControlVisibility( myCtl, visible, false);
	
	return JobOK;

	#else
	
	GlobalToLocal( &myPt);
	
	visible = IsControlVisible( myCtl); //(*myCtl)->contrlVis;
	SetControlVisibility( myCtl, true, false);	//(*myCtl)->contrlVis = 255;
	HiliteControl( myCtl, kControlButtonPart);
	
	if( function == 0L)
	{
		ControlActionUPP	MyControlUPP = NewControlActionUPP( myTrackAction);
		
		val = TrackControl( myCtl, myPt, (ControlActionUPP) MyControlUPP);
		DisposeControlActionUPP( MyControlUPP);
	}
	else val = TrackControl( myCtl, myPt, (ControlActionUPP) function);
	
	
	SetControlVisibility( myCtl, visible, false);
	
	return val;
	#endif
}

short NewOffscreenBitMap(BitMap *thePixMap, Rect *picRect)
{
	BitMap				offscreenPixMap ;
	char				*offscreenData ;
	short 				offrowbytes;
	short				thedepth;
	unsigned long		memoryRequest ;
	

	/* get some memory for our offscreenPixMap^^.baseAddr */
	thedepth = 1;

	offrowbytes = 1 + (picRect->right-picRect->left)/8;
	memoryRequest = (long)((picRect->bottom-picRect->top+1)* (long) offrowbytes) + 4;

	offscreenData = MyNewPtr(memoryRequest) ;
	if (offscreenData == 0L) return -1;
	
	offscreenPixMap.bounds = *picRect ;
	offscreenPixMap.rowBytes = offrowbytes;
	offscreenPixMap.baseAddr = offscreenData ;
	*thePixMap = offscreenPixMap;

	return (0);
}

void ZapBitMap(BitMap *offscreenPixMap)
{
	char			*offscreenData ;

	/* Since DisposPixMap doesn't free the data memory we have to
			dispose the pointer to the data first !! */

	offscreenData = offscreenPixMap->baseAddr; 
	MyDisposePtr( &offscreenData);
	offscreenPixMap->baseAddr = 0L;
}

/*void BitToPic(PicHandle *thepict, BitMap LeDessin, Rect *THER)
{
Rect					imageRect;
GDHandle				CurrentGDevice ;
GrafPtr					currentPort ;
GrafPtr					offscreenPort ;		
GrafPort				offscreenPortData;
Rect					aRect ;
short					part, inc ;
short					pic_SIZE;
	
	imageRect = *THER;

	CurrentGDevice = GetGDevice() ;
	GetPort(&currentPort) ;

	offscreenPort = &offscreenPortData;
	OpenPort(offscreenPort) ;

	(*offscreenPort->visRgn)->rgnBBox = (*offscreenPort->clipRgn)->rgnBBox ;

	SetPort(offscreenPort) ;
	SetPortBits(&LeDessin) ;

	*thepict = OpenPicture(&imageRect);
	aRect = imageRect;
	pic_SIZE = (imageRect.bottom - imageRect.top) +1;
	
	aRect.bottom = 0;
	part = pic_SIZE/10;
	for ( inc = 0; inc <= 9; inc++ )  {
		aRect.top = aRect.bottom;
		aRect.bottom+= part;
		CopyBits(&LeDessin,&LeDessin,&aRect,&aRect,srcCopy,0L);
		}
		
	if (part* 10 != pic_SIZE ) {
		aRect.top = part * 10;
		aRect.bottom = pic_SIZE;

		CopyBits(&LeDessin,&LeDessin,&aRect,&aRect,srcCopy,0L);
		}
	ClosePicture();
	SetPort(currentPort);
}*/

void SetDText (DialogPtr dlog, short item, Str255 str)
{
Handle			itemHandle;
short			itemType;
Rect			itemRect;
Str255			myStr;
ControlHandle	control;
OSErr			err;

	GetDialogItemAsControl( dlog, item, &control );
	err = SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	if( err) MyDebugStr( __LINE__, __FILE__, "Error in SetDText");
	DrawOneControl( control);
}

void WriteCText (DialogPtr dlog, short item, char *str)
{
Handle	itemHandle;
short	itemType, iWidth;
Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	
	if( itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "WriteCText itemHandle = 0L");

	MyC2PStr( str);
	
	iWidth = StringWidth( (unsigned char*) str);
	
	MoveTo( itemRect.left + (itemRect.right - itemRect.left)/2 - iWidth/2, itemRect.bottom);
	
	DrawString( (unsigned char*) str);
	MyP2CStr( (unsigned char*) str);
}

void GetDText (DialogPtr dlog, short item, StringPtr str)
{
Handle	itemHandle;
short	itemType;
Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	if( itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "Error in SetDText");
	if( itemType >= 128) itemType -= 128;
	if( itemType != 8 && itemType != 16) MyDebugStr( __LINE__, __FILE__, "Error in itemType");
	
	GetIText (itemHandle, str);
}

void OSType2Str( OSType type, Str255 str)
{
	short i;

	str[ 0] = 4;
	BlockMoveData( &type, str+1, 4);

	for( i = 4; i > 0; i--)
	{
		if( str[ i] == ' ') str[ 0]--;
		else return;
	}
}

void OSType2Ptr( OSType type, Ptr str)
{
	short i;
	
	BlockMoveData( &type, str, 4);
	str[ 4] = 0;
}

OSType Ptr2OSType( Ptr str)
{
	short 	i;
	OSType	type;
	
	i = strlen( str);
	if( i > 4) i = 4;
	type = '    ';
	BlockMoveData( str, &type, i);
	
	return type;
}

/*void pStrcpy(register unsigned char *s1, register unsigned char *s2)
{
	register short len, i;
	
	if (*s2 <= 220) 
	{
		len = *s2;
		for ( i = 0; i <= len; i++)
		{
			s1[ i] = s2[ i];
		}
	}
	else MyDebugStr( __LINE__, __FILE__, "String to biiig !");
}*/


void pStrcat(register unsigned char *s1, register unsigned char *s2)
{
	register unsigned char *p;
	register short len, i;
	
	if (*s1+*s2<=255) 
	{
		p = *s1 + s1 + 1;
		*s1 += (len = *s2++);
	}
	else 
	{
		*s1 = 255;
		p = s1 + 256 - (len = *s2++);
	}
	for (i=len; i; --i) *p++ = *s2++;
}

void ErasePixMap( PixMapHandle pix)
{
long	rowbytes, bb;

	rowbytes = (*pix)->rowBytes + 0x8000;
	for( bb = 0; bb < (*pix)->bounds.bottom * rowbytes; bb++)
	{
		*((*pix)->baseAddr + bb) = 0xFF;
	}
}

short NewOffscreenPixMap(PixMapHandle *thePixMap, Rect *picRect)
{
	PixMapHandle		offscreenPixMap ;
	char				*offscreenData ;
	short 				thedepth;
	long 				offrowbytes, i;
	unsigned long 		memoryRequest ;
	ColorTable			*myCTable, **srcCTable;
	

	/* get some memory for our offscreenPixMap^^.baseAddr */
	thedepth = 8;

	offrowbytes = ((thedepth*(picRect->right-picRect->left) + 31) / 32) * 4;
	memoryRequest = (long)((picRect->bottom-picRect->top+1)*offrowbytes) + 4;

	offscreenData = MyNewPtr( memoryRequest);
	if (offscreenData == nil)
	{
		*thePixMap = (PixMapHandle) -1L;
		return( -1);
	}
	
	offscreenPixMap =  NewPixMap();
	(*offscreenPixMap)->bounds		= *picRect ;
	(*offscreenPixMap)->rowBytes	= offrowbytes+0x8000 ;  
	(*offscreenPixMap)->baseAddr	= offscreenData ;
	(*offscreenPixMap)->pixelType	= 0 ;
	(*offscreenPixMap)->pixelSize	= 8 ;			/* bits per pixel	*/
	(*offscreenPixMap)->cmpCount 	= 1 ;			/* 1 byte/pixel 	*/
	(*offscreenPixMap)->cmpSize 	= 8 ;			/* 8 bits/byte 		*/

	DisposeHandle( (Handle) (*offscreenPixMap)->pmTable);
	
	(*offscreenPixMap)->pmTable = GetCTable( 8);
//	DetachResource( (Handle) (*offscreenPixMap)->pmTable);
//	if( HandToHand( (Handle*) &(*offscreenPixMap)->pmTable) != noErr) MyDebugStr( __LINE__, __FILE__, "HandToHand Error");
//	DisposeHandle( (*offscreenPixMap)->pmTable);

	*thePixMap = offscreenPixMap;
	return ( noErr);
}

Ptr NewADDRPtrI( short SizeBuffer)
{
	Ptr		tempPtr;
	
	tempPtr = MyNewPtr( (long) (SizeBuffer));
	return( (Ptr) tempPtr);
}

void ZapPixMap(PixMapHandle *offscreenPixMap)
{
	if( (**offscreenPixMap)->pmTable != 0L)
	{
		MyDisposHandle( (Handle*) &(**offscreenPixMap)->pmTable);
		(**offscreenPixMap)->pmTable = 0L;
	}

	MyDisposePtr( &(**offscreenPixMap)->baseAddr);
	DisposPixMap( *offscreenPixMap);
	*offscreenPixMap = 0L;
}

/*void PixToPic(PicHandle *thepict, PixMapHandle theDocument, Rect imageRect)
{
PixMapHandle			offscreenPixMap ;
GDHandle				CurrentGDevice ;
GrafPtr					currentPort ;
CGrafPtr				offscreenPort ;		
CGrafPort				offscreenPortData;
Rect					aRect ;
OSErr					errCode;

	CurrentGDevice = GetGDevice() ;
	GetPort(&currentPort) ;
	
	errCode = NewOffscreenPixMap(&offscreenPixMap, &imageRect);
	if(errCode != noErr) {
			SysBeep(100);
			return;
			}

	offscreenPort = &offscreenPortData;
	OpenCPort( offscreenPort) ;

	(*offscreenPort->visRgn)->rgnBBox = (*offscreenPort->clipRgn)->rgnBBox ;

	SetPort( (GrafPtr) offscreenPort) ;
	SetPortPix(offscreenPixMap) ;

	
	aRect.bottom = imageRect.bottom - imageRect.top;
	aRect.right = imageRect.right - imageRect.left;
	aRect.top = 0;
	aRect.left = 0;
	
	*thepict = OpenPicture(&aRect);
		
	CopyBits( (BitMap*) *theDocument,(BitMap*) *offscreenPixMap, &imageRect, &aRect, srcCopy, 0L);
	
	ClosePicture();
	
	(*offscreenPixMap)->pmTable = nil ; 
	ZapPixMap( &offscreenPixMap);		

	SetPort(currentPort);
	SetGDevice(CurrentGDevice);
}*/

short PicToPix (PicHandle thePic, PixMapHandle *offscreenPixMap)
{
	Rect				picRect;
	GDHandle			oldGDeviceH;
	OSErr				errCode;
	CGrafPtr			oldPort;
	GWorldPtr			theGWorld = 0L;
	
	picRect.bottom 	= (*thePic)->picFrame.bottom	- (*thePic)->picFrame.top;
	picRect.right 	= (*thePic)->picFrame.right		- (*thePic)->picFrame.left;
	picRect.top 	= 0;	picRect.left 	= 0;
	
	GetGWorld( &oldPort, &oldGDeviceH);
	
	NewGWorld(	&theGWorld,
				8,
				&picRect,
				0L,				// CTabHandle
				nil,
				(GWorldFlags) 0);
	
	errCode = NewOffscreenPixMap( &*offscreenPixMap, &picRect);
	if( errCode != noErr) MyDebugStr( __LINE__, __FILE__, "Error in PicToPix");
	
	LockPixels( GetPortPixMap( theGWorld));
	SetGWorld( theGWorld, 0L);
	
	DrawPicture( thePic, &picRect);
	
	CopyBits(	(BitMap*) *GetPortPixMap( theGWorld),
 				(BitMap*) **offscreenPixMap,
 				&picRect,
 				&picRect,
 				srcCopy,				//ditherCopy
 				0L);
 	
 	SetGWorld( oldPort, oldGDeviceH);				
	DisposeGWorld( theGWorld);
	
	return 0;
}

short IclToPix (Handle theIcon, PixMapHandle *offscreenPixMap)
{	Rect						picRect;
	OSErr						errCode;

	short							i,x;
	char						temp1,temp2;

	picRect.bottom 	= 32;
	picRect.right 	= 32;
	picRect.top 	= 0;
	picRect.left 	= 0;

	errCode = NewOffscreenPixMap(&*offscreenPixMap, &picRect);
	if(errCode != noErr) {
			ExitToShell();
			return(errCode);
			}
	
	if( theDepth == 8)
	{
		for( i = 0; i < picRect.bottom * picRect.right; i++)
		(***offscreenPixMap).baseAddr[i] = (char) (*theIcon)[i];
	}
	else
	{
		for( i=0, x=0; i< picRect.bottom * picRect.right/2; i++,x++)
		{
			temp1 = (*theIcon)[x++] / 16;
			temp2 = (*theIcon)[x] / 16;
			temp1 = (char) (temp1 << 4);
			(***offscreenPixMap).baseAddr[i]= (char) (temp1 + temp2);
		}
	}
	
	return 0;
}

unsigned short RangedRdm( unsigned short min, unsigned short max)
{
	unsigned short		qdRdm;
	long				range, t;

	qdRdm = Random();
	range = max - min;
	t = (qdRdm * range) / 65536;
	return( t+min);
}

void DessineRgn(Rect *PixMapRect, Rect *RectPict)
{
Rect	CarreVerif=*RectPict;
long	Var1,Var2;

Var1 = (long) (RectPict->bottom - RectPict->top);
Var2 = (long) (Var1 * (PixMapRect->right) / (PixMapRect->bottom));
RectPict->right = (short) (RectPict->left + Var2);

if(CarreVerif.right < RectPict->right)
{
RectPict->right = CarreVerif.right;
Var1 = (long) (RectPict->right - RectPict->left);
Var2 = (long) (Var1 * (PixMapRect->bottom) / (PixMapRect->right));
RectPict->bottom = (short) (RectPict->top + Var2);
}
}

/*#ifdef COMPIL68K
pascal OSErr SetDialogDefaultItem (DialogPtr theDialog, short newItem) = {0x303C,0x0304,0xAA68};
#endif*/

void	FrameButton(DialogPtr theDia, short whichButton, Boolean drawIt)
{
	SetDialogDefaultItem( theDia, whichButton);
}

short SetScroll(ControlHandle vScroll, TEHandle TEH)
{
	register short	n;
	double			height;
	
	height = TEGetHeight( 0, (*TEH)->nLines, TEH);
	height /= (double) (*TEH)->nLines;

	n = (**TEH).nLines - (((**TEH).viewRect.bottom-(**TEH).viewRect.top) / height);

	if ((**TEH).teLength > 0 && (*((**TEH).hText))[(**TEH).teLength-1]=='\r') n++;

	SetCtlMax( vScroll, n > 0 ? n : 0);
	
	if( gUseControlSize) SetControlViewSize( vScroll, ((**TEH).viewRect.bottom-(**TEH).viewRect.top) / height);

	return 0;
}

void FrameRectRelief( Rect *theRect)
{
RGBColor	theColor;
Rect		aCopy;

#define GrisClair	0xFFFF
#define GrisFonce	0x4000
	
if( (**(**GetMainDevice()).gdPMap).pixelSize < 8)
{
	aCopy.left = theRect->left ;
	aCopy.right = theRect->right +1;
	aCopy.top = theRect->top ;
	aCopy.bottom = theRect->bottom +1;
	ForeColor( blackColor);
	FrameRect( &aCopy);
	return;
}
MoveTo( theRect->left, theRect->top);

theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
RGBForeColor( &theColor);
LineTo( theRect->right, theRect->top);

theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
RGBForeColor( &theColor);
LineTo( theRect->right, theRect->bottom);

theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
RGBForeColor( &theColor);
LineTo( theRect->left, theRect->bottom);

theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
RGBForeColor( &theColor);
LineTo( theRect->left, theRect->top);


ForeColor( blackColor);
}


void SetFontStyle( DialogPtr dialog, SInt16 item, ControlFontStyleRec& style )
{
	ControlHandle 	control;
	OSErr			err;
	
	err = ::GetDialogItemAsControl( dialog, item, &control );
	
	if ( err == noErr )
		::SetControlFontStyle( control, &style );
}

void ChangeEditFont(DialogPtr TheDia)
{
	ControlFontStyleRec	style;
	
	style.flags = kControlUseFontMask;
	style.font = kControlFontSmallBoldSystemFont;	
	SetFontStyle( TheDia, 4, style);
}
