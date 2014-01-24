#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"

#define MAXPKEY	200

void ProcessDoItemPress(long ref, short whichItem, DialogPtr whichDialog);
void UPDATE_TrackActive(void);

	extern		DialogPtr 		EditorDlog, theProgressDia, CubeDlog, AdapDlog, MODListDlog, FindDlog, ToolsDlog, PatListDlog, PianoDlog, MozartDlog, InstruViewDlog, PartiDlog;
	extern		DialogPtr		DigitalDlog, ClassicDlog, InstruListDlog, TrackViewDlog, MemoryDlog, OscilloDlog, SpectrumDlog, HelpDlog, AHelpDlog, StaffDlog, WaveDlog;


static	Handle 			PKEYRes;
static	unsigned char	*PKEYDesc[ MAXPKEY];
static	short			FKeyWind[ MAXPKEY];
static	short			FKeyItem[ MAXPKEY], FKeymaxItems;
static	MenuHandle		FKeyMenu;

void InitFKeyMenu(void)
{
	short 	i;
	Ptr		tempPtr;
	
	PKEYRes = GetResource('PKEY', 128);
	if (PKEYRes == NULL) MyDebugStr(__LINE__, __FILE__, "Error loading PKEY Resource");
	
	DetachResource(PKEYRes);
	HLock(PKEYRes);
	
	tempPtr = *PKEYRes;
	
	FKeymaxItems = EndianS16_BtoN(*((short*)tempPtr));
	tempPtr +=2;
	
	FKeymaxItems++;
	
	for (i = 0 ; i < FKeymaxItems; i++) {
		PKEYDesc[ i] = (unsigned char*) tempPtr;
		tempPtr += tempPtr[0] + 1;
		
		FKeyWind[i] = *((long*)tempPtr);
		tempPtr +=4;
		
		FKeyItem[i] = *((long*)tempPtr);
		tempPtr +=4;
	}
	
	HUnlock(PKEYRes);
	
	//////// Menu Creation
	
	FKeyMenu = NewMenu(366, "\pFKey Menu");
	
	for (i = 0 ; i < FKeymaxItems; i++)
	{
		AppendMenu(FKeyMenu, PKEYDesc[ i]);
	}
}

void KillFKeyMenu()
{
	DisposeHandle(PKEYRes);
	PKEYRes = NULL;
}

void GetFKeyDesc(short whichFKey, Str255 str)
{
	short	i;
	
	for (i = 0 ; i < FKeymaxItems; i++)
	{
		if (thePrefs.FKeyItem[ whichFKey] == FKeyItem[ i] &&
			thePrefs.FKeyWind[ whichFKey] == FKeyWind[ i])
			{
				pStrcpy(str, PKEYDesc[ i]);
				
				return;
			}
	}
	
	pStrcpy(str, "\p-");
}

Boolean PressFKeyMenu(short itemID, DialogPtr dia, short whichFKey, Str255	str)
{
	long		mresult, i, curSelec;
	Point		Zone;
	short		temp;
	Boolean		returnVal = false;
	
	short		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	
	GetDialogItem(dia, itemID, &itemType, &itemHandle, &itemRect);
	
	InsertMenu(FKeyMenu, hierMenu);
	
	Zone.h = itemRect.left;	Zone.v = itemRect.top;
	
	LocalToGlobal(&Zone);
	
	curSelec = 1;
	
	if (thePrefs.FKeyItem[ whichFKey] != 0)
	{
		for (i = 0 ; i < FKeymaxItems; i++)
		{
			if (thePrefs.FKeyItem[ whichFKey] == FKeyItem[ i] &&
				thePrefs.FKeyWind[ whichFKey] == FKeyWind[ i])
				{
					curSelec = i+1;
				}
		}
	}
	
	SetItemMark(FKeyMenu, curSelec, 0xa5);
	
	mresult = PopUpMenuSelect(	FKeyMenu,
								Zone.v,
								Zone.h,
								curSelec);
	
	SetItemMark(FKeyMenu, curSelec, 0);
	
	if (HiWord(mresult ) != 0 )
	{
		temp = LoWord(mresult);
		
		pStrcpy(str, PKEYDesc[ temp-1]);
		
		thePrefs.FKeyItem[ whichFKey] = FKeyItem[ temp-1];
		thePrefs.FKeyWind[ whichFKey] = FKeyWind[ temp-1];
		
		returnVal = true;
	}
	
	DeleteMenu(GetMenuID(FKeyMenu));//(*FKeyMenu)->menuID);
	
	return returnVal;
}

