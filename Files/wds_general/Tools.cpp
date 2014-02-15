#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <CoreMIDI/CoreMIDI.h>
#include "Tools.h"
#include "PlayerPROApp.h"

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

void PlayerPRO::Tools::DoHelp(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

PlayerPRO::Tools::Tools() : wds_general::wds_general(143)
{
	CreateDialog();
}

PlayerPRO::Tools::~Tools()
{
	
}

/*****************************/

extern	long		TickInterne;
extern	RGBColor	theColor;

void FlushPlugin();
void ScanTime();
Boolean IsMyTypeMODList(DragReference theDrag);
void PurgeVSTEffects();
pascal void myForeAction(ControlHandle theCntl, short ctlPart);
pascal void myBackAction(ControlHandle theCntl, short ctlPart);
void SetCurrentMOD(Str255 theMODName);


pascal OSErr MyTrackingTools(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	unsigned long	attributes;
	RgnHandle		theRgn;
	Point			theMouse, localMouse;
	Rect			caRect;
	PlayerPRO::Tools *inTools = static_cast<PlayerPRO::Tools *>(handlerRefCon);
	
	if ((message != kDragTrackingEnterHandler) && (!inTools->GetCanAcceptDrag()))
		return noErr;
	
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {
		case kDragTrackingEnterHandler:
			inTools->SetCanAcceptDrag(IsMyTypeMODList(theDrag));
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
			
			GetPortBounds(inTools->GetDialogPort(), &caRect);
			
			if (PtInRect(localMouse, &caRect)) {
				RectRgn(theRgn = NewRgn(), &caRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}
			
			//
			//	If this application is the sender, do not allow tracking through
			//	the selection in the window that sourced the drag.
			//
			
			if (attributes & kDragInsideSenderWindow) {
				
			}
			break;
			
		case kDragTrackingLeaveWindow:
			HideDragHilite(theDrag);
			break;
			
		case kDragTrackingLeaveHandler:
			
			break;
	}
	RGBBackColor(&theColor);
	
	return noErr;
}

pascal OSErr MyReceiveTools(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	HFSFlavor			myFlavor;
	unsigned short		items;
	Size				textSize;
	short				index, mouseDownModifiers, mouseUpModifiers;
	OSErr				iErr;
	OSType				sign = 'SNPL';
	AppleEvent			aeEvent, reply;
	AEDesc				target, listElem, fileList;
	
	ItemReference		theItem;
	DragAttributes		attributes;
	
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	HideDragHilite(theDrag);
	
	//
	//	Un fichier en provenance du Finder
	//
	
	iErr = AECreateDesc(typeApplSignature,
						(Ptr)&sign,
						sizeof(sign),
						&target);
	
	iErr = AECreateAppleEvent(kCoreEventClass,
							  kAEOpenDocuments,
							  &target,
							  kAutoGenerateReturnID,
							  kAnyTransactionID,
							  &aeEvent);
	
	iErr = AECreateList(NULL, 0, false, &fileList);
	
	CountDragItems(theDrag, &items);
	
	TheApp->SetCursorOnNumber(PlayerPRO::watchCrsr);
	
	for (index = 1; index <= items; index++) {
		GetDragItemReferenceNumber(theDrag, index, &theItem);
		
		iErr = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
		
		if (iErr == noErr) {
			Boolean targetIsFolder, wasAliased;
			
			GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
			
			ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
			
			AECreateDesc(typeFSS, (Ptr) &myFlavor.fileSpec, sizeof(myFlavor.fileSpec), &listElem);
			
			iErr = AEPutDesc(&fileList, 0, &listElem);
			if (iErr)
				return iErr;
			
			AEDisposeDesc(&listElem);
			
			iErr = AEPutParamDesc(&aeEvent, keyDirectObject,&fileList);
			if(iErr)
				return iErr;
		}
	}
	
	iErr = AEDisposeDesc(&fileList);
	
	iErr = AESend(&aeEvent,
				  &reply,
				  kAENoReply,
				  kAENormalPriority,
				  kAEDefaultTimeout,
				  NULL,
				  NULL);
	if (iErr)
		return iErr;
	
	TheApp->SetCursorToQDArrow();
	
	return noErr;
}

void PlayerPRO::Tools::DrawTimeBar()
{
	GrafPtr SavePort;
	
	if (TheApp->curMusic == NULL)
		return;
	GetPort(&SavePort);
	SetPortDialogPort(theDialog);
	
	SetControlValue(progCntl, (oldTime * 100) / maxTime);
	SetPort(SavePort);
}

void PlayerPRO::Tools::DoNull()
{
	GrafPtr	savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	if (oldPartition != TheApp->MADDriver->PartitionReader || oldPL != TheApp->MADDriver->PL) {
		oldPartition = TheApp->MADDriver->PartitionReader;
		oldPL = TheApp->MADDriver->PL;
		
		if (oldPL < TheApp->curMusic->header->numPointers) {
			if (oldTime != (TimeScanPtr[oldPL])[oldPartition]) {
				DateTimeRec		dtrp;
				Str255			aStr, bStr;
				
				/**/
				
				oldTime = (TimeScanPtr[oldPL])[oldPartition];
				
				SecondsToDate(oldTime / 60, &dtrp);
				
				NTStr(2, dtrp.minute, (Ptr)aStr);
				MyC2PStr((Ptr)aStr);
				NTStr(2, dtrp.second, (Ptr)bStr);
				MyC2PStr((Ptr)bStr);
				pStrcat(aStr, (StringPtr)"\p:");
				pStrcat(aStr, bStr);
				
				RGBBackColor(&theColor);
				SetDText(theDialog, 8, aStr);
			}
		}
		
		DrawTimeBar();
	}
	
	DoNullCmdWindow();
	
	// **** **** **** **** **** **** **** **** **** **** **** **** **** ****
	
	// **** **** **** **** **** **** **** **** **** **** **** **** **** ****
	
	SetPort(savePort);
}

void PlayerPRO::Tools::UpdateToolsWindow(DialogPtr GetSelection)
{
	GrafPtr		SavePort;
	RgnHandle	visibleRegion;
	
	GetPort(&SavePort);
	SetPortDialogPort(GetSelection);
	
	BeginUpdate(::GetDialogWindow(GetSelection));
	
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
	
	GetPortVisibleRegion(::GetDialogPort(GetSelection), visibleRegion);
	
	UpdateDialog(GetSelection, visibleRegion);
	
	DrawTimeBar();
	
	Draw1Control(progCntl);
	
	SetCurrentMOD(TheApp->curMusic->musicFileName);
	/*******/
	EndUpdate(::GetDialogWindow(GetSelection));
	
	QDFlushPortBuffer(::GetDialogPort(GetSelection), visibleRegion);
	DisposeRgn(visibleRegion);

	SetPort(SavePort);
}

void PlayerPRO::Tools::CreateDialog()
{
	Rect	itemRect;
	Handle	itemHandle;
	short	itemType;
	int		i;
	
	SetWindEtat(GetDialogWindow());
	SetPortDialogPort(theDialog);
	
	LoopCntlState = !TheApp->MADDriver->JumpToNextPattern;
	PreviousLoop = -1;
	
	MySizeWindow(theDialog, 183, 48, false);
	
	workPixPat = GetPixPat(130);
	
	CreateCmdDlog();
	
	ForeUPP = NewControlActionUPP(myForeAction);
	BackUPP = NewControlActionUPP(myBackAction);
	
	GetDialogItem(theDialog, 10, &itemType, &itemHandle, &itemRect);
	progCntl = NewControl(GetDialogWindow(),
						  &itemRect,
						  "\p.",
						  true,
						  0,		//icl8 id
						  0,
						  100,
						  80,
						  0);
	
	GetDialogItemAsControl(theDialog, 5, &playCntl);
	GetDialogItemAsControl(theDialog, 4, &stopCntl);
	GetDialogItemAsControl(theDialog, 46, &LoopCntl);
	GetDialogItemAsControl(theDialog, 44, &RecordCntl);
	GetDialogItemAsControl(theDialog, 3, &BackCntl);
	SetControlAction(BackCntl, BackUPP);
	
	GetDialogItemAsControl(theDialog, 2, &JumpBeforeCntl);
	GetDialogItemAsControl(theDialog, 6, &ForCntl);
	SetControlAction(ForCntl, ForeUPP);
	
	GetDialogItemAsControl(theDialog, 7, &JumpNextCntl);
	
	HiliteControl(playCntl, 0);
	HiliteControl(stopCntl, kControlButtonPart);
	
	TextFont(4);
	TextSize(9);
	
	MyTrackingToolsUPP = NewDragTrackingHandlerUPP(MyTrackingTools);
	MyReceiveToolsUPP = NewDragReceiveHandlerUPP(MyReceiveTools);
	
	InstallTrackingHandler(MyTrackingToolsUPP, GetDialogWindow(), (void *)this);
	InstallReceiveHandler(MyReceiveToolsUPP, GetDialogWindow(), (void *)this);
	
	for (i = 0; i < 256; i++) {
		TimeScanPtr[i] = NULL;
	}
	
	//SetItemMark(ViewsMenu, mTools, checkMark);
}

void PlayerPRO::Tools::CloseToolsWindow()
{
	HideWindow(GetDialogWindow());

	//SetItemMark(ViewsMenu, mTools, noMark);
}

extern DialogPtr PianoDlog;

void PlayerPRO::Tools::DoRecord()
{
	PianoRecording = !PianoRecording;
	
	if (PianoRecording)
		HiliteControl(RecordCntl, kControlButtonPart);
	else
		HiliteControl(RecordCntl, 0);
}

void PlayerPRO::Tools::DoRecule()
{
	if (TheApp->MADDriver-> PartitionReader != oldPartition2) {
		TheApp->MADDriver->PartitionReader -= 2;
		
		if (TheApp->MADDriver->PartitionReader < 0 && TheApp->MADDriver->PL >= 2) {
			TheApp->MADDriver->PL -= 2;
			TheApp->MADDriver->Pat = (TheApp->curMusic->header)->oPointers[TheApp->MADDriver->PL];
			TheApp->MADDriver->PL++;
			TheApp->MADDriver->PartitionReader = 62;
		}
		
		if (TheApp->MADDriver->PartitionReader < 0)
			TheApp->MADDriver->PartitionReader = 62;
		
		oldPartition2 = TheApp->MADDriver->PartitionReader;
	}
}

void PlayerPRO::Tools::DoPause()
{
	if (PianoDlog != NULL)
		ResetPiano();
	
	FlushPlugin();

	TheApp->MusicPlayActive = false;
	
	TheApp->MADDriver->Reading = false;
	MADPurgeTrack(TheApp->MADDriver);
	MADCleanDriver(TheApp->MADDriver);
	//PurgeVSTEffects();
	
	if (TheApp->thePrefs.GoToStop) {
		TheApp->MADDriver->Pat = RememberPat;
		TheApp->MADDriver->PartitionReader = RememberReader;
		TheApp->MADDriver->PL = RememberPL;
	}
	
	HiliteControl(playCntl, 0);
	HiliteControl(stopCntl, kControlButtonPart);
	
	if (MIDIHardwareAlreadyOpen) {
		if (TheApp->MADDriver->DriverSettings.driverMode == MIDISoundDriver) {
#if MACOS9VERSION
			OMSAllNotesOff();
#endif
		}
		AllNoteOff(TheApp->MADDriver);
	}
	
	QTDoAction(false);
}

void PlayerPRO::Tools::DoStop()
{
	DoPause();
	
	TheApp->MADDriver->PartitionReader = 0;
	TheApp->MADDriver->PL = 0;
	TheApp->MADDriver->Pat = (TheApp->curMusic->header)->oPointers[TheApp->MADDriver->PL];
	TheApp->MADDriver->PL++;
}

void PlayerPRO::Tools::DoSearchUp()
{
	short newPL, newPartitionReader = 0;
	
	newPL = TheApp->MADDriver->PL;
	newPL++;
	if (newPL >= TheApp->curMusic->header->numPointers)
		newPL--;
	
	MADPurgeTrack(TheApp->MADDriver);
	
	TheApp->MADDriver->PL = newPL;
	TheApp->MADDriver->Pat = TheApp->curMusic->header->oPointers[TheApp->MADDriver->PL];
	TheApp->MADDriver->PartitionReader = newPartitionReader;
	
	MADCheckSpeed(TheApp->curMusic, TheApp->MADDriver);
}

void PlayerPRO::Tools::DoSearchDown()
{
	short newPL, newPartitionReader = 0;
	
	newPL = TheApp->MADDriver->PL;
	newPL--;
	if (newPL <= 0)
		newPL = 0;
	
	MADPurgeTrack(TheApp->MADDriver);
	
	TheApp->MADDriver->PL = newPL;
	TheApp->MADDriver->Pat = TheApp->curMusic->header->oPointers[TheApp->MADDriver->PL];
	TheApp->MADDriver->PartitionReader = newPartitionReader;
	
	MADCheckSpeed(TheApp->curMusic, TheApp->MADDriver);
}

void PlayerPRO::Tools::SetCurrentMOD(ConstStr255Param theMODName)
{
	Rect	itemRect; /* Temporary rectangle */
	GrafPtr	SavePort;
	short	itemType;
	Handle	itemHandle;
	Str255	aStr, aStr2;
	
	GetPort(&SavePort);
	SetPortDialogPort(theDialog);
	
	TextFont(4);
	TextSize(9);
	
	//ForeColor(blackColor);
	//RGBBackColor(&theColor);
	
	pStrcpy(aStr, theMODName);
	pStrcat(aStr, (StringPtr)"\p - ");
	NumToString(TheApp->curMusic->header->numChn, aStr2);
	pStrcat(aStr, aStr2);
	
	GetDialogItem(theDialog , 1, &itemType, &itemHandle, &itemRect);
	TETextBox(aStr + 1, aStr[0], &itemRect, teJustCenter);
	
	//RGBBackColor(&theColor);
	//ForeColor(blackColor);
	
	SetPort(SavePort);
}

void PlayerPRO::Tools::DoRemember()
{
	RememberPat = TheApp->MADDriver->Pat;
	RememberReader = TheApp->MADDriver->PartitionReader;
	RememberPL = TheApp->MADDriver->PL;
}

void PlayerPRO::Tools::DoPlay()
{
	GrafPtr savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	HiliteControl(stopCntl, 0);
	HiliteControl(playCntl, kControlButtonPart);
	
	ScanTime();
	MADCheckSpeed(TheApp->curMusic, TheApp->MADDriver);
	
	if (TheApp->MusicPlayActive == true)
		return;
	TheApp->MusicPlayActive = true;
	
	RememberPat		= TheApp->MADDriver->Pat;
	RememberReader	= TheApp->MADDriver->PartitionReader;
	RememberPL		= TheApp->MADDriver->PL;
	
	TheApp->MADDriver->Reading = true;
	MADPurgeTrack(TheApp->MADDriver);
	
	SetPort(savePort);
}

static bool IsPlay;

pascal void myBackAction(ControlHandle theCntl, short ctlPart)
{
	if (ctlPart == kControlButtonPart) {
		if (!IsPlay)
			TheApp->ToolsWindow->DoPlay();
		TheApp->ToolsWindow->DoRecule();
	} else if (!IsPlay)
		TheApp->ToolsWindow->DoPause();
	
	TheApp->DoGlobalNull();
	
	WaitNextEvent(everyEvent, &TheApp->theEvent, 1, NULL);
}

static short	doubleSpeed;
static Boolean	alreadyReady;

pascal void myForeAction(ControlHandle theCntl, short ctlPart)
{
	if (ctlPart == kControlButtonPart) {
		if (!alreadyReady) {
			alreadyReady = true;
			if (IsPlay) {
				TheApp->MADDriver->VExt = doubleSpeed;
				//ChangeSpeed();
			} else
				TheApp->ToolsWindow->DoPlay();
		}
	} else {
		if (alreadyReady) {
			alreadyReady = false;
			if (IsPlay) {
				TheApp->MADDriver->VExt = doubleSpeed / 2;
				//ChangeSpeed();
			} else
				TheApp->ToolsWindow->DoPause();
		}
	}
	
	TheApp->DoGlobalNull();
	
	WaitNextEvent(everyEvent, &TheApp->theEvent, 1, NULL);
}

void PlayerPRO::Tools::ScanTime()
{
	short			i, x, y;
	short			dstPL;
	Cmd				*aCmd;
	
	float			timeResult;
	long			time;
	long			speed, finespeed;
	
	timeResult		= 0;
	time			= 0;
	speed			= TheApp->curMusic->header->speed;
	finespeed		= TheApp->curMusic->header->tempo;
	
	dstPL	= 0;
	
	for (i = 0; i < TheApp->curMusic->header->numPointers; i++) {
		if (TimeScanPtr[i] != NULL)
			DisposePtr((Ptr)TimeScanPtr[i]);
		
		TimeScanPtr[i] = (long*) NewPtr(TheApp->curMusic->partition[TheApp->curMusic->header->oPointers[i]]->header.size * sizeof(long));
		
		for (x = 0; x < TheApp->curMusic->partition[TheApp->curMusic->header->oPointers[i]]->header.size; x++) {
			time ++;
			
			(TimeScanPtr[i])[x] = timeResult + (time * 125L * speed * 60) / (50 * finespeed);
			
			for (y = 0; y <  TheApp->curMusic->header->numChn; y++) {
				aCmd = GetMADCommand(x, y, TheApp->curMusic->partition[TheApp->curMusic->header->oPointers[i]]);
				if (aCmd == NULL) {
					MyDebugStr(__LINE__, __FILE__, "Could not find the selected command!");
					return;
				}
				/** SpeedE **/
				
				if (aCmd->cmd == speedE) {
					/** Compute time for this interval **/
					
					timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
					time = 0;
					
					/************************************/
					
					if (aCmd->arg < 32) {
						if (aCmd->arg != 0)
							speed = aCmd->arg;
					} else {
						if (aCmd->arg != 0)
							finespeed = aCmd->arg;
					}
				}
				
				/** SkipE **/
				
				if (aCmd->cmd == skipE) {
					for (; x < TheApp->curMusic->partition[TheApp->curMusic->header->oPointers[i]]->header.size; x++) {
						(TimeScanPtr[i])[x] = timeResult + (time * 125L * speed * 60) / (50 * finespeed);
					}
				}
			}
		}
	}
	
	timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
	{
		DateTimeRec	dtrp;
		Str255		aStr, bStr;
		
		maxTime = timeResult;
		
		SecondsToDate(timeResult / 60, &dtrp);
		
		NTStr(2, dtrp.minute, (Ptr) aStr);		MyC2PStr((Ptr) aStr);
		NTStr(2, dtrp.second, (Ptr) bStr);		MyC2PStr((Ptr) bStr);
		pStrcat(aStr, (StringPtr)"\p:");
		pStrcat(aStr, bStr);
		
		SetDText(theDialog, 9, aStr);
		

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

#if 0
void ScanSpeedChange(short newPL, short newPartitionReader)
{
	short			i, x, y;
	short			dstPL;
	Cmd				*aCmd;
	Boolean			CmdSpeed = false, FineFound = false;

	dstPL	= 0;
	//newPartitionReader = 63;
	
	for (i = newPL; i >= dstPL; i--)
	{
		for (x = newPartitionReader; x >= 0; x--)
		{
			for (y = curMusic->header->numChn-1; y >= 0 ; y--)
			{
				aCmd = GetMADCommand(x, y, curMusic->partition[curMusic->header->oPointers[i]]);
				
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
}
#endif

void PlayerPRO::Tools::DoChangeLoop()
{
	LoopCntlState = !LoopCntlState;
	
	if (LoopCntlState) {
		HiliteControl(LoopCntl, kControlButtonPart);
		TheApp->MADDriver->JumpToNextPattern = false;
	} else {
		HiliteControl(LoopCntl, 0);
		TheApp->MADDriver->JumpToNextPattern = true;
	}
}

void PlayerPRO::Tools::DoItemPressTools(short whichItem, DialogPtr whichDialog)
{
	short				temp = 0,itemType;
	Point				myPt;
	Handle				itemHandle;
	Rect				itemRect;
	GrafPtr				savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	switch (whichItem) {
		case 2:
			if (MyTrackControl(JumpBeforeCntl,TheApp-> theEvent.where, NULL)) {
				DoSearchDown();
			}
			break;
			
		case 3:
			IsPlay = TheApp->MusicPlayActive;
			MyTrackControl(BackCntl, TheApp->theEvent.where, BackUPP);
			if (!IsPlay)
				DoPause();
			break;
			
		case 44:
			if (MyTrackControl(RecordCntl, TheApp->theEvent.where, NULL)) {
				DoRecord();
			}
			break;
			
		case 46:
			if (MyTrackControl(LoopCntl, TheApp->theEvent.where, NULL)) {
				DoChangeLoop();
			}
			break;
			
		case 4:
			if (MyTrackControl(stopCntl, TheApp->theEvent.where, NULL)) {
				DoPause();
			}
			break;
			
		case 5:
			if (MyTrackControl(playCntl, TheApp->theEvent.where, NULL)) {
				DoPlay();
			}
			break;
			
		case 6:
			doubleSpeed = TheApp->MADDriver->VExt *2;
			IsPlay = TheApp->MusicPlayActive;
			alreadyReady = false;
			
			MyTrackControl(ForCntl, TheApp->theEvent.where, ForeUPP);
			
			if (IsPlay) {
				TheApp->MADDriver->VExt = doubleSpeed / 2;
				//ChangeSpeed();
			} else
				DoPause();
			break;
			
		case 7:
			if (MyTrackControl(JumpNextCntl, TheApp->theEvent.where, NULL)) {
				DoSearchUp();
			}
			break;
			
		case 10:
			do {
				GetDialogItem(theDialog , 10, &itemType, &itemHandle, &itemRect);
				GetMouse(&myPt);
				
				TheApp->DoGlobalNull();
				
				WaitNextEvent(everyEvent, &TheApp->theEvent, 1, NULL);
				
				if (temp != myPt.h) {
					if (myPt.h < itemRect.left)
						myPt.h = itemRect.left;
					else if (myPt.h > itemRect.right)
						myPt.h = itemRect.right;
					
					temp = myPt.h;
					itemRect.right = myPt.h;
					
					GetDialogItem (theDialog, 10, &itemType, &itemHandle, &itemRect);
					itemRect.left = myPt.h;
					
					GetDialogItem (theDialog, 10, &itemType, &itemHandle, &itemRect);
					
					MADSetMusicStatus(TheApp->MADDriver, itemRect.left, itemRect.right, myPt.h);
					
					DrawTimeBar();
				}
			} while (Button());
			break;
	}
	
	DoItemPressCmdDlog(whichItem, whichDialog);
}
