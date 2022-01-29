#include "MAD.h"
#include "RDriver.h"
#include "shuddup.h"
#include "mixedmode.h"

#include "PPPlug.h"

typedef struct
{
	Handle		xxxx;
	Str63		MenuName;
	Str63		AuthorString;
	FSSpec		file;
	OSType		type;
	OSType		mode;
	Boolean		hasPPCCode;
	OSType		InsSamp;
}	PPINFilterInfo;

OSErr ExtractFile( NavCBRecPtr callBackParms, FSSpec	*finalSpec);
OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

static	PPInfoPlug	thePPInfoPlug;
static	PPINFilterInfo 	*ThePPINPlug;
static	short		tPlug, sampPlug, instPlug, instMenuID[ 100], sampMenuID[ 100];
static	MenuHandle	PPINMenuSample, PPINMenuInstrument;

OSErr TESTmain(	OSType					order,						// Order to execute
				InstrData					*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFile,					// IN/OUT file
				PPInfoPlug				*thePPInfoPlug);

typedef OSErr (*MyProcPtr) ( OSType,  InstrData*, sData**, short*, FSSpec*, PPInfoPlug*);

OSErr CallPPINPlugIns( 		short					PlugNo,					// CODE du plug
						OSType					order,
						short					ins,
						short					*samp,
						FSSpec					*spec)
{
OSErr			myErr;
short			fileID;
GrafPtr			savedPort;
ConnectionID		connID;
Ptr				mainAddr;
Str255			errName;
static OSErr		(*mainPLUG)( OSType,  InstrData*, sData**, short*, FSSpec*, PPInfoPlug*);




	GetPort( &savedPort);

//	HGetVol( 0L, &vRefNum, &dirIDCopy);
//	HSetVol( 0L, ThePPINPlug[ PlugNo].file.vRefNum, ThePPINPlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePPINPlug[ PlugNo].file, fsCurPerm);
	
	thePPInfoPlug.fileType = ThePPINPlug[ PlugNo].type;
	
	myErr = GetDiskFragment( &ThePPINPlug[ PlugNo].file, 0, kWholeFork, ThePPINPlug[ PlugNo].file.name, kLoadLib, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( order,
						&curMusic->fid[ ins],
						&curMusic->sample[ curMusic->fid[ ins].firstSample],
						samp,
						spec,
						&thePPInfoPlug);
		 
	//	DisposePtr( (Ptr) mainPLUG);
		
		CloseConnection( &connID);
	}
	
	curMusic->fid[ ins].firstSample =  ins * MAXSAMPLE;
	
	CloseResFile( fileID);	
	
	SetPort( savedPort);


	return myErr;
	
/*	if( myErr != noErr)
	{
		//	MADErreur( myErr);
		return( myErr);
	}
	else return noErr;	*/
}

OSErr CallPPINPlugInsOther( 		short					PlugNo,					// CODE du plug
							OSType					order,
							short					*samp,
							FSSpec					*spec,
							InstrData					*insData,
							sData					**sampData)
{
OSErr			myErr;
short			fileID;
GrafPtr			savedPort;
ConnectionID		connID;
Ptr				mainAddr;
Str255			errName;
static OSErr		(*mainPLUG)( OSType,  InstrData*, sData**, short*, FSSpec*, PPInfoPlug*);

	GetPort( &savedPort);

//	HGetVol( 0L, &vRefNum, &dirIDCopy);
//	HSetVol( 0L, ThePPINPlug[ PlugNo].file.vRefNum, ThePPINPlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePPINPlug[ PlugNo].file, fsCurPerm);
	

	myErr = GetDiskFragment( &ThePPINPlug[ PlugNo].file, 0, kWholeFork, ThePPINPlug[ PlugNo].file.name, kLoadLib, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( order,
						insData,
						sampData,
						samp,
						spec,
						&thePPInfoPlug);
		
	//	DisposePtr( (Ptr) mainPLUG);
		
		CloseConnection( &connID);
	}
	
	insData->firstSample =  0;
	
	CloseResFile( fileID);	
	
	SetPort( savedPort);
	
	return myErr;
}

