/*
 *  OSX-PlugImport.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//TODO: Use CoreFoundation to link Plugins to the framework
#include "RDriver.h"
#include "RDriverInt.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include "FileUtils.h"

extern void NSLog(CFStringRef format, ...);

#define MAXPLUG	40
#define RSRCNAME "\pRsrc Plug Sys##"

#include "PPPlug.h"
#include "PPPrivate.h"
#define CharlMADcheckLength 10


#pragma mark Plug-in Locations
//There are many places that a plug-in might be kept in OS X
/*
 * Possible plugin places:
 * Local Application Support/PlayerPRO/PlugIns
 * User Application Support/PlayerPRO/PlugIns
 * Application Contents/PlugIns
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
	
	//Local systemwide plugins
	temp1 = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/Library/Application Support/PlayerPRO/Plugins"), kCFURLPOSIXPathStyle, true);
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	
	//User plugins
	temp1 = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("~/Library/Application Support/PlayerPRO/Plugins"), kCFURLPOSIXPathStyle, true);
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	
	
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

void MADInitImportPlug( MADLibrary *inMADDriver, FSRefPtr PluginFolder)
{
	inMADDriver->ThePlug = (PlugInfo*) NewPtr( MAXPLUG * sizeof( PlugInfo));
	inMADDriver->TotalPlug = 0;
	if (PluginFolder != NULL) {
		Boolean UnusedBool1, isFolder;
		FSResolveAliasFile(PluginFolder, TRUE, &isFolder, &UnusedBool1);
	}
	CFMutableArrayRef PlugLocations;
	if (PluginFolder == NULL) {
		PlugLocations = GetDefaultPluginFolderLocations();
	} else {
		PlugLocations = GetPluginFolderLocationsWithFSRef(PluginFolder);
	}
	CFRetain(PlugLocations);
	
	CFRelease(PlugLocations);
}

OSErr CallImportPlug(MADLibrary				*inMADDriver,
					 short					PlugNo,			// CODE du plug
					 OSType					order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 PPInfoRec				*info)
{
	OSErr					iErr = noErr;
	FSRefPtr				tempRef = NULL;
	CFPlugInRef				tempPlugRef /*= inMADDriver->ThePlug[PlugNo].IOPlug*/;
	MADFileFormatPlugin		**formatPlugA = NULL;
	Boolean					foundInterface = false;
	CFArrayRef				factories;
	MADDriverSettings		driverSettings;
	
	
	//  See if this plug-in implements the Test type.
	factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( kPlayerPROModFormatTypeID, tempPlugRef );
	
	
	if ( factories != NULL )
	{
		CFIndex	factoryCount;
		CFIndex	index;
		//TODO: Shrink this so that it doesn't call IUnknownVTbl
		
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
					IUnknownVTbl **iunknown;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( NULL, factoryID, kPlayerPROModFormatTypeID );
					
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
			return MADUnknownErr;
		}
	}
	else {
		return MADUnknownErr;
	}
	
	if (foundInterface == false) {
		return MADCannotFindPlug;
	}
	
	
	CFRelease( factories );
	iErr = (*formatPlugA)->ThePlugMain(order, AlienFile, theNewMAD, info, &driverSettings);
	if (tempRef != NULL) {
		DisposePtr((Ptr)tempRef);
		tempRef = NULL;
	}
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
	
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		{
			Boolean				foundInterface = false;
			CFArrayRef			factories;
			CFPlugInRef			tempPlugRef /*= inMADDriver->ThePlug[PlugNo].IOPlug*/;
			MADFileFormatPlugin	**formatPlugA = NULL;
			//TODO: Shrink this so that it doesn't call IUnknownVTbl
			factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( kPlayerPROModFormatTypeID, tempPlugRef );
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
							IUnknownVTbl **iunknown;
							
							//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
							iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( NULL, factoryID, kPlayerPROModFormatTypeID );
							
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
			}
			
			CFRelease( factories );
			int RelCount;
			if(foundInterface != false)
			{
				do {
					RelCount = (*formatPlugA)->Release(formatPlugA);
				} while (RelCount > 0);
			}
			
			
		}
		//		CFRelease(inMADDriver->ThePlug[i].IOPlug);
		CFRelease(inMADDriver->ThePlug[i].MenuName);
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		//		CFRelease(inMADDriver->ThePlug[i].file);
		CFRelease(inMADDriver->ThePlug[i].filename);
	}
	
	DisposePtr( (Ptr) inMADDriver->ThePlug);		inMADDriver->ThePlug = NULL;
}

OSErr PPInfoFile( MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile, PPInfoRec *InfoRec)
{
	return -8;
}

OSErr PPImportFile( MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	return -8;
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

OSErr PPIdentifyFSRef( MADLibrary *inMADDriver, char *type, FSRefPtr AlienRef)
{
	UInt8 AlienFileName[PATH_MAX];
	Boolean isFolder, UnusedBool1;
	FSResolveAliasFile(AlienRef, TRUE, &isFolder, &UnusedBool1);
	
	FSRefMakePath(AlienRef, AlienFileName, PATH_MAX);
	
	return PPIdentifyFile(inMADDriver, type, (char*)AlienFileName);
}


Boolean	MADPlugAvailable( MADLibrary *inMADDriver, char* kindFile)
{
	short		i;
	
	if( !strcmp( kindFile, "MADK")) return true;
	//	if (kindFile == 'MADK') return TRUE;
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		OSType temp1;
		temp1 = Ptr2OSType(kindFile);
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type)) return true;
		//if( temp1 == inMADDriver->ThePlug[i].type) return TRUE;
	}
	return FALSE;
}

OSErr PPExportFile( MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	return -8;
}

OSErr PPTestFile( MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile)
{
	return -8;
}

OSType GetPPPlugType( MADLibrary *inMADDriver, short ID, OSType mode)
{
	return -8;
}
