/********************						***********************/
//
//	Player PRO 5.0 - PLAYERPRO CODE -
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <QuickTime/QuickTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include "PPPrivate.h"
#include "Navigation.h"

// the requested dimensions for our sample open customization area:
#define kCustomWidth 410
#define kCustomHeight 20
#define kControlListID 149

enum {
	allMusics = 1,
	allReadable,
	allFiles,
	MADK = allFiles + 2,
	others = MADK + 2
};

extern Cursor PlayCrsr;

// customization globals:
static	Handle 		gDitlList = NULL;
static	short 		gLastTryWidth;
static	short 		gLastTryHeight;
static	UInt16 		gfirstItem = 0;

static Boolean 		needUp, AddAll;
static MenuHandle	showWhatMenu;
static OSType		plugListO[MAXPLUG + others];
static OSType		specificType;
extern short		showWhat;
static MADMusic		*previewPartition = NULL;
static Boolean		gEraseAdd, gEraseAddCurrent, gUpdateCurrentFile;

Boolean QTTestConversion(FSSpec *file, OSType fileType);

CFArrayRef CopySupportedMusicUTIs();

OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize)
{
	Handle		h;
	ByteCount	dataSize;
	ByteCount	acSize;
	
	if (typeCode)
		*typeCode = desc->descriptorType;
	
	h = (Handle)desc->dataHandle;
	dataSize = GetHandleSize(h);
	if (dataSize > maximumSize)
		acSize = maximumSize;
	else
		acSize = dataSize;
	BlockMoveData(*h, dataBuffer, acSize);
	
	if (actualSize)
		*actualSize = acSize;
	
	return noErr;
}

#if 0
static OSErr ExtractFiles(NavCBRecPtr callBackParms, FSSpecArrayPtr *finalSpec, long *totCount)
{
	OSErr			theErr = -1;
	AEDescList 		selectionList;
	
	if (!finalSpec) {
		return MADParametersErr;
	}
	
	if (NavCustomControl(callBackParms->context, kNavCtlGetSelection, &selectionList) == noErr) {
		long		count = 0;
		long		index;
		
		if (AECountItems(&selectionList, &count) == noErr) {
			*finalSpec = (FSSpecArrayPtr)NewPtr(count * sizeof(FSSpec));
			if (totCount) {
				*totCount = count;
			}
			for (index = 1; index <= count; index++) {
				AEDesc theItemDesc;
				AEKeyword keyword;
				
				if (AEGetNthDesc(&selectionList, index, typeFSS, &keyword, &theItemDesc) == noErr) {
					// put something in there:
					if (MyAEGetDescData(&theItemDesc, NULL, &(*finalSpec)[index], sizeof(FSSpec), NULL) == noErr) {
						theErr = noErr;
					}
					AEDisposeDesc(&theItemDesc);
				}
			}
		}
		AEDisposeDesc(&selectionList);
	}
	
	return theErr;
}
#endif


OSErr ExtractFile(NavCBRecPtr callBackParms, FSSpec	*finalSpec)
{
	OSErr			theErr = -1;
	AEDescList 		selectionList;
	
	if (NavCustomControl(callBackParms->context, kNavCtlGetSelection, &selectionList) == noErr) {
		long		count = 0;
		long		index;
		
		if (AECountItems(&selectionList, &count) == noErr) {
			//	for (index=1; index<=count; index++ )
			index = 1;
			{
				AEDesc theItemDesc;
				AEKeyword keyword;
				
				if (AEGetNthDesc(&selectionList, index, typeFSS, &keyword, &theItemDesc) == noErr) {
					// put something in there:
					if (MyAEGetDescData(&theItemDesc, NULL, finalSpec, sizeof(FSSpec), NULL) == noErr) {
						theErr = noErr;
					}
					AEDisposeDesc(&theItemDesc);
				}
			}
		}
		AEDisposeDesc(&selectionList);
	}
	
	return theErr;
}

