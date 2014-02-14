#include <Carbon/Carbon.h>
#include "PrivateList.h"
#include "Shuddup.h"

/**/

void PLScrollIntPartition(short curVal, short sVal, long lRefCon);

void DrawPartiListItem(short ID);
void DrawMODListItem(short iD);
void DrawInstruListItem(short iD);
void UpdatePlugsAbout(DialogPtr aDialog);
void DrawPlugInfo(short iD);

/**/

extern DialogPtr PartiDlog, MODListDlog, InstruListDlog, EditorDlog;

/**/

void PLCheckSelect(PrivateList *aL)
{
	Boolean	needChange = false;

	if (aL->select.left >= aL->maxX || aL->select.right >= aL->maxX)
		needChange = true;
	
	if (aL->select.top >= aL->maxY || aL->select.bottom >= aL->maxY)
		needChange = true;
	
	if (aL->select.left < 0 || aL->select.right < 0 ||
		aL->select.top < 0 || aL->select.bottom < 0 )
		needChange = true;
	
	if (needChange) {
		Rect cRect = aL->select;
	
		PLSetSelect(-1, -1, -1, -1, aL);
	
		PLSetSelect(cRect.left, cRect.top, cRect.right, cRect.bottom, aL);
	}
}

void PLRect(Rect *dstRect, Point cell, PrivateList	*aL)
{
	dstRect->left = aL->rect.left + (cell.h - PLGetCtlValue(aL->xScroll)) * aL->LCell;
	dstRect->right = dstRect->left +  aL->LCell;

	dstRect->top = aL->rect.top + (cell.v - PLGetCtlValue(aL->yScroll)) * aL->HCell;
	dstRect->bottom = dstRect->top +  aL->HCell;
}

void PLUpdateItem(Point cell, PrivateList *aL)
{
	Rect		aRect;
	GrafPtr		savePort;
	RgnHandle	saveClipRgn;
	RGBColor	color;
	
	if (cell.v < 0)
		cell.v = 0;
	if (cell.h < 0)
		cell.h = 0;
	if (cell.v >= aL->maxY)
		cell.v = aL->maxY-1;
	if (cell.h >= aL->maxX)
		cell.h = aL->maxX-1;
	
	GetPort(&savePort);
	SetPortDialogPort(aL->aDia);
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	ClipRect(&aL->rect);
	
	/* Erase old Rect */
	
	PLRect(&aRect, cell, aL);
	
	ForeColor(whiteColor);
	PaintRect(&aRect);
	ForeColor(blackColor);
	
	/**/
	
	GetBackColor(&color);
	BackColor(whiteColor);
	
	switch (GetWRefCon(GetDialogWindow(aL->aDia))) {
		case RefParti:
			DrawPartiListItem(cell.v);
			break;
			
		case RefMODList:
			DrawMODListItem(cell.v);
			break;
			
		case RefInstruList:
			DrawInstruListItem(cell.v);
			break;
			
		case 99802:
			DrawPlugInfo(cell.v);
			break;
	}
	RGBBackColor(&color);
	
	SetClip(saveClipRgn);
 	DisposeRgn(saveClipRgn);
	
	SetPort(savePort);
}

void PLUpdate(PrivateList *aL)
{
	switch (GetWRefCon(GetDialogWindow(aL->aDia))) {
		case RefParti:
			UpdatePartiWindow(aL->aDia);
			break;
			
		case RefMODList:
			UpdateMODListWindow(aL->aDia);
			break;
			
		case RefInstruList:
			UpdateInstruListWindow(aL->aDia);
			break;
			
		case RefPartition:
			UpdatePartitionWindow(aL->aDia);
			break;
			
		case 99802:
			UpdatePlugsAbout(aL->aDia);
			break;
	}
}

