#include <Carbon/Carbon.h>
#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "Utils.h"

#define ON	true
#define OFF	false

/******** HELP MODULE ********/
enum
{
	HNew	= 7,
	HLoad	= 3,
	HSave	= 4,
	HDel	= 5,
	HInfo	= 6,
	HOpenA	= 8,
	HList	= 1
};

static pascal Boolean myDragClickLoop(void);
pascal OSErr MyPATTrackingHandler(short, WindowPtr, void *, DragReference);
pascal OSErr MyPATReceiveDropHandler(WindowPtr, void*, DragReference);
pascal OSErr MyPATSendDataProc(FlavorType, void *, ItemReference, DragReference);

pascal OSErr MyPATTrackingHandler(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr MyPATReceiveDropHandler(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
pascal OSErr MyPATSendDataProc(FlavorType theFlavor, void *refCon, ItemReference theItem, DragReference theDrag);
static short AHelp[] = {HNew, HLoad, HSave, HDel, HInfo, HOpenA, HList};

void DoHelpPatList(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/


extern	EventRecord				theEvent;
extern	DialogPtr				EditorDlog, ClassicDlog;
extern	RGBColor				theColor;
extern	Boolean					DragManagerUse;

		DialogPtr				PatListDlog;
		
static 	ListHandle				PatList2;
static	Rect					PatRectList2, CurRect, CurChar;
static	short					PatCopy, ReaderCopy, HCell, LChar;
static	short					CurrentPat, CharNo;
static	short					DragPatSource;
static	long 					lastWhen = 0;
static	ControlHandle			myListControl, InfoBut, LoadBut, DelBut, SaveBut, NewBut, OpenBut;
static	Boolean					canAcceptDrag;

static	DragSendDataUPP			mySendDataUPP;
static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

void TurnRadio(short	item, DialogPtr	dlog, Boolean alors);
void SetPatList2(ListHandle	theList);
Boolean DialogPatternInfo(short);
Boolean DragPatternFunction(RgnHandle myRgn, short theNo, EventRecord *theEvent);
void LoadAPatternInt(FSSpec sFile, short selectedPos);
void SaveAPatternInt(FSSpec	sFile, short theID);
void DeleteAPattern(Boolean AskDelete);
void DuplicatePattern(short);
short GetPatternSelect();

static Boolean firstCall;

static pascal Boolean myDragClickLoop(void)
{
	Point		aPt, theCell;
	Rect		cellRect;
	RgnHandle	tempRgn;

	if (DragManagerUse)
	{
		if (!firstCall)
		{
			theCell.v = 0;	theCell.h = 0;
			if (LGetSelect(true, &theCell, PatList2))
			{
				if (WaitMouseMoved(theEvent.where))
				{
					LRect(&cellRect, theCell, PatList2);
					
					tempRgn = NewRgn();
					RectRgn(tempRgn, &cellRect);
					
					GetMouse(&aPt);
					LocalToGlobal(&aPt);
					theEvent.where = aPt;
					
					DragPatternFunction(tempRgn, theCell.v, &theEvent);
					
					DisposeRgn(tempRgn);
					
					return false;
				}
			}
		}
		else firstCall = false;
	}
	return(true);
}


/***********************************************************************************************************/
/************************************************           ************************************************/
/************************************************ RECEPTION ************************************************/
/************************************************           ************************************************/
/***********************************************************************************************************/

static	ControlHandle	selectedControl;

pascal OSErr MyPATTrackingHandler(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{	
	short				offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;
	short           	items;
	FlavorFlags     	theFlags;
	OSErr           	result;
	long				textSize;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;

	if (!mainSystemDrag) return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return(noErr);

	SetPortWindowPort(theWindow);
	BackColor(whiteColor);
	
	GetDragAttributes(theDrag, &attributes);

	switch (message)
	{
		case kDragTrackingEnterHandler:
			canAcceptDrag = false;
		
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
			result = GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags);
			if (result == noErr)
			{
				Boolean		targetIsFolder, wasAliased;
			
				GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
				
				GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		
				ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
				HSetVol(NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
				FSpGetFInfo(&myFlavor.fileSpec, &fndrInfo);
				
				if (fndrInfo.fdType == 'PATN') canAcceptDrag = true;
		    }
		    
		    if (attributes & kDragInsideSenderWindow) canAcceptDrag = true;
			break;

		case kDragTrackingEnterWindow:
		
			break;

		case kDragTrackingInWindow:
			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);

			selectedControl = NULL;

			if (attributes & kDragInsideSenderWindow)
			{
				Rect contrlRect;
				
				HideDragHilite(theDrag);
				
				GetControlBounds(SaveBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl(SaveBut, kControlButtonPart);	selectedControl = SaveBut;}
				else HiliteControl(SaveBut, 0);
				
				GetControlBounds(InfoBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl(InfoBut, kControlButtonPart);	selectedControl = InfoBut;}
				else HiliteControl(InfoBut, 0);
				
				GetControlBounds(DelBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) 	{ HiliteControl(DelBut, kControlButtonPart);		selectedControl = DelBut;}
				else HiliteControl(DelBut, 0);
				
				GetControlBounds(OpenBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl(OpenBut, kControlButtonPart);	selectedControl = OpenBut;}
				else HiliteControl(OpenBut, 0);
				
				GetControlBounds(LoadBut, &contrlRect);
				
				if (PtInRect(localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl(LoadBut, kControlButtonPart);	selectedControl = LoadBut;}
				else HiliteControl(LoadBut, 0);
			}
			else if (PtInRect(localMouse, &PatRectList2))
			{
				PatRectList2.right -= 15;
				
				RectRgn(theRgn = NewRgn(), &PatRectList2);
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
				
				PatRectList2.right += 15;
			}
			break;

		case kDragTrackingLeaveWindow:
			HideDragHilite(theDrag);
			break;

		case kDragTrackingLeaveHandler:
		
			break;
	}
	RGBBackColor(&theColor);

	return(noErr);
}

pascal OSErr MyPATReceiveDropHandler(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	FSSpec				pseudoFF;
	HFSFlavor			myFlavor;
	OSErr				result;
	Handle				mySnd;
	Rect				theRect, srcRect;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Ptr					textData;
	StScrpHandle		stylHandle;
	Size				textSize;
	short				offset, selStart, selEnd, mouseDownModifiers, mouseUpModifiers, moveInstru;
	Point				theCell;
	Str255				myStr;

	if (!mainSystemDrag) return dragNotAcceptedErr;

	SetPortWindowPort(theWindow);
	if (!canAcceptDrag) return(dragNotAcceptedErr);

	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);

	if (selectedControl != NULL)	// Drag sur un bouton !!!!
	{
		HiliteControl(selectedControl, 0);
		
		if (selectedControl == SaveBut)		HandlePatternChoice(6);
		if (selectedControl == InfoBut)		DialogPatternInfo(GetPatternSelect());
		if (selectedControl == DelBut)		HandlePatternChoice(8);
		if (selectedControl == LoadBut)		HandlePatternChoice(5);
		if (selectedControl == OpenBut)
		{
			theCell.v = theCell.h = 0;
			if (LGetSelect(true, &theCell, PatList2))
			{
			/*	MADDriver->Pat = theCell.v;
				MADDriver->PartitionReader = 0;
				MADPurgeTrack(MADDriver);*/
					
				if (ClassicDlog != NULL) SelectWindow2(GetDialogWindow(ClassicDlog));
				else CreateClassicWindow();
			}
		}

		selectedControl = NULL;
		
		return noErr;
	}

	moveInstru = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	if (moveInstru) return noErr;

	BackColor(whiteColor);

	HideDragHilite(theDrag);
	
	RGBBackColor(&theColor);

	curMusic->hasChanged = true;
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);

	if (result == noErr)
	{
		Boolean		targetIsFolder, wasAliased;
	
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		
		ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		SaveUndo(UAllPatterns, 0, "\pUndo 'Drop pattern file'");

		LoadAPatternInt(myFlavor.fileSpec, curMusic->header->numPat);
	
		UpdatePatListInfo();
		UpdatePartiInfo();
	
		BackColor(whiteColor);
		theCell.v = 0;	theCell.h = 0;
		while (LGetSelect(true, &theCell, PatList2)) LSetSelect(false, theCell, PatList2);
		theCell.v = curMusic->header->numPat - 1;
		LSetSelect(true, theCell, PatList2);
		LAutoScroll(PatList2);
		RGBBackColor(&theColor);
		
		HiliteControl(InfoBut, 0);
		HiliteControl(DelBut, 0);
		HiliteControl(SaveBut, 0);
		HiliteControl(OpenBut, 0);
		
		return(noErr);
	}

	return(dragNotAcceptedErr);
}

pascal OSErr MyPATSendDataProc(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag)
{
	AEDesc			dropLoc;
	FSSpec			target;
	OSErr			err;
	Boolean			wasChanged;
	OSType			compressionType;
	short			SoundType, i, x;
	Handle			theSound;
	long			SoundSize;
	Str255			aStr;
	Str255			bStr;
	WDPBRec			wdpb;

	if (theFlavor == 'VCT2')
	{
		err = GetDropLocation(theDrag, &dropLoc);
		if (err) return (err);

		err = ResolveAlias(nil, (AliasHandle) dropLoc.dataHandle, &target, &wasChanged);
		if (err) return (err);
		
		AEDisposeDesc(&dropLoc);
		
		PathNameFromDirIDTRUE(		target.parID,
									target.vRefNum,
									aStr);
									
		pStrcat(aStr, "\p:");
		pStrcat(aStr, target.name);
		pStrcat(aStr, "\p:");
		
		wdpb.ioNamePtr = aStr;
		PBHSetVolSync(&wdpb);
		HGetVol(NULL, &target.vRefNum, &target.parID);
		
		bStr[ 0] = 20;
		for (x = 0; x < 20; x++) bStr[ x + 1] = curMusic->partition[ DragPatSource]->header.name[ x];
		for (x = 1; x < 20; x++) if (bStr[ x] == 0) { bStr[ 0] = x - 1; break;}

		if (bStr[ 0] == 0)
		{
			NumToString(DragPatSource, aStr);
			pStrcpy(bStr, "\pPattern #");
			pStrcat(bStr, aStr);
		}
		FSMakeFSSpec(target.vRefNum, target.parID, bStr, &target);

		HSetVol(NULL, target.vRefNum, target.parID);
		
		SaveAPatternInt(target, DragPatSource);

		err = SetDragItemFlavorData(theDrag, theItem, theFlavor, &target, sizeof(target), 0);
		if (err) return (err);
	}

	return (noErr);
}

Boolean DragPatternFunction(RgnHandle myRgn, short theNo, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	StScrpHandle		theStyl;
	AEDesc				dropLocation;
	DragAttributes		attributes;
	short				mouseDownModifiers, mouseUpModifiers, copyText;
	long				inOutBytes;
	short				temp;
	Handle				theSound;
	Str255				theStr;
	PromiseHFSFlavor	myNewFile;
	FSSpec				mySpec;
	Rect				dragRegionRect;
	
	if (!DragManagerUse) return false;

	//******************************************
	// Prepare la pattern pour etre exporte !!
	//******************************************

	DragPatSource = theNo;

	CopyRgn(myRgn, dragRegion = NewRgn());
	SetPt(&theLoc, 0, 0);
	LocalToGlobal(&theLoc);
	OffsetRgn(dragRegion, theLoc.h, theLoc.v);

	NewDrag(&theDrag);
	
	myNewFile.fileType			=	'PATN';
	myNewFile.fileCreator		=	'SNPL';
	myNewFile.fdFlags			=	0;
	myNewFile.promisedFlavor	=	'VCT2';
	AddDragItemFlavor(theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof(myNewFile), flavorNotSaved);
	AddDragItemFlavor(theDrag, 1, 'VCT2', NULL, 0, 0);

	result = SetDragSendProc(theDrag, mySendDataUPP, NULL);

	SetDragItemBounds(theDrag, 1, GetRegionBounds(dragRegion, &dragRegionRect));

	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);

	result = TrackDrag(theDrag, theEvent, dragRegion);

	if (result != noErr && result != userCanceledErr)
	{
		return(true);
	}

	GetDragAttributes(theDrag, &attributes);
	if (!(attributes & kDragInsideSenderApplication))
	{
		GetDropLocation(theDrag, &dropLocation);

		GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
		copyText = (mouseDownModifiers | mouseUpModifiers) & optionKey;

		if ((!copyText) && (DropLocationIsFinderTrash(&dropLocation)))
		{
		//	DeleteAPattern(true);
		}
		else
		{
		}

		AEDisposeDesc(&dropLocation);
	}

	//
	//	Dispose of the drag.
	//

	DisposeDrag(theDrag);

	DisposeRgn(dragRegion);

	return(true);
}

void ActivePatternMenu(Boolean Activ)
{
	if (Activ)
	{
		EnableMenuItem(PatternEditMenu, 4);
		EnableMenuItem(PatternEditMenu, 5);
		EnableMenuItem(PatternEditMenu, 6);
		EnableMenuItem(PatternEditMenu, 7);
		EnableMenuItem(PatternEditMenu, 8);
		
		SetItemMark(PatternEditMenu, 2, checkMark);
	}
	else
	{
		DisableMenuItem(PatternEditMenu, 4);
		DisableMenuItem(PatternEditMenu, 5);
		DisableMenuItem(PatternEditMenu, 6);
		DisableMenuItem(PatternEditMenu, 7);
		DisableMenuItem(PatternEditMenu, 8);
		
		SetItemMark(PatternEditMenu, 2, noMark);
	}
}

void DeleteAPattern(Boolean AskDelete)
{
	long				oldSize, newSize;
	Boolean				MusiqueOn = false;
	short				i;
	Point				theCell = { 0, 0};

	if (!LGetSelect(true, &theCell, PatList2)) return;

	if (curMusic->header->numPat <= 1)
	{
		Erreur(49, -434);
		return;
	}
	
	if (AskDelete) if (InfoL(50) == false) return;

	SaveUndo(UAllPatterns, 0, "\pUndo 'Delete Pattern'");

	curMusic->hasChanged = true;
	if (MADDriver->Reading == true) MusiqueOn = true;
	MADDriver->Reading = false;
	
	/***** Update la pattern list ******/
	
	theCell.h = 0;
	for (theCell.v = (*PatList2)->dataBounds.bottom-1; theCell.v >= 0; theCell.v--)
	{
		if (curMusic->header->numPat > 1)
		{
			if (LGetSelect(false, &theCell, PatList2))
			{
				curMusic->header->numPat--;
				
				MyDisposePtr((Ptr*) &  curMusic->partition[ theCell.v]);
				curMusic->partition[ theCell.v] = NULL;
			
				for (i = theCell.v; i < curMusic->header->numPat; i++)
				{
					curMusic->partition[ i] = curMusic->partition[ i + 1];
				}
				for (i = curMusic->header->numPat; i < MAXPATTERN; i++) curMusic->partition[ i] = NULL;
				
				/***** Update la partition list ******/
				for (i = 0; i < 128; i++)
				{
					if (curMusic->header->oPointers[ i] > theCell.v) curMusic->header->oPointers[ i]--;
					else if (curMusic->header->oPointers[ i] == theCell.v) curMusic->header->oPointers[ i] = 0;
				}
				LSetSelect(false, theCell, PatList2);
			}
		}
	}
	MADReset(MADDriver);
	
	if (MusiqueOn)
		MADDriver->Reading = true;
	
	UpdateEditorInfo();
	UpdatePatListInfo();
	UpdatePartiInfo();
}

void SaveAPatternInt(FSSpec	sFile, short theID)
{
	short	x, fRefNum;
	OSErr	iErr;
	long	inOutBytes;

	FSpDelete(&sFile);
	iErr = FSpCreate(&sFile, 'SNPL', 'PATN', smSystemScript);
	if (iErr == noErr) {
		iErr = FSpOpenDF(&sFile, fsCurPerm, &fRefNum);
		
		for (x = 0; x < sFile.name[0]; x++)
			curMusic->partition[theID]->header.name[x] = sFile.name[x + 1];
		for (x = sFile.name[0]; x < 20; x++)
			curMusic->partition[theID]->header.name[x] = 0;
		
		inOutBytes = GetPtrSize((Ptr)curMusic->partition[theID]);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, curMusic->partition[theID], NULL);
		iErr = FSCloseFork(fRefNum);
	}

	UpdatePatListInfo();
	UpdatePartiInfo();
}

