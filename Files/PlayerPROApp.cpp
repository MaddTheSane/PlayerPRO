/*
 *  PlayerPROApp.cpp
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayerPROApp.h"
extern "C" {
#include "PreferenceHandler.h"
}

static const short FKEYCo[] = {0x7A, 0x78, 0x63, 0x76, 0x60, 0x61, 0x62, 0x64, 0x65, 0x6D, 0x67, 0x6F, 0x69, 0x6B, 0x71};

#define HIM 14
#define WIM 27
#define kPlayerPROClass		'SNPL'
#define kPlayerPROSignature	'SNPL'
#define kErreurID			'AERR'
#define kFinderSig			'FNDR'
#define kAEFinderEvents		'FNDR'
#define kSystemType			'MACS'
#define kAEOpenSelection	'sope'
#define keySelection		'fsel'

#define	VERSION			0x0592
#define	PLAYERPREF		"\pPlayerPRO Prefs"
#define PLAYERBOOK		"\pPlayerPRO Bookmarks"
#define MLSAVENAME		"\pMusic List"
#define wInHelpButton	20
#define	MAXURL			90

short Quality, Mode;

EXP void MyDebugStr(short line, char *file, char *text)
{
	fprintf(stderr, "PlayerPRO: %s:%u error text:%s!\n", file, line, text);
	
	Str255	fileS, lineS, dateS, textS;
	
	NumToString(line, lineS);
	strlcpy((Ptr)fileS, file, sizeof(fileS));
	strlcpy((Ptr)textS, text, sizeof(textS));
	strlcpy((Ptr)dateS, __DATE__, sizeof(dateS));
	
	MYC2PStr((Ptr)fileS);
	MYC2PStr((Ptr)textS);
	MYC2PStr((Ptr)dateS);
	
	ParamText(textS, fileS, lineS, dateS);
	switch (Alert(130, NULL)) {
		case 1:
			return;
			break;
			
		case 5:
			Debugger();
			break;
			
		case 6:
			abort();
			break;
	}
}

void PlayerPRO::PlayerPROApp::SKVolume(short vol)
{
	MADSetHardwareVolume(vol);
	
	thePrefs.volumeLevel = vol;
}

void PlayerPRO::PlayerPROApp::GetMDEFRect(Rect *aRect, Rect *menuRect, short whichItem)
{
	short Xpos, Ypos;
	
	whichItem--;
	
	Xpos = whichItem - (whichItem/12)*12;
	Ypos = whichItem / 12;
	
	aRect->left		= menuRect->left + Xpos * WIM;
	aRect->right	= menuRect->left + (Xpos + 1) * WIM;
	
	aRect->top		= menuRect->top + Ypos * HIM;
	aRect->bottom	= menuRect->top + (Ypos + 1) * HIM;
}

void PlayerPRO::PlayerPROApp::DoGlobalNull()
{
	GrafPtr savePort;
	
	GetPort(&savePort);
	
	GetKeys(km);
	
	//if (thePrefs.MidiKeyBoard)
	//	DoMIDIHarwareRecord();
	
	if (MADDriver->musicEnd == true) {
		DoLoadOtherMusic(true);
	}
	
	//VSTEditorDoNull();
	//DoVisualNull();
	DoNullPattern();
	DoNullSpectrum();
	DoNull();
	DoNullInstruList();
	//DoNullInstrument();
	DoNullEditor();
	DoNullClassic();
	DoNullMozart();
	DoNullTools();
	DoNullDigiWindow();
	DoNullWave();
	DoNullMODList();
	DoNullQT();
	DoNullStaff();
	DoAIFFExporting();
	DoNullParti();
	DoNullAdap();
	DoNullInstruView();
	//DoNullTrackView();
	//MyNullHook();
	
	//if (PianoDlog != NULL && MusicPlayActive == true)
	//	DoNullPiano();
	
	/*** Active Help ***/
	
	if (thePrefs.ActiveHelp)
		DoAHelpInfo();
	
	/*******************/
	
	SetPort(savePort);
}

void PlayerPRO::PlayerPROApp::DoOSEvent(EventRecord *event, bool ModalCall)
{
	switch ((event->message >> 24) & 0xFF) {
		case mouseMovedMessage:
			break;
			
		case suspendResumeMessage:
			if (event->message & resumeFlag) {
				ActivateProcedure(ModalCall);
				HiliteMenu(0);
				
				ShowFloatingWindows();
			} else {
				//if (AHelpDlog != NULL)
				//	HideWindow(GetDialogWindow(AHelpDlog));
				HideFloatingWindows();	//HideWindow(GetDialogWindow(ToolsDlog));
				if (oldWindow != NULL) {
					HiliteWindow(oldWindow, false);
					EraseGrowIcon(GetDialogFromWindow(oldWindow));
				}
				SetCursor(GetQDGlobalsArrow(&qdarrow));
			}
			break;
	}
}

short PlayerPRO::PlayerPROApp::ConvertCharToNote(char theChar)
{
	short i;
	
	if (theChar >= 0 && theChar < CHAR_MAX) {
		i = thePrefs.PianoKey[theChar];
		
		if (i != 0xFF && i != -1)
			i += thePrefs.pianoOffset * 12;
		
		if((i >= 0 && i < NUMBER_NOTES) || i == 0xFF)
			return i;
		else
			return -1;
	} else
		return -1;
}

static Boolean IsDialogWindow(WindowPtr window)
{
	if (window == NULL)
		return FALSE;
	
    return (GetWindowKind(window) == dialogKind);
}

void PlayerPRO::PlayerPROApp::EventLoop2()
{
	short			i, x;
	GrafPtr			savePort;
	short			theChar;
	Rect			tempRect;
	Str255			str1;
	Boolean			gotEvent;
	
	gotEvent = WaitNextEvent(everyEvent, &theEvent, 1, gCursorRgn);
	
	//PressInDialog = false;
	switch (theEvent.what) {
		case activateEvt:
			//DoActivateEvent(&theEvent, (theEvent.modifiers & activeFlag) != 0, false);
		{
			WindowPtr theWindow = (WindowPtr)theEvent.message;
			
			if (IsDialogWindow(theWindow)) {
				DialogRef		dialog;
				SInt16			itemHit;
				
				//	dialog = GetDialogFromWindow(theWindow);
				
				dialog = NULL;
				
				DialogSelect(&theEvent, &dialog, &itemHit);
				
				//UpdateALLWindow();
				
				{
					Rect caRect;
					
					GetPort(&savePort);
					SetPortWindowPort(theWindow);
					GetPortBounds(GetWindowPort(theWindow), &caRect);
					InvalWindowRect(theWindow, &caRect);
					SetPort(savePort);
					
					DoUpdateEvent(&theEvent);
				}
			}
		}
			break;
			
		case osEvt:
			DoOSEvent(&theEvent, false);
			break;
			
		case kHighLevelEvent:
			AEProcessAppleEvent (&theEvent);
			break;
			
		case keyUp:
			if (thePrefs.MacKeyBoard) {
				theChar = theEvent.message & charCodeMask;
				str1[0] = 1;	str1[1] = theChar;
				//	UpperString(str1, true);
				theChar = str1[1];
				
#if 0
				if (theChar >= 0 && theChar < 300) {
					i = thePrefs.PianoKey[theChar];
					
					if (i != 0xFF)
						i += thePrefs.pianoOffset * 12;
					
					if((i >= 0 && i < NUMBER_NOTES) || i == 0xFF) {
						for(x = 0; x < 10;x++) {
							if (TouchMem[x] == i && TouchMem[x] >= 0) {
								if (PianoDlog != NULL) {
									GetPort(&savePort);
									SetPortDialogPort(PianoDlog);
									
									GetToucheRect(&tempRect, TouchMem[x]);
									EffaceTouche(TouchMem[x], &tempRect);
									
									SetPort(savePort);
								}
								
								switch (thePrefs.KeyUpMode) {
									case eStop:
										MADDriver->chan[TrackMem[x]].loopBeg = 0;
										MADDriver->chan[TrackMem[x]].loopSize = 0;
										MADDriver->chan[TrackMem[x]].maxPtr = MADDriver->chan[TrackMem[x]].curPtr;
										
										if (PianoRecording)
											NPianoRecordProcess(0xFF, 0xFF, 0x10, TrackMem[x]);
										break;
										
									case eNoteOFF:
										MADDriver->chan[TrackMem[x]].KeyOn = false;
										
										if (PianoRecording)
											NPianoRecordProcess(0xFE, 0xFF, 0xFF, TrackMem[x]);
										break;
								}
								
								if (MADDriver->DriverSettings.driverMode == MIDISoundDriver) {
									if (MADDriver->NoteOld[TrackMem[x]] != -1)
									{
										NoteOff(MADDriver->InstuNoOld[TrackMem[x]], MADDriver->NoteOld[TrackMem[x]], MADDriver->VelocityOld[TrackMem[x]], MADDriver);
										MADDriver->NoteOld[TrackMem[x]] = -1;
									}
								}
								
								SelectToucheMozart(-1, 0);
								
								TouchMem[x] = -1;
							}
						}
					}
				}
#endif
			}
			break;
			
		case mouseDown:
			DoMouseDown(theEvent);
			break;
			
		case updateEvt:
			DoUpdateEvent(&theEvent);
			break;
			
		case nullEvent:
			mainSystemDrag = true;
			if (!thePrefs.ThreadUse)
				DoGlobalNull();
			else
				YieldToAnyThread();
			break;
			
		case keyDown:
		case autoKey:
#if 0
			if (VisualDlog != NULL) {
				if (oldWindow == GetDialogWindow(VisualDlog)) {
					DoKeyVisual(VisualDlog);
				}
			}
#endif
			
		{
			for (i = 0; i < 15; i++) {
				if (FKEYCo[i] == (theEvent.message & keyCodeMask) >> 8L) {
					//PressFKey(i);
					break;
				}
			}
			
			theChar = theEvent.message & charCodeMask;
			
			
			if ((theEvent.modifiers & cmdKey) != 0) {
				HandleMenuChoice(MenuKey(theChar));
				
				if (theChar == deletekey) {
					if (GetWRefCon(oldWindow) == RefMODList) {
						GlobalDoKey(oldWindow, theChar);
					}
				}
				
				break;
			} else if (oldWindow != NULL) {
				if (AcceptKeysTools() || (theChar == 9))
					DoKeyPressCmdDlog(theChar);
				else
					GlobalDoKey(oldWindow, theChar);
			}
			
			//
			
			if (theChar == '+')
				DoSearchUp();
			else if (theChar == '-')
				DoSearchDown();
			if (theChar == 27)
				DoPause();	// ESC = Pause
			//	if (theChar == '/') DoChangeLoop();
			
			//
			
			if (theEvent.what != autoKey) {
				short	 ins;
				
				///
				
				if (oldWindow != NULL) {
					if (GetWRefCon(oldWindow) == RefSample)
						break;
				}
				
				if (AcceptKeysTools())
					break;
				
#if 0
				if (EditorDlog != NULL) {
					if (!GetIns(&ins, NULL) && oldWindow != GetDialogWindow(EditorDlog))
						break;
				}
#endif
				
				///
				
				if (thePrefs.MacKeyBoard) {
					i = ConvertCharToNote(theChar);
					if (i != -1) {
						short	track = GetWhichTrackPlay();
						short	eff = 0, arg = 0, volCmd = 0xFF;
						
#if 0
						if (PianoRecording)
							NPianoRecordProcess(i, -1, 0xFF, track);
						else if (EditorDlog != NULL) {
							if (oldWindow == GetDialogWindow(EditorDlog))
								DigitalEditorProcess(i, &eff, &arg, &volCmd);
						}
#endif
						
						SelectTouche(i, -1);
						SelectToucheMozart(i, 0);
						
						TouchIn++;
						if (TouchIn < 0 || TouchIn >= 10)
							TouchIn = 0;
						TouchMem[TouchIn] = i;
						TrackMem[TouchIn] = track;
						if (TrackMem[TouchIn] < 0 || TrackMem[TouchIn] >= MADDriver->DriverSettings.numChn) TrackMem[TouchIn] = 0;
						
						if (GetIns(&ins, NULL))
							DoPlayInstruInt(i, ins, eff, arg, volCmd, &MADDriver->chan[track], 0, 0);
					}
				}
			} else
				//DoNullInstrument();
				;
		}
			break;
	}
}

