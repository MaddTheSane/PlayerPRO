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
#import <Foundation/Foundation.h>
#include "FileUtils.h"
#include "PPPrivate.h"

#define MAXPLUG	40

#include "PPPlug.h"
#include "PPPrivate.h"
#define CharlMADcheckLength 10

const CFStringRef kMadPlugMenuNameKey =		CFSTR("MADPlugMenuName");
const CFStringRef kMadPlugAuthorNameKey =	CFSTR("MADPlugAuthorName");
const CFStringRef kMadPlugUTITypesKey =		CFSTR("MADPlugUTITypes");
const CFStringRef kMadPlugTypeKey =			CFSTR("MADPlugType");
const CFStringRef kMadPlugDoesImport =		CFSTR("MADCanImport");
const CFStringRef kMadPlugDoesExport =		CFSTR("MADCanExport");
const CFStringRef kMadPlugModeKey =			CFSTR("MADPlugMode");

static CFTypeID stringtype = 0;
static CFTypeID numbertype = 0;
static CFTypeID arraytype = 0;
static CFTypeID booleantype = 0;

typedef enum _MADPlugCapabilities {
	PPMADCanDoNothing = 0,
	PPMADCanImport = 1,
	PPMADCanExport = 2,
	PPMADCanDoBoth = PPMADCanImport | PPMADCanExport
} MADPlugCapabilities;

static Boolean GetBoolFromType(CFTypeRef theType)
{
	//We don't need to test for the other CFTypeIDs because they should already be set up
	if (booleantype == 0) {
		booleantype = CFBooleanGetTypeID();
	}
	CFTypeID theID = CFGetTypeID(theType);
	if (theID == booleantype) {
		return CFBooleanGetValue(theType);
	} else if (theID == numbertype) {
		int theVal = 0;
		CFNumberGetValue(theType, kCFNumberIntType, &theVal);
		return theVal != 0;
	} else if (theID == stringtype) {
		//FIXME: get a CoreFoundation function here to ease the transition to ARC when that happens.
		return [(NSString*)theType boolValue];
	} else return false;
}

static Boolean fillPlugFromBundle(CFBundleRef theBundle, PlugInfo *thePlug)
{
	//We don't need to test for the other CFTypeIDs because they should already be set up
	if (arraytype == 0) {
		arraytype = CFArrayGetTypeID();
	}
	
	{
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;

		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugMenuNameKey);
		if (OpaqueDictionaryType == NULL) {
			goto badplug;
		}
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			thePlug->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		}
		else goto badplug;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugAuthorNameKey);
		if (OpaqueDictionaryType == NULL) {
			thePlug->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, CFSTR("No Author"));
		} else {
			InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
			if (InfoDictionaryType == stringtype) {
				thePlug->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
			} else {
				thePlug->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, CFSTR("No Author"));
			}
		}
		
		//See if the plug-in uses MADCanExport and MADCanImport. 
		{
			CFTypeRef importValue = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugDoesImport);
			CFTypeRef exportValue = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugDoesExport);
			if (importValue != NULL || exportValue != NULL) {
				MADPlugCapabilities possibilities = PPMADCanDoNothing;
				if (importValue != NULL) {
					if(GetBoolFromType(importValue))
						possibilities = PPMADCanImport;
				}
				if (exportValue != NULL) {
					if(GetBoolFromType(exportValue))
						possibilities |= PPMADCanExport;
				}
				switch (possibilities) {
					case PPMADCanImport:
						thePlug->mode = MADPlugImport;
						break;
						
					case PPMADCanExport:
						thePlug->mode = MADPlugExport;
						break;
						
					case PPMADCanDoBoth:
						thePlug->mode = MADPlugImportExport;
						break;
						
					default:
						goto badplug3;
						break;
				}
			} else { //If not, use the likely-to-be broken method.
				OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugModeKey);
				if (OpaqueDictionaryType == NULL) {
					goto badplug3;
				}
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
			}
		}
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugUTITypesKey);
		if (OpaqueDictionaryType == NULL) {
			goto badplug3;
		}
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
		goto badplug4;
	thePlug->file = theBundle;
	CFRetain(thePlug->file);
	
	return true;
	
badplug4:
	CFRelease(thePlug->UTItypes);
badplug3:
	CFRelease(thePlug->AuthorString);
	CFRelease(thePlug->MenuName);
badplug:
	return false;
}

