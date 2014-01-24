#include "Undo.h"
#include <Carbon/Carbon.h>
#include "PPPlug.h"
#include "RDriverInt.h"
#include "PrivateList.h"
#include "Shuddup.h"
void UpdateStaffInfo();
void UpdateStaffWindow(DialogPtr GetSelection);

/******** HELP MODULE ********/
enum
{
	HNote	= 5,
	HSelect	= 3,
	HPlay	= 8,
	HLoad	= 7,
	HSave	= 21,
	HInfo	= 22,
	HFilter = 23,
	HPref	= 6,
	HL		= 13,
	HIns	= 10
};

static short AHelp[] =
{HNote, HSelect, HPlay, HLoad, HSave, HInfo, HFilter, HPref, HL, HIns};

void DoHelpStaff(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/

extern	MenuHandle				OsciHMenu;
extern	WindowPtr				oldWindow;
extern	DialogPtr				InstruListDlog;
extern	KeyMap					km;
extern	Boolean					DragManagerUse;
extern	Cursor					PlayCrsr, HandCrsr, beamCrsr, CHandCrsr, ZoomInCrsr, ZoomOutCrsr, NoteCrsr, DelCrsr;
extern	RGBColor				theColor;
extern	short					BlackWhite[ 12];
		DialogPtr				StaffDlog;

static	ControlHandle			prefBut, playBut, noteBut, selecBut, infoBut, saveBut, loadBut, FXBut;
static	ControlHandle			notesIcl[ 6], modifIcl[ 3];	//, durIcl[ 2];
static	short					PatternSizeCopy, curNoteLength, curModif; //, curDur;
static	RGBColor				Gray = {48059, 48059, 48059};

static	ControlHandle			xScroll, yScroll;
static	short					curInstru, CurrentPat, ReaderCopy, CurrentNumChn, XSize, YSize, mode, DiffStartH, DiffStartV;
static	Rect					StaffRect, CurRect;

static	short					startXSelec, endXSelec;
static	short					startYSelec, endYSelec;

static	PicHandle				MainNote;
static	short					demiton[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
static	short					tondemi[ 7] = {0,    2,    4, 5,    7,    9,    11};

static	short 					gThumbPrev;

static	DragSendDataUPP			mySendDataUPP;
static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

static	Boolean					canAcceptDrag;
static	Str255					curDisplayedNote;

static	short					TempsSize;

#define BASETOP		24L
#define INTERLINE	3.5
#define FULLINE		7
#define BEMOLSIZE	8

enum
{
	cPoint  	= 1,
	cLiaison	= 2
};

enum
{
	cNul,
	cUp,
	cDown
};

enum
{
	noteM = 0,
	selecM
};

static CIconHandle silence[5], noteIcn[5], noteinv[5], demitonIcon[3], ptIcon;

void CleanInvert(Rect *a, Rect *b);
pascal void actionProcStaff(ControlHandle theControl, short ctlPart);
void GetNoteString(short note, Str255 string);
pascal OSErr	MyTrackingStaff(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr	MyReceiveDropStaff(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
pascal OSErr 	MySendDataProcEditor(FlavorType theFlavor,  void *refCon, ItemReference theItem, DragReference theDrag);
Boolean 		DragStaff(RgnHandle myRgn, Pcmd	*myPcmd, EventRecord *theEvent);
Cmd* GetCmd(short row, short track, Pcmd* myPcmd);
Pcmd* CreatePcmdFromSelection();
void PasteCmdStaff(Pcmd *myPcmd);
void DrawStaffReader();
void EraseStaffReader();
void UPDATE_TrackActive();



short GetMaxXStaff()
{
	short	ret;

	ret = 1 + GetControlValue(xScroll) + (StaffRect.right - StaffRect.left) / XSize;
	if (ret > curMusic->partition[ CurrentPat]->header.size) ret = curMusic->partition[ CurrentPat]->header.size;
	
	return ret;
}

short GetMaxYStaff()
{
	short	ret;

	ret = 1 + GetControlValue(yScroll) + (StaffRect.bottom - StaffRect.top) / YSize;
	if (ret > curMusic->header->numChn) ret = curMusic->header->numChn;
	
	return ret;
}

void SetStaffControl()
{
	short	tt;
	Rect	caRect;
	
	// XScroll
	if (xScroll != 0) {
		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
		
		MyMoveControl(xScroll, StaffRect.left-1, caRect.bottom-15);
		MySizeControl(xScroll, caRect.right + 2 - StaffRect.left - 15, 16);
		
		SetControlMinimum(xScroll, 0);
		
		tt = curMusic->partition[ CurrentPat]->header.size - ((caRect.right - 14) - StaffRect.left) / XSize;		if (tt < 0) tt = 0;
		SetControlMaximum(xScroll, tt);
		
		if (gUseControlSize) SetControlViewSize(xScroll, ((caRect.right - 14) - StaffRect.left) / XSize);
	}
	
	// YScroll
	if (yScroll != 0) {
		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
		
		MyMoveControl(yScroll, caRect.right-15, BASETOP + 16);
		MySizeControl(yScroll, 16, caRect.bottom - (BASETOP + 16) + 1 - 15);
		
		SetControlMinimum(yScroll, 0);
		
		tt = curMusic->header->numChn  - ((caRect.bottom - 14) - StaffRect.top) / YSize;	if (tt < 0) tt = 0;
		SetControlMaximum(yScroll, tt);
		
		if (gUseControlSize)
			SetControlViewSize(yScroll, ((caRect.bottom - 14) - StaffRect.top) / YSize);
	}
	
	SetMaxWindow(StaffRect.left + curMusic->partition[ CurrentPat]->header.size*XSize + 14,
				 StaffRect.top + curMusic->header->numChn*YSize + 15,
				 StaffDlog);
}

void CreateCurStaffRect()
{
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	StaffRect.right = caRect.right - 15;
	StaffRect.bottom = caRect.bottom - 15;
	
	if (startXSelec == -1 && startYSelec == -1 && endXSelec == -1 && endYSelec == -1)
	{
		SetRect(&CurRect, 0, 0, 0, 0);
	}
	else
	{
		CurRect.top = StaffRect.top + (startYSelec - GetControlValue(yScroll))*YSize;
		CurRect.bottom = StaffRect.top + (endYSelec - GetControlValue(yScroll))*YSize + YSize;
		
		CurRect.left = StaffRect.left + (startXSelec - GetControlValue(xScroll))*XSize;
		CurRect.right = StaffRect.left + (endXSelec - GetControlValue(xScroll))*XSize + XSize;
	}
}

void ConvertPt2Note(Point pt, short *position, short *track, short *note)
{
	short	ptv, temp2;
	float	tempf;
	
	pt.h -= StaffRect.left;
	pt.h /= XSize;
	pt.h += GetControlValue(xScroll);
	
	if (pt.h < 0) pt.h = 0;
	else if (pt.h >= curMusic->partition[ CurrentPat]->header.size) pt.h = curMusic->partition[ CurrentPat]->header.size - 1;
	
	pt.v -= StaffRect.top;
	ptv   = pt.v + GetControlValue(yScroll)*YSize;
	pt.v /= YSize;
	pt.v += GetControlValue(yScroll);
	
	if (pt.v < 0) pt.v = 0;
	else if (pt.v >= curMusic->header->numChn) pt.v = curMusic->header->numChn - 1;
	
	tempf = ptv - pt.v*YSize;
	tempf -= YSize/2;
	tempf -= 3;
	
	tempf = tempf / INTERLINE;
	tempf += 8. * INTERLINE;
	
	temp2 = tempf;
	
	tempf /= 7.;
	
	tempf = 0 + (((short)tempf)*12) - tondemi[ 6 - temp2%7];
	
	if (note != NULL)
	{
		*note = 96 - tempf;
		if (*note < 0) *note = 0;
		if (*note >= 96) *note = 95;
	}
	if (position != NULL) 	*position = pt.h;
	if (track != NULL) 		*track = pt.v;
}

Boolean aNoteStaff(Point pt, short *whichNote)
{
	Cmd		*cmd;
	short	pos, track, note;
	
	ConvertPt2Note(pt, &pos, &track, &note);
	
	if (whichNote != NULL) *whichNote = note;
	
	cmd = GetMADCommand(pos, track, curMusic->partition[ CurrentPat]);
	
	if (cmd->note == note || cmd->note == note+1)
	{
		return true;
	}
	else return false;
}

void DrawNoteCarre(Point *myPt)
{
	short	iType, note;
	Handle	iHandle;
	Rect	iRect;
	Str255	str;
	
	GetDialogItem(StaffDlog, 12, &iType, &iHandle, &iRect);
	iRect.right--;	iRect.top++;
	
	if (myPt == NULL)
	{
		TETextBox(curDisplayedNote+1, curDisplayedNote[ 0], &iRect, teCenter);
	}
	else
	{
		ConvertPt2Note(*myPt, NULL, NULL, &note);
		GetNoteString(note, str);
		
		if (!EqualString(str, curDisplayedNote, true, true))
		{
			pStrcpy(curDisplayedNote, str);
			TETextBox(curDisplayedNote+1, curDisplayedNote[ 0], &iRect, teCenter);
		}
	}
}

void DoNullStaff(void)
{
	Str255		String, aStr;
	GrafPtr		SavePort;
	short		temp;
	Point		myPt;
 	Rect		caRect, tempRect;
 	Boolean		MakeUpdate = false;
	
 	if (StaffDlog == NULL)
		return;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
	
	if (ReaderCopy != MADDriver->PartitionReader) {
		EraseStaffReader();
		
		ReaderCopy = MADDriver->PartitionReader;
		
		if (ReaderCopy < curMusic->partition[ CurrentPat]->header.size) {
			if (ReaderCopy > GetMaxXStaff() || ReaderCopy < GetControlValue(xScroll)) {
				SetControlValue(xScroll, ReaderCopy);
				
				MakeUpdate = true;
			} else
				DrawStaffReader();
		}
	}
	
	if (CurrentPat != MADDriver->Pat || PatternSizeCopy != curMusic->partition[ CurrentPat]->header.size) {
		Str255			theStr;
		short			x;
		
		CurrentPat = MADDriver->Pat;
		
		if (curMusic->partition[ CurrentPat]->header.size != PatternSizeCopy) {
			PatternSizeCopy = curMusic->partition[ CurrentPat]->header.size;
			
			GetPortBounds(GetDialogPort(StaffDlog), &caRect);
			
			SetStaffControl();
		}
		
		/**/
		
		NumToString((long) CurrentPat, String);
		pStrcpy(aStr, "\p");
		if (CurrentPat < 10) pStrcat(aStr, "\p0");
		if (CurrentPat < 100) pStrcat(aStr, "\p0");
		pStrcat(aStr, String);
		
		pStrcpy(String, "\pPattern: ");
		pStrcat(String, aStr);
		
		theStr[ 0] = 20;
		for (x = 0; x < 20; x++) theStr[ x + 1] = curMusic->partition[ CurrentPat]->header.name[ x];
		for (x = 1; x < 20; x++) if (theStr[ x] == 0) { theStr[ 0] = x - 1; break;}
		pStrcat(String, "\p ");
		pStrcat(String, theStr);
		
		SetWTitle(GetDialogWindow(StaffDlog), String);
		
		/**/
		
		MakeUpdate = true;
	}
	
	if (curMusic->header->numChn != CurrentNumChn) MakeUpdate = true;
	
	if (oldWindow == GetDialogWindow(StaffDlog)) {
		RgnHandle	visibleRegion;
		
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetWindowPort(oldWindow), visibleRegion);
		
		if (PtInRgn(myPt, visibleRegion)) {
			if (PtInRect(myPt, &StaffRect)) {
				DrawNoteCarre(&myPt);
				
					temp = (myPt.v - StaffRect.top) / YSize;
				 	//temp = (myPt.v - StaffRect.top) - YSize*temp;
				 if ((myPt.v - StaffRect.top) - YSize*temp == YSize) {
				 SetCursor(&HandCrsr);
				 } else {
					GetKeys(km);
					 if (IsPressed(0x37) && PtInRect(myPt, &CurRect)) {	// On joue la note = PlayCrsr
						SetCursor(&PlayCrsr);
					} else {
						switch (mode) {
							case noteM:
							if (aNoteStaff(myPt, NULL))
								SetCursor(&HandCrsr);
							else
								SetCursor(&NoteCrsr);
							break;
							
							case selecM:
							if (PtInRect(myPt, &CurRect))
								SetCursor(&HandCrsr);
							else
								SetCursor(&beamCrsr);
							break;
						}
					}
				}
			} else {
				short	itemType;
				Handle	itemHandle;
				
				GetDialogItem(StaffDlog, 2, &itemType, &itemHandle, &tempRect);
				tempRect.left = StaffRect.left;
				tempRect.right = StaffRect.right;
				
				if (PtInRect(myPt, &tempRect))
					SetCursor(&PlayCrsr);
				else {
					GetDialogItem(StaffDlog, 4, &itemType, &itemHandle, &tempRect);
					tempRect.left = StaffRect.left - (((*MainNote)->picFrame.right - (*MainNote)->picFrame.left));
					tempRect.right = StaffRect.left;
					tempRect.top = StaffRect.top;
					tempRect.bottom = StaffRect.bottom;
					
					if (PtInRect(myPt, &tempRect))
						SetCursor(&PlayCrsr);
					else
						SetCursor(GetQDGlobalsArrow(&qdarrow));
				}
			}
		} else
			SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		DisposeRgn(visibleRegion);
	}
	
	if (MakeUpdate) {
		UpdateStaffInfo();
		UpdateStaffWindow(StaffDlog);
	}
	
	SetPort(SavePort);
}

void DoGrowStaff(void)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp;
	Point		aPt = { 0, 0};
	short		tempA, tempB;
	BitMap		screenBits;
	
	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
	
	temp.left = 476;
	temp.right = StaffRect.left + curMusic->partition[ CurrentPat]->header.size*XSize + 15;
	temp.top = 110;
	
	temp.bottom = StaffRect.top + curMusic->header->numChn*YSize + 16;
	
	LocalToGlobal(&aPt);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	if (temp.bottom < temp.top) temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(StaffDlog), theEvent.where, &temp);
	
	if (lSizeVH != 0) {
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	} else {
		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
		
		if (caRect.right >= temp.right)
			tempA = temp.right-1;
		else
			tempA = caRect.right;
		
		if (caRect.bottom >= temp.bottom)
			tempB = temp.bottom-1;
		else
			tempB = caRect.bottom;
	}
	
	MySizeWindow(StaffDlog, tempA, tempB , true);
	
	CreateCurStaffRect();
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	EraseRect(&caRect);
	InvalWindowRect(GetDialogWindow(StaffDlog), &caRect);
	
	SetStaffControl();
	
	SetPort(SavePort);
}

void DrawStaffReader()
{
	short			itemType, temp, add=1;
	Rect			fRect;
	Handle			itemHandle;
	char			String[ 200];
	Rect			tRect;
	RgnHandle		savedClip;
	
	if (curMusic->partition[CurrentPat]->header.size > 100)
		temp = 3;
	else
		temp = 2;
	
	savedClip = NewRgn();
 	GetClip(savedClip);
 	
	GetDialogItem(StaffDlog, 2, &itemType, &itemHandle, &fRect);
	
	tRect = fRect;
	tRect.left = StaffRect.left;
	tRect.right = StaffRect.right;
	ClipRect(&tRect);
	
	
	tRect.left	= StaffRect.left + (ReaderCopy - GetControlValue(xScroll)) * XSize + XSize/2 - (add*XSize)/2;
	tRect.right	= tRect.left + (add*XSize);
	
	ForeColor(redColor);
	NTStr(temp, ReaderCopy , (Ptr) String);
	tRect.top++;	tRect.right--;
	
	TETextBox(String, strlen(String), &tRect, teCenter);
	RGBBackColor(&theColor);
	
	SetClip(savedClip);
	DisposeRgn(savedClip);
}

void EraseStaffReader()
{
	short			itemType, temp, add=1;
	Rect			fRect;
	Handle			itemHandle;
	char			String[200];
	Rect			tRect;
	RgnHandle		savedClip;
	
	if (curMusic->partition[CurrentPat]->header.size > 100)
		temp = 3;
	else
		temp = 2;
	
	savedClip = NewRgn();
 	GetClip(savedClip);
 	
	GetDialogItem(StaffDlog, 2, &itemType, &itemHandle, &fRect);
	
	tRect = fRect;
	tRect.left = StaffRect.left;
	tRect.right = StaffRect.right;
	ClipRect(&tRect);
	
	
	tRect.left	= StaffRect.left + (ReaderCopy - GetControlValue(xScroll)) * XSize + XSize/2 - (add*XSize)/2;
	tRect.right	= tRect.left + (add*XSize);
	
	if (thePrefs.UseMarkers) {
		if (ReaderCopy < thePrefs.MarkersOffSet)
			ForeColor(whiteColor);
		else {
			RGBColor	yellC = {65535, 65535, 39321};
			short		tempB;
			
			tempB = (ReaderCopy - thePrefs.MarkersOffSet) / thePrefs.MarkersSize;
			if (tempB % 2 == 0) RGBForeColor(&yellC);
			else ForeColor(whiteColor);
		}
	} else
		ForeColor(whiteColor);
	NTStr(temp, ReaderCopy, (Ptr)String);
	tRect.top++;
	tRect.right--;
	TETextBox(String, strlen(String), &tRect, teCenter);
	RGBBackColor(&theColor);
	
	SetClip(savedClip);
	DisposeRgn(savedClip);
}

void DrawStaffFrame()
{
	short			i, itemType, temp, add = 0;
	Rect			fRect;
	Handle			itemHandle;
	char			String[ 200];
	Rect			caRect, tRect;
	RgnHandle		savedClip;
	
	savedClip = NewRgn();
 	GetClip(savedClip);
 	
 	
 	
	GetDialogItem(StaffDlog, 2, &itemType, &itemHandle, &fRect);
	
	/**/
	
	// X BARRE
	
	MoveTo(0, fRect.top);
	LineTo(StaffRect.right, fRect.top);
	
	MoveTo(0, StaffRect.top);
	LineTo(StaffRect.right, StaffRect.top);
	
	tRect = fRect;
	tRect.right = StaffRect.right;
	ClipRect(&tRect);
	
	if (curMusic->partition[ CurrentPat]->header.size > 100) temp = 3;
	else temp = 2;
	
	if (temp <= 2)
	{
		if (XSize > 8) 			add = 1;
		else if (XSize > 4)		add = 2;
		else 					add = 4;
	}
	else if (temp == 3)
	{
		if (XSize > 16) 		add = 1;
		else if (XSize > 8) 	add = 2;
		else if (XSize > 4)		add = 4;
		else 					add = 8;
	}
	
	tRect.top	= fRect.top + 1;
	tRect.bottom= fRect.bottom;
	
	tRect.left = 0;
	tRect.right = StaffRect.left + XSize/2 - (add*XSize)/2;
	
	BackColor(redColor);
	EraseRect(&tRect);
	RGBBackColor(&theColor);
	MoveTo(tRect.right-1, tRect.top);
	LineTo(tRect.right-1, tRect.bottom);
	
	for (i = GetControlValue(xScroll); i < GetMaxXStaff(); i+= add) {
		tRect.left	= StaffRect.left + (i - GetControlValue(xScroll)) * XSize + XSize/2 - (add*XSize)/2;
		tRect.right	= tRect.left + (add*XSize);
		
		NTStr(temp, i , (Ptr) String);
		tRect.right--;
		
		if (thePrefs.UseMarkers) {
			if (i < thePrefs.MarkersOffSet)
				ForeColor(whiteColor);
			else {
				RGBColor	yellC = {65535, 65535, 39321};
				short		tempB;
				
				tempB = (i - thePrefs.MarkersOffSet) / thePrefs.MarkersSize;
				if (tempB % 2 == 0)
					RGBForeColor(&yellC);
				else
					ForeColor(whiteColor);
			}
		} else
			ForeColor(whiteColor);
		TETextBox(String, strlen(String), &tRect, teCenter);
		
		RGBBackColor(&theColor);
		tRect.right++;
		
		MoveTo(tRect.right-1, tRect.top);
		LineTo(tRect.right-1, tRect.bottom);
		
		if (add != 1) {
			MoveTo(tRect.left + (tRect.right-tRect.left) / 2, tRect.bottom - 1);
			LineTo(tRect.left + (tRect.right-tRect.left) / 2, tRect.bottom);
		}
	}
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	if (tRect.right < caRect.right) {
		tRect.left = tRect.right;
		tRect.right = caRect.right;
		
		EraseRect(&tRect);
	}
	
	/**/
	
	// Y BARRE
	
	tRect = StaffRect;
	tRect.left	= 0;
	tRect.right	= StaffRect.left;
	
	ClipRect(&tRect);
	
	for (i = GetControlValue(yScroll); i < GetMaxYStaff(); i++) {
		Str255		str;
		short		add;
		
		tRect.left	= 0;
		tRect.right	= StaffRect.left - ((*MainNote)->picFrame.right - (*MainNote)->picFrame.left);
		
		tRect.top	= StaffRect.top + 1 + (i - GetControlValue(yScroll)) * YSize;
		tRect.bottom= tRect.top + YSize - 1;
		
		SwitchColor(i);
		
		PaintRect(&tRect);
		
		ForeColor(blackColor);
		NTStr(2, i +1, (Ptr) String);
		
		MoveTo(1, tRect.top + 3 +(tRect.bottom - tRect.top) / 2);
		
		if (MADDriver->Active[i])
			ForeColor(blackColor);
		else
			RGBForeColor(&Gray);
		
		DrawString(MyC2PStr(String));
		
		ForeColor(blackColor);
		
		MoveTo(tRect.right-1, tRect.top);
		LineTo(tRect.right-1, tRect.bottom);
		
		MoveTo(0, tRect.bottom);
		LineTo(StaffRect.left-1, tRect.bottom);
		
		//Draw MainNote
		
		tRect.left = StaffRect.left - ((*MainNote)->picFrame.right - (*MainNote)->picFrame.left);
		tRect.right = StaffRect.left;
		
		ForeColor(whiteColor);
		PaintRect(&tRect);
		ForeColor(blackColor);
		
		tRect.top = tRect.top + (tRect.bottom - tRect.top - ((*MainNote)->picFrame.bottom - (*MainNote)->picFrame.top))/2;
		tRect.bottom = tRect.top + ((*MainNote)->picFrame.bottom - (*MainNote)->picFrame.top);
		
		if (MADDriver->Active[i])
			ForeColor(blackColor);
		else
			RGBForeColor(&Gray);
		
		DrawPicture(MainNote, &tRect);
		
		TextFont(kFontIDHelvetica);
		TextFace(bold);
		TextSize(20);
		
		if (MADDriver->Active[ i]) ForeColor(blackColor);
		else RGBForeColor(&Gray);
		
		
		if (thePrefs.TempsNum > 9 || thePrefs.TempsUnit > 9)
			add = -8;
		else
			add = 0;
		
		MoveTo(tRect.left + 32 + add, tRect.top + 23);
		NumToString(thePrefs.TempsNum, str);
		DrawString(str);
		
		MoveTo(tRect.left + 32 + add, tRect.top + 40);
		NumToString(thePrefs.TempsUnit, str);
		DrawString(str);
		
		MoveTo(tRect.left + 32 + add, tRect.top + 65);
		NumToString(thePrefs.TempsNum, str);
		DrawString(str);
		
		MoveTo(tRect.left + 32 + add, tRect.top + 82);
		NumToString(thePrefs.TempsUnit, str);
		DrawString(str);
		
		ForeColor(blackColor);
		
		TextFont(kFontIDGeneva);
		TextSize(9);
		TextFace(0);
		
		tRect.top	= StaffRect.top + 1 + (i - GetControlValue(yScroll)) * YSize;
		tRect.bottom= tRect.top + YSize - 1;
	}
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	if (tRect.bottom < caRect.bottom) {
		tRect.top = tRect.bottom+1;
		tRect.bottom = caRect.bottom;
		
		EraseRect(&tRect);
	}	
	
	
	SetClip(savedClip);
	DisposeRgn(savedClip);
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	MoveTo(StaffRect.left-1, StaffRect.top);
	LineTo(StaffRect.left-1, caRect.bottom);
	/**/
	
	DrawStaffReader();
	DrawNoteCarre(NULL);
}

void UpdateStaffInfo()
{
	GrafPtr		SavePort;
	Rect		caRect, cellRect;
	Boolean		ctlUpdate = false;
	Str255		aStr;

	if (StaffDlog == NULL)
		return;
		
	/**/
	
	/**/
	
 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
	
	YSize = thePrefs.TrackHeight;
	TempsSize = (thePrefs.TempsNum * 16) / thePrefs.TempsUnit;
	if (TempsSize < 1)
		TempsSize = 1;
	
	SetStaffControl();
	CreateCurStaffRect();
	
	ReaderCopy = MADDriver->PartitionReader;		CurrentPat = MADDriver->Pat;
	
	if (curMusic->header->numChn != CurrentNumChn) {
		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
		
		EraseRect(&caRect);
		InvalWindowRect(GetDialogWindow(StaffDlog), &caRect);
		CurrentNumChn = curMusic->header->numChn;
		ctlUpdate = true;
	}
	
	if (ctlUpdate)
		SetStaffControl();
	
	CreateCurStaffRect();
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	cellRect.left = 0;
	cellRect.top = StaffRect.top - 14;
	cellRect.right = caRect.right;
	cellRect.bottom = caRect.bottom;
	InvalWindowRect(GetDialogWindow(StaffDlog), &cellRect);
	
	strcpy((Ptr)aStr, curMusic->fid[curInstru-1].name);
	MyC2PStr((Ptr)aStr);
	SetDText(StaffDlog, 24, aStr);
	
	SetPort(SavePort);
}

void UpdateStaffNote(short track)
{
	GrafPtr		SavePort;
	Rect		aR;

	if (StaffDlog == NULL)
		return;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
	
	aR.left		= StaffRect.left;
	aR.right 	= StaffRect.right;
	aR.top		= StaffRect.top + 1 + (track - GetControlValue(yScroll)) * YSize;
	aR.bottom	= aR.top + YSize;
	
	if (SectRect(&aR, &StaffRect, &aR))
		InvalWindowRect(GetDialogWindow(StaffDlog), &aR);
	
	SetPort(SavePort);
}

short Note2PosStaff(short note)
{
	short temp2, temp;

	temp2 = note/12;
	temp = note%12;
	temp = demiton[ temp];
	
	return YSize / 2 + 3 + INTERLINE * (4 * FULLINE - (temp2 * FULLINE + temp) + 2);
}

void DrawStaffNotes()
{
	RgnHandle	saveClipRgn;
	short		i, x, z, prevNote = -1, demitonID = 0, tr, startTT, endTT;
	Rect		tRect;
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	
	for (x = GetControlValue(yScroll); x < GetMaxYStaff(); x++) {
		tRect = StaffRect;
		
		tRect.top		= StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
		tRect.bottom	= tRect.top + YSize;
		if (tRect.bottom > StaffRect.bottom) tRect.bottom = StaffRect.bottom;
		
		ClipRect(&tRect);
		
		ForeColor(whiteColor);
		PaintRect(&tRect);
		ForeColor(blackColor);
		
		// Continue les lignes porteuses
		tRect.bottom	= tRect.top + YSize;
		MoveTo(tRect.left, tRect.bottom - 1);
		LineTo(tRect.right, tRect.bottom - 1);
		
		/////
		for (i = GetControlValue(xScroll); i < GetMaxXStaff() ; i++) {
			tRect.left	= StaffRect.left + (i - GetControlValue(xScroll)) * XSize;
			
			if (i % TempsSize == 0) {
				MoveTo(tRect.left-1, tRect.top);
				LineTo(tRect.left-1, tRect.bottom);
			}
		}
		/////
		
		tRect.top		= StaffRect.top + (x - GetControlValue(yScroll)) * YSize;
		tRect.top 		+= (tRect.bottom - tRect.top - ((*MainNote)->picFrame.bottom - (*MainNote)->picFrame.top))/2;
		tRect.bottom 	= tRect.top + ((*MainNote)->picFrame.bottom - (*MainNote)->picFrame.top);
		
		for (z = 0; z < 5; z++) {
			MoveTo(StaffRect.left, tRect.top + 9 + z * FULLINE);
			LineTo(StaffRect.right, tRect.top + 9 + z * FULLINE);
		}
		
		for (z = 0; z < 5; z++) {
			MoveTo(StaffRect.left, tRect.top + 16 + 5 * FULLINE + z * FULLINE);
			LineTo(StaffRect.right, tRect.top + 16 + 5*FULLINE + z * FULLINE);
		}
		
		if (thePrefs.StaffShowAllNotes) {
			startTT  = x;
			endTT = x+1;
		} else {
			startTT	= 0;
			endTT	= curMusic->header->numChn;
		}
		
		for (tr = startTT; tr < endTT; tr++) {
			if (tr == x) {
				ForeColor(blackColor);
			} else {
				ForeColor(redColor);
			}
			
			if (MADDriver->Active[tr])
				ForeColor(blackColor);
			else
				RGBForeColor(&Gray);
			
			for (i = 0; i < GetMaxXStaff() ;) {
				Cmd 	*cmd, *note;
				short	startNote, endNote, startScan;
				
				startScan = i;
				
				// Scan pour le debut de la note - NOTE ON
				
				note = NULL;
				for(;; i++) {
					cmd = GetMADCommand(i, tr, curMusic->partition[CurrentPat]);
					
					if (cmd->note != 0xFF && cmd->note != 0xFE) {
						note = cmd;
						break;
					}
					if (i >= curMusic->partition[CurrentPat]->header.size)
						break;
				}
				startNote = i;
				i++;
				
				if (note) {
					for(;; i++) { // Scan pour la fin de la note - NOTE OFF
						cmd = GetMADCommand(i, tr, curMusic->partition[CurrentPat]);
						
						if (cmd->note != 0xFF)
							break;
						if (i >= curMusic->partition[CurrentPat]->header.size)
							break;
					}
					endNote = i;
				}
				
				if (startScan < startNote && startNote >= GetControlValue(xScroll)) {		// Ajout d'un SILENCE
					CIconHandle	silIcon;
					
					short silenceLength = startNote - startScan;
					
					while (silenceLength > 0) {
						short remove, silenceLengthCopy, ttt;
						
						silIcon = NULL;
						
						silenceLengthCopy = silenceLength;
						
						ttt = (startNote - silenceLength + TempsSize)/TempsSize;			// Verifie si passe une barriere
						ttt = ttt * TempsSize;
						if (startScan + silenceLength > ttt)
						{
							silenceLength = ttt - (startNote - silenceLength);
						}
						SilenceEncore :
						
						if (silenceLength > TempsSize)
							silenceLength = TempsSize;
						
						tRect.left = StaffRect.left + BEMOLSIZE/2 + (startNote - silenceLengthCopy - GetControlValue(xScroll)) * XSize;
						
						if (silenceLength >= 16) {
							silIcon = silence[0];
							remove = 16;
						} else if (silenceLength >= 8) {
							silIcon = silence[1];
							remove = 8;
						} else if (silenceLength >= 4) {
							silIcon = silence[2];
							remove = 4;
						} else if (silenceLength >= 2) {
							silIcon = silence[3];
							remove = 2;
						} else if (silenceLength >= 1) {
							silIcon = silence[4];
							remove = 1;
						} else MyDebugStr(__LINE__, __FILE__, "");
						
						tRect.right		= tRect.left + (*silIcon)->iconPMap.bounds.right;
						tRect.top		= StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
						tRect.top 		+= Note2PosStaff(48);
						tRect.bottom	= tRect.top + (*silIcon)->iconPMap.bounds.bottom;
						
						if (silIcon) PlotCIcon(&tRect, silIcon);
						else MyDebugStr(__LINE__, __FILE__, "");
						
						silenceLengthCopy	-= remove;
						silenceLength		-= remove;
						
						if (silenceLength != silenceLengthCopy) {
							if (silenceLength > 0)
								goto SilenceEncore;
						}
						
						silenceLength = silenceLengthCopy;
					}
				}
				
				if (note != NULL && endNote >= GetControlValue(xScroll)) {
					if (note->note != 0xFF && note->note != 0xFE) {
						Boolean			P1;
						CIconHandle		silIcon;
						
						
						short 	silenceLength = endNote - startNote;
						Boolean	first = true;
						
						while (silenceLength > 0) {
							short	prevleft, remove;
							short 	silenceLengthCopy, ttt;
							
							
							silenceLengthCopy = silenceLength;
							
							ttt = (endNote - silenceLength + TempsSize)/TempsSize;			// Verifie si passe une barriere
							ttt = ttt * TempsSize;
							if (startNote + silenceLength > ttt) {
								silenceLength = ttt - (endNote - silenceLength);
							}
						NoteEncore:
							if (silenceLength > TempsSize)
								silenceLength = TempsSize;
							
							silIcon = NULL;
							P1 = false;
							
							tRect.left = StaffRect.left + (endNote - silenceLengthCopy - GetControlValue(xScroll)) * XSize;
							
							if (silenceLength >= 16) {
								silIcon = noteIcn[0];
								remove = 16;
							} else if (silenceLength >= 12) {
								silIcon = noteIcn[1];
								P1 = true;
								remove = 12;
							} else if (silenceLength >=  8) {
								silIcon = noteIcn[1];
								remove = 8;
							} else if (silenceLength >=  6) {
								silIcon = noteIcn[2];
								P1 = true;
								remove = 6;
							} else if (silenceLength >=  4) {
								silIcon = noteIcn[2];
								remove = 4;
							} else if (silenceLength >=  3) {
								silIcon = noteIcn[3];
								P1 = true;
								remove = 3;
							} else if (silenceLength >=  2) {
								silIcon = noteIcn[3];
								remove = 2;
							} else if (silenceLength >=  1) {
								silIcon = noteIcn[4];
								remove = 1;
							} else MyDebugStr(__LINE__, __FILE__, "");
							
							prevleft 		= tRect.right;
							tRect.right		= tRect.left + (*demitonIcon[1])->iconPMap.bounds.right;
							tRect.top		= StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
							tRect.top 		+= Note2PosStaff(note->note);
							tRect.bottom	= tRect.top + (*demitonIcon[ 1])->iconPMap.bounds.bottom;
							
							if (!first) {			// Dessine la liaison
								Rect tempr;
								
								tempr.left 	= prevleft; 	tempr.top = tRect.top;
								tempr.right = tRect.left; 	tempr.bottom = tRect.bottom;
								
								if (tempr.left == tempr.right) {
									tempr.left = prevleft - 10;
									
									tempr.right = tRect.right;
									tempr.top = tRect.top - 10;	tempr.bottom = tRect.bottom - 10;
								}
								
								PenSize(3, 3);
								FrameArc(&tempr, 270, 180);
								PenSize(1, 1);
							} else if (thePrefs.StaffShowLength) {
								RGBColor	rgb;
								Rect		tempr = tRect;
								
								tempr.top		= tempr.bottom - 6;
								tempr.right		= tempr.left - 1 + XSize * (endNote - startNote);
								
								LMGetHiliteRGB(&rgb);
								
								HiliteColor(&theColor);
								BackColor(whiteColor);
								LMSetHiliteMode(LMGetHiliteMode() & hiliteBit);
								InvertRect(&tempr);
								
								HiliteColor(&rgb);
							}
							
							// Diese? Bemol?
							if (BlackWhite[note->note % 12]) {
								if (first) {
									if (prevNote == -1)
										demitonID = 2;
									else if (prevNote <= note->note)
										demitonID = 2;
									else if (prevNote > note->note)
										demitonID = 1;
								}
								
								if (demitonID == 2)
									PlotCIcon(&tRect, demitonIcon[2]);
								else if (demitonID == 1) {
									tRect.top		= StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
									tRect.top		+= Note2PosStaff(note->note+1);
									tRect.bottom	= tRect.top + (*demitonIcon[1])->iconPMap.bounds.bottom;
									
									PlotCIcon(&tRect, demitonIcon[ 1]);
								} else MyDebugStr(__LINE__, __FILE__, "Your are in BIG troubles...");
								
								tRect.left += BEMOLSIZE;
							}
							else tRect.left += BEMOLSIZE / 2;
							
							if (silIcon == NULL) MyDebugStr(__LINE__, __FILE__, "");
							if (ptIcon == NULL) MyDebugStr(__LINE__, __FILE__, "");
							
							tRect.right = tRect.left + (*silIcon)->iconPMap.bounds.right;
							
							if (silIcon)
								PlotCIcon(&tRect, silIcon);
							if (P1)
								PlotCIcon(&tRect, ptIcon);
							
							// Lignes suplemantaires
							
							tRect.left = StaffRect.left + (endNote - silenceLengthCopy - GetControlValue(xScroll)) * XSize;
							tRect.right = tRect.left + XSize;
							
							if (note->note >= 60 && note->note <= 61) {
								short temp = Note2PosStaff(62);
								short	yy = StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
								
								MoveTo(tRect.left, 19 + yy + temp);
								LineTo(tRect.right, 19 + yy + temp);
							}
							
							if (note->note > 70) {
								Boolean		OnOff = true;
								short		yy = StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
								
								for (z = 78; z <  note->note + 3; z ++) {
									if (!BlackWhite[z % 12]) {
										if (OnOff) {
											short temp = Note2PosStaff(z);
											
											//ForeColor(redColor);
											MoveTo(tRect.left, 19 + yy + temp);
											LineTo(tRect.right, 19 + yy + temp);
											//ForeColor(blackColor);
										}
										OnOff = !OnOff;
									}
								}
							}
							
							if (note->note < 41) {
								Boolean		OnOff = true;
								short		yy = StaffRect.top + 1 + (x - GetControlValue(yScroll)) * YSize;
								
								for (z = 41; z > note->note; z --) {
									if (!BlackWhite[z % 12]) {
										if (OnOff) {
											short temp = Note2PosStaff(z);
											
											//ForeColor(redColor);
											MoveTo(tRect.left, 19 + yy + temp);
											LineTo(tRect.right, 19 + yy + temp);
											//ForeColor(blackColor);
										}
										OnOff = !OnOff;
									}
								}
							}
							
							if (first) {				// Affiche le no de l'instrument
								Str255	str;
								Rect	strRect;
								short	width;
								
								NumToString(note->ins, str);
								
								ForeColor(blackColor);
								TextFace(bold);
								
								width = StringWidth(str);
								
								strRect.right = tRect.left + ((tRect.right - tRect.left) + width)/2;
								strRect.left = strRect.right - width;
								strRect.right++;
								strRect.top = tRect.bottom +1;
								strRect.bottom = tRect.bottom + 10;
								
								ForeColor(whiteColor);
								PaintRect(&strRect);
								
								ForeColor(blackColor);
								MoveTo(strRect.left + 1, strRect.bottom - 1);
								DrawString(str);
								TextFace(0);
								
								prevNote = note->note;
							}
							
							first = false;
							
							silenceLengthCopy	-= remove;
							silenceLength		-= remove;
							
							if (silenceLength != silenceLengthCopy) {
								if (silenceLength > 0)
									goto NoteEncore;
							}
							
							silenceLength = silenceLengthCopy;
						}
					}
				}
			}
			ForeColor(blackColor);
		}
	}
	/**/
	
	SetClip(saveClipRgn);
	DisposeRgn(saveClipRgn);
}

void  UpdateStaffWindow(DialogPtr GetSelection)
{
	Rect   		caRect, tempRect;
	GrafPtr		SavePort;
	
	GetPort(&SavePort);
	SetPortDialogPort(StaffDlog);
	
	BeginUpdate(GetDialogWindow(StaffDlog));
	
	DrawDialog(StaffDlog);
	
	GetPortBounds(GetDialogPort(StaffDlog), &caRect);
	
	StaffRect.right = caRect.right - 15;
	StaffRect.bottom = caRect.bottom - 15;
	
	DrawStaffFrame();
	
	DrawStaffNotes();
	
	RgnHandle	saveClipRgn;
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	
	StaffRect.top++;
	ClipRect(&StaffRect);
	StaffRect.top--;
	
	BackColor(whiteColor);
	
	CreateCurStaffRect();
	
	LMSetHiliteMode(LMGetHiliteMode() & hiliteBit);
	InvertRect(&CurRect);
	RGBBackColor(&theColor);
	
	SetClip(saveClipRgn);
	DisposeRgn(saveClipRgn);
	
	tempRect = StaffRect;
	tempRect.left = 0;
	tempRect.top = StaffRect.top + 1 + (curMusic->header->numChn - GetControlValue(yScroll)) * YSize;
	if (SectRect(&tempRect, &StaffRect, &tempRect))
		EraseRect(&tempRect);
	
	DrawGrowIcon(GetDialogWindow(StaffDlog));
	
	EndUpdate(GetDialogWindow(StaffDlog));
	
	SetPort(SavePort);
}

void CalculDiffStartStaff(void)
{
	Point	myPt;
	short	pos, track;
	
	myPt = theEvent.where;
	GlobalToLocal(&myPt);
	
	ConvertPt2Note(myPt, &pos, &track, NULL);

	DiffStartH = startYSelec - track;
	DiffStartV = startXSelec - pos;
}

Pcmd* CreatePcmdFromSelectionStaff(void)
{
	Cmd			*cmd, *cmd2;
	Pcmd		*myPcmd;
	short		X, Y, count;
	
	if (startXSelec == -1) return NULL;
	
	count = (endXSelec - startXSelec + 1) * (endYSelec - startYSelec + 1);
	if (count <= 0) return NULL;
	
	myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + count * sizeof(Cmd));
	if (myPcmd == NULL) {
		MyDebugStr(__LINE__, __FILE__, "Memory WARNING");
		return NULL;
	}
	
	myPcmd->structSize 	= sizeof(Pcmd) + count * sizeof(Cmd);
	myPcmd->tracks 		= endYSelec - startYSelec + 1;
	myPcmd->length 		= endXSelec - startXSelec + 1;
	myPcmd->trackStart 	= startYSelec;
	myPcmd->posStart 	= startXSelec;
	
	for (X = startYSelec; X <= endYSelec; X++)
	{
		for (Y = startXSelec; Y <= endXSelec; Y++)
		{
			cmd = GetMADCommand(Y, X, curMusic->partition[ CurrentPat]);
			cmd2 = GetCmd(Y - startXSelec, X - startYSelec, myPcmd);
			
			*cmd2 = *cmd;
		}
	}
	return myPcmd;
}

