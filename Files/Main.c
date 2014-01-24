#define MAINSYSTEM2

#include <Carbon/Carbon.h>
#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "Help.h"
#include <QuickTime/QuickTime.h>
#include "PPResDefs.h"
#include "PPPrivate.h"
#include <CoreMIDI/CoreMIDI.h>
#include "PreferenceHandler.h"

void DoChangeLoop(void);
void VSTEditorDoNull(void);
void VSTEditorDoItemPress(short itemHit, DialogPtr aDia);
void VSTEditorUpdate(DialogPtr aDia);
Boolean VSTEditorClose(DialogPtr aDia, short itemHit);
void 	CloseVSTPlug(void);
void 	CallVST(short no);
void 	InitVSTPlug(void);
void 	VSTActivate(short no, Boolean state);
void  	UpdateVisualWindow(DialogPtr GetSelection);
void 	CloseVisual(void);
void 	CreateVisualWindow(short ID);
void 	DoItemPressVisual(short whichItem, DialogPtr whichDialog);
void 	InitVisual(void);
void 	DoVisualNull();
void 	VisualFullScreen(void);
void 	InitFourier(void);
void 	CloseFourier(void);
short 	maxVisualPlug(void);
void 	HandleVSTChoice(short item, VSTEffect **);
void 	DoKeyVisual(DialogPtr whichDialog);
void 		CloseBookMarks();
void 		InitBookMarks();
Boolean		TestProcessorChip(void);
void 		DoInternetMenu(short theItem);
void		WindowsSettingsInit();
void		MyNullHook();
void		HandlePrefSubMenu(short item);
void		PressFKey(short whichFKey);
void		AESendOpenFile(FSSpec *spec);
void 		DoNullMODList();
void		HandleNewSound(short theItem);
void 		RecordButtonPiano();
void		NCOPYSample(long, long, short, short);
void 		NPASTESample(long, short, short);
void 		DoGrowPiano();
void		HandlePatternChoice(short);
Boolean		CheckTimePL();
void 		HandleExportFile(short);
void 		HandleImportFile(short);
void 		ClosePlayerWindow(DialogPtr);
void		DoNullMemWindow();
short		GetPatternSelect();
void 		ScrollTextClose(Boolean);
void 		ScrollTextInit(Boolean, Boolean);
void 		NPianoRecordProcess(short, short, short, short);
Boolean 	PageSetUp();
void	 	DragManagerInstalled();
void 		CreateHelpOnline(short whichSection);
void 		ShowSection(short selectionID);
Boolean 	DialogPatternInfo(short thePos);
void 		DeleteAPattern(Boolean AskDelete);
void		ChangeMozartTracks(short Id);
Boolean		FindInternal(short FindType);
Boolean		AcceptKeysTools();
void		DoPreferences();
void 		WriteCText (DialogPtr dlog, short item, char *str);
void		OpenMODListSTCf(FSSpec);
void		NOpenMusic();
void 		SetcurMusicListFile(Boolean	val);
void		ProcessDoItemPress(long ref, short whichItem, DialogPtr whichDialog);
OSType 		Ptr2OSType(Ptr str);
MADMusic*	CreateFreeMADI();
OSErr		inMADPlaySoundData(Ptr soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, unsigned long rate, Boolean stereo);
void		ImportAudioCD();
void		WindowsSettingsMenu(short item);
void		COPYQuicktime();
void		InitFFTSampleFilter(void);

extern pascal void MyDlgFilterNav(		NavEventCallbackMessage 	callBackSelector, 
								NavCBRecPtr 				callBackParms, 
								NavCallBackUserData 		callBackUD);

WindowPtr NextWindowVisible(WindowPtr	whichWindow);

//#define VERSION 0x05A0 //since we don't have different preferences, keeping old value
#define	VERSION			0x0592
#define	PLAYERPREF		"\pPlayerPRO Prefs"
#define PLAYERBOOK		"\pPlayerPRO Bookmarks"
#define MLSAVENAME		"\pMusic List"
#define wInHelpButton 	20
#define	MAXURL			90

static	MADMusic	*SwitchCurMusic, *tempCurMusic;
static	Boolean		AlternateBuffer;
extern	char 		ASIO_DRIVER_NAME[ 255];

enum
{
	Find = 0,
	Next,
	ReplaceFind,
	ReplaceAll
};

enum
{
	allMusics = 1,
	allReadable = 2,
	allFiles = 3,
	MADK = 5,
	others = 7
};

static short FKEYCo[ 16] = { 0x7A, 0x78, 0x63, 0x76, 0x60, 0x61, 0x62, 0x64, 0x65, 0x6D, 0x67, 0x6F, 0x69, 0x6B, 0x71};

	// Valeur pour DoNULL
	
				WindowPtr		oldWindow;
				Boolean			DebuggingMode, PatchSave, HelpAvalaible;
				Cursor			HelpCrsr, NoteCrsr, DelCrsr, PlayCrsr, HandCrsr, beamCrsr, pencilCrsr, pencilCrsrStereo, CHandCrsr, ContextCrsr;
				Cursor			ZoomInCrsr, ZoomOutCrsr;
				RGBColor		theColor;
				short			mainVRefNum, theOldDepth;
				long			mainParID;
				Boolean			newQuicktime;

				ProcessSerialNumber				playerPROPSN;
	
				SndListHandle	InSound, OutSound;
	static		short			WindowStateNOW;
	static		short			MainResFile;
		 		short			showWhat;
	static		short			Quality, Mode;
	static		Boolean			NoSelectWindow2, NOWScrollMode, ShowIt;
	
	static		Str255			URLs[ MAXURL], URLsDesc[ MAXURL];
	static		long			URLsNo;
	static		RgnHandle		gCursorRgn;

				MenuHandle		WinSettingsMenu, OsciTypeMenu, SpectrumTypeMenu, OsciHMenu, OsciModeMenu, NewSoundMenu, PrefSubMenu;
				
	
	extern		Boolean			MIDIHardware;
	extern		KeyMap			km;
	extern		PixMapHandle	PianoPix;	//, EditorPix;
	extern		SndChannelPtr	SKChannel;
	extern		DialogPtr 		EQDlog, VisualDlog, EditorDlog, theProgressDia, AdapDlog, MODListDlog, FindDlog, ToolsDlog, PatListDlog, PianoDlog, MozartDlog, InstruViewDlog, PartiDlog;
	extern		DialogPtr		QuicktimeDlog, DigitalDlog, CubeDlog, ClassicDlog, InstruListDlog, TrackViewDlog, MemoryDlog, OscilloDlog, SpectrumDlog, HelpDlog, AHelpDlog, StaffDlog, WaveDlog;
	extern		MenuHandle		TrackView;
	extern		short			LastCanal, ToneGenerator;
	extern		Boolean			QuicktimeInstruAvailable, MicroPhone, CubeReady, SpectrumMicrophone, OscilloMicrophone, PianoRecording;
	extern		VSTEffect*		masterVST[ MAXVST];

#define TEMPNAME "\pTemp PlayerPRO File"

/*void LMGetKeyMap()
{

}*/

void GetSizeString(long size, Str255 str, Boolean Convert)
{
	if (Convert) {
		if (size < 9999) {
			NumToString(size, str);
			pStrcat(str, "\p B");
		} else {
			NumToString(size/1024, str);
			pStrcat(str, "\p KiB");
		}
	} else {
		NumToString(size, str);
	}
}

void CreateTempFile()
{
	FSSpec	spec;
	OSErr	iErr;
	
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
	if (iErr == noErr) {
		FSMakeFSSpec(spec.vRefNum, spec.parID, TEMPNAME, &spec);
		iErr = FSpCreate(&spec, 'xxxx', 'xxxx', smSystemScript);
	}
}

OSErr inMADPlaySoundData(Ptr soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, unsigned long rate, Boolean stereo)
{
	OSErr iErr = MADPlaySoundData(MADDriver, soundPtr, size, channel, note, amplitude, 0, 0, rate, stereo);
	Boolean	continueLoop;
	
	if (iErr == noErr)
	{
		continueLoop = true;
		while (continueLoop)
		{
			GetKeys(km);
			
			if (MADDriver->chan[ channel].samplePtr == NULL) continueLoop = false;
			if (MADIsPressed((unsigned char*) km, 0x37) && MADIsPressed((unsigned char*) km, 0x2F)) continueLoop = false;
			if (Button()) continueLoop = false;
			DoGlobalNull();
		}
		
		if (MADDriver->chan[ channel].samplePtr != NULL)
		{
			MADDriver->chan[ channel].curPtr 		= MADDriver->chan[ channel].maxPtr;
			MADDriver->chan[ channel].samplePtr	= NULL;
			MADDriver->chan[ channel].lAC			= 0;
			MADDriver->chan[ channel].loopBeg 	= 0;
			MADDriver->chan[ channel].loopSize 	= 0;
		}
	}
	
	return iErr;
}

void DeleteTempFile()
{
	FSSpec	spec;
	OSErr	iErr;
	
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
	
	pStrcpy(spec.name, TEMPNAME);
	iErr = FSpDelete(&spec);
}

