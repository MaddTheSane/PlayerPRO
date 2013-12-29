#include <Carbon/Carbon.h>
#include "GetFileIcon.h"

//TODO: rewrite to handle icns icons?
typedef struct getIconData { 
	OSType		fileCreator;
	OSType		fileType;
	short		DTRefNum;
} GetIconData;

extern Boolean IsVolEjected(short vRefNum);
extern OSErr GetCustomFileIcon(FSSpec *filespec, IconSelectorValue iconSelector, Handle *theSuite);
extern OSErr GetNormalFileIcon(CInfoPBRec *cpb, IconSelectorValue iconSelector, Handle *theSuite);
extern void GetFinderFilename(StringPtr _finderFilename);
extern pascal OSErr GetIconProc(ResType theType, Handle *theIcon, void *yourDataPtr);
extern short FindDesktopDatabase(short firstVRefNum, OSType fileCreator); // returns a DT refnum or 0
extern Boolean InOneDesktop(short vRefNum, OSType fileCreator, short *dtRefNum);
extern pascal OSErr GetResourceIcons(Handle *theSuite, short theID, long theSelector);
extern OSErr CopyEachIcon(Handle theSuite);
extern pascal OSErr CopyOneIcon(ResType theType, Handle *theIcon, void *yourDataPtr);
extern short FindGenericIconID(OSType theType, Boolean *inFinder);
extern pascal OSErr Get1IconSuite(Handle *theSuite, short theID, long theSelector);
extern pascal OSErr Get1Icon(ResType theType, Handle *theIcon, void *resID);
extern pascal OSErr TestHandle(ResType theType, Handle *theIcon, void *yourDataPtr);


/*	Custom icons numbered -16496 appear in aliases to volumes and
	servers.  I don't know where this is documented.	*/
#define	kVolumeAliasIconResource	-16496

Boolean IsVolEjected(short);
OSErr GetCustomFileIcon(FSSpec *, IconSelectorValue, Handle *);
OSErr GetNormalFileIcon(CInfoPBRec *, IconSelectorValue, Handle *);
void GetFinderFilename(StringPtr);
pascal OSErr GetIconProc(ResType, Handle *, void *);
short FindDesktopDatabase(short, OSType);
Boolean InOneDesktop(short, OSType, short *);
pascal OSErr GetResourceIcons(Handle *, short, long);
pascal OSErr CopyOneIcon(ResType, Handle *, void *);
OSErr CopyEachIcon(Handle);
short FindGenericIconID(OSType, Boolean *);
pascal OSErr Get1IconSuite(Handle *, short, long);
pascal OSErr Get1Icon(ResType, Handle *, void *);
pascal OSErr TestHandle(ResType, Handle *, void *);


/*	------------------------------------------------------------------
	GetFileIcon		Given a file specification for a file, folder, or
					volume, create an appropriate icon suite
					and find its label color.
	------------------------------------------------------------------ */
pascal OSErr GetFileIcon(
/* --> */	FSSpec				*thing,
/* --> */	IconSelectorValue	iconSelector,
/* <-- */	Handle				*theSuite)
{
	CInfoPBRec		cpb;
	OSErr			err;


	*theSuite = NULL;

	if( IsVolEjected(thing->vRefNum) )
	{
		err = volOffLinErr;
	}
	else
	{
		cpb.hFileInfo.ioVRefNum		= thing->vRefNum;
		cpb.hFileInfo.ioDirID		= thing->parID;
		cpb.hFileInfo.ioNamePtr		= thing->name;
		cpb.hFileInfo.ioFDirIndex	= 0;
		err = PBGetCatInfoSync( &cpb );

		if( !err )
		{
			if( (cpb.hFileInfo.ioFlAttrib & ioDirMask) == 0)	// file
			{
				if(cpb.hFileInfo.ioFlFndrInfo.fdFlags & kHasCustomIcon)
				{
					err = GetCustomFileIcon(thing, iconSelector, theSuite );
				}
				else	// no custom icon
				{
					err = GetNormalFileIcon(&cpb, iconSelector, theSuite);
				}
			}
			// ----------- end of normal case --------------
		}	
	}

	// ------- error handler ---------
	/*if(thing->parID == fsRtParID)	// a volume
	{
		if(err == volOffLinErr)
		{
			*labelColor = ttOffline;
		}
		err = GetVolumeIcon(thing->vRefNum, iconSelector, theSuite);
	}*/

	return( err );
}