Pcmd* CreatePcmdFromNoteStaff(Point myPt)
{
	Cmd			*cmd;
	Pcmd		*myPcmd;
	
	if (aNoteStaff(myPt, NULL))
	{
		short	pos, track;
		
		myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + sizeof(Cmd));
		if (myPcmd == NULL) {
			MyDebugStr(__LINE__, __FILE__, "Memory WARNING");
			return NULL;
		}
		
		ConvertPt2Note(myPt, &pos, &track, NULL);
		
		myPcmd->structSize 	= sizeof(Pcmd) + sizeof(Cmd);
		myPcmd->tracks 		= 1;
		myPcmd->length 		= 1;
		myPcmd->trackStart 	= track;
		myPcmd->posStart 	= pos;
		
		cmd = GetMADCommand(pos, track, curMusic->partition[ CurrentPat]);
		
		myPcmd->myCmd[ 0]	= *cmd;
		
		return myPcmd;
	}
	
	return NULL;
}

void OpenPcmdStaff(FSSpec	*mySpec)
{
	OSErr				iErr;
	long				inOutBytes;
	short				fRefNum;	
	Pcmd				*myPcmd;
	
	if (mySpec == NULL)
	{
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Open Pcmd File'");
		
		iErr = DoStandardOpen(mySpec, "\pPcmd file", 'Pcmd');
		
		if (iErr) return;
	}
	
	if (FSpOpenDF(mySpec, fsCurPerm, &fRefNum) == noErr) {
		GetEOF(fRefNum, &inOutBytes);
		myPcmd = (Pcmd*) MyNewPtr(inOutBytes);
		iErr = FSRead(fRefNum, &inOutBytes, (Ptr) myPcmd);
		
		PasteCmdStaff(myPcmd);
		
		curMusic->hasChanged = true;
		
		iErr = FSCloseFork(fRefNum);
	}
}