EXP void MyDebugStr(short line, Ptr file, Ptr text)
{
	fprintf(stderr, "PlayerPRO: %s:%u error text:%s!\n", file, line, text);
	
	Str255	fileS, lineS, dateS, textS;
	
	NumToString(line, lineS);
	strcpy((Ptr) fileS, file);
	strcpy((Ptr) textS, text);
	strcpy((Ptr) dateS, __DATE__);
	
	MyC2PStr((Ptr) fileS);
	MyC2PStr((Ptr) textS);
	MyC2PStr((Ptr) dateS);
	
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

void CloseALLWindow(void)
{
	WindowPtr	aWind, tempWind;
	
	tempWind = FrontWindow();
	aWind = GetNextWindow(tempWind);
	
	while (aWind != NULL) {
		ClosePlayerWindow(GetDialogFromWindow(aWind));
		
		aWind = GetNextWindow(aWind);
	}
	
	ClosePlayerWindow(GetDialogFromWindow(tempWind));
}

void TESTBugs(void)
{
	Handle	ttHdl;
	
	do {
		ttHdl = NewHandleClear(5410);
		if (ttHdl == NULL) MyDebugStr(__LINE__, __FILE__, "");
		SetHandleSize(ttHdl, FreeMem() - 5000);
		if (ttHdl != NULL) MyDisposHandle(& ttHdl);
		
	} while (Button() == false);
	if (MemError() != noErr) MyDebugStr(__LINE__, __FILE__, "");
}

void SKVolume(short vol)
{
	MADSetHardwareVolume(vol);
	
	thePrefs.volumeLevel = vol;
}

void ClosePlayerWindow(DialogPtr	theDia)
{
	switch(GetWRefCon(GetDialogWindow(theDia)))
	{
		case RefVST:
			VSTEditorClose(theDia, 11);
			break;
			
		case RefStaff:
			CloseStaff();
			break;
			
		case RefWave:
			CloseWave();
			break;
			
		case RefCube:
			CloseCube();
			break;
			
		case RefSpectrum:
			CloseSpectrum();
			break;
			
		case RefPlayer:
			CloseOscillo();
			break;
			
		case RefMemory:
			CloseMem();
			break;
			
		case RefInstruList:
			CloseInstruListWindow();
			break;
			
		case RefInstruView:
			CloseInstruViewDlog();
			break;
			
			/*	case RefTrackView:
			 CloseTrackViewDlog();
			 break;*/
			
		case RefMozart:
			CloseMozartWindow();
			break;
			
			/*	case RefCmd:
			 CloseCmdDlog();
			 break;
			 */	
		case RefPartition:
			ClosePartitionWindow();
			break;
			
		case RefAdaptators:
			CloseAdapWindow();
			break;
			
		case RefMODList:
			CloseMODListWindow();
			break;
			
		case RefTools:
			HideWindow(GetDialogWindow(ToolsDlog));
			SetItemMark(ViewsMenu, mTools, noMark);
			break;
			
		case RefSample:
			CloseSampleWindow(theDia);
			break;
			
		case RefVisual:
			CloseVisual();
			break;
			
		case RefEqualizer:
			CloseEQ();
			break;
			
		case RefQuicktime:
			CloseQT();
			break;
			
		case RefPatList:
			ClosePatList();
			break;
			
		case RefParti:
			CloseParti();
			break;
			
		case RefHelp:
			CloseHelpOnline();
			break;
			
		case RefActiveHelp:
			CloseAHelp();
			break;
			
		case RefPiano:
			ClosePiano();
			break;
			
			/*	case RefFindReplace:
			 CloseFind();
			 break;
			 */	
		case RefClassic:
			CloseClassicWindow();
			break;
			
		case RefDigiView:
			CloseDigiList();
			break;
			
		case RefSndExport:
			StopAIFFExporting();
			break;
	}
}

void SetWindEtat(WindowPtr	whichWindow)
{
	Rect		tempRect, caRect;
	long		WRef = GetWRefCon(whichWindow);
	
	if (WindowStateNOW != -1)
	{
		if (thePrefs.WinPosO[ WindowStateNOW][ WRef].h != 0)
		{
			MoveWindow(whichWindow,
					   thePrefs.WinPosO[ WindowStateNOW][ WRef].h,
					   thePrefs.WinPosO[ WindowStateNOW][ WRef].v,
					   false);
			
			MoveWindow(whichWindow,
					   thePrefs.WinPosO[ WindowStateNOW][ WRef].h,
					   thePrefs.WinPosO[ WindowStateNOW][ WRef].v,
					   false);
			
			if (thePrefs.WinPosO[ WindowStateNOW][ WRef].v < GetMBarHeight()) {
				thePrefs.WinPosO[ WindowStateNOW][ WRef].v = 100;
				thePrefs.WinPosO[ WindowStateNOW][ WRef].h = 100;
				MoveWindow(	whichWindow,
						   100,
						   100,
						   false);
			}
			
			tempRect. top = thePrefs.WinPosO[ WindowStateNOW][ WRef].v;
			tempRect. left = thePrefs.WinPosO[ WindowStateNOW][ WRef].h;
			
			GetPortBounds(GetWindowPort(whichWindow), &caRect);
			
			tempRect. bottom = tempRect. top + caRect.bottom;
			tempRect. right = tempRect. left + caRect.right;
			
			if (!RectInRgn(&tempRect, GetGrayRgn()))
			{
				thePrefs.WinPosO[ WindowStateNOW][ WRef].v = 100;
				thePrefs.WinPosO[ WindowStateNOW][ WRef].h = 100;
				MoveWindow(	whichWindow,
						   100,
						   100,
						   false);
			}
		}
		
		if (thePrefs.WinHiO[ WindowStateNOW][ WRef] == 0) return;
		
		if (thePrefs.WinLargO[ WindowStateNOW][ WRef] != 0)
		{
			MySizeWindow(GetDialogFromWindow(whichWindow),
						 thePrefs.WinLargO[ WindowStateNOW][ WRef],
						 thePrefs.WinHiO[ WindowStateNOW][ WRef],
						 true);
		}
	}
	else
	{
		if (thePrefs.WinPos[ WRef].h != 0)
		{
			MoveWindow(whichWindow,
					   thePrefs.WinPos[ WRef].h,
					   thePrefs.WinPos[ WRef].v,
					   false);
			
			if (thePrefs.WinPos[ WRef].v < GetMBarHeight()) {
				thePrefs.WinPos[ WRef].v = 100;
				thePrefs.WinPos[ WRef].h = 100;
				MoveWindow(whichWindow,
						   100,
						   100,
						   false);
			}
			
			tempRect. top = thePrefs.WinPos[ WRef].v;
			tempRect. left = thePrefs.WinPos[ WRef].h;
			
			GetPortBounds(GetWindowPort(whichWindow), &caRect);
			
			tempRect. bottom = tempRect. top + caRect.bottom;
			tempRect. right = tempRect. left + caRect.right;
			
			if (!RectInRgn(&tempRect, GetGrayRgn()))
			{
				thePrefs.WinPos[ WRef].v = 100;
				thePrefs.WinPos[ WRef].h = 100;
				MoveWindow(whichWindow,
						   100,
						   100,
						   false);
			}
		}
		
		if (thePrefs.WinHi[ WRef] == 0) return;
		
		if (thePrefs.WinLarg[ WRef] != 0)
		{
			MySizeWindow(GetDialogFromWindow(whichWindow),
						 thePrefs.WinLarg[ WRef],
						 thePrefs.WinHi[ WRef],
						 true);
		}
	}
	
	ChangeDialogFont(GetDialogFromWindow(whichWindow));
}

extern long DBTick;

void CheckOneWindow(WindowPtr pWindow)
{
	Rect caRect;
	
	GetPortBounds(GetWindowPort(pWindow), &caRect);
	
	thePrefs.WinHi[ GetWRefCon(pWindow)] = caRect.bottom;
	thePrefs.WinLarg[ GetWRefCon(pWindow)] = caRect.right;
}

void SaveWindowState(short id)
{
	short		i, order;
	WindowPtr	pWindow;
	Point		Start;
	GrafPtr		savedPort;
	Rect		caRect;
	
	for (i= 0 ; i < MAXWINDOWS; i++) thePrefs.WinEtatO[ id][ i] = 0;	// Fenêtre inactive
	for (i= 0 ; i < MAXWINDOWS; i++) thePrefs.WinIDO[ id][ i] = -1;	// Aucune fenêtre
	
	pWindow = FrontWindow();
	order = 0;
	do {
		if (pWindow)
		{
			GetPortBounds(GetWindowPort(pWindow), &caRect);
			
			thePrefs.WinHiO[ id][ GetWRefCon(pWindow)] = caRect.bottom;
			thePrefs.WinLargO[ id][ GetWRefCon(pWindow)] = caRect.right;
			thePrefs.WinEtatO[ id][ GetWRefCon(pWindow)] = 1;						// Fenêtre active
			if (GetWRefCon(pWindow) != RefSample)	//GetWRefCon(pWindow) != RefTools && 
			{
				thePrefs.WinIDO[ id][ order] = GetWRefCon(pWindow);
				order++;
			}
			
			GetPort(&savedPort);
			SetPortWindowPort(pWindow);
			Start.h = Start.v = 0;
			LocalToGlobal(&Start);
			thePrefs.WinPosO[ id][ GetWRefCon(pWindow)].v = Start.v;
			thePrefs.WinPosO[ id][ GetWRefCon(pWindow)].h = Start.h;
			SetPort(savedPort);
		}
		
		pWindow = NextWindowVisible(pWindow);
		
	} while (pWindow != NULL);
}

void CheckShowWindow(void)
{
	short			i, order;
	WindowPtr		pWindow;
	GrafPtr			savedPort;
	Point			Start;
	Rect			caRect;
	
	for (i= 0 ; i < MAXWINDOWS; i++) thePrefs.WinEtat[ i] = 0;	// Fenêtre inactive
	for (i= 0 ; i < MAXWINDOWS; i++) thePrefs.WinID[ i] = -1;	// Aucune fenêtre
	
	order = 0;
	do
	{
		pWindow = FrontWindow();
		
		if (pWindow)
		{
			GetPortBounds(GetWindowPort(pWindow), &caRect);
			
			thePrefs.WinHi[ GetWRefCon(pWindow)] = caRect.bottom;
			thePrefs.WinLarg[ GetWRefCon(pWindow)] = caRect.right;
			thePrefs.WinEtat[ GetWRefCon(pWindow)] = 1;						// Fenêtre active
			
			if (GetWRefCon(pWindow) != RefSample)	//GetWRefCon(pWindow) != RefTools && 
			{
				thePrefs.WinID[ order] = GetWRefCon(pWindow);
				order++;
			}
			
			GetPort(&savedPort);
			SetPortWindowPort(pWindow);
			Start.h = Start.v = 0;
			LocalToGlobal(&Start);
			thePrefs.WinPos[ GetWRefCon(pWindow)].v = Start.v;
			thePrefs.WinPos[ GetWRefCon(pWindow)].h = Start.h;
			SetPort(savedPort);
			
			ClosePlayerWindow(GetDialogFromWindow(pWindow));
		}
	} while (pWindow != NULL);
	
	oldWindow = NULL;
	SelectWindow2(NULL);
}

void ShowWindowPref(short whichState)
{
	short	order;
	long	ID;
	
	if (whichState != -1)
	{
		WindowStateNOW = whichState;
		SetWindEtat(GetDialogWindow(ToolsDlog));
	}
	
	order = 29;
	while (order >= 0)
	{
		if (whichState == -1) ID = thePrefs.WinID[ order];
		else
		{
			WindowStateNOW = whichState;
			ID = thePrefs.WinIDO[ whichState][ order];
		}
		
		if (ID != -1)
		{
			switch(ID)
			{
				case RefStaff:			CreateStaffWindow();			if (StaffDlog != NULL) SelectWindow(GetDialogWindow(StaffDlog));		break;
				case RefWave:			CreateWaveWindow();				if (WaveDlog != NULL) SelectWindow(GetDialogWindow(WaveDlog));		break;
					//	case RefCube:			CreateCubeWindow();				if (CubeDlog != NULL) SelectWindow(GetDialogWindow(CubeDlog));		break;
				case RefInstruList:		CreateInstruListWindow();		if (InstruListDlog != NULL) SelectWindow(GetDialogWindow(InstruListDlog));	break;
				case RefPartition:		CreatePartitionWindow();		if (EditorDlog != NULL) SelectWindow(GetDialogWindow(EditorDlog));		break;
				case RefPatList:		CreatePatListWindow();			if (PatListDlog != NULL) SelectWindow(GetDialogWindow(PatListDlog));		break;
				case RefParti:			CreatePartiWindow();			if (PartiDlog != NULL) SelectWindow(GetDialogWindow(PartiDlog));		break;
				case RefClassic:		CreateClassicWindow();			if (ClassicDlog != NULL) SelectWindow(GetDialogWindow(ClassicDlog));		break;
				case RefMozart:			CreateMozartWindow();			if (MozartDlog != NULL) SelectWindow(GetDialogWindow(MozartDlog));		break;
				case RefInstruView:		CreateInstruView();				if (InstruViewDlog != NULL) SelectWindow(GetDialogWindow(InstruViewDlog));	break;
					//	case RefTrackView:		CreateTrackView();				if (TrackViewDlog != NULL) SelectWindow(GetDialogWindow(TrackViewDlog));	break;
				case RefDigiView:		CreateDigiListWindow();			if (DigitalDlog != NULL) SelectWindow(GetDialogWindow(DigitalDlog));		break;
				case RefHelp:			CreateHelpOnline(0);			if (HelpDlog != NULL) SelectWindow(GetDialogWindow(HelpDlog));		break;
				case RefAdaptators:		ShowWindow(GetDialogWindow(AdapDlog));			if (AdapDlog != NULL) {SetWindEtat(GetDialogWindow(AdapDlog)); SetUpScroll(); SelectWindow(GetDialogWindow(AdapDlog));}	SetItemMark(ViewsMenu, mAdap, checkMark);	break;
				case RefMODList:		SetWindEtat(GetDialogWindow(MODListDlog));		DoGrowMODList(MODListDlog);	if (MODListDlog != NULL) ShowWindow(GetDialogWindow(MODListDlog));	SelectWindow(GetDialogWindow(MODListDlog));		SetItemMark(FileMenu, 9, checkMark);	break;
				case RefPlayer:			CreateOscilloWindow();			if (OscilloDlog != NULL) SelectWindow(GetDialogWindow(OscilloDlog));		break;
				case RefPiano:			CreatePianoWindow();			if (PianoDlog != NULL) SelectWindow(GetDialogWindow(PianoDlog));		break;
				case RefMemory:			CreateMemWindow();				if (MemoryDlog != NULL) SelectWindow(GetDialogWindow(MemoryDlog));		break;
				case RefSpectrum:		CreateSpectrumWindow();			if (SpectrumDlog != NULL) SelectWindow(GetDialogWindow(SpectrumDlog));	break;
				case RefEqualizer:		CreateEQWindow();				if (EQDlog != NULL) SelectWindow(GetDialogWindow(EQDlog));	break;
				case RefVisual:			CreateVisualWindow(thePrefs.lastVisualPlugin);	if (VisualDlog != NULL) SelectWindow(GetDialogWindow(VisualDlog));	break;
			}
		}
		
		WindowStateNOW = -1;
		
		order--;
	}
}

WindowPtr NextWindowVisible(WindowPtr	whichWindow)
{
	//WindowPeek 	myWin;
	//WindowPeek	temp;
	WindowPtr	temp, myWin;
	Boolean		Sortie = false;
	
	do {
		myWin = whichWindow;
		temp = MacGetNextWindow(myWin);
		
		if (temp == NULL) return(NULL);
		else if (MacIsWindowVisible(temp) != true) whichWindow = (WindowPtr) MacGetNextWindow(myWin);
		else Sortie = true;
	} while (Sortie == false);
	
	return(temp);
}

void EraseGrowIcon(DialogPtr	theDialog)
{
	Rect	temp;
	GrafPtr	SavePort;
	Rect	caRect;
	
	switch(GetWRefCon(GetDialogWindow(theDialog)))
	{
		case RefMODList:
		case RefPatList:
		case RefParti:
		case RefWave:
		case RefStaff:
		case RefClassic:
		case RefPartition:
		case RefInstruList:
		case RefMozart:
		case RefSample:
		case RefPiano:
		case RefAdaptators:
		case RefDigiView:
		case RefPlayer:
		case RefSpectrum:
		case RefHelp:
			
			break;
			
		default:	return;		break;
	}
	
	GetPort(&SavePort);
	SetPortDialogPort(theDialog);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp.right = caRect.right;
	temp.bottom = caRect.bottom;
	temp.left = temp.right - 15;
	temp.top = temp.bottom - 14;
	EraseRect(&temp);
	InvalWindowRect(GetDialogWindow(theDialog), &temp);
	
	//InsetRect(&temp, -1, -1);
	//FrameRect(&temp);
	
	SetPort(SavePort);
}

void SelectWindow2(	WindowPtr	whichWindow)
{
	if (whichWindow != NULL)
	{
		if (IsValidWindowPtr(whichWindow)) SelectWindow(whichWindow);
		oldWindow = whichWindow;	//FrontNonFloatingWindow();
	}
}

//static Boolean HasDoneSwith;

void MusiqueDriverInit(void)
{
	OSErr				iErr;
	MADDriverSettings	init;
	
	
	/**** Initialise le MOD Driver ****/
	//curMusic = 0;
	//curMusic->header = (MADSpec*) NULL;
	//for (i = 0; i < MAXINSTRU ; i++)  for (x = 0; x < MAXSAMPLE ; x++) curMusic->sample[ i][ x] = (sData*) NULL;
	//for (i = 0; i < MAXPATTERN ; i++) curMusic->partition[ i] = (PatData*) NULL;
	/**********************************/
	
	if (thePrefs.numChn < 2 || thePrefs.numChn > 32) thePrefs.numChn = 4;
	
	init.numChn			= thePrefs.numChn;
	init.outPutBits		= thePrefs.outPutBits;
	init.outPutRate		= thePrefs.outPutRate;
	init.outPutMode		= DeluxeStereoOutPut;	// force DeluxeStereoOutPut
	init.driverMode		= thePrefs.driverMode;
	init.surround		= thePrefs.surround;
	init.MicroDelaySize	= thePrefs.MicroDelaySize;
	init.Reverb			= thePrefs.Reverb;
	init.ReverbSize		= thePrefs.ReverbSize;
	init.ReverbStrength	= thePrefs.ReverbStrength;
	init.sysMemory		= false;
	init.TickRemover	= thePrefs.TickRemover;
	init.oversampling	= thePrefs.oversampling;
	
	if (thePrefs.LoopType != 0) init.repeatMusic = false;
	else init.repeatMusic = true;
	
	//if (init.driverMode == ASCSoundDriver || init.driverMode == AWACSoundDriver) Switch();
	
	iErr = MADCreateDriver(&init, gMADLib, &MADDriver);
	if (iErr) MyDebugStr(__LINE__, __FILE__, "MusicDriver ERROR !");
	
	MADDriver->VolGlobal = thePrefs.softVolumeLevel;
	MADDriver->SendMIDIClockData = thePrefs.SendMIDIClockData;
}

Handle MyNewHandle(long	size)
{
	Handle		aPtr;
	
	if (size < 0) MyDebugStr(__LINE__, __FILE__, "MyNewHandle 1");
	
	aPtr = NewHandle(size);
	if (aPtr == NULL)
		return NULL;
	
	/*	HLock(aPtr);
	 for (aL = 0; aL < size; aL++) (*aPtr)[ aL] = 0xEE;
	 HUnlock(aPtr);*/
	
	if (MemError()) MyDebugStr(__LINE__, __FILE__, "MyNewHandle 4");
	
	return aPtr;
}


void MyDisposeHandle(Handle *aHandle)
{
	MyDisposHandle(aHandle);
}

void MyDisposHandle(Handle *aHandle)
{
	long	size;
	char	cMemTags;
	
	if (*aHandle == NULL) MyDebugStr(__LINE__, __FILE__, "*aHandle2");
	
	cMemTags = HGetState(*aHandle);
	
	size = GetHandleSize(*aHandle);
	if (size < 0) MyDebugStr(__LINE__, __FILE__, "aHandle33");
	else
	{
		/*	HLock(*aHandle);
		 for (aL = 0; aL < size; aL++) (**aHandle)[ aL] = 0xEE;
		 HUnlock(*aHandle);*/
	}
	
	DisposeHandle(*aHandle);
	
	*aHandle = (Handle) NULL;
	
	if (MemError()) MyDebugStr(__LINE__, __FILE__, "aHandle55");
}

Ptr MyNewPtr(long size)
{
	Ptr		aPtr;
	
	if (size < 0) MyDebugStr(__LINE__, __FILE__, "MyNewPtr 1");
	
	aPtr = NewPtr(size);
	if (aPtr == NULL) return NULL;
	
	if (MemError()) MyDebugStr(__LINE__, __FILE__, "MyNewPtr 1");
	
	return aPtr;
}

void MyDisposePtr(Ptr *aPtr)
{
	long	size;
	
	if (*aPtr == NULL) MyDebugStr(__LINE__, __FILE__, "pCodeMyDisposePtr == NULL");
	
	size = GetPtrSize(*aPtr);
	if (size < 0) MyDebugStr(__LINE__, __FILE__, "pCodeMyDisposePtr : size < 0");
	
	DisposePtr(*aPtr);
	
	*aPtr = NULL;
	
	if (MemError())
	{
		MyDebugStr(__LINE__, __FILE__, "DisposePtr");
	}
}

/*************************************************/

#define HIM		14
#define WIM		27

void GetMDEFRect(Rect *aRect, Rect *menuRect, short whichItem)
{
	short	Xpos, Ypos;
	
	whichItem--;
	
	Xpos = whichItem - (whichItem/12)*12;
	Ypos = whichItem / 12;
	
	aRect->left 	= menuRect->left + Xpos*WIM;
	aRect->right 	= menuRect->left + (Xpos + 1)*WIM;
	
	aRect->top 		= menuRect->top + Ypos*HIM;
	aRect->bottom 	= menuRect->top + (Ypos + 1)*HIM;
}

pascal void MyMenuNoteDefProc(short msg, MenuRef whichMenu, Rect *menuRect, Point hitPt, short *itemID)
{
	short					itemCount, j, mouseItem, mSize, Xpos, Ypos;
	Str255					str;
	Rect					tempRect;
	GDHandle				aH;
	RGBColor				theColor;
	MenuTrackingDataPtr		mTracking;
	
	GrafPtr				savedPort;
	short				savedFont;
	short				savedSize;
	
	
	GetPort(&savedPort);
	
	savedFont = GetPortTextFont((CGrafPtr) savedPort);
	savedSize = GetPortTextSize((CGrafPtr) savedPort);
	
	
	itemCount = CountMenuItems(whichMenu);
	
	TextFont(0);	TextSize(0);
	
	switch (msg)
	{
		case mDrawMsg:
			theColor.red = 56797;
			theColor.green = 56797;
			theColor.blue = 56797;
			
			//	RGBForeColor(&theColor);
			PaintRect(menuRect);
			
			ForeColor(blackColor);
			//	RGBBackColor(&theColor);
			
			for (j = 1;  j <= itemCount;  j++)
			{
				GetMenuItemText(whichMenu, j, str);
				tempRect.bottom--;
				GetMDEFRect(&tempRect, menuRect, j);
				
				
				if (GetMenuID(whichMenu) == j) ForeColor(redColor);
				else ForeColor(blackColor);
				TETextBox(str+1, str[ 0], &tempRect, teJustCenter);
			}
			
			ForeColor(blackColor);
			break;
			
		case kMenuFindItemMsg:
			mTracking = (MenuTrackingDataPtr) itemID;
			
			if (!PtInRect(hitPt, menuRect)) mouseItem = 0;
			else
			{
				mouseItem = (hitPt.v - menuRect->top) / HIM;
				mouseItem *= 12;
				
				mouseItem += (hitPt.h - menuRect->left) / WIM;
				mouseItem++;
				
				if (mouseItem >= itemCount) mouseItem = itemCount;
			}
			
			if (mouseItem == 0)
			{		/* out of bounds or disabled */
				GetMDEFRect(&tempRect, menuRect, mTracking->itemSelected);
				InvertRect(&tempRect);
				mTracking->itemSelected = 0;						/* return "cancel" code */
			}
			else if (mouseItem != mTracking->itemSelected )
			{
				GetMDEFRect(&tempRect, menuRect, mTracking->itemSelected);
				InvertRect(&tempRect);
				
				GetMDEFRect(&tempRect, menuRect, mouseItem);
				InvertRect(&tempRect);
				mTracking->itemSelected = mouseItem; 				/* return new */
			}
			
			mTracking->itemUnderMouse = mTracking->itemSelected;
			mTracking->itemRect = tempRect;
			break;
			
		case mSizeMsg:
			mSize = 0;
			for (j=1;  j<=itemCount;  j++ )
			{
				GetMenuItemText(whichMenu, j, str);
				if (StringWidth(str) > mSize) mSize = StringWidth(str);
			}
			
			SetMenuWidth(whichMenu, mSize + 2);
			SetMenuHeight(whichMenu, itemCount*HIM);	/* assume 12 pts high */
			break;
			
		case mPopUpMsg:
			
			SetMenuID(whichMenu, *itemID);
			//(*whichMenu)->menuID = *itemID;
			
			Xpos = (*itemID - 1) - ((*itemID - 1)/12)*12;
			Ypos = (*itemID - 1) / 12;
			
			menuRect->top = hitPt.h - (Ypos * HIM);
			menuRect->left = hitPt.v - (Xpos * WIM);
			
			Ypos = (itemCount - 1) / 12;
			
			menuRect->right = menuRect->left + 12*WIM;
			menuRect->bottom = menuRect->top + (Ypos+1)*HIM;
			
			GetMouse(&hitPt);
			
			aH = GetDeviceList();
			do
			{
				if (aH != NULL)
				{
					if (PtInRect(hitPt, &(*(*aH)->gdPMap)->bounds))
					{
						Rect 		ar = (*(*aH)->gdPMap)->bounds;
						
						if (menuRect->bottom > ar.bottom) 	OffsetRect(menuRect, 0, ar.bottom - menuRect->bottom);
						if (menuRect->top < ar.top)			OffsetRect(menuRect, 0, ar.top - menuRect->top);
						if (menuRect->right > ar.right) 	OffsetRect(menuRect, ar.right - menuRect->right, 0);
						if (menuRect->left < ar.left)		OffsetRect(menuRect, ar.left - menuRect->left, 0);
					}
				}
				
				aH = GetNextDevice(aH);
			}
			while (aH != NULL);
			break;
			
	}
	
	TextFont(savedFont);	TextSize(savedSize);
}

#define HIM		14
#define WIM2		15

void GetMDEFRect2(Rect *aRect, Rect *menuRect, short row, Boolean HiLo)
{
	if (!HiLo) aRect->left 	= menuRect->left;
	else aRect->left 	= menuRect->left + WIM2 + 1;
	
	aRect->right 	= aRect->left + WIM2;
	
	aRect->top 		= menuRect->top + row*HIM;
	aRect->bottom 	= aRect->top + HIM;
}

pascal void MyMenu0xFFDefProc(short msg, MenuRef whichMenu, Rect *menuRect, Point hitPt, short *itemID)
{
	short				j, i, o, mouseItem, mSize, Ypos;
	Str255				str, bstr;
	Rect				tempRect;
	Boolean				HiLo;
	RGBColor			theColor;
	MenuTrackingDataPtr	mTracking;
	GrafPtr				savedPort;
	short				savedFont;
	short				savedSize;
	
	
	GetPort(&savedPort);
	
	savedFont = GetPortTextFont((CGrafPtr) savedPort);
	savedSize = GetPortTextSize((CGrafPtr) savedPort);
	
	TextFont(0);	TextSize(0);
	
	switch (msg)
	{
		case kMenuInitMsg:
			*itemID = noErr;
			break;
			
		case mDrawMsg:
			theColor.red = 56797;
			theColor.green = 56797;
			theColor.blue = 56797;
			
			ForeColor(blackColor);
			//RGBBackColor(&theColor);
			
			for (i = 0;  i < 2;  i++)
			{
				for (j = 0;  j < 16;  j++)
				{
					GetMenuItemText(whichMenu, j + 1, str);
					
					GetMDEFRect2(&tempRect, menuRect, j, i);
					
					if (i == 1) o = (GetMenuID(whichMenu)-1) & 0x0F;
					else
					{
						o = (GetMenuID(whichMenu)-1) & 0xF0;
						o >>= 4;
					}
					
					if (o == j) ForeColor(redColor);
					else ForeColor(blackColor);
					
					TETextBox(str+1, str[ 0], &tempRect, teJustCenter);
				}
			}
			
			ForeColor(blackColor);
			
			MoveTo(menuRect->left + (menuRect->right - menuRect->left) / 2, menuRect->top);
			LineTo(menuRect->left + (menuRect->right - menuRect->left) / 2, menuRect->bottom-HIM);
			
			MoveTo(menuRect->left, menuRect->bottom-HIM*3);
			LineTo(menuRect->right, menuRect->bottom-HIM*3);
			break;
			
		case kMenuFindItemMsg:
			mTracking = (MenuTrackingDataPtr) itemID;
			
			tempRect = *menuRect;
			tempRect.bottom -= HIM*3;
			if (!PtInRect(hitPt, &tempRect))
			{
				mouseItem = 0;
				HiLo = false;
				
				if(Button()) goto TEST;
			}
			else
			{
				if (GetMenuID(whichMenu) != 2000)
				{
					mTracking->itemSelected = GetMenuID(whichMenu);
					//	*itemID = GetMenuID(whichMenu);
					SetMenuID(whichMenu, 2000);
					if (mTracking->itemSelected > 0)
					{
						GetMDEFRect2(&tempRect, menuRect, (mTracking->itemSelected)-1 & 0x000F, true);			InvertRect(&tempRect);
						GetMDEFRect2(&tempRect, menuRect, (((mTracking->itemSelected)-1) & 0x00F0)>>4, false);	InvertRect(&tempRect);
					}
					
					// **** Draw bottom ****
					
					GetMenuItemText(whichMenu, 1 + ((mTracking->itemSelected)-1 & 0x000F), bstr);
					GetMenuItemText(whichMenu, 1 + ((((mTracking->itemSelected)-1) & 0x00F0)>>4), str);
					str[ 0]++;
					str[ 2] = bstr[ 1];
					
					SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM*3+1, menuRect->right, menuRect->bottom);
					TETextBox(str+1, str[ 0], &tempRect, teCenter);
					
					
					{
						short tempValue = mTracking->itemSelected-1;
						NumToString(tempValue, str);
						SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM*2 + 1, menuRect->right, menuRect->bottom);
						TETextBox(str+1, str[ 0], &tempRect, teCenter);
					}
					
					{
						short tempValue = (char) (mTracking->itemSelected-1);
						NumToString(tempValue, str);
						SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM + 1, menuRect->right, menuRect->bottom);
						TETextBox(str+1, str[ 0], &tempRect, teCenter);
					}
				}
				
				HiLo = (hitPt.h - menuRect->left) / WIM2;
				mouseItem = (hitPt.v - menuRect->top) / HIM;
				
				if (!HiLo)
				{
					mouseItem <<= 4;
					
					if (mTracking->itemSelected != 0) mouseItem += ((mTracking->itemSelected)-1) & 0x000F;
				}
				else
				{
					if (mTracking->itemSelected != 0) mouseItem += ((mTracking->itemSelected)-1) & 0x00F0;
				}
				mouseItem++;
				
			TEST:
				
				if (mouseItem == 0)
				{
					if (mTracking->itemSelected > 0)
					{
						GetMDEFRect2(&tempRect, menuRect, (mTracking->itemSelected)-1 & 0x000F, true);			InvertRect(&tempRect);
						GetMDEFRect2(&tempRect, menuRect, (((mTracking->itemSelected)-1) & 0x00F0)>>4, false);	InvertRect(&tempRect);
					}
					mTracking->itemSelected = 0;
					
					SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM*3+1, menuRect->right, menuRect->bottom);
					EraseRect(&tempRect);
				}
				else if (mouseItem != mTracking->itemSelected)
				{
					if (mTracking->itemSelected > 0)
					{
						GetMDEFRect2(&tempRect, menuRect, (mTracking->itemSelected)-1 & 0x000F, true);			InvertRect(&tempRect);
						GetMDEFRect2(&tempRect, menuRect, (((mTracking->itemSelected)-1) & 0x00F0)>>4, false);	InvertRect(&tempRect);
					}
					
					GetMDEFRect2(&tempRect, menuRect, (mouseItem-1) & 0x000F, true);		InvertRect(&tempRect);
					GetMDEFRect2(&tempRect, menuRect, ((mouseItem-1) & 0x00F0)>>4, false);	InvertRect(&tempRect);
					
					mTracking->itemSelected = mouseItem;
					
					// **** Draw bottom ****
					
					GetMenuItemText(whichMenu, 1 + ((mTracking->itemSelected)-1 & 0x000F), bstr);
					GetMenuItemText(whichMenu, 1 + ((((mTracking->itemSelected)-1) & 0x00F0)>>4), str);
					str[ 0]++;
					str[ 2] = bstr[ 1];
					
					SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM*3 + 1, menuRect->right, menuRect->bottom);
					TETextBox(str+1, str[ 0], &tempRect, teCenter);
					
					{
						short tempValue = mTracking->itemSelected-1;
						NumToString(tempValue, str);
						SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM*2 + 1, menuRect->right, menuRect->bottom);
						TETextBox(str+1, str[ 0], &tempRect, teCenter);
					}
					
					{
						short tempValue = (char) (mTracking->itemSelected-1);
						NumToString(tempValue, str);
						SetRect(&tempRect, menuRect->left, menuRect->bottom-HIM + 1, menuRect->right, menuRect->bottom);
						TETextBox(str+1, str[ 0], &tempRect, teCenter);
					}
				}
			}
			
			mTracking->itemUnderMouse = mTracking->itemSelected;
			mTracking->itemRect = tempRect;
			
			break;
			
		case mSizeMsg:
			mSize = 0;
			for (j = 0;  j< 16;  j++ )
			{
				GetMenuItemText(whichMenu, j, str);
				if (StringWidth(str) > mSize) mSize = StringWidth(str);
			}
			
			SetMenuWidth(whichMenu, 2 * mSize + 2);
			SetMenuHeight(whichMenu, 19 * HIM);
			
			//	(*whichMenu)->menuWidth  = 2 * mSize + 2;
			//	(*whichMenu)->menuHeight = 17 * HIM;
			break;
			
			
		case mPopUpMsg:
		{
			GDHandle	aH;
			
			SetMenuID(whichMenu, *itemID);
			//	(*whichMenu)->menuID = *itemID;
			
			GetMouse(&hitPt);
			
			Ypos = (((*itemID)-1) & 0x00F0)>>4;
			
			menuRect->top = hitPt.v - (Ypos * HIM + HIM/2);
			menuRect->left = hitPt.h - WIM2/2;
			
			menuRect->right = menuRect->left + 2*WIM2 + 1;
			menuRect->bottom = menuRect->top + 19*HIM;
			
			aH = GetDeviceList();
			do
			{
				if (aH != NULL)
				{
					if (PtInRect(hitPt, &(*(*aH)->gdPMap)->bounds))
					{
						Rect 		ar = (*(*aH)->gdPMap)->bounds;
						
						if (menuRect->bottom > ar.bottom) 	OffsetRect(menuRect, 0, ar.bottom - menuRect->bottom);
						if (menuRect->top < ar.top)			OffsetRect(menuRect, 0, ar.top - menuRect->top);
						if (menuRect->right > ar.right) 	OffsetRect(menuRect, ar.right - menuRect->right, 0);
						if (menuRect->left < ar.left)		OffsetRect(menuRect, ar.left - menuRect->left, 0);
					}
				}
				
				aH = GetNextDevice(aH);
			}
			while (aH != NULL);
		}
			break;
	}
	
	TextFont(savedFont);	TextSize(savedSize);
}

