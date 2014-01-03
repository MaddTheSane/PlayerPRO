#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>

#define TEXTHI 		12
#define ITEMNO 		14
#define TEXTLARG	50

	extern	EventRecord				theEvent;
	extern	RGBColor				theColor;

			DialogPtr				DigitalDlog;

	static	long					oldDigiValue[ ITEMNO][ MAXTRACK];
	static	long					oldDigiValue2[ MAXTRACK];
	static	Rect					viewRect;
	static	ControlHandle			ControlDigi;

void NTStr( short	NoDigi, short		val, Ptr	theText);
void GetNoteString( short note, Str255	string);

void SetUpScrollDigi(void)
{
	MyMoveControl( ControlDigi, viewRect.left - 1, viewRect.bottom + 1);
	MySizeControl( ControlDigi, viewRect.right - viewRect.left + 2, 16);
	
	SetControlMinimum( ControlDigi, 0);
	SetControlMaximum( ControlDigi, MAXTRACK - (viewRect.right - viewRect.left) / TEXTLARG);
	
	if (gUseControlSize) SetControlViewSize( ControlDigi, (viewRect.right - viewRect.left) / TEXTLARG);
}

void DoGrowDigi( DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		temp = {0};
	short		tempB, tempA, avant;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	Rect		caRect;
	
	GetPort( &SavePort);
 	SetPortDialogPort( theDialog);
	
	temp.left = 381;
	temp.right = viewRect.left + MAXTRACK * TEXTLARG + 2;
	
	LocalToGlobal( &aPt);
	
	if (temp.right < temp.left) temp.bottom = temp.top;
	//	else if (temp.right > qd.screenBits.bounds.right - aPt.h) temp.right = qd.screenBits.bounds.right - aPt.h -2;
	
	GetPortBounds( GetDialogPort( theDialog), &caRect);
	
	temp.top = temp.bottom = caRect.bottom;
	
#if MACOS9VERSION
	temp.top++;
	temp.bottom++;
#endif
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( theDialog), theEvent.where, &temp);
	
	if (lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	GetPortBounds( GetDialogPort( theDialog), &caRect);
	
	avant = caRect.bottom - 15;
	
	MySizeWindow( theDialog, tempA, tempB, true);
	
	GetPortBounds( GetDialogPort( theDialog), &caRect);
	
	viewRect.right = caRect.right - 2;
	
	EraseRect( &caRect);
	InvalWindowRect( GetDialogWindow( theDialog), &caRect);
	
	SetUpScrollDigi();
	
	SetPort( SavePort);
}

void NoteName( short	Period, Ptr	String)
{
	short			NNames[ 12] =	{'C ','C#','D ','D#','E ','F ','F#','G ','G#','A ','A#','B '};
	short			NCount;
	Str255		WorkStr;
	
/*	if(Period == 0)
	{
     	strcpy( String, "000");
     	return;
    }
   
	NCount = 1;
    while( NCount <= 61)
    {
    	if (Period >= MADDriver->pitchTable[ NCount][ 0] )
    	{
    		OctavesName( NCount, String);
    		
    		return;
    	}
    	NCount++;
    }*/
	strcpy( String, "000");
}

