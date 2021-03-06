/*
 *  OSX-PlugImport.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#import <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADFileUtils.h"
#include "MADPrivate.h"
#include "MADPlug.h"
#define CharlMADcheckLength 10

const CFStringRef kMadPlugMenuNameKey =		CFSTR("MADPlugMenuName");
const CFStringRef kMadPlugAuthorNameKey =	CFSTR("MADPlugAuthorName");
const CFStringRef kMadPlugUTITypesKey =		CFSTR("MADPlugUTITypes");
const CFStringRef kMadPlugTypeKey =			CFSTR("MADPlugType");
const CFStringRef kMadPlugDoesImport =		CFSTR("MADCanImport");
const CFStringRef kMadPlugDoesExport =		CFSTR("MADCanExport");
const CFStringRef kMadPlugModeKey =			CFSTR("MADPlugMode");

static CFTypeID stringtype;
static CFTypeID numbertype;
static CFTypeID arraytype;
static CFTypeID booleantype;

typedef NS_OPTIONS(unsigned char, MADPlugCapabilities) {
	PPMADCanDoNothing = 0,
	PPMADCanImport = 1 << 0,
	PPMADCanExport = 1 << 1,
	PPMADCanDoBoth = PPMADCanImport | PPMADCanExport
};

static BOOL GetBoolFromType(CFTypeRef theType)
{
	CFTypeID theID = CFGetTypeID(theType);
	if (theID == booleantype) {
		return CFBooleanGetValue(theType);
	} else if (theID == numbertype) {
		int theVal = 0;
		CFNumberGetValue(theType, kCFNumberIntType, &theVal);
		return theVal != 0;
	} else if (theID == stringtype) {
		return [(__bridge NSString*)theType boolValue];
	} else
		return NO;
}

static BOOL fillPlugFromBundle(CFBundleRef theBundle, PlugInfo *thePlug)
{
	{
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;

		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugMenuNameKey);
		if (OpaqueDictionaryType == NULL)
			goto badplug;
		
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			thePlug->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
		} else
			goto badplug;
		
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
					if (GetBoolFromType(importValue))
						possibilities = PPMADCanImport;
				}
				
				if (exportValue != NULL) {
					if (GetBoolFromType(exportValue))
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
				if (OpaqueDictionaryType == NULL)
					goto badplug3;
				
				InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
				if (InfoDictionaryType == stringtype) {
					char fallbackOSType[20] = {0};
					const char *thecOSType = NULL;
					if (CFStringGetCString((CFStringRef)OpaqueDictionaryType, fallbackOSType, sizeof(fallbackOSType), kCFStringEncodingMacRoman))
						thecOSType = fallbackOSType;
					if (thecOSType == NULL)
						goto badplug3;
					
					thePlug->mode = Ptr2OSType(thecOSType);
				} else if (InfoDictionaryType == numbertype) {
					OSType theplugType;
					CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberSInt32Type, &theplugType);
					thePlug->mode = theplugType;
				} else
					goto badplug3;
			}
		}
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(theBundle, kMadPlugUTITypesKey);
		if (OpaqueDictionaryType == NULL)
			goto badplug3;
		
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == arraytype) {
			thePlug->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, (CFArrayRef)OpaqueDictionaryType);
		} else if (InfoDictionaryType == stringtype) {
			NSArray *utiArray = @[[NSString stringWithString:(__bridge NSString*)OpaqueDictionaryType]];
			thePlug->UTItypes = CFBridgingRetain(utiArray);
		} else
			goto badplug3;
	}
	
	thePlug->IOPlug = CFBundleGetFunctionPointerForName(theBundle, CFSTR("PPImpExpMain"));
	if (!thePlug->IOPlug)
		goto badplug4;
	thePlug->file = theBundle;
	CFRetain(thePlug->file);
	
	return YES;
	
badplug4:
	CFRelease(thePlug->UTItypes);
badplug3:
	CFRelease(thePlug->AuthorString);
	CFRelease(thePlug->MenuName);
badplug:
	return NO;
}

static BOOL MakeMADPlug(MADLibrary *inMADDriver, CFBundleRef tempBundle)
{
	static dispatch_once_t typeIDToken;
	PlugInfo *FillPlug;
	dispatch_once(&typeIDToken, ^{
		stringtype = CFStringGetTypeID();
		numbertype = CFNumberGetTypeID();
		arraytype = CFArrayGetTypeID();
		booleantype = CFBooleanGetTypeID();
	});
	
	if (!tempBundle) {
		return NO;
	}
	
	if ((inMADDriver->TotalPlug + 1) >= MAXPLUG) {
		MADDebugStr(__LINE__, __FILE__, "More plugs than allocated for!");
		return NO;
	}
	
	FillPlug = &(inMADDriver->ThePlug[inMADDriver->TotalPlug]);
	{
		CFTypeID InfoDictionaryType;
		CFTypeRef OpaqueDictionaryType;
		BOOL filled;
		FillPlug->version = CFBundleGetVersionNumber(tempBundle);
		
		
		OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundle, kMadPlugTypeKey);
		if (OpaqueDictionaryType == NULL) {
			goto badplug;
		}
		InfoDictionaryType = CFGetTypeID(OpaqueDictionaryType);
		if (InfoDictionaryType == stringtype) {
			size_t strlength = 0;
			char fallbackOSType[20] = {0};
			const char * tempstring = NULL;
			if (CFStringGetCString((CFStringRef)OpaqueDictionaryType, fallbackOSType, sizeof(fallbackOSType), kCFStringEncodingMacRoman))
				tempstring = fallbackOSType;
			if (tempstring == NULL)
				goto badplug;
			strcpy(FillPlug->type, "    ");
			strlength = strlen(tempstring);
			if (strlength > 4)
				strlength = 4;
			
			for (int i = 0; i < strlength; i++) {
				if (tempstring[i] == 0) {
					FillPlug->type[i] = ' ';
				} else
					FillPlug->type[i] = tempstring[i];
			}
			FillPlug->type[4] = 0;
		} else if (InfoDictionaryType == numbertype) {
			OSType theplugType;
			CFNumberGetValue((CFNumberRef)OpaqueDictionaryType, kCFNumberSInt32Type, &theplugType);
			OSType2Ptr(theplugType, FillPlug->type);
		} else
			goto badplug;
		
		{
			//Check to see if there's a plug-in that matches the type.
			for (int i = 0; i < inMADDriver->TotalPlug; i++) {
				if (strcmp(FillPlug->type, inMADDriver->ThePlug[i].type) == 0) {
					if (inMADDriver->ThePlug[i].version < FillPlug->version) {
						PlugInfo newInfo;
						Boolean gotFilled = fillPlugFromBundle(tempBundle, &newInfo);
						if (gotFilled) {
							CFRelease(inMADDriver->ThePlug[i].AuthorString);
							CFRelease(inMADDriver->ThePlug[i].file);
							CFRelease(inMADDriver->ThePlug[i].MenuName);
							CFRelease(inMADDriver->ThePlug[i].UTItypes);
							strcpy(newInfo.type, inMADDriver->ThePlug[i].type);
							inMADDriver->ThePlug[i] = newInfo;
							inMADDriver->ThePlug[i].version = CFBundleGetVersionNumber(tempBundle);
							memset(FillPlug, 0, sizeof(PlugInfo));
							return true;
						} else {
							goto badplug;
						}
					} else {
						memset(FillPlug, 0, sizeof(PlugInfo));
						return false;
					}
				}
			}
		}
		
		filled = fillPlugFromBundle(tempBundle, FillPlug);
		if (filled) {
			inMADDriver->TotalPlug++;
			return YES;
		} else {
			goto badplug;
		}
	}
badplug:
	memset(FillPlug, 0, sizeof(PlugInfo));
	return NO;
}

#pragma mark Plug-in Locations
//There are many places that a plug-in might be kept in OS X
/*
 * Possible plugin places:
 * Local Application Support/PlayerPRO/PlugIns
 * User Application Support/PlayerPRO/PlugIns
 * Application Contents/PlugIns
 * Framework PlugIns
 */

