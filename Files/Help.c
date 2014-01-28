#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Help.h"
#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>

//#include "asiodrivers.h"

//extern	AsioDrivers*	asioDrivers;
extern	char 			ASIO_DRIVER_NAME[ 255];

extern	Boolean		Stereo, StereoMixing, NewSoundManager, MIDIHardware, MIDIHardwareAlreadyOpen;
extern	Prefs		thePrefs;
extern	DialogPtr	EditorDlog, ClassicDlog, AdapDlog, MozartDlog;
extern	MenuHandle	TrackView;
extern	RGBColor	theColor;
extern	short		LastCanal;
extern	Boolean		PianoRecording;
extern	WindowPtr	oldWindow;
extern	Boolean		EditorKeyRecording;
extern	short		curTrack;

static	short		base, PrefsType;
static	DialogPtr	prefDlog;
static	Rect		PianoRectList, TrackRectList;
		KeyMap		km;
static	ListHandle	PianoList, TrackList;

Boolean PressFKeyMenu(short itemID, DialogPtr dia, short whichFKey, Str255	str);
void TurnRadio(short item, DialogPtr	dlog, Boolean alors);
void ControlSwitch(short item, DialogPtr	dlog, short	Switch);
void SaveAdaptorsFile(FSSpec *file);
void LoadAdaptatorsInt(FSSpec *file);
void SelectOMSConnections(Boolean Input);
Boolean PressMenuItem(short menuID, short itemID, DialogPtr dia, long *curVal, short remove, Str255	str);
void OpenMIDIHardware();
void VSTFilter(DialogPtr theDialog, EventRecord *theEventI, short *itemHit);
void MyTETextBox(RGBColor *backColor, Rect *rect, Str255 str, Boolean black);
Boolean	IsPressed(unsigned short k);


short GetWhichTrackPlay()
{
	short returnTrack = 0;
	
	if (EditorDlog != NULL) {
		if (oldWindow == GetDialogWindow(EditorDlog) && EditorKeyRecording == true) {
			return curTrack;
		}
	}
	
	switch (thePrefs.RecordAllTrack) {	// all tracks
		case 1:
		{
			short xxx = 0;
			
			LastCanal++;
			
			while ((curMusic->header->chanVol[LastCanal] == 0
					|| MADDriver->Active[LastCanal] == false) && xxx <= curMusic->header->numChn) {
				xxx++;
				LastCanal++;
				if (LastCanal >= MADDriver->DriverSettings.numChn)
					LastCanal = 0;
			}
			if (LastCanal >= MADDriver->DriverSettings.numChn)
				LastCanal = 0;
			
			returnTrack = LastCanal;
		}
			break;
			
		case 0:	// Selected track
			returnTrack = LastCanal;
			break;
			
		case 2:	// Current Track
			returnTrack = LastCanal = curTrack;
			break;
			
		case 3:	// SelectedTracks
		{
			short xxx = 0;
			
			LastCanal++;
			
			while ((curMusic->header->chanVol[LastCanal] == 0 || thePrefs.SelectedTracks[LastCanal] == false
					|| MADDriver->Active[LastCanal] == false) && xxx <= curMusic->header->numChn) {
				xxx++;
				LastCanal++;
				if (LastCanal >= MADDriver->DriverSettings.numChn)
					LastCanal = 0;
			}
			if (LastCanal >= MADDriver->DriverSettings.numChn)
				LastCanal = 0;
			
			returnTrack = LastCanal;
		}
			break;
	}
	
	return returnTrack;
}

void SwitchDriverTo(short aaa)
{
	Boolean	 			IsPlaying, IsEQ, IsJumpToNextPattern;
	MADDriverSettings	init;
	OSErr				iErr;
	FSSpec				spec;
	long				fulltime, curTime, i;
	double				filter[ (EQPACKET*2) + 2];
	
	IsPlaying = MADDriver->Reading;
	IsEQ = MADDriver->Equalizer;
	IsJumpToNextPattern = MADDriver->JumpToNextPattern;
	for (i = 0; i <= EQPACKET*2; i++)
	{
		filter[ i] = MADDriver->Filter[ i];
	}
	
	//// Save AdapValue ////
	
#define ADAPNAME "\pAdap.Temp"
	
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
	
	pStrcpy(spec.name, ADAPNAME);
	
	SaveAdaptorsFile(&spec);
	
	///////////////////////
	
	MADGetMusicStatus(MADDriver, &fulltime, &curTime);
	
	MADDriver->Reading = false;
	if (MADStopDriver(MADDriver) != noErr) MyDebugStr(__LINE__, __FILE__, "FatalError Driver X");
	if (MADDisposeDriver(MADDriver) != noErr) MyDebugStr(__LINE__, __FILE__, "FatalError Driver Y");
	
	SetCursor(&watchCrsr);
	
	init.numChn			= thePrefs.numChn;
	init.outPutBits		= thePrefs.outPutBits;
	init.outPutRate		= thePrefs.outPutRate;
	init.outPutMode		= thePrefs.outPutMode;
	init.driverMode		= thePrefs.driverMode;
	init.surround		= thePrefs.surround;
	init.MicroDelaySize	= thePrefs.MicroDelaySize;
	init.Reverb			= thePrefs.Reverb;
	init.ReverbSize		= thePrefs.ReverbSize;
	init.ReverbStrength	= thePrefs.ReverbStrength;
	init.sysMemory		= false;
	init.TickRemover	= thePrefs.TickRemover;
	init.oversampling	= thePrefs.oversampling;
	
	if (thePrefs.LoopType != 0)
		init.repeatMusic = false;
	else
		init.repeatMusic = true;
	
	iErr = MADCreateDriver(&init, gMADLib, &MADDriver);
	if (iErr) MyDebugStr(__LINE__, __FILE__, "MusicDriver ERROR !");
	
	MADStartDriver(MADDriver);
	MADAttachDriverToMusic(MADDriver, curMusic, NULL);
	
	MADSetMusicStatus(MADDriver, 0, fulltime, curTime);
	
	//// Load AdapValue ////
	
	LoadAdaptatorsInt(&spec);
	FSpDelete(&spec);
	
	///////////////////////
	
	if (IsPlaying)
		MADDriver->Reading = true;
	
	if (IsEQ)
		MADDriver->Equalizer = true;
	
	MADDriver->JumpToNextPattern = IsJumpToNextPattern;
	
	for (i = 0; i <= EQPACKET * 2; i++) {
		MADDriver->Filter[ i] = filter[ i];
	}
	
	MADDriver->SendMIDIClockData = thePrefs.SendMIDIClockData;
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
}

void DrawAllNote(void)
{
	Str255	String;
	Point	cSize;
	short	i,x;
	Str255	tempString;
	
	cSize.h = cSize.v = 0;
	LSetDrawingMode(true, PianoList);
	
	for (i = 0; i < NUMBER_NOTES + 1; i++) {
		if (i == NUMBER_NOTES) {
			pStrcpy(String, "\p000");
			
			for(x=0; x<290; x++) if (thePrefs.PianoKey[ x] == 0xFF) break;
		} else {
			GetNoteString(i, String);
			
			for(x = 0; x < 290; x++) if (thePrefs.PianoKey[x] == i)
				break;
		}
		
		if (x <= 260) {
			tempString[0] = 1;
			tempString[1] = x;
			pStrcat(String, "\p <- ");
			pStrcat(String, tempString);
		}
		
		LSetCell(String + 1, String[ 0], cSize, PianoList);
		cSize.v++;
	}
}

void PrefFilterPiano(DialogPtr theDialog, EventRecord *theEventI, short *itemHit)
{
	WindowPtr	whichWindow;
	short		thePart,i, theChar;
	GrafPtr		oldPort;
	Point		aPoint, theCell;
	Str255		str2;
	
	GetPort(&oldPort);
	SetPortDialogPort(prefDlog);
	
	switch (theEventI->what) {
		case mouseDown:
			thePart = FindWindow(theEventI->where, &whichWindow);
			
			if (whichWindow == GetDialogWindow(prefDlog)) {
				aPoint.v = theEventI->where.v;
				aPoint.h = theEventI->where.h;
				GlobalToLocal(&aPoint);
				if (PrefsType == EDITOR && PtInRect(aPoint, &PianoRectList) == true) {
					TextFont(4);
					TextSize(9);
					LClick(aPoint, theEventI->modifiers, PianoList);
					TextFont(0);
					TextSize(0);
				}
				
				if (PrefsType == EDITOR && PtInRect(aPoint, &TrackRectList) == true) {
					if (TrackList) {
						TextFont(4);
						TextSize(9);
						LClick(aPoint, theEventI->modifiers, TrackList);
						TextFont(0);
						TextSize(0);
					}
				}
			}
			break;
			
		case keyDown:
		case autoKey:
			if (PrefsType == EDITOR) {
				TextFont(4);
				TextSize(9);
				theChar = theEventI->message & charCodeMask;
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, PianoList)) {
					if (theChar == 0x1E) {
						if (theCell.v > 0) {
							LSetSelect(false, theCell, PianoList);
							theCell.v--;
							LSetSelect(true, theCell, PianoList);
							LAutoScroll(PianoList);
						}
					} else if (theChar == 0x1F)  {
						if (theCell.v < NUMBER_NOTES) {
							LSetSelect(false, theCell, PianoList);
							theCell.v++;
							LSetSelect(true, theCell, PianoList);
							LAutoScroll(PianoList);
						}
					} else if (theChar == 0x08)  {
						if (theCell.v == NUMBER_NOTES)
							theCell.v = 0xFF;	// NO NOTE
						
						for (i = 0; i < 299; i++) {
							if (thePrefs.PianoKey[i] == theCell.v)
								thePrefs.PianoKey[i] = -1;
						}
						DrawAllNote();
					} else {
						if (theCell.v == NUMBER_NOTES)
							theCell.v = 0xFF;	// NO NOTE
						
						str2[0] = 1;
						str2[1] = theChar;
						MyP2CStr(str2);
						theChar = str2[0];
						
						/**** Set pour le prefs*/
						
						for(i=0; i<299; i++) {
							if (thePrefs.PianoKey[i] == theCell.v)
								thePrefs.PianoKey[i] = -1;
						}
						
						if (theChar > 0) {
							thePrefs.PianoKey[theChar] = theCell.v;
							
							DrawAllNote();
							
							LSetSelect(false, theCell, PianoList);
							theCell.v++;
							LSetSelect(true, theCell, PianoList);
							LAutoScroll(PianoList);
						}
					}
				}
				TextFont(0);
				TextSize(0);
			}
			break;
	}
	
	SetPort(oldPort);
	
	return;
}

