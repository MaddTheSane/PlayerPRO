#include <Carbon/Carbon.h>
#include <string.h>
#include <QuickTime/QuickTime.h>
#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "PrivateList.h"
#include "PPPlug.h"

/******** HELP MODULE ********/
enum
{
	HRec	= 22,
	HSave	= 21,
	HLoad	= 20,
	HDel	= 19,
	HWind	= 24,
	HIndi	= 26,
	HList	= 1,
	HInfo	= 18,
	HOpenA	= 23
};

//static pascal Boolean myDragClickLoop(void);
static short AHelp[] = {HRec, HSave, HLoad, HDel, HWind, HIndi, HList, HInfo, HOpenA};

void DoHelpInstruList(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

	/*****************************/

#define POSINS	16
#define POSSAMP 45

extern	short					LastCanal, ToneGenerator, BlackWhite[ 12];
extern	EventRecord				theEvent;
extern	DialogPtr				SampleDlog[ 65], AHelpDlog, theProgressDia;
extern	Boolean					PianoRecording, QuicktimeInstruAvailable, DragManagerUse;
extern	RGBColor				theColor;
extern	MenuHandle				NewSoundMenu;
extern	Cursor					HandCrsr, beamCrsr, pencilCrsr, CHandCrsr;
extern	WindowPtr				oldWindow;
extern	Movie					QTMovie;
extern	Boolean					gMovieQuicktime;
extern	FSSpec					QTFile;

static	PrivateList				myList;

static 	Rect					ButtonRect;
static	ControlHandle			LoadBut, DelBut, SaveBut, RecBut, OpenBut, FlipBut, PlayBut;

static	short					DestIns, DestSamp, DragIns, DragSamp;
static	Boolean					canAcceptDrag;
static	PixMapHandle			mySABut, OP, CP, OPB, CPB, ArrowPix;
static	Boolean					*SAButState, *InstrUsed;

static	DragSendDataUPP			mySendDataUPP;
static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

		DialogPtr				InstruListDlog;

OSErr			CloseSampleData(DialogPtr	TheDia);
void			ShowSampleData(DialogPtr	TheDia);
Boolean 		DragInstrument(RgnHandle , short , EventRecord *);
pascal OSErr 	MyReceiveDropHandler(WindowPtr , void* , DragReference );
pascal OSErr 	MyTrackingHandler(short , WindowPtr , void *, DragReference );
void			NCreateSampleWindow(short ins, short samp);
void 			SetInstruMozart(short myInstru);
void			MAppelPlug(short , short, short whichPlug);
void			SetSelectionZero(short no);
void			NNumToString(short no, Str255 aStr);
void			ConvertIDtoInsSamp(short iD, short *ins, short *samp);
short			ComputeMaxY();
void			SetSampName(short	ins, short samp, Str255 theNewName);
void			NDoPlayInstru(short	Note, short Instru, short effect, short arg, short vol);
short			ConvertInsSampToID(short ins, short samp);
void			OctavesName(short	id, Ptr	String);
void			HandleNewSound(short theItem);
void			ConvertInstrumentMode(sData	*curData, short);
void			SetInstruEditor(short instru);
OSErr CloseInstruData(DialogPtr TheDia);

Ptr MyExp1to3(Ptr sound, unsigned long numSampleFrames)
{
	Ptr		inState, outState;
	Ptr		outBuffer;

	outState = NewPtrClear(128);
	inState = NewPtrClear(128);
	if (inState == nil) return NULL;
	
	outBuffer = NewPtr(numSampleFrames*6);
	if (outBuffer == nil) return NULL;
		
	DisposePtr(sound);
	sound = outBuffer;
	
	DisposePtr(inState);
	DisposePtr(outState);

	return sound;
}

Ptr MyExp1to6(Ptr sound, unsigned long numSampleFrames)
{
	Ptr inState, outState;
	Ptr outBuffer;
	
	outState = NewPtrClear(128);
	inState = NewPtrClear(128);
	if (inState == nil)
		return NULL;
	
	outBuffer = NewPtr(numSampleFrames * 6);
	if (outBuffer == nil)
		return NULL;
	
	DisposePtr(sound);
	sound = outBuffer;
	
	DisposePtr(inState);
	DisposePtr(outState);
	
	return sound;
}

Ptr NSndToPtr(Ptr soundPtr, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	short 			soundFormat, numChannels;
	short 			numSynths = 1, numCmds = 1, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	long			i;
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	*stereo = false;
	
	// determine what format sound we have.
	soundFormat = *(short*) soundPtr;
	
	switch (soundFormat) {
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths.
			numCmds = *(short*)(soundPtr+4+numSynths*6);		// get # commands.
			break;
			
		case 2:						// format 2 sound.
			numSynths = 0;			// format 2 sounds have no synth's.
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:					// jack says, what about 12? or 6?
			MyDebugStr(__LINE__, __FILE__, " NSndToHandle... Burkk");
			break;
	}
	
	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) (((Ptr) soundPtr) + offset);
	
	switch (header->encode) {
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;
			
			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if (numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if (sampleRate != NULL)
				*sampleRate	= CmpHeader->sampleRate;
			if (baseFreq != NULL)
				*baseFreq 	= CmpHeader->baseFrequency;
			
			MusSize = (*CmpHeader).numFrames;
			if (*stereo) {
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			BlockMoveData((*CmpHeader).sampleArea, soundPtr, MusSize);
			
			switch (CompressID) {
				case threeToOne:
					MusSize *= 2;
					soundPtr = MyExp1to3(soundPtr, MusSize);
					MusSize *= 3;
					break;
					
				case sixToOne:
					soundPtr = MyExp1to6(soundPtr, MusSize);
					MusSize *= 6;
					break;
					
				default:
					return NULL;
					break;
			}
			
			break;
			
		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			
			MusSize = ExtHeader->numFrames;
			numChannels = ExtHeader->numChannels;
			
			*loopStart = ExtHeader->loopStart;
			*loopEnd = ExtHeader->loopEnd;
			*sampleSize = ExtHeader->sampleSize;
			
			if (sampleRate != NULL) 	*sampleRate	= ExtHeader->sampleRate;
			if (baseFreq != NULL) 	*baseFreq 	= ExtHeader->baseFrequency;
			
			if (numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if (*stereo) {
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			
			if (*sampleSize == 16) {
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			if (numChannels == 1)
				BlockMoveData(ExtHeader->sampleArea, soundPtr, MusSize);
			else if (numChannels == 2) {
				BlockMoveData(ExtHeader->sampleArea, soundPtr, MusSize);
			} else {
				if (*sampleSize == 8) {
					for (i = 0; i < MusSize; i ++) {
						soundPtr[i] = ExtHeader->sampleArea[i * numChannels];
					}
				} else {
					MusSize /= 2;
					for (i = 0; i < MusSize; i ++) {
						((short*)soundPtr)[i] = ((short*)ExtHeader->sampleArea)[i * numChannels];
					}
					MusSize *= 2;
				}
			}
			break;
			
		default:
		case stdSH:
			*loopStart = header->loopStart;
			*loopEnd = header->loopEnd;
			
			if (sampleRate != NULL)
				*sampleRate	= header->sampleRate;
			if (baseFreq != NULL)
				*baseFreq 	= header->baseFrequency;
			
			MusSize = header->length;
			BlockMoveData((*header).sampleArea, soundPtr, MusSize);
			break;
	}
	SetPtrSize(soundPtr, MusSize);
	
	switch (*sampleSize) {
		case 8:
			ConvertInstrumentIn((Byte*) soundPtr, MusSize);
			break;
	}
	
	if (*loopEnd - *loopStart < 4) {
		*loopEnd = 0;
		*loopStart = 0;
	}
	
	return soundPtr;
}


OSErr AddSoundToMAD(Ptr				theSound,
					long			lS,
					long			lE,
					short			sS,
					short			bFreq,
					unsigned long	rate,
					Boolean			stereo,
					Str255			name,
					short			ins,
					short			*sampleID)
{
	long 	inOutBytes, i;
	sData	*curData;

	//if (*sampleID > MAXSAMPLE) return MADParametersErr;

	inOutBytes = GetPtrSize(theSound);
	
	///////
	
	if (*sampleID >= 0) {		// replace this sample
	
		curData = curMusic->sample[ curMusic->fid[ ins].firstSample + *sampleID];
	} else {					// add a sample : why? because *sampleID == -1
	
		*sampleID = curMusic->fid[ ins].numSamples;
		//if (InsHeader->numSamples >= MAXSAMPLE) return -1;
		
		curData = MADCreateSample(curMusic, ins, *sampleID);
	}
	
	if (curData->data != NULL)
		DisposePtr(curData->data);
	curData->data = theSound;
	
	curData->size		= inOutBytes;
	curData->loopBeg	= lS;
	curData->loopSize	= lE - lS;
	curData->vol		= 64;
	curData->amp		= sS;
	curData->c2spd		= rate >> 16L;
	curData->relNote	= 60 - bFreq;
	curData->stereo		= stereo;
	
	for(i=0; i<32; i++) {
		if (i < name[0])
			curData->name[i] = name[i + 1];
		else
			curData->name[i] = '\0';
	}
	
	return noErr;
}


void DrawBottomInfo()
{
	long	fileSize, sampleNum, insNum ,i, x;
	Rect	tempRect, caRect;
	Str255	aStr, tempStr;
	
	fileSize = 0;
	sampleNum = 0;
	insNum = 0;
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < curMusic->fid[i].numSamples ; x++) {
			sData *curData = curMusic->sample[curMusic->fid[i].firstSample + x];
			fileSize += curData->size;
		}
		
		sampleNum += curMusic->fid[ i].numSamples;
		
		if (curMusic->fid[i].numSamples > 0)
			insNum++;
	}
	
	GetPortBounds(GetDialogPort(InstruListDlog), &caRect);
	
	SetRect(&tempRect,
			myList.rect.left,
			myList.rect.bottom + 1,
			myList.rect.right - 15,
			caRect.bottom);
	
	
	
	EraseRect(&tempRect);
	
	MoveTo(7, myList.rect.bottom + 11);
	pStrcpy(tempStr, "\p");
	
	GetSizeString(fileSize, aStr, true);
	pStrcat(aStr, "\p, #ins: ");
	
	pStrcat(tempStr, aStr);
	
	NumToString(insNum, aStr);
	pStrcat(tempStr, aStr);
	
	NumToString(sampleNum, aStr);
	pStrcat(tempStr, "\p, #samples: ");
	pStrcat(tempStr, aStr);
	
	DrawString(tempStr);
	
	//if (QDIsPortBuffered(GetDialogPort(InstruListDlog)))
	//	QDFlushPortBuffer(GetDialogPort(InstruListDlog), NULL);
	
}

void ActiveInstrumentMenu(Boolean Activ)
{
	if (Activ) {
		EnableMenuItem(InstruEditMenu, 3);
		EnableMenuItem(InstruEditMenu, 4);
		EnableMenuItem(InstruEditMenu, 5);
		EnableMenuItem(InstruEditMenu, 6);
		EnableMenuItem(InstruEditMenu, 7);
		
		SetItemMark(InstruEditMenu, 1, checkMark);
	} else {
		DisableMenuItem(InstruEditMenu, 3);
		DisableMenuItem(InstruEditMenu, 4);
		DisableMenuItem(InstruEditMenu, 5);
		DisableMenuItem(InstruEditMenu, 6);
		DisableMenuItem(InstruEditMenu, 7);
		
		SetItemMark(InstruEditMenu, 1, noMark);
	}
}

void DoGrowInstruList(DialogPtr	theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp;
	short		tempB, tempA, avant;
	Point		aPt = { 0, 0};
	BitMap		screenBits;


	GetPort(&SavePort);
 	SetPortDialogPort(theDialog);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp.left = caRect.right;
	temp.top = myList.rect.top + 193;
	temp.right = caRect.right;
	
	temp.bottom = myList.rect.top + myList.maxY * myList.HCell;
	temp.bottom += 16;

	LocalToGlobal(&aPt);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	if (temp.bottom < temp.top) temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;

	if (theEvent.what == mouseDown) {
		lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);
	}
	
	if (lSizeVH != 0) {
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	} else {
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	avant = caRect.bottom - 15;
	
	MySizeWindow(theDialog, tempA, tempB, true);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp = myList.rect;
	temp.bottom = caRect.bottom;
	temp.right = caRect.right;
	EraseRect(&temp);
	InvalWindowRect(GetDialogWindow(theDialog), &temp);
	
	myList.rect.bottom = caRect.bottom - 15;
	
	PLSetControl(&myList);

	SetPort(SavePort);
}

void GetQualityString(short ID, Str255 str)
{
	switch (ID) {
		case 1:
			pStrcpy(str, "\pF#5");
			break;

		case 2:
			pStrcpy(str, "\pF#4");
			break;

		case 3:
			pStrcpy(str, "\pF#3");
			break;

		default:
			pStrcpy(str, "\p???");
			break;
	}

}

void AutoPosition(DialogPtr aDia)
{
	Point		Position, mouse;
	Rect		ViewRect, caRect;
	short		XSize, YSize;
	GDHandle	aH;
	BitMap		screenBits;
	
	GetMouse(&mouse);
	LocalToGlobal(&mouse);
	
	GetPortBounds(GetDialogPort(aDia), &caRect);
	
	XSize = (caRect.right - caRect.left);
	YSize = (caRect.bottom - caRect.top);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	SetRect(&ViewRect, screenBits.bounds.left + 8, screenBits.bounds.top + 43,
						screenBits.bounds.right - 8, screenBits.bounds.bottom - 8);
	
	aH = GetDeviceList();
	do {
		aH = GetNextDevice(aH);
		if (aH != NULL) {
			if (PtInRect(mouse, &(*(*aH)->gdPMap)->bounds)) {
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect(&ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	} while (aH != NULL);
	
	if (AHelpDlog != NULL)
		ViewRect.top += 17;

	Position.h = mouse.h - XSize/2;
	if (Position.h + XSize >= ViewRect.right)
		Position.h = ViewRect.right - XSize;
	else if (Position.h <= ViewRect.left)
		Position.h = ViewRect.left;

	Position.v = mouse.v - YSize/2;
	if (Position.v + YSize >= ViewRect.bottom)
		Position.v = ViewRect.bottom - YSize;
	else if (Position.v <= ViewRect.top)
		Position.v = ViewRect.top;

	SetDialogDefaultItem(aDia, 1 );
	SetDialogCancelItem(aDia, 2 );

	MoveWindow(GetDialogWindow(aDia), Position.h, Position.v, false);

	ShowWindow(GetDialogWindow(aDia));
}

static	Rect			SPianoRect[ NUMBER_NOTES];
extern 	short			dialogModifiers;
static	ControlHandle	sScroll;
static	DialogPtr		EditInfoDlog;

static	sData			*curEData;
static	short			sampleEID, insEID;


enum {
	normalKey = 1,
	redKey,
	greenKey
};

void DrawSmallPianoKey(short i, short color, Rect aRect)
{
	Rect	cRect = aRect;

	if (BlackWhite[ i % 12]) {		// Black Key
		static const RGBColor myRed = { 30583, 0, 0};
		
		InsetRect(&aRect, 1, 2);
		
		switch (color) {
			case normalKey:
				ForeColor(blackColor);
				break;
				
			case redKey:
				RGBForeColor(&myRed);
				break;
				
			case greenKey:
				ForeColor(yellowColor);
				break;
		}
		
		aRect.top--;
		PaintRect(&aRect);
	} else {						// White Key
		InsetRect(&aRect, 3, 1);
		
		switch (color) {
			case normalKey:
				ForeColor(whiteColor);
				break;
				
			case redKey:
				ForeColor(redColor);
				break;
				
			case greenKey:
				ForeColor(yellowColor);
				break;
		}
		
		PaintRect(&aRect);
		
		aRect = cRect;
		
		PenSize(2, 1);
		
		if (i > 0 && BlackWhite[(i - 1) % 12])
			MoveTo(aRect.left+1, aRect.bottom - 21);
		else
			MoveTo(aRect.left+1, aRect.top + 1);
		LineTo(aRect.left+1, aRect.bottom - 2);
		
		
		if (BlackWhite[(i + 1) % 12])
			MoveTo(aRect.right - 3, aRect.bottom - 21);
		else
			MoveTo(aRect.right - 3, aRect.top + 1);
		LineTo(aRect.right - 3, aRect.bottom - 2);
		
		PenSize(1, 1);
	}
	ForeColor(blackColor);
}

void NInitSmallPiano(Rect mainRect, Rect *listRect)
{
	Rect	itemRect;
	short	i, prevP;
	
#define WHITESIZE	9
#define BLACKSIZE	5
	
	itemRect = mainRect;	//GetDialogItem(TheDia, 29, &itemType, &itemHandle, &itemRect);
	itemRect.bottom -= 10;
	
	prevP = itemRect.left;
	
	for (i = 0; i < NUMBER_NOTES ; i++) {
		listRect[ i] = itemRect;
	
		if (BlackWhite[i % 12]) {
			listRect[i].left = prevP;
			listRect[i].right = listRect[i].left + BLACKSIZE;
			listRect[i].bottom -= 20;
			
			prevP += BLACKSIZE - 3;
		} else {
			listRect[ i].left = prevP;
			listRect[ i].right = listRect[ i].left + WHITESIZE;
			
			if (BlackWhite[ (i + 1) % 12])
				prevP += WHITESIZE - 3;
			else
				prevP += WHITESIZE - 1;
		}
	}
}

void DrawWhatSample(short ins, short samp)
{
	short 	i;
	
	for (i = 0; i < NUMBER_NOTES; i++) {
		if (curMusic->fid[ins].what[i] == samp) {
			DrawSmallPianoKey(i, redKey, SPianoRect[i]);
		}
	}
}

void NPressSmallPiano(short itemHit, DialogPtr TheDia, short theInstru)
{
	Point		Mouse;
	Handle		itemHandle;
	short		Position, itemType, i;
	Str255		aStr;
	Rect		itemRect;
	long		curSPos = -1;
	Boolean		firstTime = true;
	
	if (curMusic->fid[ theInstru].numSamples == 0) return;
	
	if (CloseSampleData(TheDia) != noErr)
		return;
	if (CloseInstruData(TheDia) != noErr)
		return;

	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();

	do {
		WaitNextEvent(everyEvent, &theEvent, 1, NULL);
		
		//if (QDIsPortBuffered(GetDialogPort(TheDia)))
    		QDFlushPortBuffer(GetDialogPort(TheDia), NULL);
    				
		DoGlobalNull();
		WaitNextEvent(everyEvent, &theEvent, 1, NULL);
		
		GetMouse(&Mouse);
	
		Position = -1;
		for (i = 0; i < NUMBER_NOTES; i++) {
			if (PtInRect(Mouse, &SPianoRect[i])) {
				Position = i;
				if (SPianoRect[ i].right - SPianoRect[i].left == 5)
					goto OK;
			}
		}
		
		if (Position >= 0) {
			OK:
			if (Position != curSPos) {
				if (curSPos != -1) {
					if (curMusic->fid[ theInstru].what[curSPos] == sampleEID)
						DrawSmallPianoKey(curSPos, redKey, SPianoRect[curSPos]);
					else
						DrawSmallPianoKey(curSPos, normalKey, SPianoRect[curSPos]);
				}
				DrawSmallPianoKey(Position, greenKey, SPianoRect[Position]);
				curSPos = Position;
				
				if (dialogModifiers & optionKey) {	// Key assignement
				
					curMusic->fid[ theInstru].what[ curSPos] = sampleEID;
				} else {
					MADDriver->chan[ LastCanal].KeyOn = false;
					NDoPlayInstru(curSPos, theInstru, 0, 0, 0xFF);
				}
				GetNoteString(curSPos, aStr);
				GetDialogItem(EditInfoDlog, 14, &itemType, &itemHandle, &itemRect);
				TextFace(bold);
				TETextBox(aStr+1, aStr[ 0], &itemRect, teJustRight);
				TextFace(0);
				
				firstTime = false;
			} else {
				if (firstTime) {
					firstTime = false;
					NDoPlayInstru(curSPos, theInstru, 0, 0, 0xFF);
				}
			}
		}
	} while (Button());
	
	MADDriver->chan[LastCanal].KeyOn = false;
	
	if (curSPos != -1) {
		if (curMusic->fid[theInstru].what[curSPos] == sampleEID)
			DrawSmallPianoKey(curSPos, redKey, SPianoRect[curSPos]);
		else
			DrawSmallPianoKey(curSPos, normalKey, SPianoRect[curSPos]);
	}
	GetDialogItem(EditInfoDlog, 14, &itemType, &itemHandle, &itemRect);
	EraseRect(&itemRect);
}

#define LEFTBOX 5

void UpdateEditInstruWindow(DialogPtr theDia)
{
	GrafPtr	savePort;
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle;
	Str255	aStr, bStr;

	GetPort(&savePort);
	SetPortDialogPort(theDia);
	
	BeginUpdate(GetDialogWindow(theDia));
	
	DrawDialog(theDia);
	oldFrameButton(theDia);
	
	GetDialogItem(theDia, 44, &itemType, &itemHandle, &itemRect);
	itemRect.left = LEFTBOX;
	if (curMusic->fid[ insEID].numSamples == 0)
	{
		pStrcpy(aStr, "\pNo samples in this instrument");
		
		itemRect.right -= 16;
		TETextBox(aStr + 1, aStr[ 0], &itemRect, teJustCenter);
		itemRect.right += 16;
	}
	
	FrameRect(&itemRect);
	
	if (curMusic->fid[ insEID].numSamples != 0)
	{
		DrawWhatSample(insEID, sampleEID);
	}
	
	GetDialogItem(theDia, 25, &itemType, &itemHandle, &itemRect);
	pStrcpy(aStr, "\pInstrument Info - ");
	NumToString(insEID+1, bStr);
	pStrcat(aStr, bStr);
	TextFace(bold);
	TETextBox(aStr+1, aStr[ 0], &itemRect, teJustCenter);
	TextFace(0);
	
	GetDialogItem(theDia, 13, &itemType, &itemHandle, &itemRect);
	pStrcpy(aStr, "\pSample Info (");
	NumToString(curMusic->fid[ insEID].numSamples, bStr);
	pStrcat(aStr, bStr);
	if (curMusic->fid[ insEID].numSamples > 1) pStrcat(aStr, "\p samples)");
	else pStrcat(aStr, "\p sample)");
	TextFace(bold);
	TETextBox(aStr+1, aStr[ 0], &itemRect, teJustCenter);
	TextFace(0);
		
	EndUpdate(GetDialogWindow(theDia));
	SetPort(savePort);
}

void ChangeDialogFont(DialogPtr aDia)
{
	short					i, num, iType;
	Rect					iRect;
	Handle					iHandle;
	ControlFontStyleRec		inStyle;
	ControlHandle 			ctl;
	long					actualSize;
	
	TextFont(kFontIDGeneva);
	TextSize(9);
	
	num = CountDITL(aDia);
	
	for (i = 1; i <= num; i++)
	{
		GetDialogItem(aDia, i, &iType, &iHandle, &iRect);
		GetDialogItemAsControl(aDia, i, &ctl);
		
		if (iType >= 128) iType -= 128;
		
		if (iType == 5 || iType == 6 || iType == 8)
		{
			GetControlData(ctl, kControlNoPart, kControlFontStyleTag, sizeof(inStyle), (Ptr) &inStyle, &actualSize);
			
			if (inStyle.flags == 0)
			{
				inStyle.flags = kControlUseFontMask + kControlUseSizeMask;
				inStyle.font = kFontIDGeneva;
				inStyle.size = 9;
			
				SetControlFontStyle(ctl, &inStyle);
			}
		}
		
		if (iType == 16)
		{
			GetControlData(ctl, kControlNoPart, kControlFontStyleTag, sizeof(inStyle), (Ptr) &inStyle, &actualSize);
			
			if (inStyle.flags == 0)
			{
				inStyle.flags = kControlUseFontMask + kControlUseSizeMask;
				inStyle.font = kFontIDGeneva;
				inStyle.size = 9;
			
				SetControlFontStyle(ctl, &inStyle);
			}
		}
	}
}

pascal void actionProcInstru(ControlHandle theControl, short ctlPart)
{
	short			maxValue, minValue, curVal, sVal;
	
	if (ctlPart <= 0)
		return;
	
	maxValue = GetControlMaximum(theControl);
	minValue = GetControlMinimum(theControl);
	curVal = sVal = GetControlValue(theControl);
	
	switch (ctlPart) {
		case kControlUpButtonPart:
		case kControlPageUpPart:
			curVal -= 1;
			if (curVal < minValue) curVal = minValue;
			break;
			
		case kControlDownButtonPart:
		case kControlPageDownPart:
			curVal += 1;
			if (curVal > maxValue) curVal = maxValue;
			break;
			
		case kControlIndicatorPart:
			sVal = -1;
			break;
	}
	
	if (sVal != curVal) {
		if (CloseSampleData(EditInfoDlog) == noErr) {
			short 	itemType;
			Handle	itemHandle;
			Rect	itemRect;
			
			CreateInstruList();
			DrawInfoInstrument();
			UpdateSampleWindows();
			UpdateInstruMenu();
			
			SetControlValue(theControl, curVal);
			
#if 0
#define STEP 20
			if (ctlPart != kControlIndicatorPart)
			{
				for (i = 0; i < STEP; i++)
				{
					GetDialogItem(EditInfoDlog, 44, &itemType, &itemHandle, &itemRect);
					itemRect.left = LEFTBOX + 1;
					itemRect.right -= 16;
					itemRect.top++;		itemRect.bottom--;
					
					if (curVal < sVal) ScrollRect(&itemRect, 0, (itemRect.bottom - itemRect.top) / STEP, NULL);
					else ScrollRect(&itemRect, 0, - (itemRect.bottom - itemRect.top) / STEP, NULL);
					
					Delay(1, &tL);
				}
			}
#endif
			
			EraseRect(&itemRect);
			InvalWindowRect(GetDialogWindow(EditInfoDlog), &itemRect);
			
			GetDialogItem(EditInfoDlog, 29, &itemType, &itemHandle, &itemRect);
			InvalWindowRect(GetDialogWindow(EditInfoDlog), &itemRect);
			
			sampleEID = curVal;
			curEData = curMusic->sample[ curMusic->fid[ insEID].firstSample + sampleEID];
			ShowSampleData(EditInfoDlog);
			
			SelectDialogItemText(EditInfoDlog, 45, 0, 32767);
		}
	}
}

void ShowSampleData(DialogPtr TheDia)
{
	Str255	theStr, aStr;
	
	if (curEData != NULL) {
		NumToString(sampleEID+1, theStr);
		SetDText(TheDia, 30, theStr);
		NumToString(curEData->loopBeg, theStr);
		SetDText(TheDia, 4, theStr);
		NumToString(curEData->loopSize, theStr);
		SetDText(TheDia, 5, theStr);
		NumToString(curEData->relNote, theStr);
		SetDText(TheDia, 18, theStr);
		NumToString(curEData->vol, theStr);
		SetDText(TheDia, 7, theStr);
		NumToString(curEData->amp, theStr);
		SetDText(TheDia, 21, theStr);
		NumToString(curEData->c2spd, theStr);
		SetDText(TheDia, 6, theStr);
		
		if (curEData->stereo)
			SetDText(TheDia, 52, "\pStereo");
		else
			SetDText(TheDia, 52, "\pMono");
		
		strcpy((Ptr)theStr, curEData->name);
		MyC2PStr((Ptr)theStr);
		SetDText(TheDia, 45, theStr);
		
		NumToString(60 - curEData->relNote, aStr);
		pStrcat(aStr, "\p/");
		OctavesName(48 - curEData->relNote, (Ptr) theStr);
		MyC2PStr( (Ptr) theStr);
		pStrcat(aStr, theStr);
		SetDText(TheDia, 27, aStr);
		
		NumToString(sampleEID+1, aStr);
		SetDText(TheDia, 38, aStr);
		
		switch (curEData->loopType) {
			case eClassicLoop:
				SetDText(TheDia, 55, "\pClassic");
				break;
				
			case ePingPongLoop:
				SetDText(TheDia, 55, "\pPing-Pong");
				break;
		}
	}
}

OSErr CloseInstruData(DialogPtr	TheDia)
{
	Str255	theStr, Str2;
	long	res;
	
	strcpy((Ptr) Str2, curMusic->fid[ insEID].name);
	MyC2PStr((Ptr) Str2);
	
	GetDText(TheDia, 3, (StringPtr) &theStr);
	SetInstruName(insEID, theStr);
	
	if (!EqualString(theStr, Str2, true, true)) curMusic->hasChanged = true;
	
	GetDText(TheDia, 32, (StringPtr)&theStr);
	StringToNum(theStr, &res);
	if (res < 0 || res > 32767) {
		Erreur(78, -1);
		SelectDialogItemText(TheDia, 32, 0, 32767);
		return -1;
	}
	if (curMusic->fid[ insEID].volFade != res)
		curMusic->hasChanged = true;
	curMusic->fid[insEID].volFade = res;
	
	return noErr;
}

OSErr CloseSampleData(DialogPtr	TheDia)
{
	Str255	theStr;
	long	res;

	if (curEData == NULL)
		return noErr;
	
	MADPurgeTrackIfInstru(MADDriver, insEID);
	
	GetDText(TheDia, 45, theStr);
	SetSampName(insEID, sampleEID, theStr);
	GetDText(TheDia, 4, theStr);
	StringToNum(theStr, &res);
	curEData->loopBeg = res;
	GetDText(TheDia, 5, theStr);
	StringToNum(theStr, &res);
	curEData->loopSize = res;
	if (curEData->loopBeg + curEData->loopSize > curEData->size) {
		Erreur(20, -1);
		SelectDialogItemText(TheDia, 5, 0, 32767);
		return -1;
	}

	GetDText (TheDia, 7, theStr);
	StringToNum(theStr, &res);
	if (res < 0 || res > 64) {
		Erreur(77, -1);
		SelectDialogItemText(TheDia, 7, 0, 32767);
		return -1;
	}
	curEData->vol = res;

	GetDText(TheDia, 6, theStr);
	StringToNum(theStr, &res);
	if (res < 1 || res >= 50000)
	{
		Erreur(66, -1);
		SelectDialogItemText(TheDia, 6, 0, 32767);
		return -1;
	}
	curEData->c2spd = res;
	
	GetDText(TheDia, 18, theStr);
	StringToNum(theStr, &res);
	if (res < -96 || res >= 96) {
		Erreur(76, -1);
		SelectDialogItemText(TheDia, 18, 0, 32767);
		return -1;
	}
	curEData->relNote = res;

	curMusic->hasChanged = true;
	
	return noErr;
}

void NEditInstruInfo(short ins, short samp)
{
	long			Result, mresult, i;
	short			itemHit, itemType;
	DialogPtr		TheDia;
	Handle			itemHandle;
	Str255			theStr, aStr;
	short			noScroll;
	Rect			caRect, itemRect;
	Point			myPt;
	GrafPtr			myPort;
	MenuHandle		tempMenu;
	Boolean			changeInstru;
	
	GetPort(&myPort);
	
	EditInfoDlog = TheDia = GetNewDialog(146,NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	
	AutoPosition(TheDia);
	
	if (curMusic->fid[ ins].numSamples == 0)
		noScroll = 0;
	
	GetDialogItem(TheDia, 44, &itemType, &itemHandle, &itemRect);
	sScroll = NewControl(GetDialogWindow(TheDia),
						 &itemRect,
						 "\p.",
						 true,
						 0,
						 0,
						 0,
						 gScrollBarID,
						 0);
	
	ChangeDialogFont(TheDia);
	
CHANGEINSTRU:
	
	SaveUndo(USample, ins, "\pUndo 'Instrument Editing'");
	
	GetDialogItem(TheDia, 29, &itemType, &itemHandle, &itemRect);
	NInitSmallPiano(itemRect, SPianoRect);
	
	changeInstru = false;
	
	if (curMusic->fid[ins].numSamples == 0) {
		noScroll = 0;
		HideDialogItem(TheDia, 17);
		HideDialogItem(TheDia, 30);
		HideDialogItem(TheDia, 4);
		HideDialogItem(TheDia, 9);
		HideDialogItem(TheDia, 11);
		HideDialogItem(TheDia, 6);
		
		HideDialogItem(TheDia, 46);
		HideDialogItem(TheDia, 10);
		HideDialogItem(TheDia, 12);
		HideDialogItem(TheDia, 45);
		HideDialogItem(TheDia, 20);
		HideDialogItem(TheDia, 21);
		
		HideDialogItem(TheDia, 24);
		HideDialogItem(TheDia, 19);
		HideDialogItem(TheDia, 18);
		HideDialogItem(TheDia, 27);
		HideDialogItem(TheDia, 5);
		HideDialogItem(TheDia, 7);
		HideDialogItem(TheDia, 37);
		HideDialogItem(TheDia, 34);
	} else {
		noScroll = curMusic->fid[ins].numSamples - 1;
		ShowDialogItem(TheDia, 17);
		ShowDialogItem(TheDia, 30);
		ShowDialogItem(TheDia, 4);
		ShowDialogItem(TheDia, 9);
		ShowDialogItem(TheDia, 11);
		ShowDialogItem(TheDia, 6);
		
		ShowDialogItem(TheDia, 46);
		ShowDialogItem(TheDia, 10);
		ShowDialogItem(TheDia, 12);
		ShowDialogItem(TheDia, 45);
		ShowDialogItem(TheDia, 20);
		ShowDialogItem(TheDia, 21);
		
		ShowDialogItem(TheDia, 24);
		ShowDialogItem(TheDia, 19);
		ShowDialogItem(TheDia, 18);
		ShowDialogItem(TheDia, 27);
		ShowDialogItem(TheDia, 5);
		ShowDialogItem(TheDia, 7);
		ShowDialogItem(TheDia, 37);
		ShowDialogItem(TheDia, 34);
	}
	
	SetControlMaximum(sScroll, noScroll);
	
	pStrcpy(theStr, "\pInstrument Info - ");
	NumToString(ins+1, aStr);
	pStrcat(theStr, aStr);
	SetWTitle(GetDialogWindow(TheDia), theStr);
	
	/******************/
	// Set Sample Infos
	
	if (samp >= 0) {
		sampleEID = samp;
		SetControlValue(sScroll, samp);
	} else
		sampleEID = 0;
	insEID = ins;
	if (curMusic->fid[ ins].numSamples > 0)
		curEData = curMusic->sample[curMusic->fid[ins].firstSample + sampleEID];
	else
		curEData = NULL;
	
	/******************/
	// Instrument infos
	
	strcpy((Ptr)theStr, curMusic->fid[ins].name);
	MyC2PStr((Ptr)theStr);
	SetDText(TheDia, 3, theStr);
	
	NumToString(curMusic->fid[ins].volFade, theStr);
	SetDText(TheDia, 32, theStr);
	
	// Volume
	
	if (curMusic->fid[ins].volType & EFON)
		TurnRadio(33, TheDia, true);
	else
		TurnRadio(35, TheDia, true);
	if (curMusic->fid[ins].volType & EFSUSTAIN)
		TurnRadio(36, TheDia, true);
	if (curMusic->fid[ins].volType & EFLOOP)
		TurnRadio(39, TheDia, true);
	if (!(curMusic->fid[ins].volType & EFNOTE))
		TurnRadio(22, TheDia, true);
	
	// Panning
	
	if (curMusic->fid[ins].pannType & EFON)
		TurnRadio(48, TheDia, true);
	else
		TurnRadio(15, TheDia, true);
	if (curMusic->fid[ins].pannType & EFSUSTAIN)
		TurnRadio(43, TheDia, true);
	if (curMusic->fid[ins].pannType & EFLOOP)
		TurnRadio(40, TheDia, true);
	if (!(curMusic->fid[ins].pannType & EFNOTE))
		TurnRadio(41, TheDia, true);
	
	ShowSampleData(TheDia);
	
	if (samp < 0)
		SelectDialogItemText(TheDia, 3, 0, 32767);
	else
		SelectDialogItemText(TheDia, 45, 0, 32767);
	
	if (ins == 0)
		ControlSwitch(58, TheDia, 255);
	else
		ControlSwitch(58, TheDia, 0);
	
	if (ins == MAXINSTRU-1)
		ControlSwitch(59, TheDia, 255);
	else
		ControlSwitch(59, TheDia, 0);
	
OnRepart:
	
	do {
		MyModalDialog(TheDia, &itemHit);
		
		switch (itemHit) {
			case 58:		// Previous
				if (ins > 0) {
					ins--;
					samp = 0;
					itemHit = 1;
					changeInstru = true;
					GetPortBounds(GetDialogPort(TheDia), &caRect);
					InvalWindowRect(GetDialogWindow(TheDia), &caRect);
					EraseRect(&caRect);
					NSelectInstruList(ins, samp);
				}
				break;
				
			case 59:		// Next
				if (ins < MAXINSTRU - 1) {
					ins++;
					samp = 0;
					itemHit = 1;
					changeInstru = true;
					GetPortBounds(GetDialogPort(TheDia), &caRect);
					InvalWindowRect(GetDialogWindow(TheDia), &caRect);
					EraseRect(&caRect);
					NSelectInstruList(ins, samp);
				}
				break;
				
			case 15:
			case 48:
				if (itemHit == 48)
					curMusic->fid[ins].pannType |= EFON;
				else if (itemHit == 15)
					curMusic->fid[ ins].pannType &= ~EFON;
				
				if (curMusic->fid[ins].pannType & EFON)	 {
					TurnRadio(48, TheDia, true);
					TurnRadio(15, TheDia, false);
				} else {
					TurnRadio(48, TheDia, false);
					TurnRadio(15, TheDia, true);
				}
				curMusic->hasChanged = true;
				break;
				
			case 41:
				if (curMusic->fid[ins].pannType & EFNOTE)
					curMusic->fid[ins].pannType &= ~EFNOTE;
				else
					curMusic->fid[ins].pannType |= EFNOTE;
				
				if (!(curMusic->fid[ins].pannType & EFNOTE))
					TurnRadio(41, TheDia, true);
				else
					TurnRadio(41, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case 22:
				if (curMusic->fid[ins].volType & EFNOTE)
					curMusic->fid[ins].volType &= ~EFNOTE;
				else
					curMusic->fid[ins].volType |= EFNOTE;
				
				if (!(curMusic->fid[ins].volType & EFNOTE))
					TurnRadio(22, TheDia, true);
				else
					TurnRadio(22, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case 43:
				if (curMusic->fid[ ins].pannType & EFSUSTAIN)
					curMusic->fid[ ins].pannType &= ~EFSUSTAIN;
				else
					curMusic->fid[ ins].pannType |= EFSUSTAIN;
				
				if (curMusic->fid[ ins].pannType & EFSUSTAIN)
					TurnRadio(43, TheDia, true);
				else
					TurnRadio(43, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case 40:
				if (curMusic->fid[ins].pannType & EFLOOP)
					curMusic->fid[ins].pannType &= ~EFLOOP;
				else
					curMusic->fid[ins].pannType |= EFLOOP;
				
				if (curMusic->fid[ ins].pannType & EFLOOP)
					TurnRadio(40, TheDia, true);
				else
					TurnRadio(40, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case -5:
				GetDText(TheDia, 18, aStr);
				if (aStr[0] > 0) {
					StringToNum(aStr, &Result);
					if (Result > -96 && Result < 96) {
						NumToString(60 - Result, aStr);
						pStrcat(aStr, "\p/");
						OctavesName(48 - Result, (Ptr)theStr);
						MyC2PStr((Ptr)theStr);
						pStrcat(aStr, theStr);
						SetDText(TheDia, 27, aStr);
					}
				}
				break;
				
			case 35:
			case 33:
				if (itemHit == 33)
					curMusic->fid[ins].volType |= EFON;
				else if (itemHit == 35)
					curMusic->fid[ins].volType &= ~EFON;
				
				if (curMusic->fid[ins].volType & EFON)	 {
					TurnRadio(33, TheDia, true);
					TurnRadio(35, TheDia, false);
				} else {
					TurnRadio(33, TheDia, false);
					TurnRadio(35, TheDia, true);
				}
				curMusic->hasChanged = true;
				break;
				
			case 36:
				if (curMusic->fid[ins].volType & EFSUSTAIN)
					curMusic->fid[ins].volType &= ~EFSUSTAIN;
				else
					curMusic->fid[ins].volType |= EFSUSTAIN;
				
				if (curMusic->fid[ins].volType & EFSUSTAIN)
					TurnRadio(36, TheDia, true);
				else
					TurnRadio(36, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case 39:
				if (curMusic->fid[ ins].volType & EFLOOP)
					curMusic->fid[ ins].volType &= ~EFLOOP;
				else
					curMusic->fid[ ins].volType |= EFLOOP;
				
				if (curMusic->fid[ ins].volType & EFLOOP)
					TurnRadio(39, TheDia, true);
				else
					TurnRadio(39, TheDia, false);
				
				curMusic->hasChanged = true;
				break;
				
			case 37:
				tempMenu = GetMenu(163);
				
				InsertMenu(tempMenu, hierMenu );
				GetDialogItem(TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				GetDText(TheDia, 6, theStr);
				//	NumToString(curEData->c2spd, theStr);
				for (i = 1; i <= CountMenuItems(tempMenu); i++) {
					GetMenuItemText(tempMenu, i, aStr);
					aStr[ 0] = theStr[ 0];
					if (EqualString(theStr, aStr, false, false))
						break;
				}
				
				if (i <= CountMenuItems(tempMenu))
					SetItemMark(tempMenu, i, 0xa5);
				else
					i = 1;
				
				mresult = PopUpMenuSelect(tempMenu,
										  myPt.v,
										  myPt.h,
										  i);
				
				if (i <= CountMenuItems(tempMenu))
					SetItemMark(tempMenu, i, 0);
				
				if (HiWord(mresult) != 0) {
					long	tempL;
					
					if (CloseSampleData(TheDia) != noErr)
						goto OnRepart;
					
					GetMenuItemText(tempMenu, LoWord(mresult), theStr);
					for (i = 1; i <= theStr[ 0]; i++) {
						if (theStr[i] == ' ')
							break;
					}
					theStr[0] = i - 1;
					
					StringToNum(theStr, &tempL);
					
					curEData->c2spd =  tempL;
					ShowSampleData(EditInfoDlog);
				}
				DeleteMenu(GetMenuID(tempMenu));
				DisposeMenu(tempMenu);
				break;
				
			case 44:
			{
				Point			myPt;
				//short			bogus, ctlPart;
				
				GetMouse(&myPt);
				
				if (TestControl( sScroll, myPt))
#if 0
					ctlPart = FindControl(myPt, GetDialogWindow(TheDia), &theControl);
				if (ctlPart == kControlIndicatorPart)
				{
					bogus = TrackControl(theControl, myPt, NULL);
					if (bogus != 0)
					{
						actionProcInstru(theControl, kControlIndicatorPart);
					}
				}
				else if (ctlPart > 0)
#endif
				{
					ControlActionUPP	MyControlUPP;
					
					MyControlUPP = NewControlActionUPP(actionProcInstru);
					TrackControl(sScroll, myPt, MyControlUPP);
					DisposeControlActionUPP(MyControlUPP);
				}
			}
				break;
				
			case 29:
				NPressSmallPiano(itemHit, TheDia, ins);
				break;
				
			case 56:
				tempMenu = GetMenu(173);
				
				InsertMenu(tempMenu, hierMenu);
				
				GetDialogItem(TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(tempMenu, curEData->loopType + 1, 0xa5);
				
				Result = PopUpMenuSelect(tempMenu,
										 myPt.v,
										 myPt.h,
										 curEData->loopType + 1);
				
				SetItemMark(tempMenu, curEData->loopType + 1, 0);
				
				if (HiWord(Result) != 0) {
					if (LoWord(Result) != curEData->loopType + 1) {
						curEData->loopType = LoWord(Result) - 1;
						
						ShowSampleData(EditInfoDlog);
					}
				}
				
				DeleteMenu(GetMenuID(tempMenu));
				DisposeMenu(tempMenu);
				break;
				
			case 53:
				tempMenu = GetMenu(172);
				
				InsertMenu(tempMenu, hierMenu);
				
				GetDialogItem(TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(tempMenu, curEData->stereo + 1, 0xa5);
				
				if (curEData->stereo == false) {
					DisableMenuItem(tempMenu, 4);
					DisableMenuItem(tempMenu, 5);
					DisableMenuItem(tempMenu, 6);
				}
				
				Result = PopUpMenuSelect(tempMenu,
										 myPt.v,
										 myPt.h,
										 curEData->stereo + 1);
				
				SetItemMark(tempMenu, curEData->stereo + 1, 0);
				
				if (HiWord(Result) != 0) {
					if (LoWord(Result) != curEData->stereo + 1) {
						if (InfoL(91) == true) {
							if (CloseSampleData(TheDia) != noErr)
								goto OnRepart;
							
							ConvertInstrumentMode(curEData, LoWord(Result));
							
							//curEData->stereo = LoWord(Result) - 1;
							
							ShowSampleData(EditInfoDlog);
						}
					}
				}
				
				DeleteMenu(GetMenuID(tempMenu));
				DisposeMenu(tempMenu);
				break;
				
			case 24:
				tempMenu = GetMenu(148);
				
				InsertMenu(tempMenu, hierMenu);
				
				GetDialogItem(TheDia, 24, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(tempMenu, curEData->amp / 8, 0xa5);
				
				Result = PopUpMenuSelect(tempMenu,
										 myPt.v,
										 myPt.h,
										 curEData->amp / 8);
				
				SetItemMark(tempMenu, curEData->amp / 8, 0);
				
				if (HiWord(Result) != 0) {
					if (LoWord(Result) * 8 != curEData->amp) {
						if (InfoL(53) == true) {
							if (CloseSampleData(TheDia) != noErr) goto OnRepart;
							
							ConvertInstrumentAmpli(curEData, LoWord(Result) * 8);
							
							curEData->amp = LoWord(Result) * 8;
							
							ShowSampleData(EditInfoDlog);
						}
					}
				}
				
				DeleteMenu(GetMenuID(tempMenu));
				DisposeMenu(tempMenu);
				break;
		}
		
	}while (itemHit != 1 && itemHit != 2 && itemHit != 61 && itemHit != 60);
	
	if (itemHit != 2)
	{
		// Sample Infos
		
		if (CloseSampleData(TheDia) != noErr) goto OnRepart;
		
		// Instrument Infos
		
		if (CloseInstruData(TheDia) != noErr) goto OnRepart;
		
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
	}
	else
	{
		DoUndo();
	}
	
	if (changeInstru) goto CHANGEINSTRU;
	
	if (itemHit == 61) NCreateSampleWindow(ins, -2);
	if (itemHit == 60) NCreateSampleWindow(ins, -1);
	
	DisposeDialog(TheDia);
	SetPort(myPort);
}

void DoPlayInstruInt2(sData *curData, InstrData *insData, short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end, long samp)
{
//	short	samp;
	
	if (Instru < 0) Instru = 0;
	
	if (Note < 0 || Note > NUMBER_NOTES) return;
	
	MADCheckSpeed(curMusic, MADDriver);
	
	/***********************/
	/* Pour le MIDI Driver */
	/***********************/
	if (MADDriver->DriverSettings.driverMode == MIDISoundDriver)
	{
		if (MADDriver->NoteOld[ curVoice->ID] != -1)
		{
			NoteOff(MADDriver->InstuNoOld[ curVoice->ID], MADDriver->NoteOld[ curVoice->ID], MADDriver->VelocityOld[ curVoice->ID], MADDriver);
			MADDriver->NoteOld[ curVoice->ID] = -1;
		}
		
		SampleMIDI(curVoice, curVoice->ins, Note, MADDriver);
		
		MADDriver->InstuNoOld[ curVoice->ID]	= curVoice->ins;
		MADDriver->NoteOld[ curVoice->ID]		= Note;
		MADDriver->VelocityOld[ curVoice->ID]	= curVoice->vol;
	}
	
	if (insData->numSamples == 0) return;
	
	if (samp == 0xFF) samp					= insData->what[ Note];
	
	if (curData == NULL) return;
	if (curData->size <= 0) return;
	
	curVoice->TrackID		= curVoice->ID;
	curVoice->prevPtr		= NULL;
	curVoice->samplePtr		= NULL;
	curVoice->fineTune		= curData->c2spd;
	curVoice->note			= Note + curData->relNote;
	curVoice->period 		= GetOldPeriod(curVoice->note, curVoice->fineTune, MADDriver);
	curVoice->KeyOn			= true;
	curVoice->vol 			= 64;
	curVoice->volFade		= 32767;
	curVoice->nextvolFade	= 32767;
	curVoice->amp			= curData->amp;
	curVoice->pann 			= curMusic->header->chanPan[ curVoice->ID];
	curVoice->ins 			= Instru;
	curVoice->samp			= samp;
	curVoice->stereo		= curData->stereo;
	curVoice->maxPtr 		= curVoice->begPtr = curData->data;
	curVoice->curPtr		= curVoice->begPtr + start;
	curVoice->loopType		= curData->loopType;
	curVoice->pingpong		= false;
	curVoice->PanningE8		= false;
	curVoice->PatternLoopE6 = 1;
	curVoice->PatternLoopE6Count = 0;
	curVoice->PatternLoopE6ID = 0;
	curVoice->trig			= 0;
	curVoice->preOff		= 0xFFFFFFFF;
	curVoice->preVal		= 0;
	curVoice->spreVal		= 0;
	curVoice->preVal2		= *curVoice->curPtr;
	if (curVoice->amp == 8) curVoice->preVal2R	= *(curVoice->curPtr+1);
	else curVoice->preVal2R	= *(curVoice->curPtr+2);
	curVoice->spreVal2		= *(short*) curVoice->curPtr;
	curVoice->spreVal2R		= *(short*) (curVoice->curPtr+2);
	
	if (end != 0) curVoice->maxPtr	= curData->data + end;
	else curVoice->maxPtr 		+= curData->size;
	curVoice->viboffset 	= 0;
	
	if (end == 0 && start == 0)
	{
		if (curData->loopSize > 2)
		{
			curVoice->maxPtr = (Ptr) (	(long) curData->data +
										(long) curData->loopBeg +
										(long) curData->loopSize);
		}
		curVoice->loopBeg 		= curData->loopBeg;
		curVoice->loopSize 		= curData->loopSize;
	}
	else
	{
		curVoice->loopBeg 		= 0;
		curVoice->loopSize 		= 0;
		curVoice->loopType		= 0;
	}
	
	if (vol != 0xFF)
	{
		if(vol >= 0x10 && vol <= 0x50)
		{
			curVoice->vol = vol - 0x10;
			if (curVoice->vol < MIN_VOLUME) curVoice->vol = MIN_VOLUME;
			else if (curVoice->vol > MAX_VOLUME) curVoice->vol = MAX_VOLUME;
			
			curVoice->volcmd = 0;
		}
		else curVoice->volcmd = vol;
	}
	else curVoice->volcmd = 0;
	
	curVoice->cmd			= effect;
	curVoice->arg 			= arg;
	SetUpEffect(curVoice, MADDriver);
	
	MADDriver->Tube[ curVoice->ID] 		= curVoice->vol;
	MADDriver->InstruTube[ Instru] 		= 64;
	MADDriver->InstruActif[ Instru] 		= curVoice->ID;
	
	StartEnvelope(curVoice);
	StartPanning(curVoice);
	ProcessEnvelope(curVoice, MADDriver, true);
	ProcessPanning(curVoice, MADDriver, true);
	ProcessFadeOut(curVoice, MADDriver);
}

void DoPlayInstruInt(short	Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end)
{
	//sData	*curData;
	short	samp;
	
	if (Instru < 0) return;
	
	samp = curMusic->fid[ Instru].what[ Note];
	
	DoPlayInstruInt2(curMusic->sample[ curMusic->fid[ Instru].firstSample + samp], &curMusic->fid[ Instru], Note, Instru, effect, arg, vol, curVoice, start, end, 0xFF);
}

void NDoPlayInstru(short Note, short Instru, short effect, short arg, short vol)
{
	Channel			*curVoice;
	short			track;
	
	if (Note >= NUMBER_NOTES) return;
	
	track = GetWhichTrackPlay();
	curVoice = &MADDriver->chan[ track];

	DoPlayInstruInt(Note, Instru, effect, arg, vol, curVoice, 0, 0);
}

void DoKeyPlay(short note)
{
	short	ins;
	
	if (InstruListDlog == NULL)
		return;
	
	if (GetIns(&ins, NULL)) {
		NDoPlayInstru(note, ins, 0, 0, 0xFF);
	}
}

void CreateInstruList(void)
{
	short	i;
	GrafPtr	SavePort;
	
	if (InstruListDlog == NULL)
		return;
	
	GetPort(&SavePort);
	SetPortDialogPort(InstruListDlog);
	
	InvalWindowRect(GetDialogWindow(InstruListDlog), &myList.rect);
	
	if (myList.maxY != ComputeMaxY()) {
		myList.maxY = ComputeMaxY();
		PLSetControl(&myList);
	}
	
	for (i = 0; i < myList.maxY; i++)  SAButState[i] = false;
	
	DrawBottomInfo();
	
	SetPort(SavePort);
}

void FreeInstruList(short iD)
{
	Rect		itemRect;
	short		ins, samp;
	
	ConvertIDtoInsSamp(iD, &ins, &samp);
	
	if (thePrefs.OCArrow[ins]) {
		if (samp >= 0) {
			sData	*curData = curMusic->sample[curMusic->fid[ins].firstSample + samp];
			
			itemRect.left = 2;
			itemRect.right = itemRect.left + 7;
			itemRect.top = myList.rect.top + 2 + (iD - GetControlValue(myList.yScroll))*myList.HCell;
			itemRect.bottom = itemRect.top + 7;
			
			if (curMusic->fid[ins].numSamples == 0)
				RGBForeColor(&theColor);
			else
				RGBForeColor(&theColor);
			
			if (curData->size <= 0)
				ForeColor(whiteColor);
			
			PaintRect(&itemRect);
			
			ForeColor(blackColor);
		}
	} else {
		Rect destRect;
		
		destRect = (*CP)->bounds;
		destRect.top = myList.rect.top + myList.HCell*(iD - GetControlValue(myList.yScroll));
		destRect.bottom = destRect.top + (*CP)->bounds.bottom;
		
		CopyBits((BitMap*) *(CP),
				 (BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
				 &(*CP)->bounds,
				 &destRect,
				 srcCopy,
				 NULL);
	}
}

void PaintInstruList(short InstruNo, short track)
{
	Rect itemRect;

	SwitchColor(track);
	
	itemRect.left = 2;
	itemRect.right = itemRect.left + 7;
	itemRect.top = myList.rect.top + 2 + (InstruNo - GetControlValue(myList.yScroll))*myList.HCell;
	itemRect.bottom = itemRect.top + 7;

	PaintRect(&itemRect);
	
	ForeColor(blackColor);
}

void PaintArrowList(short ins, short t)
{
	short y;

	SwitchColor(t);
	
	y = myList.rect.top + 2 + (ins - GetControlValue(myList.yScroll))*myList.HCell;
	
	MoveTo(5, y+1);	LineTo(5, y+5);
	MoveTo(6, y+2);	LineTo(6, y+4);
	MoveTo(7, y+3);	LineTo(7, y+3);

	ForeColor(blackColor);
}

//static short lastPosS[ MAXTRACK];

void DoNullInstruList(void)
{
	short		i;
	short 		max, maxChannels;
	//long		maxVal, curVal;
	GrafPtr		savedPort;
	RgnHandle	saveClipRgn;
	
	if (InstruListDlog == NULL)
		return;
	
	GetPort(&savedPort);
	SetPortDialogPort(InstruListDlog);
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	
	ButtonRect.bottom = myList.rect.bottom;
	ClipRect(&ButtonRect);
	
	max = PLGetMaxYValue(&myList);
	
	for (i = GetControlValue(myList.yScroll); i < max; i++) InstrUsed[i] = false;
	
	maxChannels = MADDriver->MultiChanNo;
	if (MADDriver->DriverSettings.numChn > maxChannels)
		maxChannels = MADDriver->DriverSettings.numChn;
	
	for(i=0; i < maxChannels; i++) {		//MADDriver->DriverSettings.numChn
		short InstruNo, SampNo;
		
		InstruNo	= MADDriver->chan[ i].ins;
		SampNo		= MADDriver->chan[ i].samp;
		
		if (InstruNo >= GetControlValue(myList.yScroll) || InstruNo < max) {
			if (SampNo < curMusic->fid[ InstruNo].numSamples) {
#if 0
				sData	*curData = SampleDataN(InstruNo, SampNo);
				
				maxVal = (long) MADDriver->chan[ i].maxPtr;
				//maxVal -=(long) curData->data;
				
				curVal = (long) MADDriver->chan[ i].curPtr;
				//curVal -= (long) curData->data;
#endif
				if (MADDriver->chan[i].maxPtr > MADDriver->chan[i].curPtr && MADDriver->chan[i].curPtr != NULL) {
					short temp;
					
					temp = ConvertInsSampToID(InstruNo, SampNo);
					
					InstrUsed[ temp] = true;
					SAButState[ temp] = true;
					
					if (thePrefs.OCArrow[ InstruNo]) PaintInstruList(temp, MADDriver->chan[ i].TrackID);
					else PaintArrowList(temp, MADDriver->chan[ i].TrackID);
					
#if 0
					if (myList.select.top == temp && InstruListDlog->portRect.right == 300) {
						Handle	iHandle;
						Rect	iRect;
						short	iType;
						long	tL;
						
						GetDialogItem(InstruListDlog, 30, &iType, &iHandle, &iRect);
						
						tL = (((long) (iRect.right - iRect.left) * curVal) / maxVal);
						if (tL != lastPosS[ i]) {
							lastPosS[ i] = tL;
							iRect.left = iRect.left + tL;
							iRect.right = iRect.left+1;
							
							SetClip(saveClipRgn);
							
							SwitchColor(i);
							PaintRect(&iRect);
							
							ClipRect(&ButtonRect);
						}
					}
#endif
				}
			}
		}
	}
	
	for (i = GetControlValue(myList.yScroll); i < max; i++) {
		if (!InstrUsed[ i])
		{
			if (SAButState[ i])
			{
				SAButState[ i] = false;
				
				FreeInstruList(i);
			}
		}
	}
	
	SetClip(saveClipRgn);
	DisposeRgn(saveClipRgn);
	
#if 0
	if (oldWindow == InstruListDlog) {
		PLGetSelectRect(&tRect, &myList);
		GetMouse(&pt);
		if (PtInRect(pt, &tRect)) {
			SetCursor(&HandCrsr);
		} else
			SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
#endif
	
	SetPort(savedPort);
}

void ConvertIDtoInsSamp(short iD, short *ins, short *samp)
{
	short	totsamp;
	
	*ins		= 0;
	totsamp		= 0;
	
	while (iD >= totsamp + *ins) {
		if (thePrefs.OCArrow[*ins]) {
			totsamp += curMusic->fid[*ins].numSamples;
		}
		(*ins)		++;
	}
	
	(*ins)--;
	if (samp != NULL) {
		*samp = -1;
		
		if (thePrefs.OCArrow[*ins])
			*samp = curMusic->fid[*ins].numSamples - (totsamp + *ins - iD) - 1;
		else
			*samp = 0 - (totsamp + *ins - iD) - 1;
	}
}

short ConvertInsSampToID(short ins, short samp)
{
	short i, ret = 0;
	
	for (i = 0; i < ins; i++) {
		if (thePrefs.OCArrow[i])
			ret += curMusic->fid[i].numSamples;
	}
	ret += ins;
	
	if (samp >= curMusic->fid[ins].numSamples)
		samp = curMusic->fid[ins].numSamples-1;
	if (samp >= 0 && thePrefs.OCArrow[ins] == true)
		ret += samp + 1;
	
	return ret;
}


short ComputeMaxY()
{
	short i, tot;
	
	tot = MAXINSTRU;
	for (i = 0; i < MAXINSTRU; i++) {
		if (thePrefs.OCArrow[i])
			tot += curMusic->fid[i].numSamples;
	}
	
	return tot;
}

void DrawInstruListItem(short iD)
{
	short		pos;
	Str255		aStr, tempStr;
	Rect		destRect;
	
	short		ins, samp;
	RgnHandle	UpdateRgn = NULL;
	
	pos = iD - GetControlValue(myList.yScroll);
	
	ConvertIDtoInsSamp(iD, &ins, &samp);
	
	if (samp < 0) {	// Instrument
		/**/
		
		destRect = (*mySABut)->bounds;
		destRect.top = myList.rect.top + myList.HCell*pos;
		destRect.bottom = destRect.top + myList.HCell;
		
		if (curMusic->fid[ins].numSamples <= 0) {
			RGBForeColor(&theColor);
			PaintRect(&destRect);
			ForeColor(blackColor);
		} else {
			destRect = (*OP)->bounds;
			destRect.top = myList.rect.top + myList.HCell*pos;
			destRect.bottom = destRect.top + (*OP)->bounds.bottom;
			
			if (thePrefs.OCArrow[ins]) {
				CopyBits((BitMap*)*(OP),
						 (BitMap*)*GetPortPixMap(GetDialogPort(InstruListDlog)),
						 &(*OP)->bounds,
						 &destRect,
						 srcCopy,
						 NULL);
			} else {
				CopyBits((BitMap*)*(CP),
						 (BitMap*)*GetPortPixMap(GetDialogPort(InstruListDlog)),
						 &(*CP)->bounds,
						 &destRect,
						 srcCopy,
						 NULL);
			}
		}
		
		
		MoveTo(destRect.right, destRect.top);
		LineTo(destRect.right, destRect.bottom-1);
		
		/**/
		
		NNumToString(ins + 1, aStr);
		pStrcat(aStr, "\p ");
		
		strcpy((Ptr) tempStr, curMusic->fid[ ins].name);
		MyC2PStr((Ptr)tempStr);
		pStrcat(aStr, "\p ");
		pStrcat(aStr, tempStr);
		
		MoveTo(myList.rect.left + POSINS, myList.rect.top + 10 + myList.HCell*pos);
		TextFace(bold);
		DrawString(aStr);
		TextFace(0);
	} else {
		sData *curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
		
		/**/
		
		destRect = (*mySABut)->bounds;
		destRect.top = myList.rect.top + myList.HCell*pos-1;
		destRect.bottom = destRect.top + (*mySABut)->bounds.bottom;
		
		CopyBits((BitMap*) *(mySABut),
				 (BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
				 &(*mySABut)->bounds,
				 &destRect,
				 srcCopy,
				 NULL);
		
		MoveTo(destRect.right, destRect.top);
		LineTo(destRect.right, destRect.bottom);
		
		FreeInstruList(iD);
		
		/**/
		
		if (curData->loopSize > 0) {
			destRect = (*ArrowPix)->bounds;
			destRect.left = myList.rect.left + POSINS + 3;
			destRect.right = destRect.left + (*ArrowPix)->bounds.right;
			destRect.top = myList.rect.top + 2 + myList.HCell*pos;
			destRect.bottom = destRect.top + (*ArrowPix)->bounds.bottom;
			
			CopyBits((BitMap*) *(ArrowPix),
					 (BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
					 &(*ArrowPix)->bounds,
					 &destRect,
					 srcCopy,
					 NULL);
		}
		/**/
		strcpy((Ptr)tempStr, curData->name);
		MyC2PStr((Ptr)tempStr);
		pStrcpy(aStr, tempStr);
		
		MoveTo(myList.rect.left + POSSAMP, myList.rect.top + 10 + myList.HCell*pos);
		
		DrawString(aStr);
	}
	
	if (samp == curMusic->fid[ins].numSamples-1 || thePrefs.OCArrow[ins] == false) {
		MoveTo(myList.rect.left, myList.rect.top -1+ myList.HCell*(pos+1));
		LineTo(myList.rect.right, myList.rect.top -1+ myList.HCell*(pos+1));
	} else {
		RGBForeColor(&theColor);
		MoveTo(myList.rect.left, myList.rect.top -1+ myList.HCell*(pos+1));
		LineTo(myList.rect.left + (*mySABut)->bounds.right-1, myList.rect.top -1+ myList.HCell*(pos+1));
		ForeColor(blackColor);
	}
	
	if (iD >= myList.select.top && iD <= myList.select.bottom) {
		Rect 			t;
		
		LMSetHiliteMode((UInt8) (LMGetHiliteMode() & 0x7F));
		
		t.left = (*mySABut)->bounds.right + 1;
		t.right = myList.rect.right;
		t.top = myList.rect.top + myList.HCell*pos;
		t.bottom = t.top + myList.HCell-1;
		
		InvertRect(&t);
	}
}
					
void DrawSmallSamplePreview()
{
	short		iType, ins, samp;
	Handle		iHandle;
	Rect		caRect, iRect;
	GWorldPtr	theGWorld = NULL;
	GDHandle	oldGDeviceH;
	CGrafPtr	oldPort;
	
	GetPortBounds(GetDialogPort(InstruListDlog), &caRect);
	
	if (caRect.right != 300)
		return;
	
	GetDialogItem(InstruListDlog, 30, &iType, &iHandle, &iRect);
	iRect.right = caRect.right-1;
	
	if (myList.select.top >= 0) {
		ConvertIDtoInsSamp(myList.select.top, &ins, &samp);
		
		if (samp < 0) {
			if (curMusic->fid[ins].numSamples > 0)
				samp = 0;
		}
		
		if (samp >= 0) {
			sData	*curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
			Rect	loopRect;
			
			GetGWorld(&oldPort, &oldGDeviceH);
			theGWorld = NULL;
			NewGWorld(&theGWorld, 8, &iRect, NULL, NULL, 0);
			LockPixels(GetPortPixMap(theGWorld));
			
			SetGWorld(theGWorld, NULL);
			
			PaintRect(&iRect);
			
			ForeColor(whiteColor);
			
			if (curData->size <= 0)  {
				Str255		aStr;
				short		itemType;
				
				ForeColor(redColor);
				
				pStrcpy(aStr, "\pNo data");
				itemType = StringWidth(aStr);
				
				MoveTo(iRect.left + (iRect.right - iRect.left)/2 - itemType/2,
					   iRect.top + (iRect.bottom - iRect.top)/2 + 3);
				DrawString(aStr);
				ForeColor(blackColor);
			} else {
				if (curData->stereo) {
					ForeColor(blueColor);
					DrawSampleInt(curData->size,
								  0,
								  0,
								  iRect.right - iRect.left,
								  iRect.bottom - iRect.top,
								  iRect.right - iRect.left,
								  iRect.top,
								  iRect.left,
								  1,
								  curData);
				}
				
				ForeColor(redColor);
				DrawSampleInt(curData->size,
							  0,
							  0,
							  iRect.right - iRect.left,
							  iRect.bottom - iRect.top,
							  iRect.right - iRect.left,
							  iRect.top,
							  iRect.left,
							  0,
							  curData);
				
				ForeColor(whiteColor);
			}
			
			if (curData->loopSize > 0) {
				loopRect = iRect;
				loopRect.left = iRect.left + (curData->loopBeg * (iRect.right - iRect.left)) / curData->size;
				loopRect.right = loopRect.left + (curData->loopSize * (iRect.right - iRect.left)) / curData->size;
				
				loopRect.top++;
				loopRect.bottom--;
				ForeColor(redColor);
				FrameRect(&loopRect);
			}
			
			ForeColor(blackColor);
			FrameRect(&iRect);
			//ForeColor(blackColor);
			
			
			SetGWorld(oldPort, oldGDeviceH);
			
			CopyBits((BitMap*) *GetPortPixMap(theGWorld),
					 (BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
					 &iRect,
					 &iRect,
					 srcCopy,
					 NULL);
			
			UnlockPixels(GetPortPixMap(theGWorld));
			DisposeGWorld(theGWorld);
			
		} else
			EraseRect(&iRect);
	} else
		EraseRect(&iRect);
}

void UpdateInstruListWindow(DialogPtr GetSelection)
{
	Rect   		caRect, tempRect;
	int			i;
	GrafPtr		SavePort;
	RgnHandle	saveClipRgn = NULL;
	RgnHandle	visibleRegion = NULL;

	GetPort(&SavePort);
	SetPortDialogPort(GetSelection);
	BeginUpdate(GetDialogWindow(GetSelection));
	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion(GetDialogPort(GetSelection), visibleRegion);
	
	UpdateDialog(GetSelection, visibleRegion);
	
	saveClipRgn = NewRgn();
	GetClip(saveClipRgn);
	
	ClipRect(&myList.rect);
	
	ForeColor(whiteColor);
	tempRect = myList.rect;
	tempRect.left = (*mySABut)->bounds.right;
	PaintRect(&tempRect);
	ForeColor(blackColor);
	
	BackColor(whiteColor);
	for (i = GetControlValue(myList.yScroll); i < PLGetMaxYValue(&myList); i++) {
		DrawInstruListItem(i);
	}
	RGBBackColor(&theColor);
	
	SetClip(saveClipRgn);
	DisposeRgn(saveClipRgn);
	
	GetPortBounds(GetDialogPort(GetSelection), &caRect);
	
	MoveTo(0, myList.rect.top - 1);
	LineTo(caRect.right, myList.rect.top - 1);
	
	MoveTo(myList.rect.left-1, myList.rect.top - 1);
	LineTo(myList.rect.left-1, myList.rect.bottom);
	
	MoveTo(0, myList.rect.bottom);
	LineTo(caRect.right, myList.rect.bottom);
	
	/*********************/
	
	DrawGrowIconP(GetSelection);
	
	DrawBottomInfo();
	
	DrawSmallSamplePreview();
	
	QDFlushPortBuffer(GetDialogPort(GetSelection), visibleRegion);
	DisposeRgn(visibleRegion);
	EndUpdate(GetDialogWindow(GetSelection));

	
	SetPort(SavePort);
}

void OutPutHexShort(Str255 , short , short );

void EraseSampleInfo()
{
	Str255	String = "\p-";
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	
	SetDText(InstruListDlog, 10,String);
	SetDText(InstruListDlog, 11,String);
	SetDText(InstruListDlog, 12,String);
	SetDText(InstruListDlog, 13,String);
	SetDText(InstruListDlog, 15,String);
	SetDText(InstruListDlog, 17,String);
	SetDText(InstruListDlog, 32,String);
	
	GetDialogItem(InstruListDlog, 30, &iType, &iHandle, &iRect);
	EraseRect(&iRect);
}

void EraseInstrumentInfo(void)
{
	Str255	String;

	String[0]=1;
	String[1]='-';
	SetDText(InstruListDlog, 8, String);
	SetDText(InstruListDlog, 9, String);
	SetDText(InstruListDlog, 29, String);
	
	EraseSampleInfo();

	HiliteControl(PlayBut, 255);
	HiliteControl(LoadBut, 255);
	HiliteControl(DelBut, 255);
	HiliteControl(SaveBut, 255);
	HiliteControl(RecBut, 255);
	HiliteControl(OpenBut, 255);
}

void NSelectInstruList(short instru, short sample)
{
	GrafPtr		savePort;
	Point		theCell;

	if (InstruListDlog == NULL)
		return;
	if (instru < 0 || instru >= MAXINSTRU)
		return;

	GetPort(&savePort);
	SetPortDialogPort(InstruListDlog);
	
	BackColor(whiteColor);
	
	theCell.v = ConvertInsSampToID(instru, sample);
	theCell.h = 0;
	PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
	RGBBackColor(&theColor);
	
	DrawInfoInstrument();
	
	SetPort(savePort);
}

void DrawInfoInstrument(void)
{
	Str255		String;
	Point		cSize;
	GrafPtr		SavePort;
	
	if (InstruListDlog == NULL)
		return;
	
	GetPort(&SavePort);
	SetPortDialogPort(InstruListDlog);
	
	DrawSmallSamplePreview();
	
	cSize.v = cSize.h = 0;
	if (PLGetSelect(&cSize, &myList)) {
		short ins, samp;
		
		//DrawBottomInfo();
		HiliteControl(PlayBut, 0);
		HiliteControl(LoadBut, 0);
		HiliteControl(DelBut, 0);
		HiliteControl(RecBut, 0);
		HiliteControl(OpenBut, 0);
		
		ConvertIDtoInsSamp(cSize.v, &ins, &samp);
		
		SetInstruMozart(ins);
		SetInstruEditor(ins);
		
		NTStr(3, ins + 1, (Ptr) String);
		MyC2PStr((Ptr) String);
		
		SetDText(InstruListDlog, 8, String);
		NumToString(curMusic->fid[ ins].numSamples, String);
		SetDText(InstruListDlog, 29,String);
		
		if (samp == -1) {
			long x, fileSize = 0;
			
			for (x = 0; x < curMusic->fid[ ins].numSamples ; x++)
			{
				sData	*curData = curMusic->sample[ curMusic->fid[ ins].firstSample + x];
				
				fileSize += curData->size;
			}
			
			GetSizeString(fileSize, String, true);
			
			SetDText(InstruListDlog, 9, String);
		}
		
		if (curMusic->header != NULL) {
			// Instrument informations
			
			if (curMusic->fid[ ins].numSamples > 0) HiliteControl(SaveBut, 0);
			else if (samp < 0) HiliteControl(SaveBut, 255);
			
			if (samp >= 0) {		// Sample informations
				sData	*curData = curMusic->sample[curMusic->fid[ins].firstSample +  samp];
				
				GetSizeString(curData->size, String, true);
				SetDText(InstruListDlog, 9, String);
				GetSizeString(curData->loopBeg, String, true);
				SetDText(InstruListDlog, 10,String);
				GetSizeString(curData->loopSize, String, true);
				SetDText(InstruListDlog, 11,String);
				NumToString(curData->vol, String);
				SetDText(InstruListDlog, 12,String);
				NumToString(curData->c2spd, String);
				SetDText(InstruListDlog, 13,String);
				NumToString(curData->amp, String);
				SetDText(InstruListDlog, 17,String);
				NumToString(curData->relNote, String);
				SetDText(InstruListDlog, 15,String);
				if (curData->stereo)
					SetDText(InstruListDlog, 32, "\pStereo");
				else
					SetDText(InstruListDlog, 32, "\pMono");
				
				if (curData->size)
					HiliteControl(SaveBut, 0);
				else
					HiliteControl(SaveBut, 255);
			} else
				EraseSampleInfo();
		} else
			EraseInstrumentInfo();
	} else
		EraseInstrumentInfo();
	
	RgnHandle UpdateRgn = NewRgn();
	GetWindowRegion(GetDialogWindow(InstruListDlog), kWindowUpdateRgn, UpdateRgn);
	QDFlushPortBuffer(GetDialogPort(InstruListDlog), UpdateRgn);
	DisposeRgn(UpdateRgn);
	
	SetPort(SavePort);
}

static Boolean firstCall;

Boolean DragInstruSelect(void)
{
	Point		aPt;
	Rect		cellRect;
	RgnHandle	tempRgn;
	short		ins, samp;

	if (DragManagerUse) {
		if (GetIns(&ins, &samp)) {
			if (WaitMouseMoved(theEvent.where)) {
				PLGetSelectRect(&cellRect, &myList);
				cellRect.left += (*mySABut)->bounds.right + 1;
				if (samp < 0 && thePrefs.OCArrow[ins] == true) {
					cellRect.bottom = cellRect.top + (curMusic->fid[ins].numSamples+1) * myList.HCell;
				}
				
				tempRgn = NewRgn();
				RectRgn(tempRgn, &cellRect);
				
				GetMouse(&aPt);
				LocalToGlobal(&aPt);
				theEvent.where = aPt;
				
				DragInstrument(tempRgn, myList.select.top, &theEvent);
				
				DisposeRgn(tempRgn);
				
				return true;
			}
		}
	}
	
	return false;
}


/***********************************************************************************************************/
/************************************************           ************************************************/
/************************************************ RECEPTION ************************************************/
/************************************************           ************************************************/
/***********************************************************************************************************/

/*
 *	MyTrackingHandler
 *
 *	This is the drag tracking handler for windows in the DragText application.
 */

static	OSType DragType;

Boolean IsMyTypeAvailable(DragReference theDrag)
{
	FlavorFlags     	theFlags;
	ItemReference   	theItem;
	OSErr           	result;
	long				textSize;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	
	DragType = 0;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
    
    result = GetFlavorFlags(theDrag, theItem, 'Pcmd', &theFlags);
	if (result == noErr)
		return true;
	
	result = GetFlavorFlags(theDrag, theItem, 'snd ', &theFlags);
	if (result == noErr)
		return true;
	
	result = GetFlavorFlags(theDrag, theItem, kDragFlavorTypeHFS, &theFlags);
	if (result == noErr) {
		Boolean		targetIsFolder, wasAliased;
		
		GetFlavorDataSize(theDrag, theItem, kDragFlavorTypeHFS, &textSize);
		
		GetFlavorData(theDrag, theItem, kDragFlavorTypeHFS, &myFlavor, &textSize, 0);
		
		ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		FSpGetFInfo(&myFlavor.fileSpec, &fndrInfo);
		
		if (fndrInfo.fdType == 'INso') {
			DragType = fndrInfo.fdType;
			return true;
		}
		
		if (PPINAvailablePlug(fndrInfo.fdType, &DragType) == noErr)
			return true;
		else if (PPINIdentifyFile(&DragType, &myFlavor.fileSpec) == noErr)
			return true;
		else
			return false;
    }
	
    return false;
}

static ControlHandle selectedControl;

pascal OSErr MyTrackingHandler(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{

	unsigned long		attributes;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;

	if (!mainSystemDrag)
		return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag))
		return noErr;

	SetPortWindowPort(theWindow);
	BackColor(whiteColor);
	
	GetDragAttributes(theDrag, &attributes);

	switch (message) {
		case kDragTrackingEnterHandler:
			DragType = 0;
			if (attributes & kDragInsideSenderWindow)
				canAcceptDrag = true;
			else
				canAcceptDrag = IsMyTypeAvailable(theDrag);
			break;
		
		case kDragTrackingEnterWindow:
		
			break;

		case kDragTrackingInWindow:
			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);

			DestIns = -1;
			DestSamp = -1;
			selectedControl = NULL;
			
			if (PtInRect(localMouse, &myList.rect)) {
				if (DragType == 'INso') {	// If it is an instrument list !
					DestIns = 0;		// To avoid an err from Receive function
				
					tempRect = myList.rect;
					tempRect.left += (*mySABut)->bounds.right + 1;
					
					RectRgn(theRgn = NewRgn(), &tempRect);
					ShowDragHilite(theDrag, theRgn, true);
					DisposeRgn(theRgn);
				} else {
					theCell.h = 0;
					for (theCell.v = 0; theCell.v < myList.maxY; theCell.v++) {
						PLRect(&tempRect, theCell, &myList);
						if (PtInRect(localMouse, &tempRect)) {
							ConvertIDtoInsSamp(theCell.v, &DestIns, &DestSamp);
							
							if (DragType == 'INST' || (DragIns >= 0 && DragSamp < 0)) {
								Point	tCell;
								
								DestSamp = -1;
								tCell.v = ConvertInsSampToID(DestIns, DestSamp);
								tCell.h = 0;
								PLRect(&tempRect, tCell, &myList);
								
								if (thePrefs.OCArrow[ DestIns])
									tempRect.bottom = tempRect.top + (curMusic->fid[ DestIns].numSamples+1) * myList.HCell;
								else
									tempRect.bottom = tempRect.top + myList.HCell;
							}
							
							tempRect.bottom --;
							tempRect.left += (*mySABut)->bounds.right + 1;
							
							RectRgn(theRgn = NewRgn(), &tempRect);
							ShowDragHilite(theDrag, theRgn, true);
							DisposeRgn(theRgn);
						}
					}
				}
			} else if (attributes & kDragInsideSenderWindow) {
				Rect	contrlRect;
				
				HideDragHilite(theDrag);
				
				GetControlBounds(SaveBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(SaveBut, kControlButtonPart);
					selectedControl = SaveBut;
				} else
					HiliteControl(SaveBut, 0);
				
				GetControlBounds(PlayBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(PlayBut, kControlButtonPart);
					selectedControl = PlayBut;
				} else
					HiliteControl(PlayBut, 0);
				
				GetControlBounds(DelBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(DelBut, kControlButtonPart);
					selectedControl = DelBut;
				} else
					HiliteControl(DelBut, 0);
				
				GetControlBounds(OpenBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(OpenBut, kControlButtonPart);
					selectedControl = OpenBut;
				} else
					HiliteControl(OpenBut, 0);
				
				GetControlBounds(RecBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(RecBut, kControlButtonPart);
					selectedControl = RecBut;
				} else
					HiliteControl(RecBut, 0);
				
				GetControlBounds(LoadBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) {
					HiliteControl(LoadBut, kControlButtonPart);
					selectedControl = LoadBut;
				} else
					HiliteControl(LoadBut, 0);
			}
			break;

		case kDragTrackingLeaveWindow:
			HideDragHilite(theDrag);
			break;

		case kDragTrackingLeaveHandler:
		
			break;

	}
	RGBBackColor(&theColor);

	return(noErr);
}

pascal OSErr MyReceiveDropHandler(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	HFSFlavor		myFlavor;
	OSErr			result;
	ItemReference	theItem;
	DragAttributes	attributes;
	Size			textSize;
	short			mouseDownModifiers, mouseUpModifiers, moveInstru;
	Point			theCell;
	Str255			myStr;
	
	if (!mainSystemDrag)
		return dragNotAcceptedErr;
	
	SetPortWindowPort(theWindow);
	if (DestIns == -1 && selectedControl == NULL) {
		return dragNotAcceptedErr;
	}
	if (!canAcceptDrag)
		return dragNotAcceptedErr;
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	if (selectedControl != NULL) {	// Drag sur un bouton !!!!
		HiliteControl(selectedControl, 0);
		
		if (selectedControl == SaveBut)
			HandleInstruChoice(4);
		//if (selectedControl == PlayBut)		DoKeyPressInstruList(getinfo, 0);
		if (selectedControl == DelBut)
			HandleInstruChoice(5);
		if (selectedControl == RecBut)
			HandleNewSound(3);
		if (selectedControl == LoadBut)
			HandleNewSound(1);
		if (selectedControl == OpenBut) {
			theCell.v = 0;	theCell.h = 0;
			if (PLGetSelect(&theCell, &myList)) {
				short	ins, samp;
				ConvertIDtoInsSamp(theCell.v, &ins, &samp);
				NCreateSampleWindow(ins, samp);
			}
		}
		
		selectedControl = NULL;
		
		return noErr;
	}
	
	moveInstru = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	if (moveInstru && DragIns == DestIns && DragSamp == DestSamp)
		return noErr;
	
	BackColor(whiteColor);
	HideDragHilite(theDrag);
	RGBBackColor(&theColor);
	
	curMusic->hasChanged = true;
	
	//  Un instrument & sample déplacé à l'intérieur de la fenêtre
	
	if ((attributes & kDragInsideSenderWindow) && (DragIns != DestIns || DragSamp != DestSamp)) {		// Delete the source, if moving without option key.
		
		if (DragSamp < 0) {			// Déplacement d'un instrument
			InstrData	*srcIns, *dstIns;
			sData		*srcData, *dstData;
			short		x;
			
			SaveUndo(UAllSamples, 0, "\pUndo 'Move instrument'");
			
			srcIns = &curMusic->fid[DragIns];
			dstIns = &curMusic->fid[DestIns];
			MADKillInstrument(curMusic, DestIns);
			
			for (x = 0; x < srcIns->numSamples; x++) {
				dstData = MADCreateSample(curMusic, DestIns, x);
				srcData = curMusic->sample[curMusic->fid[DragIns].firstSample +  x];
				
				*dstData = *srcData;
				
				dstData->data = NewPtr(dstData->size);
				if (dstData->data != NULL) {
					BlockMoveData(srcData->data, dstData->data, dstData->size);
				} else {
					Erreur(63, MemError());
					MADKillInstrument(curMusic, DestIns);
					
					return dragNotAcceptedErr;
				}
			}
			
			x = dstIns->firstSample;
			*dstIns = *srcIns;
			dstIns->firstSample = x;
			
			if (moveInstru) {
				MADKillInstrument(curMusic, DragIns);
			}
		} else {							// Déplacement d'un sample
			if (DestSamp < 0) {			// Rajout d'un sample à un instrument
				sData		*srcData, *dstData;
				
				SaveUndo(UAllSamples, 0, "\pUndo 'Move sample'");
				
				DestSamp = curMusic->fid[ DestIns].numSamples;
				//if (DestSamp >= MAXSAMPLE) return -1;
				
				//curMusic->fid[ DestIns].numSamples++;
				
				dstData = MADCreateSample(curMusic, DestIns, DestSamp);
				srcData = curMusic->sample[ curMusic->fid[ DragIns].firstSample +  DragSamp];
				
				*dstData = *srcData;
				
				dstData->data = NewPtr(dstData->size);
				if (dstData->data != NULL) {
					BlockMoveData(srcData->data, dstData->data, dstData->size);
				} else {
					Erreur(63, MemError());
					MADPurgeTrackIfInstru(MADDriver, DestIns);
					MADKillSample(curMusic, DestIns, DestSamp);
					return(dragNotAcceptedErr);
				}
				
				if (moveInstru) {
					if (DestIns == DragIns && DestSamp <= DragSamp)
						DragSamp++;
					MADPurgeTrackIfInstru(MADDriver, DragIns);
					MADKillSample(curMusic, DragIns, DragSamp);
				}
			} else {						// Remplacement d'un sample
				sData		*srcData, *dstData;
				
				SaveUndo(UAllSamples, 0, "\pUndo 'Replace sample'");
				
				srcData = curMusic->sample[ curMusic->fid[ DragIns].firstSample +  DragSamp];
				dstData = curMusic->sample[ curMusic->fid[ DestIns].firstSample +  DestSamp];
				
				if (dstData == NULL) {
					MyDebugStr(__LINE__, __FILE__, "Arggggggg");
					return cantGetFlavorErr;
				}
				if (dstData->data != NULL) { DisposePtr(dstData->data);	dstData->data = NULL;}
				
				*dstData = *srcData;
				
				dstData->data = NewPtr(dstData->size);
				if (dstData->data != NULL) {
					BlockMoveData(srcData->data, dstData->data, dstData->size);
				} else {
					Erreur(63, MemError());
					MADPurgeTrackIfInstru(MADDriver, DestIns);
					MADKillSample(curMusic, DestIns, DestSamp);
					return dragNotAcceptedErr;
				}
				
				if (moveInstru) {
					//if (DestIns == DragIns && DestSamp <= DragSamp) DragSamp++;
					MADPurgeTrackIfInstru(MADDriver, DragIns);
					MADKillSample(curMusic, DragIns, DragSamp);
				}
			}
		}
		
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		
		NSelectInstruList(DestIns, DestSamp);
		
		return noErr;
	}
	
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, kDragFlavorTypeHFS, &textSize);
	
	if (result == noErr) {
		Boolean		targetIsFolder, wasAliased;
		OSErr		iErr;
		
		SaveUndo(UAllSamples, 0, "\pUndo 'Drop sound file'");
		
		if (gMovieQuicktime) {
			FSSpec			tempFile;
			Movie			aMovie;
			TimeValue		selectionTime, selectionDuration;
			
			GetMovieSelection(QTMovie, &selectionTime, &selectionDuration);
			
			if (selectionDuration == 0)
				aMovie = QTMovie;
			else
				aMovie = CopyMovieSelection(QTMovie);
			
			pStrcpy(tempFile.name, QTFile.name);
			iErr = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &tempFile.vRefNum, &tempFile.parID);
			if (iErr) MyDebugStr(__LINE__, __FILE__, "FindFolder");
			
			FSpDelete(&tempFile);
			
			iErr = ConvertMovieToFile(aMovie, nil, &tempFile, 'WAVE', 'TVOD', 0, nil, 0, nil);
			if (iErr) MyDebugStr(__LINE__, __FILE__, "ConvertMovieToFile");
			
			iErr = NOpenSampleInt(DestIns, DestSamp, tempFile);
			if (iErr) MyDebugStr(__LINE__, __FILE__, "NOpenSampleInt");
			
			FSpDelete(&tempFile);
			
			if (selectionDuration != 0)
				DisposeMovie(aMovie);
		} else {
			GetFlavorData(theDrag, theItem, kDragFlavorTypeHFS, &myFlavor, &textSize, 0);
			
			ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
			
			iErr = NOpenSampleInt(DestIns, DestSamp, myFlavor.fileSpec);
			MADErreur(iErr);
		}
		
		
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		
		NSelectInstruList(DestIns, DestSamp);
		
		return iErr;
	}
	
	//	Un drag de type 'PCMD'
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, 'Pcmd', &textSize);
	
	if (result == noErr) {
		FSSpec	newFile;
		short	fRefNum;
		OSErr	iErr;
		
		pStrcpy(newFile.name, "\pDigital Selection");
		iErr = FindFolder(kOnSystemDisk, kDesktopFolderType, kCreateFolder, &newFile.vRefNum, &newFile.parID);
		if (iErr) MyDebugStr(__LINE__, __FILE__, "FindFolder");
		
		FSpDelete(&newFile);
		
		iErr = FSpCreate(&newFile, 'TVOD', 'AIFF', smSystemScript);
		iErr = FSpOpenDF(&newFile, fsCurPerm, &fRefNum);
		
		if (CreateAIFFExporting(true, fRefNum, &newFile, 'AIFF', NULL)) {
			while (theProgressDia != NULL)
				DoAIFFExporting();
			
			/////////////////
			
			curMusic->hasChanged = true;
			
			UpdateALLWindow();
			
			SaveUndo(UAllSamples, 0, "\pUndo 'Create Sample from Digital Selection'");
			
			iErr = NOpenSampleInt(DestIns, DestSamp, newFile);
			
			if (iErr == -10)
				MADErreur(iErr);
			
			CreateInstruList();
			DrawInfoInstrument();
			UpdateSampleWindows();
			UpdateInstruMenu();
			
			NSelectInstruList(DestIns, DestSamp);
		}
		SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		FSpDelete(&newFile);
		
		return noErr;
	}
	
	//	Un drag de type 'snd '
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, 'snd ', &textSize);
	
	if (result == noErr) {
		Ptr				theSound;
		long			lS, lE, bFreq;
		short			sS;
		unsigned long	rate;
		Boolean			stereo;
		
		SaveUndo(UAllSamples, 0, "\pUndo 'Drop sound'");
		
		//////////////// System 7.c ////////////////////
		
		theSound = NewPtr(textSize);
		
		if (theSound != NULL) {
			GetFlavorData(theDrag, theItem, 'snd ', theSound, &textSize, 0);
			
			theSound = NSndToPtr(theSound, &lS, &lE, &sS, &rate, &bFreq, &stereo);
			
			if (theSound != NULL)
				AddSoundToMAD(theSound,
							  lS,
							  lE,
							  sS,
							  bFreq,
							  rate,
							  stereo,
							  "\p",
							  DestIns,
							  &DestSamp);
		}
		
		//////////////////////////////////////
		
		result = GetFlavorDataSize(theDrag, theItem, 'STR ', &textSize);
		if (result == noErr) {
			GetFlavorData(theDrag, theItem, 'STR ', &myStr, &textSize, 0);
			SetSampName(DestIns, DestSamp, myStr);
		}
		
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		
		NSelectInstruList(DestIns, DestSamp);
		
		return noErr;
	}
	
	return dragNotAcceptedErr;
}

pascal OSErr MySendDataProc(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag)
{
	AEDesc			dropLoc;
	FSSpec			target;
	OSErr			err;
	Boolean			wasChanged;
	short			fRefNum, beginNumber;
	Str255			sName, tempStr;
	WDPBRec			wdpb;
	OSType			SoundType;
	
	if (theFlavor == 'VCT3' || theFlavor == 'VCT4') {
		err = GetDropLocation(theDrag, &dropLoc);
		if (err)
			return err;
		
		err = ResolveAlias(nil, (AliasHandle) dropLoc.dataHandle, &target, &wasChanged);
		if (err)
			return err;
		
		AEDisposeDesc(&dropLoc);
		
		PathNameFromDirIDTRUE(target.parID,
									target.vRefNum,
									sName);
		
		pStrcat(sName, "\p:");
		pStrcat(sName, target.name);
		pStrcat(sName, "\p:");
		
		wdpb.ioNamePtr = sName;
		//PBHSetVol(&wdpb, false);
		PBHSetVolSync(&wdpb);
		HGetVol(NULL, &target.vRefNum, &target.parID);
		
		if (NSoundQualityExportSnd(DragIns, DragSamp, &SoundType, sName) == noErr) {
			FSMakeFSSpec(target.vRefNum, target.parID, sName, &target);
			beginNumber = 1;
		
			ReEXPORT:
			
		//HSetVol(NULL, target.vRefNum, target.parID);
			err = FSpOpenDF(&target, fsCurPerm, &fRefNum);
			if (err == noErr) {
				FSCloseFork(fRefNum);
				beginNumber++;
				
				pStrcpy(target.name, sName);
				NumToString(beginNumber, tempStr);
				if (target.name[ 0] > 27) target.name[ 0] = 27;
				pStrcat(target.name, "\p #");
				pStrcat(target.name, tempStr);
				
				if (beginNumber > 90) {
					Erreur(63, -754);
					return dragNotAcceptedErr;
				}
				
				goto ReEXPORT;
			} else if (err == fnfErr) {
				NSaveSampleInt(DragIns, DragSamp, SoundType, &target);
				
				err = SetDragItemFlavorData(theDrag, theItem, theFlavor, &target, sizeof(target), 0);
				if (err)
					return (err);
			} else {
				Erreur(63, err);
				return dragNotAcceptedErr;
			}
		} else
			return dragNotAcceptedErr;
		
		return noErr;
	} else
		return dragNotAcceptedErr;
}

Boolean DragInstrument(RgnHandle myRgn, short no, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	AEDesc				dropLocation;
	DragAttributes		attributes;
	short				mouseDownModifiers, mouseUpModifiers, copyText;
	long				inOutBytes;
	short				temp;
	Handle				theSound = NULL;
	Str255				theStr;
	PromiseHFSFlavor	myNewFile;
	Rect				dragRegionRect;
	Pcmd				*myPcmd;
	
	if (!DragManagerUse)
		return false;

	//******************************************
	// Prepare l'instrument pour etre exporte !!
	//******************************************
	
	ConvertIDtoInsSamp(no, &DragIns, &DragSamp);
	
	if (DragSamp >= 0) {
		sData				*curData = curMusic->sample[ curMusic->fid[ DragIns].firstSample +  DragSamp];
		short				numChan;
		
		if (curData == NULL) {
			MyDebugStr(__LINE__, __FILE__, "Errorrr");
			return false;
		}
		if (curData->data == NULL) {
			MyDebugStr(__LINE__, __FILE__, "Errorrr");
			return false;
		}
		
		if (curData->size == 0)
			return false;
		
		theSound = NewHandle(4096);
		if (theSound == NULL)
			return false;
		
		inOutBytes = GetPtrSize(curData->data);
		if (inOutBytes != curData->size)
			return false;
		
		if (curData->stereo)
			numChan = 2;
		else
			numChan = 1;
		
		SetupSndHeader((SndListHandle) theSound,
							numChan,
							((unsigned long) curData->c2spd)<<16L,
							curData->amp,
							'NONE',
							60 - curData->relNote,
							inOutBytes,
							&temp);
		
		SetHandleSize(theSound, inOutBytes + temp);
		if (MemError() != noErr) {
			DisposeHandle(theSound);
			Erreur(63, MemError());
			return false;
		}
		
		AddLoopToSndHandle(theSound,
							curData->loopBeg,
							curData->loopBeg + curData->loopSize);
		
		HLock(theSound);
		BlockMoveData(curData->data, *theSound + temp, inOutBytes);
		if (curData->amp == 8)
			ConvertInstrumentIn((Byte*)(*theSound + temp), inOutBytes);
		else
			ConvertInstrumentIn16((short*)(*theSound + temp), inOutBytes);
	}
	//******************************************
	//******************************************
	
	dragRegion = NewRgn();
	CopyRgn(myRgn, dragRegion);
	SetPt(&theLoc, 0, 0);
	LocalToGlobal(&theLoc);
	OffsetRgn(dragRegion, theLoc.h, theLoc.v);

	NewDrag(&theDrag);
	
	if (DragSamp >= 0) {	// Drag d'un sample
		sData	*curData = curData = curMusic->sample[ curMusic->fid[ DragIns].firstSample +  DragSamp];
	
		myNewFile.fileType			=	'snd ';
		myNewFile.fileCreator		=	'SNPL';
		myNewFile.fdFlags			=	0;
		myNewFile.promisedFlavor	=	'VCT3';
		AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
		AddDragItemFlavor(theDrag, 1, 'VCT3', NULL, 0, 0);
		
		AddDragItemFlavor(theDrag, 1, 'snd ', *theSound, GetHandleSize(theSound), 0);
		
		strcpy((Ptr)theStr, curData->name);
		MyC2PStr((Ptr)theStr);
		AddDragItemFlavor(theDrag, 1, 'STR ', &theStr, sizeof(theStr), 0);
	} else {				// Drag d'un instrument
		myNewFile.fileType			=	'MINo';
		myNewFile.fileCreator		=	'SNPL';
		myNewFile.fdFlags			=	0;
		myNewFile.promisedFlavor	=	'VCT4';
		AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
		AddDragItemFlavor(theDrag, 1, 'VCT4', NULL, 0, 0);
	}
	
	
	// Add the Pcmd thing...
	Cmd	*cmd;
	
	myPcmd = (Pcmd*) NewPtrClear(sizeof(Pcmd) + 1 * sizeof(Cmd));
	myPcmd->structSize 	= sizeof(Pcmd) + 1 * sizeof(Cmd);
	myPcmd->tracks 		= 1;
	myPcmd->length 		= 1;
	myPcmd->trackStart 	= 1;
	myPcmd->posStart 	= 1;
	cmd = GetCmd(0, 0, myPcmd);
	MADKillCmd(cmd);
	cmd->ins	=	DragIns + 1;
	
	if (curMusic->fid[ DragIns].numSamples > 0) {
		cmd->note	=	48 - curMusic->sample[ curMusic->fid[ DragIns].firstSample]->relNote;
	} else {
		cmd->note	=	48;
	}
	AddDragItemFlavor(theDrag, 1, 'Pcmd', myPcmd, myPcmd->structSize, 0);
	
	result = SetDragSendProc(theDrag, mySendDataUPP, NULL);
	
	SetDragItemBounds(theDrag, 1, GetRegionBounds(dragRegion, &dragRegionRect));
	
	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);
	
	result = TrackDrag(theDrag, theEvent, dragRegion);
	
	if (result != noErr && result != userCanceledErr) {
		return true;
	}
	
	//
	//	Check to see if the drop occurred in the Finder's Trash. If the drop occurred
	//	in the Finder's Trash and a copy operation wasn't specified, delete the
	//	source selection. Note that we can continute to get the attributes, drop location
	//	modifiers, etc. of the drag until we dispose of it using DisposeDrag.
	//
	
	GetDragAttributes(theDrag, &attributes);
	if (!(attributes &kDragInsideSenderApplication)) {
		GetDropLocation(theDrag, &dropLocation);

		GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
		copyText = (mouseDownModifiers | mouseUpModifiers) & optionKey;
		
		if ((!copyText) && (DropLocationIsFinderTrash(&dropLocation))) {
		} else {
		}

		AEDisposeDesc(&dropLocation);
	}

	//
	//	Dispose of the drag.
	//

	DisposeDrag(theDrag);
	DisposeRgn(dragRegion);
	
	if (DragSamp >= 0) {
		HUnlock(theSound);
		DisposeHandle(theSound);
		
		DisposePtr((Ptr)myPcmd);
	}
	
	return true;
}

void CreateInstruListWindow(void)
{
	Rect		caRect, itemRect;
	Handle		itemHandle;
	short		itemType;
	FontInfo	ThisFontInfo;
	PicHandle	myPict;
	
	if (InstruListDlog != NULL) {
		SetWindEtat(GetDialogWindow(InstruListDlog));
		return;
	}
	
	SAButState = (Boolean*)NewPtrClear(sizeof(Boolean) * 32000);
	InstrUsed = (Boolean*)NewPtrClear(sizeof(Boolean) * 32000);
	
	InstruListDlog = GetNewDialog(140, NULL, GetDialogWindow(ToolsDlog));
	
	SetWindEtat(GetDialogWindow(InstruListDlog));
	GetPortBounds(GetDialogPort(InstruListDlog), &caRect);
	
	if (caRect.right != 200)
		MySizeWindow(InstruListDlog, 200, caRect.bottom, true);
	
	SetPortDialogPort(InstruListDlog);
	
	TextFont(kFontIDGeneva);
	TextSize(9);
	
	/****************/
	
	GetDialogItem(InstruListDlog, 1, &itemType, &itemHandle, &myList.rect);
	myList.rect.right -= 15;
	myList.rect.left = 0;
	
	SetRect(&itemRect, 0, 0, 30, 16);
	myList.yScroll = NewControl(GetDialogWindow(InstruListDlog),
								&itemRect,
								"\p.",
								true,
								0,
								0,
								0,
								gScrollBarID,
								0);
	
	myList.xScroll = NULL;
	myList.maxX = 1;
	myList.maxY = MAXINSTRU;
	myList.aDia = InstruListDlog;
	myList.type = dragItem + onlyOne;
	
	GetFontInfo(&ThisFontInfo);
	myList.HCell = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading + 1;
	myList.LCell = myList.rect.right - myList.rect.left;
	
	SetRect(&myList.select, 0, 0, 0, 0);
	
	/****************/
	
	CreateInstruList();
	
	theEvent.what = 0;
	DoGrowInstruList(InstruListDlog);
	SelectWindow2(GetDialogWindow(InstruListDlog));
	ShowWindow(GetDialogWindow(InstruListDlog));
	
	ActiveInstrumentMenu(true);
	
	GetDialogItem(InstruListDlog , 18, &itemType, &itemHandle, &itemRect);
	PlayBut = NewControl(GetDialogWindow(InstruListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 160,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(InstruListDlog , 20, &itemType, &itemHandle, &itemRect);
	LoadBut = NewControl(GetDialogWindow(InstruListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 152,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(InstruListDlog , 19, &itemType, &itemHandle, &itemRect);
	DelBut = NewControl(GetDialogWindow(InstruListDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						150,
						kControlBevelButtonNormalBevelProc,
						0);
	GetDialogItem(InstruListDlog , 21, &itemType, &itemHandle, &itemRect);
	SaveBut = NewControl(GetDialogWindow(InstruListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 151,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(InstruListDlog , HRec, &itemType, &itemHandle, &itemRect);
	RecBut = NewControl(GetDialogWindow(InstruListDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						183,
						kControlBevelButtonNormalBevelProc,
						0);
	GetDialogItem(InstruListDlog , 23, &itemType, &itemHandle, &itemRect);
	OpenBut = NewControl(GetDialogWindow(InstruListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 159,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(InstruListDlog , 24, &itemType, &itemHandle, &itemRect);
	FlipBut = NewControl(GetDialogWindow(InstruListDlog),
						 &itemRect,
						 "\p",
						 true,
						 1,
						 0,
						 1,
						 kControlTriangleProc,
						 0);
	
	DrawInfoInstrument();
	
	GetPortBounds(GetDialogPort(InstruListDlog), &caRect);
	
	SetMaxWindow(caRect.right + 3, 0, InstruListDlog);
	
	myPict = GetPicture(142);
	PicToPix(myPict, &ArrowPix);
	ReleaseResource((Handle) myPict);
	
	myPict = GetPicture(143);
	PicToPix(myPict, &mySABut);
	ReleaseResource((Handle) myPict);
	
	myPict = GetPicture(131);
	PicToPix(myPict, &CP);
	ReleaseResource((Handle) myPict);
	
	myPict = GetPicture(144);
	PicToPix(myPict, &OP);
	ReleaseResource((Handle) myPict);
	
	myPict = GetPicture(146);
	PicToPix(myPict, &OPB);
	ReleaseResource((Handle) myPict);
	
	myPict = GetPicture(147);
	PicToPix(myPict, &CPB);
	ReleaseResource((Handle) myPict);
	
	SetRect(&ButtonRect, myList.rect.left, myList.rect.top, myList.rect.left + (*mySABut)->bounds.right, myList.rect.bottom);
	
	if (DragManagerUse) {
		
		MyTrackingHandlerUPP 		= NewDragTrackingHandlerUPP(MyTrackingHandler);
		MyReceiveDropHandlerUPP 	= NewDragReceiveHandlerUPP(MyReceiveDropHandler);
		mySendDataUPP 				= NewDragSendDataUPP(MySendDataProc);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingHandlerUPP, GetDialogWindow(InstruListDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveDropHandlerUPP, GetDialogWindow(InstruListDlog), (void *) NULL);
	}
}

void CloseInstruListWindow(void)
{
	if (InstruListDlog != NULL) {
		DisposePtr((Ptr)SAButState);
		DisposePtr((Ptr)InstrUsed);

		ZapPixMap(&ArrowPix);
		ZapPixMap(&mySABut);
		ZapPixMap(&CP);
		ZapPixMap(&OP);
		ZapPixMap(&CPB);
		ZapPixMap(&OPB);
	
		if (DragManagerUse) {
			RemoveTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(InstruListDlog));
			RemoveReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(InstruListDlog));
			
			DisposeDragTrackingHandlerUPP(MyTrackingHandlerUPP);
			DisposeDragReceiveHandlerUPP(MyReceiveDropHandlerUPP);
			DisposeDragSendDataUPP(mySendDataUPP);
		}
		DisposeDialog(InstruListDlog);
	}
	InstruListDlog = NULL;
	
	ActiveInstrumentMenu(false);
}

Boolean GetIns(short *ins, short *samp)
{
	Point	theCell;
	
	if (InstruListDlog == NULL)
		return false;
	
	theCell.h = 0;
	//theCell.v = *ins;
	theCell.v = 0;
	
	
	if (PLGetSelect(&theCell, &myList)) {
		ConvertIDtoInsSamp(theCell.v, ins, samp);
		return true;
	}
	return false;
}

extern EventRecord theEvent;

void DoItemPressInstruList(short whichItem, DialogPtr whichDialog)
{
	Cell		theCell;
	short		temp;
	int			i;
	Point		myPt;
	Boolean		DoubleClick;
	Rect		itemRect;
	GrafPtr		SavePort;
		
 	GetPort(&SavePort);
 	SetPortDialogPort(InstruListDlog);

	if (theEvent.what == mouseDown) {
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		PLScroll(myPt, &myList);
		
		itemRect = myList.rect;
		itemRect.left = 0;
		itemRect.right = (*mySABut)->bounds.right;
		if (PtInRect(myPt, &itemRect)) {
			short	ins, samp;
			Boolean	inBB;
			
			theCell = myPt;
			PLConvertPoint(&theCell, &myList);
			
			ConvertIDtoInsSamp(theCell.v, &ins, &samp);
			
			if (curMusic->fid[ ins].numSamples > 0 && samp < 0) {
				itemRect.top = myList.rect.top + (theCell.v - PLGetCtlValue(myList.yScroll)) * myList.HCell;
				itemRect.bottom = itemRect.top + (*OP)->bounds.bottom;
				
				inBB = false;
				do {
					GetMouse(&myPt);
					if (PtInRect(myPt, &itemRect)) {
						if (!inBB) {
							inBB = true;
							// Paint IN
							
							if (thePrefs.OCArrow[ins]) {
							itemRect.bottom = itemRect.top + (*OPB)->bounds.bottom;
							
							CopyBits((BitMap*) *(OPB),
										(BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
										&(*OPB)->bounds,
										&itemRect,
										srcCopy,
										NULL);
							} else {
							itemRect.bottom = itemRect.top + (*CPB)->bounds.bottom;
							
							CopyBits((BitMap*) *(CPB),
										(BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
										&(*CPB)->bounds,
										&itemRect,
										srcCopy,
										NULL);
							}
						}
					} else {
						if (inBB) {
							inBB = false;
							// Paint OUT
							
							if (thePrefs.OCArrow[ ins]) {
							itemRect.bottom = itemRect.top + (*OP)->bounds.bottom;
							
							CopyBits((BitMap*) *(OP),
										(BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
										&(*OP)->bounds,
										&itemRect,
										srcCopy,
										NULL);
							} else {
							itemRect.bottom = itemRect.top + (*CP)->bounds.bottom;
							
							CopyBits((BitMap*) *(CP),
										(BitMap*) *GetPortPixMap(GetDialogPort(InstruListDlog)),
										&(*CP)->bounds,
										&itemRect,
										srcCopy,
										NULL);
							}
						}
					}
				} while (Button());
				
				if (inBB) {
					if (samp < 0) {
						thePrefs.OCArrow[ ins] = !thePrefs.OCArrow[ins];
						
						if ((theEvent.modifiers & optionKey) != 0) {
							for (i = 0; i < MAXINSTRU ; i++) thePrefs.OCArrow[i] = thePrefs.OCArrow[ins];
						}
						
						theCell.v = ConvertInsSampToID(ins, samp);
						theCell.h = 0;
						PLSetSelect(theCell.h, theCell.v, theCell.h, theCell.v, &myList);
						
						CreateInstruList();
						DrawInfoInstrument();
					}
				}
			} else if (curMusic->fid[ ins].numSamples > 0 && samp >= 0) {
				short 	track = GetWhichTrackPlay();
				Channel	*curVoice = &MADDriver->chan[ track];
				
				DoPlayInstruInt2(curMusic->sample[ curMusic->fid[ ins].firstSample + samp], &curMusic->fid[ ins], 48 - curMusic->sample[ curMusic->fid[ ins].firstSample + samp]->relNote, ins, 0, 0, 0xFF, curVoice, 0, 0, samp);
				
				while (Button()) {
					DoGlobalNull();
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				}
				MADDriver->chan[track].loopBeg = 0;
				MADDriver->chan[track].loopSize = 0;
				MADDriver->chan[track].maxPtr = MADDriver->chan[track].curPtr;
			}
		} else if (PtInRect(myPt, &myList.rect)) {
			firstCall = true;
			
			DoubleClick = PLClick(myPt, theEvent.modifiers, &myList);
			if (DoubleClick) {
				theCell.v = 0;	theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					short	ins, samp;
					
					ConvertIDtoInsSamp(theCell.v, &ins, &samp);
					
					if (samp < 0) {
						if (GetIns(&temp, &samp))
							NEditInstruInfo(temp, samp);
					} else
						NCreateSampleWindow(ins, samp);
				}
			} else {
				PLGetSelectRect(&itemRect, &myList);
				if (PtInRect(myPt, &itemRect)) {
					DragInstruSelect();
				}
			}
			
			DrawInfoInstrument();
			whichItem = -1;
		}
	}

	switch (whichItem) {
		case 18:
			if (GetControlHilite(PlayBut) == 0) {// && MyTrackControl(PlayBut, theEvent.where, NULL))
				HiliteControl(PlayBut, kControlButtonPart);
			
				theCell.v = 0;	theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					short	ins, samp;
				
					ConvertIDtoInsSamp(theCell.v, &ins, &samp);
					
					if (curMusic->fid[ ins].numSamples > 0 && samp >= 0) {
						short 	track = GetWhichTrackPlay();
						Channel	*curVoice = &MADDriver->chan[ track];
						
						DoPlayInstruInt2(curMusic->sample[ curMusic->fid[ ins].firstSample + samp], &curMusic->fid[ ins], 48 - curMusic->sample[ curMusic->fid[ ins].firstSample + samp]->relNote, ins, 0, 0, 0xFF, curVoice, 0, 0, samp);
						
						while (Button()) {
							DoGlobalNull();
							WaitNextEvent(everyEvent, &theEvent, 1, NULL);
						}
						MADDriver->chan[track].loopBeg = 0;
						MADDriver->chan[track].loopSize = 0;
						MADDriver->chan[track].maxPtr = MADDriver->chan[ track].curPtr;
					} else {
						short 	track = GetWhichTrackPlay();
						Channel	*curVoice = &MADDriver->chan[ track];
						
						DoPlayInstruInt(48, ins, 0, 0, 0xFF, curVoice, 0, 0);
						
						while (Button()) {
							WaitNextEvent(everyEvent, &theEvent, 1, NULL);
							DoGlobalNull();
						}
						MADDriver->chan[ track].loopBeg = 0;
						MADDriver->chan[ track].loopSize = 0;
						MADDriver->chan[ track].maxPtr = MADDriver->chan[ track].curPtr;
					}
				}
				
				HiliteControl(PlayBut, 0);
			}
			break;

		case 19:
			if (GetControlHilite(DelBut) == 0 && MyTrackControl(DelBut, theEvent.where, NULL)) {
				DoKeyPressInstruList(0x08, -1);
			}
			break;

		case 20:
			if (GetControlHilite(LoadBut) == 0 && MyTrackControl(LoadBut, theEvent.where, NULL)) {
				HandleNewSound(1);
			}
			break;

		case 21:
			if (GetControlHilite(SaveBut) == 0  && MyTrackControl(SaveBut, theEvent.where, NULL)) {
				HandleInstruChoice(4);
			}
			break;
		
		case HRec:
			//if (GetControlHilite(RecBut) == 0)
			{
				Point		Zone;
				long		mresult;
				Rect		contrlRect;
				
				//HiliteControl(RecBut, kControlButtonPart);
				
				GetControlBounds(RecBut, &contrlRect);
				
				Zone.h = contrlRect.right - 13;
				Zone.v = contrlRect.top;
				
				LocalToGlobal(&Zone);
				
				mresult = PopUpMenuSelect(NewSoundMenu,
											Zone.v,
											Zone.h,
											0 );
				
				if (HiWord(mresult ) != 0 ) {
					curMusic->hasChanged = true;
					
					HandleNewSound(LoWord(mresult));
				}
				
				//HiliteControl(RecBut, 0);
			}
			break;
			
		case 23:
			if (GetControlHilite(OpenBut) == 0 && MyTrackControl(OpenBut, theEvent.where, NULL)) {
				theCell.v = 0;	theCell.h = 0;
				if (PLGetSelect(&theCell, &myList)) {
					short	ins, samp;
				
					ConvertIDtoInsSamp(theCell.v, &ins, &samp);
				
					if (samp < 0) {
						if (GetIns(&temp, &samp))
							NEditInstruInfo(temp, samp);
					} else
						NCreateSampleWindow(ins, samp);
				}
			}
			break;
			
		case 24:
			if (GetControlHilite(FlipBut) == 0  && MyTrackControl(FlipBut, theEvent.where, NULL)) {
				Rect	caRect;
				
				EraseGrowIcon(whichDialog);
				
				GetPortBounds(GetDialogPort(whichDialog), &caRect);
				
				if (caRect.right == 300) {
					MySizeWindow(whichDialog, 200, caRect.bottom, true);
				//	SetControlValue(FlipBut, 161);
				} else {
					MySizeWindow(whichDialog, 300, caRect.bottom, true);
				//	SetControlValue(FlipBut, 162);
				}
				
				GetPortBounds(GetDialogPort(whichDialog), &caRect);
				
				EraseGrowIcon(whichDialog);
				SetMaxWindow(caRect.right + 3, 0, whichDialog);
				
				if (caRect.right == 300)
					SetControlValue(FlipBut, 0);
				else
					SetControlValue(FlipBut, 1);
			}
			break;
	}
	
	SetPort(SavePort);
}

void DoKeyPressInstruList(short theChar, short xxxxxxx)
{
	GrafPtr		SavePort;
	short		ins, samp;
	Point		theCell = { 0, 0};

	if (InstruListDlog == NULL) return;
	
	GetPort(&SavePort);
	SetPortDialogPort(InstruListDlog);

	//BackColor(whiteColor);

	PLDoArrows(theChar, &myList);
	
	if (theChar == deletekey) {
		curMusic->hasChanged = true;

		SaveUndo(UAllSamples, 0, "\pUndo 'Delete instrument'");
		
		if (GetIns(&ins, &samp)) {
			MADPurgeTrackIfInstru(MADDriver, ins);
			
			if (samp >= 0)
				MADKillSample(curMusic, ins, samp);
			else
				MADKillInstrument(curMusic, ins);
		}
		
		CreateInstruList();
		UpdateSampleWindows();
	} else if (theChar == 0x0D || theChar == 0x03) {
		theCell.v = 0;
		theCell.h = 0;
		if (PLGetSelect(&theCell, &myList))
		{
			short	ins, samp, temp;
			
			ConvertIDtoInsSamp(theCell.v, &ins, &samp);
			
			if (samp < 0) {
				if (GetIns(&temp, &samp)) NEditInstruInfo(temp, samp);
			}
			else NCreateSampleWindow(ins, samp);
		}
	}
	else if (theChar == selectAll) {
#if 0
		theCell.v = 0;
		theCell.h = 0;
		do {
			LSetSelect(true, theCell, InstruList);
		} while (LNextCell(true, true, &theCell, InstruList));
#endif
	} else if (theChar == getinfo) {
		short temp;
		
		if (GetIns(&temp, &samp))
			NEditInstruInfo(temp, samp);
	} else if (theChar == '/' || theChar == '*') {
		short 	temp;
		
		if (GetIns(&temp, &samp)) {
			if (samp >= 0) {
				sData	*curData;
				
				curData = curMusic->sample[ curMusic->fid[ temp].firstSample +  samp];
				
				if (theChar == '/') {
					if (curData->c2spd > 5)
						curData->c2spd -= 5;
						
					curMusic->hasChanged = true;
				} else {
					if (curData->c2spd < 60000)
						curData->c2spd += 5;
						
					curMusic->hasChanged = true;
				}
			}
		}
	}
	
//	RGBBackColor(&theColor);
	
	theCell.v = 0;	theCell.h = 0;
	if (PLGetSelect(&theCell, &myList))
		DrawInfoInstrument();
	else
		EraseInstrumentInfo();
	
	SetPort(SavePort);
}

void PASTEInstruList()
{
	if (GetIns(&DestIns, &DestSamp)) {
		if (DestSamp < 0) {			// Rajout d'un sample à un instrument
			SaveUndo(UAllSamples, 0, "\pUndo 'Paste sample'");
			
			NPASTESample(0, DestIns, DestSamp);
		} else {						// Remplacement d'un sample
			sData		*dstData;
			
			SaveUndo(UAllSamples, 0, "\pUndo 'Paste sample'");
			
			MADPurgeTrackIfInstru(MADDriver, DestIns);
			
		//MADKillSample(curMusic, DestIns, DestSamp);
						
			// Delete old sample
			if (curMusic->sample[DestIns * MAXSAMPLE + DestSamp]->data)
				DisposePtr(curMusic->sample[DestIns * MAXSAMPLE + DestSamp]->data);
			DisposePtr((Ptr)curMusic->sample[DestIns * MAXSAMPLE + DestSamp]);
			curMusic->sample[DestIns * MAXSAMPLE + DestSamp] = NULL;
			
			// Create the new sample
			dstData = (sData*) NewPtrClear(sizeof(sData));
			dstData->size		= 0;
			dstData->loopBeg	= 0;
			dstData->loopSize	= 0;
			dstData->vol		= MAX_VOLUME;
			dstData->c2spd		= NOFINETUNE;
			dstData->loopType	= eClassicLoop;
			dstData->amp		= 8;
			dstData->relNote	= 0;
			// dstData->name
			dstData->data		= NULL;

			curMusic->sample[DestIns * MAXSAMPLE + DestSamp] = dstData;
			
			dstData->data = NewPtr(0);
			if (dstData->data) {
				NPASTESample(0, DestIns, DestSamp);
			}
		}
	}
}

static	short ILindex = 1;

void SaveInstrumentsList()
{
	FSSpec	spec;
	Str255	str, str2;
	OSErr	iErr;

	NumToString(ILindex, str2);

	pStrcpy(str, "\pInstruments List #");
	pStrcat(str, str2);
	
	if (DoCustomSave("\pSave this instruments list as:", str, 'INso', &spec))
		return;
	
	ILindex++;
	
	//////////////////////////////////////////////////////////////
	
	{
		short	refNum, i, x;
		long	inOutCount;
	
		iErr = FSpDelete(&spec);
		iErr = FSpCreate(&spec, 'SNPL', 'INso', smSystemScript);
		iErr = FSpOpenDF(&spec, fsCurPerm, &refNum);
		
		if (iErr == noErr) {
			inOutCount = sizeof(InstrData) * MAXINSTRU;
			iErr = FSWriteFork(refNum, fsAtMark, 0, inOutCount, curMusic->fid, NULL);
			
			for (i = 0; i < MAXINSTRU ; i++) {
				for (x = 0; x < curMusic->fid[i].numSamples; x++) {
					//TODO: write byteswapped data.
					sData	*curData;
					
					curData = curMusic->sample[curMusic->fid[i].firstSample + x];
					
					inOutCount = sizeof(sData);
					iErr = FSWriteFork(refNum, fsAtMark, 0, inOutCount, curData, NULL);
					
					inOutCount = curData->size;
					iErr = FSWriteFork(refNum, fsAtMark, 0, inOutCount, curData->data, NULL);
				}
			}
		}
		iErr = FSCloseFork(refNum);
	}
}

OSErr OpenInstrumentsList(FSSpec *file)
{
	short		x, i, srcFile;
	long		inOutCount;
	OSErr		theErr;
	
	SaveUndo(UAllSamples, 0, "\pUndo 'Open Instruments List file'");
	
	theErr = FSpOpenDF(file, fsCurPerm, &srcFile);
	if (theErr) return theErr;
	
	for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(curMusic, x);
	
	// **** HEADER ***
	inOutCount = sizeof(InstrData) * MAXINSTRU;
	theErr = FSRead(srcFile, &inOutCount, curMusic->fid);
	
	// **** INSTRUMENTS ***
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < curMusic->fid[i].numSamples ; x++) {
			sData	*curData;
		
			// ** Read Sample header **
		
			curData = curMusic->sample[i * MAXSAMPLE + x] = (sData*)MADNewPtr(sizeof(sData), MADDriver->lib);
			if (curData == NULL) return MADNeedMemory;
			//TODO: read byte-swapped data
			inOutCount = sizeof(sData);
			theErr = FSRead(srcFile, &inOutCount, curData);
			
			// ** Read Sample DATA
			
			curData->data = MADNewPtr(curData->size, MADDriver->lib);
			if (curData->data == NULL) return MADNeedMemory;
			
			inOutCount = curData->size;
			theErr = FSRead(srcFile, &inOutCount, curData->data);
		}
	}
	// *********************
	
	theErr = FSCloseFork(srcFile);
	
	return noErr;
}