MADMusic* CreateFreeMADK(void);
void WriteMADKFile(FSSpec *newFile, MADMusic	*music);
int MIDImain(int argc, char *argv[]);

Boolean RunningOnClassic(void)
{
    UInt32 response;
    
    return (Gestalt(gestaltMacOSCompatibilityBoxAttr,  (SInt32 *) &response) == noErr) \
	&& ((response &&  (1 << gestaltMacOSCompatibilityBoxPresent)) != 0);
}

OSErr GetApplicationPackageFSSpec(FSSpecPtr theFSSpecPtr)
{
	OSErr err;
	Str255 applName;
	ProcessSerialNumber psn;
	ProcessInfoRec info;
	
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName = applName;
	info.processAppSpec = theFSSpecPtr;
	err = GetCurrentProcess(&psn);
	if (err != noErr) return err;
	err = GetProcessInformation(&psn, &info);
	
	{
		CInfoPBRec	block;
		Str255		directoryName;
		
		block.dirInfo.ioDrParID = theFSSpecPtr->parID;
		block.dirInfo.ioNamePtr = directoryName;
		block.dirInfo.ioVRefNum = theFSSpecPtr->vRefNum;
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfoSync(&block);
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfoSync(&block);
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfoSync(&block);
		
		theFSSpecPtr->parID = block.dirInfo.ioDrParID;
		theFSSpecPtr->vRefNum = block.dirInfo.ioVRefNum;
	}
	return err;
}

OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr)
{
	OSErr err = fnfErr;
	CFBundleRef myAppsBundle = CFBundleGetMainBundle();
	if (myAppsBundle == NULL) return err;
	CFURLRef myBundleURL = CFBundleCopyBundleURL(myAppsBundle);
	if (myBundleURL == NULL) return err;
	FSRef myBundleRef;
	Boolean ok = CFURLGetFSRef(myBundleURL, &myBundleRef);
	CFRelease(myBundleURL);
	if (!ok) return err;
	return FSGetCatalogInfo(&myBundleRef, kFSCatInfoNone, NULL, NULL, theFSSpecPtr, NULL);
}

OSErr GetApplicationResourceFSSpecFromBundle(FSSpecPtr theFSSpecPtr)
{
	OSErr err = fnfErr;
	CFBundleRef myAppsBundle = CFBundleGetMainBundle();
	if (myAppsBundle == NULL) return err;
	CFURLRef myBundleURL = CFBundleCopyResourcesDirectoryURL(myAppsBundle);
	if (myBundleURL == NULL) return err;
	FSRef myBundleRef;
	Boolean ok = CFURLGetFSRef(myBundleURL, &myBundleRef);
	CFRelease(myBundleURL);
	if (!ok) return err;
	return FSGetCatalogInfo(&myBundleRef, kFSCatInfoNone, NULL, NULL, theFSSpecPtr, NULL);
}

OSErr GetExecutableParentFSSpecFromBundle(FSSpecPtr theFSSpecPtr)
{
	OSErr err = fnfErr;
	CFBundleRef myAppsBundle = CFBundleGetMainBundle();
	if (myAppsBundle == NULL) return err;
	CFURLRef myBundleURL = CFBundleCopyExecutableURL(myAppsBundle);
	if (myBundleURL == NULL) return err;
	FSRef myBundleRef;
	Boolean ok = CFURLGetFSRef(myBundleURL, &myBundleRef);
	CFRelease(myBundleURL);
	if (!ok) return err;
	return FSGetCatalogInfo(&myBundleRef, kFSCatInfoNone, NULL, NULL, theFSSpecPtr, NULL);
}

int main(int argc, char* argv[])
{
	long					OldVolL;
	OSErr					iErr;
	unsigned long			secs, i;
	DateTimeRec				dtrp;
	MenuDefSpec				defSpec;
	MenuDefSpec				defSpec2;
	
	WindowStateNOW = -1;
	ShowIt = true;
	UseAEErreur = false;
	
	ToolBoxInit();
	
	HGetVol(NULL, &mainVRefNum, &mainParID);
	
#if 0
	MIDImain(0, NULL);
	
	//////////////////////////
	{
		MADMusic	*tempMusic;
		FSSpec		file;
		
		tempMusic = CreateFreeMADK();
		
		
		
		FindFolder(kOnSystemDisk, kSystemDesktopFolderType, kCreateFolder, &file.vRefNum, &file.parID);
		pStrcpy(file.name, "\pMADK");
		
		WriteMADKFile(&file, tempMusic);
		
		ExitToShell();
		
	}
#endif
	//////////////////////////
	
	
	curMusic = NULL;
	SwitchCurMusic = NULL;
	AlternateBuffer = false;
	
	{
		Handle	TempHandle = GetResource('MADK', 128);
		if (TempHandle)
		{
			DetachResource(TempHandle);
			HLock(TempHandle);
			iErr = MADLoadMusicPtr(&curMusic, *TempHandle);
			iErr = MADLoadMusicPtr(&SwitchCurMusic, *TempHandle);
			pStrcpy(SwitchCurMusic->musicFileName, "\pUntitled");
			
			HUnlock(TempHandle);
			DisposeHandle(TempHandle);
		}
		else MyDebugStr(__LINE__, __FILE__, "Fatal MEMORY ERROR 35: NEED MORE MEMORY !");
	}
	
	////////
	
	defSpec.defType = kMenuDefProcPtr;
	defSpec.u.defProc = NewMenuDefUPP(MyMenu0xFFDefProc );
	RegisterMenuDefinition(200, &defSpec);
	
	defSpec2.defType = kMenuDefProcPtr;
	defSpec2.u.defProc = NewMenuDefUPP(MyMenuNoteDefProc );
	RegisterMenuDefinition(1972, &defSpec2);
	
	//	thePrefs.ThreadUse = true;
	thePrefs.ThreadUse = false;
	
	//	EventLoop();
	
	
	GetCurrentProcess(&playerPROPSN);
	
	InitBookMarks();
	InitFourier();
	
	/////////////////////// 
	
	InSound = (SndListHandle)GetResource('snd ', 128);
	if (InSound == NULL)
		abort();
	DetachResource((Handle)InSound);
	
	OutSound = (SndListHandle)GetResource('snd ', 129);
	if (OutSound == NULL)
		abort();
	DetachResource((Handle)OutSound);
#ifdef DEBUG_MODE
	DebuggingMode = true;
#else
	DebuggingMode = false;
#endif
	
	gUseNavigation = true;
	
	MacOSXSystem = true;
	
	AppearanceManager = true;
	gUseControlSize = true;
	gScrollBarID = kControlScrollBarLiveProc;
	newQuicktime = true;
	
	EnterMovies();	
	
	NewSoundManager31 = true;
	NewSoundManager = true;
	
	if (DebuggingMode)
	{
		GetDateTime(&secs);
		SecondsToDate(secs, &dtrp);
		
	}
	
	MainResFile = CurResFile();
	
	CreateTempFile();
	
	/************/
	
	MyDlgFilterDesc = NewModalFilterUPP(MyDlgFilter);
	mylClickLoopDesc = NewListClickLoopUPP(mylClickLoop);
	MyDlgFilterNavDesc = NewNavEventUPP(MyDlgFilterNav);
	
	theColor.red = 56797;
	theColor.green = 56797;
	theColor.blue = 56797;
	
	PianoPix = NULL;	//EditorPix = 
	QuicktimeDlog = EditorDlog = theProgressDia = InstruViewDlog = MODListDlog = ToolsDlog = PatListDlog = PianoDlog = MemoryDlog = OscilloDlog = PartiDlog = (DialogPtr) NULL;
	TrackViewDlog = CubeDlog = HelpDlog = DigitalDlog = AHelpDlog = ClassicDlog = EQDlog = VisualDlog = InstruListDlog = AdapDlog = MozartDlog = SpectrumDlog = FindDlog = WaveDlog = StaffDlog = (DialogPtr) NULL;
	MicroPhone = false;
	SpectrumMicrophone = false;
	OscilloMicrophone = false;
	NOWScrollMode = false;
	PianoRecording = false;
	LastCanal = 0;
	showWhat = allMusics;
	/************/
	
	{
		CursHandle myCursH;
		
		myCursH = GetCursor(357);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			watchCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(iBeamCursor);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			beamCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(300);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			pencilCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(137);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			pencilCrsrStereo = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(135);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			HelpCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(134);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			DelCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(130);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			NoteCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(133);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			PlayCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(132);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			CHandCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(131);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			HandCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(128);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			ZoomInCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(129);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			ZoomOutCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
		
		myCursH = GetCursor(138);
		if (myCursH == NULL)
			MyDebugStr(__LINE__, __FILE__, "");
		else {
			DetachResource((Handle) myCursH);
			HLock((Handle) myCursH);
			ContextCrsr = **myCursH;
			HUnlock((Handle) myCursH);
			DisposeHandle((Handle) myCursH);
		}
	}
	/************/
	
	if (TestProcessorChip() == false) {
		Erreur(101, 101);
		abort();
	}
	
	
	StartDialog();
	
	/************/
	/************/
	
	{
		FSSpec	appSpec;
		
		GetApplicationPackageFSSpecFromBundle(&appSpec);
		
		pStrcpy(appSpec.name, "\pPlugs");
		
		MADInitLibrary(&appSpec, false, &gMADLib);
	}
	
	/************/
	/************/
	
	InitMIDIHarware();
	InitRollCrsr();
	InitFindReplace();
	InitPlayWhenClicked();
	InitStringEditor();
	//InitPrintRegistration();
	InitPrefs();
	InitQuicktimeInstruments();
	InitSoundInput();
	InitFKeyMenu();
	InitQTWindow();
	InitFFTSampleFilter();
	
	SetFKeyMode(false);
	
	/**********************************/
	/* Check if HelpFile is avalaible */
	/**********************************/
	
	
	HelpAvalaible = true;
	
	/**********************************/
	
	DragManagerInstalled();
	
	oldWindow = NULL;
	
	//SysCheck();
	
	InitPlug();
	InitPPDGPlug();
	InitPPINPlug();
	
	//InitCubePlug();
	
	DoPreferences();
	
	GetDefaultOutputVolume(&OldVolL);
	SKVolume(thePrefs.volumeLevel);
	
	MusiqueDriverInit();
	
	HSetVol(NULL, mainVRefNum, mainParID);
	
	MenuBarInit();
	
	InitVisual();
	
	InitVSTPlug();
	
	MusicPlayActive = false;
	if (curMusic) curMusic->hasChanged = false;
	
	InitUndo();
	
	InstallAE();
	
	InitPrinting();
	
	NoSelectWindow2 = true;
	StartTime = TickCount();
	
	InitOscillo();
	
	
	CreateToolsDlog();
	
	InitSampleWindow();
	
	CreateMODListWindow();
	
	InitAdapWindow();
	
	MADDisposeMusic(&curMusic, MADDriver);
	curMusic = NULL;
	
	ImportFile("\pUntitled", -55, 0, 'Rsrc');
	
	MADStartDriver(MADDriver);
	
	InitEQWindow();
	
	if (thePrefs.RememberMusicList)
	{
		FSSpec	mList;
		
		FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &mList.vRefNum, &mList.parID);
		pStrcpy(mList.name, MLSAVENAME);
		
		AESendOpenFile(&mList);
		SetcurMusicListFile(false);
	}
	
	EndDialog();
	
	ShowWindowPref(-1);
	
	NoSelectWindow2 = false;
	
	ShowWindow(GetDialogWindow(ToolsDlog));
	SelectWindow(GetDialogWindow(ToolsDlog));
	SetWindowClass(GetDialogWindow(ToolsDlog), kFloatingWindowClass);
	SelectWindow2(NextWindowVisible(GetDialogWindow(ToolsDlog)));
	UpdateALLWindow();
	
	
	/***********/
	
OnRefaitEvent:
	
	End = false;
	
	//TODO: Threading?
	thePrefs.ThreadUse = false;
	//thePrefs.ThreadUse = true;
	
	
	DeleteTempFile();
	
	if (thePrefs.ActiveHelp) {
		CreateAHelpWindow();
	}
	
	UpdateALLWindow();
	
	EventLoop();
	
	/***********/
	
	/*****/
	if (curMusic->hasChanged)  {
		if (GereChanged() != noErr)
			goto OnRefaitEvent;
	}
	
	tempCurMusic = curMusic;
	curMusic = SwitchCurMusic;
	SwitchCurMusic = tempCurMusic;
	
	if (curMusic->hasChanged)  {
		if (GereChanged() != noErr)
			goto OnRefaitEvent;
	}
	/*****/
	
	if (GereMusicListChanged() != noErr)
		goto OnRefaitEvent;
	
	if (thePrefs.RememberMusicList) {
		FSSpec	prefMusicList;
		
		FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &prefMusicList.vRefNum, &prefMusicList.parID);
		FSMakeFSSpec(prefMusicList.vRefNum, prefMusicList.parID, MLSAVENAME, &prefMusicList);
		
		SaveMyMODListSTCf(prefMusicList);
	}
	
	SelectWindow(GetDialogWindow(ToolsDlog));
	CheckShowWindow();
	
	thePrefs.numChn = MADDriver->DriverSettings.numChn;
	thePrefs.softVolumeLevel = MADDriver->VolGlobal;
	
	thePrefs.previousSpec.generalPitch	= 252;
	thePrefs.previousSpec.generalSpeed	= 252;
	thePrefs.previousSpec.generalPan	= MADDriver->globPan;
	thePrefs.previousSpec.EPitch		= MADDriver->FreqExt;
	thePrefs.previousSpec.ESpeed		= MADDriver->VExt;
	thePrefs.previousSpec.generalVol	= MADDriver->VolGlobal;
	
	for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanPan[ i] = curMusic->header->chanPan[ i];
	for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanVol[ i] = curMusic->header->chanVol[ i];
	
	
	WriteCFPreferences();
	
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	
	if (MicroPhone) MicroOff();
	//if (NOWScrollMode) HandleViewsChoice(mScrollT);
	FlushEvents(everyEvent,0);
	//DeSwitch();
	
	CloseMIDIHarware();
	CloseBookMarks();
	
	//if (HelpAvalaible) CloseResFile(iHelpPP);
	
	//SetInternetMapping();
	
	CloseFourier();
	
	
	CloseVSTPlug();
	
	SetDefaultOutputVolume(OldVolL);
	
End:;
	
	ExitMovies();
	MADDisposeLibrary(gMADLib);
	
	return EXIT_SUCCESS;
}

static	Boolean 		PressInDialog;
static	long			checkMemory;
short					TouchMem[11];
short					TrackMem[11];
short					TouchIn;

short NEWdoDlgEvt(EventRecord *evp, WindowPtr whichWindow)
{
	DialogPtr		whichDialog;
	short			whichItem = -1;
	
	SetPortWindowPort(whichWindow);
	
	//#if MACOS9VERSION
	//	if (DialogSelect(evp, &whichDialog, &whichItem) == false ) return(0);
	DialogSelect(evp, &whichDialog, &whichItem);
	/*#else
	 whichDialog = GetDialogFromWindow(whichWindow);
	 pt = evp->where;
	 GlobalToLocal(&pt);
	 if (whichDialog)
	 {
	 //whichItem = FindDialogItem(whichDialog, pt);
	 //whichItem++;
	 for (i = 0 ; i < CountDITL(whichDialog); i++)
	 {
	 GetDialogItem(whichDialog, i, &itemType, &item, &box);
	 if (PtInRect(pt, &box))
	 {
	 whichItem = i;
	 }
	 }
	 }
	 #endif
	 
	 #if MACOS9VERSION
	 LMSetGhostWindow(NULL);
	 #endif*/
	
	ProcessDoItemPress(GetWRefCon(GetDialogWindow(whichDialog)), whichItem, whichDialog);
	
	return 0;
}

void ProcessDoItemPress(long ref, short whichItem, DialogPtr whichDialog)
{
	switch (ref) {
		case RefVST:
			VSTEditorDoItemPress(whichItem, whichDialog);
			break;
			
		case RefCube:
			DoItemPressCube(whichItem, whichDialog);
			break;
			
		case RefPlayer:
			DoItemPressOscillo(whichItem, whichDialog);
			break;
			
		case RefSpectrum:
			DoItemPressSpectrum(whichItem, whichDialog);
			break;
			
		case RefDigiView:
			DoItemPressDigiList(whichItem, whichDialog);
			break;
			
		case RefMozart:
			DoItemPressMozart(whichItem, whichDialog);
			break;
			
		case RefInstruList:
			DoItemPressInstruList(whichItem, whichDialog);
			break;
			
		case RefPartition:
			DoItemPressPartition(whichItem, whichDialog);
			break;
			
		case RefStaff:
			DoItemPressStaff(whichItem, whichDialog);
			break;
			
		case RefWave:
			DoItemPressWave(whichItem, whichDialog);
			break;
			
		case RefAdaptators:
			DoItemPressAdap(whichItem, whichDialog);
			break;
			
		case RefMODList:
			DoItemPressMODList(whichItem, whichDialog);
			break;
			
		case RefTools:
			DoItemPressTools(whichItem, whichDialog);
			break;
			
		case RefSample:
			DoItemPressSample(whichItem, whichDialog);
			break;
			
		case RefQuicktime:
			DoItemPressQT(whichItem, whichDialog);
			break;
			
		case RefEqualizer:
			DoItemPressEQ(whichItem, whichDialog);
			break;
			
		case RefVisual:
			DoItemPressVisual(whichItem, whichDialog);
			break;
			
		case RefPatList:
			DoItemPressPatList(whichItem, whichDialog);
			break;
			
		case RefParti:
			DoItemPressParti(whichItem, whichDialog);
			break;
			
		case RefHelp:
			DoItemPressHelpOnline(whichItem, whichDialog);
			break;
			
		case RefPiano:
			DoItemPressPiano(whichItem, whichDialog);
			break;
			
		case RefClassic:
			DoItemPressClassic(whichItem, whichDialog);
			break;
	}
}

