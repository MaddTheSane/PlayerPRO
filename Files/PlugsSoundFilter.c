/* Init Plugs-Ins */

#include "MAD.h"
#include "RDriver.h"
#include "Shuddup.h"
#include <MixedMode.h>

#include "PPPlug.h"
#include "PPPrivate.h"

#define MAXFILTERSPLUGS 100

typedef struct FilterInfo
{
	Str63			MenuName;
#ifdef MACOS9VERSION
	FSSpec			file;
#else
	CFBundleRef		file;
#endif
	OSType			Type;
#ifdef MACOS9VERSION
	Boolean			hasPPCCode;
#else
	PPFiltersPlugin	**PlugData;
#endif
} FilterInfo;

		short		ToneGenerator;
static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo 	*ThePlug;
static	short		tPlug;
		MenuHandle	SampleMenu;
extern	WindowPtr	oldWindow;

OSErr TESTmain(	Ptr,
				long ,
				long ,
				long ,
				PPInfoPlug*,
				long);

#ifndef MACOS9VERSION
static PPFiltersPlugin** PPFilterLoadPlug(CFBundleRef theBundle)
{
	CFPlugInRef			plugToTest = CFBundleGetPlugIn(theBundle);
	CFArrayRef			factories = NULL;
	Boolean				foundInterface = FALSE;
	PPFiltersPlugin		**formatPlugA = NULL;
	
	//  See if this plug-in implements the Test type.
	factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( kPlayerPROFiltersPlugTypeID, plugToTest );
	
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
					iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( kCFAllocatorDefault, factoryID, kPlayerPROFiltersPlugTypeID );
					
					if ( iunknown )
					{
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface( iunknown, CFUUIDGetUUIDBytes( kPlayerPROFiltersPlugInterfaceID ), (LPVOID *)( &formatPlugA ) );
						
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
#endif

typedef OSErr (*MyProcPtr) ( sData*, long , long , PPInfoPlug*, long);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
extern void NSLog(CFStringRef format, ...);

OSErr NCallPlugIns( 		short						PlugNo,					// CODE du plug
												sData						*theInsData,
												long 						start,
												long 						end,
												long						stereoMode)
{
	OSErr				myErr;
	short				fileID;
	GrafPtr				savedPort;
#ifdef MACOS9VERSION
	CFragConnectionID	connID;
	Ptr					mainAddr;
	Str255				errName;
	static OSErr		(*mainPLUG)( sData*, long , long , PPInfoPlug*, long);

	if( ThePlug[ PlugNo].Type != 'PLug')
	{
		return noErr;	//return( CallSEPlugIns(	PlugNo, theInsData, start, end));
	}
	
	GetPort( &savedPort);
	
//	HGetVol( NULL, &vRefNum, &dirIDCopy);
//	HSetVol( NULL, ThePlug[ PlugNo].file.vRefNum, ThePlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePlug[ PlugNo].file, fsCurPerm);
	
//	myErr = TEST3main( theInsData, start, end, &thePPInfoPlug, stereoMode);
	
	myErr = GetDiskFragment( &ThePlug[ PlugNo].file, 0, kCFragGoesToEOF, ThePlug[ PlugNo].file.name, kLoadCFrag, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( theInsData, start, end, &thePPInfoPlug, stereoMode);
		
//			DisposePtr( (Ptr) mainPLUG);
		
		myErr = CloseConnection( &connID);
	}
	
	CloseResFile( fileID);	
	
#else
	PPFiltersPlugin	**InstrPlugA = ThePlug[PlugNo].PlugData;
	
	GetPort( &savedPort);
	
	fileID = CFBundleOpenBundleResourceMap(ThePlug[ PlugNo].file);
	
	
	myErr = (*InstrPlugA)->FiltersMain( theInsData, start, end, &thePPInfoPlug, stereoMode);
	
		
	CFBundleCloseBundleResourceMap(ThePlug[ PlugNo].file, fileID);	
	
#endif
	
	SetPort( savedPort);

	if( myErr != noErr)
	{
		Erreur( 40, myErr);
		return( myErr);
	}
	else return noErr;
}

void LoadPLUGSE(	short	No, StringPtr	theName)
{
	NSLog(CFSTR("Umm... what is this?"));
}

void LoadPLUG(	short	No, StringPtr	theName)
{
#if MACOS9VERSION
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

	GetIndString( ThePlug[ No].MenuName, 1000, 1);

	if(EqualString( ThePlug[ No].MenuName, "\pTone Generator...", false, false)) ToneGenerator = No;

	ThePlug[ No].Type = 'PLug';

	CloseResFile( fileID);
	/*************************/
#endif
}

static long PlugsFolderOK;

void ScanDirPlug( long dirID, short VRefNum)
{
#if MACOS9VERSION
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
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PLug')
#else
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'plug')
#endif
		{	
			HGetVol( NULL, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( NULL, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > MAXFILTERSPLUGS) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
				LoadPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
	/*		if( info.hFileInfo.ioFlFndrInfo.fdType == 'ÃEff')
				LoadPLUGSE( tPlug, info.hFileInfo.ioNamePtr);*/
			
			tPlug++;
			
			iErr = HSetVol( NULL, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
#endif
}

void InitSampleMenu(void)
{
	short	i;

	SampleMenu = GetMenu( 136);

	for(i=0; i< tPlug; i++)
	{
		AppendMenu( SampleMenu, ThePlug[ i].MenuName);
	}
}

short PressSampleMenu( Rect	*PopUpRect)
{
	long		mresult;
	Point		Zone;
	short	i;

	InsertMenu( SampleMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal( &Zone);
	
	mresult = PopUpMenuSelect(	SampleMenu,
								Zone.v,
								Zone.h,
								0 );
	
	DeleteMenu( GetMenuID( SampleMenu));
	
	if ( HiWord( mresult ) != 0 )
	{
		return LoWord( mresult );
	}
	else return -1;
}

void InitPlug(void)
{
	thePPInfoPlug.RPlaySoundUPP					= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP			= UpdateALLWindow;

	thePPInfoPlug.MyDlgFilterUPP				= MyDlgFilterDesc;	
	
	ThePlug = (FilterInfo*) MyNewPtr( MAXFILTERSPLUGS * sizeof( FilterInfo));
	
	tPlug			= 0;
	ToneGenerator	= -1;
	PlugsFolderOK	= 0;

#if MACOS9VERSION
	short	vRefNum;
	long	dirID;
	FSSpec	spec;
	
	thePPInfoPlug.RPlaySoundUPP			= NewRoutineDescriptor( (ProcPtr) inMADPlaySoundData, RPlaySoundCallMode, GetCurrentArchitecture());
	thePPInfoPlug.UpdateALLWindowUPP 		= NewRoutineDescriptor( (ProcPtr) UpdateALLWindow, UpdateALLWindowCallMode, GetCurrentArchitecture());
	
	HGetVol( NULL, &vRefNum, &dirID);
		
	GetApplicationPackageFSSpecFromBundle( &spec);
	ScanDirPlug( spec.parID, spec.vRefNum);
	
	HSetVol( NULL, vRefNum, dirID);
#else
	
	CFArrayRef  PlugLocsDigital = GetDefaultPluginFolderLocations();
	CFIndex		i, x, PlugLocNums;

	PPFiltersPlugin** tempMADPlug = NULL;
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
				tempMADPlug = PPFilterLoadPlug(tempPlugRef);
				if (tempMADPlug) {
					if( tPlug > MAXFILTERSPLUGS) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
					
#pragma mark This is where we add the plug to the plug library.
					short		resFileNum = CFBundleOpenBundleResourceMap(tempBundleRef);
					CFRetain(tempBundleRef);
					
					ThePlug[tPlug].PlugData = tempMADPlug;
					ThePlug[tPlug].file = tempBundleRef;
					GetIndString( ThePlug[tPlug].MenuName, 1000, 1);
					ThePlug[tPlug].Type = 'PLug';
					
					CFBundleCloseBundleResourceMap(tempBundleRef, resFileNum);
					tPlug++;
				}
			}
		}
	}	
	
#endif
	
	InitSampleMenu();
}

short GetMaxSoundFilterPlugs( void)
{
	return tPlug;
}