void PLScrollInt(short curVal, short sVal, long lRefCon, PrivateList *aL)
{
	if (GetWRefCon(GetDialogWindow(aL->aDia)) == RefPartition) {
		PLScrollIntPartition(curVal, sVal, lRefCon);
		return;
	}
	
	if (sVal != curVal) {
		RGBColor		bColor;
		RgnHandle		aRgn;
		Rect			aRect = aL->rect;
		
		GetBackColor(&bColor);
		BackColor(whiteColor);
		
		switch (lRefCon) {
			case yScrollNum:
				aRgn = NewRgn();
				
				ScrollRect(&aRect, 0, (sVal - curVal) * aL->HCell, aRgn);
				
				if (sVal - curVal == 1 && IsRegionRectangular(aRgn)) {
					Point		cPt;
					
					cPt.h = 0;
					cPt.v = PLGetCtlValue(aL->yScroll);
					
					PLUpdateItem(cPt, aL);
				} else if (sVal - curVal == -1 && IsRegionRectangular(aRgn)) {
					Point		cPt;
					
					cPt.h = 0;
					cPt.v = PLGetMaxYValue(aL);
					
					cPt.v--;
					if (cPt.v >= 0)
						PLUpdateItem(cPt, aL);
					
					cPt.v--;
					if (cPt.v >= 0)
						PLUpdateItem(cPt, aL);
				} else {
					PLUpdate(aL);
					//QDFlushPortBuffer(GetDialogPort(aL->aDia), aRgn);
				}
				DisposeRgn(aRgn);
				break;
				
			case xScrollNum:
				aRgn = NewRgn();
				
				ScrollRect(&aRect, (sVal - curVal) * aL->LCell, 0, aRgn);
				
				PLUpdate(aL);
				//QDFlushPortBuffer(GetDialogPort(aL->aDia), aRgn);
				DisposeRgn(aRgn);
				break;
		}
		
		RGBBackColor(&bColor);
	}
}

static PrivateList *myIntList;

void PLSetMyIntList(PrivateList *aL)
{
	myIntList = aL;
}

pascal void PLactionProcPartition(ControlHandle theControl, short ctlPart)
{
	long	lRefCon;
	short	maxValue, minValue, curVal, sVal;
	
	if (ctlPart <= 0) return;
	if (theControl == NULL) return;
	
	lRefCon = GetControlReference(theControl);
	maxValue = GetControlMaximum(theControl);
	minValue = GetControlMinimum(theControl);
	curVal = sVal = GetControlValue(theControl);
	
	switch (ctlPart) {
		case kControlUpButtonPart:
			curVal -= 1;
			if (curVal < minValue)
				curVal = minValue;
			break;
			
		case kControlDownButtonPart:
			curVal += 1;
			if (curVal > maxValue)
				curVal = maxValue;
			break;
			
		case kControlPageUpPart:
			switch (lRefCon) {
			case yScrollNum:
					curVal -= PLGetMaxYValue(myIntList) - GetControlValue(myIntList->yScroll) - 1;
					break;
					
			case xScrollNum:
					curVal -= PLGetMaxXValue(myIntList) - GetControlValue(myIntList->xScroll) - 1;
					break;
		}
			
			if (curVal < minValue)
				curVal = minValue;
			break;
			
		case kControlPageDownPart:
			switch (lRefCon) {
			case yScrollNum:
				curVal += PLGetMaxYValue(myIntList) - GetControlValue(myIntList->yScroll) - 1;
				break;
				
			case xScrollNum:
				curVal += PLGetMaxXValue(myIntList) - GetControlValue(myIntList->xScroll) - 1;
				break;
		}
			if (curVal > maxValue)
				curVal = maxValue;
			break;
	}
	
	if (curVal != sVal) {
		PLSetCtlValue(theControl, curVal);
		PLScrollInt(curVal, sVal, lRefCon, myIntList);
	} else if (ctlPart == kControlIndicatorPart) {
		PLScrollInt(curVal, myIntList->thumb, lRefCon, myIntList);
		myIntList->thumb = curVal;
	}
}

short PLGetCtlValue(ControlHandle aH)
{
	if (aH == NULL)
		return 0;
	else {
		return GetControlValue(aH);
	}
}

void PLSetCtlValue(ControlHandle aH, short val)
{
	if (aH != NULL)
		SetControlValue(aH, val);
}

