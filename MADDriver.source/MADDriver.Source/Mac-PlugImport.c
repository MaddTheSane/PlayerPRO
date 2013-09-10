/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include <MixedMode.h>

#include "RDriver.h"
#include "RDriverInt.h"
#include <Aliases.h>
#include "FileUtils.h"
#include <Files.h>
#include <string.h>
#include <CodeFragments.h>

#define MAXPLUG	40
#define RSRCNAME "\pRsrc Plug Sys##"

static inline void pStrCpy( unsigned char *s1, unsigned char *s2)
{
	register short len, i;
	
	if (*s2 <= 220) 
	{
		len = *s2;
		for ( i = 0; i <= len; i++)
		{
			s1[ i] = s2[ i];
		}
	}
	else MyDebugStr( __LINE__, __FILE__, "String to biiig !");
}

Handle MADGet1Resource( OSType type, short id, MADLibrary* init)
{
	if( init->sysMemory)
	{
		Handle	tH, dH;
		
		tH = Get1Resource( type, id);
		if( tH == NULL) return NULL;
		
		DetachResource( tH);
		HNoPurge( tH);
		
		dH = NewHandle( GetHandleSize( tH));
		if( dH == NULL) MyDebugStr( __LINE__, __FILE__, "");
		if( MemError() != noErr) MyDebugStr( __LINE__, __FILE__, "");
		
		HLock( dH);
		HLock( tH);
		
		BlockMoveData( *tH, *dH, GetHandleSize( tH));
		
		HUnlock( dH);
		HUnlock( tH);
		
		DisposeHandle( tH);
		tH = NULL;
		
		return dH;
	}
	else
	{
		Handle dH = Get1Resource( type, id);
		if( dH == NULL) return NULL;
		
		DetachResource( dH);
		HNoPurge( dH);
		
		return dH;
	}
}

//typedef OSErr (*MyProcPtr) ( OSType, char*, MADMusic*, PPInfoRec*, MADDriverSettings *);

OSErr CallImportPlug( 		MADLibrary		*inMADDriver,
													short					PlugNo,			// CODE du plug
													OSType				order,
													char					*AlienFile,
													MADMusic			*theNewMAD,
													PPInfoRec			*info)
{
	OSErr					myErr;
	short					fileID = 0;
	MADDriverSettings		driverSettings;
	CFragConnectionID		connID;
	Ptr						mainAddr;
	Str255					errName;
	static OSErr			(*mainPLUG)( OSType, char*, MADMusic*, PPInfoRec*, MADDriverSettings *);

	driverSettings.sysMemory = false;

	if( !EqualString( RSRCNAME, inMADDriver->ThePlug[ PlugNo].file.name, false, false))
	{
		fileID = FSpOpenResFile( &inMADDriver->ThePlug[ PlugNo].file, fsCurPerm);
		UseResFile( fileID);
	}
	
	myErr = GetDiskFragment( &inMADDriver->ThePlug[ PlugNo].file, 0, kCFragGoesToEOF, inMADDriver->ThePlug[ PlugNo].file.name, kLoadCFrag, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( order, AlienFile, theNewMAD, info, &driverSettings);
		
//			DisposePtr( (Ptr) mainPLUG);
		
		CloseConnection( &connID);
	}

//	myErr = TESTmain( order, AlienFile, theNewMAD, info, &driverSettings);
	
	if( !EqualString( RSRCNAME, inMADDriver->ThePlug[ PlugNo].file.name, false, false))
		CloseResFile( fileID);
	
//	theNewMAD->currentDriver = NULL;
	
	return( myErr);
}

