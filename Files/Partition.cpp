#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include <string.h>
#include "PrivateList.h"

/******** HELP MODULE ********/
enum
{
	Hopen	= 4,
	HLenght	= 6,
	HPat	= 5,
	HParti	= 1,
	HPatt	= 7,
	HRemove	= 11,
	HAdd	= 12
};

static short AHelp[] = {Hopen, HLenght, HPat, HParti, HPatt, HAdd, HRemove};

void DoHelpPartition(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/


#define ON			true
#define OFF			false
#define LISTSIZE	256

#define POSPOS	10
#define POSID	54
#define POSNAME 81

extern	EventRecord				theEvent;
extern	DialogPtr				EditorDlog, ClassicDlog;
extern	RGBColor				theColor;
extern	MenuHandle				thePatternMenu;
extern	WindowPtr				oldWindow;
extern	Cursor					pencilCrsr;

		DialogPtr				PartiDlog;
		
static	PixMapHandle			myPopUp;
static	short					LChar;
static	short					PatCopy2;
static	ControlHandle			InfoBut, OpenBut, FlipBut, AddBut, RemoveBut;
static	PrivateList				myList;

void SetMobiusRect(Rect *rect, short left, short top, short right, short bottom);
void TurnRadio(short, DialogPtr, Boolean);
void UpdatePartitionWindow(DialogPtr);
void SelectCurrentParti(void);
short PicToPix (PicHandle thePic, PixMapHandle *offscreenPixMap);
void ZapPixMap(PixMapHandle *offscreenPixMap);
void UpdatePatternMenu();
void OpenSelectedPattern(short no);

void UpdatePartiInfo(void)
{
	GrafPtr		SavePort;
	Rect		caRect, itemRect;
	
	if (PartiDlog == NULL)
		return;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(PartiDlog);
	
	GetPortBounds(GetDialogPort(PartiDlog), &caRect);
	
	myList.rect.right = caRect.right;
	
	//MySizeWindow(PartiDlog, myList.rect.right, PartiDlog->portRect.bottom, false);
	myList.rect.right -= 15;
	myList.rect.left = 0;
	myList.LCell = myList.rect.right - myList.rect.left;
	PLSetControl(&myList);
	
	UpdatePatternMenu();
	
	//PatCopy2 = -1;
	SelectCurrentParti();
	
	InvalWindowRect(GetDialogWindow(PartiDlog), &myList.rect);
	
	GetPortBounds(GetDialogPort(PartiDlog), &caRect);
	InvalWindowRect(GetDialogWindow(PartiDlog), &caRect);
	
	SetRect(&itemRect, 0, myList.rect.bottom, myList.rect.right, caRect.bottom);
	InvalWindowRect(GetDialogWindow(PartiDlog), &itemRect);
	
	SetPort(SavePort);
}

#if 0
void DialogPatEdit(short thePos)
{
	long		mresult;
	short		whichItem, itemHit, itemType, i, x;
	DialogPtr	TheDia;
	Str255		theStr, aStr;
	GrafPtr		myPort;
	Handle		itemHandle;
	Rect		itemRect;
	Point		myPt;
	
	GetPort(&myPort);
	
	TheDia = GetNewDialog(149, NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	AutoPosition(TheDia);
	
	NumToString((long) curMusic->header->numPointers, theStr);
	SetDText(TheDia, 7, theStr);
	SelectDialogItemText(TheDia, 7, 0, 32767);
	
OnRepart:
	
	do {
		ModalDialog(MyDlgFilterDesc, &itemHit);
	} while (itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1) {
		GetDText (TheDia, 7, (StringPtr) &theStr);
		StringToNum(theStr, &mresult);
		if (mresult < 1 || mresult > 256) {
			Erreur(28, -6);
			SelectDialogItemText(TheDia, 7, 0, 32767);
			goto OnRepart;
		}
		curMusic->header->numPointers = mresult;
		
		UpdatePartiInfo();
		
		curMusic->hasChanged = true;
	}
	
	DisposeDialog(TheDia);
	SetPort(myPort);
}
#endif

void DoGrowParti(DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp;
	short		tempB, tempA;
	Point		aPt = {0, 0};
	BitMap		screenBits;
	
	
	GetPort(&SavePort);
 	SetPortDialogPort(theDialog);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp.left = caRect.right;
	temp.top = myList.rect.top + 70;
	temp.right = caRect.right;
	
	temp.bottom = myList.rect.top + LISTSIZE * myList.HCell;
	temp.bottom += 16;
	
	LocalToGlobal(&aPt);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	if (temp.bottom < temp.top)
		temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v)
		temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown)
		lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);
	
	if (lSizeVH != 0) {
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	} else {
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	MySizeWindow(theDialog, tempA, tempB, true);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp = myList.rect;
	temp.bottom = caRect.bottom;
	EraseRect(&temp);
	InvalWindowRect(GetDialogWindow(theDialog), &temp);
	
	myList.rect.bottom = caRect.bottom - 15;
	
	PLSetControl(&myList);
	
	SetPort(SavePort);
}