WindowPtr FindRefWindow(long ref)
{
	WindowPtr	aWind;
	
	aWind = FrontWindow();
	
	while (aWind != NULL) {
		if (ref == GetWRefCon(aWind))
			return (WindowPtr)aWind;
		
		aWind = GetNextWindow(aWind);
	}
	
	return NULL;
}

void PressFKey(short whichFKey) 
{
	DialogPtr	curDia;
	short		i;
	
	if (thePrefs.FKeyTracks)
	{
		if ((theEvent.modifiers & optionKey) != 0)	// Solo
		{
			short	noActive;
			
			for (i = 0, noActive = 0; i < curMusic->header->numChn; i++)
			{
				if (MADDriver->Active[ i] == true)
				{
					noActive++;
				}
			}
			
			if (noActive <= 1 && MADDriver->Active[ whichFKey] == true)
			{
				for (i = 0, noActive = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = true;
			}
			else
			{
				for (i = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = false;
				MADDriver->Active[ whichFKey] = true;
			}
		}
		else
		{
			MADDriver->Active[ whichFKey] = !MADDriver->Active[ whichFKey];	
		}
		
		UPDATE_TrackActive();
	}
	else
	{
		if (!thePrefs.FKeyActive[ whichFKey]) return;
		if (!thePrefs.FKeyItem[ whichFKey]) return;
		
		if (!FindRefWindow(thePrefs.FKeyWind[ whichFKey]))
		{
			switch(thePrefs.FKeyWind[ whichFKey])
			{
				case RefStaff:			CreateStaffWindow();				break;
				case RefWave:			CreateWaveWindow();					break;
				case RefCube:			CreateCubeWindow();					break;
				case RefInstruList:		CreateInstruListWindow();			break;
				case RefPartition:		CreatePartitionWindow();			break;
				case RefPatList:		CreatePatListWindow();				break;
				case RefParti:			CreatePartiWindow();				break;
				case RefClassic:		CreateClassicWindow();				break;
				case RefMozart:			CreateMozartWindow();				break;
				case RefInstruView:		CreateInstruView();					break;
				case RefTrackView:		CreateTrackView();					break;
				case RefDigiView:		CreateDigiListWindow();				break;
				case RefHelp:			CreateHelpOnline(0);				break;
				case RefAdaptators:		ShowWindow(GetDialogWindow(AdapDlog));				SetItemMark(ViewsMenu, mAdap, checkMark);	break;
				case RefMODList:		DoGrowMODList(MODListDlog);		SelectWindow(GetDialogWindow(MODListDlog));		break;
				case RefPlayer:			CreateOscilloWindow();				SetItemMark(ViewsMenu, mOscilloV, checkMark);	break;
				case RefPiano:			CreatePianoWindow();				break;
				case RefMemory:			CreateMemWindow();					break;
				case RefSpectrum:		CreateSpectrumWindow();				break;
			}
		}
		
		curDia = GetDialogFromWindow(FindRefWindow(thePrefs.FKeyWind[ whichFKey]));
		
		if (curDia)
		{
			GrafPtr		curPort;
			short		itemType;
			Handle		itemHandle;
			Rect		itemRect;
			
			GetPort(&curPort);
			SetPortDialogPort(curDia);
			
			SetFKeyMode(true);
			
			GetDialogItem(curDia, thePrefs.FKeyItem[ whichFKey], &itemType, &itemHandle, &itemRect);
			
			theEvent.where.h = itemRect.left + (itemRect.right - itemRect.left) / 2;
			theEvent.where.v = itemRect.top + (itemRect.bottom - itemRect.top) / 2;
			
			LocalToGlobal(&theEvent.where);
			
			ProcessDoItemPress(thePrefs.FKeyWind[ whichFKey], thePrefs.FKeyItem[ whichFKey], curDia);
			SetFKeyMode(false);
			
			SetPort(curPort);
		}
	}
}