Boolean	IsVolEjected( short vRefNum )
{
	OSErr			err;
	HVolumeParam	vol_pb;
	
	vol_pb.ioNamePtr	= NULL;
	vol_pb.ioVRefNum	= vRefNum;
	vol_pb.ioVolIndex	= 0;
	err = PBHGetVInfoSync( (HParmBlkPtr )&vol_pb );
	
	return( (err == noErr) && (vol_pb.ioVDRefNum > 0) );
}



OSErr	GetCustomFileIcon(
/* --> */	FSSpec				*filespec,
/* --> */	IconSelectorValue	iconSelector,
/* <-- */	Handle				*theSuite)
{
	short	saveResFile, customResFile;
	OSErr	err;
	
	saveResFile = CurResFile();
	SetResLoad( false );
	customResFile = FSpOpenResFile(filespec, fsRdPerm);

	SetResLoad( true );

	if(customResFile == -1)
	{
		err = ResError();
	}
	else
	{
		err = GetResourceIcons(theSuite, kCustomIconResource, iconSelector);
		if( !err )
		{
			if( IsSuiteEmpty( *theSuite ) )
			{
				err = GetResourceIcons(theSuite, kVolumeAliasIconResource, iconSelector);
			}
		}

		CloseResFile( customResFile );
		UseResFile( saveResFile );
	}
	return( err );
}


OSErr	GetNormalFileIcon(
/* --> */	CInfoPBRec			*cpb,
/* --> */	IconSelectorValue	iconSelector,
/* <-- */	Handle				*theSuite)
{
	OSErr			err = noErr;
	long			dataSize;
	Handle			iconData;
	Byte			iconType;
	GetIconData		getData;
	short			iconID;
	Boolean			inFinder;
	short			saveResFile, FinderResFile, sysVRefNum;
	long			sysDirID;
	Str255			finderName;
	IconActionUPP	getIconProcPtr;


	iconID = FindGenericIconID(cpb->hFileInfo.ioFlFndrInfo.fdType, &inFinder );
	saveResFile = CurResFile();

	if( inFinder )
	{
		FindFolder(kOnSystemDisk, kSystemFolderType, kDontCreateFolder, &sysVRefNum, &sysDirID);

		SetResLoad( false );
		GetFinderFilename( finderName );
		FinderResFile = HOpenResFile(sysVRefNum, sysDirID, finderName, fsRdPerm);
		SetResLoad( true );

		if(FinderResFile == -1)
		{
			err = ResError();
		}
		else
		{
			err = GetResourceIcons(theSuite, iconID, iconSelector);
			CloseResFile( FinderResFile );
		}
	}
	else	// icons in desktop DB or in System
	{
		getData.DTRefNum = FindDesktopDatabase(cpb->dirInfo.ioVRefNum,
			cpb->hFileInfo.ioFlFndrInfo.fdCreator );

		if(getData.DTRefNum != 0)	// the right icons are in some desktop
		{
			err = NewIconSuite( theSuite );
			if( !err )
			{
				getData.fileCreator	= cpb->hFileInfo.ioFlFndrInfo.fdCreator;
				getData.fileType	= cpb->hFileInfo.ioFlFndrInfo.fdType;
				if(getData.fileType == kApplicationAliasType)
				{
					getData.fileType = 'APPL';
				}
				getIconProcPtr = NewIconActionUPP( GetIconProc );
				err = ForEachIconDo(*theSuite, iconSelector, getIconProcPtr, &getData);
				DisposeIconActionUPP( getIconProcPtr );
			}
		}
		if( (getData.DTRefNum == 0) || IsSuiteEmpty( *theSuite ) )
		{
			UseResFile( 0 );
			err = GetResourceIcons(theSuite, iconID, iconSelector);
		}
	}

	UseResFile( saveResFile );

	return( err );
}


