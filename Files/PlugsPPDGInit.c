#include "MAD.h"
#include "RDriver.h"
#include "Shuddup.h"

#include "PPPlug.h"
#include "PPPrivate.h"
#define FilterPlugMax 100

typedef struct FilterInfo
{
	PPDigitalPlugin	**PlugCode;
	CFStringRef		MenuName;
	CFBundleRef		file;
	OSType			Type;
} FilterInfo;

static	PPInfoPlug	thePPInfoPlug;
static	FilterInfo	*ThePlug;
static	short		tPlug;
static	MenuHandle	PPDGMenu;

short GetNoPPDG()
{
	return tPlug;
}

OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

OSErr CallPPDGPlugIns(short PlugNo, Pcmd *myPcmd)
{
	OSErr			iErr = noErr;
	PPDigitalPlugin	**formatPlugA = ThePlug[PlugNo].PlugCode;
	short			resFileNum;
	GrafPtr			savedPort;
	
	GetPort(&savedPort);
	resFileNum = CFBundleOpenBundleResourceMap(ThePlug[PlugNo].file);
	
	iErr = (*formatPlugA)->MyProcPtr(myPcmd, &thePPInfoPlug);
	
	CFBundleCloseBundleResourceMap(ThePlug[PlugNo].file, resFileNum);
	SetPort(savedPort);
	
	
	if (iErr != noErr)
	{
		Erreur(40, iErr);
	}	
	
	return iErr;
}

#define PPDGLoadPlug(theBundle) (PPDigitalPlugin**)GetCOMPlugInterface(theBundle, kPlayerPRODigitalPlugTypeID, kPlayerPRODigitalPlugInterfaceID)

void InitPPDGPlug(void)
{
	thePPInfoPlug.RPlaySoundUPP			= inMADPlaySoundData;
	thePPInfoPlug.UpdateALLWindowUPP 	= UpdateALLWindow;
	thePPInfoPlug.MyDlgFilterUPP		= MyDlgFilterDesc;
	
	
	CFArrayRef  PlugLocsDigital = GetDefaultPluginFolderLocations();
	CFIndex		i, x, PlugLocNums;
	
	ThePlug = (FilterInfo*) MyNewPtr(FilterPlugMax * sizeof(FilterInfo));
	
	tPlug	= 0;
	
	PPDigitalPlugin** tempMADPlug = NULL;
	PlugLocNums = CFArrayGetCount(PlugLocsDigital);
	for (i=0; i < PlugLocNums; i++) {
		CFIndex		PlugNums;
		CFArrayRef	somePlugs;
		CFURLRef	aPlugLoc;
		aPlugLoc = CFArrayGetValueAtIndex(PlugLocsDigital, i);
		somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, NULL);
		PlugNums = CFArrayGetCount(somePlugs );
		if (PlugNums > 0) {
			for (x = 0; x < PlugNums; x++) {
				CFPlugInRef tempPlugRef = NULL;
				CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
				tempPlugRef = CFBundleGetPlugIn(tempBundleRef);
				tempMADPlug = PPDGLoadPlug(tempPlugRef);
				if (tempMADPlug) {
					if (tPlug > FilterPlugMax) {
						MyDebugStr(__LINE__, __FILE__, "Too many plugs!");
						break;
					}
					
#pragma mark This is where we add the plug to the plug library.
					short		resFileNum = CFBundleOpenBundleResourceMap(tempBundleRef);
					CFTypeRef	OpaqueDictionaryType = NULL;
					
					ThePlug[tPlug].PlugCode = tempMADPlug;
					ThePlug[tPlug].file = tempBundleRef;
					CFRetain(ThePlug[tPlug].file);
					OpaqueDictionaryType = CFBundleGetValueForInfoDictionaryKey(tempBundleRef, kMadPlugMenuNameKey);
					if(CFGetTypeID(OpaqueDictionaryType) == CFStringGetTypeID())
						ThePlug[tPlug].MenuName = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)OpaqueDictionaryType);
					ThePlug[tPlug].Type = 'PPDG';
					
					CFBundleCloseBundleResourceMap(tempBundleRef, resFileNum);
					tPlug++;
				}
			}
		}
		CFRelease(somePlugs);
	}
	CFRelease(PlugLocsDigital);
	
	InitPPDGMenu();
}

//TODO: close PPDG Plugins?
//If we don't, memory leaks! But The OS should take care of it when the program quits.
//I think.

void InitPPDGMenu(void)
{
	short	i;
	
	PPDGMenu = GetMenu(159);
	
	for(i=0; i < tPlug; i++) {
		Str255	PPDGPlugMenuName;
		GetPStrFromCFString(ThePlug[ i].MenuName, PPDGPlugMenuName);

		AppendMenu(PPDGMenu, PPDGPlugMenuName);
	}
}

short PressPPDGMenu(Rect *PopUpRect)
{
	long	mresult;
	Point	Zone;
	short	i;
	
	InsertMenu(PPDGMenu, hierMenu);
	
	Zone.h = PopUpRect->left;
	Zone.v = PopUpRect->top;
	
	LocalToGlobal(&Zone);
	
	mresult = PopUpMenuSelect(PPDGMenu, Zone.v, Zone.h, 0);
	
	DeleteMenu(GetMenuID(PPDGMenu));
	
	if (HiWord(mresult) != 0 ) {
		return LoWord(mresult);
	} else
		return -1;
}
