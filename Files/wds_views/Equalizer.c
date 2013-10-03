#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>
#include <QuickTime/QuickTime.h>

#define MAXBAR 8

	extern	EventRecord				theEvent;
	extern	Cursor					HandCrsr;
	extern	MenuHandle				ViewsMenu;
	extern	Boolean 				DragManagerUse, newQuicktime;
	
			DialogPtr				EQDlog;
	
	static	ControlHandle			EQCntl[ MAXBAR];
	static	ControlHandle			SaveBut, LoadBut, OnOff;
	static	short					*logoScale;

double EQInterpolate(double p,double p1,double p2,double v1,double v2);


void DrawCourbe( DialogPtr dia)
{
	GrafPtr		SavePort;
	short		itemType, i;
	Rect		itemRect;
	Handle		itemHandle;
	
	GetDialogItem( dia , 10, &itemType, &itemHandle, &itemRect);
	
	EraseRect( &itemRect);
	
	MoveTo( itemRect.left, itemRect.top + (MADDriver->Filter[ 0] * 256) / 2);
	
	for( i = 0; i < EQPACKET*2; i++)
	{
		LineTo( itemRect.left+i/4, itemRect.top + (MADDriver->Filter[ i] * 256) / 2);
	}
}

void  UpdateEQWindow( DialogPtr GetSelection)
{
	GrafPtr		SavePort;
	short		itemType, i;
	Rect		itemRect;
	Handle		itemHandle;
	Rect		caRect;
	
	

	GetPort( &SavePort);
	SetPortDialogPort( EQDlog);

	BeginUpdate( GetDialogWindow( EQDlog));

	for( i = 0; i<MAXBAR;i++) SetControlVisibility( EQCntl[ i], true, false);

	ForeColor( blackColor);
	
	GetPortBounds( GetDialogPort( GetSelection), &caRect);
	
	MoveTo( 0, 25);		LineTo( caRect.right, 25);
	
	GetDialogItem( EQDlog , 2, &itemType, &itemHandle, &itemRect);
	MoveTo( itemRect.left + (itemRect.right - itemRect.left)/2, itemRect.top);
	GetDialogItem( EQDlog , 2 + MAXBAR -1, &itemType, &itemHandle, &itemRect);
	LineTo( itemRect.left + (itemRect.right - itemRect.left)/2, itemRect.top);
	
	DrawDialog( EQDlog);
	
/*	ForeColor( whiteColor);
	PaintRect( &itemRect);*/
	
	for( i = 0; i<MAXBAR;i++) SetControlVisibility( EQCntl[ i], false, false);
	
	EndUpdate( GetDialogWindow( EQDlog));

	SetPort( SavePort);
} 

void DoNullEQ( void)
{
	GrafPtr				SavePort;
	
	if( EQDlog == NULL) return;
	
	GetPort( &SavePort);
	SetPortDialogPort( EQDlog);
	
	SetPort( SavePort);
}

void RecomputeEQ( void)
{
long	prev, next, prevC, nextC, i, x, khz, temp;
Str255	str;
	
	if( EQDlog == NULL) return;
	
	// Recompute the filter enveloppe
	
	prev = 0;
	prevC = 0;
	next = logoScale[ ((EQPACKET*2) / (MAXBAR-1))/4]*4;
	nextC = 1;
	
	for( i = 0 ; i < EQPACKET*2; i++)
	{
		if( i > next)
		{
			temp = (((nextC+1) * EQPACKET*2) / (MAXBAR-1))/4;
			
			if( temp < 256)
			{
				prev = next;
				prevC = nextC;
				
				next = logoScale[ temp]*4;
				nextC ++;
			}
			else
			{
				prev = next;
				prevC = nextC;
				
				next = EQPACKET*2;
				nextC = MAXBAR - 1;
			}
		}
		
		thePrefs.Filter[ i] = MADDriver->Filter[ i] = EQInterpolate( i, prev, next, GetControlValue( EQCntl[ prevC]) / 100.0, GetControlValue( EQCntl[ nextC]) / 100.0);
	}
	
/*	for( i = 0; i < MAXBAR; i++)
		{
			 x = (i * EQPACKET*2) / (MAXBAR-1);
			 
			 if( x / 4 >= 255) x = 4 * 255;
			 x = logoScale[ x / 4] * 4;
			
			if( x >= EQPACKET*2) x = (EQPACKET*2) -1;
			SetControlValue( EQCntl[ i], MADDriver->Filter[ x] * 100.0);
		}*/
	
	//DrawCourbe( EQDlog);
	
	khz = MADDriver->DriverSettings.outPutRate>>16;
	
	for( i = 0; i < MAXBAR; i++)
	{
		x = (i * EQPACKET) / MAXBAR;
		
		x = logoScale[ x/2]*4;
		
		x = (x * khz)/(EQPACKET*2);
		
		if( x < 1000)
		{
			sprintf( (Ptr) str, "%.1f Hz", (double) x);
			MyC2PStr( (Ptr) str);
		}
		else
		{
			sprintf( (Ptr) str, "%.1f Khz", (double) x / (double) 1000.0);
			MyC2PStr( (Ptr) str);
		}
		SetDText( EQDlog, 18+i, str);
		
		
		NumToString( GetControlValue( EQCntl[ i]), str);
		pStrcat( str, "\p %");
		
		SetDText( EQDlog, 30 + i, str);
	}
}