OSErr	PPTestFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile)
{
	short				i;
	MADMusic		aMAD;
	PPInfoRec		InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !MADstrcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'TEST', AlienFile, &aMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr PPMADInfoFile( char	*AlienFile, PPInfoRec	*InfoRec)
{
	MADSpec		*theMAD;
	long			fileSize;
	short			fileID;
	
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

OSErr	PPInfoFile( MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile, PPInfoRec	*InfoRec)
{
	short			i;
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

OSErr	PPExportFile( MADLibrary *inMADDriver, char	*kindFile, char	*AlienFile, MADMusic	*theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !MADstrcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			return( CallImportPlug( inMADDriver, i, 'EXPL', AlienFile, theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr	PPImportFile( MADLibrary *inMADDriver, char	*kindFile, char *AlienFile, MADMusic	**theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !MADstrcmp( kindFile, inMADDriver->ThePlug[ i].type))
		{
			*theNewMAD = (MADMusic*) MADNewPtrClear( sizeof( MADMusic), inMADDriver);
			if( !*theNewMAD) return -1L;
			
			return( CallImportPlug( inMADDriver, i, 'IMPL', AlienFile, *theNewMAD, &InfoRec));
		}
	}
	return MADCannotFindPlug;
}

OSErr CheckMADFile( Ptr name)
{
	UNFILE			refNum;
	char			charl[ 10];
	OSErr			err;
	
	refNum = iFileOpen( name);
	if( !refNum) return -1;
	else
	{
		iRead( 10, charl, refNum);
		
		if( charl[ 0] == 'M' &&							// MADK
				charl[ 1] == 'A' &&
				charl[ 2] == 'D' &&
				charl[ 3] == 'K') err = noErr;
		else err = -1;
		
		iClose( refNum);
	}
	
	return err;
}

OSErr	PPIdentifyFile( MADLibrary *inMADDriver, char *type, char *AlienFile)
{
	UNFILE			refNum;
	short				i;
	PPInfoRec		InfoRec;
	OSErr				iErr = noErr;
	
	strcpy( type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpen( AlienFile);
	if( !refNum) return -1;
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
			strcpy( type, inMADDriver->ThePlug[ i].type);
			
			return noErr;
		}
	}
	
	strcpy( type, "!!!!");
	return MADCannotFindPlug;
}

OSType	GetPPPlugType( MADLibrary *inMADDriver, short ID, OSType mode)
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
				BlockMoveData( inMADDriver->ThePlug[ i].type, &type, xx);

				return type;
			}
			x++;
		}
	}

	MyDebugStr( __LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}

Boolean	MADPlugAvailable( MADLibrary		*inMADDriver, char	*kindFile)
{
	short		i;

	if( !strcmp( kindFile, "MADK")) return true;
	
	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if( !strcmp( kindFile, inMADDriver->ThePlug[ i].type)) return true;
	}
	return false;
}

void LoadImportPLUG( MADLibrary		*inMADDriver, short	No, FSSpec	*theSpec)
{
	Handle		theRes;
	short		fileID;
	Str255		tStr;

	
	inMADDriver->ThePlug[ No].file = *theSpec;
	
	{
		Boolean		targetIsFolder, wasAliased;
		
		ResolveAliasFile( &inMADDriver->ThePlug[ No].file, true, &targetIsFolder, &wasAliased);
	}

	pStrCpy( inMADDriver->ThePlug[ No].filename, inMADDriver->ThePlug[ No].file.name);
	
	fileID = FSpOpenResFile( theSpec, fsCurPerm);
	
	/** CODE du Plug-in **/
	
	GetIndString( tStr, 1000, 1);
	BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].type, 4);
	inMADDriver->ThePlug[ No].type[ 4] = 0;
	
	GetIndString( tStr, 1000, 2);
	BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].mode, 4);
	
	GetIndString( inMADDriver->ThePlug[ No].MenuName, 1000, 3);
	GetIndString( inMADDriver->ThePlug[ No].AuthorString, 1000, 4);
	
	CloseResFile( fileID);
}

static short PlugsFolderOK;

void NScanDirImportPlug( MADLibrary		*inMADDriver, long dirID, short VRefNum, Str255	Fname)
{
	CInfoPBRec		info;
	Str255			tempStr;
	short			i;
	Boolean			targetIsFolder, wasAliased;
	FSSpec			theSpec;
	FInfo			fndrInfo;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID			= dirID;
		info.hFileInfo.ioFDirIndex		= i;
		
		if (PBGetCatInfoSync( &info) != noErr) break;
		
		FSMakeFSSpec( info.hFileInfo.ioVRefNum, dirID, info.hFileInfo.ioNamePtr, &theSpec);
		ResolveAliasFile( &theSpec, true, &targetIsFolder, &wasAliased);
		FSpGetFInfo( &theSpec, &fndrInfo);
		
		if( targetIsFolder)			//if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( Fname[ 0] > 0)
			{
				if( EqualString( info.hFileInfo.ioNamePtr, Fname, false, false) || PlugsFolderOK > 0)	// == true ||
				//	EqualString( theSpec.name, Fname, false, false) == true)
				{
					CInfoPBRec		ci;

					ci.hFileInfo.ioNamePtr 		= theSpec.name;
					ci.hFileInfo.ioVRefNum 		= theSpec.vRefNum;
					ci.hFileInfo.ioDirID  		= theSpec.parID;
					ci.hFileInfo.ioFDirIndex	= 0;
					
					PBGetCatInfoSync( &ci);
					
					PlugsFolderOK++;
					
					NScanDirImportPlug( inMADDriver, ci.hFileInfo.ioDirID, ci.hFileInfo.ioVRefNum, Fname);
					
					PlugsFolderOK--;
				}
			}
		}
		else if( fndrInfo.fdType == 'IMPL')
		{
			if( inMADDriver->TotalPlug < MAXPLUG)
			{
				LoadImportPLUG( inMADDriver, inMADDriver->TotalPlug, &theSpec);
				inMADDriver->TotalPlug++;
			}
		}
	}
}

