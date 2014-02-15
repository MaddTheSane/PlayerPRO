/*
 *  PlayerPROApp.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROAPP_H__
#define __PLAYERPROAPP_H__

#include <Carbon/Carbon.h>
#include "RDriver.h"
extern "C" {
#include "Shuddup.h"
}
#include <string>
#include "wds_menu.h"
#include "wds_editor.h"
#include "wds_general.h"
#include "Tools.h"
#include <vector>

typedef struct CrsrRec {
	short total;
	short current;
	short no[30];
} CrsrRec;


namespace PlayerPRO {
	enum CursorNumber {
		HelpCrsr = 0,
		NoteCrsr,
		DelCrsr,
		PlayCrsr,
		HandCrsr,
		beamCrsr,
		pencilCrsr,
		pencilCrsrStereo,
		CHandCrsr,
		ContextCrsr,
		watchCrsr,
		ZoomInCrsr,
		ZoomOutCrsr,
		TotalCursors
	};
	
	enum {
		Find = 0,
		Next,
		ReplaceFind,
		ReplaceAll
	};
	
	enum {
		allMusics = 1,
		allReadable = 2,
		allFiles = 3,
		MADK = 5,
		others = 7
	};
	
	class PlayerPROApp {
	public:
		PlayerPROApp();
		~PlayerPROApp();
		
		KeyMap				km;
		RGBColor			theColor;
		ModalFilterUPP		MyDlgFilterDesc;
		ListClickLoopUPP	mylClickLoopDesc;
		NavEventUPP			MyDlgFilterNavDesc;
		
		bool		gUseControlSize, MusicPlayActive, UseAEErreur;
		short		gScrollBarID, curvRefNum;
		long		curparID, StartTime;
		bool		Micro, End, Record;
		DialogPtr	ToolsDlog;
		Str63		lastLoadMODListName;
		bool		DragManagerUse;
		
		bool		mainSystemDrag, Direct, Stereo, StereoMixing, NewSoundManager, NewSoundManager31, Audio16;
		short		theDepth, RowBytes;
		Prefs		thePrefs;
		MenuHandle 	InternetMenu, AppleMenu, NoteMenu, InstruMenu, EffectMenu, TracksNumberMenu, BaseNote, InstruEditMenu, PatternEditMenu;
		MenuHandle	ExportMenu, EditorMenu, FileMenu, VSTMenu, ViewsMenu, EditMenu, WindowMenu, HelpMenu;
		EventRecord	theEvent;
		WindowPtr	oldWindow;
		
		std::string ENote(int idx);
		std::string EInstru(int idx);
		std::string EArgu(int idx);
		std::string EEffect(int idx);
		
		MADDriverRec		*MADDriver;
		MADMusic			*curMusic;
		MADLibrary			*gMADLib;
		const EventTypeSpec	*CarbonWinList();
		
		//void RollCursor();
		static Boolean QTTypeConversion(OSType fileType);
		static Boolean CheckFileType(FSSpec theSpec, OSType theType);
		
		static OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
		static OSErr GetApplicationResourceFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
		
		static void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName);
		void ScanDir(long dirID, short VRefNum, Boolean recurse);
		void DoScanDir(long DirID, short VRefNum);
		
		void Run();
		
		void		SetPortFromOldWindow();
		CGrafPtr	GetPortFromOldWindow();
		DialogPtr	GetDialogFromOldWindow();
		
		void InitRollCrsr();
		void RollCursor();
		void CloseRollCrsrc();
		static void GetSizeString(long size, Str255 str, Boolean Convert);
		
		void SetCursorToQDArrow();
		ProcessSerialNumber GetPlayerPROPSN();
		void SetCursorOnNumber(short theNum);
		static void GetMDEFRect(Rect *aRect, Rect *menuRect, short whichItem);
		void DoGlobalNull();
		void SKVolume(short vol);
		void DoOSEvent(EventRecord *event, bool ModalCall);
		short ConvertCharToNote(char theChar);
		void EventLoop2();
		
		static void UseSameLeft(WindowPtr whichWindow);
		static bool PtRect(Point thePoint, Rect *theRect);
		void DrawVersion();
		
		short TouchMem[11];
		short TrackMem[11];
		short TouchIn;
		short showWhat;

		static OSErr FindAProcess(OSType typeToFind, OSType creatorToFind, ProcessSerialNumberPtr processSN);
		OSErr OpenSelection(FSSpecPtr theDoc);
		ConstStr255Param GetVersionString();
		
		void StartDialog();
		void EndDialog();
		
		static OSErr NGetFileName(FSSpec *spec);
		void InitImportMenu(void);
		short IntInfoL(short ID);
		short InfoL(short ID);
		void MADErreur(OSErr err);
		void IntErreur(short ID, OSErr theErr);
		void OtherIntErreur(short ID, OSErr theErr, Str255 otherstr);
		void Erreur(short ID, OSErr theErr);
		
		void DeletePreferences();
		void DoPreferences();
		
		void DoReset();
		Tools *ToolsWindow;
		
	private:
		Str255 asc_WorkStr;
		long	previousTime;
		CrsrRec	MyCrsrRec;
		Cursor	*myCursor;
		std::vector<wds_general*> *generalWindows;
		MADMusic	*SwitchCurMusic, *tempCurMusic;
		Boolean		AlternateBuffer;
		Cursor	cursors[TotalCursors];
		Cursor	qdarrow;

		short	mainVRefNum, theOldDepth;
		long	mainParID;
		
		DialogPtr			myStartUpDlog;
		Str255				versString;
		RgnHandle			gCursorRgn;
		ProcessSerialNumber	playerPROPSN;
		void LoadOldFilePrefs(FSIORefNum fRefNum);
	};
}

extern PlayerPRO::PlayerPROApp *TheApp;

extern "C" void pstrinsert(StringPtr dst, StringPtr src);
extern "C" void pstrcat(StringPtr dst, StringPtr src);

#endif