void DoNullDigiWindow(void)
{
	short		i, x, start, end;
 	Rect		tempRect, aRect;
 	Str255		tempStr = "\p";
 	GrafPtr		savePort;
 	long		val;
 	RgnHandle	saveClip;
	
	if (!DigitalDlog) return;
	
	GetPort( &savePort);
	SetPortDialogPort( DigitalDlog);
	
	TextFont( 4);	TextSize( 9);
	
	saveClip = NewRgn();
	GetClip( saveClip);
 	tempRect = viewRect;
 	tempRect.top -= TEXTHI + 2;
 	ClipRect( &tempRect);
	
	tempRect = viewRect;
	
	tempRect.right = tempRect.left + TEXTLARG;
	tempRect.bottom = tempRect.top + TEXTHI;
	
	start = GetControlValue( ControlDigi);
	end = start + 1 + (viewRect.right - viewRect.left) / TEXTLARG;
	if (end > MAXTRACK) end = MAXTRACK;
	
	for (x = 0; x < ITEMNO; x++)
	{
		/****** Instrument *********/
		for (i = start; i < end; i ++)
		{
			switch( x)
			{
				case 0:
					val = MADDriver->chan[ i].ins + 1;
					NTStr( 2, val, (Ptr) tempStr);
					MyC2PStr( (Ptr) tempStr);
				break;
				
				case 1:
					if (MADDriver->chan[ i].note < NUMBER_NOTES)
					{
						val = MADDriver->chan[ i].note;
						GetNoteString( val, tempStr);
					}
					else val = oldDigiValue[ x][ i];
				break;
				
				case 2:
					val = MADDriver->chan[ i].period;
					NumToString( val, tempStr);
				break;
				
				case 3:
					val = MADDriver->chan[ i].cmd;
					if (val == -1) pStrcpy( tempStr, "\p--");
					else
					{
						val = MADDriver->chan[ i].cmd;
						sprintf( (Ptr) tempStr, "%lX", val);
						MyC2PStr( (Ptr) tempStr);
					}
				break;
				
				case 4:
					if (MADDriver->chan[ i].cmd == -1) pStrcpy( tempStr, "\p--");
					else
					{
						val = MADDriver->chan[ i].arg;
						sprintf( (Ptr) tempStr, "%lX", val);
						MyC2PStr( (Ptr) tempStr);
					}
				break;
				
				case 5:
					val = MADDriver->chan[ i].vol;
					NTStr( 2, val, (Ptr) tempStr);
					MyC2PStr( (Ptr) tempStr);
				break;
				
				case 6:
					if (MADDriver->chan[ i].maxPtr - MADDriver->chan[ i].curPtr < 0) val = 0;
					else val = MADDriver->chan[ i].maxPtr - MADDriver->chan[ i].curPtr;
					NumToString( val, tempStr);
				break;
				
				case 7:
					if (MADDriver->chan[ i].maxPtr - MADDriver->chan[ i].begPtr < 0) val = 0;
					else val = MADDriver->chan[ i].maxPtr - MADDriver->chan[ i].begPtr;
					NumToString( val, tempStr);
				break;
				
				case 8:
					val = MADDriver->chan[ i].loopBeg;
					NumToString( val, tempStr);
				break;
				
				case 9:
					val = MADDriver->chan[ i].loopSize;
					NumToString( val, tempStr);
				break;
				
				case 10:
					if (MADDriver->chan[ i].maxPtr - MADDriver->chan[ i].begPtr > 0 &&
						MADDriver->chan[ i].curPtr > MADDriver->chan[ i].begPtr + MADDriver->chan[ i].loopBeg &&
						MADDriver->chan[ i].loopSize > 2) val = 1;
					else val = 0;
					
					if (oldDigiValue[ x][ i] != val)
					{
						if (val ) ForeColor( redColor);
						else ForeColor( blackColor);
						
						aRect = tempRect;
						InsetRect( &aRect, 15, 2);
						PaintRect( &aRect);
						aRect.left--;	aRect.top--;
						FrameRectRelief( &aRect);
						
						oldDigiValue[ x][ i] = val;
					}
				break;
				
				case 13:
					if (oldDigiValue[ x][ i] != val)
					{
						val = 10;
						oldDigiValue[ x][ i] = val;
						
						SwitchColor( i);
						aRect = tempRect;
						InsetRect( &aRect, 15, 2);
						PaintRect( &aRect);
						aRect.left--;	aRect.top--;
						FrameRectRelief( &aRect);
						
						ForeColor( blackColor);
					}
				break;
				
				case 11:
					if (MADDriver->chan[ i].maxPtr > MADDriver->chan[ i].curPtr) val = 1;
					else val = 0;
					
					if (oldDigiValue[ x][ i] != val)
					{
						if (val ) ForeColor( redColor);
						else ForeColor( blackColor);
						
						aRect = tempRect;
						InsetRect( &aRect, 15, 2);
						PaintRect( &aRect);
						aRect.left--;	aRect.top--;
						FrameRectRelief( &aRect);
						
						oldDigiValue[ x][ i] = val;
					}
				break;
				
				case 12:
					if (i < MADDriver->DriverSettings.numChn) val = 1;
					else val = 0;
						
						
					if (oldDigiValue[ x][ i] != val)
					{
						if (val ) ForeColor( redColor);
						else ForeColor( blackColor);
						
						aRect = tempRect;
						InsetRect( &aRect, 15, 2);
						PaintRect( &aRect);
						aRect.left--;	aRect.top--;
						FrameRectRelief( &aRect);
						
						oldDigiValue[ x][ i] = val;
					}
				break;
			}
			
			if (oldDigiValue[ x][ i] != val)
			{
				oldDigiValue[ x][ i] = val;
				TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
				ForeColor( blackColor);
			}
			tempRect.right += TEXTLARG;		tempRect.left += TEXTLARG;
		}
	//	tempRect.right -= MAXTRACK*TEXTLARG;	tempRect.left -= MAXTRACK*TEXTLARG;
		
		tempRect.left = viewRect.left;
		tempRect.right = tempRect.left + TEXTLARG;

		tempRect.top += TEXTHI;					tempRect.bottom += TEXTHI;
	}
	
	ForeColor( blackColor);
	
	SetClip( saveClip);
	DisposeRgn( saveClip);
	
	tempRect.top += TEXTHI;		tempRect.bottom += TEXTHI;
	tempRect.top += TEXTHI;		tempRect.bottom += TEXTHI;
	tempRect.top += TEXTHI;		tempRect.bottom += TEXTHI;
	tempRect.right = tempRect.left + 20;
	if (oldDigiValue2[ 0] != MADDriver->speed)
	{
		oldDigiValue2[ 0] = MADDriver->speed;
		NumToString( MADDriver->speed, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}
	tempRect.left += 2*TEXTLARG;		tempRect.right += 2*TEXTLARG;
	
	if (oldDigiValue2[ 1] != MADDriver->finespeed)
	{
		oldDigiValue2[ 1] = MADDriver->finespeed;
		NumToString( MADDriver->finespeed, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}
	tempRect.left -= 2*TEXTLARG;		tempRect.right -= 2*TEXTLARG;
	tempRect.top += TEXTHI;		tempRect.bottom += TEXTHI;
	
	if (oldDigiValue2[ 2] != MADDriver->Pat)
	{
		oldDigiValue2[ 2] = MADDriver->Pat;
		NumToString( MADDriver->Pat, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}
	tempRect.left += 2*TEXTLARG;		tempRect.right += 2*TEXTLARG;
	
	if (oldDigiValue2[ 3] != MADDriver->PartitionReader)
	{
		oldDigiValue2[ 3] = MADDriver->PartitionReader;
		NumToString( MADDriver->PartitionReader, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}
	tempRect.left -= 2*TEXTLARG;		tempRect.right -= 2*TEXTLARG;
	tempRect.top += TEXTHI;		tempRect.bottom += TEXTHI;
	
	if (oldDigiValue2[ 4] != MADDriver->DriverSettings.numChn)
	{
		oldDigiValue2[ 4] = MADDriver->DriverSettings.numChn;
		NumToString( MADDriver->DriverSettings.numChn, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}
	tempRect.left += 2*TEXTLARG;		tempRect.right += 2*TEXTLARG;
	
	if (oldDigiValue2[ 5] != curMusic->header->numChn)
	{
		oldDigiValue2[ 5] = curMusic->header->numChn;
		NumToString( curMusic->header->numChn, tempStr);
		TETextBox( tempStr + 1, tempStr[0], &tempRect, teCenter);
	}

	SetPort( savePort);
}

void  UpdateDigiListWindow(DialogPtr GetSelection)
{ 
		Rect   		tempRect, aRect;
 		GrafPtr		SavePort;
 		Str255		tempStr, aStr;
 		short		i ,x, start, end;
 		RgnHandle	saveClip;
 		RgnHandle	visibleRegion;
 		
 		
  		GetPort( &SavePort);
 		SetPortDialogPort( DigitalDlog);

		TextFont( 4);	TextSize( 9);

		BeginUpdate( GetDialogWindow( DigitalDlog));
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( DigitalDlog), visibleRegion);
		
 		UpdateDialog( DigitalDlog, visibleRegion);
		
		DisposeRgn( visibleRegion);
		
		tempRect = viewRect;
		
		tempRect.top -= TEXTHI + 2;
		tempRect.left -=2;	tempRect.left ++;
		FrameRectRelief( &tempRect);
		
		TextFace( bold);
		
		tempRect = viewRect;
		
		aRect.left = 0;
		aRect.right = tempRect.left - 2;
		aRect.top = tempRect.top;
		aRect.bottom = aRect.top + TEXTHI;
		
		pStrcpy(tempStr, "\pInstrument");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pNote");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pPeriod");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pEffect");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pArgument");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pVolume");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pBytes left");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pInstru Size");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pLoop Pos");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pLoop Size");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pIn Loop?");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pIn Progress?");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pActif?");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;

		pStrcpy(tempStr, "\pColor");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		/*** GŽnŽral Information ***/
		
		tempRect = viewRect;
		
		tempRect.top = aRect.top;	tempRect.bottom = aRect.bottom;
		pStrcpy(tempStr, "\pGeneral Information");
		TETextBox( tempStr + 1, tempStr[ 0], &tempRect, teJustCenter);
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pSpeed");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left += 2*TEXTLARG;		aRect.right += 2*TEXTLARG;
		
		pStrcpy(tempStr, "\pFinespeed");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left -= 2*TEXTLARG;		aRect.right -= 2*TEXTLARG;
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pPattern");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left += 2*TEXTLARG;		aRect.right += 2*TEXTLARG;
		
		pStrcpy(tempStr, "\pPosition");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left -= 2*TEXTLARG;		aRect.right -= 2*TEXTLARG;
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		pStrcpy(tempStr, "\pTracks Driver");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left += 2*TEXTLARG;		aRect.right += 2*TEXTLARG;
		
		pStrcpy(tempStr, "\pTracks Music");
		TETextBox( tempStr + 1, tempStr[ 0], &aRect, teJustRight);
		aRect.left -= 2*TEXTLARG;		aRect.right -= 2*TEXTLARG;
		aRect.top += TEXTHI;		aRect.bottom += TEXTHI;
		
		/********* LŽgende haut *************/

		saveClip = NewRgn();
 		GetClip( saveClip);
 		tempRect = viewRect;
 		tempRect.top -= TEXTHI + 2;
 		ClipRect( &tempRect);

		tempRect = viewRect;
	
		tempRect.right = tempRect.left + TEXTLARG;
		tempRect.bottom = tempRect.top;
		tempRect.top -= TEXTHI;
		
		start = GetControlValue( ControlDigi);
		end = start + 1 + (viewRect.right - viewRect.left) / TEXTLARG;
		if (end > MAXTRACK) end = MAXTRACK;
		
		for (x = GetControlValue( ControlDigi); x < MAXTRACK; x++)
		{
			NumToString( x+1, aStr);
			pStrcpy( tempStr, aStr);
			TETextBox( tempStr+ 1, tempStr[0], &tempRect, teCenter);
			tempRect.right += TEXTLARG;		tempRect.left += TEXTLARG;
		}
		
		for (x = 0; x < ITEMNO ; x++)
		{
			for (i = start; i < end; i++) oldDigiValue[ x][ i] = -900;
		}
		
		for (x = 0; x < MAXTRACK; x++)
		{	
			oldDigiValue2[ x] = -900;
		}
		
		TextFace( normal);
		
		SetClip( saveClip);
		DisposeRgn( saveClip);
		
		DrawGrowIconP( DigitalDlog);
		
		EndUpdate( GetDialogWindow( DigitalDlog));

		SetPort( SavePort);
} 