void DrawPartiListItem(short iD)
{
	Rect   		tempRect;
	Str255		tempStr, aStr;
	short		pos;
	
	if (iD >= myList.maxY)
		return;
	
	pos = iD - GetControlValue(myList.yScroll);
	
	tempRect.left = myList.rect.left + 30 + POSPOS;
	tempRect.right = tempRect.left + (*myPopUp)->bounds.right;
	
	tempRect.top = myList.rect.top + 2 + myList.HCell*pos;
	tempRect.bottom = tempRect.top + (*myPopUp)->bounds.bottom;
	
	CopyBits((BitMap*)*myPopUp,
			 (BitMap*)*GetPortPixMap(GetDialogPort(PartiDlog)),
			 &(*myPopUp)->bounds,
			 &tempRect,
			 srcCopy,
			 NULL);
	
	/** POSITION **/
	
	NumToString(iD + 1, tempStr);
	
	pStrcpy(aStr, "\p");
	if (iD + 1 < 10) pStrcat(aStr, "\p ");
	if (iD + 1 < 100) pStrcat(aStr, "\p ");
	pStrcat(aStr, tempStr);
	pStrcat(aStr, "\p:");
	
	tempRect.left = myList.rect.left + POSPOS -3;
	tempRect.top = myList.rect.top + myList.HCell*pos;
	tempRect.right = tempRect.left + 30;
	tempRect.bottom = tempRect.top + myList.HCell;
	RGBForeColor(&theColor);
	
	MoveTo(tempRect.left, tempRect.top);
	LineTo(tempRect.left, tempRect.bottom);
	
	MoveTo(tempRect.right, tempRect.top);
	LineTo(tempRect.right, tempRect.bottom);
	
	ForeColor(blackColor);
	
	MoveTo(myList.rect.left + POSPOS, myList.rect.top + 10 + myList.HCell*pos);
	DrawString(aStr);
	
	if (curMusic != NULL) {
		/** LENGTH **/
		if (iD < curMusic->header->numPointers) {
			PixPatHandle	workPixPat;
			PenState		penState;
			
			tempRect.left	= myList.rect.left + 1;
			tempRect.right	= tempRect.left + 5;
			
			tempRect.top	= myList.rect.top + myList.HCell * pos;
			tempRect.bottom	= tempRect.top + myList.HCell;
			
			
			GetPenState(&penState);
			workPixPat = GetPixPat(130);
			PenPixPat(workPixPat);
			
			ForeColor(redColor);
			PaintRect(&tempRect);
			ForeColor(blackColor);
			
			SetPenState(&penState);
		}
		
		/** ID 	**/
		
		NumToString(curMusic->header->oPointers[iD], tempStr);
		
		pStrcpy(aStr, "\p");
		if (curMusic->header->oPointers[iD]< 10) pStrcat(aStr, "\p ");
		if (curMusic->header->oPointers[iD]< 100) pStrcat(aStr, "\p ");
		pStrcat(aStr, tempStr);
		
		MoveTo(myList.rect.left + POSID, myList.rect.top + 10 + myList.HCell*pos);
		DrawString(aStr);
		
		/** NAME **/
		
		strncpy((Ptr) aStr, curMusic->partition[curMusic->header->oPointers[iD]]->header.name, 32);
		MyC2PStr((Ptr) aStr);
		
		MoveTo(myList.rect.left + POSNAME, myList.rect.top + 10 + myList.HCell*pos);
		DrawString(aStr);
		
		if (iD >= myList.select.top && iD <= myList.select.bottom) {
			tempRect.left = myList.rect.left + 6;
			tempRect.right = myList.rect.right;
			
			tempRect.top = myList.rect.top + myList.HCell*pos;
			tempRect.bottom = tempRect.top + myList.HCell;
			
			LMSetHiliteMode((UInt8)(LMGetHiliteMode() & 0x7F));
			InvertRect(&tempRect);
		}
	}
}

