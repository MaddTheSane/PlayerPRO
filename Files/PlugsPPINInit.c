#include "MAD.h"
#include "RDriver.h"
#include "Shuddup.h"
#include "Undo.h"
#include "RDriverInt.h"
#include <MixedMode.h>

#include "PPPlug.h"
#include "PPPrivate.h"

#define MAXINSTRPLUG 100

typedef struct PPINFilterInfo
{
	PPInstrumentPlugin	**xxxx;
	CFStringRef			MenuName;
	CFStringRef			AuthorString;
	CFArrayRef			UTITypes;
	CFBundleRef			file;
	OSType				type;
	OSType				mode;
	Boolean				isSamp;
} PPINFilterInfo;

void DoPlayInstruInt2( sData *curData, InstrData *insData, short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end, long samp);
extern Cursor PlayCrsr;

#define kCustomWidth			410
#define kCustomHeight			20
#define kControlListID			164

static	Handle 		gDitlList = NULL;
static	short 		gLastTryWidth;
static	short 		gLastTryHeight;
static	UInt16 		gfirstItem = 0;


OSErr ExtractFile( NavCBRecPtr callBackParms, FSSpec	*finalSpec);
OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
pascal Boolean MyInsCustomFilter( AEDesc *theItem, void *info, NavCallBackUserData callBackUD, NavFilterModes filterMode);
pascal void myCustomEventNAVProc(	NavEventCallbackMessage 	callBackSelector, 
								 NavCBRecPtr 				callBackParms, 
								 NavCallBackUserData 		callBackUD);
void HandleCustomMouseNAVDown(NavCBRecPtr callBackParms);
void InitPPINMenu(void);
extern void NSLog(CFStringRef format, ...);


static	PPInfoPlug		thePPInfoPlug;
static	PPINFilterInfo 	*ThePPINPlug;
static	short			tPlug, sampPlug, instPlug, instMenuID[ MAXINSTRPLUG], sampMenuID[ MAXINSTRPLUG];
static	MenuHandle		PPINMenuSample, PPINMenuInstrument;

static 	Boolean		showAllFiles;
static 	Boolean 	needUp;
static 	Boolean		AddAll;
static	Boolean 	PlayWhenClicked, DoPlayASoundFile;
static	Boolean		ShowAllFiles;
static	FSSpec		lastItem;


void InitPlayWhenClicked(void)
{
	PlayWhenClicked = false;
	ShowAllFiles = false;
}

OSErr PPINGetPlugByID( OSType *type, short id, short samp)
{
	if( samp >= 0)
	{
		if( id >= sampPlug) return -1;
		else
		{
			*type = ThePPINPlug[ sampMenuID[id]].type;
			return noErr;
		}
	}
	else
	{
		if( id >= instPlug) return -1;
		else
		{
			*type = ThePPINPlug[ instMenuID[id]].type;
			return noErr;
		}
	}
}

