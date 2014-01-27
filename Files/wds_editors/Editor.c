#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "Undo.h"
#include "PPPlug.h"
#include "PrivateList.h"

/******** HELP MODULE ********/
enum
{
	HPref	= 6,		// 1
	HLoad	= 3,		// 2
	HSave	= 4,		// 3
	HUp		= 10,		// 4
	HInfo	= 2,		// 5
	HPlay	= 7,		// 6
	HRec	= 11,		// 7
	HOpenA	= 8,		// 8
	HTrack	= 5,		// 9
	HPos	= 9,		// 10
	HPatt	= 1,		// 11
	HFX		= 12,		// 12
	HLoop	= 13,		// 13
	HFind	= 14,		// 14
	HDown	= 23,		// 15
	HFill	= 18,
	HD1		= 37,
	HD2		= 38,
	HD3		= 39,
	HD4		= 40,
	HStep	= 21
};

//static pascal Boolean myDragClickLoop(void);
static short AHelp[] =
{HPref, HLoad, HSave, HUp, HInfo, HPlay, HRec, HOpenA, HTrack, HPos, HPatt, HFX, HLoop, HFind, HDown, HFill, HD1, HD2, HD3, HD4, HStep};

void DoHelpEditor(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

	/*****************************/

extern	WindowPtr				oldWindow;
//extern	DialogPtr					InstruListDlog;
extern	KeyMap					km;
extern	short					curTrack, curActif;
extern	Boolean					DragManagerUse, PianoRecording;
extern	Cursor					ContextCrsr, PlayCrsr, HandCrsr, CHandCrsr;
extern	RGBColor				theColor;
extern	TEHandle				TEH[ 6];
extern	DialogPtr				AdapDlog;
extern	DialogPtr				theProgressDia;
		DialogPtr					EditorDlog;
		Rect						CurRect, CurChar;
static	short					previousYCurRect, ReaderCopy, LChar;
static	short					totItem, curInstru, curStep;
static	short					CurrentPat, DiffStartH, DiffStartV;
static	short					DefaultFX, DefaultArg, DefaultVol;
static	Str32					NoteCharTemp;
static	Point						CurStr, CellSelec;
static	ControlHandle				FillBut, CheckBut[ 4], OpenBut, SaveBut, InfoBut, PrefBut, ShowBut, PlayBut, DownBut, UpBut, RecBut, FXBut, TraceBut, FindBut;
static	Boolean						DidTryToDrag;
static	RGBColor					Gray[ 5];
static	RGBColor					yellC, greenC, blueC, redC;
		Boolean					EditorKeyRecording;

static	PrivateList				myList;

static	long					tLH;
static	Boolean				canAcceptDrag, OnOffInstru, OnOffFX, OnOffArg, OnOffVolume;
static	DragSendDataUPP		mySendDataUPP;
static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

#define LEFTBORD		21
#define UPSIZE			13

void ActiveCmdWindow(short whichItem);
void 			GetCurrentDlogCmd(short *pos, short *track);
Boolean 		CreateNoteString(Cmd		*theCommand, Str255 mainStr, Boolean);
pascal OSErr	MyTrackingEditor(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr	MyReceiveDropEditor(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
pascal OSErr 	MySendDataProcEditor(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag);
Boolean 		DragEditor(RgnHandle myRgn, Pcmd	*myPcmd, EventRecord *theEvent);
Pcmd*		CreatePcmdFromSelection();
void 			DrawLeft(RgnHandle, Boolean);
void			SaveUndo(short UndoType, short ID, Str255 textMenu);
void			ShowPrefs(short PrefStartup);
void			DrawEditorUp();
void			PasteCmdEditor(Point theCell, Pcmd *myPcmd);
void			PrintTEHandle(TEHandle hTE);
void			ApplyBut(short value);
void			SetMobiusRect(Rect *rect, short left, short top, short right, short bottom);
void			InternalEditorInit();
OSErr		CallPPDGPlugIns(short PlugNo, Pcmd *myPcmd);
Cmd* 		GetCmdDlogActif();
void UPDATE_TrackActive(void);
void DoNullEditor(void);
void MyLUpdate();
void MyLUpdateSpecific(short startY, short endY);
void CreatePartitionWindow(void);
void ClosePartitionWindow(void);
void DoKeyPressEditor(short theChar);


Cmd* GetCmd(short row, short track, Pcmd* myPcmd)
{
	if (row < 0)
		row = 0;
	else if (row >= myPcmd->length)
		row = myPcmd->length -1;

	if (track < 0)
		track = 0;
	else if (track >= myPcmd->tracks)
		track = myPcmd->tracks -1;
	
	return &(myPcmd->myCmd[(myPcmd->length * track) + row]);
}

void ConvertPointAndType(Point *cPt, short *Type)
{
	short	dist = cPt->h;
	short	ttI = 0;
	
	cPt->h -= myList.rect.left;				cPt->v -= myList.rect.top;
	cPt->h /= myList.LCell;					cPt->v /= myList.HCell;
	cPt->h += GetControlValue(myList.xScroll);	cPt->v += GetControlValue(myList.yScroll);

	// Type Identifier
	
	dist -= myList.rect.left + (cPt->h - GetControlValue(myList.xScroll)) * myList.LCell;
	
	TextFont(4);	TextSize(9);
	dist /= CharWidth(' ');
	
	*Type = -1;
	if (thePrefs.DigitalInstru && *Type == -1) {
		ttI += 4;
		if (dist < ttI)
			*Type = InstruTE;
	}
	if (thePrefs.DigitalNote && *Type == -1) {
		ttI += 4;
		if (dist < ttI)
			*Type = NoteTE;
	}
	if (thePrefs.DigitalEffect && *Type == -1) {
		ttI += 2;
		if (dist < ttI)
			*Type = EffectTE;
	}
	if (thePrefs.DigitalArgu && *Type == -1) {
		ttI += 3;
		if (dist < ttI)
			*Type = ArguTE;
	}
	if (thePrefs.DigitalVol && *Type == -1) {
		ttI += 3;
		if (dist < ttI)
			*Type = VolumeTE;
	}
	
	if (cPt->h < 0)
		cPt->h = 0;
	if (cPt->v < 0)
		cPt->v = 0;
	
	if (cPt->h >= curMusic->header->numChn)
		cPt->h = curMusic->header->numChn - 1;
	if (cPt->v >= curMusic->partition[CurrentPat]->header.size)
		cPt->v = curMusic->partition[CurrentPat]->header.size - 1;
}

Cmd * GetEditorSelectCmd(short *track, short *position)
{
	Cell	theCell = {0, 0};

	if (EditorDlog != NULL) {
		if (PLGetSelect(&theCell, &myList)) {
			*track = theCell.h;	*position = theCell.v;
			return GetMADCommand(theCell.v, theCell.h, curMusic->partition[CurrentPat]);
		} else
			return NULL;
	} else
		return NULL;
}

void SetSelectionEditor(short FPat, short FPos, short FTrack)
{
	GrafPtr			savePort;
	Cell			theCell = { 0, 0};
	unsigned long	ttt;
	
	if (EditorDlog == NULL)
		CreatePartitionWindow();
	
	MADDriver->Pat = FPat;
	DoNullEditor();
	
	GetPort(&savePort);
	SetPortDialogPort(EditorDlog);
	
	PLSetSelect(-1, -1, -1, -1, &myList);
	
	theCell.h = FTrack;	theCell.v = FPos;
	
	Delay(2, &ttt);
	
	PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
	PLAutoScroll(&myList);
	
	SetPort(savePort);
}

#if 0
void CreateEditorPixMap(short PatID)
{
	GDHandle			oldGDeviceH;
	OSErr				errCode;
	CGrafPtr			oldPort;
	GWorldPtr			theGWorld = NULL;
	Rect				aRect, nRect;
	short				i;
	Str32				aStr;
	Boolean				BigNumber;
	
	if (EditorPix != NULL) ZapPixMap(&EditorPix);

	GetGWorld(&oldPort, &oldGDeviceH);


	if (curMusic->partition[ PatID]->header.size < 100) BigNumber = false;
	else BigNumber = true;
	
	SetRect(&aRect, 0, 0, LEFTBORD, myList.HCell * curMusic->partition[ PatID]->header.size);
	
	NewGWorld(	&theGWorld, 0, &aRect, nil, nil, (GWorldFlags) 0);
	
	if (NewOffscreenPixMap(&EditorPix, &aRect) != noErr) MyDebugStr(__LINE__, __FILE__, "");

	LockPixels(theGWorld->portPixMap);
	SetGWorld(theGWorld, NULL);

	ForeColor(blackColor);
	
	TextFont(4);	TextSize(9);

	SetRect(&nRect, 0, 0, LEFTBORD, myList.HCell);
	for (i = 0; i < curMusic->partition[ PatID]->header.size; i++)
	{
		NTStr(3, i, (Ptr) aStr);
		TETextBox((Ptr) aStr, 3, &nRect, teCenter);
		
		nRect.top += myList.HCell;
		nRect.bottom += myList.HCell;
	}
	
	CopyBits(	(BitMap*) *theGWorld->portPixMap,
 				(BitMap*) *EditorPix,
 				&(*EditorPix)->bounds,
 				&(*EditorPix)->bounds,
 				srcCopy,
 				NULL);

	SetGWorld(oldPort, oldGDeviceH);

 	UnlockPixels(theGWorld->portPixMap);
	DisposeGWorld(theGWorld);
	
	InvalWindowRect(GetDialogWindow(&aRect);
}
#endif

void SetMaxWindow(short maxRight, short maxBottom, DialogPtr	whichDialog)
{
	GrafPtr		savedPtr;
	Rect			caRect, stdRect, ttt;
	ControlHandle	aCtl;
	Point			localPt;
	
	GetPort(&savedPtr);
	SetPortDialogPort(whichDialog);
	
	GetWindowStandardState(GetDialogWindow(whichDialog), &stdRect);
	
	//GetPortBounds(GetDialogPort(whichDialog), &caRect);
	
	stdRect.right = stdRect.left + maxRight;
	stdRect.bottom = stdRect.top + maxBottom;
	
	SetWindowStandardState(GetDialogWindow(whichDialog), &stdRect);
	
#if 0
	localPt.v = localPt.h = 1;
	FindControl(localPt,whichDialog,&aCtl);
	
	if (aCtl == NULL) {
		long tL = ((long) maxRight<<16L) + (long) maxBottom;
		
		SetRect(&ttt, 0, 0, 2, 2);
		aCtl = NewControl(whichWindow, &ttt,"\p", false, 0, 0, 0,0,  tL);
	} else {
		long tL = ((long) maxRight<<16L) + (long) maxBottom;
		
		SetControlReference(aCtl, tL);
	}
	
	wPeek = (WindowPeek) whichWindow;
	wspd = (WStateData*) *(wPeek->dataHandle);
	
	if (maxRight != 0) {
		//	if (wspd->stdState.left + maxRight > qd.screenBits.bounds.right - 2) maxRight = qd.screenBits.bounds.right - 2 - wspd->stdState.left;
		wspd->stdState.right = wspd->stdState.left + maxRight;
	}
	
	if (maxBottom != 0) {
		//	if (wspd->stdState.top + maxBottom > qd.screenBits.bounds.bottom - 2) maxBottom = qd.screenBits.bounds.bottom - 2 - wspd->stdState.top;
		wspd->stdState.bottom = wspd->stdState.top + maxBottom;
	}
#endif
	
	SetPort(savedPtr);
}

void SelectCurrentActif(void)
{
	GrafPtr	savePort;
	Cell	theCell = { 0, 0};

	if (EditorDlog != NULL)
	{
		GetPort(&savePort);
		SetPortDialogPort(EditorDlog);
		
		theCell.h = curTrack;
		theCell.v = ReaderCopy;	//MADDriver->PartitionReader;
		
	/*	if (!PLGetSelect(&theCell, &myList))
		{
			theCell.h = curTrack;
			theCell.v = MADDriver->PartitionReader;
			*/
			PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			PLAutoScroll(&myList);
	//	}
		SetPort(savePort);
	}
}

void OctavesName(short id, char *String)
{
	const char	NNames[][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
	/*	{'Do','Do#','Ré','Ré#','Mi','Fa','Fa#','Sol','Sol#','La','La#','Si'};	*/
	Str255		WorkStr;
	
	if (id == 0xFF) {
		strcpy(String, "---");
		return;
	}
	
	NumToString((id / 12), WorkStr);
	MyP2CStr(WorkStr);
	String[0] = NNames[(id) % 12][0];
	String[1] = NNames[(id) % 12][1];
	String[2]='\0';
	strcat(String, (Ptr)WorkStr);
}

short ConvertNote2No(Str32 myTT)
{
	long	ThZ;
	short	Oct;

	Oct = myTT[3] - 48;
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	switch (myTT[1])
	{
		case 'C':
		case 'c':
			Oct += 0;
			break;
			
		case 'D':
		case 'd':
			Oct += 2;
			break;
			
		case 'E':
		case 'e':
			Oct += 4;
			break;
			
		case 'F':
		case 'f':
			Oct += 5;
			break;
			
		case 'G':
		case 'g':
			Oct += 7;
			break;
			
		case 'A':
		case 'a':
			Oct += 9;
			break;
			
		case 'B':
		case 'b':
			Oct += 11;
			break;
		
		default:
			Oct = 0xFF;
			break;
	}
	
	if (Oct != 0xFF) {
		if (myTT[2] == '#')
			Oct++;
		if (Oct >= NUMBER_NOTES)
			Oct = NUMBER_NOTES-1;
		if (Oct < 0)
			Oct = 0;
	}
	
	return Oct;
}

void CheckColumn(void)
{
	if (GetControlMaximum(myList.xScroll) != curMusic->header->numChn - (myList.rect.right - myList.rect.left) / myList.LCell) {
		DrawEditorUp();
		PLSetControl(&myList);
	}
}

void OutPutHexShort(Str255 outch, short column, short shortNumber)
{
	Str255			buffer;
	unsigned short	temporaryInteger;
	unsigned short	nextTemporary;
	unsigned short	digit;
	unsigned short	i, j;
//	# define abs(a) ((a) <0? - (a):(a))
	
	temporaryInteger = abs(shortNumber);
	for(i = 1; i <= 10; i++) {
		nextTemporary = temporaryInteger / 16;
		digit = temporaryInteger - nextTemporary*16;
		temporaryInteger = nextTemporary;
		
		if((digit >= 0) && (digit <= 9)) buffer[i] = digit + '0';	
		else buffer[i] = (digit-10) + 'A';
		
		if (nextTemporary < 0) break;
	}
	
	j=column;
	
	for(i = 1; i <= column; i++) {
		outch[ i] = buffer[j];
		j--;
	}
	outch[ 0] = column;
}

void NTStr(short NoDigi, short val, char *theText)
{
	short	temp;
	
	switch (NoDigi) {
		case 1:
			theText[0] = 0x30 + val;
			theText[1] = 0;
			break;
			
		case 2:
			temp = val/10;
			
			theText[0] = 0x30 + temp;
			theText[1] = 0x30 + val - temp*10;
			theText[2] = 0;
			break;
			
		case 3:
			if (val < 10) {
				theText[0] = 0x30;
				theText[1] = 0x30;
				theText[2] = 0x30 + val;
			} else if (val < 100) {
				temp = val / 10;
				
				theText[0] = 0x30;
				theText[1] = 0x30 + temp;
				theText[2] = 0x30 + val - temp * 10;
			} else {
				temp = val / 100;
				
				theText[0] = 0x30 + temp;
				val -= temp*100;
				
				temp = val/10;
				theText[1] = 0x30 + temp;
				theText[2] = 0x30 + val - temp * 10;
			}
			theText[3] = 0;
			break;
			
		default:
			MyDebugStr(__LINE__, __FILE__, "NStr Error !!");
			break;
	}
}

void SetPatternCell(short pos, short track)
{
	GrafPtr		SavePort;
	Rect			aR;
	
	if (EditorDlog == NULL)
		return;
	
	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
	
	aR.left = myList.rect.left + (track - GetControlValue(myList.xScroll)) * myList.LCell;		aR.right = aR.left + myList.LCell;
	aR.top = myList.rect.top + (pos - GetControlValue(myList.yScroll)) * myList.HCell;		aR.bottom = aR.top + myList.HCell;
	InvalWindowRect(GetDialogWindow(EditorDlog), &aR);
	
	SetPort(SavePort);
}

void GetNoteString(short note, Str255 string)
{
	if (note < 0 || note > NUMBER_NOTES)
		pStrcpy(string, "\p---");

	string[ 0] = 3;
	string[ 1] = ENote[note][0];
	string[ 2] = ENote[note][1];
	string[ 3] = ENote[note][2];
}

void InitStringEditor(void)
{
	Str255		stemp;
	short		i, x;

	for (i = 1; i <= MAXINSTRU; i++) {
		NTStr(3, i, (Ptr)stemp);
		
		EInstru[i][0] = stemp[0];
		EInstru[i][1] = stemp[1];
		EInstru[i][2] = stemp[2];
	}
	EInstru[0][0] = '0';
	EInstru[0][1] = '0';
	EInstru[0][2] = '0';

	for (i = 0; i < NUMBER_NOTES; i++) {
		OctavesName(i, (Ptr)stemp);
		
		ENote[i][0] = stemp[0];
		ENote[i][1] = stemp[1];
		ENote[i][2] = stemp[2];
	}
	ENote[ 0xFF][ 0] = '0';
	ENote[ 0xFF][ 1] = '0';
	ENote[ 0xFF][ 2] = '0';
	
	ENote[ 0xFE][ 0] = 'O';
	ENote[ 0xFE][ 1] = 'F';
	ENote[ 0xFE][ 2] = 'F';
	

	for (i = 0; i < 16; i++) {
		OutPutHexShort(stemp, 1, i);
		MyP2CStr(stemp);
		EEffect[ i] = stemp[ 0];
	}
	EEffect[ 16] = 'G';
	EEffect[ 17] = 'L';
	EEffect[ 18] = 'O';
	
	for (i = 0; i <= 256; i++)
	{
		OutPutHexShort(stemp, 2, i);
		MyP2CStr(stemp);
		EArgu[ i][ 0] = stemp[ 0];
		EArgu[ i][ 1] = stemp[ 1];
	}
}

void ConvertTEH(unsigned char *dst, short length)
{
	CharsHandle cHdle = TEGetText(TEH[ curActif]);
	
	if ((*TEH[ curActif])->teLength == 0)
	{
		dst[ 0] = ' ';
		if (length >= 2) dst[ 1] = ' ';
		if (length >= 3) dst[ 2] = ' ';
	}
	else if ((*TEH[ curActif])->teLength == 1)
	{
		dst[ 0] =  (*cHdle)[ 0];
		if (length >= 2) dst[ 1] = ' ';
		if (length >= 3) dst[ 2] = ' ';
	}
	else if ((*TEH[ curActif])->teLength == 2)
	{
		dst[ 0] = (*cHdle)[ 0];
		if (length >= 2) dst[ 1] = (*cHdle)[ 1];
		if (length >= 3) dst[ 2] =' ';
	}
	else if ((*TEH[ curActif])->teLength == 3)
	{
		dst[ 0] = (*cHdle)[ 0];
		if (length >= 2) dst[ 1] = (*cHdle)[ 1];
		if (length >= 3) dst[ 2] = (*cHdle)[ 2];
	}
	else MyDebugStr(__LINE__, __FILE__, "Major ERROR");
}

Boolean CreateNoteString(Cmd *theCommand, Str255 mainStr, Boolean AllStr)
{
 	short				i, charID = 1;
 	Str255				stemp;
 	Boolean				Note = false;

	if (thePrefs.DigitalInstru || AllStr)
	{
		if (AcceptKeysTools() == true && theCommand == GetCmdDlogActif() && curActif == InstruTE && (MADDriver->Reading == false || thePrefs.MusicTrace == false))
		{
			Note = true;	ConvertTEH(&mainStr[ charID], 3);		charID += 3;
		}
		else if (theCommand->ins != 0)
		{
			Note = true;
			mainStr[ charID++] = EInstru[ theCommand->ins][ 0];
			mainStr[ charID++] = EInstru[ theCommand->ins][ 1];
			mainStr[ charID++] = EInstru[ theCommand->ins][ 2];
		}
		else
		{
			mainStr[ charID++] = ' ';	mainStr[ charID++] = ' '; mainStr[ charID++] = ' ';
		}
	}

	if (thePrefs.DigitalNote || AllStr)
	{
		if (charID != 1) mainStr[ charID++] = ' ';

		if (AcceptKeysTools() == true && theCommand == GetCmdDlogActif() && curActif == NoteTE && (MADDriver->Reading == false || thePrefs.MusicTrace == false))
		{
			Note = true;	ConvertTEH(&mainStr[ charID], 3);		charID += 3;
		}
		else if (theCommand->note != 0xFF)
		{
			Note = true;
			mainStr[ charID++] = ENote[ theCommand->note][ 0];
			mainStr[ charID++] = ENote[ theCommand->note][ 1];
			mainStr[ charID++] = ENote[ theCommand->note][ 2];
		}
		else
		{
			mainStr[ charID++] = ' ';
			mainStr[ charID++] = ' ';
			mainStr[ charID++] = ' ';
		}
	}

	if (thePrefs.DigitalEffect || AllStr)
	{
		if (charID != 1) mainStr[ charID++] = ' ';

		if (AcceptKeysTools() == true && theCommand == GetCmdDlogActif() && curActif == EffectTE && (MADDriver->Reading == false || thePrefs.MusicTrace == false))
		{
			Note = true;	ConvertTEH(&mainStr[ charID], 1);		charID += 1;
		}
		else if (theCommand->cmd != 0)
		{
			Note = true;
			mainStr[ charID++] = EEffect[ theCommand->cmd];
		}
		else
		{
			if (theCommand->arg != 0) mainStr[ charID++] = EEffect[ theCommand->cmd];
			else mainStr[ charID++] = ' ';
		}
	}

	if (thePrefs.DigitalArgu || AllStr)
	{
		if (charID != 1) mainStr[ charID++] = ' ';

		if (AcceptKeysTools() == true && theCommand == GetCmdDlogActif() && curActif == ArguTE && (MADDriver->Reading == false || thePrefs.MusicTrace == false))
		{
			Note = true;	ConvertTEH(&mainStr[ charID], 2);		charID += 2;
		}
		else if (theCommand->arg != 0)
		{
			Note = true;
			mainStr[ charID++] = EArgu[ theCommand->arg][ 0];
			mainStr[ charID++] = EArgu[ theCommand->arg][ 1];
		}
		else
		{
			mainStr[ charID++] = ' ';
			mainStr[ charID++] = ' ';
		}
	}
	
	if (thePrefs.DigitalVol || AllStr)
	{
		if (charID != 1) mainStr[ charID++] = ' ';

		if (AcceptKeysTools() == true && theCommand == GetCmdDlogActif() && curActif == VolumeTE && (MADDriver->Reading == false || thePrefs.MusicTrace == false))
		{
			Note = true;	ConvertTEH(&mainStr[ charID], 2);		charID += 2;
		}
		else if (theCommand->vol != 0xFF)
		{
			Note = true;
			mainStr[ charID++] = EArgu[ theCommand->vol][ 0];
			mainStr[ charID++] = EArgu[ theCommand->vol][ 1];
		}
		else
		{
			mainStr[ charID++] = ' ';
			mainStr[ charID++] = ' ';
		}
	}

	mainStr[ 0] = charID - 1;

	return Note;
}

void SetNewPat()
{
	short	x;
	Str255	String, aStr, theStr;
	Rect		caRect;

	myList.maxY = curMusic->partition[ CurrentPat]->header.size;
	if (myList.maxX != curMusic->header->numChn)
	{
		myList.maxX = curMusic->header->numChn;
		CheckColumn();
		
		GetPortBounds(GetDialogPort(EditorDlog), &caRect);
		
		myList.rect.right = caRect.right - 15;
	//	myList.rect.right = myList.rect.left + //curMusic->header->numChn * myList.LCell;
	}

	PLCheckSelect(&myList);

	if (GetControlMaximum(myList.yScroll) !=
		curMusic->partition[ CurrentPat]->header.size  - (myList.rect.bottom - myList.rect.top) / myList.HCell)
		{
			PLSetControl(&myList);
			DrawLeft(NULL, false);
		}
	
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
	
	pStrcat(String, "\p- ");
	NumToString((long) curMusic->partition[ CurrentPat]->header.size, theStr);
	pStrcat(String, theStr);
	pStrcat(String, "\p Rows");
	
	SetWTitle(GetDialogWindow(EditorDlog), String);
}

void UpdateEditorInfo(void)
{
	GrafPtr	SavePort;
	Rect		cellRect;
	Point		theCell = {0,0};
	FontInfo	ThisFontInfo;
	Rect		caRect;

	if (EditorDlog == NULL) return;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
		
	DoNullEditor();
	
//	TextFont(4);	TextSize(9);
	
	GetFontInfo(&ThisFontInfo);
	myList.HCell = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading - thePrefs.LinesHeight;
	tLH = thePrefs.LinesHeight/2;
	
	DoNullEditor();
	
	SetNewPat();
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	cellRect = myList.rect;
	cellRect.top -= UPSIZE;
	cellRect.left = 0;
	cellRect.right = caRect.right - 15;
	InvalWindowRect(GetDialogWindow(EditorDlog), &cellRect);
	
	SetMaxWindow(	myList.rect.left + 15 + curMusic->header->numChn * myList.LCell,
					myList.rect.top + 15 + curMusic->partition[ CurrentPat]->header.size * myList.HCell,
					EditorDlog);
	
	CreateCurRect();

	SetPort(SavePort);
}

void UpdatePartitionWindow(DialogPtr);

void CreateCurRect()
{
	short	temp;

	temp = ReaderCopy - GetControlValue(myList.yScroll);

	CurRect.top = myList.rect.top + temp*myList.HCell;
	CurRect.bottom = CurRect.top + myList.HCell;
	CurRect.left = 	LEFTBORD;					//;
//	CurRect.right = EditorDlog->portRect.right - 15;	//LEFTBORD;
	
	CurRect.right = myList.rect.right;
	if (CurRect.right > myList.rect.left + (myList.maxX - GetControlValue(myList.xScroll)) * myList.LCell)
		CurRect.right = myList.rect.left + (myList.maxX - GetControlValue(myList.xScroll)) * myList.LCell;
	
	if (CurRect.top < myList.rect.top) CurRect.top = myList.rect.top;
	if (CurRect.bottom < myList.rect.top) SetRect(&CurRect, 0, 0, 0, 0);

	if (CurRect.bottom > myList.rect.bottom) CurRect.bottom = myList.rect.bottom;
	if (CurRect.top > myList.rect.bottom) SetRect(&CurRect, 0, 0, 0, 0);
	
	previousYCurRect = ReaderCopy;
}

void DrawLeft(RgnHandle	clipRgn, Boolean Update)
{
	Rect		nRect;
	Str255		aStr, tempStr;
	short		i, xO;
	RgnHandle	saveClipRgn;
	
	saveClipRgn = NewRgn();
 	GetClip(saveClipRgn);
 	
 	SetRect(&nRect, 0, myList.rect.top, LEFTBORD, myList.rect.bottom);
 	
 	if (clipRgn == NULL)
 	{
		ClipRect(&nRect);
	}
	else
	{
		SetClip(clipRgn);
	}
	
 	EraseRect(&nRect);
	
	xO = myList.rect.top + myList.HCell;
	
	for (i = GetControlValue(myList.yScroll); i < PLGetMaxYValue(&myList); i++)
	{
		if (thePrefs.UseMarkers)
		{
			if (i >= thePrefs.MarkersOffSet)
			{
				short	temp;
				Rect	tRect;
				
			
				temp = (i - thePrefs.MarkersOffSet) / thePrefs.MarkersSize;
				if (temp % 2 == 0) RGBForeColor(&yellC);
				else ForeColor(whiteColor);
				
				tRect.left = 0;
				tRect.right = LEFTBORD;
				tRect.top = myList.rect.top + (i - GetControlValue(myList.yScroll)) * myList.HCell;
				tRect.bottom = tRect.top + myList.HCell;
				PaintRect(&tRect);
			}
		}
		
		ForeColor(blackColor);
		
		NumToString(i, tempStr);
 		
 		pStrcpy(aStr, "\p");
		if (i < 10) pStrcat(aStr, "\p0");
		if (i < 100) pStrcat(aStr, "\p0");
		pStrcat(aStr, tempStr);
		
		MoveTo(2, xO - 2 + tLH);
		DrawString(aStr);
		
		xO += myList.HCell;
	}
	
	if (!Update) InvalWindowRect(GetDialogWindow(EditorDlog), &CurRect);
	
	CreateCurRect();
	
	RGBForeColor(&theColor);
	
	PenMode(adMin);
	PaintRect(&CurRect);
	PenMode(srcCopy);
	
	ForeColor(blackColor);
	
//	ReaderCopy = -1;
	
	SetClip(saveClipRgn);
 	DisposeRgn(saveClipRgn);
}

void MyTETextBox(RGBColor *backColor, Rect *rect, Str255 str, Boolean black)
{
	short length;
	
	RGBForeColor(backColor);
	
	PaintRect(rect);
	
	length = StringWidth(str);
	
	length = (rect->right - rect->left - length) / 2;
	
	MoveTo(rect->left + length, rect->bottom-2);
	
	if (black) ForeColor(blackColor);
	else RGBForeColor(&Gray[ 0]);
		
	DrawString(str);
	
}

void DrawEditorUp(void)
{
	short				i, itemType;
	Handle				itemHandle;
	Point				theCell;
	Rect				tempRect = {0}, bRect, itemRect;
	Str255				tempStr, aStr;
	Rect				caRect;
	ThemeDrawingState	state;

	GetDialogItem(EditorDlog, 5, &itemType, &itemHandle, &bRect);
	bRect.top++;
	bRect.bottom--;
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	bRect.right = caRect.right;
	RGBForeColor(&theColor);
	PaintRect(&bRect);		
	ForeColor(blackColor);

	/*::*/GetThemeDrawingState(&state );
	
	for (i = 0; i < PLGetMaxXValue(&myList) - GetControlValue(myList.xScroll); i++)
	{
		tempRect.top = bRect.top;				tempRect.bottom = bRect.bottom;
		tempRect.left = LEFTBORD + i*myList.LCell;		tempRect.right = tempRect.left + myList.LCell;
		
		NumToString(i + 1 + GetControlValue(myList.xScroll), aStr);
		
		MyTETextBox(&thePrefs.tracksColor[ i + GetControlValue(myList.xScroll)], &tempRect, aStr, MADDriver->Active[ i]);
		
		ForeColor(blackColor);
		
		MoveTo(tempRect.left, bRect.top);		LineTo(tempRect.left, myList.rect.top-1);
	}
	
	MoveTo(tempRect.right, bRect.top);			LineTo(tempRect.right, myList.rect.top-1);
	
//	BackColor(whiteColor);
	SetThemeDrawingState(state, true );
}

void DoNullEditor(void)
{
	Str255		String, aStr;
	GrafPtr		SavePort;
	short		temp;
	Point		myPt;
 	Rect		tempRect;
 	Boolean		MakeUpdate = false;
 	RgnHandle	visibleRegion;
 	
 	if (EditorDlog == NULL) return;
 
 	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);

	if (CurrentPat != MADDriver->Pat)
	{
		if (MADDriver->PartitionReader == 0 && MADDriver->smallcounter <= 1 && MADDriver->speed > 1)	// Wait a bit more...
	/*	{
		}
		else*/
		
		if (MusicPlayActive == true)
		{
			ReaderCopy = MADDriver->PartitionReader;
			SetControlValue(myList.yScroll, ReaderCopy);
		}
		
		CurrentPat = MADDriver->Pat;
		SetNewPat();
		
		tempRect = myList.rect;
		if (myList.maxY != curMusic->partition[ CurrentPat]->header.size)
		{
			Rect	caRect;
	
			GetPortBounds(GetDialogPort(EditorDlog), &caRect);

			tempRect.left = 0;
			
			tempRect.right = caRect.right - 15;
			InvalWindowRect(GetDialogWindow(EditorDlog), &tempRect);
			
			InvalWindowRect(GetDialogWindow(EditorDlog), &CurRect);
			SetRect(&CurRect, -1, -1, -1, -1);
			
			MakeUpdate = true;
		}
		else
		{
			MyLUpdate();
			DrawLeft(NULL, false);
		}
	}

	if (ReaderCopy != MADDriver->PartitionReader)
	{
		ReaderCopy = MADDriver->PartitionReader;
		
		if (MusicPlayActive == true)
		{
				/* Page per page scrolling */
		
			if (ReaderCopy >= PLGetMaxYValue(&myList) ||
				ReaderCopy < GetControlValue(myList.yScroll))
				{
				//	Rect	aRect = myList.rect;
				//	aRect.left = 0;
					
					SetControlValue(myList.yScroll, ReaderCopy);
				//	InvalWindowRect(GetDialogWindow(&aRect);
					
					if (!MakeUpdate)
					{
						MyLUpdate();
						DrawLeft(NULL, false);
					}
				//	MakeUpdate = true;
				}
				
				/* Continuous scrolling */
				
			/*	if (ReaderCopy != GetControlValue(myList.yScroll) && ReaderCopy <= GetControlMaximum(myList.yScroll))
				{
					short	curVal = GetControlValue(myList.yScroll);
					Rect	aRect = myList.rect;
					
					aRect.left = 0;
				
					SetControlValue(myList.yScroll, ReaderCopy);
				
					ScrollRect(&aRect, 0, (curVal - ReaderCopy) * myList.HCell, NULL);
				
					MyLUpdateSpecific(PLGetMaxYValue(&myList) - (ReaderCopy - curVal) - 1, PLGetMaxYValue(&myList));
					
					PLGetSelectRect(&SelectRect, NULL);
				}*/
		}
		
		//
		
	//	InvalWindowRect(GetDialogWindow(&CurRect);
		SetRect(&CurRect, -1, -1, -1, -1);
		MyLUpdateSpecific(previousYCurRect, previousYCurRect+1);	//+1
		
		if (ReaderCopy < curMusic->partition[ CurrentPat]->header.size)
		{
			CreateCurRect();
			
			RGBForeColor(&theColor);
			
			PenMode(adMin);
			PaintRect(&CurRect);
			PenMode(srcCopy);
			
			ForeColor(blackColor);
		}
	}
	
	if (oldWindow == GetDialogWindow(EditorDlog))
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetWindowPort(oldWindow), visibleRegion);
		
		if (FrontWindow() == GetDialogWindow(ToolsDlog) && PtInRgn(myPt, visibleRegion))
		{
			Rect	SelectRect;
			
			PLGetSelectRect(&SelectRect, &myList);
			
			SectRect(&SelectRect, &myList.rect, &SelectRect);
			
			if ((theEvent.modifiers & 0xFF00) == thePrefs.FastDigitalEdition) SetCursor(&ContextCrsr);
			else if ((theEvent.modifiers & cmdKey) != 0)
			{
				Rect atRect = myList.rect;
				
				atRect.left = 0;
				
				if (PtInRect(myPt, &atRect)) SetCursor(&PlayCrsr);
				else SetCursor(GetQDGlobalsArrow(&qdarrow));
			}
			else if (DragManagerUse == true && PtInRect(myPt, &SelectRect))
			{
				SetCursor(&HandCrsr);
			}
			else SetCursor(GetQDGlobalsArrow(&qdarrow));
		}
		else SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		DisposeRgn(visibleRegion);
	}
	
	if (MakeUpdate) UpdatePartitionWindow(EditorDlog);
	
	SetPort(SavePort);
}

void DoGrowPartition(void)
{
	long	lSizeVH;
	GrafPtr	SavePort;
	Rect	temp, cellRect;
	Point	theCell = { 0, 0}, aPt = { 0, 0};
	short	tempA, tempB;
	Handle	itemHandle;
	short	itemType;
	Rect	caRect;
	BitMap	screenBits;
	
	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);

	temp.left = 100;
	temp.right = myList.rect.left + 16 + curMusic->header->numChn * myList.LCell;
	if (temp.right < temp.left) temp.right = temp.left;
	temp.top = 110;
	
	temp.bottom = myList.rect.top + 16 + curMusic->partition[ CurrentPat]->header.size * myList.HCell;
//	temp.bottom += 16;

	LocalToGlobal(&aPt);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	if (temp.bottom < temp.top) temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(EditorDlog), theEvent.where, &temp);
	
	if (lSizeVH != 0)
	{
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	}
	else
	{
		GetPortBounds(GetDialogPort(EditorDlog), &caRect);

		if (caRect.right >= temp.right) tempA = temp.right-1;
		else tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	MySizeWindow(EditorDlog, tempA, tempB , true);

	GetPortBounds(GetDialogPort(EditorDlog), &caRect);

	GetDialogItem(EditorDlog, 1, &itemType, &itemHandle, &myList.rect);
	myList.rect.left = LEFTBORD;
	myList.rect.right = caRect.right - 15;	//myList.rect.left +  curMusic->header->numChn * myList.LCell;
	myList.rect.bottom = caRect.bottom - 15;
	
	PLSetControl(&myList);
	
	EraseRect(&caRect);
	InvalWindowRect(GetDialogWindow(EditorDlog), &caRect);
	CreateCurRect();
	
	SetPort(SavePort);
}

void DrawGrowIconP(DialogPtr	GetSelection)
{
	Rect   		tempRect;
	RgnHandle		savedClip;
	GrafPtr		SavePort;
	Rect			caRect;
	
	GetPort(&SavePort);
	SetPortDialogPort(GetSelection);
	
	savedClip = NewRgn();
	GetClip(savedClip);
	
	GetPortBounds(GetDialogPort(GetSelection), &caRect);
	
	tempRect.right = caRect.right;
	tempRect.bottom = caRect.bottom;
	tempRect.left = tempRect.right - 15;
	tempRect.top = tempRect.bottom - 15;
	
	ClipRect(&tempRect);
	
	DrawGrowIcon(GetDialogWindow(GetSelection));
	
	SetClip(savedClip);
	DisposeRgn(savedClip);
	
	SetPort(SavePort);
}

void DrawEditorLegende(void)
{
	short	i;
	Str255	aStr;

	for (i = GetControlValue(myList.yScroll); i < PLGetMaxYValue(&myList); i++)
	{
		MoveTo(2, (i - GetControlValue(myList.yScroll)) * myList.HCell + myList.rect.top + 8);
	
		NTStr(2, i + 1, (Ptr) aStr);
		MyC2PStr((Ptr) aStr);
		DrawString(aStr);
	}
}

void DrawGrayLines(short xPos)
{
	short	xx = 0, t;
	short	tot = 0;
	
	RGBForeColor(&theColor);
	
	if (thePrefs.DigitalInstru)
	{
		tot++;
		if (tot < totItem)
		{
			xx += 3*LChar;				t = xPos + xx + 2;
			MoveTo(t, myList.rect.top);		LineTo(t, myList.rect.bottom);
			
			xx += LChar;
		}
	}
	
	if (thePrefs.DigitalNote)
	{
		tot++;
		if (tot < totItem)
		{
			xx += 3*LChar;				t = xPos + xx + 2;
			MoveTo(t, myList.rect.top);		LineTo(t, myList.rect.bottom);
			
			xx += LChar;
		}
	}
	
	if (thePrefs.DigitalEffect)
	{
		tot++;
		if (tot < totItem)
		{
			xx += 1*LChar;				t = xPos + xx + 2;
			MoveTo(t, myList.rect.top);		LineTo(t, myList.rect.bottom);
			
			xx += LChar;
		}
	}
	
	if (thePrefs.DigitalArgu)
	{
		tot++;
		if (tot < totItem)
		{
			xx += 2*LChar;						t = xPos + xx + 2;
			MoveTo(t, myList.rect.top);		LineTo(t, myList.rect.bottom);
			
			xx += LChar;
		}
	}
	
	if (thePrefs.DigitalVol)
	{
		tot++;
		if (tot < totItem)
		{
			xx += 2*LChar;						t = xPos + xx + 2;
			MoveTo(t, myList.rect.top);		LineTo(t, myList.rect.bottom);
		}
	}
	
	ForeColor(blackColor);
}

void DrawMarkers(short y1, short y2)
{
	Rect	tRect;
	short	i, temp, off;
	
	tRect = myList.rect;
	if (tRect.right > tRect.left + (myList.maxX - GetControlValue(myList.xScroll)) * myList.LCell)
		tRect.right = tRect.left + (myList.maxX - GetControlValue(myList.xScroll)) * myList.LCell;
	
	off = GetControlValue(myList.yScroll);
	
	for (i = y1; i < y2; i ++)
	{
		if (i < thePrefs.MarkersOffSet) ForeColor(whiteColor);
		else
		{
			temp = (i - thePrefs.MarkersOffSet) / thePrefs.MarkersSize;
			if (temp % 2 == 0) RGBForeColor(&yellC);
			else ForeColor(whiteColor);
		}
		
		tRect.top = myList.rect.top + (i - off) * myList.HCell;
		tRect.bottom = tRect.top + myList.HCell;
		PaintRect(&tRect);
	}
	
	tRect.top = myList.rect.top + (i - off) * myList.HCell;
	tRect.bottom = myList.rect.bottom;
	EraseRect(&tRect);
		
}

void DrawSelectionZone(Rect *inRect)
{	
	RGBColor		hilite;
	
	LMGetHiliteRGB(&hilite);

//	LMSetHiliteMode((UInt8) (LMGetHiliteMode() & 0x7F));

	if (hilite.red == 0 &&
		hilite.green == 0 &&
		hilite.blue == 0)
		{
			InvertRect(inRect);
		}
	else
	{
		RGBForeColor(&hilite);
		
		PenMode(adMin);
		PaintRect(inRect);
		PenMode(srcCopy);
		
		ForeColor(blackColor);
	}
	
	if (AcceptKeysTools())
	{
		Rect 		tRect;
		Point		cell;
		
		if (thePrefs.MusicTrace && MADDriver->Reading == true) {}
		else
		{
		GetCurrentDlogCmd(&cell.v, &cell.h);
		
		PLRect(&tRect, cell, &myList);
		
		tRect.right = tRect.left;
		
		ForeColor(redColor);
		
		if (thePrefs.DigitalInstru) tRect.right += 4*LChar;
		if (thePrefs.DigitalInstru && curActif == InstruTE) FrameRect(&tRect);
		if (thePrefs.DigitalInstru) tRect.left += 4*LChar;
		
		if (thePrefs.DigitalNote) tRect.right += 4*LChar;
		if (thePrefs.DigitalNote && curActif == NoteTE) FrameRect(&tRect);
		if (thePrefs.DigitalNote) tRect.left += 4*LChar;
		
		if (thePrefs.DigitalEffect) tRect.right += 2*LChar;
		if (thePrefs.DigitalEffect && curActif == EffectTE) FrameRect(&tRect);
		if (thePrefs.DigitalEffect) tRect.left += 2*LChar;
		
		if (thePrefs.DigitalArgu) tRect.right += 3*LChar;
		if (thePrefs.DigitalArgu && curActif == ArguTE) FrameRect(&tRect);
		if (thePrefs.DigitalArgu) tRect.left += 3*LChar;
		
		if (thePrefs.DigitalVol) tRect.right += 3*LChar;
		if (thePrefs.DigitalVol && curActif == VolumeTE) FrameRect(&tRect);
		if (thePrefs.DigitalVol) tRect.left += 3*LChar;
		
		ForeColor(blackColor);
		}
	}
}

void MyLUpdate()
{
	short		i, x;
	short		xPos, yPos;
	Str255		aStr;
	Cmd			*CmdPtr;
	RgnHandle		saveClip;
	Rect			tRect;
	Rect			caRect;
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);

	tRect = myList.rect;
	tRect.left = myList.rect.left + curMusic->header->numChn * myList.LCell;
	tRect.right = caRect.right - 15;
	EraseRect(&tRect);

	saveClip = NewRgn();
	GetClip(saveClip);
	
	ClipRect(&myList.rect);
	
	if (thePrefs.UseMarkers) DrawMarkers(GetControlValue(myList.yScroll), PLGetMaxYValue(&myList));
	else EraseRect(&myList.rect);
	
	xPos = myList.rect.left + 4;
	for (x = GetControlValue(myList.xScroll); x < PLGetMaxXValue(&myList); x++)
	{
		DrawGrayLines(xPos);
	
		MoveTo(xPos - 4, myList.rect.top);		LineTo(xPos - 4, myList.rect.bottom);
		
		yPos		=	myList.rect.top + 9;
		CmdPtr	=	GetMADCommand(GetControlValue(myList.yScroll), x, curMusic->partition[ CurrentPat]);
		
		if (MADDriver->Active[ x] == false) RGBForeColor(&Gray[ 0]);
		
		for (i = GetControlValue(myList.yScroll); i < PLGetMaxYValue(&myList); i++)
		{
			if (CreateNoteString(CmdPtr, aStr, false))
			{
				MoveTo(xPos, yPos-tLH);
				DrawString(aStr);
			}
			
			CmdPtr++;
			yPos += myList.HCell;
		}
		
		ForeColor(blackColor);
		
		xPos += myList.LCell;		// Ligne suivante
	}
	
	MoveTo(xPos - 4, myList.rect.top);		LineTo(xPos - 4, myList.rect.bottom + 1);
	
	PLGetSelectRect(&tRect, &myList);
	tRect.left++;
	DrawSelectionZone(&tRect);
	
	RGBForeColor(&theColor);
	PenMode(adMin);
	PaintRect(&CurRect);
	PenMode(srcCopy);
	ForeColor(blackColor);
	
	SetClip(saveClip);
	DisposeRgn(saveClip);
}

