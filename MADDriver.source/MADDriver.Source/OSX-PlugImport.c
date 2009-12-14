/*
 *  OSX-PlugImport.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//TODO: Use CoreFoundation to link Plugins to the framework and app
#include "RDriver.h"
#include "RDriverInt.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include "FileUtils.h"

extern void NSLog(CFStringRef format, ...);

#define MAXPLUG	40
#define RSRCNAME "\pRsrc Plug Sys##"
#define RSRCSUFFIX ".rsrc"

#include "PPPlug.h"
#include "PPPrivate.h"
#define CharlMADcheckLength 10

static inline OSErr GetFSSpecFromCFBundle(FSSpecPtr out, CFBundleRef in)
{
	OSErr		iErr = noErr;
	FSRef		tempRef;
	CFURLRef	tempURL;
	tempURL = CFBundleCopyBundleURL(in);
	iErr = CFURLGetFSRef(tempURL, &tempRef);
	CFRelease(tempURL);
	if (iErr) {
		iErr = noErr;
	}
	else {
		return fnfErr;
	}
	
	return FSGetCatalogInfo(&tempRef, kFSCatInfoNone, NULL, NULL, out, NULL);
}

#define BASERES 1000
static void MakeMADPlug(MADFileFormatPlugin **tempMADPlug, MADLibrary *inMADDriver, CFBundleRef tempBundle)
{
	OSStatus iErr = noErr;
	short PlugNum = inMADDriver->TotalPlug;
	PlugInfo *FillPlug = &(inMADDriver->ThePlug[PlugNum]);
	{
		CFStringRef PlugName;
		{
			CFDictionaryRef bundleInfoDict;
			
			// Get an instance of the non-localized keys.
			bundleInfoDict = CFBundleGetInfoDictionary( tempBundle );
			
			// If we succeeded, look for our property.
			if ( bundleInfoDict != NULL ) {
				PlugName = CFDictionaryGetValue( bundleInfoDict, kCFBundleExecutableKey );
			}
		}
		CFURLRef rsrcRef = NULL, plugResource;
		plugResource = CFBundleCopyResourcesDirectoryURL(tempBundle);
		CFMutableStringRef plugRsrcName = CFStringCreateMutableCopy(kCFAllocatorDefault, 255, PlugName);
		CFStringAppend(plugRsrcName, CFSTR(RSRCSUFFIX));
		rsrcRef = CFURLCreateWithString(kCFAllocatorDefault, plugRsrcName, plugResource);

		FSRef rsrcRefRef;
		CFURLGetFSRef(rsrcRef, &rsrcRefRef);
		short resFileNum;
		FSOpenResourceFile(&rsrcRefRef, 0, 0, fsCurPerm, &resFileNum);
		if(resFileNum == -1)
		{
			OSStatus whatErr = ResError();
			NSLog(CFSTR("Error of type %i occured"), whatErr);
			CFRelease(rsrcRef);
			CFRelease(plugResource);
			CFRelease(plugRsrcName);
			return;			
		}
		UseResFile(resFileNum);
		
		Str255 tStr;
		GetIndString( tStr, BASERES, 1);
		BlockMoveData( tStr + 1, &FillPlug->type, 4);
		FillPlug->type[ 4] = 0;
			
		GetIndString( tStr, BASERES, 2);
		BlockMoveData( tStr + 1, &FillPlug->mode, 4);
#ifdef __LITTLE_ENDIAN__
		MOT32(&FillPlug->mode);
#endif
			
		GetIndString( FillPlug->MenuName, BASERES, 3);
		GetIndString( FillPlug->AuthorString, BASERES, 4);
		CFRelease(rsrcRef);
		CFRelease(plugResource);
		CFRelease(plugRsrcName);
		CloseResFile(resFileNum);
		FSCloseFork(resFileNum);
	}
	
	FillPlug->IOPlug = tempMADPlug;
	iErr = GetFSSpecFromCFBundle(&(FillPlug->file), tempBundle);
	
	inMADDriver->TotalPlug++;
}
#undef BASERES	

#pragma mark Plug-in Locations
//There are many places that a plug-in might be kept in OS X
/*
 * Possible plugin places:
- * Local Application Support/PlayerPRO/PlugIns
- * User Application Support/PlayerPRO/PlugIns
- * Application Contents/PlugIns
 * Local Framework PlugIns
 * User Framework PlugIns
 * Application Contents/Frameworks/PlugIns
 
 */
//static const CFStringRef PluginFolderLocations[] = {CFSTR("/Library/Application Support/PlayerPRO/Plugins"), 
//CFSTR("~/Library/Application Support/PlayerPRO/Plugins"),CFSTR("")};