void SaveAPattern(void)
{
	short				itemType,i, x, selectedPos, thePos;
	long				inOutBytes, Id;
	Rect				itemRect;
	Point				where = { -1, -1}, theCell ={ 0, 0};
	OSErr				iErr;
	GrafPtr				myPort;
	SFTypeList 			typeList;
	FSSpec				spec;
	Ptr					tempPtr;
	short				fRefNum;
	Str255				theStr;
	
	if (LGetSelect(true, &theCell, PatList2))
		thePos = theCell.v;
	else
		return;
	
	theStr[ 0] = 20;
	for (x = 0; x < 20; x++)
		theStr[ x + 1] = curMusic->partition[ thePos]->header.name[ x];
	for (x = 1; x < 20; x++) {
		if (theStr[x] == 0) {
			theStr[0] = x - 1;
			break;}
	}
	
	if (DoCustomSave("\pSave this pattern as:", theStr, 'PATN', &spec)) return;
	
	iErr = HSetVol(NULL, spec.vRefNum, spec.parID);
	
	SaveAPatternInt(spec, thePos);
	
	UpdatePatListInfo();
	UpdatePartiInfo();
}

void LoadAPatternInt(FSSpec sFile, short selectedPos)
{
	short					iFileRefI, tracksNo;
	long					inOutBytes;
	PatData					*theNewPattern;
	Boolean					readingActif = MADDriver->Reading;
	
	MADDriver->Reading = false;
	
	//HSetVol(NULL, sFile.vRefNum, sFile.parID);
	
	FSpOpenDF(&sFile, fsCurPerm, &iFileRefI);
	GetEOF(iFileRefI, &inOutBytes);
	
	if (selectedPos < curMusic->header->numPat) MyDisposePtr((Ptr*) &curMusic->partition[ selectedPos]);
	
	theNewPattern = (PatData*) MyNewPtr(inOutBytes);
	
	FSRead(iFileRefI, &inOutBytes, theNewPattern);
	FSCloseFork(iFileRefI);
	
	/****/
	
	inOutBytes = (long) theNewPattern + (long) GetPtrSize((Ptr) theNewPattern) - (long) &theNewPattern->Cmds;
	tracksNo = inOutBytes / (sizeof(Cmd) * theNewPattern->header.size);
	
	if (tracksNo == curMusic->header->numChn)
	{
		curMusic->partition[ selectedPos] = theNewPattern;
	}
	else
	{
		short			x, z;
		Cmd				*inACmd, *outACmd;
		
		curMusic->partition[ selectedPos] = (PatData*) MyNewPtr(sizeof(PatHeader) + curMusic->header->numChn * theNewPattern->header.size * sizeof(Cmd));
		curMusic->partition[ selectedPos]->header.size = theNewPattern->header.size;
		curMusic->partition[ selectedPos]->header.compMode = 'NONE';
		BlockMoveData(theNewPattern->header.name, curMusic->partition[ selectedPos]->header.name, 20);
		curMusic->partition[ selectedPos]->header.patBytes = theNewPattern->header.size * sizeof(Cmd) * curMusic->header->numChn;
		curMusic->partition[ selectedPos]->header.unused2 = 0;
		
		for (x = 0; x < theNewPattern->header.size; x++)
		{
			for (z = 0; z < curMusic->header->numChn; z++)
			{
				outACmd = GetMADCommand(x, z, curMusic->partition[ selectedPos]);
				
				if (z < tracksNo)
				{
					inACmd = GetMADCommand(x, z, theNewPattern);
					*outACmd = *inACmd;
				}
				else
				{
					MADKillCmd(outACmd);
				}
			}
		}
	}
	
	/****/
	
	if (selectedPos == curMusic->header->numPat) curMusic->header->numPat++;
	
	MADDriver->Reading = readingActif;
}