/*void InitImportMenu(void)
{
short	i;

	ImportMenu	= GetMenu( 138);
	ExportMenu	= GetMenu( 139);


	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
		switch( inMADDriver->ThePlug[ i].mode)
		{
			case 'EXIM':
				AppendMenu( ImportMenu, inMADDriver->ThePlug[ i].MenuName);
				AppendMenu( ExportMenu, inMADDriver->ThePlug[ i].MenuName);
			break;
			
			case 'EXPL':
				AppendMenu( ExportMenu, inMADDriver->ThePlug[ i].MenuName);
			break;
			
			case 'IMPL':
				AppendMenu( ImportMenu, inMADDriver->ThePlug[ i].MenuName);
			break;
		}
	}

	InsertMenu( ImportMenu, hierMenu);
	InsertMenu( ExportMenu, hierMenu);
}*/

void NScanResource( MADLibrary *inMADDriver)
{
	short	i;
	
#define BASERES	1000
	
	for( i = 0; i < MAXPLUG; i++)
	{
		Boolean ResourceOK;
		Handle	aRes, bRes;
		
		ResourceOK = true;
		
		aRes = MADGet1Resource( 'CODE', BASERES + i, inMADDriver);
		if( aRes == NULL) ResourceOK = false;
		else
		{
			DisposeHandle( aRes);
			aRes = NULL;
		}
		
		bRes = MADGet1Resource( 'STR#', BASERES + i, inMADDriver);
		if( bRes == NULL) ResourceOK = false;
		else
		{
			DisposeHandle( bRes);
			bRes = NULL;
		}
		
		if( inMADDriver->TotalPlug < MAXPLUG && ResourceOK == true)
		{
			short		No = inMADDriver->TotalPlug;
			Handle		theRes;
			Str255		tStr;
			
		//	theName = LMGetCurApName();
			
			HGetVol( NULL, &inMADDriver->ThePlug[ No].file.vRefNum, &inMADDriver->ThePlug[ No].file.parID);
			pStrCpy( inMADDriver->ThePlug[ No].file.name, RSRCNAME);
			
			/** CODE du Plug-in **/
			
			GetIndString( tStr, BASERES+i, 1);
			BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].type, 4);
			inMADDriver->ThePlug[ No].type[ 4] = 0;
			
			GetIndString( tStr, BASERES+i, 2);
			BlockMoveData( tStr + 1, &inMADDriver->ThePlug[ No].mode, 4);
			
			GetIndString( inMADDriver->ThePlug[ No].MenuName, BASERES+i, 3);
			GetIndString( inMADDriver->ThePlug[ No].AuthorString, BASERES+i, 4);
			
			inMADDriver->TotalPlug++;
		}
	}
}

void MADInitImportPlug( MADLibrary *inMADDriver, FSRefPtr PluginFolder)
{
	FSSpecPtr spec2;
	if(PluginFolder != NULL)
	{
		spec2 = (FSSpecPtr)NewPtr(sizeof(FSSpec));
		FSGetCatalogInfo(PluginFolder, kFSCatInfoNone, NULL, NULL, spec2, NULL);
	}
	else {
		spec2 = NULL;
	}

	MInitImportPlug(inMADDriver, spec2);
	
	if (spec2 != NULL) {
		DisposePtr((Ptr)spec2);
	}
}

void MInitImportPlug( MADLibrary *inMADDriver, FSSpec *PlugsFolderName)
{
	short		vRefNum;
	long		dirID;
	
	HGetVol( NULL, &vRefNum, &dirID);
	
	inMADDriver->ThePlug = (PlugInfo*) MADNewPtr( MAXPLUG * sizeof( PlugInfo), inMADDriver);
	inMADDriver->TotalPlug = 0;
	
	if( PlugsFolderName != NULL)
	{
		PlugsFolderOK = 0;
		NScanDirImportPlug( inMADDriver, PlugsFolderName->parID, PlugsFolderName->vRefNum, PlugsFolderName->name);
	}
	
	HSetVol( NULL, vRefNum, dirID);
	
	NScanResource( inMADDriver);
}

void CloseImportPlug( MADLibrary		*inMADDriver)
{
	short	i;

	for( i = 0; i < inMADDriver->TotalPlug; i++)
	{
	}

	DisposePtr( (Ptr) inMADDriver->ThePlug);		inMADDriver->ThePlug = NULL;
}