void DoItemPressStaff(short whichItem, DialogPtr whichDialog)
{
	short				temp, i;
	GrafPtr				SavePort;
	Boolean				DoubleClick;
	Point				myPt;
	ControlHandle		theControl;
	ControlActionUPP	MyControlUPP;
	
	GetPort(&SavePort);
	SetPortDialogPort(StaffDlog);
	
	if (theEvent.what == mouseDown) {
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		DoubleClick = false;
		
		if (PtInRect(myPt, &StaffRect)) {
			GetKeys(km);
			if (IsPressed(0x37) && PtInRect(myPt, &CurRect)) {	// On joue la note = PlayCrsr
				
			PlaySelection:
				
				MADDriver->PartitionReader = startXSelec;
				SetCommandTrack(startYSelec, -1);
				
				HiliteControl(playBut, kControlButtonPart);
				
				DoRemember();
				
				for (i = 0; i < startYSelec; i++)			MADDriver->TrackReading[ i] = false;
				for (i = endYSelec+1; i < MAXTRACK; i++)	MADDriver->TrackReading[ i] = false;
				
				MADDriver->Reading = true;
				while (Button() == true  && MADDriver->PartitionReader <= endXSelec)  {
					DoGlobalNull();
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				}
				MADDriver->Reading = false;
				while (Button() == true) {
					DoGlobalNull();
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				}
				DoPause();
				
				for (i = 0; i < MAXTRACK; i++)					MADDriver->TrackReading[ i] = true;		// Restore
				
				HiliteControl(playBut, 0);
			} else {
				switch (mode) {
					case selecM:
						if (PtInRect(myPt, &CurRect)) {
							Point		myCPt;
							RgnHandle	tempRgn;
							Pcmd		*myPcmd;
							
							myCPt = myPt;
							while (Button()) {
								GetMouse(&myPt);
								if (myCPt.h != myPt.h || myCPt.v != myPt.v)
									break;
							}
							if (!Button())
								goto newSelec;
							
							tempRgn = NewRgn();
							RectRgn(tempRgn, &CurRect);
							
							myPcmd = CreatePcmdFromSelectionStaff();
							if (myPcmd != NULL) {
								CalculDiffStartStaff();
								
								SetCursor(&CHandCrsr);
								
								DragStaff(tempRgn, myPcmd, &theEvent);
								
								DiffStartV = 0;
								DiffStartH = 0;
								
								MyDisposePtr((Ptr*) &myPcmd);
							}
							
							DisposeRgn(tempRgn);
						}
						else	newSelec:
						{
							Point	curPt, start;
							Rect	prevRect = { -1, -1, -1, -1};
							short	cendXSelec = endXSelec, cstartXSelec = startXSelec, cendYSelec = endYSelec, cstartYSelec = startYSelec;
							
							ConvertPt2Note(myPt, 	&start.h, &start.v, NULL);
							
							if (theEvent.modifiers & shiftKey)
							{
								if (start.h < startXSelec)  start.h		= 	cendXSelec;
								else start.h		= 	cstartXSelec;
								if (start.v < startYSelec)  start.v		= 	cendYSelec;
								else start.v	= 	cstartYSelec;
							}
							
							do {
								Rect		aRect;
								RgnHandle	saveClipRgn;
								
								GetMouse(&curPt);
								
								if (!PtInRect(curPt, &StaffRect)) {
									if (curPt.h > StaffRect.right)
										actionProcStaff(xScroll, kControlDownButtonPart);
									else if (curPt.h < StaffRect.left)
										actionProcStaff(xScroll, kControlUpButtonPart);
									
									if (curPt.v > StaffRect.bottom)
										actionProcStaff(yScroll, kControlDownButtonPart);
									else if (curPt.v < StaffRect.top)
										actionProcStaff(yScroll, kControlUpButtonPart);
								}
								
								ConvertPt2Note(curPt, &aRect.left, &aRect.top, NULL);
								
								SetMobiusRect(&aRect, aRect.left, aRect.top, start.h, start.v);
								
								if (!EqualRect(&aRect, &prevRect)) {
									//RgnHandle	tempRgn = NULL;
									Rect		oldRect;
									
									prevRect = aRect;
									
									startXSelec = aRect.left;	endXSelec = aRect.right;
									startYSelec = aRect.top;	endYSelec = aRect.bottom;
									
									saveClipRgn = NewRgn();
									GetClip(saveClipRgn);
									
									StaffRect.top++;
									ClipRect(&StaffRect);
									StaffRect.top--;
									
									BackColor(whiteColor);
									
									oldRect = CurRect;
									CreateCurStaffRect();
									
									CleanInvert(&oldRect, &CurRect);
									
									RGBBackColor(&theColor);
									
									SetClip(saveClipRgn);
									DisposeRgn(saveClipRgn);
								}
								
								WaitNextEvent(everyEvent, &theEvent, 1, NULL);
								
								if (QDIsPortBuffered(GetDialogPort(whichDialog)))
									QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
								
							} while (Button());
							
							SetDigitalSelection(startYSelec, startXSelec, endYSelec+1, endXSelec+1);
							
							MADDriver->PartitionReader = startXSelec;
							SetCommandTrack(startYSelec, -1);
						}
						break;
						
					case noteM:
						if (aNoteStaff(myPt, NULL))
						{
							Point		myCPt;
							RgnHandle	tempRgn;
							Pcmd		*myPcmd;
							short		pos, track, note;
							
							myCPt = myPt;
							/*	while (Button())
							 {
							 GetMouse(&myPt);
							 if (myCPt.h != myPt.h || myCPt.v != myPt.v) break;
							 }*/
							if (!Button()) goto newSelec;
							
							// Select current note
							
							ConvertPt2Note(myPt, &pos, &track, &note);
							
							InvalWindowRect(GetDialogWindow(whichDialog), &CurRect);
							
							startXSelec = pos;
							endXSelec	= pos;
							
							startYSelec = track;
							endYSelec	= track;
							
							CreateCurStaffRect();
							InvalWindowRect(GetDialogWindow(whichDialog), &CurRect);
							
							//
							
							RectRgn(tempRgn = NewRgn(), &(*noteIcn[ 3])->iconPMap.bounds);
							
							OffsetRgn(tempRgn, myPt.h - 8, myPt.v - 16);
							
							myPcmd = CreatePcmdFromNoteStaff(myPt);
							if (myPcmd != NULL)
							{
								//CalculDiffStartStaff();
								
								DiffStartH = 0;
								DiffStartV = 0;
								
								SetCursor(&CHandCrsr);
								
								DragStaff(tempRgn, myPcmd, &theEvent);
								
								DiffStartV = 0;		DiffStartH = 0;
								
								MyDisposePtr((Ptr*) &myPcmd);
							}
							
							DisposeRgn(tempRgn);
						} else {
							short	pos, track, note, tte;
							Cmd		*cmd;
							
							SaveUndo(UPattern, CurrentPat, "\pUndo 'Add Note in Classic Editor'");
							
							GetMouse(&myPt);
							
							ConvertPt2Note(myPt, &pos, &track, &note);
							
							cmd = GetMADCommand(pos, track, curMusic->partition[ CurrentPat]);
							
							if (curModif == cUp)		note++;
							if (curModif == cDown)		note--;
							
							cmd->note	= note;
							cmd->ins	= curInstru;
							
							UPDATE_NoteBOUCLE(pos, track);
							
							tte = 0;
							//	if (curDur & cPoint) tte = curNoteLength/2;
							
							if (curNoteLength == 0)	// ONLY KEY OFF
							{
								cmd->note = 0xFE;
								cmd->ins = 0;
								UPDATE_NoteBOUCLE(pos, track);
							}
							else
							{
								for (i = 1; i <= curNoteLength; i++)
								{
									if (pos + i < curMusic->partition[ CurrentPat]->header.size)
									{
										cmd = GetMADCommand(pos + i, track, curMusic->partition[ CurrentPat]);
										
										if (i == curNoteLength)
										{
											if (cmd->note == 0xFF)
											{
												cmd->note = 0xFE;
												cmd->ins = 0;
											}
										}
										else
										{
											cmd->note = 0xFF;
											cmd->ins = 0;
										}
										
										UPDATE_NoteBOUCLE(pos + i, track);
									}
								}
							}
							UPDATE_NoteFINISH();
							
							MADDriver->PartitionReader = pos;
							
							SetCommandTrack(track, -1);
						}
						break;
				}
			}
		}
		else
		{
			short	itemType;
			Handle	itemHandle;
			Rect	tempRect;
			
			GetDialogItem(StaffDlog, 2, &itemType, &itemHandle, &tempRect);
			tempRect.left = StaffRect.left;
			tempRect.right = StaffRect.right;
			
			if (PtInRect(myPt, &tempRect))
			{
				Boolean		IsPlay = MusicPlayActive;
				Boolean		IsJump = MADDriver->JumpToNextPattern;
				
				MADDriver->JumpToNextPattern = false;
				
				GetMouse(&myPt);
				myPt.h -= StaffRect.left;
				myPt.h /= XSize;
				myPt.h += GetControlValue(xScroll);
				
				if (myPt.h < 0) myPt.h = 0;
				else if (myPt.h >= curMusic->partition[ CurrentPat]->header.size) myPt.h = curMusic->partition[ CurrentPat]->header.size - 1;
				MADDriver->PartitionReader = myPt.h;
				
				if (IsPlay == false) DoPlay();
				
				while (Button())
				{
					DoGlobalNull();
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				}
				
				MADDriver->JumpToNextPattern = IsJump;
				
				if (IsPlay == false) DoPause();
			}
			
			tempRect.top = StaffRect.top;
			tempRect.bottom = StaffRect.bottom;
			tempRect.left	= 0;
			tempRect.right	= StaffRect.left - ((*MainNote)->picFrame.right - (*MainNote)->picFrame.left);
			if (PtInRect(myPt, &tempRect))
			{
				short 	track, note;
				Point	tPt;
				
				GetMouse(&tPt);
				
				ConvertPt2Note(tPt, NULL, &track, &note);
				
				if ((theEvent.modifiers & cmdKey) != 0)			// Mute
				{
					MADDriver->Active[ track] = !MADDriver->Active[ track];
					
					UPDATE_TrackActive();
				}
				else if ((theEvent.modifiers & optionKey) != 0)	// Solo
				{
					short	noActive;
					
					for (i = 0, noActive = 0; i < curMusic->header->numChn; i++)
					{
						if (MADDriver->Active[ i] == true)
						{
							noActive++;
						}
					}
					
					if (noActive <= 1 && MADDriver->Active[ track] == true)
					{
						for (i = 0, noActive = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = true;
					}
					else
					{
						for (i = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = false;
						MADDriver->Active[ track] = true;
					}
					
					UPDATE_TrackActive();
				}
			}
			
			//	GetDialogItem(StaffDlog, 4, &itemType, &itemHandle, &tempRect);
			tempRect.top = StaffRect.top;
			tempRect.bottom = StaffRect.bottom;
			tempRect.left	= StaffRect.left - ((*MainNote)->picFrame.right - (*MainNote)->picFrame.left);
			tempRect.right	= StaffRect.left;
			
			if (PtInRect(myPt, &tempRect))
			{
				short prevV = 0;
				
				do
				{
					short 	track, note;
					Point	tPt;
					
					GetMouse(&tPt);
					
					ConvertPt2Note(tPt, NULL, &track, &note);
					
					if (prevV != note)
					{
						DoPlayInstruInt(note, curInstru-1, 0, 0, 0xFF, &MADDriver->chan[ track], 0, 0);
						
						DrawNoteCarre(&tPt);
						
						prevV = note;
					}
					
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					
					if (QDIsPortBuffered(GetDialogPort(StaffDlog)))
						QDFlushPortBuffer(GetDialogPort(StaffDlog), NULL);
					
				}while (Button());
				
				MADKeyOFF(MADDriver, -1);
			}
			
			//	ctlPart = FindControl(myPt, GetDialogWindow(whichDialog), &theControl);
			
			theControl = NULL;
			if (TestControl( xScroll, myPt)) theControl = xScroll;
			if (TestControl( yScroll, myPt)) theControl = yScroll;
			
			if (theControl == xScroll || theControl == yScroll)
			{
				MyControlUPP = NewControlActionUPP(actionProcStaff);
				gThumbPrev = GetControlValue(theControl);
				TrackControl(theControl, myPt, MyControlUPP);
				DisposeControlActionUPP(MyControlUPP);
			}
		}
	}   						/* End of mouseDown */
	
	switch(whichItem) {
		case 21:
			if (MyTrackControl(saveBut, theEvent.where, NULL))
				SavePcmdFile(CreatePcmdFromSelectionStaff());
			break;
			
		case 8:
			if (GetControlHilite(loadBut) == 0 && MyTrackControl(loadBut, theEvent.where, NULL))
			{
				OpenPcmdStaff(NULL);
			}
			break;
			
		case 22:
			if (GetControlHilite(infoBut) == 0 && MyTrackControl(infoBut, theEvent.where, NULL))
			{
				DialogPatternInfo(CurrentPat);
			}
			break;
			
		case 6:
			if (GetControlHilite(prefBut) == 0 && MyTrackControl(prefBut, theEvent.where, NULL))
			{
#include "Help.h"
				
				ShowPrefs(TRUECLASSIC);
			}
			break;
			
		case 23:
			if (GetControlHilite(FXBut) == 0 && MyTrackControl(FXBut, theEvent.where, NULL))
			{
				short	itemType;
				Rect	itemRect;
				Handle	itemHandle;
				
				HiliteControl(FXBut, kControlButtonPart);
				
				GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				itemRect.left += 20;
				temp = PressPPDGMenu(&itemRect);
				
				if (temp != -1)
				{
					Pcmd*	myPcmd;
					
					myPcmd = CreatePcmdFromSelectionStaff();
					
					CallPPDGPlugIns(temp-1, myPcmd);
					
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Classic Editor Plug'");
					
					PasteCmdStaff(myPcmd);
					
					MyDisposePtr((Ptr*) &myPcmd);
				}
				
				HiliteControl(FXBut, 0);
			}
			break;
			
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			for (i = 0; i < 6; i++) HiliteControl(notesIcl[ i], 0);
			HiliteControl(notesIcl[ whichItem-13], kControlButtonPart);
			
			switch(whichItem)
		{
			case 13:	curNoteLength = 16;	break;
			case 14:	curNoteLength = 8;	break;
			case 15:	curNoteLength = 4;	break;
			case 16:	curNoteLength = 2;	break;
			case 17:	curNoteLength = 1;	break;
		}
			break;
			
		case 25:	// KEY OFF
			for (i = 0; i < 6; i++) HiliteControl(notesIcl[ i], 0);
			HiliteControl(notesIcl[ 5], kControlButtonPart);
			curNoteLength = 0;
			break;
			
#if 0
		case 21:
		case 22:
			if (GetControlHilite(durIcl[ whichItem-21]) == kControlButtonPart) HiliteControl(durIcl[ whichItem-21], 0);
			else HiliteControl(durIcl[ whichItem-21], kControlButtonPart);
			
			switch(whichItem)
		{
			case 21:	if (curDur & cPoint) 	curDur -= cPoint; 	else curDur += cPoint;		break;
			case 22:	if (curDur & cLiaison)	curDur -= cLiaison; else curDur += cLiaison;	break;
		}
			break;
#endif
			
		case 18:
		case 19:
		case 20:
			for (i = 0; i < 3; i++) HiliteControl(modifIcl[ i], 0);
			HiliteControl(modifIcl[ whichItem-18], kControlButtonPart);
			
			switch(whichItem)
		{
			case 18:	curModif = cNul;	break;
			case 19:	curModif = cDown;	break;
			case 20:	curModif = cUp;	break;
		}
			break;
			
		case 7:
			if (GetControlHilite(playBut) == 0)
			{
				whichItem = -1;
				goto PlaySelection;
			}
			break;
			
		case 3:
		case 5:
			HiliteControl(noteBut, 0);
			HiliteControl(selecBut, 0);
			
			switch(whichItem)
		{
			case 3:		mode = selecM;	HiliteControl(selecBut, kControlButtonPart);	break;
			case 5:		mode = noteM;	HiliteControl(noteBut, kControlButtonPart);	break;
		}
			break;
			
		case 11:
		case 24:
		case 10:
		{
			Str255	aStr;
			long	mresult;
			short 	itemType;
			Handle	itemHandle;
			Rect	itemRect;
			
			InsertMenuItem(InstruMenu, "\pNo Ins", 0);
			
			InsertMenu(InstruMenu, hierMenu );
			GetDialogItem(StaffDlog, 10, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(InstruMenu, curInstru+1 , 0xa5);
			
			mresult = PopUpMenuSelect(	InstruMenu,
									  myPt.v,
									  myPt.h,
									  curInstru+1 );
			
			SetItemMark(InstruMenu, curInstru+1 , 0);
			
			if (HiWord(mresult ) != 0) {
				curInstru = (Byte) LoWord(mresult) - 1;
				
				if (curInstru > 0) {
					NumToString(curInstru, aStr);
					SetDText(StaffDlog, 11, aStr);
					
					NSelectInstruList(curInstru - 1, -1);
					
					strcpy((Ptr) aStr, curMusic->fid[ curInstru-1].name);
					MyC2PStr((Ptr) aStr);
					SetDText(StaffDlog, 24, aStr);
				} else {
					SetDText(StaffDlog, 11, "\p");
					SetDText(StaffDlog, 24, "\pNo Instrument");
				}
				
			}
			DeleteMenu(GetMenuID(InstruMenu));
			DeleteMenuItem(InstruMenu, 1);
		}
			break;
	}
	SetPort(SavePort);
}

pascal void actionProcStaff(ControlHandle theControl, short ctlPart)
{
	long			lRefCon;
	short			maxValue, minValue, curVal, sVal;
	Rect			tRect;

	if (ctlPart <= 0)
		return;

	lRefCon = GetControlReference(theControl);
	maxValue = GetControlMaximum(theControl);
	minValue = GetControlMinimum(theControl);
	curVal = sVal = GetControlValue(theControl);

	switch(ctlPart) {
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
					curVal -= GetMaxYStaff() - GetControlValue(yScroll) - 1;
					break;
					
				case xScrollNum:
					curVal -= GetMaxXStaff() - GetControlValue(xScroll) - 1;
					break;
			}
			
			if (curVal < minValue)
				curVal = minValue;
			break;
		
		case kControlPageDownPart:
			switch (lRefCon) {
				case yScrollNum:
					curVal += GetMaxYStaff() - GetControlValue(yScroll) - 1;
					break;
					
				case xScrollNum:
					curVal += GetMaxXStaff() - GetControlValue(xScroll) - 1;
					break;
			}
			if (curVal > maxValue)
				curVal = maxValue;
			break;
		
		case kControlIndicatorPart:
			sVal = gThumbPrev;
			gThumbPrev = curVal;
			break;
	}
	
	if (sVal != curVal) {
		RgnHandle		aRgn;
		
		SetControlValue(theControl, curVal);
		
		aRgn = NewRgn();
		
		switch(lRefCon) {
			case yScrollNum:
				tRect = StaffRect;
				tRect.left = 0;
				tRect.top++;
				ScrollRect(&tRect, 0, (sVal - curVal) *YSize, aRgn);
				
				CreateCurStaffRect();
				InvalWindowRgn(GetDialogWindow(StaffDlog), aRgn);
				UpdateStaffWindow(StaffDlog);
				break;
			
			case xScrollNum:
				StaffRect.top -= 14;
				ScrollRect(&StaffRect, (sVal - curVal) *XSize, 0, aRgn);
				StaffRect.top += 14;
				
				CreateCurStaffRect();
				InvalWindowRgn(GetDialogWindow(StaffDlog), aRgn);
				UpdateStaffWindow(StaffDlog);
				break;
		}
		DisposeRgn(aRgn);
	}
}

void CreateStaffWindow(void)
{
	Rect			itemRect;
	Handle			itemHandle;
	short			itemType, i;
	Str255			aStr;
	
	if (!DragManagerUse) {
		Erreur(89, 89);
		return;
	}
	
	if (StaffDlog != NULL) {
		SetWindEtat(GetDialogWindow(StaffDlog));
		return;
	}
	
	SetItemMark(EditorMenu, 3, checkMark);
	
	StaffDlog = GetNewDialog(134, NULL, GetDialogWindow(ToolsDlog));
	SetWindEtat(GetDialogWindow(StaffDlog));
	
	SetPortDialogPort(StaffDlog);
	
	pStrcpy(curDisplayedNote, "\p");
	
	/**/
	
	curInstru = 1;
	
	MainNote = GetPicture(500);
	
	for (i = 0; i < 5; i++) {
		GetDialogItem(StaffDlog, 13 + i, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		notesIcl[i] = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
								 0, kControlContentIconSuiteRes, 800 + i,
								 kControlBevelButtonNormalBevelProc, 0);
	}
	curNoteLength = 16;
	HiliteControl(notesIcl[0], kControlButtonPart);
	
	GetDialogItem(StaffDlog , 25, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	notesIcl[5] = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
							 0, kControlContentIconSuiteRes, 805,
							 kControlBevelButtonNormalBevelProc, 0);
	
	for (i = 0; i < 3; i++) {
		GetDialogItem(StaffDlog , 18 + i, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		modifIcl[i] = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
								  0, kControlContentIconSuiteRes, 900 + i,
								  kControlBevelButtonNormalBevelProc, 0);
	}
	curModif = cNul;
	HiliteControl(modifIcl[0], kControlButtonPart);
	
#if 0
	for (i = 0; i < 2; i++) {
		GetDialogItem(StaffDlog , 21+i, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		durIcl[i] = NewControl(StaffDlog,
								&itemRect,
								"\p.",
								true,
								903 + i,		//icl8 id - AZ List
								-32768,
								32767,
								512,
								0);
	}
	curDur = 0;
#endif
	
	GetDialogItem(StaffDlog, 3, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	selecBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						  0, kControlContentIconSuiteRes, 176,
						  kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 6, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	prefBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 174, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 8, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	loadBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 152, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 21, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	saveBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 151, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 22, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	infoBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 149, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 23, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	FXBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
					   0, kControlContentIconSuiteRes, 185, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 7, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	playBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 160, kControlBevelButtonNormalBevelProc, 0);
	
	GetDialogItem(StaffDlog, 5, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	noteBut = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p", true,
						 0, kControlContentIconSuiteRes, 140, kControlBevelButtonNormalBevelProc, 0);
	
#if 0
	GetDialogItem(StaffDlog, 6, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	zoomBut = NewControl(StaffDlog,
						 &itemRect,
						 "\p.",
						 true,
						 177,		//icl8 id - AZ List
						 -32768,
						 32767,
						 512,
						 0);
#endif
	
	/**/
	
	
	mode = noteM;
	HiliteControl(noteBut, kControlButtonPart);
	
	SetRect(&itemRect, 0, 0, 30, 16);
	xScroll = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p.", true, 0, 0, 0, gScrollBarID, xScrollNum);
	
	SetRect(&itemRect, 0, 0, 30, 16);
	yScroll = NewControl(GetDialogWindow(StaffDlog), &itemRect, "\p.", true, 0, 0, 0, gScrollBarID, yScrollNum);
	
	TextFont(kFontIDGeneva);
	TextSize(9);
	
	TempsSize = (thePrefs.TempsNum * 16) / thePrefs.TempsUnit;
	if (TempsSize < 1)
		TempsSize = 1;
	YSize = thePrefs.TrackHeight;
	XSize = BEMOLSIZE + 12;
	GetDialogItem(StaffDlog, 1, &itemType, &itemHandle, &StaffRect);
	
	NumToString(curInstru, aStr);
	SetDText(StaffDlog, 11, aStr);
	strcpy((Ptr)aStr, curMusic->fid[curInstru-1].name);
	MyC2PStr((Ptr)aStr);
	SetDText(StaffDlog, 24, aStr);
	
	ReaderCopy = MADDriver->PartitionReader;
	CurrentPat = MADDriver->Pat;
	CurrentNumChn = curMusic->header->numChn;
	
	SetStaffControl();
	
	startXSelec = endXSelec = 0;
	startYSelec = endYSelec = 0;
	
	CreateCurStaffRect();
	
	ShowWindow(GetDialogWindow(StaffDlog));
	SelectWindow2(GetDialogWindow(StaffDlog));
	
	ptIcon = GetCIcon(800);
	for (i = 0; i < 5; i++) silence[i] 		= GetCIcon(400+i);
	for (i = 0; i < 5; i++) noteIcn[i] 		= GetCIcon(500+i);
	for (i = 0; i < 5; i++) noteinv[i] 		= GetCIcon(600+i);
	for (i = 0; i < 3; i++) demitonIcon[i] 	= GetCIcon(700+i);
	
	MyTrackingHandlerUPP 	= NewDragTrackingHandlerUPP(MyTrackingStaff);
	MyReceiveDropHandlerUPP = NewDragReceiveHandlerUPP(MyReceiveDropStaff);
	mySendDataUPP 			= NewDragSendDataUPP(MySendDataProcEditor);
	
	InstallTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(StaffDlog), NULL);
	InstallReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(StaffDlog), NULL);
}

void CloseStaff(void)
{
	Point	Start;
	short	i;
	Rect	caRect;
	
	if (StaffDlog != NULL) {
		SetPortDialogPort(StaffDlog);
		
		RemoveTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(StaffDlog));
		RemoveReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(StaffDlog));
		
		DisposeDragTrackingHandlerUPP(MyTrackingHandlerUPP);
		DisposeDragReceiveHandlerUPP(MyReceiveDropHandlerUPP);
		DisposeDragSendDataUPP(mySendDataUPP);
		
		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
		
		thePrefs.WinHi[GetWRefCon(GetDialogWindow(StaffDlog))] = caRect.bottom;
		thePrefs.WinLarg[GetWRefCon(GetDialogWindow(StaffDlog))] = caRect.right;
		thePrefs.WinEtat[GetWRefCon(GetDialogWindow(StaffDlog))] = 1;
		
		Start.h = Start.v = 0;
		LocalToGlobal(&Start);
		thePrefs.WinPos[GetWRefCon(GetDialogWindow(StaffDlog))].v = Start.v;
		thePrefs.WinPos[GetWRefCon(GetDialogWindow(StaffDlog))].h = Start.h;
		
		ReleaseResource((Handle)MainNote);
		DisposeHandle((Handle)ptIcon);
		for (i = 0; i < 5; i++) DisposeHandle((Handle)silence[i]);
		for (i = 0; i < 5; i++) DisposeHandle((Handle)noteIcn[i]);
		for (i = 0; i < 5; i++) DisposeHandle((Handle)noteinv[i]);
		for (i = 0; i < 3; i++) DisposeHandle((Handle)demitonIcon[i]);
		
		DisposeDialog(StaffDlog);
	}
	StaffDlog = NULL;
	
	SetItemMark(EditorMenu, 3, noMark);
}