short PLGetMaxYValue(PrivateList *aL)
{
	short	ret;
	
	ret = 1 + PLGetCtlValue(aL->yScroll) + (aL->rect.bottom - aL->rect.top) / aL->HCell;
	if (ret > aL->maxY)
		ret = aL->maxY;
	
	return ret;
}

short PLGetMaxXValue(PrivateList *aL)
{
	short	ret;
	
	ret = 1 + PLGetCtlValue(aL->xScroll) + (aL->rect.right - aL->rect.left) / aL->LCell;
	if (ret > aL->maxX)
		ret = aL->maxX;
	
	return ret;
}

Boolean PLConvertPoint(Point *cPt, PrivateList	*aL)
{
	Boolean	 result = true;

	cPt->h -= aL->rect.left;
	cPt->v -= aL->rect.top;
	cPt->h /= aL->LCell;
	cPt->v /= aL->HCell;
	
	if (aL->xScroll != NULL)
		cPt->h += PLGetCtlValue(aL->xScroll);
	
	if (aL->yScroll != NULL)
		cPt->v += PLGetCtlValue(aL->yScroll);

	if (cPt->h < 0) {
		cPt->h = 0;
		result = false;
	}
	if (cPt->v < 0) {
		cPt->v = 0;
		result = false;
	}
	
	if (cPt->h >= aL->maxX) {
		cPt->h = aL->maxX - 1;
		result = false;
	}
	if (cPt->v >= aL->maxY) {
		cPt->v = aL->maxY - 1;
		result = false;
	}
	
	return result;
}

Boolean PLGetSelect(Point *myPt, PrivateList	*aL)
{
	if(	myPt->h >= aL->select.left && myPt->h <= aL->select.right &&
		myPt->v >= aL->select.top && myPt->v <= aL->select.bottom) {
		return true;
	} else {
		if (myPt->v < aL->select.top) {
			myPt->v = aL->select.top;
			myPt->h = aL->select.left;
		} else if (myPt->v > aL->select.bottom) {
			return false;
		} else {
			myPt->h = aL->select.left;
			if (myPt->h > aL->select.right)
				myPt->v++;
		}
	}
	
	return true;
}

void PLGetSelectRect(Rect *dstRect, PrivateList	*aL)
{
	long temp;
	
	if (aL->select.left == -1 && aL->select.top == -1 && aL->select.right == -1 && aL->select.bottom == -1) {
		SetRect(dstRect, 0, 0 ,0 ,0);
		return;
	}
	
	dstRect->left = aL->rect.left + (aL->select.left - PLGetCtlValue(aL->xScroll)) * aL->LCell;
	dstRect->right = dstRect->left + (aL->select.right - aL->select.left + 1) * aL->LCell;
	
	
	temp = aL->rect.top + (aL->select.top - PLGetCtlValue(aL->yScroll)) * aL->HCell;
	if (temp < -32000)
		temp = -32000;
	dstRect->top = temp;
	temp = dstRect->top + (aL->select.bottom - aL->select.top + 1) * aL->HCell;
	if (temp > 32000)
		temp = 32000;
	dstRect->bottom = temp;

}

