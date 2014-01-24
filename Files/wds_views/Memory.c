#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>

	extern	EventRecord				theEvent;
	
	static	unsigned long			FreeMemGlobalK;
	static	ControlHandle			progCntl;
	
			DialogPtr				MemoryDlog;
void PaintMemory(Rect	*theRect);


void DoGrowMem(DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp = {0}, cellRect, tempRect;
	short		cur, tempB, tempA, itemType, avant;
	Handle		itemHandle;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	BitMap		screenBits;

	GetPort(&SavePort);
 	SetPortDialogPort(theDialog);

	temp.left = 381;
//	temp.right = viewRect.left + MAXTRACK * TEXTLARG + 2;

	LocalToGlobal(&aPt);

	GetQDGlobalsScreenBits(&screenBits);

	if (temp.right < temp.left) temp.bottom = temp.top;
	else if (temp.right > screenBits.bounds.right - aPt.h) temp.right = screenBits.bounds.right - aPt.h -2;

	GetPortBounds(GetDialogPort(theDialog), &caRect);

	temp.top = temp.bottom = caRect.bottom;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);

	if (lSizeVH != 0)
	{
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	}
	else
	{
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	avant = caRect.bottom - 15;
	
	MySizeWindow(theDialog, tempA, tempB, true);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	EraseRect(&caRect);
	InvalWindowRect(GetDialogWindow(theDialog), &caRect);

	SetPort(SavePort);
}

void DoNullMemWindow(void)
{
	short		i, Larg, x, start, end;
 	short		itemType;
 	Handle		itemHandle;
 	Rect		tempRect, itemRect;
 	Str255		tempStr, aStr;
 	GrafPtr		savePort;
 	long		val, UsedMem, tempLong, tAlpha;
 	Boolean		GrafUpdate = false;
	
	if (MemoryDlog == NULL) return;
	
	GetPort(&savePort);
	SetPortDialogPort(MemoryDlog);
	
	tAlpha = FreeMem() / 1024;
	
	if (tAlpha != FreeMemGlobalK)
	{
		FreeMemGlobalK = tAlpha;
		
		NumToString(FreeMemGlobalK, tempStr);	pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 6, tempStr);
		UsedMem = 1;
		
		UsedMem /= 1024;
		UsedMem -= FreeMemGlobalK;
		NumToString(UsedMem, tempStr);		pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 3, tempStr);
		tAlpha = 0;
		
		if (AppearanceManager) SetControlValue(progCntl, (UsedMem * 100) / (FreeMemGlobalK + UsedMem));
		
		//***** Driver ********
		tempLong = 0;
	//	if (MADDriver->Vol != NULL) 			tempLong += GetPtrSize(MADDriver->Vol);
		if (MADDriver->IntDataPtr != NULL) 	tempLong += GetPtrSize(MADDriver->IntDataPtr);
		
	/*	for (i = 0; i < MAX_PITCH; i++)
		{
			if (MADDriver->FreqHandle[ i] != NULL) tempLong += GetPtrSize((Ptr) MADDriver->FreqHandle[ i]);
		}*/
		tempLong /= 1024;
		NumToString(tempLong, tempStr);
		pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 9, tempStr);		// Driver
		
		NumToString((tempLong * 100)/ UsedMem, tempStr);
		if (tempStr[ 0] == 1)
		{
			pStrcpy(aStr, "\p0");
			pStrcat(aStr, tempStr);
			pStrcpy(tempStr, aStr);
		}
		pStrcat(tempStr, "\p%");
		SetDText(MemoryDlog, 10, tempStr);
		
		tAlpha += tempLong;
		
		//**** Instruments ********
		tempLong = 0;
		for (i = 0; i < MAXINSTRU; i++)
		{
			for (x = 0; x < curMusic->fid[i].numSamples; x++)
			{
				if (curMusic->sample[ curMusic->fid[ i].firstSample + x] == NULL) MyDebugStr(__LINE__, __FILE__, "numSamples ERR");
				tempLong += curMusic->sample[ curMusic->fid[ i].firstSample + x]->size;
			}
		}
		tempLong /= 1024;
		NumToString(tempLong, tempStr);
		pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 12, tempStr);		// Instruments

		NumToString((tempLong * 100)/ UsedMem, tempStr);
		if (tempStr[ 0] == 1)
		{
			pStrcpy(aStr, "\p0");
			pStrcat(aStr, tempStr);
			pStrcpy(tempStr, aStr);
		}
		pStrcat(tempStr, "\p%");
		SetDText(MemoryDlog, 13, tempStr);

		tAlpha += tempLong;
		
		//***** Partition ********
		tempLong = 0;
		for (i = 0; i < curMusic->header->numPat; i++)
		{
			tempLong += GetPtrSize((Ptr) curMusic->partition[ i]);
		}
		tempLong /= 1024;
		
		NumToString(tempLong, tempStr);
		pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 15, tempStr);		// Partition

		NumToString((tempLong * 100)/ UsedMem, tempStr);
		if (tempStr[ 0] == 1)
		{
			pStrcpy(aStr, "\p0");
			pStrcat(aStr, tempStr);
			pStrcpy(tempStr, aStr);
		}
		pStrcat(tempStr, "\p%");
		SetDText(MemoryDlog, 16, tempStr);

		tAlpha += tempLong;
		
		//****** Player PRO ********
		tempLong = UsedMem - tAlpha;
		NumToString(tempLong, tempStr);
		pStrcat(tempStr, "\p Kb");
		SetDText(MemoryDlog, 18, tempStr);		// Player PRO

		NumToString((tempLong * 100)/ UsedMem, tempStr);
		if (tempStr[ 0] == 1)
		{
			pStrcpy(aStr, "\p0");
			pStrcat(aStr, tempStr);
			pStrcpy(tempStr, aStr);
		}
		pStrcat(tempStr, "\p%");
		SetDText(MemoryDlog, 19, tempStr);
		GrafUpdate = true;
	}

	if (GrafUpdate)
	{
		GetDialogItem (MemoryDlog, 1, &itemType, &itemHandle, &itemRect);
		PaintMemory(&itemRect);
	}

	SetPort(savePort);
}

