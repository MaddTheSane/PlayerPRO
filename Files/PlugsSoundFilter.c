/* Init Plugs-Ins */

#include "MAD.h"
#include "RDriver.h"
#include "Shuddup.h"

#include "PPPlug.h"
#include "PPPrivate.h"

#define MAXFILTERSPLUGS 100

typedef struct FilterInfo
{
	PPFiltersPlugin	**PlugData;
	CFStringRef		MenuName;
	CFBundleRef		file;
	OSType			Type;
} FilterInfo;

		short		ToneGenerator;
static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo 	*ThePlug;
static	short		tPlug;
		MenuHandle	SampleMenu;
extern	WindowPtr	oldWindow;

OSErr TESTmain(	Ptr,
				long ,
				long ,
				long ,
				PPInfoPlug*,
				long);

#define PPFilterLoadPlug(theBundle) (PPFiltersPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROFiltersPlugTypeID, kPlayerPROFiltersPlugInterfaceID)

typedef OSErr (*MyProcPtr) ( sData*, long , long , PPInfoPlug*, long);
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
extern void NSLog(CFStringRef format, ...);

OSErr NCallPlugIns( 		short						PlugNo,					// CODE du plug
												sData						*theInsData,
												long 						start,
												long 						end,
												long						stereoMode)
{
	OSErr			myErr;
	short			fileID;
	GrafPtr			savedPort;
	PPFiltersPlugin	**InstrPlugA = ThePlug[PlugNo].PlugData;
	
	GetPort( &savedPort);
	
	fileID = CFBundleOpenBundleResourceMap(ThePlug[ PlugNo].file);
	
	
	myErr = (*InstrPlugA)->FiltersMain( theInsData, start, end, &thePPInfoPlug, stereoMode);
	
		
	CFBundleCloseBundleResourceMap(ThePlug[ PlugNo].file, fileID);	
	
	SetPort( savedPort);

	if( myErr != noErr)
	{
		Erreur( 40, myErr);
		return( myErr);
	}
	else return noErr;
}

void LoadPLUGSE( short No, StringPtr theName)
{
	NSLog(CFSTR("Umm... what is this?"));
}

static long PlugsFolderOK;

void InitSampleMenu(void)
{
	short	i;

	SampleMenu = GetMenu( 136);

	for(i=0; i< tPlug; i++)
	{
		Str255 pMenuName;
		GetPStrFromCFString(ThePlug[ i].MenuName, pMenuName);

		AppendMenu( SampleMenu, pMenuName);
	}
}

short PressSampleMenu( Rect	*PopUpRect)
{
	long	mresult;
	Point	Zone;
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
	thePPInfoPlug.RPlaySoundUPP					= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP			= UpdateALLWindow;

	thePPInfoPlug.MyDlgFilterUPP				= MyDlgFilterDesc;	
	
	ThePlug = (FilterInfo*) MyNewPtr( MAXFILTERSPLUGS * sizeof( FilterInfo));
	
	tPlug			= 0;
	ToneGenerator	= -1;
	PlugsFolderOK	= 0;
	
	CFArrayRef  PlugLocsDigital = GetDefaultPluginFolderLocations();
	CFIndex		i, x, PlugLocNums;

	PPFiltersPlugin** tempMADPlug = NULL;
	PlugLocNums = CFArrayGetCount(PlugLocsDigital);
	for (i=0; i < PlugLocNums; i++) {
		CFIndex		PlugNums;
		CFArrayRef	somePlugs;
		CFURLRef	aPlugLoc;
		aPlugLoc = CFArrayGetValueAtIndex(PlugLocsDigital, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, NULL);
		PlugNums = CFArrayGetCount( somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFPlugInRef tempPlugRef = NULL;
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				tempMADPlug = PPFilterLoadPlug(tempPlugRef);
				if (tempMADPlug) {
					if( tPlug > MAXFILTERSPLUGS) 
					{
						MyDebugStr( __LINE__, __FILE__, "Too many plugs");
						break;
					}
					
#pragma mark This is where we add the plug to the plug library.
					CFTypeRef	OpaqueDictionaryType = NULL;
					FilterInfo	*curPlug = &(ThePlug[tPlug]);
					curPlug->PlugData = tempMADPlug;
					curPlug->file = tempBundleRef;
					CFRetain(curPlug->file);
					OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(curPlug->file, kMadPlugMenuNameKey);
					if(CFGetTypeID(OpaqueDictionaryType) == CFStringGetTypeID())
						curPlug->MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
					
					curPlug->Type = 'PLug';
					if (ToneGenerator == -1) {
						CFArrayRef factories = CFPlugInFindFactoriesForPlugInTypeInPlugIn(kPlayerPROFiltersPlugTypeID, curPlug->file );
						CFIndex	factoryCount, index;
						
						factoryCount	= CFArrayGetCount( factories );
						for ( index = 0 ; (index < factoryCount) ; index++ )
						{
							CFUUIDRef	factoryID;
							factoryID = (CFUUIDRef) CFArrayGetValueAtIndex( factories, index );
							if (CFEqual(factoryID, CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x25, 0xFA, 0x16, 0xEC, 0x75, 0xFF, 0x45, 0x14, 0x9C, 0x84, 0x72, 0x02, 0x36, 0x00, 0x44, 0xB9) /*Tone Generator UUID*/)) {
								ToneGenerator = tPlug;
							}
						}
						CFRelease(factories);
					}
					tPlug++;
				}
			}
		}
		
		CFRelease(somePlugs);
	}	
	CFRelease(PlugLocsDigital);
	
	InitSampleMenu();
}

short GetMaxSoundFilterPlugs( void)
{
	return tPlug;
}