void DoGlobalNull()
{
	GrafPtr		savePort;
	
	theDepth = (*(*GetGDevice())->gdPMap)->pixelSize;
	
	GetPort(&savePort);
	
	GetKeys(km);
	
	//if (thePrefs.MidiKeyBoard) DoMIDIHarwareRecord();
	
	if (DebuggingMode)
	{
#if 0
		unsigned long			secs;
		NumVersion				nVers;
		DateTimeRec				dtrp;
		
		if (MemError() != noErr)
		{
			pStrcpy(str1, "\pMemError ID:");
			NumToString(MemError(), str2);
			pStrcat(str1, str2);
			
			MyDebugStr(str1);
		}
		
		if (ResError() != noErr)
		{
			pStrcpy(str1, "\pResError ID:");
			NumToString(ResError(), str2);
			pStrcat(str1, str2);
			
			MyDebugStr(str1);
		}	
		
		for (i = 0; i < MAXINSTRU; i++)
		{
			for (x = 0; x < curMusic->fid[ i].numSamples; x++)
			{
				if (curMusic->sample[ curMusic->fid[ i].firstSample + x] == NULL) MyDebugStr(__LINE__, __FILE__, "Sound Size not CORRECT !");
				
				
				if (curMusic->sample[ curMusic->fid[ i].firstSample + x]->data != NULL)
				{
					if (curMusic->sample[ curMusic->fid[ i].firstSample + x]->size != GetPtrSize(curMusic->sample[ curMusic->fid[ i].firstSample + x]->data))
					{
						MyDebugStr(__LINE__, __FILE__, "Sound Size not CORRECT !");
					}
				}
			}
		}
#endif
		
	}
	
	if (MADDriver->musicEnd == true) {
		DoLoadOtherMusic(true);
	}
	
	VSTEditorDoNull();
	DoVisualNull();
	DoNullPattern();
	DoNullSpectrum();
	DoNull();
	DoNullInstruList();
	DoNullInstrument();
	DoNullEditor();
	DoNullClassic();
	DoNullMozart();
	DoNullTools();
	DoNullDigiWindow();
	//DoNullCubeWindow();
	DoNullWave();
	DoNullMODList();
	DoNullQT();
	DoNullStaff();
	DoAIFFExporting();
	DoNullParti();
	DoNullAdap();
	DoNullInstruView();
	//DoNullTrackView();
	MyNullHook();
	
	if (checkMemory < TickCount()) {
		checkMemory = TickCount() + 60;
		DoNullMemWindow();
		//if (FreeMem() < 50000) Erreur(9, 0);
	}
	
	if (PianoDlog != NULL && MusicPlayActive == true) DoNullPiano();
	
	if (IsCodeOK()) //??ATTENTION, CA TUE LE QUITEVENT SOUS MACOS X
	{
		if (TickCount() - StartTime > 72000)		// 20 Minutes
		{
			if (ShowIt)
			{
				Erreur(32, -4);
				ShowIt = false;
			}
			End = true;
		}
	}
	
	/*** Active Help ***/
	
	if (thePrefs.ActiveHelp) DoAHelpInfo();
	
	/*******************/
	
	SetPort(savePort);
}

void GlobalDoKey(WindowPtr theWind, char theChar)
{
	//if (theChar == 8) return;
	
	switch (GetWRefCon(theWind)) {
		case RefInstruList:
			DoKeyPressInstruList(theChar, -1);
			break;
			
		case RefClassic:
			DoKeyPressClassic(theChar);
			break;
			
#if 0
		case RefCmd:
			DoKeyPressCmdDlog(theChar);
			break;
#endif
			
		case RefPatList:
			DoKeyPressPatList(theChar);
			break;
			
		case RefParti:
			DoKeyPressParti(theChar);
			break;
			
		case RefMODList:
			DoKeyPressMODList(theChar);
			break;
			
		case RefPartition:
			DoKeyPressEditor(theChar);
			break;
			
		case RefStaff:
			DoKeyPressStaff(theChar);
			break;
			
		case RefWave:
			DoKeyPressWave(theChar);
			break;
			
		case RefSample:
			DoKeyPressSample(GetDialogFromWindow(theWind), theChar);
			break;
			
		case RefQuicktime:
			
			break;
			
		case RefMozart:
			DoKeyPressMozart(theChar);
			break;
	}
}

void ActivateProcedure(Boolean ModalCall)
{
	if (AHelpDlog != NULL)
	{
		ShowWindow(GetDialogWindow(AHelpDlog));
	}
	//ShowWindow(GetDialogWindow(ToolsDlog));
	//if (!ModalCall) SelectWindow(GetDialogWindow(ToolsDlog));
	
	if (!ModalCall) {
		if (oldWindow != NULL) {
			SelectWindow2(oldWindow);
			EraseGrowIcon(GetDialogFromWindow(oldWindow));
		}
	} else {
		if (oldWindow != NULL) {
			HiliteWindow(oldWindow, true);
			EraseGrowIcon(GetDialogFromWindow(oldWindow));
		}
	}
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
}

void DoOSEvent(EventRecord *event, Boolean ModalCall)
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
				if (AHelpDlog != NULL)
					HideWindow(GetDialogWindow(AHelpDlog));
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

#if 0
void DoActivateEvent(EventRecord *event, Boolean activate, Boolean ModalCall)
{
	if (IsAppWindow((WindowPtr) event->message))
	{
		if (activate)
		{
			ActivateProcedure(ModalCall);
			HiliteMenu(0);
		}
		else
		{
			if (AHelpDlog != NULL) HideWindow(GetDialogWindow(AHelpDlog));
			HideWindow(GetDialogWindow(ToolsDlog));
			if (oldWindow != NULL)
			{
				HiliteWindow(oldWindow, false);
				EraseGrowIcon(GetDialogFromWindow(oldWindow));
			}
			SetCursor(GetQDGlobalsArrow(&qdarrow));
		}
	}
}

void EventLoop3(void)
{
	WaitNextEvent(everyEvent, &theEvent, 1, NULL);
	
	//	theDepth = (*(*GetGDevice())->gdPMap)->pixelSize;
	
	//	if (theOldDepth != theDepth) ReComputeVA();
	//	theDepth = theOldDepth;
	
	switch(theEvent.what)
	{
		case updateEvt:
			DoUpdateEvent(&theEvent);
			break;
			
		case osEvt:
			DoOSEvent(&theEvent, false);
			break;
	}
}
#endif