void MyLUpdateSpecific(short startY, short endY)
{
	short		i, x;
	short		xPos, yPos;
	Str255		aStr;
	Cmd			*CmdPtr;
	RgnHandle	saveClip;
	Rect		eraseRect, tRect;
	
	saveClip = NewRgn();
	GetClip(saveClip);
	
	if (endY >= curMusic->partition[ CurrentPat]->header.size)
	{
		SetRect(&eraseRect, myList.rect.left, myList.rect.top + (startY - GetControlValue(myList.yScroll)) * myList.HCell, myList.rect.right, myList.rect.bottom);
	}
	else
	{
		SetRect(&eraseRect, myList.rect.left, myList.rect.top + (startY - GetControlValue(myList.yScroll)) * myList.HCell, myList.rect.right, myList.rect.top + (startY - GetControlValue(myList.yScroll)) * myList.HCell + (endY - startY)*myList.HCell);
	}
	
	if (eraseRect.bottom > myList.rect.bottom) eraseRect.bottom = myList.rect.bottom;
	if (eraseRect.top < myList.rect.top) eraseRect.top = myList.rect.top;
	
	ClipRect(&eraseRect);

	if (thePrefs.UseMarkers) DrawMarkers(startY, endY);
	else EraseRect(&eraseRect);
	
	xPos = myList.rect.left + 4;
	for (x = GetControlValue(myList.xScroll); x < PLGetMaxXValue(&myList); x++)
	{
		DrawGrayLines(xPos);
		
		MoveTo(xPos - 4, myList.rect.top);		LineTo(xPos - 4, myList.rect.bottom);
		
		yPos	=	myList.rect.top + 9 + (startY - GetControlValue(myList.yScroll)) * myList.HCell;
		CmdPtr	=	GetMADCommand(startY, x, curMusic->partition[ CurrentPat]);
		
		if (MADDriver->Active[ x] == false) RGBForeColor( &Gray[ 0]);
		
		for (i = startY; i < endY; i++)
		{
			if (CreateNoteString(CmdPtr, aStr, false)) { MoveTo(xPos, yPos-tLH);	DrawString(aStr); }
			
			CmdPtr++;
			yPos += myList.HCell;
		}
		
		ForeColor(blackColor);
		
		xPos += myList.LCell;		// Ligne suivante
	}
	
	MoveTo(xPos - 4, myList.rect.top);		LineTo(xPos - 4, myList.rect.bottom + 1);
	
	PLGetSelectRect(&tRect, &myList);
	tRect.left++;
	DrawSelectionZone(&tRect);
	
	RGBForeColor(&theColor);
	PenMode(adMin);
	PaintRect(&CurRect);
	PenMode(srcCopy);
	ForeColor(blackColor);
	
	SetClip(saveClip);
	DisposeRgn(saveClip);
}