/*OSErr LoadAPatternNav(FSSpec	*spec)
{
	NavReplyRecord			theReply;
	NavDialogOptions		dialogOptions;
	NavTypeListHandle		openList;
	OSErr					iErr;
	
	// default behavior for browser and dialog:
	iErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	dialogOptions.dialogOptionFlags	-=	kNavAllowPreviews;
	dialogOptions.dialogOptionFlags	+=	kNavNoTypePopup;
	dialogOptions.dialogOptionFlags	-=	kNavAllowStationery;
	dialogOptions.dialogOptionFlags	+=	kNavDontAutoTranslate;
	dialogOptions.dialogOptionFlags	-=	kNavAllowMultipleFiles;
	
	pStrcpy(dialogOptions.clientName, "\pPlayerPRO");
	
	openList = (NavTypeListHandle) NewHandle(sizeof(NavTypeList) + 1 * sizeof(OSType));
	if (openList ) HLock((Handle)openList);
	
	(*openList)->componentSignature = 'SNPL';
	(*openList)->osTypeCount		= 1;
	(*openList)->osType[ 0] 		= 'PATN';
	
	iErr = NavGetFile(			NULL,	// use system's default location
								&theReply,
								&dialogOptions,
								MyDlgFilterNavDesc,
								NULL,	// no custom previews
								NULL,
								openList, //,
								(NavCallBackUserData) 2L);
	
	if (openList != NULL)
	{
		HUnlock((Handle)openList);
		DisposeHandle((Handle)openList);
	}
	
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
	
	return iErr;
}*/

void LoadAPattern(void)
{
	short				itemType,i, theNo, selectedPos;
	long				inOutBytes, Id;
	Rect				itemRect;
	Point				where = { -1, -1}, theCell ={ 0, 0};
	OSErr				iErr;
	GrafPtr				myPort;
	FSSpec				spec;
	Ptr					tempPtr;
	
	selectedPos = curMusic->header->numPat;
	
	GetPort(&myPort);
	
	if (DoStandardOpen(&spec, "\ppattern file", 'PATN')) return;
	
	curMusic->hasChanged = true;
	SetCursor(&watchCrsr);
	
	SaveUndo(UAllPatterns, 0, "\pUndo 'Open Pattern'");
	
	LoadAPatternInt(spec, selectedPos);
	
	/** Select loaded pattern **/
	
	theCell.h = theCell.v = 0;
	while (LGetSelect(true, &theCell, PatList2)) LSetSelect(false, theCell, PatList2);
	theCell.h = 0;	theCell.v = (*PatList2)->dataBounds.bottom - 1;
	LSetSelect(true, theCell, PatList2);
	HiliteControl(InfoBut, 0);		HiliteControl(DelBut, 0);
	HiliteControl(SaveBut, 0);		HiliteControl(OpenBut, 0);
	
	/**/
	
	UpdatePatListInfo();
	UpdatePartiInfo();
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
}

void AddAPattern(void)
{
	long					oldSize, newSize, u, v;
	Boolean					MusiqueOn = false;
	Point					theCell = { 0, 0};
	
	SaveUndo(UAllPatterns, 0, "\pUndo 'New Pattern'");
	
	curMusic->hasChanged = true;
	if (MADDriver->Reading == true) MusiqueOn = true;
	MADDriver->Reading = false;
	
	/****** ALLOCATION *********/
	
	curMusic->partition[ curMusic->header->numPat] = (PatData*) NewPtrClear(sizeof(PatHeader) + curMusic->header->numChn * 64L * sizeof(Cmd));
	if (MemError()) MyDebugStr(__LINE__, __FILE__, "Error in AddAPattern...");
	
	curMusic->partition[ curMusic->header->numPat]->header.size = 64L;
	curMusic->partition[ curMusic->header->numPat]->header.compMode = 'NONE';
	strcpy((Ptr) curMusic->partition[ curMusic->header->numPat]->header.name, "New pattern");
	curMusic->partition[ curMusic->header->numPat]->header.patBytes = 0;
	curMusic->partition[ curMusic->header->numPat]->header.unused2 = 0;
	
	for (u = 0; u < curMusic->header->numChn; u++)
	{
		for (v = 0; v < curMusic->partition[ curMusic->header->numPat]->header.size; v++)
		{
			MADKillCmd(GetMADCommand( v,  u, curMusic->partition[ curMusic->header->numPat]));
		}
	}
	
	curMusic->header->numPat++;
	
	if (MusiqueOn) MADDriver->Reading = true;
	
	UpdatePatListInfo();
	UpdatePartiInfo();
	
	while (LGetSelect(true, &theCell, PatList2)) LSetSelect(false, theCell, PatList2);
	
	theCell.v = curMusic->header->numPat - 1;
	LSetSelect(true, theCell, PatList2);
	
	HiliteControl(InfoBut, 0);		HiliteControl(DelBut, 0);
	HiliteControl(SaveBut, 0);		HiliteControl(OpenBut, 0);
	
	if (DialogPatternInfo(theCell.v) == false)
	{
		LSetSelect(true, theCell, PatList2);
		DeleteAPattern(false);
	}
	
	LSetSelect(true, theCell, PatList2);
}

