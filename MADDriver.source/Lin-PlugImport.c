#include "RDriver.h"
#include "MADFileUtils.h"
#include "MADPrivate.h"
#include <dlfcn.h>
#include <dirent.h>

// TODO: get plug-in paths from an environment variable.

#ifndef PPCPlugSuffix
#define PPCPlugSuffix ".so"
#endif

#define CharlMADcheckLength 10
typedef MADErr (*FILLPLUG)(PlugInfo *);

MADErr PPMADInfoFile(const char *AlienFile, MADInfoRec *InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	
	theMAD = (MADSpec*)malloc(sizeof(MADSpec) + 200);
	
	fileID = iFileOpenRead(AlienFile);
	if (!fileID) {
		free(theMAD);
		return MADReadingErr;
	}
	fileSize = iGetEOF(fileID);
	
	iRead(sizeof(MADSpec), theMAD, fileID);
	iClose(fileID);
	
	strcpy(InfoRec->internalFileName, theMAD->name);
	
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

MADErr CallImportPlug(MADLibrary	*inMADDriver,
					  short			PlugNo,			// CODE du plug
					  MADFourChar	order,
					  char			*AlienFile,
					  MADMusic		*theNewMAD,
					  MADInfoRec		*info)
{
	MADDriverSettings driverSettings = {0};
	
	return (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
}

static int PlugFileCheck(const struct dirent *toTest)
{
	// Deliberately skip over non-regular files,
	// including directories and symlinks.
	// TODO: go through symlinks and check if pointing to an already-existing item
	if (toTest->d_type == DT_REG || toTest->d_type == DT_UNKNOWN) {
		// Skip files starting with a dot, as these are regularly system files.
		if (toTest->d_name[0] == '.') {
			return 0;
		}
		// ...and exclude files that don't start with "lib"
		if (strncmp("lib", toTest->d_name, 3) != 0) {
			return 0;
		}
		// Also exclude items that don't have PPCPlugSuffix anywhere in it.
		// The default is ".so" on *NIX platforms
		if (strstr(toTest->d_name, PPCPlugSuffix) == NULL) {
			return 0;
		}
		
		return 1;
	}
	
	return 0;
}

static char **listDirContents(const char *dirName)
{
	struct dirent **nameList;
	int numTypes = scandir(dirName, &nameList, PlugFileCheck, NULL);
	char **toRet = calloc(numTypes + 1, sizeof(char));
	bool hasTrailingSlash = false;
	int i;
	do {
		size_t finalLen = strnlen(dirName, PATH_MAX);
		if (finalLen == PATH_MAX) {
			hasTrailingSlash = false;
			break;
		}
		hasTrailingSlash = dirName[finalLen] == '/';
	} while (0);
	
	for (i = 0; i < numTypes; i++) {
		char aTmpPath[PATH_MAX] = {0};
		snprintf(aTmpPath, PATH_MAX, "%s%s%s", dirName, hasTrailingSlash ? "" : "/", nameList[i]->d_name);
		toRet[i] = strdup(aTmpPath);
	}
	free(nameList);
	
	return toRet;
}

void MInitImportPlug(MADLibrary *inMADDriver, const char *PlugsFolderName)
{
	int i = 0;
	char **plugNames = listDirContents(PlugsFolderName);
	char *plugName;
	//int plugPaths = 0;
	inMADDriver->ThePlug = (PlugInfo*) calloc(MAXPLUG, sizeof(PlugInfo));
	inMADDriver->TotalPlug = 0;
	//TODO: iterate other plug-in paths
	if (plugNames == NULL) {
		return;
	}
	while ((plugName = plugNames[i++]) && inMADDriver->TotalPlug < 40) {
		inMADDriver->ThePlug[i].hLibrary = dlopen(plugName, RTLD_LAZY);
		FILLPLUG plugFill = (FILLPLUG)dlsym(inMADDriver->ThePlug[i].hLibrary, "FillPlug");
		inMADDriver->ThePlug[i].IOPlug = (MADPLUGFUNC)dlsym(inMADDriver->ThePlug[i].hLibrary, "PPImpExpMain");
		if(plugFill && inMADDriver->ThePlug[i].IOPlug) {
			(*plugFill)(&inMADDriver->ThePlug[i]);
			inMADDriver->TotalPlug++;
		}
	}
	
	// clean-up
	i = 0;
	while ((plugName = plugNames[i++])) {
		free(plugName);
	}
	free(plugNames);
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	int i;
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		dlclose(inMADDriver->ThePlug[i].hLibrary);
	}
	free(inMADDriver->ThePlug);
}

MADErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADInfoRec *InfoRec)
{
	int			i;
	MADMusic	aMAD;
	
	if (!strcmp(kindFile, "MADK")) {
		return PPMADInfoFile(AlienFile, InfoRec);
	}
	
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
			*theNewMAD = (MADMusic*) calloc(sizeof(MADMusic), 1);
			if (!theNewMAD) {
				return MADNeedMemory;
			}
			
			return CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
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
		
		if (charl[0] == 'M' &&							// MADK
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
	
	strcpy(type, "!!!!");
	return MADCannotFindPlug;
}

bool MADPlugAvailable(const MADLibrary *inMADDriver, const char* kindFile)
{
	int i;
	
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
	int			i;
	MADInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

MADErr PPTestFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile)
{
	int			i;
	MADMusic	aMAD;
	MADInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

MADFourChar GetPPPlugType(MADLibrary *inMADDriver, short ID, MADFourChar mode)
{
	int i, x;
	
	if (ID >= inMADDriver->TotalPlug) MADDebugStr(__LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				short 	xx;
				MADFourChar	type = '    ';
				
				xx = strlen(inMADDriver->ThePlug[i].type);
				if (xx > 4)
					xx = 4;
				memcpy(&type, inMADDriver->ThePlug[i].type, xx);
				MADBE32(&type);
				
				return type;
			}
			x++;
		}
	}
	
	MADDebugStr(__LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return MADNoErr;
}