void PaintMemory(Rect	*theRect)
{
	long	FreeMe, TotalMem;
	short		longRec, temp;
	Rect	CopyRect;
	
	if (AppearanceManager)
	{
		Draw1Control(progCntl);
		return;
	}
	
	CopyRect = *theRect;
	
	theRect->left ++;
	theRect->top ++;
	
	FreeMe 		= FreeMem();
		
	TotalMem = 1;
		
	longRec 	= theRect->right - theRect->left;
	
	temp 		= theRect->right;
	
	theRect->right = theRect->left + ((TotalMem - FreeMe)*longRec) / TotalMem;
	
	ForeColor(redColor);
	PaintRect(theRect);
	
	theRect->left = theRect->right;
	theRect->right = temp;
	
	ForeColor(greenColor);
	PaintRect(theRect);
	
	ForeColor(blackColor);
	*theRect = CopyRect;
}

void  UpdateMemWindow(DialogPtr GetSelection)
{ 
		Rect   		tempRect, itemRect;
 		GrafPtr		SavePort;
 		Str255		tempStr, aStr;
 		short		itemType, Larg, i ,x, start, end;
 		Handle		itemHandle;
 		RgnHandle	saveClip;
 		RgnHandle	visibleRegion;
 		
 		GetPort(&SavePort);
 		SetPortDialogPort(MemoryDlog);
		
		BeginUpdate(GetDialogWindow(MemoryDlog));
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(MemoryDlog), visibleRegion);
		
 		UpdateDialog(MemoryDlog, visibleRegion);
		
		DisposeRgn(visibleRegion);
		
		if (!AppearanceManager)
		{
			GetDialogItem (MemoryDlog, 1, &itemType, &itemHandle, &itemRect);
			FrameRectRelief(&itemRect);
		
			PaintMemory(&itemRect);
		}
		
	/*	GetDialogItem (MemoryDlog, 4, &itemType, &itemHandle, &itemRect);
		if (AppearanceManager)
		{
			RGBColor rgb;
	
			LMGetHiliteRGB(&rgb);
			RGBForeColor(&rgb);
		}
		else ForeColor(redColor);
		PaintRect(&itemRect);
		FrameRectRelief(&itemRect);
		
		GetDialogItem (MemoryDlog, 7, &itemType, &itemHandle, &itemRect);
		ForeColor(greenColor);
		PaintRect(&itemRect);
		FrameRectRelief(&itemRect);
	*/	
		EndUpdate(GetDialogWindow(MemoryDlog));

		SetPort(SavePort);
} 

void DoItemPressMem(short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
		Cell			theCell;
		long			tempLong;
 		short				temp, bogus, ctlPart;
 		Rect			tempRect;
 		GrafPtr			SavePort;
 		ControlHandle	theControl;
 		Point			myPt;
 		
 		GetPort(&SavePort);
 		SetPortDialogPort(MemoryDlog);
 
		SetPort(SavePort);
}

void CreateMemWindow(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;

	if (MemoryDlog != NULL)
	{
		SetWindEtat(GetDialogWindow(MemoryDlog));
		return;
	}

	MemoryDlog = GetNewDialog(163, NULL, GetDialogWindow(ToolsDlog));
	
	SetWindEtat(GetDialogWindow(MemoryDlog));
	SetPortDialogPort(MemoryDlog);
	
	TextFont(kFontIDGeneva);
	TextSize(9);
	
	ShowWindow(GetDialogWindow(MemoryDlog));
	SelectWindow2(GetDialogWindow(MemoryDlog));
	
	SetItemMark(ViewsMenu, mMemory, checkMark);
	
	FreeMemGlobalK = 0;
	
	if (AppearanceManager)
	{
		GetDialogItem(MemoryDlog, 1, &itemType, &itemHandle, &itemRect);
		progCntl = NewControl(	GetDialogWindow(MemoryDlog),
							&itemRect,
							"\p.",
							true,
							0,		//icl8 id
							0,
							100,
							80,
							0);
	}
}

void CloseMem(void)
{
	if (MemoryDlog != NULL) DisposeDialog(MemoryDlog);
	MemoryDlog = NULL;
	
	SetItemMark(ViewsMenu, mMemory, noMark);
}