void PurgePattern(void)
{
	Point				theCell = { 0, 0};
	short				thePos, tracks, position;
	Cmd			 		*aCmd;
	
	if (LGetSelect(true, &theCell, PatList2)) thePos = theCell.v;
	else return;
	
	if (InfoL(48) == false) return;

	curMusic->hasChanged = true;

	for (tracks = 0; tracks < curMusic->header->numChn; tracks++)
	{
		for (position = 0; position < curMusic->partition[ thePos]->header.size; position++)
		{
			aCmd = GetMADCommand(position, tracks, curMusic->partition[ thePos]);
			MADKillCmd(aCmd);
		}
	}
	
	UPDATE_Pattern();
}

void UpdatePatListInfo(void)
{
	GrafPtr		SavePort;
	Rect		caRect;
	
	if (PatListDlog == NULL) return;

 	GetPort(&SavePort);
 	SetPortDialogPort(PatListDlog);

	TextFont(4);
	TextSize(9);
	
	LSetDrawingMode(false, PatList2);
	SetPatList2(PatList2);
	LSetDrawingMode(true, PatList2);
	
	PatCopy = -1;
	
	GetPortBounds(GetDialogPort(PatListDlog), &caRect);
	
	InvalWindowRect(GetDialogWindow(PatListDlog), &caRect);
	
	SetPort(SavePort);
}

short GetPatternSelect(void)
{
	Point theCell = { 0, 0};
	
	if (PatListDlog == NULL)
		return 0;
	
	if (LGetSelect(true, &theCell, PatList2))
		return theCell.v;
	else
		return 0;
}

void SetSpeed(PatData* tempMusicPat, short val)
{
	Cmd			*aCmd;
	
	aCmd = GetMADCommand(0, 0, tempMusicPat);
	
	if (val == -1) {
		aCmd->cmd = 0;
		aCmd->arg = 0;
	} else {
		aCmd->cmd = speedE;
		aCmd->arg = val;
	}
}

void SetFineSpeed(PatData* tempMusicPat, short val)
{
	Cmd			*aCmd;
	
	aCmd = GetMADCommand(0, 1, tempMusicPat);
	
	if (val == -1) {
		aCmd->cmd = 0;
		aCmd->arg = 0;
	} else {
		aCmd->cmd = speedE;
		aCmd->arg = val;
	}
}

short GetSpeed(PatData* tempMusicPat)
{
	Cmd *aCmd = GetMADCommand(0, 0, tempMusicPat);
	
	if (aCmd->cmd == speedE) {
		if (aCmd->arg < 32)
			return aCmd->arg;
	}
	
	return -1;
}

short GetFineSpeed(PatData* tempMusicPat)
{
	Cmd *aCmd = GetMADCommand(0, 1, tempMusicPat);
	
	if (aCmd->cmd == speedE) {
		if (aCmd->arg >= 32)
			return aCmd->arg;
		else
			return -1;
	}

	return -1;
}

void ChangeDialogFont(DialogPtr aDia);