void PlayerPRO::PlayerPROApp::Run()
{
	short i;
	
	for(i = 0; i < 10; i++) TouchMem[i] = -1;
	for(i = 0; i < 10; i++) TrackMem[i] = 0;
	
	SetEventMask(everyEvent);
	//gCursorRgn = NewRgn();
	//SetEmptyRgn(gCursorRgn);
	
	gCursorRgn = NULL;
	
	//#if MACOS9VERSION
	do {
		EventLoop2();
	} while (End == false);
	//#else
	//	CarbonEventLoop();
	//#endif
	
	//TODO: Window registration?
	//InstallMenuEventHandler(window, NewEventHandlerUPP(MyWindowEventHandler), 3, list, 0, &ref);
}

pascal OSStatus CarbonWindowEventHandler(EventHandlerCallRef myHandler, EventRef event, void* userData)
{
#pragma unused (myHandler, userData)
	WindowRef			window;
	Rect				bounds;
	OSStatus			result = eventNotHandledErr;
	
	GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof(window), NULL, &window);
	
	if (GetEventKind(event) == kEventWindowDrawContent) {
		PlayerPRO::TheApp->theEvent.message = (unsigned long) window;
		DoUpdateEvent(&PlayerPRO::TheApp->theEvent);
		result = noErr;
	} else if (GetEventKind(event) == kEventWindowBoundsChanged) {
		InvalWindowRect(window, GetWindowPortBounds(window, &bounds));
		result = noErr;
	} else if (GetEventKind(event) == kEventWindowClose) {
		ClosePlayerWindow(GetDialogFromWindow(window));
		result = noErr;
	}
	
	return result;
}

pascal short MyGetDirHook(short item, DialogPtr dPtr)
{
	short	itemType,i;
	Handle	itemHandle;
	Rect	itemRect;
	
	switch (item) {
		case 10:
		case 11:
		case 12:
			for (i = 10; i <= 12; i++) {
				GetDialogItem(dPtr, i, &itemType, &itemHandle, &itemRect);
				SetControlValue((ControlHandle)itemHandle, 0);
			}
			GetDialogItem(dPtr, item, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle)itemHandle, 255);
			Quality = item;
			break;
			
		case 14:
		case 15:
			for (i = 14; i <= 15; i++) {
				GetDialogItem(dPtr, i, &itemType, &itemHandle, &itemRect);
				SetControlValue((ControlHandle)itemHandle, 0);
			}
			GetDialogItem (dPtr, item, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 255);
			Mode = item;
			break;
			
		case -1:
			GetDialogItem(dPtr, Mode, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle)itemHandle, 255);
			GetDialogItem(dPtr, Quality, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle)itemHandle, 255);
			break;
	}
	return item;
}

void PlayerPRO::PlayerPROApp::UseSameLeft(WindowPtr whichWindow)
{
	Rect caRect, stdRect;
	
	GetWindowStandardState(whichWindow, &stdRect);
	
	GetPortBounds(GetWindowPort(whichWindow), &caRect);
	
	stdRect.left = PlayerPRO::TheApp->thePrefs.WinPos[GetWRefCon(whichWindow)].h;
	stdRect.right = stdRect.left + caRect.right;
	
	SetWindowStandardState(whichWindow, &stdRect);
}

bool PlayerPRO::PlayerPROApp::PtRect(Point thePoint, Rect *theRect)
{
	if(thePoint.h <= theRect->right &&
	   thePoint.h >= theRect->left  &&
	   thePoint.v <= theRect->bottom &&
	   thePoint.v >= theRect->top)
		return true;
	else
		return false;
}

void PlayerPRO::PlayerPROApp::DrawVersion()
{
	Str255				strTemp;
	char				str2[300];
	Handle				aHandle;
	
	/**********************/
	ForeColor(whiteColor);
	TextFont(21);
	TextSize(12);
	
	pStrcpy((unsigned char*)versString, "\pVers:");
	
	aHandle = GetResource('vers', 1);
	pStrcpy(strTemp, (unsigned char*)(*aHandle) + 6L);
	ReleaseResource(aHandle);
	
	pStrcat((unsigned char*)versString, strTemp);
	pStrcpy((unsigned char*)str2, versString);
	
	MyP2CStr((unsigned char*)str2);
	WriteCText(GetDialogFromWindow(FrontWindow()), 2, (Ptr)str2);
	
	TextFont(0);
	TextSize(0);
	ForeColor(blackColor);
	/**********************/
}

void PlayerPRO::PlayerPROApp::StartDialog()
{
	short				itemType;
	Handle				itemHandle;
	Rect				itemRect;
	
	SetCursorOnNumber(watchCrsr);
	
	myStartUpDlog = GetNewDialog(171, NULL, (WindowPtr) -1L);
	if (myStartUpDlog == NULL)
		abort();
	SetPortDialogPort(myStartUpDlog);
	
	GetDialogItem(myStartUpDlog, 2, &itemType, &itemHandle, &itemRect);
	OffsetRect(&itemRect, 0, -50);
	SetDialogItem(myStartUpDlog, 2, itemType, itemHandle, &itemRect);
	
	DrawDialog(myStartUpDlog);
	WaitNextEvent(everyEvent, &theEvent, 1, NULL);
	
#ifdef DEBUG
	Rect caRect;
	
	ForeColor(redColor);
	
	GetPortBounds(GetDialogPort(myStartUpDlog), &caRect);
	
	MoveTo(5, caRect.top + 20);
	
	TextSize(20);
	
	DrawString("\pDebugging Mode");
	//Unless we do this, the "Debugging Mode" text won't display unless a window moves over it
	QDFlushPortBuffer(GetDialogPort(myStartUpDlog), NULL);
#endif
}

void PlayerPRO::PlayerPROApp::EndDialog()
{
	long		oldTicks;
	
	SetPortDialogPort(myStartUpDlog);
	thePrefs.NoStart++;
	WriteCFPreferences();
	
	ForeColor(whiteColor);
	TextFont(21);
	TextSize(12);
	
	oldTicks = 0;
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	ForeColor(blackColor);
	TextFont(0);
	TextSize(0);
	
	FlushEvents(everyEvent, 0);
	
	//Delay(180, &oldTicks);
	
	DisposeDialog(myStartUpDlog);
}

static OSType specificType;

pascal Boolean MyCustomFileFilter2(CInfoPBRec *pb, void *myDataPtr)
{
	short	i;
	FSSpec	spec;
	char	tempC[5];
	
	if (pb == NULL)  {
		MyDebugStr(__LINE__, __FILE__, "pb is NULL!");
		return false;
	}
	
	if (pb->hFileInfo.ioFlAttrib & 16)
		return false;
	
	switch (PlayerPRO::TheApp->showWhat) {
		case PlayerPRO::allMusics:
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'sTAT')
				return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'STCf')
				return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'MADK')
				return false;
			
			for (i = 0; i < PlayerPRO::TheApp->gMADLib->TotalPlug; i++) {
				//TODO: UTI Type
				if (pb->hFileInfo.ioFlFndrInfo.fdType == Ptr2OSType(PlayerPRO::TheApp->gMADLib->ThePlug[i].type))
					return false;
			}
			
			if (QTTypeConversion(pb->hFileInfo.ioFlFndrInfo.fdType))
				return false;
			break;
			
		case PlayerPRO::allReadable:
			RollCursor();
			
			if (EqualString("\pIcon", pb->hFileInfo.ioNamePtr, false, false)) MyDebugStr(__LINE__, __FILE__, "");
			
			
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'sTAT')
				return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'STCf')
				return false;
			
			FSMakeFSSpec(pb->hFileInfo.ioVRefNum, 0, pb->hFileInfo.ioNamePtr, &spec);
			
			if (MADMusicIdentifyFSp(PlayerPRO::TheApp->gMADLib, tempC, &spec) == noErr)
				return false;
			else return true;
			break;
			
		case PlayerPRO::allFiles:
			return false;
			break;
			
		case PlayerPRO::MADK:
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'MADK')
				return false;
			break;
			
		default:
			if (pb->hFileInfo.ioFlFndrInfo.fdType == specificType)
				return false;
			break;
	}
	
	return true;
}

void WriteSupportedFormat(DialogPtr	aDia)
{
	short	i;
	Str255	text;
	
	pStrcpy(text, "\pMADK");
	
	for (i = 0; i < PlayerPRO::TheApp->gMADLib->TotalPlug; i++) {
		Str255 pMenuName;
		GetPStrFromCFString(PlayerPRO::TheApp->gMADLib->ThePlug[i].MenuName, pMenuName);
		
		pStrcat(text, (StringPtr)"\p - ");
		pStrcat(text, pMenuName);
	}
	
	SetDText(aDia, 14, text);
}

static Boolean AddAll;

