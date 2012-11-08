/********************						***********************/
//
//	Player PRO 5.0 -- MAD Class for BeOS -
//
//	Library Version 5.0
//
//	To use with MAD Library for BeOS: CodeWarrior
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	Thank you for your interest in PlayerPRO !
//
//	Special Thanks to:
//
//	Dario Accornero <adario@cs.bu.edu>
//
//	For his BeOS support and help!
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		rossetantoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "FileUtils.h"

static	Boolean LoadPlugLib( Ptr name, PlugInfo* plug );
static	void	ScanPlugDir( char *name );

static MADLibrary *inMADDriver;

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
				charl[ 3] == 'I') err = noErr;
		else err = -1;
		
		iClose( refNum);
	}
	
	return err;
}

//	DA on 9/9/98

OSErr CallImportPlug( 	short		PlugNo,
						OSType		order,
						Ptr			AlienFile,
						MADMusic	*theNewMAD,
						PPInfoRec	*info)
{
	OSErr				myErr;
	MADDriverSettings	settings;
	
	settings.sysMemory = inMADDriver->sysMemory;
	myErr = (*(inMADDriver->ThePlug[ PlugNo].IOPlug)) (order, AlienFile, theNewMAD, info, &settings);
	
	return( myErr);
}

OSErr	PPTestFile( char	*kindFile, Ptr AlienFile)
{
	short			i;
	MADMusic		aMAD;
	PPInfoRec		InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( i, 'TEST', AlienFile, &aMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPInfoFile( char	*kindFile, char	*AlienFile, PPInfoRec	*InfoRec)
{
	short			i;
	MADMusic		aMAD;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( i, 'INFO', AlienFile, &aMAD, InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPExportFile( char	*kindFile, char	*AlienFile, MADMusic	*theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( i, 'EXPL', AlienFile, theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPImportFile( MADLibrary *MADLib, char	*kindFile, char	*AlienFile, MADMusic	**theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			*theNewMAD = (MADMusic*) MADNewPtrClear( sizeof( MADMusic), inMADDriver);
			if( !*theNewMAD) return -1L;
			
			return( CallImportPlug( i, 'IMPL', AlienFile, *theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPIdentifyFile( MADLibrary *lib, char	*type, Ptr AlienFile)
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
		strcpy( type, "MADI");
		return noErr;
	}
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( CallImportPlug( i, 'TEST', AlienFile, 0L, &InfoRec) == noErr)
		{
			strcpy( type, inMADDriver->ThePlug[ i].type);
			
			return noErr;
		}
	}
	
	strcpy( type, "!!!!");
	
	return MADCannotFindPlug;
}

EXP	Boolean	MADPlugAvailable( char	*kindFile)
{
	short		i;

	if( !strcmp( kindFile, "MADI")) return true;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type)) return true;
	}
	return false;
}

//	DA on 9/9/98

static	Boolean LoadPlugLib( Ptr name, PlugInfo* plug )
{
	strcpy( plug->file, name);
	
	plug->hLibrary = load_add_on( name );
	if ( !plug->hLibrary )
		return	false;
	
	OSErr (*FillPlugCode)( PlugInfo *p );
	FillPlugCode = NULL;
	status_t	symbolGet = get_image_symbol(	plug->hLibrary,
												"FillPlug",
												B_SYMBOL_TYPE_TEXT,
												&FillPlugCode );
	
	if ( symbolGet == B_NO_ERROR && FillPlugCode )
	{
		FillPlugCode( plug);
	
		MADPlug	plugCode = NULL;
		
		symbolGet = get_image_symbol(	plug->hLibrary,
										"mainPLUG",
										B_SYMBOL_TYPE_TEXT,
										&plugCode );
		if ( symbolGet == B_NO_ERROR && plugCode )
		{
			plug->IOPlug = plugCode;
			return	true;
		}
		else
		{
			unload_add_on( plug->hLibrary );
			return	false;
		}
	}
	else
	{
		unload_add_on( plug->hLibrary );
		return	false;
	}
}

//	DA on 9/9/98

static	void	ScanPlugDir( char *FindFolder )
{
	BDirectory	plugsDir( FindFolder );
	entry_ref	plugsRef;
	while( plugsDir.GetNextRef( &plugsRef ) == B_NO_ERROR )
	{
		if( inMADDriver->TotalPlug < MAXPLUG)
		{
			char myCompleteFilename[1024];
			sprintf( myCompleteFilename, "%s/%s", FindFolder, plugsRef.name );
			if( LoadPlugLib( myCompleteFilename, &inMADDriver->ThePlug[ inMADDriver->TotalPlug]))
				inMADDriver->TotalPlug++;
		}
	}
}

//	DA on 9/9/98

void MInitImportPlug( MADLibrary *lib, char *PlugsFolderName)
{
	//	Signal there are no valid plugs yet.

	inMADDriver = lib;
	inMADDriver->TotalPlug = 0;
	inMADDriver->ThePlug = (PlugInfo*) MADNewPtr( MAXPLUG * sizeof( PlugInfo), inMADDriver);

	//	Construct valid path, independent of launch mode.
	
	app_info	aInfo;
	status_t	error = be_app->GetAppInfo( &aInfo );
	BEntry		fileEntry( &aInfo.ref );
	BPath		filePath;
	fileEntry.GetPath( &filePath );
	BPath		parentPath;
	filePath.GetParent( &parentPath );
	char		pathBuffer[1024];
	strcpy( pathBuffer, parentPath.Path() );

	//	Build main plugs path and scan it.

	char		FindFolder[1024];	
	if ( PlugsFolderName && strlen( PlugsFolderName ) )
		sprintf( FindFolder, "%s/%s", pathBuffer, PlugsFolderName );
	else
		strcpy( FindFolder, pathBuffer );
	
	ScanPlugDir( FindFolder );
}

//	DA on 9/9/98

void CloseImportPlug( MADLibrary *lib)
{
	short i;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
		unload_add_on( inMADDriver->ThePlug[ i].hLibrary );
}