void LoadPPINPLUG(	short	No, StringPtr	theName)
{
Handle		theRes;
short		fileID, i, temp;
Str255		tStr;
char			aStr[ 255];

	/***********************/

	HGetVol( 0L, &ThePPINPlug[ No].file.vRefNum, &ThePPINPlug[ No].file.parID);
	pStrcpy( ThePPINPlug[ No].file.name, theName);
	
	
	{
		Boolean		targetIsFolder, wasAliased;
		
	ResolveAliasFile( &ThePPINPlug[ No].file, true, &targetIsFolder, &wasAliased);
	}

	
	fileID = FSpOpenResFile( &ThePPINPlug[ No].file, fsCurPerm);
	
	GetIndString( tStr, 1000, 1);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].type, 4);
	
	GetIndString( tStr, 1000, 2);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].mode, 4);
	
	GetIndString( ThePPINPlug[ No].MenuName, 1000, 3);
	GetIndString( ThePPINPlug[ No].AuthorString, 1000, 4);
	
	GetIndString( tStr, 1000, 5);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].InsSamp, 4);
	if( ThePPINPlug[ No].InsSamp != 'SAMP' && ThePPINPlug[ No].InsSamp != 'INST') MyDebugStr( __LINE__, __FILE__, "Plug-Ins SAMP/INST Error");
	
	CloseResFile( fileID);
	/*************************/
}

static long PlugsFolderOK;

void ScanDirPPINPlug( long dirID, short VRefNum)
{
		CInfoPBRec		info;
		Str255			tempStr, volName;
		long				dirIDCopy;
		short			i, vRefNum;
		OSErr			iErr;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfo(&info, false) != noErr) break;
		
		#if MACOS9VERSION
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PPIN')
		#else
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'ppin')
		#endif
		{	
			HGetVol( 0L, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( 0L, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > 50) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
			LoadPPINPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
			tPlug++;
			
			iErr = HSetVol( 0L, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirPPINPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
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
			switch( ThePPINPlug[ i].InsSamp)
			{
				case 'SAMP':
					AppendMenu( PPINMenuSample, ThePPINPlug[ i].MenuName);
					sampMenuID[ sampPlug] = i;
					sampPlug++;
				break;
				
				case 'INST':
					AppendMenu( PPINMenuInstrument, ThePPINPlug[ i].MenuName);
					instMenuID[ instPlug] = i;
					instPlug++;
				break;
				
				default:
					MyDebugStr( __LINE__, __FILE__, "Plug-Ins SAMP/INST Error Menu");
				break;
			}
		}
	}
}

void GetSampInstPPIN( short *samp, short *ins)
{
	*samp = sampPlug;
	*ins = instPlug;
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

OSType PressPPINMenu( Rect	*PopUpRect, OSType curType, short samp, Str255 name)
{
	long			mresult;
	Point			Zone;
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
			pStrcpy( name, ThePPINPlug[ instMenuID[ LoWord( mresult)-1]].MenuName);
			return ThePPINPlug[ instMenuID[ LoWord( mresult)-1]].type;
		}
		else
		{
			pStrcpy( name, ThePPINPlug[ sampMenuID[ LoWord( mresult)-1]].MenuName);
			return ThePPINPlug[ sampMenuID[ LoWord( mresult)-1]].type;
		}
	}
	else return '????';
}