Boolean DialogPatternInfo(short thePos)
{
	long					mresult, MusicSize;
	short					whichItem, itemHit, itemType, i, x, curSelecPat;
	DialogPtr				TheDia;
	Str255					theStr, aStr;
	GrafPtr					myPort;
	MenuHandle				thePatternMenuIN;
	Handle					itemHandle;
	Rect					itemRect;
	Point					theCell = { 0, 0}, myPt;
	PatData					*newPattern;
	
	GetPort(&myPort);
	
	SaveUndo(UPattern, thePos, "\pUndo 'Pattern Info Editing'");
	
	TheDia = GetNewDialog(167, NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	ChangeDialogFont(TheDia);
	
	AutoPosition(TheDia);
	
	theStr[ 0] = 20;
	for (x = 0; x < 20; x++) theStr[ x + 1] = curMusic->partition[ thePos]->header.name[ x];
	for (x = 1; x < 20; x++) if (theStr[ x] == 0) { theStr[ 0] = x - 1; break;}
	SetDText(TheDia, 3, theStr);
	SelectDialogItemText(TheDia, 3, 0, 32767);
	
	NumToString(curMusic->partition[ thePos]->header.size, theStr);
	SetDText(TheDia, 4, theStr);
	
	NumToString(thePos, theStr);
	SetDText(TheDia, 13, theStr);
	
	NumToString(curMusic->header->numChn, theStr);
	SetDText(TheDia, 8, theStr);
	
	NumToString(GetPtrSize((Ptr) curMusic->partition[ thePos]), theStr);
	SetDText(TheDia, 10, theStr);
	
	x = GetSpeed(curMusic->partition[ thePos]);
	if (x > 0) {
		TurnRadio(17, TheDia, true);
		
		NumToString(x, theStr);
		SetDText(TheDia, 19, theStr);
	}
	x = GetFineSpeed(curMusic->partition[ thePos]);
	if (x > 0) {
		TurnRadio(18, TheDia, true);
		
		NumToString(x, theStr);
		SetDText(TheDia, 20, theStr);
	}
	
	if (curMusic->partition[ thePos]->header.compMode == 'MAD1') {
		SetDText(TheDia, 16, "\pMAD1");
		curSelecPat = 1;
	} else {
		SetDText(TheDia, 16, "\pNONE");
		curSelecPat = 0;
	}
	thePatternMenuIN = GetMenu(147);
	
	
OnRepart:
	
	do {
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(TheDia, &itemHit);
		
		switch (itemHit)
		{
			case  15:
				InsertMenu(thePatternMenuIN, hierMenu );
				GetDialogItem(TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(thePatternMenuIN, curSelecPat + 1, 0xa5);
				
				mresult = PopUpMenuSelect(	thePatternMenuIN,
										  myPt.v,
										  myPt.h,
										  curSelecPat + 1);
				
				SetItemMark(thePatternMenuIN, curSelecPat + 1, 0);
				
				if (HiWord(mresult) != 0 )
				{
					curSelecPat = (Byte) LoWord(mresult) - 1;
					if (curSelecPat == 1)
					{
						curMusic->partition[ thePos]->header.compMode = 'MAD1';
						SetDText(TheDia, 16, "\pMAD1");
					}
					else
					{
						SetDText(TheDia, 16, "\pNONE");
						curMusic->partition[ thePos]->header.compMode = 'NONE';
					}
				}
				DeleteMenu(GetMenuID(thePatternMenuIN));
				break;
				
			case 17:
			case 18:
				InverseRadio(itemHit, TheDia);
				break;
		}
		
	}while (itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		curMusic->hasChanged = true;
		
		GetDialogItem (TheDia, 17, &itemType, &itemHandle, &itemRect);
		if (GetControlValue((ControlHandle) itemHandle) != 0) {
			GetDText (TheDia, 19, theStr);
			StringToNum(theStr, &mresult);
			if (mresult < 1 || mresult > 31) {
				SelectDialogItemText(TheDia, 19, 0 , 32767);
				MyPPBeep();
				goto OnRepart;
			}
			SetSpeed(curMusic->partition[ thePos], mresult);
		} else {
			x = GetSpeed(curMusic->partition[thePos]);
			if (x != -1)
				SetSpeed(curMusic->partition[thePos], -1);
		}
		
		GetDialogItem (TheDia, 18, &itemType, &itemHandle, &itemRect);
		if (GetControlValue((ControlHandle) itemHandle) != 0) {
			GetDText (TheDia, 20, theStr);
			StringToNum(theStr, &mresult);
			if (mresult < 32 || mresult > 255) {
				SelectDialogItemText(TheDia, 20, 0 , 32767);
				MyPPBeep();
				goto OnRepart;
			}
			
			SetFineSpeed(curMusic->partition[ thePos], mresult);
		} else {
			x = GetFineSpeed(curMusic->partition[ thePos]);
			if (x != -1) SetFineSpeed(curMusic->partition[ thePos], -1);
		}
		
		GetDText(TheDia, 4, theStr);
		StringToNum(theStr, &mresult);
		if (mresult < 1 || mresult > MAXPATTERNSIZE)	//if (mresult != 64)
		{
			Erreur(52, -721);
			goto OnRepart;
		}
		
		if (mresult != curMusic->partition[ thePos]->header.size) {
			short 				u, v, mm;
			Cmd					*inCmd, *ouCmd;
			
			newPattern = (PatData*) NewPtrClear(sizeof(PatHeader) + curMusic->header->numChn * mresult * sizeof(Cmd));
			newPattern->header.size = mresult;
			if (MADDriver->PartitionReader > newPattern->header.size) MADDriver->PartitionReader = 0;
			
			if (mresult > curMusic->partition[ thePos]->header.size) mm = curMusic->partition[ thePos]->header.size;
			else mm = mresult;
			
			/** Copy old notes **/
			
			for (u = 0; u < curMusic->header->numChn; u++) {
				for (v = 0; v < mm; v++) {
					ouCmd = GetMADCommand(v, u, curMusic->partition[thePos]);
					inCmd = GetMADCommand(v, u, newPattern);
					
					*inCmd = *ouCmd;
				}
			}
			
			/** New notes **/
			
			for (u = 0; u < curMusic->header->numChn; u++) {
				for (v = mm; v < newPattern->header.size; v++) {
					MADKillCmd(GetMADCommand(v, u, newPattern));
				}
			}
			
			newPattern->header.compMode = curMusic->partition[ thePos]->header.compMode;
			
			MyDisposePtr((Ptr*) &curMusic->partition[ thePos]);
			curMusic->partition[ thePos] = newPattern;
			
			UPDATE_Pattern();
		}
		
		GetDText(TheDia, 3, theStr);
		if (theStr[ 0] > 20)
			theStr[ 0] = 20;
		for (x = 0; x < theStr[ 0]; x++)
			curMusic->partition[thePos]->header.name[x] = theStr[x + 1];
		for (x = theStr[0]; x < 20; x++)
			curMusic->partition[thePos]->header.name[x] = 0;
		
		UpdateEditorInfo();
		UpdateWaveInfo();
		UpdateStaffInfo();
		UpdatePartiInfo();
		UpdatePatListInfo();
		SetWClassic(thePos);
		
		ScanTime();
	}
	
	DisposeMenu(thePatternMenuIN);
	DisposeDialog(TheDia);
	SetPort(myPort);
	
	if (itemHit == 1)
		return true;
	else
		return false;
}

void UpdatePartitionWindow(DialogPtr);

void DoGrowPatList(DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp, cellRect, tempRect;
	short		cur, tempB, tempA, itemType, avant;
	Handle		itemHandle;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	BitMap		screenBits;
	
	
	GetPort(&SavePort);
 	SetPortDialogPort(theDialog);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp.top = PatRectList2.top + 70;
	temp.left = caRect.right;
	temp.right = caRect.right;
	
	GetQDGlobalsScreenBits(&screenBits);
	
	LRect(&cellRect, theCell, PatList2);
	//temp.bottom = PatRectList2.top + (*PatList2)->dataBounds.bottom*(cellRect.bottom - cellRect.top);
	temp.bottom = screenBits.bounds.bottom;
	
	LocalToGlobal(&aPt);
	
	if (temp.bottom < temp.top)
		temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v)
		temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown)
		lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);
	
	if (lSizeVH != 0) {
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	} else {
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	avant = caRect.bottom - 15;
	
	MySizeWindow(theDialog, tempA, tempB, true);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	LSize(PatRectList2.right - PatRectList2.left - 15, tempB - PatRectList2.top - 15, PatList2);
	PatRectList2.bottom = caRect.bottom - 15;
	
	if (avant > PatRectList2.bottom)
		avant = PatRectList2.bottom;
	tempRect.top = caRect.top;
	tempRect.bottom = caRect.bottom;
	tempRect.left = 0;
	tempRect.right = caRect.right;
	
	EraseRect(&tempRect);
	InvalWindowRect(GetDialogWindow(theDialog), &tempRect);
	
	SetPort(SavePort);
}

void  UpdatePatListWindow(DialogPtr GetSelection)  	/* Pointer to this dialog */
{ 
		Rect   		caRect, tempRect;   			/* Temporary rectangle */
 		GrafPtr		SavePort;
 		Point		theCell;
 		Ptr			theStr;
 		Str255		tempStr, aStr;
 		RgnHandle	visibleRegion;
 		
 		
 		GetPort(&SavePort);
 		SetPortDialogPort(PatListDlog);

 		TextFont(4);
 		TextSize(9);

		BeginUpdate(GetDialogWindow(PatListDlog));
		
		DrawDialog(PatListDlog);

		GetPortBounds(GetDialogPort(GetSelection), &caRect);
		
		MoveTo(0,  25);
		LineTo(caRect.right, 25);

		MoveTo(0, PatRectList2.top - 1);
		LineTo(caRect.right, PatRectList2.top - 1);
	
		MoveTo(0, PatRectList2.bottom);
		LineTo(caRect.right, PatRectList2.bottom);
	
		SetRect(	&tempRect,
					PatRectList2.left,
					PatRectList2.bottom + 1,
					PatRectList2.right - 15,
					caRect.bottom);
		EraseRect(&tempRect);
		
		MoveTo(7, PatRectList2.bottom + 11);
		pStrcpy(tempStr, "\pNumber of patterns:");
		NumToString(curMusic->header->numPat, aStr);
		pStrcat(tempStr, aStr);

		TextFont(kFontIDGeneva);
		DrawString(tempStr);
		TextFont(4);

		DrawGrowIcon(GetDialogWindow(GetSelection));
		
		EndUpdate(GetDialogWindow(PatListDlog));
		
		SetPort(SavePort);
} 

void DoNullPattern(void)
{
	Cell		theCell;
	GrafPtr		SavePort;
	Rect		caRect;

	if (PatListDlog == NULL) return;
	
	/// Select current playing pattern
	if (PatCopy != MADDriver->Pat)
	{
		GetPort(&SavePort);
	 	SetPortDialogPort(PatListDlog);
	 	
	 	TextFont(4);	TextSize(9);
		
		PatCopy = MADDriver->Pat;
		
		BackColor(whiteColor);
		
		theCell.v = theCell.h = 0;
		while (LGetSelect(true, &theCell, PatList2)) LSetSelect(false, theCell, PatList2);
	
		theCell.v = PatCopy;
		LSetSelect(true, theCell, PatList2);
		HiliteControl(InfoBut, 0);
		HiliteControl(DelBut, 0);
		HiliteControl(SaveBut, 0);
		HiliteControl(OpenBut, 0);
		
		LAutoScroll(PatList2);
		
		GetPortBounds(GetDialogPort(PatListDlog), &caRect);
		InvalWindowRect(GetDialogWindow(PatListDlog), &caRect);
		
		UpdatePatListWindow(PatListDlog);
		
		RGBBackColor(&theColor);
		
		SetPort(SavePort);
	}
}

/*
Boolean CheckValidPatList(Byte theValue, short thePos)
{
short		i, numPat, temp;

	for(i=0, numPat = 0; i<128; i++)
	{
		if (curMusic->header->oPointers[i] >= numPat)
		{
			numPat = curMusic->header->oPointers[i];
		}
	}

	for(i=0, temp = 0; i<128; i++)
	{
		if (curMusic->header->oPointers[i] == numPat)
		{
			numPat = curMusic->header->oPointers[i];
			temp++;
		}
	}

	if (theValue < 0 || theValue > numPat)	// pattern qui n'existe pas
	{
		Erreur(26, -76);
		return false;
	}
	else if (temp == 1 && (curMusic->header)->oPointers[ thePos] == numPat) // la plus haute pattern ne peut pas etre detruite
	{
		Erreur(36, -1);
		return false;
	}
	else return true;
}
*/