void PLSetSelect(short left, short top, short right, short bottom, PrivateList *aL)
{
	Rect			tt, oldRect;
	
	PLGetSelectRect(&oldRect, aL);		oldRect.left++;
	
	/* Erase previous selection */
	if ((aL->type & onlyOne) != 0 && aL->select.left != -1) // || aL->type == dragItem
	{
		if (aL->select.top == aL->select.bottom) {
			Point aa;
			
			aa.h = aL->select.left;
			aa.v = aL->select.top;
			
			SetRect(&aL->select, -1, -1, -1, -1);
			
			PLUpdateItem(aa, aL);
		}
	}
	
	if (left == -1 && top == -1 && right == -1 && bottom == -1) {
		PLGetSelectRect(&tt, aL);
		InvalWindowRect(GetDialogWindow(aL->aDia), &tt);
		
		SetRect(&aL->select, left, top, right, bottom);
		
		return;
	}
	
	if (top < 0)
		top = 0;
	if (top >= aL->maxY)
		top = aL->maxY-1;
	if (bottom < 0)
		bottom = 0;
	if (bottom >= aL->maxY)
		bottom = aL->maxY-1;
	
	if (left < 0)
		left = 0;
	if (left >= aL->maxX)
		left = aL->maxX-1;
	if (right < 0)
		right = 0;
	if (right >= aL->maxX)
		right = aL->maxX-1;
	
	SetRect(&aL->select, left, top, right, bottom);
	
	if((aL->type & onlyOne) != 0) {	//aL->type == dragItem
		Point aa;
		
		aa.h = aL->select.left;
		aa.v = aL->select.top;
		
		PLUpdateItem(aa, aL);
	} else {
		Rect	uRect, tRect;
		
		PLGetSelectRect(&tt, aL);
		InvalWindowRect(GetDialogWindow(aL->aDia), &tt);
		
		if (SectRect(&oldRect, &aL->rect, &uRect))
			InvalWindowRect(GetDialogWindow(aL->aDia), &uRect);
		
		PLGetSelectRect(&tRect, aL);
		tRect.left++;
		if (SectRect(&tRect, &aL->rect, &uRect))
			InvalWindowRect(GetDialogWindow(aL->aDia), &uRect);
		
		if (SectRect(&oldRect, &tRect, &tRect))
			ValidWindowRect(GetDialogWindow(aL->aDia),  &tRect);
	}
}

void PLAutoScroll(PrivateList	*aL)
{
	if (aL->select.left == -1 && aL->select.top == -1 && aL->select.right == -1 && aL->select.bottom == -1) {
		return;
	} else {
		short XDist, YDist;
		
		XDist = aL->rect.left + (aL->select.left - PLGetCtlValue(aL->xScroll)) * aL->LCell;
		YDist = aL->rect.top + (aL->select.top - PLGetCtlValue(aL->yScroll)) * aL->HCell;
		
		if (YDist < aL->rect.top || YDist > aL->rect.bottom - aL->HCell) {
			Rect		aa = aL->rect;
			short	prevVal = GetControlValue(aL->yScroll);
			
			aa.left = 0;
			
			PLUpdate(aL);		// Important: s'il reste des rŽgions ˆ updater !!! L'offset va changer
			
			PLSetCtlValue(aL->yScroll, aL->select.top);
			PLScrollInt(GetControlValue(aL->yScroll), prevVal, GetControlReference(aL->yScroll), aL);
		}
		
		if (XDist < aL->rect.left || XDist > aL->rect.right - aL->LCell) {
			Rect		aa = aL->rect;
			short	prevVal = GetControlValue(aL->xScroll);
			
			aa.left = 0;
			
			PLUpdate(aL);		// Important: s'il reste des rŽgions ˆ updater !!! L'offset va changer
			
			PLSetCtlValue(aL->xScroll, aL->select.left);
			PLScrollInt(GetControlValue(aL->xScroll), prevVal, GetControlReference(aL->xScroll), aL);
		}
	}
}

void PLSetControl(PrivateList *aL)
{
	short	tt;
	
	// XScroll
	if (aL->xScroll != NULL) {
		MyMoveControl(aL->xScroll, aL->rect.left, aL->rect.bottom);
		MySizeControl(aL->xScroll, aL->rect.right + 1 - aL->rect.left, 16);
		
		SetControlMinimum(aL->xScroll, 0);
		
		tt = aL->maxX - (aL->rect.right - aL->rect.left) / aL->LCell;
		if (tt < 0)
			tt = 0;
		SetControlMaximum(aL->xScroll, tt);
		
		if (gUseControlSize) SetControlViewSize(aL->xScroll, (aL->rect.right - aL->rect.left) / aL->LCell);
	}
	
	// YScroll
	if (aL->yScroll != NULL) {
		MyMoveControl(aL->yScroll, aL->rect.right, aL->rect.top - 1);
		MySizeControl(aL->yScroll, 16, aL->rect.bottom - aL->rect.top + 2);
		
		SetControlMinimum(aL->yScroll, 0);
		
		tt = aL->maxY  - (aL->rect.bottom - aL->rect.top) / aL->HCell;
		if (tt < 0)
			tt = 0;
		SetControlMaximum(aL->yScroll, tt);
		
		if (gUseControlSize)
			SetControlViewSize(aL->yScroll, (aL->rect.bottom - aL->rect.top) / aL->HCell);
	}
}