void PPINGetFileName( void)
{
	Point	where = { -1, -1};
	short	ins, samp, i;
	OSErr	iErr;
	FSSpec	replyspec;
	
	if( !GetIns( &ins, &samp))
	{
		Erreur( 13, ins);
		return;
	}
	
	needUp = false;
	AddAll = false;
	DoPlayASoundFile = true;
	
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	NavTypeListHandle	openList;
	NavEventUPP			eventUPP = NewNavEventUPP( myCustomEventNAVProc);
	NavObjectFilterUPP 	filterProcUPP = NewNavObjectFilterUPP( MyInsCustomFilter);
		
		// default behavior for browser and dialog:
	iErr = NavGetDefaultDialogOptions( &dialogOptions);
		
	dialogOptions.dialogOptionFlags	-=	kNavAllowPreviews;
	dialogOptions.dialogOptionFlags	+=	kNavNoTypePopup;
	dialogOptions.dialogOptionFlags	-=	kNavAllowStationery;
	dialogOptions.dialogOptionFlags	+=	kNavDontAutoTranslate;
	dialogOptions.dialogOptionFlags	-=	kNavAllowMultipleFiles;
		
		pStrcpy( dialogOptions.clientName, "\pPlayerPRO");
		
		/*	openList = (NavTypeListHandle) NewHandle( sizeof(NavTypeList) + (tPlug+1) * sizeof(OSType));
		 if ( openList ) HLock((Handle)openList);
		 
		 (*openList)->componentSignature	= 'SNPL';
		 (*openList)->osTypeCount			= tPlug+1;
		 (*openList)->osType[ 0]			= 'INso';
		 for( i = 1; i <= tPlug; i++) (*openList)->osType[ i] = ThePPINPlug[ i].type;*/
		
		
		iErr = NavGetFile(			NULL,	// use system's default location
						  &theReply,
						  &dialogOptions,
						  eventUPP,		//MyDlgFilterNavDesc,
						  NULL,	// no custom previews
						  filterProcUPP,
						  NULL, 		//,
						  (NavCallBackUserData) 2L);
		
		DisposeNavEventUPP( eventUPP);
		DisposeNavObjectFilterUPP( filterProcUPP);
		
		/*	if (openList != NULL)
		 {
		 HUnlock((Handle)openList);
		 DisposeHandle((Handle)openList);
		 }*/
		
		if (theReply.validRecord && iErr == noErr)
		{
			AEDesc 	resultDesc;
			long	index, count;
			
			// we are ready to open the document(s), grab information about each file for opening:
			iErr = AECountItems(&(theReply.selection),&count);
			for (index=1;index<=count;index++)
			{
				AEKeyword keyword;
				
				if ((iErr = AEGetNthDesc(&(theReply.selection), index, typeFSS, &keyword, &resultDesc)) == noErr)
				{
					if ((iErr = MyAEGetDescData ( &resultDesc, NULL, &replyspec, sizeof ( FSSpec ), NULL )) == noErr)
						
						iErr = AEDisposeDesc(&resultDesc);
				}
			}
			
			iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
		}
		else iErr = -1;
		
		if( iErr) return;
	
	
	curMusic->hasChanged = true;
	
	UpdateALLWindow();
	
	SaveUndo( UAllSamples, 0, "\pUndo 'Open instrument'");
	
	iErr = NOpenSampleInt( ins, samp, replyspec);
	
	if( iErr == -10)
	{
		RAWImportFile( &replyspec);
		return;
	}
	else MADErreur( iErr);
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}


OSErr CallPPINPlugIns( short PlugNo, OSType order, short ins, short *samp, FSSpec *spec)
{
	OSErr				myErr = noErr;
	short				fileID;
	GrafPtr				savedPort;
	Ptr					mainAddr;
	Str255				errName;
	PPInstrumentPlugin	**InstrPlugA = ThePPINPlug[PlugNo].xxxx;
	
	GetPort( &savedPort);
	fileID = CFBundleOpenBundleResourceMap( ThePPINPlug[ PlugNo].file);
	thePPInfoPlug.fileType = ThePPINPlug[ PlugNo].type;
	
	myErr = (*InstrPlugA)->InstrMain( order,
									 &curMusic->fid[ ins],
									 &curMusic->sample[ curMusic->fid[ ins].firstSample],
									 samp,
									 spec,
									 &thePPInfoPlug);
	
	curMusic->fid[ ins].firstSample =  ins * MAXSAMPLE;
	
	CFBundleCloseBundleResourceMap(ThePPINPlug[ PlugNo].file, fileID);
	SetPort( savedPort);
	
	return myErr;
}

OSErr CallPPINPlugInsOther( 		short					PlugNo,					// CODE du plug
						   OSType					order,
						   short					*samp,
						   FSSpec					*spec,
						   InstrData					*insData,
						   sData					**sampData)
{
	OSErr				myErr;
	short				fileID;
	GrafPtr				savedPort;
	Ptr					mainAddr;
	Str255				errName;
	PPInstrumentPlugin	**InstrPlugA = ThePPINPlug[PlugNo].xxxx;
	
	GetPort( &savedPort);
	fileID = CFBundleOpenBundleResourceMap( ThePPINPlug[ PlugNo].file);
	
	
	myErr = (*InstrPlugA)->InstrMain( order,
									 insData,
									 sampData,
									 samp,
									 spec,
									 &thePPInfoPlug);
	
	
	insData->firstSample =  0;
	
	CFBundleCloseBundleResourceMap(ThePPINPlug[ PlugNo].file, fileID);	
	SetPort( savedPort);
	
	return myErr;
}

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

static void MakePPINPlug(PPInstrumentPlugin **tempPPINPlug, PPINFilterInfo *ThePPINPlug, CFBundleRef tempBundle);