void GetFinderFilename(
/* <-- */	StringPtr       _finderFilename)
{
	Str255          _defaultFinderFilename="\pFinder";
	StringPtr       _lowMemFinderName;

	_lowMemFinderName = LMGetFinderName();
	if( (_lowMemFinderName != (StringPtr )nil) && (_lowMemFinderName[0] > 0))
		BlockMoveData(_lowMemFinderName, _finderFilename, _lowMemFinderName[0]+1);
	else
		BlockMoveData(_defaultFinderFilename, _finderFilename, _defaultFinderFilename[0]+1);
}


/*	------------------------------------------------------------------
	GetIcon		This is an IconAction procedure to fill in one
					slot of an icon suite, given a file type, creator,
					and desktop database.
	------------------------------------------------------------------ */
	pascal OSErr GetIconProc(ResType theType, Handle *theIcon, void *yourDataPtr)
{
	OSErr			err;
	GetIconData		*data;
	DTPBRec			deskRec;

	err = noErr;
	data = (GetIconData *)yourDataPtr;
	*theIcon = NewHandle( kLarge8BitIconSize );

	if( !(*theIcon) )
	{
		err = memFullErr;
	}
	else
	{
		HLock( *theIcon );
	
		deskRec.ioDTRefNum		= data->DTRefNum;
		deskRec.ioDTBuffer		= **theIcon;
		deskRec.ioDTReqCount	= kLarge8BitIconSize;
		deskRec.ioFileCreator	= data->fileCreator;
		deskRec.ioFileType		= data->fileType;
	
		switch( theType )
		{
			case kLarge1BitMask:
				deskRec.ioIconType = kLargeIcon;
				break;
	
			case kLarge4BitData:
				deskRec.ioIconType = kLarge4BitIcon;
				break;
	
			case kLarge8BitData:
				deskRec.ioIconType = kLarge8BitIcon;
				break;
	
			case kSmall1BitMask:
				deskRec.ioIconType = kSmallIcon;
				break;
	
			case kSmall4BitData:
				deskRec.ioIconType = kSmall4BitIcon;
				break;
	
			case kSmall8BitData:
				deskRec.ioIconType = kSmall8BitIcon;
				break;
			
			default:
				// The desktop database does not have "mini" icons
				deskRec.ioIconType = 1000;
				break;
		}

		err = PBDTGetIconSync( &deskRec );
		if(err == noErr)
		{
			HUnlock( *theIcon );
			SetHandleSize(*theIcon, deskRec.ioDTActCount);
		}
		else
		{
			DisposeHandle( *theIcon );
			*theIcon = NULL;
			err = noErr;
		}
	}
	return( err );
}


/*	------------------------------------------------------------------
	Find_desktop_database			Find the reference number of a
									desktop database containing icons
									for a specified creator code.
	The search begins on a specified volume, but covers all volumes.
	------------------------------------------------------------------ */
	short	FindDesktopDatabase(
/* --> */	short	firstVRefNum,
/* --> */	OSType	fileCreator)	// returns a DT refnum or 0
{
	OSErr			err;
	XVolumeParam	vpb;
	short			DTRefNum = 0;
	
	if(!InOneDesktop(firstVRefNum, fileCreator, &DTRefNum)) {
		vpb.ioNamePtr = NULL;
		for(vpb.ioVolIndex = 1; PBXGetVolInfoSync( &vpb) == noErr; ++vpb.ioVolIndex) {
			if(vpb.ioVRefNum == firstVRefNum)
				continue;
			if( InOneDesktop(vpb.ioVRefNum, fileCreator, &DTRefNum) )
				break;
		}
	}
	return DTRefNum;
}