#if 0
pascal short MyDlgHook2(short item, DialogPtr theDialog, void *myDataPtr)
{
	Boolean				MiseAjour = false;
	//	StandardFileReply	*IntReply;
	GrafPtr				savePort;
	FInfo				fndrInfo;
	Str255				str;
	char				tempC[5];
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	if (myDataPtr == NULL) MyDebugStr(__LINE__, __FILE__, "");
	
	IntReply = (StandardFileReply*) myDataPtr;
	
	switch (item)
	{
		case getOpen:
			if (IntReply->sfType != 'sTAT' && IntReply->sfType != 'STCf')
			{
				OSType type;
				
				switch (showWhat)
				{
					case allReadable:
					case allFiles:
						MADMusicIdentifyFSp(gMADLib, tempC, &IntReply->sfFile);
						type = Ptr2OSType(tempC);
						break;
						
					default:
						type = IntReply->sfType;
						break;
				}
				
				if (type == '!!!!' || CheckFileType(IntReply->sfFile, type) == false)
				{
					Erreur(4, -86);
					item = sfHookNullEvent;
				}
				else
				{
					HSetVol(NULL, IntReply->sfFile.vRefNum, IntReply->sfFile.parID);
					// HGetFInfo(IntReply->sfFile.vRefNum, IntReply->sfFile.parID, IntReply->sfFile.name, &fndrInfo);
					// fndrInfo.fdCreator = 'SNPL';
					// SetFInfo(IntReply->sfFile.name, 0, &fndrInfo);
				}
			}
			break;
			
		case sfHookFirstCall:
			WriteSupportedFormat(theDialog);
			
			GetMenuItemText(showWhatMenu, showWhat, str);
			SetDText(theDialog, 19, str);
			break;
			
		case 19:
		case 20:
		{
			Point	Zone;
			short	itemType;
			Rect	itemRect;
			Handle	itemHandle;
			long	mresult;
			
			InsertMenu(showWhatMenu, hierMenu );
			GetDialogItem(theDialog, 20, &itemType, &itemHandle, &itemRect);
			
			Zone.h = itemRect.right - 13;
			Zone.v = itemRect.top;
			
			LocalToGlobal(&Zone);
			
			SetItemMark(showWhatMenu, showWhat, 0xa5);
			mresult = PopUpMenuSelect(	showWhatMenu,
									  Zone.v,
									  Zone.h,
									  showWhat );
			SetItemMark(showWhatMenu, showWhat, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				if (LoWord(mresult) != showWhat)
				{
					showWhat = LoWord(mresult);
					
					GetMenuItemText(showWhatMenu, showWhat, str);
					SetDText(theDialog, 19, str);
					
					specificType = plugListO[showWhat];
					
					item = sfHookRebuildList;
				}
			}
			
			DeleteMenu(GetMenuID(showWhatMenu));
		}
			break;
			
		case 16:
			item = getOpen;
			
			AddAll = true;
			break;
			
		case 17:	// Preview
			if (IntReply->sfType != 'sTAT' && IntReply->sfType != 'STCf')
			{
				OSType type;
				
				switch (showWhat)
				{
					case allReadable:
					case allFiles:
						MADMusicIdentifyFSp(gMADLib, tempC, &IntReply->sfFile);
						type = Ptr2OSType(tempC);
						break;
						
					default:
						type = IntReply->sfType;
						break;
				}
				
				if (type == '!!!!' || CheckFileType(IntReply->sfFile, type) == false)
				{
					Erreur(4, -86);
					item = sfHookNullEvent;
				}
				else
				{
					MADMusic	*savedPartition;
					
					//	if (MADCopyCurrentPartition(&savedPartition) == noErr)
					{
						MADDriver->Reading = false;
						MADPurgeTrack(MADDriver);
						MADCleanDriver(MADDriver);
						
						DoStop();
						
						HSetVol(NULL, IntReply->sfFile.vRefNum, IntReply->sfFile.parID);
						
						SetCursor(&PlayCrsr);
						
						OSType2Ptr(type, tempC);
						
						if (MADPlugAvailable(gMADLib, tempC))		// Is available a plug to open this file?
						{
							if (MADLoadMusicFilePString(gMADLib, &savedPartition, tempC, IntReply->sfFile.name) == noErr)			// Load this music with help of Plugs
								// in application folder or in 'Plugs' folder
							{
								MADAttachDriverToMusic(MADDriver, savedPartition, NULL);
								
								MADCheckSpeed(savedPartition, MADDriver);
								MADDriver->Reading = true;		// Read the current partition in memory
								
								while (!Button())
								{
									/** Do what you want here....					**/
									/** Bla bla...									**/
									/** By example:	Run your realtime 3D game... 	**/
								}
								while (Button()) {};
								
								MADDriver->Reading = false;		// Stop reading current partition
								//	MADStopDriver();						// Stop driver interrupt function
								
								MADAttachDriverToMusic(MADDriver, curMusic, NULL);
								
								MADDisposeMusic(&savedPartition, MADDriver);				// Dispose the current music
							}
						}
						
						FlushEvents(everyEvent,0);
						
						//	MADDriver->Reading = readingCopy;
						
						SetCursor(GetQDGlobalsArrow(&qdarrow));
					}
				}
			}
			break;
			
		case 10:	// Add
			if (IntReply->sfType != 'sTAT' && IntReply->sfType != 'STCf')
			{
				OSType type;
				
				switch (showWhat)
				{
					case allReadable:
					case allFiles:
						MADMusicIdentifyFSp(gMADLib, tempC, &IntReply->sfFile);
						type = Ptr2OSType(tempC);
						break;
						
					default:
						type = IntReply->sfType;
						break;
				}
				
				if (type == '!!!!' || CheckFileType(IntReply->sfFile, type) == false)
				{
					Erreur(4, -86);
					item = sfHookNullEvent;
				}
				else
				{
					HSetVol(NULL, IntReply->sfFile.vRefNum, IntReply->sfFile.parID);
					
					AddMODList(true,
							   IntReply->sfFile.name,
							   IntReply->sfFile.vRefNum,
							   IntReply->sfFile.parID);
				}
			}
			else
			{
				ImportFile(IntReply->sfFile.name, IntReply->sfFile.vRefNum, IntReply->sfFile.parID, IntReply->sfType);
			}
			break;
			
		case -5:
		case sfHookNullEvent:
			if (needUp)
			{
				Str255		a;
				OSType		type;
				
				if ((showWhat == allFiles || showWhat == allReadable) && MADMusicIdentifyFSp(gMADLib, tempC, &IntReply->sfFile) == noErr)
				{
					type = Ptr2OSType(tempC);
					OSType2Str(type, a);
				}
				else
				{
					if (HGetFInfo(IntReply->sfFile.vRefNum, IntReply->sfFile.parID, IntReply->sfFile.name, &fndrInfo) == noErr)
					{
						OSType2Str(fndrInfo.fdType, a);
					}
					else a[0] = 0;
				}
				SetDText(theDialog, 11, a);
				
				SetCursor(GetQDGlobalsArrow(&qdarrow));
				
				needUp = false;
			}
			break;
			
		case sfHookOpenFolder:
		case sfItemFileListUser:
		case sfItemDesktopButton:
		case sfHookSetActiveOffset:
			needUp = true;
			break;
			
		default:
			if ((item & 0xF000) == 0x1000) needUp = true;
			break;
	}
	
	SetPort(savePort);
	
	return item;
}
#endif

OSErr PlayerPRO::PlayerPROApp::NGetFileName(FSSpec *spec)
{
	return DoCustomOpen(spec);
}

void PlayerPRO::PlayerPROApp::InitImportMenu()
{
	short i;
	
	ExportMenu = GetMenu(139);
	
	for (i = 0; i < gMADLib->TotalPlug; i++) {
		Str255 pMenuName;
		GetPStrFromCFString(gMADLib->ThePlug[i].MenuName, pMenuName);
		
		switch (gMADLib->ThePlug[i].mode) {
			case 'EXIM':
			case 'EXPL':
				AppendMenu(ExportMenu, pMenuName);
				break;
		}
	}
	
	InsertMenu(ExportMenu, hierMenu);
}


