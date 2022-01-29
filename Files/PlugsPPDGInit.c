#include "MAD.h"
#include "RDriver.h"
#include "shuddup.h"
#include "mixedmode.h"

#include "PPPlug.h"

typedef struct
{
	Str63		MenuName;
	FSSpec		file;
	OSType		Type;
	Boolean		hasPPCCode;
}	FilterInfo;

static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo 		*ThePlug;
static	short		tPlug;
static	MenuHandle	PPDGMenu;

short GetNoPPDG()
{
	return tPlug;
}

OSErr TESTmain(	Pcmd*,
				PPInfoPlug*);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

typedef OSErr (*MyProcPtr) ( Pcmd *,  PPInfoPlug*);

OSErr CallPPDGPlugIns( 		short					PlugNo,					// CODE du plug
						Pcmd 					*myPcmd)
{
OSErr			myErr;
short			fileID;
GrafPtr			savedPort;
ConnectionID		connID;
Ptr				mainAddr;
Str255			errName;
static OSErr		(*mainPLUG)( Pcmd*, PPInfoPlug*);

	GetPort( &savedPort);

//	HGetVol( 0L, &vRefNum, &dirIDCopy);
//	HSetVol( 0L, ThePlug[ PlugNo].file.vRefNum, ThePlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePlug[ PlugNo].file, fsCurPerm);
	
	  myErr = GetDiskFragment( &ThePlug[ PlugNo].file, 0, kWholeFork, ThePlug[ PlugNo].file.name, kLoadLib, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( myPcmd, &thePPInfoPlug);
		
//			DisposePtr( (Ptr) mainPLUG);
		
		CloseConnection( &connID);
	}
	
	CloseResFile( fileID);	

	SetPort( savedPort);

	if( myErr != noErr)
	{
		Erreur( 40, myErr);
		return( myErr);
	}
	else return noErr;
}

void LoadPPDGPLUG(	short	No, StringPtr	theName)
{
Handle		theRes;
short		fileID, i, temp;
Str255		tStr;

	/***********************/

	HGetVol( 0L, &ThePlug[ No].file.vRefNum, &ThePlug[ No].file.parID);
	pStrcpy( ThePlug[ No].file.name, theName);
	
	{
	Boolean		targetIsFolder, wasAliased;
		
	ResolveAliasFile( &ThePlug[ No].file, true, &targetIsFolder, &wasAliased);
	}

	
	fileID = FSpOpenResFile( &ThePlug[ No].file, fsCurPerm);

	/** CODE du Plug-in **/
	
	GetIndString( ThePlug[ No].MenuName, 1000, 1);

	ThePlug[ No].Type = 'PPDG';

	CloseResFile( fileID);
	/*************************/
}

static long PlugsFolderOK;

void ScanDirPPDGPlug( long dirID, short VRefNum)
{
		CInfoPBRec		info;
		Str255			tempStr, volName;
		long				dirIDCopy;
		short			i, vRefNum;
		OSErr			iErr;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfo(&info, false) != noErr) break;
		
		#if MACOS9VERSION
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PPDG')
		#else
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'ppdg')
		#endif
		{	
			HGetVol( 0L, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( 0L, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > 50) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
			LoadPPDGPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
			tPlug++;
			
			iErr = HSetVol( 0L, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirPPDGPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
}

void InitPPDGMenu(void)
{
short	i;

	PPDGMenu = GetMenu( 159);

	for(i=0; i< tPlug; i++)
	{
		AppendMenu( PPDGMenu, ThePlug[ i].MenuName);
	}
}

short PressPPDGMenu( Rect	*PopUpRect)
{
	long	mresult;
	Point	Zone;
	short	i;

	InsertMenu( PPDGMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal( &Zone);
	
	mresult = PopUpMenuSelect(	PPDGMenu,
							Zone.v,
							Zone.h,
							0 );
	
	DeleteMenu( GetMenuID( PPDGMenu));
	
	if ( HiWord( mresult ) != 0 )
	{
		return LoWord( mresult );
	}
	else return -1;
}

void InitPPDGPlug(void)
{
	short		vRefNum;
	long			dirID;
	FSSpec		spec;
	
/*		thePPInfoPlug.RPlaySoundUPP			= NewRoutineDescriptor( (ProcPtr) inMADPlaySoundData, RPlaySoundCallMode, GetCurrentArchitecture());
		thePPInfoPlug.UpdateALLWindowUPP 		= NewRoutineDescriptor( (ProcPtr) UpdateALLWindow, UpdateALLWindowCallMode, GetCurrentArchitecture());*/

		thePPInfoPlug.RPlaySoundUPP			= inMADPlaySoundData;
		thePPInfoPlug.UpdateALLWindowUPP 		= UpdateALLWindow;
		thePPInfoPlug.MyDlgFilterUPP			= MyDlgFilterDesc;	
	
	ThePlug = (FilterInfo*) MyNewPtr( 100 * sizeof( FilterInfo));

	HGetVol( 0L, &vRefNum, &dirID);
	
	GetApplicationPackageFSSpecFromBundle( &spec);
	
	tPlug		= 0;
	PlugsFolderOK = 0;
	ScanDirPPDGPlug( spec.parID, spec.vRefNum);
	
	InitPPDGMenu();

	HSetVol( 0L, vRefNum, dirID);
}