void  UpdatePartitionWindow(DialogPtr GetSelection)
{ 
	Rect   		tempRect, itemRect;
	GrafPtr		SavePort;
	Point		theCell;
	Ptr			theStr;
	Str255		tempStr, aStr;
	short		i, itemType;
	Handle		itemHandle;
	Rect			caRect;
	RgnHandle	visibleRegion;
	
	GetPort(&SavePort);
	SetPortDialogPort(EditorDlog);
	
	BeginUpdate(GetDialogWindow(EditorDlog));
	
	yellC			= thePrefs.yellC;
	
	GetDialogItem(EditorDlog, 5, &itemType, &itemHandle, &tempRect);
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	itemRect.top = 0;
	itemRect.bottom = tempRect.top;
	itemRect.left = 0;
	itemRect.right = caRect.right;
	//	RGBForeColor(&theColor);
	//	PaintRect(&itemRect);
	
	SetRect(&itemRect, 0, caRect.bottom, LEFTBORD, caRect.bottom);
	//	PaintRect(&itemRect);
	ForeColor(blackColor);
	
	TextFont(0);	TextSize(12);
	
	//		RGBBackColor(&theColor);
	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion(GetDialogPort(EditorDlog), visibleRegion);
	
	UpdateDialog(EditorDlog, visibleRegion);
	
	DisposeRgn(visibleRegion);
	
	//		BackColor(whiteColor);
	
	TextFont(4);	TextSize(9);
	
	MyLUpdate();
	
	DrawLeft(NULL, true);
	
	DrawGrowIconP(EditorDlog);
	
	DrawEditorUp();
	
	MoveTo(0, myList.rect.bottom);
	LineTo(LEFTBORD, myList.rect.bottom);
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	MoveTo(0, myList.rect.top - 1);
	LineTo(caRect.right, myList.rect.top - 1);
	
	GetDialogItem(EditorDlog, 5, &itemType, &itemHandle, &tempRect);
	
	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	MoveTo(0, tempRect.top);
	LineTo(caRect.right, tempRect.top);
	
	EndUpdate(GetDialogWindow(EditorDlog));
	
	SetPort(SavePort);
} 