// *****************************************************************************
// *
// *	HandleCommandPopup()
// *	
// *****************************************************************************
void HandleCommandPopup(ControlHandle thePopup, NavCBRecPtr callBackParms)
{
	OSErr 	theErr = noErr;
	short 	selection = 0;
	
	selection = GetControlValue(thePopup)-1;
	switch (selection) {
		case kNavCtlShowDesktop:
		case kNavCtlScrollHome:
		case kNavCtlScrollEnd:
		case kNavCtlPageUp:
		case kNavCtlPageDown:
			theErr = NavCustomControl(callBackParms->context,selection,NULL);
			break;
			
		case kNavCtlShowSelection:
			theErr = NavCustomControl(callBackParms->context,kNavCtlShowSelection,NULL);
			break;
			
		case kNavCtlOpenSelection:
		{
			AEDesc itemToOpen;
			theErr = NavCustomControl(callBackParms->context,kNavCtlOpenSelection,&itemToOpen);
			if (itemToOpen.descriptorType == typeFSS)
			{
				// you may open the file as described by 'itemToOpen', or do whatever you want with it:
				AEDisposeDesc(&itemToOpen);
			}
			break;
		}
			
		case kNavCtlCancel:
			theErr = NavCustomControl(callBackParms->context,kNavCtlCancel,NULL);
			break;
			
		case kNavCtlAccept:
			theErr = NavCustomControl(callBackParms->context,kNavCtlAccept,NULL);
			break;
	}
}