void  UpdatePartiWindow(DialogPtr GetSelection)  	/* Pointer to this dialog */
{ 
	Rect   		caRect, tempRect;
	GrafPtr		SavePort;
	Str255		tempStr, aStr;
	short		i;
	RgnHandle	saveClipRgn;
	RgnHandle	visibleRegion;
	
	GetPort(&SavePort);
	SetPortDialogPort(PartiDlog);
	
	BeginUpdate(GetDialogWindow(PartiDlog));
	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion(GetDialogPort(PartiDlog), visibleRegion);
	
	UpdateDialog(PartiDlog, visibleRegion);
	
	DisposeRgn(visibleRegion);
	
	/*********************/
	BackColor(whiteColor);
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	ClipRect(&myList.rect);
	
	ForeColor(whiteColor);
	PaintRect(&myList.rect);
	ForeColor(blackColor);
	
	/**********************/
	
	for (i = GetControlValue(myList.yScroll); i < PLGetMaxYValue(&myList); i++) {
		DrawPartiListItem(i);
	}
	
	RGBBackColor(&theColor);
	
	SetClip(saveClipRgn);
	DisposeRgn(saveClipRgn);
	/*********************/
	
	GetPortBounds(GetDialogPort(GetSelection), &caRect);
	
	MoveTo(0, 25);
	LineTo(caRect.right, 25);
	
	MoveTo(myList.rect.left-1, myList.rect.top - 1);
	LineTo(myList.rect.left-1, myList.rect.bottom);
	
	MoveTo(0, myList.rect.top - 1);
	LineTo(caRect.right, myList.rect.top - 1);
	
	MoveTo(0, myList.rect.bottom);
	LineTo(caRect.right, myList.rect.bottom);
	
	SetRect(&tempRect,
			myList.rect.left,
			myList.rect.bottom + 1,
			caRect.right-15,
			caRect.bottom);
	EraseRect(&tempRect);
	
	MoveTo(7, myList.rect.bottom + 11);
	pStrcpy(tempStr, "\pLength : ");
	NumToString(curMusic->header->numPointers, aStr);
	pStrcat(tempStr, aStr);
	
	DrawString(tempStr);
	
	DrawGrowIconP(GetSelection);
	
	EndUpdate(GetDialogWindow(PartiDlog));
	
	SetPort(SavePort);
} 

void SelectCurrentParti(void)
{
	Cell		theCell;
	GrafPtr		SavePort;
	
	if (PartiDlog == NULL)
		return;
	
	if (PatCopy2 != MADDriver->PL) {
		GetPort(&SavePort);
		SetPortDialogPort(PartiDlog);
		
		PatCopy2 = MADDriver->PL;
		
		theCell.v = theCell.h = 0;
		theCell.v = PatCopy2;
		
		if (!PLGetSelect(&theCell, &myList)) {
			PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			
			PLAutoScroll(&myList);
		}
		
		SetPort(SavePort);
	}
}

//static long lastWhen = 0;

Boolean CheckValidParti(Byte theValue, short thePos)
{
	short		i, numPat, temp;
	
	for (i = 0, numPat = 0; i<256; i++) {
		if (curMusic->header->oPointers[i] >= numPat) {
			numPat = curMusic->header->oPointers[i];
		}
	}
	
	for (i = 0, temp = 0; i<256; i++) {
		if (curMusic->header->oPointers[i] == numPat) {
			numPat = curMusic->header->oPointers[i];
			temp++;
		}
	}
	
	if (theValue < 0 || theValue > numPat) {	// pattern qui n'existe pas
		Erreur(26, -76);
		return false;
	}
#if 0
	else if (temp == 1 && curMusic->header->oPointers[thePos] == numPat) { // la plus haute pattern ne peut pas etre detruite
		Erreur(36, -1);
		return false;
	}
#endif
	else
		return true;
}

