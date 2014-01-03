#include "Shuddup.h"
#include <Carbon/Carbon.h>

extern 	Boolean 	End;
extern	DialogPtr	MODListDlog;

enum {
	kPlayerPROClass			= 'SNPL',
	kPlayerPROSignature		= 'SNPL',
	
	kErreurID				= 'AERR',
	kPlayMusicID			= 'PLAY',
	kStopMusicID			= 'STOP',
	
	kSetSpeedID				= 'Sped',
	kSetVolumeID			= 'Volu',
	kSetPitchID				= 'Pitc',
	
	kGotoPatternID			= 'GPat',
	kGotoPositionID			= 'GPos',
	kGotoPercentID			= 'GPer'
	
};

static	AEEventHandlerUPP	AEODocDesc,
							AEQAppDesc,
							AEPlayMusicDesc,
							AEStopMusicDesc,
							AESetSpeedDesc,
							AESetVolumeDesc,
							AESetPitchDesc,
							AEGotoPatternDesc,
							AEGotoPositionDesc,
							AEGotoPercentDesc,
							AEErreurDesc;


/********************************/

pascal OSErr	AEODoc(const AppleEvent *theAppleEvent, AppleEvent* reply, long handlerRefCon);
pascal OSErr	AEQDoc(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
static pascal OSErr  	ANEQApp (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);

pascal OSErr	AEPlayMusic(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
pascal OSErr	AEStopMusic(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
pascal OSErr	AESetVolume(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
pascal OSErr	AESetSpeed( const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
pascal OSErr	AESetDriver(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
OSErr MyGotRequiredParams (const AppleEvent *theAppleEvent);

/********************************/

void IntErreur(short ID, OSErr theErr);
Boolean OpenFirst2(short pos);
void MODListSelectThisMusic(Str255 str);
OSType 		Ptr2OSType(Ptr str);

extern short				DropZone;
extern ProcessSerialNumber	playerPROPSN;
extern Boolean				ReceivedAMusicInMusicList;

pascal OSErr  AEODoc (const AppleEvent *theAppleEvent, AppleEvent* reply, long handlerRefCon)
{
	FSSpec		myFSS;
	AEDescList	docList;
	OSErr		err;
	long		index, itemsInList;
	Size		actualSize;
	AEKeyword	keywd;
	DescType	returnedType;
	Point		theCell;
	FInfo		fndrInfo;
	CInfoPBRec	info;
	Str255		aStr;
	
	// get the direct parameter--a descriptor list--and put it into a docList
	
	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList, &docList);
	if (err) return err;

	// check for missing parameters
	err = MyGotRequiredParams (theAppleEvent);
	if (err) return err;

	// count the number of descriptor records in the list
	err = AECountItems (&docList, &itemsInList);

	// now get each descriptor record from the list, coerce the returned
	// data to an FSSpec record, and open the associated file
	
	SetCursor(&watchCrsr);
	
	UpdateALLWindow();
	
	for (index = 1; index <= itemsInList; index++)
	{
		err = AEGetNthPtr (&docList, index, typeFSS, &keywd, &returnedType, (Ptr) &myFSS, sizeof(myFSS), &actualSize);
		
		err = HGetFInfo(myFSS.vRefNum, myFSS.parID, myFSS.name, &fndrInfo);
		
		if (err != noErr)
		{
			info.dirInfo.ioVRefNum = myFSS.vRefNum;
			info.dirInfo.ioDrDirID = myFSS.parID;
			info.dirInfo.ioFDirIndex = -1;
			info.dirInfo.ioNamePtr = myFSS.name;
			info.hFileInfo.ioNamePtr = myFSS.name;
			info.hFileInfo.ioDirID = myFSS.parID;
			info.hFileInfo.ioFDirIndex = 0;
			
			if (PBGetCatInfo(&info, false) == noErr)
			{
				DoScanDir(info.dirInfo.ioDrDirID, myFSS.vRefNum);
				
				if (index == 1)
				{
					Boolean 	rrr;
					
					if (thePrefs.AutomaticOpen)
					{
						if (DropZone >= 0) rrr = OpenFirst2(DropZone);
						else rrr = OpenFirst2(0);
						
						DropZone = -1;
						
						if (thePrefs.AutoPlayWhenOpen && rrr) DoPlay();		// WANT TO PLAY ?
					}
				}
			}
		}
		else
		{
			OSType 	tempType;
			char	tempC[ 5];
			
			if (OpenableFile(fndrInfo.fdType, &myFSS) == true ||
				fndrInfo.fdType == 'sTAT' ||
				fndrInfo.fdType == 'STCf' ||
				MADMusicIdentifyFSp(gMADLib, tempC, &myFSS) == noErr)
			{
				if (fndrInfo.fdType != 'sTAT' && fndrInfo.fdType != 'STCf')
				{
					AddMODList(	false,
									myFSS.name,
									myFSS.vRefNum,
									myFSS.parID);
					
					if (!ReceivedAMusicInMusicList)
					{
						if (index == 1)
						{
							if (ImportFile(myFSS.name, myFSS.vRefNum, myFSS.parID, fndrInfo.fdType))
							{
								if (thePrefs.AutoPlayWhenOpen) DoPlay();		// WANT TO PLAY ?
							}
						}
					}
					else
					{
						if (index == 1) MODListSelectThisMusic(myFSS.name);
					}
				}
				else	// Music File List
				{
					if (GereMusicListChanged() == noErr)
					{
						ClearMODList();
						
						if (index == 1)
						{
							if (ImportFile(myFSS.name, myFSS.vRefNum, myFSS.parID, fndrInfo.fdType))
							{
								if (thePrefs.AutoPlayWhenOpen) DoPlay();		// WANT TO PLAY ?
							}
						}
					}
				}
			}
		}
	}
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	UpdateALLWindow();
	
	err = AEDisposeDesc (&docList);
	
	ReceivedAMusicInMusicList = false;
	
	return noErr;
}

static pascal OSErr  ANEQApp (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon)
{
	AEDescList	docList;
	OSErr		err;
	long		itemsInList;

	End = true;
/*	// get the direct parameter--a descriptor list--and put it into a docList
	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList, &docList);
	if (err) return err;

	// check for missing parameters
	err = MyGotRequiredParams (theAppleEvent);
	if (err) return err;

	// count the number of descriptor records in the list
	err = AECountItems (&docList, &itemsInList);


	err = AEDisposeDesc (&docList);*/
	return noErr;
}

pascal OSErr  AEErreur(const AppleEvent *theAppleEvent, AppleEvent *reply, long refcon)
{
	AEDescList				docList;
	OSErr					err;
	long					index, itemsInList;
	Size					actualSize;
	AEKeyword				keywd;
	DescType				returnedType;
	OSErr					dErr[ 2];
	
	NMRec					myNotification;
	Boolean					notif;
	ProcessSerialNumber		PSN;
	Handle					hd;
		

	// get the direct parameter--a descriptor list--and put it into a docList
	
	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList, &docList);
	if (err) return err;
	
	// check for missing parameters
	err = MyGotRequiredParams (theAppleEvent);
	if (err) return err;
	
	// count the number of descriptor records in the list
	err = AECountItems(&docList, &itemsInList);
	
	// now get each descriptor record from the list, coerce the returned
	// data to an FSSpec record, and open the associated file
	
	err = AEGetNthPtr (&docList, 1, 'dErr', &keywd, &returnedType, (Ptr) &dErr, sizeof(dErr), &actualSize);
		
	GetFrontProcess(&PSN);
	if (PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN)
	{
		notif = true;
		
		myNotification.qType	= nmType;			//set queue type
		myNotification.nmMark	= 1;				//put mark in Application menu
		
		err = GetIconSuite(&hd, 128, svAllSmallData);
		myNotification.nmIcon	= hd;				//alternating icon
		myNotification.nmSound	= (Handle) -1;		//play system alert
		
		myNotification.nmStr	= NULL;				//display alert box
		myNotification.nmResp	= nil;				//no response procedure
		myNotification.nmRefCon	= 0;				//not needed
		
		err = NMInstall(&myNotification);
		if (err) notif = false;
	}
	else notif = false;
	
	IntErreur(dErr[ 0], dErr[ 1]);
	
	if (notif)
	{
		NMRemove(&myNotification);
		err = DisposeIconSuite(hd, true);
	}
	err = AEDisposeDesc (&docList);
	
	return noErr;
}

pascal OSErr  AEStopMusic(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	DoPause();
	return noErr;
}

pascal OSErr  AEPlayMusic(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	DoPlay();
	return noErr;
}

pascal OSErr  AENextPattern(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	DoSearchUp();
	return noErr;
}

pascal OSErr  AEPreviousPattern(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	DoSearchDown();
	return noErr;
}

pascal OSErr  AESetSpeed(const AppleEvent *event, AppleEvent *reply, long refcon)
{
OSErr		err;
DescType	actualtype;
Size		actualSize;
short		AESpeed;

	err = AEGetParamPtr(	event, keyDirectObject, typeChar,
							&actualtype, (Ptr)(&AESpeed), 2, &actualSize);

	if (AESpeed > 0 && AESpeed < 20)
	{
	//	speed = AESpeed;
	//	ChangeSpeed();
	}
	return noErr;
}

pascal OSErr  AESetPitch(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	return noErr;
}

pascal OSErr  AESetVolume(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	return noErr;
}

pascal OSErr  AEGotoPattern(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	return noErr;
}

pascal OSErr  AEGotoPosition(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	return noErr;
}

pascal OSErr AEGotoPercent(const AppleEvent *event, AppleEvent *reply, long refcon)
{
	return noErr;
}

OSErr MyGotRequiredParams (const AppleEvent *theAppleEvent)
{
	DescType	returnedType;
	Size	actualSize;
	OSErr	err;

	err = AEGetAttributePtr (theAppleEvent, keyMissedKeywordAttr,
									typeWildCard, &returnedType, nil, 0,
									&actualSize);
	if (err == errAEDescNotFound)	// you got all the required parameters
			return noErr;
	else if (!err)			// you missed a required parameter
			return errAEEventNotHandled;
	else					// the call to AEGetAttributePtr failed
			return err;
}

Boolean AppleEventsInstalled (void)
{
	return true;
}

short InstallAE(void)
{
	Boolean		aEvents;
	OSErr		err;

	aEvents = AppleEventsInstalled();
	//appleevents.h
	if (aEvents)
	{
			AEODocDesc 			= NewAEEventHandlerUPP(AEODoc);
			AEQAppDesc 			= NewAEEventHandlerUPP(ANEQApp);
			AEPlayMusicDesc		= NewAEEventHandlerUPP(AEPlayMusic);
			AEStopMusicDesc		= NewAEEventHandlerUPP(AEStopMusic);
			AESetSpeedDesc		= NewAEEventHandlerUPP(AESetSpeed);
			AESetVolumeDesc		= NewAEEventHandlerUPP(AESetVolume);
			AESetPitchDesc		= NewAEEventHandlerUPP(AESetPitch);
			AEGotoPatternDesc	= NewAEEventHandlerUPP(AEGotoPattern);
			AEGotoPositionDesc	= NewAEEventHandlerUPP(AEGotoPosition);
			AEGotoPercentDesc	= NewAEEventHandlerUPP(AEGotoPercent);
			AEErreurDesc		= NewAEEventHandlerUPP(AEErreur);
			
			err = AEInstallEventHandler (kCoreEventClass, kAEOpenDocuments, AEODocDesc, 0, false);
			err = AEInstallEventHandler (kCoreEventClass, kAEQuitApplication, AEQAppDesc, 0, false);
			
			err = AEInstallEventHandler (kPlayerPROClass, kErreurID			, AEErreurDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kPlayMusicID		, AEPlayMusicDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kStopMusicID		, AEStopMusicDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kSetSpeedID		, AESetSpeedDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kSetVolumeID		, AESetVolumeDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kSetPitchID		, AESetPitchDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kGotoPatternID	, AEGotoPatternDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kGotoPositionID	, AEGotoPositionDesc, 0, false);
			err = AEInstallEventHandler (kPlayerPROClass, kGotoPercentID	, AEGotoPercentDesc, 0, false);
	}
	else
	{
		 Erreur(33, -1);
		 ExitToShell();
	}
	return 0;
}

OSStatus LaunchURLC(ConstStr255Param urlStr)
{
    OSStatus err;
    ICInstance inst;
    long startSel;
    long endSel;

    err = ICStart(&inst, 'SNPL');           // Use your creator code if you have one!
    if (err == noErr)
    {
            if (err == noErr) {
                startSel = 0;
                endSel = urlStr[0];
                err = ICLaunchURL(inst, "\p", (char *) &urlStr[1], urlStr[0], &startSel, &endSel);
            }
    //    #endif
        (void) ICStop(inst);
    }
    return (err);
}


void AESendOpenFile(FSSpec *spec)
{
	//FIXME: Instruments seems to think that this function leaks quite a bit
	OSType				sign = 'SNPL';
	AppleEvent			aeEvent, replyAE;
	AEDesc				target, listElem, fileList;
	OSErr				iErr;

	iErr = AECreateDesc(	typeProcessSerialNumber,
							(Ptr) &playerPROPSN,
							sizeof(playerPROPSN),
							&target);
	
	iErr = AECreateAppleEvent(	kCoreEventClass,
								kAEOpenDocuments,
								&target,
								kAutoGenerateReturnID,
								kAnyTransactionID,
								&aeEvent);
	
	iErr = AECreateList(NULL, 0, false, &fileList);

	AECreateDesc(typeFSS, (Ptr) spec, sizeof(*spec), &listElem);
	
	iErr = AEPutDesc(&fileList, 0, &listElem);
	if (iErr) return;
	
	AEDisposeDesc(&listElem);
	
	iErr = AEPutParamDesc(&aeEvent, keyDirectObject, &fileList);
	if(iErr) return;
	iErr = AEDisposeDesc(&fileList);
	
	iErr = AESend(	&aeEvent,
					&replyAE,
					kAENoReply,
					kAEHighPriority,
					kAEDefaultTimeout,
					NULL,
					NULL);
	
	iErr = AEDisposeDesc(&target);

	if (iErr) return;
	
}