// *****************************************************************************
// *
// *	HandleCustomMouseDown()	
// *
// *****************************************************************************
void HandleCustomMouseDown(NavCBRecPtr callBackParms)
{
	OSErr			theErr = noErr;
	ControlHandle	whichControl;
	Point 			where = callBackParms->eventData.eventDataParms.event->where;
	short			theItem = 0;
	UInt16 			firstItem = 0;
	short			realItem = 0;
	short			partCode = 0;
	short			itemType;
	Handle			itemHandle;
	Rect			itemRect;
	FSSpec			spec;
	
	GlobalToLocal(&where);
	theItem = FindDialogItem(GetDialogFromWindow(callBackParms->window), where);	// get the item number of the control
	partCode = FindControl(where,callBackParms->window,&whichControl);	// get the control itself
	
	// ask NavServices for the first custom control's ID:
	if (callBackParms->context != 0) {	// always check to see if the context is correct
		theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
		realItem = theItem - firstItem + 1;		// map it to our DITL constants:
	}
	
	switch (realItem) {
		case 10:
			if (previewPartition)	// STOP Preview
			{
				MADDriver->Reading = false;							// Stop reading current partition
				//	MADStopDriver(MADDriver);							// Stop driver interrupt function
				MADAttachDriverToMusic(MADDriver, curMusic, NULL);
				MADDisposeMusic(&previewPartition, MADDriver);				// Dispose the current music
				previewPartition = NULL;
				
				GetDialogItem(GetDialogFromWindow(callBackParms->window), 10 + firstItem, &itemType, &itemHandle, &itemRect);
				SetControlTitle((ControlHandle) itemHandle, "\pPreview");
				
				if (gEraseAddCurrent) ControlSwitch(firstItem + 10, GetDialogFromWindow(callBackParms->window), 255);
			} else if (ExtractFile(callBackParms, &spec) == noErr) {
				char	tempC[5];
				OSType	type = GetOSTypeFromSpecUsingUTI(spec);
				
				if (type != 0) {
					if (type != 'STCf') {
						switch (showWhat) {
							case allReadable:
							case allFiles:
								MADMusicIdentifyFSp(gMADLib, tempC, &spec);
								type = Ptr2OSType(tempC);
								break;
								
							default:
								break;
						}
						
						if (type == '!!!!' || CheckFileType(spec, type) == false) {
							Erreur(4, -86);
						} else {
							MADDriver->Reading = false;
							MADPurgeTrack(MADDriver);
							MADCleanDriver(MADDriver);
							
							DoStop();
							
							HSetVol(NULL, spec.vRefNum, spec.parID);
							
							OSType2Ptr(type, tempC);
							
							if (MADPlugAvailable(gMADLib, tempC)) {		// Is available a plug to open this file?
								if (MADLoadMusicFilePString(gMADLib, &previewPartition, tempC, spec.name) == noErr)			// Load this music with help of Plugs
									// in application folder or in 'Plugs' folder
								{
									MADAttachDriverToMusic(MADDriver, previewPartition, NULL);
									
									MADCheckSpeed(previewPartition, MADDriver);
									MADDriver->Reading = true;		// Read the current partition in memory
								}
							}
							
							GetDialogItem(GetDialogFromWindow(callBackParms->window), 10 + firstItem, &itemType, &itemHandle, &itemRect);
							SetControlTitle((ControlHandle) itemHandle, "\pStop");
						}
					}
				}
			}
			break;
			
		case 9:	// AddALL
			if (ExtractFile(callBackParms, &spec) == noErr) {
				CInfoPBRec	block;
				Str255		directoryName;
				
				block.dirInfo.ioDrParID = spec.parID;
				block.dirInfo.ioNamePtr = directoryName;
				
				block.dirInfo.ioVRefNum = spec.vRefNum;
				block.dirInfo.ioFDirIndex = -1;
				block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
				if (PBGetCatInfoSync(&block) == noErr) {
					pStrcpy(spec.name, directoryName);
					spec.parID = block.dirInfo.ioDrParID;
					AESendOpenFile(&spec);
				}
			}
			break;
			
		case 8:
			if (ExtractFile(callBackParms, &spec) == noErr) {
				OSType	type;
				char	tempC[5];
				
				if ((type = GetOSTypeFromSpecUsingUTI(spec))) {
					if (type != 'STCf') {
						
						switch (showWhat) {
							case allReadable:
							case allFiles:
								MADMusicIdentifyFSp(gMADLib, tempC, &spec);
								type = Ptr2OSType(tempC);
								
								if (type == '!!!!') {
									if (QTTestConversion(&spec, type) == false) {
										type = '!!!!';
									}
								}
								break;
								
							default:
								break;
						}
						
						if (type == '!!!!' || CheckFileType(spec, type) == false) {
							Erreur(4, -86);
						} else {
							HSetVol(NULL, spec.vRefNum, spec.parID);
							
							AddMODList(true,
									   spec.name,
									   spec.vRefNum,
									   spec.parID);
						}
					} else {
						ImportFile(spec.name, spec.vRefNum, spec.parID, type);
					}
				} else {	// Probably a folder or a disk...
					AESendOpenFile(&spec);
				}
			}
			break;
			
		case 7:
		case 6:
		{
			Point	Zone;
			short	itemType;
			Rect	itemRect;
			Handle	itemHandle;
			long	mresult;
			Str255	str;
			
			InsertMenu(showWhatMenu, hierMenu );
			GetDialogItem(GetDialogFromWindow(callBackParms->window), firstItem + 7, &itemType, &itemHandle, &itemRect);
			
			Zone.h = itemRect.right - 13;
			Zone.v = itemRect.top;
			
			LocalToGlobal(&Zone);
			
			SetItemMark(showWhatMenu, showWhat, 0xa5);
			mresult = PopUpMenuSelect(showWhatMenu,
									  Zone.v,
									  Zone.h,
									  showWhat );
			SetItemMark(showWhatMenu, showWhat, 0);
			
			if (HiWord(mresult) != 0) {
				if (LoWord(mresult) != showWhat) {
					AEDesc selectionList;
					
					showWhat = LoWord(mresult);
					
					GetMenuItemText(showWhatMenu, showWhat, str);
					SetDText(GetDialogFromWindow(callBackParms->window), firstItem + 6, str);
					
					InvalWindowRect(callBackParms->window, &callBackParms->customRect);
					
					specificType = plugListO[showWhat];
					
					theErr = NavCustomControl(callBackParms->context, kNavCtlGetLocation, &selectionList);
					theErr = NavCustomControl(callBackParms->context, kNavCtlSetLocation, &selectionList);
				}
			}
			DeleteMenu(GetMenuID(showWhatMenu));
		}
	}
}