void DoKeyPressStaff(short theChar)
{
	Rect				caRect;
	Cmd					*theCommand;
	GrafPtr				SavePort;
	
 	if (curMusic == NULL) return;
 	
 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
 	
	if (theChar == deletekey) { // Delete
		short	X, Y;

 		SaveUndo(UPattern, CurrentPat, "\pUndo 'Delete Classic Editor'");
 		
		for (X = startYSelec; X <= endYSelec; X++) {
			for (Y = startXSelec; Y <= endXSelec; Y++) {
				theCommand = GetMADCommand(Y, X, curMusic->partition[CurrentPat]);
				MADKillCmd(theCommand);
				
				UPDATE_NoteBOUCLE(Y, X);
			}
		}
		
		UPDATE_NoteFINISH();
 	} else if (theChar == selectAll) {
 		startXSelec = startYSelec = 0;
 		
 		endYSelec = curMusic->header->numChn-1;
 		endXSelec = curMusic->partition[ CurrentPat]->header.size-1;
 		
 		GetPortBounds(GetDialogPort(StaffDlog), &caRect);
 		
 		InvalWindowRect(GetDialogWindow(StaffDlog), &caRect);
 	} else if (theChar ==  9) {
 		HiliteControl(noteBut, 0);
		HiliteControl(selecBut, 0);
		
		mode++;
		if (mode > 1)
			mode = 0;
		switch (mode) {
			case selecM:
				HiliteControl(selecBut, kControlButtonPart);
				break;
				
			case noteM:
				HiliteControl(noteBut, kControlButtonPart);
				break;
		}
 	}
	SetPort(SavePort);
}