short ConvertCharToNote(char theChar)
{
	short i;
	
	if ((short) theChar >= 0 && (short) theChar < 300) {
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
	
    return(GetWindowKind(window ) == dialogKind);
}

void ActivateAllWindows()
{
	WindowPtr theWindow;
	
	theWindow = GetNextWindow(FrontWindow());
	
	if (theWindow == NULL)
		return;
	
	do {
		theEvent.message = (unsigned long) theWindow;
		theEvent.modifiers = activeFlag;
		theEvent.what = activateEvt;
		
		theWindow = (WindowPtr)theEvent.message;
		
		if (IsDialogWindow(theWindow)) {
			DialogRef		dialog;
			SInt16			itemHit;
			GrafPtr			savePort;
			
			dialog = NULL;
			DialogSelect(&theEvent, &dialog, &itemHit);
			
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
		
		theWindow = GetNextWindow(theWindow);
	}
	while (theWindow != NULL);
}

void EventLoop2(void)
{
	short			i, x;
	GrafPtr			savePort;
	short			theChar;
	Rect			tempRect;
	Str255			str1;
	Boolean			gotEvent;
	
	gotEvent = WaitNextEvent(everyEvent, &theEvent, 1, gCursorRgn);
	
	//	PressInDialog = false;
	switch (theEvent.what) {
			
		case activateEvt:
			//	DoActivateEvent(&theEvent, (theEvent.modifiers & activeFlag) != 0, false);
		{
			WindowPtr theWindow = (WindowPtr)theEvent.message;
			
			//	ShowWindow(theWindow);
			
			/*	#if MACOS9VERSION
			 if (FrontWindow() == ToolsDlog)
			 {
			 if (theWindow != ToolsDlog) break;
			 }
			 #endif*/
			
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
				
				if (theChar >= 0 && theChar < 300) {
					i = thePrefs.PianoKey[theChar];
					
					if (i != 0xFF)
						i += thePrefs.pianoOffset*12;
					
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
								
								switch(thePrefs.KeyUpMode) {
									case eStop:
										MADDriver->chan[TrackMem[x]].loopBeg = 0;
										MADDriver->chan[TrackMem[x]].loopSize = 0;
										MADDriver->chan[TrackMem[x]].maxPtr = MADDriver->chan[TrackMem[x]].curPtr;
										
										if (PianoRecording)
											NPianoRecordProcess(0xFF, 0xFF, 0x10, TrackMem[ x]);
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
			if (VisualDlog != NULL) {
				if (oldWindow == GetDialogWindow(VisualDlog)) {
					DoKeyVisual(VisualDlog);
				}
			}
			
		{
			for (i = 0; i < 15; i++) {
				if (FKEYCo[ i] == (theEvent.message & keyCodeMask) >> 8L) {
					PressFKey(i);
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
				
				if (AcceptKeysTools()) break;
				
				if (EditorDlog != NULL) {
					if (!GetIns(&ins, NULL) && oldWindow != GetDialogWindow(EditorDlog))
						break;
				}
				
				///
				
				if (thePrefs.MacKeyBoard) {
					i = ConvertCharToNote(theChar);
					if (i != -1) {
						short	track = GetWhichTrackPlay();
						short	eff = 0, arg = 0, volCmd = 0xFF;
						
						if (PianoRecording)
							NPianoRecordProcess(i, -1, 0xFF, track);
						else if (EditorDlog != NULL) {
							if (oldWindow == GetDialogWindow(EditorDlog))
								DigitalEditorProcess(i, &eff, &arg, &volCmd);
						}
						
						SelectTouche(i, -1);
						SelectToucheMozart(i, 0);
						
						TouchIn++;
						if (TouchIn < 0 || TouchIn >= 10)
							TouchIn = 0;
						TouchMem[TouchIn] = i;
						TrackMem[TouchIn] = track;
						if (TrackMem[TouchIn] < 0 || TrackMem[TouchIn] >= MADDriver->DriverSettings.numChn) TrackMem[TouchIn] = 0;
						
						if (GetIns(&ins, NULL))
							DoPlayInstruInt(i, ins, eff, arg, volCmd, &MADDriver->chan[ track], 0, 0);
					}
				}
			} else
				DoNullInstrument();
		}
			break;
	}
}

void EventLoop(void)
{
	short		i;
	
	checkMemory = 0;
	
	for(i=0; i<10; i++) TouchMem[i] = -1;
	for(i=0; i<10; i++) TrackMem[i] = 0;
	
	SetEventMask(everyEvent);
	//	gCursorRgn = NewRgn();
	//	SetEmptyRgn(gCursorRgn);
	
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
	
    if (GetEventKind(event) == kEventWindowDrawContent)
    {
    	theEvent.message = (unsigned long) window;
    	DoUpdateEvent(&theEvent);
        result = noErr;
    }
    else if (GetEventKind(event) == kEventWindowBoundsChanged)
    {
        InvalWindowRect(window, GetWindowPortBounds(window, &bounds));
        result = noErr;
    }
	
    else if (GetEventKind(event) == kEventWindowClose)
    {
        ClosePlayerWindow(GetDialogFromWindow(window));
        result = noErr;
    }
    
    return result;
}

#if 0
static void CarbonEventLoop(void)
{
	RunApplicationEventLoop();
}
#endif

pascal short MyGetDirHook(short item, DialogPtr dPtr)
{
	short				itemType,i;
	Handle				itemHandle;
	Rect				itemRect;
	
	switch(item) {
		case 10:
		case 11:
		case 12:
			for (i = 10; i <= 12; i++) {
				GetDialogItem (dPtr, i, &itemType, &itemHandle, &itemRect);
				SetControlValue((ControlHandle) itemHandle, 0);
			}
			GetDialogItem (dPtr, item, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 255);
			Quality = item;
			break;
			
		case 14:
		case 15:
			for (i = 14; i <= 15; i++) {
				GetDialogItem (dPtr, i, &itemType, &itemHandle, &itemRect);
				SetControlValue((ControlHandle) itemHandle, 0);
			}
			GetDialogItem (dPtr, item, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 255);
			Mode = item;
			break;
			
		case -1:
			GetDialogItem (dPtr, Mode, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 255);
			GetDialogItem (dPtr, Quality, &itemType, &itemHandle, &itemRect);
			SetControlValue((ControlHandle) itemHandle, 255);
			break;
	}
	return(item);
}

void DragReferencedWindow(WindowPtr windowToDrag, Point startPoint, const Rect *draggingBounds)
{
	Rect		dragRect;
	GrafPtr		savePort;
	RgnHandle	dragRegion;
	long		dragResult;
	short		topLimit;
	short		newHorizontalWindowPosition;
	short		newVerticalWindowPosition;
	short		horizontalOffset;
	short		verticalOffset;
	Point		pt;
	
	if (WaitMouseUp()) {
		topLimit = GetMBarHeight() + 4;
		if (AHelpDlog != NULL) AddWindowBar();
		
		dragRect = *draggingBounds;
		if (dragRect.top < topLimit) dragRect.top = topLimit;
		
		GetPort(&savePort);
		
		SetClip(GetGrayRgn());
		
		ClipAbove(windowToDrag);
		
		// Create a region to drag
		dragRegion = NewRgn();
		
		GetWindowRegion(windowToDrag, kWindowStructureRgn, dragRegion);
		
		// Drag the window around
		dragResult = DragGrayRgn(dragRegion, startPoint, &dragRect, &dragRect, 0, nil);
		
		// Restore the port for coordinate conversion.
		if (AHelpDlog != NULL)
			RemoveWindowBar();
		
		SetPort(savePort);
		
		if (dragResult != 0)
		{
			horizontalOffset = dragResult & 0xFFFF;
			verticalOffset = dragResult >> 16;
			
			// Only move it if it stayed inside the dragging box.
			if (verticalOffset != -32768)
			{
				Rect	rgnBBox;
				
				//windowContentRegion = NewRgn();
				//GetWindowRegion(windowToDrag, kWindowContentRgn, windowContentRegion);
				//GetRegionBounds(windowContentRegion, &rgnBBox);
				
				SetPortWindowPort(windowToDrag);
				
				GetPortBounds(GetWindowPort(windowToDrag), &rgnBBox);
				
				pt.v = rgnBBox.top;
				pt.h = rgnBBox.left;
				
				LocalToGlobal(&pt);
				
				SetPort(savePort);
				
				newHorizontalWindowPosition = pt.h + horizontalOffset;
				newVerticalWindowPosition = pt.v + verticalOffset;
				
				MoveWindow((WindowPtr)windowToDrag, newHorizontalWindowPosition, newVerticalWindowPosition, false);
				
				//	DisposeRgn(windowContentRegion);
			}
		}
		
		DisposeRgn(dragRegion);
	}
}

void UseSameLeft(WindowPtr whichWindow)
{
	//	WindowPeek	wPeek;
	//	WStateData	*wspd;
	Rect		caRect, stdRect;
	
	//	wPeek = (WindowPeek) whichWindow;
	
	//	wspd = (WStateData*) *(wPeek->dataHandle);
	
	GetWindowStandardState(whichWindow, &stdRect);
	
	GetPortBounds(GetWindowPort(whichWindow), &caRect);
	
	stdRect.left = thePrefs.WinPos[GetWRefCon(whichWindow)].h;
	stdRect.right = stdRect.left + caRect.right;
	
	SetWindowStandardState(whichWindow, &stdRect);
}

/*typedef pascal long (*myProcPtr) (short varCode, WindowPeek window, short message, long param);
 
 
 Boolean TrackInHelpButton(WindowPtr wind, Point where)
 {
 ProcPtr	myProcPtr = (ProcPtr) *((WindowPeek) wind)->windowDefProc;
 
 //(long) (*myProcPtr) (((WindowPeek) wind)->windowKind, (WindowPeek) wind, wHit, where);
 
 return true;
 }*/

void DoMouseDown(EventRecord theEventI)
{
	short			thePart;
	Point			Start;
	WindowPtr 		whichWindow;
	char			stillInGoAway;
	//GrafPtr			savedPort;
	long			menuChoice;
	Boolean			mouseInDown;
	Rect			caRect;
	GrafPtr			savedPort;
	
	thePart = FindWindow(theEventI.where, &whichWindow);
	
	// A REMETTRE
	
	DesactivateCmdWindow();
	UpdateALLWindow();
	
	switch(thePart)
	{
		case inContent:
			PressInDialog = true;
#if 0
			if (whichWindow != FrontNonFloatingWindow() )
			{
				SelectWindow(whichWindow );
			}
			else //if (IsDialogEvent(&theEvent))
			{
				NEWdoDlgEvt(&theEvent, whichWindow);
			}
#endif
			
			// A REMETTRE
			
			if (whichWindow == GetDialogWindow(ToolsDlog)) {
				//	SelectWindow(whichWindow);
				//if (IsDialogEvent(&theEvent))
				{
					short	whichItem;
					Point	pt;
					
					GetPort(&savedPort);
					SetPortWindowPort(whichWindow);
					
					pt = theEventI.where;
					GlobalToLocal(&pt);
					
					whichItem = FindDialogItem(ToolsDlog, pt);
					whichItem++;
					
					//	if (DialogSelect(&theEventI, &ToolsDlog, &whichItem))
					{
						ProcessDoItemPress(GetWRefCon(whichWindow), whichItem, ToolsDlog);
					}
					//NEWdoDlgEvt(&theEvent, whichWindow);
					
					SetPort(savedPort);
				}
			} else {
				if (GetWRefCon(whichWindow) != RefActiveHelp) {
					mouseInDown = true;
					if (oldWindow != whichWindow) 	{
						SelectWindow2(whichWindow);
						//SelectWindow(whichWindow);
						
						//theEventI.message = (long) whichWindow;
						//DoUpdateEvent(&theEventI);
						//if (GetWRefCon(whichWindow) == RefMozart) mouseInDown = false;
						
						PostEvent(mouseDown, 0);
					} else {
						if (mouseInDown) {
							//	if (IsDialogEvent(&theEvent))
							{
								NEWdoDlgEvt(&theEvent, whichWindow);
							}
						}
					}
				}
			}
			
			SetPortWindowPort(whichWindow);
			break;
			
		case inZoomIn:
		case inZoomOut:
			if (TrackBox(whichWindow, theEventI.where, thePart)) {
				theEvent.what = 0;
				
				AdjustZoomIn(whichWindow);
				
				EraseGrowIcon(GetDialogFromWindow(whichWindow));
				
				SetPortWindowPort(whichWindow);
				
				switch(GetWRefCon(whichWindow)) {
					case RefVisual:
						VisualFullScreen();
						break;
						
					case RefTools:
						SwitchSize();
						DesactivateCmdWindow();
						break;
						
					case RefClassic:
						ZoomWindow(whichWindow, thePart, false);
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						DoGrowClassic();
						break;
						
					case RefPiano:
						ZoomWindow(whichWindow, thePart, false);
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						DoGrowPiano();
						break;
						
					case RefPlayer:
						ZoomWindow(whichWindow, thePart, false);
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						DoGrowOscillo();
						break;
						
					case RefSpectrum:
						ZoomWindow(whichWindow, thePart, false);
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						DoGrowSpectrum();
						break;
						
					case RefMemory:
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						
						if (caRect.bottom == 33) MySizeWindow(GetDialogFromWindow(whichWindow), caRect.right, 90, true);
						else MySizeWindow(GetDialogFromWindow(whichWindow), caRect.right, 33, true);
						break;
						
					case RefMODList:
						UseSameLeft(whichWindow);
						
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						DoGrowMODList(GetDialogFromWindow(whichWindow));
						break;
						
					case RefHelp:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						DoGrowHelpOnline();
						break;
						
					case RefParti:
						UseSameLeft(whichWindow);
						
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						DoGrowParti(GetDialogFromWindow(whichWindow));
						break;
						
					case RefPatList:
						UseSameLeft(whichWindow);
						
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						DoGrowPatList(GetDialogFromWindow(whichWindow));
						break;
						
					case RefInstruList:
						UseSameLeft(whichWindow);
						
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						DoGrowInstruList(GetDialogFromWindow(whichWindow));
						break;
						
					case RefStaff:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						DoGrowStaff();
						break;
						
					case RefWave:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						DoGrowWave();
						break;
						
					case RefPartition:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						DoGrowPartition();
						break;
						
					case RefAdaptators:
						UseSameLeft(whichWindow);
						
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						
						DoGrowAdap(GetDialogFromWindow(whichWindow));
						break;
						
					case RefDigiView:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						SetPortWindowPort(whichWindow);
						GetPortBounds(GetWindowPort(whichWindow), &caRect);
						EraseRect(&caRect);
						InvalWindowRect(whichWindow, &caRect);
						
						DoGrowDigi(GetDialogFromWindow(whichWindow));
						break;
						
					case RefSample:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						DoGrowSample(GetDialogFromWindow(whichWindow));
						break;
						
					case RefMozart:
						ZoomWindow(whichWindow, thePart, false);
						theEvent.what = 0;
						
						DoGrowMozart(GetDialogFromWindow(whichWindow));
						break;
						
					case RefInstruView:
						DoGrowInstruView();
						break;
						
						/*	case RefTrackView:
						 DoGrowTrackView();
						 break;*/
				}
				
				//	wspd->stdState = savedStdState;
				
				EraseGrowIcon(GetDialogFromWindow(whichWindow));
			}
			Start.h = Start.v = 0;
			LocalToGlobal(&Start);
			
			thePrefs.WinPos[ GetWRefCon(whichWindow)].v = Start.v;
			thePrefs.WinPos[ GetWRefCon(whichWindow)].h = Start.h;
			break;
			
		case inDrag:
			if (oldWindow != whichWindow)
			{
				SelectWindow2(whichWindow);
				theEventI.message = (long) whichWindow;
				DoUpdateEvent(&theEventI);
			}
			else SelectWindow2(whichWindow);
			SetPortWindowPort(whichWindow);
			
#if 0
			if (IsPressed(0x3A) && HelpAvalaible)
			{
				SetCursor(&HelpCrsr);
				
				while (Button());
				
				switch(GetWRefCon(whichWindow))
				{
					case 	RefInstruList:		ShowSection(0);	break;
					case 	RefPartition:		ShowSection(0);	break;
					case 	RefInstruEdit:		ShowSection(0);	break;
					case 	RefBoutons:			ShowSection(0);	break;
					case 	RefPatList:			ShowSection(0);	break;
					case 	RefAdaptators:		ShowSection(0);	break;
					case 	RefMODList:			ShowSection(0);	break;
					case 	RefTools:			ShowSection(0);	break;
					case 	RefSample:			ShowSection(0);	break;
					case 	RefPiano:			ShowSection(0);	break;
					case 	RefClassic:			ShowSection(0);	break;
					case 	RefPlayer:			ShowSection(0);	break;
					case 	RefMozart:			ShowSection(0);	break;
					case 	RefInstruView:		ShowSection(0);	break;
					case 	RefDigiView:		ShowSection(0);	break;
					case 	RefCmd:				ShowSection(0);	break;
					case 	RefPref:			ShowSection(0);	break;
					case 	RefTrackView:		ShowSection(0);	break;
					case 	RefMemory:			ShowSection(0);	break;
					case 	RefSpectrum:		ShowSection(0);	break;
					case 	RefParti:			ShowSection(0);	break;
					case 	RefHelp:			ShowSection(0);	break;
				}
				
				SetCursor(GetQDGlobalsArrow(&qdarrow));
			}
			else
#endif
			{
				BitMap	screenBits;
				
				GetQDGlobalsScreenBits(&screenBits);
				
				DragWindow(whichWindow, theEventI.where, &screenBits.bounds);			
				
				Start.h = Start.v = 0;
				LocalToGlobal(&Start);
				
				
				thePrefs.WinPos[ GetWRefCon(whichWindow)].v = Start.v;
				thePrefs.WinPos[ GetWRefCon(whichWindow)].h = Start.h;
			}
			break;
			
		case inGoAway:
			stillInGoAway = TrackGoAway(whichWindow, theEventI.where);
			if (stillInGoAway) {
				GetKeys(km);
				if (IsPressed(0x3A)) {
					CloseALLWindow();
					
					SelectWindow2(FrontWindow());
				} else {
					SelectWindow2(NextWindowVisible(whichWindow));
					
					CheckOneWindow(whichWindow);
					
					ClosePlayerWindow(GetDialogFromWindow(whichWindow));
				}
			}
			break;
			
		case inMenuBar:
			menuChoice = MenuSelect(theEventI.where);
			//if (theMenuRgn != NULL) ResetMenuRgn(true);
			
			HandleMenuChoice(menuChoice);
			break;
			
		case inGrow:
			switch(GetWRefCon(whichWindow)) {
			case RefPiano:
				DoGrowPiano();
				break;
				
			case RefSpectrum:
				DoGrowSpectrum();
				break;
				
			case RefPlayer:
				DoGrowOscillo();
				break;
				
			case RefHelp:
				DoGrowHelpOnline();
				break;
				
			case RefParti:
				DoGrowParti(GetDialogFromWindow(whichWindow));
				break;
				
			case RefPatList:
				DoGrowPatList(GetDialogFromWindow(whichWindow));
				break;
				
			case RefMozart:
				DoGrowMozart(GetDialogFromWindow(whichWindow));
				break;
				
			case RefSample:
				DoGrowSample(GetDialogFromWindow(whichWindow));
				break;
				
			case RefInstruList:
				DoGrowInstruList(GetDialogFromWindow(whichWindow));
				break;
				
			case RefPartition:
				DoGrowPartition();
				break;
				
			case RefWave:
				DoGrowWave();
				break;
				
			case RefStaff:
				DoGrowStaff();
				break;
				
			case RefAdaptators:
				DoGrowAdap(GetDialogFromWindow(whichWindow));
				break;
				
			case RefDigiView:
				DoGrowDigi(GetDialogFromWindow(whichWindow));
				break;
				
			case RefMODList:
				DoGrowMODList(GetDialogFromWindow(whichWindow));
				break;
				
			case RefTools:
				break;
				
			case RefClassic:
				DoGrowClassic();
				break;
		}
			GetPortBounds(GetWindowPort(whichWindow), &caRect);
			
			thePrefs.WinHi[ GetWRefCon(whichWindow)] = caRect.bottom;
			break;
	}
}

void DoUpdateEvent(EventRecord *theEventI)
{
	if (theEventI->message == 0) MyDebugStr(__LINE__, __FILE__, "Err DoUpdateEvent");
	
	//if (EmptyRgn(((WindowPeek) theEventI->message)->updateRgn)) return;
	
	switch(GetWRefCon(((WindowPtr) theEventI->message)))
	{
		case RefVST:
			VSTEditorUpdate(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
			//case RefCube:
			//UpdateCubeWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			//break;
			
		case RefPref:
			UpdatePrefWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefMozart:
			UpdateMozartWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefPlayer:
			UpdateOscilloWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefSpectrum:
			UpdateSpectrumWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefInstruList:
			UpdateInstruListWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefInstruView:
			UpdateInstruView(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
			/*	case RefTrackView:
			 UpdateTrackView(GetDialogFromWindow((WindowPtr) theEventI->message));
			 break;*/
			
		case RefPartition:
			UpdatePartitionWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefStaff:
			UpdateStaffWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefWave:
			UpdateWaveWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefAdaptators:
			UpdateAdapWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefMODList:
			UpdateMODListWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefTools:
			UpdateToolsWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefSample:
			UpdateSampleWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefQuicktime:
			UpdateQTWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefEqualizer:
			UpdateEQWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefVisual:
			UpdateVisualWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefHelp:
			UpdateHelpOnline();
			break;
			
		case RefActiveHelp:
			UpdateAHelpWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefParti:
			UpdatePartiWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefPatList:
			UpdatePatListWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefPiano:
			UpdatePianoWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
			/*	case RefFindReplace:
			 UpdateFind(GetDialogFromWindow((WindowPtr) theEventI->message));
			 break;
			 */	
		case RefClassic:
			UpdateClassicWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefDigiView:
			UpdateDigiListWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefMemory:
			UpdateMemWindow(GetDialogFromWindow((WindowPtr) theEventI->message));
			break;
			
		case RefSndExport:
			UpdateWorkingWindow();
			break;
			
		default:
			break;
	}
}

void UpdateALLWindow(void)
{
	WindowPtr	aWind;
	EventRecord	aEvt;
	
	aWind = FrontWindow();
	
	while (aWind != NULL)
	{
		if (IsWindowVisible(aWind))
		{
			RgnHandle	updateRgn = NewRgn();
			
			GetWindowRegion(aWind, kWindowUpdateRgn, updateRgn);
			
			if (!EmptyRgn(updateRgn))
			{
				aEvt.message = (long) aWind;
				DoUpdateEvent(&aEvt);
			}
			
			DisposeRgn(updateRgn);
		}	
		//if (QDIsPortBuffered(GetWindowPort(aWind)))
		//QDFlushPortBuffer(GetWindowPort(aWind), NULL);
		
		aWind = GetNextWindow(aWind);
	}
}

void ForceUpdateALLWindow(void)
{
	WindowPtr	aWind;
	GrafPtr		savePort;
	EventRecord	aEvt;
	Rect		caRect;
	
	aWind = FrontWindow();
	
	while (aWind != NULL)
	{
		if (IsWindowVisible(aWind))
		{
			GetPort(&savePort);
			SetPortWindowPort(aWind);
			GetPortBounds(GetWindowPort(aWind), &caRect);
			InvalWindowRect(aWind, &caRect);
			SetPort(savePort);
			
			aEvt.message = (long) aWind;
			DoUpdateEvent(&aEvt);
		}
		aWind = GetNextWindow(aWind);
	}
}

Boolean PtRect(Point	thePoint,	Rect *theRect)
{
	
	if(thePoint.h  <= theRect->right &&
	   thePoint.h  >= theRect->left  &&
	   thePoint.v  <= theRect->bottom &&
	   thePoint.v  >= theRect->top) return true;
	else return false;
}

extern Str255	versString;

void DrawVERSION(void)
{
	Str255				strTemp;
	char				str2[ 300];
	Handle				aHandle;
	
	/**********************/
	ForeColor(whiteColor);
	TextFont(21);		TextSize(12);
	
	pStrcpy((unsigned char*) versString, "\pVers:");
	
	aHandle = GetResource('vers', 1);
	pStrcpy(strTemp, (unsigned char*) (*aHandle) + 6L);
	ReleaseResource(aHandle);
	
	pStrcat((unsigned char*) versString, strTemp);
	pStrcpy((unsigned char*) str2, versString);
	
	MyP2CStr((unsigned char*) str2);
	WriteCText(GetDialogFromWindow(FrontWindow()), 2, (Ptr) str2);
	
	/*	strcpy(str2, "Internal: ");
	 NumToString((long) thePrefs.NoStart, strTemp);
	 MyP2CStr(strTemp);
	 strcat(str2, (Ptr) strTemp);
	 WriteCText(FrontWindow(), 3, (Ptr) str2);
	 */
	
	
	TextFont(0);	TextSize(0);
	ForeColor(blackColor);
	/**********************/
}

/**************************************************************************************************
 *
 *	Start Up Proceeding, open or print file, or create a new file.
 *
 **************************************************************************************************/

static DialogPtr	myStartUpDlog;

void StartDialog(void)
{
	SetCursor(&watchCrsr);
	
	myStartUpDlog = GetNewDialog(171, NULL, (WindowPtr) -1L);
	if (myStartUpDlog == NULL)
		abort();
	SetPortDialogPort(myStartUpDlog);
	
	{
		short	itemType;
		Handle	itemHandle;
		Rect	itemRect;
		
		GetDialogItem(myStartUpDlog, 2, &itemType, &itemHandle, &itemRect);
		OffsetRect(&itemRect, 0, -50);
		SetDialogItem(myStartUpDlog, 2, itemType, itemHandle, &itemRect);
	}
	
	DrawDialog(myStartUpDlog);
	WaitNextEvent(everyEvent, &theEvent, 1, NULL);
	
	//	if (QDIsPortBuffered(GetDialogPort(myStartUpDlog)))
	//		QDFlushPortBuffer(GetDialogPort(myStartUpDlog), NULL);
	
	if (DebuggingMode)
	{
		Rect	caRect;
		
		ForeColor(redColor);
		
		GetPortBounds(GetDialogPort(myStartUpDlog), &caRect);
		
		MoveTo(5, caRect.top + 20);
		
		TextSize(20);
		
		DrawString("\pDebugging Mode");
		//Unless we do this, the "Debugging Mode" text won't display unless a window moves over it
		if (QDIsPortBuffered(GetDialogPort(myStartUpDlog)))
			QDFlushPortBuffer(GetDialogPort(myStartUpDlog), NULL);
		
	}
}

void EndDialog(void)
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
	TextFont(0);	TextSize(0);
	
	FlushEvents(everyEvent, 0);
	
	//Delay(180, &oldTicks);
	
	DisposeDialog(myStartUpDlog);
}

extern pascal Boolean MyDlgFilterSF(DialogPtr theDlg, EventRecord *theEvt, short *itemHit, void *data);

static OSType		specificType;

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
	
	switch(showWhat) {
		case allMusics:
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'sTAT') return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'STCf') return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'MADK') return false;
			
			for (i = 0; i < gMADLib->TotalPlug; i++)
			{
				if (pb->hFileInfo.ioFlFndrInfo.fdType == Ptr2OSType(gMADLib->ThePlug[ i].type))	return false;
			}
			
			if (QTTypeConversion(pb->hFileInfo.ioFlFndrInfo.fdType)) return false;
			break;
			
		case allReadable:
			RollCursor();
			
			if (EqualString("\pIcon", pb->hFileInfo.ioNamePtr, false, false)) MyDebugStr(__LINE__, __FILE__, "");
			
			
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'sTAT') return false;
			if (pb->hFileInfo.ioFlFndrInfo.fdType == 'STCf') return false;
			
			
			FSMakeFSSpec(pb->hFileInfo.ioVRefNum, 0, pb->hFileInfo.ioNamePtr, &spec);
			
			
			if (MADMusicIdentifyFSp(gMADLib, tempC, &spec) == noErr) return false;
			else return true;
			break;
			
		case allFiles:		return false;	break;
			
		case MADK:	if (pb->hFileInfo.ioFlFndrInfo.fdType == 'MADK') return false;	break;
			
		default:	if (pb->hFileInfo.ioFlFndrInfo.fdType == specificType) return false;	break;
	}
	
	return true;
}

void WriteSupportedFormat(DialogPtr	aDia)
{
	short	i;
	Str255	text;
	
	pStrcpy(text, "\pMADK");
	
	for (i = 0; i < gMADLib->TotalPlug; i++) {
		Str255 pMenuName;
		GetPStrFromCFString(gMADLib->ThePlug[ i].MenuName, pMenuName);
		
		pStrcat(text, "\p - ");
		pStrcat(text, pMenuName);
	}
	
	SetDText(aDia, 14, text);
}

//static Boolean 		needUp;
static Boolean		AddAll;
//static MenuHandle	showWhatMenu;
//static OSType		plugListO[ 25];

#if 0
pascal short MyDlgHook2(short item, DialogPtr theDialog, void *myDataPtr)
{
	Boolean				MiseAjour = false;
	//	StandardFileReply	*IntReply;
	GrafPtr				savePort;
	FInfo				fndrInfo;
	Str255				str;
	char				tempC[ 5];
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	if (myDataPtr == NULL) MyDebugStr(__LINE__, __FILE__, "");
	
	IntReply = (StandardFileReply*) myDataPtr;
	
	switch(item)
	{	
		case getOpen:
			if (IntReply->sfType != 'sTAT' && IntReply->sfType != 'STCf')
			{
				OSType type;
				
				switch(showWhat)
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
					
					specificType = plugListO[ showWhat];
					
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
				
				switch(showWhat)
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
				
				switch(showWhat)
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
					else a[ 0] = 0;
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

OSErr NGetFileName(FSSpec *spec)
{
	return DoCustomOpen(spec);
}

void NOpenMusic()
{
	if (GereChanged() != noErr)
		return;
	
	{
		FSSpec	reply;
		
		AddAll = false;
		
		//#if MACOS9VERSION
		if (NGetFileName(&reply) == noErr)
		{
			SetCursor(GetQDGlobalsArrow(&qdarrow));
			
			if (AddAll) {
				CInfoPBRec	block;
				Str255		directoryName;
				
				block.dirInfo.ioDrParID = reply.parID;
				block.dirInfo.ioNamePtr = directoryName;
				
				block.dirInfo.ioVRefNum = reply.vRefNum;
				block.dirInfo.ioFDirIndex = -1;
				block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
				if (PBGetCatInfoSync(&block) == noErr) {
					pStrcpy(reply.name, directoryName);
					reply.parID = block.dirInfo.ioDrParID;
					AESendOpenFile(&reply);
				}
			}
			else AESendOpenFile(&reply);
		}
		//#else
#if 0
		if (DoStandardOpen(&reply, "\pOpen a MADI music file", 'MADI') == noErr)
		{
			OSType 	tempType;
			char	tempC[ 5];
			FInfo		fndrInfo;
			OSErr	err;
			
			err = HGetFInfo(reply.vRefNum, reply.parID, reply.name, &fndrInfo);
			
			if (OpenableFile(fndrInfo.fdType, &reply) == true ||
			   fndrInfo.fdType == 'sTAT' ||
			   fndrInfo.fdType == 'STCf' ||
			   MADMusicIdentifyFSp(gMADLib, tempC, &reply) == noErr)
			{
				if (fndrInfo.fdType != 'sTAT' && fndrInfo.fdType != 'STCf')
				{
					AddMODList(false,
							   reply.name,
							   reply.vRefNum,
							   reply.parID);
					
					if (ImportFile(reply.name, reply.vRefNum, reply.parID, fndrInfo.fdType))
					{
						if (thePrefs.AutoPlayWhenOpen) DoPlay();		// WANT TO PLAY ?
					}
				}
			}
			
		}
#endif
		
	}
	
	PatchSave = false;
}

void ActiveInstrumentMenu(Boolean);
void ActivePatternMenu(Boolean);

void InitImportMenu(void)
{
	short	i;
	
	ExportMenu	= GetMenu(139);
	
	for (i = 0; i < gMADLib->TotalPlug; i++)
	{
		Str255 pMenuName;
		GetPStrFromCFString(gMADLib->ThePlug[ i].MenuName, pMenuName);
		
		switch(gMADLib->ThePlug[ i].mode)
		{
			case 'EXIM':
			case 'EXPL':
				AppendMenu(ExportMenu, pMenuName);
				break;
		}
	}
	
	InsertMenu(ExportMenu, hierMenu);
}

void MenuBarInit(void)
{
	Handle			myMenuBar;
	short				i;
	Str255			NomPrenom;
	long 			response;
	OSStatus		err = noErr;
	
	/****** MAIN MENU ********/
	
	myMenuBar			=	GetNewMBar(MAINMENUBAR);
	SetMenuBar(myMenuBar);
	
	
	FileMenu			=	GetMenuHandle(mFileMenu);
	
	// check to see if we should modify the File menu's quit item in
	// order to follow the Aqua HI quit guideline
	err = Gestalt(gestaltMenuMgrAttr, &response);
	if ((err == noErr) && (response & gestaltMenuMgrAquaLayoutMask))
	{
		DeleteMenuItem(FileMenu, 22);
		DeleteMenuItem(FileMenu, 21);
	}
	
	AppleMenu			=	GetMenuHandle(mAppleMenu);
	if (!HelpAvalaible) DisableMenuItem(AppleMenu, 2);
	
	PatchSave			=	true;
	DisableMenuItem(FileMenu, 18);
	
	if (!MIDIHardware) DisableMenuItem(FileMenu, 19);
	if (!MIDIHardware) DisableMenuItem(FileMenu, 20);
	
	InstruEditMenu		=	GetMenuHandle(mInstrumentsMenu);
	ActiveInstrumentMenu(false);
	
	PrefSubMenu			=	GetMenu(200);
	InsertMenu(PrefSubMenu, hierMenu);
	
	NewSoundMenu		= 	GetMenu(154);
	
	if (!QuicktimeInstruAvailable) DisableMenuItem(NewSoundMenu, 3);
	if (ToneGenerator == -1) DisableMenuItem(NewSoundMenu, 2);		
	
	InsertMenu(NewSoundMenu, hierMenu);
	
	EditorMenu			=	GetMenuHandle(mEditorMenu);
	ViewsMenu			=	GetMenuHandle(mViewsMenu);	
	
	VSTMenu				=	GetMenu(180);
	//	if (!CubeReady) DisableMenuItem(ViewsMenu, 20);
	
	EditMenu			=	GetMenuHandle(mEditMenu);
	
	WindowMenu			=	GetMenuHandle(mWindowMenu);
	HelpMenu			=	GetMenuHandle(mHelpMenu);
	
	if (!HelpAvalaible) DisableMenuItem(HelpMenu, 1);
	
	PatternEditMenu		=	GetMenuHandle(mPatternsMenu);
	ActivePatternMenu(false);
	
	InitImportMenu();
	WindowsSettingsInit();
	
	DrawMenuBar();
	
	/******************/
	
	InstruMenu			=	GetMenu(134);
	InternetMenu		=	GetMenuHandle(mInternetMenu);
	for (i = 0; i < URLsNo; i++)
	{
		AppendMenu(InternetMenu, URLsDesc[ i]);
	}
	
	EffectMenu			=	GetMenu(135);
	TracksNumberMenu	=	GetMenu(142);
	for (i = 2; i < 100; i+= 2)
	{
		Str255 str;
		
		NumToString(i, str);
		AppendMenu(TracksNumberMenu, str);
	}
	
	BaseNote			= 	GetMenu(143);
	TrackView			=	GetMenu(137);
	OsciTypeMenu		= 	GetMenu(150);
	SpectrumTypeMenu	= 	GetMenu(153);
	OsciHMenu			=	GetMenu(151);
	OsciModeMenu		=	GetMenu(168);
	
	// Note Menu
	
	NoteMenu			=	GetMenu(141);
	
	GetNoteString(0, NomPrenom);	SetMenuItemText(NoteMenu, 1, NomPrenom);
	for (i = 1; i < NUMBER_NOTES; i++)
	{
		GetNoteString(i, NomPrenom);
		AppendMenu(NoteMenu, NomPrenom);
	}
	//TODO: Window and Help menus
}

short IntInfoL(short ID)
{
	short					itemHit;
	Str255					theString;
	GrafPtr					savedPort;
	DialogPtr				aDia;
	ProcessSerialNumber		PSN;
	
	GetFrontProcess(&PSN);
	if (PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
	   PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) SetFrontProcess(&PSN);
	
	GetPort(&savedPort);
	
	GetIndString(theString, 128, ID);
	ParamText(theString, NULL, NULL, NULL);
	
	aDia = GetNewDialog(3001, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	do {
		//	ModalDialog(MyDlgFilterDesc, &itemHit);
		
		MyModalDialog(aDia, &itemHit);
		
	}while (itemHit != 1 && itemHit != 2);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
	
	if (itemHit == 1) return(1);
	else return(0);
}

short InfoL(short ID)
{
	ProcessSerialNumber		PSN;
	
	GetFrontProcess(&PSN);
	if(PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
	   PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) {
		return true;
	} else
		return IntInfoL(ID);
}

void MADErreur(OSErr err)
{
	switch(err)
	{
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
			Erreur(70, err);
			break;
			
		case MADCannotFindPlug:
			Erreur(71, err);
			break;
			
		default:
			Erreur(70, err);
			break;
	}
}

void IntErreur(short ID, OSErr theErr)
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
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	do {
		MyModalDialog(aDia, &itemHit);	//ModalDialog(MyDlgFilterDesc, &itemHit);
	} while (itemHit != 1);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
}

void OtherIntErreur(short ID, OSErr theErr, Str255 otherstr)
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
	
	SetDText(aDia, 9, otherstr);
	
	AutoPosition(aDia);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	do {
		MyModalDialog(aDia, &itemHit);	//ModalDialog(MyDlgFilterDesc, &itemHit);
		
	} while (itemHit != 1);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	
	UpdateALLWindow();
}


void Erreur(short ID, OSErr theErr)
{
	ProcessSerialNumber		PSN;
	
	GetFrontProcess(&PSN);
	if (PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
	   PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN ||
	   UseAEErreur == true)
	{
		OSType				sign = 'SNPL';
		AppleEvent			aeEvent, reply;
		AEDesc				target, listElem, fileList;
		OSErr				err[ 2], iErr;
		
#define kPlayerPROClass			'SNPL'
#define kPlayerPROSignature		'SNPL'
		
#define kErreurID				'AERR'
		
		
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
		
		iErr = AECreateList(nil,0,false, &fileList);
		
		//
		
		err[ 0] = ID;
		err[ 1] = theErr;
 		AECreateDesc('dErr', (Ptr) &err, sizeof(err), &listElem);
		
		iErr = AEPutDesc(&fileList, 0, &listElem);
		if (iErr) return;
		
		AEDisposeDesc(&listElem);
		
		iErr = AEPutParamDesc(&aeEvent, keyDirectObject,&fileList);
		if(iErr) return;
		iErr = AEDisposeDesc(&fileList);
		
		//
		
		iErr = AESend(&aeEvent,
					  &reply,
					  kAENoReply,
					  kAENormalPriority,
					  kAEDefaultTimeout,
					  NULL,
					  NULL);
	}
	else IntErreur(ID, theErr);
}

OSErr GereChanged(void)
{
	short		itemHit;
	DialogPtr	aDia;
	GrafPtr		savedPort;
	
	if (curMusic)
	{
		if (curMusic->hasChanged == false) return noErr;
	}
	
	GetPort(&savedPort);
	
	if (curMusic)
	{
		ParamText(curMusic->musicFileName, NULL, NULL, NULL);
	}
	
	aDia = GetNewDialog(3002, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	
	do
	{
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(aDia, &itemHit);
		
	}while (itemHit != 1 && itemHit != 2 && itemHit != 3);
	
	DisposeDialog(aDia);
	SetPort(savedPort);
	UpdateALLWindow();
	
	switch(itemHit)
	{
		case 1:
			if (curMusic->originalFormat != 'MADK') SaveMOD(true, 'MADK');
			else SaveMOD(false, 'MADK');
			curMusic->hasChanged = false;
			return noErr;
			break;
			
		case 2:
			curMusic->hasChanged = false;
			return noErr;
			break;
			
		case 3:
			return -1;
			break;
	}
	
	return noErr;
}

void WindowsSettingsInit()
{
	short	i;
	
	WinSettingsMenu	= GetMenu(176);
	
	for (i = 0; i < 3; i++)
	{
		SetMenuItemText(WinSettingsMenu, i+1, thePrefs.WinNames[ i]);
	}
	
	InsertMenu(WinSettingsMenu, hierMenu);
}

void WindowsSettingsOption()
{
	Str255		theString;
	long 		mresult, i;
	GrafPtr		savedPort;
	DialogPtr	aDia;
	short		itemHit, curSelec, itemType;
	MenuHandle	tMenu;
	Rect		tempRect;
	Handle		itemHandle;
	Point		myPt;
	
	GetPort(&savedPort);
	
	aDia = GetNewDialog(136, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	
	
	tMenu = NewMenu(266, "\pWindows State");
	
	for (i = 0; i < 3; i++)
	{
		AppendMenu(tMenu, thePrefs.WinNames[ i]);
	}
	
	curSelec = 0;
	SetDText(aDia, 7, "\p1");
	
	SetDText(aDia, 4, thePrefs.WinNames[ curSelec]);
	SelectDialogItemText(aDia, 4, 0, 300);
	
	do
	{
		//	ModalDialog(MyDlgFilterDesc, &itemHit);
		
		MyModalDialog(aDia, &itemHit);
		
		switch(itemHit)
		{
			case 8:
				InsertMenu(tMenu, hierMenu );
				GetDialogItem(aDia, itemHit, &itemType, &itemHandle, &tempRect);
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(tMenu, curSelec+1, 0xa5);
				
				mresult = PopUpMenuSelect(	tMenu,
										  myPt.v,
										  myPt.h,
										  curSelec+1);
				
				SetItemMark(tMenu, curSelec+1, 0);
				
				if (HiWord(mresult ) != 0 )
				{
					curSelec = LoWord(mresult) - 1;
					SetDText(aDia, 4, thePrefs.WinNames[ curSelec]);
					SelectDialogItemText(aDia, 4, 0, 300);
					
					NumToString(curSelec + 1, theString);
					SetDText(aDia, 7, theString);
				}
				DeleteMenu(GetMenuID(tMenu));
				break;
		}
		
	}while (itemHit != 1 && itemHit != 2);
	
	DisposeMenu(tMenu);
	
	if (itemHit == 1)
	{
		SaveWindowState(curSelec);
		
		GetDText(aDia, 4, thePrefs.WinNames[ curSelec]);
		
		if (thePrefs.WinNames[ curSelec][ 0] > 30) thePrefs.WinNames[ curSelec][ 0] = 30;
		
		for (i = 0; i < 3; i++)
		{
			SetMenuItemText(WinSettingsMenu, i+1, thePrefs.WinNames[ i]);
		}
	}
	
	DisposeDialog(aDia);
	SetPort(savedPort);
}

void WindowsSettingsMenu(short item)
{
	switch(item)
	{	
		case 1:
		case 2:
		case 3:
			if (theEvent.modifiers & controlKey)
			{
				SaveWindowState(item-1);
			}
			else
			{
				CheckShowWindow();
				
				if (thePrefs.ActiveHelp) CreateAHelpWindow();
				ShowWindowPref(item-1);
				
				ShowWindow(GetDialogWindow(ToolsDlog));
				SelectWindow(GetDialogWindow(ToolsDlog));
				SelectWindow2(NextWindowVisible(GetDialogWindow(ToolsDlog)));
				UpdateALLWindow();
			}
			break;
			
		case 5:
			WindowsSettingsOption();
			break;
	}
}

void HandleMenuChoice(long menuChoice)
{
	short theMenu;
	short	theItem;
	
	if (menuChoice!=0)
	{
		theMenu = HiWord(menuChoice);
		theItem = LoWord(menuChoice);
		
		switch(theMenu)
		{
			case mAppleMenu:
				HandleAppleChoice(theItem);
				break;
				
			case mEditMenu:
				HandleEdit(theItem);
				break;
				
			case mFileMenu:
				HandleFileChoice(theItem);
				break;
				
			case mPatternsMenu:
				HandlePatternChoice(theItem);
				break;
				
			case 140:
				HandleOtherChoice(theItem);
				break;
				
				/*	case 180:
				 HandleVSTChoice(theItem-3, &masterVST[ theItem-3]);
				 break;	*/
				
			case mViewsMenu:
				HandleViewsChoice(theItem);
				break;
				
			case mInstrumentsMenu:
				HandleInstruChoice(theItem);
				break;
				
			case 139:
				HandleExportFile(theItem);
				break;
				
			case 176:
				WindowsSettingsMenu(theItem);
				break;
				
			case 138:
				//	HandleImportFile(theItem);
				break;
				
			case 154:
				HandleNewSound(theItem);
				break;
				
			case 200:
				ShowPrefs(2000 + theItem - 1);
				break;
				
			case mInternetMenu:
				DoInternetMenu(theItem);
				break;
				
			case mHelpMenu:
				HandleHelpChoice(theItem);
				break;
				
			case mWindowMenu:
				HandleWindowChoice(theItem);
				break;
				
		}
		HiliteMenu(0);
	}
}

void HandleAppleChoice(short theItem)
{
	Str255 		accName;
	
	switch(theItem)
	{
		case mAboutPlayerPRO:
			DoAbout();					
			break;
			
		case mGeneralInfo:
			DoHelp();
			break;
			
			/*	case 4:
			 AboutPlugs(); //TODO: revive AboutPlugs()?
			 break;*/
			
		default:
			GetMenuItemText(AppleMenu, theItem, accName);
			break;
	}	
}

void HandleInstruChoice(short theItem)
{
	switch (theItem) {
		case 1:
			if (InstruListDlog != NULL) {
				if (GetDialogWindow(InstruListDlog) == oldWindow) {
					SelectWindow2(NextWindowVisible(GetDialogWindow(InstruListDlog)));
					CheckOneWindow(GetDialogWindow(InstruListDlog));
					ClosePlayerWindow(InstruListDlog);
				} else {
					SelectWindow2(GetDialogWindow(InstruListDlog));
					SetPortDialogPort(InstruListDlog);
				}
			} else
				CreateInstruListWindow();
			break;
			
		case 3:
			
			break;
			
		case 4:
			SaveSample();
			break;
			
		case 5:
			DoKeyPressInstruList(0x08, -1);
			break;
			
		case 7:
			SaveInstrumentsList();
			break;
			
		default:
			Erreur(12, -1);
			break;
	}
}

void HandleViewsChoice(short theItem)
{
	switch(theItem)
	{
#if 0
		case mMemory:
			if (MemoryDlog != NULL)
			{
				if (GetDialogWindow(MemoryDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(MemoryDlog)));
					CheckOneWindow(GetDialogWindow(MemoryDlog));
					ClosePlayerWindow(MemoryDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(MemoryDlog));
					SelectWindow2(GetDialogWindow(MemoryDlog));
				}
			}
			else CreateMemWindow();
			break;
			
		case mMusic:
			if (MODListDlog != NULL)
			{
				if (GetDialogWindow(MODListDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(MODListDlog)));
					CheckOneWindow(GetDialogWindow(MODListDlog));
					ClosePlayerWindow(MODListDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(MODListDlog));
					SelectWindow2(GetDialogWindow(MODListDlog));
					SetItemMark(ViewsMenu, mMusic, checkMark);
				}
			}
			break;
#endif
			
		case mPiano:
			if (PianoDlog != NULL)
			{
				if (GetDialogWindow(PianoDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(PianoDlog)));
					CheckOneWindow(GetDialogWindow(PianoDlog));
					ClosePlayerWindow(PianoDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(PianoDlog));
					SelectWindow2(GetDialogWindow(PianoDlog));
					SetPortDialogPort(PianoDlog);
				}
			}
			else CreatePianoWindow();
			break;
			
		case mOscilloV:
			if (OscilloDlog != NULL)
			{
				if (GetDialogWindow(OscilloDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(OscilloDlog)));
					CheckOneWindow(GetDialogWindow(OscilloDlog));
					ClosePlayerWindow(OscilloDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(OscilloDlog));
					SelectWindow2(GetDialogWindow(OscilloDlog));
					SetPortDialogPort(OscilloDlog);
				}
			}
			else CreateOscilloWindow();
			break;
			
		case mSpectrumV:
			if (SpectrumDlog != NULL)
			{
				if (GetDialogWindow(SpectrumDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(SpectrumDlog)));
					CheckOneWindow(GetDialogWindow(SpectrumDlog));
					ClosePlayerWindow(SpectrumDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(SpectrumDlog));
					SelectWindow2(GetDialogWindow(SpectrumDlog));
					SetPortDialogPort(SpectrumDlog);
				}
			}
			else CreateSpectrumWindow();
			break;
			
		case mAdap:
			if (AdapDlog != NULL)
			{
				if (GetDialogWindow(AdapDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(AdapDlog)));
					CheckOneWindow(GetDialogWindow(AdapDlog));
					ClosePlayerWindow(AdapDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(AdapDlog));
					SelectWindow2(GetDialogWindow(AdapDlog));
					SetPortDialogPort(AdapDlog);
					SetItemMark(ViewsMenu, mAdap, checkMark);
				}
			}
			else CreateAdapWindow();
			break;
			
		case mPatternV:
			if (ClassicDlog != NULL)
			{
				if (GetDialogWindow(ClassicDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(ClassicDlog)));
					CheckOneWindow(GetDialogWindow(ClassicDlog));
					ClosePlayerWindow(ClassicDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(ClassicDlog));
					SetPortDialogPort(ClassicDlog);
					SetItemMark(ViewsMenu, mPatternV, checkMark);
				}
			}
			else CreateClassicWindow();
			break;
			
#if 0	
		case mDigitalV:
			if (DigitalDlog != NULL)
			{
				if (GetDialogWindow(DigitalDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(DigitalDlog)));
					CheckOneWindow(GetDialogWindow(DigitalDlog));
					ClosePlayerWindow(DigitalDlog);
				}
				else
				{
					SelectWindow2( GetDialogWindow(DigitalDlog));
					SetPortDialogPort(DigitalDlog);
				}
			}
			else CreateDigiListWindow();
			break;