static long 	lastTickCount;
static Point	lastCell;

Boolean PLClick(Point pt, short modifiers, PrivateList *aL)
{
	Rect	tRect, oldRect, uRect;
	Point	cPt, theCell;
	Point	curPt, oldPt;
	
	oldPt.h = aL->select.left;
	oldPt.v = aL->select.top;
	
	cPt = pt;
	if (!PLConvertPoint(&cPt, aL)) {			// Click outside list
		PLSetSelect(-1, -1, -1, -1, aL);
		goto End;
	}
	
	if ((aL->type & dragItem) != 0) {
		PLGetSelectRect(&oldRect, aL);
		if (PtInRect(pt, &oldRect)) {
			curPt = cPt;
			goto End;
		}
	}
	
	if (modifiers & shiftKey) {
		theCell.v = theCell.h = 0;
		
		if (PLGetSelect(&theCell, aL)) {
			if (cPt.v < theCell.v) {
				theCell.v += aL->select.bottom - aL->select.top;
			}
			
			cPt = theCell;
		}
	} else if (aL->select.top != aL->select.bottom) {
		PLGetSelectRect(&tRect, aL);
		InvalWindowRect(GetDialogWindow(aL->aDia), &tRect);
	}
	
	do {
		Boolean	Moved;
		
		Moved = false;
		
		GetMouse(&curPt);
		
		if (!PtInRect(curPt, &aL->rect)) {
			myIntList = aL;
			
			if (curPt.v < aL->rect.top)
				PLactionProcPartition(aL->yScroll, kControlUpButtonPart);
			if (curPt.v > aL->rect.bottom)
				PLactionProcPartition(aL->yScroll, kControlDownButtonPart);
			
			if (curPt.h < aL->rect.left)
				PLactionProcPartition(aL->xScroll, kControlUpButtonPart);
			if (curPt.h > aL->rect.right)
				PLactionProcPartition(aL->xScroll, kControlDownButtonPart);
			
			Moved = true;
		}
		
		PLConvertPoint(&curPt, aL);
		
		if (curPt.v != oldPt.v || curPt.h != oldPt.h || Moved == true) {
			oldPt = curPt;
			
			/* Get old Rect */
			PLGetSelectRect(&oldRect, aL);
			
			/* Set New Selection Rect */
			if ((aL->type & onlyOne) != 0 && aL->select.left != -1) {	//dragItem
				Point aa;
				
				aa.h = aL->select.left;
				aa.v = aL->select.top;
				
				SetRect(&aL->select, -1, -1, -1, -1);
				
				PLUpdateItem(aa, aL);		// old item
			}
			
			if ((aL->type & onlyOne) != 0)
				cPt = curPt;			// dragItem
			SetMobiusRect(&aL->select, cPt.h, cPt.v, curPt.h, curPt.v);
			
			/* Update */
			if ((aL->type & onlyOne) != 0) {	// dragItem
				Point aa;
				
				aa.h = aL->select.left;
				aa.v = aL->select.top;
				
				PLUpdateItem(aa, aL);		// new item
			} else {
				if (SectRect(&oldRect, &aL->rect, &uRect))
					InvalWindowRect(GetDialogWindow(aL->aDia), &uRect);
				
				PLGetSelectRect(&tRect, aL);
				if (SectRect(&tRect, &aL->rect, &uRect))
					InvalWindowRect(GetDialogWindow(aL->aDia), &uRect);
				
				if (SectRect(&oldRect, &tRect, &tRect))
					ValidWindowRect(GetDialogWindow(aL->aDia), &tRect);
				
				PLUpdate(aL);
			}
			
		}
		
		if ((aL->type & dragItem) != 0) goto End;
	} while (Button());
	
End:
	
	if (TickCount() - lastTickCount < GetDblTime()) {
		if (curPt.h == lastCell.h && curPt.v == lastCell.v)
			return true;
	}
	lastTickCount = TickCount();
	lastCell = curPt;
	return false;
}