static CFMutableArrayRef CreateDefaultPluginFolderLocations(void) CF_RETURNS_RETAINED;
CFMutableArrayRef CreateDefaultPluginFolderLocations(void)
{
	@autoreleasepool {
		NSFileManager *fm = [NSFileManager defaultManager];
		NSMutableArray *PlugFolds = [NSMutableArray arrayWithCapacity:6];
		NSURL *aURL;
		//Application Main Bundle
		NSBundle *mainBundle = [NSBundle mainBundle];
		if (mainBundle != nil) {
			aURL = [mainBundle builtInPlugInsURL];
			if ([aURL checkResourceIsReachableAndReturnError:NULL]) {
				[PlugFolds addObject:aURL];
			}
		}
		
		mainBundle = [NSBundle bundleWithIdentifier:@"net.sourceforge.playerpro.PlayerPROCore"];
		if (mainBundle != nil) {
			aURL = [mainBundle builtInPlugInsURL];
			if ([aURL checkResourceIsReachableAndReturnError:NULL]) {
				[PlugFolds addObject:aURL];
			}
		}
		
		mainBundle = [NSBundle bundleWithIdentifier:@"net.sourceforge.playerpro.PlayerPROKit"];
		if (mainBundle != nil) {
			aURL = [mainBundle builtInPlugInsURL];
			if ([aURL checkResourceIsReachableAndReturnError:NULL]) {
				[PlugFolds addObject:aURL];
			}
		}
		
		//Local systemwide plugins
		aURL = [NSURL fileURLWithPathComponents: @[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSLocalDomainMask appropriateForURL:nil create:NO error:NULL] path], @"PlayerPRO", @"Plugins"]];
		if ([aURL checkResourceIsReachableAndReturnError:NULL]) {
			[PlugFolds addObject:aURL];
		}
		
		//User plugins
		aURL = [NSURL fileURLWithPathComponents: @[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:NO error:NULL] path], @"PlayerPRO", @"Plugins"]];
		if ([aURL checkResourceIsReachableAndReturnError:NULL]) {
			[PlugFolds addObject:aURL];
		}
		
		return (CFMutableArrayRef)CFBridgingRetain(PlugFolds);
	}
}