void GetDigitalSelection(short *XStart, short *YStart, short *XEnd, short *YEnd, short *currentPat)
{
	if (EditorDlog == NULL) return;

	if (XStart) *XStart = myList.select.left;
	if (XEnd) *XEnd	= myList.select.right+1;
	
	if (YStart) *YStart	= myList.select.top;
	if (YEnd) *YEnd	= myList.select.bottom+1;
	
	if (currentPat) *currentPat = CurrentPat;
}

void SetDigitalSelection(short XStart, short YStart, short XEnd, short YEnd)
{
	GrafPtr		savePort;
	Point		theCell;
	
	if (EditorDlog == NULL) return;
	
	GetPort(&savePort);
	SetPortDialogPort(EditorDlog);
	
	PLSetSelect(XStart, YStart, XEnd - 1, YEnd - 1, &myList);
	
	SetPort(savePort);
}

void CalculDiffStart(void)
{
	Point	aCell, myPt;

	myPt = theEvent.where;
	GlobalToLocal(&myPt);

	aCell = myPt;
	PLConvertPoint(&aCell, &myList);

	DiffStartH = myList.select.left - aCell.h;
	DiffStartV = myList.select.top - aCell.v;
}

void DragSelect(void)
{
	Point		theCell, aPt;
	Rect		bRect;
	RgnHandle	tempRgn;
	Pcmd		*myPcmd;
	
	if (DragManagerUse)
	{
		SetCursor(&CHandCrsr);
	
		if (WaitMouseMoved(theEvent.where))
		{
			DidTryToDrag = true;
			
			CalculDiffStart();
			PLGetSelectRect(&bRect, &myList);
			
			tempRgn = NewRgn();
			RectRgn(tempRgn, &bRect);
			
			myPcmd = CreatePcmdFromSelection();
			if (myPcmd != NULL)
			{
				DragEditor(tempRgn, myPcmd, &theEvent);
				
				DiffStartV = 0;		DiffStartH = 0;
				
				MyDisposePtr((Ptr*) &myPcmd);
			}
			DisposeRgn(tempRgn);
		}
	}
}

void SavePcmdFile(Pcmd *myPcmd)
{
	FSSpec				reply;
	Str255				defaultname, tStr;
	OSErr				iErr;
	long					inOutBytes;
	short				fRefNum;

	if (myPcmd == NULL) return;

	pStrcpy(defaultname, "\pMy Pcmd (");
	
	NumToString(myPcmd->tracks, tStr);
	pStrcat(defaultname, tStr);
	pStrcat(defaultname, "\px");
	NumToString(myPcmd->length, tStr);
	pStrcat(defaultname, tStr);
	pStrcat(defaultname, "\p)");
	
	iErr = DoCustomSave("\pSave Pcmd file", defaultname, 'Pcmd', &reply);
	if (iErr) return;
	
//	HSetVol(NULL, reply.vRefNum, reply.parID);
	
	if (FSpOpenDF(&reply, fsCurPerm, &fRefNum) != noErr)
	{
		iErr = FSpDelete(&reply);
		iErr = FSpCreate(&reply, 'SNPL', 'Pcmd', smSystemScript);
		iErr = FSpOpenDF(&reply, fsCurPerm, &fRefNum);
		if (iErr) goto EndPcmd;
	}
	iErr = FSSetForkPosition(fRefNum, fsFromStart, 0);

	inOutBytes = GetPtrSize((Ptr)myPcmd);
	SwapPcmd(myPcmd);
	iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, myPcmd, NULL);
	iErr = FSCloseFork(fRefNum);

	EndPcmd:

	MyDisposePtr((Ptr*) &myPcmd);
}

void OpenPcmdFile(FSSpec	*mySpec)
{
	Str255				defaultname;
	OSErr				iErr;
	long					inOutBytes;
	short				fRefNum;	
	Pcmd				*myPcmd;

	if (mySpec == NULL)
	{
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Open Pcmd File'");
		
		iErr = DoStandardOpen(mySpec, "\pPcmd file", 'Pcmd');
		
		if (iErr) return;
	}
	
	if (FSpOpenDF(mySpec, fsCurPerm, &fRefNum) == noErr)
	{
		CellSelec.v = 0;	CellSelec.h = 0;
		if (PLGetSelect(&CellSelec, &myList))
		{
			GetEOF(fRefNum, &inOutBytes);
			myPcmd = (Pcmd*) MyNewPtr(inOutBytes);
			iErr = FSRead(fRefNum, &inOutBytes, (Ptr) myPcmd);
			
			PasteCmdEditor(CellSelec, myPcmd);
			
			curMusic->hasChanged = true;
		}
		iErr = FSCloseFork(fRefNum);
	}
}

static 	long 		lastTickCount;
static 	Point		lastCellMLClick;

Boolean MLClick(Point pt, short modifiers)
{
	Rect		tRect, oldRect, uRect;
	Point		cPt, bPt, theCell;
	Point		curPt, oldPt = { 0, 0};
	Cmd			*theCmd;
	MenuHandle	tMenu;
	
	//	if ((modifiers & cmdKey) != 0 && (modifiers & optionKey) != 0)
	
	if (modifiers == thePrefs.FastDigitalEdition)	//& thePrefs.FastDigitalEdition
	{
		long	mresult;
		short	type, curSelec;
		Point	tP, myPt;
		Str255	aStr;
		
		tP = pt;
		ConvertPointAndType(&tP, &type);
		
		theCmd = GetMADCommand(tP.v, tP.h, curMusic->partition[ CurrentPat]);
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		switch (type)
		{
			case VolumeTE:
 				tMenu = GetMenu(161);
				InsertMenu(tMenu, hierMenu );
				
				curSelec = theCmd->vol;
				if (curSelec == 0xFF) curSelec = 0;
				
				myPt = pt;
				LocalToGlobal(&myPt);
				
				//	SetItemMark(tMenu, curSelec+1, 0xa5);
				
				mresult = PopUpMenuSelect(	tMenu,
										  myPt.v,
										  myPt.h,
										  curSelec+1);
				
				//	SetItemMark(tMenu, curSelec+1, 0);
				
				if (HiWord(mresult ) != 0 )
				{
					curMusic->hasChanged = true;
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Command Editing'");
					
					theCmd->vol = LoWord(mresult) -1;
					if (theCmd->vol < 0x10) theCmd->vol = 0xFF;
					
					UPDATE_Note(tP.v, tP.h);
				}
				DeleteMenu(GetMenuID(tMenu));
				DisposeMenu(tMenu);
				break;
				
			case ArguTE:
				tMenu = GetMenu(161);
				InsertMenu(tMenu, hierMenu );
				
				curSelec = theCmd->arg;
				
				myPt = pt;
				LocalToGlobal(&myPt);
				
				//	SetItemMark(tMenu, curSelec+1, 0xa5);
				
				mresult = PopUpMenuSelect(	tMenu,
										  myPt.v,
										  myPt.h,
										  curSelec+1);
				
				//	SetItemMark(tMenu, curSelec+1, 0);
				
				if (HiWord(mresult ) != 0 )
				{
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Command Editing'");
					
					theCmd->arg = LoWord(mresult) -1;
					
					UPDATE_Note(tP.v, tP.h);
				}
				DeleteMenu(GetMenuID(tMenu));
				DisposeMenu( tMenu);
				break;
				
			case InstruTE:
				InsertMenuItem(InstruMenu, "\p --", 0);
 				
				InsertMenu(InstruMenu, hierMenu );
				curSelec = theCmd->ins;
				
				myPt = pt;
				LocalToGlobal(&myPt);
				
				SetItemMark(InstruMenu, curSelec + 1, 0xa5);
				
				mresult = PopUpMenuSelect(	InstruMenu,
										  myPt.v - 6,
										  myPt.h - 3,
										  curSelec + 1);
				
				
				SetItemMark(InstruMenu, curSelec + 1, 0);
				
				if (HiWord(mresult) != 0)
				{
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Command Editing'");
					
					theCmd->ins = LoWord(mresult) -1;
					
					UPDATE_Note(tP.v, tP.h);
				}
				DeleteMenu(GetMenuID(InstruMenu));
				
				DeleteMenuItem(InstruMenu, 1);
				break;
				
			case NoteTE:
				InsertMenuItem(NoteMenu, "\p000", NUMBER_NOTES);
				InsertMenuItem(NoteMenu, "\pOFF", NUMBER_NOTES+1);
				
 				InsertMenu(NoteMenu, hierMenu );
				curSelec = theCmd->note;
				if (curSelec == 0xFF) curSelec = NUMBER_NOTES;
				if (curSelec == 0xFE) curSelec = NUMBER_NOTES+1;
				
				myPt = pt;	LocalToGlobal(&myPt);
				
				mresult = PopUpMenuSelect(	NoteMenu,
										  myPt.v - 6,
										  myPt.h - 3,
										  curSelec + 1);
				
				if (HiWord(mresult) != 0 )
				{
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Command Editing'");
					
					theCmd->note = LoWord(mresult) -1;
					if (theCmd->note == NUMBER_NOTES) theCmd->note = 0xFF;
					if (theCmd->note == NUMBER_NOTES+1) theCmd->note = 0xFE;
					
					UPDATE_Note(tP.v, tP.h);
				}
				DeleteMenu(GetMenuID(NoteMenu));
				
				DeleteMenuItem(NoteMenu, NUMBER_NOTES + 1);
				DeleteMenuItem(NoteMenu, NUMBER_NOTES + 1);
				break;
				
			case EffectTE:
				InsertMenu(EffectMenu, hierMenu);
				curSelec = theCmd->cmd + 1;
				
				myPt = pt;
				LocalToGlobal(&myPt);
				
				SetItemMark(EffectMenu, curSelec, 0xa5);
				
				mresult = PopUpMenuSelect(	EffectMenu,
										  myPt.v - 6,
										  myPt.h - 3,
										  curSelec);
				
				SetItemMark(EffectMenu, curSelec, 0);
				
				if (HiWord(mresult ) != 0 )
				{
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Command Editing'");
					
					theCmd->cmd = LoWord(mresult) -1;
					
					UPDATE_Note(tP.v, tP.h);
				}
				DeleteMenu(GetMenuID(EffectMenu));
				break;
		}
	}
	else
	{
		PLGetSelectRect(&oldRect, &myList);
		if (PtInRect(pt, &oldRect))
		{
			DragSelect();
		}
		else
		{
			cPt = pt;
			PLConvertPoint(&cPt, &myList);
			
			if (modifiers & shiftKey)
			{
				theCell.v = theCell.h = 0;
				
				if (PLGetSelect(&theCell, &myList))
				{
					cPt = theCell;
				}
			}
			else
			{
				PLSetSelect(-1, -1, -1, -1, &myList);
			}
			
			do
			{
				Boolean	Moved;
				
				Moved = false;
				
				GetMouse(&curPt);
				
				if (!PtInRect(curPt, &myList.rect))
				{
					PLSetMyIntList(&myList);
					
					if (curPt.v < myList.rect.top)		PLactionProcPartition(myList.yScroll, kControlUpButtonPart);
					if (curPt.v > myList.rect.bottom)	PLactionProcPartition(myList.yScroll, kControlDownButtonPart);
					
					if (curPt.h < myList.rect.left)		PLactionProcPartition(myList.xScroll, kControlUpButtonPart);
					if (curPt.h > myList.rect.right)		PLactionProcPartition(myList.xScroll, kControlDownButtonPart);
					
					Moved = true;
				}
				
				if (curPt.v != oldPt.v || curPt.h != oldPt.h || Moved == true)
				{
					oldPt = curPt;
					
					/* Erase old Rect */
					
					PLGetSelectRect(&oldRect, &myList);		oldRect.left++;
					/* Set New Selection Rect */
					
					bPt = curPt;
					PLConvertPoint(&bPt, &myList);
					
					SetMobiusRect(&myList.select, cPt.h, cPt.v, bPt.h, bPt.v);
					
					/* Update */
					
					if (SectRect(&oldRect, &myList.rect, &uRect)) InvalWindowRect(GetDialogWindow(EditorDlog), &uRect);
					
					PLGetSelectRect(&tRect, &myList);	tRect.left++;
					if (SectRect(&tRect, &myList.rect, &uRect)) InvalWindowRect(GetDialogWindow(EditorDlog), &uRect);
					
					if (SectRect(&oldRect, &tRect, &tRect)) ValidWindowRect(GetDialogWindow(EditorDlog), &tRect);
					
					UpdatePartitionWindow(EditorDlog);
				}
				
				WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				
				if (QDIsPortBuffered(GetDialogPort(EditorDlog)))
					QDFlushPortBuffer(GetDialogPort(EditorDlog), NULL);
				
			}
			while (Button());
		}
	}
	
	GetMouse(&curPt);
	if (TickCount() - lastTickCount < GetDblTime())	//LMGetDoubleTime())
	{
		if (curPt.h == lastCellMLClick.h && curPt.v == lastCellMLClick.v) return true;
	}
	lastTickCount = TickCount();
	lastCellMLClick = curPt;
	
	return false;
}

void SetInstruEditor(short instru)
{
	char	 	text[ 10];
	Str255	aStr;
	GrafPtr	savedPort;
	
	if (EditorDlog == NULL) return;

	GetPort(&savedPort);
	SetPortDialogPort(EditorDlog);

	curInstru = instru + 1;
	
	NTStr(3, curInstru, text);
	
	pStrcpy(aStr, "\pIns: ");
	pStrcat(aStr, MyC2PStr(text));
	
	TextFont(0);	TextSize(0);
//	RGBBackColor(&theColor);
	//SetDText(EditorDlog, 16, MyC2PStr(text));
	SetControlTitle(CheckBut[ 3], aStr);
	
//	strcpy((Ptr) aStr, curMusic->fid[ curInstru-1].name);
//	MyC2PStr((Ptr) aStr);
//	SetDText(EditorDlog, 18, aStr);
	TextFont(4);	TextSize(9);
	
//	BackColor(whiteColor);
	
	SetPort(savedPort);
}