void InitPPINPlug(void)
{
	short		vRefNum;
	long		dirID;
	FSSpec		spec;
	
	/*#if MACOS9VERSION
	thePPInfoPlug.RPlaySoundUPP			= NewRoutineDescriptor( (ProcPtr) inMADPlaySoundData, RPlaySoundCallMode, GetCurrentArchitecture());
	thePPInfoPlug.UpdateALLWindowUPP 		= NewRoutineDescriptor( (ProcPtr) UpdateALLWindow, UpdateALLWindowCallMode, GetCurrentArchitecture());
	#endif*/
		thePPInfoPlug.RPlaySoundUPP			= inMADPlaySoundData;
		thePPInfoPlug.UpdateALLWindowUPP 		= UpdateALLWindow;

	HGetVol( 0L, &vRefNum, &dirID);
	
	thePPInfoPlug.MyDlgFilterUPP			= MyDlgFilterDesc;	
	
	ThePPINPlug = (PPINFilterInfo*) MyNewPtr( 100 * sizeof( PPINFilterInfo));
	
	GetApplicationPackageFSSpecFromBundle( &spec);
	
	tPlug		= 0;
	PlugsFolderOK = 0;
	ScanDirPPINPlug( spec.parID, spec.vRefNum);
	
	InitPPINMenu();

	HSetVol( 0L, vRefNum, dirID);
}

short	PPINGetNo()
{
	return tPlug;
}

OSErr	PPINGetPlugName( OSType kindFile, Str255 name)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			pStrcpy( name, ThePPINPlug[ i].MenuName);
			return noErr;
		}
	}
	return MADCannotFindPlug;
}

void DoPlayInstruInt2( sData *curData, InstrData *insData, short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end, long samp);
extern Cursor PlayCrsr;

OSErr	PPINPlayFile( OSType	kindFile, FSSpec *AlienFile)
{
	short		i, temp;
	InstrData		*insData;
	sData		**sampData;
	OSErr		err = -1;
	
	sampData = (sData**) NewPtrClear( sizeof( sData*) * (long) MAXSAMPLE);
	if( sampData == 0L) return -1;
	
	insData = (InstrData*) NewPtrClear( sizeof( sData*) * (long) MAXSAMPLE);
	if( insData == 0L) return -1;
	
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
			
			if( MADDriver->chan[ track].maxPtr <= MADDriver->chan[ track].curPtr || MADDriver->chan[ track].curPtr == 0L) continueLoop = false;
			if( MADIsPressed( (unsigned char*) km, 0x37) && MADIsPressed( (unsigned char*) km, 0x2F)) continueLoop = false;
			if( Button()) continueLoop = false;
			
			DoGlobalNull();
		}
		
		SetCursor( GetQDGlobalsArrow( &qdarrow));
		
		MADPurgeTrack( MADDriver);
		
		for( i = 0; i < insData->numSamples; i++)
		{
			if( sampData[ i] != 0L)
			{
				if( sampData[ i]->data != 0L) DisposePtr( (Ptr) sampData[ i]->data);
				DisposePtr( (Ptr) sampData[ i]);
				sampData[ i] = 0L;
			}
		}
	}
	
	DisposePtr( (Ptr) sampData);
	DisposePtr( (Ptr) insData);
	
	return err;
}

OSErr	PPINTestFile( OSType	kindFile, FSSpec	*AlienFile)
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

OSErr	PPINAvailablePlug( OSType	kindFile, OSType *plugType)
{
	short		i;
	
	for( i = 0; i < tPlug; i++)
	{
		if( kindFile == ThePPINPlug[ i].type)
		{
			if( plugType != 0L) *plugType = ThePPINPlug[ i].InsSamp;
			return noErr;
		}
	}
	
	if( plugType != 0L) *plugType = 'NONE';
	return MADCannotFindPlug;
}

OSErr	PPINIdentifyFile( OSType *type, FSSpec	*AlienFile)
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

OSErr	PPINImportFile( OSType	kindFile, short ins, short *samp, FSSpec	*AlienFile)
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

OSErr	PPINExportFile( OSType	kindFile, short ins, short samp, FSSpec	*AlienFile)
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
////////////////////////////////////////////////////////////////////

