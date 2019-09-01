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
#include "MADFileUtils.h"
#include "MADPrivate.h"

#define CharlMADcheckLength 10

#include "embeddedPlugs.h"

typedef struct iPlugInfo {
	OSType		mode;
	UInt32		version;
	CFStringRef	MenuName;
	CFStringRef	AuthorString;
	CFStringRef *UTITypes;
	MADPLUGFUNC	IOPlug;
	char		type[5];
} iPlugInfo;

#define PLUGVERS(a, b, c, d) (a << 24 | b << 16 | (c | finalStage) << 8 | d)

static const iPlugInfo iOSPlugInfo[] = {
	{
		.IOPlug = main669,
		.MenuName = CFSTR("669 Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "6669",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.669"), NULL},
	},
	{
		.IOPlug = mainAMF,
		.MenuName = CFSTR("AMF Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "AMF ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.amf"), NULL},
	},
	{
		.IOPlug = mainDMF,
		.MenuName = CFSTR("DMF Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "DMF ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.dmf"), NULL},
	},
	//Disabled on iOS because it isn't playing correctly
#if TARGET_OS_OSX
	{
		.IOPlug = mainIT,
		.MenuName = CFSTR("ImpulseTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "IT  ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.on.users.jtlim.ImpulseTracker.it"), CFSTR("org.videolan.it"), NULL},
	},
#endif
	{
		.IOPlug = mainMADfg,
		.MenuName = CFSTR("MAD-fg Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADF",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("com.quadmation.playerpro.madfg"), NULL},
	},
	{
		.IOPlug = mainMADH,
		.MenuName = CFSTR("MADH Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADH",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("com.quadmation.playerpro.madh"), NULL},
	},
	{
		.IOPlug = mainMADI,
		.MenuName = CFSTR("MADI Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MADI",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("com.quadmation.playerpro.madi"), NULL},
	},
	{
		.IOPlug = mainMED,
		.MenuName = CFSTR("OctaMED Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MED ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("com.medsoundstudio.octamed"), NULL},
	},
	{
		.IOPlug = mainMOD,
		.MenuName = CFSTR("FastTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "STrk",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImportExport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.mod"), NULL},
	},
	{
		.IOPlug = mainMTM,
		.MenuName = CFSTR("MTM Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "MTM ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.mtm"), NULL},
	},
	{
		.IOPlug = mainOkta,
		.MenuName = CFSTR("Oktalyzer Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "Okta",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.okta"), NULL},
	},
	{
		.IOPlug = mainS3M,
		.MenuName = CFSTR("ScreamTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "S3M ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImportExport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.s3m"), CFSTR("org.videolan.s3m"), NULL},
	},
	{
		.IOPlug = mainULT,
		.MenuName = CFSTR("ULT Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "ULT ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.ult"), NULL},
	},
	//disabled because it conflicts with XM
#if 0
	{
		.IOPlug = mainUMX,
		.MenuName = CFSTR("UMX Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "UMX ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.umx"), NULL},
	},
#endif
	{
		.IOPlug = mainXM,
		.MenuName = CFSTR("FastTracker Files"),
		.AuthorString = CFSTR("Antoine Rosset"),
		.type = "XM  ",
		.version = PLUGVERS(2, 0, 0, 0),
		.mode = MADPlugImportExport,
		.UTITypes = (CFStringRef[]){CFSTR("net.sourceforge.playerpro.xm"), CFSTR("org.videolan.xm"), NULL},
	}
};

static MADErr PPMADInfoFile(const char *AlienFile, MADInfoRec *InfoRec)
{
	MADSpec	*theMAD;
	long	fileSize;
	UNFILE	fileID;
	MADErr	MADCheck;
	
	if ((MADCheck = CheckMADFile(AlienFile)) != MADNoErr) {
		return MADCheck;
	}

	theMAD = malloc(sizeof(MADSpec) + 200);
	
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
	
	free(theMAD); theMAD = NULL;
	
	return MADNoErr;
}

MADErr CallImportPlug(MADLibrary *inMADDriver, int PlugNo, OSType order, char *AlienFile, MADMusic *theNewMAD, MADInfoRec *info)
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
	CFMutableArrayRef tmpArray = CFArrayCreateMutable(kCFAllocatorDefault, 5, &kCFTypeArrayCallBacks);
	int i = 0;
	while (src->UTITypes[i]) {
		CFStringRef tmpStr = src->UTITypes[i++];
		CFArrayAppendValue(tmpArray, tmpStr);
	}
	dst->UTItypes = CFArrayCreateCopy(kCFAllocatorDefault, tmpArray);
	CFRelease(tmpArray);
	dst->file = CFBundleGetBundleWithIdentifier(CFSTR("net.sourceforge.playerpro.PlayerPROCore"));
	if (dst->file == NULL) {
		dst->file = CFBundleGetMainBundle();
	}
	if (dst->file) {
		CFRetain(dst->file);
	}
}

void MInitImportPlug(MADLibrary *inMADDriver, const char *PlugsFolderName)
{
	if (PlugsFolderName) {
		fprintf(stderr, "PlayerPROCore: Custom plug-in path %s ignored: This configuration cannot load custom binaries.\n", PlugsFolderName);
	}
	CFIndex totalInterfaces = sizeof(iOSPlugInfo) / sizeof(iOSPlugInfo[0]);
	inMADDriver->ThePlug = (PlugInfo*)calloc(sizeof(PlugInfo), totalInterfaces);
	inMADDriver->TotalPlug = 0;
	for (CFIndex i = 0; i < totalInterfaces; i++) {
		if (inMADDriver->TotalPlug - 1 < totalInterfaces) {
			MovePluginInfoOver(&iOSPlugInfo[i], &inMADDriver->ThePlug[inMADDriver->TotalPlug]);
			inMADDriver->TotalPlug++;
		}
	}
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	int i;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		CFRelease(inMADDriver->ThePlug[i].AuthorString);
		CFRelease(inMADDriver->ThePlug[i].UTItypes);
		CFRelease(inMADDriver->ThePlug[i].MenuName);
		if (inMADDriver->ThePlug[i].file) {
			CFRelease(inMADDriver->ThePlug[i].file);
		}
	}
	free(inMADDriver->ThePlug);
	inMADDriver->ThePlug = NULL;
}

MADErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADInfoRec *InfoRec)
{
	int			i;
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
	int			i;
	MADInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			*theNewMAD = (MADMusic*)calloc(sizeof(MADMusic), 1);
			if (!theNewMAD)
				return MADNeedMemory;
			
			MADErr err = CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
			if (err != MADNoErr) {
				free(*theNewMAD);
				*theNewMAD = NULL;
			}
			return err;
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
	int			i;
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
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == MADNoErr) {
			strcpy(type, inMADDriver->ThePlug[i].type);
			return MADNoErr;
		}
	}
	
	return MADCannotFindPlug;
}

bool MADPlugAvailable(const MADLibrary *inMADDriver, const char* kindFile)
{
	int i;
	
	if (!strncmp(kindFile, "MADK", 4))
		return TRUE;
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strncmp(kindFile, inMADDriver->ThePlug[i].type, 4))
			return TRUE;
	}
	
	return FALSE;
}

MADErr PPExportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	int			i;
	MADInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strncmp(kindFile, inMADDriver->ThePlug[i].type, 4))
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
	}
	return MADCannotFindPlug;
}

MADErr PPTestFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile)
{
	int			i;
	MADMusic	aMAD;
	MADInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
	}
	return MADCannotFindPlug;
}

OSType GetPPPlugType(MADLibrary *inMADDriver, short ID, OSType mode)
{
	short i, x;
	
	if (ID >= inMADDriver->TotalPlug) MADDebugStr(__LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				short	xx;
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
