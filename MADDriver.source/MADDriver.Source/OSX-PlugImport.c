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

static Handle MADGet1Resource( OSType type, short id, MADLibrary* init)
{
	Handle dH = Get1Resource( type, id);
	if( dH == NULL) return NULL;
	
	DetachResource( dH);
	HNoPurge( dH);
	
	return dH;
}


void NScanResource( MADLibrary *inMADDriver)
{
	short	i;
	
#define BASERES	5000
	
	for( i = 0; i < MAXPLUG; i++)
	{
		Boolean ResourceOK;
		Handle	aRes, bRes;
		
		ResourceOK = true;
		
		aRes = MADGet1Resource( 'CODE', BASERES + i, inMADDriver);
		if( aRes == NULL) ResourceOK = false;
		else
		{
			DisposeHandle( aRes);
			aRes = NULL;
		}
		
		bRes = MADGet1Resource( 'STR#', BASERES + i, inMADDriver);
		if( bRes == NULL) ResourceOK = false;
		else
		{
			DisposeHandle( bRes);
			bRes = NULL;
		}
		
		if( inMADDriver->TotalPlug < MAXPLUG && ResourceOK == true)
		{
			short		No = inMADDriver->TotalPlug;
			Handle		theRes;
			Str255		tStr;
			
			//	theName = LMGetCurApName();
			
			HGetVol( NULL, &inMADDriver->ThePlug[ No].file.vRefNum, &inMADDriver->ThePlug[ No].file.parID);
			pStrcpy( inMADDriver->ThePlug[ No].file.name, RSRCNAME);
			
			/** CODE du Plug-in **/
			
			GetIndString( tStr, BASERES+i, 1);
			BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].type, 4);
			inMADDriver->ThePlug[ No].type[ 4] = 0;
			
			GetIndString( tStr, BASERES+i, 2);
			BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].mode, 4);
			
			GetIndString( inMADDriver->ThePlug[ No].MenuName, BASERES+i, 3);
			GetIndString( inMADDriver->ThePlug[ No].AuthorString, BASERES+i, 4);
			
			inMADDriver->TotalPlug++;
		}
	}
}

void MADInitImportPlug( MADLibrary *inMADDriver, FSRefPtr PluginFolder)
{
	CFMutableArrayRef PlugLocations = NULL;

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
	{
		
		
	}
	CFRelease(PlugLocations);
	CFRelease(PlugLocations);
	PlugLocations = NULL;
	NScanResource(inMADDriver);
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
	
	//  See if this plug-in implements the Test type.
	
	
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
	ULONG RelCount = 0;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		MADFileFormatPlugin	**formatPlugA = inMADDriver->ThePlug[i].IOPlug;
		do {
			RelCount = (*formatPlugA)->Release(formatPlugA);
		} while (RelCount > 0);			
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
