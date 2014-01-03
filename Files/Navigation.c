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

// the requested dimensions for our sample open customization area:
#define kCustomWidth			410
#define kCustomHeight			20
#define kControlListID			149

extern Cursor	PlayCrsr;

// customization globals:
static	Handle 		gDitlList = NULL;
static	short 		gLastTryWidth;
static	short 		gLastTryHeight;
static	UInt16 		gfirstItem = 0;

static Boolean 		needUp, AddAll;
static MenuHandle	showWhatMenu;
static OSType		plugListO[ 25];
static OSType		specificType;
extern short		showWhat;
static MADMusic		*previewPartition = NULL;
static Boolean		gEraseAdd, gEraseAddCurrent, gUpdateCurrentFile;

Boolean QTTestConversion(FSSpec *file, OSType fileType);

CFArrayRef CopySupportedMusicUTIs();

enum {
	allMusics = 1,
	allReadable = 2,
	allFiles = 3,
	MADK = 5,
	others = 7
};

OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize)
{
	Handle		h;
	ByteCount	dataSize;
	ByteCount	acSize;
	
	if (typeCode) *typeCode = desc->descriptorType;
	
	h = (Handle)desc->dataHandle;
	dataSize = GetHandleSize(h);
	if (dataSize > maximumSize)
		acSize = maximumSize;
	else
		acSize = dataSize;
	BlockMoveData(*h, dataBuffer, acSize);
	
	if (actualSize) *actualSize = acSize;
	
	return noErr;
}

OSErr ExtractFile(NavCBRecPtr callBackParms, FSSpec	*finalSpec)
{
	OSErr			theErr = -1;
	AEDescList 		selectionList;
	
	if (NavCustomControl(callBackParms->context, kNavCtlGetSelection, &selectionList ) == noErr)
	{
		long		count = 0;
		UInt16 		firstItem = 0;
		Cell		beforeCell = {0,0};
		Handle		itemH;
		Rect		itemRect;
		short		itemType;
		long		index;
		
		if (AECountItems(&selectionList, &count ) == noErr)
		{
			//	for (index=1; index<=count; index++ )
			index = 1;
			{
				AEDesc theItemDesc;
				AEKeyword keyword;
				
				if (AEGetNthDesc(&selectionList, index, typeFSS, &keyword, &theItemDesc ) == noErr)
				{
					// put something in there:
					if (MyAEGetDescData (&theItemDesc, NULL, finalSpec, sizeof (FSSpec ), NULL ) == noErr)
					{
						theErr = noErr;
					}
					AEDisposeDesc(&theItemDesc);
				}
			}
		}
		AEDisposeDesc(&selectionList );
	}
	
	return theErr;
}