short PlayerPRO::PlayerPROApp::IntInfoL(short ID)
{
	short				itemHit;
	Str255				theString;
	GrafPtr				savedPort;
	DialogPtr			aDia;
	ProcessSerialNumber	PSN;
	
	GetFrontProcess(&PSN);
	if (PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN)
		SetFrontProcess(&PSN);
	
	GetPort(&savedPort);
	
	GetIndString(theString, 128, ID);
	ParamText(theString, NULL, NULL, NULL);
	
	aDia = GetNewDialog(3001, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	
	SetCursorToQDArrow();
	
	do {
		MyModalDialog(aDia, &itemHit);
	} while (itemHit != 1 && itemHit != 2);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
	
	if (itemHit == 1)
		return 1;
	else
		return 0;
}

short PlayerPRO::PlayerPROApp::InfoL(short ID)
{
	ProcessSerialNumber		PSN;
	
	GetFrontProcess(&PSN);
	if(PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
	   PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) {
		return true;
	} else
		return IntInfoL(ID);
}

void PlayerPRO::PlayerPROApp::MADErreur(OSErr err)
{
	switch (err) {
		case noErr:
			break;
			
		case MADOrderNotImplemented:
			Erreur(67, err);
			break;
			
		case MADNeedMemory:
			Erreur(68, err);
			break;
			
		case MADFileNotSupportedByThisPlug:
			Erreur(69, err);
			break;
			
		case MADUnknowErr:
		default:
			Erreur(70, err);
			break;
			
		case MADCannotFindPlug:
			Erreur(71, err);
			break;
	}
}

void PlayerPRO::PlayerPROApp::IntErreur(short ID, OSErr theErr)
{
	short					itemHit;
	DialogPtr				aDia;
	Str255					theString, theString2, theMemStr, theRsrStr;
	GrafPtr					savedPort;
	
	GetPort(&savedPort);
	
	GetIndString(theString, 128, ID);
	NumToString(theErr, theString2);
	
	NumToString(MemError(), theMemStr);
	NumToString(ResError(), theRsrStr);
	
	aDia = GetNewDialog(3000, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	ParamText(theString, theString2, theMemStr, theRsrStr);
	
	AutoPosition(aDia);
	
	SetCursorToQDArrow();
	
	do {
		MyModalDialog(aDia, &itemHit);
	} while (itemHit != 1);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
}

void PlayerPRO::PlayerPROApp::OtherIntErreur(short ID, OSErr theErr, Str255 otherstr)
{
	short		itemHit;
	DialogPtr	aDia;
	Str255		theString, theString2, theMemStr, theRsrStr;
	GrafPtr		savedPort;
	
	GetPort(&savedPort);
	
	GetIndString(theString, 128, ID);
	NumToString(theErr, theString2);
	
	NumToString(MemError(), theMemStr);
	NumToString(ResError(), theRsrStr);
	
	aDia = GetNewDialog(3000, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	ParamText(theString, theString2, theMemStr, theRsrStr);
	
	SetDText(aDia, 9, otherstr);
	
	AutoPosition(aDia);
	
	SetCursorToQDArrow();
	
	do {
		MyModalDialog(aDia, &itemHit);
	} while (itemHit != 1);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
}


void PlayerPRO::PlayerPROApp::Erreur(short ID, OSErr theErr)
{
	ProcessSerialNumber PSN;
	
	GetFrontProcess(&PSN);
	if (PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN ||
		UseAEErreur == true) {
		OSType		sign = 'SNPL';
		AppleEvent	aeEvent, reply;
		AEDesc		target, listElem, fileList;
		OSErr		err[2], iErr;
		
		iErr = AECreateDesc(typeApplSignature,
							(Ptr) &sign,
							sizeof(sign),
							&target);
		
		iErr = AECreateAppleEvent(kPlayerPROClass,
								  kErreurID,
								  &target,
								  kAutoGenerateReturnID,
								  kAnyTransactionID,
								  &aeEvent);
		
		iErr = AECreateList(NULL, 0, false, &fileList);
		
		err[0] = ID;
		err[1] = theErr;
		AECreateDesc('dErr', (Ptr)&err, sizeof(err), &listElem);
		
		iErr = AEPutDesc(&fileList, 0, &listElem);
		if (iErr)
			return;
		
		AEDisposeDesc(&listElem);
		
		iErr = AEPutParamDesc(&aeEvent, keyDirectObject,&fileList);
		if(iErr)
			return;
		iErr = AEDisposeDesc(&fileList);
		
		//
		
		iErr = AESend(&aeEvent,
					  &reply,
					  kAENoReply,
					  kAENormalPriority,
					  kAEDefaultTimeout,
					  NULL,
					  NULL);
	} else
		IntErreur(ID, theErr);
}

void PlayerPRO::PlayerPROApp::DeletePreferences()
{
	ResetCFPreferences();
}

void PlayerPRO::PlayerPROApp::LoadOldFilePrefs(FSIORefNum fRefNum)
{
#pragma pack(push, 2)
	struct {
		short			Version;
		Boolean			Loop;
		Boolean			Stereo;
		Boolean			PPCMachine;
		Boolean 		xxxxxxxx;
		Fixed			Frequence;
		Boolean			LargeWindow;
		Boolean			AutoCreator;
		Boolean			MicroRecording;
		short			oldShort;
		Boolean			AffichageDIGI;
		short			NoStart;
		Point			a1[30];
		short			a2[30];
		short			PianoKey[300];
		Boolean			oldBool;
		Boolean			InstruListBig;
		Boolean			PatListBig;
		Boolean			AutoScroll;
		short			LoopType;
		short			a3[30];
		Boolean			Registred;
		long			SNumber;
		Byte			Decode;
		Str32			AutorName;
		long			checkSum;
		long			Mz;
		unsigned long	firstStart;
		short			a4[30];
		RGBColor		tracksColor[MAXTRACK];
		short			unused[MAXTRACK];
		short			PianoPos;
		short			volumeLevel;
		Boolean			MADCompression;
		Boolean			SSText;
		Boolean			SSStars;
		Boolean			SSJumping;
		Boolean			FText;
		Boolean			FStars;
		Boolean			FBalls;
		short			TextS;
		Boolean			ThreadUse;
		Boolean			FSinScroll;
		Boolean			RememberMusicList;
		Boolean			OscilloLine;
		long			a5[30];
		Boolean			DigitalInstru;
		Boolean			DigitalNote;
		Boolean			DigitalEffect;
		Boolean			DigitalArgu;
		Boolean			DigitalVol;
		Boolean			GoToStop;
		Boolean			DriverEffects[20];
		Boolean			MADC;
		short			OscilloSize;
		short			OscilloType;
		short			fileTypeExportSND;
		OSType			CompressionExportSND;
		Boolean			ActiveHelp;
		short			SpectrumSize;
		short			SpectrumType;
		
		/** Driver Settings **/
		
		short			numChn;								// Active tracks from 2 to 32, automatically setup when a new music is loaded
		short			outPutBits;							// 8 or 16 Bits
		UnsignedFixed	outPutRate;							// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
		short			outPutMode;							// MonoOutPut, StereoOutPut or DeluxeStereoOutPut ?
		short			driverMode;							// ASCSoundDriver, AWACSoundDriver, MIDISoundDriver or SoundManagerDriver
		Boolean			antiAliasing;						// Use AntiAliasing filter ?
		
		/** Mozart Prefs **/
		
		Boolean			UseOctaveMarkers;
		Boolean			UseMarkers;
		short			MarkersSize;
		short			MarkersOffSet;
		short			MozartX;
		
		/** News 4.5	**/
		
		Boolean			SpectrumScale;
		Boolean			ClassicalProjection;
		Boolean			PianoOctaveMarkers;
		Boolean			SmallPiano;
		Boolean			FastMusicList;
		long			FastDigitalEdition;
		
		Boolean			MacKeyBoard;
		Boolean			MidiKeyBoard;
		Boolean			QKMidiKeyBoard;
		
		Boolean			MIDIVelocity;
		Boolean			MIDIChanInsTrack;
		
		short			KeyUpMode;
		
		/** News 4.5.1	**/
		
		Boolean			Interpolation;
		Boolean			MicroDelay;
		long			MicroDelaySize;
		Boolean			surround;
		
		/** News 4.5.2	**/
		
		Boolean			OCArrow[MAXINSTRU];
		short			MozartC1h, MozartC2h;
		OSType			SoundTypeSamp;
		OSType			SoundTypeIns;
		short			LinesHeight;
		Boolean			SaveMusicList;
		short			softVolumeLevel;
		
		Boolean			oADAPremember;
		Boolean			oADAPsave;
		Boolean			oADAPuse;
		
		Boolean			osciTile;
		
		Boolean			addExtension;
		
		Boolean			AutoPlayWhenOpen;
		
		Boolean			Reverb;
		long			ReverbSize;
		long			ReverbStrength;
		
		Boolean			oldDirectVideo;
		
		/** News 4.5.3 **/
		
		Boolean			TickRemover;
		Boolean			AutomaticOpen;
		Boolean			FinePositioning;
		
		short			ChannelType;
		short			amplitude;
		OSType			Compressor;
		Fixed			FrequenceSpeed;
		
		Byte			RecordAllTrack;			// 0 : a track, 1 : all tracks, 2 : current track
		
		/** News 4.5.4	**/
		
		Boolean			StaffShowAllNotes;
		Boolean			StaffShowLength;
		short			TempsNum;
		short			TempsUnit;
		short			TrackHeight;
		
		short			FKeyActive[20];
		short			FKeyItem[20];
		short			FKeyWind[20];
		
		/** News 4.5.7 **/
		
		Boolean			NewPrefSystem;
		unsigned char	NewPrefsCode[30];
		
		/** News 4.5.8 **/
		
		Boolean			keyMapNote;
		
		/** News 5.0.0 **/
		
		RGBColor		yellC;
		short			whichEditorPatterns;
		Boolean			MusicTrace;
		long			oversampling;
		
		/** News 5.0.3 **/
		
		short			RAWBits;
		long			RAWRate;
		Boolean			RAWStereo;
		Boolean			RAWSigned;
		Boolean			RAWLittleEndian;
		Boolean			RAWEOF;
		long			RAWLength;
		long			RAWHeader;
		
		Byte			SelectedTracks[MAXTRACK];
		
		Point			a6[3][30];
		short			a7[3][30];
		short			a8[3][30];
		short			a9[3][30];
		long			a10[3][30];
		Str32			WinNames[3];
		
		// 5.2
		Boolean			clickSound;
		Boolean			patternWrapping;
		Boolean			SendMIDIClockData;
		short			pianoOffset;
		
		// 5.3
		Boolean				DontUseFilesMix;
		struct OLDMADSpec	previousSpec;
		
		// 5.5
		
		Point			WinPosO[3][MAXWINDOWS];
		short			WinEtatO[3][MAXWINDOWS];
		short			WinLargO[3][MAXWINDOWS];
		short			WinHiO[3][MAXWINDOWS];
		long			WinIDO[3][MAXWINDOWS];
		Point			WinPos[MAXWINDOWS];
		short			WinEtat[MAXWINDOWS];
		short			WinHi[MAXWINDOWS];
		short			WinLarg[MAXWINDOWS];
		long			WinID[MAXWINDOWS];
		double			Filter[EQPACKET * 2];
		Boolean			useEQ;
		
		// 5.6
		
		short				lastVisualPlugin;
		Boolean				editorSoundDrag;
		MADDriverSettings	DirectDriverType;
		short				channelNumber;
		Boolean				FKeyTracks;
		
		// 5.7
		
		long				Previous_globalEffect[10];
		Boolean				Previous_globalFXActive;
		long				Previous_chanEffect[MAXTRACK][4];
		FXBus				Previous_chanBus[MAXTRACK];
		FXSets				Previous_Sets[MAXTRACK];
		
		// 5.8
		
		Str255				ASIODriverName;
	} oldPrefs;
#pragma pack(pop)
	OSErr theErr;
	SInt64	inOutBytes;
	
	theErr = FSGetForkSize(fRefNum, &inOutBytes);
	if (inOutBytes == 0) {
		theErr = FSCloseFork(fRefNum);
		return;
	}
	theErr = FSReadFork(fRefNum, fsAtMark, 0, inOutBytes, &oldPrefs, NULL);
	theErr = FSCloseFork(fRefNum);
#ifdef __LITTLE_ENDIAN__
	{
		int i, x;
		oldPrefs.Version = EndianS16_BtoN(oldPrefs.Version);
		oldPrefs.Frequence = EndianS32_BtoN(oldPrefs.Frequence);
		oldPrefs.oldShort = EndianS16_BtoN(oldPrefs.oldShort);
		oldPrefs.NoStart = EndianS16_BtoN(oldPrefs.NoStart);
		for (i=0; i < 30; i++) {
			oldPrefs.a1[i].v = EndianS16_BtoN(oldPrefs.a1[i].v);
			oldPrefs.a1[i].h = EndianS16_BtoN(oldPrefs.a1[i].h);
			oldPrefs.a2[i] = EndianS16_BtoN(oldPrefs.a2[i]);
			oldPrefs.a3[i] = EndianS16_BtoN(oldPrefs.a3[i]);
			oldPrefs.a4[i] = EndianS16_BtoN(oldPrefs.a4[i]);
			oldPrefs.a5[i] = EndianS32_BtoN(oldPrefs.a5[i]);
			for (x = 0; x<3; x++) {
				oldPrefs.a6[x][i].v = EndianS16_BtoN(oldPrefs.a6[x][i].v);
				oldPrefs.a6[x][i].h = EndianS16_BtoN(oldPrefs.a6[x][i].h);
				oldPrefs.a7[x][i] = EndianS16_BtoN(oldPrefs.a7[x][i]);
				oldPrefs.a8[x][i] = EndianS16_BtoN(oldPrefs.a8[x][i]);
				oldPrefs.a9[x][i] = EndianS16_BtoN(oldPrefs.a9[x][i]);
				oldPrefs.a10[x][i] = EndianS32_BtoN(oldPrefs.a10[x][i]);
			}
		}
		for (i=0; i < 300; i++) {
			oldPrefs.PianoKey[i] = EndianS16_BtoN(oldPrefs.PianoKey[i]);
		}
		
		oldPrefs.LoopType = EndianS16_BtoN(oldPrefs.LoopType);
		oldPrefs.SNumber = EndianS32_BtoN(oldPrefs.SNumber);
		oldPrefs.checkSum = EndianS32_BtoN(oldPrefs.checkSum);
		oldPrefs.Mz = EndianS32_BtoN(oldPrefs.Mz);
		oldPrefs.firstStart = EndianU32_BtoN(oldPrefs.firstStart);
		for (i=0; i < MAXTRACK; i++) {
			oldPrefs.tracksColor[i].red = EndianU16_BtoN(oldPrefs.tracksColor[i].red);
			oldPrefs.tracksColor[i].green = EndianU16_BtoN(oldPrefs.tracksColor[i].green);
			oldPrefs.tracksColor[i].blue = EndianU16_BtoN(oldPrefs.tracksColor[i].blue);
			//		oldPrefs.unused[i] = EndianS16_BtoL(oldPrefs.unused[i]);
			oldPrefs.Previous_chanBus[i].copyId = EndianS16_BtoL(oldPrefs.Previous_chanBus[i].copyId);
			oldPrefs.Previous_Sets[i].track = EndianS16_BtoL(oldPrefs.Previous_Sets[i].track);
			oldPrefs.Previous_Sets[i].id = EndianS16_BtoL(oldPrefs.Previous_Sets[i].id);
			oldPrefs.Previous_Sets[i].FXID = EndianS32_BtoL(oldPrefs.Previous_Sets[i].FXID);
			oldPrefs.Previous_Sets[i].noArg = EndianS16_BtoL(oldPrefs.Previous_Sets[i].noArg);
			for (x=0; x < 4; x++) {
				oldPrefs.Previous_chanEffect[i][x] = EndianS32_BtoN(oldPrefs.Previous_chanEffect[i][x]);
			}
		}
		
		oldPrefs.PianoPos = EndianS16_BtoN(oldPrefs.PianoPos);
		oldPrefs.volumeLevel = EndianS16_BtoN(oldPrefs.volumeLevel);
		oldPrefs.TextS = EndianS16_BtoN(oldPrefs.TextS);
		oldPrefs.OscilloSize = EndianS16_BtoN(oldPrefs.OscilloSize);
		oldPrefs.OscilloType = EndianS16_BtoN(oldPrefs.OscilloType);
		oldPrefs.fileTypeExportSND = EndianS16_BtoN(oldPrefs.fileTypeExportSND);
		oldPrefs.CompressionExportSND = EndianU32_BtoN(oldPrefs.CompressionExportSND);
		oldPrefs.SpectrumSize = EndianS16_BtoN(oldPrefs.SpectrumSize);
		oldPrefs.SpectrumType = EndianS16_BtoN(oldPrefs.SpectrumType);
		oldPrefs.numChn = EndianS16_BtoN(oldPrefs.numChn);
		oldPrefs.outPutBits = EndianS16_BtoN(oldPrefs.outPutBits);
		oldPrefs.outPutRate = EndianU32_BtoN(oldPrefs.outPutRate);
		oldPrefs.outPutMode = EndianS16_BtoN(oldPrefs.outPutMode);
		oldPrefs.driverMode = EndianS16_BtoN(oldPrefs.driverMode);
		oldPrefs.MarkersSize = EndianS16_BtoN(oldPrefs.MarkersSize);
		oldPrefs.MarkersOffSet = EndianS16_BtoN(oldPrefs.MarkersOffSet);
		oldPrefs.MozartX = EndianS16_BtoN(oldPrefs.MozartX);
		oldPrefs.FastDigitalEdition = EndianS32_BtoN(oldPrefs.FastDigitalEdition);
		oldPrefs.KeyUpMode = EndianS16_BtoN(oldPrefs.KeyUpMode);
		oldPrefs.MicroDelaySize = EndianS32_BtoN(oldPrefs.MicroDelaySize);
		oldPrefs.MozartC1h = EndianS16_BtoN(oldPrefs.MozartC1h);
		oldPrefs.MozartC2h = EndianS16_BtoN(oldPrefs.MozartC2h);
		oldPrefs.SoundTypeSamp = EndianU32_BtoN(oldPrefs.SoundTypeSamp);
		oldPrefs.SoundTypeIns = EndianU32_BtoN(oldPrefs.SoundTypeIns);
		oldPrefs.LinesHeight = EndianS16_BtoN(oldPrefs.LinesHeight);
		oldPrefs.softVolumeLevel = EndianS16_BtoN(oldPrefs.softVolumeLevel);
		oldPrefs.ReverbSize = EndianS32_BtoN(oldPrefs.ReverbSize);
		oldPrefs.ReverbStrength = EndianS32_BtoN(oldPrefs.ReverbStrength);
		oldPrefs.ChannelType = EndianS16_BtoN(oldPrefs.ChannelType);
		oldPrefs.amplitude = EndianS16_BtoN(oldPrefs.amplitude);
		oldPrefs.Compressor = EndianU32_BtoN(oldPrefs.Compressor);
		oldPrefs.FrequenceSpeed = EndianS32_BtoN(oldPrefs.FrequenceSpeed);
		oldPrefs.TempsNum = EndianS16_BtoN(oldPrefs.TempsNum);
		oldPrefs.TempsUnit = EndianS16_BtoN(oldPrefs.TempsUnit);
		oldPrefs.TrackHeight = EndianS16_BtoN(oldPrefs.TrackHeight);
		
		for (i=0; i < 20; i++) {
			oldPrefs.FKeyActive[i] = EndianS16_BtoN(oldPrefs.FKeyActive[i]);
			oldPrefs.FKeyItem[i] = EndianS16_BtoN(oldPrefs.FKeyItem[i]);
			oldPrefs.FKeyWind[i] = EndianS16_BtoN(oldPrefs.FKeyWind[i]);
		}
		
		oldPrefs.yellC.red = EndianU16_BtoN(oldPrefs.yellC.red);
		oldPrefs.yellC.green = EndianU16_BtoN(oldPrefs.yellC.green);
		oldPrefs.yellC.blue = EndianU16_BtoN(oldPrefs.yellC.blue);
		
		oldPrefs.whichEditorPatterns = EndianS16_BtoN(oldPrefs.whichEditorPatterns);
		oldPrefs.oversampling = EndianS32_BtoN(oldPrefs.oversampling);
		oldPrefs.RAWBits = EndianS16_BtoN(oldPrefs.RAWBits);
		oldPrefs.RAWRate = EndianS32_BtoN(oldPrefs.RAWRate);
		oldPrefs.RAWLength = EndianS32_BtoN(oldPrefs.RAWLength);
		oldPrefs.RAWHeader = EndianS32_BtoN(oldPrefs.RAWHeader);
		oldPrefs.pianoOffset = EndianS16_BtoN(oldPrefs.pianoOffset);
		oldPrefs.previousSpec.MAD = EndianU32_BtoN(oldPrefs.previousSpec.MAD);
		oldPrefs.previousSpec.EPitch = EndianS32_BtoN(oldPrefs.previousSpec.EPitch);
		oldPrefs.previousSpec.ESpeed = EndianS32_BtoN(oldPrefs.previousSpec.ESpeed);
		oldPrefs.previousSpec.speed = EndianS16_BtoN(oldPrefs.previousSpec.speed);
		oldPrefs.previousSpec.tempo = EndianS16_BtoN(oldPrefs.previousSpec.tempo);
		
		for (x = 0; x < EQPACKET*2; x++)
		{
			union {
				UInt64 inte;
				double doub;
			} v;
			v.doub = oldPrefs.Filter[x];
			v.inte = EndianU64_BtoN(v.inte);
			oldPrefs.Filter[x] = v.doub;
		}
		for (i=0; i < MAXWINDOWS; i++) {
			oldPrefs.WinPos[i].v = EndianS16_BtoN(oldPrefs.WinPos[i].v);
			oldPrefs.WinPos[i].h = EndianS16_BtoN(oldPrefs.WinPos[i].h);
			oldPrefs.WinEtat[i] = EndianS16_BtoN(oldPrefs.WinEtat[i]);
			oldPrefs.WinHi[i] = EndianS16_BtoN(oldPrefs.WinHi[i]);
			oldPrefs.WinLarg[i] = EndianS16_BtoN(oldPrefs.WinLarg[i]);
			oldPrefs.WinID[i] = EndianS32_BtoN(oldPrefs.WinID[i]);
			
			for (x = 0; x < 3; x++) {
				oldPrefs.WinPosO[x][i].v = EndianS16_BtoN(oldPrefs.WinPosO[x][i].v);
				oldPrefs.WinPosO[x][i].h = EndianS16_BtoN(oldPrefs.WinPosO[x][i].h);
				oldPrefs.WinEtatO[x][i] = EndianS16_BtoN(oldPrefs.WinEtatO[x][i]);
				oldPrefs.WinLargO[x][i] = EndianS16_BtoN(oldPrefs.WinLargO[x][i]);
				oldPrefs.WinIDO[x][i] = EndianS32_BtoN(oldPrefs.WinIDO[x][i]);
				oldPrefs.WinHiO[x][i] = EndianS16_BtoN(oldPrefs.WinHiO[x][i]);
			}
		}
		
		oldPrefs.lastVisualPlugin = EndianS16_BtoN(oldPrefs.lastVisualPlugin);
		oldPrefs.channelNumber = EndianS16_BtoN(oldPrefs.channelNumber);
		
		oldPrefs.DirectDriverType.numChn = EndianS16_BtoN(oldPrefs.DirectDriverType.numChn);
		oldPrefs.DirectDriverType.outPutBits = EndianS16_BtoN(oldPrefs.DirectDriverType.outPutBits);
		oldPrefs.DirectDriverType.outPutRate = EndianU32_BtoN(oldPrefs.DirectDriverType.outPutRate);
		oldPrefs.DirectDriverType.outPutMode = EndianS16_BtoN(oldPrefs.DirectDriverType.outPutMode);
		oldPrefs.DirectDriverType.driverMode = EndianS16_BtoN(oldPrefs.DirectDriverType.driverMode);
		oldPrefs.DirectDriverType.MicroDelaySize = EndianS32_BtoN(oldPrefs.DirectDriverType.MicroDelaySize);
		oldPrefs.DirectDriverType.ReverbSize = EndianS32_BtoN(oldPrefs.DirectDriverType.ReverbSize);
		oldPrefs.DirectDriverType.ReverbStrength = EndianS32_BtoN(oldPrefs.DirectDriverType.ReverbStrength);
		oldPrefs.DirectDriverType.oversampling = EndianS32_BtoN(oldPrefs.DirectDriverType.oversampling);
		for (i=0; i < 10; i++) {
			oldPrefs.Previous_globalEffect[i] = EndianS16_BtoN(oldPrefs.Previous_globalEffect[i]);
		}
		
	}
#endif
	if (oldPrefs.Version == VERSION && inOutBytes == sizeof(oldPrefs)) {
		// SUCCES
#define MoveBasicDatatype(val) thePrefs.val = oldPrefs.val
#define MoveArray(val) memcpy(thePrefs.val, oldPrefs.val, sizeof(thePrefs.val))
		MoveBasicDatatype(yellC);
		MoveBasicDatatype(addExtension);
		MoveBasicDatatype(amplitude);
		MoveBasicDatatype(antiAliasing);
		MoveBasicDatatype(AutoCreator);
		MoveBasicDatatype(AutomaticOpen);
		MoveBasicDatatype(AutoPlayWhenOpen);
		MoveBasicDatatype(channelNumber);
		MoveBasicDatatype(ClassicalProjection);
		MoveBasicDatatype(clickSound);
		MoveBasicDatatype(Compressor);
		MoveBasicDatatype(DigitalArgu);
		MoveBasicDatatype(DigitalEffect);
		MoveBasicDatatype(DigitalInstru);
		MoveBasicDatatype(DigitalNote);
		MoveBasicDatatype(DigitalVol);
		MoveBasicDatatype(DirectDriverType);
		MoveBasicDatatype(DontUseFilesMix);
		MoveBasicDatatype(driverMode);
		MoveBasicDatatype(editorSoundDrag);
		MoveBasicDatatype(FastDigitalEdition);
		MoveBasicDatatype(FastMusicList);
		//MoveBasicDatatype(FBalls);
		MoveArray(Filter);
		MoveBasicDatatype(FinePositioning);
		MoveArray(FKeyActive);
		MoveArray(FKeyItem);
		MoveBasicDatatype(FKeyTracks);
		MoveBasicDatatype(FrequenceSpeed);
		//MoveBasicDatatype(FSinScroll);
		//MoveBasicDatatype(FStars);
		//MoveBasicDatatype(FText);
		MoveBasicDatatype(GoToStop);
		MoveBasicDatatype(keyMapNote);
		MoveBasicDatatype(KeyUpMode);
		MoveBasicDatatype(lastVisualPlugin);
		MoveBasicDatatype(LinesHeight);
		MoveBasicDatatype(LoopType);
		MoveBasicDatatype(MacKeyBoard);
		MoveBasicDatatype(MADCompression);
		MoveBasicDatatype(MarkersOffSet);
		MoveBasicDatatype(MarkersSize);
		MoveBasicDatatype(MicroDelaySize);
		MoveBasicDatatype(MIDIChanInsTrack);
		MoveBasicDatatype(MidiKeyBoard);
		MoveBasicDatatype(MIDIVelocity);
		MoveBasicDatatype(MozartC1h);
		MoveBasicDatatype(MozartC2h);
		MoveBasicDatatype(MozartX);
		MoveBasicDatatype(MusicTrace);
		MoveBasicDatatype(NoStart);
		MoveBasicDatatype(numChn);
		MoveArray(OCArrow);
		MoveBasicDatatype(OscilloLine);
		MoveBasicDatatype(OscilloSize);
		MoveBasicDatatype(OscilloType);
		MoveBasicDatatype(osciTile);
		MoveBasicDatatype(outPutBits);
		MoveBasicDatatype(outPutMode);
		MoveBasicDatatype(outPutRate);
		MoveBasicDatatype(oversampling);
		MoveBasicDatatype(patternWrapping);
		MoveArray(PianoKey);
		MoveBasicDatatype(PianoOctaveMarkers);
		MoveBasicDatatype(pianoOffset);
		MoveBasicDatatype(PianoPos);
		MoveArray(Previous_chanBus);
		MoveArray(Previous_chanEffect);
		MoveArray(Previous_globalEffect);
		MoveArray(Previous_Sets);
		MoveBasicDatatype(Previous_globalFXActive);
		MoveBasicDatatype(previousSpec);
		MoveBasicDatatype(RAWBits);
		MoveBasicDatatype(RAWEOF);
		MoveBasicDatatype(RAWHeader);
		MoveBasicDatatype(RAWLength);
		MoveBasicDatatype(RAWLittleEndian);
		MoveBasicDatatype(RAWRate);
		MoveBasicDatatype(RAWSigned);
		MoveBasicDatatype(RAWStereo);
		MoveBasicDatatype(RecordAllTrack);
		MoveBasicDatatype(RememberMusicList);
		MoveBasicDatatype(Reverb);
		MoveBasicDatatype(ReverbSize);
		MoveBasicDatatype(ReverbStrength);
		MoveBasicDatatype(SaveMusicList);
		MoveArray(SelectedTracks);
		MoveBasicDatatype(SendMIDIClockData);
		MoveBasicDatatype(SmallPiano);
		MoveBasicDatatype(softVolumeLevel);
		MoveBasicDatatype(SoundTypeIns);
		MoveBasicDatatype(SoundTypeSamp);
		MoveBasicDatatype(SpectrumScale);
		MoveBasicDatatype(SpectrumSize);
		MoveBasicDatatype(SpectrumType);
		//MoveBasicDatatype(SSJumping);
		//MoveBasicDatatype(SSStars);
		//MoveBasicDatatype(SSText);
		MoveBasicDatatype(StaffShowAllNotes);
		MoveBasicDatatype(StaffShowLength);
		MoveBasicDatatype(surround);
		MoveBasicDatatype(TempsNum);
		MoveBasicDatatype(TempsUnit);
		//MoveBasicDatatype(ThreadUse);
		MoveBasicDatatype(TickRemover);
		MoveBasicDatatype(TrackHeight);
		MoveArray(tracksColor);
		MoveBasicDatatype(useEQ);
		MoveBasicDatatype(UseMarkers);
		MoveBasicDatatype(UseOctaveMarkers);
		MoveBasicDatatype(volumeLevel);
		MoveBasicDatatype(whichEditorPatterns);
		MoveArray(WinEtat);
		MoveArray(WinEtatO);
		MoveArray(WinHi);
		MoveArray(WinHiO);
		MoveArray(WinID);
		MoveArray(WinIDO);
		MoveArray(WinLarg);
		MoveArray(WinLargO);
		MoveArray(WinNames);
		MoveArray(WinPos);
		MoveArray(WinPosO);
		MoveBasicDatatype(yellC);
#undef MoveBasicDatatypes
#undef MoveArray
		
		WriteCFPreferences();
	}
}

void PlayerPRO::PlayerPROApp::DoPreferences()
{
	OSErr	iErr;
	short	vRefNum, fRefNum;
	long	DirID;
	FSSpec	spec;
	
	RegisterCFDefaults();
	ReadCFPreferences();
	
#if 0
	Gestalt(gestaltHardwareAttr, &gestaltAnswer);
	myBit = gestaltHasASC;
	
	hasASC = false;
	
	Gestalt(gestaltSoundAttr, &gestaltAnswer);
	gestaltAnswer = EndianS32_BtoN(gestaltAnswer);
	
	myBit = gestaltStereoCapability;
	Stereo = BitTst(&gestaltAnswer, 31-myBit);
	
	myBit = gestaltStereoMixing;
	StereoMixing = BitTst(&gestaltAnswer, 31-myBit);
	
	myBit = gestalt16BitSoundIO;
	Audio16 = BitTst(&gestaltAnswer, 31-myBit);
	
	if (NewSoundManager31)
	{
		short sSize;
		GetSoundOutputInfo(NULL, siSampleSize, &sSize);
		if (sSize >= 16)
			Audio16 = true;
	}
#else
	Stereo = StereoMixing = Audio16 = true;
#endif
	
	// Look for and load old preferences
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &vRefNum, &DirID);
	FSMakeFSSpec(vRefNum, DirID, PLAYERPREF, &spec);
	iErr = FSpOpenDF(&spec, fsRdPerm, &fRefNum);
	if (iErr == noErr) {
		LoadOldFilePrefs(fRefNum);
		//If found, also delete old preferences
		//iErr = FSpDelete(&spec);
		ReadCFPreferences();
	}
#if 0
	if (iErr == fnfErr) {
		iErr = FSpCreate(&spec, 'SNPL', 'PREF', smSystemScript);
		iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
		if (iErr != noErr) MyDebugStr(__LINE__, __FILE__, "PlayerPREF ERROR 32");
		aHandle = GetResource('AGGA', DEFAULTPREFSNUM);
		if (aHandle != NULL) {
			DetachResource(aHandle);
			HLock(aHandle);
			BlockMoveData(*aHandle, &thePrefs, GetHandleSize(aHandle));
			HUnlock(aHandle);
			DisposeHandle(aHandle);
			SwapPrefs(&thePrefs);
		} else {
			Erreur(87, 87);
			abort();
		}
		
		thePrefs.Version = VERSION;
		thePrefs.NoStart = 0;
		thePrefs.checkSum = 0;
		thePrefs.Mz = 300;
#if defined(powerc) || defined (__powerc) || defined(__ppc__)
		thePrefs.PPCMachine = true;
#else
		thePrefs.PPCMachine = false;
#endif
		thePrefs.MADCompression	= true;
		
		thePrefs.KeyUpMode 			= eNoteOFF;
		thePrefs.MacKeyBoard 		= true;
		thePrefs.MidiKeyBoard 		= false;
		thePrefs.QKMidiKeyBoard		= false;
		
		thePrefs.MIDIVelocity		= true;
		thePrefs.MIDIChanInsTrack	= false;
		
		thePrefs.AutoCreator = true;
		thePrefs.MicroRecording = false;
		thePrefs.AffichageDIGI = false;
		//thePrefs.PianoPos = 0;
		//GetSoundVol((void *)&thePrefs.volumeLevel);
		//thePrefs.volumeLevel++;
		//thePrefs.volumeLevel *= 8;
		GetDefaultOutputVolume((long*) &tempL);
		thePrefs.volumeLevel = tempL.v;
		thePrefs.TextS = 0;
		thePrefs.ThreadUse = false;
		//thePrefs.ThreadUse = true;
		
		
		thePrefs.FSinScroll = true;
		thePrefs.SSText = true;
		thePrefs.SSStars = true;
		thePrefs.SSJumping = false;
		thePrefs.FText = true;
		thePrefs.FStars = true;
		thePrefs.FBalls = false;
		thePrefs.MADC = false;
		thePrefs.OscilloSize = 128;
		thePrefs.OscilloType = 0;	// OutPutAudio
		
		thePrefs.SpectrumSize = 128;
		thePrefs.SpectrumType = 0;	// OutPutAudio
		
		thePrefs.fileTypeExportSND = 1;
		thePrefs.CompressionExportSND = 'NONE';
		thePrefs.ActiveHelp = true;
		
		thePrefs.UseOctaveMarkers = true;
		thePrefs.UseMarkers = true;
		thePrefs.MarkersSize = 3;
		thePrefs.MarkersOffSet = 0;
		thePrefs.MozartX = 3;
		thePrefs.SpectrumScale = 1;		// linear
		thePrefs.ClassicalProjection = false;
		thePrefs.PianoOctaveMarkers = true;
		thePrefs.SmallPiano = false;
		thePrefs.FastMusicList = true;
		thePrefs.FastDigitalEdition = controlKey;	//optionKey + cmdKey;
		thePrefs.DigitalInstru = true;
		thePrefs.DigitalNote = true;
		thePrefs.DigitalEffect = true;
		thePrefs.DigitalArgu = true;
		thePrefs.DigitalVol = true;
		thePrefs.MozartC2h = 0;
		thePrefs.MozartC1h = 0;
		
		GetDateTime(&thePrefs.firstStart);
		
		thePrefs.WinHi[RefTools] = 0;
		thePrefs.WinPos[RefTools].h = 0;
		thePrefs.SoundTypeSamp = '\?\?\?\?';
		thePrefs.SoundTypeIns = '\?\?\?\?';
		thePrefs.LinesHeight = 0;
		thePrefs.SaveMusicList = false;
		thePrefs.softVolumeLevel = 64;
		
		thePrefs.oADAPremember = false;
		thePrefs.oADAPsave = false;
		thePrefs.oADAPuse = false;
		
		thePrefs.osciTile = true;
		
		thePrefs.addExtension = true;
		thePrefs.clickSound = false;
		thePrefs.patternWrapping = false;
		thePrefs.AutoPlayWhenOpen = true;
		thePrefs.OscilloLine = true;
		//TODO: implement i386 code
		if (thePrefs.PPCMachine) thePrefs.Interpolation = true;
		else thePrefs.Interpolation = false;
		thePrefs.MicroDelay = false;
		thePrefs.MicroDelaySize = 30;
		
		thePrefs.Reverb = false;
		thePrefs.ReverbStrength = 60;
		thePrefs.ReverbSize = 25;
		//thePrefs.DirectVideo = true;
		thePrefs.FinePositioning = true;
		
		thePrefs.amplitude			= 16;
		thePrefs.Compressor			= 'NONE';
		thePrefs.FrequenceSpeed		= rate44khz;
		thePrefs.channelNumber		= 2;
		
		thePrefs.DirectDriverType.numChn			= 4;
		thePrefs.DirectDriverType.outPutBits		= 16;
		thePrefs.DirectDriverType.outPutRate		= rate44khz;
		thePrefs.DirectDriverType.outPutMode		= DeluxeStereoOutPut;	// force DeluxeStereoOutPut
		thePrefs.DirectDriverType.driverMode		= SoundManagerDriver;
		thePrefs.DirectDriverType.surround			= false;
		thePrefs.DirectDriverType.MicroDelaySize	= 0;
		thePrefs.DirectDriverType.Reverb			= false;
		thePrefs.DirectDriverType.ReverbSize		= 25;
		thePrefs.DirectDriverType.ReverbStrength	= 60;
		thePrefs.DirectDriverType.sysMemory			= false;
		thePrefs.DirectDriverType.TickRemover		= true;
		thePrefs.DirectDriverType.oversampling		= 4;
		
		thePrefs.RecordAllTrack		= 1;	// all tracks !
		
		thePrefs.StaffShowAllNotes	= false;
		thePrefs.StaffShowLength	= false;
		thePrefs.TempsNum			= 4;
		thePrefs.TempsUnit			= 4;
		thePrefs.TrackHeight		= 130;
		
		thePrefs.NewPrefSystem		= false;
		pStrcpy(thePrefs.NewPrefsCode, "\p");
		
		thePrefs.yellC.red 			= 65535;
		thePrefs.yellC.green 		= 65535;
		thePrefs.yellC.blue 		= 39321;
		thePrefs.whichEditorPatterns= RefPartition;
		
		thePrefs.numChn			= 4;					// Active tracks from 2 to 32, automatically setup when a new music is loaded
		
		if (Audio16)
		{
			thePrefs.outPutBits		= 16;					// 8 or 16 Bits
			thePrefs.outPutRate		= rate44khz;			// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
		}
		else
		{
			thePrefs.outPutBits		= 8;					// 8 or 16 Bits
			thePrefs.outPutRate		= rate22khz;			// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
		}
		thePrefs.surround		= false;
		thePrefs.TickRemover	= true;
		thePrefs.keyMapNote		= false;
		thePrefs.MusicTrace		= true;
		thePrefs.oversampling	= 1;
		thePrefs.RAWBits		= 8;
		thePrefs.RAWRate		= rate22khz >> 16;
		thePrefs.RAWStereo		= false;
		thePrefs.RAWSigned		= false;
#ifdef __BIG_ENDIAN__
		thePrefs.RAWLittleEndian= false;
#else
		thePrefs.RAWLittleEndian= true;
#endif
		thePrefs.RAWEOF			= true;
		thePrefs.RAWLength		= 0;
		thePrefs.RAWHeader		= 0;
		thePrefs.SendMIDIClockData	= false;
		thePrefs.pianoOffset	= 0;
		thePrefs.DontUseFilesMix = false;
		
		thePrefs.previousSpec.generalPitch = 252;
		thePrefs.previousSpec.generalSpeed = 252;
		thePrefs.previousSpec.generalPan = 64;
		thePrefs.previousSpec.EPitch = 8000;
		thePrefs.previousSpec.ESpeed = 8000;
		thePrefs.previousSpec.generalVol = 64;
		
		for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanPan[i] = 256;
		for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanVol[i] = 64;
		
		for (i = 0; i < 20; i++)
		{
			thePrefs.FKeyActive[i]		= false;
			thePrefs.FKeyItem[i] 		= 0;
			thePrefs.FKeyWind[i]		= 0;
		}
		
		pStrcpy(thePrefs.WinNames[0], "\pState 1");
		pStrcpy(thePrefs.WinNames[1], "\pState 2");
		pStrcpy(thePrefs.WinNames[2], "\pState 3");
		
		for (x = 0; x < 3; x++)
		{
			for (i = 0; i < MAXWINDOWS; i++)
			{
				thePrefs.WinPosO[x][i] = thePrefs.WinPos[i];
				thePrefs.WinEtatO[x][i] = thePrefs.WinEtat[i];
				thePrefs.WinLargO[x][i] = thePrefs.WinLarg[i];
				thePrefs.WinHiO[x][i] = thePrefs.WinHi[i];
				thePrefs.WinIDO[x][i] = thePrefs.WinID[i];
			}
		}
		
		for (i = 0; i < MAXINSTRU; i++) thePrefs.OCArrow[i] = true;
		
		thePrefs.outPutMode = DeluxeStereoOutPut;
		thePrefs.driverMode = SoundManagerDriver;
		thePrefs.useEQ = false;
		thePrefs.lastVisualPlugin = 0;
		thePrefs.editorSoundDrag = false;
		thePrefs.channelNumber = 2;
		thePrefs.FKeyTracks = true;
		
		for (i = 0; i < EQPACKET*2; i++)	thePrefs.Filter[i] = 1.0;
		
		for (i = 0; i < MAXTRACK; i++) thePrefs.SelectedTracks[i] = false;
		
		pStrcpy(thePrefs.ASIODriverName, "\pApple Sound Manager");
		
		// Ecrasement par les vieilles preferences
		if (tempPrefs != NULL)
		{
			BlockMoveData(tempPrefs, &thePrefs, GetPtrSize((Ptr) tempPrefs));
			
			DisposePtr((Ptr) tempPrefs);
			tempPrefs = NULL;
		}
		
		thePrefs.Version = VERSION;
		
		// Ecriture sur disque des preferences
		
		inOutBytes = sizeof(Prefs);
		iErr = FSSetForkPosition(fRefNum, fsFromStart, 0);
#ifdef __LITTLE_ENDIAN__
		Prefs *outPrefs = (Prefs*)NewPtr(sizeof(Prefs));
		*outPrefs = thePrefs;
		SwapPrefs(outPrefs);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, outPrefs, NULL);
		DisposePtr((Ptr)outPrefs);
#else
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &thePrefs, NULL);
#endif
		iErr = FSCloseFork(fRefNum);
		
		goto ReLoadPrefs;
	}
#endif
	
	thePrefs.ActiveHelp = false;
	
	/*for (i = 0; i < 7; i++)
	 {
	 BlockMoveData(&thePrefs.tracksColor[i * 32], &thePrefs.tracksColor[32 + i * 32], 32 * 6);
	 }*/
	
	/////////
}