// *****************************************************************************
// *
// *	myEventProc()	
// *
// *****************************************************************************
pascal void myCustomEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
	OSErr	theErr = noErr;
	FSSpec	spec;
	
	//if (callBackUD != 2) return;
	
	switch (callBackSelector) {
		case kNavCBEvent:
			switch (callBackParms->eventData.eventDataParms.event->what) {
				case kHighLevelEvent:
					AEProcessAppleEvent(callBackParms->eventData.eventDataParms.event);
					break;
					
				case nullEvent:
					if (gEraseAdd != gEraseAddCurrent) {
						short firstItem;
						
						gEraseAddCurrent = gEraseAdd;
						
						if (gEraseAddCurrent) {
							theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
							if (previewPartition == NULL) ControlSwitch(firstItem + 10, GetDialogFromWindow(callBackParms->window), 255);
							ControlSwitch(firstItem + 9, GetDialogFromWindow(callBackParms->window), 255);
							ControlSwitch(firstItem + 8, GetDialogFromWindow(callBackParms->window), 255);
						} else {
							theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
							ControlSwitch(firstItem + 10, GetDialogFromWindow(callBackParms->window), 0);
							ControlSwitch(firstItem + 9, GetDialogFromWindow(callBackParms->window), 0);
							ControlSwitch(firstItem + 8, GetDialogFromWindow(callBackParms->window), 0);
						}
					}
					
					if (gUpdateCurrentFile)
						gUpdateCurrentFile = false;
					
					DoGlobalNull();
					break;
					
				case updateEvt:
					if ((WindowPtr)callBackParms->eventData.eventDataParms.event->message == callBackParms->window) {
						
					} else {
						EventRecord *event = callBackParms->eventData.eventDataParms.event;
						GrafPtr		savedPort;
						Rect		caRect;
						
						GetPort(&savedPort);
						SetPortWindowPort((WindowPtr)event->message);
						
						GetPortBounds(GetWindowPort((WindowPtr)event->message), &caRect);
						
						InvalWindowRect(((WindowPtr)event->message), &caRect);
						SetPort(savedPort);
						
						DoUpdateEvent(callBackParms->eventData.eventDataParms.event);
					}
					break;
					
				case mouseDown:
					HandleCustomMouseDown(callBackParms);
					break;
					
				default:
					break;
			}
			break;
			
		case kNavCBSelectEntry:
			if (ExtractFile(callBackParms, &spec) == noErr) {
				FInfo	fndrInfo;
				
				if (FSpGetFInfo(&spec, &fndrInfo) == noErr) {
					gEraseAdd = false;
				} else {
					gEraseAdd = true;
				}
			}
			break;
			
		case kNavCBCustomize:
		{
			// here are the desired dimensions for our custom area:
			short neededWidth = callBackParms->customRect.left + kCustomWidth;
			short neededHeight = callBackParms->customRect.top + kCustomHeight;
			
			// check to see if this is the first round of negotiations:
			if ((callBackParms->customRect.right == 0) && (callBackParms->customRect.bottom == 0)) {
				// it is, so tell NavServices what dimensions we want:
				callBackParms->customRect.right = neededWidth;
				callBackParms->customRect.bottom = neededHeight;
			} else {
				// we are in the middle of negotiating:
				if (gLastTryWidth != callBackParms->customRect.right)
					if (callBackParms->customRect.right < neededWidth)	// is NavServices width too small for us?
						callBackParms->customRect.right = neededWidth;
				
				if (gLastTryHeight != callBackParms->customRect.bottom)	// is NavServices height too small for us?
					if (callBackParms->customRect.bottom < neededHeight)
						callBackParms->customRect.bottom = neededHeight;
			}
			
			// remember our last size so the next time we can re-negotiate:
			gLastTryWidth = callBackParms->customRect.right;
			gLastTryHeight = callBackParms->customRect.bottom;
		}
			break;
			
		case kNavCBAdjustRect:
			if (callBackParms->context != 0) {	// always check to see if the context is correct
				short firstItem, itemType;
				Handle	itemHandle;
				Rect	itemRect;
				
				theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
				if (theErr == noErr) {
					GetDialogItem(GetDialogFromWindow(callBackParms->window), 8 + firstItem, &itemType, &itemHandle, &itemRect);
					MoveControl((ControlHandle)itemHandle, callBackParms->customRect.right - 200, callBackParms->customRect.top + 10);
					
					GetDialogItem(GetDialogFromWindow(callBackParms->window), 10 + firstItem, &itemType, &itemHandle, &itemRect);
					MoveControl((ControlHandle)itemHandle, callBackParms->customRect.right - 100, callBackParms->customRect.top + 10);
				}
			}
			break;
			
		case kNavCBStart:
			// add the rest of the custom controls via the DITL resource list:
			gDitlList = GetResource('DITL', kControlListID);
			
			if ((gDitlList != NULL)&&(ResError() == noErr)) {
				if ((theErr = NavCustomControl(callBackParms->context,kNavCtlAddControlList,gDitlList)) == noErr) {
					Str255	str;
					
					theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&gfirstItem);	// ask NavServices for our first control ID
					
					GetMenuItemText(showWhatMenu, showWhat, str);
					SetDText(GetDialogFromWindow(callBackParms->window), gfirstItem + 6, str);
					InvalWindowRect(callBackParms->window, &callBackParms->customRect);
					
					myCustomEventProc(kNavCBAdjustRect, callBackParms, callBackUD);
				}
			}
			break;
			
		case kNavCBAccept:
			if (AddAll == false) {
				if (ExtractFile(callBackParms, &spec) == noErr) {
					char	tempC[5];
					OSType	type;
					FSSpec	spec;
					
					if ((type = GetOSTypeFromSpecUsingUTI(spec))) {
						if (type != 'STCf') {
							switch (showWhat) {
								case allReadable:
								case allFiles:
									MADMusicIdentifyFSp(gMADLib, tempC, &spec);
									type = Ptr2OSType(tempC);
									break;
									
								default:
									break;
							}
							
							if (type == '!!!!' || CheckFileType(spec, type) == false) {
								Erreur(4, -86);
							}
						}
					}
				}
			}
			break;
			
		case kNavCBTerminate:
			// release our appended popup menu:
			if (gDitlList)
				ReleaseResource(gDitlList);
			break;
	}
}