void InitPPINPlug(void)
{
	short		vRefNum;
	long		dirID;
	FSSpec		spec;
	CFArrayRef  PlugLocsDigital = GetDefaultPluginFolderLocations();
	CFIndex		i, x, PlugLocNums;
	
	thePPInfoPlug.RPlaySoundUPP				= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP 		= UpdateALLWindow;
	thePPInfoPlug.MyDlgFilterUPP			= MyDlgFilterDesc;	
		
	ThePPINPlug = (PPINFilterInfo*) MyNewPtr( MAXINSTRPLUG * sizeof( PPINFilterInfo));
	
//	GetApplicationPackageFSSpecFromBundle( &spec);
	
	tPlug		= 0;

	PPInstrumentPlugin** tempMADPlug = NULL;
	PlugLocNums = CFArrayGetCount(PlugLocsDigital);
	for (i=0; i < PlugLocNums; i++) {
		CFIndex		PlugNums;
		CFArrayRef	somePlugs;
		CFURLRef	aPlugLoc;
		aPlugLoc = CFArrayGetValueAtIndex(PlugLocsDigital, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, NULL);
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				CFPlugInRef tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				
				tempMADPlug = PPINLoadPlug(tempPlugRef);
				if (tempMADPlug) {
					MakePPINPlug(tempMADPlug, &(ThePPINPlug[tPlug]), tempBundleRef);
				}
			}
		}
		CFRelease(somePlugs);
	}
	CFRelease(PlugLocsDigital);
	
	InitPPINMenu();
}

//TODO: close PPDG Plugins?
//If we don't, memory leaks! But The OS should take care of it when the program quits.
//I think.

OSType PressPPINMenu( Rect *PopUpRect, OSType curType, short samp, Str255 name)
{
	long		mresult;
	Point		Zone;
	short		i, sel;
	MenuHandle	PPINMenu;
	
	if( samp < 0) PPINMenu = PPINMenuInstrument;
	else PPINMenu = PPINMenuSample;
	
	InsertMenu( PPINMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal( &Zone);
	
	sel = 1;
	for(i=0; i< CountMenuItems( PPINMenu); i++)
	{
		if( samp < 0)
		{
			if( ThePPINPlug[ instMenuID[i]].type == curType) sel = i+1;
		}
		else if( ThePPINPlug[ sampMenuID[i]].type == curType) sel = i+1;
	}
	
	SetItemMark( PPINMenu, sel, 0xa5);
	mresult = PopUpMenuSelect(	PPINMenu,
							  Zone.v,
							  Zone.h,
							  sel );
	SetItemMark( PPINMenu, sel, 0);
	
	DeleteMenu( GetMenuID( PPINMenu));
	
	if ( HiWord( mresult ) != 0)
	{
		if( samp < 0)
		{
			Str255 pMenuName;
			GetPStrFromCFString(ThePPINPlug[ instMenuID[ LoWord( mresult)-1]].MenuName, pMenuName);

			pStrcpy( name, pMenuName);
			return ThePPINPlug[ instMenuID[ LoWord( mresult)-1]].type;
		}
		else
		{
			Str255 pMenuName;
			GetPStrFromCFString(ThePPINPlug[ sampMenuID[ LoWord( mresult)-1]].MenuName, pMenuName);

			pStrcpy( name, pMenuName);
			return ThePPINPlug[ sampMenuID[ LoWord( mresult)-1]].type;
		}
	}
	else return '\?\?\?\?';
}

OSErr PPINAvailablePlug( OSType kindFile, OSType *plugType)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			if( plugType != NULL)
			{
				if (ThePPINPlug[ i].isSamp == TRUE) {
					*plugType = 'SAMP';
				}
				else {
					*plugType = 'INST';
				}
			}
			return noErr;
		}
	}
	
	if( plugType != NULL) *plugType = 'NONE';
	return MADCannotFindPlug;
}