void DoNullParti(void)
{
	GrafPtr		SavePort;
	short		itemType;
	Rect		itemRect;
	Point		pt;
	Handle		itemHandle;
	
	if (PartiDlog == NULL)
		return;
	
	GetPort(&SavePort);
	SetPortDialogPort(PartiDlog);
	
	if(GetWRefCon(oldWindow) == RefParti) {
		pt = theEvent.where;
		GlobalToLocal(&pt);
		
		GetDialogItem(PartiDlog, 6, &itemType, &itemHandle, &itemRect);
		
		if (PtInRect(pt, &itemRect)) {
			SetCursor(&pencilCrsr);
		} else
			SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
	
	SelectCurrentParti();
	
	SetPort(SavePort);
}

//static Boolean firstList;

void DoItemPressParti(short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
	Cell	theCell;
	long	mresult;
	short	i, itemType, newPL, newPartitionReader, newPat;
	Rect	caRect, itemRect;
	GrafPtr	SavePort;
	Point	myPt = {0};
	Handle	itemHandle;
	Boolean	DoubleClick;
	
	GetPort(&SavePort);
	SetPortDialogPort(PartiDlog);
	
	if (theEvent.what == mouseDown) { /* See if a mouse click */
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		PLScroll(myPt, &myList);
		
		itemRect = myList.rect;
		itemRect.left = POSPOS;
		
		if (PtInRect(myPt, &itemRect)) {
			if (myPt.h > myList.rect.left + 30 + POSPOS && myPt.h < myList.rect.left + 30 + POSPOS + (*myPopUp)->bounds.right) {
				theCell.v = (myPt.v - myList.rect.top)/myList.HCell;
				theCell.h = 0;
				
				myPt.v = myList.rect.top + theCell.v*myList.HCell;	myPt.h = 2;
				LocalToGlobal(&myPt);
				
				theCell.v += GetControlValue(myList.yScroll);
				
				/***/
				if (theCell.v < 256) {
					InsertMenu(thePatternMenu, hierMenu);
					
					SetItemMark(thePatternMenu, curMusic->header->oPointers[theCell.v] + 1, 0xa5);
					
					mresult = PopUpMenuSelect(thePatternMenu,
											  myPt.v,
											  myPt.h + 10,
											  curMusic->header->oPointers[theCell.v] + 1);
					
					SetItemMark(thePatternMenu, curMusic->header->oPointers[theCell.v] + 1, 0);
					
					if (HiWord(mresult ) != 0 ) {
						SaveUndo(UHeader, 0, "\pUndo 'Partition Modification'");
						
						curMusic->header->oPointers[theCell.v] = (Byte) LoWord(mresult) - 1;
						
						MADDriver->PL = theCell.v;
						MADDriver->Pat = curMusic->header->oPointers[MADDriver->PL];
						MADDriver->PartitionReader = 0;
						MADPurgeTrack(MADDriver);
						
						curMusic->hasChanged = true;
						
						UpdatePartiInfo();
						
						ScanTime();
					}
					DeleteMenu(GetMenuID(thePatternMenu));
				}
			} else {
				DoubleClick = PLClick(myPt, theEvent.modifiers, &myList);
				
				theCell.v = theCell.h = 0;
				
				if (PLGetSelect(&theCell, &myList)) {
				CLICKLIST:
					
					HiliteControl(InfoBut, 0);
					HiliteControl(OpenBut, 0);
					
					if (theCell.v != PatCopy2) {
						PatCopy2 = theCell.v;
						
						newPL = PatCopy2;
						newPat = curMusic->header->oPointers[newPL];
						
						if (newPL >= curMusic->header->numPointers && MADDriver->Reading == true) {
							newPL = 0;
							// MADDriver->musicEnd = true;
						}
						newPartitionReader = 0;
						
						MADDriver->PL = newPL;
						MADDriver->PartitionReader = newPartitionReader;
						MADDriver->Pat = newPat;
						
						MADCheckSpeed(curMusic, MADDriver);
						
						MADPurgeTrack(MADDriver);
					}
				} else {
					HiliteControl(InfoBut, 255);
					HiliteControl(OpenBut, 255);
				}
			}
		}
	}   						/* End of mouseDown */
	
	switch (whichItem) {
		case 10:
			if (GetControlHilite(FlipBut) == 0 && MyTrackControl(FlipBut, theEvent.where, NULL)) {
				EraseGrowIcon(whichDialog);
				
#define SMALLSIZE 94
				
				GetPortBounds(GetDialogPort(whichDialog), &caRect);
				
				if (caRect.right == SMALLSIZE) {
					SetDText(whichDialog, 9, "\pName");
					
					MySizeWindow(whichDialog, 200, caRect.bottom, true);
					SetControlValue(FlipBut, 0);
				} else {
					SetDText(whichDialog, 9, "\p");
					
					MySizeWindow(whichDialog, SMALLSIZE, caRect.bottom, true);
					SetControlValue(FlipBut, 1);
				}
				
				EraseGrowIcon(whichDialog);
				
				UpdatePartiInfo();
				
				SetMaxWindow(caRect.right + 3, 0, whichDialog);
			}
			break;
			
		case 7:
			if (GetControlHilite(InfoBut) == 0 && MyTrackControl(InfoBut, theEvent.where, NULL)) {
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					DialogPatternInfo(curMusic->header->oPointers[theCell.v]);
				}
			}
			break;
			
		case 6:	// Partition length
			/***/
		{
			short before = curMusic->header->numPointers;
			
			SaveUndo(UHeader, 0, "\pUndo 'Partition Length'");
			
			theCell.v = (myPt.v - myList.rect.top)/myList.HCell;
			theCell.v += GetControlValue(myList.yScroll);
			theCell.h = 0;
			
			curMusic->header->numPointers = theCell.v + 1;
			//if (curMusic->header->numPointers > 256) curMusic->header->numPointers = 256;
			
			GetDialogItem(whichDialog, 6, &itemType, &itemHandle, &itemRect);
			SetMobiusRect(&itemRect,
						  itemRect.left,
						  itemRect.top + (before - GetControlValue(myList.yScroll)) * myList.HCell,
						  itemRect.right,
						  itemRect.top + (curMusic->header->numPointers - GetControlValue(myList.yScroll)) * myList.HCell);
			InvalWindowRect(GetDialogWindow(whichDialog), &itemRect);
			GetDialogItem(whichDialog, 6, &itemType, &itemHandle, &itemRect);
			InvalWindowRect(GetDialogWindow(whichDialog), &itemRect);
			
			GetPortBounds(GetDialogPort(whichDialog), &caRect);
			
			SetRect(&itemRect, 0, myList.rect.bottom, myList.rect.right, caRect.bottom);
			InvalWindowRect(GetDialogWindow(whichDialog), &itemRect);
			
			curMusic->hasChanged = true;
			
			ScanTime();
		}
			/***/
			break;
			
		case 4:
			if (GetControlHilite(OpenBut) == 0 && MyTrackControl(OpenBut, theEvent.where, NULL)) {
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					MADDriver->PL = theCell.v;
					OpenSelectedPattern(curMusic->header->oPointers[MADDriver->PL]);
				}
			}
			break;
			
		case 12:
			if (GetControlHilite(AddBut) == 0 && MyTrackControl(AddBut, theEvent.where, NULL)) {
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					for (i = MAXPOINTER - 1; i >= theCell.v + 1; i--) {
						curMusic->header->oPointers[i] = curMusic->header->oPointers[i - 1];
					}
					//curMusic->header->oPointers[theCell.v] = curMusic->header->oPointers[theCell.v-1];
					
					curMusic->header->numPointers++;
					//if (curMusic->header->numPointers > 256) curMusic->header->numPointers = 256;
					
					curMusic->hasChanged = true;
					UpdatePartiInfo();
					ScanTime();
				}
			}
			break;
			
		case 11:
			if (GetControlHilite(RemoveBut) == 0 && MyTrackControl(RemoveBut, theEvent.where, NULL)) {
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					for (i = theCell.v; i < MAXPOINTER - 1; i++) {
						curMusic->header->oPointers[i] = curMusic->header->oPointers[i + 1];
					}
					curMusic->header->oPointers[MAXPOINTER - 1] = 0;
					
					curMusic->header->numPointers--;
					if (curMusic->header->numPointers < 1)
						curMusic->header->numPointers = 1;
					
					curMusic->hasChanged = true;
					UpdatePartiInfo();
					ScanTime();
				}
			}
			break;
	}
	
	SetPort(SavePort);
}