pascal Boolean MyCustomFilter(AEDesc *theItem, void *info, NavCallBackUserData callBackUD, NavFilterModes filterMode)
{
	short		i;
	FSRef		spec;
	OSType		type;
	char		tempC[5];
	
	if (MyAEGetDescData(theItem, NULL, &spec, sizeof(FSRef), NULL) == noErr) {
		if ((type = GetOSTypeFromRefUsingUTI(spec))) {
			switch (showWhat) {
				case allMusics:
					if (type == 'STCf' || type == 'MADK')
						return true;
					
					for (i = 0; i < gMADLib->TotalPlug; i++) {
						if (type == Ptr2OSType(gMADLib->ThePlug[i].type))
							return true;
					}
					
					if (QTTypeConversion(type))
						return true;
					break;
					
				case allReadable:
					if (type == 'STCf')
						return true;
					
					if (MADMusicIdentifyFSRef(gMADLib, tempC, &spec) == noErr)
						return true;
					break;
					
				case allFiles:
					return true;
					break;
					
				case MADK:
					if (type == 'MADK')
						return true;
					break;
					
				default:
					if (type == specificType)
						return true;
					break;
			}
			return false;
		}
	}
	
	return true;
}

pascal Boolean MyCustomFilter2(AEDesc *theItem, void *info, NavCallBackUserData callBackUD, NavFilterModes filterMode)
{
	FSSpec		spec;
	OSType		type;
	
	if (MyAEGetDescData(theItem, NULL, &spec, sizeof(FSSpec), NULL) == noErr) {
		if ((type = GetOSTypeFromSpecUsingUTI(spec))) {
			switch (specificType) {
				case 'ANYK':
					return true;
					break;
					
				default:
					if (type == specificType)
						return true;
					else
						return false;
					break;
			}
		}
	}
	
	return true;
}