/*	------------------------------------------------------------------
	InOneDesktop			Determine whether the desktop database for
							one particular volume contains icons for
							a given creator code, and if so, return its
							reference number.
	------------------------------------------------------------------
*/
	Boolean	InOneDesktop(
/* --> */	short	vRefNum,
/* --> */	OSType	fileCreator,
/* <-- */	short	*dtRefNum)
{
	OSErr		err;
	DTPBRec		deskRec;
	Boolean		retVal;
	
	HParamBlockRec         _myHPB;
	GetVolParmsInfoBuffer  _infoBuffer;
	
	retVal = false;	// default to failure
	deskRec.ioNamePtr = NULL;
	deskRec.ioVRefNum = vRefNum;
	
	// check to make sure we've got a database first:
	_myHPB.ioParam.ioNamePtr  = (StringPtr)nil;
	_myHPB.ioParam.ioVRefNum  = vRefNum;
	_myHPB.ioParam.ioBuffer   = (Ptr)&_infoBuffer;
	_myHPB.ioParam.ioReqCount = sizeof(_infoBuffer);
	if ( ((err=PBHGetVolParms(&_myHPB,false/*async*/))!=noErr) ||
	((_infoBuffer.vMAttrib&(1L<<bHasDesktopMgr))==0) )
		return( retVal );

	err = PBDTGetPath( &deskRec );
	if( !err )
	{
		/*	We want to ignore any non-icon data, such as the 'paul'
			item that is used for drag-and-drop. */
		deskRec.ioFileCreator = fileCreator;
		deskRec.ioIndex = 1;
		do
		{
			deskRec.ioTagInfo = 0;
			err = PBDTGetIconInfoSync( &deskRec );
			deskRec.ioIndex += 1;
		}while( (err == noErr) && (deskRec.ioIconType <= 0) );
	
		if(err == noErr)
		{
			retVal = true;
			*dtRefNum = deskRec.ioDTRefNum;
		}
	}
	return( retVal );
}


pascal OSErr GetResourceIcons(
/* <-- */	Handle	*theSuite,
/* --> */	short	theID,
/* --> */	long	theSelector)
{
	OSErr	err;
	
	err = Get1IconSuite(theSuite, theID, theSelector);
	if(err == noErr)
	{
		err = CopyEachIcon( *theSuite );
	}
	return( err );
}


pascal OSErr CopyOneIcon(
/* --> */	ResType		theType,
/* <-> */	Handle		*theIcon,
/* --- */	void		*yourDataPtr)
{
	OSErr	err;
	
	if(*theIcon != NULL)
	{
		LoadResource( *theIcon );
		err = HandToHand( theIcon );
		if(err != noErr)
			*theIcon = NULL;
	}
	return( noErr );
}


OSErr CopyEachIcon(
/* <-> */	Handle theSuite)
{
	IconActionUPP	copyOneIconProc;
	OSErr			err;
	copyOneIconProc = NewIconActionUPP( CopyOneIcon );
	err = ForEachIconDo(theSuite, svAllAvailableData, copyOneIconProc, NULL);
	DisposeIconActionUPP( copyOneIconProc );
	return( err );
}


typedef struct genericIconInfo { OSType type; short id; } GenericIconInfo;
GenericIconInfo gGenericFinderIcons[]={ {'ifil',12500},
                                               {'ifil',12500},
                                               {'sfil',14000},
                                               {'ffil',14500},
                                               {'tfil',14501},
                                               {'kfil',14750},
                                               {'FFIL',15500},
                                               {'DFIL',15750}
                                              };
