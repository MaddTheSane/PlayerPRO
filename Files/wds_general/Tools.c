#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <CoreMIDI/CoreMIDI.h>

/******** HELP MODULE FOR TOOLS ********/
enum
{
	HBackP	= 2,
	HBack	= 3,
	HStop	= 4,
	HPlay	= 5,
	HFor	= 6,
	HForP	= 7,
	HIndi	= 10,
	HName	= 1,
	HIns	= 31,
	HNote	= 32,
	HEff	= 33,
	HArg	= 34,
	HCellP	= 22,
	HDel	= 21,
	HSel	= 40,
	HLoop	= 46
};
extern Boolean MIDIHardwareAlreadyOpen;

static long *TimeScanPtr[256];
static short AHelp[] = {HBackP, HBack, HStop, HPlay, HFor, HForP, HIndi, HName, HIns,
	HNote, HEff, HArg, HCellP, HDel, HSel, HLoop, 44};

void DoHelpTools(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/

extern	long					TickInterne;
extern	RGBColor				theColor;
extern	Boolean					DragManagerUse;
		
		Boolean					PianoRecording;
		
static	short					oldPartition, oldPartition2, oldPL;
static	long					oldStart, oldEnd, maxTime, oldTime;
static	ControlHandle			playCntl, progCntl, stopCntl, RecordCntl, BackCntl, ForCntl, JumpNextCntl, JumpBeforeCntl, LoopCntl;
static	Boolean					canAcceptDrag;
static	short					RememberPat, RememberReader, RememberPL;
static	PixPatHandle			workPixPat;
static	short					LoopCntlState, PreviousLoop;
static	ControlActionUPP		ForeUPP, BackUPP;

void FlushPlugin(void);
void ScanTime();
Boolean IsMyTypeMODList(DragReference theDrag);
void PurgeVSTEffects(void);
DragTrackingHandlerUPP	MyTrackingToolsUPP;
DragReceiveHandlerUPP	MyReceiveToolsUPP;
pascal void myForeAction(ControlHandle theCntl, short ctlPart);
pascal void myBackAction(ControlHandle theCntl, short ctlPart);
void SetCurrentMOD(Str255 theMODName);


pascal OSErr MyTrackingTools(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	short				result, offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect, caRect;

	if (!mainSystemDrag) return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return(noErr);

	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {

		case kDragTrackingEnterHandler:
			canAcceptDrag = IsMyTypeMODList(theDrag);
		break;

		case kDragTrackingEnterWindow:
			
		break;

		case kDragTrackingInWindow:
			//
			//	We receive InWindow messages as long as the mouse is in one of our windows
			//	during a drag. We draw the window highlighting and blink the insertion caret
			//	when we get these messages.
			//

			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);

			//
			//	Show or hide the window highlighting when the mouse enters or leaves the
			//	TextEdit field in our window (we don't want to show the highlighting when
			//	the mouse is over the window title bar or over the scroll bars).
			//
			
			GetPortBounds(GetDialogPort(ToolsDlog), &caRect);

			if (PtInRect(localMouse, &caRect))
			{
				RectRgn(theRgn = NewRgn(), &caRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}

			//
			//	If this application is the sender, do not allow tracking through
			//	the selection in the window that sourced the drag.
			//

			if (attributes & kDragInsideSenderWindow)
			{
			
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

pascal OSErr MyReceiveTools(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	HFSFlavor			myFlavor;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				index, mouseDownModifiers, mouseUpModifiers;
	OSErr				iErr;
	//
	OSType				sign = 'SNPL';
	AppleEvent			aeEvent, reply;
	AEDesc				target, listElem, fileList;
	//

	if (!mainSystemDrag) return dragNotAcceptedErr;

	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	HideDragHilite(theDrag);

	//
	//	Un fichier en provenance du Finder
	//
	
	iErr = AECreateDesc(	typeApplSignature,
							(Ptr) &sign,
							sizeof(sign),
							&target);
	
	iErr = AECreateAppleEvent(	kCoreEventClass,
								kAEOpenDocuments,
								&target,
								kAutoGenerateReturnID,
								kAnyTransactionID,
								&aeEvent);
	
	iErr = AECreateList(nil,0,false, &fileList);

	CountDragItems(theDrag, &items);

	SetCursor(&watchCrsr);

	for (index = 1; index <= items; index++)
	{
		GetDragItemReferenceNumber(theDrag, index, &theItem);
		
		iErr = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
		if (iErr == noErr)
		{
			Boolean		targetIsFolder, wasAliased;
		
			GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
			
			ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
			AECreateDesc(typeFSS, (Ptr) &myFlavor.fileSpec, sizeof(myFlavor.fileSpec), &listElem);
			
			iErr = AEPutDesc(&fileList, 0, &listElem);
			if (iErr) return iErr;
			
			AEDisposeDesc(&listElem);
			
			iErr = AEPutParamDesc(&aeEvent, keyDirectObject,&fileList);
			if(iErr) return iErr;
		}
	}
	
	iErr = AEDisposeDesc(&fileList);
	
	iErr = AESend(	&aeEvent,
					&reply,
					kAENoReply,
					kAENormalPriority,
					kAEDefaultTimeout,
					NULL,
					NULL);
	if (iErr) return iErr;
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	return noErr;
}

void DrawTimeBar(void)
{
		Rect   			itemRect;
 		long			tempL, curTime;
		GrafPtr			SavePort;
		short			itemType, i;
		Handle			itemHandle;
		PenState		penState;

	if (curMusic == NULL) return;
	GetPort(&SavePort);
	SetPortDialogPort(ToolsDlog);
	
	if (AppearanceManager)
	{
		SetControlValue(progCntl, (oldTime * 100) / maxTime);
	}
	else
	{
		GetDialogItem(ToolsDlog, 10, &itemType, &itemHandle, &itemRect);
		InsetRect(&itemRect, 2, 2);
		i = itemRect.right;
		itemRect.right = itemRect.left + (long) (oldTime * (long) (itemRect.right - itemRect.left) / maxTime);
		if (itemRect.right > i) itemRect.right = i;
		
		GetPenState(&penState);
		PenPixPat(workPixPat);
		
		PaintRect(&itemRect);
		
		SetPenState(&penState);
		
		itemRect.left = itemRect.right;
		itemRect.right = i;
		RGBForeColor(&theColor);
		
		PaintRect(&itemRect);
		
		ForeColor(blackColor);
	}
	SetPort(SavePort);
}

void DoNullTools(void)
{
	GrafPtr	savePort;
	short	tro;

	GetPort(&savePort);
	SetPortDialogPort(ToolsDlog);

	if (oldPartition != MADDriver->PartitionReader || oldPL != MADDriver->PL)
	{
		oldPartition	= MADDriver->PartitionReader;
		oldPL			= MADDriver->PL;
		
		if (oldPL < curMusic->header->numPointers)
		{
			if (oldTime != (TimeScanPtr[ oldPL])[ oldPartition])
			{
				DateTimeRec		dtrp;
				Str255			aStr, bStr;
				long			temp, temp2;
				
				/**/
				
				oldTime = (TimeScanPtr[ oldPL])[ oldPartition];
				
				SecondsToDate(oldTime / 60, &dtrp);
				
				NTStr(2, dtrp.minute, (Ptr) aStr);		MyC2PStr((Ptr) aStr);
				NTStr(2, dtrp.second, (Ptr) bStr);		MyC2PStr((Ptr) bStr);
				pStrcat(aStr, "\p:");
				pStrcat(aStr, bStr);
				
				RGBBackColor(&theColor);
				SetDText(ToolsDlog, 8, aStr);
			}
		}
		
		DrawTimeBar();
	}
	
	DoNullCmdWindow();

// **** **** **** **** **** **** **** **** **** **** **** **** **** ****

/*tro = LoopCntlState;

if ((theEvent.modifiers & alphaLock) !=0)
{
	if (PreviousLoop != true) tro = true;

	PreviousLoop = true;
}
else
{
	if (PreviousLoop != false) tro = false;

	PreviousLoop = false;
}

if (LoopCntlState != tro)
{
	LoopCntlState = tro;
	
	if (LoopCntlState)
	{
		HiliteControl(LoopCntl, kControlButtonPart);
		MADDriver->JumpToNextPattern = false;
	}
	else
	{
		HiliteControl(LoopCntl, 0);
		MADDriver->JumpToNextPattern = true;
	}
}*/
// **** **** **** **** **** **** **** **** **** **** **** **** **** ****

SetPort(savePort);
}

void UpdateToolsWindow(DialogPtr	GetSelection)
{
		Rect   		itemRect;
 		GrafPtr		SavePort;
 		short		itemType;
 		Handle		itemHandle;
 		RgnHandle	visibleRegion;


 		GetPort(&SavePort);
 		SetPortDialogPort(GetSelection);
 		
 		BeginUpdate(GetDialogWindow(GetSelection));
 		
		UpdateCmdDlogWindow(GetSelection);
		
		Draw1Control(playCntl);
		Draw1Control(stopCntl);
		Draw1Control(RecordCntl);
		Draw1Control(BackCntl);
		Draw1Control(ForCntl);
		Draw1Control(JumpNextCntl);
		Draw1Control(JumpBeforeCntl);
		Draw1Control(LoopCntl);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(GetSelection), visibleRegion);
		
		UpdateDialog(GetSelection, visibleRegion);
		
		DisposeRgn(visibleRegion);
		
 		DrawTimeBar();
 		
		if (!AppearanceManager)
		{
	 		GetDialogItem(ToolsDlog, 10, &itemType, &itemHandle, &itemRect);
	 		
			FrameRect(&itemRect);
 		}
		else Draw1Control(progCntl);
		
 		SetCurrentMOD(curMusic->musicFileName);
		/*******/		
		EndUpdate(GetDialogWindow(GetSelection));
		
		SetPort(SavePort);
}

void CreateToolsDlog(void)
{
	Rect		itemRect;
	Handle		itemHandle;
	short		itemType, i;
	
	ToolsDlog = GetNewDialog(143, NULL, (WindowPtr) -1L);
	SetWindEtat(GetDialogWindow(ToolsDlog));
	SetPortDialogPort(ToolsDlog);
	
	LoopCntlState = !MADDriver->JumpToNextPattern;
	PreviousLoop = -1;

	MySizeWindow(ToolsDlog, 183, 48, false);

	workPixPat = GetPixPat(130);
	
	CreateCmdDlog();
	
	ForeUPP = NewControlActionUPP(myForeAction);
	BackUPP = NewControlActionUPP(myBackAction);
	
	if (AppearanceManager)
	{
		GetDialogItem(ToolsDlog, 10, &itemType, &itemHandle, &itemRect);
		progCntl = NewControl(	GetDialogWindow(ToolsDlog),
							&itemRect,
							"\p.",
							true,
							0,		//icl8 id
							0,
							100,
							80,
							0);
	}
	
	GetDialogItemAsControl(ToolsDlog, 5, &playCntl);
/*	GetDialogItem(ToolsDlog , 5, &itemType, &itemHandle, &itemRect);
	playCntl = NewControl(	GetDialogWindow(ToolsDlog),
							&itemRect,
							"\p.",
							true,
							136,		//icl8 id
							-32768,
							32767,
							512,
							0);*/
	
	GetDialogItemAsControl(ToolsDlog, 4, &stopCntl);
/*	GetDialogItem(ToolsDlog , 4, &itemType, &itemHandle, &itemRect);
	stopCntl = NewControl(	GetDialogWindow(ToolsDlog),
							&itemRect,
							"\p.",
							true,
							137,		//icl8 id
							-32768,
							32767,
							512,
							0);*/
	
	GetDialogItemAsControl(ToolsDlog, 46, &LoopCntl);

	GetDialogItemAsControl(ToolsDlog, 44, &RecordCntl);
/*	GetDialogItem(ToolsDlog , 44, &itemType, &itemHandle, &itemRect);
	RecordCntl = NewControl(	GetDialogWindow(ToolsDlog),
								&itemRect,
								"\p.",
								true,
								130,		//icl8 id
								-32768,
								32767,
								512,
								0);*/
	
	GetDialogItemAsControl(ToolsDlog, 3, &BackCntl);
	SetControlAction(BackCntl, BackUPP );
	
/*	GetDialogItem(ToolsDlog , 3, &itemType, &itemHandle, &itemRect);
	BackCntl = NewControl(GetDialogWindow(	ToolsDlog),
							&itemRect,
							"\p.",
							true,
							138,		//icl8 id
							-32768,
							32767,
							512,
							0);*/

	GetDialogItemAsControl(ToolsDlog, 2, &JumpBeforeCntl);
/*	GetDialogItem(ToolsDlog , 2, &itemType, &itemHandle, &itemRect);
	JumpBeforeCntl = NewControl(	GetDialogWindow(ToolsDlog),
								&itemRect,
								"\p.",
								true,
								139,		//icl8 id
								-32768,
								32767,
								512,
								0);*/

	GetDialogItemAsControl(ToolsDlog, 6, &ForCntl);
	SetControlAction(ForCntl, ForeUPP );
/*	GetDialogItem(ToolsDlog , 6, &itemType, &itemHandle, &itemRect);
	ForCntl = NewControl(	GetDialogWindow(ToolsDlog),
								&itemRect,
								"\p.",
								true,
								135,		//icl8 id
								-32768,
								32767,
								512,
								0);*/

	GetDialogItemAsControl(ToolsDlog, 7, &JumpNextCntl);
/*	GetDialogItem(ToolsDlog , 7, &itemType, &itemHandle, &itemRect);
	JumpNextCntl = NewControl(	GetDialogWindow(ToolsDlog),
								&itemRect,
								"\p.",
								true,
								134,		//icl8 id
								-32768,
								32767,
								512,
								0);*/
								
	HiliteControl(playCntl, 0);
	HiliteControl(stopCntl, kControlButtonPart);
	
	TextFont(4);	TextSize(9);
	
	if (DragManagerUse)
	{
		MyTrackingToolsUPP = NewDragTrackingHandlerUPP(MyTrackingTools);
		MyReceiveToolsUPP = NewDragReceiveHandlerUPP(MyReceiveTools);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingToolsUPP, GetDialogWindow(ToolsDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveToolsUPP, GetDialogWindow(ToolsDlog), (void *) NULL);
	}
	
	for (i = 0; i < 256; i++)
	{
		TimeScanPtr[ i] = NULL;
	}

	SetItemMark(ViewsMenu, mTools, checkMark);
}

void CloseToolsWindow(void)
{
	HideWindow(GetDialogWindow(ToolsDlog));

	SetItemMark(ViewsMenu, mTools, noMark);
}

extern 	EventRecord			theEvent;
extern	DialogPtr			PianoDlog;

void DoRecord()
{
	PianoRecording = !PianoRecording;
	
	if (PianoRecording) HiliteControl(RecordCntl, kControlButtonPart);
	else HiliteControl(RecordCntl, 0);
}

void DoRecule(void)
{
	if(MADDriver-> PartitionReader != oldPartition2)
	{
		MADDriver->PartitionReader -= 2;
		
		if (MADDriver->PartitionReader < 0 && MADDriver->PL >= 2)
		{
			MADDriver->PL -= 2;
			MADDriver->Pat = (curMusic->header)->oPointers[ MADDriver->PL];
			MADDriver->PL++;
			MADDriver->PartitionReader = 62;
		}
		
		if (MADDriver->PartitionReader < 0 ) MADDriver->PartitionReader = 62;
		
		oldPartition2 = MADDriver->PartitionReader;
	}
}

void DoPause(void)
{
	if (PianoDlog != NULL) ResetPiano();
	
	FlushPlugin();

	MusicPlayActive = false;
	
	MADDriver->Reading = false;
	MADPurgeTrack(MADDriver);
	MADCleanDriver(MADDriver);	
//	PurgeVSTEffects();
	
	if (thePrefs.GoToStop)
	{
		MADDriver->Pat = RememberPat;
		MADDriver->PartitionReader = RememberReader;
		MADDriver->PL = RememberPL;
	}
	
	HiliteControl(playCntl, 0);
	HiliteControl(stopCntl, kControlButtonPart);
	
	if (MIDIHardwareAlreadyOpen)
	{
		if (MADDriver->DriverSettings.driverMode == MIDISoundDriver)
		{
#if MACOS9VERSION
			OMSAllNotesOff();
#endif
		}
		AllNoteOff(MADDriver);
	}
	
	QTDoAction(false);
}

void DoStop(void)
{
	DoPause();
	
	MADDriver->PartitionReader = 0;
	MADDriver->PL = 0;
	MADDriver->Pat = (curMusic->header)->oPointers[ MADDriver->PL];
	MADDriver->PL++;
}

void DoSearchUp(void)
{
short	newPL, newPartitionReader = 0;

	newPL = MADDriver->PL;
	newPL++;
	if (newPL >= curMusic->header->numPointers) newPL--;
	
	MADPurgeTrack(MADDriver);

	MADDriver->PL = newPL;
	MADDriver->Pat = curMusic->header->oPointers[ MADDriver->PL];
	MADDriver->PartitionReader = newPartitionReader;

	MADCheckSpeed(curMusic, MADDriver);
}

void DoSearchDown(void)
{
short	newPL, newPartitionReader = 0;

	newPL = MADDriver->PL;
	newPL--;
	if (newPL <= 0) newPL = 0;
	
	MADPurgeTrack(MADDriver);

	MADDriver->PL = newPL;
	MADDriver->Pat = curMusic->header->oPointers[ MADDriver->PL];
	MADDriver->PartitionReader = newPartitionReader;

	MADCheckSpeed(curMusic, MADDriver);
}

extern	RGBColor	theColor;
extern	short			theDepth;

void SetCurrentMOD(Str255 theMODName)
{
	Rect   		itemRect;   			/* Temporary rectangle */
 	GrafPtr		SavePort;
 	short		itemType;
 	Handle		itemHandle;
 	Str255		aStr, aStr2;
 
 	GetPort(&SavePort);
 	SetPortDialogPort(ToolsDlog);
 	
 	TextFont(4);	TextSize(9);
 	
 //	ForeColor(blackColor);
//	RGBBackColor(&theColor);
 	
 	pStrcpy(aStr, theMODName);
 	pStrcat(aStr, "\p - ");
 	NumToString(curMusic->header->numChn, aStr2);
 	pStrcat(aStr, aStr2);
 	
 	GetDialogItem(ToolsDlog , 1, &itemType, &itemHandle, &itemRect);
 	TETextBox(aStr + 1, aStr[ 0], &itemRect, teJustCenter);
	
 //	RGBBackColor(&theColor);
//	ForeColor(blackColor);
	
	SetPort(SavePort);
}

void DoRemember(void)
{
	RememberPat = MADDriver->Pat;
	RememberReader = MADDriver->PartitionReader;
	RememberPL = MADDriver->PL;
}

void DoPlay(void)
{
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(ToolsDlog);
	
	HiliteControl(stopCntl, 0);
	HiliteControl(playCntl, kControlButtonPart);
	
	ScanTime();
	MADCheckSpeed(curMusic, MADDriver);
	
	if (MusicPlayActive == true) return;
	MusicPlayActive = true;

	RememberPat 		= MADDriver->Pat;
	RememberReader 		= MADDriver->PartitionReader;
	RememberPL 			= MADDriver->PL;

	MADDriver->Reading = true;
	MADPurgeTrack(MADDriver);
	
	SetPort(savePort);
}

static Boolean IsPlay;

pascal void myBackAction(ControlHandle theCntl, short ctlPart)
{
	if (ctlPart == kControlButtonPart)
	{
		if (!IsPlay) DoPlay();
		DoRecule();
	}
	else if (!IsPlay) DoPause();

	DoGlobalNull();
	
	WaitNextEvent(everyEvent, &theEvent, 1, NULL);
}

static	short		doubleSpeed;
static	Boolean		alreadyReady;

pascal void myForeAction(ControlHandle theCntl, short ctlPart)
{
	if (ctlPart == kControlButtonPart)
	{
		if (!alreadyReady)
		{
			alreadyReady = true;
			if (IsPlay)
			{
				MADDriver->VExt = doubleSpeed;
			//	ChangeSpeed();
			}
			else DoPlay();
		}
	}
	else
	{
		if (alreadyReady)
		{
			alreadyReady = false;
			if (IsPlay)
			{
				MADDriver->VExt = doubleSpeed / 2;
			//	ChangeSpeed();
			}
			else DoPause();
		}
	}

	DoGlobalNull();
	
	WaitNextEvent(everyEvent, &theEvent, 1, NULL);
}

void ScanTime()
{
	short			i, x, y;
	short			dstPL;
	Cmd				*aCmd;

	float			timeResult;
	long			time;
	long			speed, finespeed;

	timeResult		= 0;
	time			= 0;
	speed			= curMusic->header->speed;
	finespeed		= curMusic->header->tempo;
	
	dstPL	= 0;
	
	for (i = 0; i < curMusic->header->numPointers; i++)
	{
		if (TimeScanPtr[ i] != NULL) DisposePtr((Ptr) TimeScanPtr[ i]);
		
		TimeScanPtr[ i] = (long*) NewPtr(curMusic->partition[ curMusic->header->oPointers[ i]]->header.size * sizeof(long) );
		
		for (x = 0; x < curMusic->partition[ curMusic->header->oPointers[ i]]->header.size; x++)
		{
			time ++;
			
			(TimeScanPtr[ i])[ x] = timeResult + (time * 125L * speed * 60) / (50 * finespeed);
			
			for (y = 0; y <  curMusic->header->numChn; y++)
			{
				aCmd = GetMADCommand(x, y, curMusic->partition[ curMusic->header->oPointers[ i]]);
				if (aCmd == NULL) 
				{
					MyDebugStr(__LINE__, __FILE__, "Could not find the selected command!");
					return;
				}
				/** SpeedE **/
				
				if (aCmd->cmd == speedE)
				{
					/** Compute time for this interval **/

					timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
					time = 0;
					
					/************************************/
					
					if (aCmd->arg < 32)
					{
						if (aCmd->arg != 0) speed = aCmd->arg;
					}
					else
					{
						if (aCmd->arg != 0) finespeed = aCmd->arg;
					}
				}
				
				/** SkipE **/
				
				if (aCmd->cmd == skipE)
				{
					for (; x < curMusic->partition[ curMusic->header->oPointers[ i]]->header.size; x++)
					{
						(TimeScanPtr[ i])[ x] = timeResult + (time * 125L * speed * 60) / (50 * finespeed);
					}
				}

			}
		}
	}
	
	timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
	{
		DateTimeRec	dtrp;
		Str255		aStr, bStr;
		long		temp;
		

		maxTime = timeResult;
		
		SecondsToDate(timeResult / 60, &dtrp);
		
		NTStr(2, dtrp.minute, (Ptr) aStr);		MyC2PStr((Ptr) aStr);
		NTStr(2, dtrp.second, (Ptr) bStr);		MyC2PStr((Ptr) bStr);
		pStrcat(aStr, "\p:");
		pStrcat(aStr, bStr);
		
		SetDText(ToolsDlog, 9, aStr);
		

	/*	SecondsToDate(curTimeResult, &dtrp);
		
		NTStr(2, dtrp.minute, (Ptr) aStr);		MyC2PStr((Ptr) aStr);
		NTStr(2, dtrp.second, (Ptr) bStr);		MyC2PStr((Ptr) bStr);
		pStrcat(aStr, "\p:");
		pStrcat(aStr, bStr);
		
		SetDText(ToolsDlog, 8, aStr);
	*/
		/**/
	}
}

/*void ScanSpeedChange(short newPL, short newPartitionReader)
{
	short			i, x, y;
	short			dstPL;
	Cmd				*aCmd;
	Boolean			CmdSpeed = false, FineFound = false;

	dstPL	= 0;
//	newPartitionReader = 63;
	
	for (i = newPL; i >= dstPL; i--)
	{
		for (x = newPartitionReader; x >= 0; x--)
		{
			for (y = curMusic->header->numChn-1; y >= 0 ; y--)
			{
				aCmd = GetMADCommand(x, y, curMusic->partition[ curMusic->header->oPointers[ i]]);
				
				if (aCmd->cmd == speedE)
				{					
					if (aCmd->arg < 32)
					{
						if (aCmd->arg != 0)
						{
							if (CmdSpeed == false) MADDriver->speed = aCmd->arg;
							CmdSpeed = true;
						}
					}
					else
					{
						if (aCmd->arg != 0)
						{
							if (FineFound == false) MADDriver->finespeed = aCmd->arg;
							FineFound = true;
						}
					}
				}
			}
			if (CmdSpeed == true && FineFound == true) return;
		}
		newPartitionReader = 63;
	}
		
	if (dstPL == 0)
	{
		if (!CmdSpeed) 	MADDriver->speed 		= curMusic->header->speed;
		if (!FineFound) MADDriver->finespeed 	= curMusic->header->tempo;
	}
}*/

void DoChangeLoop(void)
{
	LoopCntlState = !LoopCntlState;
	
	if (LoopCntlState)
	{
		HiliteControl(LoopCntl, kControlButtonPart);
		MADDriver->JumpToNextPattern = false;
	}
	else
	{
		HiliteControl(LoopCntl, 0);
		MADDriver->JumpToNextPattern = true;
	}
}

void DoItemPressTools(short whichItem, DialogPtr whichDialog)
{
	short				temp = 0,itemType, newPL, newPat, newPartitionReader;
	Point				myPt;
	Handle				itemHandle;
	Rect				itemRect;
	long				tempLong, max, val;
	GrafPtr				savePort;
	Boolean				ReadingCopy;
	
	GetPort(&savePort);
	SetPortDialogPort(ToolsDlog);
	
	switch(whichItem)
	{
		case 2:
			if (MyTrackControl(JumpBeforeCntl, theEvent.where, NULL))
			{
				DoSearchDown();
			}
			break;
			
		case 3:
			IsPlay = MusicPlayActive;
			MyTrackControl(BackCntl, theEvent.where, BackUPP);
			if (!IsPlay) DoPause();
			break;
			
		case 44:
			if (MyTrackControl(RecordCntl, theEvent.where, NULL))
			{
				DoRecord();
			}
			break;
			
		case 46:
			if (MyTrackControl(LoopCntl, theEvent.where, NULL))
			{
				DoChangeLoop();
			}
			break;
			
		case 4:
			if (MyTrackControl(stopCntl, theEvent.where, NULL))
			{
				DoPause();
			}
			break;
			
		case 5:
			if (MyTrackControl(playCntl, theEvent.where, NULL))
			{
				DoPlay();
			}
			break;
			
		case 6:
			doubleSpeed = MADDriver->VExt *2;
			IsPlay = MusicPlayActive;
			alreadyReady = false;
			
			
			MyTrackControl(ForCntl, theEvent.where, ForeUPP);
			
			if (IsPlay)
			{
				MADDriver->VExt = doubleSpeed / 2;
				//	ChangeSpeed();
			}
			else DoPause();
			break;
			
		case 7:
			if (MyTrackControl(JumpNextCntl, theEvent.where, NULL))
			{
				DoSearchUp();
			}
			break;
			
		case 10:
			do
			{
				GetDialogItem(ToolsDlog , 10, &itemType, &itemHandle, &itemRect);
				GetMouse(&myPt);
				
				DoGlobalNull();
				
				WaitNextEvent(everyEvent, &theEvent, 1, NULL);
				
				if (temp != myPt.h)
				{
					short i;
					
					if (myPt.h < itemRect.left) 		myPt.h = itemRect.left;
					else if (myPt.h > itemRect.right)	myPt.h = itemRect.right;
					
					temp = myPt.h;
					itemRect.right = myPt.h;
					
					GetDialogItem (ToolsDlog , 10, &itemType, &itemHandle, &itemRect);
					itemRect.left = myPt.h;
					
					GetDialogItem (ToolsDlog, 10, &itemType, &itemHandle, &itemRect);
					
					MADSetMusicStatus(MADDriver, itemRect.left, itemRect.right, myPt.h);
					
					DrawTimeBar();
				}
			}while (Button());
			break;
	}
	
	DoItemPressCmdDlog(whichItem, whichDialog);
	
}