OSErr DoCustomOpen(FSSpec *spec)
{
	NavReplyRecord				theReply;
	NavDialogCreationOptions	dialogOptions;
	OSErr						theErr = noErr;
	long						i;
	NavEventUPP					eventUPP = NewNavEventUPP(myCustomEventProc);
	NavObjectFilterUPP			filterProcUPP = NewNavObjectFilterUPP(MyCustomFilter);
	NavDialogRef				diagRef = NULL;
	NavTypeListPtr				listPtr = NULL;
	
	
	gEraseAdd = false;
	gEraseAddCurrent = false;
	gUpdateCurrentFile = false;
	
	needUp		= true;
	AddAll		= false;
	
	previewPartition = NULL;
	
	showWhatMenu = GetMenu(500);
	
	listPtr = (NavTypeListPtr)NewPtrClear(sizeof(NavTypeList) + sizeof(OSType) * gMADLib->TotalPlug + 1);
	listPtr->componentSignature = 'SNPL';
	
	for (i = 0; i < gMADLib->TotalPlug; i++) {
		Str255 pMenuName;
		OSType theType = Ptr2OSType(gMADLib->ThePlug[i].type);
		GetPStrFromCFString(gMADLib->ThePlug[i].MenuName, pMenuName);
		switch (gMADLib->ThePlug[i].mode) {
			case 'EXIM':
			case 'IMPL':
				AppendMenu(showWhatMenu, pMenuName);
				plugListO[CountMenuItems(showWhatMenu)] = theType;
				listPtr->osType[listPtr->osTypeCount++] = theType;
				break;
				
			default:
				
				break;
		}
	}
	listPtr->osType[listPtr->osTypeCount++] = 'STCf';
	
	specificType = plugListO[showWhat];
	
	// default behavior for browser and dialog:
	theErr = NavGetDefaultDialogCreationOptions(&dialogOptions);
	
	dialogOptions.optionFlags &= ~(kNavAllowPreviews | kNavAllowMultipleFiles);
	dialogOptions.optionFlags |= kNavDontAutoTranslate | kNavNoTypePopup;
	
	dialogOptions.clientName = CFSTR("PlayerPRO 5");
	
	
	//TODO: Open based on UTI, not Mac OS filetype.
	//Mac OS X does NOT set filetype or creator based on file extention on any version I know of.
	//TODO: Use a more Quartz UI-friendly way of selecting what filetype to use.
	theErr = NavCreateGetFileDialog(&dialogOptions, &listPtr, eventUPP, NULL, filterProcUPP, NULL, &diagRef);
	NavDialogRun(diagRef);
	
	UpdateALLWindow();
	
	if (previewPartition) {
		MADDriver->Reading = false;							// Stop reading current partition
		//MADStopDriver(MADDriver);							// Stop driver interrupt function
		MADAttachDriverToMusic(MADDriver, curMusic, NULL);
		MADDisposeMusic(&previewPartition, MADDriver);				// Dispose the current music
		previewPartition = NULL;
	}
	
	theErr = NavDialogGetReply(diagRef, &theReply);
	
	if (theReply.validRecord && theErr == noErr) {
		AEDesc 	resultDesc;
		long	index, count;
		
		// we are ready to open the document(s), grab information about each file for opening:
		theErr = AECountItems(&(theReply.selection), &count);
		for (index=1;index<=count;index++) {
			AEKeyword keyword;
			
			if ((theErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &keyword,&resultDesc)) == noErr) {
				if ((theErr = MyAEGetDescData (&resultDesc, NULL, spec, sizeof(FSSpec), NULL )) == noErr)
					
					theErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		theErr = NavDisposeReply(&theReply);	// clean up after ourselves
	} else
		theErr = -1;
	
	NavDialogDispose(diagRef);
	NavDisposeReply(&theReply);
	DisposeMenu(showWhatMenu);
	DisposePtr((Ptr)listPtr);
	
	DisposeNavEventUPP(eventUPP);
	DisposeNavObjectFilterUPP(filterProcUPP);
	
	return theErr;
}

OSErr DoCustomSave(Str255 bStr, Str255 fileName, OSType theType, FSSpec *spec)
{
	NavReplyRecord				reply;
	NavDialogCreationOptions	dialogOptions;
	OSErr						theErr;
	NavDialogRef				diaRef = NULL;
	CFStringRef					fileNameCF, bStrCF;
	
	theErr = NavGetDefaultDialogCreationOptions(&dialogOptions);
	
	dialogOptions.clientName = CFSTR("PlayerPRO 5");
	fileNameCF = CFStringCreateWithPascalString(kCFAllocatorDefault, fileName, kCFStringEncodingMacRoman);
	bStrCF = CFStringCreateWithPascalString(kCFAllocatorDefault, bStr, kCFStringEncodingMacRoman);
	dialogOptions.saveFileName = fileNameCF;
	dialogOptions.windowTitle = bStrCF;
	
	NavCreatePutFileDialog(&dialogOptions, theType, 'SNPL', MyDlgFilterNavDesc, NULL, &diaRef);
	NavDialogRun(diaRef);
		
	theErr = NavDialogGetReply(diaRef, &reply);
	
	UpdateALLWindow();
	
	if (reply.validRecord) {
		AEDesc 	resultDesc;
		AEKeyword keyword;
		
		resultDesc.dataHandle = NULL;
		
		// retrieve the returned selection:
		// since only 1 selection is possible, we get the first AEDesc:
		if ((theErr = AEGetNthDesc(&(reply.selection),1,typeFSS, &keyword ,&resultDesc)) == noErr) {
			MyAEGetDescData(&resultDesc, NULL, spec, sizeof(FSSpec), NULL);
			
			theErr = NavCompleteSave(&reply, kNavTranslateInPlace);
		}
		
	} else
		theErr = -1;
	
	NavDialogDispose(diaRef);
	NavDisposeReply(&reply);
	CFRelease(bStrCF);
	CFRelease(fileNameCF);
	
	return theErr;
}

OSErr DoStandardOpen(FSSpec	*spec, Str255 string, OSType inType)
{
	NavReplyRecord			theReply;
	NavDialogOptions		dialogOptions;
	NavTypeListHandle		openList = NULL;
	OSErr					iErr;
	NavObjectFilterUPP		filterProcUPP = NULL;
	
	// default behavior for browser and dialog:
	iErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	dialogOptions.dialogOptionFlags	&=	~(kNavAllowPreviews | kNavDontAutoTranslate | kNavAllowMultipleFiles);
	dialogOptions.dialogOptionFlags	|=	kNavNoTypePopup | kNavSupportPackages;
	
	pStrcpy(dialogOptions.clientName, string);
	
	if (inType == 'ANYK') {
		specificType = 'ANYK';
	} else {
		specificType = inType;
		filterProcUPP = NewNavObjectFilterUPP(MyCustomFilter2);
		
		openList = (NavTypeListHandle)NewHandle(sizeof(NavTypeList) + 1 * sizeof(OSType));
		if (openList) {
			HLock((Handle)openList);
			
			(*openList)->componentSignature	= 'SNPL';
			(*openList)->osTypeCount		= 1;
			(*openList)->osType[0]			= inType;
		}
	}
	
	iErr = NavGetFile(NULL,	// use system's default location
					  &theReply,
					  &dialogOptions,
					  MyDlgFilterNavDesc,
					  NULL,	// no custom previews
					  filterProcUPP,
					  openList,
					  (NavCallBackUserData) 2L);
		
	if (openList != NULL) {
		HUnlock((Handle)openList);
		DisposeHandle((Handle)openList);
	}
	
	if (theReply.validRecord && iErr == noErr) {
		AEDesc 	resultDesc;
		long	index, count;
		
		// we are ready to open the document(s), grab information about each file for opening:
		iErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++) {
			AEKeyword keyword;
			
			if ((iErr = AEGetNthDesc(&(theReply.selection),index,typeFSS,&keyword,&resultDesc)) == noErr) {
				if ((iErr = MyAEGetDescData(&resultDesc, NULL, spec, sizeof(FSSpec), NULL)) == noErr)
					iErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	} else
		iErr = -1;
	
	UpdateALLWindow();
	
	return iErr;
}

CFArrayRef CopySupportedMusicUTIs()
{
	CFIndex i;
	//TODO: this!
	CFMutableArrayRef UTIs = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(UTIs, CFSTR("com.quadmation.playerpro.madk"));
	
	for (i = 0; i < gMADLib->TotalPlug; i++) {
		CFArrayAppendArray(UTIs, gMADLib->ThePlug[i].UTItypes, CFRangeMake(0, CFArrayGetCount(gMADLib->ThePlug[i].UTItypes)));
	}
	
	CFArrayRef outPutUTIs = CFArrayCreateCopy(kCFAllocatorDefault, UTIs);
	CFRelease(UTIs);
	return outPutUTIs;
}

OSType GetOSTypeFromRefUsingUTI(FSRef tmpRef)
{
	CFStringRef		utiStr, extStr;
	CFURLRef		theURL;
	FSCatalogInfo	fileCat = {0};
	FileInfo		*fndrInfo = (FileInfo*)&fileCat.finderInfo;
	OSStatus		iErr;
	OSType			retType;
	
	iErr = FSGetCatalogInfo(&tmpRef, kFSCatInfoFinderInfo, &fileCat, NULL, NULL, NULL);
	if (iErr == noErr && (fndrInfo->fileType != 0 && fndrInfo->fileType != 0x3F3F3F3F && fndrInfo->fileType != '    ')) {
		retType = fndrInfo->fileType;
		extStr = UTCreateStringForOSType(retType);
		if (!extStr) {
			return retType;
		}
		utiStr = UTTypeCreatePreferredIdentifierForTag(kUTTagClassOSType, extStr, NULL);
		CFRelease(extStr);
		if (!utiStr) {
			return retType;
		}
		extStr = UTTypeCopyPreferredTagWithClass(utiStr, kUTTagClassOSType);
		CFRelease(utiStr);
		if (!extStr) {
			return retType;
		}
		
		retType = UTGetOSTypeFromString(extStr);
		CFRelease(extStr);
		return retType;
	}
	
	if (!(theURL = CFURLCreateFromFSRef(kCFAllocatorDefault, &tmpRef))) {
		return 0;
	}
	extStr = CFURLCopyPathExtension(theURL);
	CFRelease(theURL);
	if (!extStr) {
		return 0x3F3F3F3F;
	}
	utiStr = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extStr, NULL);
	CFRelease(extStr);
	if (!utiStr) {
		return 0x3F3F3F3F;
	}
	
	extStr = UTTypeCopyPreferredTagWithClass(utiStr, kUTTagClassOSType);
	CFRelease(utiStr);
	if (!extStr) {
		return 0x3F3F3F3F;
	}
	
	retType = UTGetOSTypeFromString(extStr);
	CFRelease(extStr);
	return retType;
}

OSType GetOSTypeFromSpecUsingUTI(FSSpec theSpec)
{
	FSRef		tmpRef;
	OSStatus	iErr;
	iErr = FSpMakeFSRef(&theSpec, &tmpRef);
	if (iErr) {
		return 0;
	}
	return GetOSTypeFromRefUsingUTI(tmpRef);
}