void DoItemPressPartition(short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
	Cell				theCell;
	long				tempLong;
	short			temp, temp2, i, bogus, ctlPart, type;
	Rect				cellRect;
	GrafPtr			SavePort;
	Cmd				*theCommand;
	Boolean			DoubleClick;
	Point				myPt = {0}, lastCell = { -1, -1};
	ControlHandle		theControl;
	ControlActionUPP	MyControlUPP;
	Str255			aStr;
	
	GetPort(&SavePort);
	SetPortDialogPort(EditorDlog);
	
 	//	TextFont(4);
 	//	TextSize(9);
	
	if (theEvent.what == mouseDown)
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		PLScroll(myPt, &myList);
		
		
		lastCell.v = lastCell.h = 0;
		PLGetSelect(&lastCell, &myList);
		
		DoubleClick = false;
		
		if (PtInRect(myPt, &myList.rect))
		{
			UpdateCurrentCmd();
			
			if ((theEvent.modifiers & cmdKey) != 0 && (theEvent.modifiers & optionKey) == 0)
			{
				PLGetSelectRect(&cellRect, &myList);
				if (!PtInRect(myPt, &cellRect))
				{
					
					
					theCell.v = theCell.h = 0;
					//	PLSetSelect(-1, -1, -1, -1, &myList);
					
					theCell = myPt;
					PLConvertPoint(&theCell, &myList);
					
					PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
					
					
					
					//	Convert click + Find ins, cmd et allume le, sauf si keyboard piano!!!
				}
			}
			else
			{
				DoubleClick = MLClick(myPt, theEvent.modifiers);
			}
			
			theCell.v = theCell.h = 0;
			if (PLGetSelect(&theCell, &myList))
			{
				Point	aCell;
				
				if (theEvent.modifiers == thePrefs.FastDigitalEdition)	//& thePrefs.FastDigitalEdition
				{
					myPt = theEvent.where;
					GlobalToLocal(&myPt);
				}
				else GetMouse(&myPt);
				
				HiliteControl(OpenBut, 0);	HiliteControl(SaveBut, 0);
				HiliteControl(ShowBut, 0);	HiliteControl(PlayBut, 0);
				HiliteControl(UpBut, 0); 		HiliteControl(FXBut, 0);
				HiliteControl(DownBut, 0);
				
				if (OnOffInstru == false && OnOffFX == false && OnOffArg == false && OnOffVolume == false) ControlSwitch(18, EditorDlog, 255);
				else ControlSwitch(18, EditorDlog, 0);
				
				aCell = myPt;
				PLConvertPoint(&aCell, &myList);
				
				if (PtInRect(myPt, &myList.rect))
				{
					if (aCell.v != lastCell.v || aCell.h != lastCell.h || MADDriver->Reading == false)
					{
						MADDriver->Pat = CurrentPat;
						if (thePrefs.MusicTrace) MADDriver->PartitionReader = aCell.v;
						SetCommandTrack(aCell.h, aCell.v);
					}
				}
				
				if (MADDriver->Reading == false)
				{
					GetKeys(km);
					if (IsPressed(0x37))	// On joue la note = PlayCrsr
					{
						short		VolExtCopy[ MAXTRACK];
						long		rePat;
						long		reReader;
						long		rePL;
						
					PlaySelection:
						
						rePat = MADDriver->Pat;
						reReader = MADDriver->PartitionReader;
						rePL = MADDriver->PL;
						
						MADDriver->PartitionReader = theCell.v;
						SetCommandTrack(theCell.h, theCell.v);
						
						HiliteControl(PlayBut, kControlButtonPart);
						
						DoRemember();
						
						for (i = 0; i < myList.select.left; i++)			MADDriver->TrackReading[ i] = false;
						for (i = myList.select.right+1; i < MAXTRACK; i++)	MADDriver->TrackReading[ i] = false;
						
						MADDriver->Reading = true;
						while (Button() == true  && MADDriver->PartitionReader <= myList.select.bottom && MADDriver->PartitionReader >= myList.select.top && rePat == MADDriver->Pat)
						{
							DoGlobalNull();
							WaitNextEvent(everyEvent, &theEvent, 1, NULL);
						}
						MADDriver->Reading = false;
						while (Button() == true)
						{
							DoGlobalNull();
							WaitNextEvent(everyEvent, &theEvent, 1, NULL);
						}
						DoPause();
						
						MADDriver->Pat = rePat;
						MADDriver->PartitionReader = reReader;
						MADDriver->PL = rePL;
						
						for (i = 0; i < MAXTRACK; i++)					MADDriver->TrackReading[ i] = true;		// Restore
						
						HiliteControl(PlayBut, 0);
					}
					/*	else if (DoubleClick)
					 {
					 
					 
					 }*/
					else if (DidTryToDrag == false)
					{
						if (DoubleClick)
						{
							ShowCurrentCmdNote();
							SetCommandTrack(aCell.h, aCell.v);
						}
						
						theCell.v = theCell.h = 0;
						if (PLGetSelect(&theCell, &myList))
						{
							if (EditorKeyRecording == false && lastCell.h == theCell.h &&  lastCell.v==theCell.v)
							{
								ConvertPointAndType(&myPt, &type);
								DoNullCmdWindow();
								ActiveCmdWindow(type);
							}
						}
					}
					
					DidTryToDrag = false;
				}
			}
			else
			{
				HiliteControl(OpenBut, 255);
				HiliteControl(SaveBut, 255);
				HiliteControl(ShowBut, 255);
				HiliteControl(PlayBut, 255);
				HiliteControl(UpBut, 255);
				HiliteControl(DownBut, 255);
				HiliteControl(FXBut, 255);
				
				ControlSwitch(18, EditorDlog, 255);
			}
		}
	}
	
	switch (whichItem)
	{
		case 18:
			if (GetControlHilite(FillBut) == 0)	// && )
			{
				short X, Y;
				
				MyTrackControl(FillBut, theEvent.where, NULL);
				
				curMusic->hasChanged = true;
				
				SaveUndo(UPattern, CurrentPat, "\pUndo 'Fill Digital Editor'");
				
				for (X = myList.select.left; X <= myList.select.right; X++)
				{
					for (Y = myList.select.top; Y <= myList.select.bottom; Y++)
					{
						theCommand = GetMADCommand(Y, X, curMusic->partition[ CurrentPat]);
						
						if (OnOffInstru)		theCommand->ins 	= curInstru;		//else theCommand->ins = 0;
						if (OnOffFX)		theCommand->cmd 	= DefaultFX;		//else theCommand->cmd = 0;
						if (OnOffArg)		theCommand->arg 	= DefaultArg;		//else theCommand->arg = 0;
						if (OnOffVolume)
						{
							if (DefaultVol == 0)	theCommand->vol	= 0xFF;
							else theCommand->vol = DefaultVol;
						}
						
						UPDATE_NoteBOUCLE(Y, X);
					}
				}
				
				UPDATE_NoteFINISH();
				
				UpdatePartitionWindow(EditorDlog);
			}
			break;
			
		case 40:
		case 37:
		case 38:
		case 39:
			//	RGBBackColor(&theColor);
			if(MyTrackControl((ControlHandle) CheckBut[ whichItem - 37], theEvent.where, NULL))
			{
				SetControlValue((ControlHandle) CheckBut[ whichItem - 37], !GetControlValue((ControlHandle) CheckBut[ whichItem - 37]));
				
				switch (whichItem)
				{
					case 40:	OnOffInstru = !OnOffInstru;	break;	//Instru
					case 37:	OnOffFX = !OnOffFX;			break;	// FX
					case 38:	OnOffArg = !OnOffArg;		break;	// Arg
					case 39:	OnOffVolume= !OnOffVolume;	break;	// Volume
				}
				
				if (OnOffInstru == false && OnOffFX == false && OnOffArg == false && OnOffVolume == false)
				{
					ControlSwitch(18, EditorDlog, 255);
				}
				else
				{
					ControlSwitch(18, EditorDlog, 0);
				}
			}
			//	BackColor(whiteColor);
			break;
			
		case 30:
		{
			short 		itemType;
			Handle		itemHandle;
			Rect			itemRect;
			long			mresult;
			MenuHandle	curMenu;
			
			curMenu = GetMenu(161);
			
			InsertMenu(curMenu, hierMenu );
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(curMenu, DefaultVol +1, 0xa5);
			
			mresult = PopUpMenuSelect(	curMenu,
									  myPt.v,
									  myPt.h,
									  DefaultVol+1);
			
			SetItemMark(curMenu, DefaultVol +1, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				DefaultVol = LoWord(mresult)-1;
				
				pStrcpy(aStr, "\pVol: ");
				
				aStr[ 6] = EArgu[ DefaultVol][ 0];
				aStr[ 7] = EArgu[ DefaultVol][ 1];
				aStr[ 0] += 2;
				
				TextFont(0);	TextSize(0);
				//	RGBBackColor(&theColor);
				SetControlTitle(CheckBut[ 2], aStr);
				//	BackColor(whiteColor);
				TextFont(4);	TextSize(9);
				
				OnOffVolume = true;
				SetControlValue((ControlHandle) CheckBut[ 2], OnOffVolume);
			}
			DeleteMenu(GetMenuID(curMenu));
			DisposeMenu(curMenu);
		}
			break;
			
		case 31:
		{
			short 		itemType;
			Handle		itemHandle;
			Rect			itemRect;
			long			mresult;
			MenuHandle	curMenu;
			
			curMenu = GetMenu(161);
			
			InsertMenu(curMenu, hierMenu );
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(curMenu, DefaultArg + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	curMenu,
									  myPt.v,
									  myPt.h,
									  DefaultArg+1);
			
			
			
			SetItemMark(curMenu, DefaultArg + 1, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				DefaultArg = LoWord(mresult) -1;
				
				pStrcpy(aStr, "\pArg: ");
				
				aStr[ 6] = EArgu[ DefaultArg][ 0];
				aStr[ 7] = EArgu[ DefaultArg][ 1];
				aStr[ 0] += 2;
				
				TextFont(0);	TextSize(0);
				//	RGBBackColor(&theColor);
				SetControlTitle(CheckBut[ 1], aStr);
				//	BackColor(whiteColor);
				TextFont(4);	TextSize(9);
				
				OnOffArg = true;
				SetControlValue((ControlHandle) CheckBut[ 1], OnOffArg);
			}
			DeleteMenu(GetMenuID(curMenu));
			DisposeMenu(curMenu);
		}
			break;
			
		case 32:
		{
			short 		itemType;
			Handle		itemHandle;
			Rect			itemRect;
			long			mresult;
			
			InsertMenu(EffectMenu, hierMenu );
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(EffectMenu, DefaultFX+1, 0xa5);
			
			mresult = PopUpMenuSelect(	EffectMenu,
									  myPt.v,
									  myPt.h,
									  DefaultFX+1);
			
			SetItemMark(EffectMenu, DefaultFX+1, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				DefaultFX = LoWord(mresult)-1;
				
				pStrcpy(aStr, "\pFX: ");
				
				aStr[ 5] = EEffect[ DefaultFX];
				aStr[ 0] ++;
				
				TextFont(0);	TextSize(0);
				//	RGBBackColor(&theColor);
				SetControlTitle(CheckBut[ 0], aStr);
				//	BackColor(whiteColor);
				TextFont(4);	TextSize(9);
				
				OnOffFX = true;
				SetControlValue((ControlHandle) CheckBut[ 0], OnOffFX);
			}
			DeleteMenu(GetMenuID(EffectMenu));
		}
			break;
			
		case 21:	// Step
		case 20:
		{
			short 		itemType;
			Handle		itemHandle;
			Rect			itemRect;
			long			mresult;
			char			text[ 10];
			MenuHandle	curMenu;
			
			curMenu = GetMenu(178);
			
			InsertMenu(curMenu, hierMenu );
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(curMenu, curStep, 0xa5);
			
			mresult = PopUpMenuSelect(	curMenu,
									  myPt.v,
									  myPt.h,
									  curStep);
			
			SetItemMark(curMenu, curStep, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				curStep = LoWord(mresult);
				
				NTStr(1, curStep, text);
				
				TextFont(0);	TextSize(0);
				//	RGBBackColor(&theColor);
				SetDText(whichDialog, 20, MyC2PStr(text));
				//	BackColor(whiteColor);
				TextFont(4);	TextSize(9);
			}
			DeleteMenu(GetMenuID(curMenu));
			DisposeMenu( curMenu);
		}
			break;
			
		case 17:
		{
			short 	itemType;
			Handle	itemHandle;
			Rect		itemRect;
			long		mresult;
			char		text[ 10];
			
			InsertMenuItem(InstruMenu, "\pNo Ins", 0);
			
			InsertMenu(InstruMenu, hierMenu );
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(InstruMenu, curInstru + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	InstruMenu,
									  myPt.v,
									  myPt.h,
									  curInstru + 1);
			
			SetItemMark(InstruMenu, curInstru + 1, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				curInstru = LoWord(mresult) - 1;
				
				NTStr(3, curInstru, text);
				
				pStrcpy(aStr, "\pIns: ");
				pStrcat(aStr, MyC2PStr(text));
				
				TextFont(0);	TextSize(0);
				//	RGBBackColor(&theColor);
				SetControlTitle(CheckBut[ 3], aStr);
				
				//	strcpy((Ptr) aStr, curMusic->fid[ curInstru-1].name);
				//	MyC2PStr((Ptr) aStr);
				//	SetDText(whichDialog, 18, aStr);
				
				TextFont(4);	TextSize(9);
				
				//	BackColor(whiteColor);
				
				NSelectInstruList(curInstru - 1, -1);
				
				OnOffInstru = true;
				SetControlValue((ControlHandle) CheckBut[ 3], OnOffInstru);
			}
			DeleteMenu(GetMenuID(InstruMenu));
			DeleteMenuItem(InstruMenu, 1);
		}
			break;
			
		case 13:
			if(MyTrackControl(TraceBut, theEvent.where, NULL))
			{
				thePrefs.MusicTrace = !thePrefs.MusicTrace;
				
				if (thePrefs.MusicTrace) HiliteControl(TraceBut, kControlButtonPart);
				else HiliteControl(TraceBut, 0);
			}
			break;
			
		case 14:
			if(MyTrackControl(FindBut, theEvent.where, NULL))
			{
				HandleOtherChoice(6);
			}
			break;
			
		case 2:
			if (GetControlHilite(InfoBut) == 0 && MyTrackControl(InfoBut, theEvent.where, NULL))
			{
				DialogPatternInfo(CurrentPat);
			}
			break;
			
		case 3:		
			if (GetControlHilite(OpenBut) == 0 && MyTrackControl(OpenBut, theEvent.where, NULL))
			{
				OpenPcmdFile(NULL);
			}
			break;
			
		case 23:
			if (GetControlHilite(DownBut) == 0  && MyTrackControl(DownBut, theEvent.where, NULL))
			{
				DoKeyPressEditor('/');
			}
			break;
			
		case 10:
			if (GetControlHilite(UpBut) == 0  && MyTrackControl(UpBut, theEvent.where, NULL))
			{
				DoKeyPressEditor('*');
			}
			break;
			
		case 12:
			if (GetControlHilite(FXBut) == 0)// && MyTrackControl(FXBut, theEvent.where, NULL))
			{
				short	itemType;
				Rect	itemRect;
				Handle	itemHandle;
				
				//	HiliteControl(FXBut, kControlButtonPart);
				
				GetDialogItem(whichDialog, 12, &itemType, &itemHandle, &itemRect);
				
				temp = PressPPDGMenu(&itemRect);
				
				if (temp != -1)
				{
					Pcmd*	myPcmd;
					
					myPcmd = CreatePcmdFromSelection();
					
					CallPPDGPlugIns(temp-1, myPcmd);
					
					SaveUndo(UPattern, CurrentPat, "\pUndo 'Digital Editor Plug'");
					
					theCell.h = theCell.v = 0;
					if (PLGetSelect(&theCell, &myList)) PasteCmdEditor(theCell, myPcmd);
					
					MyDisposePtr((Ptr*) &myPcmd);
				}
				
				//	HiliteControl(FXBut, 0);
			}
			break;
			
		case 11:
			if (PianoRecording)
			{
				Erreur(82, -2);
				break;
			}
			
			if(MyTrackControl(RecBut, theEvent.where, NULL))
			{
				EditorKeyRecording = !EditorKeyRecording;
				if (EditorKeyRecording)
				{
					HiliteControl(RecBut, kControlButtonPart);
					
					/*	RGBBackColor(&theColor);
					 for (i = 0; i < 4; i++) HiliteControl(CheckBut[ i], 0);
					 BackColor(whiteColor);*/
				}
				else
				{
					HiliteControl(RecBut, 0);
					
					/*	RGBBackColor(&theColor);
					 for (i = 0; i < 4; i++) HiliteControl(CheckBut[ i], 255);
					 BackColor(whiteColor);*/
				}
			}
			break;
			
		case 4:
			if (GetControlHilite(SaveBut) == 0  && MyTrackControl(SaveBut, theEvent.where, NULL))
			{
				SavePcmdFile(CreatePcmdFromSelection());
			}
			break;
			
		case 6:
			if (GetControlHilite(PrefBut) == 0  && MyTrackControl(PrefBut, theEvent.where, NULL))
			{
#include "Help.h"
				
				ShowPrefs(DIGITAL);
			}
			break;
			
		case 7:
			if (GetControlHilite(PlayBut) == 0)
			{
				whichItem = -1;
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) goto PlaySelection;
			}
			break;
			
		case 8:
			if (GetControlHilite(ShowBut) == 0  && MyTrackControl(ShowBut, theEvent.where, NULL))
			{
				theCell.v = theCell.h = 0;
				if (PLGetSelect(&theCell, &myList))
				{
					ShowCurrentCmdNote();
					SetCommandTrack(theCell.h, theCell.v);
				}
			}
			break;
			
		case 9:
		case 5:
			if (myPt.h < myList.rect.left && myPt.v < myList.rect.top)
			{
				PLSetSelect(0, 0, curMusic->header->numChn, curMusic->partition[ CurrentPat]->header.size, &myList);
			}
			else if (myPt.h < myList.rect.left)
			{
				short start;
				
				theCell.v = (myPt.v - myList.rect.top) / myList.HCell;
				theCell.v += GetControlValue(myList.yScroll);
				
				if ((theEvent.modifiers & shiftKey) != 0)
				{
					if (theCell.v > myList.select.top) start = myList.select.top;
					else
					{
						start = theCell.v;
						theCell.v = myList.select.bottom;
					}
				}
				else start = theCell.v;
				
				PLSetSelect(0, start, curMusic->header->numChn-1, theCell.v, &myList);
				
				if ((theEvent.modifiers & cmdKey) != 0)
				{
					Boolean IsPlay = MADDriver->Reading;
					Boolean	IsJump = MADDriver->JumpToNextPattern;
					
					MADDriver->JumpToNextPattern = false;
					MADDriver->PartitionReader = start;
					
					DoPlay();
					
					while (Button())
					{
						DoGlobalNull();
						WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					}
					
					if (IsPlay == false) DoPause();
					
					MADDriver->JumpToNextPattern = IsJump;
				}
			}
			else if (myPt.v < myList.rect.top)
			{
				short	start;
				Rect	caRect;
				
				theCell.h = (myPt.h - myList.rect.left) / myList.LCell;
				theCell.h += GetControlValue(myList.xScroll);
				
				if ((theEvent.modifiers & cmdKey) != 0)			// Mute
				{
					MADDriver->Active[ theCell.h] = !MADDriver->Active[ theCell.h];
					
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
					
					if (noActive <= 1 && MADDriver->Active[ theCell.h] == true)
					{
						for (i = 0, noActive = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = true;
					}
					else
					{
						for (i = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = false;
						MADDriver->Active[ theCell.h] = true;
					}
					
					UPDATE_TrackActive();
				}
				else if ((theEvent.modifiers & shiftKey) != 0)
				{
					if (theCell.h > myList.select.left) start = myList.select.left;
					else
					{
						start = theCell.h;
						theCell.h = myList.select.right;
					}
					
					PLSetSelect(start, 0, theCell.h, curMusic->partition[ CurrentPat]->header.size-1, &myList);
				}
				else
				{
					start = theCell.h;
					PLSetSelect(start, 0, theCell.h, curMusic->partition[ CurrentPat]->header.size-1, &myList);
				}
			}
			
			theCell.v = theCell.h = 0;
			if (PLGetSelect(&theCell, &myList))
			{
				if (theCell.v != lastCell.v || theCell.h != lastCell.h || MADDriver->Reading == false)
				{
					MADDriver->Pat = CurrentPat;
					if (thePrefs.MusicTrace) MADDriver->PartitionReader = theCell.v;
					SetCommandTrack(theCell.h, theCell.v);
				}
			}
			break;
	}
	
	theCell.v = theCell.h = 0;
	if (PLGetSelect(&theCell, &myList))
	{
		HiliteControl(OpenBut, 0);		HiliteControl(SaveBut, 0);
		HiliteControl(ShowBut, 0);		HiliteControl(PlayBut, 0);
		HiliteControl(UpBut, 0);			HiliteControl(FXBut, 0);
		HiliteControl(DownBut, 0);
		
		if (OnOffInstru == false && OnOffFX == false && OnOffArg == false && OnOffVolume == false) ControlSwitch(18, EditorDlog, 255);
		else ControlSwitch(18, EditorDlog, 0);
	}
	else
	{
		HiliteControl(OpenBut, 255);	HiliteControl(SaveBut, 255);
		HiliteControl(ShowBut, 255);	HiliteControl(PlayBut, 255);
		HiliteControl(UpBut, 255);	HiliteControl(FXBut, 255);
		HiliteControl(DownBut, 0);	ControlSwitch(18, EditorDlog, 255);
	}
	
	SetPort(SavePort);
}