void InitEQWindow( void)
{
	long	i;
	
	for( i = 0 ; i < EQPACKET*2; i++)
	{
		MADDriver->Filter[ i] = thePrefs.Filter[ i];
	}
	
	MADDriver->Equalizer = thePrefs.useEQ;
}

void LoadEQ( void)
{
	FSSpec	spec;
	OSErr	iErr;
	long	inOutBytes, x, i;
	short	fRefNum;
	
	iErr = DoStandardOpen( &spec, "\pEQ file", 'EQ  ');
	
	if( iErr == noErr)
	{
		iErr = FSpOpenDF( &spec, fsCurPerm, &fRefNum);
	
		inOutBytes = sizeof( double) * EQPACKET*2;
		iErr = FSRead( fRefNum, &inOutBytes, MADDriver->Filter);
		
		iErr = FSCloseFork( fRefNum);
		
		for( i = 0; i < MAXBAR; i++)
		{
			 x = (i * EQPACKET*2) / (MAXBAR-1);
			 
			 if( x / 4 >= 255) x = 4 * 255;
			 x = logoScale[ x / 4] * 4;
			
			if( x >= EQPACKET*2) x = (EQPACKET*2) -1;
			
			SetControlVisibility( EQCntl[ i], true, false);
			
			SetControlValue( EQCntl[ i], MADDriver->Filter[ x] * 100.0);
			
			SetControlVisibility( EQCntl[ i], false, false);
		}
		
		RecomputeEQ();
	}
}

void SaveEQ( void)
{
	FSSpec				spec;
	Str255				theStr;
	long				inOutBytes;
	OSErr				iErr;
	short				fRefNum;
	
	pStrcpy( theStr, "\pEQ file");
	
	if( DoCustomSave( "\pSave EQ settings as:", theStr, 'EQ  ', &spec)) return;
	
	FSpDelete( &spec);
	iErr = FSpCreate( &spec, 'SNPL', 'EQ  ',smSystemScript);
	if( iErr == noErr)
	{
		iErr = FSpOpenDF( &spec, fsCurPerm, &fRefNum);
		
		inOutBytes = sizeof( double) * EQPACKET*2;
		iErr = FSWrite( fRefNum, &inOutBytes, MADDriver->Filter);
		
		iErr = FSCloseFork( fRefNum);
	}
}

