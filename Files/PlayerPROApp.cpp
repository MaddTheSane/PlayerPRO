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

