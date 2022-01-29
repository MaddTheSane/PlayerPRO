/* Init Plugs-Ins */

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

short		ToneGenerator;
static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo 		*ThePlug;
static	short		tPlug;
			MenuHandle	SampleMenu;
extern	WindowPtr	oldWindow;

OSErr TESTmain(	Ptr,
				long ,
				long ,
				long ,
				PPInfoPlug*,
				long);

typedef OSErr (*MyProcPtr) ( sData*, long , long , PPInfoPlug*, long);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

OSErr NCallPlugIns( 		short						PlugNo,					// CODE du plug
												sData						*theInsData,
												long 						start,
												long 						end,
												long						stereoMode)
{
OSErr						myErr;
short						fileID;
GrafPtr					savedPort;
ConnectionID		connID;
Ptr							mainAddr;
Str255					errName;
static OSErr		(*mainPLUG)( sData*, long , long , PPInfoPlug*, long);

	if( ThePlug[ PlugNo].Type != 'PLug')
	{
		return noErr;	//return( CallSEPlugIns(	PlugNo, theInsData, start, end));
	}
	
	GetPort( &savedPort);
	
//	HGetVol( 0L, &vRefNum, &dirIDCopy);
//	HSetVol( 0L, ThePlug[ PlugNo].file.vRefNum, ThePlug[ PlugNo].file.parID);
	fileID = FSpOpenResFile( &ThePlug[ PlugNo].file, fsCurPerm);
	
//	myErr = TEST3main( theInsData, start, end, &thePPInfoPlug, stereoMode);
	
	myErr = GetDiskFragment( &ThePlug[ PlugNo].file, 0, kWholeFork, ThePlug[ PlugNo].file.name, kLoadLib, &connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		myErr = mainPLUG ( theInsData, start, end, &thePPInfoPlug, stereoMode);
		
//			DisposePtr( (Ptr) mainPLUG);
		
		myErr = CloseConnection( &connID);
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

void LoadPLUGSE(	short	No, StringPtr	theName)
{

}

void LoadPLUG(	short	No, StringPtr	theName)
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

	GetIndString( ThePlug[ No].MenuName, 1000, 1);

	if(EqualString( ThePlug[ No].MenuName, "\pTone Generator...", false, false)) ToneGenerator = No;

	ThePlug[ No].Type = 'PLug';

	CloseResFile( fileID);
	/*************************/
}

static long PlugsFolderOK;

void ScanDirPlug( long dirID, short VRefNum)
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
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PLug')
		#else
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'plug')
		#endif
		{	
			HGetVol( 0L, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( 0L, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > 50) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
				LoadPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
	/*		if( info.hFileInfo.ioFlFndrInfo.fdType == '�Eff')
				LoadPLUGSE( tPlug, info.hFileInfo.ioNamePtr);*/
			
			tPlug++;
			
			iErr = HSetVol( 0L, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
}

void InitSampleMenu(void)
{
short	i;

	SampleMenu = GetMenu( 136);

	for(i=0; i< tPlug; i++)
	{
		AppendMenu( SampleMenu, ThePlug[ i].MenuName);
	}
}

short PressSampleMenu( Rect	*PopUpRect)
{
	long		mresult;
	Point		Zone;
	short	i;

	InsertMenu( SampleMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal( &Zone);
	
	mresult = PopUpMenuSelect(	SampleMenu,
								Zone.v,
								Zone.h,
								0 );
	
	DeleteMenu( GetMenuID( SampleMenu));
	
	if ( HiWord( mresult ) != 0 )
	{
		return LoWord( mresult );
	}
	else return -1;
}

void InitPlug(void)
{
	short			vRefNum;
	long			dirID;
	FSSpec		spec;
	
/*	#if MACOS9VERSION
	thePPInfoPlug.RPlaySoundUPP			= NewRoutineDescriptor( (ProcPtr) inMADPlaySoundData, RPlaySoundCallMode, GetCurrentArchitecture());
	thePPInfoPlug.UpdateALLWindowUPP 		= NewRoutineDescriptor( (ProcPtr) UpdateALLWindow, UpdateALLWindowCallMode, GetCurrentArchitecture());
	#endif*/
	
		thePPInfoPlug.RPlaySoundUPP					= inMADPlaySoundData;
		thePPInfoPlug.UpdateALLWindowUPP 		= UpdateALLWindow;

	
		thePPInfoPlug.MyDlgFilterUPP			= MyDlgFilterDesc;	
	
	ThePlug = (FilterInfo*) MyNewPtr( 100 * sizeof( FilterInfo));

	HGetVol( 0L, &vRefNum, &dirID);
	
	tPlug		= 0;
	ToneGenerator	= -1;
	PlugsFolderOK = 0L;
	
	GetApplicationPackageFSSpecFromBundle( &spec);
	ScanDirPlug( spec.parID, spec.vRefNum);
	
	InitSampleMenu();

	HSetVol( 0L, vRefNum, dirID);
}

short GetMaxSoundFilterPlugs( void)
{
	return tPlug;
}