static Boolean MakeMADPlug(MADLibrary *inMADDriver, CFBundleRef tempBundle)
{
	if ((inMADDriver->TotalPlug + 1) >= MAXPLUG) {
		PPDebugStr(__LINE__, __FILE__, "More plugs than allocated for!");
		return false;
	}
	if (stringtype == 0) {
		stringtype = CFStringGetTypeID();
	}
	if (numbertype == 0) {
		numbertype = CFNumberGetTypeID();
	}
	
	PlugInfo *FillPlug = &(inMADDriver->ThePlug[inMADDriver->TotalPlug]);
	{
		FillPlug->version = CFBundleGetVersionNumber(tempBundle);
		
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugTypeKey);
		if (OpaqueDictionaryType == NULL) {
			goto badplug;
		}
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
				if (strcmp(FillPlug->type, inMADDriver->ThePlug[i].type) == 0) {
					//NSLog(@"Plug-ins %@ and %@ are similar", inMADDriver->ThePlug[i].file, tempBundle);
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
							//NSLog(@"Using %@ (Newer than previous)", tempBundle);
							return true;
						} else {
							//NSLog(@"NOT using %@ (Newer than previous, could not initialize)", tempBundle);
							goto badplug;
						}
					} else {
						//NSLog(@"NOT using %@ (Not newer than previous)", tempBundle);
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
	NSLog(@"Error with plug-in %@", tempBundle);
	return false;
}

#pragma mark Plug-in Locations
//There are many places that a plug-in might be kept in OS X
/*
 * Possible plugin places:
 * Local Application Support/PlayerPRO/PlugIns
 * User Application Support/PlayerPRO/PlugIns
 * Application Contents/PlugIns
 * Framework PlugIns
 * Application Contents/Frameworks/PlugIns
 */

static CFMutableArrayRef CreateDefaultPluginFolderLocations()
{
	@autoreleasepool {
		NSFileManager *fm = [NSFileManager defaultManager];
		CFMutableArrayRef PlugFolds = CFArrayCreateMutable(kCFAllocatorDefault, 6, &kCFTypeArrayCallBacks);
		CFURLRef temp1;
		//Application Main Bundle
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		if (mainBundle != NULL) {
			temp1 = CFBundleCopyBuiltInPlugInsURL(mainBundle);
			CFArrayAppendValue(PlugFolds, temp1);
			CFRelease(temp1);
			temp1 = NULL;
		}
		
#ifndef MAINPLAYERPRO
		mainBundle = CFBundleGetBundleWithIdentifier(CFSTR("net.sourceforge.playerpro.PlayerPROCore"));
		if (mainBundle != NULL) {
			temp1 = CFBundleCopyBuiltInPlugInsURL(mainBundle);
			CFArrayAppendValue(PlugFolds, temp1);
			CFRelease(temp1);
			temp1 = NULL;
		}
		
		mainBundle = CFBundleGetBundleWithIdentifier(CFSTR("net.sourceforge.playerpro.PlayerPROKit"));
		if (mainBundle != NULL) {
			temp1 = CFBundleCopyBuiltInPlugInsURL(mainBundle);
			CFArrayAppendValue(PlugFolds, temp1);
			CFRelease(temp1);
			temp1 = NULL;
		}

#endif
		
		//Local systemwide plugins
		temp1 = (CFURLRef)[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSLocalDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"];
		CFArrayAppendValue(PlugFolds, temp1);
		temp1 = NULL;
		
		//User plugins
		temp1 = (CFURLRef)[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"];
		CFArrayAppendValue(PlugFolds, temp1);
		temp1 = NULL;
		
		return PlugFolds;
	}
}

static Boolean CompareTwoCFURLs(CFURLRef urla, CFURLRef urlb)
{
	// Check if we're running Lion or later
	if (&kCFURLFileResourceIdentifierKey == NULL) {
		//If not, compare using FSCompareFSRefs
		FSRef refA, refB;
		Boolean bothAreGood = true;
		if (CFURLGetFSRef(urla, &refA) == false)
			bothAreGood = false;
		if (CFURLGetFSRef(urlb, &refB) == false)
			bothAreGood = false;
		if (bothAreGood) {
			if (FSCompareFSRefs(&refA, &refB) == noErr)
			{
				return true;
			}else{
				return false;
			}
		} else return false;
	} else {
		CFTypeRef refA = NULL, refB = NULL;
		Boolean bothAreValid = true;
		
		if (CFURLCopyResourcePropertyForKey(urla, kCFURLFileResourceIdentifierKey, &refA, NULL) == false)
			bothAreValid = false;
		if (CFURLCopyResourcePropertyForKey(urlb, kCFURLFileResourceIdentifierKey, &refB, NULL) == false)
			bothAreValid = false;
		Boolean theSame = false;
		if (bothAreValid) {
			theSame = CFEqual(refA, refB);
		}
		
		if (refA) {
			CFRelease(refA);
		}
		if (refB) {
			CFRelease(refB);
		}
		return theSame;
	}
}

static CFMutableArrayRef CreatePluginFolderLocationsWithFolderPath(char *UserAddedPlace)
{
	CFMutableArrayRef FoldLocs = CreateDefaultPluginFolderLocations();
	BOOL isTheSame = NO;
	CFIndex i;
	CFURLRef custfolder = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)UserAddedPlace, strlen(UserAddedPlace), true);

	for (i = 0; i < CFArrayGetCount(FoldLocs); i++) {
		CFURLRef index = CFArrayGetValueAtIndex(FoldLocs, i);
		if (CompareTwoCFURLs(custfolder, index) == true) {
			isTheSame = YES;
			break;
		}
	}
	if (isTheSame == NO) {
		CFArrayInsertValueAtIndex(FoldLocs, 0, custfolder);
	}
	CFRelease(custfolder);
	return FoldLocs;
}

