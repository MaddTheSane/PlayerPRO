/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "FileUtils.h"
#include "PPPrivate.h"

//TODO: Move to unicode functions?

OSErr CheckMADFile(char *name)
{
	UNFILE	refNum;
	char	charl[20];
	OSErr	err;
	
	refNum = iFileOpenRead(name);
	if (!refNum)
		return MADReadingErr;
	else {
		iRead( 10, charl, refNum);
		
		if (charl[0] == 'M' &&
			charl[1] == 'A' &&
			charl[2] == 'D' &&
			charl[3] == 'K')
			err = noErr;
		else
			err = MADIncompatibleFile;
		
		iClose(refNum);
	}
	
	return err;
}

OSErr TESTmain(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

OSErr CallImportPlug(MADLibrary*	inMADDriver,
					 short			PlugNo,				// CODE ID
					 OSType			order,
					 Ptr			AlienFile,
					 MADMusic		*theNewMAD,
					 PPInfoRec		*info)
{
	MADDriverSettings driverSettings;
	
	memset(&driverSettings, 0, sizeof(MADDriverSettings));
	return (inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, &driverSettings);
}

OSErr PPTestFile(MADLibrary* inMADDriver, char *kindFile, char *AlienFile)
{
	short		i;
	MADMusic	aMAD;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
		}
	}
	
	return MADCannotFindPlug;
}

OSErr PPMADInfoFile(char *AlienFile, PPInfoRec	*InfoRec)
{
	MADSpec	*theMAD;
	long	fileSize;
	UNFILE	fileID;
	
	theMAD = (MADSpec*)malloc(sizeof(MADSpec) + 200);
	
	fileID = iFileOpenRead(AlienFile);
	if (!fileID) {
		free(theMAD);
		return MADReadingErr;
	}
	fileSize = iGetEOF(fileID);
	
	iRead(sizeof(MADSpec), theMAD, fileID);
	iClose(fileID);
	
	strcpy_s(InfoRec->internalFileName, sizeof(theMAD->name), theMAD->name);
	
	InfoRec->totalPatterns = theMAD->numPat;
	InfoRec->partitionLength = theMAD->numPointers;
	InfoRec->totalTracks = theMAD->numChn;
	InfoRec->signature = 'MADK';
	strcpy_s(InfoRec->formatDescription, sizeof(InfoRec->formatDescription), "MADK");
	InfoRec->totalInstruments = theMAD->numInstru;
	InfoRec->fileSize = fileSize;
	
	free(theMAD);
	theMAD = NULL;
	
	return noErr;
}

OSErr PPInfoFile(MADLibrary* inMADDriver, char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short		i;
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

OSErr PPExportFile(MADLibrary* inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSErr PPImportFile(MADLibrary* inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp( kindFile, inMADDriver->ThePlug[i].type)) {
			OSErr theErr = noErr;
			*theNewMAD = (MADMusic*)calloc(sizeof(MADMusic), 1);
			if (!*theNewMAD)
				return MADNeedMemory;
			
			theErr = CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
			if (theErr != noErr) {
				free(*theNewMAD);
			}
			return theErr;
		}
	}
	
	return MADCannotFindPlug;
}

OSErr PPIdentifyFile(MADLibrary* inMADDriver, char *type, char *AlienFile)
{
	FILE*		refNum;
	short		i;
	PPInfoRec	InfoRec;
	OSErr		iErr;
	
	strcpy_s(type, 5, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpenRead(AlienFile);
	if (!refNum)
		return MADReadingErr;
	iClose(refNum);
	
	// Is it a MAD file?
	iErr = CheckMADFile(AlienFile);
	if (iErr == noErr) {
		strcpy_s(type, 5, "MADK");
		return noErr;
	}
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == noErr) {
			strcpy_s(type, 5, inMADDriver->ThePlug[i].type);
			
			return noErr;
		}
	}
	
	strcpy_s(type, 5, "!!!!");
	
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable(MADLibrary* inMADDriver, char *kindFile)
{
	short i;
	
	if (!strcmp( kindFile, "MADK"))
		return true;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return true;
	}
	
	return false;
}