OSErr PPINImportFile( OSType kindFile, short ins, short *samp, FSSpec *AlienFile)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			return( CallPPINPlugIns( i, 'IMPL', ins, samp, AlienFile));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPINExportFile( OSType kindFile, short ins, short samp, FSSpec *AlienFile)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			return( CallPPINPlugIns( i, 'EXPL', ins, &samp, AlienFile));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPINTestFile( OSType kindFile, FSSpec *AlienFile)
{
	short		i, temp;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			return( CallPPINPlugIns( i, 'TEST', 0, &temp, AlienFile));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPINIdentifyFile( OSType *type, FSSpecPtr AlienFile)
{
	short		i, refNum, temp;
	OSErr		iErr;
	
	*type = '!!!!';
	
	for( i = 0; i < tPlug; i++)
	{
		if( CallPPINPlugIns( i, 'TEST', 0, &temp, AlienFile) == noErr)
		{
			*type = ThePPINPlug[ i].type;
			return noErr;
		}
	}
	
	*type = '!!!!';
	return MADCannotFindPlug;
}

void InitPPINMenu(void)
{
	short	i;
	
	PPINMenuSample = GetMenu( 166);
	PPINMenuInstrument = GetMenu( 167);
	
	sampPlug = instPlug = 0;
	
	for(i=0; i< tPlug; i++)
	{
		if( ThePPINPlug[ i].mode == 'EXIM' || ThePPINPlug[ i].mode == 'EXPL')
		{
			Str255 pMenuName;
			GetPStrFromCFString(ThePPINPlug[ i].MenuName, pMenuName);

			if( ThePPINPlug[ i].isSamp)
			{
				AppendMenu( PPINMenuSample, pMenuName);
				sampMenuID[ sampPlug] = i;
				sampPlug++;
			}
			else
			{
				AppendMenu( PPINMenuInstrument, pMenuName);
				instMenuID[ instPlug] = i;
				instPlug++;
			}
		}
	}
}

void GetSampInstPPIN( short *samp, short *ins)
{
	*samp = sampPlug;
	*ins = instPlug;
}

short PPINGetNo()
{
	return tPlug;
}

OSErr PPINGetPlugName( OSType kindFile, Str255 name)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[i].type)
		{
			Str255 pMenuName;
			GetPStrFromCFString(ThePPINPlug[i].MenuName, pMenuName);

			pStrcpy( name, pMenuName);
			return noErr;
		}
	}
	return MADCannotFindPlug;
}

OSErr PPINPlayFile( OSType kindFile, FSSpec *AlienFile)
{
	short		i, temp;
	InstrData		*insData;
	sData		**sampData;
	OSErr		err = -1;
	
	sampData = (sData**) NewPtrClear( sizeof( sData*) * (long) MAXSAMPLE);
	if( sampData == NULL) return -1;
	
	insData = (InstrData*) NewPtrClear( sizeof( sData*) * (long) MAXSAMPLE);
	if( insData == NULL) return -1;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			if( PPINTestFile( kindFile, AlienFile) == noErr)
			{
				temp = -1;
				err = CallPPINPlugInsOther( i, 'IMPL', &temp, AlienFile, insData, sampData);
				goto ENDPLAY;
			}
		}
	}
ENDPLAY:
	
	if( err == noErr)
	{
#define	NOTE			48
		short		track, samp = insData->what[ NOTE];
		Channel		*curVoice;
		Boolean		continueLoop;
		KeyMap		km;
		
		track = GetWhichTrackPlay();
		curVoice = &MADDriver->chan[ track];
		
		DoPlayInstruInt2( sampData[ samp], insData, NOTE, -1, 0x00, 0x00, 64, curVoice, 0, 0, 0xFF);
		
		SetCursor( &PlayCrsr);
		
		while( Button()) {};
		
		continueLoop = true;
		while( continueLoop)
		{
			GetKeys( km);
			
			if( MADDriver->chan[ track].maxPtr <= MADDriver->chan[ track].curPtr || MADDriver->chan[ track].curPtr == NULL) continueLoop = false;
			if( MADIsPressed( (unsigned char*) km, 0x37) && MADIsPressed( (unsigned char*) km, 0x2F)) continueLoop = false;
			if( Button()) continueLoop = false;
			
			DoGlobalNull();
		}
		
		SetCursor( GetQDGlobalsArrow( &qdarrow));
		
		MADPurgeTrack( MADDriver);
		
		for( i = 0; i < insData->numSamples; i++)
		{
			if( sampData[ i] != NULL)
			{
				if( sampData[ i]->data != NULL) DisposePtr( (Ptr) sampData[ i]->data);
				DisposePtr( (Ptr) sampData[ i]);
				sampData[ i] = NULL;
			}
		}
	}
	
	DisposePtr( (Ptr) sampData);
	DisposePtr( (Ptr) insData);
	
	return err;
}

