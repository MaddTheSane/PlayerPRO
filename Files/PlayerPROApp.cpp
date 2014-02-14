/*
 *  PlayerPROApp.cpp
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayerPROApp.h"

void PlayerPRO::PlayerPROApp::Run()
{

}


Boolean PlayerPRO::PlayerPROApp::QTTypeConversion(OSType fileType)
{
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
			MyP2CStr(theSpec.name);
			if (CheckMADFile((Ptr) theSpec.name) == noErr)
				Response = true;
			else
				Response = false;
			MyC2PStr((Ptr)theSpec.name);
			break;
			
		default:
			MyP2CStr(theSpec.name);
			OSType2Ptr(theType, tempC);
			err = PPTestFile(gMADLib, tempC, (Ptr)theSpec.name);
			MyC2PStr((Ptr)theSpec.name);
			
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
	
	if (remonte) return;
	
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