////

static	short			PcmdTracks, PcmdLength, DropX, DropY, DropNote;
static	Rect			dragRect;
static	ControlHandle	selectedControl;

pascal OSErr MyTrackingStaff(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)

{	short			result, fRefNum;
	long			textSize, inOutBytes;
	unsigned long	attributes;
	ItemReference	theItem;
	RgnHandle		theRgn;
	Point			theMouse, localMouse;
	Rect			tempRect;
	FlavorFlags     theFlags;
	Pcmd			*myPcmd;
	RgnHandle		saveClip;
	HFSFlavor		myFlavor;
	FInfo			fndrInfo;
	OSErr			iErr;
	
	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag))
		return noErr;
	
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {
		case kDragTrackingEnterHandler:
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
			
			result = GetFlavorFlags(theDrag, theItem, 'Pcmd', &theFlags);
			if (result == noErr) {
				canAcceptDrag = true;
				
				result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
				if (result == noErr) {
					/****************/
					myPcmd = (Pcmd*) MyNewPtr(textSize);
					GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
					PcmdTracks = myPcmd->tracks;
					if (PcmdTracks <= 0)
						PcmdTracks = 1;
					PcmdLength = myPcmd->length;
					if (PcmdLength <= 0)
						PcmdLength = 1;
					MyDisposePtr((Ptr*) &myPcmd);
					/****************/
				} else {
					PcmdTracks = 1;
					PcmdLength = 1;
				}
			} else if (GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags) == noErr) {
				Boolean	targetIsFolder, wasAliased;
				
				GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
				GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
				
				ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
				
				FSpGetFInfo(&myFlavor.fileSpec, &fndrInfo);
				
				switch(fndrInfo.fdType) {
					case 'Pcmd':
						/****************/
						PcmdTracks = 1;
						PcmdLength = 1;
						iErr = FSpOpenDF(&myFlavor.fileSpec, fsCurPerm, &fRefNum);
						if (iErr == noErr) {
							SInt64 tmp;
							FSGetForkSize(fRefNum, &tmp);
							inOutBytes = tmp;
							myPcmd = (Pcmd*)MyNewPtr(inOutBytes);
							FSReadFork(fRefNum, fsAtMark, 0, inOutBytes, myPcmd, NULL);
							PcmdTracks = myPcmd->tracks;
							PcmdLength = myPcmd->length;
							FSCloseFork(fRefNum);
							MyDisposePtr((Ptr*)&myPcmd);
						}
						/****************/
						
						canAcceptDrag = true;
						break;
						
					default:
						canAcceptDrag = false;
						break;
				}
			}
			else canAcceptDrag = false;
			break;
			
		case kDragTrackingEnterWindow:
			
			break;
			
		case kDragTrackingInWindow:
			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);
			
			BackColor(whiteColor);
			
			selectedControl = NULL;
			
			if (attributes & kDragInsideSenderWindow) {
				Rect coRect;
				
				GetControlBounds(saveBut, &coRect);
				if (PtInRect(localMouse, &coRect) && selectedControl == NULL) {
					HiliteControl(saveBut, kControlButtonPart);
					selectedControl = saveBut;
				} else
					HiliteControl(saveBut, 0);
			}
			
			saveClip = NewRgn();
			GetClip(saveClip);
			ClipRect(&StaffRect);
			
			if (PtInRect(localMouse, &StaffRect)) {
				short pos, track, note;
				
				ConvertPt2Note(localMouse, &pos, &track, &note);
				
				pos += DiffStartV;
				track += DiffStartH;
				
				tempRect.top		= StaffRect.top + (track - GetControlValue(yScroll)) * YSize;
				tempRect.bottom		= tempRect.top + PcmdTracks * YSize;
				
				switch (mode) {
					case noteM:
						if (attributes & kDragInsideSenderWindow) {
							tempRect.top	= StaffRect.top + (track - GetControlValue(yScroll)) * YSize;
							tempRect.top 	+= Note2PosStaff(note) + 16;
							
							tempRect.bottom	= tempRect.top + 3;
						}
						break;
				}
				
				tempRect.left = StaffRect.left + (pos - GetControlValue(xScroll)) * XSize;
				tempRect.right = tempRect.left + PcmdLength * XSize;
				
				DropX		= pos;
				DropY		= track;
				if (attributes & kDragInsideSenderWindow)
					DropNote = note;
				else
					DropNote = -1;
				
				if (tempRect.left != dragRect.left || tempRect.top != dragRect.top) {
					RectRgn(theRgn = NewRgn(), &tempRect);
					
					ShowDragHilite(theDrag, theRgn, true);
					DisposeRgn(theRgn);
					
					dragRect = tempRect;
				}
			} else {
				DropX = DropY = DropNote = -1;
				HideDragHilite(theDrag);
			}
			
			RGBBackColor(&theColor);
			
			SetClip(saveClip);
			DisposeRgn(saveClip);
			
			DrawNoteCarre(&localMouse);
			break;
			
		case kDragTrackingLeaveWindow:
			BackColor(whiteColor);
			HideDragHilite(theDrag);
			RGBBackColor(&theColor);
			break;
			
		case kDragTrackingLeaveHandler:
			break;
	}
	
	return noErr;
}

