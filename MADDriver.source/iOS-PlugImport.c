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

#define CharlMADcheckLength 10

#define MAXPLUG	40

#include "embeddedPlugs.h"

typedef struct iPlugInfo {
	OSType		mode;
	UInt32		version;
	CFStringRef	MenuName;
	CFStringRef	AuthorString;
	CFStringRef UTIType;
	MADPLUGFUNC	IOPlug;
	char		type[5];
} iPlugInfo;

#define PLUGVERS 2 << 24 | 0 << 16 | 0 << 8 | 0

static const iPlugInfo iOSPlugInfo[] = {
	{
		.IOPlug = main669,
		.MenuName = CFSTR("669 Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "6669",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.669"),
	},
	{
		.IOPlug = mainAMF,
		.MenuName = CFSTR("AMF Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "AMF ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.amf"),
	},
	{
		.IOPlug = mainDMF,
		.MenuName = CFSTR("DMF Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "DMF ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.dmf"),
	},
	//Disabled on iOS because it isn't playing correctly
#if !TARGET_OS_IPHONE
	{
		.IOPlug = mainIT,
		.MenuName = CFSTR("ImpulseTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "IT  ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.it"),
	},
#endif
	{
		.IOPlug = mainMADfg,
		.MenuName = CFSTR("MAD-fg Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADF",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("com.quadmation.playerpro.madfg"),
	},
	{
		.IOPlug = mainMADH,
		.MenuName = CFSTR("MADH Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADH",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("com.quadmation.playerpro.madh"),
	},
	{
		.IOPlug = mainMADI,
		.MenuName = CFSTR("MADI Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADI",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("com.quadmation.playerpro.madi"),
	},
	{
		.IOPlug = mainMED,
		.MenuName = CFSTR("MED Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MED ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.med"),
	},
	{
		.IOPlug = mainMOD,
		.MenuName = CFSTR("FastTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "STrk",
		.version = PLUGVERS,
		.mode = MADPlugImportExport,
		.UTIType = CFSTR("net.sourceforge.playerpro.mod"),
	},
	{
		.IOPlug = mainMTM,
		.MenuName = CFSTR("MTM Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MTM ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.mtm"),
	},
	{
		.IOPlug = mainOkta,
		.MenuName = CFSTR("OktaMed Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "Okta",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.okta"),
	},
	{
		.IOPlug = mainS3M,
		.MenuName = CFSTR("ScreamTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "S3M ",
		.version = PLUGVERS,
		.mode = MADPlugImportExport,
		.UTIType = CFSTR("net.sourceforge.playerpro.med"),
	},
	{
		.IOPlug = mainULT,
		.MenuName = CFSTR("ULT Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "ULT ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.ult"),
	},
	//disabled because it conflicts with XM
#if 0
	{
		.IOPlug = mainUMX,
		.MenuName = CFSTR("UMX Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "UMX ",
		.version = PLUGVERS,
		.mode = MADPlugImport,
		.UTIType = CFSTR("net.sourceforge.playerpro.umx"),
	},
#endif
	{
		.IOPlug = mainXM,
		.MenuName = CFSTR("FastTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "XM  ",
		.version = PLUGVERS,
		.mode = MADPlugImportExport,
		.UTIType = CFSTR("net.sourceforge.playerpro.xm"),
	}
};

static MADErr PPMADInfoFile(const char *AlienFile, PPInfoRec *InfoRec)
{
	MADSpec	*theMAD;
	long	fileSize;
	UNFILE	fileID;
	
	theMAD = (MADSpec*) malloc(sizeof(MADSpec) + 200);
	
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
	theMAD = NULL;
	
	return MADNoErr;
}

MADErr CallImportPlug(MADLibrary *inMADDriver, short PlugNo, OSType order, char *AlienFile, MADMusic *theNewMAD, PPInfoRec *info)
{
	MADDriverSettings driverSettings = {0};
	
	return (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
}

static void MovePluginInfoOver(const iPlugInfo *src, PlugInfo *dst)
{
	dst->IOPlug = src->IOPlug;
	dst->MenuName = CFStringCreateCopy(kCFAllocatorDefault, src->MenuName);
	dst->AuthorString = CFStringCreateCopy(kCFAllocatorDefault, src->AuthorString);
	strcpy(dst->type, src->type);
	dst->mode = src->mode;
	dst->version = src->version;
	CFMutableArrayRef tmpArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
	CFStringRef tmpStr = CFStringCreateCopy(kCFAllocatorDefault, src->UTIType);
	CFArrayAppendValue(tmpArray, tmpStr);
	CFRelease(tmpStr);
	dst->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, tmpArray);
	CFRelease(tmpArray);
#if !TARGET_OS_IPHONE
	dst->file = CFBundleGetMainBundle();
	if (dst->file) {
		CFRetain(dst->file);
	}
#endif
}

void MInitImportPlug(MADLibrary *inMADDriver, const char *PlugsFolderName)
{
	if (PlugsFolderName) {
		fprintf(stderr, "Custom plug-in path %s ignored: the static libraries cannot load custom binaries.\n", PlugsFolderName);
	}
	CFIndex i, totalInterfaces;
	totalInterfaces = sizeof(iOSPlugInfo) / sizeof(iOSPlugInfo[0]);
	inMADDriver->ThePlug = (PlugInfo*)calloc(sizeof(PlugInfo), totalInterfaces);
	inMADDriver->TotalPlug = 0;
	for (i = 0; i < totalInterfaces; i++) {
		if (inMADDriver->TotalPlug - 1 < totalInterfaces) {
			MovePluginInfoOver(&iOSPlugInfo[i], &inMADDriver->ThePlug[inMADDriver->TotalPlug]);
			inMADDriver->TotalPlug++;
		}
	}
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	short i;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		CFRelease(inMADDriver->ThePlug[i].UTItypes);
		CFRelease(inMADDriver->ThePlug[i].MenuName);
#if !TARGET_OS_IPHONE
		if (inMADDriver->ThePlug[i].file) {
			CFRelease(inMADDriver->ThePlug[i].file);
		}
#endif
	}
	free(inMADDriver->ThePlug);
	inMADDriver->ThePlug = NULL;
}

MADErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short		i;
	MADMusic	aMAD;
	
	if (!strcmp(kindFile, "MADK"))
		return PPMADInfoFile(AlienFile, InfoRec);
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugInfo, AlienFile, &aMAD, InfoRec);
		}
	}
	
	return MADCannotFindPlug;
}

MADErr PPImportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			*theNewMAD = (MADMusic*)calloc(sizeof(MADMusic), 1);
			if (!theNewMAD)
				return MADNeedMemory;
			
			OSErr err = CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
			if (err != MADNoErr) {
				free(*theNewMAD);
				*theNewMAD = NULL;
			}
			return err;
		}
	}
	
	return MADCannotFindPlug;
}

OSErr CheckMADFile(char* name)
{
	UNFILE	refNum;
	char	charl[CharlMADcheckLength];
	OSErr	err;
	
	refNum = iFileOpenRead(name);
	if (!refNum)
		return MADReadingErr;
	else {
		iRead(CharlMADcheckLength, charl, refNum);
		
		if (charl[0] == 'M' &&	// MADK
			charl[1] == 'A' &&
			charl[2] == 'D' &&
			charl[3] == 'K')
			err = MADNoErr;
		else
			err = MADIncompatibleFile;
		
		iClose(refNum);
	}
	
	return err;
}

MADErr PPIdentifyFile(MADLibrary *inMADDriver, char *type, char *AlienFile)
{
	UNFILE		refNum;
	short		i;
	PPInfoRec	InfoRec;
	OSErr		iErr = MADNoErr;
	
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
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == MADNoErr) {
			strcpy(type, inMADDriver->ThePlug[i].type);
			return MADNoErr;
		}
	}
	
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable(MADLibrary *inMADDriver, char* kindFile)
{
	short i;
	
	if (!strcmp(kindFile, "MADK"))
		return TRUE;
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return TRUE;
	}
	
	return FALSE;
}

MADErr PPExportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
	}
	return MADCannotFindPlug;
}

OSErr PPTestFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile)
{
	short		i;
	MADMusic	aMAD;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
	}
	return MADCannotFindPlug;
}

OSType GetPPPlugType(MADLibrary *inMADDriver, short ID, OSType mode)
{
	short i, x;
	
	if (ID >= inMADDriver->TotalPlug) PPDebugStr(__LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				short	xx;
				OSType	type = '    ';
				
				xx = strlen(inMADDriver->ThePlug[i].type);
				if (xx > 4)
					xx = 4;
				memcpy(inMADDriver->ThePlug[i].type, &type, xx);
				PPBE32(&type);
				return type;
			}
			x++;
		}
	}
	
	PPDebugStr(__LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return '!!!!';
}