void CreateEQWindow( void)
{
	Rect					itemRect, tempRect, dataBounds;
	Handle					itemHandle, rsrc;
	short					x, itemType, itemHit, temp, i;
	Point					cSize;
	FontInfo				ThisFontInfo;
	Str255					String;
	GrafPtr					savePort;
	
	if( EQDlog != NULL) return;
	
	rsrc = GetResource( 'LogC', 128);
	DetachResource( rsrc);
	
	logoScale = (short*)	NewPtrClear( 256 * sizeof( short));
	
	HLock( rsrc);
	BlockMoveData( *rsrc, logoScale, 256 * sizeof( short));
	HUnlock( rsrc);
	DisposeHandle( rsrc);
	
	
	SetItemMark( ViewsMenu, mEqualizer, checkMark);
	
	EQDlog = GetNewDialog( 183, NULL, GetDialogWindow( ToolsDlog));

	SetWindEtat( GetDialogWindow(EQDlog));
	SetPortDialogPort( EQDlog);
	
	TextFont( kFontIDGeneva);
	TextSize( 9);
	
	
	for( i = 0; i < MAXBAR; i++)
	{
		GetDialogItem( EQDlog, i + 2, &itemType, &itemHandle, &itemRect);
		EQCntl[ i] = NewControl( GetDialogWindow( EQDlog), &itemRect, "\p", true, MADDriver->Filter[ (i * EQPACKET*2) / MAXBAR] * 100.0, 0, 200, 57, 0);
		
		if( MADDriver->Equalizer) HiliteControl( EQCntl[ i], 0);
		else HiliteControl( EQCntl[ i], 255);
	}
	
	for( i = 0; i < MAXBAR; i++)
		{
			 x = (i * EQPACKET*2) / (MAXBAR-1);
			 
			 if( x / 4 >= 255) x = 4 * 255;
			 x = logoScale[ x / 4] * 4;
			
			if( x >= EQPACKET*2) x = (EQPACKET*2) -1;
			SetControlValue( EQCntl[ i], MADDriver->Filter[ x] * 100.0);
		}
		
		SetControlVisibility( EQCntl[ i], false, false);
		
	SelectWindow2( GetDialogWindow( EQDlog));
	SizeWindow( GetDialogWindow( EQDlog), 216, 190, true);

	ShowWindow( GetDialogWindow( EQDlog));

//	TurnRadio( 1, EQDlog, MADDriver->Equalizer);
	
	
	GetDialogItem( EQDlog , 14, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	LoadBut = NewControl( 	GetDialogWindow( EQDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							152,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem( EQDlog , 15, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	SaveBut = NewControl( 	GetDialogWindow( EQDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							151,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem( EQDlog , 17, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	OnOff = NewControl( 	GetDialogWindow( EQDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							214,
							kControlBevelButtonNormalBevelProc,
							0);
	
	if( MADDriver->Equalizer) HiliteControl( OnOff, kControlButtonPart);
	else HiliteControl( OnOff, 0);
	
	
//	SizeWindow( EQDlog, 350, 460, true);
	
	RecomputeEQ();
	
	return;
}

void DoItemPressEQ( short whichItem, DialogPtr whichDialog)
{
 		short				temp,itemType, newPL, newPat, newPartitionReader;
		Point				myPt;
		Handle				itemHandle;
		Rect				itemRect;
		long				i, x, oldH = 0, tempLong, max, val;
		GrafPtr				savePort;
		Boolean				ReadingCopy;
		ControlActionUPP	MyControlUPP;
		FSSpec				newFile;
		Str255				str;
		
		GetPort( &savePort);
		SetPortDialogPort( whichDialog);
		
			switch( whichItem)
			{
				case 17:
				//	InverseRadio( 1, EQDlog);
					
					MADDriver->Equalizer = !MADDriver->Equalizer;
					
					if( MADDriver->Equalizer) HiliteControl( OnOff, kControlButtonPart);
					else HiliteControl( OnOff, 0);
					
					for( i = 0; i < MAXBAR; i++)
					{
						SetControlVisibility( EQCntl[ i], true, false);
						
						if( MADDriver->Equalizer) HiliteControl( EQCntl[ i], 0);
						else HiliteControl( EQCntl[ i], 255);
						
						SetControlVisibility( EQCntl[ i], false, false);
					}
					
					
				break;
				
				case 14:
					if( GetControlHilite( LoadBut) == 0  && MyTrackControl( LoadBut, theEvent.where, NULL))
					{
						LoadEQ();
					}
				break;
				
				case 15:
					if( GetControlHilite( SaveBut) == 0  && MyTrackControl( SaveBut, theEvent.where, NULL))
					{
						SaveEQ();
					}
				break;
			}
			
			if( whichItem >= 2 && whichItem < 2 + MAXBAR)
			{
				GetMouse( &myPt);
				GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				if(PtInRect( myPt, &itemRect))
				{
					
					while( Button())
					{
						DoGlobalNull();
						SetPortDialogPort( whichDialog);
						GetMouse( &myPt);
						
						x = FindDialogItem( whichDialog, myPt)+1;
						
						if( x != 0)
						{
							if( x-2 < 0) x = 0;
							if( x-2 >= MAXBAR) x = 0;
						}
						
						if( x != 0 && x != whichItem)
						{
							whichItem = x;
							GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
							oldH = -1;
						}
						
						if( oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if( myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
					
							val = (200* (myPt.h - itemRect.left)) / ( itemRect.right-itemRect.left);
							
							SetControlVisibility( EQCntl[ whichItem-2], true, false);
							
							SetControlValue( EQCntl[ whichItem-2], val);
							
							SetControlVisibility( EQCntl[ whichItem-2], false, false);
							
						/*	NumToString( val, str);
							pStrcat( str, "\p %");
							
							SetDText( whichDialog, 30 + whichItem-2, str);*/
						}
						
						RecomputeEQ();
						
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
					}
				}
			}
		
		SetPort( savePort);
}

void CloseEQ(void)
{
	OSErr	iErr;

	if( EQDlog != NULL)
	{
		DisposeDialog( EQDlog);
		EQDlog = NULL;
		
		SetItemMark( ViewsMenu, mEqualizer, noMark);
		
		if( logoScale != NULL) DisposePtr( (Ptr) logoScale);	logoScale = NULL;
	}
	EQDlog = NULL;
	
	thePrefs.useEQ = MADDriver->Equalizer;
	
//	SetItemMark( ViewsMenu, m3D, noMark);
}