OSErr PlayerPRO::PlayerPROApp::FindAProcess(OSType typeToFind, OSType creatorToFind, ProcessSerialNumberPtr processSN)
{
	ProcessInfoRec	tempInfo;
	FSSpec			procSpec;
	Str31			processName;
	OSErr			myErr = noErr;
	
	// start at the beginning of the process list
	processSN->lowLongOfPSN = kNoProcess;
	processSN->highLongOfPSN = kNoProcess;
	
	// initialize the process information record
	tempInfo.processInfoLength = sizeof(ProcessInfoRec);
	tempInfo.processName = (StringPtr)&processName;
	tempInfo.processAppSpec = &procSpec;
	
	do {
		myErr = GetNextProcess(processSN);
		if (myErr == noErr)
			GetProcessInformation(processSN, &tempInfo);
	} while((tempInfo.processSignature != creatorToFind ||
			 tempInfo.processType != typeToFind) ||
			myErr != noErr);
	return myErr;
}

OSErr PlayerPRO::PlayerPROApp::OpenSelection(FSSpecPtr theDoc)
{
	AppleEvent	aeEvent;	// the event to create;
	AEDesc	myAddressDesc;	// descriptors for the
	AEDesc	aeDirDesc;
	AEDesc	listElem;
	AEDesc	fileList;			// our list
	FSSpec	dirSpec;
	AliasHandle	dirAlias;				// alias to directory with our file
	AliasHandle	fileAlias;				// alias of the file itself
	ProcessSerialNumber process;		// the finder's psn
	OSErr			myErr;					// duh
	
	// Get the psn of the Finder and create the target address for the .
	if (FindAProcess(kFinderSig, kSystemType, &process))
		return procNotFound;
	myErr = AECreateDesc(typeProcessSerialNumber,(Ptr)&process,
						 sizeof(process), &myAddressDesc);
	if (myErr)
		return myErr;
	
	// Create an empty
	myErr = AECreateAppleEvent (kAEFinderEvents, kAEOpenSelection, &myAddressDesc, kAutoGenerateReturnID, kAnyTransactionID, &aeEvent);
	if (myErr)
		return myErr;
	
	// Make an FSSpec and alias for the parent folder, and an alias for the file
	FSMakeFSSpec(theDoc->vRefNum, theDoc->parID, NULL, &dirSpec);
	NewAlias(NULL, &dirSpec, &dirAlias);
	NewAlias(NULL, theDoc, &fileAlias);
	
	// Create the file list.
	myErr = AECreateList(NULL, 0, false, &fileList);
	if (myErr)
		return myErr;
	
	/* Create the folder descriptor
	 */
	HLock((Handle)dirAlias);
	AECreateDesc(typeAlias, (Ptr)*dirAlias, GetHandleSize
				 ((Handle) dirAlias), &aeDirDesc);
	HUnlock((Handle)dirAlias);
	DisposeHandle((Handle)dirAlias);
	
	if ((myErr = AEPutParamDesc(&aeEvent,keyDirectObject,&aeDirDesc)) ==
		noErr) {
		AEDisposeDesc(&aeDirDesc);
		HLock((Handle)fileAlias);
		
		AECreateDesc(typeAlias, (Ptr)*fileAlias,
					 GetHandleSize((Handle)fileAlias), &listElem);
		HUnlock((Handle)fileAlias);
		DisposeHandle((Handle)fileAlias);
		myErr = AEPutDesc(&fileList,0,&listElem);
	}
	
	if (myErr)
		return myErr;
	
	AEDisposeDesc(&listElem);
	
	myErr = AEPutParamDesc(&aeEvent,keySelection,&fileList);
	if (myErr)
		return myErr;
	
	myErr = AEDisposeDesc(&fileList);
	if (myErr)
		return myErr;
	
	myErr = AESend(&aeEvent, NULL,
				   kAENoReply | kAEAlwaysInteract | kAECanSwitchLayer,
				   kAENormalPriority, kAEDefaultTimeout, NULL, NULL);
	
	AEDisposeDesc(&aeEvent);
	SetFrontProcess(&process);
	
	return noErr;
}