void UpdateCompleteEditor(void)
{
	if (EditorDlog != NULL)
	{
		ClosePartitionWindow();
		CreatePartitionWindow();
	}
}

void PLScrollIntPartition(short curVal, short sVal, long lRefCon)
{
	short			maxValue, minValue, XX, itemType;
	RgnHandle		aRgn;
	Handle			itemHandle;
	RgnHandle		clipRgn;
	
	
	if (sVal != curVal)
	{
		Rect	aRect = myList.rect, bRect;
		
		switch (lRefCon)
		{
			case yScrollNum:
				aRgn = NewRgn();
				
				aRect.left = 0;
				
				ScrollRect(&aRect, 0, (sVal - curVal) * myList.HCell, aRgn);
				
				if (IsRegionRectangular(aRgn) == false)
				{
					InvalWindowRgn(GetDialogWindow(EditorDlog), aRgn);
					UpdatePartitionWindow(EditorDlog);
				}
				else
				{
					if (sVal > curVal) MyLUpdateSpecific(curVal, sVal);
					else MyLUpdateSpecific(PLGetMaxYValue(&myList) - (curVal - sVal) - 1, PLGetMaxYValue(&myList));
					
					DrawLeft(aRgn, false);
				}
				
				
				
				//	DoNullEditor();
				
				DisposeRgn(aRgn);
				break;
				
			case xScrollNum:
				aRgn = NewRgn();
				
				CreateCurRect();
				
				ScrollRect(&aRect, (sVal - curVal) * myList.LCell, 0, aRgn);
				
				InvalWindowRgn(GetDialogWindow(EditorDlog), aRgn);
				UpdatePartitionWindow(EditorDlog);
				
				DrawEditorUp();
				
				DisposeRgn(aRgn);
				break;
		}
	}
}

void CreatePartitionWindow(void)
{
	Rect				itemRect, tempRect, dataBounds;
	Handle			itemHandle;
	short			i, itemType, itemHit, temp, iWidth;
	Point				cSize;
	FontInfo			ThisFontInfo;
	Str255			String;
	Cmd				aCmd;
	ControlHandle		aCtl;

	if (EditorDlog != NULL)
	{
		SetWindEtat(GetDialogWindow(EditorDlog));
		return;
	}

	OnOffFX = OnOffArg = OnOffVolume = false;
	OnOffInstru = true;
	curInstru = 0;
	curStep = 1;
	
	DefaultFX = 0;
	DefaultArg = 0;
	DefaultVol = 0x00;
	
	SetItemMark(EditorMenu, 1, checkMark);
	
	EditorDlog = GetNewDialog(139, NULL, GetDialogWindow(ToolsDlog));
	SetWindEtat(GetDialogWindow(EditorDlog));
	
	SetPortDialogPort(EditorDlog);
	
	myList.select.left = -1;		myList.select.right = -1;
	myList.select.top = -1;			myList.select.bottom = -1;
	
	myList.type = 0;
	myList.aDia = EditorDlog;
	
	Gray[ 0].red = Gray[ 0].green = Gray[ 0].blue = 48059;
	Gray[ 1].red = Gray[ 1].green = Gray[ 1].blue = 43690;
	Gray[ 2].red = Gray[ 2].green = Gray[ 2].blue = 34952;
	Gray[ 3].red = Gray[ 3].green = Gray[ 3].blue = 30583;
	
	yellC			= thePrefs.yellC;
	
	greenC.red 	= 39321;
	greenC.green 	= 65535;
	greenC.blue	= 39321;
	
	blueC.red 		= 26214;
	blueC.green 	= 65535;
	blueC.blue		= 65535;
	
	redC.red		= 65535;
	redC.green	= 13107;
	redC.blue		= 39321;
	
	if (OnOffInstru == false && OnOffFX == false && OnOffArg == false && OnOffVolume == false)
	{
		ControlSwitch(18, EditorDlog, 255);
	}
	else
	{
		ControlSwitch(18, EditorDlog, 0);
	}
	
	SetRect(&itemRect, 0, 0, 30, 16);
	myList.xScroll = NewControl(	GetDialogWindow(EditorDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									xScrollNum);

	SetRect(&itemRect, 0, 0, 30, 16);
	myList.yScroll = NewControl(	GetDialogWindow(EditorDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									yScrollNum);

	
	GetDialogItem(EditorDlog , 12, &itemType, &itemHandle, &itemRect);
	FXBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							185,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(EditorDlog , 2, &itemType, &itemHandle, &itemRect);
	InfoBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							149,
							kControlBevelButtonNormalBevelProc,
							0);
	GetDialogItem(EditorDlog , 18, &itemType, &itemHandle, &itemRect);
	FillBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							218,
							kControlBevelButtonNormalBevelProc,
							0);
	GetDialogItem(EditorDlog , 3, &itemType, &itemHandle, &itemRect);
	OpenBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							152,
							kControlBevelButtonNormalBevelProc,
							0);
	GetDialogItem(EditorDlog , 4, &itemType, &itemHandle, &itemRect);
	SaveBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							151,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(EditorDlog , 6, &itemType, &itemHandle, &itemRect);
	PrefBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							174,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(EditorDlog , 8, &itemType, &itemHandle, &itemRect);
	ShowBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							159,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(EditorDlog , 7, &itemType, &itemHandle, &itemRect);
	PlayBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							160,
							kControlBevelButtonNormalBevelProc,
							0);
	GetDialogItem(EditorDlog , 10, &itemType, &itemHandle, &itemRect);
	UpBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							205,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(EditorDlog , 23, &itemType, &itemHandle, &itemRect);
	DownBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							206,
							kControlBevelButtonNormalBevelProc,
							0);


	GetDialogItem(EditorDlog , 11, &itemType, &itemHandle, &itemRect);
	RecBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							180,
							kControlBevelButtonNormalBevelProc,
							0);
							
	GetDialogItem(EditorDlog , 13, &itemType, &itemHandle, &itemRect);
	TraceBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							217,
							kControlBevelButtonNormalBevelProc,
							0);
							
	GetDialogItem(EditorDlog , 14, &itemType, &itemHandle, &itemRect);
	FindBut = NewControl(	GetDialogWindow(EditorDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							194,
							kControlBevelButtonNormalBevelProc,
							0);
	
	for (i = 0; i < 4; i++)
	{
		GetDialogItem(EditorDlog , 37 + i, &itemType, &itemHandle, &itemRect);
		CheckBut[ i] = NewControl(GetDialogWindow(EditorDlog), &itemRect, "\p", true, 0, 0, 1, 1, 0);
		//HiliteControl(CheckBut[ i], 255);
	}
	SetControlTitle(CheckBut[ 3], "\pIns: 000");		SetControlValue(CheckBut[ 3], true);
	SetControlTitle(CheckBut[ 2], "\pVol: 00");
	SetControlTitle(CheckBut[ 1], "\pArg: 00");
	SetControlTitle(CheckBut[ 0], "\pFX: 0");
	
	EditorKeyRecording = false;
	
	if (thePrefs.MusicTrace) HiliteControl(TraceBut, kControlButtonPart);
	
	HiliteControl(OpenBut, 0);	HiliteControl(SaveBut, 0);
	HiliteControl(ShowBut, 0);	HiliteControl(PlayBut, 0);
	HiliteControl(UpBut, 0); 		HiliteControl(FXBut, 0);
	HiliteControl(DownBut, 0); 
	
	TextFont(4);	TextSize(9);
	
	NoteCharTemp[1] = NoteCharTemp[2] = NoteCharTemp[3] = ' ';
	CurRect.left = CurRect.right = CurRect.bottom = CurRect.top = 0;
	CurChar.left = CurChar.right = CurChar.bottom = CurChar.top = 0;
	
	GetFontInfo(&ThisFontInfo);			/* Get the current font sizes */
	LChar = ThisFontInfo.widMax;
	myList.HCell = cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading - thePrefs.LinesHeight;
	
	tLH = thePrefs.LinesHeight/2;
	
	ReaderCopy = MADDriver->PartitionReader;		CurrentPat = MADDriver->Pat;

	DiffStartV = 0;		DiffStartH = 0;
	
	InternalEditorInit();
		
	ShowWindow(GetDialogWindow(EditorDlog));
	SelectWindow2(GetDialogWindow(EditorDlog));

	if (DragManagerUse)
	{
		MyTrackingHandlerUPP 		= NewDragTrackingHandlerUPP(MyTrackingEditor);
		MyReceiveDropHandlerUPP 	= NewDragReceiveHandlerUPP(MyReceiveDropEditor);
		mySendDataUPP 				= NewDragSendDataUPP(MySendDataProcEditor);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingHandlerUPP, GetDialogWindow(EditorDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveDropHandlerUPP, GetDialogWindow(EditorDlog), (void *) NULL);
	}

	ApplyBut(0);
	
	PLSetSelect(0, 0, 0, 0, &myList);
	
	CreateCurRect();
	InvalWindowRect(GetDialogWindow(EditorDlog), &CurRect);
	
	if (GetIns(&curInstru, NULL))
	{
		SetInstruEditor(curInstru);
	}
}

void InternalEditorInit()
{
	Rect			itemRect, tempRect, dataBounds;
	Handle			itemHandle;
	short			itemType, itemHit, temp, iWidth;
	Point			cSize;
	FontInfo		ThisFontInfo;
	Str255			String;
	Cmd				aCmd;
	GrafPtr			SavePort;
	Rect	caRect;

	if (EditorDlog == NULL) return;
	
	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
	
	/*** Détermine la longueur d'une cell ***/

	aCmd.ins	=	33;		aCmd.note			=	20;
	aCmd.cmd	=	4;		aCmd.arg			=	23;
	aCmd.vol	=	23;

	CreateNoteString(&aCmd, String, false);
	iWidth = StringWidth(String) + 6;
	myList.LCell = cSize.h = iWidth;

	GetPortBounds(GetDialogPort(EditorDlog), &caRect);
	
	GetDialogItem(EditorDlog, 1, &itemType, &itemHandle, &myList.rect);
	myList.rect.left = LEFTBORD;
	myList.rect.right = caRect.right - 15;	//myList.rect.left +  curMusic->header->numChn * myList.LCell;
	myList.rect.bottom = caRect.bottom - 15;
	
	SetNewPat();
	
	SetMaxWindow(	myList.rect.left + 15 + curMusic->header->numChn * cSize.h,
					myList.rect.top + 15 + curMusic->partition[ CurrentPat]->header.size * myList.HCell,
					EditorDlog);
	
	theEvent.what = 0;
	DoGrowPartition();
	
	InvalWindowRect(GetDialogWindow(EditorDlog), &myList.rect);
	
	totItem = 0;
	if (thePrefs.DigitalInstru)		totItem++;
	if (thePrefs.DigitalNote)		totItem++;
	if (thePrefs.DigitalEffect)		totItem++;
	if (thePrefs.DigitalArgu)		totItem++;
	if (thePrefs.DigitalVol)		totItem++;
	
	SetPort(SavePort);
}

void ClosePartitionWindow(void)
{
	Point	Start;
	Rect	caRect;

	if (EditorDlog != NULL)
	{
		SetPortDialogPort(EditorDlog);
	
		if (DragManagerUse)
		{
			RemoveTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(EditorDlog));
			RemoveReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(EditorDlog));
		
			DisposeDragTrackingHandlerUPP(MyTrackingHandlerUPP);
			DisposeDragReceiveHandlerUPP(MyReceiveDropHandlerUPP);
			DisposeDragSendDataUPP(mySendDataUPP);
		}
		
		GetPortBounds(GetDialogPort(EditorDlog), &caRect);
		
		thePrefs.WinHi[ GetWRefCon(GetDialogWindow(EditorDlog))] = caRect.bottom;
		thePrefs.WinLarg[ GetWRefCon(GetDialogWindow(EditorDlog))] = caRect.right;
		thePrefs.WinEtat[ GetWRefCon(GetDialogWindow(EditorDlog))] = 1;
		
		Start.h = Start.v = 0;
		LocalToGlobal(&Start);
		thePrefs.WinPos[ GetWRefCon(GetDialogWindow(EditorDlog))].v = Start.v;
		thePrefs.WinPos[ GetWRefCon(GetDialogWindow(EditorDlog))].h = Start.h;

	//	LDispose(PatList);
		DisposeDialog(EditorDlog);
		
		ApplyBut(255);
	}
	EditorDlog = NULL;
	
	SetItemMark(EditorMenu, 1, noMark);
}