static BOOL CompareTwoNSURLs(NSURL *urla, NSURL *urlb)
{
	id refA = nil, refB = nil;
	BOOL bothAreValid = YES;
	BOOL theSame = NO;
	
	if ([urla getResourceValue:&refA forKey:NSURLFileResourceIdentifierKey error:NULL] == NO) {
		bothAreValid = NO;
	} else if ([urlb getResourceValue:&refB forKey:NSURLFileResourceIdentifierKey error:NULL] == NO) {
		bothAreValid = NO;
	}
	
	if (bothAreValid)
		theSame = [refA isEqual:refB];
	
	return theSame;
}

static inline bool CompareTwoCFURLs(CFURLRef urla, CFURLRef urlb)
{
	@autoreleasepool {
		return CompareTwoNSURLs((__bridge NSURL*)urla, (__bridge NSURL*)urlb);
	}
}

static CFMutableArrayRef CreatePluginFolderLocationsWithFolderPath(const char *UserAddedPlace) CF_RETURNS_RETAINED;
CFMutableArrayRef CreatePluginFolderLocationsWithFolderPath(const char *UserAddedPlace)
{
	CFMutableArrayRef FoldLocs = CreateDefaultPluginFolderLocations();
	BOOL isTheSame = NO;
	CFURLRef custfolder = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8*)UserAddedPlace, strlen(UserAddedPlace), true);

	for (CFIndex i = 0; i < CFArrayGetCount(FoldLocs); i++) {
		CFURLRef index = CFArrayGetValueAtIndex(FoldLocs, i);
		if (CompareTwoCFURLs(custfolder, index) == true) {
			isTheSame = YES;
			break;
		}
	}
	if (isTheSame == NO)
		CFArrayInsertValueAtIndex(FoldLocs, 0, custfolder);
	
	CFRelease(custfolder);
	return FoldLocs;
}

static MADErr PPMADInfoFile(const char *AlienFile, MADInfoRec *InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	MADErr		MADCheck;
	
	if ((MADCheck = CheckMADFile(AlienFile)) != MADNoErr) {
		return MADCheck;
	}
	
	theMAD = (MADSpec*)malloc(sizeof(MADSpec) + 200);
	
	fileID = iFileOpenRead(AlienFile);
	if (!fileID) {
		free(theMAD);
		return MADReadingErr;
	}
	
	fileSize = iGetEOF(fileID);
	iRead(sizeof(MADSpec), theMAD, fileID);
	iClose(fileID);
	
	strlcpy(InfoRec->internalFileName, theMAD->name, sizeof(theMAD->name));
	
	InfoRec->totalPatterns = theMAD->numPat;
	InfoRec->partitionLength = theMAD->numPointers;
	InfoRec->totalTracks = theMAD->numChn;
	InfoRec->signature = 'MADK';
	strcpy(InfoRec->formatDescription, "MADK");
	InfoRec->totalInstruments = theMAD->numInstru;
	InfoRec->fileSize = fileSize;
	free(theMAD);

	return MADNoErr;
}