static CFMutableArrayRef GetDefaultPluginFolderLocations()
{
	CFMutableArrayRef PlugFolds = CFArrayCreateMutable(kCFAllocatorDefault, 20, &kCFTypeArrayCallBacks);
	CFURLRef temp1;
	//Application Main Bundle
	temp1 = CFBundleCopyBuiltInPlugInsURL(CFBundleGetMainBundle());
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	temp1 = NULL;
	
	//Local systemwide plugins
	temp1 = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/Library/Application Support/PlayerPRO/Plugins"), kCFURLPOSIXPathStyle, true);
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	temp1 = NULL;
	
	//User plugins
	temp1 = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("~/Library/Application Support/PlayerPRO/Plugins"), kCFURLPOSIXPathStyle, true);
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	temp1 = NULL;

	
	return PlugFolds;
}

static CFMutableArrayRef GetPluginFolderLocationsWithFSRef(FSRefPtr UserAddedPlace)
{
	CFMutableArrayRef FoldLocs = GetDefaultPluginFolderLocations();
	CFURLRef custfolder = CFURLCreateFromFSRef(kCFAllocatorDefault, UserAddedPlace);
	CFArrayAppendValue(FoldLocs, custfolder);
	CFRelease(custfolder);
	return FoldLocs;
}

static Handle MADGet1Resource( OSType type, short id, MADLibrary* init)
{
	Handle dH = Get1Resource( type, id);
	if( dH == NULL) return NULL;
	
	DetachResource( dH);
	HNoPurge( dH);
	
	return dH;
}

OSErr PPMADInfoFile( char *AlienFile, PPInfoRec	*InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	short		fileID;
	
	theMAD = (MADSpec*) NewPtr( sizeof( MADSpec) + 200);
	
	fileID = iFileOpen( AlienFile);
	if( !fileID)
	{
		DisposePtr( (Ptr) theMAD);
		return -1;
	}
	fileSize = iGetEOF( fileID);
	
	iRead( sizeof( MADSpec), (Ptr) theMAD, fileID);
	iClose( fileID);
	
	strcpy( InfoRec->internalFileName, theMAD->name);
	
	InfoRec->totalPatterns = theMAD->numPat;
	InfoRec->partitionLength = theMAD->numPointers;
	InfoRec->totalTracks = theMAD->numChn;
	InfoRec->signature = 'MADK';
	strcpy( InfoRec->formatDescription, "MADK");
	InfoRec->totalInstruments = theMAD->numInstru;
	InfoRec->fileSize = fileSize;
	
	DisposePtr( (Ptr) theMAD);	
	theMAD = NULL;
	
	return noErr;
}

static MADFileFormatPlugin **GetMADPlugInterface(CFPlugInRef plugToTest)
{		
	CFArrayRef				factories = NULL;
	Boolean					foundInterface = FALSE;
	MADFileFormatPlugin		**formatPlugA = NULL;

	//  See if this plug-in implements the Test type.
	factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( kPlayerPROModFormatTypeID, plugToTest );
	
	if ( factories != NULL )
	{
		CFIndex	factoryCount;
		CFIndex	index;
		
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
					iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( kCFAllocatorDefault, factoryID, kPlayerPROModFormatTypeID );
					
					if ( iunknown )
					{
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface( iunknown, CFUUIDGetUUIDBytes( kPlayerPROModFormatInterfaceID ), (LPVOID *)( &formatPlugA ) );
						
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
			return NULL;
		}
	}
	else {
		return NULL;
	}
	
	
	return formatPlugA;
}	

void MADInitImportPlug( MADLibrary *inMADDriver, FSRefPtr PluginFolder)
{
	CFMutableArrayRef PlugLocations = NULL;
	CFArrayRef		somePlugs = NULL;
	CFIndex			PlugLocNums, PlugNums, i, x;
	
	inMADDriver->ThePlug = (PlugInfo*) NewPtr( MAXPLUG * sizeof( PlugInfo));
	inMADDriver->TotalPlug = 0;
	if (PluginFolder != NULL) {
		Boolean UnusedBool1, isFolder;
		FSResolveAliasFile(PluginFolder, TRUE, &isFolder, &UnusedBool1);
	}
	if (PluginFolder == NULL) {
		PlugLocations = GetDefaultPluginFolderLocations();
	} else {
		PlugLocations = GetPluginFolderLocationsWithFSRef(PluginFolder);
	}
	CFRetain(PlugLocations);
	PlugLocNums	= CFArrayGetCount( PlugLocations );

	for (i=0; i < PlugLocNums; i++) {
		CFURLRef aPlugLoc;
		aPlugLoc = CFArrayGetValueAtIndex(PlugLocations, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, CFSTR("ppimpexp"));
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFPlugInRef tempPlugRef = NULL;
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				MADFileFormatPlugin** tempMADPlug = NULL;
				tempMADPlug = GetMADPlugInterface(tempPlugRef);
				if (tempMADPlug) {
					MakeMADPlug(tempMADPlug, inMADDriver, tempBundleRef);
				}
			
			}
		}
		CFRelease(somePlugs);
	}
	CFRelease(PlugLocations);
	CFRelease(PlugLocations);
	PlugLocations = NULL;