Boolean DragStaff(RgnHandle myRgn, Pcmd	*myPcmd, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	Ptr					myText;
	PromiseHFSFlavor	myNewFile;
	Rect				reRect;
	
	dragRegion = NewRgn();
	
	CopyRgn(myRgn, dragRegion);
	SetPt(&theLoc, 0, 0);
	LocalToGlobal(&theLoc);
	OffsetRgn(dragRegion, theLoc.h, theLoc.v);
	
	NewDrag(&theDrag);
	
	myNewFile.fileType			= 'Pcmd';
	myNewFile.fileCreator		= 'SNPL';
	myNewFile.fdFlags			= 0;
	myNewFile.promisedFlavor	= 'VCT1';
	AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
	AddDragItemFlavor(theDrag, 1, 'VCT1', NULL, 0, 0);
	AddDragItemFlavor(theDrag, 1, 'Pcmd', myPcmd, myPcmd->structSize, 0);
	
	myText = ConvertPcmd2Text(myPcmd);
	AddDragItemFlavor(theDrag, 1, 'TEXT', myText, GetPtrSize(myText), 0);
	MyDisposePtr(& myText);
	
	result = SetDragSendProc(theDrag, mySendDataUPP, NULL);
	
	GetRegionBounds(dragRegion, &reRect);
	
	SetDragItemBounds(theDrag, 1, &reRect);
	
	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);
	
	result = TrackDrag(theDrag, theEvent, dragRegion);
	
	if (result != noErr && result != userCanceledErr)
		return true;
	
	DisposeDrag(theDrag);
	DisposeRgn(dragRegion);
	
	return true;
}