pascal void myCustomEventNAVProc(	NavEventCallbackMessage 	callBackSelector, 
								 NavCBRecPtr 				callBackParms, 
								 NavCallBackUserData 		callBackUD)
{
	OSErr		theErr = noErr;
	short 		index = 0;
	FSSpec		spec;
	
	//	if( callBackUD != 2) return;
	
	switch (callBackSelector)
	{
		case kNavCBEvent:
			switch (callBackParms->eventData.eventDataParms.event->what)
		{
			case kHighLevelEvent:
				AEProcessAppleEvent( callBackParms->eventData.eventDataParms.event);
				break;
				
			case nullEvent:
				DoGlobalNull();
				
				if( DoPlayASoundFile == false)
				{
					if( ExtractFile( callBackParms, &spec) == noErr)
					{
						FInfo		fndrInfo;
						
						if( FSpGetFInfo( &spec, &fndrInfo) == noErr)
						{
							PPINPlayFile( fndrInfo.fdType, &spec);
						}
					}
					
					DoPlayASoundFile = true;
				}
				break;
				
			case updateEvt:
				if( (WindowPtr) callBackParms->eventData.eventDataParms.event->message == callBackParms->window)
				{
					
				}
				else
				{
					EventRecord *event = callBackParms->eventData.eventDataParms.event;
					GrafPtr		savedPort;
					Rect			caRect;
					
					GetPort( &savedPort);
					SetPortWindowPort( (WindowPtr) event->message);
					GetPortBounds( GetWindowPort( (WindowPtr) event->message), &caRect);
					
					InvalWindowRect( (WindowPtr) event->message, &caRect);
					SetPort( savedPort);
					
					DoUpdateEvent( callBackParms->eventData.eventDataParms.event);
				}
				break;
				
			case mouseDown:
				HandleCustomMouseNAVDown( callBackParms);
				break;
				
			default:
				break;
		}
			break;
			
		case kNavCBSelectEntry:
			if( PlayWhenClicked == true)
			{
				DoPlayASoundFile = false;
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
				if( theErr == noErr)
				{
					TurnRadio( 1 + firstItem, GetDialogFromWindow( callBackParms->window), PlayWhenClicked);
					TurnRadio( 2 + firstItem, GetDialogFromWindow( callBackParms->window), ShowAllFiles);
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
					
					InvalWindowRect( callBackParms->window, &callBackParms->customRect);
					
					myCustomEventNAVProc( kNavCBAdjustRect, callBackParms, callBackUD);
				}
			break;
		}
			
		case kNavCBAccept:
			break;
			
		case kNavCBTerminate:
			// release our appended popup menu:
			if (gDitlList)
				ReleaseResource(gDitlList);	
			break;
	}
}

pascal Boolean MyInsCustomFilter( AEDesc *theItem, void *info, NavCallBackUserData callBackUD, NavFilterModes filterMode)
{
	Boolean		ready = false;
	short		i;
	FSSpec		spec;
	OSType		type;
	char		tempC[ 5];
	FInfo		fndrInfo;
	
	if( MyAEGetDescData ( theItem, NULL, &spec, sizeof ( FSSpec ), NULL ) == noErr)
	{
		if( FSpGetFInfo( &spec, &fndrInfo) == noErr)
		{
			if( ShowAllFiles) return true;
			else
			{
				for( i = 1; i <= tPlug; i++) if( fndrInfo.fdType == ThePPINPlug[ i].type) return true;
				
				return false;
			}
		}
	}
	
	return true;
}

// *****************************************************************************
// *
// *	HandleCustomMouseDown()	
// *
// *****************************************************************************
void HandleCustomMouseNAVDown(NavCBRecPtr callBackParms)
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
	AEDesc 			selectionList;
	
	GlobalToLocal(&where);
	theItem = FindDialogItem( GetDialogFromWindow( callBackParms->window),where);	// get the item number of the control
	partCode = FindControl(where,callBackParms->window,&whichControl);	// get the control itself
	
	// ask NavServices for the first custom control's ID:
	if (callBackParms->context != 0)	// always check to see if the context is correct
	{
		theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);	
		realItem = theItem - firstItem + 1;		// map it to our DITL constants:	
	}
	
	switch( realItem)
	{
		case 1:
			InverseRadio( firstItem + 1, GetDialogFromWindow( callBackParms->window));
			PlayWhenClicked = !PlayWhenClicked;
			break;
			
		case 2:
			InverseRadio( firstItem + 2, GetDialogFromWindow( callBackParms->window));
			ShowAllFiles = !ShowAllFiles;
			
			theErr = NavCustomControl(callBackParms->context, kNavCtlGetLocation, &selectionList);
			theErr = NavCustomControl(callBackParms->context, kNavCtlSetLocation, &selectionList);
			break;
	}
}

