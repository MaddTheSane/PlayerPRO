/*	Fade In Vol		*/
/*	v 1.0			*/
/*	1995 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include "MAD.h"
#include "PPPlug.h"


#define HIM		14
#define WIM		27

static void GetMDEFRect( Rect *aRect, Rect *menuRect, short whichItem)
{
	short	Xpos, Ypos;
	
	whichItem--;
	
	Xpos = whichItem - (whichItem/12)*12;
	Ypos = whichItem / 12;
	
	aRect->left 	= menuRect->left + Xpos*WIM;
	aRect->right 	= menuRect->left + (Xpos + 1)*WIM;
	
	aRect->top 		= menuRect->top + Ypos*HIM;
	aRect->bottom 	= menuRect->top + (Ypos + 1)*HIM;
}

static pascal void MyMenuNoteDefProc(short msg, MenuRef whichMenu, Rect *menuRect, Point hitPt, short *itemID)
{
	short					itemCount, j, mouseItem, mSize, Xpos, Ypos;
	Str255					str;
	Rect					tempRect;
	GDHandle				aH;
	RGBColor				theColor;
	MenuTrackingDataPtr		mTracking;

	GrafPtr				savedPort;
	short				savedFont;
	short				savedSize;
	
	
	GetPort( &savedPort);
	
	savedFont = GetPortTextFont( (CGrafPtr) savedPort);
	savedSize = GetPortTextSize( (CGrafPtr) savedPort);

	
	itemCount = CountMenuItems( whichMenu);

	TextFont( 0);	TextSize( 0);

	switch (msg)
	{
	case mDrawMsg:
		theColor.red = 56797;
		theColor.green = 56797;
		theColor.blue = 56797;
		
	//	RGBForeColor( &theColor);
		PaintRect( menuRect);
		
		ForeColor( blackColor);
	//	RGBBackColor( &theColor);
		
		for ( j = 1;  j <= itemCount;  j++)
		{
			GetMenuItemText( whichMenu, j, str);
			tempRect.bottom--;
			GetMDEFRect( &tempRect, menuRect, j);
			
			
			if( GetMenuID( whichMenu) == j) ForeColor( redColor);
			else ForeColor( blackColor);
			TETextBox( str+1, str[ 0], &tempRect, teJustCenter);
		}
		
		ForeColor( blackColor);
	break;

	case kMenuFindItemMsg:
		mTracking = (MenuTrackingDataPtr) itemID;
	
		if( !PtInRect( hitPt, menuRect)) mouseItem = 0;
		else
		{
			mouseItem = (hitPt.v - menuRect->top) / HIM;
			mouseItem *= 12;
			
			mouseItem += (hitPt.h - menuRect->left) / WIM;
			mouseItem++;
			
			if( mouseItem >= itemCount) mouseItem = itemCount;
		}
		
		if (mouseItem == 0)
		{		/* out of bounds or disabled */
			GetMDEFRect( &tempRect, menuRect, mTracking->itemSelected);
			InvertRect( &tempRect);
			mTracking->itemSelected = 0;						/* return "cancel" code */
		}
		else if ( mouseItem != mTracking->itemSelected )
		{
			GetMDEFRect( &tempRect, menuRect, mTracking->itemSelected);
			InvertRect( &tempRect);
			
			GetMDEFRect( &tempRect, menuRect, mouseItem);
			InvertRect( &tempRect);
			mTracking->itemSelected = mouseItem; 				/* return new */
		}
		
		mTracking->itemUnderMouse = mTracking->itemSelected;
		mTracking->itemRect = tempRect;
		break;

	case mSizeMsg:
		mSize = 0;
		for ( j=1;  j<=itemCount;  j++ )
		{
			GetMenuItemText( whichMenu, j, str);
			if( StringWidth( str) > mSize) mSize = StringWidth( str);
		}
		
		SetMenuWidth( whichMenu, mSize + 2);
		SetMenuHeight( whichMenu, itemCount*HIM);	/* assume 12 pts high */
		break;

	case mPopUpMsg:
		
		SetMenuID( whichMenu, *itemID);
		//(*whichMenu)->menuID = *itemID;
		
		Xpos = (*itemID - 1) - ((*itemID - 1)/12)*12;
		Ypos = (*itemID - 1) / 12;
		
		menuRect->top = hitPt.h - (Ypos * HIM);
		menuRect->left = hitPt.v - (Xpos * WIM);
		
		Ypos = (itemCount - 1) / 12;
		
		menuRect->right = menuRect->left + 12*WIM;
		menuRect->bottom = menuRect->top + (Ypos+1)*HIM;
		
		GetMouse( &hitPt);
		
		aH = GetDeviceList();
		do
		{
			if( aH != NULL)
			{
				if( PtInRect( hitPt, &(*(*aH)->gdPMap)->bounds))
				{
					Rect 		ar = (*(*aH)->gdPMap)->bounds;
					
					if( menuRect->bottom > ar.bottom) 	OffsetRect( menuRect, 0, ar.bottom - menuRect->bottom);
					if( menuRect->top < ar.top)			OffsetRect( menuRect, 0, ar.top - menuRect->top);
					if( menuRect->right > ar.right) 	OffsetRect( menuRect, ar.right - menuRect->right, 0);
					if( menuRect->left < ar.left)		OffsetRect( menuRect, ar.left - menuRect->left, 0);
				}
			}
			
			aH = GetNextDevice( aH);
		}
		while( aH != NULL);
	break;

	}
	
	TextFont( savedFont);	TextSize( savedSize);
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
	OSErr			err;

	GetDialogItemAsControl( dlog, item, &control );
	err = SetControlData( control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
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

Cmd* GetCmd( short row, short	track, Pcmd*	myPcmd)
{
	if( row < 0) row = 0;
	else if( row >= myPcmd->length) row = myPcmd->length -1;

	if( track < 0) track = 0;
	else if( track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return( &(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

static void OctavesName(short	id, Str255	String)
{
	short			NNames[ 12] =	{'C ','C#','D ','D#','E ','F ','F#','G ','G#','A ','A#','B '};
	Str255			WorkStr;
	
	if( id == 0xFF)
	{
		pStrcpy( String, "\p---");
		return;
	}
	
	NumToString( (id / 12), WorkStr);
	String[ 0] = 3;
	String[ 1] = NNames[ (id) % 12]>>8;
	String[ 2] = NNames[ (id) % 12];
  	String[ 3] = WorkStr[ 1];
}

static MenuHandle CreateMenu()
{
	MenuHandle	returnMenu;
	short		i;
	Str255		aStr;
	
	returnMenu = GetMenu( 141);
	
	OctavesName( 1, aStr);		SetMenuItemText( returnMenu, 1, aStr);
	for( i = 1; i < 96; i++)
	{
		OctavesName( i, aStr);
		AppendMenu( returnMenu, aStr);
	}
	
	return returnMenu;
}

static short Text2Note( Str255 myTT)
{
	short		Oct;

	Oct = myTT[ 3] - 48;
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	switch( myTT[1])
	{
		case 'C':	case'c':	Oct += 0;	break;
		case 'D':	case'd':	Oct += 2;	break;
		case 'E':	case'e':	Oct += 4;	break;
		case 'F':	case'f':	Oct += 5;	break;
		case 'G':	case'g':	Oct += 7;	break;
		case 'A':	case'a':	Oct += 9;	break;
		case 'B':	case'b':	Oct += 11;	break;
		
		default:	Oct = 0xFF;		break;
	}
	
	if( Oct != 0xFF)
	{
		if( myTT[ 2] == '#') Oct++;
		if( Oct >= 96) Oct = 0xFF;
		if( Oct < 0) Oct = 0xFF;
	}
	
	return( Oct);
}

OSErr mainFadeNote( Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit, itemType;
	Rect				itemRect;
	Handle				itemHandle;
	Str255				aStr;
	MenuHandle			noteMenu;
	Point				myPt;
	long				Result;
	MenuDefSpec			defSpec2;
	
	defSpec2.defType = kMenuDefProcPtr;
	defSpec2.u.defProc = NewMenuDefUPP( MyMenuNoteDefProc );
	RegisterMenuDefinition( 1972, &defSpec2);

	myDia = GetNewDialog( 128, NULL, (WindowPtr) -1L);
	SetPortDialogPort( myDia);
	AutoPosition( myDia);

	SetDText( myDia, 3, "\pC 3");
	SetDText( myDia, 4, "\pC 7");
	SelectDialogItemText( myDia, 3, 0, 200);
	
	noteMenu = CreateMenu();
	
	do
	{
		RESTART:
	
//		#if defined(powerc) || defined(__powerc)
		ModalDialog( thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//		#else
//		ModalDialog( (ModalFilterProcPtr) thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//		#endif
		
		switch( itemHit)
		{
			case 7:
			case 8:
				InsertMenu( noteMenu, hierMenu);
				
				GetDialogItem( myDia, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				GetDText( myDia, itemHit-4, aStr);
				SetItemMark( noteMenu, Text2Note( aStr) + 1, 0xa5);
				
				Result = PopUpMenuSelect(	noteMenu,
											myPt.v,
											myPt.h,
											Text2Note( aStr) + 1);
				
				SetItemMark( noteMenu, Text2Note( aStr) + 1, 0);
				
				if ( HiWord( Result ) != 0 )
				{
					OSErr	iErr;
				
					OctavesName( LoWord( Result)-1, aStr);
					SetDText( myDia, itemHit-4, aStr);
					SelectDialogItemText( myDia, 3, 0, 200);
				}
				
				DeleteMenu( GetMenuID( noteMenu));
			break;
		}
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		short	track, row;
		long	from, to;
		Cmd		*myCmd;
	
		GetDText( myDia, 3, aStr);		from 	= Text2Note( aStr);
		GetDText( myDia, 4, aStr);		to 		= Text2Note( aStr);
		
		// Check values
		
		if( from < 0 || from >= 96)
		{
			SelectDialogItemText( myDia, 3, 0, 200);
			SysBeep( 1);
			goto RESTART;
		}
		
		if( to < 0 || to >= 96)
		{
			SelectDialogItemText( myDia, 4, 0, 200);
			SysBeep( 1);
			goto RESTART;
		}
		
		for( track = 0; track < myPcmd->tracks; track++)
		{
			for( row = 0; row < myPcmd->length; row++)
			{
				myCmd = GetCmd( row, track, myPcmd);
				
				if( myPcmd->length > 1)			// no zero div !!
					myCmd->note	= from + ((to-from) * row) / (myPcmd->length-1);
				
				// my fade command : 0x10 min vol, 0x50 : max vol
				// Refer to MAD description for more informations			
			}
		}
	}
	
	DisposeDialog( myDia);
		
	DisposeMenu( noteMenu);
	
	return noErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x95, 0x45, 0xDB, 0x21, 0x5A, 0xDE, 0x49, 0xDC, 0x97, 0x17, 0x09, 0x3D, 0x09, 0xEC, 0x4D, 0x39)
//9545DB21-5ADE-49DC-9717-093D09EC4D39
#define PLUGINFACTORY FadeNoteFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeNote //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