typedef OSErr (*PLUGFILLDLLFUNC)(PlugInfo*);

Boolean LoadPlugLib(char *name, PlugInfo* plug)
{
	PLUGFILLDLLFUNC	fpFuncAddress;
	OSErr			err;
	
	strcpy_s(plug->file, sizeof(plug->file), name);
	
	plug->hLibrary = LoadLibraryA(name);
	if (!plug->hLibrary)
		return false;
	
	plug->IOPlug = (PLUGDLLFUNC)GetProcAddress(plug->hLibrary, "PPImpExpMain");
	if (!plug->IOPlug) {
		FreeLibrary(plug->hLibrary);
		return false;
	}
	
	fpFuncAddress = (PLUGFILLDLLFUNC)GetProcAddress(plug->hLibrary, "FillPlug");
	if (!fpFuncAddress) {
		FreeLibrary(plug->hLibrary);
		return false;
	}
	
	err = (*fpFuncAddress)(plug);
	
	if(err != noErr) {
		FreeLibrary(plug->hLibrary);
		return false;
	}
	
	return true;
}

//FIXME: 200 seems rather small to me...
//#define MAXFOLDLEN 200
//PATH_MAX on Windows is also small...
//#define MAXFOLDLEN MAX_PATH
#define MAXFOLDLEN (MAX_PATH * 2)
//#define MAXFOLDLEN 255

void MInitImportPlug(MADLibrary* inMADDriver, char *PlugsFolderName)
{
	HANDLE				hFind;
	WIN32_FIND_DATAA	fd;
	BOOL				bRet = TRUE;
	char				FindFolder[MAXFOLDLEN], inPlugsFolderName[MAXFOLDLEN];
	///////////
	inMADDriver->TotalPlug = 0;
	///////////
	
	if (PlugsFolderName) {
		strcpy_s(inPlugsFolderName, MAXFOLDLEN, PlugsFolderName);
		strcat_s(inPlugsFolderName, MAXFOLDLEN, "/");
		
		strcpy_s( FindFolder, MAXFOLDLEN, inPlugsFolderName);
	} else {
		strcpy_s(inPlugsFolderName, MAXFOLDLEN, "/");
		strcpy_s(FindFolder, MAXFOLDLEN, inPlugsFolderName);
	}
	strcat_s(FindFolder, MAXFOLDLEN, "*.PLG");
	
	hFind = FindFirstFileA(FindFolder, &fd);
	
	inMADDriver->ThePlug = (PlugInfo*) calloc( MAXPLUG, sizeof( PlugInfo));
	
	while( hFind != INVALID_HANDLE_VALUE && bRet) {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			if (inMADDriver->TotalPlug < MAXPLUG) {
				char myCompleteFilename[MAXFOLDLEN];
				
				strcpy_s(myCompleteFilename, MAXFOLDLEN, inPlugsFolderName);
				strcat_s(myCompleteFilename, MAXFOLDLEN, fd.cFileName);
				
				if (LoadPlugLib(myCompleteFilename, &inMADDriver->ThePlug[inMADDriver->TotalPlug]))
					inMADDriver->TotalPlug++;
			}
		}
		
		bRet = FindNextFileA(hFind, &fd);
	}
	
	FindClose(hFind);
}

void CloseImportPlug(MADLibrary* inMADDriver)
{
	short i;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		FreeLibrary(inMADDriver->ThePlug[i].hLibrary);
	}
	
	free(inMADDriver->ThePlug);
	inMADDriver->ThePlug = NULL;
}

OSType GetPPPlugType(MADLibrary *inMADDriver, short ID, OSType mode)
{
	short i, x;
	
	if (ID >= inMADDriver->TotalPlug) PPDebugStr( __LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				short	xx;
				OSType	type = '    ';
				
				xx = strlen(inMADDriver->ThePlug[i].type);
				if (xx > 4)
					xx = 4;
				memcpy(&type, inMADDriver->ThePlug[i].type, xx);
				PPBE32(&type);
				
				return type;
			}
			x++;
		}
	}
	
	PPDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}