#endif
			
		case mEqualizer:
			if (EQDlog != NULL)
			{
				if (GetDialogWindow(EQDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(EQDlog)));
					CheckOneWindow(GetDialogWindow(EQDlog));
					ClosePlayerWindow(EQDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(EQDlog));
					SetPortDialogPort(EQDlog);
				}
			}
			else CreateEQWindow();
			break;
			
#if 0
		case mInsV:
			if (InstruViewDlog != NULL)
			{
				if (GetDialogWindow(InstruViewDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(InstruViewDlog)));
					CheckOneWindow(GetDialogWindow(InstruViewDlog));
					ClosePlayerWindow(InstruViewDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(InstruViewDlog));
					SetPortDialogPort(InstruViewDlog);
				}
			}
			else CreateInstruView();
			break;
			
		case mFullView:
			if (NOWScrollMode) HandleViewsChoice(mScrollT);
			FullView();
			break;
			
		case mScreenSaver:
			if (NOWScrollMode) HandleViewsChoice(mScrollT);
			ScreenSaver();
			break;
#endif
			
		case mTools:
			if (IsWindowVisible(GetDialogWindow(ToolsDlog)))
			{
				ClosePlayerWindow(ToolsDlog);
			}
			else
			{
				ShowWindow(GetDialogWindow(ToolsDlog));
				SelectWindow(GetDialogWindow(ToolsDlog));
				SelectWindow2(oldWindow);
				SetItemMark(ViewsMenu, mTools, checkMark);
			}
			break;
			
#if 0
		case mOnlineHelp:
			if (AHelpDlog == NULL)
			{
				CreateAHelpWindow();
				thePrefs.ActiveHelp = true;
			}
			else
			{
				thePrefs.ActiveHelp = false;
				CloseAHelp();
			}
			break;
			
		case mScrollT:
			if (NOWScrollMode == false)
			{
				if (theDepth != 8)
				{
					Erreur(23, -6);
				}
				else
				{
					NOWScrollMode = true;
					HideBottom();
					ScrollTextInit(true, true);
					SetMenuItemText(ViewsMenu, mScrollT, "\pScrollText OFF");
				}
			}
			else
			{
				NOWScrollMode = false;
				ScrollTextClose(true);
				ShowBottom();
				SetMenuItemText(ViewsMenu, mScrollT, "\pScrollText ON");
			}
			break;
			
		case m3D:
			if (CubeDlog != NULL)
			{
				SelectWindow2(CubeDlog);
				SetPortDialogPort(CubeDlog);
			}
			else CreateCubeWindow();
			break;
			
#endif
			
		default:
			if (theItem - mVisual < maxVisualPlug())
			{
				if (VisualDlog != NULL)
				{
					if (GetDialogWindow(VisualDlog) == oldWindow)
					{
						SelectWindow2(NextWindowVisible(GetDialogWindow(VisualDlog)));
						CheckOneWindow(GetDialogWindow(VisualDlog));
						ClosePlayerWindow(VisualDlog);
					}
					else
					{
						SelectWindow2(GetDialogWindow(VisualDlog));
						SetPortDialogPort(VisualDlog);
					}
				}
			}
			CreateVisualWindow(theItem - mVisual);
			break;
	}	
}

void HandlePatternChoice(short theItem)
{
	switch(theItem)
	{
		case 1:
			if (PartiDlog != NULL)
			{
				if (GetDialogWindow(PartiDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(PartiDlog)));
					CheckOneWindow(GetDialogWindow(PartiDlog));
					ClosePlayerWindow(PartiDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(PartiDlog));
					SetPortDialogPort(PartiDlog);
				}
			}
			else CreatePartiWindow();
			break;
			
		case 2:
			if (PatListDlog != NULL)
			{
				if (GetDialogWindow(PatListDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(PatListDlog)));
					CheckOneWindow(GetDialogWindow(PatListDlog));
					ClosePlayerWindow(PatListDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(PatListDlog));
					SetPortDialogPort(PatListDlog);
				}
			}
			else CreatePatListWindow();
			break;
			
		case 4:	// New
			AddAPattern();
			break;
			
		case 5:	// Load
			LoadAPattern();
			break;
			
		case 6:	// Save
			SaveAPattern();
			break;
			
		case 8:	// Delete
			DeleteAPattern(false);
			break;
			
		case 7:	// Purge
			PurgePattern();
			break;
			
		default:
			Erreur(12, -1);
			break;
	}
}

