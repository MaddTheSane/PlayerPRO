#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPlug.h"
#include "Undo.h"
#include "Navigation.h"

void UpdateMozartInfoInternal(void);

/******** HELP MODULE ********/
enum
{
	HTrack	= 15,
	HInstru	= 17,
	HEffect	= 20,
	HInfo	= 8,
	HPref	= 7,
	HSee	= 6,
	HNote	= 1,
	HSele	= 2,
	HPlay	= 4,
	HDele	= 3,
	HParti	= 9,
	HZoom	= 5
};

static short AHelp[] =
{HTrack, HInstru, HEffect, HInfo, HPref, HSee, HNote, HSele, HPlay, HDele, HParti, HZoom};

void DoHelpMozart(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/

#define LegendeHi	12
#define LegendeLa	51
#define LegendeLaS	20

enum
{
	NoteM = 1,
	CrossM = 2,
	TrashM = 3,
	PlayM = 4,
	ZoomM = 5,
	SeeAllM = 6,
	PrefM = 7,
	HelpM = 8
};
#define MODESIZE 5

static	short					DiffStartH, DiffStartV, YSize, PatCopy, ReaderCopy, Mode, copyMode, ZoomInter, PianoMozartTrack, PianoMozart, PatternSizeCopy;
static	short					CurrentNumChn;
static	Rect					MozartRect, TrashRect;
static	ControlHandle			c1h, c2h, c3h, NoteBut, ZoomBut, SelBut, PoubBut, HelpBut, prefBut, PlayBut;
static	Cursor					CrossCrsr;
static	Boolean					showAllNotes;
static	Str255					curDisplayedNote;
static	short					MOLargList[ 10] = { 5, 8, 12, 14, 16, 20, 25, 30};
static	short					MOHautList[ 10] = { 5, 8, 10, 10, 12, 20, 25, 30};
static	short 					gThumbPrev;

static	ControlActionUPP		MyControlUPP;

		MenuHandle				TrackView;

static	short					selecTrack, curInstru, curEffect, curArgu, curVol, MOLarg, MOHaut;
static	Point					selecStart, selecEnd;
static	BitMap					xLabel, yLabel;
static	GWorldPtr 				globalGWorld, grilleGWorld;
static	RGBColor				Gray[ 5];
static	RGBColor				yellC, greenC, blueC, redC;

static	DragSendDataUPP			mySendDataUPP;
static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

static	short					MidMOLarg, MidMOHaut;	

extern	Boolean					DragManagerUse;
extern	RGBColor				theColor;
extern	KeyMap					km;
extern	WindowPtr				oldWindow;
extern	Cursor					PlayCrsr, NoteCrsr, DelCrsr, HandCrsr, CHandCrsr, ZoomInCrsr, ZoomOutCrsr;
extern	short					BlackWhite[ 12];

		DialogPtr				MozartDlog;

void		SetUpLabels();
void		ErasePartitionReader();
void		DrawPartitionReader();
void		TraceGrille2(short xTr, short yTr, Rect *itemRect);
void		UpdateMozartWindow(DialogPtr GetSelection);
short		TrackMarquee(Point, Rect*, Rect*);
pascal void	actionProc( ControlHandle,  short);
void 		DoPlayInstruInt(short	Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end);
Ptr			ConvertPcmd2Text(Pcmd *myPcmd);
void		NSelectInstruList(short , short);
void		DoNullMozart(void);
void		PatCopyCheck();
pascal OSErr	MyTrackingBox(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr	MyReceiveDropBox(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
pascal OSErr 	MySendDataProcEditor(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag);
Boolean DragBox(RgnHandle myRgn, Pcmd	*myPcmd, EventRecord *theEvent);
void		UPDATE_TrackActive(void);

Pcmd* CreatePcmdFromSelectionMozart(void);
#define	TICKDELAY 2

short GetMaxYBox()
{
	short	ret;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	ret = 1 + GetControlValue(c3h) + (caRect.bottom - MozartRect.top) / YSize;
	if (ret > curMusic->header->numChn)
		ret = curMusic->header->numChn;
	
	return ret;
}

void TraceGrille2(short xTr, short yTr, Rect *dstRect)
{
	short	i, mini, maxi, temp;
	Rect	tRect, cRect = *dstRect;
	Boolean	OnOff;
	
	/** Markers **/
	if (thePrefs.UseMarkers)
	{
		tRect = *dstRect;
		mini = (dstRect->left	- xTr) / MOLarg;				if (mini < 0)	mini = 0;
		maxi = (dstRect->right - xTr) / MOLarg;		maxi+=2;	if (maxi > curMusic->partition[ PatCopy]->header.size + 1)	maxi = curMusic->partition[ PatCopy]->header.size + 1;
		
		for (i = mini; i< maxi; i ++)
		{
			if (i >= thePrefs.MarkersOffSet)
			{
				temp = (i - thePrefs.MarkersOffSet) / thePrefs.MarkersSize;
				if (temp % 2 == 0)
				{
					RGBForeColor(&yellC);
					tRect.left = xTr + i * MOLarg;
					tRect.right = tRect.left + MOLarg;
					PaintRect(&tRect);
				}
			}
		}
	}
	
	/** Octaves Markers **/
	if (thePrefs.UseOctaveMarkers)
	{
		tRect = *dstRect;
		OnOff = true;
		RGBForeColor(&greenC);
		
		mini = (dstRect->top	- yTr) / MOHaut;				if (mini < 0)	mini = 0;
		maxi = (dstRect->bottom - yTr) / MOHaut;	maxi+=2;	if (maxi > NUMBER_NOTES)	maxi = NUMBER_NOTES;
		
		for (i = 0; i < mini; i++)
		{
			if (i % 12 == 0)
			{
				OnOff = !OnOff;
				if (OnOff) RGBForeColor(&greenC);
				else RGBForeColor(&blueC);
			}
		}
		
		for (i = mini; i < maxi; i ++)
		{
			if (i % 12 == 0)
			{
				OnOff = !OnOff;
				if (OnOff) RGBForeColor(&greenC);
				else RGBForeColor(&blueC);
			}
			
			if (BlackWhite[ (NUMBER_NOTES - i)  % 12])
			{
				tRect.bottom		= yTr + i * MOHaut;
				tRect.top			= tRect.bottom - MOHaut;
				
				PaintRect(&tRect);
			}
		}
	}
	BackColor(whiteColor);
	
	/** Grille **/
	
	RGBForeColor(&Gray[ 3]);
	
	mini = (dstRect->top	- yTr) / MOHaut;				if (mini < 0)	mini = 0;
	maxi = (dstRect->bottom - yTr) / MOHaut;	maxi+=2;	if (maxi > NUMBER_NOTES)	maxi = NUMBER_NOTES;
	
	for (i = mini; i <= maxi; i ++)
	{
		MoveTo(dstRect->left, yTr + i * MOHaut);
		LineTo(dstRect->right,  yTr + i * MOHaut);
	}
	
	mini = (dstRect->left	- xTr) / MOLarg;				if (mini < 0)	mini = 0;
	maxi = (dstRect->right - xTr) / MOLarg;		maxi+=2;	if (maxi > curMusic->partition[ PatCopy]->header.size + 1)	maxi = curMusic->partition[ PatCopy]->header.size + 1;
	
	for (i = mini; i < maxi; i ++)
	{	
		MoveTo(xTr + i * MOLarg, dstRect->top);
		LineTo(xTr + i * MOLarg, dstRect->bottom);
	}
	
	ForeColor(blackColor);
	
	*dstRect = cRect;
}

void SelectGoodMode(void)
{
	HiliteControl(SelBut, 0);	HiliteControl(NoteBut, 0);
	HiliteControl(PlayBut, 0);	HiliteControl(PoubBut, 0);
	HiliteControl(ZoomBut, 0);

	switch (Mode)
	{
		case NoteM:		HiliteControl(NoteBut, kControlButtonPart);	break;			//kControlIndicatorPart
		case CrossM:	HiliteControl(SelBut, kControlButtonPart);	break;
		case TrashM:	HiliteControl(PoubBut, kControlButtonPart);	break;
		case PlayM:		HiliteControl(PlayBut, kControlButtonPart);	break;
		case ZoomM:		HiliteControl(ZoomBut, kControlButtonPart);	break;
	}
}

short GetMozartTrack(void)
{
	if (MozartDlog == NULL) return -1;
	return selecTrack;
}

void GetMozartTrackRect(Rect *r, short t)
{
Rect	caRect;

	r->top = MozartRect.top + (t - GetControlValue(c3h))*YSize;
	r->left = MozartRect.left;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	r->right = MozartRect.right;
	if (r->right > caRect.right-15)
		r->right = caRect.right-15;
	
	r->bottom = r->top + YSize;
	if (r->bottom > caRect.bottom-15)
		r->bottom = caRect.bottom-15;
}

void GetShowVal(short *StartX, short *EndX, short *StartY, short *EndY)
{
	Rect	caRect;

	*StartX = GetControlValue(c2h) / MOLarg;
	if (*StartX > 0) (*StartX)--;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	*EndX = (*StartX) + ((caRect.right - MozartRect.left - 15) / MOLarg);
	(*EndX)+=3;

	if (*EndX > curMusic->partition[ PatCopy]->header.size) *EndX = curMusic->partition[ PatCopy]->header.size;
	
	*StartY = GetControlValue(c1h) / MOHaut;
	*EndY = *StartY + YSize / MOHaut;
	*EndY += 2;
	
	if (*EndY > NUMBER_NOTES) *EndY = NUMBER_NOTES;
}

void ResetSelection(void)
{
	short XE, XS, YE, YS;

	GetShowVal(&XS, &XE, &YS, &YE);

	selecStart.h = selecEnd.h = XS+1;		selecEnd.h--;
	selecStart.v = selecEnd.v = YS+1;		selecEnd.v--;
	selecTrack = 0;
}

short PosToNote3(Point aPt, short	*yAxe, short *track)
{
	short	xTr, yTr, xPos;
	
	// Identifie le track
	
	*track = aPt.v - MozartRect.top;
	*track /= YSize;
	*track += GetControlValue(c3h);
		
	if (*track < 0) *track = 0;
	else if (*track >= curMusic->header->numChn) *track = curMusic->header->numChn - 1;

	// Identifie la pos
	
	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
	
	xPos = aPt.h - MozartRect.left + xTr;
	xPos /= MOLarg;
	if (xPos < 0) xPos = 0;
	else if (xPos > curMusic->partition[ PatCopy]->header.size) xPos = curMusic->partition[ PatCopy]->header.size;
	
	// Identifie la note
	
	if (yAxe != NULL)
	{
		*yAxe = aPt.v - MozartRect.top + yTr - (*track - GetControlValue(c3h))*YSize;
		*yAxe /= MOHaut;
		
		if (*yAxe < 0) *yAxe = 0;
		else if (*yAxe > NUMBER_NOTES) *yAxe = NUMBER_NOTES;
	}
	return xPos;
}

short PosToNote4(Point aPt, short	*yAxe, short track)
{
	short	xTr, yTr, xPos;
	
	// Identifie la pos
	
	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
	
	xPos = aPt.h - MozartRect.left + xTr;
	xPos /= MOLarg;
	if (xPos < 0) xPos = 0;
	else if (xPos > curMusic->partition[ PatCopy]->header.size) xPos = curMusic->partition[ PatCopy]->header.size;
	
	// Identifie la note
	
	if (yAxe != NULL)
	{
		*yAxe = aPt.v - MozartRect.top + yTr - (track - GetControlValue(c3h))*YSize;
		*yAxe /= MOHaut;
		
		if (*yAxe < 0) *yAxe = 0;
		else if (*yAxe > NUMBER_NOTES) *yAxe = NUMBER_NOTES;
	}
	return xPos;
}

void CheckSelecValue(void)
{
	/** X Check **/

	if (selecStart.h < 0) selecStart.h = 0;
	if (selecStart.h >= curMusic->partition[ PatCopy]->header.size) selecStart.h = curMusic->partition[ PatCopy]->header.size - 1;

	if (selecEnd.h < 0) selecEnd.h = 0;
	if (selecEnd.h >= curMusic->partition[ PatCopy]->header.size) selecEnd.h = curMusic->partition[ PatCopy]->header.size - 1;

	/** Y Check **/

	if (selecStart.v < 0) selecStart.v = 0;
	if (selecStart.v >= NUMBER_NOTES) selecStart.v = NUMBER_NOTES-1;

	if (selecEnd.v < 0) selecEnd.v = 0;
	if (selecEnd.v >= NUMBER_NOTES) selecEnd.v = NUMBER_NOTES-1;
}

Point NoteToPos2(Point aNote, short track)
{
	Point	pos;
	
	short	xTr, yTr;

	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
	
	pos.h = aNote.h*MOLarg - xTr  + MozartRect.left;
	pos.v = aNote.v*MOHaut - yTr  + MozartRect.top + (track - GetControlValue(c3h))*YSize;
	
	return pos;
}

void GetSelectionRect2(Rect *selecRect, Boolean ViewOnly)
{
	Point	Pt1, Pt2;
	Rect	viewRect;
	
	Pt1 = NoteToPos2(selecStart, selecTrack);
	
	selecEnd.v++;	selecEnd.h++;
	Pt2 = NoteToPos2(selecEnd, selecTrack);
	selecEnd.v--;	selecEnd.h--;
	
	Pt2.h++;		Pt2.v++;
	Pt2Rect(Pt1, Pt2, selecRect);
	
	if (ViewOnly)
	{
		Rect	caRect;
		
		GetPortBounds(GetDialogPort(MozartDlog), &caRect);
		
		viewRect = MozartRect;
		viewRect.top = MozartRect.top + (selecTrack - GetControlValue(c3h))*YSize;
		viewRect.bottom = viewRect.top + YSize;
		viewRect.right = caRect.right-15;
		if (viewRect.bottom > caRect.bottom -15)
			viewRect.bottom = caRect.bottom -15;
			
		SectRect(&viewRect, selecRect, selecRect);
	}
}

void InvalSelectionRect()
{
	Rect	r;
	
	GetSelectionRect2(&r, true);
	InvalWindowRect(GetDialogWindow(MozartDlog), &r);
}

Boolean PointSelection(Point aPt)
{
	Rect	markee;

	GetSelectionRect2(&markee, true);

	return(PtInRect(aPt, &markee));
}

void MozartHelp(void)
{
	short					itemHit;
	DialogPtr				TheDia;
	GrafPtr					myPort;

	GetPort(&myPort);

	TheDia = GetNewDialog(138,NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);		
	AutoPosition(TheDia);

	do {
		ModalDialog(MyDlgFilterDesc, &itemHit);
	} while (itemHit != 1);

	DisposeDialog(TheDia);
	SetPort(myPort);
}

void DrawCubeNote2(Rect *destRect, short val, short track, Boolean GrayDraw)
{
	Str255		NoteStr;
	
	destRect->left++;	destRect->top++;
	
	if (GrayDraw)
	{
		RGBForeColor(&theColor);
		PaintRect(destRect);
		
		SwitchColor(track);
		MoveTo(destRect->left, destRect->bottom-1);
		LineTo(destRect->right, destRect->bottom-1);
		ForeColor(blackColor);
	}
	else
	{
		SwitchColor(track);
		PaintRect(destRect);
		ForeColor(blackColor);
	}
	
	if (MOLarg > 20)
	{
		NTStr(3, val, (Ptr) NoteStr);
		MyC2PStr((Ptr) NoteStr);
		
		MoveTo(destRect->left + (destRect->right - destRect->left)/2 - StringWidth(NoteStr)/2, destRect->bottom - MidMOHaut - 2);
		DrawString(NoteStr);
	}
	else if (MOLarg > 8)
	{
		if (val < 100)
		{
			NTStr(2, val, (Ptr) NoteStr);
			MyC2PStr((Ptr) NoteStr);
			
			MoveTo(destRect->left + (destRect->right - destRect->left)/2 - StringWidth(NoteStr)/2, destRect->bottom - MidMOHaut - 2);
			DrawString(NoteStr);
		}
	}
	
	destRect->left--;	destRect->top--;
}

void UpdateSelectedRect(Rect *before, Rect *then)
{
	RgnHandle aRgn, bRgn, dRgn;
	
	aRgn = NewRgn();
	bRgn = NewRgn();
	dRgn = NewRgn();
	
	RectRgn(aRgn, before);
	RectRgn(bRgn, then);
	UpdateMozartWindow(MozartDlog);
	
	DiffRgn(aRgn, bRgn, dRgn);
	QDFlushPortBuffer(GetDialogPort(MozartDlog), dRgn);
	
	DiffRgn(bRgn, aRgn, dRgn);
	QDFlushPortBuffer(GetDialogPort(MozartDlog), dRgn);
	
	DisposeRgn(aRgn);
	DisposeRgn(bRgn);
	DisposeRgn(dRgn);
}

void DeleteSelecNote(short start, short end)
{
	short				tt, Pos, xTr, yTr;
	Cmd					*theCommand;
		

	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
		
	for (tt = start; tt <= end; tt++)
	{
		theCommand = GetMADCommand(tt, selecTrack, curMusic->partition[ PatCopy]);

		Pos = NUMBER_NOTES - theCommand->note-1;

		if (tt >= selecStart.h && tt <= selecEnd.h && Pos >= selecStart.v && Pos <= selecEnd.v)
		{
			MADKillCmd(theCommand);
			
			UPDATE_NoteBOUCLE(tt, selecTrack);
		}
	}
	UPDATE_NoteFINISH();
	//UpdateMozartInfoInternal();
}

void SetMobiusRect(Rect *rect, short left, short top, short right, short bottom)
{
	if (left > right) {
		rect->left = right;
		rect->right = left;
	} else {
		rect->left = left;
		rect->right = right;
	}
	if (top > bottom) {
		rect->top = bottom;
		rect->bottom = top;
	} else {
		rect->top = top;
		rect->bottom = bottom;
	}
}

Point topLeft(Rect theRect)
{
	Point toReturn;
	toReturn.h = theRect.top;
	toReturn.v = theRect.left;
	return toReturn;
}

Point botRight(Rect theRect)
{
	Point toReturn;
	toReturn.h = theRect.bottom;
	toReturn.v = theRect.right;
	return toReturn;
}

short TrackMarquee(Point start, Rect *resultRect, Rect *maxRect)
{
	Point		mouse, note, noteStart;
	Rect		nowRect, thenRect, itemRect;
	short		track, starto, endo, temp, oldV;
	
	SetRect(&nowRect, start.h, start.v, start.h, start.v);
	thenRect = nowRect;
	
	noteStart.h = PosToNote3(start, &noteStart.v, &track);
	
	do
	{
		GetMouse(&mouse);
		
		//****** Déplacement de la zone
		GetMozartTrackRect(&itemRect, track);
		
		if (!PtInRect(mouse, &itemRect))
		{
			if (mouse.h > itemRect.right) actionProc(c2h, kControlDownButtonPart);
			else if (mouse.h < itemRect.left) actionProc(c2h, kControlUpButtonPart);
			
			if (mouse.v > itemRect.bottom) actionProc(c1h, kControlDownButtonPart);
			else if (mouse.v < itemRect.top) actionProc(c1h, kControlUpButtonPart);
			
			GetSelectionRect2(&thenRect, false);	// thenRect contient la selection actuelle
		}
		// -- //
		
		note.h = PosToNote4(mouse, &note.v, track);
		mouse = NoteToPos2(note, track);
		start = NoteToPos2(noteStart, track);
		
		SetMobiusRect(&nowRect, start.h, start.v, mouse.h, mouse.v);
		nowRect.right ++;
		nowRect.bottom++;
		
		if (!EqualRect(&nowRect, &thenRect))
		{
			/***** PREPARE L'UPDATE DES NOTES *******/
			
			starto	= PosToNote4(topLeft(thenRect), NULL, track);
			endo	= PosToNote4(topLeft(nowRect), NULL, track);
			
			if (starto == endo)
			{
				starto	= PosToNote4(botRight(thenRect), NULL, track);
				endo	= PosToNote4(botRight(nowRect), NULL, track);
			}

			if (starto > endo)
			{
				temp = endo;
				endo = starto;
				starto = temp;
			}
			
			oldV = selecStart.v;
			selecStart.h	= PosToNote4(topLeft(nowRect), &selecStart.v, track);
			
			oldV = selecEnd.v;
			selecEnd.h		= PosToNote4(botRight(nowRect), &selecEnd.v, track) - 1;
			selecEnd.v--;
			
			selecTrack = track;
			
			UpdateSelectedRect(&nowRect, &thenRect);
			/***************/
			
			thenRect = nowRect;
		}
	}
	while (StillDown());
	
	*resultRect = thenRect;
	
	return 0;
}

void SetUpTracksMenu(void)
{
	short	i;
	Str255	str, str2;
	
	DisposeMenu(TrackView);
	
	TrackView = GetMenu(137);		// 137
	
	for (i = 0; i < curMusic->header->numChn; i++)
	{
		pStrcpy(str, "\pTrack ");
		NumToString(i + 1, str2);
		pStrcat(str, str2);
		AppendMenu(TrackView, str);
	}
}

void SwitchColor(short TrackNo)
{
	RGBForeColor(&thePrefs.tracksColor[ TrackNo]);
}

Boolean CreateNoteString(Cmd *theCommand, Str255 mainStr, Boolean AllStr);

void DrawNewNote(void)
{
	Str255		aStr;
	short		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	GrafPtr		savePort;
	Cmd			newNote;
	Boolean		copy[ 5];
	FontInfo	ThisFontInfo;
	
	copy[ 0] = thePrefs.DigitalInstru;
	copy[ 1] = thePrefs.DigitalNote;
	copy[ 2] = thePrefs.DigitalEffect;
	copy[ 3] = thePrefs.DigitalArgu;
	copy[ 4] = thePrefs.DigitalVol;
	
	GetPort(&savePort);
	SetPortDialogPort(MozartDlog);
	
	newNote.ins 	= curInstru;
	newNote.note 	= 0xFF;
	newNote.cmd 	= curEffect;
	newNote.arg 	= curArgu;
	newNote.vol 	= curVol;
	
	thePrefs.DigitalInstru	= true;
	thePrefs.DigitalNote	= false;
	thePrefs.DigitalEffect	= true;
	thePrefs.DigitalArgu	= true;
	thePrefs.DigitalVol		= true;
	
	
	CreateNoteString(&newNote, aStr, false);
	
	GetDialogItem(MozartDlog, 21, &itemType, &itemHandle, &itemRect);
	ForeColor(whiteColor);
	PaintRect(&itemRect);
	
	ForeColor(blackColor);
	FrameRectRelief(&itemRect);
	MoveTo(itemRect.left + 3, itemRect.bottom - 4);
	DrawString(aStr);
	
	TextFont(4);	TextSize(9);
	GetFontInfo(&ThisFontInfo);
	RGBForeColor(&theColor);
	MoveTo(itemRect.left + ThisFontInfo.widMax*3 + ThisFontInfo.widMax-1, itemRect.top);
	LineTo(itemRect.left + ThisFontInfo.widMax*3 + ThisFontInfo.widMax-1, itemRect.bottom);
	
	MoveTo(itemRect.left + ThisFontInfo.widMax*5 + ThisFontInfo.widMax-1, itemRect.top);
	LineTo(itemRect.left + ThisFontInfo.widMax*5 + ThisFontInfo.widMax-1, itemRect.bottom);
	
	MoveTo(itemRect.left + ThisFontInfo.widMax*8 + ThisFontInfo.widMax-1, itemRect.top);
	LineTo(itemRect.left + ThisFontInfo.widMax*8 + ThisFontInfo.widMax-1, itemRect.bottom);
	
	MoveTo(itemRect.left + ThisFontInfo.widMax*11 + ThisFontInfo.widMax-1, itemRect.top);
	LineTo(itemRect.left + ThisFontInfo.widMax*11 + ThisFontInfo.widMax-1, itemRect.bottom);
	
	ForeColor(blackColor);
	
	SetPort(savePort);
	
	thePrefs.DigitalInstru	= copy[ 0];
	thePrefs.DigitalNote	= copy[ 1];
	thePrefs.DigitalEffect	= copy[ 2];
	thePrefs.DigitalArgu	= copy[ 3];
	thePrefs.DigitalVol		= copy[ 4];
	
}

void SwitchColorBack(short TrackNo)
{
	RGBBackColor(&thePrefs.tracksColor[ TrackNo]);
}

extern	DialogPtr	CmdDlog;

void UpdateInstruMenu(void)
{
	int		x, i, z;
	short	menuNum;
	Str255	mainStr, stemp;
	
	x = CountMenuItems(InstruMenu);
	for (i = 0; i < x; i++) DeleteMenuItem(InstruMenu, 1);
	
	menuNum = 0;
	for (i=0; i<curMusic->header->numInstru; i++)
	{
		if (curMusic->fid[i].numSamples > 0) menuNum = i;
		
		strcpy((Ptr) stemp, curMusic->fid[i].name);
		
		MyC2PStr((Ptr) stemp);
		
		if (stemp[ 0] > 0) menuNum = i;
	}
	
	if (menuNum < 16) menuNum = 16;
	
	menuNum++;
	
	for (i=0; i<menuNum; i++)
	{
		NTStr(3, i+1, (Ptr) stemp);
		strcpy((Ptr) mainStr, (Ptr) stemp);
		if (curMusic->fid[i].numSamples > 0) strcat((Ptr) mainStr, "*-");
		else strcat((Ptr) mainStr, " -");
		
		strcpy((Ptr) stemp, curMusic->fid[i].name);
		stemp[21] = '\0';
		strcat((Ptr) mainStr, (Ptr) stemp);
		
		mainStr[ 31] = '\0';
		
		MyC2PStr((Ptr) mainStr);
		
		for (z = 1; z <= mainStr[ 0]; z++)
		{
			if (mainStr[ z] == '(') mainStr[ z] = ' ';
			if (mainStr[ z] == ')') mainStr[ z] = ' ';
		}
		AppendMenu(InstruMenu, mainStr);
	}
}

Boolean aNote2(Point theCell, Point *PointNote, short track)
{
	Cmd					*theCommand;
	short				val, Pos;
	
	theCommand = GetMADCommand(theCell.h, track, curMusic->partition[ PatCopy]);
	
	val = theCommand->note;
	if (val > 0)
	{
		Pos = NUMBER_NOTES - val-1;
		if (Pos == theCell.v)
		{
			PointNote->h = theCell.h;
			PointNote->v = track;
			return true;
		}
	}
	return false;
}

void SetWinControl(short tempA, short tempB, DialogPtr theDia)
{
	short	itemType, max;
	Handle	itemHandle;
	Rect	itemRect;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(theDia), &caRect);
	
	MyMoveControl(c2h, MozartRect.left, caRect.bottom-15);
	MySizeControl(c2h, caRect.right - MozartRect.left - 14, 16);	//
	max = MozartRect.right - tempA + 15 + 1;	if (max < 0) max = 0;
	SetControlMaximum(c2h, max);
	if (gUseControlSize) SetControlViewSize(c2h, tempA - 15 - 1);
	
	MyMoveControl(c3h, caRect.right - 15, MozartRect.top);	// - LegendeHi
	MySizeControl(c3h, 16, tempB -14 - MozartRect.top);
	max = curMusic->header->numChn  - ((caRect.bottom - 14) - MozartRect.top) / YSize;	if (max < 0) max = 0;
	SetControlMaximum(c3h, max);
	if (gUseControlSize) SetControlViewSize(c3h, ((caRect.bottom - 14) - MozartRect.top) / YSize);
	
	GetDialogItem(MozartDlog, 13, &itemType, &itemHandle, &itemRect);
	MyMoveControl(c1h, itemRect.left, MozartRect.top);
	MySizeControl(c1h, 16, YSize+1);
	max = MozartRect.bottom - MozartRect.top - YSize;	if (max < 0) max = 0;
	SetControlMaximum(c1h, max);
	if (gUseControlSize) SetControlViewSize(c1h, MozartRect.top + YSize);
	
	SetMaxWindow(MozartRect.left + 16 + MOLarg*curMusic->partition[ PatCopy]->header.size,
				 MozartRect.top + 17 + curMusic->header->numChn*YSize,
				 theDia);
}

void SetWMozart(short No)
{
	Str255		theTitle, AStr;
	
	pStrcpy(theTitle, "\pPattern: ");
	NumToString((long) No, AStr);
	pStrcat(theTitle, AStr);
	
	SetWTitle(GetDialogWindow(MozartDlog), theTitle);
}

void DoGrowMozart(DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		temp;
	short		tempB, tempA;
	Point		aPt = {0, 0};
	
	GetPort(&SavePort);
	SetPortDialogPort(theDialog);
	
	temp.left = 223;
	temp.top = MozartRect.top + YSize + 15;
	temp.right = MozartRect.left + 17 + MOLarg*(curMusic->partition[ PatCopy]->header.size);
	temp.bottom = MozartRect.top + 17 + MOHaut*NUMBER_NOTES;
	
	LocalToGlobal(&aPt);
	
	if (temp.bottom < temp.top) temp.bottom = temp.top;
	//else if (temp.bottom >= qd.screenBits.bounds.bottom - aPt.v) temp.bottom = qd.screenBits.bounds.bottom - aPt.v -1;
	
	if (temp.right < temp.left) temp.right = temp.left;
	//else if (temp.right >= qd.screenBits.bounds.right - aPt.h) temp.right = qd.screenBits.bounds.right - aPt.h -1;
	
	
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);
	
	if (lSizeVH != 0)
	{
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	}
	else
	{
		Rect	caRect;
		
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
		
		if (tempA >= temp.right) tempA = temp.right-1;
		if (tempB >= temp.bottom) tempB = temp.bottom-1;
	}
	
	MySizeWindow(theDialog, tempA, tempB, true);
	SetWinControl(tempA, tempB, theDialog);
	
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	EraseRect(&caRect);
	UpdateMozartInfoInternal();
	InvalWindowRect(GetDialogWindow(theDialog), &caRect);
	
	SetPort(SavePort);
}

void Update1Note(short tt, short track)
{
	short				xTr, yTr;
	Cmd					*cmd;
	Rect				itemRect;
	GrafPtr				SavePort;
	
	if (MozartDlog == NULL) return;
	
	GetPort(&SavePort);
	SetPortDialogPort(MozartDlog);
	
	if (thePrefs.ClassicalProjection)
	{
		InvalWindowRect(GetDialogWindow(MozartDlog), &MozartRect);
	}
	else
	{
		cmd = GetMADCommand(tt, track, curMusic->partition[ PatCopy]);
		xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
		
		GetMozartTrackRect(&itemRect, track);
		
		itemRect.left = MozartRect.left + tt * MOLarg - xTr;		itemRect.right = itemRect.left + MOLarg;
		itemRect.left++;
		
		InvalWindowRect(GetDialogWindow(MozartDlog), &itemRect);
	}
	
	SetPort(SavePort);
}

void GetPianoMozartRect(Rect *aRect)
{
	Point	Pt1;
	Rect	r;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	if (PianoMozart == -1)
	{
		SetRect(aRect, -1, -1, -1, -1);
		return;
	}
	
	Pt1.v = PianoMozart;
	Pt1.h = 0;
	
	Pt1 = NoteToPos2(Pt1, PianoMozartTrack);
	
	aRect->top = Pt1.v;
	aRect->bottom = Pt1.v + MOHaut + 1;
	
	aRect->left = MozartRect.left;
	aRect->right = caRect.right-15;
	
	GetMozartTrackRect(&r, PianoMozartTrack);
	SectRect(&r, aRect, aRect);
}

void SelectToucheMozart(short touche, short track)
{
	Rect	aRect;
	GrafPtr	savePort;
	
	if (MozartDlog == NULL) return;
	
	GetPort(&savePort);
	SetPortDialogPort(MozartDlog);
	
	if (touche != -1) touche = NUMBER_NOTES - touche;
	
	/** Efface l'ancieene **/
	
	if (PianoMozart != -1)
	{
		GetPianoMozartRect(&aRect);
		InvalWindowRect(GetDialogWindow(MozartDlog), &aRect);
	}
	
	PianoMozart = touche;
	PianoMozartTrack = track;
	
	/** Affiche la nouvelle **/
	
	if (PianoMozart != -1)
	{
		GetPianoMozartRect(&aRect);
		InvalWindowRect(GetDialogWindow(MozartDlog), &aRect);
	}
	
	UpdateMozartWindow(MozartDlog);
	
	SetPort(savePort);
}

long ComputeRight(short note, short ins)
{
	long	cellBytes;
	long	insBytes;
	long long	aDD;
	long	period;
	long	result;
	sData	*curData;
	short	samp;
	
	ins--;
	
	samp			= curMusic->fid[ ins].what[ note];
	
	result 			= 1L;
	
	if (samp < curMusic->fid[ ins].numSamples)
	{
		/* Compute ins bytes */
		
		curData			= curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
		if (curData->loopSize > 0) return 25000;
		
		note			+= curData->relNote;
		
		period = GetOldPeriod(note, curData->c2spd, MADDriver);
		aDD = ((long long)AMIGA_CLOCKFREQ2 << BYTEDIV) / (period * (rate22khz>>16));
		insBytes = (curData->size << BYTEDIV)/ aDD;
		
		/* Compute cell size in bytes */
		
		cellBytes		=  ((rate22khz >> 16) * 125L) / (50);
		cellBytes 		/= MADDriver->finespeed;
		cellBytes		*= MADDriver->speed;
		
		/* Results */
		
		result = (insBytes * MOLarg) / cellBytes;
		if (result < 1) result = 1;
	}
	
	return result;
}

void ComputeGrilleGWorld()
{
	Rect			r;
	GWorldPtr 		currPort;
	GDHandle 		currDev;
	Rect			caRect;
	
	if (MozartDlog == NULL) return;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	GetMozartTrackRect(&r, GetControlValue(c3h));
	r.right = caRect.right-15;
	//r.bottom = MozartDlog->portRect.bottom-15;
	
	GetGWorld(&currPort, &currDev);
	
	if (grilleGWorld == NULL)
	{
		NewGWorld(&grilleGWorld, 0, &r, nil, nil, 0);
		
		LockPixels(GetGWorldPixMap(grilleGWorld));
	}
	else
	{
		Rect pixBounds;
		
		GetPixBounds(GetGWorldPixMap(grilleGWorld), &pixBounds);
		
		if (!EqualRect(&pixBounds, &r))
		{
			UnlockPixels(GetGWorldPixMap(grilleGWorld));
			DisposeGWorld(grilleGWorld);
			
			NewGWorld(&grilleGWorld, 0, &r, nil, nil, 0);
			LockPixels(GetGWorldPixMap(grilleGWorld));
		}
	}
	
	SetGWorld(grilleGWorld, nil);
	
	Rect pixBounds;
	
	GetPixBounds(GetGWorldPixMap(grilleGWorld), &pixBounds);
	
	EraseRect(&pixBounds);
	TraceGrille2(-GetControlValue(c2h), -GetControlValue(c1h), &pixBounds);
	
	if (MozartRect.right - MozartRect.left < pixBounds.right)
	{
		r = pixBounds;
		r.left = MozartRect.right - MozartRect.left;
		r.right = pixBounds.right;
		EraseRect(&r);
	}
	
	SetGWorld(currPort, currDev);
}

void UpdateMozartInfoInternal(void)
{
	short   			itemType, i;
	Handle				itemHandle;
	short				tt, val, Pos, xTr, yTr, tI, StartX, EndX, StartY, EndY;
	Boolean				Note;
	Cmd					*theCommand;
	Rect				destRect, itemRect, theClipRect, pixMapRect;
	GrafPtr				SavePort;
	GWorldPtr 			currPort;
	GDHandle 			currDev;
	Rect				caRect;
	
	
	if (MozartDlog == NULL) return;
	
	// IMPORTANT: permet l'update des PatCopy et autres !!!!
	if (PatCopy != MADDriver->Pat || PatternSizeCopy != curMusic->partition[ PatCopy]->header.size)	DoNullMozart();
	
	CurrentNumChn = curMusic->header->numChn;
	
	GetPort(&SavePort);
	SetPortDialogPort(MozartDlog);
	
	GetShowVal(&StartX, &EndX, &StartY, &EndY);
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	/* Rect à afficher */
	pixMapRect.top = MozartRect.top - LegendeHi;
	pixMapRect.left = 0;
	pixMapRect.right = caRect.right-15;
	pixMapRect.bottom = caRect.bottom -15;
	
	GetGWorld(&currPort, &currDev);
	
	if (globalGWorld == NULL)
	{
		QDErr	aErr;
		
		ComputeGrilleGWorld();
		
		aErr = NewGWorld(&globalGWorld, 0, &pixMapRect, nil, nil, 0);
		if (aErr)
		{
			Erreur(9, 9);
			SetRect(&pixMapRect, 0, 0, 5, 5);
			NewGWorld(&globalGWorld, 0, &pixMapRect, nil, nil, 0);
		}
		
		LockPixels (GetGWorldPixMap(globalGWorld));
		
		SetGWorld (globalGWorld, nil);
		SetOrigin(pixMapRect.left, pixMapRect.top);
	}
	else
	{
		Rect pixBounds;
		
		GetPixBounds(GetGWorldPixMap(globalGWorld), &pixBounds);
		
		if (!EqualRect(&pixBounds, &pixMapRect))
		{
			QDErr	aErr;
			
			UnlockPixels (GetGWorldPixMap(globalGWorld));
			DisposeGWorld(globalGWorld);
			
			ComputeGrilleGWorld();
			
			aErr = NewGWorld(&globalGWorld, 0, &pixMapRect, nil, nil, 0);
			if (aErr)
			{
				Erreur(9, 9);
				SetRect(&pixMapRect, 0, 0, 5, 5);
				NewGWorld(&globalGWorld, 0, &pixMapRect, nil, nil, 0);
			}
			
			LockPixels (GetGWorldPixMap(globalGWorld));
			
			SetGWorld (globalGWorld, nil);
			SetOrigin(pixMapRect.left, pixMapRect.top);
		}
		else SetGWorld (globalGWorld, nil);
	}
	ClipRect(&pixMapRect);
	EraseRect(&pixMapRect);
	
	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
	
	/********* Légendes *************/
	
	// X Label
	
	destRect.left = pixMapRect.left + LegendeLa - xTr;
	destRect.top = pixMapRect.top;
	destRect.right = destRect.left + xLabel.bounds.right;
	destRect.bottom = destRect.top + xLabel.bounds.bottom;
	
	CopyBits(&xLabel,
			 (BitMap *) (*(GetGWorldPixMap(globalGWorld))),		//GetPortBitMapForCopyBits
			 &xLabel.bounds,
			 &destRect,
			 srcCopy,
			 NULL);
	
	SetRect(&destRect,
			pixMapRect.left,
			pixMapRect.top,
			pixMapRect.left + LegendeLa,
			pixMapRect.top + LegendeHi);
	
	RGBForeColor(&theColor);
	PaintRect(&destRect);
	ForeColor(blackColor);
	
	MoveTo(LegendeLa, pixMapRect.top);
	LineTo(pixMapRect.right, pixMapRect.top);
	
	TextFont(4);	TextSize(9);
	
	// Tracks
	
#if 0
	for (tI = GetControlValue(c3h); tI < curMusic->header->numChn; tI++)
	{
		// Position des controls gauches
		
		(*c1h[ tI])->contrlRect.top		= MozartRect.top + (tI - GetControlValue(c3h))*YSize;
		(*c1h[ tI])->contrlRect.bottom	= (*c1h[ tI])->contrlRect.top + YSize + 1;
	}
#endif
	
	GetDialogItem(MozartDlog, 16, &itemType, &itemHandle, &itemRect);
	for (tI = GetControlValue(c3h); tI < GetMaxYBox(); tI++)	//
	{
		Str255		String;
		
		/* Le ClipRect */
		theClipRect.left = pixMapRect.left;
		theClipRect.top = MozartRect.top + (tI - GetControlValue(c3h))*YSize;
		theClipRect.bottom = theClipRect.top + YSize;
		theClipRect.right = MozartRect.left;
		
		ClipRect(&theClipRect);
		
		destRect.left	= itemRect.left;
		destRect.right	= itemRect.right;
		
		destRect.top	= MozartRect.top + 1 + (tI - GetControlValue(c3h)) * YSize;
		destRect.bottom	= destRect.top + YSize - 1;
		
		SwitchColor(tI);
		PaintRect(&destRect);
		
		ForeColor(blackColor);
		
		if (MADDriver->Active[ tI]) ForeColor(blackColor);
		else RGBForeColor(&Gray[ 0]);
		
		
		NTStr(2, tI +1, (Ptr) String);
		
		MoveTo(1, destRect.top + 3 +(destRect.bottom - destRect.top)/2);
		DrawString(MyC2PStr((Ptr) String));
		
		ForeColor(blackColor);
		
		MoveTo(destRect.right, theClipRect.top);	LineTo(destRect.right, theClipRect.bottom);
		
		// On commmence l'update de chaque track
		
		// Y Label
		
		destRect = yLabel.bounds;
		destRect.top -= yTr - (tI - GetControlValue(c3h))*YSize;
		destRect.bottom -= yTr - (tI - GetControlValue(c3h))*YSize;
		
		CopyBits(&yLabel,
				 (BitMap *) (*(GetGWorldPixMap(globalGWorld))),
				 &yLabel.bounds,
				 &destRect,
				 srcCopy,
				 NULL);
		
		MoveTo(yLabel.bounds.left-1, theClipRect.top);	LineTo(yLabel.bounds.left-1, theClipRect.bottom);
		
		
		MoveTo(0, theClipRect.top);	LineTo(theClipRect.right, theClipRect.top);
		
		/**********************************/
		
		/*** La partition en elle-même ****/
		
		/* Le ClipRect */
		theClipRect.left = MozartRect.left;
		theClipRect.top = MozartRect.top + (tI - GetControlValue(c3h))*YSize;
		theClipRect.bottom = theClipRect.top + YSize;
		theClipRect.right = pixMapRect.right;
		
		
		ClipRect(&theClipRect);
		
		Rect pixBounds;
		
		GetPixBounds(GetGWorldPixMap(grilleGWorld), &pixBounds);
		
		if (MADDriver->Active[ tI]) ForeColor(blackColor);
		else RGBForeColor(&Gray[ 0]);
		
		CopyBits((BitMap *) (*(GetGWorldPixMap(grilleGWorld))),
				 (BitMap *) (*(GetGWorldPixMap(globalGWorld))),
				 &pixBounds,
				 &theClipRect,
				 srcCopy,
				 NULL);
		
		ForeColor(blackColor);
		//TraceGrille2(-xTr + LegendeLa, -yTr + MozartRect.top + (tI - GetControlValue(c3h))*YSize, &theClipRect);
		
		//calculer une grille pixmap et la modifier que si zoom ou change size
		
		MoveTo(0, theClipRect.top);	LineTo(theClipRect.right, theClipRect.top);
		
		theClipRect.top++;
		ClipRect(&theClipRect);
		theClipRect.top--;
		
		if (showAllNotes)
		{
			for (i = 0 ; i < MADDriver->DriverSettings.numChn; i++)
			{
				if (i != tI)
				{
					for (tt = StartX; tt < EndX; tt++)
					{
						Note = false;
						theCommand = GetMADCommand(tt, i, curMusic->partition[ PatCopy]);
						
						val = theCommand->note;
						if (val != 0xFF) Note = true;
						Pos = NUMBER_NOTES - val - 1;
						
						val = theCommand->ins;
						if (val != 0) Note = true;
						
						if (Note == true)
						{
							destRect.left = tt * MOLarg - xTr + theClipRect.left;
							destRect.right = destRect.left + MOLarg;
							destRect.top = Pos * MOHaut - yTr + theClipRect.top;
							destRect.bottom = destRect.top + MOHaut;
							
							DrawCubeNote2(&destRect, val, i, true);
						}
					}
				}
			}
		}
		
		/** Drawing Notes **/
		{
			short destRight = 0;
			
			
			if (thePrefs.ClassicalProjection)
			{
				StartX 				= 0;
				destRight 			= -1;
				MADCheckSpeed(curMusic, MADDriver);
			}
			
			for (tt = StartX; tt < EndX; tt++)
			{
				Note = false;
				theCommand = GetMADCommand(tt, tI, curMusic->partition[ PatCopy]);
				
				val = theCommand->note;
				Pos = NUMBER_NOTES - val - 1;
				if (val != 0xFF) Note = true;
				
				val = theCommand->ins;
				if (val != 0) Note = true;
				
				if (Note == true)
				{
					if (thePrefs.ClassicalProjection)
					{
						destRect.left = tt * MOLarg - xTr + theClipRect.left;
						destRect.top = Pos * MOHaut - yTr + theClipRect.top;
						destRect.bottom = destRect.top + MOHaut;
						destRect.right = destRect.left + MOLarg;
						
						destRight = destRect.left + ComputeRight(theCommand->note, theCommand->ins);
						if (destRight < destRect.right)
						{
							destRect.right = destRight;
							destRight = -1;
						}
						
						DrawCubeNote2(&destRect, val, tI, false);
					}
					else
					{
						destRect.left = tt * MOLarg - xTr + theClipRect.left;
						destRect.right = destRect.left + MOLarg;
						destRect.top = Pos * MOHaut - yTr + theClipRect.top;
						destRect.bottom = destRect.top + MOHaut;
						
						DrawCubeNote2(&destRect, val, tI, false);
					}
				}
				else if (thePrefs.ClassicalProjection)
				{
					destRect.left = tt * MOLarg - xTr + theClipRect.left;
					destRect.right = destRect.left + MOLarg;
					
					if (destRight < destRect.right)
					{
						destRect.right = destRight;
						destRight = -1;
					}
					
					SwitchColor(tI);
					destRect.top++;
					PaintRect(&destRect);
					destRect.top--;
					ForeColor(blackColor);
				}
			}
		}
	}
	
	ClipRect(&pixMapRect);
	
	if (GetMaxYBox() == curMusic->header->numChn)
	{
		MoveTo(pixMapRect.left, theClipRect.bottom);
		LineTo(pixMapRect.right, theClipRect.bottom);
	}
	
	SetGWorld (currPort, currDev);
	
	CheckSelecValue();
	
	SetPort(SavePort);
}

void DoNullMozart(void)
{
	GrafPtr		SavePort;
	short		track;
	Rect		tempRect, clipRect;
	Point		pt, pt2, pt3;

 	if (MozartDlog == NULL)
		return;
  
 	GetPort(&SavePort);
 	SetPortDialogPort(MozartDlog);

	//*********
	
	if (ReaderCopy != MADDriver->PartitionReader && MADDriver->PartitionReader < curMusic->partition[ PatCopy]->header.size)
	{
		ErasePartitionReader();
		ReaderCopy = MADDriver->PartitionReader;
		DrawPartitionReader();
	}
	
	if (curMusic->header->numChn != CurrentNumChn)
	{
		Rect	caRect;
	
		GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
		CurrentNumChn = curMusic->header->numChn;
		
		SetWinControl(caRect.right, caRect.bottom, MozartDlog);
		
		InvalWindowRect(GetDialogWindow(MozartDlog), &caRect);
		UpdateMozartInfoInternal();
		UpdateMozartWindow(MozartDlog);
	}
	
	if (PatCopy != MADDriver->Pat || PatternSizeCopy != curMusic->partition[ PatCopy]->header.size)
	{
		Rect	caRect;
		
		GetPortBounds(GetDialogPort(MozartDlog), &caRect);
		
		PatCopy = MADDriver->Pat;
		MozartRect.right = MozartRect.left + MOLarg * curMusic->partition[ PatCopy]->header.size;
		MozartRect.bottom = MozartRect.top + MOHaut * NUMBER_NOTES;

		if (curMusic->partition[ PatCopy]->header.size != PatternSizeCopy)
		{
			PatternSizeCopy = curMusic->partition[ PatCopy]->header.size;
			
			SetWinControl(caRect.right, caRect.bottom, MozartDlog);
			SetUpLabels();
			ComputeGrilleGWorld();
			
			InvalWindowRect(GetDialogWindow(MozartDlog), &caRect);
		}
		
		tempRect = MozartRect;
		tempRect.left = 0;
		tempRect.right = caRect.right-15;
		tempRect.top -= LegendeHi;
		InvalWindowRect(GetDialogWindow(MozartDlog), &tempRect);
		
		SetWMozart(PatCopy);
		UpdateMozartInfoInternal();
		UpdateMozartWindow(MozartDlog);
	}
	
	if(GetWRefCon(oldWindow) == RefMozart)
	{
		RgnHandle visibleRegion;
		
		pt = theEvent.where;
		GlobalToLocal(&pt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(MozartDlog), visibleRegion);
		
		if (PtInRgn(pt, visibleRegion))
		{
			if (theEvent.modifiers & shiftKey)		{	if (Mode != TrashM)	{ copyMode = Mode;	Mode = TrashM;	SelectGoodMode();}}
			else if (theEvent.modifiers & cmdKey)	{	if (Mode != PlayM)	{ copyMode = Mode;	Mode = PlayM;	SelectGoodMode();}}
			else if (Mode != copyMode)
			{
				Mode = copyMode;
				SelectGoodMode();
			}
			
			{
				Rect LegendeXRect;
				Rect LegendeYRect;
				Rect	caRect;
	
				GetPortBounds(GetDialogPort(MozartDlog), &caRect);
				
				SetRect(&LegendeXRect, MozartRect.left, MozartRect.top - LegendeHi, caRect.right-15, MozartRect.top);
				SetRect(&LegendeYRect, MozartRect.left - LegendeLaS, MozartRect.top, MozartRect.left, caRect.bottom-15);

				if (MozartRect.right < caRect.right-15) SetRect(&clipRect, MozartRect.left, MozartRect.top, MozartRect.right, caRect.bottom-15);
				else SetRect(&clipRect, MozartRect.left, MozartRect.top, caRect.right-15, caRect.bottom-15);

				if (PtInRect(pt, &clipRect))
				{
					short	iType, note;
					Handle	iHandle;
					Rect	iRect;
					Str255	str;
					
					GetDialogItem(MozartDlog, 22, &iType, &iHandle, &iRect);
					iRect.right--;	iRect.top++;
					
					PosToNote3(pt, &note, &track);
					note = NUMBER_NOTES - note - 1;
					GetNoteString(note, str);
					
					if (!EqualString(str, curDisplayedNote, true, true))
					{
						pStrcpy(curDisplayedNote, str);
						TETextBox(curDisplayedNote+1, curDisplayedNote[ 0], &iRect, teCenter);
					}
					
					switch (Mode)
					{
						case NoteM:
							pt2.h = PosToNote3(pt, &pt2.v, &track);
							if (aNote2(pt2, &pt3, track)) SetCursor(&HandCrsr);
							else SetCursor(&NoteCrsr);
						break;
						
						case CrossM:
							if (PointSelection(pt)) SetCursor(&HandCrsr);
							else SetCursor(&CrossCrsr);
						break;
						
						case PlayM:
							SetCursor(&PlayCrsr);
						break;
						
						case TrashM:
							SetCursor(&DelCrsr);
						break;

						case ZoomM:
							if (theEvent.modifiers & optionKey) SetCursor(&ZoomOutCrsr);
							else SetCursor(&ZoomInCrsr);
						break;
					}
				}
				else if (PtInRect(pt, &LegendeXRect)) SetCursor(&PlayCrsr);
				else if (PtInRect(pt, &LegendeYRect)) SetCursor(&PlayCrsr);
				else SetCursor(GetQDGlobalsArrow(&qdarrow));
			}
		}
		else SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		DisposeRgn(visibleRegion);
	}
	
	SetPort(SavePort);
}

void UpdateMozartInfo(void)
{
	GrafPtr		SavePort;
	Rect	caRect;
	

	if (MozartDlog == NULL) return;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(MozartDlog);
 	
	UpdateMozartInfoInternal();
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);

	InvalWindowRect(GetDialogWindow(MozartDlog), &caRect);
	
	SetPort(SavePort);
}