pascal void actionProcDigi(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			maxValue, minValue, curVal, sVal;
RgnHandle		aRgn;
Rect			tempRect;

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
				curVal -= (viewRect.right - viewRect.left) / TEXTLARG;
				curVal++;
				if (curVal < minValue) curVal = minValue;
			break;
			
			case kControlPageDownPart:
				curVal += (viewRect.right - viewRect.left) / TEXTLARG;
				curVal--;
				if (curVal > maxValue) curVal = maxValue;
			break;
		}
		
		SetControlValue( theControl, curVal);
		
		if (sVal != curVal)
		{
			SetRect( &tempRect, viewRect.left, viewRect.top - TEXTHI + 2, viewRect.right, viewRect.bottom);
		
			aRgn = NewRgn();
		
			ScrollRect( &tempRect, (sVal - curVal)*TEXTLARG, 0, aRgn);
			EraseRgn( aRgn);
			InvalWindowRgn( GetDialogWindow( DigitalDlog), aRgn);
			
			UpdateDigiListWindow( DigitalDlog);
			DoNullDigiWindow();
			
			DisposeRgn( aRgn);
		}
}

void DoItemPressDigiList( short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
 		short				bogus, ctlPart;
 		Rect				tempRect;
 		GrafPtr				SavePort;
 		ControlHandle		theControl;
 		Point				myPt;
 		ControlActionUPP	MyControlUPP;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( DigitalDlog);
 
 		if (theEvent.what == mouseDown) /* See if a mouse click */
		{
			myPt = theEvent.where;
			GlobalToLocal( &myPt);
			
			ctlPart = FindControl( myPt, GetDialogWindow( whichDialog), &theControl);
	
			if (ctlPart == kControlIndicatorPart)
			{
				bogus = TrackControl( theControl, myPt, NULL);
				if (bogus != 0)
				{
					tempRect = viewRect;
					tempRect.top -= TEXTHI + 2;
					
					EraseRect( &tempRect);
					InvalWindowRect( GetDialogWindow( whichDialog), &tempRect);
				}
			}
			else if (ctlPart > 0)
			{
    			MyControlUPP = NewControlActionUPP( actionProcDigi);
				TrackControl(theControl, myPt, MyControlUPP);
				DisposeControlActionUPP( MyControlUPP);
			}
		}

		SetPort( SavePort);
}