void DigitalEditorProcess(short whichNote, short *eff, short *arg, short *volCmd)
{
	Point 				theCell;
	Rect					cellRect, eRect, sRect;
	Str255				theStr;
	short				theNo, temp;
	Cmd					*theCommand;
	GrafPtr				SavePort;
	
	if (EditorDlog == NULL) return;
	if (!EditorKeyRecording) return;
	
	if (PianoRecording)
	{
		EditorKeyRecording = false;
		HiliteControl(RecBut, 0);
		return;
	}
	
	GetPort(&SavePort);
	SetPortDialogPort(EditorDlog);
	
	theCell.v = theCell.h = 0;
	if (PLGetSelect(&theCell, &myList))
	{
		theCommand = GetMADCommand(theCell.v, theCell.h, curMusic->partition[ CurrentPat]);
		
		//	if (InstruListDlog != NULL)
		//	{
		//	aCell.v = 0;	aCell.h = 0;
		//	if (GetIns(&aCell.v, NULL))
		{
			//	if (whichNote == 0) aCell.v = -1;
			
			SaveUndo(UPattern, CurrentPat, "\pUndo 'Key Press in Digital Editor'");
			
			if (OnOffInstru)		theCommand->ins 	= curInstru;		//else theCommand->ins = 0;
			theCommand->note = whichNote;
			if (OnOffFX)		theCommand->cmd 	= DefaultFX;		//else theCommand->cmd = 0;
			if (OnOffArg)		theCommand->arg 	= DefaultArg;		//else theCommand->arg = 0;
			if (OnOffVolume)
			{
				if (DefaultVol == 0)	theCommand->vol	= 0xFF;
				else theCommand->vol = DefaultVol;
			}
			//else theCommand->vol = 0xFF;
			
			if (eff) *eff = theCommand->cmd;
			if (arg) *arg = theCommand->arg;
			if (volCmd) *volCmd = theCommand->vol;
			
			UPDATE_Note(theCell.v, theCell.h);	
			
			theCell.v += curStep;
			MADDriver->Pat = CurrentPat;
			
			if (theCell.v >= curMusic->partition[ CurrentPat]->header.size)
			{
				if (MADDriver->JumpToNextPattern && thePrefs.patternWrapping == false)
				{
					short newPL = MADDriver->PL;
					newPL++;
					if (newPL >= curMusic->header->numPointers) newPL--;
					else
					{
						theCell.v -= curMusic->partition[ CurrentPat]->header.size;
						
						MADDriver->PL = newPL;
						MADDriver->Pat = curMusic->header->oPointers[ MADDriver->PL];
					}
				}
				else
				{
					theCell.v -= curMusic->partition[ CurrentPat]->header.size;
					MADDriver->Pat = CurrentPat;
				}
			}
			
			PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			
			if (thePrefs.MusicTrace) MADDriver->PartitionReader = theCell.v;
			SetCommandTrack(theCell.h, theCell.v);
			
			PLAutoScroll(&myList);
		}
		//	else Erreur(13, 0);
		//	}
		//	else Erreur(13, 0);
	}
	
	SetPort(SavePort);
}

void DoKeyPressEditor(short theChar)
{
	Point 				theCell;
	Rect				cellRect, eRect, sRect;
	Str255				theStr;
	Rect					bRect;
	Handle				itemHandle;
	short				theNo, temp, itemType;
	Cmd					*theCommand;
	GrafPtr				SavePort;
	unsigned	long			ll;
	
	
 	if (curMusic == NULL) return;
 	
 	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
 	
 	if(	theChar == 0x0B ||
	   theChar == 0x0C ||
	   theChar == 0x04 ||
	   theChar == 0x01)
 	{
 		PLDoArrows(theChar, &myList);
 	}
 	
 	if(	theChar == 0x1E ||
	   theChar == 0x03 ||
	   theChar == 0x1F ||
	   theChar == 0x1C ||
	   theChar == 9 ||
	   theChar == 0x0D ||
	   theChar == 0x1D )
 	{
		UpdateCurrentCmd();
		
		theCell.v = theCell.h = 0;
		if (PLGetSelect(&theCell, &myList))
		{
			Rect	rect = myList.select;
			
	 		if (theEvent.modifiers & shiftKey)
			{
				DesactivateCmdWindow();
				UpdatePartitionWindow(EditorDlog);
				
		 		switch (theChar)
		 		{
		 			case 0x1E:	rect.top--;		break;
		 			case 0x0D:
		 			case 0x1F:	rect.bottom++;	break;
		 			case 0x1C:	rect.left--;	break;
					case 0x1D:	rect.right++;	break;
				}
				
				PLSetSelect(rect.left, rect.top, rect.right, rect.bottom, &myList);
			}
			else
			{
		 		switch (theChar)
		 		{
			 		case 0x1E:	theCell.v -= curStep;		break;
			 		case 0x1F:	theCell.v += curStep;		break;
						
			 		case 0x03:
			 		case 0x0D:
			 			DoItemPressPartition(18, EditorDlog);	// Press Fill
			 			GetDialogItem(EditorDlog, 18, &itemType, &itemHandle, &bRect);
			 			HiliteControl((ControlHandle) itemHandle, kControlButtonPart);
			 			Delay(7, &ll);
			 			HiliteControl((ControlHandle) itemHandle, 0);
			 			theCell.v += curStep;
						break;
						
			 		case 0x1C:	theCell.h--;				break;
						
						
						//	case 9:	//tab
					case 0x1D:	theCell.h++;		break;
				}
				
				MADDriver->Pat = CurrentPat;
				
				if (theCell.v < 0)
				{
					if (MADDriver->JumpToNextPattern && thePrefs.patternWrapping == false)
					{
						short newPL = MADDriver->PL;
						newPL--;
						if (newPL < 0 ) newPL++;
						else
						{
							theCell.v += myList.maxY;
							
							MADDriver->PL = newPL;
							MADDriver->Pat = curMusic->header->oPointers[ MADDriver->PL];
						}
					}
					else
					{
						theCell.v += myList.maxY;
						
						MADDriver->Pat = CurrentPat;
					}
				}
				if (theCell.v >= myList.maxY)
				{
					if (MADDriver->JumpToNextPattern && thePrefs.patternWrapping == false)
					{
						short newPL = MADDriver->PL;
						newPL++;
						if (newPL >= curMusic->header->numPointers) newPL--;
						else
						{
							theCell.v -= myList.maxY;
							
							MADDriver->PL = newPL;
							MADDriver->Pat = curMusic->header->oPointers[ MADDriver->PL];
						}
					}
					else
					{
						theCell.v -= myList.maxY;
						
						MADDriver->Pat = CurrentPat;
					}
				}
				if (theCell.h < 0) theCell.h = myList.maxX-1;
				if (theCell.h >= myList.maxX) theCell.h = 0;
				
				PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			}
			
			theCell.v = theCell.h = 0;
			if (PLGetSelect(&theCell, &myList))
			{
				if (thePrefs.MusicTrace) MADDriver->PartitionReader = theCell.v;
				SetCommandTrack(theCell.h, theCell.v);
			}
			
			DoGlobalNull();
			
			//	WaitNextEvent(everyEvent, &theEvent, 1, NULL);	// ne pas le mettre... a cause du PLSetSelect! et du update
			
			PLAutoScroll(&myList);
	 	}
 	}
 	else if (theChar == '/' || theChar == '*')
 	{
 		short	X, Y;
		Rect	tRect;
		
		curMusic->hasChanged = true;
		
 		SaveUndo(UPattern, CurrentPat, "\pUndo 'Change Note in Digital Editor'");
 		
		for (X = myList.select.left; X <= myList.select.right; X++)
		{
			for (Y = myList.select.top; Y <= myList.select.bottom; Y++)
			{
				theCommand = GetMADCommand(Y, X, curMusic->partition[ CurrentPat]);
				
				if (theCommand->note != 0xFF)
				{
					if ((theEvent.modifiers & shiftKey) != 0)
					{
						if (curInstru == theCommand->ins)
						{
							if (theChar == '/') if (theCommand->note > 0 && theCommand->note <= NUMBER_NOTES-1) theCommand->note--;
							if (theChar == '*') if (theCommand->note < NUMBER_NOTES-1) theCommand->note++;
						}
					}
					else
					{
						if (theChar == '/') if (theCommand->note > 0 && theCommand->note <= NUMBER_NOTES-1) theCommand->note--; 
						if (theChar == '*') if (theCommand->note < NUMBER_NOTES-1) theCommand->note++;
					}
				}
				
				theCell.v = Y;
				theCell.h = X;
				
				UPDATE_NoteBOUCLE(theCell.v, theCell.h);
			}
		}
		
		PLGetSelectRect(&tRect, &myList);
		if (SectRect(&tRect, &myList.rect, &tRect)) InvalWindowRect(GetDialogWindow(EditorDlog), &tRect);
		
		UPDATE_NoteFINISH();
 	}
 	else if (theChar == deletekey) // Delete
 	{
		short	X, Y;
		Rect	tRect;
		
		curMusic->hasChanged = true;
		
 		SaveUndo(UPattern, CurrentPat, "\pUndo 'Delete Digital Editor'");
 		
		for (X = myList.select.left; X <= myList.select.right; X++)
		{
			for (Y = myList.select.top; Y <= myList.select.bottom; Y++)
			{
				theCommand = GetMADCommand(Y, X, curMusic->partition[ CurrentPat]);
				theCommand->ins		= 0;			theCommand->note	= 0xFF;
				theCommand->cmd		= 0;			theCommand->arg		= 0;
				theCommand->vol		= 0xFF;
				
				theCell.v = Y;
				theCell.h = X;
				
				UPDATE_NoteBOUCLE(theCell.v, theCell.h);
			}
		}
		
		PLGetSelectRect(&tRect, &myList);
		if (SectRect(&tRect, &myList.rect, &tRect)) InvalWindowRect(GetDialogWindow(EditorDlog), &tRect);
		
		UPDATE_NoteFINISH();
 	}
 	else if (theChar == 0x09)
 	{
		//	DoKeyPressEditor(0x1F);
		//	theChar = 0;
 	}
 	else if (theChar == selectAll)
 	{
 		PLSetSelect(0, 0, curMusic->header->numChn, curMusic->partition[ CurrentPat]->header.size, &myList);
 	}
 	else if (theChar == getinfo)
 	{
 		DialogPatternInfo(CurrentPat);
 	}
	
	theCell.v = theCell.h = 0;
	if (PLGetSelect(&theCell, &myList))
	{
		HiliteControl(OpenBut, 0);
		HiliteControl(SaveBut, 0);
		HiliteControl(ShowBut, 0);
		HiliteControl(PlayBut, 0);
		HiliteControl(UpBut, 0);
		HiliteControl(DownBut, 0);
		HiliteControl(FXBut, 0);
	}
	else
	{
		HiliteControl(OpenBut, 255);
		HiliteControl(SaveBut, 255);
		HiliteControl(ShowBut, 255);
		HiliteControl(PlayBut, 255);
		HiliteControl(DownBut, 255);
		HiliteControl(UpBut, 255);
		HiliteControl(FXBut, 255);
	}
	
	SetPort(SavePort);
}

Ptr ConvertPcmd2Text(Pcmd *myPcmd)
{
	short			i, x;
	Str255			myStr;
	Ptr				myText;
	long			mSize;
	
	mSize = (long) 5 + myPcmd->tracks * (long) myPcmd->length * 16L;
	myText = NewPtrClear(mSize);
	if (myText == NULL) MyDebugStr(__LINE__, __FILE__, "Memory Error");
	strcpy(myText, "");
	
	for (i = 0; i < myPcmd->length; i++)
	{
		for (x = 0; x < myPcmd->tracks; x++)
		{
			Cmd		*myCmd = GetCmd(i, x, myPcmd);
			
			if (CreateNoteString(myCmd, myStr, true))
			{
				MyP2CStr(myStr);
				strcat(myText, (Ptr) myStr);
			}
			else strcat(myText, "              ");
			
			if (x < myPcmd->tracks - 1) strcat(myText, "\t");
			else strcat(myText, "\r");
		}
	}
	
	if (strlen(myText) >= mSize) MyDebugStr(__LINE__, __FILE__, "ZZZ");
	
	return myText;
}

Pcmd* CreatePcmdFromSelection(void)
{
	Point				theCell;
	Cmd					*cmd, *cmd2;
	Pcmd				*myPcmd;
	short				X, Y, count;
	
	theCell.h = theCell.v = 0;
	if (!PLGetSelect(&theCell, &myList)) return NULL;
	
	count = (myList.select.bottom - myList.select.top + 1) * (myList.select.right - myList.select.left + 1);
	if (count <= 0) return NULL;
	
	myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + count * sizeof(Cmd));
	if (myPcmd == NULL) {
		MyDebugStr(__LINE__, __FILE__, "Memory WARNING");
		return NULL;
	}
	
	myPcmd->structSize 	= sizeof(Pcmd) + count * sizeof(Cmd);
	myPcmd->tracks 		= myList.select.right - myList.select.left + 1;
	myPcmd->length 		= myList.select.bottom - myList.select.top + 1;
	myPcmd->trackStart 	= myList.select.left;
	myPcmd->posStart 	= myList.select.top;
	
	for (X = myList.select.left; X <= myList.select.right; X++)
	{
		for (Y = myList.select.top; Y <= myList.select.bottom; Y++)
		{
			cmd = GetMADCommand(Y, X, curMusic->partition[ CurrentPat]);
			cmd2 = GetCmd(Y - myList.select.top, X - myList.select.left, myPcmd);
			
			*cmd2 = *cmd;
		}
	}
	return myPcmd;
}

void ApplyOnAllCell(short ins, short note, short ef, short argu, short vol)
{
	Point			theCell;
	Cmd			*theCommand;
	short		X, Y;
	
	if (EditorDlog == NULL) return;
	
	theCell.h = theCell.v = 0;
	
	for (X = myList.select.left; X <= myList.select.right; X++)
	{
		for (Y = myList.select.top; Y <= myList.select.bottom; Y++)
		{
			theCell.v = Y;		theCell.h = X;
			
			theCommand = GetMADCommand(theCell.v, theCell.h, curMusic->partition[ CurrentPat]);
			
			if (ins != -1)	theCommand->ins		= ins;
			if (note != -1)	theCommand->note		= note;
			if (ef != -1)	theCommand->cmd		= ef;
			if (argu != -1)	theCommand->arg		= argu;
			if (vol != -1)	theCommand->vol		= vol;
			
			UPDATE_NoteBOUCLE(theCell.v, theCell.h);
		}
	}
	
	UPDATE_NoteFINISH();
}

void PasteCmdEditor(Point theCell, Pcmd *myPcmd)
{
	short			itemType,i,StartH, x;
	long			inOutBytes, iL, scrapOffset, lCntOrErr;
	Handle			theHandle;
	Point			aPt;
	Cmd				*cmd, *cmd2;
	Rect			sRect, eRect;
	GrafPtr			SavePort;
	
	SwapPcmd(myPcmd);
	
	GetPort(&SavePort);
	SetPortDialogPort(EditorDlog);
	
	/*** Deselect all cells ***/
	
	PLSetSelect(-1, -1, -1, -1, &myList);
	
	/**************************/
	
	PLSetSelect(theCell.h, theCell.v, theCell.h + myPcmd->tracks - 1, theCell.v + myPcmd->length - 1, &myList);
	
	StartH = theCell.h;
	for (i = 0; i < myPcmd->length; i++)
	{
		theCell.h = StartH;
		
		for (x = 0; x < myPcmd->tracks; x++)
		{
			if (theCell.v >= 0 &&
			   theCell.v < curMusic->partition[ CurrentPat]->header.size &&
			   theCell.h >= 0 &&
			   theCell.h < curMusic->header->numChn)
			{
				cmd = GetMADCommand(theCell.v, theCell.h, curMusic->partition[ CurrentPat]);
				cmd2 = GetCmd(i, x, myPcmd);
				
				*cmd = *cmd2;
				
				UPDATE_NoteBOUCLE(theCell.v, theCell.h);
			}
			
			theCell.h++;
		}
		
		theCell.v++;
		if (theCell.v >= curMusic->partition[ CurrentPat]->header.size) i = myPcmd->length;
	}
	
	UPDATE_NoteFINISH();
	
	/*** Update de la région ***/
	
	PLGetSelectRect(&eRect, &myList);
	InvalWindowRect(GetDialogWindow(EditorDlog), &eRect);
	UpdatePartitionWindow(EditorDlog);
	
	SetPort(SavePort);
}

