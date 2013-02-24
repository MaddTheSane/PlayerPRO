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

#define RSRCNAME "\pRsrc Plug Sys##"

OSErr CheckMADFile( Ptr name)
{
	UNFILE			refNum;
	char				charl[ 20];
	OSErr				err;
	
	refNum = iFileOpen( name);
	if( !refNum) return -1;
	else
	{
		iRead( 10, charl, refNum);
		
		if( charl[ 0] == 'M' &&
				charl[ 1] == 'A' &&
				charl[ 2] == 'D' &&
				charl[ 3] == 'K') err = noErr;
		else err = -1;
		
		iClose( refNum);
	}
	
	return err;
}

OSErr TESTmain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

OSErr CallImportPlug(	MADLibrary*				inMADDriver,
						short					PlugNo,				// CODE ID
						OSType					order,
						Ptr						AlienFile,
						MADMusic				*theNewMAD,
						PPInfoRec				*info)
{
OSErr				myErr;
MADDriverSettings 	driverSettings;

	driverSettings.sysMemory = inMADDriver->sysMemory;
	
	myErr = noErr;
	
	myErr = (inMADDriver->ThePlug[ PlugNo].IOPlug) (order, AlienFile, theNewMAD, info, &driverSettings);
	
	return( myErr);
}

OSErr	PPTestFile( MADLibrary* inMADDriver,char	*kindFile, Ptr AlienFile)
{
	short			i;
	MADMusic	aMAD;
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

OSErr	PPInfoFile( MADLibrary* inMADDriver, char	*kindFile, FSSpec	*AlienFile, PPInfoRec	*InfoRec)
{
	short			i;
	MADMusic	aMAD;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'INFO', AlienFile, &aMAD, InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPExportFile( MADLibrary* inMADDriver, char	*kindFile, FSSpec	*AlienFile, MADMusic	*theNewMAD)
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

OSErr	PPImportFile( MADLibrary* inMADDriver, char	*kindFile, FSSpec	*AlienFile, MADMusic	**theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			*theNewMAD = (MADMusic*) MADNewPtrClear( sizeof( MADMusic), inMADDriver);
			if( !*theNewMAD) return -1L;
			
			return( CallImportPlug( inMADDriver, i, 'IMPL', AlienFile, *theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPIdentifyFile( MADLibrary* inMADDriver, char	*type, Ptr AlienFile)
{
	FILE*				refNum;
	short				i;
	PPInfoRec		InfoRec;
	OSErr				iErr;
	
	strcpy( type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpen( AlienFile);
	if( !refNum) return -1;
	iClose( refNum);
	
	// Is it a MAD file?
	iErr = CheckMADFile( AlienFile);
	if( iErr == noErr)
	{
		strcpy( type, "MADK");
		return noErr;
	}
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( CallImportPlug( inMADDriver, i, 'TEST', AlienFile, 0L, &InfoRec) == noErr)
		{
			strcpy( type, inMADDriver->ThePlug[ i].type);
			
			return noErr;
		}
	}
	
	strcpy( type, "!!!!");
	
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable( MADLibrary* inMADDriver, char	*kindFile)
{
	short		i;

	if( !strcmp( kindFile, "MADK")) return true;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type)) return true;
	}
	return false;
}

typedef OSErr (*PLUGFILLDLLFUNC) ( PlugInfo*);

Boolean LoadPlugLib( Ptr name, PlugInfo* plug)
{
	PLUGFILLDLLFUNC		fpFuncAddress;
	OSErr							err;
	
	strcpy( plug->file, name);
	
	plug->hLibrary = LoadLibrary( name);
	if( !plug->hLibrary) return false;
	
	plug->IOPlug = (PLUGDLLFUNC) GetProcAddress( plug->hLibrary, "mainPLUG");
	if( !plug->IOPlug) return false;
	
	fpFuncAddress = (PLUGFILLDLLFUNC) GetProcAddress( plug->hLibrary, "FillPlug");
	if( !fpFuncAddress) return false;
	
	err = (*fpFuncAddress)( plug);
	
	return true;
}

void MInitImportPlug( MADLibrary* inMADDriver, char *PlugsFolderName)
{
	///////////
	inMADDriver->TotalPlug = 0;
	///////////
	
	{
	HANDLE				hFind;
	WIN32_FIND_DATA		fd;
	BOOL				bRet = TRUE;
	char				FindFolder[ 200], inPlugsFolderName[ 200];
	
	if( PlugsFolderName)
	{
		strcpy( inPlugsFolderName, PlugsFolderName);
		strcat( inPlugsFolderName, "/");
		
		strcpy( FindFolder, inPlugsFolderName);
	}
	else
	{
		strcpy( inPlugsFolderName, "/");
		strcpy( FindFolder, inPlugsFolderName);
	}
	strcat( FindFolder, "*.PLG");
	
	hFind = FindFirstFile( FindFolder, &fd);
	
	inMADDriver->ThePlug = (PlugInfo*) MADNewPtr( MAXPLUG * sizeof( PlugInfo), inMADDriver);
	
	while( hFind != INVALID_HANDLE_VALUE && bRet)
	{
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if( inMADDriver->TotalPlug < MAXPLUG)
			{
				char myCompleteFilename[ 200];
				
				strcpy( myCompleteFilename, inPlugsFolderName);
				strcat( myCompleteFilename, fd.cFileName);
				
				if( LoadPlugLib( myCompleteFilename, &inMADDriver->ThePlug[ inMADDriver->TotalPlug])) inMADDriver->TotalPlug++;
			}
		}
		
		bRet = FindNextFile( hFind, &fd);
	}
	
	FindClose( hFind);
	}
	///////////
}

void CloseImportPlug( MADLibrary* inMADDriver)
{
	short i;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
			FreeLibrary( inMADDriver->ThePlug[ i].hLibrary);
	}
}