/*void SetMaxBottomWindow( short maxRight, DialogPtr	whichWindow)
{
	WindowPeek	wPeek;
	WStateData	*wspd;

	wPeek = ( WindowPeek) whichWindow;
	
	wspd = (WStateData*) *(wPeek->dataHandle);
	
	if (maxRight > qd.screenBits.bounds.bottom - 2) maxRight = qd.screenBits.bounds.bottom - 2;
	wspd->stdState.bottom = maxRight;
}*/

void CreateDigiListWindow(void)
{
	Rect		itemRect;
	Handle		itemHandle;
	short		itemType;
	GrafPtr		savePort;
	Rect	caRect;
	
	

	if (DigitalDlog != NULL)
	{
		SetWindEtat( GetDialogWindow(DigitalDlog));
		return;
	}
	
	GetPort( &savePort);
	
	SetItemMark( ViewsMenu, 4, checkMark);
	
	DigitalDlog = GetNewDialog( 150, NULL, GetDialogWindow( ToolsDlog));
	
	SetWindEtat( GetDialogWindow(DigitalDlog));
	SetPortDialogPort( DigitalDlog);
	
	GetPortBounds( GetDialogPort( DigitalDlog), &caRect);
	MySizeWindow( DigitalDlog, caRect.right, 22 * TEXTHI, false);
	
//	SetMaxBottomWindow( DigitalDlog->portRect.bottom + 42, DigitalDlog);
	
	GetPortBounds( GetDialogPort( DigitalDlog), &caRect);
	
	SetMaxWindow( 0, caRect.bottom + 42, DigitalDlog);
	
	GetDialogItem ( DigitalDlog, 1, &itemType, &itemHandle, &viewRect);
	
	viewRect.bottom = viewRect.top + 2 + (TEXTHI )*ITEMNO;
	
	GetDialogItem ( DigitalDlog, 2, &itemType, &itemHandle, &itemRect);	
	ControlDigi = (ControlHandle) itemHandle;
	
	TextFont(4);
	TextSize(9);
	
	theEvent.what = 0;
	DoGrowDigi( DigitalDlog);
	
	ShowWindow( GetDialogWindow( DigitalDlog));
	SelectWindow2( GetDialogWindow( DigitalDlog));
	
	SetPort( savePort);
}

void CloseDigiList(void)
{
	if (DigitalDlog != NULL)
	{
		DisposeDialog( DigitalDlog);
		SetItemMark( ViewsMenu, 4, noMark);
	}
	DigitalDlog = NULL;
}