//	NScanResource(inMADDriver);
}

OSErr CallImportPlug(MADLibrary				*inMADDriver,
					 short					PlugNo,			// CODE du plug
					 OSType					order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 PPInfoRec				*info)
{
	OSErr					iErr = noErr;
	MADFileFormatPlugin		**formatPlugA = inMADDriver->ThePlug[PlugNo].IOPlug;
	MADDriverSettings		driverSettings;
	
	driverSettings.sysMemory = false;
	
	iErr = (*formatPlugA)->ThePlugMain(order, AlienFile, theNewMAD, info, &driverSettings);
	return iErr;
}

void MInitImportPlug( MADLibrary *inMADDriver, FSSpecPtr PlugsFolderName)
{
	FSRefPtr convDir = NULL;
	if(PlugsFolderName != NULL)
	{
		convDir = (FSRefPtr)NewPtr(sizeof(FSRef));
		OSErr iErr;
		iErr = FSpMakeFSRef(PlugsFolderName, convDir);
		if(iErr =! noErr)
		{
			DisposePtr((Ptr)convDir);
			convDir = NULL;
		}
	}
	
	MADInitImportPlug(inMADDriver, convDir);
	if(convDir != NULL) DisposePtr((Ptr)convDir);
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	short	i;
	ULONG	RelCount = 0;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		MADFileFormatPlugin	**formatPlugA = inMADDriver->ThePlug[i].IOPlug;
		do {
			RelCount = (*formatPlugA)->Release(formatPlugA);
		} while (RelCount > 0);			
	}
	DisposePtr( (Ptr) inMADDriver->ThePlug);		inMADDriver->ThePlug = NULL;
}

OSErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short			i;
	MADMusic	aMAD;
	
	if( !strcmp( kindFile, "MADK"))
	{
		PPMADInfoFile( AlienFile, InfoRec);
		
		return noErr;
	}
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'INFO', AlienFile, &aMAD, InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPImportFile( MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			*theNewMAD = (MADMusic*) MADNewPtrClear( sizeof( MADMusic), inMADDriver);
			if( !*theNewMAD) return MADNeedMemory;
			
			return( CallImportPlug( inMADDriver, i, 'IMPL', AlienFile, *theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr CheckMADFile(char* name)
{
	UNFILE				refNum;
	char				charl[CharlMADcheckLength];
	OSErr				err;
	
	refNum = iFileOpen( name);
	if( !refNum) return MADReadingErr;
	else
	{
		iRead(CharlMADcheckLength, charl, refNum);
		
		if( charl[ 0] == 'M' &&							// MADK
		   charl[ 1] == 'A' &&
		   charl[ 2] == 'D' &&
		   charl[ 3] == 'K') err = noErr;
		else err = MADIncompatibleFile;
		
		iClose( refNum);
	}
	return err;
}

OSErr PPIdentifyFile( MADLibrary *inMADDriver, char *type, char *AlienFile)
{
	UNFILE				refNum;
	short				i;
	PPInfoRec			InfoRec;
	OSErr				iErr = noErr;
	
	strcpy( type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpen( AlienFile);
	if( !refNum) return MADReadingErr;
	else
	{
		if( iGetEOF( refNum) < 100) iErr = -36;
		iClose( refNum);
		if( iErr) return iErr;
	}
	
	// Is it a MAD file?
	iErr = CheckMADFile( AlienFile);
	if( iErr == noErr)
	{
		strcpy( type, "MADK");
		return noErr;
	}
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( CallImportPlug( inMADDriver, i, 'TEST', AlienFile, NULL, &InfoRec) == noErr)
		{
			strcpy(type, inMADDriver->ThePlug[i].type);
			return noErr;
		}
	}
	
	strcpy( type, "!!!!");
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable( MADLibrary *inMADDriver, char* kindFile)
{
	short		i;
	
	if( !strcmp( kindFile, "MADK")) return TRUE;
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		OSType temp1;
		temp1 = Ptr2OSType(kindFile);
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type)) return TRUE;
	}
	return FALSE;
}

OSErr PPExportFile( MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'EXPL', AlienFile, theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPTestFile( MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile)
{
	short			i;
	MADMusic		aMAD;
	PPInfoRec		InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'TEST', AlienFile, &aMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSType GetPPPlugType( MADLibrary *inMADDriver, short ID, OSType mode)
{
	short	i, x;
	
	if( ID >= inMADDriver->TotalPlug) MyDebugStr( __LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for( i = 0, x = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( inMADDriver->ThePlug[ i].mode == mode || inMADDriver->ThePlug[ i].mode == 'EXIM')
		{
			if( ID == x)
			{
				short 	xx;
				OSType	type;
				
				xx = strlen( inMADDriver->ThePlug[ i].type);
				if( xx > 4) xx = 4;
				type = '    ';
				BlockMoveData( inMADDriver->ThePlug[ i].type, &type, xx);
				
				return type;
			}
			x++;
		}
	}
	
	MyDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}