pascal OSErr MyReceiveDropStaff(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	OSErr				result;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				mouseDownModifiers, mouseUpModifiers, movePcmd;
	Pcmd				*myPcmd;
	HFSFlavor			myFlavor;
	
	if (!mainSystemDrag || !canAcceptDrag)
		return dragNotAcceptedErr;
	if (DropX == -1 && DropY == -1 && selectedControl == NULL)
		return dragNotAcceptedErr;
	
	SetPortWindowPort(theWindow);
	
	DiffStartV = 0;		DiffStartH = 0;
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	if (attributes & kDragInsideSenderWindow) {
		if (selectedControl != NULL) {
			HiliteControl(selectedControl, 0);
			
			if (selectedControl == saveBut)
				SavePcmdFile(CreatePcmdFromSelectionStaff());
			//if (selectedControl == poubBut) 	DoKeyPressStaff(8);
			selectedControl = NULL;
			
			return noErr;
		}
	}
	
	movePcmd = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	
	BackColor(whiteColor);
	HideDragHilite(theDrag);
	RGBBackColor(&theColor);
	
	curMusic->hasChanged = true;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
	
	if (result == noErr) {
		myPcmd = (Pcmd*) MyNewPtr(textSize);
		if (myPcmd != NULL) {
			GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
			
			if (movePcmd) {		// Delete source
				// La source est déjà selectionnée...
				
				DoKeyPressStaff(8);
			} else {
				SaveUndo(UPattern, CurrentPat, "\pUndo 'Move Selection'");
				InvalWindowRect( (theWindow), &CurRect);
			}
			
			InvalWindowRect(theWindow, &CurRect);
			
			startXSelec = DropX;
			startYSelec = DropY;
			
			PasteCmdStaff(myPcmd);
			
			MyDisposePtr((Ptr*)&myPcmd);
			
			return noErr;
		} else
			return dragNotAcceptedErr;
	}
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
	if (result == noErr) {
		Boolean		targetIsFolder, wasAliased;
		
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Drop Pcmd File'");
		
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		
		ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		startXSelec = DropX;
		startYSelec = DropY;
		
		OpenPcmdStaff(&myFlavor.fileSpec);
		
		return noErr;
	}
	
	return dragNotAcceptedErr;
}