static 	Boolean		showAllFiles;
static 	Boolean 		needUp;
static 	Boolean		AddAll;
static	Boolean 		PlayWhenClicked, DoPlayASoundFile;
static	Boolean		ShowAllFiles;
static	FSSpec		lastItem;

extern pascal Boolean MyDlgFilterSF( DialogPtr theDlg, EventRecord *theEvt, short *itemHit, void *data);

void InitPlayWhenClicked(void)
{
	PlayWhenClicked = false;
	ShowAllFiles = false;
}

pascal Boolean PPINCustomFileFilter( CInfoPBRec*	pb, void *myDataPtr)
{
Boolean		ready = false;
short		i;

	if( pb == 0L) return false;
	
	if( pb->hFileInfo.ioFlAttrib & 16) return false;
	
	if( showAllFiles) return false;
	else
	{
		if( pb->hFileInfo.ioFlFndrInfo.fdType == 'INso') return false;
		
		for( i = 0; i < tPlug; i++)
		{
			if( pb->hFileInfo.ioFlFndrInfo.fdType == ThePPINPlug[ i].type)	return false;
		}
	}
	return true;
}

void PPINWriteSupportedFormat( DialogPtr	aDia)
{
	short	i;
	Str255	text;
	
	pStrcpy( text, "\pInstruments List");
	
	for( i = 0; i < tPlug; i++)
	{
		pStrcat( text, "\p - ");
		pStrcat( text, ThePPINPlug[ i].MenuName);
	}
	
	SetDText( aDia, 15, text);
}

pascal short PPINDlgHook( short item, DialogPtr theDialog, void *myDataPtr)
{
	Boolean				MiseAjour = false;
	StandardFileReply	*IntReply;
	GrafPtr				savePort;
	FInfo				fndrInfo;
	
	GetPort( &savePort);
	SetPortDialogPort( theDialog);
	
	if( myDataPtr == 0L) MyDebugStr( __LINE__, __FILE__, "");
	
	IntReply = (StandardFileReply*) myDataPtr;
	
	switch( item)
	{	
		case getOpen:
			{
				OSType type;
			
				if( showAllFiles)
				{
					PPINIdentifyFile( &type, &IntReply->sfFile);
				}
				else type = IntReply->sfType;
				
				if( type == '!!!!' || PPINTestFile( type, &IntReply->sfFile) != noErr)
				{
					if( type != 'INso')
					{
						Erreur( 4, -86);
						item = sfHookNullEvent;
					}
				}
				else HSetVol( 0L, IntReply->sfFile.vRefNum, IntReply->sfFile.parID);
			}
		break;
		
		case sfHookFirstCall:
			PPINWriteSupportedFormat( theDialog);
			
			TurnRadio( 13, theDialog, showAllFiles);
			TurnRadio( 11, theDialog, PlayWhenClicked);
		break;
		
		case 11:
			InverseRadio( 11, theDialog);
			PlayWhenClicked = !PlayWhenClicked;
		break;
		
		case 13:
			InverseRadio( 13, theDialog);
			showAllFiles = !showAllFiles;
			
			item = sfHookRebuildList;
		break;
		
		case -5:
		case sfHookNullEvent:
			if( needUp)
			{
				Str255		a;
				OSType		type;
			
				if( HGetFInfo( IntReply->sfFile.vRefNum, IntReply->sfFile.parID, IntReply->sfFile.name, &fndrInfo) == noErr)
				{
					OSType2Str( fndrInfo.fdType, a);
				}
				else a[ 0] = 0;
				
				SetDText( theDialog, 12, a);
				
				needUp = false;
				
				if( item == sfHookNullEvent)
				{
					if( PlayWhenClicked == true && a[ 0] != 0)
					{
						if( 	lastItem.vRefNum != IntReply->sfFile.vRefNum ||
							lastItem.parID != IntReply->sfFile.parID ||
							RelString( lastItem.name, IntReply->sfFile.name, true, true) != 0)
							{
								lastItem = IntReply->sfFile;
								PPINPlayFile( fndrInfo.fdType, &IntReply->sfFile);
							}
					}
				}
			}
		break;
		
		case sfHookOpenFolder:
		case sfItemFileListUser:
		case sfItemDesktopButton:
		default:
			if( (item & 0xF000) == 0x1000 || item == 103 || item == 7) needUp = true;
		break;
	}
	
	SetPort( savePort);
	
	return(item);
}