void PLScroll(Point myPt, PrivateList *aL)
{
	ControlHandle	theCtl = NULL;
	short			ctlPart = 0, bogus;
	
	if (TestControl(aL->yScroll, myPt))
		theCtl = aL->yScroll;
	if (TestControl(aL->xScroll, myPt))
		theCtl = aL->xScroll;
	
	if (theCtl == aL->xScroll || theCtl == aL->yScroll) {
		myIntList = aL;
		
		if (gUseControlSize) {
			ControlActionUPP	MyControlUPP;
			
			MyControlUPP = NewControlActionUPP(PLactionProcPartition);
			myIntList->thumb = PLGetCtlValue(theCtl);
			HandleControlClick(theCtl, myPt, 0, MyControlUPP);
			DisposeControlActionUPP(MyControlUPP);
		} else {
			if (ctlPart == kControlIndicatorPart) {
				bogus = TrackControl(theCtl, myPt, NULL);
				if (bogus != 0) {
					InvalWindowRect(GetDialogWindow(aL->aDia), &aL->rect);
					
					if (GetWRefCon(GetDialogWindow(aL->aDia)) == RefPartition) {
						DrawLeft(NULL, true);
						DrawEditorUp();
						InvalWindowRect(GetDialogWindow(EditorDlog), &aL->rect);
					}
					PLUpdate(aL);
				}
			} else if (ctlPart > 0) {
				ControlActionUPP MyControlUPP;
				
				MyControlUPP = NewControlActionUPP(PLactionProcPartition);
				TrackControl(theCtl, myPt, MyControlUPP);
				DisposeControlActionUPP(MyControlUPP);
				InvalWindowRect(GetDialogWindow(EditorDlog), &aL->rect);
			}
		}
	}
}

void PLDoArrows(short theChar, PrivateList	*aL)
{
	Point	theCell;
	Rect	tRect;
	short	sVal;
	GrafPtr	savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(aL->aDia);
	
	switch (theChar) {
		case 0x0B:	// page up
			PLSetMyIntList(aL);
			PLactionProcPartition(aL->yScroll, kControlPageUpPart);
			break;
			
		case 0x0C:	// page down
			PLSetMyIntList(aL);
			PLactionProcPartition(aL->yScroll, kControlPageDownPart);
			break;
			
		case 0x04:	// end
			PLSetMyIntList(aL);
			sVal = GetControlValue(aL->yScroll);
			
			PLSetCtlValue(aL->yScroll, GetControlMaximum(aL->yScroll));
			PLScrollInt(GetControlMaximum(aL->yScroll), sVal, GetControlReference(aL->yScroll), aL);
			break;
			
		case 0x01:	// home
			PLSetMyIntList(aL);
			sVal = GetControlValue(aL->yScroll);
			
			PLSetCtlValue(aL->yScroll, GetControlMinimum(aL->yScroll));
			PLScrollInt(GetControlMinimum(aL->yScroll), sVal, GetControlReference(aL->yScroll), aL);
			break;
	}
	
	theCell.v = 0;	theCell.h = 0;
	if (PLGetSelect(&theCell, aL)) {
		if (aL->select.top != aL->select.bottom) {
			PLGetSelectRect(&tRect, aL);
			InvalWindowRect(GetDialogWindow(aL->aDia), &tRect);
		}
		
		switch (theChar) {
			case 0x1E:
				if (theCell.v > 0)
				{
					PLAutoScroll(aL);
					
					theCell.v--;
					PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, aL);
					if (theCell.v < PLGetCtlValue(aL->yScroll)) {
						PLSetCtlValue(aL->yScroll, PLGetCtlValue(aL->yScroll)-1);
						PLScrollInt(0, 1, yScrollNum, aL);
					}
				}
				break;
				
			case 0x1F:
				if (theCell.v < aL->maxY -1) {
					theCell.v++;
					PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, aL);
					PLAutoScroll(aL);
				}
				break;
		}
	}
	
	SetPort(savePort);
}