void AppendPref(short theID, DialogPtr theDia)
{
	Handle	theDITL;
	short	numberItems = CountDITL(theDia);
	
	if (numberItems - base > 0)
		ShortenDITL(theDia, numberItems - base);
	
	DrawDialog(theDia);
	
	theDITL = GetResource('DITL', theID);
	if (theDITL == NULL)
		abort();
	
	AppendDITL(theDia, theDITL, overlayDITL);
	ReleaseResource(theDITL);
}

void InitCLASSICAL(void)
{
	Str255		aStr;
	
	TurnRadio(5 + base, prefDlog, thePrefs.UseMarkers);
	TurnRadio(11 + base, prefDlog, thePrefs.UseOctaveMarkers);
	TurnRadio(4 + base, prefDlog, thePrefs.ClassicalProjection);	
	
	NumToString(thePrefs.MarkersSize, aStr);
	SetDText(prefDlog, base + 9, aStr);
	NumToString(thePrefs.MarkersOffSet, aStr);
	SetDText(prefDlog, base + 7, aStr);
}

void CloseCLASSICAL()
{
	Str255		aStr;
	long		temp;
	
	GetDText(prefDlog, base + 9, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersSize = temp;
	GetDText(prefDlog, base + 7, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersOffSet = temp;
	
	ComputeGrilleGWorld();
	UpdateMozartInfo();
	UpdateEditorInfo();
	UpdateStaffInfo();
}

void InitDIGITAL(void)
{
	Str255		aStr;
	
	if (thePrefs.DigitalInstru)
		TurnRadio(base + 1, prefDlog, true);
	if (thePrefs.DigitalNote)
		TurnRadio(base + 2, prefDlog, true);
	if (thePrefs.DigitalEffect)
		TurnRadio(base + 3, prefDlog, true);
	if (thePrefs.DigitalArgu)
		TurnRadio(base + 4, prefDlog, true);
	if (thePrefs.DigitalVol)
		TurnRadio(base + 6, prefDlog, true);
	
	if (thePrefs.FastDigitalEdition & controlKey)
		TurnRadio(base + 16, prefDlog, true);
	if (thePrefs.FastDigitalEdition & shiftKey)
		TurnRadio(base + 17, prefDlog, true);
	if (thePrefs.FastDigitalEdition & cmdKey)
		TurnRadio(base + 18, prefDlog, true);
	if (thePrefs.FastDigitalEdition & optionKey)
		TurnRadio(base + 19, prefDlog, true);
	
	TurnRadio(8 + base, prefDlog, thePrefs.UseMarkers);
	
	NumToString(thePrefs.MarkersSize, aStr);
	SetDText(prefDlog, base + 10, aStr);
	NumToString(thePrefs.MarkersOffSet, aStr);
	SetDText(prefDlog, base + 12, aStr);
	
	switch (thePrefs.LinesHeight) {
		case 0:
			TurnRadio(22 + base, prefDlog, true);
			break;
			
		case 2:
			TurnRadio(23 + base, prefDlog, true);
			break;
	}
	
	TurnRadio(27 + base, prefDlog, false);
	TurnRadio(28 + base, prefDlog, false);
	
	if (thePrefs.MusicTrace)
		TurnRadio(28 + base, prefDlog, true);
	else
		TurnRadio(27 + base, prefDlog, true);
	
	TurnRadio(base + 30, prefDlog, false);
	TurnRadio(base + 31, prefDlog, false);
	
	if (thePrefs.patternWrapping)
		TurnRadio(30 + base, prefDlog, true);
	else
		TurnRadio(31 + base, prefDlog, true);
	
	if (thePrefs.editorSoundDrag)
		TurnRadio(34 + base, prefDlog, true);
	else
		TurnRadio(35 + base, prefDlog, true);
}

void CloseDIGITAL()
{
	Str255		aStr;
	long		temp;
	
	GetDText(prefDlog, base + 10, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersSize = temp;
	GetDText(prefDlog, base + 12, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersOffSet = temp;
	
	ComputeGrilleGWorld();
	UpdateMozartInfo();
	UpdateEditorInfo();
	UpdateStaffInfo();
}

void InitDRIVER(void)
{
	short			itemType;
	Handle			itemHandle;
	Rect			itemRect;
	Str255			aStr, bStr;
	NumVersion		nVers;
	long			nVersLong;
	UnsignedFixed	tRate;
	/////
	
	/**/
	
	nVers = SndSoundManagerVersion();
	
	pStrcpy(aStr, "\pSound Manager ");
	NumToString(nVers.majorRev, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p.");
	NumToString(nVers.minorAndBugRev >> 4, bStr);
	pStrcat(aStr, bStr);
	if ((nVers.minorAndBugRev & 0x0F) > 0) {
		pStrcat(aStr, "\p.");
		NumToString(nVers.minorAndBugRev & 0x0F, bStr);
		pStrcat(aStr, bStr);
	}
	GetDialogItem (prefDlog, base + 2, &itemType, &itemHandle, &itemRect);
	SetControlTitle((ControlHandle) itemHandle, aStr);
	
	/**/
#if MACOS9VERSION
	nVersLong = OMSVersion();
#else
	nVersLong = 0;
#endif
	if (nVersLong > 0) {
		BlockMoveData(&nVersLong, &nVers, 4);
		
		pStrcpy(aStr, "\pMIDI Output - (OMS ");
		NumToString(nVers.majorRev, bStr);
		pStrcat(aStr, bStr);
		pStrcat(aStr, "\p.");
		NumToString(nVers.minorAndBugRev >> 4, bStr);
		pStrcat(aStr, bStr);
		if ((nVers.minorAndBugRev & 0x0F) > 0) {
			pStrcat(aStr, "\p.");
			NumToString(nVers.minorAndBugRev & 0x0F, bStr);
			pStrcat(aStr, bStr);
		}
		
		pStrcat(aStr, "\p)");
		GetDialogItem (prefDlog, base + 5, &itemType, &itemHandle, &itemRect);
		SetControlTitle((ControlHandle) itemHandle, aStr);
	}
	/**/
	
	//	TurnRadio(base + 15, prefDlog, thePrefs.antiAliasing);
	thePrefs.antiAliasing	= false;
	
	TurnRadio(base + 30, prefDlog, thePrefs.surround);
	
	ControlSwitch(base + 11, prefDlog, 255);
	
	GetKeys(km);
	
	if (IsPressed(0x31) == false) {
		if (Audio16 == false) {
			ControlSwitch(base + 8, prefDlog, 255);
			ControlSwitch(base + 10, prefDlog, 255);
		}
		
		if (MIDIHardware == false) {
			ControlSwitch(base + 5, prefDlog, 255);
			ControlSwitch(base + 28, prefDlog, 255);
		}
			
			GetSoundOutputInfo(NULL, siSampleRate, &tRate);
			
			if (tRate < rate44khz)
				ControlSwitch(base + 8, prefDlog, 255);
			if (tRate < rate22050hz)
				ControlSwitch(base + 7, prefDlog, 255);
			if (tRate < rate11025hz)
				ControlSwitch(base + 6, prefDlog, 255);
		
	}
	ControlSwitch(base + 3, prefDlog, 255);
	SetDText(prefDlog, base + 25, "\p");
	
	switch (thePrefs.driverMode) {
		case MIDISoundDriver:
			TurnRadio(base + 5, prefDlog, true);
			break;
			
		case ASIOSoundManager:
			TurnRadio(base + 3, prefDlog, true);
			
			ControlSwitch(base + 9, prefDlog, 255);
			thePrefs.outPutBits = 16;	// 8 Bits
			ControlSwitch(base + 6, prefDlog, 255);
			thePrefs.outPutRate = rate44khz;
			ControlSwitch(base + 7, prefDlog, 255);
			break;
			
		case SoundManagerDriver:
			TurnRadio(base + 2, prefDlog, true);
			
				ControlSwitch(base + 10, prefDlog, 0);
				ControlSwitch(base + 8, prefDlog, 0);
			
			ControlSwitch(base + 6, prefDlog, 0);
			ControlSwitch(base + 7, prefDlog, 0);
			ControlSwitch(base + 12, prefDlog, 0);
			
				GetSoundOutputInfo(NULL, siSampleRate, &tRate);
				
				if (tRate < rate44khz)
					ControlSwitch(base + 8, prefDlog, 255);
				if (tRate < rate22050hz)
					ControlSwitch(base + 7, prefDlog, 255);
				if (tRate < rate11025hz)
					ControlSwitch(base + 6, prefDlog, 255);
			break;
	}
	
	TurnRadio(base + 9, prefDlog, false);
	TurnRadio(base + 10, prefDlog, false);
	switch (thePrefs.outPutBits) {
		case 8:
			TurnRadio(base + 9, prefDlog, true);
			break;
			
		case 16:
			TurnRadio(base + 10, prefDlog, true);
			break;
	}
	
	TurnRadio(base + 6, prefDlog, false);
	TurnRadio(base + 7, prefDlog, false);
	TurnRadio(base + 8, prefDlog, false);	
	switch (thePrefs.outPutRate) {
		case rate11025hz:
		case rate11khz:
			TurnRadio(base + 6, prefDlog, true);
			break;
			
		default:
		case rate22khz:
		case rate22050hz:
			TurnRadio(base + 7, prefDlog, true);
			break;
			
		case rate44khz:
			TurnRadio(base + 8, prefDlog, true);
			break;
	}
	
	if (thePrefs.oversampling > 1) {
		long	tempLong;
		Str255	str1, str2;
		
		TurnRadio(base + 12, prefDlog, true);
		
		tempLong = thePrefs.outPutRate >> 16;
		tempLong *= thePrefs.oversampling;
		tempLong /= 1000;
		
		NumToString(tempLong, str1);
		pStrcat(str1, "\p Khz / ");
		
		NumToString(thePrefs.oversampling, str2);
		pStrcat(str1, str2);
		pStrcat(str1, "\p x");
		
		SetDText(prefDlog, base + 13, str1);
	} else {
		TurnRadio(base + 12, prefDlog, false);
		SetDText(prefDlog, base + 13, "\pOff");
	}
	
	if (thePrefs.MicroDelaySize)
		TurnRadio(base + 20, prefDlog, true);
	else
		TurnRadio(base + 20, prefDlog, false);
	TurnRadio(base + 32, prefDlog, thePrefs.Reverb);
	
	NumToString(thePrefs.MicroDelaySize, aStr);
	pStrcat(aStr, "\p ms");
	SetDText(prefDlog, base + 4, aStr);
	
	NumToString(thePrefs.ReverbSize, aStr);
	pStrcat(aStr, "\p ms");
	SetDText(prefDlog, base + 31, aStr);
	
	NumToString(thePrefs.ReverbStrength, aStr);
	pStrcat(aStr, "\p %");
	SetDText(prefDlog, base + 15, aStr);
	
	ControlSwitch(base + 32, prefDlog, 0);
	ControlSwitch(base + 30, prefDlog, 0);
	
	if (thePrefs.outPutMode != DeluxeStereoOutPut) {
		TurnRadio(base + 20, prefDlog, false);
		ControlSwitch(base + 20, prefDlog, 255);
	} else {
		ControlSwitch(base + 20, prefDlog, 0);
	}
}

void InitMUSICLIST(void)
{
	switch (thePrefs.LoopType) {
		case 0:
		default:
			InverseRadio(base + 1, prefDlog);
			break;
			
		case 1:
			InverseRadio(base + 2, prefDlog);
			break;
			
		case 2:
			InverseRadio(base + 3, prefDlog);
			break;
			
		case 3:
			InverseRadio(base + 7, prefDlog);
			break;
	}
	
	if (thePrefs.AutomaticOpen)
		InverseRadio(base + 8, prefDlog);
	if (thePrefs.RememberMusicList)
		InverseRadio(base + 15, prefDlog);
	if (thePrefs.GoToStop)
		InverseRadio(base + 11, prefDlog);
	if (thePrefs.FastMusicList)
		InverseRadio(base + 6, prefDlog);
	if (thePrefs.SaveMusicList)
		InverseRadio(base + 12, prefDlog);
	if (thePrefs.AutoPlayWhenOpen)
		InverseRadio(base + 13, prefDlog);
}

void InitCOMPRESSION(void)
{
	short		i;
	MenuHandle	tMenu;
	Str255		str;
	
	TurnRadio(base + 1, prefDlog, thePrefs.MADCompression);
	
	//	ControlSwitch(base + 2, prefDlog, 255);
	//	TurnRadio(base + 2, prefDlog, thePrefs.MADC);
	
	//	TurnRadio(base + 5, prefDlog, thePrefs.ADAPremember);
	//	TurnRadio(base + 6, prefDlog, thePrefs.ADAPsave);
	//	TurnRadio(base + 8, prefDlog, thePrefs.ADAPuse);
	TurnRadio(base + 10, prefDlog, thePrefs.addExtension);
	TurnRadio(base + 16, prefDlog, thePrefs.clickSound);
	TurnRadio(base + 19, prefDlog, thePrefs.DontUseFilesMix);
	TurnRadio(base + 6, prefDlog, thePrefs.OscilloLine);
	
	switch (thePrefs.whichEditorPatterns) {
		case -1:
		default:
			i = 1;
			break;
			
		case RefPartition:
			i = 2;
			break;
			
		case RefMozart:
			i = 3;
			break;
			
		case RefStaff:
			i = 4;
			break;
			
		case RefWave:
			i = 5;
			break;
			
		case RefClassic:
			i = 6;
			break;
	}
	
	tMenu = GetMenu(174);
	
	GetMenuItemText(tMenu, i, str);
	SetDText(prefDlog, base + 14, str);
	
	DisposeMenu(tMenu);
}

void InitCOLOR(short colorID)
{
	short		itemType,i, x, larg, haut;
	Handle		itemHandle;
	Rect		itemRect, aRc;
	Str255		aStr;
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(prefDlog);
	
	GetDialogItem (prefDlog, base + 1, &itemType, &itemHandle, &itemRect);
	
	larg = itemRect.right - itemRect.left;
	larg /= 8;
	
	haut = itemRect.bottom - itemRect.top;
	haut /= 12;
	
	itemRect.right = itemRect.left + larg*8 + 2;
	itemRect.bottom = itemRect.top + haut*12 + 2;
	
	if (colorID == -1) {
		PaintRect(&itemRect);
		
		for (i = 0; i< 8; i++) {
			for (x = 0; x < 12; x++) {
				aRc.left = itemRect.left + i*larg + 2;
				aRc.right = aRc.left + larg - 2;
				
				aRc.top = itemRect.top + x*haut + 2;
				aRc.bottom = aRc.top + haut - 2;
				
				FrameRect(&aRc);
				SwitchColorBack(i + 8*x);
				ForeColor(blackColor);
				NTStr(2, i + 8*x + 1, (Ptr) aStr);
				
				MyC2PStr((Ptr) aStr);
				MyTETextBox(&thePrefs.tracksColor[ i + 8*x], &aRc, aStr, true);
			}
		}
	} else {
		x = colorID / 8;
		i = colorID - x * 8;
		
		aRc.left = itemRect.left + i*larg + 2;
		aRc.right = aRc.left + larg - 2;
		
		aRc.top = itemRect.top + x*haut + 2;
		aRc.bottom = aRc.top + haut - 2;
		
		FrameRect(&aRc);
		SwitchColorBack(i + 8*x);
		ForeColor(blackColor);
		NTStr(2, i + 8*x + 1, (Ptr) aStr);
		
		MyC2PStr((Ptr) aStr);
		MyTETextBox(&thePrefs.tracksColor[ i + 8*x], &aRc, aStr, true);
		
		//TETextBox((Ptr) aStr, 2, &aRc, teCenter);
	}
	
	ForeColor(blackColor);
	RGBBackColor(&theColor);
	
	SetPort(savePort);
}

void DoMUSICLIST(short itemHit)
{
	short		itemType,i;
	Handle		itemHandle;
	Rect		itemRect;
	
	itemHit -= base;
	
	switch (itemHit) {
		case 1:
		case 2:
		case 3:
		case 7:
			for(i = 1; i <= 3; i++) {
				GetDialogItem (prefDlog, i + base, &itemType, &itemHandle, &itemRect);
				if (i != itemHit)
					SetControlValue((ControlHandle)itemHandle, 0);
			}
			GetDialogItem (prefDlog, 7 + base, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 0);
			
			GetDialogItem (prefDlog, itemHit+base, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 1);
			
			if (itemHit != 7)
				thePrefs.LoopType = itemHit - 1;
			else
				thePrefs.LoopType = 3;
			
			if (thePrefs.LoopType != 0)
				MADDriver->DriverSettings.repeatMusic = false;
			else
				MADDriver->DriverSettings.repeatMusic = true;
			break;
			
		case 8:
			InverseRadio(base + 8, prefDlog);
			thePrefs.AutomaticOpen = !thePrefs.AutomaticOpen;
			break;
			
		case 15:
			InverseRadio(base + 15, prefDlog);
			thePrefs.RememberMusicList = !thePrefs.RememberMusicList;
			break;
			
		case 11:
			InverseRadio(base + 11, prefDlog);
			thePrefs.GoToStop = !thePrefs.GoToStop;
			break;
			
		case 12:
			InverseRadio(base + 12, prefDlog);
			thePrefs.SaveMusicList = !thePrefs.SaveMusicList;
			break;
			
		case 13:
			InverseRadio(base + 13, prefDlog);
			thePrefs.AutoPlayWhenOpen = !thePrefs.AutoPlayWhenOpen;
			break;
			
		case 6:
			InverseRadio(base + 6, prefDlog);
			thePrefs.FastMusicList = !thePrefs.FastMusicList;
			
			UpdateMODListInfo();
			break;
	}
}
#define	COLARG 15

static short CurrentCase;

void PaintMyRect(void)
{
	Rect	aRect;
	
	aRect.left = (CurrentCase - (CurrentCase/32) * 32) * COLARG;
	aRect.left += 2;
	
	aRect.top = (CurrentCase/32) * COLARG;
	aRect.top += 2;
	
	aRect.bottom = aRect.top + COLARG;
	aRect.right = aRect.left + COLARG;
	
	FrameRect(&aRect);
	InsetRect(&aRect, -1, -1);
	FrameRect(&aRect);
}

void DrawChooseColorWindow(void)
{
	Rect			aRect;
	CTabHandle		hCTab;
	RGBColor		aCol = {0, 0, 0};
	short			i, x;
	
	hCTab = GetCTable(72);
	
	ForeColor(blackColor);
	
	for (i = 0; i < 8; i++) {
		for (x = 0; x < 32; x++) {
			aRect.top = i*COLARG;
			aRect.top += 3;
			
			aRect.left = x*COLARG;
			aRect.left += 3;
			
			aRect.bottom = aRect.top + COLARG - 2;
			aRect.right = aRect.left + COLARG - 2;
			
			RGBForeColor(&(*hCTab)->ctTable[ i * 32 + x].rgb);
			PaintRect(&aRect);
		}
	}
	
	ForeColor(redColor);
	PaintMyRect();
	ForeColor(blackColor);
	RGBForeColor(&aCol);
	
	//DisposeCTable(hCTab);
}

void DoCOLOR(short itemHit)
{
	short		itemType, larg, haut;
	Handle		itemHandle;
	Rect		itemRect, aRc;
	Point		myPt, where = {-1, -1};
	Rect		caRect;
	
	itemHit -= base;
	
	switch (itemHit) {
		case 1:
			GetDialogItem (prefDlog, itemHit+base, &itemType, &itemHandle, &itemRect);
			
			larg = itemRect.right - itemRect.left;
			larg /= 8;
			
			haut = itemRect.bottom - itemRect.top;
			haut /= 12;
			
			GetMouse(&myPt);
			
			myPt.h -= itemRect.left;
			myPt.h /= larg;
			
			myPt.v -= itemRect.top;
			myPt.v /= haut;
			
			aRc.left = itemRect.left + myPt.h*larg + 1;
			aRc.right = aRc.left + larg;
			aRc.top = itemRect.top + myPt.v*haut + 1;
			aRc.bottom = aRc.top + haut;
			
			ForeColor(redColor);
			FrameRect(&aRc);
			InsetRect(&aRc, -1, -1);
			FrameRect(&aRc);
			
			while (Button()) {}
			
			ForeColor(blackColor);
			FrameRect(&aRc);
			InsetRect(&aRc, 1, 1);
			FrameRect(&aRc);
			
			GetColor(where, "\pSelect a new color", &thePrefs.tracksColor[myPt.h + 8 * myPt.v], &thePrefs.tracksColor[ myPt.h + 8*myPt.v]);
			
			InitCOLOR(myPt.h + 8*myPt.v);
			
			if (ClassicDlog != NULL) {
				SetPortDialogPort(ClassicDlog);
				GetPortBounds(GetDialogPort(ClassicDlog), &caRect);
				InvalWindowRect(GetDialogWindow(ClassicDlog), &caRect);
			}
			
			if (EditorDlog != NULL) {
				SetPortDialogPort(EditorDlog);
				GetPortBounds(GetDialogPort(EditorDlog), &caRect);
				InvalWindowRect(GetDialogWindow(EditorDlog), &caRect);
			}
			
			if (AdapDlog != NULL) {
				SetPortDialogPort(AdapDlog);
				GetPortBounds(GetDialogPort(AdapDlog), &caRect);
				InvalWindowRect(GetDialogWindow(AdapDlog), &caRect);
			}
			
			SetPortDialogPort(prefDlog);
			break;
	}
}

void CreateTrackList()
{
	short		itemType;
	int			i;
	Handle		itemHandle;
	Rect		tempRect, dataBounds;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		aStr;
	
	TextFont(4);
	TextSize(9);
	
	GetDialogItem(prefDlog, base+19, &itemType, &itemHandle, &TrackRectList);
	
	GetFontInfo(&ThisFontInfo);
	cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading;
	
	tempRect = TrackRectList;
	tempRect.right = tempRect.right - 15;
	
	if (tempRect.right <= (tempRect.left + 15))
		tempRect.right = tempRect.left + 15;
	
	tempRect.right /= 3;
	tempRect.right *= 3;
	
	InsetRect(&tempRect, -1, -1);
	FrameRect(&tempRect);
	InsetRect(&tempRect, 1, 1);
	SetRect(&dataBounds, 0, 0, 3, 33);
	cSize.h = (tempRect.right - tempRect.left)/3;
	/****************/
	
	TrackList =  LNew(&tempRect,
					  &dataBounds,
					  cSize,
					  0,
					  GetDialogWindow(prefDlog),
					  true,
					  false,
					  false,
					  true);
	//	(*TrackList)->selFlags = ;
	
	cSize.h = cSize.v = 0;
	LSetDrawingMode(true, TrackList);
	
	for (i = 0; i < 99; i++) {	//
		NumToString(i + 1, aStr);
		
		LSetCell(aStr + 1, aStr[0], cSize, TrackList);
		
		if (thePrefs.SelectedTracks[i])
			LSetSelect(true, cSize, TrackList);
		else
			LSetSelect(false, cSize, TrackList);
		
		cSize.h++;
		
		if (cSize.h % 3 == 0) {
			cSize.v++;
			cSize.h = 0;
		}
	}
	
	TextFont(4);
	TextSize(9);
}

void InitEDITOR(void)
{
	short		itemType;
	Handle		itemHandle;
	Rect		tempRect, dataBounds;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		aStr, bStr;
	
	TextFont(4);
	TextSize(9);
	
	GetDialogItem(prefDlog, base+6, &itemType, &itemHandle, &PianoRectList);
	
	GetFontInfo(&ThisFontInfo);
	cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading;
	
	tempRect = PianoRectList;
	tempRect.right = tempRect.right - 15;
	
	if (tempRect.right <= (tempRect.left + 15))
		tempRect.right = tempRect.left + 15;
	
	InsetRect(&tempRect, -1, -1);
	FrameRect(&tempRect);
	InsetRect(&tempRect, 1, 1);
	SetRect(&dataBounds, 0, 0, 1, NUMBER_NOTES+1);
	cSize.h = (tempRect.right - tempRect.left);
	/****************/	
	
	PianoList =  LNew(&tempRect, 
					  &dataBounds,
					  cSize,
					  0,
					  GetDialogWindow(prefDlog),
					  true,
					  false,
					  false,
					  true);
	(*PianoList)->selFlags = lOnlyOne + lNoExtend;
	
	DrawAllNote();
	
	////////////////////////////////////////////////////
	
	TrackList = NULL;
	
	if (thePrefs.RecordAllTrack == 3) {
		CreateTrackList();
	}
	////////////////////////////////////////////////////
	
	switch (thePrefs.KeyUpMode) {
		case eNothing:
			TurnRadio(base + 13, prefDlog, true);
			break;
			
		case eNoteOFF:
			TurnRadio(base + 7, prefDlog, true);
			break;
			
		case eStop:
			TurnRadio(base + 4, prefDlog, true);
			break;
	}
	
	TurnRadio(base + 3, prefDlog, thePrefs.PianoOctaveMarkers);
	TurnRadio(base + 9, prefDlog, thePrefs.SmallPiano);
	
	TurnRadio(base + 2, prefDlog, thePrefs.MacKeyBoard);
	
	TextFont(0);
	TextSize(0);
	
	/**/
	
	NumToString(LastCanal + 1, aStr);
	SetDText(prefDlog, base + 24, aStr);
	
	if (thePrefs.pianoOffset >= 0) {
		pStrcpy(aStr, "\p+ ");
		NumToString(thePrefs.pianoOffset, bStr);
	} else {
		pStrcpy(aStr, "\p- ");
		NumToString(-thePrefs.pianoOffset, bStr);
	}
	pStrcat(aStr, bStr);
	SetDText(prefDlog, base + 10, aStr);
	
	switch (thePrefs.RecordAllTrack) {
		case 1:
			TurnRadio(base + 25, prefDlog, true);
			break;
			
		case 0:
			TurnRadio(base + 22, prefDlog, true);
			break;
			
		case 2:
			TurnRadio(base + 14, prefDlog, true);
			break;
			
		case 3:
			TurnRadio(base + 28, prefDlog, true);
			break;
	}
	
	/**/
}

void InitMIDI()
{
	Str255		aStr, bStr;
	NumVersion	nVers;
	long		nVersLong;
	
	if (MIDIHardware) {
		TurnRadio(base + 6, prefDlog, thePrefs.MidiKeyBoard);
		TurnRadio(base + 10, prefDlog, thePrefs.SendMIDIClockData);
	} else {
		ControlSwitch(base + 6, prefDlog, 255);		// MIDI Input
		ControlSwitch(base + 10, prefDlog, 255);		// MIDI Clock
		
		ControlSwitch(base + 2, prefDlog, 255);		// Set Up
		//ControlSwitch(base + 9, prefDlog, 255);		// Activate
		ControlSwitch(base + 11, prefDlog, 255);		// Set Up
	}
	
#if 0
	if (MIDIHardwareAlreadyOpen)
	{
		ControlSwitch(base + 9, prefDlog, 255);
	}
#endif
	
	TurnRadio(base + 3, prefDlog, thePrefs.MIDIChanInsTrack);
	TurnRadio(base + 4, prefDlog, thePrefs.MIDIVelocity);
	TurnRadio(base + 5, prefDlog, thePrefs.FinePositioning);
	
	/**/
	
#if MACOS9VERSION
	nVersLong = OMSVersion();
#else 
	nVersLong = 0;
#endif
	if (nVersLong > 0) {
		BlockMoveData(&nVersLong, &nVers, 4);
		
		pStrcpy(aStr, "\pMIDI (OMS ");
		NumToString(nVers.majorRev, bStr);
		pStrcat(aStr, bStr);
		pStrcat(aStr, "\p.");
		NumToString(nVers.minorAndBugRev >> 4, bStr);
		pStrcat(aStr, bStr);
		if ((nVers.minorAndBugRev & 0x0F) > 0)
		{
			pStrcat(aStr, "\p.");
			NumToString(nVers.minorAndBugRev & 0x0F, bStr);
			pStrcat(aStr, bStr);
		}
		
		pStrcat(aStr, "\p)");
		SetDText(prefDlog, base + 7, aStr);
	}
	else 
		ControlSwitch(base + 2, prefDlog, 255);
}

void DoMIDI(short itemHit)
{
	switch (itemHit - base) {
		case 10:
			InverseRadio(itemHit, prefDlog);
			thePrefs.SendMIDIClockData = !thePrefs.SendMIDIClockData;
			
			MADDriver->SendMIDIClockData = thePrefs.SendMIDIClockData;
			
			if (MADDriver->SendMIDIClockData) {
				OpenMIDIHardware();
			}
			break;
			
		case 11:
			SelectOMSConnections(false);
			break;
			
		case 5:
			InverseRadio(itemHit, prefDlog);
			thePrefs.FinePositioning = !thePrefs.FinePositioning;
			break;
			
		case 2:
			SelectOMSConnections(true);
			break;
			
		case 6:
			InverseRadio(itemHit, prefDlog);
			thePrefs.MidiKeyBoard = !thePrefs.MidiKeyBoard;
			break;
			
		case 3:
			InverseRadio(itemHit, prefDlog);
			thePrefs.MIDIChanInsTrack = !thePrefs.MIDIChanInsTrack;
			break;
			
		case 4:
			InverseRadio(itemHit, prefDlog);
			thePrefs.MIDIVelocity = !thePrefs.MIDIVelocity;
			break;
			
#if 0
		case 9:
			ControlSwitch(itemHit, prefDlog, 255);
			ControlSwitch(base + 2, prefDlog, 0);
			
			OpenMIDIHardware();
			break;
#endif
	}
}

void DoEDITOR(short itemHit)
{
	short		itemType,i;
	Handle		itemHandle;
	Rect		itemRect;
	Point		myPt;
	long		mresult;
	Str255		theStr;
	
	switch (itemHit - base) {
		case 14:
		case 22:
		case 25:
		case 28:
			TurnRadio(base + 22, prefDlog, false);
			TurnRadio(base + 25, prefDlog, false);
			
			TurnRadio(base + 14, prefDlog, false);
			TurnRadio(base + 28, prefDlog, false);
			
			
			TurnRadio(itemHit, prefDlog, true);
			
			switch (itemHit - base) {
				case 14:
					thePrefs.RecordAllTrack = 2;
					break;
					
				case 22:
					thePrefs.RecordAllTrack = 0;
					break;
					
				case 25:
					thePrefs.RecordAllTrack = 1;
					break;
					
				case 28:
					thePrefs.RecordAllTrack = 3;
					break;
			}
			
			if (thePrefs.RecordAllTrack == 3) {
				if (TrackList == NULL)
					CreateTrackList();
			} else {
				if (TrackList) {
					Point cSize = {0, 0};
					
					for (i = 0; i < 99; i++) {
						thePrefs.SelectedTracks[i] = LGetSelect(false, &cSize, TrackList);
						
						cSize.h++;
						if (cSize.h % 3 == 0) {
							cSize.v++;
							cSize.h = 0;
						}
					}
					
					LDispose(TrackList);
					InsetRect(&TrackRectList, -1, -1);
					EraseRect(&TrackRectList);
					InsetRect(&TrackRectList, 1, 1);
					TrackList = NULL;
				}
			}
			
			InitRecording();
			break;
			
		case 23:
			InsertMenu(TrackView, hierMenu);
			GetDialogItem(prefDlog, itemHit, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;
			myPt.h = itemRect.left;
			
			LocalToGlobal(&myPt);
			
			SetItemMark(TrackView, LastCanal + 1, 0xa5);
			
			mresult = PopUpMenuSelect(TrackView, myPt.v, myPt.h, LastCanal + 1);
			
			SetItemMark(TrackView, LastCanal + 1, 0);
			
			if (HiWord(mresult) != 0) {
				LastCanal = (Byte)LoWord(mresult) - 1;
				
				NumToString(LastCanal + 1, theStr);
				SetDText(prefDlog, base + 24, theStr);
			}
			DeleteMenu(GetMenuID(TrackView));
			
			InitRecording();
			break;
			
			
		case 13:
			TurnRadio(base + 13, prefDlog, false);
			TurnRadio(base + 7, prefDlog, false);
			TurnRadio(base + 4, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			thePrefs.KeyUpMode = eNothing;
			break;
			
		case 7:
			TurnRadio(base + 13, prefDlog, false);
			TurnRadio(base + 7, prefDlog, false);
			TurnRadio(base + 4, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			thePrefs.KeyUpMode = eNoteOFF;
			break;
			
		case 4:
			TurnRadio(base + 13, prefDlog, false);
			TurnRadio(base + 7, prefDlog, false);
			TurnRadio(base + 4, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			thePrefs.KeyUpMode = eStop;
			break;
			
		case 9:
			InverseRadio(itemHit, prefDlog);
			thePrefs.SmallPiano = !thePrefs.SmallPiano;
			break;
			
		case 3:
			InverseRadio(itemHit, prefDlog);
			thePrefs.PianoOctaveMarkers = !thePrefs.PianoOctaveMarkers;
			break;
			
		case 2:
			InverseRadio(itemHit, prefDlog);
			thePrefs.MacKeyBoard = !thePrefs.MacKeyBoard;
			break;
			
	}
}

void CloseEDITORF()
{
	Rect itemRect;

	LDispose(PianoList);
	itemRect = PianoRectList;
	InsetRect(&itemRect, -1, -1);
	FrameRect(&itemRect);
	
	//ForeColor(whiteColor);
	EraseRect(&itemRect);
	//ForeColor(blackColor);

	if (TrackList) {
		Point	cSize = {0, 0};
		int		i;
		
		for (i = 0; i < 99; i++) {
			thePrefs.SelectedTracks[i] = LGetSelect(false, &cSize, TrackList);
			
			cSize.h++;
			if (cSize.h % 3 == 0) {
				cSize.v++;
				cSize.h = 0;
			}
		}
		
		LDispose(TrackList);
		itemRect = TrackRectList;
		InsetRect(&itemRect, -1, -1);
		FrameRect(&itemRect);
		
		//ForeColor(whiteColor);
		EraseRect(&itemRect);
		//ForeColor(blackColor);
		
		TrackList = NULL;
	}
	
	UpdatePressChar();
	UpdatePianoInfo();
}

void DoCOMPRESSION(short itemHit)
{
	short		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	Str255		str;
	
	switch (itemHit - base) {
		case 1:
			InverseRadio(itemHit, prefDlog);
			
			thePrefs.MADCompression = !thePrefs.MADCompression;
			break;
#if 0
		case 2:
			InverseRadio(itemHit, prefDlog);		thePrefs.MADC = !thePrefs.MADC;
			break;
			
		case 5:
			InverseRadio(itemHit, prefDlog);		thePrefs.ADAPremember = !thePrefs.ADAPremember;
			break;
			
			
		case 8:
			InverseRadio(itemHit, prefDlog);		thePrefs.ADAPuse = !thePrefs.ADAPuse;
			break;
#endif
		case 19:
			InverseRadio(itemHit, prefDlog);
			thePrefs.DontUseFilesMix = !thePrefs.DontUseFilesMix;
			break;
			
		case 16:
			InverseRadio(itemHit, prefDlog);
			thePrefs.clickSound = !thePrefs.clickSound;
			break;
			
		case 6:
			InverseRadio(itemHit, prefDlog);
			thePrefs.OscilloLine = !thePrefs.OscilloLine;
			ResetOscilloscope();
			break;
			
		case 10:
			InverseRadio(itemHit, prefDlog);
			thePrefs.addExtension = !thePrefs.addExtension;
			break;
			
		case 2: // POPUP
		case 14:
		{
			long		mresult;
			Point		Zone;
			short		temp = 0;
			MenuHandle	tMenu;
			
			GetDialogItem(prefDlog, base + 2, &itemType, &itemHandle, &itemRect);
			
			tMenu = GetMenu(174);
			InsertMenu(tMenu, hierMenu);
			
			Zone.h = itemRect.left;
			Zone.v = itemRect.top;
			
			LocalToGlobal(&Zone);
			
			switch (thePrefs.whichEditorPatterns) {
				case -1:
					temp = 1;
					break;
					
				case RefPartition:
					temp = 2;
					break;
					
				case RefMozart:
					temp = 3;
					break;
					
				case RefStaff:
					temp = 4;
					break;
					
				case RefWave:
					temp = 5;
					break;
					
				case RefClassic:
					temp = 6;
					break;
			}
			
			mresult = PopUpMenuSelect(tMenu, Zone.v, Zone.h, temp);
			
			if (HiWord(mresult) != 0) {
				temp = LoWord(mresult);
				
				switch (temp) {
					case 1:
						thePrefs.whichEditorPatterns = -1;
						break;
						
					case 2:
						thePrefs.whichEditorPatterns = RefPartition;
						break;
						
					case 3:
						thePrefs.whichEditorPatterns = RefMozart;
						break;
						
					case 4:
						thePrefs.whichEditorPatterns = RefStaff;
						break;
						
					case 5:
						thePrefs.whichEditorPatterns = RefWave;
						break;
						
					case 6:
						thePrefs.whichEditorPatterns = RefClassic;
						break;
				}
				
				GetMenuItemText(tMenu, temp, str);
				SetDText(prefDlog, base + 14, str);
			}
			
			DeleteMenu(GetMenuID(tMenu));
			DisposeMenu(tMenu);
			
		}
			break;
	}
}

void DoCLASSICAL(short itemHit)
{
	switch (itemHit - base) {
		case 5:
			thePrefs.UseMarkers = !thePrefs.UseMarkers;
			TurnRadio(5 + base, prefDlog, thePrefs.UseMarkers);
			break;
			
		case 11:
			thePrefs.UseOctaveMarkers = !thePrefs.UseOctaveMarkers;
			TurnRadio(11 + base, prefDlog, thePrefs.UseOctaveMarkers);
			break;
			
		case 4:
			thePrefs.ClassicalProjection = !thePrefs.ClassicalProjection;
			TurnRadio(4 + base, prefDlog, thePrefs.ClassicalProjection);
			break;
	}
}

void DoKEY(short itemHit)
{
	short	i;
	Str255	str;
	
	switch (itemHit - base) {
		case 42:
			TurnRadio(base + 42, prefDlog, true);
			TurnRadio(base + 1, prefDlog, false);
			
			thePrefs.FKeyTracks = true;
			
			for (i = 0; i < 13; i++) {
				//TurnRadio(base + 2 + i, prefDlog, thePrefs.FKeyActive[ i]);
				InactiveControl(base + 2 + i, prefDlog);
			}
			break;
			
		case 1:
			TurnRadio(base + 42, prefDlog, false);
			TurnRadio(base + 1, prefDlog, true);
			
			thePrefs.FKeyTracks = false;
			
			for (i = 0; i < 13; i++) {
				//TurnRadio(base + 2 + i, prefDlog, thePrefs.FKeyActive[ i]);
				ActiveControl(base + 2 + i, prefDlog);
			}
			break;
			
		default:
			if (itemHit - base >= 2 && itemHit - base <= 14) {
				thePrefs.FKeyActive[itemHit - base - 2] = !thePrefs.FKeyActive[itemHit - base - 2];
				TurnRadio(itemHit, prefDlog, thePrefs.FKeyActive[itemHit - base - 2]);
			}
			else if (itemHit - base >= 15 && itemHit - base <= 27) {
				if (PressFKeyMenu(itemHit, prefDlog, itemHit - base - 15, str))
					SetDText(prefDlog, itemHit - 15 + 28, str);
			}
			break;
	}
}

void GetFKeyDesc(short whichFKey, Str255 str);

void InitKEY()
{
	short 	i;
	Str255	str;
	
	if (thePrefs.FKeyTracks) {
		//TurnRadio(base + 42, prefDlog, true);
		TurnRadio(base + 1, prefDlog, false);
		
		for (i = 0; i < 13; i++) {
			//TurnRadio(base + 2 + i, prefDlog, thePrefs.FKeyActive[ i]);
			InactiveControl(base + 2 + i, prefDlog);
		}
	} else {
		TurnRadio(base + 42, prefDlog, false);
		TurnRadio(base + 1, prefDlog, true);
	}
	
	for (i = 0; i < 13; i++) {
		if (thePrefs.FKeyActive[i]) {
			GetFKeyDesc(i, str);
			SetDText(prefDlog, base + 28 + i, str);
		} else
			SetDText(prefDlog, base + 28 + i, "\p-");
		
		TurnRadio(base + 2 + i, prefDlog, thePrefs.FKeyActive[ i]);
	}
}

void InitSTAFF()
{
	Str255		aStr;
	
	TurnRadio(8 + base, prefDlog, thePrefs.StaffShowLength);
	TurnRadio(3 + base, prefDlog, thePrefs.UseMarkers);
	
	NumToString(thePrefs.MarkersSize, aStr);
	SetDText(prefDlog, base + 5, aStr);
	NumToString(thePrefs.MarkersOffSet, aStr);
	SetDText(prefDlog, base + 7, aStr);
	
	NumToString(thePrefs.TempsNum, aStr);
	SetDText(prefDlog, base + 13, aStr);
	NumToString(thePrefs.TempsUnit, aStr);
	SetDText(prefDlog, base + 11, aStr);
	
	NumToString(thePrefs.TrackHeight, aStr);
	SetDText(prefDlog, base + 15, aStr);
}

void DoSTAFF(short itemHit)
{
	Str255	aStr;
	long	temp;
	
	switch (itemHit - base) {
		case 8:
			thePrefs.StaffShowLength = !thePrefs.StaffShowLength;
			TurnRadio(8 + base, prefDlog, thePrefs.StaffShowLength);
			break;
			
		case 16:
			temp = thePrefs.TrackHeight;
			if (PressMenuItem(171, base + 16, prefDlog, &temp, 0, aStr)) {
				thePrefs.TrackHeight = temp;
				SetDText(prefDlog, base + 15, aStr);
			}
			break;
			
		case 3:
			thePrefs.UseMarkers = !thePrefs.UseMarkers;
			TurnRadio(3 + base, prefDlog, thePrefs.UseMarkers);
			break;
	}
}

Boolean CloseSTAFF()
{
	Str255		aStr;
	long		temp;
	
	GetDText(prefDlog, base + 11, aStr);
	StringToNum(aStr, &temp);
	if (temp < 2 || temp > 16) {
		SysBeep(1);
		SelectDialogItemText(prefDlog, base + 11, 0, 32767);
		return false;
	} else
		thePrefs.TempsUnit = temp;
	
	GetDText(prefDlog, base + 13, aStr);
	StringToNum(aStr, &temp);
	if (temp < 2 || temp > 16) {
		SysBeep(1);
		SelectDialogItemText(prefDlog, base + 13, 0, 32767);
		return false;
	} else
		thePrefs.TempsNum = temp;
	
	GetDText(prefDlog, base + 5, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersSize = temp;
	GetDText(prefDlog, base + 7, aStr);
	StringToNum(aStr, &temp);
	thePrefs.MarkersOffSet = temp;
	
	ComputeGrilleGWorld();
	UpdateMozartInfo();
	UpdateEditorInfo();
	UpdateStaffInfo();
	
	return true;
}

void DoDIGITAL(short itemHit)
{
	short		itemType;
	Handle		itemHandle;
	Rect		caRect, itemRect;
	Point		where = {-1, -1};
	
	switch (itemHit - base) {
		case 35:
		case 34:
			thePrefs.editorSoundDrag = !thePrefs.editorSoundDrag;
			
			TurnRadio(35 + base, prefDlog, false);
			TurnRadio(34 + base, prefDlog, false);
			
			if (thePrefs.editorSoundDrag)
				TurnRadio(34 + base, prefDlog, true);
			else
				TurnRadio(35 + base, prefDlog, true);
			break;
			
		case 30:
		case 31:
			thePrefs.patternWrapping = !thePrefs.patternWrapping;
			
			TurnRadio(30 + base, prefDlog, false);
			TurnRadio(31 + base, prefDlog, false);
			
			if (thePrefs.patternWrapping)
				TurnRadio(30 + base, prefDlog, true);
			else
				TurnRadio(31 + base, prefDlog, true);
			break;
			
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
			InverseRadio(itemHit, prefDlog);
			
			switch (itemHit - base) {
				case 1:
					thePrefs.DigitalInstru = !thePrefs.DigitalInstru;
					break;
					
				case 2:
					thePrefs.DigitalNote = !thePrefs.DigitalNote;
					break;
					
				case 3:
					thePrefs.DigitalEffect = !thePrefs.DigitalEffect;
					break;
					
				case 4:
					thePrefs.DigitalArgu = !thePrefs.DigitalArgu;
					break;
					
				case 6:
					thePrefs.DigitalVol = !thePrefs.DigitalVol;
					break;
			}
			
			InternalEditorInit();
			break;
			
		case 14:
			GetColor(where, "\pSelect a new color", &thePrefs.yellC, &thePrefs.yellC);
			
			GetDialogItem(prefDlog, base + 14, &itemType, &itemHandle, &itemRect);
			InvalWindowRect(GetDialogWindow(prefDlog), &itemRect);
			
			if (EditorDlog != NULL) {
				SetPortDialogPort(EditorDlog);
				GetPortBounds(GetDialogPort(EditorDlog), &caRect);
				InvalWindowRect(GetDialogWindow(EditorDlog), &caRect);
			}
			break;
			
		case 27:
			thePrefs.MusicTrace = !thePrefs.MusicTrace;
			
			TurnRadio(27 + base, prefDlog, false);
			TurnRadio(28 + base, prefDlog, false);
			if (thePrefs.MusicTrace)
				TurnRadio(28 + base, prefDlog, true);
			else
				TurnRadio(27 + base, prefDlog, true);
			break;
			
		case 28:
			thePrefs.MusicTrace = !thePrefs.MusicTrace;
			
			TurnRadio(27 + base, prefDlog, false);
			TurnRadio(28 + base, prefDlog, false);
			if (thePrefs.MusicTrace)
				TurnRadio(28 + base, prefDlog, true);
			else
				TurnRadio(27 + base, prefDlog, true);
			break;
			
		case 16:
			if (thePrefs.FastDigitalEdition & controlKey)
				thePrefs.FastDigitalEdition &= ~controlKey;
			else
				thePrefs.FastDigitalEdition |= controlKey;
			
			InverseRadio(itemHit, prefDlog);
			break;
			
		case 17:
			if (thePrefs.FastDigitalEdition & shiftKey)
				thePrefs.FastDigitalEdition &= ~shiftKey;
			else
				thePrefs.FastDigitalEdition |= shiftKey;
			
			InverseRadio(itemHit, prefDlog);
			break;
			
		case 18:
			if (thePrefs.FastDigitalEdition & cmdKey)
				thePrefs.FastDigitalEdition &= ~cmdKey;
			else
				thePrefs.FastDigitalEdition |= cmdKey;
			
			InverseRadio(itemHit, prefDlog);
			break;
			
		case 19:
			if (thePrefs.FastDigitalEdition & optionKey)
				thePrefs.FastDigitalEdition &= ~optionKey;
			else
				thePrefs.FastDigitalEdition |= optionKey;
			
			InverseRadio(itemHit, prefDlog);
			break;
			
		case 8:
			thePrefs.UseMarkers = !thePrefs.UseMarkers;
			TurnRadio(8 + base, prefDlog, thePrefs.UseMarkers);
			
			if (EditorDlog != NULL) {
				SetPortDialogPort(EditorDlog);
				GetPortBounds(GetDialogPort(EditorDlog), &caRect);
				InvalWindowRect(GetDialogWindow(EditorDlog), &caRect);
			}
			break;
			
		case 22:
		case 23:
			TurnRadio(22 + base, prefDlog, false);
			TurnRadio(23 + base, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			
			switch (itemHit - base) {
				case 22:
					thePrefs.LinesHeight = 0;
					break;
					
				case 23:
					thePrefs.LinesHeight = 2;
					break;
			}
			break;
	}
}


void UpdatePrefWindow(DialogPtr	theDia)
{
	GrafPtr		savePort;
	Rect		itemRect;
	short		itemType;
	Handle		itemHandle;
	RgnHandle	visibleRegion;
	
	GetPort(&savePort);
	SetPortDialogPort(theDia);
	
	BeginUpdate(GetDialogWindow(theDia));
	
	DrawDialog(theDia);
	oldFrameButton(theDia);
	
	switch (PrefsType) {
		case COLOR:
			InitCOLOR(-1);
			break;
			
		case DRIVER:
			break;
			
		case CLASSICAL:
			break;
			
		case EDITOR:
			TextFont(4);
			TextSize(9);
			
			itemRect = PianoRectList;
			InsetRect(&itemRect, -1, -1);
			FrameRect(&itemRect);
			
			visibleRegion = NewRgn();
			
			GetPortVisibleRegion(GetDialogPort(theDia), visibleRegion);
			
			LUpdate(visibleRegion, PianoList);
			
			if (TrackList) {
				itemRect = TrackRectList;
				InsetRect(&itemRect, -1, -1);
				FrameRect(&itemRect);
				
				LUpdate(visibleRegion, TrackList);
			}
			
			DisposeRgn(visibleRegion);
			
			TextFont(0);
			TextSize(0);
			break;
			
		case DIGITAL:
			GetDialogItem(theDia, base + 14, &itemType, &itemHandle, &itemRect);
			RGBForeColor(&thePrefs.yellC);
			PaintRect(&itemRect);
			ForeColor(blackColor);
			FrameRectRelief(&itemRect);
			break;
			
		case TRUECLASSIC:
			break;
			
		case FKEYPREF:
			break;
	}
	
	EndUpdate(GetDialogWindow(theDia));
	
	SetPort(savePort);
}

Boolean	IsPressed(unsigned short k)
{
	unsigned char *km2;
	km2 = (unsigned char*)&km;
	return (Boolean)((km2[k >> 3] >> (k & 7)) & 1);
}

short GetTracksMenuPos(short Noni)
{
	short curSelec = Noni / 2;
	if (curSelec <= 0 || curSelec > MAXTRACK / 2)
		curSelec = 2;
	return curSelec;
}

short GetTracksMenuVal(short Noni)
{
	short curSelec = Noni * 2;
	if (curSelec <= 0 || curSelec > MAXTRACK)
		curSelec = 4;
	return curSelec;
}

short GetRateMenuPos(UnsignedFixed Noni)
{
	switch (Noni) {
		case rate5khz:
			return 1;
			break;
			
		case rate11khz:
			return 2;
			break;
			
		case rate16khz:
			return 3;
			break;
			
		case rate22khz:
		default:
			return 4;
			break;
			
		case rate44khz:
			return 5;
			break;
			
		case rate48khz:
			return 6;
			break;
			
	}
}

UnsignedFixed GetRateMenuVal(short Noni)
{
	switch (Noni) {
		case 1:
			return rate5khz;
			break;
			
		case 2:
			return rate11khz;
			break;
			
		case 3:
			return rate16khz;
			break;
			
		case 4:
			return rate22khz;
			break;
			
		case 5:
		default:
			return rate44khz;
			break;
			
		case 6:
			return rate48khz;
			break;
	}
}	

#define CSTTEST	4000L

void TestProcessor(void)
{
	short		proto = 1;
	long		oldTicks, normal;
	int			i;
	short		dItem;
	Str255		theString;
	GrafPtr		savedPort;
	Boolean		MyRead;

	MyRead = MADDriver->Reading;
	MADStopDriver(MADDriver);
	
	oldTicks = TickCount();
	for (i = 0 ; i < CSTTEST; i++) {
		proto *= 123;
		proto -= 255;
		proto /= 791;
	}
	normal = TickCount() - oldTicks;
	
	MADStartDriver(MADDriver);
	MADDriver->Reading = true;
	oldTicks = TickCount();
	for (i = 0 ; i < CSTTEST; i++) {
		proto *= 123;
		proto -= 255;
		proto /= 791;
	}
	
	i = TickCount() - oldTicks;

	MADDriver->Reading = MyRead;

	GetPort(&savedPort);

	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	NumToString((long)((i * 100L) / normal), theString);

	ParamText(theString, 0, 0, 0);

	dItem = Alert(131, NULL);

	UpdateALLWindow();
	
	SetPort(savedPort);
}

Boolean PressMenuItem(short menuID, short itemID, DialogPtr dia, long *curVal, short remove, Str255	str)
{
	long		mresult, i, curSelec = 1;
	Point		Zone;
	short		temp;
	MenuHandle	tMenu;
	Boolean		returnVal = false;
	
	short		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	
	GetDialogItem(dia, itemID, &itemType, &itemHandle, &itemRect);
	
	tMenu = GetMenu(menuID);
	InsertMenu(tMenu, hierMenu);
	
	Zone.h = itemRect.left;
	Zone.v = itemRect.top;
	
	LocalToGlobal(&Zone);
	
	for (i = 0 ; i < CountMenuItems(tMenu); i++) {
		Str255	str;
		long	r;
		
		GetMenuItemText(tMenu, i+1, str);
		
		str[0] -= remove;
		StringToNum(str, &r);
		str[0] += remove;
		
		if (r == *curVal)
			curSelec = i + 1;
	}
	
	SetItemMark(tMenu, curSelec, 0xa5);
	
	mresult = PopUpMenuSelect(tMenu, Zone.v, Zone.h, curSelec);
	
	SetItemMark(tMenu, curSelec, 0);
	
	if (HiWord(mresult) != 0) {
		long	r;
		
		temp = LoWord(mresult);
		
		GetMenuItemText(tMenu, temp, str);
		str[0] -= remove;
		StringToNum(str, &r);
		str[0] += remove;
		
		*curVal = r;
		
		returnVal = true;
	}
	
	DeleteMenu(GetMenuID(tMenu));
	DisposeMenu(tMenu);
	
	return returnVal;
}

void DoDRIVER(short	itemHit)
{
	Str255			aStr;
	UnsignedFixed	tRate;
	
	switch (itemHit - base) {
		case 28:
			SelectOMSConnections(false);
			break;
			
		case 6:
		case 7:
		case 8:
			TurnRadio(base + 6, prefDlog, false);
			TurnRadio(base + 7, prefDlog, false);
			TurnRadio(base + 8, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			
			switch (itemHit - base) {
				case 6:
					thePrefs.outPutRate = rate11khz;
					
					
					GetSoundOutputInfo(NULL, siSampleRate, &tRate);
					
					if (tRate == rate11025hz ||
						tRate == rate22050hz) {
						thePrefs.outPutRate = rate11025hz;
					}
					
					break;
					
				case 7:
					thePrefs.outPutRate = rate22khz;
					
					GetSoundOutputInfo(NULL, siSampleRate, &tRate);
					
					if (tRate == rate11025hz ||
						tRate == rate22050hz) {
						thePrefs.outPutRate = rate22050hz;
					}
					
					break;
					
				case 8:
					thePrefs.outPutRate = rate44khz;
					
					GetSoundOutputInfo(NULL, siSampleRate, &tRate);
					
					if (tRate == rate11025hz ||
						tRate == rate22050hz) {
						thePrefs.outPutRate = rate22050hz;
					}
					
					break;
			}
			
			InitDRIVER();
			SwitchDriverTo(0);
			
			//ResetAdaptators();
			SetWindowEnviron();
			SetWindowSpectrum();
			RecomputeEQ();
			break;
			
		case 2:
		case 3:
		case 5:
			TurnRadio(base + 2, prefDlog, false);
			TurnRadio(base + 3, prefDlog, false);
			TurnRadio(base + 5, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			
			switch (itemHit - base) {
				case 2:
					thePrefs.driverMode = SoundManagerDriver;
					break;
					
				case 3:
					thePrefs.driverMode = ASIOSoundManager;
					break;
					
				case 5:
					thePrefs.driverMode = MIDISoundDriver;
					break;
			}
			InitDRIVER();
			SwitchDriverTo(0);
			InitDRIVER();
			
			ResetAdaptators();
			SetWindowEnviron();
			SetWindowSpectrum();
			RecomputeEQ();
			break;
			
		case 9:
		case 10:
			TurnRadio(base + 9, prefDlog, false);
			TurnRadio(base + 10, prefDlog, false);
			
			TurnRadio(itemHit, prefDlog, true);
			
			switch (itemHit - base) {
			case 9:
					thePrefs.outPutBits = 8;
					break;
					
			case 10:
					thePrefs.outPutBits = 16;
					break;
		}
			InitDRIVER();
			SwitchDriverTo(0);
			break;
			
		case 30:
			InverseRadio(base + 30, prefDlog);
			thePrefs.surround = !thePrefs.surround;
			MADDriver->DriverSettings.surround = thePrefs.surround;
			break;
			
		case 32:
			InverseRadio(base + 32, prefDlog);
			thePrefs.Reverb = !thePrefs.Reverb;
			
			SwitchDriverTo(0);
			break;
			
		case 12:
			InverseRadio(base + 12, prefDlog);
			if (thePrefs.oversampling > 1) {
				thePrefs.oversampling = 1;
				
				SetDText(prefDlog, base + 13, "\pOff");
			} else {
				long 	tempLong;
				Str255	str1, str2;
				
				thePrefs.oversampling = 4;
				
				tempLong = thePrefs.outPutRate >> 16;
				tempLong *= thePrefs.oversampling;
				tempLong /= 1000;
				
				NumToString(tempLong, str1);
				pStrcat(str1, "\p Khz / ");
				
				NumToString(thePrefs.oversampling, str2);
				pStrcat(str1, str2);
				pStrcat(str1, "\p x");
				
				SetDText(prefDlog, base + 13, str1);
			}
			
			SwitchDriverTo(0);
			
			SetWindowEnviron();
			SetWindowSpectrum();
			RecomputeEQ();
			break;
			
		case 17:
		case 25:
			if (/*asioDrivers != NULL*/ 1) {
				
			}
			break;
			
		case 35:
			if (PressMenuItem(175, base + 35, prefDlog, &thePrefs.oversampling, 2, aStr)) {
				long 	tempLong;
				Str255	str1, str2;
				
				tempLong = thePrefs.outPutRate >> 16;
				tempLong *= thePrefs.oversampling;
				tempLong /= 1000;
				
				NumToString(tempLong, str1);
				pStrcat(str1, "\p Khz / ");
				
				NumToString(thePrefs.oversampling, str2);
				pStrcat(str1, str2);
				pStrcat(str1, "\p x");
				
				SetDText(prefDlog, base + 13, str1);
				
				if (thePrefs.oversampling > 1)
					TurnRadio(base + 12, prefDlog, true);
				else
					TurnRadio(base + 12, prefDlog, false);
				
				SwitchDriverTo(0);
			}
			break;
			
		case 20:
			InverseRadio(base + 20, prefDlog);
			if (thePrefs.MicroDelaySize)
				thePrefs.MicroDelaySize = 0;
			else
				thePrefs.MicroDelaySize = 25;
			
			NumToString(thePrefs.MicroDelaySize, aStr);
			pStrcat(aStr, "\p ms");
			SetDText(prefDlog, base + 4, aStr);
			
			SwitchDriverTo(0);
			break;
			
		case 26:
			if (PressMenuItem(165, base + 26, prefDlog, &thePrefs.MicroDelaySize, 3, aStr)) {
				SetDText(prefDlog, base + 4, aStr);
				
				if (thePrefs.MicroDelaySize)
					TurnRadio(base + 20, prefDlog, true);
				else
					TurnRadio(base + 20, prefDlog, false);
				
				SwitchDriverTo(0);
			}
			break;
			
		case 33:
			if (PressMenuItem(170, base + 33, prefDlog, &thePrefs.ReverbSize, 3, aStr)) {
				SetDText(prefDlog, base + 31, aStr);
				SwitchDriverTo(0);
			}
			break;
			
		case 34:
			if (PressMenuItem(169, base + 34, prefDlog, &thePrefs.ReverbStrength, 2, aStr)) {
				SetDText(prefDlog, base + 15, aStr);
				SwitchDriverTo(0);
			}
			break;
	}
}

void InitPrefs(void)
{
	PrefsType = DRIVER;
}

void ShowPrefs(short PrefStartup)
{
	short			itemType,i;
	Handle			itemHandle;
	Rect			itemRect;
	short			itemHit, temp;
	GrafPtr			OldPort;
	Prefs			*copyPrefs;
	
	copyPrefs = (Prefs*)NewPtrClear(sizeof(Prefs));
	*copyPrefs = thePrefs;
	
	//ModalFilterUPP	MyPrefFilterDesc = NewModalFilterUPP(PrefFilter);
	
	GetPort(&OldPort);
	
	prefDlog = GetNewDialog(133, NULL, (WindowPtr) -1L);
	SetPortDialogPort(prefDlog);
	
	AutoPosition(prefDlog);
	
	base = CountDITL(prefDlog);
	
	if (PrefStartup != -1)
		PrefsType = PrefStartup;
	AppendPref(PrefsType, prefDlog);
	switch (PrefsType) {
		case COLOR:
			/*InitCOLOR();*/
			break;
			
		case DRIVER:
			InitDRIVER();
			break;
			
		case MUSICLIST:
			InitMUSICLIST();
			break;
			
		case EDITOR:
			InitEDITOR();
			break;
			
		case COMPRESSION:
			InitCOMPRESSION();
			break;
			
		case CLASSICAL:
			InitCLASSICAL();
			break;
			
		case DIGITAL:
			InitDIGITAL();
			break;
			
		case TRUECLASSIC:
			InitSTAFF();
			break;
			
		case FKEYPREF:
			InitKEY();
			break;
			
		case MIDIINPUT:
			InitMIDI();
			break;
	}
	
	GetDialogItem(prefDlog, 3, &itemType, &itemHandle, &itemRect);
	SetControlValue((ControlHandle)itemHandle, PrefsType + 1 - 2000);
	
	do {
		ErreurClose:
		
		//ModalDialog(MyPrefFilterDesc, &itemHit);
		MyModalDialog(prefDlog, &itemHit);
		
		if (itemHit == 3) {
			GetDialogItem (prefDlog, itemHit, &itemType, &itemHandle, &itemRect);
			temp = GetControlValue((ControlHandle)itemHandle) - 1;
			temp += 2000;
			
			if (temp != PrefsType) {
				switch (PrefsType) {
					case FKEYPREF:
						break;
						
					case TRUECLASSIC:
						if (!CloseSTAFF())
							goto ErreurClose;
						break;
						
					case CLASSICAL:
						CloseCLASSICAL();
						break;
						
					case EDITOR:
						CloseEDITORF();
						break;
						
					case DIGITAL:
						CloseDIGITAL();
						break;
				}
				PrefsType = temp;
				
				AppendPref(PrefsType, prefDlog);
				switch (PrefsType) {
					case COLOR:
						/*InitCOLOR();*/
						break;
						
					case DRIVER:
						InitDRIVER();
						break;
						
					case MUSICLIST:
						InitMUSICLIST();
						break;
						
					case EDITOR:
						InitEDITOR();
						break;
						
					case COMPRESSION:
						InitCOMPRESSION();
						break;
						
					case CLASSICAL:
						InitCLASSICAL();
						break;
						
					case DIGITAL:
						InitDIGITAL();
						break;
						
					case TRUECLASSIC:
						InitSTAFF();
						break;
						
					case FKEYPREF:
						InitKEY();
						break;
						
					case MIDIINPUT:
						InitMIDI();
						break;
				}
			}
		}
		
		switch (PrefsType) {
			case COLOR:
				DoCOLOR(itemHit);
				break;
				
			case COMPRESSION:
				DoCOMPRESSION(itemHit);
				break;
				
			case DRIVER:
				DoDRIVER(itemHit);
				break;
				
			case MUSICLIST:
				DoMUSICLIST(itemHit);
				break;
				
			case EDITOR:
				DoEDITOR(itemHit);
				break;
				
			case MIDIINPUT:
				DoMIDI(itemHit);
				break;
				
			case CLASSICAL:
				DoCLASSICAL(itemHit);
				break;
				
			case FKEYPREF:
				DoKEY(itemHit);
				break;
				
			case TRUECLASSIC:
				DoSTAFF(itemHit);
				break;
				
			case DIGITAL:
				DoDIGITAL(itemHit);
				break;
		}
	} while (itemHit != 1 && itemHit != 2);
	
	switch (PrefsType) {
		case FKEYPREF:
			
			break;
			
		case TRUECLASSIC:
			if (!CloseSTAFF()) goto ErreurClose;
			break;
			
		case CLASSICAL:
			CloseCLASSICAL();
			break;
			
		case DIGITAL:
			CloseDIGITAL();
			break;
			
		case EDITOR:
			CloseEDITORF();
			break;
	}
	
	if (itemHit == 1) {
		WritePreferences();
	} else {
		long	tSize;
		Ptr		aPtr, bPtr;
		
		tSize = sizeof(Prefs);
		
		aPtr = (Ptr) &thePrefs;
		bPtr = (Ptr) copyPrefs;
		
		for (i = 0; i < tSize ; i++) {
			if (aPtr[i] != bPtr[i]) {
				thePrefs = *copyPrefs;
				
				SwitchDriverTo(0);
				
				ResetOscilloscope();
				
				ResetAdaptators();
				SetWindowEnviron();
				SetWindowSpectrum();
				RecomputeEQ();
				
				i = tSize;
				
				break;
			}
		}
		
	}
	
	DisposeDialog(prefDlog);
	//DisposeRoutineDescriptor(MyPrefFilterDesc);
	DisposePtr((Ptr) copyPrefs);
	SetPort(OldPort);
}
