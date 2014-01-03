#include <Carbon/Carbon.h>
#include "MAD.h"
#include "RDriver.h"

extern 	Boolean 	OnTourne;
extern	DialogPtr	myDialog;

OSErr MyGotRequiredParams (AppleEvent *theAppleEvent);
pascal OSErr  MyHandleODoc (const AppleEvent *theAppleEvent, AppleEvent* reply, long handlerRefCon);
pascal OSErr  MyHandleQDoc (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
pascal OSErr  MyHandleQApp (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon);
/********************************/
/********************************/

extern MADDriverRec	*MADDriver;
extern MADMusic		*music;
extern MADLibrary	*MADLib;


void OSType2Str( OSType type, Str255 str)
{
	short i;

	str[ 0] = 4;
	BlockMoveData( &type, str+1, 4);

	for( i = 4; i > 0; i--)
	{
		if (str[ i] == ' ') str[ 0]--;
		else return;
	}
}

void OSType2Ptr( OSType type, Ptr str)
{
	BlockMoveData( &type, str, 4);
	str[ 4] = 0;
}

Boolean AppleEventsInstalled (void)
{
	OSErr err;
	long  result;

	err = Gestalt (gestaltAppleEventsAttr, &result);
	return (!err && ((result >> gestaltAppleEventsPresent) & 0x0001));
}

pascal OSErr  MyHandleQApp (const AppleEvent *theAppleEvent, AppleEvent *reply, long
														handlerRefCon)
{
	AEDescList	docList;
	OSErr		err;
	long		itemsInList;

	OnTourne = false;
	
	// get the direct parameter--a descriptor list--and put it into a docList
	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList,
									&docList);
	if (err) return err;

	// check for missing parameters
	err = MyGotRequiredParams (theAppleEvent);
	if (err) return err;

	// count the number of descriptor records in the list
	err = AECountItems (&docList, &itemsInList);

	err = AEDisposeDesc (&docList);
	return noErr;
}

OSErr MyGotRequiredParams (AppleEvent *theAppleEvent)
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

void ReadAndPlayMusic( Str255	myFile, OSType	fdType)
{
	char str[ 5];
	
	OSType2Ptr( fdType, str);
	
	if (!MADPlugAvailable( MADLib, str)) return;

	MADStopDriver( MADDriver);
	MADDisposeMusic( &music, MADDriver);

	if (MADPlugAvailable( MADLib, str))		// Is available a plug to open this file?
	{
		MADLoadMusicFilePString( MADLib, &music, str, myFile);
	}
	
	SetWTitle( GetDialogWindow(myDialog), myFile);

	MADStartDriver( MADDriver);
	MADDriver->Reading = true;
}

pascal OSErr MyHandleODoc (const AppleEvent *theAppleEvent, AppleEvent* reply, long
														handlerRefCon)
{
	FSSpec			myFSS;
	AEDescList		docList;
	OSErr			err;
	long			index, itemsInList;
	Size			actualSize;
	AEKeyword		keywd;
	DescType		returnedType;
	FInfo			fndrInfo;
	
	// get the direct parameter--a descriptor list--and put it into a docList
	
	err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList,
			&docList);
	if (err)
			return err;

	// check for missing parameters
	err = MyGotRequiredParams (theAppleEvent);
	if (err)
			return err;

	// count the number of descriptor records in the list
	err = AECountItems (&docList, &itemsInList);

	// now get each descriptor record from the list, coerce the returned
	// data to an FSSpec record, and open the associated file
	
	for (index = 1; index <= itemsInList; index++)
	{
		err = AEGetNthPtr (&docList, index, typeFSS, &keywd,
						&returnedType, (Ptr) &myFSS, sizeof(myFSS), &actualSize);
		
		err = HGetFInfo( myFSS.vRefNum, myFSS.parID, myFSS.name, &fndrInfo);
		
		err = HSetVol( 0L, myFSS.vRefNum, myFSS.parID);

		if (err == noErr)
		{
			ReadAndPlayMusic( myFSS.name, fndrInfo.fdType);
		}
	}
	
	err = AEDisposeDesc (&docList);
	return noErr;
}

short InstallAE(void)
{
	Boolean		aEvents;
	OSErr		err;

	aEvents = AppleEventsInstalled();
	
	if (aEvents)
	{
		err = AEInstallEventHandler (kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerUPP( MyHandleODoc), 0, false);
		err = AEInstallEventHandler (kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP( MyHandleQApp), 0, false);
	}

	return 0;
}
