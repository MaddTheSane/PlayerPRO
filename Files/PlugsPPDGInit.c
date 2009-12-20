#include "MAD.h"
#include "RDriver.h"
#include "Shuddup.h"
#include <MixedMode.h>

#include <CFBundle.h>

#include "PPPlug.h"
#include "PPPrivate.h"
#define FilterPlugMax 100

#ifndef MACOS9VERSION
extern void NSLog(CFStringRef format, ...);
#endif

typedef struct FilterInfo
{
	Str63			MenuName;
#if MACOS9VERSION
	Boolean			hasPPCCode;
	FSSpec			file;
#else
	PPDigitalPlugin	**PlugCode;
	CFBundleRef		file;
#endif
	OSType			Type;
} FilterInfo;

static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo	*ThePlug;
static	short		tPlug;
static	MenuHandle	PPDGMenu;

short GetNoPPDG()
{
	return tPlug;
}

OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

#if MACOS9VERSION
typedef OSErr (*MyProcPtr) ( Pcmd *,  PPInfoPlug*);

OSErr CallPPDGPlugIns( 	short					PlugNo,					// CODE du plug
					  Pcmd 					*myPcmd)
{
	OSErr				myErr;
	short				fileID;
	GrafPtr				savedPort;
	CFragConnectionID	connID;
	Ptr					mainAddr;
	Str255				errName;
	static OSErr		(*mainPLUG)( Pcmd*, PPInfoPlug*);
	
	GetPort( &savedPort);
	
	//	HGetVol( NULL, &vRefNum, &dirIDCopy);
	//	HSetVol( NULL, ThePlug[ PlugNo].file.vRefNum, ThePlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePlug[ PlugNo].file, fsCurPerm);
	
	myErr = GetDiskFragment( &ThePlug[ PlugNo].file, 0, kCFragGoesToEOF, ThePlug[ PlugNo].file.name, kLoadCFrag, &connID, (Ptr *) &mainPLUG, errName);
	
	if( myErr == noErr)
	{
		myErr = mainPLUG ( myPcmd, &thePPInfoPlug);
		
		//			DisposePtr( (Ptr) mainPLUG);
		
		CloseConnection( &connID);
	}
	
	CloseResFile( fileID);	
	
	SetPort( savedPort);
	
	if( myErr != noErr)
	{
		Erreur( 40, myErr);
		return( myErr);
	}
	else return noErr;
}

void LoadPPDGPLUG( short No, StringPtr theName)
{
	Handle		theRes;
	short		fileID, i, temp;
	Str255		tStr;
	
	/***********************/
	
	HGetVol( NULL, &ThePlug[ No].file.vRefNum, &ThePlug[ No].file.parID);
	pStrcpy( ThePlug[ No].file.name, theName);
	
	{
		Boolean		targetIsFolder, wasAliased;
		
		ResolveAliasFile( &ThePlug[ No].file, true, &targetIsFolder, &wasAliased);
	}
	
	
	fileID = FSpOpenResFile( &ThePlug[ No].file, fsCurPerm);
	
	/** CODE du Plug-in **/
	
	GetIndString( ThePlug[ No].MenuName, 1000, 1);
	
	ThePlug[ No].Type = 'PPDG';
	
	CloseResFile( fileID);
	/*************************/
}

static long PlugsFolderOK;

