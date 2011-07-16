/*
 *  OSX-PlugImport.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RDriver.h"
#include "RDriverInt.h"

#include <CoreFoundation/CoreFoundation.h>
#include "FileUtils.h"
#include "PPPrivate.h"

#define BASERES 1000

extern void NSLog(CFStringRef format, ...);

#define MAXPLUG	40
#define RSRCNAME "\pRsrc Plug Sys##"

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

const CFStringRef kMadPlugMenuNameKey =		CFSTR("MADPlugMenuName");
const CFStringRef kMadPlugAuthorNameKey =	CFSTR("MADPlugAuthorName");
const CFStringRef kMadPlugUTITypesKey =		CFSTR("MADPlugUTITypes");
const CFStringRef kMadPlugTypeKey =			CFSTR("MADPlugType");
const CFStringRef kMadPlugModeKey =			CFSTR("MADPlugMode");
//const CFStringRef kMadPlugModeKey =		CFSTR("");


static void MakeMADPlug(MADLibrary *inMADDriver, CFBundleRef tempBundle)
{
	if ((inMADDriver->TotalPlug + 1) >= MAXPLUG) {
		MyDebugStr(__LINE__, __FILE__, "More plugs than allocated for!");
	}
	CFTypeID stringtype =	CFStringGetTypeID();
	CFTypeID numbertype =	CFNumberGetTypeID();
	CFTypeID arraytype =	CFArrayGetTypeID();
	
	short PlugNum = inMADDriver->TotalPlug;
	PlugInfo *FillPlug = &(inMADDriver->ThePlug[PlugNum]);
	{
		//CFStringRef tStr;
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugMenuNameKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			FillPlug->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		}
		else goto badplug;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugAuthorNameKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			FillPlug->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		}
		else goto badplug2;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugTypeKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			short i;
			char * tempstring = CFStringGetCStringPtr((CFStringRef)OpaqueDictionaryType, kCFStringEncodingMacRoman);
			if (tempstring == NULL) goto badplug;
			for (i=0; i < 4; i++) {
				if (tempstring[i] == 0) {
					FillPlug->type[i] = ' ';
				}else FillPlug->type[i] = tempstring[i];
			}
			FillPlug->type[4] = 0;
			
		}
		else goto badplug3;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugModeKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			char * thecOSType = NULL;
			thecOSType = CFStringGetCStringPtr((CFStringRef)OpaqueDictionaryType, kCFStringEncodingMacRoman);
			
			FillPlug->mode = Ptr2OSType(thecOSType);
		}
		else if(InfoDictionaryType == numbertype)
		{
			OSType theplugType;
			CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberSInt32Type, &theplugType);
			MOT32(&theplugType);
			FillPlug->mode = theplugType;
		}
		else goto badplug3;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugUTITypesKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == arraytype) {
			FillPlug->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, (CFArrayRef)OpaqueDictionaryType);
		}
		else if(InfoDictionaryType == stringtype)
		{
			CFMutableArrayRef UTIMutableArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
			CFArrayAppendValue(UTIMutableArray, CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)InfoDictionaryType));
			FillPlug->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, UTIMutableArray);
			CFRelease(UTIMutableArray);
		}
		else goto badplug3;
	}
	CFURLRef tempURL = CFBundleCopyBundleURL(tempBundle);
	
	CFStringRef URLString = CFURLGetString(tempURL);
	CFRelease(tempURL);
	char URLcString[PATH_MAX];
	CFStringGetFileSystemRepresentation(URLString, URLcString, PATH_MAX);
	FillPlug->hLibrary = dlopen(URLcString , RTLD_LAZY);
	FillPlug->IOPlug = dlsym(FillPlug->hLibrary, "PPImpExpMain");
	if(!FillPlug->IOPlug)
		goto badplug4;
	FillPlug->file = tempBundle;
	CFRetain(FillPlug->file);

	inMADDriver->TotalPlug++;
	return;
	
badplug4:
	dlclose(FillPlug->hLibrary);
badplug3:
	CFRelease(FillPlug->AuthorString);
badplug2:
	CFRelease(FillPlug->MenuName);
badplug:
	NSLog(CFSTR("Error with plug-in %@"), tempBundle);
	return;
}

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

CFMutableArrayRef GetDefaultPluginFolderLocations()
{
	CFMutableArrayRef PlugFolds = CFArrayCreateMutable(kCFAllocatorDefault, 8, &kCFTypeArrayCallBacks);
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

static inline CFMutableArrayRef GetPluginFolderLocationsWithFSRef(FSRefPtr UserAddedPlace)
{
	CFMutableArrayRef FoldLocs = GetDefaultPluginFolderLocations();
	CFURLRef custfolder = CFURLCreateFromFSRef(kCFAllocatorDefault, UserAddedPlace);
	CFArrayAppendValue(FoldLocs, custfolder);
	CFRelease(custfolder); custfolder = NULL;
	return FoldLocs;
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
#ifndef MAINPLAYERPRO
	{
		//TODO: Add Framework plug-in paths. I'm thankful I made it a mutable data type.
	}
#endif
	PlugLocNums	= CFArrayGetCount( PlugLocations );
	
	for (i=0; i < PlugLocNums; i++) {
		CFURLRef aPlugLoc = CFArrayGetValueAtIndex(PlugLocations, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, CFSTR("ppimpexp"));
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFPlugInRef tempPlugRef = NULL;
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				MakeMADPlug(inMADDriver, tempBundleRef);
				
			}
		}
		CFRelease(somePlugs);
	}
	CFRelease(PlugLocations);
	PlugLocations = NULL;
}

OSErr CallImportPlug(MADLibrary				*inMADDriver,
					 short					PlugNo,			// CODE du plug
					 OSType					order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 PPInfoRec				*info)
{
	OSErr					iErr = noErr;
	{
#ifndef __LP64__
		GrafPtr savedPort;
		GetPort(&savedPort);
#endif
		short resFileNum = CFBundleOpenBundleResourceMap(inMADDriver->ThePlug[PlugNo].file);
		MADDriverSettings		driverSettings;
		
		driverSettings.sysMemory = false;
		
		iErr = (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);;
		
		CFBundleCloseBundleResourceMap(inMADDriver->ThePlug[PlugNo].file, resFileNum);
#ifndef __LP64__
		SetPort(savedPort);
#endif
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
		if((iErr =! noErr))
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
		dlclose(inMADDriver->ThePlug[i].hLibrary);

		CFRelease(inMADDriver->ThePlug[i].file);
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		CFRelease(inMADDriver->ThePlug[i].UTItypes);
		CFRelease(inMADDriver->ThePlug[i].MenuName);

	}
	DisposePtr( (Ptr) inMADDriver->ThePlug);		inMADDriver->ThePlug = NULL;
}

void GetPStrFromCFString(const CFStringRef source, Str255 pStrOut)
{
	CFStringGetPascalString(source, pStrOut, 255, CFStringGetSystemEncoding());
}

OSErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short		i;
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
			if( !theNewMAD) return MADNeedMemory;
			
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
				memmove( inMADDriver->ThePlug[ i].type, &type, xx);
				
				return type;
			}
			x++;
		}
	}
	
	MyDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}