// *****************************************************************************
// *
// *	myEventProc()	
// *
// *****************************************************************************
#define kCustomWidth			410
#define kCustomHeight			20
#define kControlListID			164

static	Handle 		gDitlList = 0L;
static	short 		gLastTryWidth;
static	short 		gLastTryHeight;
static	UInt16 		gfirstItem = 0;

// *****************************************************************************
// *
// *	HandleCustomMouseDown()	
// *
// *****************************************************************************
void HandleCustomMouseNAVDown(NavCBRecPtr callBackParms)
{
	OSErr			theErr = noErr;
	ControlHandle		whichControl;				
	Point 			where = callBackParms->eventData.eventDataParms.event->where;	
	short			theItem = 0;	
	UInt16 			firstItem = 0;
	short			realItem = 0;
	short			partCode = 0;
	short			itemType;
	Handle			itemHandle;
	Rect				itemRect;
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

void PPINGetFileName( void)
{
	Point				where = { -1, -1};
	StandardFileReply	reply;
	short			ins, samp, i;
	OSErr			iErr;
	
	if( !GetIns( &ins, &samp))
	{
		Erreur( 13, ins);
		return;
	}
	
	needUp = false;
	AddAll = false;
	DoPlayASoundFile = true;
	
	if( gUseNavigation)
	{
		NavReplyRecord		theReply;
		NavDialogOptions		dialogOptions;
		NavTypeListHandle		openList;
		NavEventUPP			eventUPP = NewNavEventUPP( myCustomEventNAVProc);
		NavObjectFilterUPP 		filterProcUPP = NewNavObjectFilterUPP( MyInsCustomFilter);
		
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
								0L, 		//,
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
				
				if ((iErr = AEGetNthDesc(&(theReply.selection),index,typeFSS,&keyword,&resultDesc)) == noErr)
				{
					if ((iErr = MyAEGetDescData ( &resultDesc, NULL, &reply.sfFile, sizeof ( FSSpec ), NULL )) == noErr)
					
					iErr = AEDisposeDesc(&resultDesc);
				}
			}
			
			iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
		}
		else iErr = -1;
		
		if( iErr) return;
	}
	else
	{
		#if MACOS9VERSION
		
		FileFilterYDUPP		MyCustomFileFilter2Desc = NewFileFilterYDProc( PPINCustomFileFilter);
		DlgHookYDUPP		MyDlgHook2Desc = NewDlgHookYDProc( PPINDlgHook);

		
		lastItem.name[ 0] = 0;
		
		CustomGetFile(		MyCustomFileFilter2Desc,
						-1,
						0,
						&reply,
						179,
						where,
						MyDlgHook2Desc,
						MyDlgFilterSFDesc,
						0L,
						0L,
						&reply);
						
		DisposeRoutineDescriptor( MyDlgHook2Desc);
		DisposeRoutineDescriptor( MyCustomFileFilter2Desc);
		
		UpdateALLWindow();
		
		if( !reply.sfGood) return;
		
		#endif
	}
	
	curMusic->hasChanged = true;
	
	UpdateALLWindow();
	
	SaveUndo( UAllSamples, 0, "\pUndo 'Open instrument'");
	
	iErr = NOpenSampleInt( ins, samp, reply.sfFile);
	
	if( iErr == -10)
	{
		RAWImportFile( &reply.sfFile);
		return;
	}
	else MADErreur( iErr);
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}