void HandleOtherChoice(short theItem)
{
	switch(theItem)
	{		
		case 1:
			if (EditorDlog != NULL)
			{
				if (GetDialogWindow(EditorDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(EditorDlog)));
					CheckOneWindow(GetDialogWindow(EditorDlog));
					ClosePlayerWindow(EditorDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(EditorDlog));
					SetPortDialogPort(EditorDlog);
				}
			}
			else CreatePartitionWindow();
			break;
			
		case 2:
			if (MozartDlog != NULL)
			{
				if (GetDialogWindow(MozartDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(MozartDlog)));
					CheckOneWindow(GetDialogWindow(MozartDlog));
					ClosePlayerWindow(MozartDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(MozartDlog));
					SetPortDialogPort(MozartDlog);
				}
			}
			else CreateMozartWindow();
			break;
			
		case 3:	// STAFF Editor
			if (StaffDlog != NULL)
			{
				if (GetDialogWindow(StaffDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(StaffDlog)));
					CheckOneWindow(GetDialogWindow(StaffDlog));
					ClosePlayerWindow(StaffDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(StaffDlog));
					SetPortDialogPort(StaffDlog);
				}
			}
			else CreateStaffWindow();
			break;
			
		case 4:	// WAVE Editor
			if (WaveDlog != NULL)
			{
				if (GetDialogWindow(WaveDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(WaveDlog)));
					CheckOneWindow(GetDialogWindow(WaveDlog));
					ClosePlayerWindow(WaveDlog);
				}
				else
				{
					SelectWindow2(GetDialogWindow(WaveDlog));
					SetPortDialogPort(WaveDlog);
				}
			}
			else CreateWaveWindow();
			break;
			
		case 10:
			SaveUndo(UAllPatterns, 0, "\pUndo 'Change patterns rows to 64'");
			
			DoStop();
			
			ConvertTo64Rows(curMusic);
			
			UPDATE_Total();
			ScanTime();
			UpdateALLWindow();
			break;
			
		case 9:
			if (InfoL(88))
			{
				Str255		theString;
				long 		size;
				GrafPtr		savedPort;
				DialogPtr	aDia;
				short		itemHit;
				
				DoPause();
				
				size = MADMinimize(curMusic);
				
				GetPort(&savedPort);
				
				NumToString(size, theString);
				ParamText(theString, NULL, NULL, NULL);
				
				aDia = GetNewDialog(149, NULL, (WindowPtr) -1L);
				SetPortDialogPort(aDia);
				
				AutoPosition(aDia);
				
				SetCursor(GetQDGlobalsArrow(&qdarrow));
				
				do
				{
					//	ModalDialog(MyDlgFilterDesc, &itemHit);
					
					MyModalDialog(aDia, &itemHit);
					
				}while (itemHit != 1);
				
				DisposeDialog(aDia);
				SetPort(savedPort);
				
				UPDATE_Total();
				
				ResetUndo();
				ScanTime();
				
				UpdateALLWindow();
			}
			break;
			
		case 12:
			DoChangeMODName();
			break;
			
#if 0
		case 9:
			DialogTracks();
			break;
			
		case 10:
			if (MicroPhone)
			{
				MicroOff();
				SetMenuItemText(EditorMenu, theItem, "\pTurn Microphone ON");
			}
			else
			{
				MicroOn();
				SetMenuItemText(EditorMenu, theItem, "\pTurn Microphone OFF");
			}
			break;	
#endif
			
		case 6:
			if (EditorDlog != NULL)
			{
				SelectWindow2(GetDialogWindow(EditorDlog));
				SetPortDialogPort(EditorDlog);
			}
			else CreatePartitionWindow();
			UpdateALLWindow();
			CreateFind();
			break;
			
		case 7:
			if (EditorDlog != NULL)
			{
				SelectWindow2(GetDialogWindow(EditorDlog));
				SetPortDialogPort(EditorDlog);
			}
			else CreatePartitionWindow();
			ReplaceCommand();
			CreateFind();
			break;
			
		default:
			Erreur(12, -1);
			break;
	}	
}

void DeletePreferences()
{
	ResetCFPreferences();
}

static void LoadOldFilePrefs(FSIORefNum fRefNum)
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
		MoveBasicDatatype(FBalls);
		MoveArray(Filter);
		MoveBasicDatatype(FinePositioning);
		MoveArray(FKeyActive);
		MoveArray(FKeyItem);
		MoveBasicDatatype(FKeyTracks);
		MoveBasicDatatype(FrequenceSpeed);
		MoveBasicDatatype(FSinScroll);
		MoveBasicDatatype(FStars);
		MoveBasicDatatype(FText);
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
		MoveBasicDatatype(SSJumping);
		MoveBasicDatatype(SSStars);
		MoveBasicDatatype(SSText);
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

void DoPreferences()
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
	hasASC = false;
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
		
		for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanPan[ i] = 256;
		for (i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanVol[ i] = 64;
		
		for (i = 0; i < 20; i++)
		{
			thePrefs.FKeyActive[ i]		= false;
			thePrefs.FKeyItem[ i] 		= 0;
			thePrefs.FKeyWind[ i]		= 0;
		}
		
		pStrcpy(thePrefs.WinNames[ 0], "\pState 1");
		pStrcpy(thePrefs.WinNames[ 1], "\pState 2");
		pStrcpy(thePrefs.WinNames[ 2], "\pState 3");
		
		for (x = 0; x < 3; x++)
		{
			for (i = 0; i < MAXWINDOWS; i++)
			{
				thePrefs.WinPosO[ x][ i] = thePrefs.WinPos[ i];
				thePrefs.WinEtatO[ x][ i] = thePrefs.WinEtat[ i];
				thePrefs.WinLargO[ x][ i] = thePrefs.WinLarg[ i];
				thePrefs.WinHiO[ x][ i] = thePrefs.WinHi[ i];
				thePrefs.WinIDO[ x][ i] = thePrefs.WinID[ i];
			}
		}
		
		for (i = 0; i < MAXINSTRU; i++) thePrefs.OCArrow[ i] = true;
		
		thePrefs.outPutMode = DeluxeStereoOutPut;
		thePrefs.driverMode = SoundManagerDriver;
		thePrefs.useEQ = false;
		thePrefs.lastVisualPlugin = 0;
		thePrefs.editorSoundDrag = false;
		thePrefs.channelNumber = 2;
		thePrefs.FKeyTracks = true;
		
		for (i = 0; i < EQPACKET*2; i++)	thePrefs.Filter[ i] = 1.0;
		
		for (i = 0; i < MAXTRACK; i++) thePrefs.SelectedTracks[ i] = false;
		
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
	
	if (!HelpAvalaible)
		thePrefs.ActiveHelp = false;
	
	/*for (i = 0; i < 7; i++)
	 {
	 BlockMoveData(&thePrefs.tracksColor[ i * 32], &thePrefs.tracksColor[ 32 + i * 32], 32 * 6);
	 }*/
	
	/////////
}

void HandleEdit(short item)
{
	short		theNo, samp;
	
	switch (item) {
		case 1:		// UNDO
			DoUndo();
			break;
			
		case 3:		// cut
			if (oldWindow == NULL) break;
			
			HandleEdit(4);
			HandleEdit(6);
			break;
			
		case 4:		// copy
			if (oldWindow == NULL)
				break;
			
			switch (GetWRefCon(oldWindow)) {
				case RefMODList:
					COPYMODList();
					break;
					
				case RefStaff:
					COPYStaff();
					break;
					
				case RefSample:
					COPYSampleInt(GetDialogFromWindow(oldWindow));
					break;
					
				case RefQuicktime:
					COPYQuicktime();
					break;
					
				case RefMozart:
					COPYMozart();
					break;
					
				case RefInstruList:
					if (GetIns(&theNo, &samp)) NCOPYSample(0, -1L, theNo, samp);
					else Erreur(13, theNo);
					break;
					
				case RefPartition:
					COPYEditor();
					break;
					
				case RefParti:
					COPYParti();
					break;
					
				case RefPatList:
					COPYPatList();
					break;
			}
			break;
			
		case 5:		// paste
			if (oldWindow == NULL)
				break;
			
			switch(GetWRefCon(oldWindow)) {
				case RefMODList:
					PASTEMODList();
					break;
					
				case RefSample:
					PASTESampleInt(GetDialogFromWindow(oldWindow));
					break;
					
				case RefStaff:
					PASTEStaff();
					break;
					
				case RefMozart:
					PASTEMozart();
					break;
					
				case RefInstruList:
					PASTEInstruList();
					break;
					
				case RefPartition:
					PASTEEditor();
					break;
					
				case RefParti:
					PASTEParti();
					break;
					
				case RefPatList:
					PASTEPatList();
					break;
			}
			break;
			
		case 6:		// clear
			if (oldWindow == NULL)
				break;
			
			GlobalDoKey(oldWindow, 0x08);
			break;
			
		default:
			Erreur(12, -1);
			break;
			
		case 8:		// Duplicate
			if (oldWindow == NULL) break;
			
			GlobalDoKey(oldWindow, duplicate);
			break;
			
		case 9:		// Select All
			if (oldWindow == NULL) break;
			
			GlobalDoKey(oldWindow, selectAll);
			break;
			
		case 10:	// Get Info...
			if (oldWindow == NULL) break;
			
			GlobalDoKey(oldWindow, getinfo);
			break;
			
			// Switch Memory Music
		case 12:
		{
			Boolean changedCopy;
			
			MADGetMusicStatus(MADDriver, &curMusic->fullTime, &curMusic->position);
			
			AlternateBuffer = !AlternateBuffer;
			
			if (AlternateBuffer) {
				SetMenuItemText(EditMenu, item, "\pUse Main Music Buffer");
				SetWTitle(GetDialogWindow(ToolsDlog), "\pSecond Music Buffer");
			} else {
				SetMenuItemText(EditMenu,item, "\pUse Second Music Buffer");
				SetWTitle(GetDialogWindow(ToolsDlog), "\pTools");
				
			}
			
			tempCurMusic = curMusic;
			curMusic = SwitchCurMusic;
			SwitchCurMusic = tempCurMusic;
			
			changedCopy = curMusic->hasChanged;
			
			MADAttachDriverToMusic(MADDriver, curMusic, NULL);
			
			ChangeTracksNo(curMusic->header->numChn);
			SetWindowEnviron();
			
			MODListSelectThisMusic(curMusic->musicFileName);
			SetCurrentMOD(curMusic->musicFileName);
			
			ResetUndo();
			ScanTime();
			
			UPDATE_Total();
			
			UpdateALLWindow();
			
			curMusic->hasChanged = changedCopy;
			
			MADSetMusicStatus(MADDriver, 0, curMusic->fullTime, curMusic->position);
		}
			break;
	}
}

#define kFinderSig	'FNDR'
#define kAEFinderEvents	'FNDR'
#define kSystemType	'MACS'

#define kAEOpenSelection	'sope'
#define keySelection		'fsel'

// Search through the current process list to find the given application. See
// Using the Process Manager for a similar way of doing this.
OSErr FindAProcess(OSType typeToFind, OSType creatorToFind,
				   ProcessSerialNumberPtr processSN)
{
	ProcessInfoRec	tempInfo;
	FSSpec	procSpec;
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
	return(myErr);
}

OSErr OpenSelection(FSSpecPtr theDoc)
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
	if(FindAProcess(kFinderSig,kSystemType,&process))
		return procNotFound;
	myErr = AECreateDesc(typeProcessSerialNumber,(Ptr) &process,
						 sizeof(process), &myAddressDesc);
	if(myErr)	return myErr;
	
	// Create an empty 
	myErr = AECreateAppleEvent (kAEFinderEvents, kAEOpenSelection, &myAddressDesc, kAutoGenerateReturnID, kAnyTransactionID, &aeEvent);
	if(myErr)
		return myErr;
	
	// Make an FSSpec and alias for the parent folder, and an alias for the file
	FSMakeFSSpec(theDoc->vRefNum,theDoc->parID,nil,&dirSpec);
	NewAlias(nil,&dirSpec,&dirAlias);
	NewAlias(nil,theDoc,&fileAlias);
	
	// Create the file list.
	myErr=AECreateList(nil,0,false,&fileList);
	if (myErr) return myErr;
	
	/* Create the folder descriptor
	 */
	HLock((Handle)dirAlias);
	AECreateDesc(typeAlias, (Ptr) *dirAlias, GetHandleSize
				 ((Handle) dirAlias), &aeDirDesc);
	HUnlock((Handle)dirAlias);
	DisposeHandle((Handle)dirAlias);
	
	if((myErr = AEPutParamDesc(&aeEvent,keyDirectObject,&aeDirDesc)) ==
	   noErr)
	{
		AEDisposeDesc(&aeDirDesc);
		HLock((Handle)fileAlias);
		
		AECreateDesc(typeAlias, (Ptr)*fileAlias,
					 GetHandleSize((Handle)fileAlias), &listElem);
		HUnlock((Handle)fileAlias);
		DisposeHandle((Handle)fileAlias);
		myErr = AEPutDesc(&fileList,0,&listElem);
	}
	if(myErr)
		return myErr;
	AEDisposeDesc(&listElem);
	
	myErr = AEPutParamDesc(&aeEvent,keySelection,&fileList);
	if (myErr) return myErr;
	
	myErr = AEDisposeDesc(&fileList);
	
	myErr = AESend(&aeEvent, nil,
				   kAENoReply+kAEAlwaysInteract+kAECanSwitchLayer,
				   kAENormalPriority, kAEDefaultTimeout, nil, nil);
	AEDisposeDesc(&aeEvent);
	
	SetFrontProcess(&process);
	
	return noErr;
}

static	Boolean ResetPatterns = false, ResetInstrus = false, PurgePatterns = false;

