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

extern void NSLog(CFStringRef format, ...);

#define MAXPLUG	40

//#include "PPPlug.h"
#include "PPPrivate.h"
#define CharlMADcheckLength 10

const CFStringRef kMadPlugMenuNameKey =		CFSTR("MADPlugMenuName");
const CFStringRef kMadPlugAuthorNameKey =	CFSTR("MADPlugAuthorName");
const CFStringRef kMadPlugUTITypesKey =		CFSTR("MADPlugUTITypes");
const CFStringRef kMadPlugTypeKey =			CFSTR("MADPlugType");
const CFStringRef kMadPlugModeKey =			CFSTR("MADPlugMode");

static Boolean fillPlugFromBundle(CFBundleRef theBundle, PlugInfo *thePlug)
{
	CFTypeID stringtype =	CFStringGetTypeID();
	CFTypeID numbertype =	CFNumberGetTypeID();
	CFTypeID arraytype =	CFArrayGetTypeID();

	{
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;

		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugMenuNameKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			thePlug->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		}
		else goto badplug;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugAuthorNameKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			thePlug->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		}
		else goto badplug2;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugModeKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			const char * thecOSType = NULL;
			thecOSType = CFStringGetCStringPtr((CFStringRef)OpaqueDictionaryType, kCFStringEncodingMacRoman);
			
			thePlug->mode = Ptr2OSType((char*)thecOSType);
		}
		else if(InfoDictionaryType == numbertype)
		{
			OSType theplugType;
			CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberSInt32Type, &theplugType);
			//PPBE32(&theplugType);
			thePlug->mode = theplugType;
		}
		else goto badplug3;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugUTITypesKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == arraytype) {
			thePlug->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, (CFArrayRef)OpaqueDictionaryType);
		}
		else if(InfoDictionaryType == stringtype)
		{
			CFMutableArrayRef UTIMutableArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
			CFStringRef utiName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
			CFArrayAppendValue(UTIMutableArray, utiName);
			CFRelease(utiName);
			thePlug->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, UTIMutableArray);
			CFRelease(UTIMutableArray);
		}
		else goto badplug3;
	}
	
	thePlug->IOPlug = CFBundleGetFunctionPointerForName(theBundle, CFSTR("PPImpExpMain"));
	if(!thePlug->IOPlug)
		goto badplug3;
	thePlug->file = theBundle;
	CFRetain(thePlug->file);
	
	return true;
	
badplug3:
	CFRelease(thePlug->AuthorString);
badplug2:
	CFRelease(thePlug->MenuName);
badplug:
	return false;
}

static Boolean MakeMADPlug(MADLibrary *inMADDriver, CFBundleRef tempBundle)
{
	if ((inMADDriver->TotalPlug + 1) >= MAXPLUG) {
		MyDebugStr(__LINE__, __FILE__, "More plugs than allocated for!");
		return false;
	}
	CFTypeID stringtype =	CFStringGetTypeID();
	CFTypeID numbertype =	CFNumberGetTypeID();
	
	short PlugNum = inMADDriver->TotalPlug;
	PlugInfo *FillPlug = &(inMADDriver->ThePlug[PlugNum]);
	{
		FillPlug->version = CFBundleGetVersionNumber(tempBundle);
		
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugTypeKey);
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			short i;
			const char * tempstring = CFStringGetCStringPtr((CFStringRef)OpaqueDictionaryType, kCFStringEncodingMacRoman);
			if (tempstring == NULL) goto badplug;
			for (i=0; i < 4; i++) {
				if (tempstring[i] == 0) {
					FillPlug->type[i] = ' ';
				}else FillPlug->type[i] = tempstring[i];
			}
			FillPlug->type[4] = 0;
		}
		else if(InfoDictionaryType == numbertype)
		{
			OSType theplugType;
			CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberSInt32Type, &theplugType);
			OSType2Ptr(theplugType, FillPlug->type);
		}
		else goto badplug;
		
		{
			//Check to see if there's a plug-in that matches the type.
			int i = 0;
			for (i = 0; i < inMADDriver->TotalPlug; i++) {
				if (strcmp(FillPlug->type, inMADDriver->ThePlug[i].type)) {
					NSLog(CFSTR("Plug-ins %@ and %@ are similar"), inMADDriver->ThePlug[i].file, tempBundle);
					if (inMADDriver->ThePlug[i].version < FillPlug->version) {
						PlugInfo newInfo;
						Boolean gotFilled = fillPlugFromBundle(tempBundle, &newInfo);
						if (gotFilled) {
							CFRelease(inMADDriver->ThePlug[i].AuthorString);
							CFRelease(inMADDriver->ThePlug[i].file);
							CFRelease(inMADDriver->ThePlug[i].MenuName);
							CFRelease(inMADDriver->ThePlug[i].UTItypes);
							inMADDriver->ThePlug[i].AuthorString = newInfo.AuthorString;
							inMADDriver->ThePlug[i].file = newInfo.file;
							inMADDriver->ThePlug[i].IOPlug = newInfo.IOPlug;
							inMADDriver->ThePlug[i].MenuName = newInfo.MenuName;
							inMADDriver->ThePlug[i].mode = newInfo.mode;
							inMADDriver->ThePlug[i].UTItypes = newInfo.UTItypes;
							inMADDriver->ThePlug[i].version = FillPlug->version;
							NSLog(CFSTR("Using %@ (Newer than previous)"), tempBundle);
							return true;
						} else {
							NSLog(CFSTR("NOT using %@ (Newer than previous, could not initialize)"), tempBundle);
							goto badplug;
						}
					} else {
						NSLog(CFSTR("NOT using %@ (Not newer than previous)"), tempBundle);
						return false;
					}
				}
			}
		}
		
		Boolean filled = fillPlugFromBundle(tempBundle, FillPlug);
		if (filled) {
			inMADDriver->TotalPlug++;
			return true;
		} else {
			goto badplug;
		}
	}