void DoReplace(void)
{
	long		Result;
	short		PourCent, itemHit, numPat, i, temp;
	DialogPtr	TheDia;
	Str255		theStr;
	GrafPtr		myPort;
	
	GetPort(&myPort);

	TheDia = GetNewDialog(152, NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	
	TurnRadio(4, TheDia, ON);
	
	OnRepart:

	do
	{
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(TheDia, &itemHit);
		
		switch (itemHit)
		{
			case 4: TurnRadio(4, TheDia, ON);	TurnRadio(5, TheDia, OFF);
					TurnRadio(6, TheDia, OFF);	TurnRadio(7, TheDia, OFF);
			break;
			
			case 5: TurnRadio(4, TheDia, OFF);	TurnRadio(5, TheDia, ON);
					TurnRadio(6, TheDia, OFF);	TurnRadio(7, TheDia, OFF);
			break;
			
			case 6: TurnRadio(4, TheDia, OFF);	TurnRadio(5, TheDia, OFF);
					TurnRadio(6, TheDia, ON);	TurnRadio(7, TheDia, OFF);
			break;
			
			case 7: TurnRadio(4, TheDia, OFF);	TurnRadio(5, TheDia, OFF);
					TurnRadio(6, TheDia, OFF);	TurnRadio(7, TheDia, ON);
			break;
			
			case 10:
					InverseRadio(10, TheDia);
			break;
		}
	
	}while (itemHit != 1 && itemHit != 2);

	if (itemHit == 1)
	{
		GetDText (TheDia, 9, (StringPtr) &theStr);
		StringToNum(theStr, &Result);
		if (Result < 0 || Result > 256)
		{
			Erreur(43, -6);	SelectDialogItemText(TheDia, 9, 0, 32767);
			goto OnRepart;
		}
		
		GetDText (TheDia, 12, (StringPtr) &theStr);
		StringToNum(theStr, &Result);
		if (Result < 0 || Result > 256)
		{
			SelectDialogItemText(TheDia, 12, 0, 32767);
			goto OnRepart;
		}
	}

	DisposeDialog(TheDia);
	SetPort(myPort);
}

void DialogTracks(void)
{
	long		mresult;
	short		NewValue, itemHit, i, temp, curSelec, itemType;
	DialogPtr	TheDia;
	Str255		theStr;
	GrafPtr		myPort;
	struct	Command	*oCmd;
	Boolean		IsPlaying;
	Point		myPt;
	Rect		itemRect;
	Handle		itemHandle;
	
	GetPort(&myPort);

	TheDia = GetNewDialog(154, NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	AutoPosition(TheDia);
	ChangeDialogFont(TheDia);
	
	NumToString((long) (curMusic->header)->numChn, theStr);
	SetDText(TheDia, 4, theStr);
	
	NewValue = (curMusic->header)->numChn;
	
	NumToString((long) NewValue, theStr);
	SetDText(TheDia, 6, theStr);
	
	OnRepart:

	do
	{
		//ModalDialog(MyDlgFilterDesc, &itemHit);
		MyModalDialog(TheDia, &itemHit);
		
		switch (itemHit)
		{
			case 8:
				InsertMenu(TracksNumberMenu, hierMenu);
				GetDialogItem(TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				
				curSelec = GetTracksMenuPos(curMusic->header->numChn);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal(&myPt);
				
				SetItemMark(TracksNumberMenu, curSelec, 0xa5);
				
				mresult = PopUpMenuSelect(	TracksNumberMenu,
											myPt.v,
											myPt.h,
											curSelec);
				
				SetItemMark(TracksNumberMenu, curSelec, 0);
				
				if (HiWord(mresult ) != 0 )
				{
					NewValue = GetTracksMenuVal(LoWord(mresult));
				}
				DeleteMenu(GetMenuID(TracksNumberMenu));
				
				NumToString(NewValue, theStr);
				SetDText(TheDia, 6, theStr);
			break;
		}
	
	}while (itemHit != 1 && itemHit != 2);

	if (itemHit == 1)
	{
		IsPlaying = MADDriver->Reading;
		MADDriver->Reading = false;
		
		// Update au niveau du fichier
		SetUpPartition(NewValue);
	
		// Update au niveau du driver
		MADDriver->DriverSettings.numChn = curMusic->header->numChn;
		MADChangeTracks(MADDriver, MADDriver->DriverSettings.numChn);
		SetUpTracksMenu();
		
		UPDATE_Pattern();
		
		SetCurrentMOD(curMusic->musicFileName);
		
		MADDriver->Reading = IsPlaying;
	}

	DisposeDialog(TheDia);
	SetPort(myPort);
}

void OpenSelectedPattern(short no)
{
	MADDriver->Pat = no;
	MADDriver->PartitionReader = 0;
	MADPurgeTrack(MADDriver);
	
	switch (thePrefs.whichEditorPatterns)
	{
		case -1:			break;	// No Editor
		case RefPartition:	HandleOtherChoice(1);	break;
		case RefMozart:		HandleOtherChoice(2);	break;
		case RefStaff:		HandleOtherChoice(3);	break;
		case RefWave:		HandleOtherChoice(4);	break;
		case RefClassic:	HandleViewsChoice(mPatternV);	break;
	}
}

void DoItemPressPatList(short whichItem, DialogPtr whichDialog)    			/* Item hit ID to pass to Dialog function */
{
	Cell			theCell, destCell;
	long			tempLong, lDistVH;
	short			temp, temp2;
	RgnHandle		theRgn;
	Rect			cellRect, itemRect;
	GrafPtr			SavePort;
	Cmd				theCommand;
	Point			myPt;
	Boolean			DoubleClick;
	ControlHandle	control;
	
	GetPort(&SavePort);
	SetPortDialogPort(PatListDlog);
	
	TextFont(4);
	TextSize(9);
	
	if (theEvent.what == mouseDown)
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		DoubleClick = false;
		
		//	FindControl(myPt, GetDialogWindow(whichDialog), &control);
		
		if (PtInRect(myPt, &PatRectList2) == true)
		{
			firstCall = true;
			
			if (TrackControl(myListControl, myPt, (ControlActionUPP)-1L) == 25) DoubleClick = true;
			
			theCell.v = theCell.h = 0;
			if (LGetSelect(true, &theCell, PatList2))
			{
				HiliteControl(InfoBut, 0);
				HiliteControl(DelBut, 0);
				HiliteControl(SaveBut, 0);
				HiliteControl(OpenBut, 0);
				
				if (DoubleClick)
				{
					OpenSelectedPattern(theCell.v);
				}
			}
			else
			{
				HiliteControl(InfoBut, 255);
				HiliteControl(DelBut, 255);
				HiliteControl(SaveBut, 255);
				HiliteControl(OpenBut, 255);
			}
		}
	}
	
	switch (whichItem)
	{
			/*	case 1:
			 itemRect = PatRectList2;
			 itemRect.right -= 16;
			 myPt = theEvent.where;
			 GlobalToLocal(&myPt);
			 
			 if (PtInRect(myPt, &itemRect) == true)
			 {
			 firstCall = true;
			 
			 
			 DoubleClick = false;
			 if (TrackControl(myListControl, myPt, (ControlActionUPP)-1L) == 25) DoubleClick = true;
			 
			 theCell.v = theCell.h = 0;
			 if (LGetSelect(true, &theCell, PatList2))
			 {
			 HiliteControl(InfoBut, 0);
			 HiliteControl(DelBut, 0);
			 HiliteControl(SaveBut, 0);
			 HiliteControl(OpenBut, 0);
			 
			 if (DoubleClick)
			 {
			 OpenSelectedPattern(theCell.v);
			 }
			 }
			 else
			 {
			 HiliteControl(InfoBut, 255);
			 HiliteControl(DelBut, 255);
			 HiliteControl(SaveBut, 255);
			 HiliteControl(OpenBut, 255);
			 }
			 }
			 break;*/
			
		case 3:
			if (MyTrackControl(LoadBut, theEvent.where, NULL))
			{
				HandlePatternChoice(5);
			}
			break;
			
		case 4:
			if (GetControlHilite(SaveBut) == 0 && MyTrackControl(SaveBut, theEvent.where, NULL))
			{
				HandlePatternChoice(6);
			}
			break;
			
		case 5:
			if (GetControlHilite(DelBut) == 0 && MyTrackControl(DelBut, theEvent.where, NULL))
			{
				HandlePatternChoice(8);
			}
			break;
			
		case 6:
			if (GetControlHilite(InfoBut) == 0 && MyTrackControl(InfoBut, theEvent.where, NULL))
			{
				DialogPatternInfo(GetPatternSelect());
			}
			break;
		case 7:
			if (MyTrackControl(NewBut, theEvent.where, NULL))
			{
				HandlePatternChoice(4);
			}
			break;
		case 8:
			if (GetControlHilite(OpenBut) == 0 && MyTrackControl(OpenBut, theEvent.where, NULL))
			{
				theCell.v = theCell.h = 0;
				if (LGetSelect(true, &theCell, PatList2))
				{
					/*	MADDriver->Pat = theCell.v;
					 MADDriver->PartitionReader = 0;
					 MADPurgeTrack(MADDriver);
					 
					 if (ClassicDlog != NULL) SelectWindow2(ClassicDlog);
					 else CreateClassicWindow();*/
					
					OpenSelectedPattern(theCell.v);
				}
			}
			break;
	}
	
	SetPort(SavePort);
}

void SetPatList2(ListHandle	theList)
{
	short   			theRow; 				/* The Row that we are adding */
	Point   			cSize;  				/*  Pointer to a cell in a list  */
	Str255				String;
	short				i, tt, val, temp, dataLen;
	Ptr					stemp, mainStr;
	short				tFont, tSize;		
	GrafPtr				SavePort;

	GetPort(&SavePort);
	SetPortDialogPort(PatListDlog);

	TextFont(4);
	TextSize(9);

	stemp = MyNewPtr(300);
	mainStr = MyNewPtr(300);
	
	if (curMusic->header->numPat - (*theList)->dataBounds.bottom > 0)
	{
		LAddRow(curMusic->header->numPat - (*theList)->dataBounds.bottom, 0, theList);
	}
	else if (curMusic->header->numPat - (*theList)->dataBounds.bottom < 0)
	{
		LDelRow((*theList)->dataBounds.bottom - curMusic->header->numPat, 0, theList);
	}
			
	for(tt = 0; tt < curMusic->header->numPat; tt++)
	{
		cSize.v = tt;
		
		// ID
		
		NumToString((long) tt, (unsigned char*) stemp);
		MyP2CStr((unsigned char*) stemp);
		strcpy(mainStr, "");
		if (tt< 10) strcat(mainStr, " ");
		if (tt< 100) strcat(mainStr, " ");
		strcat(mainStr, stemp);

		// SIZE
		
		strcat(mainStr, " - ");
		NumToString((long) curMusic->partition[ tt]->header.size, (unsigned char*) stemp);
		MyP2CStr((unsigned char*) stemp);
		if (curMusic->partition[ tt]->header.size< 100) strcat(mainStr, " ");
		strcat(mainStr, stemp);
		strcat(mainStr, "x");
		NumToString((long) curMusic->header->numChn, (unsigned char*) stemp);
		MyP2CStr((unsigned char*) stemp);
		if (curMusic->header->numChn< 10) strcat(mainStr, " ");
		strcat(mainStr, stemp);
		
		// NAME
		
		strcat(mainStr, " - ");
		strncpy(stemp, curMusic->partition[ tt]->header.name, 20);
		strcat(mainStr, stemp);

		cSize.h = 0;
		
		dataLen = 200;
		LGetCell(stemp, &dataLen, cSize, PatList2);
		stemp[ dataLen] = '\0';
		
		if (strcmp(stemp, mainStr) != 0)
			LSetCell(mainStr, strlen(mainStr), cSize, theList);
	}
	
	MyDisposePtr(& mainStr);	mainStr = NULL;
	MyDisposePtr(& stemp);		stemp = NULL;

	SetPort(SavePort);
}

void CreatePatListWindow(void)
{
	Rect		caRect, itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize, theCell = { 0, 0};
	FontInfo	ThisFontInfo;
	Str255		String;
	
	if (PatListDlog != NULL)
	{
		SetWindEtat(GetDialogWindow(PatListDlog));
		return;
	}
	
	PatListDlog = GetNewDialog(144, NULL, GetDialogWindow(ToolsDlog));
	
	SetWindEtat(GetDialogWindow(PatListDlog));
	
	SetPortDialogPort(PatListDlog);
	
	TextFont(4);
	TextSize(9);
	
	GetFontInfo(&ThisFontInfo);			/* Get the current font sizes */
	LChar = ThisFontInfo.widMax;
	HCell = cSize.v = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading;/* Height of a cell */
	
	/* Activable pattern list */
	
	GetPortBounds(GetDialogPort(PatListDlog), &caRect);
	
	GetDialogItem(PatListDlog, 1, &itemType, &itemHandle, &PatRectList2);
	
	PatRectList2.right = 200;
	PatRectList2.bottom = 2000;
	
	MySizeWindow(PatListDlog, PatRectList2.right, caRect.bottom, false);
	
	tempRect = PatRectList2;
	SetRect(&dataBounds, 0, 0, 1, curMusic->header->numPointers);
	cSize.h = (tempRect.right - tempRect.left);	/* Width of the list */
	
	myListControl = NewControl(GetDialogWindow(PatListDlog), &tempRect, "\pList", true, 128, 0, 0, kControlListBoxProc, 0);
	if (myListControl)
	{
		Size					actualSize;
		OSErr 					theError;
		ControlFontStyleRec		inStyle;
		
		theError = GetControlData(myListControl, kControlNoPart, kControlListBoxListHandleTag, 4, (Ptr) &PatList2, &actualSize);
		
		inStyle.flags = kControlUseFontMask + kControlUseSizeMask;
		inStyle.font = kFontIDMonaco;
		inStyle.size = 9;
		
		SetControlFontStyle(myListControl, &inStyle);
	}
	
	(*PatList2)->selFlags = lNoExtend;
	(*PatList2)->lClickLoop = NewListClickLoopUPP(myDragClickLoop);
	SetPatList2(PatList2);
	
	LSetSelect(true, theCell, PatList2);
	
	theEvent.what = 0;
	DoGrowPatList(PatListDlog);
	ShowWindow(GetDialogWindow(PatListDlog));
	SelectWindow2(GetDialogWindow(PatListDlog));
	
	ActivePatternMenu(true);
	
	GetPortBounds(GetDialogPort(PatListDlog), &caRect);
	
	SetMaxWindow(caRect.right + 3, 0, PatListDlog);
	
	GetDialogItem(PatListDlog , 6, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	InfoBut = NewControl(	GetDialogWindow(PatListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 149,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(PatListDlog , 3, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	LoadBut = NewControl(	GetDialogWindow(PatListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 152,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem(PatListDlog , 5, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	DelBut = NewControl(	GetDialogWindow(PatListDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						150,
						kControlBevelButtonNormalBevelProc,
						0);
	GetDialogItem(PatListDlog , 4, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	SaveBut = NewControl(	GetDialogWindow(PatListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 151,
						 kControlBevelButtonNormalBevelProc,
						 0);
	GetDialogItem(PatListDlog , 7, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	NewBut = NewControl(	GetDialogWindow(PatListDlog),
						&itemRect,
						"\p",
						true,
						0,
						kControlContentIconSuiteRes,
						154,
						kControlBevelButtonNormalBevelProc,
						0);
	GetDialogItem(PatListDlog , 8, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	OpenBut = NewControl(	GetDialogWindow(PatListDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 159,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	if (DragManagerUse)
	{
		MyTrackingHandlerUPP 		= NewDragTrackingHandlerUPP(MyPATTrackingHandler);
		MyReceiveDropHandlerUPP 	= NewDragReceiveHandlerUPP(MyPATReceiveDropHandler);
		mySendDataUPP 				= NewDragSendDataUPP(MyPATSendDataProc);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingHandlerUPP, GetDialogWindow(PatListDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveDropHandlerUPP, GetDialogWindow(PatListDlog), (void *) NULL);
	}
	
}

void ClosePatList(void)
{
	if (PatListDlog != NULL)
	{
		if (DragManagerUse)
		{
			RemoveTrackingHandler(MyTrackingHandlerUPP, GetDialogWindow(PatListDlog));
			RemoveReceiveHandler(MyReceiveDropHandlerUPP, GetDialogWindow(PatListDlog));
		
			DisposeListClickLoopUPP((*PatList2)->lClickLoop);
			DisposeDragTrackingHandlerUPP(MyTrackingHandlerUPP);
			DisposeDragReceiveHandlerUPP(MyReceiveDropHandlerUPP);
			DisposeDragSendDataUPP(mySendDataUPP);
		}

//		LDispose(PatList2);	PatList2 = NULL;
		DisposeDialog(PatListDlog);
	}
	PatListDlog = NULL;
	ActivePatternMenu(false);
}

void DuplicatePattern(short PatID)
{
	long			oldSize, newSize;
	Boolean			MusiqueOn = false;
	Point			theCell = { 0, 0};

	/****** ALLOCATION *********/
	
	newSize = sizeof(PatHeader) + curMusic->header->numChn * curMusic->partition[ PatID]->header.size * sizeof(Cmd);
	curMusic->partition[ curMusic->header->numPat] = (PatData*) NewPtrClear(newSize);
	if (MemError()) MyDebugStr(__LINE__, __FILE__, "Error in AddAPattern...");
	
	BlockMoveData(curMusic->partition[ PatID], curMusic->partition[ curMusic->header->numPat], newSize);
	
	curMusic->header->numPat++;
}

void DoUpDown(short theChar, ListHandle	theList)
{
	Point	theCell;
	
	TextFont(4);	TextSize(9);
	theCell.v = 0;	theCell.h = 0;
	
	if (LGetSelect(true, &theCell, theList))
	{
		if (theChar == 0x1E)
		{
			if (theCell.v > 0)
			{
				LSetSelect(false, theCell, theList);
				theCell.v--;
				LSetSelect(true, theCell, theList);
				LAutoScroll(theList);
			}
		}
		else if (theChar == 0x1F) 
		{
			if (theCell.v < (*theList)->dataBounds.bottom -1)
			{
				LSetSelect(false, theCell, theList);
				theCell.v++;
				LSetSelect(true, theCell, theList);
				LAutoScroll(theList);
			}
		}
	}
}

void DoKeyPressPatList(short theChar)
{
	GrafPtr		SavePort;
	Point		theCell;
	Rect		cellRect;
	Boolean		MusiqueOn;
	short		maxCell, i;

	if (PatListDlog == NULL) return;
	
	GetPort(&SavePort);
	SetPortDialogPort(PatListDlog);

	BackColor(whiteColor);

	DoUpDown((short) theChar, PatList2);
	
	/********************/
	/***** DUPLICATE ****/
	/********************/
	
	if (theChar == duplicate)
	{
		maxCell = (*PatList2)->dataBounds.bottom;

		SaveUndo(UAllPatterns, 0, "\pUndo 'Duplicate Pattern'");

		curMusic->hasChanged = true;
		if (MADDriver->Reading == true) MusiqueOn = true;
		else MusiqueOn = false;
		MADDriver->Reading = false;

		theCell.v = theCell.h = 0;
		while (LGetSelect(true, &theCell, PatList2))
		{
			DuplicatePattern(theCell.v);
			if (!LNextCell(true, true, &theCell, PatList2)) break;
		}
		if (MusiqueOn) MADDriver->Reading = true;
	
		UpdatePatListInfo();
		UpdatePartiInfo();

		theCell.v = theCell.h = 0;
		while (LGetSelect(true, &theCell, PatList2)) LSetSelect(false, theCell, PatList2);
		for (theCell.v = maxCell; theCell.v < (*PatList2)->dataBounds.bottom; theCell.v++) LSetSelect(true, theCell, PatList2);
	}

	/********************/
	/**** SELECT ALL ****/
	/********************/
	
	if (theChar == selectAll)
 	{
 		theCell.v = 0;	theCell.h = 0;
		do
		{
			LSetSelect(true, theCell, PatList2);
		}while (LNextCell(true, true, &theCell, PatList2));
 	}
	
	theCell.v = theCell.h = 0;
	if (LGetSelect(true, &theCell, PatList2))
	{
	/*	MADDriver->Pat = theCell.v;
		MADDriver->PartitionReader = 0;
		MADPurgeTrack(MADDriver);*/
	
		if (theChar == 0x03 || theChar == 0x0D)
		{
		/*	if (ClassicDlog != NULL)
			{
				LRect(&cellRect, theCell, PatList2);
				SelectWindow2(ClassicDlog);
			}
			else
			{
				LRect(&cellRect, theCell, PatList2);
				CreateClassicWindow();
			}*/
			
			OpenSelectedPattern(theCell.v);
		}
	}
	
	/********************/
	/***** DELETE *******/
	/********************/

	if (theChar == deletekey)
	{
		HandlePatternChoice(deletekey);
	}

	theCell.v = theCell.h = 0;
	if (LGetSelect(true, &theCell, PatList2))
	{
			HiliteControl(InfoBut, 0);		HiliteControl(DelBut, 0);
			HiliteControl(SaveBut, 0);		HiliteControl(OpenBut, 0);
	}
	else
	{
			HiliteControl(InfoBut, 255);	HiliteControl(DelBut, 255);
			HiliteControl(SaveBut, 255);	HiliteControl(OpenBut, 255);
	}

	/********************/
	/***** GET INFO *****/
	/********************/

	if (theChar == getinfo)
	{
		DialogPatternInfo(GetPatternSelect());
	}

	RGBBackColor(&theColor);

	SetPort(SavePort);
}

void COPYPatList(void)
{
	Cell				theCell;
	Cmd					*theCommand, *handleCo;
	short				count;
	OSErr				anErr;
	ScrapRef			scrap;
	
	theCell.h = theCell.v = 0;

	if (LGetSelect(true, &theCell, PatList2))
	{
		SetCursor(&watchCrsr);
				
		anErr = ClearCurrentScrap();
		
		anErr = GetCurrentScrap(&scrap);
		
		anErr = PutScrapFlavor(scrap, 'PATT', 0, GetPtrSize((Ptr) curMusic->partition[ theCell.v]), curMusic->partition[ theCell.v]);
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
}

void PASTEPatList(void)
{
	short				itemType,i,fRefNum, count, temp;
	long				inOutBytes, iL, scrapOffset, lCntOrErr;
	Handle				theHandle;
	Point				theCell;
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;
	OSErr				anErr;

	lCntOrErr = 0;
	anErr = GetCurrentScrap(&scrap);
	anErr = GetScrapFlavorFlags(scrap, 'PATT', &flags);
	if (anErr == noErr) GetScrapFlavorSize(scrap, 'PATT', &lCntOrErr);
	
	if (lCntOrErr > 0)
	{
		SetCursor(&watchCrsr);
	
		SaveUndo(UAllPatterns, 0, "\pUndo 'Paste Pattern'");

		theHandle = MyNewHandle(lCntOrErr);
		
		HLock(theHandle);
		GetScrapFlavorData(scrap, 'PATT', &lCntOrErr, *theHandle);
		HUnlock(theHandle);
		
		HLock(theHandle);
		
		curMusic->partition[ curMusic->header->numPat] = (PatData*) MyNewPtr(lCntOrErr);
		BlockMoveData(*theHandle, curMusic->partition[ curMusic->header->numPat], lCntOrErr);
		curMusic->header->numPat++;
	
	/*	MADDriver->Pat = curMusic->header->numPat -1;
		MADDriver->PartitionReader = 0;*/
		
		HUnlock(theHandle);
		MyDisposHandle(& theHandle);
		
		SetCursor(GetQDGlobalsArrow(&qdarrow));
		
		UpdatePatListInfo();
		UpdatePartiInfo();

		curMusic->hasChanged = true;
	}
}