MADErr CallImportPlug(MADLibrary			*inMADDriver,
					 int					PlugNo,			// CODE du plug
					 MADFourChar			order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 MADInfoRec				*info)
{
	MADErr				iErr = MADNoErr;
	MADDriverSettings	driverSettings = {0};
	
	iErr = (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
	
	return iErr;
}

void MInitImportPlug(MADLibrary *inMADDriver, const char *PlugsFolderName)
{
	CFMutableArrayRef	PlugLocations = NULL;
	
	inMADDriver->ThePlug = (PlugInfo*)calloc(MAXPLUG, sizeof(PlugInfo));
	inMADDriver->TotalPlug = 0;
	if (PlugsFolderName == NULL) {
		PlugLocations = CreateDefaultPluginFolderLocations();
	} else {
		PlugLocations = CreatePluginFolderLocationsWithFolderPath(PlugsFolderName);
	}
	
	@autoreleasepool {
		NSMutableArray *plugLoc = CFBridgingRelease(PlugLocations);
		PlugLocations = NULL;
		for (NSURL *aPlugLoc in plugLoc) {
			CFArrayRef somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef)aPlugLoc, CFSTR("ppimpexp"));
			CFIndex PlugNums = CFArrayGetCount(somePlugs);
			for (CFIndex x = 0; x < PlugNums; x++) {
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				MakeMADPlug(inMADDriver, tempBundleRef);
				//We do this to prevent resource/memory leak
				//Read the documentation for CFBundleCreateBundlesFromDirectory for more info
				CFRelease(tempBundleRef);
			}
			CFRelease(somePlugs);
		}
	}
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		CFRelease(inMADDriver->ThePlug[i].file);
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		CFRelease(inMADDriver->ThePlug[i].UTItypes);
		CFRelease(inMADDriver->ThePlug[i].MenuName);
	}
	free(inMADDriver->ThePlug);
	inMADDriver->ThePlug = NULL;
}

MADErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADInfoRec *InfoRec)
{
	MADMusic aMAD;
	
	if (!strcmp(kindFile, "MADK"))
		return PPMADInfoFile(AlienFile, InfoRec);
	
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugInfo, AlienFile, &aMAD, InfoRec);
		}
	}
	return MADCannotFindPlug;
}

MADErr PPImportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	MADInfoRec InfoRec = {0};
	
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			MADErr iErr;
			*theNewMAD = (MADMusic*)calloc(sizeof(MADMusic), 1);
			if (!theNewMAD)
				return MADNeedMemory;
			
			iErr = CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
			if (iErr != MADNoErr) {
				free(*theNewMAD);
				*theNewMAD = NULL;
			}
			return iErr;
		}
	}
	return MADCannotFindPlug;
}

MADErr CheckMADFile(const char* name)
{
	UNFILE	refNum;
	char	charl[CharlMADcheckLength];
	MADErr	err;
	
	refNum = iFileOpenRead(name);
	if (!refNum)
		return MADReadingErr;
	else {
		iRead(CharlMADcheckLength, charl, refNum);
		
		if (memcmp("MADK", charl, 4) == 0) {
			err = MADNoErr;
		} else {
			err = MADFileNotSupportedByThisPlug;
		}
		
		iClose(refNum);
	}
	return err;
}

MADErr PPIdentifyFile(MADLibrary *inMADDriver, char *type, char *AlienFile)
{
	UNFILE		refNum;
	MADInfoRec	InfoRec;
	MADErr		iErr = MADNoErr;
	
	strcpy(type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpenRead(AlienFile);
	if (!refNum)
		return MADReadingErr;
	else {
		if (iGetEOF(refNum) < 100)
			iErr = MADIncompatibleFile;
		iClose(refNum);
		if (iErr)
			return iErr;
	}
	
	// Is it a MAD file?
	iErr = CheckMADFile(AlienFile);
	if (iErr == MADNoErr) {
		strcpy(type, "MADK");
		return MADNoErr;
	}
	
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == MADNoErr) {
			strcpy(type, inMADDriver->ThePlug[i].type);
			return MADNoErr;
		}
	}
	
	return MADCannotFindPlug;
}

bool MADPlugAvailable(const MADLibrary *inMADDriver, const char* kindFile)
{
	if (!strcmp(kindFile, "MADK"))
		return true;
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return true;
	}
	return false;
}

#if !defined(NOEXPORTMUSIC) || NOEXPORTMUSIC == 0
MADErr PPExportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	MADInfoRec InfoRec;
	
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}
#endif

MADErr PPTestFile(MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile)
{
	MADMusic	aMAD;
	MADInfoRec	InfoRec;
	
	for (int i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

MADFourChar GetPPPlugType(MADLibrary *inMADDriver, short ID, MADFourChar mode)
{
	int i, x;
	
	if (ID >= inMADDriver->TotalPlug)
		MADDebugStr(__LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				size_t	xx;
				OSType	type = '    ';
				
				xx = strlen(inMADDriver->ThePlug[i].type);
				if (xx > 4)
					xx = 4;
				memcpy(inMADDriver->ThePlug[i].type, &type, xx);
				MADBE32(&type);
				return type;
			}
			x++;
		}
	}
	
	MADDebugStr(__LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return '!!!!';
}