void PasteCmdStaff(Pcmd *myPcmd)
{
	short			i, x;
	Cmd				*cmd, *cmd2;
	GrafPtr			SavePort;
	
	GetPort(&SavePort);
	SetPortDialogPort(StaffDlog);
	
	InvalWindowRect(GetDialogWindow(StaffDlog), &CurRect);
	
	endXSelec = startXSelec+myPcmd->length - 1;
	endYSelec = startYSelec+myPcmd->tracks - 1;
	
	for (i = 0; i < myPcmd->length; i++) {
		for (x = 0; x < myPcmd->tracks; x++) {
			if(startXSelec + i >= 0 &&
			   startXSelec + i < curMusic->partition[ CurrentPat]->header.size &&
			   startYSelec + x >= 0 &&
			   startYSelec + x < curMusic->header->numChn) {
				cmd = GetMADCommand(startXSelec + i, startYSelec + x, curMusic->partition[ CurrentPat]);
				cmd2 = GetCmd(i, x, myPcmd);
				
				*cmd = *cmd2;
				
				if (DropNote != -1) {
					switch (mode) {
						case noteM:
							cmd->note = DropNote;
							break;
					}
				}
				UPDATE_NoteBOUCLE(startXSelec + i, startYSelec + x);
			}
		}
	}
	
	UPDATE_NoteFINISH();
	
	if (startXSelec < 0)
		startXSelec = 0;
	if (endXSelec >= curMusic->partition[CurrentPat]->header.size)
		endXSelec = curMusic->partition[CurrentPat]->header.size-1;
	
	if (startYSelec < 0)
		startYSelec = 0;
	if (endYSelec >= curMusic->header->numChn)
		endYSelec = curMusic->header->numChn-1;
	
	/*** Update de la région ***/
	
	CreateCurStaffRect();
	InvalWindowRect(GetDialogWindow(StaffDlog), &CurRect);
	
	SetPort(SavePort);
}

void COPYStaff(void)
{
	Pcmd				*myPcmd;
	Ptr					myText;
	GrafPtr				SavePort;
	ScrapRef			scrap;
	OSErr				anErr;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);
	
	myPcmd = CreatePcmdFromSelectionStaff();
	if (myPcmd == NULL) {
		MyDebugStr(__LINE__, __FILE__, "CopyStaff Internal ERROR");
		SetPort(SavePort);
		return;
	}
	
	anErr = ClearCurrentScrap();
	anErr = GetCurrentScrap(&scrap);
	anErr = PutScrapFlavor(scrap, 'Pcmd', 0, GetPtrSize((Ptr) myPcmd), (Ptr) myPcmd);
	
	
	myText = ConvertPcmd2Text(myPcmd);
	anErr = PutScrapFlavor(scrap, 'TEXT', 0, GetPtrSize(myText), myText);
	
	MyDisposePtr(&myText);
	MyDisposePtr((Ptr*) &myPcmd);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	SetPort(SavePort);
}

void PASTEStaff(void)
{
	long			lCntOrErr;
	Handle			theHandle;
	GrafPtr			SavePort;
	Pcmd			*myPcmd;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	OSErr				anErr;

 	GetPort(&SavePort);
 	SetPortDialogPort(StaffDlog);

	lCntOrErr = 0;
	anErr = GetCurrentScrap(&scrap);
	anErr = GetScrapFlavorFlags(scrap, 'Pcmd', &flags);
	if (anErr == noErr) GetScrapFlavorSize(scrap, 'Pcmd', &lCntOrErr);

	if (lCntOrErr > 0)
	{
		SetCursor(&watchCrsr);
	
		theHandle = MyNewHandle(lCntOrErr);
		HLock(theHandle);
		GetScrapFlavorData(scrap, 'Pcmd', &lCntOrErr, *theHandle);
		
		myPcmd = (Pcmd*) (*theHandle);
		
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Paste Classic Editor'");
		
		PasteCmdStaff(myPcmd);
		
		HUnlock(theHandle);
		MyDisposHandle(& theHandle);
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
	
	SetPort(SavePort);
}