badplug:
	NSLog(CFSTR("Error with plug-in %@"), tempBundle);
	return false;
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

CFMutableArrayRef CreateDefaultPluginFolderLocations()
{
	CFMutableArrayRef PlugFolds = CFArrayCreateMutable(kCFAllocatorDefault, 6, &kCFTypeArrayCallBacks);
	CFURLRef temp1;
	//Application Main Bundle
	temp1 = CFBundleCopyBuiltInPlugInsURL(CFBundleGetMainBundle());
	CFArrayAppendValue(PlugFolds, temp1);
	CFRelease(temp1);
	temp1 = NULL;
	
	//Local systemwide plugins
	//TODO: better location management
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

static inline CFMutableArrayRef CreatePluginFolderLocationsWithFolderPath(char *UserAddedPlace)
{
	CFMutableArrayRef FoldLocs = CreateDefaultPluginFolderLocations();
	CFURLRef custfolder = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)UserAddedPlace, strlen(UserAddedPlace), true);
	CFURLRef absFolder = CFURLCopyAbsoluteURL(custfolder);
	CFRelease(custfolder);
	CFArrayAppendValue(FoldLocs, absFolder);
	CFRelease(absFolder);
	return FoldLocs;
}

OSErr PPMADInfoFile( char *AlienFile, PPInfoRec	*InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	
	theMAD = (MADSpec*) malloc( sizeof( MADSpec) + 200);
	
	fileID = iFileOpen( AlienFile);
	if( !fileID)
	{
		free( theMAD);
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
	
	free( theMAD);	
	theMAD = NULL;
	
	return noErr;
}

void MADInitImportPlug( MADLibrary *inMADDriver, char *PluginFolder)
{
	CFMutableArrayRef PlugLocations = NULL;
	CFArrayRef		somePlugs = NULL;
	CFIndex			PlugLocNums, PlugNums, i, x;
	
	inMADDriver->ThePlug = (PlugInfo*) malloc( MAXPLUG * sizeof( PlugInfo));
	inMADDriver->TotalPlug = 0;
	if (PluginFolder == NULL) {
		PlugLocations = CreateDefaultPluginFolderLocations();
	} else {
		PlugLocations = CreatePluginFolderLocationsWithFolderPath(PluginFolder);
	}
#ifndef MAINPLAYERPRO
	{
		//TODO: Add Framework plug-in paths. I'm thankful I made it a mutable data type.
	}
#endif
	PlugLocNums	= CFArrayGetCount( PlugLocations );
	
	for (i = 0; i < PlugLocNums; i++) {
		//TODO: version checking
		CFURLRef aPlugLoc = CFArrayGetValueAtIndex(PlugLocations, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, CFSTR("ppimpexp"));
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
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
	
	MADDriverSettings		driverSettings;
	
	iErr = (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
	
	return iErr;
}

void MInitImportPlug( MADLibrary *inMADDriver, char *PlugsFolderName)
{
	MADInitImportPlug(inMADDriver, PlugsFolderName);
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	short	i;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		CFRelease(inMADDriver->ThePlug[i].file);
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		CFRelease(inMADDriver->ThePlug[i].UTItypes);
		CFRelease(inMADDriver->ThePlug[i].MenuName);

	}
	free( inMADDriver->ThePlug);		inMADDriver->ThePlug = NULL;
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
			return CallImportPlug( inMADDriver, i, 'INFO', AlienFile, &aMAD, InfoRec);
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
			*theNewMAD = (MADMusic*) calloc( sizeof( MADMusic), 1);
			if( !theNewMAD) return MADNeedMemory;
			
			return CallImportPlug( inMADDriver, i, 'IMPL', AlienFile, *theNewMAD, &InfoRec);
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
			return CallImportPlug( inMADDriver, i, 'EXPL', AlienFile, theNewMAD, &InfoRec);
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
			return CallImportPlug( inMADDriver, i, 'TEST', AlienFile, &aMAD, &InfoRec);
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
				memcpy( inMADDriver->ThePlug[ i].type, &type, xx);
				PPBE32(&type);
				return type;
			}
			x++;
		}
	}
	
	MyDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}