static void CFStringToOSType(CFStringRef CFstri, OSType *theOSType)
{
	char * thecOSType = CFStringGetCStringPtr(CFstri, kCFStringEncodingMacRoman);
	
	*theOSType = Ptr2OSType(thecOSType);
}

static const CFStringRef kMadPlugIsSampleKey = CFSTR("MADPlugIsSample");

static void MakePPINPlug(PPInstrumentPlugin **tempPPINPlug, PPINFilterInfo *ThePPINPlugA, CFBundleRef tempBundle)
{
	if( tPlug > MAXINSTRPLUG) 
	{
		MyDebugStr( __LINE__, __FILE__, "Too many plugs");
		return;
	}
	//TODO: There's got to be a cleaner way...
	
	CFTypeID  stringtype	= CFStringGetTypeID();
	CFTypeID  numbertype	= CFNumberGetTypeID();
	CFTypeID  arraytype		= CFArrayGetTypeID();
	CFTypeID  InfoDictionaryType;
	CFTypeRef OpaqueDictionaryType = NULL;
	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugAuthorNameKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == stringtype) {
		ThePPINPlugA->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
	}
	else goto badplug;

	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugMenuNameKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == stringtype) {
		ThePPINPlugA->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
	}
	else goto badplug2;

	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugUTITypesKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == arraytype) {
		ThePPINPlugA->UTITypes = CFArrayCreateCopy(kCFAllocatorDefault, (CFArrayRef)OpaqueDictionaryType);
	}		
	else if(InfoDictionaryType == stringtype)
	{
		CFMutableArrayRef UTIMutableArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
		CFArrayAppendValue(UTIMutableArray, (CFStringRef)InfoDictionaryType);
		ThePPINPlugA->UTITypes = CFArrayCreateCopy(kCFAllocatorDefault, UTIMutableArray);
		CFRelease(UTIMutableArray);
	}
	else goto badplug3;
	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugIsSampleKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == CFBooleanGetTypeID()) {
		ThePPINPlugA->isSamp = CFBooleanGetValue((CFBooleanRef)OpaqueDictionaryType);
	}
	else if(InfoDictionaryType == numbertype)
	{
		char theplugType;
		CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberCharType, &theplugType);
		if (theplugType == 1) {
			ThePPINPlugA->isSamp = TRUE;
		}
		else {
			ThePPINPlugA->isSamp = FALSE;
		}
		
	}
	else if(InfoDictionaryType == stringtype)
	{
		CFMutableStringRef tempCFStr = CFStringCreateMutableCopy(kCFAllocatorDefault, 25, (CFStringRef)InfoDictionaryType);
		CFLocaleRef theLoc = CFLocaleCopyCurrent();
		CFStringLowercase(tempCFStr, theLoc);
		if (CFEqual(tempCFStr, CFSTR("yes")) || CFEqual(tempCFStr, CFSTR("1"))) {
			ThePPINPlugA->isSamp = TRUE;
		}
		else {
			ThePPINPlugA->isSamp = FALSE;
		}
		CFRelease(tempCFStr);
		CFRelease(theLoc);
	}
	else goto badplug4;
	
	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugTypeKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == stringtype) {
		CFStringToOSType((CFStringRef)OpaqueDictionaryType, &ThePPINPlugA->type);
	}
	else goto badplug4;

	
	
	OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugModeKey);
	InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
	if (InfoDictionaryType == stringtype) {
		CFStringToOSType((CFStringRef)OpaqueDictionaryType, &ThePPINPlugA->mode);
	}
	else goto badplug4;
	
	ThePPINPlugA->xxxx = tempPPINPlug;
	ThePPINPlugA->file = tempBundle;
	CFRetain(ThePPINPlugA->file);
	
	
	tPlug++;
	
	return;
	
badplug4:
	CFRelease(ThePPINPlugA->UTITypes);
badplug3:
	CFRelease(ThePPINPlugA->MenuName);
badplug2:
	CFRelease(ThePPINPlugA->AuthorString);
badplug:
	
	(*tempPPINPlug)->Release(tempPPINPlug);
	NSLog(CFSTR("Error with plug-in"));

	return;
}