void ScanDirPPDGPlug( long dirID, short VRefNum)
{
	CInfoPBRec	info;
	Str255		tempStr, volName;
	long		dirIDCopy;
	short		i, vRefNum;
	OSErr		iErr;
	
	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfoSync(&info) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PPDG')
		{	
			HGetVol( NULL, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( NULL, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > FilterPlugMax) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
			LoadPPDGPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
			tPlug++;
			
			iErr = HSetVol( NULL, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirPPDGPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
}

void InitPPDGPlug(void)
{
	short		vRefNum;
	long		dirID;
	FSSpec		spec;
	
	/*		thePPInfoPlug.RPlaySoundUPP			= NewRoutineDescriptor( (ProcPtr) inMADPlaySoundData, RPlaySoundCallMode, GetCurrentArchitecture());
	 thePPInfoPlug.UpdateALLWindowUPP 		= NewRoutineDescriptor( (ProcPtr) UpdateALLWindow, UpdateALLWindowCallMode, GetCurrentArchitecture());*/
	
	thePPInfoPlug.RPlaySoundUPP			= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP 	= UpdateALLWindow;
	thePPInfoPlug.MyDlgFilterUPP		= MyDlgFilterDesc;	
	
	ThePlug = (FilterInfo*) MyNewPtr( FilterPlugMax * sizeof( FilterInfo));
	
	HGetVol( NULL, &vRefNum, &dirID);
	
	GetApplicationPackageFSSpecFromBundle( &spec);
	
	tPlug			= 0;
	PlugsFolderOK	= 0;
	ScanDirPPDGPlug( spec.parID, spec.vRefNum);
	
	InitPPDGMenu();
	
	HSetVol( NULL, vRefNum, dirID);
}

#else

OSErr CallPPDGPlugIns(short PlugNo, Pcmd *myPcmd)
{
	OSErr			iErr = noErr;
	PPDigitalPlugin	**formatPlugA = ThePlug[PlugNo].PlugCode;
	short			resFileNum;
	GrafPtr			savedPort;
	
	GetPort(&savedPort);
	resFileNum = CFBundleOpenBundleResourceMap(ThePlug[PlugNo].file);
	
	iErr = (*formatPlugA)->MyProcPtr(myPcmd, &thePPInfoPlug);
	
	CFBundleCloseBundleResourceMap(ThePlug[PlugNo].file, resFileNum);
	SetPort(savedPort);
	
	
	if( iErr != noErr)
	{
		Erreur( 40, iErr);
	}	
	
	return iErr;
}

static PPDigitalPlugin** PPDGLoadPlug(CFBundleRef theBundle)
{
	CFPlugInRef				plugToTest = CFBundleGetPlugIn(theBundle);
	CFArrayRef				factories = NULL;
	Boolean					foundInterface = FALSE;
	PPDigitalPlugin			**formatPlugA = NULL;
	
	//  See if this plug-in implements the Test type.
	factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( kPlayerPRODigitalPlugTypeID, plugToTest );
	
	if ( factories != NULL )
	{
		CFIndex	factoryCount, index;
		
		factoryCount	= CFArrayGetCount( factories );
		if ( factoryCount > 0 )
		{
			for ( index = 0 ; (index < factoryCount) && (foundInterface == false) ; index++ )
			{
				CFUUIDRef	factoryID;
				
				//  Get the factory ID for the first location in the array of IDs.
				factoryID = (CFUUIDRef) CFArrayGetValueAtIndex( factories, index );
				if ( factoryID )
				{
					IUnknownVTbl **iunknown = NULL;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( kCFAllocatorDefault, factoryID, kPlayerPRODigitalPlugTypeID );
					
					if ( iunknown )
					{
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface( iunknown, CFUUIDGetUUIDBytes( kPlayerPRODigitalPlugInterfaceID ), (LPVOID *)( &formatPlugA ) );
						
						// Now we are done with IUnknown
						(*iunknown)->Release( iunknown );
						
						if ( formatPlugA )
						{
							//	We found the interface we need
							foundInterface	= true;
						}
					}
				}
			}
		}
		else {
			CFRelease(factories); factories = NULL;
			return NULL;
		}
	}
	else {
		return NULL;
	}
	CFRelease(factories); factories = NULL;
	
	return formatPlugA;
}

void LoadPPDGPLUG( short No, StringPtr theName)
{
#pragma unused(No, theName)
	NSLog(CFSTR("Umm... this isn't supposed to be called..."));
}

static long PlugsFolderOK;

void ScanDirPPDGPlug( long dirID, short VRefNum)
{
#pragma unused(dirID, VRefNum)	
	NSLog(CFSTR("Umm... this isn't supposed to be called..."));	
}

void InitPPDGPlug(void)
{
	thePPInfoPlug.RPlaySoundUPP			= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP 	= UpdateALLWindow;
	thePPInfoPlug.MyDlgFilterUPP		= MyDlgFilterDesc;
	
	
	CFArrayRef  PlugLocsDigital = GetDefaultPluginFolderLocations();
	CFIndex		i, x, PlugLocNums;
	
	ThePlug = (FilterInfo*) MyNewPtr( FilterPlugMax * sizeof( FilterInfo));
	
	PlugsFolderOK	= 0;
	tPlug			= 0;
	
	PPDigitalPlugin** tempMADPlug = NULL;
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
				CFPlugInRef tempPlugRef = NULL;
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				tempMADPlug = PPDGLoadPlug(tempPlugRef);
				if (tempMADPlug) {
					if( tPlug > FilterPlugMax) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
					
#pragma mark This is where we add the plug to the plug library.
					short		resFileNum = CFBundleOpenBundleResourceMap(tempBundleRef);
					CFRetain(tempBundleRef);
					
					ThePlug[tPlug].PlugCode = tempMADPlug;
					ThePlug[tPlug].file = tempBundleRef;
					GetIndString( ThePlug[tPlug].MenuName, 1000, 1);
					ThePlug[tPlug].Type = 'PPDG';
					
					CFBundleCloseBundleResourceMap(tempBundleRef, resFileNum);
					tPlug++;
				}
			}
		}
	}
	
	InitPPDGMenu();
}
//TODO: close PPDG Plugins?
//If we don't, memory leaks! But The OS should take care of it when the program quits.
//I think.

#endif

void InitPPDGMenu(void)
{
	short	i;
	
	PPDGMenu = GetMenu( 159);
	
	for(i=0; i< tPlug; i++)
	{
		AppendMenu( PPDGMenu, ThePlug[ i].MenuName);
	}
}

short PressPPDGMenu( Rect	*PopUpRect)
{
	long	mresult;
	Point	Zone;
	short	i;
	
	InsertMenu( PPDGMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal( &Zone);
	
	mresult = PopUpMenuSelect(	PPDGMenu,
							  Zone.v,
							  Zone.h,
							  0 );
	
	DeleteMenu( GetMenuID( PPDGMenu));
	
	if ( HiWord( mresult ) != 0 )
	{
		return LoWord( mresult );
	}
	else return -1;
}