void PlayerPRO::PlayerPROApp::DoReset()
{
#if 0
	long 		i, x;
	GrafPtr		savedPort;
	DialogPtr	aDia;
	short		itemHit, itemType;
	Rect		tempRect;
	Handle		itemHandle;
	short		tracks, position;
	Cmd			*aCmd;
	
	GetPort(&savedPort);
	
	aDia = GetNewDialog(138, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	SetCursorToQDArrow();
	
	GetDialogItem(aDia, 3, &itemType, &itemHandle, &tempRect);
	SetControlValue((ControlHandle) itemHandle, ResetPatterns);
	GetDialogItem(aDia, 5, &itemType, &itemHandle, &tempRect);
	SetControlValue((ControlHandle) itemHandle, ResetInstrus);
	GetDialogItem(aDia, 6, &itemType, &itemHandle, &tempRect);
	SetControlValue((ControlHandle) itemHandle, PurgePatterns);
	
	if (!ResetPatterns && !ResetInstrus && !PurgePatterns)
		ControlSwitch(1, aDia, 255);
	else
		ControlSwitch(1, aDia, 0);
	
	do {
		MyModalDialog(aDia, &itemHit);
		
		switch (itemHit) {
			case 3:
			case 5:
			case 6:
				GetDialogItem(aDia, itemHit, &itemType, &itemHandle, &tempRect);
				SetControlValue((ControlHandle) itemHandle, !GetControlValue((ControlHandle) itemHandle));
				
				switch (itemHit) {
					case 3:
						ResetPatterns = !ResetPatterns;
						break;
						
					case 5:
						ResetInstrus = !ResetInstrus;
						break;
						
					case 6:
						PurgePatterns = !PurgePatterns;
						break;
				}
				
				if (!ResetPatterns && !ResetInstrus && !PurgePatterns)
					ControlSwitch(1, aDia, 255);
				else
					ControlSwitch(1, aDia, 0);
				break;
		}
		
	} while (itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1) {
		DoStop();
		
		MADPurgeTrack(MADDriver);
		MADCleanDriver(MADDriver);
		
		GetDialogItem(aDia, 3, &itemType, &itemHandle, &tempRect);	// DELETE Patterns
		if (ResetPatterns) {
			for (x = 1; x < curMusic->header->numPat ; x++) {
				if (curMusic->partition[x] != NULL)
					DisposePtr((Ptr) curMusic->partition[x]);
				curMusic->partition[x] = NULL;
			}
			curMusic->header->numPat = 1;
			
			// Purge Track
			for (tracks = 0; tracks < curMusic->header->numChn; tracks++) {
				for (position = 0; position < curMusic->partition[0]->header.size; position++) {
					aCmd = GetMADCommand(position, tracks, curMusic->partition[0]);
					MADKillCmd(aCmd);
				}
			}
			
			// DELETE Partition
			for (i = 0; i < 128; i++) {
				curMusic->header->oPointers[i] = 0;
			}
			
			curMusic->header->numPointers = 1;
		}
		
		GetDialogItem(aDia, 5, &itemType, &itemHandle, &tempRect);	// Instrus
		if (ResetInstrus) {
			for (x = 0; x < MAXINSTRU ; x++) {
				MADKillInstrument(curMusic, x);
			}
		}
		
		GetDialogItem(aDia, 6, &itemType, &itemHandle, &tempRect); // Purge patterns
		if (PurgePatterns)
		{
			for (i = 0; i < curMusic->header->numPat; i++)
			{
				// Purge Track
				for (tracks = 0; tracks < curMusic->header->numChn; tracks++)
				{
					for (position = 0; position < curMusic->partition[i]->header.size; position++)
					{
						aCmd = GetMADCommand(position, tracks, curMusic->partition[i]);
						MADKillCmd(aCmd);
					}
				}
			}
		}
		
		pStrcpy(curMusic->musicFileName, "\pReseted Music");
		SetCurrentMOD(curMusic->musicFileName);
	}
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
#if 0
	if (DoWindowState) {
		CheckShowWindow();
		
		if (thePrefs.ActiveHelp) CreateAHelpWindow();
		ShowWindowPref(-1);
		
		ShowWindow(ToolsDlog);
		SelectWindow(ToolsDlog);
		SelectWindow2(NextWindowVisible(ToolsDlog));
	}
#endif
#endif
	
	MADReset(MADDriver);
	
	UPDATE_Total();
	
	ResetUndo();
	ScanTime();
	
	UpdateALLWindow();
}

ConstStr255Param PlayerPRO::PlayerPROApp::GetVersionString()
{
	return versString;
}

void PlayerPRO::PlayerPROApp::SetCursorOnNumber(short theNum)
{
	assert(theNum > -1 && theNum < TotalCursors);
	SetCursor(&cursors[theNum]);
}

PlayerPRO::PlayerPROApp::PlayerPROApp() : gUseControlSize(false), MusicPlayActive(false), UseAEErreur(false),
ToolsDlog(NULL)
{
	char tempASC[256] = "";
	pStrcpy(asc_WorkStr, MYC2PStr(tempASC));
	generalWindows = new std::vector<PlayerPRO::wds_general*>();
	
	memset(&km, 0, sizeof(km));
	memset(&theColor, 0, sizeof(theColor));
	memset(&cursors, 0, sizeof(cursors));
	GetQDGlobalsArrow(&qdarrow);
}

void HandleExportFile(short theItem)
{
	switch (theItem) {
		case 1:
			SaveMOD(true, 'AIFF');
			break;
			
		case 2:
			SaveMOD(true, 'MPG4');
			break;
			
		default:
			if (CallPlug(0))
				break;
			SaveMOD(true, GetPPPlugType(PlayerPRO::TheApp->gMADLib, theItem - 4, 'EXPL'));
			break;
	}
}

PlayerPRO::PlayerPROApp::~PlayerPROApp()
{
	delete generalWindows;
}

void PlayerPRO::PlayerPROApp::SetCursorToQDArrow()
{
	SetCursor(GetQDGlobalsArrow(&qdarrow));
}

ProcessSerialNumber PlayerPRO::PlayerPROApp::GetPlayerPROPSN()
{
	return playerPROPSN;
}

void PlayerPRO::PlayerPROApp::GetSizeString(long size, Str255 str, Boolean Convert)
{
	if (Convert) {
		if (size < 9999) {
			NumToString(size, str);
			pStrcat(str, (StringPtr)"\p B");
		} else {
			NumToString(size/1024, str);
			pStrcat(str, (StringPtr)"\p KiB");
		}
	} else {
		NumToString(size, str);
	}
}

Boolean PlayerPRO::PlayerPROApp::QTTypeConversion(OSType fileType)
{
	switch (fileType) {
		case kQTFileTypeAIFF:
		case kQTFileTypeAIFC:
		case kQTFileTypeDVC:
		case kQTFileTypeMIDI:
		case kQTFileTypeMovie:
		case kQTFileTypeWave:
		case kQTFileTypeMuLaw:
		case kQTFileTypeAVI:
		case kQTFileTypeSoundDesignerII:
		case kQTFileTypeSystemSevenSound:
		case 'MPEG':
		case 'Mp3 ':
		case 'MPG3':
		case 'mp3 ':
		case 'MP3 ':
		case 'mpg4':
		case 'PLAY':
			//case 0x3F3F3F3F:
			return true;
			break;
	}
	
	return false;
}

Boolean PlayerPRO::PlayerPROApp::CheckFileType(FSSpec theSpec, OSType theType)
{
	Boolean	Response = false;
	OSErr	err;
	FSSpec	saved;
	char	tempC[5];
	
	HGetVol(NULL, &saved.vRefNum, &saved.parID);
	HSetVol(NULL, theSpec.vRefNum, theSpec.parID);
	
	switch (theType) {
		case 'MADK':
			MYP2CStr(theSpec.name);
			if (CheckMADFile((Ptr)theSpec.name) == noErr)
				Response = true;
			else
				Response = false;
			MYC2PStr((Ptr)theSpec.name);
			break;
			
		default:
			MYP2CStr(theSpec.name);
			OSType2Ptr(theType, tempC);
			err = PPTestFile(PlayerPRO::TheApp->gMADLib, tempC, (Ptr)theSpec.name);
			MYC2PStr((Ptr)theSpec.name);
			
			if (err)
				Response = false;
			else
				Response = true;
			
			if (Response == false) {	// Try to import it with Quicktime
				Response = QTTestConversion(&theSpec, theType);
			}
			break;
	}
	
	HSetVol(NULL, saved.vRefNum, saved.parID);
	
	return Response;
}

OSErr PlayerPRO::PlayerPROApp::GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr)
{
	OSErr err = fnfErr;
	CFBundleRef myAppsBundle = CFBundleGetMainBundle();
	if (myAppsBundle == NULL)
		return err;
	CFURLRef myBundleURL = CFBundleCopyBundleURL(myAppsBundle);
	if (myBundleURL == NULL)
		return err;
	FSRef myBundleRef;
	Boolean ok = CFURLGetFSRef(myBundleURL, &myBundleRef);
	CFRelease(myBundleURL);
	if (!ok)
		return err;
	return FSGetCatalogInfo(&myBundleRef, kFSCatInfoNone, NULL, NULL, theFSSpecPtr, NULL);

}