void DrawPartitionReader()
{
	GrafPtr		SavePort;
	Rect		tRect;
	Rect	caRect;
	
	
	GetPort(&SavePort);
 	SetPortDialogPort(MozartDlog);

	tRect.top = MozartRect.top - LegendeHi;
	tRect.bottom = MozartRect.top;

	tRect.left = MozartRect.left + ReaderCopy * MOLarg - GetControlValue(c2h);
	tRect.right = tRect.left + MOLarg + 1;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	if (tRect.left < caRect.right-15) 
	{
		if (tRect.right > caRect.right-15) tRect.right = caRect.right-15;
		if (tRect.left <= MozartRect.left) tRect.left = MozartRect.left+1;

		BackColor(redColor);
		
		
		
		CopyBits ( (BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
					(BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
					&tRect,
					&tRect,
					srcCopy,
					nil);
								
		RGBBackColor(&theColor);
	}
	SetPort(SavePort);
}

void ErasePartitionReader()
{
	GrafPtr		SavePort;
	Rect		tRect;
	Rect	caRect;
	
	GetPort(&SavePort);
	SetPortDialogPort(MozartDlog);
	
	tRect.top = MozartRect.top - LegendeHi;
	tRect.bottom = MozartRect.top;
	
	tRect.left = MozartRect.left + ReaderCopy * MOLarg - GetControlValue(c2h);
	tRect.right = tRect.left + MOLarg + 1;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	if (tRect.left < caRect.right-15)
	{
		if (tRect.right > caRect.right-15) tRect.right = caRect.right-15;
		if (tRect.left <= MozartRect.left) tRect.left = MozartRect.left+1;
		
		ForeColor(blackColor);
		BackColor (whiteColor);
		
		CopyBits((BitMap *) (*(GetGWorldPixMap(globalGWorld))),
				 (BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
				 &tRect,
				 &tRect,
				 srcCopy,
				 nil);
		
		RGBBackColor(&theColor);
	}
	SetPort(SavePort);
}

void UpdateMozartWindow(DialogPtr GetSelection)
{
	Rect   		tempRect, LoRect;
	GrafPtr		SavePort;
	RgnHandle	saveClipRgn;
	Rect		caRect;
	
	GetPort(&SavePort);
	SetPortDialogPort(GetSelection);
	
	GetPortBounds(GetDialogPort(GetSelection), &caRect);
	
	//DoNullMozart();
	
	BeginUpdate(GetDialogWindow(GetSelection));
	
	/**/
	
	ForeColor(blackColor);	BackColor (whiteColor);
	
	CopyBits((BitMap *) (*(GetGWorldPixMap(globalGWorld))),
			 (BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
			 &(*GetGWorldPixMap(globalGWorld))->bounds,
			 &(*GetGWorldPixMap(globalGWorld))->bounds,
			 srcCopy,
			 nil);
	
	/* Rect du PartitionReader */
	
	DrawPartitionReader();
	
	/* Rect de sélection */
	
	RGBBackColor(&Gray[0]);
	GetSelectionRect2(&LoRect, true);
	
	CopyBits((BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
			 (BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
			 &LoRect,
			 &LoRect,
			 srcCopy,
			 nil);
	
	RGBBackColor(&redC);
	GetPianoMozartRect(&LoRect);
	CopyBits((BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
			 (BitMap*) *GetPortPixMap(GetDialogPort(MozartDlog)),
			 &LoRect,
			 &LoRect,
			 srcCopy,
			 nil);
	RGBBackColor(&theColor);
	
	/**********************/
	
	MoveTo(0, MozartRect.top);
	LineTo(caRect.right - 15, MozartRect.top);
	
	MoveTo(MozartRect.left, MozartRect.top - LegendeHi);
	LineTo(MozartRect.left, caRect.bottom - 15);
	
	DrawGrowIcon(GetDialogWindow(GetSelection));
	
	DrawNewNote();
	
	// Control c1h
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	
	GetMozartTrackRect(&tempRect, GetControlValue(c3h));
	tempRect.left = 0;
	tempRect.right = MozartRect.left;
	
	ClipRect(&tempRect);
	Draw1Control(c1h);
	SetClip(saveClipRgn );
	DisposeRgn(saveClipRgn );
	
	////////////////
	
	{
		RgnHandle visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(GetSelection), visibleRegion);
		
		//(*c1h)->contrlVis = 0;	CARBON
		UpdateDialog(GetSelection, visibleRegion);
		//(*c1h)->contrlVis = 255; CARBON
		
		DisposeRgn(visibleRegion);
	}
	
	{
		Rect	iRect;
		short	iType;
		Handle	iHandle;
		
		GetDialogItem(MozartDlog, 22, &iType, &iHandle, &iRect);
		iRect.right--;
		iRect.top++;
		TETextBox(curDisplayedNote+1, curDisplayedNote[ 0], &iRect, teCenter);
	}
	
	EndUpdate(GetDialogWindow(GetSelection));
	
	SetPort(SavePort);
} 

static	long 			lastWhen = 0;

pascal void actionProc(ControlHandle theControl, short ctlPart)
{
	long		lRefCon;
	short		maxValue, minValue, curVal, copyval;
	RgnHandle	aRgn;
	Rect		itemRect;
	Rect		caRect;
	
	if (ctlPart <= 0)
		return;
	
	lRefCon = GetControlReference(theControl);
	maxValue = GetControlMaximum(theControl);
	minValue = GetControlMinimum(theControl);
	copyval = curVal = GetControlValue(theControl);
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	switch (ctlPart)
	{
		case kControlUpButtonPart:
			if (lRefCon == 3) curVal--;
			else if (lRefCon == 2) curVal -= MOLarg;
			else curVal -= MOHaut;
			
			if (curVal < minValue) curVal = minValue;
			break;
			
		case kControlDownButtonPart:
			if (lRefCon == 3) curVal++;
			else if (lRefCon == 2) curVal += MOLarg;
			else curVal += MOHaut;
			
			if (curVal > maxValue) curVal = maxValue;
			break;
			
		case kControlPageUpPart:
			if (lRefCon == 3) curVal -= ((caRect.bottom - 14) - MozartRect.top) / YSize;
			else if (lRefCon == 2) curVal -= caRect.right - 15 - LegendeLa - 1;
			else curVal -= -1 + YSize;
			
			if (curVal < minValue) curVal = minValue;
			break;
			
		case kControlPageDownPart:
			if (lRefCon == 3) curVal+= ((caRect.bottom - 14) - MozartRect.top) / YSize;
			else if (lRefCon == 2) curVal += caRect.right - 15 - LegendeLa - 1;
			else curVal += -1 + YSize;
			
			if (curVal > maxValue) curVal = maxValue;
			break;
			
		case kControlIndicatorPart:
			copyval = gThumbPrev;
			gThumbPrev = curVal;
			break;
	}
	
	if (copyval != curVal)
	{
		aRgn = NewRgn();
		
		if (lRefCon == 3)
		{
			Rect	tempRect;
			
			SetControlValue(theControl, curVal);
			
			itemRect = MozartRect;
			itemRect.left = 0;
			itemRect.top++;
			itemRect.right = caRect.right - 15;
			itemRect.bottom = caRect.bottom - 15;
			
			ScrollRect(&itemRect, 0, (copyval - curVal)*YSize, aRgn);
			
			GetMozartTrackRect(&tempRect, GetControlValue(c3h));
			tempRect.left = 0;		tempRect.right = MozartRect.left - LegendeLaS;
			tempRect.bottom = caRect.bottom-15;
			InvalWindowRect(GetDialogWindow(MozartDlog), &tempRect);
		}
		else if (lRefCon == 2)
		{
			SetControlValue(theControl, curVal);
			
			itemRect = MozartRect;
			itemRect.top -= LegendeHi;
			itemRect.left++;
			itemRect.right = caRect.right - 15;
			itemRect.bottom = caRect.bottom - 15;
			
			ScrollRect(&itemRect, (copyval - curVal), 0, aRgn);
			
			ComputeGrilleGWorld();
		}
		else	// Control c1h
		{
			short tI;
			Rect tempRect;
			
			RgnHandle	saveClipRgn = NewRgn();
			GetClip(saveClipRgn);
			GetMozartTrackRect(&tempRect, GetControlValue(c3h));
			tempRect.left = 0;		tempRect.right = MozartRect.left - LegendeLaS;
			
			ClipRect(&tempRect);
			
			SetControlValue(theControl, curVal);
			
			SetClip(saveClipRgn);
			DisposeRgn(saveClipRgn);
			
			for (tI = GetControlValue(c3h); tI < curMusic->header->numChn; tI++)
			{
				itemRect = MozartRect;
				itemRect.left -= LegendeLaS;
				itemRect.top += 1 + (tI - GetControlValue(c3h))*YSize;
				itemRect.right = caRect.right - 15;
				itemRect.bottom = itemRect.top + YSize-1;
				
				if (itemRect.bottom > caRect.bottom - 15) itemRect.bottom = caRect.bottom - 15;
				
				ScrollRect(&itemRect, 0 , (copyval - curVal), aRgn);
			}
			
			ComputeGrilleGWorld();
		}
		
		UpdateMozartInfoInternal();
		UpdateMozartWindow(MozartDlog);
		QDFlushPortBuffer(GetDialogPort(MozartDlog), aRgn);
		
		DisposeRgn(aRgn);
	}
}

void GetNoteRect2(Rect *destRect, Point theCell, short track)
{
	short xTr, yTr;
	
	xTr = GetControlValue(c2h);	yTr = GetControlValue(c1h);
	
	destRect->left = MozartRect.left + theCell.h * MOLarg - xTr;
	destRect->right = destRect->left + MOLarg + 1;
	destRect->top = MozartRect.top + theCell.v * MOHaut - yTr + (track - GetControlValue(c3h))*YSize;
	destRect->bottom = destRect->top + MOHaut + 1;
}

void SelectNotes(void)
{
	Rect	resultRect, maxRect;
	Point	aPoint;
	short	track;
	
	aPoint = theEvent.where;
	GlobalToLocal(&aPoint);
	
	PosToNote3(aPoint, NULL, &track);
	
	GetMozartTrackRect(&maxRect, track);
	
	TrackMarquee(aPoint, &resultRect, &maxRect);
}

void SetInstruMozart(short myInstru)
{
	if (MozartDlog == NULL)
		return;
	curInstru = myInstru + 1;
	DrawNewNote();
}

void DeleteOneMozartNote(short Position, short track, short Pattern)
{
	Cmd					*curCom;
	
	curCom = GetMADCommand(Position, track, curMusic->partition[ Pattern]);
	MADKillCmd(curCom);
	
	UPDATE_Note(Position, track);
	
	curMusic->hasChanged = true;
}

void CalculDiffStartBox(void)
{
	Point	myPt;
	short	pos, track, note;
	
	myPt = theEvent.where;
	GlobalToLocal(&myPt);
	
	pos = PosToNote3(myPt, &note, &track);
	
	DiffStartH = selecStart.v - note;
	DiffStartV = selecStart.h - pos;
}

void PressPartition(Point myPt)
{
	Point	theCell, PointNote, Note, dest;
	short	track, bogus, itemType, xVal, yVal;
	Rect	destRect, itemRect;
	Handle	itemHandle;
	short	XE, XS, YE, YS;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	theCell.h = PosToNote3(myPt, &theCell.v, &track);
	
	switch (Mode) {
		case ZoomM:
			if (IsPressed(0x003A)) ZoomInter--;
			else ZoomInter++;
			
			if (ZoomInter < 0) {
				ZoomInter = 0;
				break;
			}
			if (ZoomInter > 7) {
				ZoomInter = 7;
				break;
			}
			
			thePrefs.MozartX = ZoomInter;
			
			MOLarg = MOLargList[ZoomInter];
			MOHaut = MOHautList[ZoomInter];
			
			GetDialogItem (MozartDlog, 9, &itemType, &itemHandle, &itemRect);
			SetRect(&MozartRect, itemRect.left, itemRect.top, 0, 0);
			MozartRect.right = MozartRect.left + MOLarg * curMusic->partition[ MADDriver->Pat]->header.size;
			MozartRect.bottom = MozartRect.top + MOHaut * NUMBER_NOTES;
			
			SetWinControl(caRect.right, caRect.bottom, MozartDlog);
			SetUpLabels();
			
			GetShowVal(&XS, &XE, &YS, &YE);
			SetControlValue(c2h, (theCell.h - (XE - XS)/2) * MOLarg);
			SetControlValue(c1h, (theCell.v - (YE - YS)/2) * MOHaut);
			
			ComputeGrilleGWorld();
			
			UpdateMozartInfo();
			break;
			
		case TrashM:
			bogus = -1;
			do
			{
				GetMouse(&myPt);
				
				xVal = PosToNote3(myPt, &yVal, &track);
				Note.h = xVal;	Note.v = yVal;
				
				dest = NoteToPos2(Note, track);
				
				theCell.h = PosToNote3(myPt, &theCell.v, &track);
				
				if (theCell.h != bogus)
				{
					bogus = theCell.h;
					DeleteOneMozartNote(theCell.h, track, PatCopy);
				}
				
				WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				
				if (QDIsPortBuffered(GetDialogPort(MozartDlog)))
					QDFlushPortBuffer(GetDialogPort(MozartDlog), NULL);
			}
			while (Button());
			break;
			
		case PlayM:
			bogus = -1;
			do
			{
				GetMouse(&myPt);
				
				theCell.h = PosToNote3(myPt, &theCell.v, &track);
				theCell.v = NUMBER_NOTES - theCell.v-1;
				
				if (theCell.v > 0 && theCell.v != bogus)
				{
					bogus = theCell.v;
					SelectToucheMozart(bogus+1, track);
					DoPlayInstruInt(theCell.v, curInstru - 1, 0, 0, 0xFF, &MADDriver->chan[ track], 0, 0);
				}
				DoGlobalNull();
				
				WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				
				if (QDIsPortBuffered(GetDialogPort(MozartDlog)))
					QDFlushPortBuffer(GetDialogPort(MozartDlog), NULL);
			}
			while (Button());
			
			SelectToucheMozart(-1, track);
			MADKeyOFF(MADDriver, -1);
			break;
			
		case NoteM:
			if (aNote2(theCell, &PointNote, track))
			{
				/*** Ouvre le COMMAND Dialog ***/
				
				if (theEvent.when - lastWhen <= GetDblTime())
				{
					GetNoteRect2(&destRect, theCell, track);
					
					ShowCurrentCmdNote();
					
					SetCommandTrack(track, -1);
					MADDriver->PartitionReader = theCell.h;
					SetCursor(GetQDGlobalsArrow(&qdarrow));
					
					SetPortDialogPort(MozartDlog);
				}
				else
				{
					lastWhen = theEvent.when;
					
					InvalSelectionRect();
					
					selecStart.v = selecEnd.v = theCell.v;
					selecStart.h = selecEnd.h = theCell.h;
					selecTrack = track;
					
					InvalSelectionRect();
					
					goto NewSelec;
				}
			}
			else
			{
				Pcmd		*myPcmd;
				RgnHandle	tempRgn;
				Rect		markee;
				
				myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + 1 * sizeof(Cmd));
				if (myPcmd == NULL)
				{
					MyDebugStr(__LINE__, __FILE__, "Memory WARNING");
					return;
				}
				
				myPcmd->structSize = sizeof(Pcmd) + 1 * sizeof(Cmd);
				myPcmd->tracks = 1;
				myPcmd->length = 1;
				myPcmd->trackStart = track;
				myPcmd->posStart = theCell.h;
				
				myPcmd->myCmd[ 0].ins 		= curInstru;
				myPcmd->myCmd[ 0].note 		= 95 - theCell.v;
				myPcmd->myCmd[ 0].cmd 		= curEffect;
				myPcmd->myCmd[ 0].arg 		= curArgu;
				myPcmd->myCmd[ 0].vol 		= curVol;
				
				InvalSelectionRect();
				
				selecStart.v = selecEnd.v = theCell.v;
				selecStart.h = selecEnd.h = theCell.h;
				selecTrack = track;
				
				InvalSelectionRect();
				/////////
				
				GetSelectionRect2(&markee, false);
				tempRgn = NewRgn();
				RectRgn(tempRgn, &markee);
				
				if (myPcmd != NULL)
				{
					CalculDiffStartBox();
					
					SetCursor(&CHandCrsr);
					
					DragBox(tempRgn, myPcmd, &theEvent);
					
					DiffStartV = 0;		DiffStartH = 0;
					
					MyDisposePtr((Ptr*) &myPcmd);
				}
				
				DisposeRgn(tempRgn);
			}
			break;
			
		case CrossM:
			if (PointSelection(myPt))	NewSelec:
			{
				Point		myCPt;
				RgnHandle	tempRgn;
				Pcmd		*myPcmd;
				Rect		markee;
				
				SetCursor(&CHandCrsr);
				
				myCPt = myPt;
				while (Button())
				{
					GetMouse(&myPt);
					if (myCPt.h != myPt.h || myCPt.v != myPt.v) break;
				}
				if (!Button()) goto newSelec2;
				
				GetSelectionRect2(&markee, false);
				tempRgn = NewRgn();
				RectRgn(tempRgn, &markee);
				
				myPcmd = CreatePcmdFromSelectionMozart();
				if (myPcmd != NULL)
				{
					CalculDiffStartBox();
					
					DragBox(tempRgn, myPcmd, &theEvent);
					
					DiffStartV = 0;		DiffStartH = 0;
					
					MyDisposePtr((Ptr*) &myPcmd);
				}
				
				DisposeRgn(tempRgn);
			}
			else newSelec2:
			{
				InvalSelectionRect();
				ResetSelection();
				
				SelectNotes();
			}
			break;
	}
}

void DoItemPressMozart(short whichItem, DialogPtr whichDialog)
{
	Point				myPt, theCell;
	short				bogus, track = 0, i, itemType, oldPosM, xVal, yVal;
	ControlHandle		theControl;
	GrafPtr				SavePort;
	Rect				tempRect, itemRect;
	Handle				itemHandle;
	long				mresult;
	Boolean				IsPlay;
	
	GetPort(&SavePort);
	SetPortDialogPort(whichDialog);
	
	if (theEvent.what == mouseDown) {
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		//ctlPart = FindControl(myPt, GetDialogWindow(whichDialog), &theControl);
		
		theControl = NULL;
		if (TestControl(c2h, myPt))
			theControl = c2h;
		if (TestControl(c3h, myPt))
			theControl = c3h;
		if (TestControl(c1h, myPt))
			theControl = c1h;
		
		if (GetControlReference(theControl) == 1 || theControl == c2h || theControl == c3h)
		{
#if 0
			if (ctlPart == kControlIndicatorPart && gUseControlSize == false)
			{
				RgnHandle	saveClipRgn = NewRgn();
				
				GetClip(saveClipRgn);
				
				if (GetControlReference(theControl) == 1)
				{
					Rect tempRect;
					
					GetMozartTrackRect(&tempRect, 0);
					tempRect.left = 0;		tempRect.right = MozartRect.left;
					
					ClipRect(&tempRect);
				}
				
				bogus = TrackControl(theControl, myPt, NULL);
				
				SetClip(saveClipRgn);
				DisposeRgn(saveClipRgn);
				
				if (bogus != 0)
				{
					ComputeGrilleGWorld();
					UpdateMozartInfo();
				}
			}
			else if (ctlPart > 0)
#endif
			{
				gThumbPrev = GetControlValue(theControl);
				TrackControl(theControl, myPt, MyControlUPP);
			}
		} else if (PtInRect(myPt, &MozartRect))
			PressPartition(myPt);
		else {
			tempRect.left = 0;
			tempRect.right = MozartRect.left;
			tempRect.top = MozartRect.top;
			tempRect.bottom = MozartRect.bottom;
			
			if (PtInRect(myPt, &tempRect)) {
				theCell.h = PosToNote3(myPt, &theCell.v, &track);
				
				if ((theEvent.modifiers & cmdKey) != 0) {			// Mute
					MADDriver->Active[ track] = !MADDriver->Active[ track];
					
					UPDATE_TrackActive();
				} else if ((theEvent.modifiers & optionKey) != 0) {	// Solo
					short	noActive;
					
					for (i = 0, noActive = 0; i < curMusic->header->numChn; i++) {
						if (MADDriver->Active[i] == true) {
							noActive++;
						}
					}
					
					if (noActive <= 1 && MADDriver->Active[track] == true) {
						for (i = 0, noActive = 0; i < curMusic->header->numChn; i++)
							MADDriver->Active[ i] = true;
					} else {
						for (i = 0; i < curMusic->header->numChn; i++)
							MADDriver->Active[i] = false;
						MADDriver->Active[track] = true;
					}
					
					UPDATE_TrackActive();
				}
			}
			
		}
	}
	
	switch (whichItem) {
			MenuHandle	tMenu;
			Rect	caRect;
			
		case 18:
			tMenu = GetMenu(161);
			
			InsertMenu(tMenu, hierMenu);
			GetDialogItem(MozartDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;
			myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			mresult = PopUpMenuSelect(	tMenu,
									  myPt.v,
									  myPt.h,
									  curArgu + 1);
			
			if (HiWord(mresult) != 0) {
				curArgu = (Byte)LoWord(mresult) - 1;
				
				DrawNewNote();
			}
			DeleteMenu(GetMenuID(tMenu));
			DisposeMenu(tMenu);
			break;
			
		case 19:
			tMenu = GetMenu(161);
			
			InsertMenu(tMenu, hierMenu);
			GetDialogItem(MozartDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;
			myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			if (curVol == 0xFF)
				curVol = 0;
			
			mresult = PopUpMenuSelect(tMenu,
									  myPt.v,
									  myPt.h,
									  curVol + 1);
			
			if (HiWord(mresult) != 0) {
				curVol = (Byte)LoWord(mresult) - 1;
				if (curVol < 0x10)
					curVol = 0xFF;
				
				DrawNewNote();
			}
			DeleteMenu(GetMenuID(tMenu));
			DisposeMenu(tMenu);
			break;
			
		case 17:
			InsertMenuItem(InstruMenu, "\pNo Ins", 0);
			
			InsertMenu(InstruMenu, hierMenu );
			GetDialogItem(MozartDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;
			myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(InstruMenu, curInstru + 1, 0xa5);
			
			mresult = PopUpMenuSelect(InstruMenu,
									  myPt.v,
									  myPt.h,
									  curInstru + 1);
			
			SetItemMark(InstruMenu, curInstru + 1, 0);
			
			if (HiWord(mresult) != 0) {
				curInstru = (Byte)LoWord(mresult) - 1;
				
				DrawNewNote();
				
				NSelectInstruList(curInstru - 1, -1);
			}
			DeleteMenu(GetMenuID(InstruMenu));
			DeleteMenuItem(InstruMenu, 1);
			break;
			
		case 14:
			GetMouse(&myPt);
			
			GetPortBounds(GetDialogPort(MozartDlog), &caRect);
			
			if (myPt.h < caRect.right - 15 && myPt.h > MozartRect.left) {
				Boolean	Jump2 = MADDriver->JumpToNextPattern;
				
				IsPlay = MusicPlayActive;
				if (IsPlay == false) DoPlay();
				MADDriver->JumpToNextPattern = false;
				oldPosM = -1;
				
				do {
					GetMouse(&myPt);
					if (myPt.h < caRect.right - 15 && myPt.h > MozartRect.left)
					{
						xVal = PosToNote3(myPt, &yVal, &track);
						
						if (xVal != oldPosM) {
							MADDriver->PartitionReader = oldPosM = xVal;
						}
						
						
					}
					DoGlobalNull();
					
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					
					if (QDIsPortBuffered(GetDialogPort(MozartDlog)))
    					QDFlushPortBuffer(GetDialogPort(MozartDlog), NULL);
					
				}while (Button());
				
				if (IsPlay == false)
					DoPause();
				MADDriver->JumpToNextPattern = Jump2;
			}
			break;
			
		case 11:
			bogus = -1;
			
			GetPortBounds(GetDialogPort(MozartDlog), &caRect);
			
			do {
				GetMouse(&myPt);
				if (myPt.v < caRect.bottom - 15 && myPt.v > MozartRect.top) {
					theCell.h = PosToNote3(myPt, &theCell.v, &track);
					theCell.v = NUMBER_NOTES - theCell.v-1;
					
					if (theCell.v > 0 && theCell.v != bogus) {
						bogus = theCell.v;
						SelectToucheMozart(bogus+1, track);
						DoPlayInstruInt(theCell.v, curInstru - 1, 0, 0, 0xFF, &MADDriver->chan[ track], 0, 0);
					}
				}
				DoGlobalNull();
				
				WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				
				if (QDIsPortBuffered(GetDialogPort(MozartDlog)))
					QDFlushPortBuffer(GetDialogPort(MozartDlog), NULL);
			} while (Button());
			
			SelectToucheMozart(-1, track);
			MADKeyOFF(MADDriver, -1);
			break;
			
		case HelpM:
			if (MyTrackControl(HelpBut, theEvent.where, NULL)) {
				DialogPatternInfo(PatCopy);
			}
			break;
			
		case PrefM:
			if (MyTrackControl(prefBut, theEvent.where, NULL))
			{
#include "Help.h"
				
				ShowPrefs(CLASSICAL);
			}
			break;
			
		case 20:	// Effect
			InsertMenu(EffectMenu, hierMenu );
			GetDialogItem(MozartDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;
			myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(EffectMenu, curEffect + 1, 0xa5);
			
			mresult = PopUpMenuSelect(EffectMenu,
									  myPt.v,
									  myPt.h,
									  curEffect + 1);
			
			SetItemMark(EffectMenu, curEffect + 1, 0);
			
			if (HiWord(mresult) != 0) {
				curEffect = (Byte)LoWord(mresult) - 1;
				
				DrawNewNote();
			}
			DeleteMenu(GetMenuID(EffectMenu));
			break;
			
		case NoteM:
		case CrossM:
		case TrashM:
		case PlayM:
		case ZoomM:
			copyMode = Mode = whichItem;
			
			SelectGoodMode();
			break;
			
#if 0
		case SeeAllM:
			if (MyTrackControl(SeeAll, theEvent.where, NULL))
			{
				if (showAllNotes == false)
				{
					HiliteControl(SeeAll, kControlButtonPart);
					showAllNotes = true;
				}
				else
				{
					HiliteControl(SeeAll, 0);
					showAllNotes = false;
				}
				UpdateMozartInfo();
			}
			break;
#endif
	}
	
	SetPort(SavePort);
}

void SetUpLabels()
{
	Rect			itemRect;
	Handle			itemHandle;
	short			itemType, temp, tt, add;
	FontInfo		fInfo;
	Str255			String;
	GWorldPtr 		gMyOffG = NULL;
	GWorldPtr 		currPort;
	GDHandle 		currDev;
	
	/** YLabel **/
	
	GetGWorld(&currPort, &currDev);
	
	GetDialogItem (MozartDlog, 11, &itemType, &itemHandle, &itemRect);
	
	itemRect.bottom = itemRect.top + 1 + MOHaut*NUMBER_NOTES;
	//SetRect(&itemRect, 0, 0, LegendeLa, 1 + MOHaut*NUMBER_NOTES);
	NewOffscreenBitMap(&yLabel, &itemRect);
	
	NewGWorld(&gMyOffG, 1, &itemRect, nil, nil, 0);
	LockPixels(GetGWorldPixMap(gMyOffG));
	SetGWorld(gMyOffG, NULL);
	
	EraseRect(&itemRect);
	
	TextFont(4);
	TextSize(9);
	
	GetFontInfo(&fInfo);
	MidMOHaut = (MOHaut - fInfo.ascent)/2;
	MidMOHaut--;
	
	if (MOHaut >= 8)
	{
		for (tt = 0; tt <= NUMBER_NOTES; tt++)
		{
			MoveTo(itemRect.left + 1, itemRect.top + tt*MOHaut - MidMOHaut -2);
			
			GetNoteString(NUMBER_NOTES - tt, String);
			DrawString(String);
		}
	}
	else
	{
		for (tt = 0; tt < 8; tt++)
		{
			MoveTo(itemRect.left, 	itemRect.top  + tt*12*MOHaut);
			LineTo(itemRect.right,	itemRect.top  + tt*12*MOHaut);
			
			MoveTo(itemRect.left + 4, itemRect.top + (6 + tt*12)*MOHaut - MidMOHaut);
			
			NTStr(2, 7 - tt , (Ptr) String);
			DrawString(MyC2PStr((Ptr) String));
		}
		
	}
	
	CopyBits((BitMap *) (*(GetGWorldPixMap(gMyOffG))),
			 &yLabel,
			 &itemRect,
			 &itemRect,
			 srcCopy,
			 nil);
	
	SetGWorld (currPort, currDev);
	
	UnlockPixels (GetGWorldPixMap(gMyOffG));
	DisposeGWorld(gMyOffG);
	
	/** XLabel **/
	
	SetRect(&itemRect, 0, 0, MOLarg * curMusic->partition[ PatCopy]->header.size, LegendeHi);
	NewOffscreenBitMap(&xLabel, &itemRect);
	
	NewGWorld(&gMyOffG, 1, &itemRect, nil, nil, 0);
	LockPixels (GetGWorldPixMap(gMyOffG));
	SetGWorld (gMyOffG, nil);
	
	TextFont(4);
	TextSize(9);
	
	EraseRect(&itemRect);
	
	GetFontInfo(&fInfo);
	
	if (curMusic->partition[ PatCopy]->header.size > 100) { MidMOLarg = (MOLarg - fInfo.widMax * 3)/2;	temp = 3; }
	else { MidMOLarg = (MOLarg - fInfo.widMax * 2)/2;	temp = 2; }
	
	if (temp <= 2)
	{
		if (MOLarg > 8) 		add = 1;
		else if (MOLarg > 5)	add = 2;
		else 					add = 3;
	}
	else if (temp == 3)
	{
		if (MOLarg > 16) 		add = 1;
		else if (MOLarg > 8) 	add = 2;
		else if (MOLarg > 5)	add = 3;
		else 					add = 4;
	}
	
	for (tt = 0; tt < curMusic->partition[ PatCopy]->header.size; tt += add)
	{
		if (add != 1)
		{
			MoveTo(tt*MOLarg + MOLarg/2, 11);
			LineTo(tt*MOLarg + MOLarg/2, 11);
		}
		
		MoveTo(tt*MOLarg + MidMOLarg + 1, 10);
		
		NTStr(temp, tt , (Ptr)String);
		DrawString(MyC2PStr((Ptr)String));
	}
	
	MidMOLarg = (MOLarg - fInfo.widMax * 2)/2;
	
	CopyBits((BitMap *) (*(GetGWorldPixMap(gMyOffG))),
			 &xLabel,
			 &itemRect,
			 &itemRect,
			 srcCopy,
			 nil);
	
	SetGWorld (currPort, currDev);
	
	UnlockPixels (GetGWorldPixMap(gMyOffG));
	DisposeGWorld(gMyOffG);
}

void CreateMozartWindow(void)
{
	Rect			itemRect, tempRect;
	Handle			itemHandle;
	short			itemType;
	CursHandle		tempCur;
	
	if (MozartDlog != NULL)
	{
		SetWindEtat(GetDialogWindow(MozartDlog));
		return;
	}
	
	YSize = 128;
	
	SetItemMark(EditorMenu, 2, checkMark);
	
	pStrcpy(curDisplayedNote, "\p");
	
	globalGWorld = NULL;
	grilleGWorld = NULL;
	SetRect(&tempRect, 0, 0, 1, 1);
	NewGWorld(&globalGWorld, 0, &tempRect, nil, nil, 0);
	LockPixels (GetGWorldPixMap(globalGWorld));
	
	ZoomInter = thePrefs.MozartX;
	MOLarg = MOLargList[ ZoomInter];
	MOHaut = MOHautList[ ZoomInter];
	
	PianoMozart = -1;
	
	Gray[ 0].red = Gray[ 0].green = Gray[ 0].blue = 48059;
	Gray[ 1].red = Gray[ 1].green = Gray[ 1].blue = 43690;
	Gray[ 2].red = Gray[ 2].green = Gray[ 2].blue = 34952;
	Gray[ 3].red = Gray[ 3].green = Gray[ 3].blue = 30583;
	
	yellC.red 	= 65535;
	yellC.green = 65535;
	yellC.blue 	= 39321;
	
	greenC.red 		= 39321;
	greenC.green 	= 65535;
	greenC.blue		= 39321;
	
	blueC.red 		= 26214;
	blueC.green 	= 65535;
	blueC.blue		= 65535;
	
	redC.red		= 65535;
	redC.green		= 13107;
	redC.blue		= 39321;
	
	showAllNotes = false;
	
	MozartDlog = GetNewDialog(135, NULL, GetDialogWindow(ToolsDlog));
	SetWindEtat(GetDialogWindow(MozartDlog));
	ShowWindow(GetDialogWindow(MozartDlog));
	SetPortDialogPort(MozartDlog);
	SelectWindow2(GetDialogWindow(MozartDlog));
	
	TextFont(kFontIDGeneva);
	TextSize(9);
	
	SetRect(&itemRect, 0, 0, -30, -16);
	c1h = NewControl(GetDialogWindow(MozartDlog),
					 &itemRect,
					 "\p.",
					 true,
					 0,
					 0,
					 0,
					 gScrollBarID,
					 1);
	
	SetRect(&itemRect, 0, 0, -16, -30);
	c2h = NewControl(GetDialogWindow(MozartDlog),
					 &itemRect,
					 "\p.",
					 true,
					 0,
					 0,
					 0,
					 gScrollBarID,
					 2);
	
	SetRect(&itemRect, 0, 0, -16, -30);
	c3h = NewControl(GetDialogWindow(MozartDlog),
					 &itemRect,
					 "\p.",
					 true,
					 0,
					 0,
					 0,
					 gScrollBarID,
					 3);
	
	MyControlUPP = NewControlActionUPP(actionProc);
	
	GetDialogItem (MozartDlog, 9, &itemType, &itemHandle, &itemRect);
	SetRect(&MozartRect, itemRect.left, itemRect.top, 0, 0);
	MozartRect.right = MozartRect.left + MOLarg * curMusic->partition[ MADDriver->Pat]->header.size;
	MozartRect.bottom = MozartRect.top + MOHaut * NUMBER_NOTES;
	
	
	GetDialogItem(MozartDlog, TrashM, &itemType, &itemHandle, &TrashRect);
	
	GetDialogItem(MozartDlog , TrashM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	PoubBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 150,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(MozartDlog , NoteM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	NoteBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 140,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(MozartDlog , ZoomM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ZoomBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 177,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	
	GetDialogItem(MozartDlog , HelpM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	HelpBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 149,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(MozartDlog , PrefM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	prefBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 174,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	
	/*	GetDialogItem(MozartDlog , SeeAllM, &itemType, &itemHandle, &itemRect);
	 //itemRect.right = itemRect.left;
	 SeeAll = NewControl(	GetDialogWindow(MozartDlog),
	 &itemRect,
	 "\p.",
	 true,
	 169,		//icl8 id
	 -32768,
	 32767,
	 512,
	 0);	*/
	
	GetDialogItem(MozartDlog , CrossM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	SelBut = NewControl(GetDialogWindow(MozartDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						156,
						kControlBevelButtonNormalBevelProc,
						0);
	
	GetDialogItem(MozartDlog , PlayM, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	PlayBut = NewControl(GetDialogWindow(MozartDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 160,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	copyMode = Mode = NoteM;
	HiliteControl(NoteBut, kControlButtonPart);
	
	tempCur = GetCursor(crossCursor);
	CrossCrsr = **tempCur;
	
	ReaderCopy = MADDriver->PartitionReader;
	PatCopy = MADDriver->Pat;
	
	if (!GetIns(&curInstru, NULL)) curInstru = 0;
	curInstru++;
	
	curEffect 		= 0;
	curArgu 		= 0;
	curVol 			= 0xFF;
	
	SetWMozart(MADDriver->Pat);
	SetUpTracksMenu();
	
	ResetSelection();
	
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	SetWinControl(caRect.right, caRect.bottom, MozartDlog);
	
	SetControlValue(c1h, thePrefs.MozartC1h);
	SetControlValue(c2h, thePrefs.MozartC2h);
	
	UpdateInstruMenu();
	
	SetUpLabels();
	
	UpdateMozartInfoInternal();
	
	if (DragManagerUse)
	{
		MyTrackingHandlerUPP 		= NewDragTrackingHandlerUPP(MyTrackingBox);
		MyReceiveDropHandlerUPP 	= NewDragReceiveHandlerUPP(MyReceiveDropBox);
		mySendDataUPP 				= NewDragSendDataUPP(MySendDataProcEditor);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingHandlerUPP, GetDialogWindow(MozartDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveDropHandlerUPP, GetDialogWindow(MozartDlog), (void *) NULL);
	}
}

void CloseMozartWindow(void)
{
	if (MozartDlog != NULL)
	{
		thePrefs.MozartC1h = GetControlValue(c1h);
		thePrefs.MozartC2h = GetControlValue(c2h);
		
		if (DragManagerUse)
		{
			RemoveTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(MozartDlog));
			RemoveReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(MozartDlog));
		
			DisposeDragTrackingHandlerUPP(MyTrackingHandlerUPP);
			DisposeDragReceiveHandlerUPP(MyReceiveDropHandlerUPP);
			DisposeDragSendDataUPP(mySendDataUPP);
		}
		
		DisposeDialog(MozartDlog);
		ZapBitMap(&xLabel);
		ZapBitMap(&yLabel);
		
		UnlockPixels (GetGWorldPixMap(globalGWorld));
		DisposeGWorld(globalGWorld);
		globalGWorld = NULL;
		
		UnlockPixels(GetGWorldPixMap(grilleGWorld));
		DisposeGWorld(grilleGWorld);
		grilleGWorld = NULL;
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		DisposeControlActionUPP(MyControlUPP);
	}
	MozartDlog = NULL;
	
	SetItemMark(EditorMenu, 2, noMark);
}

void DoKeyPressMozart(short theChar)
{
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(MozartDlog);
	
	if (theChar ==  9)
	{
		Mode++;
		if (Mode > MODESIZE) Mode = 1;
		
		copyMode = Mode;
		SelectGoodMode();
	}
	else if (theChar == 0x08)
	{
		SaveUndo(UPattern, PatCopy, "\pUndo 'Delete selection'");
		
		DeleteSelecNote(selecStart.h, selecEnd.h);
	}
	
	SetPort(savePort);
}

Pcmd* CreatePcmdFromSelectionMozart(void)
{
	Cmd					*theCommand;
	Pcmd				*myPcmd;
	short				count, i, Pos;
	
	count = selecEnd.h - selecStart.h + 1;
	if (count <= 0) return NULL;
	
	myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + count * sizeof(Cmd));
	if (myPcmd == NULL) MyDebugStr(__LINE__, __FILE__, "Memory WARNING");
	
	myPcmd->structSize = sizeof(Pcmd) + count * sizeof(Cmd);
	myPcmd->tracks = 1;
	myPcmd->length = count;
	myPcmd->trackStart = selecTrack;
	myPcmd->posStart = selecStart.h;
	
	count = 0;
	for (i = selecStart.h; i <= selecEnd.h; i++)
	{
		theCommand = GetMADCommand(i, selecTrack, curMusic->partition[ PatCopy]);
		Pos = NUMBER_NOTES - theCommand->note-1;
		
		if (Pos >= selecStart.v && Pos <= selecEnd.v)
		{
			myPcmd->myCmd[ count] = *theCommand;
		}
		else
		{
			MADKillCmd(&myPcmd->myCmd[ count]);
		}
		count++;
	}
	return myPcmd;
}

void COPYMozart(void)
{
	Pcmd				*myPcmd;
	Ptr					myText;
	OSErr				anErr;
	ScrapRef			scrap;
	
	myPcmd = CreatePcmdFromSelectionMozart();
	if (myPcmd == NULL) return;
	
	SetCursor(&watchCrsr);
	
	anErr = ClearCurrentScrap();
	anErr = GetCurrentScrap(&scrap);
	anErr = PutScrapFlavor(scrap, 'Pcmd', 0, GetPtrSize((Ptr) myPcmd),  (Ptr) myPcmd);
	
	myText = ConvertPcmd2Text(myPcmd);
	
	anErr = PutScrapFlavor(scrap, 'TEXT', 0, GetPtrSize(myText),  myText);
	
	MyDisposePtr(& myText);
	MyDisposePtr((Ptr*) &myPcmd);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
}

void GetMinMaxPcmd(Pcmd *myPcmd, short *minNote, short *maxNote)
{
	short	i;
	
	for (i = 0, *maxNote = 0, *minNote = 2000; i < myPcmd->length; i++)
	{
		if (myPcmd->myCmd[ i].note != 0xFF)
		{
			if (myPcmd->myCmd[ i].note > *maxNote) *maxNote = myPcmd->myCmd[ i].note;
			if (myPcmd->myCmd[ i].note >= 0 && myPcmd->myCmd[ i].note < *minNote) *minNote = myPcmd->myCmd[ i].note;
		}
	}
	
	if (*minNote == 2000) *minNote = *maxNote;
}

void PasteCmdBox(Pcmd *myPcmd)
{
	short				minNote, temp, maxNote;
	Cmd					*cmd;
	
	GetMinMaxPcmd(myPcmd, &minNote, &maxNote);
	
	for (temp = 0; temp < myPcmd->length; temp++)
	{
		if (selecStart.h + temp >= 0 && selecStart.h + temp < curMusic->partition[ PatCopy]->header.size)
		{
			cmd = GetMADCommand(selecStart.h + temp, selecTrack, curMusic->partition[ PatCopy]);
			
			if (myPcmd->myCmd[ temp].note != 0xFF || myPcmd->myCmd[ temp].ins > 0)
			{
				BlockMoveData(&myPcmd->myCmd[ temp], cmd, sizeof(Cmd));
				
				if (cmd->note != 0xFF) cmd->note += NUMBER_NOTES - selecStart.v - maxNote-1;
			}
			
			UPDATE_NoteBOUCLE(selecStart.h + temp, selecTrack);
		}
	}
	
	UPDATE_NoteFINISH();
	
	InvalSelectionRect();
	
	selecEnd.h = selecStart.h + myPcmd->length - 1;
	selecEnd.v = selecStart.v + maxNote - minNote;
	if (selecEnd.h >= curMusic->partition[ PatCopy]->header.size) selecEnd.h = curMusic->partition[ PatCopy]->header.size - 1;
	
	InvalSelectionRect();
}

void PASTEMozart(void)
{
	long				lCntOrErr;
	Handle				theHandle;
	GrafPtr				savePort;
	Pcmd				*myPcmd;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	OSErr				anErr;
	
	GetPort(&savePort);
	SetPortDialogPort(MozartDlog);
	
	lCntOrErr = 0;
	anErr = GetCurrentScrap(&scrap);
	anErr = GetScrapFlavorFlags(scrap, 'Pcmd', &flags);
	if (anErr == noErr) GetScrapFlavorSize(scrap, 'Pcmd', &lCntOrErr);
	
	if (lCntOrErr > 0)
	{
		SaveUndo(UPattern, PatCopy, "\pUndo 'Paste Classical Editor'");
		
		theHandle = MyNewHandle(lCntOrErr);
		
		HLock(theHandle);
		GetScrapFlavorData(scrap, 'Pcmd', &lCntOrErr, *theHandle);
		
		myPcmd = (Pcmd*) *theHandle;
		PasteCmdBox(myPcmd);
		
		HUnlock(theHandle);
		MyDisposHandle(& theHandle);
	}
	SetPort(savePort);
}

void OpenPcmdMozart(FSSpec *mySpec)
{
	//StandardFileReply	reply;
	OSErr				iErr;
	long				inOutBytes;
	short				fRefNum;
	Pcmd				*myPcmd;
	
	if (mySpec == NULL)
	{
		SaveUndo(UPattern, PatCopy, "\pUndo 'Open Pcmd File'");
		
		iErr = DoStandardOpen(mySpec, "\pPcmd file", 'Pcmd');
		
		if (iErr) return;
	}
	
	if (FSpOpenDF(mySpec, fsCurPerm, &fRefNum) == noErr)
	{
		GetEOF(fRefNum, &inOutBytes);
		myPcmd = (Pcmd*) MyNewPtr(inOutBytes);
		iErr = FSRead(fRefNum, &inOutBytes, (Ptr) myPcmd);
		
		PasteCmdBox(myPcmd);
		
		curMusic->hasChanged = true;
		
		iErr = FSCloseFork(fRefNum);
	}
}
////

static	short			PcmdTracks, YMove, PcmdLength, PcmdHigh, DropX, DropY, DropTrack;
static	Rect			dragRect, prevClipRect;
static	ControlHandle	selectedControl;
static	Boolean			canAcceptDrag;

static	Rect			pSelecRectX, pSelecRectY;
static	short			pTrack;

void CleanInvert(Rect *a, Rect *b)
{
	RgnHandle	tempRgn = NewRgn();
	Rect		c;
	
	OpenRgn();
	FrameRect(a);	FrameRect(b);
	CloseRgn(tempRgn);
	
	if (SectRect(a, b, &c))
	{
		RgnHandle	r1;
		
		RectRgn(r1 = NewRgn(), &c);
		
		DiffRgn(tempRgn, r1, tempRgn);
		
		DisposeRgn(r1);
	}
	
	LMSetHiliteMode(LMGetHiliteMode() & hiliteBit);
	InvertRgn(tempRgn);
	DisposeRgn(tempRgn);
}

void EraseMark()
{
	Rect v = {0, 0, 0, 0};
	
	CleanInvert(&pSelecRectX, &v);
	CleanInvert(&pSelecRectY, &v);
	
	SetRect(&pSelecRectX, 0, 0, 0, 0);
	SetRect(&pSelecRectY, 0, 0, 0, 0);
}

void DragMark2(short track)
{
	Rect	xRect, yRect;
	Rect	maxRectX, maxRectY;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(MozartDlog), &caRect);
	
	SetRect(&yRect,
			MozartRect.left - LegendeLaS,
			MozartRect.top + (DropY*MOHaut - GetControlValue(c1h)) + (track - GetControlValue(c3h))*YSize,
			MozartRect.left,
			MozartRect.top + (DropY*MOHaut - GetControlValue(c1h)) + (track - GetControlValue(c3h))*YSize + MOHaut*(selecEnd.v - selecStart.v + 1) + 1);
	if (yRect.bottom > caRect.bottom -15)
		yRect.bottom = caRect.bottom -15;
	
	SetRect(&xRect,
			MozartRect.left + (DropX*MOLarg - GetControlValue(c2h)),
			MozartRect.top - LegendeHi + 1,
			MozartRect.left + (DropX*MOLarg - GetControlValue(c2h)) + MOLarg*(selecEnd.h - selecStart.h + 1) + 1,
			MozartRect.top );
	if (xRect.right > caRect.right -15)
		xRect.right = caRect.right -15;
	
	SetRect(&maxRectX, MozartRect.left + 1, MozartRect.top - LegendeHi, caRect.right-15, MozartRect.top);
	SectRect(&maxRectX, &xRect, &xRect);
	
	CleanInvert(&pSelecRectX, &xRect);
	
	SetRect(&maxRectY, MozartRect.left-LegendeLa, MozartRect.top + 1 + (track - GetControlValue(c3h))*YSize, MozartRect.left, MozartRect.top + 1 + (track - GetControlValue(c3h))*YSize + YSize);
	SectRect(&maxRectY, &yRect, &yRect);
	
	CleanInvert(&pSelecRectY, &yRect);
	
	pSelecRectX = xRect;	pSelecRectY = yRect;		pTrack = track;
}

pascal OSErr MyTrackingBox(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	short				result, fRefNum;
	long				textSize, inOutBytes;
	unsigned long		attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse;
	Rect				tempRect;
	FlavorFlags     	theFlags;
	Pcmd				*myPcmd;
	RgnHandle			saveClip;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	OSErr				iErr;
	
	if (!mainSystemDrag)
		return noErr;
	
	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag))
		return noErr;
	
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {
		case kDragTrackingEnterHandler:
			SetRect(&prevClipRect, 0, 0, 0, 0);
			SetRect(&pSelecRectX, 0, 0, 0, 0);
			SetRect(&pSelecRectY, 0, 0, 0, 0);
			
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
			
			result = GetFlavorFlags(theDrag, theItem, 'Pcmd', &theFlags);
			if (result == noErr) {
				canAcceptDrag = true;
				
				result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
				if (result == noErr) {
					short minNote, maxNote;
					
					////////////
					myPcmd = (Pcmd*) MyNewPtr(textSize);
					GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
					PcmdTracks = myPcmd->tracks;
					if (PcmdTracks <= 0)
						PcmdTracks = 1;
					PcmdLength = myPcmd->length;
					if (PcmdLength <= 0)
						PcmdLength = 1;
					GetMinMaxPcmd(myPcmd, &minNote, &maxNote);
					if (attributes & kDragInsideSenderWindow) {
						PcmdHigh = selecEnd.v - selecStart.v + 1;
						YMove = NUMBER_NOTES - selecStart.v - maxNote -1;
					} else {
						PcmdHigh = maxNote - minNote + 1;
					}
					MyDisposePtr((Ptr*)&myPcmd);
					/////////////
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
				
				switch (fndrInfo.fdType) {
					case 'Pcmd':
						/////////////
						PcmdTracks = 1;	PcmdLength = 1;
						iErr = FSpOpenDF(&myFlavor.fileSpec, fsCurPerm, &fRefNum);
						if (iErr == noErr)
						{
							short minNote, maxNote;
							
							GetEOF(fRefNum, &inOutBytes);
							myPcmd = (Pcmd*) MyNewPtr(inOutBytes);
							FSRead(fRefNum, &inOutBytes, (Ptr) myPcmd);
							PcmdTracks = myPcmd->tracks;
							PcmdLength = myPcmd->length;
							GetMinMaxPcmd(myPcmd, &minNote, &maxNote);
							if (attributes & kDragInsideSenderWindow)
							{
								PcmdHigh = selecEnd.v - selecStart.v + 1;
								YMove = minNote - selecStart.v;
							}
							else
							{
								PcmdHigh = maxNote - minNote + 1;
							}
							FSCloseFork(fRefNum);
							MyDisposePtr((Ptr*) &myPcmd);
						}
						///////////////
						
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
			
			if (attributes & kDragInsideSenderWindow)
			{
				Rect bounds;
				
				if (PtInRect(localMouse, GetControlBounds(PoubBut, &bounds)) && selectedControl == NULL)	{ HiliteControl(PoubBut, kControlButtonPart);		selectedControl = PoubBut;}
				else HiliteControl(PoubBut, 0);
			}
			
			saveClip = NewRgn();
			GetClip(saveClip);
			
			if (PtInRect(localMouse, &MozartRect))
			{
				short 	pos, track, note;
				Rect	trackRect;
				
				pos = PosToNote3(localMouse, &note, &track);
				
				GetMozartTrackRect(&trackRect, track);
				
				pos += DiffStartV;
				note += DiffStartH;
				
				tempRect.left = trackRect.left + (pos * MOLarg - GetControlValue(c2h));
				tempRect.top = trackRect.top + (note*MOHaut - GetControlValue(c1h));
				tempRect.right = tempRect.left + PcmdLength*MOLarg + 1;
				tempRect.bottom = tempRect.top + PcmdHigh*MOHaut + 1;
				
				DropX = pos;	DropY = note;	DropTrack = track;
				
				if (tempRect.left != dragRect.left || tempRect.top != dragRect.top)
				{
					theRgn = NewRgn();
					
					RectRgn(theRgn, &tempRect);
					
					DragMark2(track);
					
					if (!EqualRect(&trackRect, &prevClipRect))
					{
						ClipRect(&prevClipRect);
						HideDragHilite(theDrag);
					}
					
					ClipRect(&trackRect);
					
					ShowDragHilite(theDrag, theRgn, true);
					DisposeRgn(theRgn);
					
					dragRect = tempRect;
					prevClipRect = trackRect;
				}
			}
			else
			{
				DropX = DropY = DropTrack = -1;
				HideDragHilite(theDrag);
				EraseMark();
			}
			
			RGBBackColor(&theColor);
			
			SetClip(saveClip);
			DisposeRgn(saveClip);
			break;
			
		case kDragTrackingLeaveWindow:
			BackColor(whiteColor);
			HideDragHilite(theDrag);
			EraseMark();
			RGBBackColor(&theColor);
			break;
			
		case kDragTrackingLeaveHandler:
			break;
	}
	
	return(noErr);
}

Boolean DragBox(RgnHandle myRgn, Pcmd	*myPcmd, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	Ptr					myText;
	PromiseHFSFlavor	myNewFile;
	Rect				dragRegionRect;
	
	if (!DragManagerUse)
		return false;
	
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
	
	SetDragItemBounds(theDrag, 1, GetRegionBounds(dragRegion, &dragRegionRect));
	
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

pascal OSErr MyReceiveDropBox(WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	OSErr				result;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				mouseDownModifiers, mouseUpModifiers, movePcmd;
	Pcmd				*myPcmd;
	HFSFlavor			myFlavor;
	
	if (!mainSystemDrag)
		return dragNotAcceptedErr;
	if (!canAcceptDrag)
		return dragNotAcceptedErr;
	if (DropX == -1 && DropY == -1 && DropTrack == -1 && selectedControl == NULL)
		return dragNotAcceptedErr;
	
	SetPortWindowPort(theWindow);
	
	DiffStartV = 0;
	DiffStartH = 0;
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	if (attributes & kDragInsideSenderWindow) {
		if (selectedControl != NULL) {
			HiliteControl(selectedControl, 0);
			
			//if (selectedControl == saveBut)		SavePcmdFile(CreatePcmdFromSelectionMozart());
			if (selectedControl == PoubBut) 	DoKeyPressMozart(8);
			selectedControl = NULL;
			
			return noErr;
		}
	}
	
	movePcmd = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	
	BackColor(whiteColor);
	HideDragHilite(theDrag);
	EraseMark();
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
				
				DoKeyPressMozart(8);
			} else
				SaveUndo(UPattern, PatCopy, "\pUndo 'Move Selection'");
			
			InvalSelectionRect();
			
			selecStart.h = DropX;
			selecEnd.h = selecStart.h + PcmdLength-1;
			selecStart.v = DropY;
			if (attributes & kDragInsideSenderWindow)
				selecStart.v += YMove;
			selecEnd.v = selecStart.v + PcmdHigh-1;
			selecTrack = DropTrack;
			
			PasteCmdBox(myPcmd);
			MyDisposePtr((Ptr*)&myPcmd);
			
			if (attributes & kDragInsideSenderWindow) {
				selecStart.v = DropY;
				selecEnd.v = selecStart.v + PcmdHigh-1;
				InvalSelectionRect();
			}
			
			return noErr;
		}
		else return dragNotAcceptedErr;
	}
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
	if (result == noErr) {
		Boolean targetIsFolder, wasAliased;
		
		SaveUndo(UPattern, PatCopy, "\pUndo 'Drop Pcmd File'");
		
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		
		ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		InvalSelectionRect();
		
		selecStart.h	= DropX;
		selecStart.v	= DropY;
		selecTrack		= DropTrack;
		
		OpenPcmdMozart(&myFlavor.fileSpec);
		
		return noErr;
	}
	
	return dragNotAcceptedErr;
}