OSErr PPMADInfoFile( char *AlienFile, PPInfoRec	*InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	
	theMAD = (MADSpec*) malloc( sizeof( MADSpec) + 200);
	
	fileID = iFileOpenRead( AlienFile);
	if( !fileID)
	{
		free( theMAD);
		return MADReadingErr;
	}
	fileSize = iGetEOF( fileID);
	
	iRead( sizeof( MADSpec), (Ptr) theMAD, fileID);
	iClose( fileID);
	
	strlcpy( InfoRec->internalFileName, theMAD->name, sizeof(theMAD->name));
	
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

OSErr CallImportPlug(MADLibrary				*inMADDriver,
					 short					PlugNo,			// CODE du plug
					 OSType					order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 PPInfoRec				*info)
{
	OSErr					iErr = noErr;
	
	CFBundleRefNum resFileNum = CFBundleOpenBundleResourceMap(inMADDriver->ThePlug[PlugNo].file);
	
	MADDriverSettings		driverSettings;
	
	iErr = (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
	
	CFBundleCloseBundleResourceMap(inMADDriver->ThePlug[PlugNo].file, resFileNum);
	
	return iErr;
}

void MInitImportPlug( MADLibrary *inMADDriver, char *PlugsFolderName)
{
	CFMutableArrayRef PlugLocations = NULL;
	CFArrayRef		somePlugs = NULL;
	CFIndex			PlugLocNums, PlugNums, i, x;
	
	inMADDriver->ThePlug = (PlugInfo*) calloc( MAXPLUG, sizeof( PlugInfo));
	inMADDriver->TotalPlug = 0;
	if (PlugsFolderName == NULL) {
		PlugLocations = CreateDefaultPluginFolderLocations();
	} else {
		PlugLocations = CreatePluginFolderLocationsWithFolderPath(PlugsFolderName);
	}
	PlugLocNums	= CFArrayGetCount( PlugLocations );
	
	for (i = 0; i < PlugLocNums; i++) {
		CFURLRef aPlugLoc = CFArrayGetValueAtIndex(PlugLocations, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, CFSTR("ppimpexp"));
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				MakeMADPlug(inMADDriver, tempBundleRef);
				//We do this to prevent resource/memory leak
				//Read the documentation for CFBundleCreateBundlesFromDirectory for more info
				//If the plug-in creation succeeded, it will bump the ref count from two (one from the CFArray) to three
				//And we can safely release it to get the proper retain count needed
				//If plug-in creation failed, the retain count is two and we probably don't want anything to do with the plug-in
				CFRelease(tempBundleRef);
			}
		}
		CFRelease(somePlugs);
	}
	CFRelease(PlugLocations);
	PlugLocations = NULL;
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
		return PPMADInfoFile( AlienFile, InfoRec);
	}
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return CallImportPlug( inMADDriver, i, MADPlugInfo, AlienFile, &aMAD, InfoRec);
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
			
			return CallImportPlug( inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSErr CheckMADFile(char* name)
{
	UNFILE				refNum;
	char				charl[CharlMADcheckLength];
	OSErr				err;
	
	refNum = iFileOpenRead( name);
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
	refNum = iFileOpenRead( AlienFile);
	if( !refNum) return MADReadingErr;
	else
	{
		if( iGetEOF( refNum) < 100) iErr = MADIncompatibleFile;
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
		if( CallImportPlug( inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == noErr)
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
			return CallImportPlug( inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
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
			return CallImportPlug( inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSType GetPPPlugType( MADLibrary *inMADDriver, short ID, OSType mode)
{
	short	i, x;
	
	if( ID >= inMADDriver->TotalPlug) PPDebugStr( __LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for( i = 0, x = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( inMADDriver->ThePlug[ i].mode == mode || inMADDriver->ThePlug[ i].mode == MADPlugImportExport)
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
	
	PPDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}