OSErr PlayerPRO::PlayerPROApp::GetApplicationResourceFSSpecFromBundle(FSSpecPtr theFSSpecPtr)
{
	OSErr err = fnfErr;
	CFBundleRef myAppsBundle = CFBundleGetMainBundle();
	if (myAppsBundle == NULL)
		return err;
	CFURLRef myBundleURL = CFBundleCopyResourcesDirectoryURL(myAppsBundle);
	if (myBundleURL == NULL)
		return err;
	FSRef myBundleRef;
	Boolean ok = CFURLGetFSRef(myBundleURL, &myBundleRef);
	CFRelease(myBundleURL);
	if (!ok)
		return err;
	return FSGetCatalogInfo(&myBundleRef, kFSCatInfoNone, NULL, NULL, theFSSpecPtr, NULL);
}


const EventTypeSpec *PlayerPRO::PlayerPROApp::CarbonWinList()
{
	static const EventTypeSpec CarbonWinlist[] = {
		{kEventClassWindow, kEventWindowClose},
		{kEventClassWindow, kEventWindowDrawContent},
		{kEventClassWindow, kEventWindowBoundsChanged}};
	return CarbonWinlist;
}

void PlayerPRO::PlayerPROApp::SetPortFromOldWindow()
{
	SetPortWindowPort(oldWindow);
}