void COPYEditor(void)
{
	Pcmd				*myPcmd;
	Ptr					myText;
	GrafPtr				SavePort;
	ScrapRef				scrap;
	OSErr				anErr;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
	
	myPcmd = CreatePcmdFromSelection();
	if (myPcmd == NULL)
	{
		MyDebugStr(__LINE__, __FILE__, "CopyEditor Internal ERROR");
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

void PASTEEditor(void)
{
	short			itemType,i,StartH, x;
	long				inOutBytes, iL, scrapOffset, lCntOrErr;
	Handle			theHandle;
	Point				theCell, aPt;
	Cmd				*theCommand;
	Rect				sRect, eRect;
	GrafPtr			SavePort;
	Pcmd			*myPcmd;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	OSErr			anErr;
	
 	GetPort(&SavePort);
 	SetPortDialogPort(EditorDlog);
	
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
		HUnlock(theHandle);
		
		HLock(theHandle);
		
		myPcmd = (Pcmd*) (*theHandle);
		
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Paste Digital Editor'");
		
		theCell.h = theCell.v = 0;
		if (PLGetSelect(&theCell, &myList)) PasteCmdEditor(theCell, myPcmd);
		
		HUnlock(theHandle);
		MyDisposHandle(& theHandle);
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
	
	SetPort(SavePort);
}

static	short			PcmdTracks, PcmdLength;
static	ControlHandle	selectedControl;
static	Rect			dragRect;

void EraseHilite(Rect *newRect)
{
	Rect 	aR;
	short	x, xPos, aa;
	
	if (thePrefs.LinesHeight == 0) aa = 2;
	else aa = 1;
	
	SetRect(&aR, dragRect.left, dragRect.top, dragRect.left + aa, dragRect.bottom);		InvalWindowRect(GetDialogWindow(EditorDlog), &aR);
	SetRect(&aR, dragRect.right - aa, dragRect.top, dragRect.right, dragRect.bottom);		InvalWindowRect(GetDialogWindow(EditorDlog), &aR);
	SetRect(&aR, dragRect.left, dragRect.top, dragRect.right, dragRect.top + aa);			InvalWindowRect(GetDialogWindow(EditorDlog), &aR);
	SetRect(&aR, dragRect.left, dragRect.bottom-aa, dragRect.right, dragRect.bottom);		InvalWindowRect(GetDialogWindow(EditorDlog), &aR);

	SetRect(&aR, newRect->left, newRect->top, newRect->left + aa, newRect->bottom);		ValidWindowRect(GetDialogWindow(EditorDlog),  &aR);
	SetRect(&aR, newRect->right - aa, newRect->top, newRect->right, newRect->bottom);		ValidWindowRect(GetDialogWindow(EditorDlog),  &aR);
	SetRect(&aR, newRect->left, newRect->top, newRect->right, newRect->top + aa);			ValidWindowRect(GetDialogWindow(EditorDlog),  &aR);
	SetRect(&aR, newRect->left, newRect->bottom-aa, newRect->right, newRect->bottom);		ValidWindowRect(GetDialogWindow(EditorDlog),  &aR);
	
	BeginUpdate(GetDialogWindow(EditorDlog));
	
	if (thePrefs.UseMarkers) DrawMarkers(GetControlValue(myList.yScroll), PLGetMaxYValue(&myList));
	else EraseRect(&myList.rect);
	
	xPos = myList.rect.left + 4;
	for (x = GetControlValue(myList.xScroll); x < PLGetMaxXValue(&myList); x++)
	{
		DrawGrayLines(xPos);
		
		MoveTo(xPos - 4, myList.rect.top);		LineTo(xPos - 4, myList.rect.bottom);
		
		xPos += myList.LCell;
	}
	
	PLGetSelectRect(&aR, &myList);
	aR.left++;
	
	DrawSelectionZone(&aR);
	
	RGBForeColor(&theColor);
	PenMode(adMin);
	PaintRect(&CurRect);
	PenMode(srcCopy);
	ForeColor(blackColor);
	
	EndUpdate(GetDialogWindow(EditorDlog));
}

pascal OSErr MyTrackingEditor(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	short				result, offset, i, fRefNum;
	long				textSize, inOutBytes;
	unsigned short		index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;
	FlavorFlags     	theFlags;
	Pcmd				*myPcmd;
	RgnHandle			saveClip;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	OSErr				iErr;
	Str255				pStr;
	
	if (!mainSystemDrag) return noErr;
	
	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return(noErr);
	
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message)
	{
		case kDragTrackingEnterHandler:
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
			
			result = GetFlavorFlags(theDrag, theItem, 'Pcmd', &theFlags);
			if (result == noErr)
			{
				canAcceptDrag = true;
				
				result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
				if (result == noErr)
				{
					/****************/
					myPcmd = (Pcmd*) MyNewPtr(textSize);
					GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
					PcmdTracks = myPcmd->tracks;
					if (PcmdTracks <= 0) PcmdTracks = 1;
					PcmdLength = myPcmd->length;
					if (PcmdLength <= 0) PcmdLength = 1;
					MyDisposePtr((Ptr*) &myPcmd);
					/****************/
				}
				else
				{
					PcmdTracks = 1;
					PcmdLength = 1;
				}
			}
			else if (GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags) == noErr)
			{
				Boolean	targetIsFolder, wasAliased;
				
				GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
				GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
				
				ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
				
				//	HSetVol(NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
				FSpGetFInfo(&myFlavor.fileSpec, &fndrInfo);
				
				switch (fndrInfo.fdType)
				{
					case 'Pcmd':
						/****************/
						PcmdTracks = 1;	PcmdLength = 1;
						iErr = FSpOpenDF(&myFlavor.fileSpec, fsCurPerm, &fRefNum);
						if (iErr == noErr)
						{
							GetEOF(fRefNum, &inOutBytes);
							myPcmd = (Pcmd*) MyNewPtr(inOutBytes);
							FSRead(fRefNum, &inOutBytes, (Ptr) myPcmd);
							PcmdTracks = myPcmd->tracks;
							PcmdLength = myPcmd->length;
							FSCloseFork(fRefNum);
							MyDisposePtr((Ptr*) &myPcmd);
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
			
			selectedControl = NULL;
			
			if (attributes & kDragInsideSenderWindow)
			{
				Rect	contrlRect;
				
				GetControlBounds(SaveBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl(SaveBut, kControlButtonPart);		selectedControl = SaveBut;}
				else HiliteControl(SaveBut, 0);
				
				/*	if (PtInRect(localMouse, &(*PoubBut)->contrlRect) && selectedControl == NULL)	{ HiliteControl(PoubBut, kControlButtonPart);		selectedControl = PoubBut;}
				 else HiliteControl(PoubBut, 0);*/
			}
			
			CellSelec.v = -1;	CellSelec.h = -1;
			
	 		saveClip = NewRgn();
	 		GetClip(saveClip);
	 		ClipRect(&myList.rect);
			
			if (PtInRect(localMouse, &myList.rect))
			{
				CellSelec = localMouse;
				
				PLConvertPoint(&CellSelec, &myList);
				CellSelec.v += DiffStartV;
				CellSelec.h += DiffStartH;
				
				tempRect.left = myList.rect.left + 1 + (CellSelec.h - GetControlValue(myList.xScroll)) * myList.LCell;
				tempRect.top = myList.rect.top + (CellSelec.v - GetControlValue(myList.yScroll)) * myList.HCell;						
				tempRect.right = tempRect.left - 1 + PcmdTracks * myList.LCell;
				tempRect.bottom = tempRect.top + PcmdLength * myList.HCell;
				
				if (tempRect.left != dragRect.left || tempRect.top != dragRect.top)
				{
					EraseHilite(&tempRect);
					
					dragRect = tempRect;
					
					ForeColor(redColor);
					
					if (thePrefs.LinesHeight == 0)	PenSize(2, 2);
					FrameRect(&dragRect);
					PenSize(1, 1);
					ForeColor(blackColor);
				}
			}
			else
			{
				if (dragRect.left != 0 &&
				   dragRect.right != 0 &&
				   dragRect.top != 0 &&
				   dragRect.bottom != 0)
				{
					Rect	ta = { 0, 0, 0, 0};
					
					EraseHilite(&ta);
					SetRect(&dragRect, 0, 0, 0, 0);
				}
			}
			
			SetClip(saveClip);
			DisposeRgn(saveClip);
			break;
			
		case kDragTrackingLeaveWindow:
			if (dragRect.left != 0 &&
			   dragRect.right != 0 &&
			   dragRect.top != 0 &&
			   dragRect.bottom != 0)
			{
				Rect	ta = { 0, 0, 0, 0};
				
				saveClip = NewRgn();
	 			GetClip(saveClip);
	 			ClipRect(&myList.rect);
	 			
				EraseHilite(&ta);
				SetRect(&dragRect, 0, 0, 0, 0);
				
				SetClip(saveClip);
				DisposeRgn(saveClip);
			}
			break;
			
		case kDragTrackingLeaveHandler:
			//	InvalWindowRect(GetDialogWindow(EditorDlog), &myList.rect);
			break;
	}
	
	return(noErr);
}

pascal OSErr MySendDataProcEditor(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag)
{
	AEDesc			dropLoc;
	FSSpec			target;
	OSErr			err;
	Boolean			wasChanged;
	OSType			compressionType;
	short			SoundType, i, fRefNum, beginNumber;
	long			SoundSize;
	Str255			sName, str, tempStr, tStr;
	WDPBRec			wdpb;
	Size			textSize;
	Pcmd			*myPcmd;
	
	if (theFlavor == 'VCT1' || theFlavor == 'VCT5')
	{
		err = GetDropLocation(theDrag, &dropLoc);
		if (err) return (err);
		
		err = ResolveAlias(nil, (AliasHandle) dropLoc.dataHandle, &target, &wasChanged);
		if (err) return (err);
		
		AEDisposeDesc(&dropLoc);
		
		/**/
		PathNameFromDirIDTRUE(		target.parID,
							  target.vRefNum,
							  sName);
		
		pStrcat(sName, "\p:");
		pStrcat(sName, target.name);
		pStrcat(sName, "\p:");
		
		wdpb.ioNamePtr = sName;
		PBHSetVolSync(&wdpb);
		HGetVol(NULL, &target.vRefNum, &target.parID);
		/**/
		
		
		err = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
		
		if (err == noErr)
		{	
			myPcmd = (Pcmd*) MyNewPtr(textSize);
			if (myPcmd != NULL)
			{
				GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
				
				//	pStrcpy(str, "\pMy Pcmd (");
				pStrcpy(str, curMusic->musicFileName);
				
				for (i = str[ 0]; i > 0; i--)
				{
					if (str[ i] == '.')
					{
						str[ 0] = i-1;
						i = 0;
					}
				}
				
				if (CFSwapInt32BigToHost(theFlavor) == 'VCT5') pStrcat(str, "\p.aiff");
				else
				{
					if (str[ 0] < 20)
					{
						pStrcat(str, "\p.pcmd ");
						NumToString(myPcmd->tracks, tStr);
						pStrcat(str, tStr);
						pStrcat(str, "\px");
						NumToString(myPcmd->length, tStr);
						pStrcat(str, tStr);
					}
				}
			}
			else return(dragNotAcceptedErr);
		}
		else return(dragNotAcceptedErr);
		
		beginNumber = 1;
		pStrcpy(sName, str);
		
		/**/
		
	ReGiveName:
		
		FSMakeFSSpec(target.vRefNum, target.parID, sName, &target);
		err = FSpOpenDF(&target, fsCurPerm, &fRefNum);
		if (err != fnfErr)
		{
			FSCloseFork(fRefNum);
			
			beginNumber++;
			NumToString(beginNumber, tempStr);
			pStrcpy(sName, str);
			if (sName[ 0] > 27) sName[ 0] = 27;
			pStrcat(sName, "\p #");
			pStrcat(sName, tempStr);
			
			goto ReGiveName;
		}
		else
		{
			if (theFlavor == 'VCT5') {
				err = FSpCreate(&target, 'TVOD', 'AIFF', smSystemScript);
				err = FSpOpenDF(&target, fsCurPerm, &fRefNum);
				
				if (CreateAIFFExporting(true, fRefNum, &target, 'AIFF', NULL))
				{
					while (theProgressDia != NULL) DoAIFFExporting();
				}
			} else {
				FSpCreate(&target, 'SNPL', 'Pcmd', smSystemScript);
				FSpOpenDF(&target, fsCurPerm, &fRefNum);
				FSWriteFork(fRefNum, fsAtMark, 0, textSize, myPcmd, NULL);
				FSCloseFork(fRefNum);
			}
			
			MyDisposePtr((Ptr*)&myPcmd);
			
			err = SetDragItemFlavorData(theDrag, theItem, theFlavor, &target, sizeof(target), 0);
			if (err)
				return err;
		}
	}
	
	return noErr;
}

Boolean DragEditor(RgnHandle myRgn, Pcmd *myPcmd, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	Ptr					myText;
	PromiseHFSFlavor	myNewFile;
	Rect				dragRegionRect;
	
	if (!DragManagerUse) return false;
	
	dragRegion = NewRgn();
	
	CopyRgn(myRgn, dragRegion);
	SetPt(&theLoc, 0, 0);
	LocalToGlobal(&theLoc);
	OffsetRgn(dragRegion, theLoc.h, theLoc.v);
	
	NewDrag(&theDrag);
	
	AddDragItemFlavor(theDrag, 1, 'Pcmd', myPcmd, myPcmd->structSize, 0);
	
	if (thePrefs.editorSoundDrag)
	{
		myNewFile.fileType			=	'AIFF';
		myNewFile.fileCreator		=	'TVOD';
		myNewFile.fdFlags			=	0;
		myNewFile.promisedFlavor	=	'VCT5';
		AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
		AddDragItemFlavor(theDrag, 1, 'VCT5', NULL, 0, 0);
	}
	else
	{
		myNewFile.fileType			=	'Pcmd';
		myNewFile.fileCreator		=	'SNPL';
		myNewFile.fdFlags			=	0;
		myNewFile.promisedFlavor	=	'VCT1';
		AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
		AddDragItemFlavor(theDrag, 1, 'VCT1', NULL, 0, 0);
	}
	
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
	
	UpdatePartitionWindow(EditorDlog);
	
	result = TrackDrag(theDrag, theEvent, dragRegion);
	
	if (result != noErr && result != userCanceledErr) return(true);
	
	DisposeDrag(theDrag);
	DisposeRgn(dragRegion);
	
	return(true);
}

pascal OSErr MyReceiveDropEditor(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	OSErr				result;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				mouseDownModifiers, mouseUpModifiers, movePcmd;
	Pcmd				*myPcmd;
	HFSFlavor			myFlavor;
	
	if (!mainSystemDrag) return dragNotAcceptedErr;
	if (!canAcceptDrag) return(dragNotAcceptedErr);
	
	SetPortWindowPort(theWindow);
	
	DiffStartV = 0;		DiffStartH = 0;
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	if (attributes & kDragInsideSenderWindow)
	{
		if (selectedControl != NULL)
		{
			HiliteControl(selectedControl, 0);
			
			if (selectedControl == SaveBut)		SavePcmdFile(CreatePcmdFromSelection());
			//	if (selectedControl == PoubBut) 	DoKeyPressEditor(8);
			selectedControl = NULL;
			
			return noErr;
		}
	}
	
	if (CellSelec.v == -1) return(dragNotAcceptedErr);
	
	movePcmd = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	
	HideDragHilite(theDrag);
	
	curMusic->hasChanged = true;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
	
	if (result == noErr)
	{	
		myPcmd = (Pcmd*) MyNewPtr(textSize);
		if (myPcmd != NULL)
		{
			GetFlavorData(theDrag, theItem, 'Pcmd', myPcmd, &textSize, 0);
			
			if (movePcmd)		// Delete source
			{
				// La source est déjà selectionnée...
				
				DoKeyPressEditor(8);
			}
			else SaveUndo(UPattern, CurrentPat, "\pUndo 'Move Selection'");
			
			PasteCmdEditor(CellSelec, myPcmd);
			
			MyDisposePtr((Ptr*) &myPcmd);
			
			return(noErr);
		}
		else return(dragNotAcceptedErr);
	}
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
	if (result == noErr)
	{
		Boolean		targetIsFolder, wasAliased;
		
		SaveUndo(UPattern, CurrentPat, "\pUndo 'Drop Pcmd File'");
		
		//	PLSetSelect(-1, -1, -1, -1, &myList);
		
		PLSetSelect(CellSelec.h, CellSelec.v, CellSelec.h, CellSelec.v, &myList);
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		OpenPcmdFile(&myFlavor.fileSpec);
		
		return(noErr);
	}
	
	return(dragNotAcceptedErr);
}

void PrintEditor(void)
{
	Pcmd		*myPcmd;
	Ptr			myText;
	TEHandle	hTE;
	TextStyle	newStyle;
	Rect		tRect = { 0, 0, 20, 20};
	
	DoKeyPressEditor(selectAll);
	myPcmd = CreatePcmdFromSelection();
	myText = ConvertPcmd2Text(myPcmd);
	
	hTE = TEStyleNew(&tRect, &tRect);
	TEInsert(myText, GetPtrSize(myText), hTE);
	
	TESetAlignment(teCenter, hTE);
	TESetSelect(0, 32767, hTE);
	newStyle.tsFont = 4;
	newStyle.tsSize = 9;
	TESetStyle(doFont + doSize, &newStyle, false, hTE);
	
	PrintTEHandle(hTE);
	
	MyDisposePtr((Ptr*) &myPcmd);
	MyDisposePtr(& myText);
	TEDispose(hTE);
}