#if 0
void NWriteSupportedFormat(DialogPtr	aDia)
{
	short	i;
	Str255	text;
	
	pStrcpy(text, "\pMADI");
	
	for (i = 0; i < MADGetMADLibraryPtr()->TotalPlug; i++)
	{
		pStrcat(text, "\p - ");
		pStrcat(text, MADGetMADLibraryPtr()->ThePlug[ i].MenuName);
	}
	
	SetDText(aDia, gfirstItem + 4, text);
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
	switch (selection)
		{								
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
#endif

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
	if (callBackParms->context != 0)	// always check to see if the context is correct
	{
		theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);	
		realItem = theItem - firstItem + 1;		// map it to our DITL constants:	
	}
	
	switch(realItem)
	{
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
			}
			else if (ExtractFile(callBackParms, &spec) == noErr)
			{
				FInfo	fndrInfo;
				char	tempC[ 5];
				
				if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
				{
					if (fndrInfo.fdType != 'sTAT' && fndrInfo.fdType != 'STCf')
					{
						OSType type;
						
						switch(showWhat)
						{
							case allReadable:
							case allFiles:
								MADMusicIdentifyFSp(gMADLib, tempC, &spec);
								type = Ptr2OSType(tempC);
								break;
								
							default:
								type = fndrInfo.fdType;
								break;
						}
						
						if (type == '!!!!' || CheckFileType(spec, type) == false)
						{
							Erreur(4, -86);
						}
						else
						{
							MADDriver->Reading = false;
							MADPurgeTrack(MADDriver);
							MADCleanDriver(MADDriver);
							
							DoStop();
							
							HSetVol(NULL, spec.vRefNum, spec.parID);
							
							OSType2Ptr(type, tempC);
							
							if (MADPlugAvailable(gMADLib, tempC))		// Is available a plug to open this file?
							{
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
#if 0
		case 9:	// AddALL
			if (ExtractFile(callBackParms, &spec) == noErr)
			{
				CInfoPBRec	block;
				Str255		directoryName;
				
				block.dirInfo.ioDrParID = spec.parID;
				block.dirInfo.ioNamePtr = directoryName;
				
				block.dirInfo.ioVRefNum = spec.vRefNum;
				block.dirInfo.ioFDirIndex = -1;
				block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
				if (PBGetCatInfo(&block, false) == noErr)
				{
					pStrcpy(spec.name, directoryName);
					spec.parID = block.dirInfo.ioDrParID;
					AESendOpenFile(&spec);
				}
			}
			break;
#endif
			
		case 8:
			if (ExtractFile(callBackParms, &spec) == noErr)
			{
				FInfo	fndrInfo;
				char	tempC[ 5];
				
				if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
				{
					if (fndrInfo.fdType != 'sTAT' && fndrInfo.fdType != 'STCf')
					{
						OSType type;
						
						switch(showWhat)
						{
							case allReadable:
							case allFiles:
								MADMusicIdentifyFSp(gMADLib, tempC, &spec);
								type = Ptr2OSType(tempC);
								
								if (type == '!!!!')
								{
									if (QTTestConversion(&spec, type) == true)
									{
										type = fndrInfo.fdType;
									}
								}
								break;
								
							default:
								type = fndrInfo.fdType;
								break;
						}
						
						if (type == '!!!!' || CheckFileType(spec, type) == false)
						{
							Erreur(4, -86);
						}
						else
						{
							HSetVol(NULL, spec.vRefNum, spec.parID);
							
							AddMODList(true,
									   spec.name,
									   spec.vRefNum,
									   spec.parID);
						}
					}
					else
					{
						ImportFile(spec.name, spec.vRefNum, spec.parID, fndrInfo.fdType);
					}
				}
				else	// Probably a folder or a disk...
				{
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
			mresult = PopUpMenuSelect(	showWhatMenu,
									  Zone.v,
									  Zone.h,
									  showWhat );
			SetItemMark(showWhatMenu, showWhat, 0);
			
			if (HiWord(mresult ) != 0 )
			{
				if (LoWord(mresult) != showWhat)
				{
					AEDesc 		selectionList;
					
					showWhat = LoWord(mresult);
					
					GetMenuItemText(showWhatMenu, showWhat, str);
					SetDText(GetDialogFromWindow(callBackParms->window), firstItem + 6, str);
					
					InvalWindowRect(callBackParms->window, &callBackParms->customRect);
					
					specificType = plugListO[ showWhat];
					
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
pascal void myCustomEventProc(	NavEventCallbackMessage 	callBackSelector, 
								NavCBRecPtr 				callBackParms, 
								NavCallBackUserData 		callBackUD)
{
	OSErr		theErr = noErr;
	short 		index = 0;
	FSSpec		spec;
	
	//if (callBackUD != 2) return;
	
	switch (callBackSelector)
	{
		case kNavCBEvent:
			switch (callBackParms->eventData.eventDataParms.event->what)
		{
			case kHighLevelEvent:
				AEProcessAppleEvent(callBackParms->eventData.eventDataParms.event);
				break;
				
			case nullEvent:
				if (gEraseAdd != gEraseAddCurrent)
				{
					short			itemType, firstItem;
					Handle			itemHandle;
					Rect			itemRect;
					
					gEraseAddCurrent = gEraseAdd;
					
					if (gEraseAddCurrent)
					{
						theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
						if (previewPartition == NULL) ControlSwitch(firstItem + 10, GetDialogFromWindow(callBackParms->window), 255);
						//	ControlSwitch(firstItem + 9, callBackParms->window, 255);
						//	ControlSwitch(firstItem + 8, callBackParms->window, 255);
					}
					else
					{
						theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
						ControlSwitch(firstItem + 10, GetDialogFromWindow(callBackParms->window), 0);
						//	ControlSwitch(firstItem + 9, callBackParms->window, 0);
						//	ControlSwitch(firstItem + 8, callBackParms->window, 0);
					}
				}
				
				if (gUpdateCurrentFile)
				{
					gUpdateCurrentFile = false;
				}
				
				DoGlobalNull();
				break;
				
			case updateEvt:
				if ((WindowPtr) callBackParms->eventData.eventDataParms.event->message == callBackParms->window)
				{
					
				}
				else
				{
					EventRecord *event = callBackParms->eventData.eventDataParms.event;
					GrafPtr		savedPort;
					Rect		caRect;
					
					GetPort(&savedPort);
					SetPortWindowPort((WindowPtr) event->message);
					
					GetPortBounds(GetWindowPort((WindowPtr) event->message), &caRect);
					
					InvalWindowRect(((WindowPtr) event->message), &caRect);
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
			if (ExtractFile(callBackParms, &spec) == noErr)
			{
				FInfo	fndrInfo;
				char	tempC[ 5];
				
				if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
				{
					gEraseAdd = false;
				}
				else
				{
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
			if ((callBackParms->customRect.right == 0) && (callBackParms->customRect.bottom == 0))
			{
				// it is, so tell NavServices what dimensions we want:
				callBackParms->customRect.right = neededWidth;
				callBackParms->customRect.bottom = neededHeight;
			}
			else
			{
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
			break;
		}
			
		case kNavCBAdjustRect:
			
			if (callBackParms->context != 0)	// always check to see if the context is correct
			{
				short firstItem, itemType;
				Handle	itemHandle;
				Rect	itemRect;
				
				theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);
				if (theErr == noErr)
				{
					GetDialogItem(GetDialogFromWindow(callBackParms->window), 8 + firstItem, &itemType, &itemHandle, &itemRect);
					MoveControl((ControlHandle) itemHandle, callBackParms->customRect.right - 200, callBackParms->customRect.top + 10);
					
					GetDialogItem(GetDialogFromWindow(callBackParms->window), 10 + firstItem, &itemType, &itemHandle, &itemRect);
					MoveControl((ControlHandle) itemHandle, callBackParms->customRect.right - 100, callBackParms->customRect.top + 10);
				}
			}
			break;
			
		case kNavCBStart:
		{
			// add the rest of the custom controls via the DITL resource list:
			gDitlList = GetResource('DITL',kControlListID);
			
			if ((gDitlList != NULL)&&(ResError() == noErr))
				if ((theErr = NavCustomControl(callBackParms->context,kNavCtlAddControlList,gDitlList)) == noErr)
				{
					Str255	str;
					
					theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&gfirstItem);	// ask NavServices for our first control ID
					
					GetMenuItemText(showWhatMenu, showWhat, str);
					SetDText (GetDialogFromWindow(callBackParms->window), gfirstItem + 6, str);
					InvalWindowRect(callBackParms->window, &callBackParms->customRect);
					
					myCustomEventProc(kNavCBAdjustRect, callBackParms, callBackUD);
				}
			break;
		}
			
		case kNavCBAccept:
			if (AddAll == false)
			{
				if (ExtractFile(callBackParms, &spec) == noErr)
				{
					FInfo	fndrInfo;
					char	tempC[ 5];
					
					if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
					{
						if (fndrInfo.fdType != 'sTAT' && fndrInfo.fdType != 'STCf')
						{
							OSType type;
							
							switch(showWhat)
							{
								case allReadable:
								case allFiles:
									MADMusicIdentifyFSp(gMADLib, tempC, &spec);
									type = Ptr2OSType(tempC);
									break;
									
								default:
									type = fndrInfo.fdType;
									break;
							}
							
							if (type == '!!!!' || CheckFileType(spec, type) == false)
							{
								
								Erreur(4, -86);
							}
							else
							{
								HSetVol(NULL, spec.vRefNum, spec.parID);
								// HGetFInfo(fndrInfo.fdFile.vRefNum, fndrInfo.fdFile.parID, fndrInfo.fdFile.name, &fndrInfo);
								// fndrInfo.fdCreator = 'SNPL';
								// SetFInfo(fndrInfo.fdFile.name, 0, &fndrInfo);
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
	Boolean		ready = false;
	short		i;
	FSSpec		spec;
	OSType		type;
	char		tempC[ 5];
	FInfo		fndrInfo;

	if (MyAEGetDescData (theItem, NULL, &spec, sizeof (FSSpec ), NULL ) == noErr)
	{
		if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
		{
			switch(showWhat)
			{
				case allMusics:
					if (fndrInfo.fdType == 'sTAT') return true;
					if (fndrInfo.fdType == 'STCf') return true;
					if (fndrInfo.fdType == 'MADK') return true;
					
					for (i = 0; i < gMADLib->TotalPlug; i++)
					{
						if (fndrInfo.fdType == Ptr2OSType(gMADLib->ThePlug[ i].type))	return true;
					}
					
					if (QTTypeConversion(fndrInfo.fdType)) return true;
				break;
				
				case allReadable:
					//if (EqualString("\pIcon", spec.name, false, false)) MyDebugStr(__LINE__, __FILE__, "");
					
					if (fndrInfo.fdType == 'sTAT') return true;
					if (fndrInfo.fdType == 'STCf') return true;
					
					if (MADMusicIdentifyFSp(gMADLib, tempC, &spec) == noErr) return true;
				break;
						
				case allFiles:		return true;	break;
				
				case MADK:	if (fndrInfo.fdType == 'MADK') return true;	break;
				
				default:	if (fndrInfo.fdType == specificType) return true;	break;
			}
			return false;
		}
	}
	
	return true;
}

pascal Boolean MyCustomFilter2(AEDesc *theItem, void *info, NavCallBackUserData callBackUD, NavFilterModes filterMode)
{
	Boolean		ready = false;
	short		i;
	FSSpec		spec;
	OSType		type;
	char		tempC[ 5];
	FInfo		fndrInfo;
	
	if (MyAEGetDescData (theItem, NULL, &spec, sizeof (FSSpec ), NULL ) == noErr)
	{
		if (FSpGetFInfo(&spec, &fndrInfo) == noErr)
		{
			switch(showWhat)
			{
				default:	if (fndrInfo.fdType == specificType) return true;	break;
			}
			return false;
		}
	}
	
	return true;
}


OSErr DoCustomOpen(FSSpec	*spec)
{	
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	OSErr				theErr = noErr;
	NavTypeListHandle	typeList = NULL;
	long				count = 0, i;
	NavEventUPP			eventUPP = NewNavEventUPP(myCustomEventProc);
	NavObjectFilterUPP 	filterProcUPP = NewNavObjectFilterUPP(MyCustomFilter);
	
	gEraseAdd = false;
	gEraseAddCurrent = false;
	gUpdateCurrentFile = false;
	
	needUp		= true;
	AddAll		= false;
	
	previewPartition = NULL;
	
	showWhatMenu = GetMenu(500);
	
	for (i = 0; i < gMADLib->TotalPlug; i++)
	{
		Str255 pMenuName;
		GetPStrFromCFString(gMADLib->ThePlug[ i].MenuName, pMenuName);
		
		switch(gMADLib->ThePlug[ i].mode)
		{
			case 'EXIM':
			case 'IMPL':
				AppendMenu(showWhatMenu, pMenuName);
				plugListO[ CountMenuItems(showWhatMenu)] = Ptr2OSType(gMADLib->ThePlug[ i].type);
				break;
		}
	}
	
	specificType = plugListO[ showWhat];
	
	// default behavior for browser and dialog:
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	dialogOptions.dialogOptionFlags	&=	~kNavAllowPreviews;
	dialogOptions.dialogOptionFlags	|=	kNavNoTypePopup;
	dialogOptions.dialogOptionFlags	&=	~kNavAllowStationery;
	dialogOptions.dialogOptionFlags	|=	kNavDontAutoTranslate;
	dialogOptions.dialogOptionFlags	&=	~kNavAllowMultipleFiles;
	
	pStrcpy(dialogOptions.clientName, "\pPlayerPRO");
	
	//	typeList = (NavTypeListHandle) GetResource('open', 300);
	
	//TODO: Open based on UTI, not Mac OS filetype.
	//Mac OS X does NOT set filetype or creator based on file extention on any version I know of.
	//TODO: Use a more Quartz UI-friendly way of selecting what filetype to use.
	theErr = NavGetFile(	NULL,	// use system's default location
						&theReply,
						&dialogOptions,
						eventUPP,
						NULL,	// no custom previews
						filterProcUPP,
						NULL, //typeList,
						(NavCallBackUserData) 0);
	
	DisposeNavEventUPP(eventUPP);
	DisposeNavObjectFilterUPP(filterProcUPP);
	
	UpdateALLWindow();
	
	if (previewPartition)
	{
		MADDriver->Reading = false;							// Stop reading current partition
		//	MADStopDriver(MADDriver);							// Stop driver interrupt function
		MADAttachDriverToMusic(MADDriver, curMusic, NULL);
		MADDisposeMusic(&previewPartition, MADDriver);				// Dispose the current music
		previewPartition = NULL;
	}
	
	if (theReply.validRecord && theErr == noErr)
	{
		AEDesc 	resultDesc;
		FInfo	fileInfo;
		long	index;
		
		// we are ready to open the document(s), grab information about each file for opening:
		theErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
		{
			AEKeyword keyword;
			
			if ((theErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &keyword,&resultDesc)) == noErr)
			{
				if ((theErr = MyAEGetDescData (&resultDesc, NULL, spec, sizeof (FSSpec ), NULL )) == noErr)
					
					theErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		theErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	}
	else theErr = -1;
	
	//if (typeList != NULL) ReleaseResource((Handle)typeList);
	
	DisposeMenu(showWhatMenu);
	
	return theErr;
}

OSErr DoCustomSave(Str255 bStr, Str255 fileName, OSType theType, FSSpec *spec)
{
	
	NavReplyRecord		reply;
	NavDialogOptions	dialogOptions;
	OSErr				theErr;
	
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	pStrcpy(dialogOptions.clientName, "\pPlayerPRO");
	pStrcpy(dialogOptions.savedFileName, fileName);
	pStrcpy(dialogOptions.windowTitle, bStr);
	
	dialogOptions.dialogOptionFlags &= ~kNavAllowStationery;
	
	NavPutFile(NULL,
			   &reply,
			   &dialogOptions,
			   MyDlgFilterNavDesc,
			   theType,
			   'SNPL',
			   NULL);
	
	UpdateALLWindow();
	
	if (reply.validRecord)
	{
		AEDesc 	resultDesc;	
		AEKeyword keyword;
		
		resultDesc.dataHandle = NULL;
		
		// retrieve the returned selection:
		// since only 1 selection is possible, we get the first AEDesc:
		if ((theErr = AEGetNthDesc(&(reply.selection),1,typeFSS, &keyword ,&resultDesc)) == noErr)
		{
			//	MyAEGetDescData (&theItemDesc, NULL, finalSpec, sizeof (FSSpec ), NULL )
			MyAEGetDescData(&resultDesc, NULL, spec, sizeof(FSSpec), NULL );
			
			//BlockMoveData(*resultDesc.dataHandle, spec, sizeof(FSSpec));
			
			theErr = NavCompleteSave(&reply, kNavTranslateInPlace);
		}
		
	}
	else return -1;
	
	return noErr;
}

OSErr DoStandardOpen(FSSpec	*spec, Str255 string, OSType inType)
{
	NavReplyRecord			theReply;
	NavDialogOptions		dialogOptions;
	//NavTypeListHandle		openList;
	OSErr					iErr;
	NavObjectFilterUPP 		filterProcUPP = NULL;
	
	// default behavior for browser and dialog:
	iErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	dialogOptions.dialogOptionFlags	&=	~kNavAllowPreviews;
	dialogOptions.dialogOptionFlags	|=	kNavNoTypePopup;
	dialogOptions.dialogOptionFlags	|=	kNavSupportPackages;
	dialogOptions.dialogOptionFlags	&=	~kNavAllowStationery;
	dialogOptions.dialogOptionFlags	&=	~kNavDontAutoTranslate;
	dialogOptions.dialogOptionFlags	&=	~kNavAllowMultipleFiles;
	
	//dialogOptions.dialogOptionFlags -=	0x002000;
	
	pStrcpy(dialogOptions.clientName, string);
	
	if (inType == 'ANYK') filterProcUPP = NULL;
	else
	{
		specificType = inType;
		
		
		filterProcUPP = NewNavObjectFilterUPP(MyCustomFilter2);
		
#if 0
		openList = (NavTypeListHandle) NewHandle(sizeof(NavTypeList) + 1 * sizeof(OSType));
		if (openList ) HLock((Handle)openList);
		
		(*openList)->componentSignature		= 'SNPL';
		(*openList)->osTypeCount			= 1;
		(*openList)->osType[ 0]				= inType;
#endif
	}
	
#if 1
	//TODO: Open based on UTI, not Mac OS filetype.
	//Mac OS X does NOT set filetype or creator based on file extention on any version I know of.
	iErr = NavGetFile(NULL,	// use system's default location
					  &theReply,
					  &dialogOptions,
					  MyDlgFilterNavDesc,
					  NULL,	// no custom previews
					  filterProcUPP,
					  NULL, //,
					  (NavCallBackUserData) 2L);
#else
	iErr = NavGetFile(		NULL,	// use system's default location
					  &theReply,
					  &dialogOptions,
					  NULL,
					  NULL,	// no custom previews
					  filterProcUPP,
					  NULL, //,
					  (NavCallBackUserData) 2L);
#endif
	
	if (filterProcUPP != NULL) DisposeNavObjectFilterUPP(filterProcUPP);
	
#if 0
	if (openList != NULL)
	{
		HUnlock((Handle)openList);
		DisposeHandle((Handle)openList);
	}
#endif
	
	if (theReply.validRecord && iErr == noErr)
	{
		AEDesc 	resultDesc;
		long	index, count;
		
		// we are ready to open the document(s), grab information about each file for opening:
		iErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
		{
			AEKeyword keyword;
			
			if ((iErr = AEGetNthDesc(&(theReply.selection),index,typeFSS,&keyword,&resultDesc)) == noErr)
			{
				if ((iErr = MyAEGetDescData (&resultDesc, NULL, spec, sizeof (FSSpec ), NULL )) == noErr)
					
					iErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	}
	else iErr = -1;
	
	UpdateALLWindow();
	
	return iErr;
}

CFArrayRef CopySupportedMusicUTIs()
{
	//TODO: this!
	CFMutableArrayRef UTIs = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(UTIs, CFSTR("com.quadmation.playerpro.madk"));
	
	CFArrayRef outPutUTIs = CFArrayCreateCopy(kCFAllocatorDefault, UTIs);
	CFRelease(UTIs);
	return outPutUTIs;
}