CGrafPtr PlayerPRO::PlayerPROApp::GetPortFromOldWindow()
{
	return GetWindowPort(oldWindow);
}

DialogPtr PlayerPRO::PlayerPROApp::GetDialogFromOldWindow()
{
	return GetDialogFromWindow(oldWindow);
}

std::string PlayerPRO::PlayerPROApp::ENote(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EInstru(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EArgu(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EEffect(int idx)
{
	return "";
}

void PlayerPRO::PlayerPROApp::InitRollCrsr()
{
	Handle		tempHandle;
	short		i;
	CursHandle	tempH;
	
	myCursor = (Cursor*) NewPtr(30 * sizeof(Cursor));
	
	previousTime = 0;
	
	tempHandle = GetResource('acur', 200);
	DetachResource(tempHandle);
	HLock(tempHandle);
	BlockMoveData(*tempHandle, &MyCrsrRec, GetHandleSize(tempHandle));
	HUnlock(tempHandle);
	DisposeHandle(tempHandle);
	
	for (i = 0; i < MyCrsrRec.total; i++) {
		tempH = GetCursor(MyCrsrRec.no[i*2]);
		DetachResource((Handle)tempH);
		HLock((Handle)tempH);
		myCursor[i] = **tempH;
		HUnlock((Handle)tempH);
		DisposeHandle((Handle)tempH);
	}
}

void PlayerPRO::PlayerPROApp::RollCursor()
{
	if (previousTime + 10 >= TickCount())
		return;
	
	previousTime = TickCount();
	
	MyCrsrRec.current ++;
	if (MyCrsrRec.current >= MyCrsrRec.total)
		MyCrsrRec.current = 0;
	
	SetCursor(&myCursor[MyCrsrRec.current]);
}

void PlayerPRO::PlayerPROApp::CloseRollCrsrc()
{
	DisposePtr((Ptr)myCursor);
}

extern	KeyMap		km;
void DrawNumberMusic();

/*
 * Pascal string utilities
 */
/*
 * pstrcat - add string 'src' to end of string 'dst'
 */
void pstrcat(StringPtr dst, StringPtr src)
{
	/* copy string in */
	BlockMoveData(src + 1, dst + *dst + 1, *src);
	/* adjust length byte */
	*dst += *src;
}

/*
 * pstrinsert - insert string 'src' at beginning of string 'dst'
 */
void pstrinsert(StringPtr dst, StringPtr src)
{
	/* make room for new string */
	BlockMoveData(dst + 1, dst + *src + 1, *dst);
	/* copy new string in */
	BlockMoveData(src + 1, dst + 1, *src);
	/* adjust length byte */
	*dst += *src;
}

extern	DialogPtr MODListDlog;
Boolean remonte;

void PlayerPRO::PlayerPROApp::DoScanDir(long DirID, short VRefNum)
{
	GrafPtr	savePort;
	Rect	caRect;
	
	GetPort(&savePort);
	SetPortDialogPort(MODListDlog);
	
	remonte = false;
	
	ScanDir(DirID, VRefNum, true);
	
	GetPortBounds(GetDialogPort(MODListDlog), &caRect);
	
	InvalWindowRect(GetDialogWindow(MODListDlog), &caRect);
	
	SetPort(savePort);
}

void PlayerPRO::PlayerPROApp::ScanDir(long dirID, short VRefNum, Boolean recurse)
{
	short			i;
	CInfoPBRec		info;
	FSSpec			spec;
	
	if (remonte)
		return;
	
	info.hFileInfo.ioNamePtr = asc_WorkStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	for (i = 1; 1; i ++) {
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		if (PBGetCatInfoSync(&info) != noErr)
			break;
		
		pStrcpy(spec.name, info.hFileInfo.ioNamePtr);
		spec.vRefNum = info.hFileInfo.ioVRefNum;
		spec.parID = dirID;
		
		RollCursor();
		
		if (OpenableFile(info.hFileInfo.ioFlFndrInfo.fdType, &spec)) {
			if (info.hFileInfo.ioFlFndrInfo.fdType != 'sTAT' && info.hFileInfo.ioFlFndrInfo.fdType != 'STCf')
				AddMODList(false,
						   info.hFileInfo.ioNamePtr,
						   info.hFileInfo.ioVRefNum,
						   dirID);
		} else if (info.hFileInfo.ioFlAttrib & 16) {
			ScanDir(info.dirInfo.ioDrDirID, VRefNum, true);
		}
		
		GetKeys(km);
		if (IsPressed(0x37) && IsPressed(0x2F)) {
			remonte = true;
			break;
		}
	}
}

void PlayerPRO::PlayerPROApp::PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName)
{
	CInfoPBRec	block;
	Str255		directoryName;
	OSErr		err;
	
	pStrcpy(directoryName, "\p");
	pStrcpy(fullPathName, "\p");
	
	block.dirInfo.ioDrParID = dirID;
	block.dirInfo.ioNamePtr = directoryName;
	do {
		block.dirInfo.ioVRefNum = vRefNum;
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfoSync(&block);
		if (fullPathName[0] < 200) {
			pstrcat(directoryName, (StringPtr)"\p: ");
			pstrinsert(fullPathName, directoryName);
		}
	} while (block.dirInfo.ioDrDirID != 2 && err == noErr);
}

EventRecord *CurrentEvent()
{
	return &PlayerPRO::TheApp->theEvent;
}

int main(int argc, char *argv[])
{
	PlayerPRO::TheApp = new PlayerPRO::PlayerPROApp();
	
	PlayerPRO::TheApp->Run();
	
	delete PlayerPRO::TheApp;
	
	return EXIT_SUCCESS;
}