GenericIconInfo gGenericSysIcons[]={ {kContainerFolderAliasType,genericFolderIconResource},
                                            {kContainerTrashAliasType,trashIconResource},
                                            {kSystemFolderAliasType,systemFolderIconResource},
                                            {'INIT',genericExtensionIconResource},
                                            {'APPL',genericApplicationIconResource},
                                            {'dfil',genericDeskAccessoryIconResource},
                                            {'pref',genericPreferencesIconResource},
                                            {kAppleMenuFolderAliasType,appleMenuFolderIconResource},
                                            {kControlPanelFolderAliasType,controlPanelFolderIconResource},
                                            {kExtensionFolderAliasType,extensionsFolderIconResource},
                                            {kPreferencesFolderAliasType,preferencesFolderIconResource},
                                            {kStartupFolderAliasType,startupFolderIconResource},
                                            {kApplicationAliasType,genericApplicationIconResource},
                                            {kExportedFolderAliasType,ownedFolderIconResource},
                                            {kDropFolderAliasType,dropFolderIconResource},
                                            {kSharedFolderAliasType,sharedFolderIconResource},
                                            {kMountedFolderAliasType,mountedFolderIconResource}
                                           };


short	FindGenericIconID(
/* --> */	OSType theType,
/* <-- */	Boolean	*inFinder)
{
	
   short id=genericDocumentIconResource; // default
   GenericIconInfo *_icon, *_endIcon;
   
    for (_icon=gGenericFinderIcons,_endIcon=_icon+sizeof(gGenericFinderIcons)/sizeof(GenericIconInfo);
         (_icon<_endIcon)&&(_icon->type!=theType); _icon++) ;
    if (!(*inFinder=(_icon<_endIcon)))
        for (_icon=gGenericSysIcons,_endIcon=_icon+sizeof(gGenericSysIcons)/sizeof(GenericIconInfo);
             (_icon<_endIcon)&&(_icon->type!=theType); _icon++) ;
    if (_icon<_endIcon)
        id = _icon->id;

	return( id );
}


/*	--------------------------------------------------------------------
	Get1IconSuite			Like GetIconSuite, but only looks in
							the current resource file.
	
	In case you're wondering why it would be necessary to ensure that
	icons come from only one file, suppose you're looking at a
	file that has its custom icon bit set, but for some reason does
	not contain a custom icon, or at least not a full family.
	Way down the resource chain, there may be another file, say a
	font file, that does have a full family of custom icons. 
	So you get an unexpected icon.
	-------------------------------------------------------------------- */

pascal OSErr Get1IconSuite(
/* <-- */	Handle	*theSuite,
/* --> */	short	theID,
/* --> */	long	theSelector)
{
	OSErr		err;
	IconActionUPP	get1IconProc;

	err = NewIconSuite( theSuite );
	if( !err )
	{
		get1IconProc = NewIconActionUPP( Get1Icon );
		err = ForEachIconDo(*theSuite, theSelector, get1IconProc, &theID);
		DisposeIconActionUPP( get1IconProc );
	}
	return( err );
}


pascal OSErr Get1Icon(
/* --> */	ResType	theType,
/* <-> */	Handle	*theIcon,
/* --> */	void	*resID)
{
	*theIcon = Get1Resource(theType, *((short*)resID));

	return( noErr );
}


pascal OSErr TestHandle(ResType theType, Handle *theIcon, void *yourDataPtr)
{
	if(*theIcon != NULL)
		*(Boolean *)yourDataPtr = false;	// not empty!
	
	return noErr;
}


Boolean IsSuiteEmpty( Handle theSuite )
{
	Boolean			retVal;
	IconActionUPP	testHandleProc;
	
	testHandleProc = NewIconActionUPP( TestHandle );
	
	retVal = true;
	ForEachIconDo(theSuite, svAllAvailableData, testHandleProc, &retVal);
	DisposeIconActionUPP( testHandleProc );
	
	return retVal ;
}