void CreatePartiWindow(void)
{
	Rect		caRect, itemRect;
	Handle		itemHandle;
	short		itemType;
	Point		cSize;
	FontInfo	ThisFontInfo;
	PicHandle	aPict;
	
	if (PartiDlog != NULL) {
		SetWindEtat(GetDialogWindow(PartiDlog));
		return;
	}
	
	SetItemMark(PatternEditMenu, 1, checkMark);
	
	PartiDlog = GetNewDialog(166, NULL, GetDialogWindow(ToolsDlog));
	
	//thePatternMenu = GetMenu(146);
	UpdatePatternMenu();
	
	aPict = GetPicture(140);
	PicToPix(aPict, &myPopUp);
	//ReleaseResource((Handle) aPict);
	
	SetWindEtat(GetDialogWindow(PartiDlog));
	SetPortDialogPort(PartiDlog);
	TextFont(kFontIDMonaco);	TextSize(9);
	
	SetRect(&itemRect, 0, 0, 30, 16);
	myList.yScroll = NewControl(GetDialogWindow(PartiDlog),
								&itemRect,
								"\p.",
								true,
								0,
								0,
								0,
								gScrollBarID,
								0);
	
	myList.xScroll = NULL;
	myList.type = 0;
	myList.maxX = 1;
	myList.maxY = LISTSIZE;
	myList.aDia = PartiDlog;
	SetRect(&myList.select, -1, -1, -1, -1);
	
	GetFontInfo(&ThisFontInfo);
	LChar = ThisFontInfo.widMax;
	myList.HCell = cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading;/* Height of a cell */
	
	GetDialogItem(PartiDlog, 1, &itemType, &itemHandle, &myList.rect);
	
	GetPortBounds(GetDialogPort(PartiDlog), &caRect);
	
	myList.rect.right = caRect.right;
	
	//MySizeWindow(PartiDlog, myList.rect.right, PartiDlog->portRect.bottom, false);
	myList.rect.right -= 15;
	myList.rect.left = 0;
	myList.LCell = myList.rect.right - myList.rect.left;
	
	GetDialogItem(PartiDlog, 7, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	InfoBut = NewControl(GetDialogWindow(PartiDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 149,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(PartiDlog, 4, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	OpenBut = NewControl(GetDialogWindow(PartiDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 159,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(PartiDlog, 12, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	AddBut = NewControl(GetDialogWindow(PartiDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						210,
						kControlBevelButtonNormalBevelProc,
						0);
	GetDialogItem(PartiDlog, 11, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	RemoveBut = NewControl(GetDialogWindow(PartiDlog),
						   &itemRect,
						   "\p",
						   true,
						   0,
						   kControlContentIconSuiteRes,
						   209,
						   kControlBevelButtonNormalBevelProc,
						   0);
	GetDialogItem(PartiDlog, 10, &itemType, &itemHandle, &itemRect);
	FlipBut = NewControl(GetDialogWindow(PartiDlog),
						 &itemRect,
						 "\p",
						 true,
						 1,
						 0,
						 1,
						 kControlTriangleProc,
						 0);
	
	GetPortBounds(GetDialogPort(PartiDlog), &caRect);
	
	if (caRect.right != SMALLSIZE) {
		SetDText(PartiDlog, 9, "\pName");
		SetControlValue(FlipBut, 0);
	} else {
		SetDText(PartiDlog, 9, "\p");
		SetControlValue(FlipBut, 1);
	}
	
	theEvent.what = 0;
	DoGrowParti(PartiDlog);
	ShowWindow(GetDialogWindow(PartiDlog));
	SelectWindow2(GetDialogWindow(PartiDlog));
	
	GetPortBounds(GetDialogPort(PartiDlog), &caRect);
	
	SetMaxWindow(caRect.right + 3, 0,  PartiDlog);
	
	PatCopy2 = -1;
}

void CloseParti(void)
{
	if (PartiDlog != NULL) {
		ZapPixMap(&myPopUp);
		
		DisposeDialog(PartiDlog);
		PartiDlog = NULL;
	}
	
	SetItemMark(PatternEditMenu, 1, noMark);
}

void DoKeyPressParti(short theChar)
{
	GrafPtr		SavePort;
	Point		theCell;
	
	if (PartiDlog == NULL)
		return;
	
	GetPort(&SavePort);
	SetPortDialogPort(PartiDlog);
	
	BackColor(whiteColor);
	
	PLDoArrows(theChar, &myList);
	
	if (theChar == deletekey) {
		theCell.v = theCell.h = 0;
		while (PLGetSelect(&theCell, &myList)) {
			if (CheckValidParti(0, theCell.v))
				curMusic->header->oPointers[theCell.v] = 0;
			theCell.v++;
		}
	}
	
	theCell.v = theCell.h = 0;
	if (PLGetSelect(&theCell, &myList)) {
		if (theCell.v != PatCopy2) {
			MADDriver->PL = theCell.v;
			MADDriver->Pat = curMusic->header->oPointers[MADDriver->PL];
			
			if (MADDriver->PL >= curMusic->header->numPointers) {
				MADDriver->PL = 0;
				//if (MADDriver->Reading) MADDriver->musicEnd = true;
			}
			
			MADDriver->PartitionReader = 0;
			MADPurgeTrack(MADDriver);
		}
		HiliteControl(InfoBut, 0);
		HiliteControl(OpenBut, 0);
	} else {
		HiliteControl(InfoBut, 255);
		HiliteControl(OpenBut, 255);
	}
	RGBBackColor(&theColor);
	
	SetPort(SavePort);
}

void COPYParti(void)
{
	Cell				theCell;
	short				count;
	Handle				theHandle;
	OSErr				anErr;
	ScrapRef			scrap;

	theCell.h = theCell.v = 0;
	
	if (PLGetSelect(&theCell, &myList)) {
		SetCursor(&watchCrsr);
		
		theHandle = NewHandle((myList.select.bottom - myList.select.top) + 1);
		HLock(theHandle);
		
		count = 0;
		theCell.h = theCell.v = 0;
		while (PLGetSelect(&theCell, &myList)) {
			(*theHandle)[count++] = curMusic->header->oPointers[theCell.v];
			theCell.v++;
		}
		
		anErr = ClearCurrentScrap();
		anErr = GetCurrentScrap(&scrap);
		anErr = PutScrapFlavor(scrap, 'PATL', 0, GetHandleSize(theHandle), (Ptr) *theHandle);
		
		HUnlock((Handle)theHandle);
	
		DisposeHandle(theHandle);
	
		SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
}

void PASTEParti(void)
{
	short				count;
	long				lCntOrErr;
	Handle				theHandle;
	Point				theCell;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	OSErr				anErr;
	
	lCntOrErr = 0;
	anErr = GetCurrentScrap(&scrap);
	anErr = GetScrapFlavorFlags(scrap, 'PATL', &flags);
	if (anErr == noErr)
		GetScrapFlavorSize(scrap, 'PATL', &lCntOrErr);
	
	if (lCntOrErr > 0) {
		SetCursor(&watchCrsr);
	
		theHandle = NewHandle(lCntOrErr);
		
		HLock(theHandle);
		GetScrapFlavorData(scrap, 'PATL', &lCntOrErr, *theHandle);
		
		theCell.h = theCell.v = 0;
		if (PLGetSelect(&theCell, &myList))
		{
			SaveUndo(UHeader, 0, "\pUndo 'Paste Partition'");
			
			for (count = 0; count < lCntOrErr ; count++) {
				if (CheckValidParti((*theHandle)[count], theCell.v + count)) {
					curMusic->header->oPointers[theCell.v + count] = (*theHandle)[count];
				}
			}
			
			PLSetSelect(0, theCell.v, 0, theCell.v+lCntOrErr-1, &myList);
		}
		
		HUnlock(theHandle);
		DisposeHandle(theHandle);
		SetCursor(GetQDGlobalsArrow(&qdarrow));
		UpdatePartiInfo();
		
		curMusic->hasChanged = true;
	}
}