void DoReset()
{
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
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	
	GetDialogItem(aDia, 3, &itemType, &itemHandle, &tempRect);	SetControlValue((ControlHandle) itemHandle, ResetPatterns);
	GetDialogItem(aDia, 5, &itemType, &itemHandle, &tempRect);	SetControlValue((ControlHandle) itemHandle, ResetInstrus);
	GetDialogItem(aDia, 6, &itemType, &itemHandle, &tempRect);	SetControlValue((ControlHandle) itemHandle, PurgePatterns);
	
	if (!ResetPatterns && !ResetInstrus && !PurgePatterns)
		ControlSwitch(1, aDia, 255);
	else
		ControlSwitch(1, aDia, 0);
	
	do {
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(aDia, &itemHit);
		
		switch(itemHit) {
			case 3:
			case 5:
			case 6:
				GetDialogItem(aDia, itemHit, &itemType, &itemHandle, &tempRect);
				SetControlValue((ControlHandle) itemHandle, !GetControlValue((ControlHandle) itemHandle));
				
				switch(itemHit) {
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
				if (curMusic->partition[ x] != NULL)
					DisposePtr((Ptr) curMusic->partition[x]);
				curMusic->partition[x] = NULL;
			}
			curMusic->header->numPat = 1;
			
			// Purge Track
			for (tracks = 0; tracks < curMusic->header->numChn; tracks++) {
				for (position = 0; position < curMusic->partition[ 0]->header.size; position++) {
					aCmd = GetMADCommand(position, tracks, curMusic->partition[ 0]);
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
					for (position = 0; position < curMusic->partition[ i]->header.size; position++)
					{
						aCmd = GetMADCommand(position, tracks, curMusic->partition[ i]);
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
	if (DoWindowState)
	{
		CheckShowWindow();
		
		if (thePrefs.ActiveHelp) CreateAHelpWindow();
		ShowWindowPref(-1);
		
		ShowWindow(ToolsDlog);
		SelectWindow(ToolsDlog);
		SelectWindow2(NextWindowVisible(ToolsDlog));
	}
#endif
	
	MADReset(MADDriver);
	
	UPDATE_Total();
	
	ResetUndo();
	ScanTime();
	
	UpdateALLWindow();
}

void HandleFileChoice(short theItem)
{
	WindowPtr			savedWindow;
	
	switch(theItem)
	{
		case 1:
			if (curMusic->hasChanged)
			{
				if (GereChanged() != noErr) return;
			}
			CreateNewMOD();
			PatchSave = false;
			break;
			
		case 2:
			NOpenMusic();
			break;
			
		case 3:
			SaveMOD(false, 'MADK');
			break;
			
		case 4:
			SaveMOD(true, 'MADK');
			break;
			
		case 6:
			// EXPORT FILE
			break;
			
		case 7:
			// RESET
			
			DoReset();
			break;
			
		case 5:
			if (oldWindow == nil) break;
			savedWindow = NextWindowVisible(oldWindow);
			
			CheckOneWindow(oldWindow);
			ClosePlayerWindow(GetDialogFromWindow(oldWindow));
			
			oldWindow = NULL;
			SelectWindow2(savedWindow);
			break;
			
		case 9:
			if (MODListDlog != NULL)
			{
				if (GetDialogWindow(MODListDlog) == oldWindow)
				{
					SelectWindow2(NextWindowVisible(GetDialogWindow(MODListDlog)));
					CheckOneWindow(GetDialogWindow(MODListDlog));
					ClosePlayerWindow(MODListDlog);
				}
				else
				{
					ShowWindow(GetDialogWindow(MODListDlog));
					SelectWindow2(GetDialogWindow(MODListDlog));
					SetItemMark(FileMenu, 9, checkMark);
				}
			}
			break;
			
		case 10:
			SaveMODList();
			break;
			
		case 11:
			if (GereMusicListChanged() == noErr)
			{
				ClearMODList();
				SetMODListWindowName();
			}
			break;
			
		case 13:
			PageSetUp();
			break;
			
		case 14:
			Print();
			break;
			
		case 16:
			ShowPrefs(DRIVER);
			break;
			
		case 17:
			// Windows Settings
			break;
			
			// OMS
			
		case 19:
			OpenMIDIHardware();
#if MACOS9VERSION
			OMSOpenCurrentStudioSetup();
#endif
			break;
			
		case 20:
			OpenMIDIHardware();
#if MACOS9VERSION
			OMSMIDISetupDialog();
#endif
			break;
			
			///
			
		case 18:
		{
			FSSpec specFolder;
			
			if (FindFolder(kOnSystemDisk, kControlPanelFolderType, kDontCreateFolder, &specFolder.vRefNum, &specFolder.parID) == noErr)
			{
				short			i;
				CInfoPBRec		info;
				FSSpec			spec;
				Str255			asc_WorkStr;
				Boolean			sucess = false;
				
				info.hFileInfo.ioNamePtr = asc_WorkStr;
				info.hFileInfo.ioVRefNum = specFolder.vRefNum;
				for (i = 1; 1; i ++)
				{
					info.hFileInfo.ioDirID = specFolder.parID;
					info.hFileInfo.ioFDirIndex = i;
					if (PBGetCatInfoSync(&info) != noErr)
						break;
					
					if (info.hFileInfo.ioFlFndrInfo.fdType == 'cdev' &&
					   info.hFileInfo.ioFlFndrInfo.fdCreator == 'soun')
					{
						
						pStrcpy(spec.name, info.hFileInfo.ioNamePtr);
						spec.vRefNum = info.hFileInfo.ioVRefNum;
						spec.parID = specFolder.parID;
						
						OpenSelection(&spec);
						
						sucess = true;
						
						break;
					}
					
					if (info.hFileInfo.ioFlFndrInfo.fdType == 'APPC' &&
					   info.hFileInfo.ioFlFndrInfo.fdCreator == 'soun')
					{
						
						pStrcpy(spec.name, info.hFileInfo.ioNamePtr);
						spec.vRefNum = info.hFileInfo.ioVRefNum;
						spec.parID = specFolder.parID;
						
						OpenSelection(&spec);
						
						sucess = true;
						
						break;
					}
				}
				
				if (!sucess)
				{
					if (FindFolder(kOnSystemDisk, kControlPanelFolderType, kDontCreateFolder, &specFolder.vRefNum, &specFolder.parID) == noErr)
					{
						short			i;
						CInfoPBRec		info;
						FSSpec			spec;
						Str255			asc_WorkStr;
						Boolean			sucess = false;
						
						info.hFileInfo.ioNamePtr = asc_WorkStr;
						info.hFileInfo.ioVRefNum = specFolder.vRefNum;
						for (i = 1; 1; i ++)
						{
							info.hFileInfo.ioDirID = specFolder.parID;
							info.hFileInfo.ioFDirIndex = i;
							if (PBGetCatInfoSync(&info) != noErr)
								break;
							
							if (info.hFileInfo.ioFlFndrInfo.fdCreator == 'alav')		//info.hFileInfo.ioFlFndrInfo.fdType == 'APPL' &&
							{
								pStrcpy(spec.name, info.hFileInfo.ioNamePtr);
								spec.vRefNum = info.hFileInfo.ioVRefNum;
								spec.parID = specFolder.parID;
								
								OpenSelection(&spec);
								
								sucess = true;
								
								break;
							}
						}
					}
				}
			}
		}
			break;
			
		case 22:
			End = true;
			break;
			
		default:
			Erreur(12, -1);
			break;
	}
}

void HandleExportFile(short theItem)
{
	switch(theItem)
	{
		case 1:
			SaveMOD(true, 'AIFF');
			break;
			
		case 2:
			SaveMOD(true, 'MPG4');
			break;
			
		default:
			if (CallPlug(0)) break;
			SaveMOD(true, GetPPPlugType(gMADLib, theItem - 4, 'EXPL'));
			break;
	}
}

void HandleWindowChoice(short theItem)
{
	//TODO: HandleWindowChoice needs to be written!
}

void HandleHelpChoice(short theItem)
{
	switch (theItem) {
		case mOnlineHelp:
			if (AHelpDlog == NULL)
			{
				CreateAHelpWindow();
				thePrefs.ActiveHelp = true;
			}
			else
			{
				thePrefs.ActiveHelp = false;
				CloseAHelp();
			}
			break;
			
		default:
			break;
	}
	
	//TODO: HandleHelpChoice needs more love!
}


static	Rect		BookmarkRectList;
static	ListHandle	BookmarkList;

void UpdateBookmarks(DialogPtr theDia)
{
	GrafPtr			savePort;
	Rect			itemRect;
	TEHandle		hTE;
	short			itemType;
 	Handle			Text, itemHandle;
	Rect			destRect;
 	StScrpHandle	theStyle;
 	RgnHandle		visibleRegion;
 	
	GetPort(&savePort);
	SetPortDialogPort(theDia);
	
	BeginUpdate(GetDialogWindow(theDia));
	
	ChangeDialogFont(theDia);
	
	DrawDialog(theDia);
	
	oldFrameButton(theDia);
	
	itemRect = BookmarkRectList;
	InsetRect(&itemRect, -1, -1);
	FrameRect(&itemRect);
	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion(GetDialogPort(theDia), visibleRegion);
	
	LUpdate(visibleRegion, BookmarkList);
	
	DisposeRgn(visibleRegion);
	
	GetDialogItem(theDia, 11, &itemType, &itemHandle, &itemRect);
	destRect = itemRect;
	
	hTE = TEStyleNew(&destRect, &itemRect);
	
	Text = GetResource('TEXT', 401);
	DetachResource(Text);
	HLock(Text);
	
	theStyle = (StScrpHandle) GetResource('styl', 401);
	DetachResource((Handle) theStyle);
	
	TESetAlignment(teCenter, hTE);
	TEStyleInsert(*Text, GetHandleSize(Text), theStyle, hTE);
	
	TESetAlignment(teJustCenter, hTE);
	
	ForeColor(whiteColor);
	PaintRect(&itemRect);
	
	ForeColor(blackColor);
	BackColor(whiteColor);
	TEUpdate(&itemRect, hTE);
	RGBBackColor(&theColor);
	
	FrameRectRelief(&itemRect);
	TEDispose(hTE);
	HUnlock(Text);
	MyDisposHandle(& Text);
	MyDisposHandle((Handle*) &theStyle);
	
	EndUpdate(GetDialogWindow(theDia));
	SetPort(savePort);
}


void AddRemoveBookmarksFilter(DialogPtr theDialog, EventRecord *theEventI, short *itemHit)
{
	WindowPtr	whichWindow;
	short		thePart;
	GrafPtr		oldPort;
	Point		aPoint;
	
	GetPort(&oldPort);
	SetPortDialogPort(theDialog);
	
	switch(theEventI->what)
	{
		case updateEvt:
		{
			
		}
			break;
			
		case mouseDown:
			thePart = FindWindow(theEventI->where, &whichWindow);
			
			if (GetWRefCon(whichWindow) == 3885)
			{
				aPoint.v = theEventI->where.v;
				aPoint.h = theEventI->where.h;
				GlobalToLocal(&aPoint);
				if (PtInRect(aPoint, &BookmarkRectList) == true)
				{
					//	TextFont(4);	TextSize(9);
					LClick(aPoint, theEventI->modifiers, BookmarkList);
					//	TextFont(0);	TextSize(0);
				}
			}
			break;
			
		case keyDown:
		case autoKey:
#if 0
			if (GetWRefCon(whichWindow) == 3885)
			{
				TextFont(4);	TextSize(9);
				theChar = theEventI->message & charCodeMask;
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, BookmarkList))
				{
					if (theChar == 0x1E)
					{
						if (theCell.v > 0)
						{
							LSetSelect(false, theCell, BookmarkList);
							theCell.v--;
							LSetSelect(true, theCell, BookmarkList);
							LAutoScroll(PianoList);
						}
					}
					else if (theChar == 0x1F) 
					{
						if (theCell.v < NUMBER_NOTES)
						{
							LSetSelect(false, theCell, BookmarkList);
							theCell.v++;
							LSetSelect(true, theCell, BookmarkList);
							LAutoScroll(PianoList);
						}
					}
					else if (theChar == 0x08) 
					{
						if (theCell.v == NUMBER_NOTES) theCell.v = 0xFF;	// NO NOTE
						
						for (i = 0; i < 299; i++)
						{
							if (thePrefs.PianoKey[ i] == theCell.v) thePrefs.PianoKey[ i] = -1;
						}
						DrawAllBookMarks();
					}
				}
				TextFont(0);	TextSize(0);
			}
#endif
			break;
	}
	
	SetPort(oldPort);
	
	return;
}

void DrawAllBookMarks()
{
	Point	cSize;
	short	i;
	
	/*	theCell.v = 0;
	 theCell.h = 0;
	 LGetSelect(true, &theCell, BookmarkList);*/
	
	cSize.h = cSize.v = 0;
	
	LSetDrawingMode(true, BookmarkList);
	
	/*	LDelRow(0, 0, BookmarkList);
	 
	 LAddRow(URLsNo, 0, BookmarkList);*/
	
	for (i = 0; i < URLsNo; i++)
	{
		LSetCell(URLsDesc[ i] + 1, URLsDesc[ i][ 0], cSize, BookmarkList);
		cSize.v++;
	}
	
	//	for (i = URLsNo; i < (*BookmarkList)->dataBounds.bottom; i++) LDelRow(1, (*BookmarkList)->dataBounds.bottom-1, BookmarkList);
	
	//	LSetSelect(true, theCell, BookmarkList);
	
	LAutoScroll(BookmarkList);
}

static	Str255		cURLs[ MAXURL], cURLsDesc[ MAXURL];

void AddRemoveBookmarks()
{
	long		x;
	short		itemHit,i, itemType;
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	DialogPtr	TheDia;
	GrafPtr		myPort;
	Point		cSize, theCell;
	FontInfo	ThisFontInfo;
	
	long		cURLsNo, lastSelected = -2;
	
	// For Undo
	
	cURLsNo = URLsNo;
	for (i = 0; i < cURLsNo; i++)
	{
		pStrcpy(cURLs[ i], URLs[ i]);
		pStrcpy(cURLsDesc[ i], URLsDesc[ i]);
	}
	
	GetPort(&myPort);
	
	TheDia = GetNewDialog(181,NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	AutoPosition(TheDia);
	ChangeDialogFont(TheDia);
	
	GetDialogItem(TheDia, 9, &itemType, &itemHandle, &BookmarkRectList);
	
	GetFontInfo(&ThisFontInfo);
	cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading;
	
	tempRect = BookmarkRectList;
	tempRect.right = tempRect.right - 15;
	
	if (tempRect.right <= (tempRect.left + 15)) tempRect.right = tempRect.left + 15;
	
	InsetRect(&tempRect, -1, -1);
	FrameRect(&tempRect);
	InsetRect(&tempRect, 1, 1);
	SetRect(&dataBounds, 0, 0, 1, URLsNo);
	cSize.h = (tempRect.right - tempRect.left);
	
	BookmarkList =  LNew(	&tempRect, 
						 &dataBounds,
						 cSize,
						 0,
						 GetDialogWindow(TheDia),
						 true,
						 false,
						 false,
						 true);
	
	(*BookmarkList)->selFlags = lOnlyOne + lNoExtend;
	
	DrawAllBookMarks();
	
	do
	{
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(TheDia, &itemHit);
		
		ChangeDialogFont(TheDia);
		
		switch(itemHit)
		{
			case 12:
				if (InfoL(100) == true)
				{
					FSSpec	spec;
					
					pStrcpy(spec.name, PLAYERBOOK);
					
					spec.vRefNum = mainVRefNum;
					spec.parID = mainParID;
					
					//	HSetVol(NULL, mainVRefNum, mainParID);
					FSpDelete(&spec);
					
					InitBookMarks();
					
					LDelRow(0, 0, BookmarkList);
					LAddRow(URLsNo, 0, BookmarkList);
					
					DrawAllBookMarks();
				}
				break;
				
			case 11:
				DoInternetMenu(3);
				break;
				
			case 3:	// Add
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, BookmarkList))
				{
					//LENVERS!!!
					
					for (i = URLsNo; i >= theCell.v+1; i--)
					{
						pStrcpy(URLs[ i], URLs[ i-1]);
						pStrcpy(URLsDesc[ i], URLsDesc[ i-1]);
					}
					
					//theCell.v++;
				}
				else theCell.v = URLsNo;
				
				GetDText(TheDia, 5, URLsDesc[ theCell.v]);
				GetDText(TheDia, 6, URLs[ theCell.v]);
				
				if (URLsDesc[ theCell.v][ 0] <= 0) pStrcpy(URLsDesc[ theCell.v], "\pYour new URL");
				if (URLs[ theCell.v][ 0] <= 0) pStrcpy(URLs[ theCell.v], "\phttp://");
				
				lastSelected = -2;
				
				LAddRow(1, (*BookmarkList)->dataBounds.bottom-1, BookmarkList);
				
				URLsNo++;
				DrawAllBookMarks();
				
				cSize.v = 0;
				cSize.h = 0;
				if (LGetSelect(true, &cSize, BookmarkList)) LSetSelect(false, cSize, BookmarkList);
				
				LSetSelect(true, theCell, BookmarkList);
				break;
				
				
			case 4:	// Remove
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, BookmarkList))
				{
					for (i = theCell.v; i < URLsNo-1; i++)
					{
						pStrcpy(URLs[ i], URLs[ i+1]);
						pStrcpy(URLsDesc[ i], URLsDesc[ i+1]);
					}
					
					URLsNo--;
				}
				
				lastSelected = -2;
				
				LDelRow(1, (*BookmarkList)->dataBounds.bottom-1, BookmarkList);
				
				DrawAllBookMarks();
				break;
				
			case 10:
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, BookmarkList))
				{
					GetDText(TheDia, 5, URLsDesc[ theCell.v]);
					GetDText(TheDia, 6, URLs[ theCell.v]);
				}
				
				DrawAllBookMarks();
				break;
				
			case -5:
			case nullEvent:
				theCell.v = 0;
				theCell.h = 0;
				if (LGetSelect(true, &theCell, BookmarkList))
				{
					ControlHandle	myCtl;
					
					GetDialogItem(TheDia, 4, &itemType, &itemHandle, &itemRect); myCtl = (ControlHandle) itemHandle;
					if (GetControlHilite(myCtl) != 0) ControlSwitch(4, TheDia, 0);
					
					GetDialogItem(TheDia, 10, &itemType, &itemHandle, &itemRect); myCtl = (ControlHandle) itemHandle;
					if (GetControlHilite(myCtl) != 0) ControlSwitch(10, TheDia, 0);
					
					if (lastSelected != theCell.v)
					{
						lastSelected = theCell.v;
						
						SetDText(TheDia, 5, URLsDesc[ theCell.v]);
						SetDText(TheDia, 6, URLs[ theCell.v]);
						
						SelectDialogItemText(TheDia, 5, 0, 32767);
					}
				}
				else
				{
					ControlHandle	myCtl;
					
					
					
					GetDialogItem(TheDia, 4, &itemType, &itemHandle, &itemRect);	myCtl = (ControlHandle) itemHandle;
					if (GetControlHilite(myCtl) != 255) ControlSwitch(4, TheDia, 255);
					
					GetDialogItem(TheDia, 10, &itemType, &itemHandle, &itemRect);	myCtl = (ControlHandle) itemHandle;
					if (GetControlHilite(myCtl) != 255) ControlSwitch(10, TheDia, 255);
					
					if (lastSelected != -1)
					{
						lastSelected = -1;
						SetDText(TheDia, 5, "\p");
						SetDText(TheDia, 6, "\p");
					}
				}
				break;
		}
		
	} while (itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1) {
		x = CountMenuItems(InternetMenu);
		for (i = 8; i <= x;i++) DeleteMenuItem(InternetMenu, 8);
		
		for (i = 0; i < URLsNo; i++) {
			AppendMenu(InternetMenu, URLsDesc[i]);
		}
	} else {
		URLsNo = cURLsNo;
		for (i = 0; i < URLsNo; i++) {
			pStrcpy(URLs[i], cURLs[i]);
			pStrcpy(URLsDesc[i], cURLsDesc[i]);
		}
	}
	
	DisposeDialog(TheDia);
	SetPort(myPort);
}

void CloseBookMarks()
{
	short		fRefNum;
	ByteCount	inOutBytes;
	long		i;
	OSErr		iErr;
	FSSpec		spec;
	char		achar;
	
	FSMakeFSSpec(mainVRefNum, mainParID, PLAYERBOOK, &spec);
	
	iErr = FSpDelete(&spec);
	iErr = FSpCreate(&spec, 'ttxt', 'TEXT', smSystemScript);
	if (iErr) return;
	
	
	iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
	if (iErr)
		return;
	
	achar = '\r';
	
	for (i = 0; i < URLsNo; i++) {
		inOutBytes = URLs[i][0];
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &URLs[i][1], &inOutBytes);
		
		inOutBytes = 1;
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &achar, &inOutBytes);
		
		inOutBytes = URLsDesc[i][0];
		FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &URLsDesc[ i][ 1], &inOutBytes);
		
		if (i != URLsNo - 1) {
			inOutBytes = 1;
			iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &achar, &inOutBytes);
			
			inOutBytes = 1;
			iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &achar, &inOutBytes);
		}
	}
	
	iErr = FSCloseFork(fRefNum);
}

void InitBookMarks()
{
	short		vRefNum, fRefNum;
	long		dirID, i, eof, prev;
	OSErr		iErr;
	FSSpec		theSpec;
	Ptr			data;
	Handle		Text;
	
	HGetVol(NULL, &vRefNum, &dirID);
	
	//	URLs[ MAXURL], URLsDesc[ MAXURL];
	
	///////// LOAD DEFAULT FILES IN ANY CASE !!!!!
	
	Text = GetResource('TEXT', 400);
	DetachResource(Text);
	HLock(Text);
	
	data = *Text;
	eof = GetHandleSize(Text);
	
	URLsNo = 0;
	for (i = 0; i < eof;)
	{
		//////////////////////
		
	RETRY1:
		
		prev = i;
		while (i < eof && data[ i] != '\r') i++;
		if (i - prev == 0 && i < eof) { i++; goto RETRY1;}
		
		URLs[ URLsNo][ 0] = i - prev;
		BlockMoveData(&data[ prev], &URLs[ URLsNo][ 1], i - prev);
		i++;
		
		//////////////////////
		
	RETRY2:
		
		prev = i;
		while (i < eof && data[ i] != '\r') i++;
		if (i - prev == 0 && i < eof) { i++; goto RETRY2;}
		
		URLsDesc[ URLsNo][ 0] = i - prev;
		BlockMoveData(&data[ prev], &URLsDesc[ URLsNo][ 1], i - prev);
		i++;
		
		//////////////////////
		
		URLsNo++;
	}
	
	HUnlock(Text);
	MyDisposHandle(& Text);
	
	//////////////////////////////////////////////////
	
	FSMakeFSSpec(vRefNum, dirID, PLAYERBOOK, &theSpec);
	
	iErr = FSpOpenDF(&theSpec, fsCurPerm, &fRefNum);
	if (iErr) {
#if 0
		if (iErr == fnfErr)
		{
			iErr = Create(PLAYERPREF, 0, 'ttxt', 'TEXT');
			iErr = FSpOpenDF(PLAYERPREF, 0, &fRefNum);
			if (iErr) return;
			
			Text = GetResource('TEXT', 400);
			DetachResource(Text);
			HLock(Text);
			
			inOutBytes = GetHandleSize(Text);
			iErr = FSWrite(fRefNum, &inOutBytes, *Text);
			
			HUnlock(Text);
			MyDisposHandle(& Text);
			
			iErr = FSCloseFork(fRefNum);
		}
#endif
		
		return;
	}
	{
		SInt64 fsize;
		iErr = FSGetForkSize(fRefNum, &fsize);
		eof = (long)fsize;
	}
	
	data = NewPtrClear(eof);
	if (data == NULL) goto END;
	
	iErr = FSRead(fRefNum, &eof, data);
	
	URLsNo = 0;
	for (i = 0; i < eof;) {
		//////////////////////
		
	RETRY3:
		
		prev = i;
		while (i < eof && data[ i] != '\r')
			i++;
		if (i - prev == 0 && i < eof) {
			i++;
			goto RETRY3;
		}
		
		URLs[URLsNo][0] = i - prev;
		BlockMoveData(&data[prev], &URLs[URLsNo][1], i - prev);
		i++;
		
		//////////////////////
		
	RETRY4:
		
		prev = i;
		while (i < eof && data[ i] != '\r')
			i++;
		if (i - prev == 0 && i < eof) {
			i++;
			goto RETRY4;
		}
		
		URLsDesc[URLsNo][0] = i - prev;
		BlockMoveData(&data[prev], &URLsDesc[URLsNo][1], i - prev);
		i++;
		
		//////////////////////
		
		URLsNo++;
	}
	
	DisposePtr(data);
	
END:
	
	iErr = FSCloseFork(fRefNum);
}

void DoInternetMenu(short theItem)
{
	short 	no;
	Str255	str;
	OSStatus err;
	
	switch(theItem)
	{
		case 1:
		case 2:
		case 3:
		case 4:
			GetIndString(str, 133, theItem);
			
			err = LaunchURLC(str);
			
			if (err == -108) Erreur(98, err);
			else if (err != noErr) Erreur(99, err);
			break;
			
		case 6:
			AddRemoveBookmarks();
			break;
			
		default:
			no = theItem - 8;
			
			err = LaunchURLC(URLs[ no]);
			
			if (err == -108) Erreur(98, err);
			else if (err != noErr) Erreur(99, err);
			break;
	}
}

OSErr ConvertDataToAIFF(FSSpec file, FSSpec *newfile);

void HandleNewSound(short theItem)
{
	short 	ins, samp;
	FSSpec	spec;
	OSErr	iErr;
	
	switch(theItem)
	{
		case 1:
			PPINGetFileName();
			break;
			
		case 2:
			
			if (GetIns(&ins, &samp))
			{
				sData	*curData;
				
				SaveUndo(USample, ins, "\pUndo 'Silence/Tone Generator'");
				
				SetSelectionZero(ins);
				
				if (samp < 0)
				{
					samp = curMusic->fid[ ins].numSamples;
					
					//		curMusic->fid[ ins].numSamples++;
					
					curData = MADCreateSample(curMusic, ins, samp);
					curData->amp = 16;	// Force 16 bits quality !
				}
				else curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
				
				NAppelPlug(ins, samp, ToneGenerator);
				
				if (curData->size == 0)
				{
					MADKillSample(curMusic, ins, samp);
				}
				
				CreateInstruList();
				DrawInfoInstrument();
				UpdateSampleWindows();
				UpdateInstruMenu();
				
				NSelectInstruList(ins, samp);
			}
			break;
			
		case 3:
			RecordSample();
			break;
			
		case 4:
			Quicktime2Converter();
			break;
			
		case 5:
			if (DoStandardOpen(&spec, "\pRAW Data", 'ANYK') == noErr)
			{
				RAWImportFile(&spec);
			}
			break;
			
		case 6:
			ImportAudioCD();
			/*	if (DoStandardOpen(&spec, "\pQuicktime Movie", 'MooV') == noErr)
			 {
			 FSSpec newfile;
			 
			 
			 ConvertDataToAIFF(spec, &newfile);
			 }*/
			break;
			
		case 7:
			if (EditorDlog == NULL)
			{
				Erreur(103, 103);
			}
			else
			{
				FSSpec	newFile;
				short	fRefNum;
				
				if (!GetIns(&ins, &samp))
				{
					Erreur(13, ins);
					return;
				}
				
				pStrcpy(newFile.name, "\pSelectionDigital.AIFF");
				iErr = FindFolder(kOnSystemDisk, kDesktopFolderType, kCreateFolder, &newFile.vRefNum, &newFile.parID);
				if (iErr) MyDebugStr(__LINE__, __FILE__, "FindFolder");
				
				FSpDelete(&newFile);
				
				iErr = FSpCreate(&newFile, 'TVOD', 'AIFF', smSystemScript);
				iErr = FSpOpenDF(&newFile, fsCurPerm, &fRefNum);
				
				if (CreateAIFFExporting(true, fRefNum, &newFile, 'AIFF', NULL))
				{
					while (theProgressDia != NULL) DoAIFFExporting();
					
					/////////////////
					
					curMusic->hasChanged = true;
					
					UpdateALLWindow();
					
					SaveUndo(UAllSamples, 0, "\pUndo 'Create Sample from Digital Selection'");
					
					iErr = NOpenSampleInt(ins, samp, newFile);
					
					if (iErr == -10) MADErreur(iErr);
					
					CreateInstruList();
					DrawInfoInstrument();
					UpdateSampleWindows();
					UpdateInstruMenu();
				}
				SetCursor(GetQDGlobalsArrow(&qdarrow));
				
				FSpDelete(&newFile);
			}
			break;
	}
}
