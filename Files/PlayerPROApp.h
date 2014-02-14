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
#include <vector>

typedef struct CrsrRec {
	short total;
	short current;
	short no[30];
} CrsrRec;

namespace PlayerPRO {
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
		Cursor		watchCrsr, qdarrow;
		Cursor		PlayCrsr, HandCrsr, CHandCrsr, ZoomInCrsr, ZoomOutCrsr;
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

	private:
		Str255 asc_WorkStr;
		long	previousTime;
		CrsrRec	MyCrsrRec;
		Cursor	*myCursor;
		std::vector<wds_general*> *generalWindows;
	};
	PlayerPROApp *TheApp;
}

extern "C" void pstrinsert(StringPtr dst, StringPtr src);
extern "C" void pstrcat(StringPtr dst, StringPtr src);

#endif

